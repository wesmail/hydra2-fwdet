#ifndef HFWDETSTRAWDIGITIZER_H
#define HFWDETSTRAWDIGITIZER_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HFwDetStrawDigiPar;

class HFwDetStrawDigitizer : public HReconstructor
{
private:
    HCategory*          fGeantFwDetCat;
    HCategory*          fFwDetStrawCalCat;
    HFwDetStrawDigiPar* fStrawDigiPar;
    HLocation fLoc;

    Int_t   trackNumber;  // GEANT track number
    Char_t  module;       // module number (0...8) - 2 modules- first station (module0), second station (module1)
    Char_t  doubleLayer;  // doubleLayer number inside module (0...4) - 4 double layers
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

    static const Int_t   nstations = 2;          // number of straw modules
    static const Int_t   nstraws_Tx[nstations];
    static const Float_t straw_diam = 10.1;      // diameter of straw
    static const Float_t T12_z[2];               // z-coord. of the begining of the 1st and 2nd layer in module
    static const Float_t Tx_x[nstations][2];     // x-coor. of the firts wire in the layer (0 or 1) in coor.sys. of dubleLayer

public:
    HFwDetStrawDigitizer();
    HFwDetStrawDigitizer(const Text_t* name, const Text_t* title);
    virtual ~HFwDetStrawDigitizer();
    void   initVariables();
    Bool_t init();
    Bool_t reinit()   { return kTRUE; }
    Int_t  execute();
    Bool_t finalize() { return kTRUE; }

    static Float_t calcCellNumber(Float_t x,Int_t m,Int_t l) {
        // Calculate cell/tube number
        // x - in coor.system of doubleLayer
        // m - module type 0 or 1
        // l - layer number in doubleLayer 0 or 1
        // RETURN cell as float value: 0-0.999999 - cell 0; 1.-1.9999... - cell1
        return (x - Tx_x[m][l])/straw_diam + 0.5;
    }

    static Float_t getRadius_S()               {return straw_diam/2.;}  //radius of the straw
    static Float_t getT12_z(Int_t l)           {return T12_z[l];}       // z-coord. of the begining of the layer number "lay" in doubleLayer
    static Int_t   getNstraws_Tx(Int_t m)      {return nstraws_Tx[m];}
    static Float_t getTx_x(Int_t m,Int_t l)    {return Tx_x[m][l];}

private:
    Bool_t fillStrawCalSim( Float_t radius, Float_t posX, Float_t posZ);

    ClassDef(HFwDetStrawDigitizer, 0);
};

#endif  /*  !HFWDETSTRAWDIGITIZER_H    */
