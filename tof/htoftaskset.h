#ifndef HTOFTASKSET_H
#define HTOFTASKSET_H

#include "htaskset.h"
#include "TString.h"

class HTofTaskSet : public HTaskSet {
protected:
    Int_t  fhitfinderVersion;   //! switch version 1 (old) and 2 new  (default)
    Bool_t fdoCluster;          //! run cluster finder , default kTRUE
    void parseArguments(TString);
public:
  HTofTaskSet(void);
  HTofTaskSet(const Text_t name[],const Text_t title[]);
  ~HTofTaskSet(void);

 // function to be used for derived task sets (HTofTaskSet, etc.)
  HTask *make(const Char_t *select="",const Option_t *option="");

  ClassDef(HTofTaskSet,1) // Set of tasks
};

#endif /* !HTOFTASKSET_H */
