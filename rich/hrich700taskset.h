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


#ifndef HRICH700TASKSET_H
#define HRICH700TASKSET_H

#include "htaskset.h"

class HRich700TaskSet : public HTaskSet {

private:

    Bool_t fNoRingFinder; //

   void   parseArguments(TString s1);


public:

   HRich700TaskSet(void);
   HRich700TaskSet(const Text_t name[], const Text_t title[]);
   ~HRich700TaskSet(void);

   // function to be used for derived task sets (HRich700TaskSet, etc.)
   HTask* make(const Char_t* select = "", const Option_t* option = "");

   ClassDef(HRich700TaskSet, 1) // Set of RICH tasks
};

#endif /* !HRICH700TASKSET_H */
