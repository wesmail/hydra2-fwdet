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
//  HRichPad
//
//  These classes contain definition of pads.
//
//////////////////////////////////////////////////////////////////////////////


#include "hades.h"
#include "hrichpad.h"
#include "hrichpadcorner.h"

using namespace std;

ClassImp(HRichPad)

HRichPad::HRichPad()
{
   fCornersNr = 0;
   fX = 0.;
   fY = 0.;
   fPadX = 0;
   fPadY = 0;
   fPadNr = 0;
   fXmin = 0.;
   fYmin = 0.;
   fXmax = 0.;
   fYmax = 0.;
   fPadFlag = 0;
   fAmplitFraction = 0.;
   fPadActive = kFALSE;

   fTheta = 0.;
   for (Int_t i = 0; i < 6; i++) {
      fXlab[i] = 0.;
      fYlab[i] = 0.;
      fZlab[i] = 0.;
      fPhi[i] = 0.;
   }
}

HRichPad::~HRichPad()
{
   reset();
}

HRichPad::HRichPad(const HRichPad& source):TObject(source)
{
   fCornersNr = 0; // this will be set to real number afterwards
   Int_t i;
   for (i = 0; i < source.fCornersNr; i++) {
      HRichPadCorner *pCor = new HRichPadCorner;
      *pCor = *((HRichPadCorner*)source.fPadCornersArray.At(i));
      addCorner(pCor);
   }

   fCornersNr = source.fCornersNr;
   fX = source.fX;
   fY = source.fY;
   fPadX = source.fPadX;
   fPadY = source.fPadY;
   fPadNr = source.fPadNr;
   fXmin = source.fXmin;
   fYmin = source.fYmin;
   fXmax = source.fXmax;
   fYmax = source.fYmax;
   fPadFlag = source.fPadFlag;
   fAmplitFraction = source.fAmplitFraction;
   fPadActive = source.fPadActive;
   fTheta = source.fTheta;
   for (Int_t i = 0; i < 6; i++) {
      fXlab[i] = source.fXlab[i];
      fYlab[i] = source.fYlab[i];
      fZlab[i] = source.fZlab[i];
      fPhi[i] = source.fPhi[i];
   }
}

HRichPad& HRichPad::operator=(const HRichPad& source)
{
   if (this != &source) {
      fX = source.fX;
      fY = source.fY;
      fPadX = source.fPadX;
      fPadY = source.fPadY;
      fPadNr = source.fPadNr;
      fXmin = source.fXmin;
      fYmin = source.fYmin;
      fXmax = source.fXmax;
      fYmax = source.fYmax;
      fPadFlag = source.fPadFlag;
      fAmplitFraction = source.fAmplitFraction;
      fPadActive = source.fPadActive;
      fTheta = source.fTheta;
      for (Int_t i = 0; i < 6; i++) {
         fXlab[i] = source.fXlab[i];
         fYlab[i] = source.fYlab[i];
         fZlab[i] = source.fZlab[i];
         fPhi[i] = source.fPhi[i];
      }
   }
   return *this;
}

void HRichPad::reset()
{
   fCornersNr = 0;
   fX = 0.;
   fY = 0.;
   fPadX = 0;
   fPadY = 0;
   fPadNr = 0;
   fPadFlag = 0;
   fAmplitFraction = 0.;
   fPadActive = kFALSE;
   fTheta = 0.;
   for (Int_t i = 0; i < 6; i++) {
      fXlab[i] = 0.;
      fYlab[i] = 0.;
      fZlab[i] = 0.;
      fPhi[i] = 0.;
   }
   if (fPadCornersArray.GetSize() > 0) {
      fPadCornersArray.Delete();
   }
}

void HRichPad::CalcNrtoXY(Int_t padsx)
{
   fPadX = fPadNr % padsx;
   fPadY = fPadNr / padsx;
}

void HRichPad::CalcXYtoNr(Int_t padsx)
{
   fPadNr = fPadX + padsx * fPadY;
}

