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
//
//////////////////////////////////////////////////////////////////////////////


#ifndef RICHPAD_H
#define RICHPAD_H

#include "TObjArray.h"
#include "TObject.h"

class HRichPadCorner;

// ****************************************************************************
//============================================================================


// ****************************************************************************
class HRichPad : public TObject {

private:

   Int_t fCornersNr;            // Number of corners
   TObjArray fPadCornersArray;  // Array of pad corners

   Float_t fX;                  // X of a pad in mm
   Float_t fY;                  // Y of a pad in mm
   Int_t   fPadX;               // X of a pad in pads units
   Int_t   fPadY;               // Y of a pad in pads units
   Int_t   fPadNr;              // Pad number
   Int_t   fPadFlag;            // 0-total outside, 1-inside, 2-partial inside
   Float_t fAmplitFraction;     // fraction amplitude

   Float_t fXlab[6];            // X of a pad in LAB in sector 0..5
   Float_t fYlab[6];            // Y of a pad in LAB in sector 0..5
   Float_t fZlab[6];            // Z of a pad in LAB in sector 0..5
   Float_t fTheta;              // Theta of a pad
   Float_t fPhi[6];             // Phi of a pad in sector 0..5

   Float_t fXmin;               // Left border of a pad
   Float_t fYmin;               // Lower border of a pad
   Float_t fXmax;               // Right border of a pad
   Float_t fYmax;               // Upper border of a pad

   Bool_t fPadActive;           // Flag active pads: 0-not actove, 1-active


public:

   HRichPad();
   virtual ~HRichPad();
   HRichPad(const HRichPad& source);
   HRichPad& operator=(const HRichPad& source);

   void reset();
   void calcPadCenter();

   Bool_t isOut(Float_t x, Float_t y);
   Bool_t isOutX(Float_t x);
   Int_t addCorner(HRichPadCorner* corner);


///////////////////////////////////////////////////////////////////
// GETTERS
   HRichPadCorner* getCorner(Int_t n);
   Int_t getCornersNr();
   Float_t getXmin();
   Float_t getYmin();
   Float_t getXmax();
   Float_t getYmax();
   Float_t getX();
   Float_t getY();
   Int_t  getPadX();
   Int_t  getPadY();
   Int_t  getPadNr();
   void   getXY(Float_t *pX, Float_t *pY);
   void   getPadXY(Int_t *pX, Int_t *pY);
   Int_t  getPadFlag();
   Bool_t getPadActive();
   Float_t getXlab(const Int_t sec);
   Float_t getYlab(const Int_t sec);
   Float_t getZlab(const Int_t sec);
   Float_t getTheta();
   Float_t getPhi(const Int_t sec);
   void getXYZlab(const Int_t sec, Float_t *pX, Float_t *pY, Float_t *pZ);
   void getAngles(const Int_t sec, Float_t *pTheta, Float_t *pPhi);
   Float_t getAmplitFraction();


///////////////////////////////////////////////////////////////////
// SETTERS
   Int_t setCorner(HRichPadCorner* corner, Int_t n);
   void setXmin(Float_t xmin);
   void setYmin(Float_t ymin);
   void setXmax(Float_t xmax);
   void setYmax(Float_t ymax);
   void setX(const Float_t x);
   void setY(const Float_t y);
   void setPadX(Int_t padx);
   void setPadY(Int_t pady);
   void setPadNr(Int_t padnr);
   void setXY(const Float_t x, const Float_t y);
   void setPadXY(Int_t padx, Int_t pady);
   void setPadFlag(Int_t flag);
   void setPadActive(Bool_t active);
   void setAmplitFraction(Float_t ampl);
   void setXlab(const Int_t sec, const Float_t x);
   void setYlab(const Int_t sec, const Float_t y);
   void setZlab(const Int_t sec, const Float_t z);
   void setXYZlab(const Int_t sec, const Float_t x,
                  const Float_t y, const Float_t z);
   void setTheta(const Float_t thetha);
   void setPhi(const Int_t sec, const Float_t phi);

