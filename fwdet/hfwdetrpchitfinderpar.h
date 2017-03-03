#ifndef HFWDETRPCHITFINDERPAR_H
#define HFWDETRPCHITFINDERPAR_H

#include "hparcond.h"

class HFwDetRpcHitFinderPar : public HParCond
{
protected:
    // Parameters needed for digitizations of rpcs
    // Geometry, layout ...
    Float_t fMatchRadius;
    Float_t fMatchTime;

public:
    HFwDetRpcHitFinderPar(const Char_t* name = "FwDetRpcHitFinderPar",
            const Char_t* title = "Hit Finder parameters for Forward Rpc Detector",
            const Char_t* context = "FwDetRpcHitFinderProduction");
    virtual ~HFwDetRpcHitFinderPar();

    Float_t getMatchRadius() const { return fMatchRadius; }
    Float_t getMatchTime() const { return fMatchTime; }

    inline void setMatchRadius(Float_t r) { fMatchRadius = r; }
    inline void setMatchTime(Float_t t) { fMatchTime = t; }

    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();
    void   print() const;

    ClassDef(HFwDetRpcHitFinderPar, 1); // Container for the Forward Rpc Detector digitization
};

#endif  /* !HFWDETRPCHITFINDERPAR_H */
