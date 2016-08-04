#ifndef __HMULTIPAGEPDF__
#define __HMULTIPAGEPDF__


#include "TPDF.h"
#include "TString.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TROOT.h"


#include <iostream>
using namespace std;

class HMultiPagePDF : public TPDF
{
private:
    TString name;
    TCanvas * c;
    Int_t  pageCt;

public:
    HMultiPagePDF(TString out="multipdf",TString option="landscape");
    ~HMultiPagePDF();
    void addPage(TCanvas* input, TString title = "");
    void close();

    ClassDef(HMultiPagePDF,0)
};



#endif
