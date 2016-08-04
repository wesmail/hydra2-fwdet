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
//  HRichPadClean
//
//  This class cleans pad plane provided by HRichAnalysis. This class
//  isn't any task but it is called for each event from
//  the HRichAnalysis::execute() function.
//
//////////////////////////////////////////////////////////////////////////////


#include "hades.h"
#include "hevent.h"
#include "hmatrixcategory.h"
#include "hrichanalysispar.h"
#include "hrichdetector.h"
#include "hrichdirclus.h"
#include "hrichgeometrypar.h"
#include "hrichpadclean.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>

using namespace std;

ClassImp(HRichPadClean)

//----------------------------------------------------------------------------
HRichPadClean::HRichPadClean()
{
   iCount       = 0;
   minX         = 200;
   maxX         = 200;
   minY         = 0;
   maxY         = 0;
   deltaPhi     = 0;
   fRichClusCat = NULL;
   iTempCluster.Set(100);
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadClean::~HRichPadClean()
{

}
//============================================================================

//----------------------------------------------------------------------------
HRichPadClean::HRichPadClean(const HRichPadClean& source) :TObject(source)
{
   iCount = source.iCount;
   iTempCluster = source.iTempCluster;
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadClean&
HRichPadClean::operator=(const HRichPadClean& source)
{
   if (this != &source) {
      iCount = source.iCount;
      iTempCluster = source.iTempCluster;
   }
   return *this;
}
//============================================================================

//----------------------------------------------------------------------------
Bool_t
HRichPadClean::init()
{
   fRichClusCat = gHades->getCurrentEvent()->getCategory(catRichDirClus);
   if (NULL == fRichClusCat) {
      fRichClusCat = gHades->getSetup()->getDetector("Rich")->buildCategory(catRichDirClus);
      if (NULL == fRichClusCat) {
         Error("init", "Category catRichDirClus could not be created");
         return kFALSE;
      } else
         gHades->getCurrentEvent()->addCategory(catRichDirClus, fRichClusCat, "Rich");
   }
   return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichPadClean::Execute(HRichAnalysis *giveMe)
{
// Cleaning algorithm works in two steps:
//   1. First the charged particle clusters are identified and cleaned.
//      Cleaning is triggered by any pad, which charge is higher than
//      the value set in HRichAnalysisPar::iCleanHighPulseUpperThreshold.
//      If such a pad is found, all "continuously" surrounding area is
//      cleaned as well. The limit for "continuous" is defined by the
//      parameter HRichAnalysisPar::iCleanHighPulseBorder.
//      Threshold is reduced for sector 3 (reduced high voltage/gain).
//      Wolfgang Koenig April 2013
//   2. Afterwards all stand alone pads with charge below certain threshold
//      defined in HRichAnalysisPar::iCleanAlonePadLowerThreshold are cleaned.
//      Condition for stand alone pad is defined by
//      HRichAnalysisPar::iCleanAlonePadBorder.

   if (giveMe->iPadFiredNr > 2) {
      // what is the reason for this check???? MJ
      Int_t iReducedNr = 0;
      HRichAnalysisPar *analParams = giveMe->getAnalysisPar();
      Float_t corFac =  analParams->fAmpCorrFac[giveMe->GetActiveSector()];

      if (analParams->isActiveCleanHighPulse) {
	 iReducedNr += CleanHighPulse(giveMe, analParams->iCleanHighPulseBorder,
		       Int_t(analParams->iCleanHighPulseUpperThreshold * corFac + 0.5F));
      }

      if (analParams->isActiveCleanAlonePad)
         iReducedNr += CleanAlonePad(giveMe, analParams->iCleanAlonePadBorder,
                       Int_t(analParams->iCleanAlonePadLowerThreshold*(0.5F + corFac*0.5F) + 0.5F));
     // use only 1/half of the effect of the sector specific corFac (Mix of electronic noise and HV driven detector gain)
      return (iReducedNr);
   } else
      return giveMe->iPadFiredNr; //either 0, 1 or 2
}

//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichPadClean::CleanHighPulse(HRichAnalysis* showMe,
                              Int_t          border,
                              Int_t          upperThr)
{
//
// This method deletes all the pads above upper threshold (upperThr)
// and the cluster connected to each pad.
//

   if (0 == border) {
      Error("CleanAlonePad", "Argument \'border\' in function CleanAlonePad must be larger than 0!");
      return -1;
   }

#ifdef HRICH_DEBUGMODE
   cout << "RICH DEBUG MODE: DeletePulse cleans the following pads: \n";
#endif

   const Int_t maxRows = showMe->GetPadsYNr();
   const Int_t maxCols = showMe->GetPadsXNr();
   Int_t iCountBefore  = 0;
   Int_t nCleaned      = 0;
   Int_t offset        = 0;
   Int_t rightBorder   = 0;
   HRichDirClus* clus  = NULL;
   HLocation loc;

   Int_t old_row = 0;
   Int_t old_col = 0;

   iCount = 0;

   for (Int_t row = 0; row < maxRows; ++row) {
      offset      = row * maxCols;
      rightBorder = showMe->pRightBorder[row];
      for (Int_t col = showMe->pLeftBorder[row]; col <= rightBorder; ++col) {
         if (showMe->GetPad(col + offset)->getAmplitude() > upperThr) {
            chargeTot = 0; // fix MJ
            minX = 200;
            maxX = 0;
            minY = 200;
            maxY = 0;
            deltaPhi = 0;
            iCountBefore = iCount;
            old_row = row;
            old_col = col;
            DeletePulse(showMe, border, col, row);
            if (old_row != row || old_col != col)
               Error("CleanHighPulse", "Numbers changed: old = [%d,%d], new = old = [%d,%d]", old_row, old_col, row, col);
            if ((iCount - iCountBefore) > 0 && nCleaned < 100) {
               iTempCluster[nCleaned++] = iCount - iCountBefore;

               // Store the cluster and its parameters to the HRichDirClus category
               clus = NULL;
               loc.set(1, 0);
               clus = static_cast<HRichDirClus*>(static_cast<HMatrixCategory*>(fRichClusCat)->getNewSlot(loc));

               if (NULL == clus)
                  Error("CleanHighPulse", "Error no slot free in HRichDirClus !!!!!!!!!!!  ");
               else {
                  clus = new(clus) HRichDirClus;
                  clus->setSector(showMe->GetActiveSector());
                  clus->setX((Int_t)(maxX + minX) / 2);
                  clus->setY((Int_t)(maxY + minY) / 2);
                  clus->setXYDim(TMath::Abs(minX - maxX),
                                 TMath::Abs(minY - maxY));
                  clus->setTotalCharge(chargeTot);
                  clus->setNrPad(iCount - iCountBefore);
                  clus->setPhiDiff(calculateDPhi(showMe, minX, minY, maxX, maxY));
                  Float_t theta = -1;
                  if (NULL != ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->
                      getPad(((UInt_t)(maxX + minX) / 2), ((UInt_t)(maxY + minY) / 2))) {
                     theta = ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->
                             getPad(((UInt_t)(maxX + minX) / 2), ((UInt_t)(maxY + minY) / 2))->getTheta();
                  }
                  clus->setTheta(theta);
               }
            }
         }
      }
   }

   showMe->iClusterCleanedNr = nCleaned;
   showMe->iClustersCleaned.Set(nCleaned);

   for (Int_t i = 0; i < nCleaned; i++)
      showMe->iClustersCleaned[i] = iTempCluster[i];

#ifdef HRICH_DEBUGMODE
   if (!iCount) cout << "None. \n" << "RICH DEBUG MODE: Total number "
                        << "of pads cleaned by CleanHighPulse = 0\n";
   else cout << "\nRICH DEBUG MODE: Total number of pads cleaned "
                << "by CleanHighPulse = " << iCount << "\n";
#endif

   return iCount;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t
HRichPadClean::CleanAlonePad(HRichAnalysis *showMe,
                             Int_t          border,
                             Int_t          lowerThr)
{
// This method deletes all single pad under threshold (lowerThr).
// A single pad is defined as pad whose first fired neighbour
// has at least a distance of "border" from the pad


   const Int_t     maxCols      = showMe->GetPadsXNr();
   const Int_t     maxRows      = showMe->GetPadsYNr();
   Int_t           leftBorder;
   Int_t           rightBorder;
   Int_t           offset;
   HRichPadSignal* pad          = NULL;

   iCount = 0;

   if (0 == border) {
      Error("CleanAlonePad", "Argument \'border\' in function CleanAlonePad must be larger than 0!");
      return -1;
   }

#ifdef HRICH_DEBUGMODE
   cout << "RICH DEBUG MODE: CleanAlonePad cleans the following pads: \n";
#endif


   for (Int_t row = 0; row < maxRows; ++row) {
      leftBorder  = showMe->pLeftBorder[row];
      rightBorder = showMe->pRightBorder[row];
      pad = showMe->GetPad(leftBorder + row * maxCols);
      for (Int_t col = leftBorder; col <= rightBorder; ++col) {
         if (pad->getAmplitude() > 0) {
            for (Int_t k = row - border; k <= row + border; ++k) {
               offset = k * maxCols;
               for (Int_t l = col - border; l <= col + border; ++l)
                  if (!(l == col && k == row))
                     if (!showMe->IsOut(l, k))
                        if (showMe->GetPad(l + offset)->getAmplitude() > 0)
                           goto nextPad;
            }

            if (pad->getAmplitude() < lowerThr) {
               pad->setAmplitude(0);
               pad->setIsCleanedSingle(kTRUE);
               iCount++;

#ifdef HRICH_DEBUGMODE
               cout << " (" << colStart + border << "," << rowStart + border << ")... ";
#endif

            }
         }
nextPad:
         ++pad;
      }
   }

#ifdef HRICH_DEBUGMODE
   if (0 == iCount) cout << "None. \n";
   else cout << "\nRICH DEBUG MODE: Total number of pads cleaned by "
                "CleanAlonePad = " << iCount << "\n";
#endif

   return iCount;
}
//============================================================================

//----------------------------------------------------------------------------
void
HRichPadClean::DeletePulse(HRichAnalysis* showYou,
                           Int_t          border,
                           Int_t          col,
                           Int_t          row)
{
// This method deletes a given pad and all the neighbours within
// a distance of "border"

   const Int_t maxCols = showYou->GetPadsXNr();
   Int_t       offset;

   chargeTot += showYou->GetPad(col + maxCols * row)->getAmplitude();

#ifdef HRICH_DEBUGMODE
   cout << " charge  " << showYou->GetPad(col + maxCols * row)->getAmplitude()
        << "  chargeTot  " << chargeTot << endl;
#endif

   showYou->GetPad(col + maxCols * row)->setAmplitude(0);
   showYou->GetPad(col + maxCols * row)->setIsCleanedHigh(kTRUE);


#ifdef HRICH_DEBUGMODE
   cout << ((HRichGeometryPar*)showYou->getGeometryPar())->getPadsPar()->getPad(col, row)
        << "  first pad  with the second method" << endl;
   cout << "  ==================== it is assignment time!========= " << endl;
   cout << " minX " <<   minX    << " maxY  " <<   maxY    << "  col  " << col << " minY  "
        <<   minY   << " row   " <<   row     << " maxY  " << maxY << endl;
#endif

   minX = minX >= col ? col : minX;
   minY = minY >= row ? row : minY;
   maxX = maxX <= col ? col : maxX;
   maxY = maxY <= row ? row : maxY;

#ifdef HRICH_DEBUGMODE
   cout << "  the harder you try the dumber you look " << endl;
   cout << " minX " << minX << " maxX " << maxX << "  col  " << col
        << " minY " << minY << " row  " << row  << " maxY  " << maxY << endl;
#endif

   iCount++;

#ifdef HRICH_DEBUGMODE
   cout << " (" << colStart + border << "," << rowStart + border << ")... ";
#endif

   for (Int_t i = row - border; i <= row + border; ++i) {
      offset = maxCols * i;
      for (Int_t j = col - border; j <= col + border; ++j)
         if (!showYou->IsOut(j, i))
            if (!(i == row && j == col))
               if (showYou->GetPad(j + offset)->getAmplitude() > 0)
                  DeletePulse(showYou, border, j, i);
   }

   return;
}
//============================================================================

//----------------------------------------------------------------------------
Float_t
HRichPadClean::calculateDPhi(HRichAnalysis *showMe,
                             Int_t          xmin,
                             Int_t          ymin,
                             Int_t          xmax,
                             Int_t          ymax)
{
   Float_t phi1 = -1;
   Float_t phi2 = -1;

#ifdef HRICH_DEBUGMODE
   cout << " xmin " << xmin << " ymin   " << ymin << " xmax  " << xmax << "   ymax " << ymax << endl;
#endif

   Int_t maxCols = showMe->GetPadsXNr();

#ifdef HRICH_DEBUGMODE
   cout << ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->getPad(xmin, ymin) << "  first pad  " << endl;
   cout << ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->getPad(xmin + maxCols * ymin) << "  first pad  " << endl;
   cout << ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->getPad(xmax + maxCols * ymax) << "  second pad  " << endl;
   cout << showMe->GetPad(xmin + maxCols * ymin) << " first pad, second method  " << endl;
#endif

   if (showMe->GetPad(xmin + maxCols * ymin))
      phi1 = ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->getPad(xmin + maxCols * ymin)->getPhi(showMe->GetActiveSector());
   if (showMe->GetPad(xmax + maxCols * ymax))
      phi2 = ((HRichGeometryPar*)showMe->getGeometryPar())->getPadsPar()->getPad(xmax + maxCols * ymax)->getPhi(showMe->GetActiveSector());


#ifdef HRICH_DEBUGMODE
   cout << "  delta Phi " << TMath::Abs(phi1 - phi2) << endl;
#endif


   if (-1 == phi1 && -1 == phi2)
      return -1;

   return TMath::Abs(phi1 - phi2);
}
//============================================================================

