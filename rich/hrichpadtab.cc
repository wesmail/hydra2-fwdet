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
//  HRichPadTab
//
//  These class contains definition of array of pads.
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hades.h"
#include "hparamlist.h"
#include "hrichpad.h"
#include "hrichpadcorner.h"
#include "hrichpadtab.h"
#include "hspectrometer.h"

#include <iostream>

using namespace std;

ClassImp(HRichPadTab)

HRichPadTab::HRichPadTab()
   : TObject()
{
   clear();
   fPadsArray = NULL;
   dummyInitialisation();
}

HRichPadTab::~HRichPadTab()
{
   deletePads();
}

void
HRichPadTab::clear()
{
   fActivePadsNr         = 0;
   fPadsLongestRow       = 0;
   fMiddlePad            = 0;
   fPadsLongestRowMiddle = 0.;
}

void
HRichPadTab::deletePads()
{
   if (NULL != fPadsArray) {
      fPadsArray->Delete();
      delete fPadsArray;
      fPadsArray = NULL;
   }
   clear();
}

Bool_t
HRichPadTab::getParams(HParamList* l)
{
   if (NULL == l) return kFALSE;
   if (!l->fill("fPadParams", &fPadParams)) return kFALSE;

   if (kFALSE == initialisePads()) return kFALSE;
   return calculatePadParameters();
}

void
HRichPadTab::printParams()
{
   cout << "HRichPadTab" << endl;
   cout << "==========================================" << endl;
   cout << "fPadParams              " << endl;
   for (Int_t i = 0; i < fPadParams.GetSize(); i += 11) {
      cout << "  ";
      for (Int_t j = 0; j < 11; ++j) {
         cout << fPadParams[i+j] << " ";
      }
      cout << endl;
   }
   cout << endl;
}

void
HRichPadTab::printParamsFull()
{
   HRichPad*       pPad    = NULL;

   if (fPadsArray->GetSize() <  getActivePadsNr()) {
      Error("printParams", "Inconsistency in number of pads");
      return;
   }

   cout << "HRichPadTab" << endl;
   cout << "==========================================" << endl;
   cout << "  fActivePadsNr.......... " << fActivePadsNr         << endl;
   cout << "  fPadsLongestRow........ " << fPadsLongestRow       << endl;
   cout << "  fMiddlePad............. " << fMiddlePad            << endl;
   cout << "  fPadsLongestRowMiddle.. " << fPadsLongestRowMiddle << endl;
   cout << "==========================================" << endl;
   cout << "fPadParams              " << endl;
   for (Int_t i = 0; i < fPadsArray->GetSize(); ++i) {
      pPad = static_cast<HRichPad*>(fPadsArray->At(i));
      if (kTRUE == pPad->getPadActive()) {
         cout << "  " << (pPad->getPadX() * 100 + pPad->getPadY()) << " ";
         cout << pPad->getTheta()   << " ";
         cout << pPad->getPhi(0)    << " ";
         cout << pPad->getPadFlag() << " ";
         cout << pPad->getXmin()    << " ";
         cout << pPad->getYmin()    << " ";
         cout << pPad->getXmax()    << " ";
         cout << pPad->getYmax()    << " ";
         if (4 != pPad->getCornersNr()) {
            Error("printParams", "Inconsistency in number of pad corners");
            continue;
         }
         for (Int_t j = 0; j < 4; ++j) {
            cout << pPad->getCorner(j)->getX() << " ";
            cout << pPad->getCorner(j)->getY() << " ";
            cout << pPad->getCorner(j)->getAreaFlag() << " ";
         }
         cout << "\\" << endl;
      }
   }
   cout << endl;
}

void
HRichPadTab::putParams(HParamList* l)
{
   if (NULL == l) return;
   l->add("fPadParams", fPadParams);
}

Int_t
HRichPadTab::createPads()
{
   deletePads();
   fPadsArray = new TClonesArray("HRichPad", (RICH_MAX_COLS * RICH_MAX_ROWS));
   return (fPadsArray) ? (RICH_MAX_COLS * RICH_MAX_ROWS) : 0;
}

