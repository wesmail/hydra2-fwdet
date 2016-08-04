#include "hgeantheader.h"
//*-- Author : Romain Holzmann, GSI
//*-- Modified : 29/04/99 by Romain Holzmann
//*-- Copyright : GSI, Darmstadt
//
//***************************************************************************
// HGeantHeader
// 
// GEANT partial event header
//
//***************************************************************************
//
// Inline functions:
//  
//   inline Int_t getEventId()             retrieve event id
//   inline Float_t getBeamEnergy()        retrieve beam energy
//   inline Float_t getImpactParameter()   retrieve impact parameter
//   inline Float_t getEventPlane()        retrieve event plane angle
//

ClassImp(HGeantHeader)

HGeantHeader::HGeantHeader(void) {
  // Default Constructor.
  eventID = 0;
  beamEnergy = 0.;
  impactParameter = 0.;
  eventPlane = 0.;  
}

HGeantHeader::HGeantHeader(HGeantHeader &aHeader) {
  // Copy constructor.
  eventID = aHeader.eventID;
  beamEnergy = aHeader.beamEnergy;
  impactParameter = aHeader.impactParameter;
  eventPlane = aHeader.eventPlane;
}

HGeantHeader::~HGeantHeader(void) {
  // Destructor.
}

void HGeantHeader::setEventId(Int_t aID) {
  // Set event id.
  eventID = aID;
}

void HGeantHeader::setBeamEnergy(Float_t aEnergy) {
  // Set beam energy.
  beamEnergy = aEnergy;
}

void HGeantHeader::setImpactParameter(Float_t aImpact) {
  // Set impact parameter
  impactParameter = aImpact;
}

void HGeantHeader::setEventPlane(Float_t aPlane) {
  // Set event plane.
  eventPlane = aPlane;
}








