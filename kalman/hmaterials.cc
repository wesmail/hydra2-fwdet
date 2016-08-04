
#include "TError.h"
#include "TString.h"

#include "hmaterials.h"
#include "hkaldef.h"
using namespace Kalman;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This class stores the properties of materials that are used in the Hades
// Mdc's: atomic mass, atomic number, density in g/cm^3, radiation length in cm
// and mean excitation energy in eV.
// Data for atomic and nuclear properties were obtained particle data group.
// http://pdg.lbl.gov/2009/AtomicNuclearProperties/index.html
// The properties can be accessed via the various get functions.
// These require a material id that is stored in the cName array. Many
// materials have several possible id's. For example, to get a property of
// hydrogen gas either "H" or "hydrogen" will work as identifier. Capitalization
// will be ignored for the id.
//
///////////////////////////////////////////////////////////////////////////////



ClassImp(HMaterials)

// Material names
const Char_t* HMaterials::cName[] = {
        // Chemical Elements
        /*  0 = Invalid Material */ "",
        /*  1 = Hydrogen gas     (H)  */ "H, Hydrogen",
        /*  2 = Helium gas       (He) */ "He, Helium",
        /*  3 = Beryllium        (Be) */ "Be, Beryllium",
        /*  4 = Carbon amorphous (C)  */ "C, Carbon",
        /*  5 = Nitrogen gas     (N)  */ "N, Nitrogen",
        /*  6 = Oxygen gas       (O)  */ "O, Oxygen",
        /*  7 = Aluminum         (Al) */ "Al, Aluminum",
        /*  8 = Argon gas        (Ar) */ "Ar, Argon",
        /*  9 = Copper           (Cu) */ "Cu, Copper",
        /* 10 = Tungsten         (W)  */ "W, Tungsten",
        /* 11 = Gold             (Au) */ "Au, Gold",
        // Mixtures
	/* 12 = Air                   */ "Air",
	/* 13 = Carbon Dioxide        */ "CO2",
        /* 14 = Butane     (C4H10)    */ "Butane, C4H10",
        /* 15 = Ethane     (C2H6)     */ "Ethane, C2H6"
        /* 16 = Mylar      (C10H8O4)n */ "Mylar, C10H8O4, Polyethylene terephthalate"
};

// Atomic mass in g/mole
const Float_t HMaterials::fA[] = {
        // Chemical Elements
        /*  0 = Invalid Material */ 1,
        /*  1 = Hydrogen gas      (H)  */   1.00794,
        /*  2 = Helium gas        (He) */   4.002602,
        /*  3 = Beryllium         (Be) */   9.012182,
        /*  4 = Carbon amorphous  (C)  */  12.0107,
        /*  5 = Nitrogen gas      (N)  */  14.0067,
        /*  6 = Oxygen gas        (O)  */  15.9994,
        /*  7 = Aluminum          (Al) */  26.9815386,
        /*  8 = Argon gas         (Ar) */  39.948,
        /*  9 = Copper            (Cu) */  63.546,
        /* 10 = Tungsten          (W)  */ 183.84,
        /* 11 = Gold              (Au) */ 196.966569,
        // Mixtures
        /* 12 = Air                    */   7.3727470 / 0.49919, // = Z / (Z/A)
	/* 13 = Carbon Dioxide         */   (0.272916 * 6 + 0.727084 * 8) / 0.49989,
        /* 14 = Butane      (C4H10)    */  21.5722880 / 0.59497,
        /* 15 = Ethane      (C2H6)     */  (0.20115 * 1. + 0.798885 * 6.) / 0.59861,
        /* 16 = Mylar       (C10H8O4)n */  48.4934920 / 0.52037
};

// Atomic number
const Float_t HMaterials::fZ[] = {
        // Chemical Elements
        /*  0 = Invalid Material */   1,
        /*  1 = Hydrogen gas      (H)  */   1,
        /*  2 = Helium gas        (He) */   2,
        /*  3 = Beryllium         (Be) */   4,
        /*  4 = Carbon amorphous  (C)  */   6,
        /*  5 = Nitrogen gas      (N)  */   7,
        /*  6 = Oxygen gas        (O)  */   8,
        /*  7 = Aluminum          (Al) */  13,
        /*  8 = Argon gas         (Ar) */  18,
        /*  9 = Copper            (Cu) */  29,
        /* 10 = Tungsten          (W)  */  74,
        /* 11 = Gold              (Au) */  79,
        // Mixtures
        /* 12 = Air                    */   7.3727470, // 0.755267*7(N) + 0.231781*8(O) + 0.012827*18(Ar) + 0.000124*6(C)
	/* 13 = Carbon Dioxide         */   0.272916 * 6 + 0.727084 * 8,
        /* 14 = Butane      (C4H10)    */  21.5722880, // 0.826592*6*4(C) + 0.173408*1*10(H)
        /* 15 = Ethane      (C2H6)     */  (0.20115 * 1. + 0.798885 * 6.),
        /* 16 = Mylar      (C10H8O4)n */   48.4934920  // 0.625017*6*10(C) + 0.041959*1*8(H) + 0.333025*8*4(O)
};

