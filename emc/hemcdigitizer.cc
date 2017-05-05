using namespace std;
#include "TRandom.h"
#include "hemcdigitizer.h" 
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hemcdetector.h" 
#include "hgeantemc.h"
#include "hgeantkine.h"
#include "hevent.h"
#include "hlinearcategory.h"
#include "hemccalsim.h"
#include "hemcgeompar.h"
#include "hemcdigipar.h"
#include "hemcsimulpar.h"
#include "hstartdef.h"
#include "hstart2hit.h"

#include <iostream>

//*-- Author   : V. Pechenov
//*-- Modified :

/////////////////////////////////////////////////////////////////////
//
//  HEmcDigitizer digitizes data, puts output values into cal data category
//
// Input data for digitization:
// Two types of HGeantEmc objects:
// 1. Hit which has track number = -777 keeps integrated number of photo electrons
// 2. Hit which has track number > 0 keeps a time of flight, momentum, track length and number of photo electrons
//
// Parameters for recalculating of the number of photo electrons to the energy deposit
// Parameters for the time and energy deposit smearings
// Parameter for the start signal smearing
//
// Two modes of reconstruction: simulation and embedding
//
// HEmcDigitizer
//  collects all tracks in the cell,
//  corrects GEANT time of flight to the time in the input EMC plane,
//  sorts tracks by corrected time and assigns the cell time to the fastest track which has energy deposit above threshold,
//  sorts tracks by energy and assigns the cell to the track which has maximal energy deposit.
//
// Digitizer retrieves a particle from track history which crosses parallel plane to EMC plane and distance to front of EMC is equal to zVertBorder.
// zVertBorder >= 0    EMC front plane is used
// zVertBorder = -130. RPC front plane is used (default value)
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
  clearCellobjects();
  cellobjects.clear();
}

void HEmcDigitizer::initVariables(void) {
  fGeantEmcCat     = NULL;
  fGeantKineCat    = NULL;
  fCalCat          = NULL;
  iterGeantEmc     = NULL;
  fGeomPar         = NULL;
  fDigiPar         = NULL;
  fStartHitCat     = NULL;
  zVertBorder      = -130.;    // [mm]
  energyDepositCut = 15.;      // [MeV]
  signalVelocity   = 299.792;  // [mm/ns]
  halfOfCellLength = 210.;     // [mm] 
  embeddingmode    = 0;
  fLoc.setNIndex(2);
  fLoc.set(2,0,0);
}

