//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//*-- Bugfixes: Wolfgang Koenig <w.koenig@gsi.de> 2013
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichRingFind
//
//  Ring recognition algorithms. This class isn't any task but it is called
//  for each event from the HRichAnalysis::execute() function.
//
//////////////////////////////////////////////////////////////////////////////


#include "TList.h"
#include "TRandom.h"

#include "hrichanalysis.h"
#include "hrichanalysispar.h"
#include "hrichgeometrypar.h"
#include "hrichhit.h"
#include "hrichhitcandidate.h"
#include "hrichringfind.h"
#include "hruntimedb.h"

#include <iomanip>
#include <iostream>
#include <math.h>
#include <stdlib.h>

using namespace std;

ClassImp(HRichRingFind)

//----------------------------------------------------------------------------
HRichRingFind::HRichRingFind()
{
   fClusterLMax4      = 0.;
   fClusterLMax8      = 0.;
   fClusterSize       = 0.;
   fMaxClusterSize    = 0;
   fMaxClusterSum     = 0;
   fMaxThrClusterSize = 0;
   iInnerCount        = 0;
   iInnerPhot4        = 0;
   iInnerPhot8        = 0;
   iMatrixHalfSize    = 0;
   iMatrixSize        = 0;
   iRingImageSize     = 0;
   iRingNr            = 0;
   lx_from            = 0;
   lx_to              = 0;
   ly_from            = 0;
   ly_to              = 0;
   maxCols            = 0;
   maxRows            = 0;
   maxRings           = 0;

   iPadCol.Set(1000);
   iPadPlane.Set(16384);
   iPadPlaneCopy.Set(16384);
   iPadRow.Set(1000);
   iRingTempImage.Set(256);

   pRings          = NULL;
   pAnalysisParams = NULL;
   pGeometryParams = NULL;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::init(HRichAnalysis* showMe)
{

   pAnalysisParams = showMe->getAnalysisPar();
   pGeometryParams = showMe->getGeometryPar();

   iInnerCount   = 0;
   iInnerPhot4   = 0;
   iInnerPhot8   = 0;
   fClusterSize  = 0.;
   fClusterLMax4 = 0.;
   fClusterLMax8 = 0.;
   iRingNr       = 0;
   iRingImageSize = pAnalysisParams->iRingMaskSize;
   iRingTempImage.Set(iRingImageSize * iRingImageSize);
   iPadPlane.Set(pGeometryParams->getPadsNr());
   iPadPlaneCopy.Set(pGeometryParams->getPadsNr());
   iPadCol.Set(pAnalysisParams->maxFiredSectorPads);
   iPadRow.Set(pAnalysisParams->maxFiredSectorPads);
   iPadActive.Set(pGeometryParams->getPadsNr());
   for (Int_t i = 0; i < pGeometryParams->getPadsNr(); ++i) {
      if (pGeometryParams->getPadsPar()->getPad(i)->getPadActive() > 0)
         iPadActive[i] = 1;
      else iPadActive[i] = 0;
   }
   maxCols = showMe->GetPadsXNr();
   maxRows = showMe->GetPadsYNr();

   iMatrixSize = pAnalysisParams->iRingMatrixSize;
   iMatrixHalfSize = iMatrixSize / 2;
   fMaxClusterSize = 0;
   fMaxClusterSum  = 0;
   fMaxThrClusterSize = 0;


   fHitList1.Delete();
   fHitList2.Delete();
   fHitCandidate.Delete();

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

   pRings = new HRichHit[maxRings];
   showMe->pRings = pRings;

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
HRichRingFind::~HRichRingFind()
{
   if (NULL != pRings) {
      delete [] pRings;
      pRings = NULL;
   }
}
//============================================================================
/*
//----------------------------------------------------------------------------
HRichRingFind::HRichRingFind(const HRichRingFind& source)
{
   cerr << "HRichRingFind object can not be initialized with values of another object!\n";
   cerr << "Default constructor will be called.\n";
   HRichRingFind();
}
//============================================================================

//----------------------------------------------------------------------------
HRichRingFind&
HRichRingFind::operator=(const HRichRingFind& source)
{
   if (this != &source) {
      cerr << "HRichRingFind object can not be assigned!\n";
      cerr << "Default constructor will be called.\n";
      HRichRingFind();
   }
   return *this;
}
//============================================================================
*/
//----------------------------------------------------------------------------
Float_t
HRichRingFind::CalcDistance(Int_t x, Int_t y, const HRichHit& ring)
{
   return sqrt(static_cast<Float_t>((x - ring.iRingX) * (x - ring.iRingX) +
                                    (y - ring.iRingY) * (y - ring.iRingY)));
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichRingFind::CalcDistance(Int_t x1, Int_t y1,
                            Int_t x2, Int_t y2)
{
   return sqrt(static_cast<Float_t>((x1 - x2) * (x1 - x2) +
                                    (y1 - y2) * (y1 - y2)));
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichRingFind::CalcDistance(const HRichHit& ring1, const HRichHit& ring2)
{
   return sqrt(static_cast<Float_t>((ring1.iRingX - ring2.iRingX) *
                                    (ring1.iRingX - ring2.iRingX) +
                                    (ring1.iRingY - ring2.iRingY) *
                                    (ring1.iRingY - ring2.iRingY)));
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichRingFind::CalcDistanceMean(const HRichHit& ring1, const HRichHit& ring2) {
   Float_t dx = ring1.fPadX - ring2.fPadX;
   Float_t dy = ring1.fPadY - ring2.fPadY;
   return sqrt(dx*dx + dy*dy);
}
//============================================================================

//----------------------------------------------------------------------------
Double_t
HRichRingFind::HomogenDistr(Double_t left, Double_t right)
{
   return gRandom->Rndm() * (right - left) + left;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichRingFind::GetAlgorithmNr(HRichAnalysis *showMe)
{
   return (pAnalysisParams->isActiveRingFindFitMatrix +
           pAnalysisParams->isActiveRingHoughTransf);
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichRingFind::Execute(HRichAnalysis *giveMe)
{
// According to the settings in the HRichAnalysisPar parameter
// container, ring finding algorithms are executed.
// - Ring candidates found by the pattern matrix
//   are stored in the list fHitList1
// - Ring candidates found by the hough trasfomation
//   are stored in the list fHitList2
// At the end the two list of candidates are merged according
// to the analysis parameter (see comment in MatchRings)


   if (0 == giveMe->GetLabelNr() || 0 == GetAlgorithmNr(giveMe)) {
      iRingNr = 0;
      CalcFakeContribution(giveMe);
      return (giveMe->iRingNr = iRingNr);
   }

   iRingNr = 0;

   if (1 == pAnalysisParams->isActiveRingFindFitMatrix) {
      RingFindFitMatrix(giveMe, pAnalysisParams->iMinimalFitMatrixRingQuality,
                        pAnalysisParams->iMinimalFitMatrixRingDistance,
                        pAnalysisParams->iHowManyFitMatrixRings);
   }

   if (1 == pAnalysisParams->isActiveRingHoughTransf) {
      RingFindHoughTransf(giveMe, pAnalysisParams->iMinimalHoughTransfRingQuality,
                          pAnalysisParams->iMinimalHoughTransfRingDistance,
                          pAnalysisParams->iHowManyHoughTransfRings);
   }

   CloseMaxRejection(&fHitList1);
   CloseMaxRejection(&fHitList2);

   iRingNr = MatchRings(giveMe, &fHitList1, &fHitList2);
// remove close partner of a pair, if both rings are identical but having different centres
   iRingNr = CleanIdenticalPairs(giveMe);
   return iRingNr;

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::RingFindFitMatrix(HRichAnalysis* showMe,
                                 Int_t          minampl,
                                 Int_t          distance,
                                 Int_t          howmanyrings)
{
// Ring recognition algorithm: Pattern Matrix

   Int_t i, j, m;
   Int_t lx, ly;
   Int_t pad;
   Int_t iRingQuality;
   const Int_t iLabelNr = showMe->GetLabelNr();

   HRichLabel* pLabel = NULL;
   HRichHit*   pHit   = NULL;

   if (howmanyrings < 1) {
      Error("RingFindFitMatrix",
            "Pattern matrix algorithm active, but iHowManyFitMatrixRings == %d! Set to 1!",
            howmanyrings);
      howmanyrings = 1;
   }

   iHitCount = 0;
   iPadPlane.Reset();
   iPadPlaneCopy.Reset();
   fHitList1.Delete();

   // loop over all the labels ( the labeling procedure is executed
   // in the hrichanalysis execute fn.)

   for (m = 0; m < iLabelNr; ++m) {
      pLabel = showMe->GetLabel(m);
      //loop over all pads belonging to a given label
      for (j = pLabel->iLowerY; j <= pLabel->iUpperY; ++j) {
         ly_from = ((j - iMatrixHalfSize < 0) ? 0 : j - iMatrixHalfSize);
         ly_to   = ((j + iMatrixHalfSize >= maxRows) ? maxRows - 1 : j + iMatrixHalfSize);

         for (i = pLabel->iLeftX; i <= pLabel->iRightX; ++i) {
            lx_from = ((i - iMatrixHalfSize < 0) ? 0 : i - iMatrixHalfSize);
            lx_to   = ((i + iMatrixHalfSize >= maxCols) ? maxCols - 1 : i + iMatrixHalfSize);

            iRingQuality = 0;

            // the center of the pattern matrix is overlapped to each pad.
            // If the pads in the pattern matrix range belong to the label
            // the corresponding patter matrix quality is summed up
            // to the total quality of this ring candidate.

            for (ly = ly_from; ly <= ly_to; ++ly) {
               for (lx = lx_from; lx <= lx_to; ++lx) {
                  pad = lx + maxCols * ly;
                  if (iPadActive[pad]) {
                     if (showMe->GetPad(pad)->getAmplitude() > 0 &&
                         showMe->GetPad(pad)->getLabel() == pLabel->iSignature) {
                        iRingQuality += pAnalysisParams->
                                        iRingMatrix[lx - i + iMatrixHalfSize + iMatrixSize *
                                                    (ly - j + iMatrixHalfSize)];
                     }
                  }
               }  // end of loop over all pattern matrix pads
            }
            if (iRingQuality > 0) {
               iPadPlane[i + maxCols*j] += iRingQuality;
            }
         }
      }  // end of loops over all pads of given label
   } // end of loops over all labels


   MaxFinding(showMe,  &fHitList1, &iPadPlane, &iPadPlaneCopy, howmanyrings, distance);
   MaxSelector(showMe, &fHitList1, &iPadPlane, &iPadPlaneCopy);
   MaxAnalysis(showMe, &fHitList1, &iPadPlane, &iPadPlaneCopy, minampl);


   for (m = 0; m < fHitList1.GetSize(); ++m) {
      pHit = static_cast<HRichHit*>(fHitList1.At(m));
      CalcRingParameters(showMe, pHit);
      pHit->fTests = TestRing(showMe, pHit, minampl);
   }
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::RingFindHoughTransf(HRichAnalysis* showMe,
                                   Int_t          minampl,
                                   Int_t          distance,
                                   Int_t          howmanyrings)
{
// Ring recognition algorithm: Hough Transform

   Int_t i, j, k, m;
   Int_t nrFired;
   Float_t fDistance;
   Float_t fRingX, fRingY, fRingR;
   Int_t iRingX, iRingY;
   Float_t fDiv;

   HRichHit*       pHit   = NULL;
   HRichLabel*     pLabel = NULL;
   HRichPadSignal* pPad   = NULL;

   if (howmanyrings < 1) {
      Error("RingFindHoughTransf",
            "Pattern matrix algorithm active, but iHowManyHoughTransfRings == %d! Set to 1!",
            howmanyrings);
      howmanyrings = 1;
   }

   iHitCount = 0;
   iPadPlane.Reset();
   iPadPlaneCopy.Reset();
   fHitList2.Delete();

   //loop over alla the labels

   for (m = 0; m < showMe->GetLabelNr(); ++m) {
      pLabel  = showMe->GetLabel(m);
      nrFired = pLabel->iFiredPadsNr;
      k = 0;

      //loop over all the label pads
      for (j = pLabel->iLowerY; j <= pLabel->iUpperY; ++j) {
         for (i = pLabel->iLeftX; i <= pLabel->iRightX; ++i) {
            pPad = showMe->GetPad(i, j);
            if (pPad->getAmplitude() > 0  &&
                pPad->getLabel() == pLabel->iSignature) {
               // the position of the fired pads in the label are stored
               // in the arrays iPadCol[i] iPadRow[i]
               iPadCol[k] = i;
               iPadRow[k] = j;
               k++;
               if (k >  nrFired) exit(1);
            }
	 }
      }

      // 3 loops to obtain all the combinations of three pads.
      // two pads in the triplet must have a distance bigger than
      // half a ring radius and lower than the matrix dimension (diameter)

      for (i = 0; i < nrFired - 2; i++) {
         for (j = i + 1; j < nrFired - 1; j++) {

            d_col_ij = iPadCol[i] - iPadCol[j];
            d_row_ij = iPadRow[i] - iPadRow[j];
            fDistance = sqrt((Float_t)(d_col_ij * d_col_ij + d_row_ij * d_row_ij));

            if (fDistance > pAnalysisParams->iRingRadius / 2 &&
		fDistance < pAnalysisParams->iRingMatrixSize) {

               for (k = j + 1; k < nrFired; k++) {

                  d_col_jk = iPadCol[j] - iPadCol[k];
                  d_row_jk = iPadRow[j] - iPadRow[k];
                  fDistance = sqrt((Float_t)(d_col_jk * d_col_jk + d_row_jk * d_row_jk));

                  if (fDistance > pAnalysisParams->iRingRadius / 2 &&
                      fDistance < pAnalysisParams->iRingMatrixSize) {

                     // ................. create  map .........
                     // for each combination of three pads the center of the ring
                     // that goes trough the pads is calculated

                     fDiv = d_col_jk * d_row_ij - d_col_ij * d_row_jk;

                     if (TMath::Abs(fDiv) >= 2.0) {

                        d2_colrow_jk = iPadCol[j] * iPadCol[j] - iPadCol[k] * iPadCol[k] +
                                       iPadRow[j] * iPadRow[j] - iPadRow[k] * iPadRow[k];
                        d2_colrow_ij = iPadCol[i] * iPadCol[i] - iPadCol[j] * iPadCol[j] +
                                       iPadRow[i] * iPadRow[i] - iPadRow[j] * iPadRow[j];

                        fRingX = 0.5 * ((Float_t)(d2_colrow_jk * d_row_ij -
                                                  d2_colrow_ij * d_row_jk)) / fDiv;

                        fRingY = 0.5 * ((Float_t)(d2_colrow_ij * d_col_jk -
                                                  d2_colrow_jk * d_col_ij)) / fDiv;

                        fRingR = sqrt((iPadCol[i] - fRingX) * (iPadCol[i] - fRingX) +
                                      (iPadRow[i] - fRingY) * (iPadRow[i] - fRingY));

			iRingX = Int_t(fRingX + 0.5F); // BugFix W.Koenig : rounding of x,y was missing (01.03.2013)
                        iRingY = Int_t(fRingY + 0.5F);

                        if (fRingR < (0.5 + pAnalysisParams->iRingRadius + pAnalysisParams->iRingRadiusError) &&
                            fRingR > (0.5 + pAnalysisParams->iRingRadius - pAnalysisParams->iRingRadiusError) &&
                            pLabel->iLeftX <= iRingX &&
                            pLabel->iRightX >= iRingX &&
                            pLabel->iLowerY <= iRingY &&
                            pLabel->iUpperY >= iRingY)
                           iPadPlane[iRingX + maxCols * iRingY] += 1;

                        // this array contains all the centers of the rings each with its
                        // weight ( how many time this pad did correspond to a ring center
                        // for a given three  pads combination.
                     }  // eof create map ...............
                  } // end distance check between 2nd and 3rd selected pad
	       } // end 3rd loop fired pads
	    } // end distance check between first 2 selected pads
	 } // end 2nd loop fired pads
      } // end 1st loop fired pads
   } // end of loop over all labels

   // ..................................... find MAX ..............
   //



   MaxFinding(showMe, &fHitList2, &iPadPlane, &iPadPlaneCopy, howmanyrings, distance);
   MaxSelector(showMe, &fHitList2, &iPadPlane, &iPadPlaneCopy);
   MaxAnalysis(showMe, &fHitList2, &iPadPlane, &iPadPlaneCopy, minampl);


   for (m = 0; m < fHitList2.GetSize(); m++) {
      pHit = (HRichHit*)(fHitList2.At(m));
      CalcRingParameters(showMe, pHit);
      pHit->fTests = TestRing(showMe, pHit, minampl);
   }


} // eof RingFindHoughTransf
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::MaxFinding(HRichAnalysis* showYou,
                          TList*         hitList,
                          TArrayI*       in,
                          TArrayI*       out,
                          Int_t          maxRings,
                          Float_t        distance)
{
// This function looks for local maxima among 8 neighbouring pads in
// the "in"  array (iPadPlabe) "and fills  the output "out" array.
// The iPadPlane contains the pads that correpond to the ring center
// each with its specific weight. Among them the Local Maxima is sought.
// The "out" array is created here in such a way that the amplitude
// of a local maximum is copied from input "in" array (iPdaPlane),
// the pads that are not local maxima are marked with '-1'.
// The pads that havent been fired are marked with '0'.
// The parameters of local maxima are first put to the list fHitCandidate
// of HRichHitCandidate objects. The list is sorted by descending
// ring quality and only the rings that are at a minimal distance
// of "distance" from each other are stored in a "hitList".

   Int_t i, j, k, l, pad, padnear, offset1, offset2;
   Int_t iHitCount = 0;
   Bool_t fMax = kTRUE;
   Int_t iHit = 0;
   HRichLabel *pLabel = NULL;


   fHitCandidate.Delete();
   // loop over all labels
   for (Int_t label = 0; label < showYou->GetLabelNr(); ++label) {
      pLabel = showYou->GetLabel(label);
      // loop over all pads in a given label.
      for (j = pLabel->iLowerY; j <= pLabel->iUpperY; j++) {
         ly_from = ((j - 1 < 0) ? 0 : j - 1);
         ly_to   = ((j + 1 >= maxRows) ? maxRows - 1 : j + 1);

         for (i = pLabel->iLeftX; i <= pLabel->iRightX; i++) {
            lx_from = ((i - 1 < 0) ? 0 : i - 1);
            lx_to   = ((i + 1 >= maxCols) ? maxCols - 1 : i + 1);

            pad = i + maxCols * j;
	    if ((*in)[pad] == 0) {
	       (*out)[pad] = 0;
	       continue;
	    }

	    fMax = kTRUE;

	    // for each pad the weight in the "in" array is compared
	    // with the weights on the 8 neighbouring and local maxima
	    // are defined. centroid of local maximum with 4 neighbores is calculated
	    Int_t nEqualNeighbors = 0; //reset counter for neighbored pads with equal quality
	    Int_t norm = (*in)[pad];
	    Int_t xSum = 0, ySum=0, nearMax=0, nextMaxCount=0;;

	    for (k = ly_from; k <= ly_to; k++) {
	       for (l = lx_from; l <= lx_to; l++) {
		  padnear = l + maxCols * k;
		  if (iPadActive[padnear] && !(l == i && k == j)) {
                     Int_t padHeight=(*in)[padnear];
		     if (padHeight > (*in)[pad]) fMax = kFALSE;
		     else {
			 // search for next+1 neighbors with higher amplitude
			 Bool_t yOk = kTRUE;
			 if(k>j) {
			     offset1 = 1;
			     if(k+offset1 >= maxRows) yOk = kFALSE;
			 } else {
			     offset1 = -1;
			     if(k+offset1 < 0) yOk = kFALSE;
			 }
			 Bool_t xOk = kTRUE;
			 if(l>i) {
			     offset2 = 1;
			     if(l+offset2 >= maxCols) xOk = kFALSE;
			 } else {
			     offset2 = -1;
			     if(l+offset2 < 0) xOk = kFALSE;
			 }
			 if(l==i || k==j) {
			     if(l==i) {
			       if(yOk) {
				 padnear = l + maxCols*(k+offset1);
				 if(yOk && iPadActive[padnear]) {
				   if((*in)[padnear] > padHeight) {
				   // padheight is contaminated by higher neighbor
				     padHeight*= (*in)[pad]/((*in)[pad]+(*in)[padnear]);
				     if((*in)[padnear] > (*in)[pad]) ++nextMaxCount;
				   }
				 }
			       }
			     } else {
			       if(xOk) {
				 padnear = l+offset2 + maxCols * k;
				 if(iPadActive[padnear]) {
				   if((*in)[padnear] > padHeight) {
				   // padheight is contaminated by higher neighbor
				     padHeight*= (*in)[pad]/((*in)[pad]+(*in)[padnear]);
				     if((*in)[padnear] > (*in)[pad]) ++nextMaxCount;
				   }
				 }
			       }
			     }
			 }
			 if (padHeight == (*in)[pad]) ++nEqualNeighbors;
			 if(padHeight > nearMax) nearMax=padHeight;
			 norm += padHeight;
			 xSum += padHeight*(l-i); //weighted mean calculation
			 ySum += padHeight*(k-j);
		     }
	          }
	       }
	    }

	    if (fMax) {
	       // for pad that is a L.M. the weight contained
	       // in the "in" array  is assigned to the "out" array
	       // and the l.m. is added to the temporary list (fHitCandidate).
	       (*out)[pad] = (*in)[pad];
               // take local max + largest height of next neighbour as ring quality criterium
	       HRichHitCandidate * pCand = new HRichHitCandidate(i,j,(*in)[pad]+nearMax,label,++iHitCount);
	       pCand->setXMean(float(xSum)/float(norm)+float(i));
	       pCand->setYMean(float(ySum)/float(norm)+float(j));
	       pCand->setNoEqualNeighbors(nEqualNeighbors);
	       fHitCandidate.Add(pCand);
	    } else (*out)[pad] = -1;

         }
      }  // end of loops over all pads of given label
   } // end of loops over all labels

   // now selection of maxRings with the highest maxima to be processed
   // checking the distance between candidates

   // The l. m. are sorted by descending quality and the distance
   // between the different candidate is checked. It has to be
   // higher than the threshold.

   fHitCandidate.Sort(kSortDescending);
   if (iHitCount >= 1) {
      Float_t dist2 = float(distance*distance);
      Float_t x1, y1, x2, y2;
      for (j = 0; j < iHitCount; j++) {
         HRichHitCandidate* pCand1 = (HRichHitCandidate*)(fHitCandidate.At(j));
         if (iHit < maxRings && pCand1->getA() > 0) {
            iHit++;
            for (i = j + 1; i < iHitCount; i++) {
	       if (iHit < maxRings) {
	          HRichHitCandidate* pCand2 = (HRichHitCandidate*)(fHitCandidate.At(i));
                  if (pCand1->getA() > 0 && pCand2->getA() > 0) {
                     x1 = pCand1->getXMean();
                     y1 = pCand1->getYMean();
                     x2 = pCand2->getXMean();
                     y2 = pCand2->getYMean();
		     Float_t dx = x2-x1;
		     Float_t dy = y2-y1;
                     if (dx*dx+dy*dy <= dist2) pCand2->setA(0);
		  }
	       }
            }
         }
      }
   }

   // the selected l.m. are added to the list hitList
   for (i = 0; i < iHitCount; i++) {
      HRichHitCandidate* pCand = (HRichHitCandidate*)(fHitCandidate.At(i));
      if (iHit>0 && pCand->getA() > 0) {
         j = pCand->getX();
         k = pCand->getY();
         l = pCand->getA();
	 HRichHit * pRichHit = new HRichHit(j, k, l, pCand->getPadLabel(), pCand->getMaxLabel());
         pRichHit->setPadX(pCand->getXMean());
         pRichHit->setPadY(pCand->getYMean());
	 hitList->Add(pRichHit);
         --iHit;
      }
   }

}
//============================================================================
//----------------------------------------------------------------------------
void
HRichRingFind::MaxSelector(HRichAnalysis* showMe,
                           TList*         hitList,
                           TArrayI*       in,
                           TArrayI*       out)
{
// This function is used to label the clusters of the local maxima.
// The pads corresponding to a L.M. were labeled in the MaxFinding
// function and their values stored in "out" array.
// A label is assigned to each L.M. (hit) belonging to the list "hitList"
// (this list has been filled at the end of the member function
// MaxFinding)
// and this label must be now propagated to all pads belonging
// to the L.M cluster. The condition to be fullfilled in order to belong
// to a cluster is that the pads are reachable from the maximum (L.M.)
// in a descending monotonous way. If a pad belongs to more than
// one clusters it is marked with '-2'. At the end all the connected fired pads
// of a local maximum are marked with the label of this maximum (even
// if they are '-2'. In case of a conflict (possible only if two
// maxima are separated by one pad) the label of maximum with
// higher amplitude is assigned to the pads.
// The propagation of a label is done in a similar way as in the case
// of cleaning algorithm (high amplitude clusters).

   Int_t i, j, k, l, m, pad, padnear;
   Int_t fMaxCode;
   HRichHit *pHit = NULL;
   //loop over all the hits
   for (m = 0; m < hitList->GetSize(); m++) {
      pHit = (HRichHit*)(hitList->At(m));
      pad = pHit->iRingX + maxCols * pHit->iRingY;
      fMaxCode = pHit->iRingMaxLabel;
      // the following function labels the pads belonging
      // to the cluster of each l.m..
      MaxMarker(showMe, in, out, pad, fMaxCode);

   } // eof loop over all local maxima


   // this part (below) is to mark also direct neighboring
   // pads of a local maximum even if they are common pads

   for (m = 0; m < hitList->GetSize(); m++) {
      pHit = (HRichHit*)(hitList->At(m));
      i = pHit->iRingX;
      j = pHit->iRingY;
      pad = i + maxCols * j;
      fMaxCode = pHit->iRingMaxLabel;

      ly_from = ((j - 1 < 0) ? 0 : j - 1);
      ly_to   = ((j + 1 >= maxRows) ? maxRows - 1 : j + 1);
      lx_from = ((i - 1 < 0) ? 0 : i - 1);
      lx_to   = ((i + 1 >= maxCols) ? maxCols - 1 : i + 1);

      for (k = ly_from; k <= ly_to; k++) {
         for (l = lx_from; l <= lx_to; l++) {
            padnear = l + maxCols * k;
            if (iPadActive[padnear] && !(l == i && k == j)) {
               if ((*out)[padnear] == -2) {
                  (*out)[padnear] = fMaxCode;
               } else {
                  if ((*out)[padnear] != 0 &&
                      (*out)[padnear] != fMaxCode &&
                      MaxLabAmpl(hitList, (*out)[padnear]) < pHit->iRingQuality) {
                     (*out)[padnear] = fMaxCode;
                  }
               }
            }
         }
      }
   } // eof second loop over all local maxima

}
//============================================================================


//----------------------------------------------------------------------------
void
HRichRingFind::MaxMarker(HRichAnalysis* showYou,
                         TArrayI*       in,
                         TArrayI*       out,
                         Int_t          nowPad,
                         Int_t          maxCode)
{
//
// This function is called by MaxSelector recursively to propagate
// given label "maxCode" of a l.m. to all pads beloning to the l.m cluster.
// All these pads are marked with '-1' in the "out" array.
// If the pad marked earlier by a label from another maximum is reached
// it is marked as a common pad with '-2'.
// The label is propagated as long as the value of the weight of the
// concatenated pads is monotonous.

   Int_t i, j, k, l, padnear, x_from, x_to, y_from, y_to;
   TArrayI iTempMatrix(9);

   i = nowPad % maxCols;
   j = nowPad / maxCols;

   (*out)[nowPad] = maxCode;

   y_from = ((j - 1 < 0) ? 0 : j - 1);
   y_to   = ((j + 1 >= maxRows) ? maxRows - 1 : j + 1);
   x_from = ((i - 1 < 0) ? 0 : i - 1);
   x_to   = ((i + 1 >= maxCols) ? maxCols - 1 : i + 1);
   // loop on the 8-connected pads around the pad "nowPad".
   for (k = y_from; k <= y_to; k++)
      for (l = x_from; l <= x_to; l++) {
         padnear = l + maxCols * k;
         if (iPadActive[padnear] && !(l == i && k == j))
            if ((*in)[padnear] <= (*in)[nowPad]) {
               // if the neighbouring pad has a weight lower
               // than the l.m. it gets the same label as l.m.
               if ((*out)[padnear] == -1) {
                  (*out)[padnear] = maxCode;
                  iTempMatrix[l-i+1 + 3*(k-j+1)] = maxCode;
               } else if ((*out)[padnear] != 0 &&
                          (*out)[padnear] != maxCode) {
                  (*out)[padnear] = -2;
                  iTempMatrix[l-i+1 + 3*(k-j+1)] = -2;
               }
            }
      }
   // this label procedure is reapeated for all the pads that have
   // been already labeled, as long as they are sorrounded by pads
   // with a lower l.m. weight.

   for (k = 0; k < 3; k++)
      for (l = 0; l < 3; l++)
         if (iTempMatrix[l + 3*k] != 0)
            MaxMarker(showYou, in, out,
                      nowPad + l - 1 + maxCols*(k - 1), iTempMatrix[l + 3*k]);

}
//============================================================================
//----------------------------------------------------------------------------
Int_t
HRichRingFind::MaxLabAmpl(TList *hitList, Int_t maxCode)
{
//
// This is an auxiliary function called by MaxSelector only.
// It returns the amplitude of a local maximum labeled with "maxCode".
//
   Int_t m = 0;
   HRichHit *pHit;

   do {
      pHit = (HRichHit*)(hitList->At(m));
      m++;
   } while (pHit->iRingMaxLabel != maxCode);

   return pHit->iRingQuality;
}
//============================================================================


//----------------------------------------------------------------------------
void
HRichRingFind::MaxAnalysis(HRichAnalysis* showMe,
                           TList*         hitList,
                           TArrayI*       in,
                           TArrayI*       out,
                           Int_t          minAmpl)
{
//
// The function analyses local maximum cluster, calculating the mean
// position of a maximum, weighted by the amplitude of pads in cluster,
// also cluster size and the amplitude sum of the pads belonging to a cluster.
// The data are stored in HRichHit structure.
//

   Int_t m, pad;
   Int_t fMaxCode;
   HRichHit *pHit = NULL;

   for (m = 0; m < hitList->GetSize(); ++m) {
      pHit = (HRichHit*)(hitList->At(m));
      pad = pHit->iRingX + maxCols * pHit->iRingY;
      fMaxCode = pHit->iRingMaxLabel;
      xMeanMax = 0.;
      yMeanMax = 0.;
      xPadMeanMax = 0.;
      yPadMeanMax = 0.;
      thetaMeanMax = 0.;
      phiMeanMax = 0.;
      fMaxClusterSize = 0;
      fMaxClusterSum = 0;
      fMaxThrClusterSize = 0;

      MaxCluster(showMe, in, out, pad, fMaxCode, minAmpl);

      xMeanMax /= fMaxClusterSum;
      yMeanMax /= fMaxClusterSum;
      xPadMeanMax /= fMaxClusterSum;
      yPadMeanMax /= fMaxClusterSum;
      thetaMeanMax /= fMaxClusterSum;
      phiMeanMax /= fMaxClusterSum;
      pHit->fX = xMeanMax;
      pHit->fY = yMeanMax;
      pHit->fMeanTheta = thetaMeanMax;
      pHit->fMeanPhi = phiMeanMax;
      pHit->fMaxClusterSize = fMaxClusterSize;
      pHit->fMaxClusterSum = fMaxClusterSum;
      pHit->fMaxThrClusterSize = fMaxThrClusterSize;

   } // eof loop over all local maxima

}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::MaxCluster(HRichAnalysis* showYou,
                          TArrayI*       in,
                          TArrayI*       out,
                          Int_t          nowPad,
                          Int_t          maxCode,
                          Int_t          minAmpl)
{
//
// Function called recursively from MaxAnalysis to analyse features of local
// maxima clusters; similar to MaxMarker.
//

   Int_t i, j, k, l, padnear, x_from, x_to, y_from, y_to;
   TArrayI iTempMatrix(9);
   HRichPad *pPad = showYou->getGeometryPar()->getPadsPar()->getPad(nowPad);

   xMeanMax += ((*in)[nowPad]) * (pPad->getX());
   yMeanMax += ((*in)[nowPad]) * (pPad->getY());
   // variables below are in pad and pad fraction units
   xPadMeanMax += ((*in)[nowPad]) * ((Float_t)(nowPad % maxCols));
   yPadMeanMax += ((*in)[nowPad]) * ((Float_t)(nowPad / maxCols));
   // variables below are angles
   thetaMeanMax += ((*in)[nowPad]) * (pPad->getTheta());
   phiMeanMax += ((*in)[nowPad]) * (pPad->getPhi(showYou->GetActiveSector()));
   fMaxClusterSize++;
   fMaxClusterSum += (*in)[nowPad];
   if ((*in)[nowPad] > minAmpl) fMaxThrClusterSize++;
   (*out)[nowPad] = 0;

   i = nowPad % maxCols;
   j = nowPad / maxCols;

   y_from = ((j - 1 < 0) ? 0 : j - 1);
   y_to   = ((j + 1 >= maxRows) ? maxRows - 1 : j + 1);
   x_from = ((i - 1 < 0) ? 0 : i - 1);
   x_to   = ((i + 1 >= maxCols) ? maxCols - 1 : i + 1);

   for (k = y_from; k <= y_to; k++)
      for (l = x_from; l <= x_to; l++) {
         padnear = l + maxCols * k;
         if (iPadActive[padnear] && !(l == i && k == j))
            if ((*out)[padnear] == maxCode) {
               (*out)[padnear] = 0;
               iTempMatrix[l-i+1 + 3*(k-j+1)] = 1;
            }
      }
   // here the function MaxCluster is called recursevely
   // in order to take into account all the pads beloning to
   // the cluster in the calculation of the cluster properties.
   for (k = 0; k < 3; k++)
      for (l = 0; l < 3; l++)
         if (iTempMatrix[l + 3*k] > 0)
            MaxCluster(showYou, in, out,
                       nowPad + l - 1 + maxCols*(k - 1), maxCode, minAmpl);
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichRingFind::TestRing(HRichAnalysis* showIt,
                        HRichHit*      hit,
                        Int_t          amplit)
{
//
// All the rings that have been found by the 2 algorithms and then
// further analyzed, are tested.
// There are five tests at the moment and they are active if their
// Status is set to 1 or 2. If Status == 1 a given test is
// performed and the result is stored in the HRichHit structure.
// If Status == 2, the test result must be positive to store
// the hit candidate in the output file. If Status == 0 the test
// is not performed and the information stored in HRichHit can
// be false or true, depending on the test (see tests for details).
//

   Int_t test   = 0;
   Int_t result = 0;
// TestRingCharge is first because it is the only test which can/should be active. W.K.
   result = (Int_t)TestRingCharge(showIt, hit);
   if (pAnalysisParams->isActiveTestCharge == 2 && result==0) return ((test = 3));
   test += 100000 * result;

   result = (Int_t)TestRatio(showIt, hit);
   if (pAnalysisParams->isActiveFiredRingPadsRatio == 2 && result==0) return ((test = 3));
   test += 1000 * result;

   result = (Int_t)TestDensity(showIt, hit);
   if (pAnalysisParams->isActiveTestDensity == 2 && result==0) return ((test = 3));
   test += 1 * result;

   result = (Int_t)TestBorder(&(*showIt), &(*hit), amplit);
   if (pAnalysisParams->isActiveBorderAmplitReduction == 2 && result==0) return ((test = 3));
   test += 10 * result;

   result = (Int_t)TestDynamic(showIt, hit, amplit);
   if (pAnalysisParams->isActiveDynamicThrAmplitude == 2 && result==0) return ((test = 3));
   test += 100 * result;

   result = (Int_t)TestAsymmetry(showIt, hit, amplit);
   if (pAnalysisParams->isActiveTestAsymmetry == 2 && result==0) return ((test = 3));
   test += 10000 * result;

   return test;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestDensity(HRichAnalysis* showYou,
                           HRichHit*      pHit)
{
//
// If density (ratio: fired pads / all active pads) in label larger
// than 2*Sm - see below - exceeds 40%, ring recognition is not performed.
// For small labels like < 2*(pattern matrix surface) this test is not performed.
//

   if (pAnalysisParams->isActiveTestDensity) {
      Int_t iLabelNr = 0, iActivePads = 0,
            iActiveSurface = 0, iMatrixSurface = 0;

      iLabelNr = pHit->iRingFreeParam;
      iActivePads = showYou->GetLabel(iLabelNr)->iFiredPadsNr;
      iActiveSurface = showYou->GetLabel(iLabelNr)->iLabeledPadsNr;
      iMatrixSurface = pAnalysisParams->iRingMaskSize * pAnalysisParams->iRingMaskSize;
      if (0 == iActiveSurface || 0 == iMatrixSurface)
         Error("TestDensity", "possible division by zero");

      if ((Float_t)iActivePads / iActiveSurface > pAnalysisParams->fThresholdDensity &&
          (Float_t)iActiveSurface / (2 * iMatrixSurface) > pAnalysisParams->fSurfaceArea)
         return kFALSE;

      pHit->setTestDens(kTRUE);
   }
   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestBorder(HRichAnalysis* showYou,
                          HRichHit*      pHit,
                          Int_t          amplit)
{
//
// Basic check for the ring amplitude.
// Reduction of the threshold amplitude at the borders of the detector
// is applied, proportionally to the part of a ring which is cut (outside).
// Maximum reduction is by 50% (it means half or more of a ring can be still
// recognised. If there is more than a half of a ring outside it must have
// iRingQuality >= amplit (without any threshold reduction) to be accepted.
// Additionally in this test fBorderFactor is written to a hit !
//

   Float_t fraction = pGeometryParams->getPadsPar()->
                      getPad((UInt_t)pHit->iRingX, (UInt_t)pHit->iRingY)->getAmplitFraction();

   pHit->fBorderFactor = fraction;

   if (!pAnalysisParams->isActiveBorderAmplitReduction && fraction < 0.95)
      if (pHit->iRingQuality < amplit) return kFALSE;

   if (pAnalysisParams->isActiveBorderAmplitReduction && amplit && fraction < 0.95) {

      if (fraction < 0.5) {
         if (pHit->iRingQuality < amplit)  return kFALSE;
      } else if (pHit->iRingQuality < (amplit * fraction)) return kFALSE;

   }
   pHit->setTestBord(kTRUE);
   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestDynamic(HRichAnalysis* showYou,
                           HRichHit*      pHit,
                           Int_t          amplit)
{
//
// The dynamic minimal (threshold) amplitude (quality) is estimated here
// if called with amplit > 0 and for rings that are mainly not outside.
// It is calculated due to highly fenomenological formula:
//
//      threshold Amplitude = amplit * e^( P1*(S/Sm - 1) + P2*(D/P3 - 1) )
//  where
//      P1 - parameter (0.055)
//      P2 - parameter (0.5)
//      Sm = ring pattern matrix surface (in number of pads units)
//      S  = number of pads in given labeled area
//      P3 = mean density (ratio: pads fired / all pads) for ring matrix area ~0.15
//      D  = density of fired pads for given labeled area
//


   Float_t fraction = pGeometryParams->getPadsPar()->
                      getPad((UInt_t)pHit->iRingX, (UInt_t)pHit->iRingY)->getAmplitFraction();

   if (!pAnalysisParams->isActiveDynamicThrAmplitude && fraction >= 0.95)
      if (pHit->iRingQuality < amplit) return kFALSE;


   if (pAnalysisParams->isActiveDynamicThrAmplitude && amplit && fraction >= 0.95) {

      Int_t iDynamicAmplit = 0, iLabelNr = 0;
      Int_t iActivePads = 0, iActiveSurface = 0, iMatrixSurface = 0;

      iLabelNr = pHit->iRingFreeParam;
      iActivePads = showYou->GetLabel(iLabelNr)->iFiredPadsNr;
      iActiveSurface = showYou->GetLabel(iLabelNr)->iLabeledPadsNr;
      iMatrixSurface = pAnalysisParams->iRingMaskSize * pAnalysisParams->iRingMaskSize;
      if (iActiveSurface == 0 || iMatrixSurface == 0)
         Error("TestDynamic", "possible division by zero");

      Float_t fSurfRatio = (Float_t)iActiveSurface / iMatrixSurface;
      Float_t fDensRatio = (Float_t)iActivePads / iActiveSurface;

      if (fSurfRatio <= 1.34 && fDensRatio <= 1.34 * pAnalysisParams->fFormulaParam3) {
         iDynamicAmplit = (Int_t)(amplit * pAnalysisParams->fLowerAmplFactor);
      } else {
         iDynamicAmplit = (Int_t)(amplit * exp(pAnalysisParams->fFormulaParam1 *
                                               (fSurfRatio - 1.) +
                                               pAnalysisParams->fFormulaParam2 *
                                               (fDensRatio / pAnalysisParams->fFormulaParam3 - 1.)));
         if (iDynamicAmplit < amplit) iDynamicAmplit = amplit;
      }
      if (iDynamicAmplit > pHit->iRingQuality) return kFALSE;
   }

   pHit->setTestDyna(kTRUE);

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestRatio(HRichAnalysis* showYou,
                         HRichHit*      pHit)
{
//
// This part calculates how many fired pads belong to ring in comparison
// with the number of fired pads outside and inside. The area of ringMask
// is scanned. In it the fired pads outside/inside of ring must
// not to exceed i.e.  33% of total number of fired pads in this area -
// at least 67% has to be inside of ring border.
// If ring is outside the test is not done (always positive).
//

   if (pAnalysisParams->isActiveFiredRingPadsRatio &&
       pGeometryParams->getPadsPar()->
       getPad((UInt_t)pHit->iRingX, (UInt_t)pHit->iRingY)->getAmplitFraction() >= 0.95) {

      Int_t k, m, n;
      Int_t iOutRing = 0, iOnRing = 0, iInRing = 0, iAllRing = 0;
      Int_t maskSize = pAnalysisParams->iRingMaskSize;
      Int_t iHalfRingMask = maskSize / 2;
      Int_t iMatrixSurface = maskSize * maskSize;

      for (k = 0; k < iMatrixSurface; k++) {
         m = (k % maskSize) - iHalfRingMask;
         n = (k / maskSize) - iHalfRingMask;
         if (!showYou->IsOut(pHit->iRingX, pHit->iRingY, m, n) &&
             showYou->GetPad(pHit->iRingX + m, pHit->iRingY + n)->getAmplitude() > 0) {
            if (pAnalysisParams->iRingMask[k] == 0) {
               iOutRing++;
            } else {
               if (pAnalysisParams->iRingMask[k] == 1) {
                  iOnRing++;
               } else {
                  if (pAnalysisParams->iRingMask[k] == 2) {
                     iInRing++;
                  }
               }
            }
         }
      }
      iAllRing = iOutRing + iOnRing + iInRing;
      if (float(iOutRing + iInRing) >= pAnalysisParams->fFiredRingPadsRatio * float(iAllRing)) return kFALSE;

   }

   pHit->setTestRati(kTRUE);

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestAsymmetry(HRichAnalysis* showYou,
                             HRichHit*      pHit,
                             Int_t          amplit)
{
//
// The asymmetry of ring is investigated. The centroid of the ring is
// defined by the difference in X and Y coordinates between the ring center
// and the center of gravity of the ring. In order to get a positive
// decision from the test  the distance must be lower than iRingRadiusError given in input data.
// Then a ring radius is calculated and it also has to fit
// in the value iRingRadius +/- iRingRadiusError.
// If a ring is clearly outside the border the test is not done (always positive).
// REMARK: ring radius and ring centroid are calculated ONLY for rings
// inside the frame! If the ring candidate is placed partially outside
// zero values for its radius and centroid are stored in HRichHit.
//

   if (pAnalysisParams->isActiveTestAsymmetry &&
       pGeometryParams->getPadsPar()->
       getPad((UInt_t)pHit->iRingX, (UInt_t)pHit->iRingY)->getAmplitFraction() >= 0.95) {

      Int_t i, j;

      Int_t maskSize = pAnalysisParams->iRingMaskSize;
      Int_t maxMaskIndex = maskSize - 1;
      Int_t iHalfRingMask = maskSize / 2;

      Float_t iPosX = 0., iPosY = 0.;
      Int_t iHowManyPads = 0;

      //
      // calculation of ring centroid , use larger matrix (iRingMaskSize instead of iRingMatrixSize) W.K. Feb. 2014
      //

      for (j = 0; j < maskSize; j++){
	 Int_t l = j - iHalfRingMask;
	 Bool_t jBorder = (j==0 || j==maxMaskIndex)? kTRUE:kFALSE;
	 for (i = 0; i < maskSize; i++){
	    if((i==0 && jBorder) || (i==maxMaskIndex && jBorder)) continue; // take corners out of quadratic matrix
            Int_t k = i - iHalfRingMask;
	    if (!showYou->IsOut(pHit->iRingX, pHit->iRingY, k, l)){
               if (showYou->GetPad(pHit->iRingX + k,
                                   pHit->iRingY + l)->getAmplitude() > 0) {
                  iPosX += k;
                  iPosY += l;
                  iHowManyPads++;
	       }
	    }
	 }
      }
      if (iHowManyPads == 0){
	  Error("HRichRingFind::TestAsymmetry", "empty ring");
      } else {
	  iPosX /= iHowManyPads;
	  iPosY /= iHowManyPads;
      }

      pHit->fRingCentroid = sqrt(iPosX * iPosX + iPosY * iPosY);

      //
      // calculation of ring radius
      //

      Int_t matrixSize = pAnalysisParams->iRingMatrixSize;
      Int_t iHalfRingMatrix = matrixSize / 2;
      Int_t maskOffset = (maskSize-matrixSize)/2;
      Float_t fRingRadius = 0.0F;
      iHowManyPads = 0;

      for (j = 0; j < matrixSize; j++) {
         Int_t l = j - iHalfRingMatrix;
         for (i = 0; i < matrixSize; i++) {
            Int_t k = i - iHalfRingMatrix;
	    if (!showYou->IsOut(pHit->iRingX, pHit->iRingY, k, l)) {
               if (showYou->GetPad(pHit->iRingX+k, pHit->iRingY+l)->getAmplitude() > 0 &&
                   pAnalysisParams->iRingMask[i+maskOffset + maskSize*(j+maskOffset)] == 1) {

                  // ring radius is calculated as harmonic mean here

		  if (k!=0 && l!=0) {
		      fRingRadius += 1.0F / sqrt(float(k*k + l*l));
		      ++iHowManyPads;
		  }
	       }
	    }
	 }
      }

      if (iHowManyPads > 0){
         fRingRadius = iHowManyPads / fRingRadius;
	 pHit->fRingRadius = fRingRadius;
      }

      if (pHit->fRingCentroid > pAnalysisParams->iRingRadiusError*2.0F ||
          TMath::Abs(fRingRadius - pAnalysisParams->iRingRadius) >
          pAnalysisParams->iRingRadiusError) return kFALSE;

   } else {
      pHit->fRingCentroid = -1.0;
      pHit->fRingRadius = 0.0F;
   }
   pHit->setTestAsym(kTRUE);

   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichRingFind::TestRingCharge(HRichAnalysis* showYou, HRichHit *hit)
{
//
// Average charge of ring candidate is checked, minimum and maximum
// value are set in the richanalysispar.txt.
//
   if (pAnalysisParams->isActiveTestCharge) {
      Float_t scalFac = pAnalysisParams->fAmpCorrFac[showYou->GetActiveSector()];
      Int_t ringMinCharge = pAnalysisParams->fRingMinCharge*scalFac + 0.5F;
      Int_t ringMaxCharge = pAnalysisParams->fRingMaxCharge*scalFac + 0.5F;

      if (hit->iRingPadNr < 1) return kFALSE;
      if (hit->iRingAmplitude / hit->iRingPadNr < ringMinCharge ||
          hit->iRingAmplitude / hit->iRingPadNr > ringMaxCharge) return kFALSE;
   }
   hit->setTestCharge(kTRUE);
   return kTRUE;
}

//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::CalcRingParameters(HRichAnalysis* showMe,
                                  HRichHit*      pHit)
{
//
// This method calculates for each ring candidate:
// 1) the number of fired pads that belong to the ring (all fired
// pads in the region 13X13 centered in the ring center which fall inside the ring mask)
// 2) the total charge of the ring obtained summing up all
// the charges of the pads that belong to the ring.
// 3) the number of photon local maxima among 5 pads, that should correspond
// to the number of photon in one ring
// 4) the number of photon local maxima among 9 pads.
// It is called at the end of the RingFindHoughTransf and RingFindFitMatrix
// function, after the lists of candidate have been filled and all properties
// calculated
// Bug removed by taking the cluster size search out of the calcFakeContribution and into this routine
// Wolfgang Koenig 29. Apr 2013
//

   Int_t i, j, k, l, m,
         iIsPhot4, iIsPhot8, iPhot4Nr, iPhot8Nr, iPad;
   Int_t iNowX, iNowY;

   iPhot4Nr = iPhot8Nr = iPad = 0;
   Int_t iShift = iRingImageSize / 2;


   iRingTempImage.Reset();
   // loop on all the pads
   for (j = 0; j < iRingImageSize; j++)
      for (i = 0; i < iRingImageSize; i++) {
         if (!showMe->IsOut(pHit->iRingX, pHit->iRingY, i-iShift, j-iShift)) {
            pHit->iRingImage[i + iRingImageSize*j] =
               showMe->GetPad(pHit->iRingX + i-iShift, pHit->iRingY + j-iShift)->getAmplitude();
         } else pHit->iRingImage[i + iRingImageSize*j] = 0;
      }
   // iRingImage contains the amplitudes of all the fired pads in
   // the 13X13 Mask centered in the ring center.
   iPhot4Nr = iPhot8Nr = 0;
   iNowX = pHit->iRingX;
   iNowY = pHit->iRingY;
   for (j = 0; j < iRingImageSize; j++) { // loop on all the pads
      for (i = 0; i < iRingImageSize; i++) {
         if (!showMe->IsOut(iNowX, iNowY, i - iShift, j - iShift)) {
            iIsPhot4 = iIsPhot8 = 0;
            m = iNowX + i - iShift + maxCols * (iNowY + j - iShift);
            if (showMe->GetPad(m)->getAmplitude() > 0 &&
                pAnalysisParams->iRingMask[i + iRingImageSize*j] == 1) {
               pHit->iRingPadNr++;
               pHit->iRingAmplitude += showMe->GetPad(m)->getAmplitude();

               //loop on the pad neighbour to determine if the pad
               // is a photon local maxima among the 4 or 8-connected pads.
	       for (k = -1; k < 2; k++) {
	          for (l = -1; l < 2; l++) {
                     if (((l == 0 && abs(k)) || (k == 0 && abs(l))) && !(l == 0 && k == 0) &&
                         !showMe->IsOut(m, l, k) &&
                         showMe->GetPad(m + l, k)->getAmplitude() >=
                         showMe->GetPad(m)->getAmplitude()) {
                        iIsPhot4++;
		     }
		  }
	       }
               if (iIsPhot4 == 0) {
                  iPhot4Nr++;
                  iRingTempImage[i + iRingImageSize*j] += 1;
		  // "iRingTempImage" contains the label of the photon local maximum
		  // =1 if a photon local maxima has only 4 lower neighbors (left,right,top,bottom)
                  // =2 if the photon has 8 lower neighbors Wolfgang Koenig Feb. 2014

               }
	       for (k = -1; k < 2; k++) {
		  for (l = -1; l < 2; l++) {
                     if (abs(l) && abs(k) && !showMe->IsOut(m, l, k) &&
                         showMe->GetPad(m + l, k)->getAmplitude() >=
                         showMe->GetPad(m)->getAmplitude()) {
                        iIsPhot8++;
		     }
		  }
	       }
               if (iIsPhot4 == 0 && iIsPhot8 == 0) {
                  iPhot8Nr++;
                  iRingTempImage[i + iRingImageSize*j] += 1;
               }
            }
	 }
      }
   }

   pHit->iRingLocalMax4 = iPhot4Nr;
   pHit->iRingLocalMax8 = iPhot8Nr;
   // the number of Clusters the ring is composed of is calculated together with some average variables.
   // Still buggy since the size for overlapping clusters is too large. Wolfgang Koenig
   iCount = 0;
   fClusterSize  = 0.;
   fClusterLMax4 = 0.;
   fClusterLMax8 = 0.;
   for (j = 0; j < iRingImageSize; j++) {// loop over all pads belonging to the ring mask
      for (i = 0; i < iRingImageSize; i++) {
	 if (iRingTempImage[i + iRingImageSize*j] > 0) {
	    iInnerCount = iInnerPhot4 = iInnerPhot8 = 0;
	    // the following function calculates the cluster
	    // properties
	    CalcRingClusters(showMe, &iRingTempImage[0], pHit, i, j);
	    if (iInnerCount) {
	       fClusterSize += iInnerCount;
	       fClusterLMax4 += iInnerPhot4;
	       fClusterLMax8 += iInnerPhot8;
	       iCount++;
	    }
	 }
      }
   }
   pHit->iRingClusterNr = iCount;
   if (iCount > 0) {
      pHit->fRingClusterSize  = fClusterSize  / iCount;
      pHit->fRingClusterLMax4 = fClusterLMax4 / iCount;
      pHit->fRingClusterLMax8 = fClusterLMax8 / iCount;
   }
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::CalcRingClusters(HRichAnalysis* showYou,
                                Int_t*       dumpArr,
                                HRichHit*      pHit,
                                Int_t          nowX,
                                Int_t          nowY)
{
//
// the function calculates the total number of pads and photon
// local maxima (4 and 8-connected pads), for all the clusters
// that belong to a ring.
//

    Int_t a, b;
    Int_t iTempMatrix[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};

// "dumpArr" contains the positions of the photon local maxima (it is
// nothing else than the "iRingTempImage" filled in the CalcRingParameter function.
   if (dumpArr[nowX + iRingImageSize*nowY] < 3) {
      ++iInnerCount;
      Int_t nowPadAmpl = pHit->iRingImage[nowX + iRingImageSize*nowY];
      dumpArr[nowX + iRingImageSize*nowY] += 3;
      // each pad already taken into account is marked with the label >= 3 (local maxima get 4 or 5)

      for (b = 0; b < 3; ++b) { // loop over the neighbouring pads
	 Int_t nextY = nowY + b - 1;
	 for (a = 0; a < 3; ++a) {
	    Int_t nextX = nowX + a - 1;
	    if (nextX >= 0 && nextX < iRingImageSize && nextY >= 0 && nextY < iRingImageSize) {
		if (!(a == 1 && b == 1)) {
                  Int_t nextPadAmpl = pHit->iRingImage[nextX + iRingImageSize*nextY];
		  if ( nextPadAmpl > 0) {
                     Int_t padLabel = dumpArr[nextX + iRingImageSize*nextY];
		     if(padLabel == 0 && nextPadAmpl < nowPadAmpl ) {
		        iTempMatrix[a][b] = 1;
		     }
		     if(padLabel==1) {
			++iInnerPhot4; // cluster extends to next local maximum
		     } else {
			if (padLabel == 2) {
			   ++iInnerPhot4;
			   ++iInnerPhot8;
			}
		     }
		  }
	       }
	    }
	 }
      }
      // CalcRingClusters is called for all the pads connected to the neighbours.
      // (See how the pads for the direct hits clusters are connected.)
   
      for (b = 0; b < 3; ++b) {
	 for (a = 0; a < 3; ++a) {
	    if (iTempMatrix[a][b] > 0)
	       CalcRingClusters(showYou, dumpArr, pHit, nowX + a-1, nowY + b-1);
	 }
      }
   }

} // eof CalcRingClusters

//============================================================================
//----------------------------------------------------------------------------
// remove partner of close pairs, if both rings are identical
Int_t HRichRingFind::CleanIdenticalPairs(HRichAnalysis* showMe) {
    const Int_t cleanDist2 = 12; //maximum search distance**2 for identical rings
    Int_t x1, y1, dx, dy;
    for (Int_t i = 1; i < iRingNr; ++i) {
        x1=pRings[i].iRingX;
        y1=pRings[i].iRingY;
        for (Int_t j = 0; j < i; ++j) {
            dx = x1- pRings[j].iRingX;
	    dy = y1- pRings[j].iRingY;
	    if(dx*dx + dy*dy <= cleanDist2) {
		if(pRings[i].iRingPadNr == pRings[j].iRingPadNr &&
		   pRings[i].iRingAmplitude == pRings[j].iRingAmplitude) {
		    for(Int_t k = i+1; k < iRingNr; ++k) pRings[k-1] = pRings[k];
		    --iRingNr;
                    --i;
		    break;
		}
	    }
        }
    }
    return iRingNr;
}

//============================================================================
//----------------------------------------------------------------------------
Int_t
HRichRingFind::MatchRings(HRichAnalysis* showMe,
                          TList*         hitList1,
                          TList*         hitList2)
{
//
// If both algorithms are active and iSuperiorAlgorithmID = 3 then
// hit with iRingAlgorithmIndex = 3 is Pattern Matrix information
// hit with iRingAlgorithmIndex = 4 is Hough Transform information
// iSuperiorAlgorithmID = 3 means all rings found independently by
// two algorithms are stored.
//
// If both algorithms are active and iSuperiorAlgorithmID = 1 or 2 then
// only rings found by both algorithms are stored. If iSuperiorAlgorithmID = 1
// information is taken from Pattern Matrix and iRingAlgorithmIndex = 5.
// If iSuperiorAlgorithmID = 2 information is taken from Hough Transform
// and iRingAlgorithmIndex = 6. This distinction is made because the
// position of a ring found by both algorithms may slightly differ and
// therefore also ring parameters may differ.
//
// If only one algorithm is active then iSuperiorAlgorithmID flag is
// ignored and rings found by the active algorithm are stored.
// If it is Pattern Matrix iRingAlgorithmIndex = 1 and in the case
// of Hough Transform iRingAlgorithmIndex = 2.
//
// In general if iRingAlgorithmIndexis is odd (1,3,5) information on a ring
// found by Pattern Matrix algorithm is stored and if iRingAlgorithmIndexis
// is even (2,4,6) information on a ring found by Hough Transform algorithm
// is stored.
//

   Bool_t iChosen       = kFALSE;
   Bool_t maxRingsFound = kFALSE;

   Int_t i = 0;
   Int_t j = 0;
   Int_t m = 0;
   Int_t listSize1 = 0;
   Int_t listSize2 = 0;

   HRichHit *pHit1 = NULL;
   HRichHit *pHit2 = NULL;

   iRingNr = 0;
   listSize1 = hitList1->GetSize();
   listSize2 = hitList2->GetSize();

//
// --- all rings found by all algorithms will be stored ---
//
   if (GetAlgorithmNr(showMe) == 2 && pAnalysisParams->iSuperiorAlgorithmID == 3) {
      Int_t maxAlgoRings = pAnalysisParams->iHowManyFitMatrixRings;
      for (m = 0; m < listSize1; m++) {
         pHit1 = (HRichHit*)(hitList1->At(m));
         if (pHit1->fTests != 3) {
            if (iRingNr >= maxAlgoRings) break;
            pRings[iRingNr] = *pHit1;
            pRings[iRingNr].iRingAlgorithmIndex = 3;
            pRings[iRingNr].iRingPatMat = pHit1->iRingQuality;
            pRings[iRingNr].iRingHouTra = 0;
            iRingNr++;
         }
      }
      maxAlgoRings = pAnalysisParams->iHowManyHoughTransfRings;
      for (m = 0; m < listSize2; m++) {
         pHit2 = (HRichHit*)(hitList2->At(m));
         if (pHit2->fTests != 3) {
            if (iRingNr >= maxAlgoRings) break;
            pRings[iRingNr] = *pHit2;
            pRings[iRingNr].iRingAlgorithmIndex = 4;
            pRings[iRingNr].iRingPatMat = 0;
            pRings[iRingNr].iRingHouTra = pHit2->iRingQuality;
            iRingNr++;
         }
      }
      CalcFakeContribution(showMe);
      return (showMe->iRingNr = iRingNr);
   }

//
// --- only rings found by both algorithms stored ---
//
   if (GetAlgorithmNr(showMe) == 2) {
      Float_t maxAlgoDist = 1.5F;
      maxRingsFound = kFALSE;
      for (i = 0; i < listSize1; i++) {
         pHit1 = (HRichHit*)(hitList1->At(i));
         if (pHit1->fTests != 3 && pHit1->iRingQuality > 0) {
            for (j = 0; j < listSize2; j++) {
               pHit2 = (HRichHit*)(hitList2->At(j));
               iChosen = kFALSE;
               if (pHit2->fTests != 3 && pHit2->iRingQuality > 0) {
                  // if the two rings are so close to each other to be the same.
                  if (CalcDistanceMean(*pHit1, *pHit2) <= maxAlgoDist) {
                     if (iRingNr >= maxRings) { // should never happen
                        maxRingsFound = kTRUE;
                        break;
                     }
                     if (pAnalysisParams->iSuperiorAlgorithmID == 1) {
                        pRings[iRingNr] = *pHit1;
                        pRings[iRingNr].iRingPatMat = pHit1->iRingQuality;
                        pRings[iRingNr].iRingHouTra = pHit2->iRingQuality;
                        pRings[iRingNr].iRingAlgorithmIndex = 5;
                     } else {
                        pRings[iRingNr] = *pHit2;
                        pRings[iRingNr].iRingPatMat = pHit1->iRingQuality;
                        pRings[iRingNr].iRingHouTra = pHit2->iRingQuality;
                        pRings[iRingNr].iRingAlgorithmIndex = 6;
                     }
                     iRingNr++;
                     iChosen = kTRUE;
                  }
               }
               if (kTRUE == iChosen) break;
            }
         }
         if (kTRUE == maxRingsFound) break;
      }
      CalcFakeContribution(showMe);

      return (showMe->iRingNr = iRingNr);
   }

//
// --- rings found by selected algorithm ---
//
   if (GetAlgorithmNr(showMe) == 1) {
      if (pAnalysisParams->isActiveRingFindFitMatrix) {
         Int_t maxAlgoRings = pAnalysisParams->iHowManyFitMatrixRings;
         for (m = 0; m < listSize1; m++) {
            pHit1 = (HRichHit*)(hitList1->At(m));
            if (pHit1->fTests != 3) {
               if (iRingNr >= maxAlgoRings) break;
               pRings[m] = *pHit1;
               pRings[m].iRingAlgorithmIndex = 1;
               pRings[m].iRingPatMat = pHit1->iRingQuality;
               pRings[m].iRingHouTra = 0;
               iRingNr++;
            }
         }
         CalcFakeContribution(showMe);
         return (showMe->iRingNr = iRingNr);
      }
      if (pAnalysisParams->isActiveRingHoughTransf) {
         Int_t maxAlgoRings = pAnalysisParams->iHowManyHoughTransfRings;
         for (m = 0; m < listSize2; m++) {
            pHit2 = (HRichHit*)(hitList2->At(m));
            if (pHit2->fTests != 3) {
               if (iRingNr >= maxAlgoRings) break;
               pRings[m] = *pHit2;
               pRings[m].iRingAlgorithmIndex = 2;
               pRings[m].iRingHouTra = pHit2->iRingQuality;
               pRings[m].iRingPatMat = 0;
               iRingNr++;
            }
         }
         CalcFakeContribution(showMe);
         return (showMe->iRingNr = iRingNr);
      }
   }

   return (showMe->iRingNr = 0); // this should never happen
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::CalcFakeContribution(HRichAnalysis *showMe)
{

   if (iRingNr == 0 &&
       pGeometryParams->getPadsPar()->getActivePadsNr() > iRingImageSize*iRingImageSize) {

      // If any ring is not found a random position iNowX, iNowY is drawn and
      // treated as if it were a ring centre. Local maxima (4- and 8-connected)
      // are calculated as well as the number of fired pads. This gives
      // the estimation of fakes contribution in real rings (number of fake
      // fired pads and fake local maxima).
      Int_t i, j, k, l, m,
            iIsPhot4, iIsPhot8, iPhot4Nr, iPhot8Nr, iPad;
      iPhot4Nr = iPhot8Nr = iPad = 0;
      Int_t iShift = iRingImageSize / 2;
      Int_t iNowX, iNowY;

      do {// center of the fake ring
         iNowX = (Int_t)HomogenDistr(pAnalysisParams->iRingRadius,
                                     maxCols - pAnalysisParams->iRingRadius);
         iNowY = (Int_t)HomogenDistr(pAnalysisParams->iRingRadius,
                                     maxRows - pAnalysisParams->iRingRadius);
      } while (showMe->IsOut(iNowX, iNowY, 0, 0));

      for (j = 0; j < iRingImageSize; j++)
         for (i = 0; i < iRingImageSize; i++)
            if (!showMe->IsOut(iNowX, iNowY, i - iShift, j - iShift)) {
               iIsPhot4 = iIsPhot8 = 0;
               m = iNowX + i - iShift + maxCols * (iNowY + j - iShift);
               if (showMe->GetPad(m)->getAmplitude() > 0 &&
                   pAnalysisParams->iRingMask[i + (iRingImageSize)*j] == 1) {
                  iPad++;
                  for (k = -1; k < 2; k++)
                     for (l = -1; l < 2; l++)
                        if (((l == 0 && abs(k)) ||
                             (k == 0 && abs(l))) &&
                            !(l == 0 && k == 0) &&
                            !showMe->IsOut(m, l, k) &&
                            showMe->GetPad(m + l, k)->getAmplitude() >=
                            showMe->GetPad(m)->getAmplitude())
                           iIsPhot4++;
                  if (iIsPhot4 == 0) iPhot4Nr++;
                  for (k = -1; k < 2; k++)
                     for (l = -1; l < 2; l++)
                        if (abs(l) && abs(k) &&
                            !showMe->IsOut(m, l, k) &&
                            showMe->GetPad(m + l, k)->getAmplitude()
                            >= showMe->GetPad(m)->getAmplitude())
                           iIsPhot8++;
                  if (iIsPhot4 == 0 && iIsPhot8 == 0) iPhot8Nr++;
               }
            }

      showMe->iFakePad = iPad;
      showMe->iFakeLocalMax4 = iPhot4Nr;
      showMe->iFakeLocalMax8 = iPhot8Nr;

   } // end of condition with iRingNr
} // eof CalcRingParameters
//============================================================================

//----------------------------------------------------------------------------
void
HRichRingFind::CloseMaxRejection(TList *hitList)
{
//
// this method tests the quality of candidates close
// to bright rings. Picking up some intensity of the
// bright ring fakes could be identified.
// If the quality of these "fakes" candidates is too
// low compared with the quality of the bright ring
// the candidates are rejected ( if the isActiveFakesRejection
// is equal 2) or stored and labeled with a flag.
// If the quality of the 2 compared rings is similar
// the centroid of both is checked, if one of the two is
// very assymmetric is rejected.

   Int_t listSize = hitList->GetSize();
   HRichHit *pHit1 = NULL;
   HRichHit *pHit2 = NULL;

   if (pAnalysisParams-> isActiveFakesRejection) {
      Float_t maxFakeDistSquared = pAnalysisParams->iRingRadius * pAnalysisParams->iRingRadius * 4.2F; // d = 8.2
      Float_t fakeQualityRatio = pAnalysisParams->fFakeQualityRatio;
      Float_t fakeCentroidCut = pAnalysisParams->fFakeCentroidCut;
   
      for (Int_t i = 0; i < listSize ; i++) {
	 pHit1 = (HRichHit*)(hitList->At(i));
	 if (pHit1->fTests != 3) {
	    for (Int_t j = 0; j < listSize ; j++) {
	       pHit2 = (HRichHit*)(hitList->At(j));
	       if (pHit2->fTests != 3) {
		  Int_t dx = pHit1->iRingX - pHit2->iRingX;
		  Int_t dy = pHit1->iRingY - pHit2->iRingY;
		  Float_t distSquared = dx * dx + dy * dy;
		  if (distSquared < maxFakeDistSquared && i != j) {
		     // only candidates whose distance is lower then ring diameter are compared
		     if (pHit1->iRingQuality + pHit2->iRingQuality == 0)
			Error("CloseMaxRejection", "division by zero"); // should never happen
		     Float_t dQ = (Float_t)(pHit1->iRingQuality - pHit2->iRingQuality)
			        / (Float_t)(pHit1->iRingQuality + pHit2->iRingQuality);

		     if (pHit2->getCentroid() > pHit1->getCentroid() + 0.5F) {
			if (dQ > fakeQualityRatio || pHit2->getCentroid() >= fakeCentroidCut) {
			   pHit2->setRejFake(0);
			}
			continue;
		     }
		     if (pHit1->getCentroid() > pHit2->getCentroid() + 0.5F) {
			if (dQ < -fakeQualityRatio || pHit1->getCentroid() >= fakeCentroidCut) {
			   pHit1->setRejFake(0);
			}
			continue;
		     }
		  }
	       }
	    }// end second loop on the ring list
	 }
      }//end first loop on the ring list
   } // end check isActive flag

   for (Int_t i = 0; i < listSize ; i++) {
      pHit1 = (HRichHit*)(hitList->At(i));
//    if ((2 == pAnalysisParams-> isActiveFakesRejection && 0 == pHit1->getRejFake()) || 3 == pHit1-> fTests) {
      if (0 == pHit1->getRejFake() || 3 == pHit1-> fTests) {
         pHit1-> fTests = 3;
      } else {
         pHit1-> fTests += pHit1->getRejFake() * 1000000;
      }
   }

}
//============================================================================

