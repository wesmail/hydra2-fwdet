//*-- AUTHOR : Ilse Koenig
//*-- Created : 27/11/2015

//_HADES_CLASS_DESCRIPTION 
//**************************************************************************
//
//  HGeantFwDet
// 
//  GEANT hit data of the new forward detector
//
// Inline functions:
//   void setTrack(Int_t aTrack)                   sets the GEANT track number
//   Int_t getTrackNumber(void)                    returns the GEANT track number
//   void setAddress (Char_t aMod, Char_t aCell)   sets the module and cell number
//   void getAddress(Char_t& aMod, Char_t& aCell)  returns the module and cell number
//   Int_t getNLocationIndex(void)                 returns 2
//   Int_t getLocationIndex(Int_t i)               returns the module index (i=0)
//                                                 or cell index (i=1)
//
//**************************************************************************

#include "hgeantfwdet.h"

ClassImp(HGeantFwDet)

HGeantFwDet::HGeantFwDet(void) {
  // Default constructor.
  trackNumber = 0;
  module      = -1;
  layer       = -1;
  cell        = -1;
  xHit        = 0.0F;
  yHit        = 0.0F;
  zHit        = 0.0F;
  pxHit       = 0.0F;
  pyHit       = 0.0F;
  pzHit       = 0.0F;
  tofHit      = 0.0F;
  trackLength = 0.0F;
  eHit        = 0.0F;
}

HGeantFwDet::HGeantFwDet(HGeantFwDet &aFwDet) {
  // Copy constructor
  trackNumber = aFwDet.trackNumber;
  module      = aFwDet.module;
  layer       = aFwDet.layer;
  cell        = aFwDet.cell;
  xHit        = aFwDet.xHit;
  yHit        = aFwDet.yHit;
  zHit        = aFwDet.zHit;
  pxHit       = aFwDet.pxHit;
  pyHit       = aFwDet.pyHit;
  pzHit       = aFwDet.pzHit;
  tofHit      = aFwDet.tofHit;
  trackLength = aFwDet.trackLength;
  eHit        = aFwDet.eHit;
}

void HGeantFwDet::setHit(Float_t aX, Float_t aY, Float_t aZ, Float_t aPx, Float_t aPy, Float_t aPz,
			 Float_t aTof, Float_t aLen, Float_t aE) {
  // Set hit data (position and momentum componets, time of flight,track length, energy loss)
  xHit        = aX;
  yHit        = aY;
  zHit        = aZ;
  pxHit       = aPx;
  pyHit       = aPy;
  pzHit       = aPz;
  tofHit      = aTof;
  trackLength = aLen;
  eHit        = aE;
}

void HGeantFwDet::getHit(Float_t& aX, Float_t& aY, Float_t& aZ, Float_t& aPx, Float_t& aPy, Float_t& aPz,
		         Float_t& aTof, Float_t& aLen, Float_t& aE) {
  // Retrieve hit data (position and momentum componets, time of flight,track length, energy loss)
  aX   = xHit;
  aY   = yHit;
  aZ   = zHit;
  aPx  = pxHit;
  aPy  = pyHit;
  aPz  = pzHit;
  aTof = tofHit;
  aLen = trackLength;
  aE   = eHit;
}  
