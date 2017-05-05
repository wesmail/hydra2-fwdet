//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HEmcCellGeomPar
//
//  Class for EMC cell geometry parameters
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hemccellgeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hemcdetector.h"
#include "hemcgeompar.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hgeomvector.h"
#include <iostream>

ClassImp(HEmcCellGeomPar)

HEmcCellGeomPar::HEmcCellGeomPar(const Char_t* name,const Char_t* title,
                                 const Char_t* context)
               : HParSet(name,title,context) {
  // constructor
  labPos = NULL;
  pDet=(HEmcDetector*)(gHades->getSetup()->getDetector("Emc"));
  if (pDet) {
    nMaxCells = pDet->getMaxComponents();
    labPos = new TObjArray(pDet->getMaxSectors() * nMaxCells);
  } else {
    Error("HEmcCellGeomPar","No Emc detector");
  }
  pGeomPar=(HEmcGeomPar*)(gHades->getRuntimeDb()->getContainer("EmcGeomPar"));
  if (!pGeomPar){
    Error("initParContainer","No EmcGeomPar parameter container");
  }
}

HEmcCellGeomPar::~HEmcCellGeomPar(void) {
  // destructor deletes the array
  if (labPos) {
    labPos->Delete();
    delete labPos;
    labPos=0;
  }
}

void HEmcCellGeomPar::clear() {
  // clears the lab positions
  if (labPos) labPos->Delete();
}

Bool_t HEmcCellGeomPar::init(HParIo*,Int_t*) {
  // intitializes the container from geometry container
  if (!pGeomPar || !labPos) return kFALSE;
  if (pGeomPar->hasChanged()) {
    HGeomCompositeVolume* refMod=pGeomPar->getRefVolume(0);
    if (refMod) {
      for(Int_t m=0;m<pGeomPar->getNumModules();m++) {
        HModGeomPar* module=pGeomPar->getModule(m);
        if (module) {
          trMod2Lab[m]=module->getLabTransform();
          for(Int_t c=0;c<pGeomPar->getNumComponents();c++) {
            HGeomVolume* cell=refMod->getComponent(c);
            if (cell && strlen(cell->GetName())>0) {
              HGeomVector* lpos=new HGeomVector;
              HGeomVector cPos=cell->getTransform().getTransVector();
              (*lpos)=trMod2Lab[m].transFrom(cPos);
              labPos->AddAt(lpos,m*nMaxCells+c);
            }
          }
        }
      }      
    }
  }   
  return kTRUE;
}

void HEmcCellGeomPar::printParam(void) {
  // prints the parameters
  if (!labPos || !pDet) return;
  Int_t sec=-1, cell=0;
  Char_t row=-1, col=-1;
  printf("Cell geometry parameters for the EMC detector\n");
  printf("sector cell row column xLab yLab zLab\n");
  for(Int_t i=0;i<=labPos->GetLast();i++) {
    sec  = (Int_t)(i/nMaxCells);
    cell = i - sec*nMaxCells;  
    HGeomVector* v=(HGeomVector*)(labPos->At(i));
    if (v) {
      pDet->getRowCol(cell,row,col);
      printf("%i  %3i  %2i  %2i  %9.3f %9.3f %9.3f \n",sec,cell,row,col,v->getX(),v->getY(),v->getZ());
    }
  }
}

const HGeomVector* HEmcCellGeomPar::getLabPos(Int_t sec, Int_t cell) const {
  if (sec>=0 && sec<6 && cell>=0 && cell<nMaxCells) return (HGeomVector*)(labPos->At(sec*nMaxCells + cell));
  else return NULL;
}
  
const HGeomVector*  HEmcCellGeomPar::getLabPos(Int_t sec, Int_t row ,Int_t col) const {
  if (!pDet) return 0;
  Int_t cell=pDet->getCell(row,col);
  return getLabPos(sec,cell);
}

 










