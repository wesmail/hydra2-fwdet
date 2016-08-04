//*-- Created : 04/12/2009 by I.Koenig
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
//  HStart2HitF
//
//  This class implements the hit finder for the Start2 detector.
//
///////////////////////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hevent.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hstart2cal.h"
#include "hstart2detector.h"
#include "hstart2hit.h"
#include "hstart2hitf.h"
#include "hstart2hitfpar.h"
#include "hstartdef.h"

#include <iomanip>
#include <iostream>
#include <algorithm>

using namespace std;

ClassImp(HStart2HitF)

HStart2HitF::HStart2HitF(void)
   : HReconstructor()
{
   // default constructor

   fCatCal    = NULL;
   fCatHit    = NULL;
   fPar       = NULL;
   fSkipEvent = kFALSE;
}

HStart2HitF::HStart2HitF(const Text_t *name, const Text_t *title, Bool_t skip)
   : HReconstructor(name, title)
{
   // constructor

   fCatCal    = NULL;
   fCatHit    = NULL;
   fPar       = NULL;
   fSkipEvent = skip;
}

Bool_t HStart2HitF::init(void)
{
   // gets the Start2Cal category and creates the Start2Hit category
   // creates an iterator which loops over all cells in Start2Cal
   // initialize parameter container

   HStart2Detector* det = static_cast<HStart2Detector*>(gHades->getSetup()->getDetector("Start"));
   if (NULL == det) {
      Error("init", "No Start Detector found.");
      return kFALSE;
   }
   fCatCal = gHades->getCurrentEvent()->getCategory(catStart2Cal);
   if (NULL == fCatCal) {
      Error("init", "HStart2Cal category not available!");
      return kFALSE;
   }
   fCatHit = det->buildCategory(catStart2Hit);
   if (NULL == fCatHit) {
      Error("init", "HStart2Hit category not available!");
      return kFALSE;
   }

   HRuntimeDb* rtdb = gHades->getRuntimeDb();
   fPar = static_cast<HStart2HitFPar*>(rtdb->getContainer("Start2HitFPar"));
   if (NULL == fPar) {
      Error("init", "Pointer to Start2HitFPar parameters is NULL");
      return kFALSE;
   }

   loc.set(1, 0);
   fActive = kTRUE;

   return kTRUE;
}

