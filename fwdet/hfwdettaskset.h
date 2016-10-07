#ifndef HFWDETTASKSET_H
#define HFWDETTASKSET_H

#include "htaskset.h"
#include "TString.h"

class HFwDetTaskSet : public HTaskSet
{
protected:
    Bool_t doStraw;
    Bool_t doScin;
    Bool_t doRpc;
    void parseArguments(TString s);

public:
    HFwDetTaskSet();
    HFwDetTaskSet(const Text_t name[], const Text_t title[]);
    virtual ~HFwDetTaskSet();
    HTask *make(const Char_t* select = "", const Option_t* option = "");

    ClassDef(HFwDetTaskSet, 0); // Set of tasks
};

#endif /* !HFWDETTASKSET_H */