void HRichPad::getXY(Float_t *pX, Float_t *pY)
{
   *pX = fX;
   *pY = fY;
}

void HRichPad::getPadXY(Int_t *pX, Int_t *pY)
{
   *pX = fPadX;
   *pY = fPadY;
}

void HRichPad::getXYZlab(const Int_t sec, Float_t *pX, Float_t *pY, Float_t *pZ)
{
   *pX = fXlab[sec];
   *pY = fYlab[sec];
   *pZ = fZlab[sec];
}

void HRichPad::getAngles(const Int_t sec, Float_t *pTheta, Float_t *pPhi)
{
   *pTheta = fTheta;
   *pPhi = fPhi[sec];
}


HRichPadCorner* HRichPad::getCorner(Int_t n)
{
   if (n < 0 || n >= fCornersNr) return NULL;
   return (HRichPadCorner*)fPadCornersArray.At(n);
}

Int_t HRichPad::addCorner(HRichPadCorner* corner)
{
   fPadCornersArray.Add(corner);
   return fCornersNr++;
}

Int_t HRichPad::setCorner(HRichPadCorner* corner, Int_t n)
{
   if (n < 0 || n >= fCornersNr) return 0;
   delete fPadCornersArray.At(n);
   fPadCornersArray.AddAt(corner, n);
   return 1;
}

void HRichPad::calcPadCenter()
{

   Int_t i;
   Float_t xcor = 0., ycor = 0., xsum = 0., ysum = 0.;

   for (i = 0; i < fCornersNr; i++) {
      getCorner(i)->getXY(&xcor, &ycor);
      xsum += xcor;
      ysum += ycor;
   }
   fX = xsum / fCornersNr;
   fY = ysum / fCornersNr;
}

Bool_t HRichPad::isOut(Float_t x, Float_t y)
{
   Int_t i, areaFlag1;
   Float_t xcor1, ycor1, xcor2, ycor2, a , b;

   for (i = 0; i < fCornersNr; i++) {

      getCorner(i)->getXY(&xcor1, &ycor1);
      areaFlag1 = getCorner(i)->getAreaFlag();
      if (i + 1 < fCornersNr) {
         getCorner(i + 1)->getXY(&xcor2, &ycor2);
      } else {
         getCorner(0)->getXY(&xcor2, &ycor2);
      }

      if (xcor1 == xcor2) {

         if (areaFlag1 > 0) {
            if (x < xcor1) return kTRUE;
         } else {
            if (x > xcor1) return kTRUE;
         }

      } else if (ycor1 == ycor2) {

         if (areaFlag1 > 0) {
            if (y < ycor1) return kTRUE;
         } else {
            if (y > ycor1) return kTRUE;
         }

      } else {

         a = (ycor2 - ycor1) / (xcor2 - xcor1);
         b = (xcor2 * ycor1 - xcor1 * ycor2) / (xcor2 - xcor1);

         if (areaFlag1 > 0) {
            if (y < a * x + b) return kTRUE;
         } else {
            if (y > a * x + b) return kTRUE;
         }

      }

   } // end of loop over all corners

   return kFALSE;
}

Bool_t HRichPad::isOutX(Float_t x)
{

   Int_t i, areaFlag1;
   Float_t xcor1, ycor1, xcor2, ycor2;

   for (i = 0; i < fCornersNr; i++) {

      getCorner(i)->getXY(&xcor1, &ycor1);
      if (i + 1 < fCornersNr) getCorner(i + 1)->getXY(&xcor2, &ycor2);
      else getCorner(0)->getXY(&xcor2, &ycor2);

      if (xcor1 == xcor2) {
         areaFlag1 = getCorner(i)->getAreaFlag();
         if (areaFlag1 > 0) {
            if (x < xcor1) return kTRUE;
         } else {
            if (x > xcor1) return kTRUE;
         }
      }

   } // end of loop over all corners

   return kFALSE;
}