Bool_t
HRichPadTab::dummyInitialisation()
{
// this is dummy initialisation of all pads structure

   UInt_t ret = 0;

   if ((RICH_MAX_COLS * RICH_MAX_ROWS) != (ret = createPads())) {
      Error("dummyInitialisation", "Pointer to fPadsArray is NULL or has wrong size (%i instead of %i).",
            ret, (RICH_MAX_COLS * RICH_MAX_ROWS));
      return kFALSE;
   }

   for (UInt_t i = 0; i < (RICH_MAX_COLS * RICH_MAX_ROWS); ++i) {
      HRichPad *pad = new HRichPad;
      if (NULL == pad) {
         Error("dummyInitialisation", "Pointer to a new pad is NULL");
         return kFALSE;
      }
      pad->setPadNr(i);
      pad->CalcNrtoXY(RICH_MAX_COLS);
      pad->setPadActive(kFALSE);
      pad->setPadFlag(0);
      pad->setAmplitFraction(0.);
      for (Int_t j = 0; j < 6; ++j) {
         pad->setPhi(j, 0.0);
      }
      pad->setTheta(0.0);
      setPad(pad, i);
      delete pad;
   }

   return kTRUE;
}

Bool_t
HRichPadTab::initialisePads()
{
// fPadParams: array with size (numberOfPads * 11) in sector 1
//               11 numbers for each pad:
//                 1:    pad_pos_id = colNumber*100 + rowNumber
//                 2:    theta
//                 3:    phi (in sector 1)
//                 4..11 x and y of the 4 pad corners
//               theta and the pad corners are identical in each sector
//               for phi the sector rotation (relative to sector 1) must be added
//-------------------------------------------------------------------------------

   UInt_t address = 0;

   for (Int_t i = 0; i < fPadParams.GetSize() / 11; ++i) {
      address = static_cast<UInt_t>(fPadParams[11*i+0]);
      HRichPad* pad = getPad(static_cast<UInt_t>(address / 100), address % 100);
      if (NULL == pad) {
         Error("initialisePads", "Pointer to the pad is NULL");
         return kFALSE;
      }
      pad->setPadActive(1);
      fActivePadsNr++;
      pad->setTheta(fPadParams[11*i+1]);
      for (Int_t j = 0; j < 6; ++j) {
         pad->setPhi(j, 60 * j + fPadParams[11*i+2]);
         if (pad->getPhi(j) > 360) {
            pad->setPhi(j, pad->getPhi(j) - 360);
         }
      }
      for (Int_t j = 0; j < 4; ++j) {
         HRichPadCorner* padcorner = new HRichPadCorner;
         padcorner->setX(fPadParams[11*i+3+2*j]);
         padcorner->setY(fPadParams[11*i+4+2*j]);
         padcorner->setCornerNr(j);
         pad->addCorner(padcorner);
      }
   } // eof reading data

   return kTRUE;
}

void
HRichPadTab::setPad(HRichPad* pPad, UInt_t col, UInt_t row)
{
   setPad(pPad, calcAddr(col, row));
}

void
HRichPadTab::setPad(HRichPad* pPad, UInt_t padNr)
{
   HRichPad* pNewPad = NULL;

   pNewPad = static_cast<HRichPad*>(getSlot(padNr));
   pNewPad = new(pNewPad) HRichPad;
   *pNewPad = *pPad;
}

HRichPad*
HRichPadTab::getPad(UInt_t col, UInt_t row)
{
   if (col >= RICH_MAX_COLS)
      return NULL;

   if (row >= RICH_MAX_ROWS)
      return NULL;

   return getPad(calcAddr(col, row));
}

