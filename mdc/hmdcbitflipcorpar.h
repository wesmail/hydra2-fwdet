#ifndef HMDCBITFLIPCORPAR_H
#define HMDCBITFLIPCORPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HMdcBitFlipCorPar : public HParCond {
protected:
    TArrayI bitshiftcorfactor;      // efficiency per module and layer
public:
    HMdcBitFlipCorPar(const Char_t* name ="MdcBitFlipCorPar",
	      const Char_t* title  ="bitflip correction parameters for Mdc",
	      const Char_t* context="MdcBitFlipCorParProduction");
    ~HMdcBitFlipCorPar();

    Int_t getBitFlipCorParameter(Int_t s,Int_t m,Int_t mbo, Int_t v)  {return bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+v];}
    void    setBitFlipCorParameter(Int_t s,Int_t m,Int_t mbo, Int_t v,Int_t value) {bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+v]=value;}
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();
    void    printParam(void);
    ClassDef(HMdcBitFlipCorPar,1) // Container for the MDC bitflip correction parameters
};
#endif 
