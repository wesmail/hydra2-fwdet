#ifndef HMDCDRIFTTIMEPAR_H
#define HMDCDRIFTTIMEPAR_H

#include "TObjArray.h"

class HMdcCal2ParSim;
class HMdcCal2ParSecSim;
class HMdcCal2ParModSim;
class HMdcCal2ParAngleSim;
class HMdcCal2Par;
class HMdcCal2ParSec;
class HMdcCal2ParMod;
class HMdcCal2ParAngle;

class HMdcDriftTimeParBin : public TObject {
  protected:
    Double_t c0;     // Parametrization on drift time versus distance (HMdcCal2ParSim)
    Double_t c1;     // and impact angle (alpha):
    Double_t c2;     // driftTime = c0 + c1*alpha + c2*dist + c3*alpha*dist
    Double_t c3;
    Double_t c0err;  // Parametrization on drift time error versus distance
    Double_t c1err;  // and impact angle (alpha):
    Double_t c2err;  // driftTimeErr =
    Double_t c3err;  //    c0err + c1err*alpha + c2err*dist + c3err*alpha*dist
    
    Double_t d0;     // Parametrization on distance versus drift time (HMdcCal2Par)
    Double_t d1;     // and impact angle (alpha):
    Double_t d2;     // distance = d0 + d1*alpha + d2*dist + d3*alpha*driftTime
    Double_t d3;
  public:
    HMdcDriftTimeParBin(void);
    ~HMdcDriftTimeParBin(void) {}
    void setC0(Double_t k)    {c0 = k;}
    void setC1(Double_t k)    {c1 = k;}
    void setC2(Double_t k)    {c2 = k;}
    void setC3(Double_t k)    {c3 = k;}
    void setC0Err(Double_t k) {c0err = k;}
    void setC1Err(Double_t k) {c1err = k;}
    void setC2Err(Double_t k) {c2err = k;}
    void setC3Err(Double_t k) {c3err = k;}

    inline Double_t calcTime(Double_t a,Double_t d);
    inline Double_t calcTimeErr(Double_t a,Double_t d);
    inline void coeffForDer(Double_t a,Double_t d, 
        Double_t& coeffDAlpha,Double_t& coeffDDist,Double_t& coeffDAlphaDDist);
    inline Double_t coeffDAlphaDPar(Double_t d);
    inline Double_t coeffDDistDPar(Double_t a);
    void print(void) const;
    void printTable(void) const;

    void setD0(Double_t k) {d0 = k;}
    void setD1(Double_t k) {d1 = k;}
    void setD2(Double_t k) {d2 = k;}
    void setD3(Double_t k) {d3 = k;}
    inline Double_t calcDistance(Double_t a,Double_t t);
    
  ClassDef(HMdcDriftTimeParBin,0) // parametrization of one bin
};

class HMdcDriftTimeParAngle : public TObject { //public TObjArray {
  protected:
    HMdcDriftTimeParBin arr[100];
  public:
    HMdcDriftTimeParAngle(void) {}
    ~HMdcDriftTimeParAngle(void) {}
    HMdcDriftTimeParBin* at(Int_t b) {return arr+b;}
  private:

  ClassDef(HMdcDriftTimeParAngle,0) // array of HMdcDriftTimeParBin objects
};

class HMdcDriftTimeParMod : public TObject {
  protected:
    HMdcDriftTimeParAngle arr[18];
    Int_t    nAngleBins;      // number of angle bins
    Int_t    lAngleBin;       // index of last bin in angle array (= nAngleBins-1)
    Double_t angleBinSize;    // angle bin size
    Double_t invAngleBinSize; // 1./angle bin size
    Double_t lastAngleBin;    // = lAngleBin*nAngleBins
    Int_t    nDistBins;       // number of distance bins
    Int_t    lDistBin;        // index of last bin in dist. array (= nDistBins-1)
    Double_t distBinSize;     // distance bin size
    Double_t invDistBinSize;  // 1./distance bin size
    Double_t lastBinDist;     // distance for last bin (=lDistBin*distBinSize)
    Double_t slopeOutside;    // slope for outside of table tracks
    
