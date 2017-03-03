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
    Char_t mod;
    Char_t lay;
    Int_t cell[FWDET_RPC_MAX_CELLS];
    Float_t x[FWDET_RPC_MAX_CELLS];
    Float_t y[FWDET_RPC_MAX_CELLS];
    Float_t tof[FWDET_RPC_MAX_CELLS];

    void print() const
    {
        printf("  mod=%d  lay=%d  hits=%d\n", mod, lay, hits_num);
        for (int i= 0; i < hits_num; ++i)
        {
            printf("     c=%d  x=%f  y=%f  tof=%f\n",
                   cell[i], x[i], y[i], tof[i]);
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

    Int_t   trackNumber;  // GEANT track number
    Char_t  geantModule;  // GEANT FwDet module number (0...8)
    Char_t  geantLayer;   // GEANT FwDet layer number (0...8)
    Int_t   geantCell;    // GEANT cell number inside module (0...8)
    Char_t  module;       // rpc module number
    Int_t   rpcNum;       // rpc cell number
    Float_t xHit;         // x of hit  (in mm) in cell coord. system
    Float_t yHit;         // y of hit  (in mm) in cell coord. system
    Float_t zHit;         // z of hit  (in mm) in cell coord. system
    Float_t pxHit;        // x component of hit momentum (in MeV/c)
    Float_t pyHit;        // y component of hit momentum (in MeV/c)
    Float_t pzHit;        // z component of hit momentum (in MeV/c)
    Float_t tofHit;       // time of flight of hit (in ns)
    Float_t trackLength;  // track length (in mm)
    Float_t eHit;         // energy deposited (in MeV)

    // variables for tracking cell hits
    Int_t c_tr;        // current (tracked) variables
    Int_t c_mod;
    Int_t c_lay;

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
    bool calculateHit(Int_t mod, Int_t lay, Int_t s, Float_t a_y, Float_t a_tof);

    bool fillHit(Int_t m, Int_t l, Int_t s, Float_t tl, Float_t tr, Float_t ql, Float_t qr);

    ClassDef(HFwDetRpcDigitizer, 0);
};

#endif  /*  !HFWDETRPCDIGITIZER_H */
