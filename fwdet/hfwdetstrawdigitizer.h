#ifndef HFWDETSTRAWDIGITIZER_H
#define HFWDETSTRAWDIGITIZER_H

#include "fwdetdef.h"
#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetStrawDigiPar;
class HFwDetStrawGeomPar;

class HFwDetStrawDigitizer : public HReconstructor
{
private:
    HCategory*          pGeantFwDetCat;
    HCategory*          pStrawCalCat;
    HFwDetStrawDigiPar* pStrawDigiPar;
    HFwDetStrawGeomPar* pStrawGeomPar;
    HLocation fLoc;

public:
    HFwDetStrawDigitizer();
    HFwDetStrawDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetStrawDigitizer();
    void   initVariables();
    Bool_t init();
    Bool_t reinit();
    Int_t  execute();
    Bool_t finalize() { return kTRUE; }

private:
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
        Int_t plane;
        Int_t straw;
        Int_t udconf;
        Float_t time;           // total time: tof + drift
        Float_t adc;            // adc(tdc) value
        Float_t radius;
        Float_t posU;
        Float_t posZ;
    };

    Bool_t fillStrawCalSim(const DigiFields & df, const GeantFields & gf);
    Float_t calcDriftTime(Float_t x) const;

//     Float_t adc_reso;
//     Float_t eloss_slope;
//     Float_t eloss_offset;
//     Float_t time_reso;
    Float_t dt_p[5];
//     Float_t start_offset;
//     Float_t threshold;
//     Float_t efficiency;

    Float_t sina[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS];
    Float_t cosa[FWDET_STRAW_MAX_MODULES][FWDET_STRAW_MAX_LAYERS];

    ClassDef(HFwDetStrawDigitizer, 0);
};

#endif  /*  !HFWDETSTRAWDIGITIZER_H    */
