#ifndef HMDCTRACKPARAM_H
#define HMDCTRACKPARAM_H

#include "hsymmat.h"
#include "hmdcgeomobj.h"
#include "TMatrixD.h"

class HMdcClusFit;
class HMdcWireData;

class HMdcTrackParam : public HMdcLineParam {
  // Track parameters for track fitter:
  protected:
    Double_t funct;             // functional
    Double_t oldFunct;          // previous value of functional
    Double_t sumWeight;         // sum of weights
    Int_t    nParam;            // number of parameters
    HSymMat4 errMatr;           // Matrix of errors
    Int_t    nMods;             // =4 - normal data; =8 - cosmic two sectors

    Int_t    numOfGoodWires;    // number of wires passed fit
    Double_t chi2perDF;         // =funct/(numOfGoodWires-nParam+1)
    Int_t    iterNumb;          // number of iteration

    Int_t    timeOffsetFlag;    // 1 - calc. time offset for each MDC
                                // 2 - calc. time offset for each segment
                                // else - calc. one time offset for all MDC's
    Double_t timeOffset[16];    // time offsets for each MDC in sec.
    Double_t errTimeOffset[16]; // errors of time offsets for each MDC in sec.
    Int_t   isTmoffFixed;       // if bit 0-15 eq.1 timeOffset for mdc is fixed

    // Variable data mambers:
    Double_t sDev[16];          // For timeOffset calculation.
    Double_t sWht[16];          // [4] - for each module.
    Double_t dTdPar[4][16];     // [nParam!][nMod]

  public:
    HMdcTrackParam(void);
    ~HMdcTrackParam(void){}
    void printParam(const Char_t* title=0) const;
    void printFunctChange(const Char_t* title=0) const;
    void printErrors(void);

    inline void copyLine(HMdcTrackParam& tp);
    inline void copyTimeOffsets(const HMdcTrackParam& tp);
    inline void copyAllParam(const HMdcTrackParam& tp);
    void      setTimeOffsets(Double_t* tos,Int_t size=4);
    void      copyNewParam(const HMdcTrackParam& tp);
    void      setOneSecData(void)                  {nMods = 4; cleanTO();}
    void      setTwoSecData(void)                  {nMods = 8; cleanTO();}
    void      setNMods(Int_t ns)                   {nMods = ns<=4 ? ns*4:16; cleanTO();}
    void      copyNMods(HMdcTrackParam& p)         {nMods = p.nMods; cleanTO();}
    void      copyPlanes(HMdcTrackParam& p);

    void      setIterNumb(Int_t it=0)              {iterNumb=it;}
    void      incIterNumb(void)                    {iterNumb++;}
    Int_t     getIterNumb(void) const              {return iterNumb;}
    Int_t     getNumParam(void) const              {return nParam;}

    void      setOldFunct(HMdcTrackParam& par)     {oldFunct=par.funct;} // for print !?
    void      saveFunct(void) {oldFunct=funct;} // for print !?
    void      clearFunct(void);
    void      addToSumsDevWt(Int_t mod, Double_t dev, Double_t wtNorm) {
                             sDev[mod] += dev*wtNorm; sWht[mod] += wtNorm;}
    void      calcTimeOffsets(Int_t tofFlag);
    void      correctMinTimeOffsets(Double_t minTos);

    Double_t* getTimeOffset(void)                  {return timeOffset;}
    Double_t  getTimeOffset(Int_t m) const         {return timeOffset[m];}
    Int_t     isMdcTimeOffsetFixed(Int_t m) const  {return (isTmoffFixed>>m)&1;}
    Int_t     isSegTimeOffsetFixed(Int_t s) const  {return (isTmoffFixed>>(s*2))&3;}
    Int_t     isTrackTimeOffsetFixed(void) const   {return isTmoffFixed ? 1:0;}
    void      getTimeOffsetDer(Double_t* der);
    void      addToTOffsetErr(Int_t m, Double_t* dTdA, Double_t wtNorm);
    void      clearTOffsetDer(void);
    void      addToTOffsetDer(Int_t m, Int_t k, Double_t add) {dTdPar[k][m]+=add;}
    void      addTimeOffsetDer1(TMatrixD& grad2);
    void      addTimeOffsetDer2(TMatrixD& grad2);
    Double_t  getSumWtNorm(Int_t m) const;
    void      calcTimeOffsetsErr(void);
    void      fillErrorsMatr(TMatrixD& matrH);