    // nTimeBins = nDistBins !
    Double_t timeBinSize;     // time bin size
    Double_t invTimeBinSize;  // 1./time bin size
    Double_t lastBinTime;     // time for last bin (=lTimeBin*distBinSize)
  public:
    HMdcDriftTimeParMod(Int_t nAnBins=18, Int_t nDsBins=100);
    ~HMdcDriftTimeParMod(void) {}
    Int_t    getNAngleBins(void) const   {return nAngleBins;}
    Double_t getAngleBinSize(void) const {return angleBinSize;}
    Int_t    getNDistBins(void) const    {return nDistBins;}
    Double_t getDistBinSize(void) const  {return distBinSize;}
    inline HMdcDriftTimeParBin* getBinDist(Double_t a, Double_t d);
    inline Int_t    calcAngleBinInd(Double_t a) const;
    inline Int_t    calcDistBinInd(Double_t d) const;
    inline Double_t calcTime(Double_t a,Double_t d);
    inline Double_t calcTimeErr(Double_t a,Double_t d);
    Bool_t testSizes(Int_t anSz, Int_t dsSz) const;
    void setDistBinSize(Double_t dBSize);
    HMdcDriftTimeParAngle* at(Int_t a) {return arr+a;}
    HMdcDriftTimeParAngle* getAngBins(Double_t al) {return arr+calcAngleBinInd(al);}
    Bool_t initContainer(HMdcCal2ParModSim& fC2PModS,HMdcCal2ParMod& fC2PMod,Double_t slOut,Double_t scaleErr);
    void print(void);
    
    Int_t    getNTimeBins(void) const    {return nDistBins;}
    Double_t getTimeBinSize(void) const  {return timeBinSize;}
    inline HMdcDriftTimeParBin* getBinTime(Double_t a, Double_t d);
    inline Int_t    calcTimeBinInd(Double_t d) const;
    inline Double_t calcDistance(Double_t a,Double_t d);
    void setTimeBinSize(Double_t tBSize);
    
  private:
    void fillDriftTime(HMdcCal2ParAngleSim& rAng1,HMdcCal2ParAngleSim& rAng2,
        HMdcDriftTimeParBin* bin,Int_t anBin, Int_t dsBin);
    void fillDriftTimeErr(HMdcCal2ParAngleSim& rAng1,HMdcCal2ParAngleSim& rAng2,
        HMdcDriftTimeParBin* bin,Int_t anBin, Int_t dsBin,Double_t scaleErr);

    void fillDriftDist(HMdcCal2ParAngle& rAng1,HMdcCal2ParAngle& rAng2,
        HMdcDriftTimeParBin* bin,Int_t anBin, Int_t tmBin);
  
  ClassDef(HMdcDriftTimeParMod,0) // array of HMdcDriftTimeParAngle objects
};

class HMdcDriftTimeParSec : public TObjArray {
  // array of pointers of type HMdcDriftTimeParMod
  protected:
    Int_t sector;
  public:
    HMdcDriftTimeParSec(void);
    ~HMdcDriftTimeParSec(void);
    inline HMdcDriftTimeParBin* getBinDist(Int_t m, Double_t a, Double_t d);
    Bool_t initContainer(HMdcCal2ParSecSim& fC2PSecS,HMdcCal2ParSec& fC2PSec,
                         Int_t sec,Double_t slOut,Double_t* scaleError);
    HMdcDriftTimeParMod* at(Int_t m) const {return (HMdcDriftTimeParMod*)At(m);}
    inline Double_t calcTime(Int_t m,Double_t a,Double_t d);
    inline Double_t calcTimeErr(Int_t m,Double_t a,Double_t d);
    void print(void);
    
