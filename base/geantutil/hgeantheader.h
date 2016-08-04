//****************************************************************************
// 
// GEANT partial event header
//
// last modified on 29/1/99  by R.Holzmann
//
//****************************************************************************
#ifndef HGEANTHEADER_H
#define HGEANTHEADER_H

#include "hlocateddataobject.h"

class HGeantHeader : public HLocatedDataObject
{
private:
  Int_t eventID;            // GEANT event ID
  Float_t beamEnergy;       // beam energy  (in AMeV)
  Float_t impactParameter;  // impact parameter  (in fm)
  Float_t eventPlane;       // event plane phi angle  (in deg)
public:
  HGeantHeader(void);
  HGeantHeader(HGeantHeader &aHeader);
  ~HGeantHeader(void);
  void setEventId(Int_t aID);
  void setBeamEnergy(Float_t aEnergy);
  void setImpactParameter(Float_t aImpact);
  void setEventPlane(Float_t aPlane);
  inline Int_t getEventId() {return eventID;}
  inline Float_t getBeamEnergy() {return beamEnergy;}
  inline Float_t getImpactParameter() {return impactParameter;}
  inline Float_t getEventPlane() {return eventPlane;}
  inline Int_t getNLocationIndex(void) {return 1;}
  inline Int_t getLocationIndex(Int_t) {return 0;}
  ClassDef(HGeantHeader,1) // GEANT partial event header class
};

#endif  /*! HGEANTHEADER_H */









