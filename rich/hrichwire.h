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
//  HRichWire
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHWIRE_H
#define HRICHWIRE_H

#include "TObject.h"

// ****************************************************************************

class HRichWire : public TObject {

private:

   Int_t   fWireNr;    // Wire number
   Float_t fWireX;     // X-position of the wire

// ****************************************************************************

public:

   HRichWire();
   virtual ~HRichWire() {}

///////////////////////////////////////////////////////////////////
// GETTERS
   Float_t getWireX();
   Int_t getWireNr();

///////////////////////////////////////////////////////////////////
// SETTERS
   void setWireX(Float_t X);
   void setWireNr(Int_t nr);


   ClassDef(HRichWire, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Float_t HRichWire::getWireX()
{
   return fWireX;
}
inline Int_t HRichWire::getWireNr()
{
   return fWireNr;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichWire::setWireX(Float_t X)
{
   fWireX = X;
}
inline void HRichWire::setWireNr(Int_t nr)
{
   fWireNr = nr;
}

#endif // HRICHWIRE_H
