#ifndef HFWDETRPCDIGIPAR_H
#define HFWDETRPCDIGIPAR_H

#include "hparcond.h"

class HFwDetRpcDigiPar : public HParCond
{
protected:
    // Parameters needed for digitizations of rpcs
    // Geometry, layout
    Int_t nDigiModel;
    Float_t fTimeReso;
    Float_t fTimeProp;
    Float_t fTimeOffset;

public:
    HFwDetRpcDigiPar(const Char_t* name = "FwDetRpcDigiPar",
            const Char_t* title = "Digitization parameters for Forward Rpc Detector",
            const Char_t* context = "FwDetRpcDigiProduction");
    virtual ~HFwDetRpcDigiPar();

    Int_t getDigiModel() const { return nDigiModel; }
    Float_t getTimeReso() const { return fTimeReso; }
    Float_t getTimeProp() const { return fTimeProp; }
    Float_t getTimeOffset() const { return fTimeOffset; }

    void setDigiModel(Int_t m) { nDigiModel = m; }
    void setTimeReso(Float_t r) { fTimeReso = r; }
    void setTimeProp(Float_t p) { fTimeProp = p; }
    void setTimeOffset(Float_t o) { fTimeOffset = o; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();

    ClassDef(HFwDetRpcDigiPar, 1); // Container for the Forward Rpc Detector digitization
};

#endif  /* !HFWDETRPCDIGIPAR_H */
