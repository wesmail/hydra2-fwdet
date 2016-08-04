#ifndef HRPCTIMEPOSPAR_H
#define HRPCTIMEPOSPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HRpcTimePosPar : public HParCond {

protected:

    TArrayF f5harmonics_16;   // Parameters of the 5 harmonics used for the correction


public:
    HRpcTimePosPar(const Char_t* name ="RpcTimePosPar",
	      const Char_t* title  ="Time-position correction parameters for the Rpc Detector",
	      const Char_t* context="RpcTimePosParProduction");
    ~HRpcTimePosPar();

    Float_t  getPar(Int_t sec, Int_t col, Int_t cell, Int_t npar) const { return f5harmonics_16[ sec*3072 + col*512 + cell*16 + npar ];}
    const Float_t* getPars(Int_t sec, Int_t col, Int_t cell)  {return &f5harmonics_16[sec*3072 + col*512 + cell*16 ]; }

    void    setPar(Int_t sec, Int_t col, Int_t cell, Int_t npar, Float_t par)  { f5harmonics_16[sec*3072 + col*512 + cell*16  + npar ] = par; }
    void    setPars(Int_t sec, Int_t col, Int_t cell, Float_t* pars)  { for(Int_t i = 0; i < 16 ; i++) f5harmonics_16.AddAt(pars[i], sec*3072 + col*512 + cell*16  + i); }

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();

    ClassDef(HRpcTimePosPar,2) // Container for the RPC hit finder parameters
};
#endif  /*!HRPCTIMEPOSPAR_H*/
