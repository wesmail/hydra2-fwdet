//*-- AUTHOR : Ilse Koenig
//*-- Createded : 10/03/2000
//*--Changed: F. Krizek  26/08/2005
//Modified by M.Golubeva 08.10.2005
//Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HWallGeomPar
//
// Container class for the basic Wall geometry parameters
//
//////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hwallgeompar.h"
#include "hgeomcompositevolume.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HWallGeomPar)

HWallGeomPar::HWallGeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Wall") {
  // constructor calling the base class constructor with the detector name
}

Bool_t HWallGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HWallParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HWallGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HWallParIo");
  if (out) return out->write(this);
  return -1;
}

Int_t HWallGeomPar::getCompNum(const TString& name) {
  // returns the cell index retrieved from the component name W00Xxxx
  Int_t length = name.Length();
  switch (length) {
    case 5:
      return ((Int_t)(name[4]-'0')-1);
      break;
    case 6:
      return ((Int_t)(name[4]-'0')*10+(Int_t)(name[5]-'0')-1);
      break;
    case 7:
      return ((Int_t)(name[4]-'0')*100+(Int_t)(name[5]-'0')*10+(Int_t)(name[6]-'0')-1);
      break;
    }
  return(-1);
}

TString HWallGeomPar::getCellName(const Int_t c) {
// returns cell name W00Xxxx retrieved from the cell index c
// c -> from 0 to 383; xxx -> from 1 to 385

  Int_t precision = 0;
  Int_t decimal, sign;
  Int_t c1=c+1;
  
  TString baseName1="W001";
  TString baseName2="W002";
  TString baseName3="W003";
  TString buffer;
  TString cellName; // cell name W00Xxxx
  
  buffer = fcvt ((Double_t)c1, precision, &decimal, &sign);
  
  //if(c<160) cellName=baseName1+buffer;
  //if(c>159&&c<248) cellName=baseName2+buffer;
  //if(c>250&&c<345) cellName=baseName3+buffer;// ATTENTION! THE UPPER LIMIT

  if(c<144) cellName=baseName1+buffer;
  if(c>143&&c<208) cellName=baseName2+buffer;
  if(c>209&&c<302) cellName=baseName3+buffer;// ATTENTION! THE UPPER LIMIT
  //cout<<cellName <<endl;

  return cellName;

}
