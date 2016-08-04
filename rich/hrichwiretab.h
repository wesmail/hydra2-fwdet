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
//  HRichWireTab
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHWIRETAB_H
#define HRICHWIRETAB_H

#include "TArrayF.h"
#include "TObjArray.h"

#include "hrichwire.h"

class HParamList;

class HRichWireTab : public TObject {

private:

   Int_t     fNrWires;      // Total number of wires
   Float_t   fDistWire;     // Distance between wires
   TObjArray fWiresArr;     // Array of wires

   TArrayF   fWirePosX;     // X-position of the wire filled from HParCond

// ****************************************************************************

public:

   HRichWireTab();
   virtual ~HRichWireTab();

   void   clear();
   void   printParams();
   void   putParams(HParamList* l);
   Bool_t getParams(HParamList* l);

   void   addWire(HRichWire* pWire) {
      fWiresArr.Add(pWire);
   }

///////////////////////////////////////////////////////////////////
// GETTERS
   Int_t getNrWires();
   Float_t getDistWire();
   HRichWire* getWire(Int_t n);

///////////////////////////////////////////////////////////////////
// SETTERS
   Int_t setWire(HRichWire* pWire, Int_t n);
   void setNrWires(Int_t n);
   void setDistWire(Float_t dist);

   ClassDef(HRichWireTab, 1)
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichWireTab::getNrWires()
{
   return fNrWires;
}
inline Float_t HRichWireTab::getDistWire()
{
   return fDistWire;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichWireTab::setNrWires(Int_t n)
{
   fNrWires = n;
}
inline void HRichWireTab::setDistWire(Float_t dist)
{
   fDistWire = dist;
}

#endif // HRICHWIRETAB_H
