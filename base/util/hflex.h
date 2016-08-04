#ifndef HFLEX_H
#define HFLEX_H

#include "TObject.h"
#include "TString.h"

#include <map>




class HFlex : public TObject {

private:

    std::map<TString,Int_t   > mInt;
    std::map<TString,Double_t> mDouble;

public:

    HFlex(){};
    ~HFlex(){};

    void     addVars(TString varsI="",TString varsD="");
    Int_t    getI(TString var);
    Double_t getD(TString var);
    void     setI(TString var,Int_t    val=0);
    void     setD(TString var,Double_t val=0);
    void     print(TString varlist="",Int_t w=15);
    ClassDef(HFlex,1)
} ;

#endif  /*!HFLEX_H*/
