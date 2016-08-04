//*-- Created : 04/12/2009 by J.Markert
//*-- Modified : 26/02/2014 by I.Koenig
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
//  HStart2HitFSim
//
//  This class implements the hit finder for the Start2 detector in simulation.
//  Since there is no START in simulation only the start hit with the given
//  time resultion is generated to give realistic resultion of the TOF and RPC.
//
//  For background simulations of the Pion beam an active Start detector was
//  implemented. The GEANT hits are digitized and the fastest hit is stored
//  with the integrated energy loss and the track number.
//  If the primary vertex is in front of the Start detector, the start time is
//  scaled to the center of the target. The task selectes the mode usually by
//  detecting the catStartGeantRaw category in the input file. if one has a file
//  containing the start, but one wants to skip the part then setDoGeantHit(kFALSE)
//  can be selected.
//  
///////////////////////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hiterator.h"
#include "hevent.h"
#include "hstart2hit.h"
#include "hstart2hitfsim.h"
#include "hstartdef.h"
#include "hgeantstart.h"
#include "hgeantkine.h"
#include "hruntimedb.h"
#include "hstart2geompar.h"
#include "hstart2digipar.h"
#include "hspecgeompar.h"
#include "hgeomvolume.h"

#include "TRandom.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HStart2HitFSim)
HStart2HitFSim* HStart2HitFSim::fHitFinder = NULL;
Bool_t          HStart2HitFSim::doGeantHit = kTRUE;
HStart2HitFSim::HStart2HitFSim(void)
   : HReconstructor()
{
   // default constructor
   fresolution    = 0.06;
   initVar();
}

HStart2HitFSim::HStart2HitFSim(const Text_t *name, const Text_t *title, Double_t resol)
   : HReconstructor(name, title)
{
   // constructor
   fresolution    = resol;
   initVar();
}

HStart2HitFSim::~HStart2HitFSim(void)
{
   // destructor
   if (iterGeantStart) delete iterGeantStart;
   clearCellobjects();
   cellobjects.clear();
}

void HStart2HitFSim::initVar(void)
{
  // sets variables in constructor
   fCatHit        = NULL;
   fCatGeantStart = NULL;
   iterGeantStart = NULL;
   fCatGeantKine  = NULL;
   fGeomPar       = NULL;
   fDigiPar       = NULL;
   fSpecGeomPar   = NULL;
   targetCenter.setXYZ(0.,0.,0.);
   fHitFinder     = this;
}

Bool_t HStart2HitFSim::init(void)
{
   // creates the Start2Hit category
   fCatHit = HCategoryManager::getCategory(catStart2Hit,2);
   if(!fCatHit) fCatHit = HCategoryManager::addCategory(catStart2Hit,"HStart2Hit",10,"Start",kFALSE);

   if(doGeantHit){
       // GEANT input data
       fCatGeantKine = HCategoryManager::getCategory(catGeantKine,2);
       fCatGeantStart = HCategoryManager::getCategory(catStartGeantRaw,2);
       if (fCatGeantKine && fCatGeantStart)
       {
	   iterGeantStart = (HIterator *)fCatGeantStart->MakeIterator("native");

	   return setParameterContainers();
       }
   }
   return kTRUE;
}

Bool_t HStart2HitFSim::reinit(void)
{
   if (fGeomPar && fGeomPar->hasChanged())
   {
      // working array with size constant over run time
      cellobjects.resize(fGeomPar->getMaxCells(), 0 );
   }
   if (fSpecGeomPar && fSpecGeomPar->hasChanged())
   {
      targetCenter.setXYZ(0.,0.,0.);
      Int_t numTargets=fSpecGeomPar->getNumTargets();
      HGeomVector* targets = new HGeomVector [numTargets];
      for(Int_t i=0;i<numTargets;i++)
      {
         HGeomVolume* targ = fSpecGeomPar->getTarget(i);
         if(!targ) {
	   Error("reinit","No target volume %i found",i);
           return kFALSE; 
         }
         targets[i] = targ->getTransform().getTransVector();
         Double_t zV1 = targ->getPoint(0)->getZ();
         Double_t zV2 = targ->getPoint(2)->getZ();
         targets[i].setZ(targets[i].getZ()+(zV1+zV2)/2.);
         Int_t t1 = i;
         for(Int_t t2=t1-1;t2>=0;t2--)
         {
            if(targets[t1].getZ()>targets[t2].getZ()) break;
            HGeomVector tmpVec(targets[t2]);
            targets[t2] = targets[t1];
            targets[t1] = tmpVec;
            t1 = t2;
         }  
      }
      targetCenter = targets[0];
      if (numTargets>1)
      {
         targetCenter += targets[numTargets-1];
         targetCenter*= 0.5;
      }
      delete [] targets;
   }
   return kTRUE;
}

