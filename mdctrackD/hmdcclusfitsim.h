#ifndef HMDCCLUSFITSIM_H
#define HMDCCLUSFITSIM_H

#include "hmdcclusfit.h"
#include <math.h>

class HMdcClusFitSim : public HMdcClusFit {
protected:
  Short_t nTracks;      // number of GEANT tracks passed fit
  Short_t nTracksClus;  // number of GEANT tracks in input of fit (in cluster)
  Int_t   geantTrack;   // geant track number which has max.num. of wires passed fit
  Short_t nTrWires;     // number of wires passed fit from track num.=geantTrack
  Short_t nTrWiresClus; // number of wires in cluster from track num.=geantTrack
  Bool_t  primary;      // =kTRUE if "geantTrack" is primary track

  Float_t x1geant;      // GEANT track parameters on the same
  Float_t y1geant;      // planes as x1,y1,z1,x2,y2,z2 in HMdcClusFit [mm]
  Float_t z1geant;      //
  Float_t x2geant;      //
  Float_t y2geant;      //
  Float_t z2geant;      //
  Bool_t  direction;    // =KTRUE if track direction is from target to meta
                        // =kFALSE if opposed. GEANT xyz =-10000. in this case
  Int_t   particleID;   // GEANT particle ID number
  Float_t momentum;     // GEANT particle momentum (in MeV/c)

public:
  HMdcClusFitSim();
  ~HMdcClusFitSim(){}

  void    setNumTracks(Short_t n)       {nTracks      = n;}
  void    setNumTracksClus(Short_t n)   {nTracksClus  = n;}
  void    setGeantTrackNum(Int_t i)     {geantTrack   = i;}
  void    setNumWiresTrack(Short_t n)   {nTrWires     = n;}
  void    setNumWiresTrClus(Short_t n)  {nTrWiresClus = n;}
  void    setX1Geant(Float_t v)         {x1geant      = v;}
  void    setY1Geant(Float_t v)         {y1geant      = v;}
  void    setZ1Geant(Float_t v)         {z1geant      = v;}
  void    setX2Geant(Float_t v)         {x2geant      = v;}
  void    setY2Geant(Float_t v)         {y2geant      = v;}
  void    setZ2Geant(Float_t v)         {z2geant      = v;}
  void    setPrimaryFlag(Bool_t fl)     {primary      = fl;}
  void    setFakeTrack(Bool_t dir=kTRUE);
  void    setXYZ1Geant(Float_t x,Float_t y,Float_t z) {x1geant=x; y1geant=y; z1geant=z;}
  void    setXYZ2Geant(Float_t x,Float_t y,Float_t z) {x2geant=x; y2geant=y; z2geant=z;}
  void    setParticleID(Int_t id)       {particleID   = id;}
  void    setMomentum(Float_t mom)      {momentum     = mom;}
  void    setFakeFlag(void)             {if(nTrWiresClus > 0) nTrWiresClus = -nTrWiresClus;}

  Short_t getNumTracks(void) const      {return nTracks;}
  Short_t getNumTracksClus(void) const  {return nTracksClus;}
  Int_t   getGeantTrackNum(void) const  {return geantTrack;}
  Short_t getNumWiresTrack(void) const  {return nTrWires;}
  Short_t getNumWiresTrClus(void) const {return nTrWiresClus >= 0 ? nTrWiresClus : -nTrWiresClus;}
  Float_t getX1Geant(void) const        {return x1geant;}
  Float_t getY1Geant(void) const        {return y1geant;}
  Float_t getZ1Geant(void) const        {return z1geant;}
  Float_t getX2Geant(void) const        {return x2geant;}
  Float_t getY2Geant(void) const        {return y2geant;}
  Float_t getZ2Geant(void) const        {return z2geant;}
  Bool_t  isPrimary(void) const         {return primary;}
  Bool_t  isTrackDirGood(void) const    {return direction;}
  Float_t getGeantPhi(void) const       {return atan2(y2geant-y1geant,x2geant-x1geant);}
  Float_t getGeantTheta(void) const     {return atan2(sqrt((x2geant-x1geant)*(x2geant-x1geant) +
                                         (y2geant-y1geant)*(y2geant-y1geant)),z2geant-z1geant);}
  void    getGeantRZmin(Float_t &zm, Float_t &r0, Float_t xBm=0., Float_t yBm=0.) 
      const {calcRZtoLineXY(zm,r0,x1geant,y1geant,z1geant, x2geant,y2geant,z2geant,xBm,yBm);}
  Int_t   getParticleID(void) const     {return particleID;}
  Float_t getMomentum(void) const       {return momentum;}
  
  Float_t dX1(void) const               {return x1-x1geant;}
  Float_t dY1(void) const               {return y1-y1geant;}
  Float_t dZ1(void) const               {return z1-z1geant;}
  Float_t dX2(void) const               {return x2-x2geant;}
  Float_t dY2(void) const               {return y2-y2geant;}
  Float_t dZ2(void) const               {return z2-z2geant;}
  Bool_t  isFakeGeant(void) const       {return nTrWiresClus < 4;}
  Float_t getPurity(void) const         {return numOfWires ? 
                                         Float_t(nTrWires)/Float_t(numOfWires) : 0.;}
  Float_t getEfficiency(void) const     {return nTrWiresClus ? 
                                         Float_t(nTrWires)/getNumWiresTrClus() : 0.;}
  void    printSimVsRec(void) const;
  
  virtual void print(void) const;
  virtual Bool_t isGeant() const        {return kTRUE;}

  ClassDef(HMdcClusFitSim,1) // geant data for HMdcClusFit
};

#endif
