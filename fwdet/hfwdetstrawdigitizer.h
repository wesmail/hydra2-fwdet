#ifndef HFWDETSTRAWDIGITIZER_H
#define HFWDETSTRAWDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetStrawDigiPar;
class HFwDetStrawGeomPar;

class TRandom3;

class HFwDetStrawDigitizer : public HReconstructor
{
private:
    HCategory*          pGeantFwDetCat;
    HCategory*          pStrawCalCat;
    HFwDetStrawDigiPar* pStrawDigiPar;
    HFwDetStrawGeomPar* pStrawGeomPar;
    HLocation fLoc;

    Int_t   trackNumber;  // GEANT track number
    Char_t  geantModule;  // module number (0...8) - 2 modules- first station (module0), second station (module1)
    Char_t  geantLayer;   // layer number (0..8) - four layers per module
    Int_t   geantCell;    // doubleLayer number inside module (0...4) - 4 double layers

    Float_t xHit;         // geant x of hit  (in mm) in cell coord. system
    Float_t yHit;         // geant y of hit  (in mm) in cell coord. system
    Float_t zHit;         // geant z of hit  (in mm) in cell coord. system
    Float_t pxHit;        // geant x component of hit momentum (in MeV/c)
    Float_t pyHit;        // geant y component of hit momentum (in MeV/c)
    Float_t pzHit;        // geant z component of hit momentum (in MeV/c)
    Float_t tofHit;       // geant time of flight of hit (in ns)
    Float_t trackLength;  // geant track length (in mm)
    Float_t eHit;         // geant energy deposited (in MeV)

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
    Bool_t fillStrawCalSim(Float_t time, Float_t adc, Float_t tof, Float_t eloss, Float_t radius, Float_t posX, Float_t posZ, Int_t straw);

    Float_t calcDriftTime(Float_t x) const;

    TRandom3 *rand;

    Float_t adc_reso;
    Float_t eloss_slope;
    Float_t eloss_offset;
    Float_t time_reso;
    Float_t dt_p[5];
    Float_t start_offset;
    Float_t threshold;
    Float_t efficiency;

    ClassDef(HFwDetStrawDigitizer, 0);
};

#endif  /*  !HFWDETSTRAWDIGITIZER_H    */