Bool_t HStart2HitFSim::setParameterContainers(void)
{
   // get parameter containers for digitization for GEANT hits
   fGeomPar=(HStart2GeomPar*)gHades->getRuntimeDb()->getContainer("Start2GeomPar");
   if (!fGeomPar)
   {
      Error("setParameterContainers","No Start2GeomPar parameter container");
      return kFALSE;
   }
   fDigiPar    =(HStart2DigiPar *)gHades->getRuntimeDb()->getContainer("Start2DigiPar");
   if (!fDigiPar)
   {
      Error("setParameterContainers","No Start2DigiPar parameter container");
      return kFALSE;
   }
   fSpecGeomPar=(HSpecGeomPar*)gHades->getRuntimeDb()->getContainer("SpecGeomPar");
   if (!fSpecGeomPar)
   {
      Error("setParameterContainers","No SpecGeomPar parameter container");
      return kFALSE;
   }
   return kTRUE;
}

Int_t HStart2HitFSim::execute(void)
{
   // fills the HStartHit category

   if(!fCatHit) return 0;

   //--------------------------------------------------
   // decision sim or embedding : real taks will
   // create StartHit before sim task
   Bool_t exists = kFALSE;
   HStart2Hit* pHit = NULL;
   pHit = (HStart2Hit*) fCatHit->getObject(0);
   if(!pHit){
       Int_t index=-1;
       pHit = HCategoryManager::newObject(pHit,fCatHit,index);
   } else  exists = kTRUE;
   //--------------------------------------------------


   //--------------------------------------------------
   // find out if primaries are inserted in front of
   // the START : decision on vertex_z
   Bool_t doStartDigi = kFALSE;
   if(!exists&&doGeantHit&&fCatGeantKine&&fCatGeantStart) {
       HGeantKine* kine=0;
       Float_t vx,vy,vz;
       Float_t prim_vertexz=10000;
       for(Int_t i = 0; i < fCatGeantKine->getEntries();i++){
	   kine = HCategoryManager::getObject(kine,fCatGeantKine,i);
	   if(kine){
               if(kine->getParentTrack()>0) continue; // looking to primaries only
	       kine->getVertex(vx,vy,vz);
	       if(sqrt(vx*vx+vy*vy)>3000)   continue; // cosmic generator
	       if(vz<prim_vertexz) {
		   prim_vertexz = vz;
                   break;
	       }
	   }
       }
       if(prim_vertexz < -100) doStartDigi = kTRUE; // particle was inserted in front of target
   }
   //--------------------------------------------------

   //--------------------------------------------------
   // Fill the start hit
   Int_t   module    = 0;
   Int_t   strip     = 0;
   if (NULL != pHit) {
       Float_t res = gRandom->Gaus(0,fresolution);
       if(exists){
	   pHit->setResolution(res);
           pHit->setSimWidth(fresolution);

       } else {
	  if (!fCatGeantStart || !doStartDigi)   // doStartDigi will be kFALSE if doGeantHit=kFALSE
          {
	     pHit->setAddress(module, strip);
	     pHit->setTimeAndWidth(res, 1);
	     pHit->setResolution(res);
	     pHit->setSimWidth(fresolution);
	     pHit->setMultiplicity(1);
          } else digitize(pHit);
	  pHit->setFlag(kTRUE);
	  pHit->setCorrFlag(2);
          pHit->setIteration(2);
       }
   } else {
       Error("execute", "Can't get slot");
       return 0;
   }
   //--------------------------------------------------

   return 0;
}