// Density (20° C, 1 atm) in g/cm^3
const Float_t HMaterials::fRho[] = {
        // Chemical Elements
        /*  0 = Invalid Material */  1,
        /*  1 = Hydrogen gas      (H)  */   8.38 * (1.e-05),
        /*  2 = Helium gas        (He) */   1.66 * (1.e-4),
        /*  3 = Beryllium         (Be) */   1.85,
        /*  4 = Carbon amorphous  (C)  */   2.0,
        /*  5 = Nitrogen gas      (N)  */   1.17 * (1.e-03),
        /*  6 = Oxygen gas        (O)  */   1.33 * (1.e-03),
        /*  7 = Aluminum          (Al) */   2.7,
        /*  8 = Argon gas         (Ar) */   1.66 * (1.e-3),
        /*  9 = Copper            (Cu) */   8.96,
        /* 10 = Tungsten          (W)  */  19.3,
        /* 11 = Gold              (Au) */  19.3,
        // Mixtures
        /* 12 = Air                    */   1.20 * (1.e-03),
	/* 13 = Carbon Dioxide         */   1.84 * (1.e-03),
        /* 14 = Butane      (C4H10)    */   2.49 * (1.e-03),
        /* 15 = Ethane      (C2H6)     */   1.26 * (1.e-03),
        /* 16 = Mylar       (C10H8O4)n */   1.40
};

// Radiation length in cm.
const Float_t HMaterials::fX0[] = {
        // Chemical Elements
        /*  0 = Invalid Material */  1,
        /*  1 = Hydrogen gas      (H)  */   7.527 * (1.e+05),
        /*  2 = Helium gas        (He) */   5.671 * (1.e+05),
        /*  3 = Beryllium         (Be) */  35.28,
        /*  4 = Carbon amorphous  (C)  */  21.35,
        /*  5 = Nitrogen gas      (N)  */   3.260 * (1.e+04),
        /*  6 = Oxygen gas        (O)  */   2.571 * (1.e+04),
        /*  7 = Aluminum          (Al) */   8.897,
        /*  8 = Argon gas         (Ar) */   1.176 * (1.e+04),
        /*  9 = Copper            (Cu) */   1.436,
        /* 10 = Tungsten          (W)  */   0.3504,
        /* 11 = Gold              (Au) */   0.3344,
        // Mixtures
        /* 12 = Air                    */   3.039 * (1.e+04),
	/* 13 = Carbon Dioxide         */   1.965 * (1.e+04),
        /* 14 = Butane     (C4H10)     */   1.817 * (1.e+04),
        /* 15 = Ethane      (C2H6)     */   3.615 * (1.e+04),
        /* 16 = Mylar     (C10H8O4)n   */  28.54
};

// Mean excitation energy in eV
const Float_t HMaterials::fI[] = {
        // Chemical Elements
        /*  0 = Invalid Material       */    1,
        /*  1 = Hydrogen gas      (H)  */   19.2,
        /*  2 = Helium gas        (He) */   41.8,
        /*  3 = Beryllium         (Be) */   63.7,
        /*  4 = Carbon amorphous  (C)  */   78.0,
        /*  5 = Nitrogen gas      (N)  */   82.0,
        /*  6 = Oxygen gas        (O)  */   95.0,
        /*  7 = Aluminum          (Al) */  166.0,
        /*  8 = Argon gas         (Ar) */  188.0,
        /*  9 = Copper            (Cu) */  322.0,
        /* 10 = Tungsten          (W)  */  727.0,
        /* 11 = Gold              (Au) */  790.0,
        // Mixtures
        /* 12 = Air                    */ 85.7,
	/* 13 = Carbon Dioxide         */ 85.0,
        /* 14 = Butane      (C4H10)    */ 48.3,
        /* 15 = Ethane      (C2H6)     */ 45.4,
        /* 16 = Mylar      (C10H8O4)n  */ 78.7
};

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Int_t HMaterials::getIdx(const Char_t *id) {
    // Finds the array index for the material with name 'id'.

    Int_t i = 0;
    Int_t iMax = nElements;
    Bool_t found = kFALSE;
    TString sid(id);
    while(!found && i < iMax) {
        sid = cName[i];
        if(sid.Contains(id, TString::kIgnoreCase)) {
            found = kTRUE;
        } else {
            i++;
        }
    }
    if(!found) {
        ::Error("getIdx()", "Could not find %s.", id);
        return 0;
    }
    return i;
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------
