#ifndef HTRENDHIST_H
#define HTRENDHIST_H
#include "TNamed.h"
#include "TString.h"
#include "TArrayI.h"
#include "TH1.h"
#include "honlinemonhist.h"

/** 1-Dim Trend Histogram.
 * Each call of the fill function moves the
 * content of the histogram by one bin to the left
 * and the new value is added in the right most bin.
*/
class HOnlineTrendHist: public HOnlineMonHist {
protected:
public:
    HOnlineTrendHist(const Char_t* name   ="",
	       const Char_t* title  ="",
	       Int_t active=0,Int_t resetable=0,
	       Int_t nBinsX=0,Float_t Xlo=0,Float_t Xup=0);
    HOnlineTrendHist(HOnlineMonHistAddon&);
    /// fill value (including moving the content)
    void    fill   (Stat_t i,Stat_t j = 0);
    virtual ~HOnlineTrendHist();
    void    getMinMax(Double_t& min,Double_t& max) ;


    ClassDef(HOnlineTrendHist,1) // trend hist
};

#endif  /*!HTRENDHIST_H*/
