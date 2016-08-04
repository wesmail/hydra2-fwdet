//*************************************************************************
//
//  HGeantWall
//
//  GEANT Forward Wall hit data
//
//  last modified on 08/12/2004 by R.Holzmann  (GSI)
//*************************************************************************
#ifndef HGEANTWALL_H
#define HGEANTWALL_H

#include "hlinkeddataobject.h"

class HGeantWall : public HLinkedDataObject
{
private:
  Int_t trackNumber;      // GEANT track number
  Float_t trackLength;    // track length  (in mm)
  Float_t eHit;           // energy deposited  (in MeV)
  Float_t xHit;           // x of hit in scintillator  (in mm)
  Float_t yHit;           // y of hit in scintillator  (in mm)
  Float_t tofHit;         // time of flight of hit  (in ns)
  Float_t momHit;         // momentum of particle at hit  (in MeV/c)
  Char_t module;          // module number (1...3) identifies size: 1=small,2=medium,3=big
  Int_t cell;             // cell number (1...384)
public:
  HGeantWall(void);
//  HGeantWall(HGeantWall &aWall);
  ~HGeantWall(void);
  inline void setTrack(Int_t aTrack) {trackNumber = aTrack;}
  void setHit(Float_t ae, Float_t ax, Float_t ay, Float_t atof,
              Float_t amom, Float_t alen);
  inline void setAddress (Char_t m, Int_t c);
  virtual Int_t getTrack(void) {return trackNumber;}
  void getHit(Float_t& ae, Float_t& ax, Float_t& ay, Float_t& atof,
              Float_t& amom, Float_t& alen);
  inline Char_t getModule(void) {return module;}
  inline Int_t getCell(void) {return cell;}
  inline Int_t getNLocationIndex(void) {return 3;}
  inline Int_t getLocationIndex(Int_t i); 

  ClassDef(HGeantWall,1) // Geant Forward Wall event data class
};

inline void HGeantWall::setAddress(Char_t m, Int_t c) {
  module = m;
  cell = c;
}

inline Int_t HGeantWall::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return module;
    case 1 : return cell;
  }
  return -1;
}


#endif  /*! HGEANTWALL_H */