HRichPad*
HRichPadTab::getPad(Float_t Xpos, Float_t Ypos)
{

   UInt_t i        = 0;
   UInt_t j        = 0;
   UInt_t nPadAddr = 0;
   UInt_t nHit     = 0;

   if (Xpos < fPadsLongestRowMiddle) {
      for (nPadAddr = fPadsLongestRow * RICH_MAX_COLS;
           nPadAddr < fMiddlePad;
           nPadAddr++) {
         if (getPad(nPadAddr)->getPadActive() == kTRUE) {
            if (!getPad(nPadAddr)->isOutX(Xpos)) {
               nHit = 1;
               break;
            }
         }
      }
   } else {
      for (nPadAddr = fMiddlePad;
           nPadAddr < fPadsLongestRow * RICH_MAX_COLS + RICH_MAX_COLS;
           nPadAddr++) {
         if (getPad(nPadAddr)->getPadActive() == kTRUE) {
            if (!getPad(nPadAddr)->isOutX(Xpos)) {
               nHit = 1;
               break;
            }
         }
      }
   }

   if (nHit == 0) {
      return NULL;
   }

   nHit = 0;
   i = nPadAddr % RICH_MAX_COLS;
   for (j = 0; j < RICH_MAX_ROWS; j++) {
      if (NULL == getPad(i, j)) {
         Error("getPad", "Pad not found");
         continue;
      }
      if (getPad(i, j)->getPadActive() == kTRUE) {
         if (!getPad(i, j)->isOut(Xpos, Ypos)) {
            nHit = 1;
            break;
         }
      }
   }

   if (nHit == 0) {
      return NULL;
   }

   return getPad(i, j);
}

Bool_t
HRichPadTab::isOut(UInt_t X, UInt_t Y)
{
   if (X >= RICH_MAX_COLS) return kTRUE;
   if (Y >= RICH_MAX_ROWS) return kTRUE;
   return kFALSE;
}

TObject*&
HRichPadTab::getSlot(Int_t nIndx)
{
   return (fPadsArray->operator[](nIndx));
}

