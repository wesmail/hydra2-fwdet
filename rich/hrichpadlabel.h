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
//  HRichPadLabel
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHPADLABEL_H
#define HRICHPADLABEL_H

#include "TArrayS.h"
#include "TObject.h"

#include "hrichanalysis.h"

// ****************************************************************************

class HRichPadLabel: public TObject {

// ****************************************************************************

private:

   Int_t iGraphArray;
   TArrayS iLabelGraphConnection;
   HRichPadLabel(const HRichPadLabel& source);

public:

   HRichPadLabel();
   HRichPadLabel(Int_t padsx, Int_t padsy);
   virtual ~HRichPadLabel();

   HRichPadLabel& operator=(const HRichPadLabel& source);

   Int_t LabelPads(HRichAnalysis *showMe, Int_t padDistance);
   Int_t Execute(HRichAnalysis *giveMe);
   void Reset();


   ClassDef(HRichPadLabel, 1)
};

//============================================================================


#endif // HRICHPADLABEL_H
