#include "hgeantmdc.h"

#include "TBuffer.h"

//*-- Author : Romain Holzmann, GSI
//*-- Modified : 1/12/2000 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//****************************************************************************
//  HGeantMdc
// 
//  GEANT MDC hit data
//
//  This class stores the GEANT hits in the MDC detector, per sector, module
//  and layer, with the central cathode plane added as 7th layer.
//
//****************************************************************************
//
// Inline functions:
//
//  inline void setTrack(Int_t track)     set GEANT track number
//  inline Int_t getTrack()               return GEANT track number
//  inline Char_t getSector(void)         return sector number
//  inline Char_t getModule(void)         return module number
//  inline Char_t getLayer(void)          return layer number
//

ClassImp(HGeantMdc)
#include <iostream>

HGeantMdc::HGeantMdc(void) {
  // Default constructor.
  trackNumber = 0;
  xHit = 0.0;
  yHit = 0.0;
  thetaHit = 0.0;
  phiHit = 0.0;
  tofHit = 0.0;
  sector = -1;
  module = -1;
  layer = -1;
}
/*
HGeantMdc::HGeantMdc(HGeantMdc &aMdc) {
  // Copy constructor.

    std::cout<<"geantmdc copy "<<std::endl;
  trackNumber = aMdc.trackNumber;
  xHit = aMdc.xHit;
  yHit = aMdc.yHit;
  thetaHit = aMdc.thetaHit;
  phiHit = aMdc.phiHit;
  tofHit = aMdc.tofHit;
  sector = aMdc.sector;
  module = aMdc.module;
  layer = aMdc.layer;
}
*/
HGeantMdc::~HGeantMdc(void) {
  // Destructor.
}

void HGeantMdc::setHit(Float_t ax, Float_t ay, Float_t atof, Float_t ptof) {
  // Fill in basic hit data:
  //    - x,y coordinates in module frame (in mm)
  //    - time of flight from primary vertex to module (in ns)
  //    - momentum of particle when hitting the module (in MeV/c) 
  xHit = ax;
  yHit = ay;
  tofHit = atof;
  momHit = ptof;
}

void HGeantMdc::setIncidence(Float_t ath, Float_t aph) {
  // Fill in angles of incidence on module (polar and azimuthal)
  // in module coordinate system (in degrees).
  thetaHit = ath;
  phiHit = aph;
}

void HGeantMdc::setAddress(Char_t s, Char_t m, Char_t l) {
  // Fill in hit location (sector, module, layer), all counting from 0.
  sector = s;
  module = m;
  layer = l;
}

void HGeantMdc::getHit(Float_t &ax, Float_t &ay, Float_t &atof, Float_t &ptof) {
  // Retrieve basic hit data (x,y,tof,momentum). 
  ax = xHit;
  ay = yHit;
  atof = tofHit;
  ptof = momHit;
}

void HGeantMdc::getIncidence(Float_t &ath, Float_t &aph) {
  // Retrieve angles of incidence (theta, phi).
  ath = thetaHit;
  aph = phiHit;
}

Int_t HGeantMdc::getLocationIndex(Int_t i) {
  // Retrieve hit location (sector, module, layer).
  switch (i) {
    case 0 : return sector;
    case 1 : return module;
    case 2 : return layer;
  }
  return -1;
}

void HGeantMdc::Streamer(TBuffer &R__b) {
   // Stream an object of class HGeantMdc.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      HLinkedDataObject::Streamer(R__b);
      R__b >> trackNumber;
      R__b >> xHit;
      R__b >> yHit;
      R__b >> thetaHit;
      R__b >> phiHit;
      R__b >> tofHit;
      if (R__v <3) momHit=0.;
      else R__b >> momHit;
      R__b >> sector;
      R__b >> module;
      R__b >> layer;
   } else { 
      R__b.WriteVersion(HGeantMdc::IsA());
      HLinkedDataObject::Streamer(R__b);
      R__b << trackNumber;
      R__b << xHit;
      R__b << yHit;
      R__b << thetaHit;
      R__b << phiHit;
      R__b << tofHit;
      R__b << momHit;
      R__b << sector;
      R__b << module;
      R__b << layer;
   } 
}





