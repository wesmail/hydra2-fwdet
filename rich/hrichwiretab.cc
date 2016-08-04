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
//  Set of wires parameters.
//
//////////////////////////////////////////////////////////////////////////////


#include "hparamlist.h"
#include "hrichwiretab.h"

#include <iostream>
using namespace std;

ClassImp(HRichWireTab)

HRichWireTab::HRichWireTab()
   : TObject()
{
   clear();
}

HRichWireTab::~HRichWireTab()
{
   clear();
}

void
HRichWireTab::clear()
{
   fDistWire = 0.0;
   fNrWires   = 0;
   fWiresArr.Delete();
}

Bool_t
HRichWireTab::getParams(HParamList* l)
{
   HRichWire* pWire = NULL;

   if (NULL == l) return kFALSE;
   if (kFALSE == l->fill("fDistWire", &fDistWire)) return kFALSE;
   if (kFALSE == l->fill("fNrWires",  &fNrWires))  return kFALSE;
   if (kFALSE == l->fill("fWirePosX", &fWirePosX)) return kFALSE;

   for (Int_t i = 0; i < fNrWires; i++) {
      pWire = new HRichWire;
      if (NULL != pWire) {
         pWire->setWireNr(i);
         pWire->setWireX(fWirePosX[i]);
         addWire(pWire);
      } else {
         Error("getParams", "Pointer to HRichWire == NULL...");
         return kFALSE;
      }
   }

   return kTRUE;
}

void
HRichWireTab::printParams()
{
   cout << "HRichWireTab" << endl;
   cout << "==========================================" << endl;
   cout << "fDistWire             " << fDistWire << endl;
   cout << "fNrWires              " << fNrWires << endl;
   cout << "fWirePosX             " << endl;
   for (Int_t i = 0; i < fNrWires; ++i) {
      cout << fWirePosX[i] << " ";
      if (9 == (i % 10)) {
         cout << endl;
      }
   }
   cout << endl << endl;
}

void
HRichWireTab::putParams(HParamList* l)
{
   if (NULL == l) return;
   l->add("fDistWire", fDistWire);
   l->add("fNrWires",  fNrWires);
   l->add("fWirePosX", fWirePosX);
}

HRichWire*
HRichWireTab::getWire(Int_t n)
{
   if ((n < 0) || (n >= fNrWires)) {
      return NULL;
   }

   return static_cast<HRichWire*>(fWiresArr.At(n));
}

Int_t
HRichWireTab::setWire(HRichWire* pWire, Int_t n)
{
   if ((n < 0) || (n >= fNrWires)) {
      return 0;
   }
   delete fWiresArr.At(n);
   fWiresArr.AddAt(pWire, n);

   return 1;
}
