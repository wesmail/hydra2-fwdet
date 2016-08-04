#ifndef HWALLTASKSET_H
#define HWALLTASKSET_H

#include "htaskset.h"
#include "TString.h"

class HWallTaskSet : public HTaskSet {

    void parseArguments(TString s1);
    Bool_t  fdoEventPlane;   // run eventplane reconstructor
public:

    HWallTaskSet(const Text_t name[]="walltaskset",const Text_t title[]="walltaskset");
    ~HWallTaskSet(void);
    HTask*  make(const Char_t* select="",const Option_t* option="");
    ClassDef(HWallTaskSet,1) // Set of tasks for the Forward Wall
};

#endif /* !HWALLTASKSET_H */
