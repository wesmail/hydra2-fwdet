#ifndef HFWDETSTRAWDIGITIZER_H
#define HFWDETSTRAWDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "TVector3.h"
#include "TMatrixD.h"

class HCategory;
class HFwDetStrawDigiPar;

class HFwDetStrawDigitizer : public HReconstructor
{
private:
    HCategory* fGeantFwDetCat;
    HCategory* fFwDetStrawCalCat;
    HFwDetStrawDigiPar* fStrawDigiPar;

    Int_t   trackNumber;  // GEANT track number
    Char_t  module;       // module number (0...8) - 2 modules- first station (module0), second station (module1)
    Char_t  cell;         // cell number inside module (0...4) - 4 double layers
    Int_t   strawNum;     // straw number in cell: even numbers 90 degree, odd numbers 0 degreee
    Float_t xHit;         // geant x of hit  (in mm) in cell coord. system
    Float_t yHit;         // geant y of hit  (in mm) in cell coord. system
    Float_t zHit;         // geant z of hit  (in mm) in cell coord. system
    Float_t pxHit;        // geant x component of hit momentum (in MeV/c)
    Float_t pyHit;        // geant y component of hit momentum (in MeV/c)
    Float_t pzHit;        // geant z component of hit momentum (in MeV/c)
    Float_t tofHit;       // geant time of flight of hit (in ns)
    Float_t trackLength;  // geant track length (in mm)
    Float_t eHit;         // geant energy deposited (in MeV)

    HLocation fLoc;

    static const size_t nstations = 2; // number of straws
    static const size_t nstraws_Tx[nstations];

    static const Float_t radius_S = 5.05; //radius of the straw

    static const size_t rad_len1 = 4;        // number of modules per layer
    static const size_t rad_len2 = 113;//(nstraws_T1 > nstraws_T2 ? nstraws_T1 : nstraws_T2) / 2; // half of the bigger 

    static const Float_t straw_diam = 10.1; // diameter of straw
    static const Float_t straw_dist = 8.75; // distance between two layers

    static const Float_t Tx_x_a[nstations];
    static const Float_t Tx_x_b[nstations];

    static const Float_t T12_z_a = -9.424;  // z-coord. of the begining of the 1st layer in module
    static const Float_t T12_z_b = -0.674;  // z-coord. of the begining of the 1st layer in module

public:
    HFwDetStrawDigitizer();
    HFwDetStrawDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetStrawDigitizer();
    void   initVariables();
    Bool_t init();
    Bool_t reinit()   { return kTRUE; }
    Int_t  execute();
    Bool_t finalize() { return kTRUE; }

    ClassDef(HFwDetStrawDigitizer, 0);
};

#endif  /*  !HFWDETSTRAWDIGITIZER_H    */