    inline HMdcDriftTimeParBin* getBinTime(Int_t m, Double_t a, Double_t t);
    inline Double_t calcDistance(Int_t m,Double_t a,Double_t t);
    
  private:

  ClassDef(HMdcDriftTimeParSec,0) // array of HMdcDriftTimeParMod objects
};

class HMdcDriftTimePar : public TObjArray {
  // array of pointers of type HMdcDriftTimeParSec
  protected:
    static HMdcDriftTimePar* fMdcDriftTimePar;
    HMdcCal2ParSim *pCal2ParSim;   // pointer to the HMdcCal2ParSim parameters
    HMdcCal2Par    *pCal2Par;      // pointer to the HMdcCal2Par parameters
    Bool_t          isInited;
    Double_t        scaleError[4]; //! scaler for error of time per module type
  public:
    static HMdcDriftTimePar* getExObject(void)          {return fMdcDriftTimePar;}
    static HMdcDriftTimePar* getObject(void);
    static void              deleteCont(void);
    inline HMdcDriftTimeParBin* getBinDist(Int_t s,Int_t m,Double_t a,Double_t d);
    Bool_t initContainer(void);
    HMdcDriftTimeParSec* at(Int_t s) const {return (HMdcDriftTimeParSec*)At(s);}
    inline Double_t calcTime(Int_t s,Int_t m,Double_t a,Double_t d);
    inline Double_t calcTimeErr(Int_t s,Int_t m,Double_t a,Double_t d);
    void print(void);
    
    inline HMdcDriftTimeParBin* getBinTime(Int_t s,Int_t m,Double_t a,Double_t t);
    inline Double_t calcDistance(Int_t s,Int_t m,Double_t a,Double_t t);
    void   setScalerTime1Err(Double_t m0,Double_t m1,Double_t m2,Double_t m3);
  protected:
    HMdcDriftTimePar(void);
    ~HMdcDriftTimePar(void);

  ClassDef(HMdcDriftTimePar,0) // array of HMdcDriftTimeParSec objects
};

//==================================================

inline Double_t HMdcDriftTimeParBin::calcTime(Double_t a,Double_t d) {
  if(a<0.) a = -a;
  if(d<0.) d = -d;
  return c0+c1*a+(c2+c3*a)*d;
}

inline void HMdcDriftTimeParBin::coeffForDer(Double_t a,Double_t d,
    Double_t& coeffDAlpha,Double_t& coeffDDist,Double_t& coeffDAlphaDDist) {
  // For calcul. of analytical derivative
  // dDrTime/dPar=coeffDAlpha*dAlpha/dPar + coeffDDist*dDist/dPar
  coeffDAlpha      = c1+c3*d;
  coeffDDist       = c2+c3*a;
  coeffDAlphaDDist = c3;  // =coeffDDistDAlpha
}

inline Double_t HMdcDriftTimeParBin::coeffDAlphaDPar(Double_t d) {
  // return coeff=dDrTime/dDist
  return c1+c3*d;
}

inline Double_t HMdcDriftTimeParBin::coeffDDistDPar(Double_t a) {
  // return coeff=dDrTime/dDist
  return c2+c3*a;
}

inline Double_t HMdcDriftTimeParMod::calcTime(Double_t a,Double_t d) {
  return getBinDist(a,d)->calcTime(a,d);
}

inline Double_t HMdcDriftTimeParSec::calcTime(Int_t m, Double_t a, Double_t d) {
  return getBinDist(m,a,d)->calcTime(a,d);
}

inline Double_t HMdcDriftTimePar::calcTime(Int_t s, Int_t m,
    Double_t a,Double_t d) {
  return getBinDist(s,m,a,d)->calcTime(a,d);
}


inline Double_t HMdcDriftTimeParBin::calcTimeErr(Double_t a,Double_t d) {
  if(a<0.) a = -a;
  if(d<0.) d = -d;
  return c0err+c1err*a+c2err*d+c3err*a*d;
}

