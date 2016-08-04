#include "hgeantshower.h"

//*-- Author : Jacek Otwinowski, Krakow
//*-- Modified : 21/11/1998 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//**************************************************************************
//
//  HGeantShower
// 
//  GEANT SHOWER hit data
//
//  This class stores the GEANT hits in the PreShower detector, per sector
//  and module.
//
//**************************************************************************
//
// Inline functions:
//
//  inline void setTrack(Int_t track)     set GEANT track number
//  inline Int_t getTrack()               return GEANT track number
//  inline Char_t getSector(void)         return sector number
//  inline Char_t getModule(void)         return module number
//

ClassImp(HGeantShower)

HGeantShower::HGeantShower(void) {
  // Default constructor.
  trackNumber = 0;
  eHit = 0.0;
  xHit = 0.0;
  yHit = 0.0;
  thetaHit = 0.0;
  phiHit = 0.0;
  betaHit = 0.0;
  sector = -1;
  module = -1;
}
/*
HGeantShower::HGeantShower(HGeantShower &aShower) {
  // Copy constructor.
  trackNumber = aShower.trackNumber;
  eHit = aShower.eHit;
  xHit = aShower.xHit;
  yHit = aShower.yHit;
  thetaHit = aShower.thetaHit;
  phiHit = aShower.phiHit;
  betaHit = aShower.betaHit;
  sector = aShower.sector;
  module = aShower.module;
}
*/
HGeantShower::~HGeantShower(void) {
  // Destructor.
}

void HGeantShower::setHit(Float_t ae, Float_t ax, Float_t ay, Float_t abeta) {
  // Fill in basic hit data:
  //    - energy in MeV
  //    - x,y in module coordinate system (in mm)
  //    - beta of particle
  eHit = ae;
  xHit = ax;
  yHit = ay;
  betaHit = abeta;
}

void HGeantShower::setIncidence(Float_t ath, Float_t aph) {
  // Fill in angles of incidence (that and phi in degrees).
  thetaHit = ath;
  phiHit = aph;
}

void HGeantShower::setAddress(Char_t s, Char_t m) {
  // Fill in hit location (sector and module number)
  sector = s;
  module = m;
}

void HGeantShower::getHit(Float_t &ae, Float_t &ax, Float_t &ay, Float_t &at) {
  // Retrieve basic hit data: energy, x, y, and beta
  ae = eHit;
  ax = xHit;
  ay = yHit;
  at = betaHit;
}

void HGeantShower::getIncidence(Float_t &ath, Float_t &aph) {
  // Retrieve angles of incidence: theta and phi
  ath = thetaHit;
  aph = phiHit;
}

Int_t HGeantShower::getLocationIndex(Int_t i) {
  // Retrieve location indices
  switch (i) {
    case 0 : return sector;
    case 1 : return module;
  }
  return -1;
}







