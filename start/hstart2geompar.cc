//*-- Created : 26/02/2014 by I.Koenig
//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HStart2GeomPar
//
// Container class for the basic Start geometry parameters used in the Pion
// beamtime (9 diamonds, each with 4 pads and mounted in two layers)
//
// Independent of the number of modules in the detector setup, only module 0
// is initialized.
//////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hstart2geompar.h"
#include "hgeomcompositevolume.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HStart2GeomPar)

HStart2GeomPar::HStart2GeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
              : HDetGeomPar(name,title,context,"Start") {
  // constructor calling the base class constructor with the detector name
  strcpy(detName, "Start");
  maxComponents = 9;
  maxCells      = maxComponents * 4;
  labPos = new TObjArray(maxCells);
  if (modules) {
    for (Int_t i=1;i<maxModules;++i) {
      if (modules->At(i)) modules->RemoveAt(i); // geometry only defined for module 0
      if (refVolumes && refVolumes->At(i)) refVolumes->RemoveAt(i);
    }
  }
  numComponents = maxComponents;
}

HStart2GeomPar::~HStart2GeomPar() {
  // destructor
  if (labPos) {
    labPos->Delete();
    delete labPos;
    labPos=0;
  }
}


Bool_t HStart2GeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  Bool_t rc = kFALSE;
  HDetParIo* input=inp->getDetParIo("HStartParIo");
  if (input) {
    for (Int_t i=1;i<maxModules;++i) set[i]=0;
    rc = input->init(this,set);
    if (rc) calcLabPositions();
  }
  return rc;
}

Int_t HStart2GeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HStartParIo");
  if (out) return out->write(this);
  return -1;
}

Int_t HStart2GeomPar::getCellIndex(Int_t compNum, Float_t xPos, Float_t yPos) {
  // returns cell index at position xPos,yPos in diamond compNum
  // Special geometry: 3 x 3 diamonds and each diamond is divided in 4 parts
  Int_t row=(Int_t)(compNum / 3);
  Int_t col=(compNum - row * 3) * 2;
  if (xPos>=0) col += 1;
  if (yPos>=0) row = row * 2 + 1;
  else row = row * 2; 
  return row * 6 + col;
}

void HStart2GeomPar::calcLabPositions(void) {
  // calculates the lab positions of a cell/strip
  if (changed) {
    HGeomCompositeVolume* refMod=getRefVolume(0);
    if (refMod) {
      HModGeomPar* module=getModule(0);
      if (module) {
        HGeomTransform trMod2Lab=module->getLabTransform();
        for(Int_t c=0;c<maxComponents;c++) {
          HGeomVolume* volu=refMod->getComponent(c);
          if (volu && strlen(volu->GetName())>0) {
            HGeomVector gPos=volu->getTransform().getTransVector();
            for (Int_t i=0;i<4;i++) {
              HGeomVector cPos =  *(volu->getPoint(i));
              cPos /= 2.;
              Int_t ind = getCellIndex(c,cPos.getX(),cPos.getY());
              cPos += gPos;
              HGeomVector* lpos=new HGeomVector;
              (*lpos)=trMod2Lab.transFrom(cPos);
              labPos->AddAt(lpos,ind);
            }
          }
        }
      }
    }      
  }   
}

void HStart2GeomPar::printLabPositions(void) {
  // prints the lab positions
  printf("Lab positions of the START detector cells\n");
  printf("cell xLab yLab zLab\n");
  for(Int_t i=0;i<=labPos->GetLast();i++) {
    HGeomVector* v=(HGeomVector*)(labPos->At(i));
    if (v) {
      printf("%i %9.3f %9.3f %9.3f \n",i,v->getX(),v->getY(),v->getZ());
    }
  }
}
