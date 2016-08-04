#ifndef HRPCHSLEWINGPAR_H
#define HRPCHSLEWINGPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HRpcSlewingPar : public HParCond {

protected:

    TArrayF fExpo1;   // Parameters of the exponential describing behaviour below fQcut
    TArrayF fExpo2;   // Parameters of the exponential describing behaviour above fQcut
    Float_t fQcut;    // Cut value for the two parameterizations. Limit region of both pars

public:
    HRpcSlewingPar(const Char_t* name ="RpcSlewingPar",
	      const Char_t* title  ="Slewing correciton parameters for the Rpc Detector",
	      const Char_t* context="RpcSlewingParProduction");
    ~HRpcSlewingPar();

    const   Float_t*  getExpo1(Int_t sec, Int_t col, Int_t cell)                { return &fExpo1[ 576*sec + 96*col + 3*cell ];}
    const   Float_t*  getExpo2(Int_t sec, Int_t col, Int_t cell)                { return &fExpo2[ 576*sec + 96*col + 3*cell ];}
            Float_t   getQcut()                                                 { return fQcut;}

    void    setExpo1par(Int_t sec, Int_t col, Int_t cell, Int_t index, Float_t par)  { fExpo1[ 576*sec + 96*col + 3*cell + index] = par; }
    void    setExpo2par(Int_t sec, Int_t col, Int_t cell, Int_t index, Float_t par)  { fExpo2[ 576*sec + 96*col + 3*cell + index] = par; }
    void    setQcut(Float_t q)  { fQcut = q; }

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();

    ClassDef(HRpcSlewingPar,1) // Container for the RPC hit finder parameters
};
#endif  /*!HRPCSLEWINGPAR_H*/