   void CalcNrtoXY(Int_t padsx);
   void CalcXYtoNr(Int_t padsx);

   ClassDef(HRichPad, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichPad::getCornersNr()
{
   return fCornersNr;
}
inline Float_t HRichPad::getXmin()
{
   return fXmin;
}
inline Float_t HRichPad::getYmin()
{
   return fYmin;
}
inline Float_t HRichPad::getXmax()
{
   return fXmax;
}
inline Float_t HRichPad::getYmax()
{
   return fYmax;
}
inline Float_t HRichPad::getX()
{
   return fX;
}
inline Float_t HRichPad::getY()
{
   return fY;
}
inline Int_t  HRichPad::getPadX()
{
   return fPadX;
}
inline Int_t  HRichPad::getPadY()
{
   return fPadY;
}
inline Int_t  HRichPad::getPadNr()
{
   return fPadNr;
}
inline Int_t  HRichPad::getPadFlag()
{
   return fPadFlag;
}
inline Bool_t HRichPad::getPadActive()
{
   return fPadActive;
}
inline Float_t HRichPad::getXlab(const Int_t sec)
{
   return fXlab[sec];
}
inline Float_t HRichPad::getYlab(const Int_t sec)
{
   return fYlab[sec];
}
inline Float_t HRichPad::getZlab(const Int_t sec)
{
   return fZlab[sec];
}
inline Float_t HRichPad::getTheta()
{
   return fTheta;
}
inline Float_t HRichPad::getPhi(const Int_t sec)
{
   return fPhi[sec];
}
inline Float_t HRichPad::getAmplitFraction()
{
   return fAmplitFraction;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichPad::setXmin(Float_t xmin)
{
   fXmin = xmin;
}
inline void HRichPad::setYmin(Float_t ymin)
{
   fYmin = ymin;
}
inline void HRichPad::setXmax(Float_t xmax)
{
   fXmax = xmax;
}
inline void HRichPad::setYmax(Float_t ymax)
{
   fYmax = ymax;
}
inline void HRichPad::setX(const Float_t x)
{
   fX = x;
}
inline void HRichPad::setY(const Float_t y)
{
   fY = y;
}
inline void HRichPad::setPadX(Int_t padx)
{
   fPadX = padx;
}
inline void HRichPad::setPadY(Int_t pady)
{
   fPadY = pady;
}
inline void HRichPad::setPadNr(Int_t padnr)
{
   fPadNr = padnr;
}
inline void HRichPad::setXY(const Float_t x, const Float_t y)
{
   fX = x;
   fY = y;
}
inline void HRichPad::setPadXY(Int_t padx, Int_t pady)
{
   fPadX = padx;
   fPadY = pady;
}
inline void HRichPad::setPadFlag(Int_t flag)
{
   fPadFlag = flag;
}
inline void HRichPad::setPadActive(Bool_t active)
{
   fPadActive = active;
}
inline void HRichPad::setAmplitFraction(Float_t ampl)
{
   fAmplitFraction = ampl;
}
inline void HRichPad::setXlab(const Int_t sec, const Float_t x)
{
   fXlab[sec] = x;
}
inline void HRichPad::setYlab(const Int_t sec, const Float_t y)
{
   fYlab[sec] = y;
}
inline void HRichPad::setZlab(const Int_t sec, const Float_t z)
{
   fZlab[sec] = z;
}
inline void HRichPad::setXYZlab(const Int_t sec, const Float_t x,
                                const Float_t y, const Float_t z)
{
   fXlab[sec] = x;
   fYlab[sec] = y;
   fZlab[sec] = z;
}
inline void HRichPad::setTheta(const Float_t thetha)
{
   fTheta = thetha;
}
inline void HRichPad::setPhi(const Int_t sec, const Float_t phi)
{
   fPhi[sec] = phi;
}

#endif // RICHPAD_H
