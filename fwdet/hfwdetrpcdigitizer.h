#ifndef HFWDETRPCDIGITIZER_H
#define HFWDETRPCDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetRpcDigiPar;

class HFwDetRpcDigitizer : public HReconstructor
{
private:
    HCategory* fGeantFwDetCat;
    HCategory* fFwDetRpcCalCat;
    HFwDetRpcDigiPar* fRpcDigiPar;

    Int_t   trackNumber;  // GEANT track number
    Char_t  geantModule;  // GEANT FwDet module number (0...8)
    Char_t  geantCell;    // GEANT cell number inside module (0...8)
    Char_t  module;       // rpc module number
    Char_t  rpcNum;       // rpc cell number
    Float_t xHit;         // x of hit  (in mm) in cell coord. system
    Float_t yHit;         // y of hit  (in mm) in cell coord. system
    Float_t zHit;         // z of hit  (in mm) in cell coord. system
    Float_t pxHit;        // x component of hit momentum (in MeV/c)
    Float_t pyHit;        // y component of hit momentum (in MeV/c)
    Float_t pzHit;        // z component of hit momentum (in MeV/c)
    Float_t tofHit;       // time of flight of hit (in ns)
    Float_t trackLength;  // track length (in mm)
    Float_t eHit;         // energy deposited (in MeV)

    HLocation fLoc;

public:
    HFwDetRpcDigitizer();
    HFwDetRpcDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetRpcDigitizer();
    void   initVariables();
    Bool_t init();
    Bool_t reinit() const { return kTRUE; }
    Int_t  execute();
    Bool_t finalize() { return kTRUE;}
    ClassDef(HFwDetRpcDigitizer, 0);
};

#endif  /*  !HFWDETRPCDIGITIZER_H */
