//*-- Author : Manuel Sanchez, Santiago
//*-- Modified : 22/11/2000 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//***************************************************************************
//
//  HGeantEmc
// 
//  GEANT EMC hit data
//
//  This class stores the GEANT hits in the EMC detector,
//  per sector, cell. Cell is one crystal
//
//****************************************************************************
//
// Inline functions:
//
//  inline void setAddress(Char_t s, Char_t m, Char_t c)
//                                        set sector, module, and cell numbers
//  inline void setTrack(Int_t track)     set GEANT track number
//  inline Int_t getTrack()               return GEANT track number
//  inline Char_t getSector(void)         return sector number
//  inline Char_t getCell(void)           return crystal number
//
#include "hgeantemc.h"

ClassImp(HGeantEmc)

HGeantEmc::HGeantEmc(void) {
  // Default constructor.
  trackNumber = 0;
  trackLength = 0.F;
  peHit  = 0.F;
  xHit   = 0.F;
  yHit   = 0.F;
  zHit   = 0.F;
  tofHit = 0.F;
  momHit = 0.F;
  sector = -1;
  cell   = 0;
}
/*
HGeantEmc::HGeantEmc(HGeantEmc &aEmc) {
  // Copy constructor.
  trackNumber = aEmc.trackNumber;
  trackLength = aEmc.trackLength;
  peHit = aEmc.peHit;
  xHit = aEmc.xHit;
  yHit = aEmc.yHit;
  zHit = aEmc.zHit;
  tofHit = aEmc.tofHit;
  momHit = aEmc.momHit;
  sector = aEmc.sector;
  cell   = aEmc.cell;
}
*/
void HGeantEmc::setHit(Float_t ae, Float_t ax, Float_t ay, Float_t az, 
                       Float_t atof, Float_t amom, Float_t alen) {
  // Set hit data (energy loss, y,x in cell system, time of flight, momentum and track length).
  peHit       = ae;
  xHit        = ax;
  yHit        = ay;
  zHit        = az;
  tofHit      = atof;
  momHit      = amom;
  trackLength = alen;
}

void HGeantEmc::getHit(Float_t& ae, Float_t& ax,  Float_t& ay, Float_t& az,
                       Float_t& atof, Float_t& amom, Float_t& alen) {
  // Retrieve hit data (energy loss, y,x in cell system, time of flight, momentum and track length).
  ae   = peHit;
  ax   = xHit;
  ay   = yHit;
  az   = zHit;
  atof = tofHit;
  amom = momHit;
  alen = trackLength;
}
