//*************************************************************************
//
//  HGeantShower
//
//  GEANT SHOWER hit data
//
//  last modified on 31/03/04 by R.Holzmann  (GSI)
//*************************************************************************
#ifndef HGEANTSHOWER_H
#define HGEANTSHOWER_H

#include "hlinkeddataobject.h"

class HGeantShower : public HLinkedDataObject
{
private:
  Int_t trackNumber;        // GEANT track number
  Float_t eHit;             // energy deposited  (in MeV)
  Float_t xHit;             // x of hit  (in mm) in module coord. system
  Float_t yHit;             // y of hit  (in mm) in module coord. system
  Float_t thetaHit;         // angle of incidence  (0-180 deg)
  Float_t phiHit;           // azimuthal angle  (0-360 deg)
  Float_t betaHit;          // beta of particle (=momentum/energy) 
  Char_t sector;            // sector number  (0...5)
  Char_t module;            // module number  (0...2)
public:
  HGeantShower(void);
//  HGeantShower(HGeantShower &aShower);
  ~HGeantShower(void);
  inline void setTrack(Int_t aTrack) {trackNumber = aTrack;}
  void setHit(Float_t ae, Float_t ax, Float_t ay, Float_t abeta);
  void setIncidence(Float_t ath, Float_t aph);
  void setAddress (Char_t s, Char_t m);
  virtual Int_t getTrack(void) {return trackNumber;}
  void getHit(Float_t &ae, Float_t &ax, Float_t &ay, Float_t &abeta);
  void getIncidence(Float_t &ath, Float_t &aph);
  inline Char_t getSector(void) {return sector;}
  inline Char_t getModule(void) {return module;}
  inline Int_t getNLocationIndex(void) {return 2;}
  Int_t getLocationIndex(Int_t i);

  ClassDef(HGeantShower,2) // GEANT SHOWER hit data class
};

#endif  /*! HGEANTSHOWER_H */








