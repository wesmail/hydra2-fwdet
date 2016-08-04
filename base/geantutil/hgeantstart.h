//*************************************************************************
//
//  HGeantStart
//
//  GEANT START hit data
//
//  
//*************************************************************************
#ifndef HGEANTSTART_H
#define HGEANTSTART_H

#include "hlinkeddataobject.h"

class HGeantStart : public HLinkedDataObject {
private:
  Int_t   trackNumber;    // GEANT track number
  Float_t eHit;           // energy deposited  (in MeV)
  Float_t xHit;           // x of hit in diamond (in mm)
  Float_t yHit;           // y of hit in diamond (in mm)
  Float_t tofHit;         // time of flight of hit  (in ns)
  Float_t momHit;         // total momentum of hitting particle (in MeV/c)
  Float_t trackLength;    // track length  (in mm)
  Char_t  cell;           // diamond index
public:
  HGeantStart(void);
//  HGeantStart(HGeantStart &aStart);
  ~HGeantStart(void) {}
  void setTrack(Int_t aTrack) {trackNumber = aTrack;}
  void setHit(Float_t ae, Float_t ax, Float_t ay, Float_t atof, Float_t amom, Float_t alen);
  void setCell(Char_t c) {cell=c;}
  Int_t getTrack(void) {return trackNumber;}
  void getHit(Float_t& ae, Float_t& ax, Float_t& ay, Float_t& atof, Float_t& amom, Float_t& alen);
  Int_t getCell(void) {return cell;}
  Int_t getNLocationIndex(void) {return 1;}
  Int_t getLocationIndex(Int_t i) {return (i==0) ? cell : -1;} 

  ClassDef(HGeantStart,1) // GEANT START hit data class
};

#endif  /*! HGEANTSTART_H */











