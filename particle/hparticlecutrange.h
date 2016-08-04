#ifndef  __HPARTICLECUTRANGE_H__
#define  __HPARTICLECUTRANGE_H__

#include "TString.h"
#include "TNamed.h"
#include "TCutG.h"
#include "TF1.h"

#include <vector>

class HParticleCutRange : public TNamed {
private:
    Double_t                 flow;    // lower cut
    Double_t                 fup ;    // upper cut
    Int_t              fCutNumber;    // number of the cut
    UInt_t                fmaxCut;    // how many version can be used at max
    TCutG*                   fCut;    // graph cut object
    TF1*                   fLowF1;    // TFormular for low cut
    TF1*                    fUpF1;    // TFormular for upper cut
    Int_t                 fF1Mode;    // 0 = "low+up" , 1="low" , 2="up" , 3="mean+width"
    std::vector<ULong64_t>fctFail;    // count evaluation == kFALSE
    std::vector<ULong64_t>fctCall;    // count all call to the evaluation
    Bool_t           fbInverseCut;    // default kFALSE , kTRUE will invert selection
    UInt_t            fmaxVersion;    // how many version have been used

public :

    HParticleCutRange(TString name="cut", Int_t num=-1,Double_t l=-1e30,Double_t u=1e30);
    ~HParticleCutRange();
    void     setMaxCut(UInt_t max=4);
    void     setInverse(Bool_t inv)   { fbInverseCut = inv;}
    void     set(Double_t l=-1e30,Double_t u=1e30) { flow = l; fup  = u; }
    void     setTF1(TF1* flow,TF1*fup=0,TString mode="low+up");

    TF1*     getTF1Low()        { return fLowF1; }
    TF1*     getTF1Up()         { return fUpF1 ; }
    TCutG*   getTCutG()         { return fCut;   }

    Float_t  getCutRate(UInt_t version=0);
    UInt_t   getNCall  (UInt_t version=0);
    UInt_t   getNFail  (UInt_t version=0);
    UInt_t   getMaxVersion()   { return fmaxVersion; }
    Int_t    getCutNumber()    { return fCutNumber;  }
    Double_t getLowCut()      { return flow;}
    Double_t getUpCut()       { return fup;}

    Bool_t   eval(Double_t var,UInt_t version = 0);
    Bool_t   evalG(Double_t var,Double_t var2,UInt_t version = 0);
    Bool_t   evalF(Double_t var,Double_t var2,UInt_t version = 0);
    void     print();
    void     resetCounter();


    ClassDef(HParticleCutRange,1)
};



#endif
