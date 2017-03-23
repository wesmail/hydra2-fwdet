#ifndef HFWDETRPCDIGITIZER_H
#define HFWDETRPCDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "fwdetdef.h"

class HCategory;
class HFwDetRpcDigiPar;
class HFwDetRpcGeomPar;

struct RpcTrackHits {
    Int_t hits_num;
    Int_t mod;
    Int_t lay;
    Int_t cell[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Float_t x[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Float_t y[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Int_t strip[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Float_t tof[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Float_t lab_x[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Float_t lab_y[FWDET_RPC_MAX_CELLS*FWDET_RPC_MAX_SUBCELLS];
    Int_t track;

    void print() const
    {
        printf("  mod=%d  lay=%d  hits=%d\n", mod, lay, hits_num);
        for (int i= 0; i < hits_num; ++i)
        {
            printf("     c=%d  x=%f  y=%f  tof=%f  strip=%d   lab=%f,%f\n",
                   cell[i], x[i], y[i], tof[i], strip[i], lab_x[i], lab_y[i]);
        }
    }
};

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
    };

//     Int_t   trackNumber;  // GEANT track number
//     Char_t  module;       // rpc module number
//     Char_t  layer;       // rpc module number
//     Int_t   rpcNum;       // rpc cell number
//     Float_t xHit;         // x of hit  (in mm) in cell coord. system
//     Float_t yHit;         // y of hit  (in mm) in cell coord. system
//     Float_t zHit;         // z of hit  (in mm) in cell coord. system
//     Float_t pxHit;        // x component of hit momentum (in MeV/c)
//     Float_t pyHit;        // y component of hit momentum (in MeV/c)
//     Float_t pzHit;        // z component of hit momentum (in MeV/c)
//     Float_t tofHit;       // time of flight of hit (in ns)
//     Float_t trackLength;  // track length (in mm)
//     Float_t eHit;         // energy deposited (in MeV)

    // variables for tracking cell hits

    Float_t x;
    Float_t y;
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

    Int_t findStrip(Float_t x);

    void processHits(const RpcTrackHits & rpc_track_hits);
    bool calculateHit(Int_t mod, Int_t lay, Int_t s, Float_t a_y, Float_t a_tof, Int_t track);

    bool fillHit(const ClusterFields & cf);

    ClassDef(HFwDetRpcDigitizer, 0);
};

#endif  /*  !HFWDETRPCDIGITIZER_H */
