
#ifndef HRICH700DIGIPAR_H_
#define HRICH700DIGIPAR_H_

#include "hparcond.h"



class HParamList;

class HRich700DigiPar : public HParCond {

private:
    Double_t fCrossTalkProbability;
    Int_t    fNofNoiseHits;

public:

    HRich700DigiPar(const Char_t* name   ="RichDigiPar",
		    const Char_t* title  ="Digi parameters for RICH",
		    const Char_t* context="RichDigiParProduction");
    virtual ~HRich700DigiPar();

    Double_t getCrossTalkProbability() { return fCrossTalkProbability;}
    Int_t    getNofNoiseHits()         { return fNofNoiseHits;}

    void     setCrossTalkProbability(Double_t p) { fCrossTalkProbability = p;}
    void     setNofNoiseHits(Int_t n)            { fNofNoiseHits = n;}


    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);

    ClassDef(HRich700DigiPar, 1)
};

#endif
