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
//  HRichPadLabel
//
//  Functions for labelling pad plane.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichanalysispar.h"
#include "hrichpadgraph.h"
#include "hrichpadlabel.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichPadLabel)


//----------------------------------------------------------------------------
HRichPadLabel::HRichPadLabel()
{
   iGraphArray = 128;
   iLabelGraphConnection.Set(iGraphArray * iGraphArray);
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadLabel::HRichPadLabel(Int_t padsx, Int_t padsy)
{
   if (padsx > padsy) iGraphArray = padsx;
   else iGraphArray = padsy;
   iLabelGraphConnection.Set(iGraphArray * iGraphArray);
}
//============================================================================

//----------------------------------------------------------------------------
HRichPadLabel::~HRichPadLabel() {}
//============================================================================
/*
//----------------------------------------------------------------------------
HRichPadLabel::HRichPadLabel(const HRichPadLabel& source) {}
//============================================================================
*/
//----------------------------------------------------------------------------
HRichPadLabel& HRichPadLabel::operator=(const HRichPadLabel& source)
{
   if (this != &source) {}

   return *this;
}
//============================================================================

//----------------------------------------------------------------------------
void HRichPadLabel::Reset()
{
   iLabelGraphConnection.Reset();
}
//============================================================================

//----------------------------------------------------------------------------
Int_t HRichPadLabel::LabelPads(HRichAnalysis *showMe, Int_t padDistance)
{

   Int_t m, n, rowOffset, xNow, yNow, xPad, yPad, xMax, yMax, nMin, nMax, mMin, mMax;
   Int_t maxCols = showMe->GetPadsXNr();
   Int_t maxRows = showMe->GetPadsYNr();
   HRichPadSignal * currentPad;

   iLabelGraphConnection.Reset();

   for (yNow = 0; yNow < maxRows; ++yNow) {
      Int_t offsetNow = yNow * maxCols;
      Int_t rightBorder = showMe->pRightBorder[yNow];
      for (xNow = showMe->pLeftBorder[yNow]; xNow <= rightBorder; ++xNow) {
         currentPad = showMe->GetPad(xNow + offsetNow);
         if (currentPad->getAmplitude() > 0) {
            currentPad->setLock(1);
            currentPad->setLabel(-1); // fired pad plus area around it makes a label
            yMax = yNow + padDistance;
            xMax = xNow + padDistance;
            for (yPad = yNow - padDistance; yPad <= yMax; ++yPad) {
               Int_t offset = maxCols * yPad;

               for (xPad = xNow - padDistance; xPad <= xMax; ++xPad) {

                  if (!(xPad == xNow && yPad == yNow))
                     if (!showMe->IsOut(xPad, yPad)) {
                        showMe->GetPad(xPad + offset)->setLabel(-1); // fired pad plus area around it makes a label
                        if (showMe->GetPad(xPad + offset)->getAmplitude() > 0 &&
                            showMe->GetPad(xPad + offset)->getLock() == 0) {
                           if (yPad >= yNow) {
                              nMin = yNow;
                              nMax = yPad;
                           } else {
                              nMin = yPad;
                              nMax = yNow;
                           }
                           if (xPad >= xNow) {
                              mMin = xNow;
                              mMax = xPad;
                           } else {
                              mMin = xPad;
                              mMax = xNow;
                           }
                           for (n = nMin; n <= nMax; n++) {
                              rowOffset = maxCols * n;
                              for (m = mMin; m <= mMax; m++)
                                 showMe->GetPad(m + rowOffset)->setLabel(-1);
                           }
                        }
                     }
               }
            }
         }
      }
   }

//   cout << endl << "-----------LABEL------------" << endl;;
//   for (Int_t kk = 0; kk < 4096; kk++) {
//    if ((kk % 64) == 0) cout << endl;
//    if (showMe->GetPad(kk)->getLabel() < 0) cout << "o";
//     else cout << ".";
//   }
//   cout << endl;


   Int_t padL, padD, marker = 0;
   Int_t maxPads = showMe->GetPadNr();
   Int_t i, k;
   for (i = 0; i < maxPads; i++)
      if (showMe->GetPad(i)->getLabel() < 0) {
         xNow = i % maxCols;
         yNow = i / maxCols;
         padL = padD = 0;
         if (xNow > 0 && showMe->GetPad(i - 1)->getLabel() > 0)
            padL = showMe->GetPad(i - 1)->getLabel();
         if (yNow > 0 && showMe->GetPad(i - maxCols)->getLabel() > 0)
            padD = showMe->GetPad(i - maxCols)->getLabel();

         currentPad = showMe->GetPad(i);
         if (!padL && !padD) currentPad->setLabel(++marker);
         else {

            if (padL && padD) {

               if (padL != padD && !iLabelGraphConnection[padL-1 + iGraphArray*(padD-1)]) {
                  iLabelGraphConnection[padL-1 + iGraphArray*(padD-1)] = 1;
                  iLabelGraphConnection[padD-1 + iGraphArray*(padL-1)] = 1;
               }
               if (padL < padD) {
                  if (padL < marker) currentPad->setLabel(padL);
                  else currentPad->setLabel(marker);
               } else {
                  if (padD < marker) currentPad->setLabel(padD);
                  else currentPad->setLabel(marker);
               }
            }

            if (padL && !padD) {
               if (padL < marker) currentPad->setLabel(padL);
               else currentPad->setLabel(marker);
            }

            if (!padL && padD) {
               if (padD < marker) currentPad->setLabel(padD);
               else currentPad->setLabel(marker);
            }

         }
      }
// end of 'for' loop


//   cout << endl << "-----------LABEL------------" << endl;;
//   for (Int_t kk = 0; kk < 4096; kk++) {
//    if ((kk % 64) == 0) cout << endl;
//    if (showMe->GetPad(kk)->getLabel() < 0) cout << "o";
//     else if (showMe->GetPad(kk)->getLabel() > 0) cout << showMe->GetPad(kk)->getLabel();
//     else cout << ".";
//   }
//   cout << endl;



   HRichPadGraph *pClusterList = NULL;
   if (marker) pClusterList = new HRichPadGraph[marker]; //new in event loop
   for (i = 0; i < marker; i++) pClusterList[i].InitGraph(i + 1, marker);

   for (k = 0; k < marker; k++)
      for (m = 0; m < marker; m++)
         if (m < k && iLabelGraphConnection[m + iGraphArray*k]) {
            pClusterList[m].AddNode(pClusterList + k);
            pClusterList[k].AddNode(pClusterList + m);
         }



   k = 0;
   for (i = 0; i < marker; i++)
      if (pClusterList[i].MarkNodes(k)) k++;


   HRichLabel *pAreaData = NULL;
   if (k) pAreaData = new HRichLabel[k]; //new in event loop
   HRichLabel::iLabelNr = k;

   if (pAreaData != NULL && pClusterList != NULL) {
      Int_t label;
      for (i = 0; i < maxPads; i++) {
         label = showMe->GetPad(i)->getLabel();
         if (label > 0) {
            --label;
            m = i % maxCols;
            n = i / maxCols;
            HRichLabel & area = pAreaData[pClusterList[label].iLabel - 1];

            if (area.iLeftX >= m) area.iLeftX = m;
            if (area.iRightX <= m) area.iRightX = m;
            if (area.iLowerY >= n) area.iLowerY = n;
            if (area.iUpperY <= n) area.iUpperY = n;
            if (showMe->GetPad(i)->getAmplitude() > 0) area.iFiredPadsNr++;
            area.iLabeledPadsNr++;
            area.iSignature = pClusterList[label].iLabel;
            showMe->GetPad(i)->setLabel(pClusterList[label].iLabel);
         }
      }
   }

   if (pClusterList) {
      delete [] pClusterList;
      pClusterList = NULL;

#ifdef HRICH_DEBUGMODE0
      cout << "RICH DEBUG MODE: pointer \'pClusterList\' (type \'class "
           "HRichPadGraph\') deleted.\n";
#endif
   }

//  now grouping section - deciding if a given label may be accepted

   for (i = 0; i < k; i++)
      if (pAreaData[i].iRightX - pAreaData[i].iLeftX < padDistance ||
          pAreaData[i].iUpperY - pAreaData[i].iLowerY < padDistance)
         pAreaData[i].Reset();

   if (showMe->pLabelArea) {
      delete [] showMe->pLabelArea;
      showMe->pLabelArea = NULL;
   }
   showMe->iLabelNr = 0;
   if (HRichLabel::iLabelNr) {
      showMe->iLabelNr = HRichLabel::iLabelNr;
      showMe->pLabelArea = new HRichLabel[HRichLabel::iLabelNr];
   }


//cout << "Temporary debug: Accepted labels: " << HRichLabel::iLabelNr << endl;

   m = 0;
   for (i = 0; i < k; i++)
      if (pAreaData[i].iSignature) showMe->pLabelArea[m++] = pAreaData[i];

#ifdef HRICH_DEBUGMODE
   cout << "RICH DEBUG MODE: LabelPads divides pads into following areas:\n";
   for (i = 0; i < HRichLabel::iLabelNr; i++) {
      HRichLabel & label = showMe->pLabelArea[i];
      cout << "Label nr " << i + 1 << " (" << label.iSignature
           << ") | left = " << label.iLeftX << " | right = "
           << label.iRightX << " (dx = "
           << label.iRightX - label.iLeftX + 1 << ") | bottom = "
           << label.iLowerY << " | top = " << label.iUpperY
           << " (dy = "
           << label.iUpperY - label.iLowerY + 1 << ") | fired = "
           << label.iFiredPadsNr << "\n";
   }
#endif

   if (pAreaData) {
      delete [] pAreaData;
      pAreaData = NULL;

#ifdef HRICH_DEBUGMODE0
      cout << "RICH DEBUG MODE: pointer \'pAreaData\' (type \'class "
           "HRichLabel\') deleted.\n";
#endif
   }

   return (HRichLabel::iLabelNr);

} // eof LabelPads
//============================================================================

//----------------------------------------------------------------------------
Int_t HRichPadLabel::Execute(HRichAnalysis *giveMe)
{

   Int_t iLabelNr = 0;

   if (giveMe->iPadFiredNr - giveMe->iPadCleanedNr > 2) { //no ring otherwise
      HRichAnalysisPar *analParams = giveMe->getAnalysisPar();

      if (analParams->isActiveLabelPads)
         iLabelNr = LabelPads(giveMe, analParams->iLabeledPadsDistance);
   }
   return iLabelNr;
}
//============================================================================







