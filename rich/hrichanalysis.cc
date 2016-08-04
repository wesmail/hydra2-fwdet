//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichAnalysis
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hlinearcategory.h"
#include "hparset.h"
#include "hrichanalysis.h"
#include "hrichanalysispar.h"
#include "hrichcal.h"
#include "hrichdetector.h"
#include "hrichgeometrypar.h"
#include "hrichhitheader.h"
#include "hrichpadclean.h"
#include "hrichpadlabel.h"
#include "hrichringfind.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichAnalysis)

//----------------------------------------------------------------------------
HRichAnalysis::HRichAnalysis() : HReconstructor()
{

   clear();

   pPadClean = new HRichPadClean;
   pPadLabel = new HRichPadLabel;
   pRingFind = new HRichRingFind;

}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysis::HRichAnalysis(const Text_t* name, const Text_t* title, Bool_t kSkip)
   : HReconstructor(name, title)
{
   clear();

   pPadClean = new HRichPadClean;
   pPadLabel = new HRichPadLabel;
   pRingFind = new HRichRingFind;
   kSkipEvtIfNoRing = kSkip;
}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysis::~HRichAnalysis()
{
   if (NULL != pPadClean) {
      delete pPadClean;
      pPadClean = NULL;
   }
   if (NULL != pPadLabel) {
      delete pPadLabel;
      pPadLabel = NULL;
   }
   if (NULL != pRingFind) {
      delete pRingFind;
      pRingFind = NULL;
   }
   if (NULL != pLabelArea) {
      delete [] pLabelArea;
      pLabelArea = NULL;
   }
   if (NULL != pPads) {
      for (Int_t i = 0; i < 6; i++)
         if (pPads[i]) delete [] pPads[i];
      delete [] pPads;
      pPads = NULL;
   }
   if (NULL != pLeftBorder) {
      delete [] pLeftBorder;
      pLeftBorder = NULL;
   }
   if (NULL != pRightBorder) {
      delete [] pRightBorder;
      pRightBorder = NULL;
   }
   if (NULL != fIter) {
      delete fIter;
      fIter = NULL;
   }
   if (NULL != fIterHitHeader) {
      delete fIterHitHeader;
      fIterHitHeader = NULL;
   }
}
//============================================================================

