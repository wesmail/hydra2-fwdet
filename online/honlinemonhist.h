#ifndef HMONHIST_H
#define HMONHIST_H
using namespace std;
#include "TNamed.h"
#include "TString.h"
#include "TArrayI.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include <iostream> 
#include <iomanip>


/** Base Class of all Histogram classes.
* Contains all setup information of the histogram and the canvas.
*/

class HOnlineMonHistAddon: public TNamed {
protected:
    // hist properties
    Int_t fIsActive;    ///< switch for active/inactive
    Int_t fisResetable; ///< switch for resetable/not resetable
    Int_t refreshrate;  ///< refreshrate of the hist
    Int_t send;         ///< send/don't send to gui
    Int_t nBinsX;       ///< number of bins in X
    Float_t Xlo;        ///< min X range of hist
    Float_t Xup;        ///< max X range of hist
    Int_t nBinsY;       ///< number of bins in Y
    Float_t Ylo;        ///< min Y range of hist
    Float_t Yup;        ///< max Y range of hist
    Int_t arraySize1;   ///< size 1 of 2-Dim array (1 if only 1-Dim is reqested
    Int_t arraySize2;   ///< size 2 of 2-Dim array
    TString XTitle;     ///< X Title of hist
    TString YTitle;     ///< Y Title of hist
    TString ZTitle;     ///< Z Title of hist
    TString Option;     ///< draw option of hist
    TString directory;  ///< output directory of hist
    TString HistType;   ///< 1S,1F,1D,2S,2F,2D hist types
    TString format;     ///< mon,trend,array,trendarray
    Int_t stat;         ///< switch for Stats option of hist
    Int_t max;          ///< max range of hist
    Int_t min;          ///< min range of hist

    Int_t linecol;      ///< line color of hist
    Int_t linestyle;    ///< line style of hist
    Int_t fillcol;      ///< fill color of hist
    Int_t fillstyle;    ///< fill style of hist
    Int_t markercol;    ///< marker color of hist
    Int_t markerstyle;  ///< marker style of hist
    Float_t markersize; ///< marker size of hist

    // canvas properties
    Int_t logX;         ///< log X option of canvas
    Int_t logY;         ///< log Y option of canvas
    Int_t logZ;         ///< log Z option of canvas
    Int_t gridX;        ///< grid X option of canvas
    Int_t gridY;        ///< grid Y option of canvas
    Bool_t drawSame;    ///< draw same option on client side

