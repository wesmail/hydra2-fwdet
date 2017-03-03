#ifndef HFWDETTASKSET_H
#define HFWDETTASKSET_H

#include "htaskset.h"
#include "TString.h"

class HFwDetTaskSet : public HTaskSet
{
protected:
    Bool_t doStrawRaw;      // unpacker (exp)
    Bool_t doStrawCal;      // calibrater (exp), digitizer (sim)
    Bool_t doScinRaw;       // unpacker (exp)
    Bool_t doScinCal;       // calibrater (exp), digitizer (sim)
    Bool_t doRpcRaw;        // unpacker (exp)
    Bool_t doRpcCal;        // calibrater (exp), digitizer (sim)
    Bool_t doRpcHitF;
    Bool_t doVectorFinder;  // tracker (exp,sim)
    void parseArguments(TString s);

public:
    HFwDetTaskSet();
    HFwDetTaskSet(const Text_t name[], const Text_t title[]);
    virtual ~HFwDetTaskSet();
    HTask *make(const Char_t* select = "", const Option_t* option = "");

    ClassDef(HFwDetTaskSet, 0); // Set of tasks
};

#endif /* !HFWDETTASKSET_H */
