#include "hgeantwall.h"

//*-- Author : Filip Krizek, UJF Rez
//*-- Modified : 25/11/2004 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//**************************************************************************
//
//  HGeantWall
// 
//  GEANT Forward Wall hit data
//
//  last modified on 25/11/2004  by R.Holzmann  (GSI)
//**************************************************************************
// Inline functions:
//
//  inline void setAddress (Char_t m, Int_t c)
//                                        set hit location (module and cell)
//  inline void setTrack(Int_t track)     set GEANT track number
//  inline Int_t getTrack()               return GEANT track number
//  inline Char_t getModule(void)         return module number
//  inline Char_t getCell(void)           return cell number
//

ClassImp(HGeantWall)

HGeantWall::HGeantWall(void) {
  // Default constructor.
  trackNumber = 0;
  trackLength = 0.0;
  eHit = 0.0;
  xHit = 0.0;
  yHit = 0.0;
  tofHit = 0.0;
  momHit = 0.0;
  module = -1;
  cell = -1;
}
/*
HGeantWall::HGeantWall(HGeantWall &aWall) {
  // Copy constructor.
  trackNumber = aWall.trackNumber;
  trackLength = aWall.trackLength;
  eHit = aWall.eHit;
  xHit = aWall.xHit;
  yHit = aWall.yHit;
  tofHit = aWall.tofHit;
  momHit = aWall.momHit;
  module = aWall.module;
  cell = aWall.cell;
}
*/
HGeantWall::~HGeantWall(void) {
  // Destructor.
}

void HGeantWall::setHit(Float_t ae, Float_t ax, Float_t ay, Float_t atof,
                       Float_t amom, Float_t alen) {
  // Fill in basic hit data:
  //   - energy in MeV
  //   - x,y of hit in module in mm
  //   - time of flight in ns
  //   - momentum of particle at hit in MeV/c
  //   - track length from creation vertex to hit in mm
  eHit = ae;
  xHit = ax;
  yHit = ay;
  tofHit = atof;
  momHit = amom;
  trackLength = alen;
}

void HGeantWall::getHit(Float_t& ae, Float_t& ax,  Float_t& ay, Float_t& atof,
                       Float_t& amom, Float_t& alen) {
  // Retrieve basic hit data: energy, x,y, time fo flight, momentum and track length
  ae = eHit;
  ax = xHit;
  ay = yHit;
  atof = tofHit;
  amom = momHit;
  alen = trackLength;
}






