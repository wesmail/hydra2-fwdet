#ifndef HMaterials_h
#define HMaterials_h

#include "TObject.h"

class HMaterials : public TObject {

private:

    static const Int_t    nElements = 17;   // Number of materials stored in this class.

    static const Char_t  *cName[nElements]; // Material names.
    static const Float_t  fA   [nElements]; // Atomic mass in g/mole.
    static const Float_t  fZ   [nElements]; // Atomic number.
    static const Float_t  fRho [nElements]; // Density in g/cm³
    static const Float_t  fX0  [nElements]; // Radiation length in cm.
    static const Float_t  fI   [nElements]; // Mean excitation energy.

protected:

    static Int_t   getIdx       (const Char_t *id);

public:

    static Float_t getA         (const Char_t *id) { return fA  [getIdx(id)]; }
    static Float_t getZ         (const Char_t *id) { return fZ  [getIdx(id)]; }
    static Float_t getDensity   (const Char_t *id) { return fRho[getIdx(id)]; }
    static Float_t getExciteEner(const Char_t *id) { return fI  [getIdx(id)]; }
    static Float_t getRadLength (const Char_t *id) { return fX0 [getIdx(id)]; }

    ClassDef(HMaterials, 0)
};

#endif // Materials_h
