//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichFrame
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hparamlist.h"
#include "hrichframe.h"
#include "hrichframecorner.h"

#include <iostream>
#include <cstdlib>

using namespace std;

ClassImp(HRichFrame)


HRichFrame::HRichFrame()
   : TObject()
{
   clear();
}


HRichFrame::~HRichFrame()
{
   clear();
}

void
HRichFrame::clear()
{
   fCornerNr = 0;
   fFrameArr.Delete();
}

Bool_t
HRichFrame::getParams(HParamList* l)
{
   HRichFrameCorner *pFrameCorner = NULL;

   if (!l) return kFALSE;
   if (!l->fill("fFrameCorners", &fFrameCorners)) return kFALSE;

// We have 2 items (X,Y,flag) per frame corner
   for (Int_t i = 0; i < fFrameCorners.GetSize() / 2; ++i) {
      pFrameCorner = new HRichFrameCorner;
      pFrameCorner->setX(fFrameCorners[2*i]);
      pFrameCorner->setY(fFrameCorners[2*i+1]);
      pFrameCorner->setCornerNr(i);
      addCorner(pFrameCorner);
   }

   return calculateFlagArea();
}

void
HRichFrame::printParams()
{
   if (fFrameArr.GetSize() < getCornerNr() || getCornerNr() != 7) {
      Error("printParams", "Inconsistency in number of frame corners (%d < %d)", fFrameArr.GetSize(), getCornerNr());
      return;
   }
   cout << "HRichFrame" << endl;
   cout << "==========================================" << endl;
   cout << "fFrameCorners        " << endl;
   for (Int_t i = 0; i < getCornerNr(); ++i) {
      cout << getCorner(i)->getX() << " "
           << getCorner(i)->getY() << " " << endl;
   }
   cout << endl;
}

void
HRichFrame::putParams(HParamList* l)
{
   if (!l) return;
   l->add("fFrameCorners", fFrameCorners);
}

HRichFrameCorner*
HRichFrame::getCorner(Int_t n)
{
   if ((n < 0) || (n >= getCornerNr())) {
      return NULL;
   }

   return static_cast<HRichFrameCorner*>(fFrameArr.At(n));
}


Int_t
HRichFrame::setCorner(HRichFrameCorner* pCorner, Int_t n)
{
   if ((n < 0) || (n >= getCornerNr())) {
      return 0;
   }

   delete fFrameArr.At(n);
   fFrameArr.AddAt(pCorner, n);

   return 1;
}

Int_t
HRichFrame::addCorner(HRichFrameCorner* pCorner)
{
   fFrameArr.Add(pCorner);
   return fCornerNr++;
}

Int_t
HRichFrame::isOut(Float_t x, Float_t y)
{

   Int_t nCornerNr = getCornerNr();

   if (nCornerNr <= 2) {
      Error("isOut", "Frame must have at least 3 corners!");
      return -1;
   }

#ifdef RICHDIGI_DEBUG3
   cout << "RICHDIGI: HRichFrame _isOut_ is now calculated \n";
#endif


   Float_t X0    = 0.;
   Float_t X1    = 0.;
   Float_t Y0    = 0.;
   Float_t Y1    = 0.;
   Float_t a     = 0.;
   Float_t b     = 0.;
   Int_t   nFlag = 0;

   for (Int_t i = 0; i < nCornerNr; i++) {
      getCorner(i)->getXY(&X0, &Y0);
      nFlag = getCorner(i)->getFlagArea();
      if (i + 1 < nCornerNr) {
      	getCorner(i + 1)->getXY(&X1, &Y1);
      } else {
      	getCorner(0)->getXY(&X1, &Y1);
      }

      if (X0 == X1) {
         if (nFlag == 0 && x > X0) return 1;
         else if (nFlag == 1 && x < X0) return 1;
      } else if (Y0 == Y1) {
         if (nFlag == 0 && y > Y0) return 1;
         else if (nFlag == 1 && y < Y0) return 1;
      } else {
         a = (Y1 - Y0) / (X1 - X0);
         b = (X1 * Y0 - X0 * Y1) / (X1 - X0);
         if (nFlag == 0 && y > (a * x + b)) return 1;
         else if (nFlag == 1 && y < (a * x + b)) return 1;
      }
   }

   return 0;
}

Bool_t
HRichFrame::calculateFlagArea()
{
// calculation of the flag area

  Int_t fArea0;
  Int_t fArea1;
  Int_t nrcor1;
  Int_t nrcor2;
  Float_t a;
  Float_t b;
  Float_t xcor1;
  Float_t xcor2;
  Float_t ycor1;
  Float_t ycor2;

   for (Int_t i = 0; i < getCornerNr(); i++) {
      getCorner(i)->getXY(&xcor1, &ycor1);
      nrcor1 = getCorner(i)->getCornerNr();
      if (i + 1 < getCornerNr()) {
         getCorner(i + 1)->getXY(&xcor2, &ycor2);
         nrcor2 = getCorner(i + 1)->getCornerNr();
      } else {
         getCorner(0)->getXY(&xcor2, &ycor2);
         nrcor2 = getCorner(0)->getCornerNr();
      }

      fArea0 = 0;
      fArea1 = 0;
      if (xcor1 == xcor2) {
         for (Int_t j = 0; j < getCornerNr(); j++) {
            if (getCorner(j)->getCornerNr() != nrcor1 &&
                getCorner(j)->getCornerNr() != nrcor2) {
               if (getCorner(j)->getX() > xcor1) fArea1++;
               if (getCorner(j)->getX() < xcor1) fArea0++;
            }
         }
         if (fArea1 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(1);
         } else if (fArea0 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(0);
         } else {
            Error("calculateFlagArea", "Inconsistency in frame corners coordinates.");
            return kFALSE;
         }

      } else if (ycor1 == ycor2) {
         for (Int_t j = 0; j < getCornerNr(); j++) {
            if (getCorner(j)->getCornerNr() != nrcor1 &&
                getCorner(j)->getCornerNr() != nrcor2) {
               if (getCorner(j)->getY() > ycor1) fArea1++;
               if (getCorner(j)->getY() < ycor1) fArea0++;
            }
         }
         if (fArea1 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(1);
         } else if (fArea0 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(0);
         } else {
            Error("calculateFlagArea", "Inconsistency in frame corners coordinates.");
            return kFALSE;
         }

      } else {
         a = (ycor2 - ycor1) / (xcor2 - xcor1);
         b = (xcor2 * ycor1 - xcor1 * ycor2) / (xcor2 - xcor1);
         for (Int_t j = 0; j < getCornerNr(); j++) {
            if (getCorner(j)->getCornerNr() != nrcor1 &&
                getCorner(j)->getCornerNr() != nrcor2) {
               if (getCorner(j)->getY() > a * getCorner(j)->getX() + b) fArea1++;
               if (getCorner(j)->getY() < a * getCorner(j)->getX() + b) fArea0++;
            }
         }
         if (fArea1 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(1);
         } else if (fArea0 + 2 == getCornerNr()) {
            getCorner(i)->setFlagArea(0);
         } else {
            Error("calculateFlagArea", "Inconsistency in frame corners coordinates.");
            return kFALSE;
         }
      }
   }

   return kTRUE;
}
