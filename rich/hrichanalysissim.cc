//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <Laura.Fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichAnalysisSim
//
//  This class besides the Rich Analysis contains
//  the necessary functions to retrieve the parent track
//  number corresponding to each pad that belongs to
//  a Ring structure.
//  The Ring Finding algorithm is executed by the class
//  HRichRingFindSim. This class contains 2 different
//  algorithms to find a ring, once a ring is found
//  the track numbers of the particles that have hit
//  the pads belonging to the ring structure are memorized
//  in an array. (Int_t *iRingTrack).
//  Once all track numbers are collected,
//  3 tracks number are saved ( HRichHitSim:track1....)
//  for each Hit with the corresponding weights (cfr. updateHits).
//  (The weight
//  is the number of pads a photon or a Ip has fired.)
//  A flag for each of the three track numbers is saved too,
//  to be able to distinguish between a Chrenkov photon
//  and a direct hit.
//
//  use kSkip in the constructor to skip writing of
//  event if no ring was found
//
//////////////////////////////////////////////////////////////////////////////


#include "hades.h"
#include "hevent.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hlinearcategory.h"
#include "hlocation.h"
#include "hmatrixcategory.h"
#include "hparcond.h"
#include "hparset.h"
#include "hrichanalysispar.h"
#include "hrichanalysissim.h"
#include "hrichcalsim.h"
#include "hrichdetector.h"
#include "hrichgeometrypar.h"
#include "hrichhitsim.h"
#include "hrichpadclean.h"
#include "hrichpadlabel.h"
#include "hrichringfindsim.h"
#include "hrichtrack.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichAnalysisSim)

