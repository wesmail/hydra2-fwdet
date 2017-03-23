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
    HLocation fLoc;

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

    struct DigiFields
    {
        Int_t mod;
        Int_t lay;
        Int_t cell;
        Float_t time;           // total time: tof + drift
        Float_t adc;            // adc(tdc) value
        Float_t radius;
        Float_t posX;
        Float_t posZ;
    };

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
