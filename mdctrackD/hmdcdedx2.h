#ifndef HMDCDEDX2_H
#define HMDCDEDX2_H
#include "TArrayF.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TGraph.h"
#include "hparcond.h"
#include "hlocation.h"

class HParamList;
class HLocation;
class HCategory;
class HMdcSeg;
class HMdcSizesCells;
class HGeantKine;


#define MAX_WIRES 80
#define N_ANGLE 18
#define N_DIST  40
#define N_PARAM  4
#define N_SHIFT_PARAM 2

#define ref_MOD 1
#define ref_ANGLE 90
#define ref_DIST 15

class HMdcDeDx2 : public HParCond {
protected:

     Double_t par     [6][4][N_ANGLE][N_DIST][N_PARAM];       // fitparams  for tot->dedx
     Double_t shiftpar[6][4][N_ANGLE][N_DIST][N_SHIFT_PARAM]; // width of tot distribution
     Double_t parMax  [6][4][N_ANGLE][N_DIST];                // max val for tot in tot->dEdx

     Double_t pargaincorr[6][4][6][220];  // gain correction per wire
     Double_t parmindistcut[4];           // max value for mindist used for calculate dEdx

     static Float_t MaxMdcMinDist[4];   // max value for mindist

     Float_t MinDistStep[4];            // step size for mindist
     Float_t AngleStep;                 // step size for angle
     Double_t hefr;                     // fraction of helium of the gas mix
     Int_t minimumWires;                // minimum required wires
     Float_t window;                    // window for truncating arround mean (units of sigma)

     HMdcSizesCells *sizescells;        //! pointer to HMdcSizesCells container
     HCategory* catcal;                 //! pointer to mdc cal1
     HCategory* cathit;                 //! pointer to mdc hit
     HCategory* catclusinf;             //! pointer to mdc clusinf
     HCategory* catclus;                //! pointer to mdc clusinf
     HLocation loccal;                  //! location object of cal1

     Bool_t isInitialized;              //! flag after init
     Int_t method;                      //! method switch for filling input
     Int_t module;                      //! method switch for filling input for module 1/2 of segment

     TArrayD measurements;              //! array of measurements
     Bool_t  useCalibration;            //! use/don't use normalization table
     Int_t ctskipmod0;                  //! counter for wires skipped with t2<=-998 in mod0 of seg
     Int_t ctskipmod1;                  //! counter for wires skipped with t2<=-998 in mod1 of seg
     static Bool_t debug;                //! kTRUE print infos of trun mean

     void     sort(Int_t);
     UChar_t  fillingInput (HMdcSeg* seg[2], Int_t inputselect = 0);
     UChar_t  select       (Float_t,Float_t,UChar_t,Float_t wind = -99.);
     void     calcSegPoints(HMdcSeg *,Double_t&,Double_t&,Double_t&,Double_t&,Double_t&,Double_t&);
     Double_t calcLength   (Int_t m,Double_t angle,Double_t dist);

 public:
    HMdcDeDx2(const Char_t* name   = "MdcDeDx2",
	      const Char_t* title  = "Mdc lookup for MDC dEdX calculation",
	      const Char_t* context= "MdcDeDx2Production");
    ~HMdcDeDx2();

    Bool_t    initContainer();
    Bool_t    createMaxPar(Bool_t print = kFALSE);

    void      putParams(HParamList*);
    Bool_t    getParams(HParamList*);
    void      clear();
    void      printParam(TString opt = "all");

    //------------set Parameters-----------------------
    void       setFuncPar     (Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t* p,Int_t size);
    void       setFuncPar     (Double_t* p);
    void       setFuncMaxPar  (Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t val);
    void       setFuncMaxPar  (Double_t* p);
    void       setFuncWidthPar(Int_t s,Int_t m,Int_t abin,Int_t dbin,Double_t* p,Int_t size);
    void       setFuncWidthPar(Double_t* p);

    void       setFuncGainPar(Int_t s,Int_t m,Int_t l,Int_t c,Double_t p);
    void       setFuncGainPar(Double_t* p);
    void       setMinDistCutPar(Int_t m,Double_t p);
    void       setMinDistCutPar(Double_t* p);

