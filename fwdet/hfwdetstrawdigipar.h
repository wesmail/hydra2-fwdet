#ifndef HFWDETSTRAWDIGIPAR_H
#define HFWDETSTRAWDIGIPAR_H

#include "hparcond.h"

#include "TArrayF.h"

class HFwDetStrawDigiPar : public HParCond
{
protected:
    // Parameters needed for digitizations of straws
    Float_t fAnalogReso;     // energy loss resolution
    Float_t fElossSlope;    // slope and offset fo drif time conversion
    Float_t fElossOffset;

    Float_t fTimeReso;          // time measurement resolution
    TArrayF fDriftRadiusPars;   // params for time->radius conversion
    TArrayF fDriftTimePars;     // params for radius->time conversion

    Float_t fStartOffset;   // start derector offset
    Float_t fThreshold;     // detection threshold
    Float_t fEfficiency;    // detection efficiency

public:
    HFwDetStrawDigiPar(const Char_t* name = "FwDetStrawDigiPar",
                       const Char_t* title = "Digitization parameters for Forward Straw Detector",
                       const Char_t* context = "FwDetDigiProduction");
    virtual ~HFwDetStrawDigiPar();

    inline Float_t getAnalogReso() const { return fAnalogReso; }
    inline Float_t getElossSlope() const { return fElossSlope; }
    inline Float_t getElossOffset() const { return fElossOffset; }

    inline Float_t getTimeReso() const { return fTimeReso; }
    Float_t getDriftTimePar(Int_t idx) const;
    Float_t getDriftRadiusPar(Int_t idx) const;

    inline Float_t getStartOffset() const { return fStartOffset; }
    inline Float_t getThreshold() const { return fThreshold; }
    inline Float_t getEfficiency() const { return fEfficiency; }

    inline void setAnalogReso(Float_t e) { fAnalogReso = e; }
    inline void setElossSlope(Float_t s) { fElossSlope = s; }
    inline void setElossOffset(Float_t o) { fElossOffset = o; }

    inline void setTimeReso(Float_t t) { fTimeReso = t; }
    void setDriftTimePar(Int_t idx, Float_t p);
    void setDriftRadiusPar(Int_t idx, Float_t p);

    inline void setStartOffset(Float_t o) { fStartOffset = o; }
    inline void setThreshold(Float_t t) { fThreshold = t; }
    inline void setEfficiency(Float_t e) { fEfficiency = e; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();
    void   print();

private:
    static const Int_t dt_pars_num = 5;

    ClassDef(HFwDetStrawDigiPar, 1); // Container for the Forward Straw Detector digitization
};

#endif  /* !HFWDETSTRAWDIGIPAR_H */
