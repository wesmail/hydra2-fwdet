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
//  These classes contain definition of corners.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichpadcorner.h"

using namespace std;

ClassImp(HRichPadCorner)

HRichPadCorner::HRichPadCorner()
{
   fXcor     = 0.0;
   fYcor     = 0.0;
   fCornerNr = 0;
   fAreaFlag = 0;
}

HRichPadCorner::HRichPadCorner(Int_t nrcor,
                               Float_t xcor,
                               Float_t ycor,
                               Int_t flag)
{
   fXcor     = xcor;
   fYcor     = ycor;
   fCornerNr = nrcor;
   fAreaFlag = flag;
}

HRichPadCorner::HRichPadCorner(const HRichPadCorner& source) :TObject(source)
{
   fXcor     = source.fXcor;
   fYcor     = source.fYcor;
   fCornerNr = source.fCornerNr;
   fAreaFlag = source.fAreaFlag;
}

HRichPadCorner& HRichPadCorner::operator=(const HRichPadCorner& source)
{
   if (this != &source) {
      fXcor     = source.fXcor;
      fYcor     = source.fYcor;
      fCornerNr = source.fCornerNr;
      fAreaFlag = source.fAreaFlag;
   }
   return *this;
}
