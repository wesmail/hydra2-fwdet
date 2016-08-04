//*-- AUTHOR : Ilse Koenig
//*-- Created : 17/01/2005
// Modified by M.Golubeva 01.11.2006
// Modified by M.Golubeva 20.02.2007

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallDetector
//
//  The Forward Wall detector class
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hwalldetector.h"
#include "walldef.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hwallcalibrater.h"
#include "hwallcalpar.h" //FK//???????
#include "hwallhitf.h"//FK//
#include "hwallrefwinpar.h"
#include "hwallonehit.h"
#include "hwallonehitf.h"
#include "hades.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"
#include "hdetpario.h"
#include "hwallparrootfileio.h"
#include "hwallparasciifileio.h"
#include "TClass.h"
#include "TArrayI.h"
#include <iostream> 
#include <iomanip>

ClassImp(HWallDetector) // Forward Wall detector class

HWallDetector::HWallDetector(void) : HDetector("Wall","The Forward Wall detector") {
  // constructor
  maxSectors=-1;
  maxModules=1;
  maxComponents=302;
  //maxComponents=288;
  modules=new TArrayI(1);
}

HWallDetector::~HWallDetector(void) {
}

HCategory* HWallDetector::buildLinearCategory(const Text_t *classname,Float_t fillRate) {
  // builds a linear category for the Forward Wall
  HLinearCategory* category=new HLinearCategory(classname,(Int_t)(maxComponents*fillRate));
  return category;
}

HCategory* HWallDetector::buildMatrixCategory(const Text_t *classname,Float_t fillRate) {
  // builds a matrix category for the Forward Wall
  Int_t sizes[1];
  sizes[0]=maxComponents;
  HMatrixCategory* category=new HMatrixCategory(classname,1,sizes,fillRate);
  return category;
}

HCategory* HWallDetector::buildCategory(Cat_t cat) {
  // builds the categories for the Forward Wall
  switch (cat) {
  case catWallRaw : return buildMatrixCategory("HWallRaw",0.5);
  case catWallCal : return buildMatrixCategory("HWallCal",0.5);
  case catWallOneHit : return buildMatrixCategory("HWallOneHit",0.5);
  case catWallHit : return buildMatrixCategory("HWallHit",0.5);
    //  case catWallGeantRaw : return buildLinearCategory("HGeantWall",0.5);
  default : return NULL;
  }
}

void HWallDetector::activateParIo(HParIo* io) {
  // activates the input/output class for the parameters
  // needed by the Forward Wall
  if (strcmp(io->IsA()->GetName(),"HParOraIo")==0) {
    io->setDetParIo("HWallParIo");
    return;
  }
  if (strcmp(io->IsA()->GetName(),"HParRootFileIo")==0) {
    HWallParRootFileIo* p=new HWallParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
    io->setDetParIo(p);
  }
  if (strcmp(io->IsA()->GetName(),"HParAsciiFileIo")==0) {
    HWallParAsciiFileIo* p=new HWallParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
    io->setDetParIo(p);
  }
}

Bool_t HWallDetector::write(HParIo* output) {
  // writes the Forward Wall setup to the output
  HDetParIo* out=output->getDetParIo("HWallParIo");
  if (out) return out->write(this);
  return kFALSE;
}

Int_t HWallDetector::getMaxModInSetup(void) {
  Int_t maxMod=1;
  for(Int_t i=0;i<maxModules;i++) {
  if (modules->At(i)) maxMod=(i>maxMod)?i:maxMod;
  //cout <<"getMaxModInSetup " <<modules->At(i) <<" " <<maxMod <<endl;
  //maxMod=1;
  }
  //Int_t maxMod=1;
  return maxMod;
}
