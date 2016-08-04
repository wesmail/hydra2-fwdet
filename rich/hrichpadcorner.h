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
//  HRichPadCorner
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef RICHPADCORNER_H
#define RICHPADCORNER_H

#include "TObject.h"

class HRichPadCorner : public TObject {

private:

   Float_t fXcor;         // X-coordinate in cm
   Float_t fYcor;         // Y-coordinate in cm
   Int_t   fCornerNr;     // Corner number
   Int_t   fAreaFlag;     //


public:

   HRichPadCorner();
   HRichPadCorner(Int_t nrcor, Float_t xcor, Float_t ycor, Int_t flag);
   virtual ~HRichPadCorner() {}
   HRichPadCorner(const HRichPadCorner& source);
   HRichPadCorner& operator=(const HRichPadCorner& source);

///////////////////////////////////////////////////////////////////
// GETTERS
   Float_t getX();
   Float_t getY();
   Int_t   getCornerNr();
   Int_t   getAreaFlag();
   void    getXY(Float_t *X, Float_t *Y);

///////////////////////////////////////////////////////////////////
// SETTERS
   void setX(Float_t X);
   void setY(Float_t Y);
   void setXY(Float_t X, Float_t Y);
   void setCornerNr(Int_t corner);
   void setAreaFlag(Int_t flag);


   ClassDef(HRichPadCorner, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Float_t HRichPadCorner::getX()
{
   return fXcor;
}
inline Float_t HRichPadCorner::getY()
{
   return fYcor;
}
inline void HRichPadCorner::getXY(Float_t *X, Float_t *Y)
{
   *X = fXcor;
   *Y = fYcor;
}
inline Int_t HRichPadCorner::getCornerNr()
{
   return fCornerNr;
}
inline Int_t HRichPadCorner::getAreaFlag()
{
   return fAreaFlag;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichPadCorner::setX(Float_t X)
{
   fXcor = X;
}
inline void HRichPadCorner::setY(Float_t Y)
{
   fYcor = Y;
}
inline void HRichPadCorner::setXY(Float_t X, Float_t Y)
{
   fXcor = X;
   fYcor = Y;
}
inline void HRichPadCorner::setCornerNr(Int_t corner)
{
   fCornerNr = corner;
}
inline void HRichPadCorner::setAreaFlag(Int_t flag)
{
   fAreaFlag = flag;
}

#endif // RICHPADCORNER_H
