using namespace std;
#include "TRandom.h"
#include <time.h>
#include "hemcdigitizer.h"
#include "emcdef.h" 
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hemcdetector.h" 
#include "hgeantemc.h"
#include "hgeantkine.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hemccalsim.h"
#include "hemccal.h"
#include "hemcgeompar.h"
#include "hemcdigipar.h"
#include <iostream>
#include <iomanip>

//*-- Author   : K. Lapidus
//*-- Modified : 

/////////////////////////////////////////////////////////////////////
//
//  HEmcDigitizer digitizes data, puts output values into
//  cal data category
//
//
/////////////////////////////////////////////////////////////////////

ClassImp(HEmcDigitizer)

HEmcDigitizer::HEmcDigitizer(void) {
  initVariables();
}

HEmcDigitizer::HEmcDigitizer(const Text_t *name, const Text_t *title) : HReconstructor(name,title) {
  initVariables();
}

HEmcDigitizer::~HEmcDigitizer(void) {
  if (iterGeantEmc) delete iterGeantEmc;
  if (randGen)      delete randGen;
  clearCellobjects();
  cellobjects.clear();
}

void HEmcDigitizer::initVariables(void) {
  fGeantEmcCat = 0;
  fCalCat      = 0;
  iterGeantEmc = 0;
  fGeomPar     = 0;
  fDigiPar     = 0;
  maxCell      = 0;
  phot2E       = 0.F;
  facESmear    = 0.F;
  fLoc.setNIndex(3);
  fLoc.set(3,0,0,0);
}

Bool_t HEmcDigitizer::init(void) {
  fEmcDet = (HEmcDetector*)(gHades->getSetup()->getDetector("Emc"));
  if(!fEmcDet){
    Error("init","No Emc Detector found");
    return kFALSE;
  }

  // working array
  maxCell = fEmcDet->getMaxComponents();
  cellobjects.resize(6*maxCell, 0 );   // size is constant over run time

  // GEANT input data
  fGeantEmcCat = gHades->getCurrentEvent()->getCategory(catEmcGeantRaw);  
  if (!fGeantEmcCat) {
    Error("HEmcDigitizer::init()","HGeant EMC input missing");
    return kFALSE;
  }
  iterGeantEmc = (HIterator *)fGeantEmcCat->MakeIterator("native");
  
  // Build the Calibration category
  fCalCat = gHades->getCurrentEvent()->getCategory(catEmcCal);  
  if (!fCalCat) {
    fCalCat = fEmcDet->buildMatrixCategory("HEmcCalSim",0.5);  
    gHades->getCurrentEvent()->addCategory(catEmcCal,fCalCat,"Emc");
  }

  randGen = new TRandom2;
  randGen->SetSeed(0);

  return setParameterContainers();
}

Bool_t HEmcDigitizer::setParameterContainers(void) {
  fGeomPar=(HEmcGeomPar*)gHades->getRuntimeDb()->getContainer("EmcGeomPar");
  if (!fGeomPar){
    Error("initParContainer","No EmcGeomPar parameter container");
    return kFALSE;
  }
  fDigiPar    =(HEmcDigiPar *)gHades->getRuntimeDb()->getContainer("EmcDigiPar");
  if (!fDigiPar){
    Error("initParContainer","No EmcDigiPar parameter container");
    return kFALSE;
  }
  return kTRUE;
}

Bool_t HEmcDigitizer::reinit(void) {
  // sets some local variables read from initialized parameter container
  phot2E               = fDigiPar->getPhot2E();
  sigmaT               = fDigiPar->getSigmaT();
  Float_t sigmaEIntern = fDigiPar->getSigmaEIntern();
  Float_t sigmaEReal   = fDigiPar->getSigmaEReal();
  facESmear = 1000.*sqrt(sigmaEReal*sigmaEReal - sigmaEIntern*sigmaEIntern);
  return kTRUE;
}

