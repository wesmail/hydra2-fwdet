#ifndef HMDCCLUSFIT_H
#define HMDCCLUSFIT_H

#include "TObject.h"
#include <math.h>

class HMdcClusFit : public TObject {
protected:
  // Address:
  Char_t  sec;          // sector [0-5]
  Char_t  seg;          // =0 or 1 segment fit, =3 sector fit
  Char_t  mod;          // =0,1,2,3 - module; =-1 - segment fit

  Char_t  fitAuthor;   // fit version, =1 - old Alexander's fit version
  Char_t  distTimeVer; // 0-calc. time by polinom., 1-by J.Market's func.
  Char_t  fitType;     // 0 - segment fit, 1 - mdc fit 
  Char_t  nParam;      // number of parameters
  
  // Fit results:
  Float_t functional;  // value of functional
  Float_t timeOffMdc1; // time offset for each MDC in sec. [ns]
  Float_t timeOffMdc2; // if timeOff==-1000. no wires in this mod.
  Float_t timeOffMdc3; //
  Float_t timeOffMdc4; //
  Short_t numOfWires;  // Num. of wires with weight > weight_min
  Char_t  numOfLayers; // Num. of layers with wire weight > weight_min
  Float_t x1;          // Track parameters: [mm]
  Float_t y1;          // Track is line (x1,y1,z1) - (x2,y2,z2)
  Float_t z1;          // in sector coor. system.
  Float_t x2;          //
  Float_t y2;          //
  Float_t z2;          //
  Short_t numIter;     // number of iterations
  Int_t   indf;        // index of the first HMdcWireFit obj. in catMdcWireFit
  Int_t   indl;        // index of the last HMdcWireFit obj. in catMdcWireFit
  Int_t   clustIndex;  // >=0 - cluster index in clus.category
                       // =-1 - index not defined
                       // <-1 - old data (ver.<3): locClus[2]=-(clusterindex+2)
  Char_t  exitFlag;    // flag of track fitter exit [1-3]
  Float_t sigmaChi2;   // final sigma of chi2 distr. which was
                       // used for weights calc. and filtering
  Bool_t  fitStatus;   // kTRUE if fit result is accepted
  
  Float_t dx1;         // x1 error
  Float_t dy1;         // y1 error
  Float_t dx2;         // x2 error
  Float_t dy2;         // y2 error

  enum {kIsFake = BIT(14)}; // bit in TObject::fBits for "fake" segment flag
public:
  HMdcClusFit(){clear();}
  ~HMdcClusFit(){;}
  inline void clear();
  
  void setAddress(Char_t sc, Char_t sg, Char_t md) {sec=sc; seg=sg; mod=md;}
  void setSec(Char_t sc)           {sec = sc;}
  void setIOSeg(Char_t sg)         {seg = sg;}
  void setMod(Char_t m)            {mod = m;}
  void setFitAuthor(Char_t v)      {fitAuthor = v;}
  void setFitVersion(Char_t v)     {fitAuthor = v;}
  void setDistTimeVer(Char_t v)    {distTimeVer = v;}
  void setFitType(Char_t v)        {fitType = v;}
  void setNParam(Char_t v)         {nParam = v;}
  void setFunMin(Float_t v)        {functional = v;}
  void setNumOfWires(Int_t v)      {numOfWires = v;}
  void setNumOfLayers(Int_t v)     {numOfLayers = v;}
  void setX1(Float_t v)            {x1 = v;}
  void setY1(Float_t v)            {y1 = v;}
  void setZ1(Float_t v)            {z1 = v;}
  void setX2(Float_t v)            {x2 = v;}
  void setY2(Float_t v)            {y2 = v;}
  void setZ2(Float_t v)            {z2 = v;}
  void setNumIter(Short_t v)       {numIter = v;}
  void setFirstWireFitInd(Int_t v) {indf = v;}
  void setLastWireFitInd(Int_t v)  {indl = v;}
  void setClustCatIndex(Int_t v)   {clustIndex = v;}
  void setExitFlag(Char_t fl)      {exitFlag = fl;}
  void setSigmaChi2(Float_t v)     {sigmaChi2 = v;}
  void setFitStatus(Bool_t stat)   {fitStatus = stat;}
  void setTimeOff(const Double_t* tos);
  void setErrors(Float_t x1e,Float_t y1e,Float_t x2e,Float_t y2e) {dx1 = x1e; dy1 = y1e;
                                                                   dx2 = x2e; dy2 = y2e;}
  
