#ifndef HKalMixture_h
#define HKalMixture_h

// from ROOT
#include "TMixture.h"


class HKalMixture : public TMixture {

private:
    Float_t  fExciteEner;     // Mean excitation energy of mixture in MeV.

    Float_t *fDensityMixt;    // Stores densities in g/mm^3 of components.
    Float_t *fExciteEnerMixt; // Stores mean excitation energy in MeV of components.
    Float_t *fRadLengthMixt;  // Stores radiation length in mm of components.

public:

    HKalMixture();
    HKalMixture(const char *name, const char *title, Int_t nmixt=1);
    virtual ~HKalMixture();

    static  void     calcMassFracFromVolFrac(Float_t w[], Int_t n, const Float_t rho[], const Float_t vol[]);
    virtual void     calcMixture            ();
    virtual void     defineElement          (Int_t n, const Float_t mat[5], Float_t w);
    virtual void     defineElement          (Int_t n, Float_t a, Float_t z, Float_t dense, Float_t exener, Float_t radl, Float_t w);
    virtual void     print                  (const char *opt="") const;

    virtual Float_t* GetDensityMixt         () const { return fDensityMixt;    }
    virtual Float_t  GetExciteEner          () const { return fExciteEner;     }
    virtual Float_t* GetExciteEnerMixt      () const { return fExciteEnerMixt; }
    virtual Float_t* GetRadLengthMixt       () const { return fRadLengthMixt;  }

    ClassDef(HKalMixture, 0)
};

#endif // HKalMixture_h
