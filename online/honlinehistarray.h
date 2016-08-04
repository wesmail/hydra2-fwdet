#ifndef HHISTARRAY_H
#define HHISTARRAY_H
#include "TNamed.h"
#include "TString.h"
#include "TArrayI.h"
#include "TObjArray.h"
#include "TH1.h"
#include "TH2.h"
#include "honlinemonhist.h"
#include "TLegend.h"

class HOnlineNamedLegend;

/** Class to draw automatic legends for HOnlineHistArrays in overlay plots.
*/
class HOnlineLegendDrawer : public TNamed {
TObjArray* array; //!< object to keep the created Legend objects
protected:
public:
    HOnlineLegendDrawer(const Char_t* name   ="",
		  const Char_t* title  ="");
    ~HOnlineLegendDrawer(void);
    /// draw Legend for HOnlineHistArray.
    /// Checks if Legend exist already, otherwise adds a new Legend
    void          draw(HOnlineMonHistAddon*);
    /// add a new Legend to the object array
    HOnlineNamedLegend* add (HOnlineMonHistAddon*);
    ClassDef(HOnlineLegendDrawer,1) // Legend drawer
};

/** Named Legend containing a TLegend
*/
class HOnlineNamedLegend : public TNamed {
protected:
TLegend* legend;  //!< pointer to internal TLegend
public:
    HOnlineNamedLegend (const Char_t* name   ="",
		  const Char_t* title  ="",HOnlineMonHistAddon* =0);
    ~HOnlineNamedLegend(void);
    /// get pointer to internal TLegend
    TLegend* getP(){return legend;}
    ClassDef(HOnlineNamedLegend,1) // Named Legend
};

//########################################################################
/**Array of 1-D Trend Histograms.
 * Contains a array of histograms of type 1S,1F or 1D.
 * The internal histograms can be acccessed via
 * getP(Int_t s,Int_t m) in coordinates od the array.
 * The call of the fill() function will shift the contents
 * of the specific histogram by on bin to the left and
 * add the new value at the right most bin.
 */
class HOnlineTrendArray : public HOnlineMonHistAddon{
protected:
    TObjArray* array; /// pointer to internal object array of hists
public:
    HOnlineTrendArray(const Char_t* name   ="",
		const Char_t* title  ="",
		Int_t active=0,Int_t resetable=0, Int_t size=1,Int_t size2=1,
		Int_t nBinsX=0,Float_t Xlo=0,Float_t Xup=0);
    /// create a HOnlineTrendArray from a HOnlineMonHistAddon
    HOnlineTrendArray(HOnlineMonHistAddon&);
    ~HOnlineTrendArray(void);
    /// get the dimensions of the array (in 2-Dim)
    void     getDim(Int_t* i,Int_t* j)    {*i=arraySize1;*j=arraySize2;}
    /// get the linear overall size of the array
    Int_t    getSizeAll()    {return arraySize1*arraySize2;}
    /// fill a histogram, moving the old content 1
    /// bin to the left and add the new value
    /// at the right most bin
    void     fill   (Int_t,Int_t,Stat_t,Stat_t val = 0);
    /// reset the histogram array
    void     reset(Int_t level,Int_t count);
    /// create the array of histograms
    void     create();
    /// draw overlay plot of all hists in array
    void     drawAll( Bool_t allowHiddenZero=1 , Double_t min = -1e200,Double_t max=1e200);
    /// draw calls drawAll();
    void     draw( Bool_t allowHiddenZero=1 );
    /// add a HOnlineTrendArray to the  own hists (see TH1::Add)
    void     add(HOnlineMonHistAddon* h2);
    /// get pointer to histogram inside the array
    TH1*     getP(Int_t s=0,Int_t m=0){return ((TH1*) (*array)[s*arraySize2+m]);}
    void     getMinMax(Double_t& min,Double_t& max);

    ClassDef(HOnlineTrendArray,1) // Mdc trend hist array
};

//#################################################################################

/**Array of 1-D Histograms.
 * Contains a array of histograms of type 1S,1F or 1D.
 * The internal histograms can be acccessed via
 * getP(Int_t s,Int_t m) in coordinates od the array.
 */