  Char_t  getSec(void) const             {return sec;}
  Char_t  getIOSeg(void) const           {return seg;}
  Char_t  getMod(void) const             {return mod;}
  Char_t  getFitAuthor(void) const       {return fitAuthor;}
  Char_t  getFitVersion(void) const      {return fitAuthor;}
  Char_t  getDistTimeVer(void) const     {return distTimeVer;}
  Char_t  getFitType(void) const         {return fitType;}
  Char_t  getNParam(void) const          {return nParam;}
  Float_t getFunMin(void) const          {return functional;}
  Float_t getTimeOff(Int_t mod) const;
  Float_t getTimeOffset(Int_t mod) const {return getTimeOff(mod);}
  Float_t getTimeOffsetMdc1(void) const  {return timeOffMdc1;}
  Float_t getTimeOffsetMdc2(void) const  {return timeOffMdc2;}
  Float_t getTimeOffsetMdc3(void) const  {return timeOffMdc3;}
  Float_t getTimeOffsetMdc4(void) const  {return timeOffMdc4;}
  Short_t getNumOfWires(void) const      {return numOfWires;}
  Short_t getTotNumOfWires(void) const   {return indl-indf+1;}
  Char_t  getNumOfLayers(void) const     {return numOfLayers;}
  Float_t getX1(void) const              {return x1;}
  Float_t getY1(void) const              {return y1;}
  Float_t getZ1(void) const              {return z1;}
  Float_t getX2(void) const              {return x2;}
  Float_t getY2(void) const              {return y2;}
  Float_t getZ2(void) const              {return z2;}
  Float_t getX1err(void) const           {return dx1;}
  Float_t getY1err(void) const           {return dy1;}
  Float_t getX2err(void) const           {return dx2;}
  Float_t getY2err(void) const           {return dy2;}
  Short_t getNumIter(void) const         {return numIter;}
  Int_t   getIndf(void) const            {return indf;}
  Int_t   getIndl(void) const            {return indl;}
  Int_t   getFirstWireFitInd(void) const {return indf;}
  Int_t   getLastWireFitInd(void) const  {return indl;}
  Int_t   getClustCatIndex(void) const   {return clustIndex;}
  Float_t getPhi(void) const             {return atan2(y2-y1,x2-x1);}
  Char_t  getExitFlag(void) const        {return exitFlag;}
  Float_t getSigmaChi2(void) const       {return sigmaChi2;}
  Bool_t  getFitStatus(void) const       {return fitStatus;}
  inline Float_t getChi2(void) const;
  inline void    getIndexis(Int_t &indexf,Int_t &indexl) const;
  inline void    getAddress(Int_t& sc, Int_t& sg, Int_t& md) const;
  inline Float_t getTheta(void) const;
  inline Int_t   getNextIndex(Int_t ind) const;
  
  void calcRZtoLineXY(Float_t &zm, Float_t &r0,
    Float_t x1, Float_t y1, Float_t z1, Float_t x2, Float_t y2, Float_t z2, 
    Float_t xBeam=0., Float_t yBeam=0.) const;
  void getRZmin(Float_t &zm, Float_t &r0,Float_t xBm=0., Float_t yBm=0.) const {
    calcRZtoLineXY(zm,r0,x1,y1,z1,x2,y2,z2,xBm,yBm);}
  void   setFakeFlag(void)     { SetBit(kIsFake); }
  void   resetFakeFlag(void)   { ResetBit(kIsFake); }
  Bool_t isFake(void) const    { return TestBit(kIsFake); }

  virtual void print(void) const;
  virtual Bool_t isGeant() const {return kFALSE;}

  ClassDef(HMdcClusFit,2) // track fit result
};

inline void HMdcClusFit::clear() {
  indf = -1;
  indl = -1;
  sec  = -1;
  seg  = -1;
  mod  = -1;
}

inline void HMdcClusFit::getIndexis(Int_t &indexf,Int_t &indexl) const {
  indexf = indf;
  indexl = indl;
}
inline void HMdcClusFit::getAddress(Int_t& sc, Int_t& sg, Int_t& md) const {
  sc = sec;
  sg = seg; 
  md = mod;
}

inline Float_t HMdcClusFit::getTheta(void) const {
  return atan2(sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)),z2-z1);
}

inline Int_t HMdcClusFit::getNextIndex(Int_t ind) const {
  //using: ind=-1;ind=getNextIndexChild(ind)
  return (ind<indf) ? indf:((ind<indl) ? ind+1:-1);
}

inline Float_t HMdcClusFit::getChi2(void) const {
  return (numOfWires>nParam) ? functional/(numOfWires-nParam):-1.;
}
 
#endif
