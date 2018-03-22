#include "hgeantrich.h"

#include "TBuffer.h"

//
//*-- Author : Romain Holzmann (r.holzmann@gsi.de)
//*-- Modified : 1999/10/07 by Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Copyright : GSI, Darmstadt
//
// ***************************************************************************
//
//  HGeantRichPhoton
// 
//  GEANT RICH hit data for Cherenkov photons.
//
// ***************************************************************************
//
//  Inline Functions:
//
//   void setTrack(Int_t aTrack)    set GEANT track number
//   void setAddress(Char_t s)      set sector number
//   Int_t getTrack()               retrieve GEANT track number
//   Float_t getEnergy()            retrieve photon energy (in eV)
//   Float_t getX()                 retrieve x position in pad plane
//   Float_t getY()                 retrieve y position in pad plane
//   Int_t getSector()              retrieve sector number
//
ClassImp(HGeantRichPhoton)

//----------------------------------------------------------------------------
HGeantRichPhoton::HGeantRichPhoton() 
{
// Default constructor.
 parentTrack = 0;
 xHit = 0.0;
 yHit = 0.0;
 eHit = 0.0;
 sector = -1;
 fPmtId = -1;
}
//============================================================================

//----------------------------------------------------------------------------
HGeantRichPhoton::HGeantRichPhoton(const HGeantRichPhoton &source) 
{
// Copy constructor.
  parentTrack = source.parentTrack;
  xHit = source.xHit;
  yHit = source.yHit;
  eHit = source.eHit;
  if(eHit < 0.) eHit = 0.;
  sector = source.sector;
  fPmtId = source.fPmtId;
}
//============================================================================

