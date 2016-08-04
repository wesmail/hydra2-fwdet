//*************************************************************************
//
//  HGeantTof
//
//  GEANT TOF hit data
//
//  last modified on 31/03/2004 by R.Holzmann  (GSI)
//*************************************************************************
#ifndef HGEANTTOF_H
#define HGEANTTOF_H

#include "hlinkeddataobject.h"

class HGeantTof : public HLinkedDataObject
{
private:
  Int_t trackNumber;      // GEANT track number
  Float_t trackLength;    // track length  (in mm)
  Float_t eHit;           // energy deposited  (in MeV)
  Float_t xHit;           // x of hit along scintillator rod (in mm)
  Float_t yHit;           // y of hit along scintillator rod (in mm)
  Float_t tofHit;         // time of flight of hit  (in ns)
  Float_t momHit;         // momentum of particle at hit  (in MeV/c)
  Char_t sector;          // sector number (0...5)
  Char_t module;          // module number (0...21, numbered in->out) + (22...25 for Tofino)
  Char_t cell;            // cell number (0...7, numbered in->out)
public:
  HGeantTof(void);
 // HGeantTof(HGeantTof &aTof);
  ~HGeantTof(void);
  inline void setTrack(Int_t aTrack) {trackNumber = aTrack;}
  void setHit(Float_t ae, Float_t ax, Float_t ay, Float_t atof,
              Float_t amom, Float_t alen);
  inline void setAddress (Char_t s, Char_t m, Char_t c);
  virtual Int_t getTrack(void) {return trackNumber;}
  void getHit(Float_t& ae, Float_t& ax, Float_t& ay, Float_t& atof,
              Float_t& amom, Float_t& alen);
  inline Char_t getSector(void) {return sector;}
  inline Char_t getModule(void) {return module;}
  inline Char_t getCell(void) {return cell;}
  inline Int_t getNLocationIndex(void) {return 3;}
  inline Int_t getLocationIndex(Int_t i); 

  ClassDef(HGeantTof,3) // GEANT TOF hit data class
};

inline void HGeantTof::setAddress(Char_t s, Char_t m, Char_t c) {
  sector = s;
  module = m;
  cell = c;
}

inline Int_t HGeantTof::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return sector;
    case 1 : return module;
    case 2 : return cell;
  }
  return -1;
}


#endif  /*! HGEANTTOF_H */











