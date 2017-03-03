#ifndef HFWDETRPCDIGIPAR_H
#define HFWDETRPCDIGIPAR_H

#include "hparcond.h"

class HFwDetRpcDigiPar : public HParCond
{
protected:
    // Parameters needed for digitizations of rpcs
    // Geometry, layout ...
    Int_t   nRpcCells;
    Float_t rpcRadius;

public:
    HFwDetRpcDigiPar(const Char_t* name = "FwDetRpcDigiPar",
            const Char_t* title = "Digitization parameters for Forward Rpc Detector",
            const Char_t* context = "FwDetRpcDigiProduction");
    virtual ~HFwDetRpcDigiPar();

    Float_t getRpcRadius() const { return rpcRadius; }
    Int_t   getNRpcCells() const { return nRpcCells; }

    void setRpcRadius(Float_t r) { rpcRadius = r; }
    void setNRpcCells(Int_t c) { nRpcCells = c; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();

    ClassDef(HFwDetRpcDigiPar, 1); // Container for the Forward Rpc Detector digitization
};

#endif  /* !HFWDETRPCDIGIPAR_H */
