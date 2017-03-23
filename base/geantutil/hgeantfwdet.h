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
  Char_t  layer;        // layer number (0...8)
  Int_t   cell;         // cell number inside module (0...999)
  Char_t  subcell;      // subcell (0..8)
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
  inline void setAddress (Char_t aMod, Char_t aLayer, Int_t aCell, Char_t aSubCell);
  void setHit(Float_t aX, Float_t aY, Float_t aZ, Float_t aPx, Float_t aPy, Float_t aPz,
              Float_t aTof, Float_t aLen, Float_t aE);
  
  Int_t getTrackNumber(void) {return trackNumber;}
  inline void getAddress(Char_t& aMod, Char_t& alayer, Int_t& aCell, Char_t& aSubCell);
  void getHit(Float_t& aX, Float_t& aY, Float_t& aZ, Float_t& aPx, Float_t& aPy, Float_t& aPz,
              Float_t& aTof, Float_t& aLen, Float_t& aE);  
  Int_t getNLocationIndex(void) {return 3;}
  inline Int_t getLocationIndex(Int_t i);

  ClassDef(HGeantFwDet,1) // GEANT hit data class for the new forward detector
};

// Sets the module and cell number
inline void HGeantFwDet::setAddress (Char_t aMod, Char_t aLayer, Int_t aCell, Char_t aSubCell) {
  module  = aMod;
  layer   = aLayer;
  cell    = aCell;
  subcell = aSubCell;
}

// Returns the module and cell number
inline void HGeantFwDet::getAddress(Char_t& aMod, Char_t& aLayer, Int_t& aCell, Char_t& aSubCell) {
  aMod     = module;
  aLayer   = layer;
  aCell    = cell;
  aSubCell = subcell;
}

inline Int_t HGeantFwDet::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return module;
    case 1 : return layer;
    case 2 : return cell;
    case 3 : return subcell;
    default: return -1;
  }
  return -1;
}

#endif  /*! HGEANTFWDET_H */
