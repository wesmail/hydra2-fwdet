#ifndef HFWDETSTRAWDIGIPAR_H
#define HFWDETSTRAWDIGIPAR_H

#include "hparcond.h"

class HFwDetStrawDigiPar : public HParCond
{
protected:
    // Parameters needed for digitizations of straws
    // Time, Energy Loss, Drift Radius
    Float_t fTimeReso;
    Float_t fElossReso;
    Float_t fDriftReso;

public:
    HFwDetStrawDigiPar(const Char_t* name = "FwDetStrawDigiPar",
                       const Char_t* title = "Digitization parameters for Forward Straw Detector",
                       const Char_t* context = "FwDetDigiProduction");
    virtual ~HFwDetStrawDigiPar();

    inline Float_t getTimeReso() const { return fTimeReso; }
    inline Float_t getElossReso() const { return fElossReso; }
    inline Int_t   getDriftReso() const { return fDriftReso; }

    inline void setTimeReso(Float_t t) { fTimeReso = t; }
    inline void setELossReso(Float_t e) { fElossReso = e; }
    inline void setDriftReso(Float_t d) { fDriftReso = d; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();

    ClassDef(HFwDetStrawDigiPar, 1); // Container for the Forward Straw Detector digitization
};

#endif  /* !HFWDETSTRAWDIGIPAR_H */