    TLegend*  legend;   ///< legend for histogram arrays
    Float_t legxmin;    ///< legend for histogram arrays
    Float_t legymin;    ///< legend for histogram arrays
    Float_t legxmax;    ///< legend for histogram arrays
    Float_t legymax;    ///< legend for histogram arrays
    TString legform;    ///< legend for histogram arrays
public:
    HOnlineMonHistAddon (const Char_t* name   ="",
		   const Char_t* title  ="",
		   Int_t active=0,Int_t resetable=0,
		   Int_t nbinsX=0,Float_t xlo=0,Float_t xup=0,
		   Int_t nbinsY=0,Float_t ylo=0,Float_t yup=0);
    /// init data members from Text_t
    HOnlineMonHistAddon (Text_t*);
    virtual  ~HOnlineMonHistAddon(void);
    void     setActive   (Int_t active)  {fIsActive=active;}
    Int_t    getActive   ()              {return fIsActive;}
    void     setResetable(Int_t reset)   {fisResetable=reset;}
    Int_t    getResetable()              {return fisResetable;}
    void     setRefreshRate(Int_t fresh) {refreshrate=fresh;}
    Int_t    getRefreshRate()            {return refreshrate;}
    void     setSend(Int_t i)            {send=i;}
    Int_t    getSend()                   {return send;}
    void     setNBinsX(Int_t i)          {nBinsX=i;}
    Int_t    getNBinsX()                 {return nBinsX;}
    void     setXLo(Float_t i)           {Xlo=i;}
    Float_t  getXLo()                    {return Xlo;}
    void     setXUp(Float_t i)           {Xup=i;}
    Float_t  getXUp()                    {return Xup;}
    void     setNBinsY(Int_t i)          {nBinsY=i;}
    Int_t    getNBinsY()                 {return nBinsY;}
    void     setYLo(Float_t i)           {Ylo=i;}
    Float_t  getYLo()                    {return Ylo;}
    void     setYUp(Float_t i)           {Yup=i;}
    Float_t  getYUp()                    {return Yup;}
    void     setSize1(Int_t i)           {arraySize1=i;}
    Int_t    getSize1()                  {return arraySize1;}
    void     setSize2(Int_t i)           {arraySize2=i;}
    Int_t    getSize2()                  {return arraySize2;}
    void     setStat(Int_t i)            {stat=i;}
    Int_t    getStat()                   {return stat;}
    void     setMax(Int_t i)             {max=i;}
    Int_t    getMax()                    {return max;}
    void     setMin(Int_t i)             {min=i;}
    Int_t    getMin()                    {return min;}
    void     setLineCol(Int_t i)         {linecol=i;}
    Int_t    getLineCol()                {return linecol;}
    void     setLineStyle(Int_t i)       {linestyle=i;}
    Int_t    getLineStyle()              {return linestyle;}
    void     setFillCol(Int_t i)         {fillcol=i;}
    Int_t    getFillCol()                {return fillcol;}
    void     setFillStyle(Int_t i)       {fillstyle=i;}
    Int_t    getFillStyle()              {return fillstyle;}
    void     setMarkerCol(Int_t i)       {markercol=i;}
    Int_t    getMarkerCol()              {return markercol;}
    void     setMarkerStyle(Int_t i)     {markerstyle=i;}
    Int_t    getMarkerStyle()            {return markerstyle;}
    void     setMarkerSize(Float_t i)    {markersize=i;}
    Float_t  getMarkerSize()             {return markersize;}
    void     setLogX(Int_t i)            {logX=i;}
    Int_t    getLogX()                   {return logX;}
    void     setLogY(Int_t i)            {logY=i;}
    Int_t    getLogY()                   {return logY;}
    void     setLogZ(Int_t i)            {logZ=i;}
    Int_t    getLogZ()                   {return logZ;}
    void     setGridX(Int_t i)           {gridX=i;}
    Int_t    getGridX()                  {return gridX;}
    void     setGridY(Int_t i)           {gridY=i;}
    Int_t    getGridY()                  {return gridY;}
    void     setDirectory(TString dir)   {directory=dir;}
    TString  getDirectory()              {return directory;}
    void     setXTitle(TString title)    {XTitle=title;}
    TString  getXTitle()                 {return XTitle;}
    void     setYTitle(TString title)    {YTitle=title;}
    TString  getYTitle()                 {return YTitle;}
    void     setZTitle(TString title)    {ZTitle=title;}
    TString  getZTitle()                 {return ZTitle;}
    void     setOption(TString opt)      {Option=opt;}
    TString  getOption()                 {return Option;}
    void     setHistType(TString type)   {HistType=type;}
    TString  getHistType()               {return HistType;}
    void     setFormat(TString form)     {format=form;}
    TString  getFormat()                 {return format;}
    void     setDrawSame(Bool_t drawS)   { drawSame = drawS;}
    Bool_t   getDrawSame()               {return drawSame;}
    TLegend* getLegend()                 {return legend;}
    void     setLegend(TLegend* leg)     {legend = leg;}
    void     getLegDim(Float_t& xmin,Float_t& ymin,Float_t& xmax,Float_t& ymax,TString& form) {form = legform; xmin=legxmin;xmax=legxmax;ymin=legymin;ymax=legymax;}
    void     setLegDim(Float_t xmin,Float_t ymin,Float_t xmax,Float_t ymax,TString form)      {legform = form;legxmin=xmin;legxmax=xmax;legymin=ymin;legymax=ymax;}
    /// Copy a Addon object to own data members
    void     copySet(HOnlineMonHistAddon&);
    /// extract a Addon object from own data members
    void     getSet(HOnlineMonHistAddon& add);
    /// read all data members from Text_t
    void     read(const Text_t*);
    /// put all data members to a formated Text_t
    void     getText(Text_t* txt);
    /// set the histogram options for TH1
    void     setHist(TH1*);
    /// set the histogram options for TH2
    void     setHist(TH2*);
    /// set the options of the canvas
    void     setCanvas(TCanvas* c);
    HOnlineMonHistAddon* createHist();

