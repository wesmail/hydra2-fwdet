#ifndef HMDCWIREFITSIM_H
#define HMDCWIREFITSIM_H

#include "hmdcwirefit.h"

class HMdcWireFitSim : public HMdcWireFit {
protected:
  Int_t   trackNum;       // geant track number (track number from HMdcCal1Sim)
  Float_t alphaGeant;     // impact angle in cell system [deg.]
  Float_t minDistGeant;   // minimum distance to track [mm]
  Float_t timeErrDigi;    // error of drift time used digitizer [ns]
  Float_t tofGeant;       // tof of track [ns]
  Bool_t  clusFitTrackFl; // =kTRUE if trackNum==HMdcClusFitSim::geantTrack
public:
  HMdcWireFitSim() {trackNum=-1; clear();}
  ~HMdcWireFitSim() {;}
  void setGeantTrackNum(Int_t n)  {trackNum=n;}
  void setGeantMinDist(Float_t v) {minDistGeant=v;}
  void setGeantAlpha(Float_t v)   {alphaGeant=v;}
  void setDigiTimeErr(Float_t v)  {timeErrDigi=v;}
  void setGeantTof(Float_t v)     {tofGeant=v;}
  void setClusFitTrFlag(Bool_t f) {clusFitTrackFl=f;}

  Int_t   getGeantTrackNum(void) const {return trackNum;}
  Float_t getGeantMinDist(void) const  {return minDistGeant;}
  Float_t getGeantAlpha(void) const    {return alphaGeant;}
  Float_t getDigiTimeErr(void) const   {return timeErrDigi;}
  Float_t getGeantTof(void) const      {return tofGeant;}
  Bool_t  isClusFitTrack(void) const   {return clusFitTrackFl;}

  virtual Bool_t isGeant() const   {return kTRUE;}
  virtual void print();

  ClassDef(HMdcWireFitSim,1)  // geant and digitizer data for HMdcWireFit
};

#endif /* HMDCWIREFITSIM_H */
