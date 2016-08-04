#ifndef HRPCHITFPAR_H
#define HRPCHITFPAR_H
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;

class HRpcHitFPar : public HParCond {
protected:
    TArrayF fPol5Pars;   // Parameters of 5th order pol parameterization of T=f(Q) 

    TArrayF fPol1Pars;   // Parameters of 1th order pol parameterization of T=f(Q) 

		Float_t fQcut;       // Cut value for the two parameterizations
public:
    HRpcHitFPar(const Char_t* name ="RpcHitFPar",
	      const Char_t* title  ="Hit Finder parameters for the Rpc Detector",
	      const Char_t* context="RpcHitFProduction");
    ~HRpcHitFPar();

		const   Float_t*  getPol5Pars(Int_t sec,Int_t col,Int_t cell)                {return &fPol5Pars[1152*sec+192*col+6*cell];}
    void    setPol5Pars(Int_t sec,Int_t col,Int_t cell,Int_t index,Float_t par)  {fPol5Pars[1152*sec+192*col+6*cell+index]=par;}

    const   Float_t*  getPol1Pars(Int_t sec,Int_t col,Int_t cell)                {return &fPol1Pars[384*sec+64*col+2*cell];}
    void    setPol1Pars(Int_t sec,Int_t col,Int_t cell,Int_t index,Float_t par)  {fPol1Pars[384*sec+64*col+2*cell+index]=par;}

    Float_t getQcut()            {return fQcut;}
    void    setQcut(Float_t qc)  {fQcut=qc;}

    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);
    void    clear();

    ClassDef(HRpcHitFPar,1) // Container for the RPC hit finder parameters
};
#endif  /*!HRPCHITFPAR_H*/
