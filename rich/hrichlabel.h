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
//  HRichLabel
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHLABEL_H
#define HRICHLABEL_H

#include "TObject.h"

class HRichLabel: public TObject {

public:

   Char_t iLeftX;
   Char_t iRightX;
   Char_t iLowerY;
   Char_t iUpperY;

   Int_t iSignature;
   Int_t iFiredPadsNr;
   Int_t iLabeledPadsNr;
   static Int_t iLabelNr;

   HRichLabel();
   virtual ~HRichLabel() {}

   HRichLabel(const HRichLabel& source);
   HRichLabel& operator=(const HRichLabel& source);

   void Reset();

   ClassDef(HRichLabel, 1)
};


#endif // HRICHLABEL_H