Int_t HStart2HitF::execute(void)
{
   // makes the hits and fills the HStartHit category


   HStart2Cal* pCal = NULL;
   HStart2Hit* pHit = NULL;

   Bool_t foundTime = kFALSE;
   Int_t  module    = -1;
   Int_t  strip     = -1;

   fSecondTime      = -10000;
   Int_t newstrip   = -1;
   Float_t firstTime= -10000;
   firstCluster .clear();
   secondCluster.clear();

   for (Int_t entry = 0; entry < fCatCal->getEntries(); ++entry) {

      if (kTRUE == foundTime) {
         break;
      }

      if (NULL == (pCal = static_cast<HStart2Cal*>(fCatCal->getObject(entry)))) {
         Error("execute", "Pointer to HStart2Cal object == NULL, returning");
         return -1;
      }

      // select start data (module == 0)
      if (0 != pCal->getModule()) {
         continue;
      }

      // check the 4 times and apply a time cut to remove BKG
      // in strip 7 at least the reference time after calibration
      // should be in the data

      for (Int_t i = 0; i < pCal->getMultiplicity() && i < pCal->getMaxMultiplicity(); ++i) {
         if (pCal->getTime(i + 1) > (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) - fPar->getWidth(pCal->getModule(), pCal->getStrip())) &&
             pCal->getTime(i + 1) < (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) + fPar->getWidth(pCal->getModule(), pCal->getStrip()))) {
            foundTime = kTRUE;
            pCal->getAddress(module, strip);
            // write data to the ouput category
            pHit = static_cast<HStart2Hit*>(fCatHit->getSlot(loc));
            if (NULL != pHit) {
               pHit = new(pHit) HStart2Hit;
               pHit->setAddress(module, strip);
               pHit->setTimeAndWidth(pCal->getTime(i + 1), pCal->getWidth(i + 1));
               pHit->setFlag(kTRUE);
	       pHit->setMultiplicity(pCal->getMultiplicity());
	       firstTime = pHit->getTime();
               newstrip = entry * 1000 + i+1;
            } else {
               Error("execute", "Can't get slot mod=%i, chan=%i", loc[0], loc[1]);
               return -1;
            }
         }
      }
   }

  // if no START time was found, now checks module 1
  if (kFALSE == foundTime) {
     for (Int_t entry = 0; entry < fCatCal->getEntries(); ++entry) {

        if (kTRUE == foundTime) {
           break;
        }

        if (NULL == (pCal = static_cast<HStart2Cal*>(fCatCal->getObject(entry)))) {
           Error("execute", "Pointer to HStart2Cal object == NULL, returning");
           return -1;
        }

        // select start data (module == 1)
        if (1 != pCal->getModule()) {
           continue;
        }

        // check the 4 times and apply a time cut to remove BKG
        // in strip 7 at least the reference time after calibration
        // should be in the data

        for (Int_t i = 0; i < pCal->getMultiplicity() && i < pCal->getMaxMultiplicity(); ++i) {
           if (pCal->getTime(i + 1) > (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) - fPar->getWidth(pCal->getModule(), pCal->getStrip())) &&
               pCal->getTime(i + 1) < (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) + fPar->getWidth(pCal->getModule(), pCal->getStrip()))) {
              foundTime = kTRUE;
              pCal->getAddress(module, strip);
              // write data to the ouput category
              pHit = static_cast<HStart2Hit*>(fCatHit->getSlot(loc));
              if (NULL != pHit) {
                 pHit = new(pHit) HStart2Hit;
                 pHit->setAddress(module, strip);
                 pHit->setTimeAndWidth(pCal->getTime(i + 1), pCal->getWidth(i + 1));
                 pHit->setFlag(kTRUE);
                 pHit->setMultiplicity(pCal->getMultiplicity());
		 firstTime = pHit->getTime();
		 newstrip = entry * 1000 + i+1;
              } else {
                 Error("execute", "Can't get slot mod=%i, chan=%i", loc[0], loc[1]);
                 return -1;
              }
           }
        }
     }
   }

   //----------------------------------------------------------------------
   // cluster info's
   vector<Int_t> vtemp;
   Float_t maxdiff   = 2; // ns with arround starttime used to cluster
   Float_t newStart2 = -100000;  // clostest scond time
   Float_t timediff  =  100000;  // local var
   Int_t    nextHit  = -1;

   if(foundTime) firstCluster.push_back(newstrip);

   for (Int_t entry = 0; entry < fCatCal->getEntries(); ++entry)
   {

       if (NULL == (pCal = static_cast<HStart2Cal*>(fCatCal->getObject(entry)))) {
	   Error("execute", "Pointer to HStart2Cal object == NULL, returning");
	   return -1;
       }

       // reject veto
       if (pCal->getModule() > 1) {
	   continue;
       }

       // check the 4 times and apply a time cut to remove BKG
       // in strip 7 at least the reference time after calibration
       // should be in the data

       for (Int_t i = 0; i < pCal->getMultiplicity() && i < pCal->getMaxMultiplicity(); ++i) {
	   if (pCal->getTime(i + 1) > (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) - fPar->getWidth(pCal->getModule(), pCal->getStrip())) &&
	       pCal->getTime(i + 1) < (fPar->getMeanTime(pCal->getModule(), pCal->getStrip()) + fPar->getWidth(pCal->getModule(), pCal->getStrip()))) {
               Float_t time = pCal->getTime(i + 1);
               newstrip     = entry * 1000 + i+1;
               Float_t diff = TMath::Abs(firstTime-time) ;

	       if(diff < maxdiff){ // first cluster

		   if(find(firstCluster.begin(),firstCluster.end(),newstrip)==firstCluster.end()){
		       firstCluster.push_back(newstrip);
		   }

	       } else {  // second cluster

		   vtemp.push_back(newstrip);

		   if(diff<timediff) {
		       timediff = diff;
		       nextHit  = newstrip;
                       newStart2= time;
		   }

	       } // end second clust
	   } // end window
       } // hitloop
   } // end strip loop

   if(vtemp.size()>0){  // there was something else
       fSecondTime = newStart2;
       secondCluster.push_back(nextHit);
       for(UInt_t i=0;i<vtemp.size();i++){
	   if(find(secondCluster.begin(),secondCluster.end(),vtemp[i])==secondCluster.end()){
	       secondCluster.push_back(vtemp[i]);
	   }
       }
   }


   //--------------------------------------------------------------------
   // fill cluster infos
   if(firstCluster.size()>0){
       pHit = static_cast<HStart2Hit*>(fCatHit->getObject(0));
       if (NULL == pHit) {
	   Error("execute()", "Pointer to HStart2Hit object == NULL");

       } else {
           pHit->setIteration(1);
	   pHit->resetClusterStrip(2);
	   for(UInt_t i=0;i<firstCluster.size();i++){
	       pHit->setClusterStrip (0, firstCluster[i]);
	   }
	   for(UInt_t i=0;i<secondCluster.size();i++){
	       pHit->setClusterStrip (1, secondCluster[i]);
	   }
	   pHit->setSecondTime(fSecondTime);

	   if(secondCluster.size()>0) pHit->setMultiplicity(2);
	   else                       pHit->setMultiplicity(1);
       }
   }
   //--------------------------------------------------------------------


   //----------------------------------------------------------------------

















   if (kFALSE == foundTime && kTRUE == fSkipEvent) {
      return kSkipEvent;
   }

   return 0;
}
