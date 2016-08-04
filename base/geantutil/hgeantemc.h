//*************************************************************************
//
//  HGeantEmc
//
//  GEANT EMC hit data
//
//  
//*************************************************************************
#ifndef HGEANTEMC_H
#define HGEANTEMC_H

#include "hlinkeddataobject.h"

class HGeantEmc : public HLinkedDataObject {
private:
  Int_t   trackNumber;    // GEANT track number
  Float_t trackLength;    // track length  (in mm)
  Float_t peHit;          // number of photo electrons
  Float_t xHit;           // x of hit along crystal (in mm)
  Float_t yHit;           // y of hit along crystal (in mm)
  Float_t zHit;           // z of hit along crystal (in mm)
  Float_t tofHit;         // time of flight of hit  (in ns)
  Float_t momHit;         // total momentum of hitting particle (in MeV/c)
  Char_t  sector;         // sector number (0...5)
  UChar_t cell;           // crystal index (0...248 for standard geometry)
public:
  HGeantEmc(void);
//  HGeantEmc(HGeantEmc &aEmc);
  ~HGeantEmc(void) {}
  void setTrack(Int_t aTrack) {trackNumber = aTrack;}
  void setHit(Float_t ae, Float_t ax, Float_t ay, Float_t az, Float_t atof, Float_t amom, Float_t alen);
  inline void setAddress (Int_t s, Int_t c);
  Int_t getTrack(void) {return trackNumber;}
  void getHit(Float_t& ae, Float_t& ax, Float_t& ay, Float_t& az, Float_t& atof, Float_t& amom, Float_t& alen);
  Int_t getSector(void) {return sector;}
  Int_t getCell(void) {return cell;}
  Int_t getNLocationIndex(void) {return 2;}
  inline Int_t getLocationIndex(Int_t i); 

  ClassDef(HGeantEmc,1) // GEANT EMC hit data class
};

inline void HGeantEmc::setAddress(Int_t s, Int_t c) {
  sector = s;
  cell   = c;
}

inline Int_t HGeantEmc::getLocationIndex(Int_t i) {
  switch (i) {
    case 0 : return sector;
    case 1 : return cell;
  }
  return -1;
}


#endif  /*! HGEANTEMC_H */