inline Double_t HMdcDriftTimeParMod::calcTimeErr(Double_t a,Double_t d) {
  return getBinDist(a,d)->calcTimeErr(a,d);
}

inline Double_t HMdcDriftTimeParSec::calcTimeErr(Int_t m,
    Double_t a,Double_t d) {
  return getBinDist(m,a,d)->calcTimeErr(a,d);
}

inline Double_t HMdcDriftTimePar::calcTimeErr(Int_t s, Int_t m,
    Double_t a,Double_t d) {
  return getBinDist(s,m,a,d)->calcTimeErr(a,d);
}

inline Int_t HMdcDriftTimeParMod::calcAngleBinInd(Double_t a) const {
  a = 90.-a;
  if(a<0.) a = -a;
  return a<lastAngleBin ? Int_t(a*invAngleBinSize) : lAngleBin;
}

inline Int_t HMdcDriftTimeParMod::calcDistBinInd(Double_t d) const {
  if(d<0.) d = -d;
  return d<lastBinDist ? Int_t(d*invDistBinSize):lDistBin;
}

inline HMdcDriftTimeParBin* HMdcDriftTimeParMod::getBinDist(Double_t a,Double_t d) {
  HMdcDriftTimeParAngle* pAngl=at(calcAngleBinInd(a));
  if(pAngl) return pAngl->at(calcDistBinInd(d));
  return 0;
}

inline HMdcDriftTimeParBin* HMdcDriftTimeParSec::getBinDist(Int_t m, Double_t a, Double_t d) {
  HMdcDriftTimeParMod* pMod=at(m);
  if(pMod) return pMod->getBinDist(a,d);
  return 0;
}

inline HMdcDriftTimeParBin* HMdcDriftTimePar::getBinDist(Int_t s, Int_t m, Double_t a, Double_t d) {
  HMdcDriftTimeParSec* pSec=at(s);
  if(pSec) return pSec->getBinDist(m,a,d);
  return 0;
}

//==================================================

inline Double_t HMdcDriftTimeParBin::calcDistance(Double_t a,Double_t t) {
  if(a<0.) a = -a;
  if(t<=0.) return d0+d1*a;
  return d0+d1*a+(d2+d3*a)*t;
}

inline Double_t HMdcDriftTimeParMod::calcDistance(Double_t a,Double_t t) {
  return getBinTime(a,t)->calcDistance(a,t);
}

inline Double_t HMdcDriftTimeParSec::calcDistance(Int_t m, Double_t a, Double_t t) {
  return getBinTime(m,a,t)->calcDistance(a,t);
}

inline Double_t HMdcDriftTimePar::calcDistance(Int_t s, Int_t m,Double_t a,Double_t t) {
  return getBinTime(s,m,a,t)->calcDistance(a,t);
}

inline Int_t HMdcDriftTimeParMod::calcTimeBinInd(Double_t t) const {
  if(t <= 0.) return 0;
 return t<lastBinTime ? Int_t(t*invTimeBinSize) : lDistBin;
}

inline HMdcDriftTimeParBin* HMdcDriftTimeParMod::getBinTime(Double_t a,Double_t t) {
  HMdcDriftTimeParAngle* pAngl=at(calcAngleBinInd(a));
  if(pAngl) return pAngl->at(calcTimeBinInd(t));
  return 0;
}

inline HMdcDriftTimeParBin* HMdcDriftTimeParSec::getBinTime(Int_t m, Double_t a, Double_t t) {
  HMdcDriftTimeParMod* pMod=at(m);
  if(pMod) return pMod->getBinTime(a,t);
  return 0;
}

inline HMdcDriftTimeParBin* HMdcDriftTimePar::getBinTime(Int_t s, Int_t m, Double_t a, Double_t t) {
  HMdcDriftTimeParSec* pSec=at(s);
  if(pSec) return pSec->getBinTime(m,a,t);
  return 0;
}

#endif  /* HMDCDRIFTTIMEPAR_H */

