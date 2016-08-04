//*************************************************************************
//
//  HGeantFwDet
//
//  GEANT hit data for the new forward detector
//
//*************************************************************************
#ifndef HGEANTFWDET_H
#define HGEANTFWDET_H

#include "hlinkeddataobject.h"

class HGeantFwDet : public HLinkedDataObject
{
private:
  Int_t   trackNumber;  // GEANT track number
  Char_t  module;       // module number (0...8)
  Char_t  cell;         // cell number inside module (0...8)
  Float_t xHit;         // x of hit  (in mm) in cell coord. system
  Float_t yHit;         // y of hit  (in mm) in cell coord. system
  Float_t zHit;         // z of hit  (in mm) in cell coord. system
  Float_t pxHit;        // x component of hit momentum (in MeV/c)
  Float_t pyHit;        // y component of hit momentum (in MeV/c)
  Float_t pzHit;        // z component of hit momentum (in MeV/c)
  Float_t tofHit;       // time of flight of hit (in ns)
  Float_t trackLength;  // track length (in mm)
  Float_t eHit;         // energy deposited (in MeV)
public:
  HGeantFwDet(void);
  HGeantFwDet(HGeantFwDet &aFwDet);
  ~HGeantFwDet(void) {}

  void setTrack(Int_t aTrack)   {trackNumber = aTrack;}
  inline void setAddress (Char_t aMod, Char_t aCell);
  void setHit(Float_t aX, Float_t aY, Float_t aZ, Float_t aPx, Float_t aPy, Float_t aPz,
              Float_t aTof, Float_t aLen, Float_t aE);
  
  Int_t getTrackNumber(void) {return trackNumber;}
  inline void getAddress(Char_t& aMod, Char_t& aCell);
  void getHit(Float_t& aX, Float_t& aY, Float_t& aZ, Float_t& aPx, Float_t& aPy, Float_t& aPz,
              Float_t& aTof, Float_t& aLen, Float_t& aE);  
  Int_t getNLocationIndex(void) {return 2;}
  inline Int_t getLocationIndex(Int_t i);

  ClassDef(HGeantFwDet,1) // GEANT hit data class for the new forward detector
};

// Sets the module and cell number
inline void HGeantFwDet::setAddress (Char_t aMod, Char_t aCell) {
  module = aMod;
  cell   = aCell;
}

// Returns the module and cell number
inline void HGeantFwDet::getAddress(Char_t& aMod, Char_t& aCell) {
  aMod  = module;
  aCell = cell;
}

inline Int_t HGeantFwDet::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return module;
    case 1 : return cell;
  }
  return -1;
}

#endif  /*! HGEANTFWDET_H */








