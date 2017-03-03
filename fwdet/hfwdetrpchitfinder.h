#ifndef HFWDETRPCHITFINDER_H
#define HFWDETRPCHITFINDER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "fwdetdef.h"

class HCategory;
class HFwDetRpcHitFinderPar;
class HFwDetRpcGeomPar;

class HFwDetRpcHitFinder : public HReconstructor
{
private:
    HCategory* pFwDetRpcCalCat;
    HCategory* pFwDetRpcHitCat;
    HFwDetRpcHitFinderPar* pFwDetRpcHitFinderPar;
    HFwDetRpcGeomPar* pFwDetRpcGeomPar;

    Char_t  module;     // FwDet RPC module number (0...1)
    Char_t  layer;      // FwDet RPC layer number (0...1)
    Char_t  strip;      // FwDet RPC layer number (0...31)

    HLocation fLoc;

    Float_t fMatchRadius;
    Float_t fMatchTime;
    Float_t mod_z[FWDET_RPC_MAX_MODULES];
    Float_t avg_z;

public:
    HFwDetRpcHitFinder();
    HFwDetRpcHitFinder(const Text_t* name, const Text_t* title);
    virtual ~HFwDetRpcHitFinder();
    void   initVariables();
    Bool_t init();
    Bool_t reinit();
    Int_t  execute();
    Bool_t finalize() { return kTRUE;}

    bool fillHit(Float_t x, Float_t y, Float_t z, Float_t tof, Int_t track);

    ClassDef(HFwDetRpcHitFinder, 0);
};

#endif  /*  !HFWDETRPCHITFINDER_H */
