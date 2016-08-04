using namespace std;
#include "honlinetrendhist.h"
#include <iostream> 
#include <iomanip>

ClassImp(HOnlineTrendHist)

HOnlineTrendHist::HOnlineTrendHist(const Char_t* name,
		       const Char_t* title,
		       Int_t active,Int_t resetable,
		       Int_t nbinsX,Float_t xlo,Float_t xup)
    :  HOnlineMonHist(name,title,active,resetable,nbinsX,xlo,xup)
{
}
HOnlineTrendHist::HOnlineTrendHist(HOnlineMonHistAddon& add)
{
    copySet(add);
    h=0;
    create();
}
void HOnlineTrendHist::fill(Stat_t a, Stat_t b)
{
    for(Int_t i=0;i<h->GetNbinsX();i++){h->SetBinContent(i,h->GetBinContent(i+1) );}
    h->SetBinContent(h->GetNbinsX(),a);
    //h->SetBinError(h->GetNbinsX(),b);
}
HOnlineTrendHist::~HOnlineTrendHist(){}


void  HOnlineTrendHist::getMinMax(Double_t& min,Double_t& max)
{
    min =  1e200;
    max = -1e200;
    for(Int_t i = 0; i < h->GetNbinsX(); i++ ) {
	Double_t val = h->GetBinContent(i+1);
	if(val < min) min = val;
	if(val > max) max = val;

    }
}