class HOnlineHistArray : public HOnlineMonHistAddon{
protected:
    TObjArray* array;
public:
    HOnlineHistArray(const Char_t* name   ="",
		  const Char_t* title  ="",
		  Int_t active=0,Int_t resetable=0, Int_t size=1,Int_t size2=1,
		  Int_t nBinsX=0,Float_t Xlo=0,Float_t Xup=0);
    /// create a HOnlineHistArray from a HOnlineMonHistAddon
    HOnlineHistArray(HOnlineMonHistAddon&);
    ~HOnlineHistArray(void);
    /// get the dimensions of the array (in 2-Dim)
    void     getDim(Int_t* i,Int_t* j)    {*i=arraySize1;*j=arraySize2;}
    /// get the linear overall size of the array
    Int_t    getSizeAll()    {return arraySize1*arraySize2;}
    /// fill a value in a given histogram inside the array
    void     fill   (Int_t,Int_t,Stat_t,Stat_t b = 0);
    /// set bin content of a given histogram inside the array
    void     setBinContent(Int_t,Int_t,Int_t,Stat_t);
    /// get bin content of a given histogram inside the array
    Stat_t   getBinContent(Int_t,Int_t,Int_t);
    /// set bin error of a given histogram inside the array
    void     setBinError  (Int_t,Int_t,Int_t,Stat_t);
    /// get bin error of a given histogram inside the array
    Stat_t   getBinError  (Int_t,Int_t,Int_t);
    /// reset the histogram array
    void     reset(Int_t level,Int_t count);
    /// create the histogram array
    void     create();
    /// draw overlay plot of all histograms
    /// insde the histogram array
    void     drawAll( Bool_t allowHiddenZero=1 , Double_t min = -1e200,Double_t max=1e200 );
    /// calls drawAll()
    void     draw( Bool_t allowHiddenZero=1 );
    /// add a HOnlineHistArray to the  own hists (see TH1::Add)
    void     add(HOnlineMonHistAddon* h2);
    /// get pointer to histogram inside the array
    TH1*     getP(Int_t s=0,Int_t m=0){return ((TH1*) (*array)[s*arraySize2+m]);}

    void     getMinMax(Double_t& min,Double_t& max);


    ClassDef(HOnlineHistArray,1) // Mdc mon hist array
};

//################################################################################
/**Array of 2-D Histograms.
 * Contains a array of histograms of type 2S,2F or 2D.
 * The internal histograms can be acccessed via
 * getP(Int_t s,Int_t m) in coordinates od the array.
*/

class HOnlineHistArray2 : public HOnlineMonHistAddon{
protected:
    TObjArray* array;
public:
    HOnlineHistArray2(const Char_t* name   ="",
		   const Char_t* title  ="",
		   Int_t active=0,Int_t resetable=0, Int_t size=1,Int_t size2=1,
		   Int_t nBinsX=0,Float_t Xlo=0,Float_t Xup=0);
    /// create a HOnlineHistArray2 from a HOnlineMonHistAddon
    HOnlineHistArray2(HOnlineMonHistAddon&);
    ~HOnlineHistArray2(void);
    /// get the dimensions of the array (in 2-Dim)
    void     getDim(Int_t* i,Int_t* j)    {*i=arraySize1;*j=arraySize2;}
    /// get the linear overall size of the array
    Int_t    getSizeAll()    {return arraySize1*arraySize2;}
    /// fill a value in a given histogram inside the array
    void     fill   (Int_t,Int_t,Stat_t,Stat_t);
    /// set bin content of a given histogram inside the array
    void     setBinContent(Int_t,Int_t,Int_t,Int_t,Stat_t);
    /// get bin content of a given histogram inside the array
    Stat_t   getBinContent(Int_t,Int_t,Int_t,Int_t);
    /// set bin error of a given histogram inside the array
    void     setBinError  (Int_t,Int_t,Int_t,Int_t,Stat_t);
    /// get bin error of a given histogram inside the array
    Stat_t   getBinError  (Int_t,Int_t,Int_t,Int_t);
    /// create the histogram array
    void     create();
    /// draw overlay plot of all histograms
    /// insde the histogram array
    void     drawAll( Bool_t allowHiddenZero=1, Double_t min = -1e200,Double_t max=1e200);
    /// calls drawAll()
    void     draw( Bool_t allowHiddenZero=1 );
    /// reset the histogram array
    void     reset(Int_t level,Int_t count);
    /// add a HOnlineHistArray2 to the  own hists (see TH1::Add)
    void     add(HOnlineMonHistAddon* h2);
    /// get pointer to histogram inside the array
    TH2*     getP(Int_t s=0,Int_t m=0){return ((TH2*) (*array)[s*arraySize2+m]);}

    void     getMinMax(Double_t& min,Double_t& max);
    ClassDef(HOnlineHistArray2,1) // mon hist array
};
#endif  /*!HHISTARRAY_H*/
