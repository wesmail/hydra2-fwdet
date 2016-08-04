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
//  HRichFrameCorner
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHFRAMECORNER_H
#define HRICHFRAMECORNER_H

#include "TObject.h"

class HRichFrameCorner: public TObject {

private:

   Float_t fXcor;        // X-coordinate of a frame corner
   Float_t fYcor;        // Y-coordinate of a frame corner
   Int_t   fCornerNr;    // Frame corner number
   Int_t   fFlagArea;    // Flag

public:

   HRichFrameCorner();
   ~HRichFrameCorner() {}

///////////////////////////////////////////////////////////////////
// GETTERS
   Float_t getX();
   Float_t getY();
   void getXY(Float_t *fX, Float_t *fY);
   Int_t getCornerNr();
   Int_t getFlagArea();

///////////////////////////////////////////////////////////////////
// SETTERS
   void setX(Float_t fX);
   void setY(Float_t fY);
   void setXY(Float_t fX, Float_t fY);
   void setCornerNr(Int_t corner);
   void setFlagArea(Int_t nFlag);

   ClassDef(HRichFrameCorner, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Float_t HRichFrameCorner::getX()
{
   return fXcor;
}
inline Float_t HRichFrameCorner::getY()
{
   return fYcor;
}
inline void HRichFrameCorner::getXY(Float_t *fX, Float_t *fY)
{
   *fX = fXcor;
   *fY = fYcor;
}
inline Int_t HRichFrameCorner::getCornerNr()
{
   return fCornerNr;
}
inline Int_t HRichFrameCorner::getFlagArea()
{
   return fFlagArea;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichFrameCorner::setX(Float_t fX)
{
   fXcor = fX;
}
inline void HRichFrameCorner::setY(Float_t fY)
{
   fYcor = fY;
}
inline void HRichFrameCorner::setXY(Float_t fX, Float_t fY)
{
   fXcor = fX;
   fYcor = fY;
}
inline void HRichFrameCorner::setCornerNr(Int_t corner)
{
   fCornerNr = corner;
}
inline void HRichFrameCorner::setFlagArea(Int_t nFlag)
{
   fFlagArea = nFlag;
}

#endif // HRICHFRAMECORNER_H
