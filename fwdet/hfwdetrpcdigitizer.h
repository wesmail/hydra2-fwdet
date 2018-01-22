#ifndef HFWDETRPCDIGITIZER_H
#define HFWDETRPCDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "fwdetdef.h"

class HCategory;
class HFwDetRpcDigiPar;
class HFwDetRpcGeomPar;

class HFwDetRpcDigitizer : public HReconstructor
{
private:
    HCategory* pGeantFwDetCat;
    HCategory* pFwDetRpcCalCat;
    HFwDetRpcDigiPar* pFwDetRpcDigiPar;
    HFwDetRpcGeomPar* pFwDetRpcGeomPar;

    struct GeantFields
    {
        Int_t   trackNumber;    // geant track number
        Float_t xHit;           // geant x of hit  (in mm) in cell coord. system
        Float_t yHit;           // geant y of hit  (in mm) in cell coord. system
        Float_t zHit;           // geant z of hit  (in mm) in cell coord. system
        Float_t pxHit;          // geant x component of hit momentum (in MeV/c)
        Float_t pyHit;          // geant y component of hit momentum (in MeV/c)
        Float_t pzHit;          // geant z component of hit momentum (in MeV/c)
        Float_t tofHit;         // geant time of flight of hit (in ns)
        Float_t trackLength;    // geant track length (in mm)
        Float_t eHit;           // geant energy deposited (in MeV)
        Float_t lab_x;          // x, y, z lab coordinates
        Float_t lab_y;
        Float_t lab_z;
    };

    struct ClusterFields {
        Int_t m;
        Int_t l;
        Int_t s;
        Float_t tl;
        Float_t tr;
        Float_t ql;
        Float_t qr;
        Float_t tof;
        Int_t track;
        Float_t x;
        Float_t y;
    };

    Float_t rot[FWDET_RPC_MAX_MODULES];
    Float_t sina[FWDET_RPC_MAX_MODULES];
    Float_t cosa[FWDET_RPC_MAX_MODULES];
    Float_t offset[FWDET_RPC_MAX_MODULES][FWDET_RPC_MAX_LAYERS];

    HLocation fLoc;

public:
    HFwDetRpcDigitizer();
    HFwDetRpcDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetRpcDigitizer();
    void   initVariables();
    Bool_t init();
    Bool_t reinit();
    Int_t  execute();
    Bool_t finalize() { return kTRUE;}

    Int_t findStrip(Int_t m, Int_t l, Float_t x);

private:
    bool fillHit(const ClusterFields & cf);

    ClassDef(HFwDetRpcDigitizer, 0);
};

#endif  /*  !HFWDETRPCDIGITIZER_H */
