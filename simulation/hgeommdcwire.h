#ifndef HGEOMMDCWIRE_H_
#define HGEOMMDCWIRE_H_

#include "TObject.h"
#include "TString.h"

class TGeoVolume;

class HGeomMdcWire {

public:
  Int_t         wireNumber;   // wire number (starting from 0)
  TString       wireName;     // volume name
  Int_t         copyNumber;   // node copy number
  Int_t         wireType;     // wire type (0: cathode or field wire, 1: sens wire)
  Float_t       gparams[3];   // Geant/ROOT parameters for a TUBE [cm]
  Float_t       gpos[3];      // Geant/ROOT position in mother [cm] (TUBE center in TRD1 center)
  HGeomMdcWire* pCopyNode;    // pointer to corresponding wire with copyNumber = 1 
  Int_t         mediumInd;    // index of Geant/ROOT medium
  TGeoVolume*   pRootVolume;  // pointer to the ROOT volume

  HGeomMdcWire(Int_t wn=-1, TString name="", Int_t cn=0, Int_t wt=-1, Float_t radius=0.f,
	       Double_t hlen=0., Double_t xpos=0., Double_t ypos=0.,  Double_t zpos=0.) {
    wireNumber  = wn;
    wireName    = name;
    copyNumber  = cn;
    wireType    = wt;
    gparams[0]  = 0.f;
    gparams[1]  = radius * 0.1f;  // in cm
    gparams[2]  = hlen * 0.1f;
    gpos[0]     = xpos * 0.1f;
    gpos[1]     = ypos * 0.1f;
    gpos[2]     = zpos * 0.1f;
    pCopyNode   = NULL;
    mediumInd   = -1;
    pRootVolume = NULL;
  }

  ~HGeomMdcWire(){};

  void setMediumInd(Int_t i)        { mediumInd = i; }
  void setWireNumber(Int_t wn)      { wireNumber = wn; }
  void setCopyNumber(Int_t cn)      { copyNumber = cn; }
  void setCopyNode(HGeomMdcWire* p) { pCopyNode = p; }
  void setRootVolume(TGeoVolume* v) { pRootVolume = v; }

  TString&      getWireName()    { return wireName; }
  Int_t         getCopyNumber()  { return copyNumber; }
  Int_t         getWireType()    { return wireType; }       
  Float_t*      getParams()      { return &gparams[0]; }
  Int_t         getNParams()     { return 3; }
  Float_t*      getPosition()    { return &gpos[0];}
  Int_t         getMediumInd()   { return mediumInd; }
  HGeomMdcWire* getCopyNode()    { return pCopyNode; }
  TGeoVolume*   getRootVolume()  { return pRootVolume; }

  void getPosition(Double_t& x, Double_t& y, Double_t& z) {
    x = gpos[0];    
    y = gpos[1];    
    z = gpos[2];    
  }

  void print();

};

#endif /* !HGEOMMDCWIRE_H */
