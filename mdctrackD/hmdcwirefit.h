#ifndef HMDCWIREFIT_H
#define HMDCWIREFIT_H

#include "TObject.h"
#include "TMath.h"

class HMdcWireFit : public TObject {
protected:
  // Address:
  Char_t  sec;           // sector
  Char_t  mod;           // MDC module
  Char_t  layer;         // MDC layer
  Short_t cell;          // MDC wire
  Char_t  timeNum;       // =1 or 2 (time1 or time2 from HMdcCal1)
  
  // Fit results:
  Float_t tdcTimeCal1;   // drift time from HMdcCal1
  Float_t tdcTime;       // tdcTimeCal1 minus time of wire offset
  Float_t dev;           // fullTime-tdcTime
  Float_t weight;        // wire weight. =0 if wire was excluded from fit
  Float_t driftTime;     // calculated drift time
  Float_t fullTime;      // driftTime + time offset
  Float_t minDist;       // min.distance from track to wire with SIGN.
                         // >0 - track pass OVER wire (in mdc coor.sys.)
  Float_t alpha;         // impact angle in cell system
  Bool_t  isInCell;      // =kFALSE if track don't cross cell
  Float_t tdcTimeErr;    // error of drift time.
  Bool_t  isUsedInFit;   // kTRUE if wire was used in fit (for alignment investigation)
  
  Float_t tot;           // time above threshold (t2-t1)
  Float_t cellPath;      // track path in this cell

public:
  HMdcWireFit()  {clear();}
  ~HMdcWireFit() {;}
  inline void clear();

  inline void setAddress(Char_t s,Char_t m,Char_t l,Short_t c);
  void    setSec(Char_t s)           {sec=s;}
  void    setMod(Char_t m)           {mod=m;}
  void    setLayer(Char_t l)         {layer=l;}
  void    setCell(Short_t c)         {cell=c;}
  void    setTimeNum(Char_t t)       {timeNum=t;}
  void    setDev(Float_t v)          {dev=v;}
  void    setWeight(Float_t v)       {weight=v;}
  void    setIsUsedFlag(Bool_t f)    {isUsedInFit=f;}
  void    setDriftTime(Float_t v)    {driftTime=v;}
  void    setFullTime(Float_t v)     {fullTime=v;}
  void    setCal1Time(Float_t v)     {tdcTimeCal1=v;}
  void    setTdcTime(Float_t v)      {tdcTime=v;}
  void    setMinDist(Float_t v)      {minDist=v;}
  void    setAlpha(Float_t v)        {alpha=v;}
  void    setIsInCell(Bool_t v)      {isInCell=v;}
  void    setTdcTimeErr(Float_t v)   {tdcTimeErr=v;}
  void    setToT(Float_t v)          {tot = v;}
  void    setCellPath(Float_t v)     {cellPath = v;}

  inline void getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c) const;
  Char_t  getSec(void) const         {return sec;}
  Char_t  getMod(void) const         {return mod;}
  Char_t  getLayer(void) const       {return layer;}
  Short_t getCell(void) const        {return cell;}
  Char_t  getTimeNum(void) const     {return timeNum;}
  Float_t getDev(void) const         {return dev;}
  Float_t getWeight(void) const      {return weight;}
  Bool_t  isWrUsedInFit(void) const  {return isUsedInFit;}
  Float_t getDriftTime(void) const   {return driftTime;}
  Float_t getFullTime(void) const    {return fullTime;}
  Float_t getTimeOffset(void) const  {return fullTime-driftTime;}
  Float_t getTdcTime(void) const     {return tdcTime>0.f ? tdcTime : 0.f;}
  Float_t getTdcTimeF(void) const    {return tdcTime;}
  Float_t getSignalTime(void) const  {return tdcTimeCal1-tdcTime;}
  Float_t getCal1Time(void) const    {return tdcTimeCal1;}
  Float_t getMinDist(void) const     {return TMath::Abs(minDist);}
  Bool_t  isUnderWire(void) const    {return minDist < 0.;}
  Bool_t  isOverWire(void) const     {return minDist >= 0.;}
  Float_t getAlpha(void) const       {return alpha;}
  Bool_t  getIsInCell(void) const    {return isInCell;}
  Float_t getTdcTimeErr(void)const   {return tdcTimeErr;}
  Float_t getChi2(void) const        {Float_t c=dev/tdcTimeErr; return c*c;}
  Float_t getToT(void) const         {return tot;}
  Float_t getCellPath(void) const    {return cellPath;}

  virtual Bool_t isGeant(void) const {return kFALSE;}
  virtual void   print(void);

  ClassDef(HMdcWireFit,2)  // track fit result for one wire
};

inline void HMdcWireFit::setAddress(Char_t s,Char_t m,Char_t l,Short_t c) {
  // Set wire address
  sec   = s;
  mod   = m;
  layer = l;
  cell  = c;
}

inline void HMdcWireFit::getAddress(Int_t& s,Int_t& m,Int_t& l,Int_t& c)
    const {
  // Get wire addres
  s = sec;
  m = mod;
  l = layer;
  c = cell;
}


inline void HMdcWireFit::clear() {
  // Default setting of the data members.
  sec = mod = layer = timeNum = -1;
  cell       = -1;
  tdcTimeErr = 1.;
}

#endif /* HMDCWIREFIT_H */
