//*-- Author : Ilse Koenig
//
//***************************************************************************
//
//  HGeantStart
// 
//  GEANT START hit data
//
//  This class stores the GEANT hits in the START detector.
//  Cell is one diamond.
//
//****************************************************************************
//
// Inline functions:
//
//  void setCell(Char_t c)         set cell number
//  void setTrack(Int_t track)     set GEANT track number
//  Char_t getCell(void)           return cell number
//  Int_t getTrack()               return GEANT track number
//

#include "hgeantstart.h"

ClassImp(HGeantStart)

HGeantStart::HGeantStart(void) {
  // Default constructor.
  trackNumber = 0;
  eHit        = 0.F;
  xHit        = 0.F;
  yHit        = 0.F;
  tofHit      = 0.F;
  momHit      = 0.F;
  trackLength = 0.F;
  cell        = -1;
}
/*
HGeantStart::HGeantStart(HGeantStart &aStart) {
  // Copy constructor.
  trackNumber = aStart.trackNumber;
  eHit        = aStart.eHit;
  xHit        = aStart.xHit;
  yHit        = aStart.yHit;
  tofHit      = aStart.tofHit;
  momHit      = aStart.momHit;
  trackLength = aStart.trackLength;
  cell        = aStart.cell;
}
*/
void HGeantStart::setHit(Float_t ae, Float_t ax, Float_t ay, 
                         Float_t atof, Float_t amom, Float_t alen) {
  // Set hit data (energy loss, x,y in cell system, time of flight, momentum and track length).
  eHit        = ae;
  xHit        = ax;
  yHit        = ay;
  tofHit      = atof;
  momHit      = amom;
  trackLength = alen;
}

void HGeantStart::getHit(Float_t& ae, Float_t& ax,  Float_t& ay,
                         Float_t& atof, Float_t& amom, Float_t& alen) {
  // Retrieve hit data (energy loss, x,y in cell system, time of flight, momentum and track length).
  ae   = eHit;
  ax   = xHit;
  ay   = yHit;
  atof = tofHit;
  amom = momHit;
  alen = trackLength;
}
