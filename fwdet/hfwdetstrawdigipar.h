#ifndef HFWDETSTRAWDIGIPAR_H
#define HFWDETSTRAWDIGIPAR_H

#include "hparcond.h"

class HFwDetStrawDigiPar : public HParCond
{
protected:
    // Parameters needed for digitizations of straws
    // Geometry, layout ...
    Int_t   nStrawCells;
    Float_t fStrawRadius;

public:
    HFwDetStrawDigiPar(const Char_t* name = "FwDetStrawDigiPar",
                       const Char_t* title = "Digitization parameters for Forward Straw Detector",
                       const Char_t* context = "FwDetDigiProduction");
    virtual ~HFwDetStrawDigiPar();

    inline Float_t getStrawRadius() const { return fStrawRadius; }
    inline Int_t   getStrawCells() const { return nStrawCells; }

    inline void setStrawRadius(Float_t r) { fStrawRadius = r; }
    inline void setStrawCells(Int_t c) { nStrawCells = c; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();

    ClassDef(HFwDetStrawDigiPar, 1); // Container for the Forward Straw Detector digitization
};

#endif  /* !HFWDETSTRAWDIGIPAR_H */