void HStart2HitFSim::digitize(HStart2Hit* pHit)
{
   // digitizes GEANT START hits and fills the HStartHit category

   HGeantStart* pGeantStart = NULL;

   Int_t   gCell       = -1;
   Int_t   trackNumber = -1;
   Float_t eHit        = 0.F;
   Float_t xHit        = 0.F;
   Float_t yHit        = 0.F;
   Float_t tofHit      = 0.F;
   Float_t momHit      = 0.F;
   Float_t trackLength = 0.F;

   //loop over the HGeantStart objects and fill temporary working objects
   clearCellobjects();
   iterGeantStart->Reset();
   while ((pGeantStart=(HGeantStart*)iterGeantStart->Next())!=0)
   {  
      trackNumber = pGeantStart->getTrack();
      gCell = pGeantStart->getCell();
      if (gCell<0 || gCell>=fGeomPar->getMaxComponents())
      {
         Warning("HStart2Digitizer:execute","Start Geant cell address invalid: cell=%i",gCell);
         continue;
      }                     
      pGeantStart->getHit(eHit, xHit, yHit, tofHit, momHit, trackLength);
      Int_t cellInd = fGeomPar->getCellIndex(gCell, xHit, yHit);
      if (cellobjects[cellInd] == 0)
      { // first time we have to create the object
         cellobjects[cellInd] = new celldata;
      }
      celldata* cdata = cellobjects[cellInd];
      celltrack* celltr;
      // CASE I: no tracks in this cell yet
      if (cdata->ctracks.size()==0)
      {
      // create new object for this cell and add them to the list of tracks for this cell
      celltr = new celltrack;
      celltr->reset();
      cdata->ctracks.push_back(celltr);
      } else
      {
         // CASE II: this cell has already tracks
         // check if current track is already present in the list 
         for(UInt_t i=0;i<cdata->ctracks.size();i++)
         {
            celltr = cdata->ctracks[i];
            // CASE II a: the current track found in list
            if (celltr->gtrack == trackNumber) break;
         }
         // CASE II b: the current track is not in list yet
         if (celltr->gtrack != trackNumber)
         {
            celltr = new celltrack;
            celltr->reset();
            cdata->ctracks.push_back(celltr);
         }
      }
      if (tofHit < celltr->gtime)
      {
         celltr->gtime     = tofHit;
         celltr->gtrack    = trackNumber;
      }
      if (eHit > celltr->geloss)
      {
         celltr-> geloss = eHit;
      }
   } // end of HGeantStart loop

   Float_t time1    = 10000.f;
   Int_t   track    = -1;
   Int_t   module   = 0;
   Int_t   strip    = 0;
   Int_t   nMult    = 0;
   Float_t totEloss = 0.f;
   for(UInt_t i = 0; i < cellobjects.size(); i ++)
   {
      celldata* cdata = cellobjects[i];
      if (cdata && cdata->ctracks.size() > 0 )
      {
         // ------  sort by time  ----------------
         cdata->sortTime();
         celltrack* celltr = cdata->ctracks[0];
         // ------  time of fastest track    -----
         Float_t gtime = celltr->gtime;
         // ------  integrated energy loss  ------
         Float_t energy = celltr->geloss; 
         Float_t dt = gtime + fDigiPar->getDeltaTimeE();
         for (UInt_t k=1;k<cdata->ctracks.size();k++)
         {
            celltrack* celltr2 = cdata->ctracks[k];
            if (celltr2->gtime < dt)
            {
               energy += celltr2->geloss;
            }
         }
         // smear energy loss and require minimum energy loss
         Float_t energyS = gRandom->Gaus(energy,energy*fDigiPar->getSigmaE());
         if (gtime>0.f && energyS>fDigiPar->getMinEloss())
         {
            nMult++;
            // scale time to target center
            const HGeomVector* cellPos = fGeomPar->getLabPos(i);
            HGeantKine* pKine = HGeantKine::getPrimary(celltr->gtrack,(HLinearCategory*)fCatGeantKine);
            if (cellPos && pKine)
            {
               Float_t vx, vy, vz, cellz, dist;
               pKine->getVertex(vx,vy,vz);
               cellz = cellPos->getZ();
               if (vz < (cellz-0.1f))
               {
                  dist = cellz - vz;
                  gtime = (targetCenter.getZ()-vz) / dist * gtime;
               }
            }
            // smear time and find fastest hit
            Float_t time = gRandom->Gaus(gtime,fDigiPar->getSigmaT());
            if (time<time1)
            {
               strip    = i;
               time1    = time;
               totEloss = energyS;
               track    = celltr->gtrack;
            }
         } 
      }
   } // end loop cells

   if (nMult==0) time1 = fDigiPar->getTimeNoHit();
   pHit->setAddress(module, strip);
   pHit->setTimeAndWidth(time1,totEloss);
   pHit->setResolution(time1);
   pHit->setMultiplicity(nMult);
   pHit->setTrack(track);
}

void HStart2HitFSim::clearCellobjects(){
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