    Double_t*  getFuncPar     (Int_t s,Int_t m,Int_t abin,Int_t dbin){return &par[s][m][abin][dbin][0];}
    void       getFuncPar     (Double_t* p);
    Double_t   getFuncMaxPar  (Int_t s,Int_t m,Int_t abin,Int_t dbin){return parMax[s][m][abin][dbin];}
    void       getFuncMaxPar  (Double_t* p);
    Double_t*  getFuncWidthPar(Int_t s,Int_t m,Int_t abin,Int_t dbin){return &shiftpar[s][m][abin][dbin][0];}
    void       getFuncWidthPar(Double_t* p);

    Double_t   getFuncGainPar (Int_t s,Int_t m,Int_t l,Int_t c){return pargaincorr[s][m][l][c];}
    void       getFuncGainPar (Double_t* p);
    Double_t   getMinDistCutPar(Int_t m,Double_t p){return parmindistcut[m];}
    void       getMinDistCutPar(Double_t* p);

    Int_t      getN_Param(void)       {return N_PARAM;}
    Int_t      getN_Shift_Param(void) {return N_SHIFT_PARAM;}
    Int_t      getN_Angle(void)       {return N_ANGLE;}
    Int_t      getN_Dist(void)        {return N_DIST;}

    //------------set user values----------------------
    Float_t   getWindow()                    {return window;}
    void      setWindow(Float_t win)         {window = win;}
    Int_t     getMinimumWires()              {return minimumWires;}
    void      setMinimumWires(Int_t minwires){minimumWires = minwires;}
    static void setDebugMode(Bool_t dodebug)   {debug = dodebug;}
    void      setUseCalibration(Bool_t ok)   {useCalibration = ok;}
    Double_t* getArray (Int_t& size)         {size = MAX_WIRES; return measurements.GetArray();}
    Float_t   calcDeDx(HMdcSeg* seg[2],Float_t*,Float_t*,UChar_t*,Float_t*,UChar_t*,
		      Int_t vers = 2,Int_t mod = 2, Bool_t useTruncMean = kTRUE, Float_t truncMeanWindow = -99.,Int_t inputselect = 0);
    //------------conversion------------------------
    Double_t toTTodEdX(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t ToT);
    Double_t dEdXToToT(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t dEdX);
    Double_t toTSigma (Int_t s,Int_t m,Double_t angle,Double_t mindist,Int_t shift = 0);
    Double_t normalize(Int_t s,Int_t m,Int_t l,Int_t c,Double_t angle,Double_t mindist,Double_t ToT);
    void     findBin  (Int_t m,Double_t* angle,Double_t* mindist,Int_t* abin,Int_t* dbin);

    //------------energy loss-----------------------
    void      setHeFraction(Double_t fr)     {hefr = fr;}
    Double_t  getHeFraction()                {return hefr;}
    static Double_t  beta(Int_t id, Double_t p);
    static Double_t  gamma(Int_t id, Double_t p);
    static Double_t  energyLoss     (Int_t id,Double_t p,Double_t hefr = 0.6);
    static TGraph*   energyLossGraph(Int_t id,Double_t hefr = 0.6,TString opt = "p",Bool_t exchange = kFALSE,Int_t markerstyle = 8,Int_t markercolor = 2,Float_t markersize = 0.7);
    static TGraph*   betaGraph(Int_t id,Int_t opt = 1,Int_t markerstyle = 8,Int_t markercolor = 2,Float_t markersize =0.7);
    Double_t         scaledTimeAboveThreshold(HGeantKine* kine = 0,Double_t p = -1,
                                              Float_t t1 = -999, Float_t t2 = -999,Float_t* t2err = 0,
					      Int_t s = 0,Int_t m = 0,Int_t l = 0,Int_t c = 0,Double_t alpha = 0,Double_t mindist = 0);
    ClassDef(HMdcDeDx2,2) //  MDC lookup table (dedx2 normalization parameters)
};
#endif  /*!HMDCDEDX2_H*/
