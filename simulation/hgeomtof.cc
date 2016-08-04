//*-- AUTHOR : Ilse Koenig
//*-- Created : 10/11/2003

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HGeomTof
//
// Class for geometry of TOF
//
/////////////////////////////////////////////////////////////

#include "hgeomtof.h"
#include "hgeomtofhit.h"

ClassImp(HGeomTof)

HGeomTof::HGeomTof() {
  // Constructor
  fName="tof";
  maxSectors=6;
  maxModules=22;
  Int_t mod[]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1};
  if (!modules) modules=new TArrayI(maxSectors*maxModules);
  for (Int_t s=0;s<maxSectors;s++) { 
    for (Int_t m=0;m<maxModules;m++) modules->AddAt(mod[m],(s*maxModules+m));
  } 
  pHit=new HGeomTofHit(this);
}

const Char_t* HGeomTof::getModuleName(Int_t m) {
  // Returns the name of module m
  if (m<9) sprintf(modName,"T0%iF",m+1);
  else sprintf(modName,"T%iF",m+1);
  return modName;
}

const Char_t* HGeomTof::getEleName(Int_t m) {
  // Returns the element name in module m
  if (m<9) sprintf(eleName,"T0%i",m+1);
  else sprintf(eleName,"T%i",m+1);
  return eleName;
}
