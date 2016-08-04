#ifndef HMDCCLUSINF_H
#define HMDCCLUSINF_H

#include "TObject.h"

class HMdcClusInf : public TObject {
protected:
  Char_t fitStatus;       // 0 - HMdcHit filled by cluster, 1 - by fit
  Char_t levelClFinding;  // level of cl.finding (minimal num. of layers)

  Char_t nModsClus;       // number of modules in cluster
  Char_t nModsFit;        // number of modules in fit

  Short_t clusSize;       // number of bins in cluster
  Short_t nMergClus;      // number of merged clusters
  Short_t nLayers;        // number of layers in cluster
  Short_t nLayersFitted;  // number of layers which contributed to fit
  Short_t nDrTimes;       // number of drift times in cluster
  Short_t nDrTimesFitted; // number of drift times which contributed to fit
  Short_t indClus;        // index of HMdcClus container
  Short_t indClusFit;     // index of HMdcClusFit container
  Float_t sigma1;         // sigma1,sigma2,alpha - cluster shape
  Float_t sigma2;         // sigma1 - long axis, sigma2 - short axis
  Float_t alpha;          // alpha - angle between long axis of cluster and X
  
  Float_t xHit;           // x-coord.of hit in MDC coor.sys. (== x in HMdcHit)
  Float_t yHit;           // y-coord.of hit in MDC coor.sys. (== y in HMdcHit)
  Float_t xClus;          // x-coord.of comb. cluster on the proj.plane
  Float_t yClus;          // y-coord.of comb. cluster on the proj.plane

  Float_t dedx;           // mean value of t2-t1 for segment
  Float_t dedxSigma;      // sigma of t2-t1 distribution in segment
  UChar_t dedxNWire;      // number of wires in segment before truncated mean procedure
  UChar_t dedxNWireCut;   // number of wires in segment cutted by truncated mean procedure

public:
    HMdcClusInf(void) : clusSize(0), nMergClus(0), nLayers(0), nLayersFitted(0),
        nDrTimes(0), nDrTimesFitted(0), indClus(-1), indClusFit(-1), dedx(0),
        dedxSigma(0), dedxNWire(0), dedxNWireCut(0) {;}
    ~HMdcClusInf(void) {;}
    void clear(void) {
	clusSize=nMergClus=nDrTimes=0;
	indClus=indClusFit=-1;
	dedx=dedxSigma=dedxNWire=dedxNWireCut=nLayers=nLayersFitted=0;
        xHit=yHit=xClus=yClus=0.;
    }
  void setIsFitted(void) {fitStatus=1;}
  void setIsNotFitted(void) {fitStatus=0;}
  void setFitStat(const Char_t st) {fitStatus=st;}
  void setLevelClFinding(const Char_t lv) {levelClFinding=lv;}
  void setNModInCl(const Char_t nm) {nModsClus=nm;}
  void setNModInFit(const Char_t nm) {nModsFit=nm;}
  void setClusSize(const Short_t sz) {clusSize=sz;}
  void setNMergClust(const Short_t nmc) {nMergClus=nmc;}
  void setNLayers(const Short_t nl) {nLayers=nl;}
  void setNLayersFitted(const Short_t nlf) {nLayersFitted=nlf;}
  void setNDrTimes(const Short_t ndt) {nDrTimes=ndt;}
  void setNDrTimesFitted(const Short_t nwf) {nDrTimesFitted=nwf;}
  void setClusIndex(const Short_t ind) {indClus=ind;}
  void setClusFitIndex(const Short_t ind) {indClusFit=ind;}
  void setSigma1(const Float_t par){sigma1=par;}
  void setSigma2(const Float_t par){sigma2=par;}
  void setAlpha(const Float_t al) {alpha=al;}
  void setShape(const Float_t s1,const Float_t s2,const Float_t al)
    {sigma1=s1; sigma2=s2;alpha=al;}

  void setdedx(Float_t dedxval){dedx=dedxval;}
  void setSigmadedx(Float_t sig){dedxSigma=sig;}
  void setNWirededx(UChar_t nw){dedxNWire=nw;}
  void setNWireCutdedx(UChar_t nw){dedxNWireCut=nw;}
  
  void setXYHit(Float_t x,Float_t y)  {xHit=x; yHit=y;} 
  void setXYClus(Float_t x,Float_t y) {xClus=x; yClus=y;}

  Float_t getdedx(){return dedx;}
  Float_t getSigmadedx(){return dedxSigma;}
  UChar_t getNWirededx(){return dedxNWire;}
  UChar_t getNWireCutdedx(){return dedxNWireCut;}
  Char_t  getFitStat(void) {return fitStatus;}
  Char_t  getLevelClFinding(void) {return levelClFinding;}
  Char_t  getNModInCl(void) {return nModsClus;}
  Char_t  getNModInFit(void) {return nModsFit;}
  Int_t   getClusSize(void) {return clusSize;}
  Short_t getNMergClust(void) {return nMergClus;}
  Short_t getNLayers() {return nLayers;}
  Short_t getNLayersFitted() {return nLayersFitted;}
  Short_t getNDrTimes(void) {return nDrTimes;}
  Short_t getNDrTimesFitted(void) {return nDrTimesFitted;}
  Short_t getClusIndex(void) {return indClus;}
  Short_t getClusFitIndex(void) {return indClusFit;}
  Float_t getSigma1(void) {return sigma1;}
  Float_t getSigma2(void) {return sigma2;}
  Float_t getAlpha(void) {return alpha;}
  void getShape(Float_t& s1,Float_t& s2,Float_t& al)
    {s1=sigma1; s2=sigma2; al=alpha;}
  Float_t getRatio(void) {return (sigma1>0.) ? sigma2/sigma1 : 0.;}
  Bool_t isClParEqual(HMdcClusInf *cf) {
    if(clusSize!=cf->clusSize || nDrTimes!=cf->nDrTimes || 
        nMergClus != cf->nMergClus || alpha!=cf->alpha) return kFALSE;
    return kTRUE;
  }
  
  Float_t getXHit(void)  {return xHit;}
  Float_t getYHit(void)  {return yHit;}
  Float_t getXClus(void) {return xClus;}
  Float_t getYClus(void) {return yClus;}

  virtual void print(void) const;

  ClassDef(HMdcClusInf,1) // information about clusters in MDC
};

#endif  /* HMDCCLUSINF_H */