Int_t HEmcDigitizer::execute(void) {
  // Digitization of GEANT hits and storage in HEmcCalSim
  HGeantEmc*  geantemc = 0;
  HEmcCalSim* cal      = 0;
  
  Int_t   cell        = -1;
  Int_t   trackNumber = -1;
  Float_t peHit       = 0.F;
  Float_t xHit        = 0.F;
  Float_t yHit        = 0.F;
  Float_t zHit        = 0.F;
  Float_t tofHit      = 0.F;
  Float_t momHit      = 0.F;
  Float_t trackLength = 0.F;
  Char_t  row         = -1;
  Char_t  col         = -1;

  //loop over the HGeantEmc objects and fill temporary working objects
  clearCellobjects();
  iterGeantEmc->Reset();   
  while ((geantemc=(HGeantEmc *)iterGeantEmc->Next())!=0) {  
    trackNumber = geantemc->getTrack();
    geantemc->getHit(peHit, xHit, yHit, zHit, tofHit, momHit, trackLength);
    fLoc[0] = geantemc->getSector();
    cell = geantemc->getCell();
    if (fLoc[0]<0 || fLoc[0]>5 || cell<0 || cell>=maxCell) {
      Warning("HEmcDigitizer:execute","Emc Geant cell address invalid: sec=%i cell=%i",fLoc[0],cell);
      continue;
    }                     
    Int_t ind = fLoc[0] * maxCell + cell;
    if (cellobjects[ind] == 0) { // first time we have to create the object
      cellobjects[ind] = new celldata;
      cellobjects[ind]->sec  = fLoc[0];
      cellobjects[ind]->cell = cell;
      cellobjects[ind]->npe  = 0;
    }
    celldata* cdata = cellobjects[ind];
    if (trackNumber == -777) {
      cdata->npe = peHit;
    } else {
      celltrack* celltr;
      // CASE I: no tracks in this cell yet
      if (cdata->ctracks.size()==0) {
        // create new object for this cell and add them to the list of tracks for this cell
        celltr = new celltrack;
        celltr->reset();
        cdata->ctracks.push_back(celltr);
      } else {
        // CASE II: this cell has already tracks
        // check if current track is already present in the list 
        for(UInt_t i=0;i<cdata->ctracks.size();i++) {
          celltr = cdata->ctracks[i];
          // CASE II a: the current track found in list
          if (celltr->gtrack == trackNumber) break;
        }
        // CASE II b: the current track is not in list yet
        if (celltr->gtrack != trackNumber) {
          celltr = new celltrack;
          celltr->reset();
          cdata->ctracks.push_back(celltr);
        }
      }
      if (tofHit < celltr->gtime) {
        celltr->gtime     = tofHit;
        celltr->gtrack    = trackNumber;
      }
    }
  } // end of HGeantEmc loop
  for(UInt_t i = 0; i < cellobjects.size(); i ++) {
    celldata* cdata = cellobjects[i];
    if (cdata) {
      fLoc[0] = cdata->sec;
      fEmcDet->getRowCol(cdata->cell,row,col);
      if (row<0 || col<0 ) {
        Warning("HEmcDigitizer:execute",
                "Emc row or column not found: cell=%i row=%i col=%i",cell,row,col);
        continue;
      }
      fLoc[1]=row;
      fLoc[2]=col;
      cal = (HEmcCalSim*) fCalCat->getSlot(fLoc);
      if (!cal) {
        Warning("HEmcDigitizer:execute","HEmcCalSim:getSlot(loc) failed!");
        continue;
      }
      cal = new(cal) HEmcCalSim;
      cal->setSector(fLoc[0]);
      cal->setCell(cdata->cell);
      cal->setRow(row);
      cal->setColumn(col);
      // ------  energy -------------
      Float_t energy = cdata->npe * phot2E;  //number of p.e. -> energy
      // additional smearing to reproduce energy resolution
      Float_t sigmaE = facESmear*sqrt(energy/1000.);
      cal->setEnergy(randGen->Gaus(energy,sigmaE));
      // ------  sort by time  --------------
      cdata->sortTime();
      // ------  time and track numbers ------
      Float_t time = 10000.F;
      for(UInt_t k = 0; k < cdata->ctracks.size() && k < 5; k++ ) {
        celltrack* celltr = cdata->ctracks[k];
        if (k==0) time = celltr->gtime;
        cal->setTrack(celltr->gtrack);
      }
      cal->setTime1(randGen->Gaus(time,sigmaT));
      cal->setNHits(1);
      cal->setTotMult(cdata->ctracks.size());
    }
  }
  return 0;
}

void  HEmcDigitizer::clearCellobjects(){
  // deletes objects in working array and sets pointer to 0. 
  // the vector is still not cleared.
  for(UInt_t i = 0; i < cellobjects.size(); i ++) {
    if (cellobjects[i]) {
      cellobjects[i]->reset();
      delete cellobjects[i];
      cellobjects[i] = 0;
    }
  }
}