//----------------------------------------------------------------------------
HRichAnalysisSim::HRichAnalysisSim(void)
{
   pRingFindSim   = new HRichRingFindSim;
   pRichCalSim    = NULL;
   fIterHitHeader = NULL;
   m_pTrackCat    = NULL;
   pRings         = NULL;

}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysisSim::HRichAnalysisSim(const Text_t*  name,
                                   const Text_t*  title,
                                   Bool_t   kSkip)
   : HRichAnalysis(name, title, kSkip)
{
   pRingFindSim     = new HRichRingFindSim;
   pRichCalSim      = NULL;
   fIterHitHeader   = NULL;
   m_pTrackCat      = NULL;
   pRings           = NULL;
   kSkipEvtIfNoRing = kSkip;
}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysisSim::~HRichAnalysisSim(void)
{
   if (NULL != pRingFindSim) {
      delete pRingFindSim;
      pRingFindSim = NULL;
   }
   if (NULL != fIterHitHeader) {
      delete  fIterHitHeader;
      fIterHitHeader = NULL;
   }
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysisSim::init()
{
// allocate input/output categories

   HRichDetector*    pRichDet     = NULL;
   HRuntimeDb*       rtdb         = NULL;
   HRichAnalysisPar* pAnalysisPar = NULL;
   HRichGeometryPar* pGeomPar     = NULL;

   pRichDet = static_cast<HRichDetector*>(gHades->getSetup()
                                          ->getDetector("Rich"));
   if (NULL == pRichDet) {
      Error("init", "Pointer to HRichDetector is NULL");
      return kFALSE;
   }

   m_pCalCat = gHades->getCurrentEvent()->getCategory(catRichCal);
   if (NULL == m_pCalCat) {
      m_pCalCat = pRichDet->buildMatrixCat("HRichCalSim", 1);
      if (NULL == m_pCalCat) {
         Error("init", "Pointer to HRichCalSim category is NULL");
         return kFALSE;
      } else
         gHades->getCurrentEvent()
         ->addCategory(catRichCal, m_pCalCat, "Rich");
   }
   fIter = static_cast<HIterator*>(m_pCalCat->MakeIterator());
   if (NULL == fIter) {
      Error("init", "Pointer to HRichCalSim iterator is NULL");
      return kFALSE;
   }

   m_pHitCat = gHades->getCurrentEvent()->getCategory(catRichHit);
   if (NULL == m_pHitCat) {
      m_pHitCat = pRichDet->buildLinearCat("HRichHitSim");
      if (NULL == m_pHitCat) {
         Error("init", "Pointer to HRichHitSim category is NULL");
         return kFALSE;
      } else gHades->getCurrentEvent()
         ->addCategory(catRichHit, m_pHitCat, "Rich");
   }

   m_pHitHdrCat = gHades->getCurrentEvent()->getCategory(catRichHitHdr);
   if (NULL == m_pHitHdrCat) {
      m_pHitHdrCat = pRichDet->buildCategory(catRichHitHdr);
      if (NULL == m_pHitHdrCat) {
         Error("init", "Pointer to HRichHitHdr category is NULL");
         return kFALSE;
      } else gHades->getCurrentEvent()
         ->addCategory(catRichHitHdr, m_pHitHdrCat, "Rich");
   }
   fIterHitHeader = static_cast<HIterator*>(m_pHitHdrCat->MakeIterator("native"));
   if (NULL == fIterHitHeader) {
      Error("init", "Pointer to HRichHitHdr iterator is NULL");
      return kFALSE;
   }

   m_pTrackCat = gHades->getCurrentEvent()->getCategory(catRichTrack);
   if (NULL == m_pTrackCat) {
      m_pTrackCat = pRichDet->buildLinearCat("HRichTrack", 1002);
      if (NULL == m_pTrackCat) {
         Error("init", "Pointer to HRichTrack category is NULL");
         return kFALSE;
      } else gHades->getCurrentEvent()
         ->addCategory(catRichTrack, m_pTrackCat , "Rich");
   }

   rtdb = gHades->getRuntimeDb();
   pAnalysisPar = static_cast<HRichAnalysisPar*>(rtdb->
                                                 getContainer("RichAnalysisParameters"));
   if (NULL == pAnalysisPar) {
      Error("init", "Pointer to HRichAnalysisPar parameters is NULL");
      return kFALSE;
   }
   setAnalysisPar(pAnalysisPar);

   pGeomPar = static_cast<HRichGeometryPar*>(rtdb->
                                             getContainer("RichGeometryParameters"));
   if (NULL == pGeomPar) {
      Error("init", "Pointer to RichGeometryParameters parameters is NULL");
      return kFALSE;
   }
   setGeometryPar(pGeomPar);

   if (kFALSE == initParameters()) {
      Error("init", "Parameters not initialized");
      return kFALSE;
   }

   if (kFALSE == pPadClean->init()) {
      Error("init", "HRichPadClean did not initialize");
      return kFALSE;
   }

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t HRichAnalysisSim::reinit()
{

   UInt_t i, j, k, m, n;

   if (kFALSE == initParameters()) {
      Error("init", "Parameters not initialized");
      return kFALSE;
   }

   pRingFindSim->init(this);


   iPadActive.Set(maxCols * maxRows);
   for (i = 0 ; i < (UInt_t)maxCols * maxRows; ++i)
      if (getGeometryPar()->getPadsPar()->getPad(i)->getPadActive() > 0)
         iPadActive[i] = 1;
      else iPadActive[i] = 0;

   Int_t iMatrixSurface = 0, iPartSurface = 0;
   Int_t iMaskSize = getAnalysisPar()->iRingMaskSize;
   Int_t iMaskSizeSquared = iMaskSize * iMaskSize;
   for (k = 0; k < (UInt_t)iMaskSizeSquared; k++)
      if (getAnalysisPar()->iRingMask[k] == 1) iMatrixSurface++;


   for (j = 0; j < (UInt_t)maxRows; j++)
      for (i = 0; i < (UInt_t)maxCols; i++)   {
         iPartSurface = 0;
         for (k = 0; k < (UInt_t)iMaskSizeSquared; k++) {
            m = (k % iMaskSize) - iMaskSize / 2;
            n = (k / iMaskSize) - iMaskSize / 2;
            if (!IsOut(i, j, m, n))
               if (getAnalysisPar()->iRingMask[k] == 1) iPartSurface++;
         }
         getGeometryPar()->getPadsPar()->
         getPad(i, j)->setAmplitFraction((Float_t)iPartSurface / (Float_t)iMatrixSurface);
      }


   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysisSim::initParameters()
{
// allocate non event by event classes

   iRingNrTot = 0;
   allPairNrTot = 0;
   sectorPairNrTot = 0;
   maxFiredTotalPads  = 3000; // upper limit of number of fired pads

   maxCols = getGeometryPar()->getColumns();
   maxRows = getGeometryPar()->getRows();
   if (pLeftBorder) delete [] pLeftBorder;
   pLeftBorder = new Short_t[maxRows];
   if (pRightBorder) delete [] pRightBorder;
   pRightBorder = new Short_t[maxRows];

   for (Int_t row = 0; row < maxRows; ++row) {
      Int_t col = 0;
      Int_t padOffset = row * maxCols;
      while (!getGeometryPar()->getPadsPar()->getPad(col + padOffset)->getPadActive() && col < maxCols) ++col;
      if (col == maxCols) {
         maxRows = row;
         break;
      }
      pLeftBorder[row] = col;
      while (getGeometryPar()->getPadsPar()->getPad(col + padOffset)->getPadActive() && col < maxCols) ++col;
      pRightBorder[row] = col - 1;
   }
   maxPads = maxRows * maxCols;


   // now creating pads array

   Int_t i;
   Int_t j;
   if (pPads) {
      for (j = 0; j < 6; j++)
         if (pPads[j])
            delete [] pPads[j];
      delete [] pPads;
   }

   pPads = new HRichPadSignal * [6];
   for (j = 0; j < 6; j++) {
      pSectorPads = pPads[j] = new HRichPadSignal[maxPads];
      for (i = 0; i < maxPads; pSectorPads[i++].clear());
   }
   pSectorPads = pPads[iActiveSector];

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysisSim::getPadsNTrack1(Int_t padx, Int_t pady, Int_t sec)
{
// This function returns for each pad the corresponding
// NTrack1 value (cfr. HRichCalSim, HRichDigitizer::execute).
// All track numbers are stored during digitization  in the
// catRichTrack Linear Category.
// NTrack1 is the index in the Track array corresponding to the
// first track for each pad.
//
// This function is called from HRichRingFindSim::CalcRingParamters.

   HLocation loc1;
   loc1.set(3, sec, pady, padx);

   pRichCalSim = (HRichCalSim*)((HMatrixCategory*)getCalCat())->getObject(loc1);
   if (NULL != pRichCalSim)
      return pRichCalSim->getTrackId(0);
   return 0;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysisSim::getPadsNTrack2()
{
// This functions returns NTrack2, which is the index in
// the Track Array corresponding to the last track for each pad.
// Pad must be identified by calling getPadsNTrack1(...) first.

   if (NULL != pRichCalSim)
      return pRichCalSim->getTrackId(1);
   return 0;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysisSim::getFlag(Int_t index)
{
// It is called from HRichRingFindSim::LookForTrack().
// This function returns the flag contained in the
// catRichTrack container at the position index.
// This flag is 0 for Chrenkov photons and 1 for IP.

   HRichTrack *trk = NULL;

   trk = (HRichTrack*)((HLinearCategory*)m_pTrackCat)->getObject(index);
   if (NULL != trk)
      return trk->getFlag();
   return  -1;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysisSim::getTrack(Int_t index)
{
//  This function returns the track number contained in the
//  catRichTrack container at the position index.

   HRichTrack *trk = NULL;

   trk = ((HRichTrack*)((HLinearCategory*)m_pTrackCat)->getObject(index));
   if (NULL != trk)
      return trk->getTrack();
   return -10;

}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysisSim::execute()
{

   HRichCal*  pCal           = NULL;;
   Int_t      iRingNrTotEvt  = 0;
   Int_t      fPadFiredTot   = 0;
   Int_t      sector         = -1;
   Int_t      padnr          = -1;
   Int_t      j;
   Int_t      ampl;

   Reset();
   for (sector = 0; sector < 6; ++sector) {
      if (fPadFired[sector] > 0) {
         fPadFired[sector] = 0;
         HRichPadSignal* pSecPads = pPads[sector];
         for (j = 0; j < maxPads; pSecPads[j++].clear());
      }
   }

   fIter->Reset();
   while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
      if ((ampl = (Int_t)pCal->getCharge()) > 0) {
         fPadFired[pCal->getSector()]++;
	 padnr = pCal->getCol() + pCal->getRow() * maxCols;
	 if(padnr >= maxPads) {
	     Warning("execute()","pPads out of range ! signal will be skipped.");
		 continue;
	 }
         pPads[pCal->getSector()][padnr].setAmplitude(ampl);
      }
   }

   fPadFiredTot = 0;
   for (sector = 0; sector < 6; ++sector) {
      fPadFiredTot += fPadFired[sector];
      if (fPadFired[sector] >= fpAnalysisPar->maxFiredSectorPads) {
         Warning("execute", "To many fired pads in sector %i. %i/%i",
                 sector, fPadFired[sector], fpAnalysisPar->maxFiredSectorPads);
         fIter->Reset();
         while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
            if (pCal->getSector() == sector)
               pCal->setIsCleanedSector(kTRUE);
         }
         return 0;
      }
   }

   if (fPadFiredTot > maxFiredTotalPads) {
      Warning("execute", "Analysis of event skipped: too many pads fired %i/%i",
              fPadFiredTot, maxFiredTotalPads);
      fIter->Reset();
      while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
         pCal->setIsCleanedSector(kTRUE);
      }
      return 0;
   }

   // **************************************************************
   // ------- loop over sectors --- begin ---
   for (sector = 0; sector < 6; ++sector)
      if (getGeometryPar()->getSectorActive(sector) > 0) {

         Reset();
         SetActiveSector(sector);
         iPadFiredNr = fPadFired[sector];

         iPadCleanedNr = pPadClean->Execute(this);
         iLabelNr      = pPadLabel->Execute(this);

         if (0 == getAnalysisPar()->isActiveLabelPads) {
            iLabelNr = 1;
            pLabelArea = new HRichLabel[1];
            pLabelArea[0].iLeftX = 0;
            pLabelArea[0].iRightX = maxCols;
            pLabelArea[0].iLowerY = 0;
            pLabelArea[0].iUpperY = maxRows;
            pLabelArea[0].iLabeledPadsNr = maxPads;
            pLabelArea[0].iFiredPadsNr = iPadFiredNr;
            pLabelArea[0].iSignature = 0;
         }

         iRingNr = pRingFindSim->Execute(this);
         iRingNrTot    += iRingNr;
         iRingNrTotEvt += iRingNr;
         updateHeaders(sector, gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
         updateHits(sector);
      }
   // ------- loop over sectors --- end ---

   fIter->Reset();
   while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
       padnr = pCal->getCol() + pCal->getRow() * maxCols;
       if(padnr >= maxPads){ // warning already above
	   continue;
	 }
      pCal->setIsCleanedSingle(pPads[pCal->getSector()][padnr].getIsCleanedSingle());
      pCal->setIsCleanedHigh(pPads[pCal->getSector()][padnr].getIsCleanedHigh());
   }

   // modification to skip event which does not contain any ring
   if (kTRUE == kSkipEvtIfNoRing && 0 == iRingNrTotEvt)
      return kSkipEvent;

   return 0;

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysisSim::updateHits(Int_t nSec)
{
// Each Hit is stored, in addition to real data for simulated one
// the tracks number corresponding to each ring are stored too.
// Maximal 3 tracks are stored for each ring. The track numbers
// are sorted according to their weights.

   HRichHitSim *hit = NULL;
   HLocation    loc;

   for (Int_t i = 0; i < iRingNr; i++) {

      loc.set(1, nSec);

      hit = (HRichHitSim *)m_pHitCat->getNewSlot(loc, NULL);
      if (hit != NULL) {
         hit = new(hit) HRichHitSim;

         HRichHitSim & ring = pRings[i];

         *hit = ring;
         hit->setSector(nSec);

         HRichPad * pad = getGeometryPar()->getPadsPar()->
                          getPad((UInt_t)ring.getRingCenterX(), (UInt_t)ring.getRingCenterY());
         if (pad) {
            hit->setTheta(pad-> getTheta());
            hit->setPhi(pad->getPhi(nSec));
         } else {
            hit->fPhi = -10;
            hit->fTheta = -10;
         }

	 Int_t k = 0;
         while (ring.iRingTrack[k]) {

            if (hit->track1 == 0) {
               hit->track1 = ring.iRingTrack[k];
               hit->flag1 =  ring.iRingFlag[k];
               (hit->weigTrack1)++;
            } else {
               if (ring.iRingTrack[k] == hit->track1) {
                  (hit->weigTrack1)++;

               } else {
                  if (hit->track2 == 0) {
                     hit->track2 = ring.iRingTrack[k];
                     hit->flag2 =  ring.iRingFlag[k];
                     (hit->weigTrack2)++;
                  } else {
                     if (ring.iRingTrack[k] == hit->track2) {
                        (hit->weigTrack2)++;

                     } else {
                        if (hit->track3 == 0) {
                           hit->track3 = ring.iRingTrack[k];
                           hit->flag3 =  ring.iRingFlag[k];
                           (hit->weigTrack3)++;
                        } else (hit->weigTrack3)++;
                     }
                  }
               }
            }
            k++;

         }//end loop on track array

         sortTracks(hit);

      }

   }//end loop on rings

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysisSim::sortTracks(HRichHitSim *hit)
{
// Sorting tracks according to their weights
// Track with the highest weight is the first one

   Int_t tmp[3];
   tmp[0] = hit->track1;
   tmp[1] = hit->flag1;
   tmp[2] = hit->weigTrack1;

   if (hit->weigTrack1 < hit->weigTrack2) { // 2>1
      if (hit->weigTrack2 > hit->weigTrack3) { // 2 is the largest
         hit->track1     = hit->track2;
         hit->flag1      = hit->flag2;
         hit->weigTrack1 = hit->weigTrack2;
         if (tmp[2] < hit->weigTrack3) { // 2>3>1
            hit->track2     = hit->track3;
            hit->flag2      = hit->flag3;
            hit->weigTrack2 = hit->weigTrack3;
            hit->track3     = tmp[0];
            hit->flag3      = tmp[1];
            hit->weigTrack3 = tmp[2];
         } else { // 2>1>=3
            hit->track2     = tmp[0];
            hit->flag2      = tmp[1];
            hit->weigTrack2 = tmp[2];
         }
      } else { // 3>= 2; 2>1
         hit->track1     = hit->track3;
         hit->flag1      = hit->flag3;
         hit->weigTrack1 = hit->weigTrack3;
         hit->track3     = tmp[0];
         hit->flag3      = tmp[1];
         hit->weigTrack3 = tmp[2];
      }
   } else { // 1>=2
      if (hit->weigTrack1 < hit->weigTrack3) { // 3>1>=2
         hit->track1     = hit->track3;
         hit->flag1      = hit->flag3;
         hit->weigTrack1 = hit->weigTrack3;
         hit->track3     = hit->track2;
         hit->flag3      = hit->flag2;
         hit->weigTrack3 = hit->weigTrack2;
         hit->track2     = tmp[0];
         hit->flag2      = tmp[1];
         hit->weigTrack2 = tmp[2];
      } else { // 1>=2; 1>=3
         if (hit->weigTrack2 < hit->weigTrack3) { // 1>=3>2
            tmp[0] = hit->track2;
            tmp[1] = hit->flag2;
            tmp[2] = hit->weigTrack2;
            hit->track2     = hit->track3;
            hit->flag2      = hit->flag3;
            hit->weigTrack2 = hit->weigTrack3;
            hit->track3     = tmp[0];
            hit->flag3      = tmp[1];
            hit->weigTrack3 = tmp[2];
         }
      } // 1>=2>=3
   }


   if(hit->track2 < 0 && hit->track3 > 0 &&
      hit->weigTrack2 > 0 &&  hit->weigTrack2 == hit->weigTrack3) {
       // move track2 down in the list (if it is noise)
       // and if track has the same weight as track3 (a "good" one!)
       tmp[0] = hit->track2;
       tmp[1] = hit->flag2;
       tmp[2] = hit->weigTrack2;
       hit->track2     = hit->track3;
       hit->flag2      = hit->flag3;
       hit->weigTrack2 = hit->weigTrack3;
       hit->track3     = tmp[0];
       hit->flag3      = tmp[1];
       hit->weigTrack3 = tmp[2];
   }

   if(hit->track1 < 0 && hit->track2 > 0 &&
      hit->weigTrack2 > 0 &&  hit->weigTrack1 == hit->weigTrack2) {
       // move track1 down in the list (if it is noise)
       // and if track has the same weight as track2 (a "good" one!)
       tmp[0] = hit->track1;
       tmp[1] = hit->flag1;
       tmp[2] = hit->weigTrack1;
       hit->track1     = hit->track2;
       hit->flag1      = hit->flag2;
       hit->weigTrack1 = hit->weigTrack2;
       hit->track2     = tmp[0];
       hit->flag2      = tmp[1];
       hit->weigTrack2 = tmp[2];
   }




}
