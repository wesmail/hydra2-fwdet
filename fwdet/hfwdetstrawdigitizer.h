#ifndef HFWDETSTRAWDIGITIZER_H
#define HFWDETSTRAWDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetStrawDigiPar;


class HFwDetStrawDigitizer : public HReconstructor {

private:
    HCategory* fGeantFwDetCat;
    HCategory* fFwDetStrawCalCat;
    HFwDetStrawDigiPar* fStrawDigiPar;

    Int_t   trackNumber;  // GEANT track number
    Char_t  geantCell;    // GEANT cell number inside module (0...8)
    Char_t  module;       // straw module number (0...1)
    Char_t  strawNum;     // straw cell number
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

    //vector <HFwDetStrawCal> strawcal;

public:
    HFwDetStrawDigitizer(void);
    HFwDetStrawDigitizer(const Text_t* name, const Text_t* title);
    ~HFwDetStrawDigitizer(void) {}
    void   initVariables();
    Bool_t init    (void);
    Bool_t reinit(void)   {return kTRUE;}
    Int_t  execute (void);
    Bool_t finalize(void) {return kTRUE;}
    ClassDef(HFwDetStrawDigitizer,0)
};

#endif  /*  !HFWDETSTRAWDIGITIZER_H    */