Bool_t
HRichPadTab::calculatePadParameters()
{
// This function calculate for each pad:
//   - sets minimum and maximum (in x and y) for given pad
//   - pad flag (if analytical formula can be used during digitisation)
//   - flag area
// and finally calls the calculation of pad centres

   Bool_t activated            = kFALSE;
   Bool_t analyticDigitisation = kFALSE;

   Int_t rowLen = 0;
   Int_t rowMax = 0;
   Int_t left   = -1;
   Int_t right  = -1;

   Float_t xcor = 0.;
   Float_t ycor = 0.;
   Float_t xmin = 0.;
   Float_t ymin = 0.;
   Float_t xmax = 0.;
   Float_t ymax = 0.;
   Float_t min  = 0.;
   Float_t max  = 0.;

   HRichPad*        pPad    = NULL;
   HRichPadCorner*  pCorner = NULL;

   Float_t x1, x2, y1, y2, a, b;
   Int_t n1, n2, area0, area1;

// Loop over all pads
   for (Int_t i = 0; i < (RICH_MAX_COLS * RICH_MAX_ROWS); ++i) {

      analyticDigitisation = kTRUE;
      pPad = getPad(i);

      // Loop over pad corners and sets minimum and maximum (in x and y) for given pad...
      for (Int_t j = 0; j < pPad->getCornersNr(); ++j) {
         pCorner = pPad->getCorner(j);
         xcor = pCorner->getX();
         ycor = pCorner->getY();
         if (j == 0) {
            xmin = xmax = xcor;
            ymin = ymax = ycor;
         }
         if (xcor < xmin) xmin = xcor;
         if (xcor > xmax) xmax = xcor;
         if (ycor < ymin) ymin = ycor;
         if (ycor > ymax) ymax = ycor;
      }

      pPad->setXmin(xmin);
      pPad->setYmin(ymin);
      pPad->setXmax(xmax);
      pPad->setYmax(ymax);


      // ...and checks if analytical formula can be used during digitisation
      if (pPad->getCornersNr() != 4) {
         analyticDigitisation = kFALSE;
      } else {
         for (Int_t j = 0; j < pPad->getCornersNr(); ++j) {
            pPad->getCorner(j)->getXY(&x1, &y1);
            if (j + 1 < pPad->getCornersNr()) {
               pPad->getCorner(j + 1)->getXY(&x2, &y2);
            } else {
               pPad->getCorner(0)->getXY(&x2, &y2);
            }
            if (x1 != x2 && y1 != y2) {
               analyticDigitisation = kFALSE;
            }
         }
      }

      if (kTRUE == analyticDigitisation) {
         pPad->setPadFlag(1);
      } else {
         pPad->setPadFlag(2);
      }


// Calculation of flag area
      for (Int_t j = 0; j < pPad->getCornersNr(); ++j) {
         pPad->getCorner(j)->getXY(&x1, &y1);
         n1 = pPad->getCorner(j)->getCornerNr();
         if (j + 1 < pPad->getCornersNr()) {
            pPad->getCorner(j + 1)->getXY(&x2, &y2);
            n2 = pPad->getCorner(j + 1)->getCornerNr();
         } else {
            pPad->getCorner(0)->getXY(&x2, &y2);
            n2 = pPad->getCorner(0)->getCornerNr();
         }

         area0 = 0;
         area1 = 0;
         if (x1 == x2) {
            for (Int_t k = 0; k < pPad->getCornersNr(); ++k) {
               if (pPad->getCorner(k)->getCornerNr() != n1 &&
                   pPad->getCorner(k)->getCornerNr() != n2) {
                  if (pPad->getCorner(k)->getX() > x1) area1++;
                  if (pPad->getCorner(k)->getX() < x1) area0++;
               }
            }
            if (area1 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(1);
            } else if (area0 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(0);
            } else {
               Error("calculatePadParameters", "Inconsistency in pads corners coordinates!");
               return kFALSE;;
            }
         } else if (y1 == y2) {
            for (Int_t k = 0; k < pPad->getCornersNr(); k++) {
               if (pPad->getCorner(k)->getCornerNr() != n1 &&
                   pPad->getCorner(k)->getCornerNr() != n2) {
                  if (pPad->getCorner(k)->getY() > y1) area1++;
                  if (pPad->getCorner(k)->getY() < y1) area0++;
               }
            }
            if (area1 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(1);
            } else if (area0 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(0);
            } else {
               Error("calculatePadParameters", "Inconsistency in pads corners coordinates!");
               return kFALSE;
            }
         } else {
            a = (y2 - y1) / (x2 - x1);
            b = (x2 * y1 - x1 * y2) / (x2 - x1);

            for (Int_t k = 0; k < pPad->getCornersNr(); ++k) {
               if (pPad->getCorner(k)->getCornerNr() != n1 &&
                   pPad->getCorner(k)->getCornerNr() != n2) {
                  if (pPad->getCorner(k)->getY() > a * pPad->getCorner(k)->getX() + b) area1++;
                  if (pPad->getCorner(k)->getY() < a * pPad->getCorner(k)->getX() + b) area0++;
               }
            }
            if (area1 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(1);
            } else if (area0 + 2 == pPad->getCornersNr()) {
               pPad->getCorner(j)->setAreaFlag(0);
            } else {
               Error("calculatePadParameters", "Inconsistency in pads corners coordinates!");
               return kFALSE;
            }
         }
      } // end of loop over all corners

      pPad->calcPadCenter();

   } // eof loop over all pads


   for (UInt_t j = 0; j < RICH_MAX_ROWS; ++j) {
      rowLen = 0;
      for (UInt_t i = 0; i < RICH_MAX_COLS; ++i) {
         if (getPad(i, j)->getPadActive()) {
            rowLen++;
         }
      }
      if (rowLen > rowMax) {
         rowMax = rowLen;
         fPadsLongestRow = j;
      }
   }

   for (Int_t i = 0; i < RICH_MAX_COLS; ++i) {
      if (getPad(i, fPadsLongestRow)->getPadActive()) {
         if (left < 0) {
            left = getPad(i, fPadsLongestRow)->getPadNr();
         }
         if (right < getPad(i, fPadsLongestRow)->getPadNr()) {
            right = getPad(i, fPadsLongestRow)->getPadNr();
         }
      }
   }

   fMiddlePad = left + (right - left) / 2 + 1;

   for (Int_t i = 0; i < RICH_MAX_COLS; ++i) {
      if (getPad(i, fPadsLongestRow)->getPadActive()) {
         if (activated == kFALSE) {
            min = getPad(i, fPadsLongestRow)->getXmin();
            max = getPad(i, fPadsLongestRow)->getXmax();
            activated = kTRUE;
         }
         if (min > getPad(i, fPadsLongestRow)->getXmin()) {
            min = getPad(i, fPadsLongestRow)->getXmin();
         }
         if (max < getPad(i, fPadsLongestRow)->getXmax()) {
            max = getPad(i, fPadsLongestRow)->getXmax();
         }
      }
   }

   fPadsLongestRowMiddle = (min + max) / 2.;

   return kTRUE;
}
