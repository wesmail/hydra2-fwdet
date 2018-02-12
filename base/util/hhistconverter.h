#ifndef __HHISTCONVERTER_H__
#define __HHISTCONVERTER_H__

#include "TObject.h"
#include "TString.h"
#include "TH1.h"

#include <ostream>

class HHistConverter : public TObject {


public:

    HHistConverter();
    ~HHistConverter();
    static void printArray(const TArray& dat,Int_t nvals = 10,Int_t width =0,Int_t start=-1,Int_t end=-1);
    static void writeArray(std::ostream& out,TString name,const TArray& dat,Int_t nvals = 10);
    static void printArrayInfo(const TArrayD& linData,TString name,Int_t nvals,Int_t width);
    static TH1* createHist(TString name, const TArrayD& linData);
    static void fillArray(const TH1* h,TArrayD& linData,TString name="",Int_t nvals = 10,Int_t width =0, Bool_t print = kTRUE);
    ClassDef(HHistConverter,0) // convert histogram from/to linear TArrayD
};

#endif //__HHISTCONVERTER_H__
