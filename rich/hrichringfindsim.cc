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
//  HRichRingFindSim
//
//  Ring recognition algorithms. This class isn't any task but it is called
//  for each event from the HRichAnalysisSim::execute() function.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichanalysispar.h"
#include "hrichanalysissim.h"
#include "hrichhitsim.h"
#include "hrichringfindsim.h"

using namespace std;

ClassImp(HRichRingFindSim)

//----------------------------------------------------------------------------
HRichRingFindSim::HRichRingFindSim()
{
   maxRings = 0;
   pRings   = NULL;
}
//============================================================================

//----------------------------------------------------------------------------
HRichRingFindSim::~HRichRingFindSim()
{
  if (NULL != pRings) {
      delete [] pRings;
      pRings = NULL;
   }
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFindSim::init(HRichAnalysisSim* showMe)
{
   maxRings = 0;
   HRichRingFind::init(showMe);

   if (pRings)
      delete [] pRings;

   if( pAnalysisParams->iSuperiorAlgorithmID < 3) {
      Int_t maxRings1=0;
      Int_t maxRings2=0;
      if (pAnalysisParams->isActiveRingFindFitMatrix) maxRings1 = pAnalysisParams->iHowManyFitMatrixRings;
      if (pAnalysisParams->isActiveRingHoughTransf) maxRings2 = pAnalysisParams->iHowManyHoughTransfRings;
      maxRings = (maxRings1 > maxRings2) ? maxRings1 : maxRings2;
   } else { // sum of rings found by each algorithm is stored
      maxRings = 0;
      if (pAnalysisParams->isActiveRingFindFitMatrix) maxRings += pAnalysisParams->iHowManyFitMatrixRings;
      if (pAnalysisParams->isActiveRingHoughTransf) maxRings += pAnalysisParams->iHowManyHoughTransfRings;
   }

   pRings = new HRichHitSim[maxRings];
   showMe->pRings = pRings;

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFindSim::LookForTrack(HRichAnalysisSim *showMe)
{

// This method stores the track numbers for EACH pad contributing to a ring
// (iRingMask == 1) and it stores maximally 3
// tracks  for each ring. At this stage the initial and final track positions in
// the catRichTrack array are stored.

   Int_t n = 0;
   Int_t j = 0;
   Int_t k = 0;
   Int_t z = 0;

   const Int_t ringHalfSize = iRingImageSize / 2;

   for (n = 0; n < iRingNr; ++n) {
      Int_t trackCount = 0;
      HRichHitSim& ring = pRings[n];

      Int_t iNowX = ring.iRingX;
      Int_t iNowY = ring.iRingY;
      for (Int_t row = iNowY - ringHalfSize; row <= iNowY + ringHalfSize; ++row) {
         for (Int_t col = iNowX - ringHalfSize; col <= iNowX + ringHalfSize; ++col) {
            if (!showMe->IsOut(col, row)) {
               if (showMe->GetPad(col, row)->getAmplitude() > 0 &&
                   1 == pAnalysisParams->iRingMask[col-(iNowX-ringHalfSize) +
                                                   (pAnalysisParams->iRingMaskSize) *
                                                   (row-(iNowY-ringHalfSize))]) {
                  ring.iRingPadNTrack1[trackCount] = showMe->getPadsNTrack1(col, row, showMe->GetActiveSector());
                  ring.iRingPadNTrack2[trackCount] = showMe->getPadsNTrack2();
                  trackCount++;
               }
            }
         }
      }

      ring.iRingPadNTrack1[trackCount] = -1; // signals end of list
      ring.iRingPadNTrack2[trackCount] = -1; // signals end of list
      j = k = z = 0;

      while (ring.iRingPadNTrack1[j] >= 0) {
         ring.iRingTrack[k] = showMe->getTrack(ring.iRingPadNTrack1[j]);
         ring.iRingFlag[k] = showMe->getFlag(ring.iRingPadNTrack1[j]);

         k++;

         if (k >= 254) {
            Warning("LookForTrack", "Boundary reached. Additional tracks will be skipped");
            break;
         }

         z = ring.iRingPadNTrack2[j] - ring.iRingPadNTrack1[j];

         //if z > 0 then it means that there is more then 1 track for the analyzed Ring
         // In this case even the second [third] track is stored.

         for (Int_t tmp = 1; tmp <= z; ++tmp) {
            ring.iRingTrack[k] = showMe->getTrack(ring.iRingPadNTrack1[j] + tmp);
            ring.iRingFlag[k] = showMe->getFlag(ring.iRingPadNTrack1[j] + tmp);
            k++;

            if (k >= 254) {
               Warning("LookForTrack", "Boundary reached. Additional tracks will be skipped");
               break;
            }
         }
         j++;
      }

      ring.iRingTrack[k] = 0; // signals end of list
   }

}
//============================================================================

//----------------------------------------------------------------------------
Int_t HRichRingFindSim::Execute(HRichAnalysisSim *giveMe)
{

   iRingNr = HRichRingFind::Execute(giveMe);

   if (iRingNr >= maxRings) {
      Warning("Execute",
              "Number of found rings (%d) exceeds the array size, only firs %d rings are taken",
              iRingNr, maxRings);
      iRingNr = maxRings;
   }

   for (Int_t i = 0; i < iRingNr; ++i) {
      pRings[i] = HRichRingFind::pRings[i];
   }

   if (iRingNr > 0)
      LookForTrack(giveMe);

   return (iRingNr);
}

















