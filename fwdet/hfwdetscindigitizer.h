#ifndef HFWDETSCINDIGITIZER_H
#define HFWDETSCINDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetScinDigiPar;

class HFwDetScinDigitizer : public HReconstructor
{
private:
    HCategory* fGeantFwDetCat;
    HCategory* fFwDetScinCalCat;
    HFwDetScinDigiPar* fScinDigiPar;

    Int_t   trackNumber;  // GEANT track number
    Char_t  geantModule;  // GEANT FwDet module number (0...8)
    Char_t  geantCell;    // GEANT cell number inside module (0...8)
    Char_t  module;       // scintillator module number (0..1)
    Char_t  scinNum;      // scintillator cell number
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
    HFwDetScinDigitizer();
    HFwDetScinDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetScinDigitizer();

    void   initVariables();
    Bool_t init();
    Bool_t reinit() { return kTRUE; }
    Int_t  execute();
    Bool_t finalize() { return kTRUE; }
    ClassDef(HFwDetScinDigitizer, 0);
};

#endif  /*  !HFWDETSCINDIGITIZER_H    */
