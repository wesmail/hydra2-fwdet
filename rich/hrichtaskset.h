//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Dan Magestro
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichTaskSet
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHTASKSET_H
#define HRICHTASKSET_H

#include "htaskset.h"

class HRichTaskSet : public HTaskSet {

private:

   Bool_t fSkipEvent;  // Switch to control skipping event w/o found ring in HRichAnalysis task.
   // Can be controlled via option arguments in the make function.
   Bool_t fNoiseOn;    // Switch to control the noise simulation in HRichDigitizer task.
   // Can be controlled via option arguments in the make function.

private:

   void   parseArguments(TString s1);


public:

   HRichTaskSet(void);
   HRichTaskSet(const Text_t name[], const Text_t title[]);
   ~HRichTaskSet(void);

   // function to be used for derived task sets (HRichTaskSet, etc.)
   HTask* make(const Char_t* select = "", const Option_t* option = "");

   ClassDef(HRichTaskSet, 1) // Set of RICH tasks
};

#endif /* !HRICHTASKSET_H */