//----------------------------------------------------------------------------
HGeantRichPhoton::~HGeantRichPhoton() 
{
// Destructor.
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichPhoton::setHit(Float_t ax, Float_t ay, Float_t ae) 
{
// Set photon hit information:  x,y position (mm) and photon energy (eV).
  xHit = ax;
  yHit = ay;
  eHit = ae;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichPhoton::getHit(Float_t &ax, Float_t &ay, Float_t &ae) 
{
// Retrieve photon hit information: x,y position (mm) and photon energy (eV).
  ax = xHit;
  ay = yHit;
  ae = eHit;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t HGeantRichPhoton::getLocationIndex(Int_t i) 
{
// Location index is sector number.
  switch (i) {
    case 0 : return sector;
  }
  return -1;
}
//----------------------------------------------------------------------------
void HGeantRichPhoton::Streamer(TBuffer &R__b)
{
   // Stream an object of class HGeantRichPhoton.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HLinkedDataObject::Streamer(R__b);
      R__b >> parentTrack;
      R__b >> xHit;
      R__b >> yHit;
      R__b >> eHit;
      R__b >> sector;
      if(R__v> 2 ) R__b >> fPmtId;
      else                 fPmtId=-1;
      R__b.CheckByteCount(R__s, R__c, HGeantRichPhoton::IsA());
   } else {
      R__c = R__b.WriteVersion(HGeantRichPhoton::IsA(), kTRUE);
      HLinkedDataObject::Streamer(R__b);
      R__b << parentTrack;
      R__b << xHit;
      R__b << yHit;
      R__b << eHit;
      R__b << sector;
      R__b << fPmtId;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
//============================================================================


//
//*-- Author : Romain Holzmann (r.holzmann@gsi.de)
//*-- Modified : 1999/10/07 by Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Copyright : GSI, Darmstadt
//
// ***************************************************************************
//
//  HGeantRichDirect
//
//  GEANT RICH hit data of direct hits to photon detector.
//
// ***************************************************************************
//
// Inline Functions:
//
//   void setAddress(Char_t s)         set sector number
//   Int_t getSector()                 retrieve sector number
//   Int_t getTrack()                  retrieve GEANT track number
//   Float_t getX()                    retrieve x position in pad plane
//   Float_t getY()                    retrieve y position in pad plane
//   Float_t getTheta()                retrieve incidence angle (theta)
//   Float_t getPhi()                  retrieve incidence angle (phi)
//   Float_t getEnergyLoss()           retrieve energy loss in detector gas
//   Float_t getTrackLengthInPhotDet() retrieve track length in detector gas
// 

ClassImp(HGeantRichDirect)

//----------------------------------------------------------------------------
HGeantRichDirect::HGeantRichDirect() 
{
// Default constructor.
  trackNumber = 0;
  xHit = 0.0;
  yHit = 0.0;
  zHit = 0.0;
  particleID = 0;
  momentum = 0.0;
  thetaHit = 0.0;
  phiHit = 0.0;
  eLoss = 0.0;
  trackLength = 0.0;
  sector = -1;
  fPmtId = -1;
}
//============================================================================
/*
//----------------------------------------------------------------------------
HGeantRichDirect::HGeantRichDirect(const HGeantRichDirect &source) 
{
// Copy constructor.
  trackNumber = source.trackNumber;
  xHit = source.xHit;
  yHit = source.yHit;
  zHit = source.zHit;
  particleID = source.particleID;
  momentum = source.momentum;
  thetaHit = source.thetaHit;
  phiHit = source.phiHit;
  eLoss = source.eLoss;
  trackLength = source.trackLength;
  sector = source.sector;
  fPmtId = source.fPmtId;
}
//============================================================================
*/
//----------------------------------------------------------------------------
HGeantRichDirect::~HGeantRichDirect() 
{
// Destructor.
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::setTrack(Int_t aTrack, Int_t aID) {
  // Set track number and particle id.
  trackNumber = aTrack;
  particleID = aID;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::setHit(Float_t ax, Float_t ay, Float_t az) {
  // Set hit location: x,x and z (in mm)
  xHit = ax;
  yHit = ay;
  zHit = az;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::setMomentum(Float_t amom, Float_t ath, Float_t aph) {
  // Set momentum and incidence angles.
  momentum = amom;
  thetaHit = ath;
  phiHit = aph;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::setELoss(Float_t ael, Float_t alen) {
  // Set energy loss and track length.
  eLoss = ael;
  trackLength = alen;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::getTrack(Int_t &aTrack, Int_t &aID) {
  // Retrieve track number and particle id.
  aTrack = trackNumber;
  aID = particleID;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::getHit(Float_t &ax, Float_t &ay, Float_t &az) {
  // Retrieve hit location.
  ax = xHit;
  ay = yHit;
  az = zHit;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::getMomentum(Float_t &amom, Float_t &ath, Float_t &aph) {
  // Retrieve momentum and angles of incidence.
  amom = momentum;
  ath = thetaHit;
  aph = phiHit;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichDirect::getELoss(Float_t &ael, Float_t &alen) {
  // Retrieve energy loss and track length.
  ael = eLoss;
  alen = trackLength;
}
//============================================================================


//
//*-- Author : Romain Holzmann (r.holzmann@gsi.de)
//*-- Modified : 1999/10/07 by Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Copyright : GSI, Darmstadt
//
// ***************************************************************************
//
//  HGeantRichMirror
// 
//  GEANT RICH hit data for mirror hits.
//
// ***************************************************************************
//
// Inline Functions:
//
//   void setNumPhot(Int_t num)        set nb of photons corresponding to this hit
//   void setAddress(Char_t s)         set sector number
//   Int_t getSector()                 retrieve sector number
//   Int_t getTrack()                  retrieve GEANT track number
//   Float_t getX()                    retrieve x position in 
//   Float_t getY()                    retrieve y position in pad plane
//   Float_t getTheta()                retrieve incidence angle (theta)
//   Float_t getPhi()                  retrieve incidence angle (phi)
//   Float_t getEnergyLoss()           retrieve energy loss in detector gas
//   Float_t getTrackLengthInPhotDet() retrieve track length in detector gas
//   void setXRing(Float_t xCoor)      set ring x coordinate in pad plane
//   void setYRing(Float_t yCoor)      set ring y coordinate in pad plane
//   Float_t getXRing()                retrieve ring x coordinate in pad plane
//   Float_t getYRing()                retrieve ring y coordinate in pad plane
//

void HGeantRichDirect::Streamer(TBuffer &R__b)
{
   // Stream an object of class HGeantRichDirect.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HLinkedDataObject::Streamer(R__b);
      R__b >> trackNumber;
      R__b >> xHit;
      R__b >> yHit;
      R__b >> zHit;
      R__b >> particleID;
      R__b >> momentum;
      R__b >> thetaHit;
      R__b >> phiHit;
      R__b >> eLoss;
      R__b >> trackLength;
      R__b >> sector;
      R__b >> numPhot;
      if (R__v > 2) R__b >> fPmtId;
      else                  fPmtId = -1;

      R__b.CheckByteCount(R__s, R__c, HGeantRichDirect::IsA());
   } else {
      R__c = R__b.WriteVersion(HGeantRichDirect::IsA(), kTRUE);
      HLinkedDataObject::Streamer(R__b);
      R__b << trackNumber;
      R__b << xHit;
      R__b << yHit;
      R__b << zHit;
      R__b << particleID;
      R__b << momentum;
      R__b << thetaHit;
      R__b << phiHit;
      R__b << eLoss;
      R__b << trackLength;
      R__b << sector;
      R__b << numPhot;
      R__b << fPmtId;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

ClassImp(HGeantRichMirror)

//----------------------------------------------------------------------------
HGeantRichMirror::HGeantRichMirror() 
{
// Default constructor.
  trackNumber = 0;
  xLep = 0.0;
  yLep = 0.0;
  zLep = 0.0;
  lepID  = 0;
  sector = -1;
  numPhot = 0;
  xRing = 0.;
  yRing = 0.;
}
//============================================================================
/*
//----------------------------------------------------------------------------
HGeantRichMirror::HGeantRichMirror(const HGeantRichMirror &source) 
{
// Copy constructor.
  trackNumber = source.trackNumber;
  xLep = source.xLep;
  yLep = source.yLep;
  zLep = source.zLep;
  lepID = source.lepID; 
  sector = source.sector;
  xRing =  source.xRing;
  yRing =  source.yRing;
  numPhot = source.numPhot;
}
//============================================================================
*/
//----------------------------------------------------------------------------
HGeantRichMirror::~HGeantRichMirror() 
{
// Destructor.
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichMirror::setTrack(Int_t aTrack, Int_t aID) {
  // Set track number and particle id.
  trackNumber = aTrack;
  lepID = aID;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichMirror::setHit(Float_t ax, Float_t ay, Float_t az) {
  // Set hit location: x,y, and z (in mm).
  xLep= ax;
  yLep= ay;
  zLep = az;
}
//============================================================================


//----------------------------------------------------------------------------
void HGeantRichMirror::getTrack(Int_t &aTrack, Int_t &aID) {
  // Retrieve track number and particle id.
  aTrack = trackNumber;
  aID = lepID;
}
//============================================================================

//----------------------------------------------------------------------------
void HGeantRichMirror::getHit(Float_t &ax, Float_t &ay, Float_t &az) {
  // Retrieve hit location.
  ax = xLep;
  ay = yLep;
  az = zLep;
}
//============================================================================

void HGeantRichMirror::setXYring(Float_t ax, Float_t ay) {
  // Set rin position: x and y.
 xRing = ax;
 yRing = ay; 
}