    /// interface for all derived objects,
    /// needed in Slot class
    virtual  void draw(Bool_t allowHiddenZero=1);
    /// interface for all derived objects
    /// needed in Slot class
    virtual  void reset(Int_t level,Int_t count);
    /// interface for all derived objects
    /// needed in Slot class
    virtual  void add(HOnlineMonHistAddon*);
    /// interface for all derived objects
    virtual  TH1* getP(Int_t s=0,Int_t m=0);
    virtual void  fill(Stat_t i,Stat_t j = 0);
    virtual void  fill(Int_t,Int_t,Stat_t,Stat_t = 0);
    virtual void  getMinMax(Double_t& min,Double_t& max) { min=max=0; }
    ClassDef(HOnlineMonHistAddon,1) // mon hist addidional features
};


/** Histogram class of 1-dim hists.
* Contains a TH1S, TH1F or TH1D histogram.
* Via getP() the internal histogram can be
* accessed and manipulated as standard histogram.
*/
class HOnlineMonHist: public HOnlineMonHistAddon {
protected:
    TH1* h;
public:
    HOnlineMonHist(const Char_t* name   ="",
		const Char_t* title  ="",
		Int_t active=0,Int_t resetable=0,
		Int_t nbinsX=0,Float_t xlo=0,Float_t xup=0);
    /// create the HOnlineMonHist from a HOnlineMonHistAddon
    HOnlineMonHist(HOnlineMonHistAddon&);
    virtual ~HOnlineMonHist();
    /// get the pointer to the internal histogram (parameters are not used yet. call getP() )
    TH1*    getP(Int_t s=0,Int_t m=0)         {return h;}
    /// reset the internal histogram
    void    reset(Int_t level,Int_t count);
    /// create the internal histogram
    void    create();
    /// draw the internal histogram
    void    draw(Bool_t allowHiddenZero=1);
    /// add the HOnlineMonHist h2 to the own hist
    void    add(HOnlineMonHistAddon* h2);

    void    getMinMax(Double_t& min,Double_t& max);

    ClassDef(HOnlineMonHist,1) // mon hist
};

/** Histogram class of 2-dim hists.
* Contains a TH2S, TH2F or TH2D histogram.
* Via getP() the internal histogram can be
* accessed and manipulated as standard histogram.
*/

class HOnlineMonHist2: public HOnlineMonHistAddon {
protected:
    TH2* h;
public:
    HOnlineMonHist2(const Char_t* name   ="",
		 const Char_t* title  ="",
		 Int_t active=0,Int_t resetable=0,
		 Int_t nbinsX=0,Float_t xlo=0,Float_t xup=0,
		 Int_t nbinsY=0,Float_t ylo=0,Float_t yup=0);
    /// create the HOnlineMonHist2 from a HOnlineMonHistAddon
    HOnlineMonHist2(HOnlineMonHistAddon&);
    ~HOnlineMonHist2();
    /// get the pointer to the internal histogram (parameters are not used yet. call getP() )
    TH2*    getP(Int_t s=0,Int_t m=0)         {return h;}
    /// reset the internal histogram
    void    reset(Int_t level,Int_t count);
    /// create the internal histogram
    void    create();
    /// draw the internal histogram
    void    draw(Bool_t allowHiddenZero=1);
    /// add the HOnlineMonHist2 h2 to the own hist
    void    add(HOnlineMonHistAddon* h2);
    void    getMinMax(Double_t& min,Double_t& max);
    ClassDef(HOnlineMonHist2,1) // mon hist
};
#endif  /*!HMONHIST_H*/
