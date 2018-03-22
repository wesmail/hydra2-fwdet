//*-- Author : Manuel Sanchez, Santiago
//*-- Modified : 22/11/2000 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//***************************************************************************
//
//  HGeantTof
// 
//  GEANT TOF hit data
//
//  This class stores the GEANT hits in the TOF and TOFINO detector,
//  per sector,  module and cell, with the TOFINO paddles being handled
//  as modules 22-25 and cell=0.
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
//  inline Char_t getModule(void)         return module number
//  inline Char_t getCell(void)           return cell number
//
#include "hgeanttof.h"

#include "TBuffer.h"

ClassImp(HGeantTof)

HGeantTof::HGeantTof(void) {
  // Default constructor.
  trackNumber = 0;
  trackLength = 0.0;
  eHit = 0.0;
  xHit = 0.0;
  yHit = 0.0;
  tofHit = 0.0;
  momHit = 0.0;
  sector = -1;
  module = -1;
  cell = -1;
}
/*
HGeantTof::HGeantTof(HGeantTof &aTof) {
  // Copy constructor.
  trackNumber = aTof.trackNumber;
  trackLength = aTof.trackLength;
  eHit = aTof.eHit;
  xHit = aTof.xHit;
  yHit = aTof.yHit;
  tofHit = aTof.tofHit;
  momHit = aTof.momHit;
  sector = aTof.sector;
  module = aTof.module;
  cell = aTof.cell;
}
*/
HGeantTof::~HGeantTof(void) {
  // Destructor.
}

void HGeantTof::setHit(Float_t ae, Float_t ax, Float_t ay, Float_t atof,
                       Float_t amom, Float_t alen) {
  // Set hit data (energy loss, y,x in module system, time of flight, momentum and track length).
  eHit = ae;
  xHit = ax;
  yHit = ay;
  tofHit = atof;
  momHit = amom;
  trackLength = alen;
}

void HGeantTof::getHit(Float_t& ae, Float_t& ax,  Float_t& ay, Float_t& atof,
                       Float_t& amom, Float_t& alen) {
  // Retrieve hit data (energy loss, y,x in module system, time of flight, momentum and track length).
  ae = eHit;
  ax = xHit;
  ay = yHit;
  atof = tofHit;
  amom = momHit;
  alen = trackLength;
}

void HGeantTof::Streamer(TBuffer &R__b) {

   // Stream an object of class HGeantTof.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion();
      HLinkedDataObject::Streamer(R__b);
      R__b >> trackNumber;
      if ( R__v <= 2 )  trackLength = 0.;
      else              R__b >> trackLength;
      R__b >> eHit;
      R__b >> xHit;
      R__b >> yHit;
      R__b >> tofHit;
      if ( R__v <= 2 )  momHit = 0.;
      else              R__b >> momHit;
      R__b >> sector;
      R__b >> module;
      R__b >> cell;
   } else {
      R__b.WriteVersion(HGeantTof::IsA());
      HLinkedDataObject::Streamer(R__b);
      R__b << trackNumber;
      R__b << trackLength;
      R__b << eHit;
      R__b << xHit;
      R__b << yHit;
      R__b << tofHit;
      R__b << momHit;
      R__b << sector;
      R__b << module;
      R__b << cell;
   }
} 