    void      addToFunct(Double_t add,Double_t wt) {funct+=add; sumWeight+=wt;}
    Double_t  getSumWeight(void) const             {return sumWeight;}
    void      copyParAndAdd(const HMdcTrackParam& tp, Int_t ip, Double_t add);
    void      copyParAndAdd(const HMdcTrackParam& tp, Int_t ip1, Double_t add1,
                            Int_t ip2, Double_t add2);

    Double_t* getErrTimeOffset(void)               {return errTimeOffset;}

    HSymMat4& getErrMatr(void)                     {return errMatr;}
    inline Float_t getParErr(Int_t i) const;
    Double_t  functional(void) const               {return funct;}
    Double_t  getNormFunctional(void) const        {return funct/sumWeight;}
    Double_t  oldFunctional(void) const            {return oldFunct;}
    Int_t     getNumOfGoodWires(void) const        {return numOfGoodWires;}
    Double_t  calcChi2PerDF(Int_t nGWires=0);
    Double_t  getChi2(void)const                   {return chi2perDF;}

    void      fillClusFit(HMdcClusFit* fClusFit);

    inline Bool_t operator > (const HMdcTrackParam& f) const;
    inline Bool_t operator < (const HMdcTrackParam& f) const;
    inline Bool_t operator < (Double_t f) const;
    inline Bool_t operator > (Double_t f) const;
    inline HMdcTrackParam& operator()(const HMdcTrackParam& tp,
        Int_t ip, Double_t add);
    inline HMdcTrackParam& operator()(const HMdcTrackParam& tp,
        Int_t ip1, Double_t add1, Int_t ip2, Double_t add2);

    inline Double_t isFunctRelChangLess(Double_t lim) const;
    Bool_t testParameters(Double_t tosMin, Double_t tosMax);
    void setFixedTimeOffset(Double_t o1,Double_t o2,Double_t o3,Double_t o4);
    void unfixTimeOffset(void) {isTmoffFixed = 0;}

  private:
    Double_t calcTosErr(Int_t m);
    Double_t calcTosErr(Int_t m1, Int_t m2);
    Double_t calcTosErr(void);
    Double_t calcTosErr(Double_t sc1, Double_t sc2, Double_t erri2) {
                        Double_t sc21=sc2/sc1; return sc21*sc21*erri2;}
    void cleanTO(void);

  ClassDef(HMdcTrackParam,0) // Track piece parameters for Dubna fitter
};

inline void HMdcTrackParam::copyAllParam(const HMdcTrackParam& tp) {
  copyNewParam(tp);
  oldFunct = tp.oldFunct;
}

inline void HMdcTrackParam::copyLine(HMdcTrackParam& tp) {
  (HMdcLineParam&)(*this)=(HMdcLineParam&)tp;
  nParam = tp.nParam;
}

inline void HMdcTrackParam::copyTimeOffsets(const HMdcTrackParam& tp) {
  for(Int_t m=0;m<16;m++) timeOffset[m]=tp.timeOffset[m];
  isTmoffFixed = tp.isTmoffFixed;
  nMods        = tp.nMods;
}

inline Float_t HMdcTrackParam::getParErr(Int_t i) const {
  return (i<0||i>=nParam) ? 0.:sqrt(errMatr.getElement(i,i));
}

inline Bool_t HMdcTrackParam::operator > (const HMdcTrackParam& f) const {
  return funct > f.funct;
}

inline Bool_t HMdcTrackParam::operator < (const HMdcTrackParam& f) const {
  return funct < f.funct;
}

inline Bool_t HMdcTrackParam::operator < (Double_t f) const {
  return funct < f;
}

inline Bool_t HMdcTrackParam::operator > (Double_t f) const {
  return funct > f;
}

inline HMdcTrackParam& HMdcTrackParam::operator()(const HMdcTrackParam& tp,
    Int_t ip, Double_t add) {
  copyParam(tp);
  addToParam(ip,add);
  isTmoffFixed = tp.isTmoffFixed;
  nMods        = tp.nMods;
  return *this;
}

inline HMdcTrackParam& HMdcTrackParam::operator()(const HMdcTrackParam& tp,
    Int_t ip1, Double_t add1, Int_t ip2, Double_t add2) {
  copyParam(tp);
  addToParam(ip1,add1,ip2,add2);
  isTmoffFixed = tp.isTmoffFixed;
  nMods        = tp.nMods;
  return *this;
}

inline Double_t HMdcTrackParam::isFunctRelChangLess(Double_t lim) const {
  return fabs((oldFunct-funct)/oldFunct) < lim;
}

#endif /* MDCTRACKPARAM_H */