Bool_t HEmcDigitizer::init(void) {
  fEmcDet = (HEmcDetector*)(gHades->getSetup()->getDetector("Emc"));
  if(!fEmcDet){
    Error("init","No Emc Detector found");
    return kFALSE;
  }

  // working array
  cellobjects.resize(cellObjectsSize(), 0 );   // size is constant over run time

  // GEANT input data
  fGeantEmcCat = gHades->getCurrentEvent()->getCategory(catEmcGeantRaw);  
  if (!fGeantEmcCat) {
    Error("HEmcDigitizer::init()","HGeant EMC input missing");
    return kFALSE;
  }
  iterGeantEmc = (HIterator *)fGeantEmcCat->MakeIterator("native");

  fGeantKineCat = (HLinearCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
  if(!fGeantKineCat){
      Error("HEmcDigitizer::init()","No catGeantKine in input!");
      return kFALSE;
  }
  
  // Build the Calibration category
  fCalCat = gHades->getCurrentEvent()->getCategory(catEmcCal);  
  if (fCalCat == NULL) {
    fCalCat = fEmcDet->buildMatrixCategory("HEmcCalSim",0.5);  
    gHades->getCurrentEvent()->addCategory(catEmcCal,fCalCat,"Emc");
  }
  if(fCalCat == NULL) return kFALSE;
  if(gHades->getEmbeddingMode()>0) embeddingmode = 1;
  
  
  fStartHitCat=gHades->getCurrentEvent()->getCategory(catStart2Hit);
  if (!fStartHitCat) Warning("init","Start hit level not defined; setting start time to 0");

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
  fSimulPar    =(HEmcSimulPar *)gHades->getRuntimeDb()->getContainer("EmcSimulPar");
  if (!fSimulPar){
    Error("initParContainer","No EmcSimulPar parameter container");
    return kFALSE;
  }
  return kTRUE;
}

Bool_t HEmcDigitizer::reinit(void) {
  // sets some local variables read from initialized parameter container
  sigmaT               = fDigiPar->getSigmaT();
  phot2Energy[0]       = fDigiPar->getPhot2E();        // for PMT type 1 (1.5inch)
  phot2Energy[1]       = fDigiPar->getPhot2E2();       // for PMT type 2 (3.0inch)
  Float_t sigmaEIntern = fDigiPar->getSigmaEIntern();
  Float_t sigmaEReal   = fDigiPar->getSigmaEReal();    // for PMT type 1 (1.5inch)
  Float_t sigmaEReal2  = fDigiPar->getSigmaEReal2();   // for PMT type 2 (3.0inch)
  facEnergSmear[0]     = 1000.*TMath::Sqrt(sigmaEReal*sigmaEReal - sigmaEIntern*sigmaEIntern);
  facEnergSmear[1]     = 1000.*TMath::Sqrt(sigmaEReal2*sigmaEReal2 - sigmaEIntern*sigmaEIntern);
  for(Int_t s=0;s<6;s++) labTrans[s] = fGeomPar->getModule(s)->getLabTransform();
  return kTRUE;
}

Int_t HEmcDigitizer::execute(void) {
  // Digitization of GEANT hits and storage in HEmcCalSim
  
  // Getting start time smearing
  Float_t startTimeSmearing = 0; //[ns]
  if (fStartHitCat) {
    HStart2Hit *pStartH = (HStart2Hit *) fStartHitCat->getObject(0);
    if(pStartH != NULL && pStartH->getResolution()!=-1000) {
      startTimeSmearing = pStartH->getResolution();
    }
  }
  
  // Case of embedding real tracks in the sim.data
  if(embeddingmode > 0) {
    if(gHades->getEmbeddingDebug() == 1) {
      fCalCat->Clear();
    } else {
      Int_t nentr = fCalCat->getEntries();
      for(Int_t n=0;n<nentr;n++) {
        HEmcCalSim* pCal = (HEmcCalSim*)fCalCat->getObject(n);
        Int_t sec  = pCal->getSector();
        Int_t cell = pCal->getCell();
        if (sec<0 || sec>5 || cell<0 || cell>=emcMaxComponents) {
          Warning("HEmcDigitizer:execute","EmcCal cell address invalid: sec=%i cell=%i",sec,cell);
          continue;
        }
        Int_t ind = cellObjectIndex(sec,cell);
        if(cellobjects[ind] == NULL) {
          cellobjects[ind] = new celldata;
          cellobjects[ind]->reset();
        }
        cellobjects[ind]->isEmbeddedReal = kTRUE;
        celltrack* celltr = new celltrack;
        celltr->reset();
        celltr->trackEn   = pCal->getEnergy();
        celltr->gtime     = pCal->getTime1();
        celltr->gtrack    = gHades->getEmbeddingRealTrackId();
        cellobjects[ind]->ctracks.push_back(celltr);
      }
    }
  }
  // loop over the HGeantEmc objects and fill temporary working objects
  iterGeantEmc->Reset();
  HGeantEmc*  geantemc = 0;
  while ((geantemc=(HGeantEmc *)iterGeantEmc->Next())!=0) {
    Int_t trackNumber = geantemc->getTrack();
    Int_t sec         = geantemc->getSector();
    Int_t cell        = geantemc->getCell();
    if (sec<0 || sec>5 || cell<0 || cell>=emcMaxComponents) {
      Warning("HEmcDigitizer:execute","Emc Geant cell address invalid: sec=%i cell=%i",sec,cell);
      continue;
    }
    if(trackNumber <= 0 && trackNumber != -777) continue; // nothing to do for real data
    
    Float_t peHit, xHit, yHit, zHit, tofHit, momHit, trackLength;
    geantemc->getHit(peHit, xHit, yHit, zHit, tofHit, momHit, trackLength);
    if(peHit == 0.) continue;                             // Number of photo electrons must be > 0

    Int_t pmtType = fSimulPar->getPmtType(sec,cell);
    if(pmtType < 1 || pmtType > 2) continue;    // never should happens
    Float_t energyHit = peHit * phot2Energy[pmtType-1];         // Convert to MeV
    
    Int_t ind = cellObjectIndex(sec,cell);
    if(cellobjects[ind] == NULL) {
      cellobjects[ind] = new celldata;
      cellobjects[ind]->reset();
      cellobjects[ind]->energy = 0.F;
    }
    celldata* cdata = cellobjects[ind];
    
    if(trackNumber == -777) {          // -777: hits with integrated number of photo electrons
      cdata->energy += energyHit;
    } else {
      HGeantEmc* pFirstEmc = getInputHit(geantemc,trackNumber);
      if(trackNumber <= 0 || pFirstEmc == NULL) continue;

      // Correction for the signal speed:
      tofHit -= (zHit+halfOfCellLength)/signalVelocity;
      // "zHit+halfOfCellLength" is distance from beginning of cell to the HGeandEmc hit

      Bool_t isInTheList = kFALSE;
      Int_t  numInpTrack = pFirstEmc->getTrack(); 

      if(std::find((cdata->inputTracks).begin(),(cdata->inputTracks).end(),numInpTrack) != cdata->inputTracks.end()) continue;
      cdata->inputTracks.push_back(numInpTrack);
      Float_t tofHitD;
      pFirstEmc->getHit(peHit, xHit, yHit, zHit, tofHitD, momHit, trackLength);
      Int_t pmtType = fSimulPar->getPmtType(sec,cell);
      energyHit = peHit * phot2Energy[pmtType-1];  // Convert to MeV

      // Is this track in the track list already:
      for(UInt_t i=0;i<cdata->ctracks.size();i++) {
        celltrack* celltr = cdata->ctracks[i];
        if (celltr->gtrack == trackNumber) {
          // The current track found in list
          isInTheList      = kTRUE;
          celltr->trackEn += energyHit;
          if (tofHit < celltr->gtime) celltr->gtime = tofHit;
          break;
        }
      }
      if( !isInTheList ) {
        // If track is not in the list yet
        // create new object for this cell and add them to the list of tracks for this cell
        celltrack* celltr = new celltrack;
        celltr->reset();
        celltr->trackEn   = energyHit;
        celltr->gtime     = tofHit;
        celltr->gtrack    = trackNumber;
        cdata->ctracks.push_back(celltr);
      }
    }
  } // end of HGeantEmc loop
  
  for(UInt_t i = 0; i < cellobjects.size(); i ++) {
    celldata* cdata = cellobjects[i];
    if (cdata!=NULL && cdata->energy>0.) {
      if(cdata->ctracks.size() == 0) continue;
      Int_t cell = cellFromIndex(i);
      fLoc[0] = sectorFromIndex(i);
      fLoc[1] = cell;
      HEmcCalSim* cal = (HEmcCalSim*) fCalCat->getSlot(fLoc);
      if (cal == NULL) {
        Warning("HEmcDigitizer:execute","HEmcCalSim:getSlot(loc) failed!");
        continue;
      }
      Float_t sigmaE = 0.;
      Float_t energy = 0.;
      if(cdata->energy > 0.) {
        Int_t pmtType = fSimulPar->getPmtType(fLoc[0],cell);
        sigmaE = TMath::Sqrt(cdata->energy/1000.) * facEnergSmear[pmtType-1];
        energy = gRandom->Gaus(cdata->energy,sigmaE);
      }
      if(!cdata->isEmbeddedReal) {
        cal = new(cal) HEmcCalSim;
        cal->setSector(fLoc[0]);
        cal->setCell(cell);
        Char_t  row,col;
        fEmcDet->getRowCol(cell,row,col);
        cal->setRow(row);
        cal->setColumn(col);
      } else {  // Embedded real hit
        energy += cal->getEnergy();
        sigmaE  = TMath::Sqrt(sigmaE*sigmaE + cal->getSigmaEnergy()*cal->getSigmaEnergy());
      }
      // ------  energy -------------
      cal->setEnergy(energy);
      cal->setSigmaEnergy(sigmaE);
      if(cal->getEnergy() < energyDepositCut) cal->setStatus1(-1);  // Energy deposit < threshold in this cell

      // ------  time and track numbers ------
      // Take track number and time from track when sum. of energy deposit exceed threshold
      cdata->sortTime();
      Float_t    energySum = 0.;
      celltrack* celltr    = NULL;
      for(UInt_t k = 0; k < cdata->ctracks.size(); k++ ) {
        celltr = cdata->ctracks[k];
        energySum += celltr->trackEn;
        if (energySum > energyDepositCut) break;
      }
      if(!cdata->isEmbeddedReal || gHades->getEmbeddingRealTrackId() != celltr->gtrack) {
        cal->setTime1(gRandom->Gaus(celltr->gtime,sigmaT) - startTimeSmearing);
        cal->setTime1Track(celltr->gtrack);
        cal->setSigmaTime1(sigmaT);
      } // else keep time and sigma from real hit
      
      // ------ sort by enegry  ---------------
      cdata->sortEnergy();
      for(UInt_t k = 0; k < cdata->ctracks.size() && k<5; k++ ) {
        celltrack* celltr = cdata->ctracks[k];
        cal->setTrack(celltr->gtrack, celltr->trackEn);
      }     
      
      cal->setNHits(1);
      cal->setTotMult(cdata->ctracks.size());
    }
  }
  
  clearCellobjects();               // clear temporary data
  return 0;
}

void  HEmcDigitizer::clearCellobjects(){
  // deletes objects in working array and sets pointer to 0
  // the vector is still not cleared
  for(UInt_t i = 0; i < cellobjects.size(); i++) {
    if(cellobjects[i]) cellobjects[i]->reset();
  }
}

HGeantEmc* HEmcDigitizer::getInputHit(HGeantEmc* pGeantEmc,Int_t &inputTrack) const {
  // Return pointer to the first HGeantEmc hit for track pGeantEmc->getTrack() or 
  // parent track if it has HGeantEmc hit in this cell
  // Return inputTrack: geant track number of the parent track of pGeantEmc->getTrack() 
  // which first reach EMC(or RPC) in this sector first
  Int_t track = pGeantEmc->getTrack();
  inputTrack  = track;
  Int_t       sec        = pGeantEmc->getSector();
  Int_t       cell       = pGeantEmc->getCell();
  HGeantKine* kine       = (HGeantKine*)fGeantKineCat->getObject(track-1);
  
  HGeantEmc*  firstHitInCell = pGeantEmc;
  
  do {
    track = kine->getTrack();
    Int_t first = kine->getFirstEmcHit();
    if(first != -1) {
      // track is in EMC
      kine->resetEmcIter();
      HGeantEmc* gemc = NULL;
      while( (gemc = (HGeantEmc*)kine->nextEmcHit()) != NULL) {
        if(gemc->getSector() != sec) continue;
        inputTrack = kine->getTrack();
        if(gemc->getCell() == cell) { 
          Float_t peHit,xHit, yHit, zHit, tofHit, momHit, trackLength;
          gemc->getHit(peHit, xHit, yHit, zHit, tofHit, momHit, trackLength);
          if(peHit > 0.) {
            firstHitInCell = gemc;
            break;  // Take the first hit
          }
        }
      }
    }
  } while((kine = kine->getParent(track,fGeantKineCat)) != NULL);
  
  if(zVertBorder < 0.) {
    // Test vertex of inputTrack and if it was borned in RPC region take parent track as inputTrack 
    HGeomVector ver;
    kine = (HGeantKine*)fGeantKineCat->getObject(inputTrack-1);
    while(kTRUE) {
      kine->getVertex(ver);
      ver  = labTrans[sec].transTo(ver);
      if(ver.getZ() < zVertBorder || ver.getZ()>0.) break;
      kine = kine->getParent(inputTrack,fGeantKineCat);
      if(kine == NULL) break;
      inputTrack = kine->getTrack();
    }
  }
    
  return firstHitInCell;
}
