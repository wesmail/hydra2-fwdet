#ifndef HFWDETSCINDIGIPAR_H
#define HFWDETSCINDIGIPAR_H

#include "hparcond.h"
#include "TArrayF.h"

class HFwDetScinDigiPar : public HParCond {
protected:
    // Parameters needed for digitizations of scintillator GEANT data
    // Geometry, layout ...
    Int_t   nScinCells;     // number of scintillator cells
    TArrayF fScinSize;        // width and height of a single cell

public:
    HFwDetScinDigiPar(const Char_t* name = "FwDetScinDigiPar",
                    const Char_t* title = "Digitization parameters for Forward Scintillator Detector",
                    const Char_t* context = "FwDetScinDigiProduction");
    virtual ~HFwDetScinDigiPar();
    Int_t getNScinCells() const { return nScinCells; }
    inline void  getScinSize(Float_t& w, Float_t& h) const;

    void setNScinCells(Int_t n) { nScinCells = n; }
    inline void setScinSize(Float_t w, Float_t h);

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();

    ClassDef(HFwDetScinDigiPar, 1); // Container for the Forward Scintillator Detector digitization
};

void HFwDetScinDigiPar::getScinSize(Float_t& w, Float_t& h) const
{
    w = fScinSize.At(0);
    h = fScinSize.At(1);
}

void HFwDetScinDigiPar::setScinSize(Float_t w, Float_t h)
{
    fScinSize.SetAt(w, 0);
    fScinSize.SetAt(h, 1);
}

#endif  /* !HFWDETSCINDIGIPAR_H */