//----------------------------------------------------------------------------
void HRichAnalysis::clear()
{

   pLabelArea     = NULL;
   pRings         = NULL;
   pPads          = NULL;
   pSectorPads    = NULL;
   fIter          = NULL;
   fIterHitHeader = NULL;
   pLeftBorder    = NULL;
   pRightBorder   = NULL;
   m_pCalCat      = NULL;
   m_pHitCat      = NULL;
   m_pHitHdrCat   = NULL;
   fpAnalysisPar  = NULL;
   fpGeomPar      = NULL;
   hithdrLoop     = NULL;

   for (Int_t i = 0; i < 6; ++i)
      fPadFired[i] = 0;

   iClustersCleaned.Reset();

   allPairNrTot      = 0;
   iActiveSector     = 0;
   iClusterCleanedNr = 0;
   iFakeLocalMax4    = 0;
   iFakeLocalMax8    = 0;
   iFakePad          = 0;
   iLabelNr          = 0;
   iPadCleanedNr     = 0;
   iPadFiredNr       = 0;
   iRingNr           = 0;
   iRingNrTot        = 0;
   sectorPairNrTot   = 0;

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysis::Reset()
{
   iClustersCleaned.Reset();

   iClusterCleanedNr = 0;
   iFakeLocalMax4    = 0;
   iFakeLocalMax8    = 0;
   iFakePad          = 0;
   iLabelNr          = 0;
   iPadCleanedNr     = 0;
   iPadFiredNr       = 0;
   iRingNr           = 0;

   if (pLabelArea) {
      delete [] pLabelArea;
      pLabelArea = NULL;
   }
}
//============================================================================
/*
//----------------------------------------------------------------------------
HRichAnalysis::HRichAnalysis(const HRichAnalysis& source)
{

   Error("HRichAnalysis", "HRichAnalysis object can not be initialized with values of another object!\nUse instead default constructor");

}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysis&
HRichAnalysis::operator=(const HRichAnalysis& source)
{
   if (this != &source) {
      Error("operator=", "HRichAnalysis object can not be assigned!");
   }
   return *this;
}
//============================================================================
*/
//----------------------------------------------------------------------------
Bool_t
HRichAnalysis::init()
{
// Allocate input/output categories, initialize iterators and parameter containers

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
      m_pCalCat = pRichDet->buildCategory(catRichCal);
      if (NULL == m_pCalCat) {
         Error("init", "Pointer to HRichCal category is NULL");
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
      m_pHitCat = pRichDet->buildCategory(catRichHit);
      if (NULL == m_pHitCat) {
         Error("init", "Pointer to HRichHit category is NULL");
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
Bool_t
HRichAnalysis::reinit()
{
   UInt_t i, j, k, m, n;
   Int_t iMatrixSurface = 0;
   Int_t iPartSurface   = 0;
   Int_t iMaskSize      = 0;
   Int_t iMaskSizeSquared = 0;

   if (kFALSE == initParameters()) {
      Error("reinit", "Parameters not initialized");
      return kFALSE;
   }

   pRingFind->init(this);


   iPadActive.Set(maxCols * maxRows);
   for (i = 0; i < (UInt_t)maxCols * maxRows; ++i)
      if (((HRichGeometryPar*)fpGeomPar)->getPadsPar()->getPad(i)->getPadActive() > 0)
         iPadActive[i] = 1;
      else
         iPadActive[i] = 0;


   iMaskSize        = ((HRichAnalysisPar*)fpAnalysisPar)->iRingMaskSize;
   iMaskSizeSquared = iMaskSize * iMaskSize;
   for (k = 0; k < (UInt_t)iMaskSizeSquared; ++k)
      if (1 == ((HRichAnalysisPar*)fpAnalysisPar)->iRingMask[k])
         iMatrixSurface++;


   for (j = 0; j < (UInt_t)maxRows; j++)
      for (i = 0; i < (UInt_t)maxCols; i++) {
         iPartSurface = 0;
         for (k = 0; k < (UInt_t)iMaskSizeSquared; k++) {
            m = (k % iMaskSize) - iMaskSize / 2;
            n = (k / iMaskSize) - iMaskSize / 2;
            if (!IsOut(i, j, m, n))
               if (1 == ((HRichAnalysisPar*)fpAnalysisPar)->iRingMask[k])
                  iPartSurface++;
         }

         ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
         getPad(i, j)->setAmplitFraction((Float_t)iPartSurface / (Float_t)iMatrixSurface);
      }

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysis::initParameters()
{
// allocate non event by event classes

   HRichGeometryPar *pGeomPar = getGeometryPar();

   iRingNrTot = 0;
   allPairNrTot = 0;
   sectorPairNrTot = 0;
   maxFiredTotalPads  = 3000; // upper limit of number of fired pads

   maxCols = pGeomPar->getColumns();
   maxRows = pGeomPar->getRows();

   if (pLeftBorder)
      delete [] pLeftBorder;
   pLeftBorder = new Short_t[maxRows];

   if (pRightBorder)
      delete [] pRightBorder;
   pRightBorder = new Short_t[maxRows];

   for (Int_t row = 0; row < maxRows; ++row) {
      Int_t col = 0;
      Int_t padOffset = row * maxCols;
      while (!pGeomPar->getPadsPar()->getPad(col + padOffset)->getPadActive() && col < maxCols)
         ++col;
      if (col == maxCols) {
         maxRows = row;
         break;
      }
      pLeftBorder[row] = col;
      while (pGeomPar->getPadsPar()->getPad(col + padOffset)->getPadActive() && col < maxCols)
         ++col;
      pRightBorder[row] = col - 1;
   }
   maxPads = maxRows * maxCols;

   // now creating pads array
   pPads = new HRichPadSignal * [6];
   for (Int_t j = 0; j < 6; ++j) {
      pSectorPads = pPads[j] = new HRichPadSignal[maxPads];
      for (Int_t i = 0; i < maxPads; pSectorPads[i++].clear());
   }
   pSectorPads = pPads[iActiveSector];

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysis::finalize()
{
   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysis::execute()
{

   HRichCal* pCal      = NULL;
   Bool_t skipSector[] = {0, 0, 0, 0, 0, 0};
   Int_t iRingNrTotEvt = 0;
   Int_t fPadFiredTot  = 0;
   Int_t lastRingNr    = 0;
   Int_t ampl          = -1;
   Int_t sector        = -1;
   Int_t padnr         = -1;
   const Int_t eventNr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
   Int_t j;

   secWithCurrent = -1;
   Reset();

   for (sector = 0; sector < 6; ++sector) {
      if (fPadFired[sector] > 0) {
         fPadFired[sector] = 0;
         HRichPadSignal* pSecPads = pPads[sector];
         for (j = 0; j < maxPads; pSecPads[j++].clear());
      }
   }

   fIter->Reset();
   while (NULL != (pCal = (HRichCal *)fIter->Next())) {
      if ((ampl = (Int_t)pCal->getCharge()) > 0) {
         sector = pCal->getSector();
         fPadFired[sector]++;
         padnr = pCal->getCol() + pCal->getRow() * maxCols;
	 if(padnr >= maxPads) {
	     Warning("execute()","pPads out of range ! signal will be skipped.");
	     continue;
	 }
	 pPads[sector][padnr].setAmplitude(ampl);
      }
      // remember that columns are x and rows are y
   }

   for (sector = 6; sector--; fPadFiredTot += fPadFired[sector]) {
      if (fPadFiredTot > maxFiredTotalPads) {
         Warning("execute", "Analysis of event %d skipped: too many pads fired %i/%i",
                 eventNr, fPadFiredTot, maxFiredTotalPads);
         fIter->Reset();
         while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
            pCal->setIsCleanedSector(kTRUE);
         }
         return 0;
      }
      if (fPadFired[sector] >= fpAnalysisPar->maxFiredSectorPads) {
         Warning("execute", "Event %d: too many fired pads in sector %i. %i/%i",
                 eventNr, sector, fPadFired[sector], fpAnalysisPar->maxFiredSectorPads);
         fIter->Reset();
         while (NULL != (pCal = static_cast<HRichCal*>(fIter->Next()))) {
            if (pCal->getSector() == sector)
               pCal->setIsCleanedSector(kTRUE);
         }
         skipSector[sector] = kTRUE;
      }
   }


   // **************************************************************

   // ------- loop over sectors --- begin ---

   lastRingNr = iRingNrTot;
   for (sector = 0; sector < 6; ++sector) {
      // Skip analysis for sector with too many fired pads
      if (kTRUE == skipSector[sector])
         continue;

      if (((HRichGeometryPar*)fpGeomPar)->getSectorActive(sector) > 0) {
         Reset();
         SetActiveSector(sector);
         iPadFiredNr = fPadFired[sector];

         // for each sector:
         //   - cleaning
         //   - labeling
         //   - ring finding
         // procedure are executed

         iPadCleanedNr = pPadClean->Execute(this);
         iLabelNr      = pPadLabel->Execute(this);

         if (0 == ((HRichAnalysisPar*)fpAnalysisPar)->isActiveLabelPads) {
            iLabelNr = 1;
            pLabelArea = new HRichLabel[1];
            pLabelArea[0].iLeftX = 0;
            pLabelArea[0].iRightX = maxCols;
            pLabelArea[0].iLowerY = 0;
            pLabelArea[0].iUpperY = maxRows;
            pLabelArea[0].iLabeledPadsNr = maxPads;
            pLabelArea[0].iFiredPadsNr = iPadFiredNr;
         }

         iRingNr = pRingFind->Execute(this);
         iRingNrTot    += iRingNr;
         iRingNrTotEvt += iRingNr;

         if (iRingNr > 1) {
            ++sectorPairNrTot;
         }

         // Found rings are stored in the hit container
         updateHeaders(sector, eventNr);
         updateHits(sector);
      }
   }

   // update information about cleaned pads
   fIter->Reset();
   while (NULL != (pCal = (HRichCal *)fIter->Next())) {
      sector = pCal->getSector();
      padnr  = pCal->getCol() + pCal->getRow() * maxCols;
      if(padnr >= maxPads) {  // warning already above
	  continue;
      }
      pCal->setIsCleanedSingle(pPads[sector][padnr].getIsCleanedSingle());
      pCal->setIsCleanedHigh(pPads[sector][padnr].getIsCleanedHigh());
   }

   // ------- loop over sectors --- end ---
   if (iRingNrTot > lastRingNr + 1) {
      ++allPairNrTot;
   }

   // modification to skip event which does not contain any ring
   if (kSkipEvtIfNoRing && iRingNrTotEvt == 0)
      return kSkipEvent;
   return 0;
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysis::updateHits(Int_t nSec)
{

   HRichHit* hit = NULL;
   HLocation loc;

   for (Int_t i = 0; i < iRingNr; i++) {
      loc.set(1, nSec);

      hit = (HRichHit*)m_pHitCat->getNewSlot(loc);

      if (NULL != hit) {
         hit = new(hit) HRichHit;

         *hit = pRings[i];
         hit->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
         hit->setSector(nSec);
         if (hit->fX > 1000.)
            Info("updateHits", "fX : %f, %i fY : %f, %i",
                 hit->fX, hit->iRingX, hit->fY, hit->iRingY);

//calculate ring center in theta, phi by averaging over next neighbours
         HRichPad* pad = ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
	     getPad((UInt_t)hit->iRingX, (UInt_t)hit->iRingY);
         hit->fPhi   = pad->getPhi(nSec);
         hit->fTheta = pad->getTheta();

	 Float_t delta = hit->fPadX - float(hit->iRingX);
	 if(delta > 0.01F) {
           HRichPad* pad1 = ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
	       getPad((UInt_t)hit->iRingX+1, (UInt_t)hit->iRingY);
	   hit->fPhi += (pad1->getPhi(nSec) - hit->fPhi)*delta;
	 } else if(delta < -0.01F){
           HRichPad* pad1 = ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
	       getPad((UInt_t)hit->iRingX-1, (UInt_t)hit->iRingY);
	   hit->fPhi -= (pad1->getPhi(nSec) - hit->fPhi)*delta;
	 }
         delta = hit->fPadY - float(hit->iRingY);
	 if(delta > 0.01F) {
           HRichPad* pad1 = ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
	       getPad((UInt_t)hit->iRingX, (UInt_t)hit->iRingY+1);
	   hit->fTheta += (pad1->getTheta() - hit->fTheta)*delta;
	 } else if(delta < -0.01F){
           HRichPad* pad1 = ((HRichGeometryPar*)fpGeomPar)->getPadsPar()->
	       getPad((UInt_t)hit->iRingX, (UInt_t)hit->iRingY-1);
	   hit->fTheta -= (pad1->getTheta() - hit->fTheta)*delta;
	 }
      }
   }
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysis::updateHeaders(const Int_t nSec,
                             const Int_t nEvNr)
{
   HRichHitHeader* hithdr = NULL;
   HLocation       loc;

   fIterHitHeader->Reset();
   hithdrLoop = NULL;

   while (NULL != (hithdrLoop = (HRichHitHeader *)fIterHitHeader->Next())) {
      secWithCurrent = hithdrLoop->getSector();
      if (nSec == secWithCurrent) {
         hithdr = hithdrLoop;
         break;
      }
   }

   if (hithdr == NULL) {
      loc.set(1, nSec);
      hithdr = (HRichHitHeader*)(m_pHitHdrCat)->getNewSlot(loc);
      if (NULL != hithdr)
         hithdr = new(hithdr) HRichHitHeader;
   }

   if (NULL != hithdr) {
      hithdr->setSector(nSec);
      hithdr->setEventNr(nEvNr);
      hithdr->setExecutedAnalysis(1);
      hithdr->iPadFiredNr = iPadFiredNr;
      hithdr->iPadCleanedNr = iPadCleanedNr;
      hithdr->iClusterCleanedNr = iClusterCleanedNr;
      hithdr->iClustersCleaned = iClustersCleaned;
      hithdr->iRingNr = iRingNr;
      hithdr->iFakePad = iFakePad;
      hithdr->iFakeLocalMax4 = iFakeLocalMax4;
      hithdr->iFakeLocalMax8 = iFakeLocalMax8;
   } else {
      Warning("updateHeaders", "No Header Object could be created");
   }
}

//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysis::SetActiveSector(Int_t sectornr)
{
   if (sectornr == iActiveSector)
      return;
   if (sectornr > 5 || sectornr < 0) {
      Error("SetActiveSector", "Sector number %d out of range (0..5)!", sectornr);
      return;
   } else if (0 == ((HRichGeometryPar*)fpGeomPar)->getSectorActive(sectornr)) {
      Error("SetActiveSector", "Sector number %d is not present (and cannot be active)!", sectornr);
      return;
   } else {
      iActiveSector = sectornr;
      pSectorPads = pPads[sectornr];
   }
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichAnalysis::SetNextSector()
{
   Int_t i = iActiveSector;
   while (i < 6) {
      i++;
      if (((HRichGeometryPar*)fpGeomPar)->getSectorActive(i) > 0) {
         pSectorPads = pPads[i];
         return (iActiveSector = i);
      }
   }
   Error("SetNextSector", "No more sectors (last sector %d)", iActiveSector);
   return iActiveSector;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysis::IsOut(Int_t nowPad, Int_t dx, Int_t dy)
{
   if (nowPad <= 0)
      return 1;
   Int_t x = nowPad % maxCols;
   Int_t y = nowPad / maxCols;

   if ((x + dx) >= 0 && (x + dx) < maxCols &&
       (y + dy) >= 0 && (y + dy) < maxRows &&
       iPadActive[nowPad+dx + maxCols*dy]) {
      return 0;
   } else {
      return 1;
   }
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichAnalysis::IsOut(Int_t x, Int_t y, Int_t dx, Int_t dy)
{
   if ((x + dx) >= 0 && (x + dx) < maxCols &&
       (y + dy) >= 0 && (y + dy) < maxRows &&
       iPadActive[x + dx + maxCols *(y + dy)]) {
      return 0;
   } else {
      return 1;
   }
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichAnalysis::Streamer(TBuffer &R__b)
{
// Stream an object of class HRichAnalysis.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      if (R__v) { }
      R__b >> iPadFiredNr;
      R__b >> iPadCleanedNr;
      R__b >> iRingNr;
      R__b >> pRings;
      R__b >> iClusterCleanedNr;
      iClustersCleaned.Streamer(R__b);
      R__b >> iFakePad;
      R__b >> iFakeLocalMax4;
      R__b >> iFakeLocalMax8;
   } else {
      R__b.WriteVersion(HRichAnalysis::IsA());
      R__b << iPadFiredNr;
      R__b << iPadCleanedNr;
      R__b << iRingNr;
      R__b << pRings;
      R__b << iClusterCleanedNr;
      iClustersCleaned.Streamer(R__b);
      R__b << iFakePad;
      R__b << iFakeLocalMax4;
      R__b << iFakeLocalMax8;
   }
}
//============================================================================
