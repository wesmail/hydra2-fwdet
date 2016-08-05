//*--- Author : Vladimir Pechenov
//*--- Modified: 19.06.07 by V.Pechenov
//*--- Modified: 16.08.05 by V.Pechenov
//*--- Modified: 11.05.02 by V. Pechenov
//*--- Modified: 08.05.01 by V. Pechenov
//*--- Modified: 07.03.01 by V. Pechenov
//*--- Modified: 27.05.00 by V. Pechenov
//*--- Modified: 24.05.00 by V. Pechenov
//*--- Modified: 07.03.00 by R. Holzmann
//*--- Modified: 31.08.99 by V. Pechenov

using namespace std;
#include "hmdcsizescells.h"
#include "hspectrometer.h"
#include "hmdcgeomstruct.h"
#include <iostream> 
#include <iomanip>
#include "math.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hmdcgetcontainers.h"
#include "hmdclayergeompar.h"
#include "hmdcgeompar.h"
#include "hmdclookupraw.h"
#include "hmdclayercorrpar.h"
#include "hmdcrawstruct.h"
#include "hruntimedb.h"
#include "hspecgeompar.h"
#include <stdlib.h>

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
//  HMdcSizesCells
//  HMdcSizesCellsSec
//  HMdcSizesCellsMod
//  HMdcSizesCellsLayer
//  HMdcSizesCellsCell
//
//  Creating the container and geting the pointer to cont.:
//  HMdcSizesCells* fSizesCells=HMdcSizesCells::getObject();
//
//  Geting the pointer to cont. (without creating):
//  HMdcSizesCells* fSizesCells=HMdcSizesCells::getExObject();
//
//  Deleting of container:
//  HMdcSizesCells::deleteCont();
//
//  Container class keep volumes sizes of wires (2-points/wire)
//  in coordinate system of sector and transformations layer<->mod,
//  layer<->sec, ...
//  It has same useful functions:
//  HMdcSizesCellsCell
//    Float_t getWireLength(void)
//
//  HMdcSizesCellsLayer:
//    HGeomTransform* getSecTrans();
//    HGeomTransform* getModTrans();
//    void transSecToRotLayer(HGeomVector& point);
//    Double_t getAlpha(const HGeomVector& p1, const HGeomVector& p2);
//    Double_t getDist(const HGeomVector& p1,
//                     const HGeomVector& p2, Int_t cell);
//    void getImpact(const HGeomVector& p1, const HGeomVector& p2,
//                  Int_t cell, Double_t &alpha, Double_t &per);
//    Double_t getImpact(Double_t x1,Double_t y1,Double_t z1,
//        Double_t x2,Double_t y2,Double_t z2, Int_t cell, Double_t &alphaPerp);
//    Bool_t getImpact(Double_t x1,Double_t y1,Double_t z1,
//        Double_t x2, Double_t y2, Double_t z2,
//        Int_t cell, Double_t &alphaDrDist, Double_t &driftDist);
//    Int_t getCellNum(Double_t x, Double_t y):
//      calculation the the cell number for the point x,y
//      on the wire plane. (x,y - in coor. sys. of layer, z=0)
//    Double_t getXSign(Double_t x1, Double_t y1, Double_t z1,
//        Double_t x2, Double_t y2, Double_t z2, Int_t cell)
//    Float_t getSignPath(Double_t x1,Double_t y1,Double_t z1,
//        Double_t x2, Double_t y2, Double_t z2, Int_t cell);
//    Float_t getSignPath(Double_t x1,Double_t y1,Double_t z1,
//        Double_t x2, Double_t y2, Double_t z2, Int_t cell, Float_t& outside);    
//
//  HMdcSizesCellsMod:
//    HGeomTransform* getSecTrans();
//
//  HMdcSizesCellsSec:
//    HGeomTransform* getLabTrans();
//
//  HMdcSizesCells:
//    Bool_t getCellVol(Int_t sec, Int_t mod, Int_t lay,
//        Int_t cell, HMdcTrap& volCell);
//    Char_t findReadOutSide(Int_t s,Int_t m,Int_t l,Int_t c)
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcSizesCellsCell)
ClassImp(HMdcSizesCellsLayer)
ClassImp(HMdcSizesCellsMod)
ClassImp(HMdcSizesCellsSec)
ClassImp(HMdcSizesCells)
    
void HMdcSizesCellsCell::clear(void) {
  wirePnt1.clear();
  wirePnt2.clear();
  status      = kFALSE;
  xReadout    = 0.;
  readOutSide = '0';
  yWirePos    = 0.;
}

HMdcSizesCells* HMdcSizesCellsLayer::getSizesCells(void) {
  return pToMod->getSizesCells();
}

HMdcSizesCells* HMdcSizesCellsMod::getSizesCells(void) {
  return pToSec->getSizesCells();
}

HMdcSizesCells* HMdcSizesCellsSec::getSizesCells(void) {
  return pToSC;
}

HMdcSizesCellsLayer::HMdcSizesCellsLayer(void) {
  cellsArray = NULL;
  nCells     = 0;
  // Constructor
}

void HMdcSizesCellsLayer::initLayer(HMdcSizesCellsMod* pMod, Int_t lay) {
  // Constructor creates an array of pointers of type HMdcSizesCellsCell
  sector = pMod->getSec();
  module = pMod->getMod();
  layer  = lay;
  pToMod = pMod;
  HMdcGetContainers* fGetCont       = HMdcGetContainers::getObject();
  HMdcGeomStruct*    fMdcGeomStruct = fGetCont->getMdcGeomStruct();

  if(nCells==0 || nCells != ((*fMdcGeomStruct)[sector][module])[layer]) {
    nCells = ((*fMdcGeomStruct)[sector][module])[layer];
    if(cellsArray != NULL) delete [] cellsArray;
    if( nCells > 0 ) cellsArray = new HMdcSizesCellsCell [nCells];
    else             cellsArray = NULL;
  }
  firstCellPart2  = 777;  // 777 - no correction for this layer
  wireOffsetPart2 = 0.;
  cosWireOrPart2  = 0.;
  sinWireOrPart2  = 0.;
}

HMdcSizesCellsLayer::~HMdcSizesCellsLayer(void) {
  // destructor
  if(cellsArray) {
    delete [] cellsArray;
    cellsArray = NULL;
  }
}

Bool_t HMdcSizesCellsLayer::setSecTrans(Double_t corZ) {
  // alignTrans: coord. system of layer will transformed to "alignTrans" sys.
  HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
  if(!fGetCont) return kFALSE;
  sysRMod = fGetCont->getModTransLayer(sector,module,layer);
  
  fGetCont->getModTransGeantLayer(geantSysRMod,sector,module,layer);
  fGetCont->getSecTransGeantLayer(geantSysRSec,sector,module,layer);
  geantPlane.setPlanePar(geantSysRSec);

  HGeomVector vc(sysRMod.getTransVector());
  if(corZ!=0) { // For alignment.
    vc.setZ(vc.getZ()+corZ);
    sysRMod.setTransVector(vc);
  }
  zModule = vc.getZ();
  sysRSec = sysRMod;
  sysRSec.transFrom(*(pToMod->getSecTrans()));
  HMdcSizesCells::copyTransfToArr(sysRSec,tSysRSec);
  setPlanePar(sysRSec);
  
  HGeomRotation rotLay;
  rotLay.setUnitMatrix();
  rotLay.setElement( cosWireOr,0);
  rotLay.setElement(-sinWireOr,1);
  rotLay.setElement( sinWireOr,3);
  rotLay.setElement( cosWireOr,4);
  rotLaySysRMod.clear();
  rotLaySysRMod.setRotMatrix(rotLay);
  rotLaySysRMod.transFrom(sysRMod);
  rotLaySysRSec.clear();
  rotLaySysRSec.setRotMatrix(rotLay);
  rotLaySysRSec.transFrom(sysRSec);
  HMdcSizesCells::copyTransfToArr(rotLaySysRSec,tRLaySysRSec);
  pntToCell[0] = tRLaySysRSec[1]*invPitch;
  pntToCell[1] = tRLaySysRSec[4]*invPitch;
  pntToCell[2] = tRLaySysRSec[7]*invPitch;
  pntToCell[3] = cellOffset - invPitch*(tRLaySysRSec[1]*tRLaySysRSec[9]+tRLaySysRSec[4]*tRLaySysRSec[10]
                                       +tRLaySysRSec[7]*tRLaySysRSec[11]);
  
  if(firstCellPart2<nCells) {
    rotLay.setUnitMatrix();
    rotLay.setElement( cosWireOrPart2,0);
    rotLay.setElement(-sinWireOrPart2,1);
    rotLay.setElement( sinWireOrPart2,3);
    rotLay.setElement( cosWireOrPart2,4);
    rotLaySysRModPart2.clear();
    rotLaySysRModPart2.setRotMatrix(rotLay);
    rotLaySysRModPart2.transFrom(sysRMod);
    rotLaySysRSecPart2.clear();
    rotLaySysRSecPart2.setRotMatrix(rotLay);
    rotLaySysRSecPart2.transFrom(sysRSec);
    HMdcSizesCells::copyTransfToArr(rotLaySysRSecPart2,tRLaySysRSecPart2);
  } else {
    rotLaySysRModPart2 = rotLaySysRMod;
    rotLaySysRSecPart2 = rotLaySysRSec;
    HMdcSizesCells::copyTransfToArr(rotLaySysRSecPart2,tRLaySysRSecPart2);
  }
  
  return kTRUE;
}

Double_t HMdcSizesCellsLayer::calcWire(Double_t x, Double_t y) const{
  // return point on the layer plane in wire float number scale!
  Double_t yRot = y*cosWireOr-x*sinWireOr;
  Double_t wire = (wireOffset + yRot)*invPitch;
  if(Int_t(wire+0.5) >= firstCellPart2) {
    yRot = y*cosWireOrPart2-x*sinWireOrPart2;
    wire = (wireOffsetPart2 + yRot)*invPitch;
  }
  return wire;
}

Int_t HMdcSizesCellsLayer::calcCellNum(Double_t x, Double_t y) const{
  //  calculation the the cell number for the point x,y
  //  on the wire plane. (x,y - in coor. sys. of layer, z=0)
  Double_t wire  = calcWire(x,y) + 0.5;
  Int_t    nWire = (Int_t)wire;
  return (wire<0. || nWire>=nCells) ? -1:nWire;
}

Int_t HMdcSizesCellsLayer::distanceSign(const HMdcLineParam& ln, Int_t cell) const{
  // HMdcLineParam ln must be in sector coor.system
  // return +1 if line path OVER wire in layer(mdc,sector) coor.system
  // return -1 if line path UNDER wire in layer(mdc,sector) coor.system
  // return 0. if cell<0 or cell>=nCells in layer
  if(cell<0 || cell>=nCells) return 0;
  Double_t x,y,z;
  Int_t parSec = ln.getSec();
  if(parSec==sector || parSec<0) {
    calcIntersection(ln.x1(),ln.y1(),ln.z1(),ln.x2(),ln.y2(),ln.z2(), x,y,z);
  } else {
    HGeomVector p1,p2;
    HMdcSizesCells* pSCells = HMdcSizesCells::getObject();
    pSCells->transLineToOtherSec(ln,sector,p1,p2);
    calcIntersection(p1.X(),p1.Y(),p1.Z(),p2.X(),p2.Y(),p2.Z(), x,y,z);
  }
  transTo(x,y,z);
  Double_t wire = calcWire(x,y) - cell;
  return (wire >= 0.) ? 1 : -1;
}

Bool_t HMdcSizesCellsLayer::calcCrCellsGeantMdc(Float_t &x,Float_t &y,Float_t theta,Float_t phi,
                                             Int_t& c1, Int_t& c2) const {
  // Calculate cells crossed by GeantMdc hit (x,y,theta,phi) for mdc digitizer.
  // return kFALSE if no cells crossed
  // return x,y corrected to the layer plane
  
  // Correction for HGeantMdc hit Z position! (zGeantHit)
  Double_t dXY    = -zGeantHit * TMath::Tan(theta * TMath::DegToRad());
  Double_t sinPhi = TMath::Sin(phi * TMath::DegToRad());
  Double_t cosPhi = TMath::Cos(phi * TMath::DegToRad());
  y += dXY * sinPhi;
  x += dXY * cosPhi;
    
  Double_t dPt = halfCatDist * TMath::Tan(theta * TMath::DegToRad());
  Double_t yc1= y - dPt * sinPhi;
  Double_t yc2= y + dPt * sinPhi;
  Double_t xc1= x - dPt * cosPhi;
  Double_t xc2= x + dPt * cosPhi;
  Double_t fc1 = calcWire(xc1,yc1) + 0.5;
  Double_t fc2 = calcWire(xc2,yc2) + 0.5;
  if(fc1 <= fc2) {
    c1 = fc1 < 0. ? 0 : fc1;
    c2 = fc2;
  } else {
    c2 = fc1;
    c1 = fc2 < 0. ? 0 : fc2;
  }
  if(fc1 < 0. && fc2 < 0.) return kFALSE;
  if(c1 >= nCells)         return kFALSE;
  if(c2 >= nCells)         c2 = nCells-1;
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::calcCrossedCells(const HMdcLineParam& ln,Float_t& cell1, Float_t& cell2) const {
  // calculation of cells crossed by line "ln" 
  // ln - sector coordinate system
  Double_t tanAlpha;
  Double_t y = calcImpactParam(ln,tanAlpha);
  Double_t dCell = halfCatDist*tanAlpha*invPitch;
  Double_t cell = (wireOffset + y)*invPitch + 0.5;
  cell1 = cell - dCell;
  cell2 = cell + dCell;
  if(cell2 < 0.) return kFALSE;
  if(cell1 < 0.) cell1 = 0.;
  else if(Int_t(cell1+0.5) >= firstCellPart2) {
    y = calcImpactParam(ln,tanAlpha,kFALSE);
    Double_t dCell = halfCatDist*tanAlpha*invPitch;
    cell = (wireOffset + y)*invPitch + 0.5;
    cell1 = cell - dCell;
    cell2 = cell + dCell;
  } else if(Int_t(cell2+0.5) >= firstCellPart2) {
    y = calcImpactParam(ln,tanAlpha,kFALSE);
    Double_t dCell = halfCatDist*tanAlpha*invPitch;
    cell = (wireOffset + y)*invPitch + 0.5;
    cell2 = cell + dCell;
  }
  if(cell1 >= nCells) return kFALSE;
  if(cell2 >= nCells) cell2 = nCells-0.0001;//nCells-1;
  return kTRUE;
}

Double_t HMdcSizesCellsLayer::calcImpactParam(const HMdcLineParam& ln,Double_t &tanAlpha,
    Bool_t isLayerFirstPart) const {
  //  return: y of the cross point of line "ln" with layer in coor.sys. of rotated layer
  //  and abs(tanAlpha)- alpha is impact angle of "ln"
  Double_t y,z,dy,dz;
  const Double_t *t = isLayerFirstPart ? tRLaySysRSec : tRLaySysRSecPart2;
  rotateTo(t,ln.x1()-t[9],ln.y1()-t[10],ln.z1()-t[11],y,z);
  rotateTo(t,ln.getDir(),dy,dz);
  Double_t dYZ = dy/dz;
  tanAlpha = TMath::Abs(dYZ);
  return y-z*dYZ;  // y is on the layer plane
}

Bool_t HMdcSizesCellsLayer::calcCrossedCells(Double_t x1, Double_t y1, 
    Double_t z1, Double_t x2, Double_t y2, Double_t z2, 
    Float_t& cell1, Float_t& cell2) const {
  // calculation of cells crossed by line x1,y1,z1-x2,y2,z2
  // x1,y1,z1, x2,y2,z2 - sector coordinate system
  transTo(x1,y1,z1);
  transTo(x2,y2,z2);
  Double_t dX  = x2-x1;
  Double_t dY  = y2-y1;
  Double_t dZI = z2-z1;
  if(dZI==0.) return kFALSE;
  dZI          = 1/dZI;
  Double_t tmp = (halfCatDist-z1)*dZI;
  Double_t x   = tmp*dX+x1;
  Double_t y   = tmp*dY+y1;
  cell1        = calcWire(x,y) + 0.5;
  tmp          = (-halfCatDist-z1)*dZI;
  x            = tmp*dX+x1;
  y            = tmp*dY+y1;
  cell2        = calcWire(x,y) + 0.5;
  if(cell1>cell2) {
    Float_t c = cell1;
    cell1     = cell2;
    cell2     = c;
  }
  if(cell1<0.) {
    if(cell2<0.) return kFALSE;
    cell1 = 0.;
  }
  if(cell2>=nCells) {
    if(cell1>=nCells) return kFALSE;
    cell2 = nCells-0.0001;//nCells-1;
  }
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::calcCrossedCells(Double_t x1, Double_t y1, 
    Double_t z1, Double_t x2, Double_t y2, Double_t z2, 
    Int_t& firstCell, Int_t& lastCell, 
    Float_t &firstCellPath,Float_t &midCellPath,Float_t &lastCellPath) const {
  // calculation of cells crossed by line x1,y1,z1 - x2,y2,z2
  // x1,y1,z1, x2,y2,z2 - sector coordinate system
  //
  // return kTRUE if crossed cell number >=0 and < nCells_in_layer
  // return region of crossed cells: firstCell - lastCell
  // and track path in cells:
  // firstCellPath for cell=firstCell
  // midCellPath   for firstCell<cell<lastCell
  // lastCellPath  for cell=lastCell
  //
  // midCellPath  = 0 if one or two cells crossed only
  // lastCellPath = 0 if one cell crossed only
  //
  // Exmaple:
  //  HMdcSizesCells* pSizesCell;
  //  HMdcSizesCellsLayers &sizesCellsLayer = (*pSizesCell)[sector][module][layer];
  //  Int_t firstCell,lastCell; 
  //  Float_t firstCellPath,midCellPath,lastCellPath;
  //  if(sizesCellsLayer->calcCrossedCells(x1,y1,z1,x2,y2,z2,firstCell,lastCell, 
  //                                       firstCellPath,midCellPath,lastCellPath) {
  //    Float_t totalePathInLayer = 0.;
  //    for(Int_t c=firstCell;c<=lastCell;++c) {
  //      Float_t cellPath;
  //      if   (c == firstCell) cellPath = firstCellPath;
  //      else (c == lastCell)  cellPath = lastCellPath;
  //      else                  cellPath = midCellPath;
  //      totalePathInLayer += cellPath;
  //      .... 
  //    }
  //  }
  
  // Transform line to the layer coor.system:
  transTo(x1,y1,z1);
  transTo(x2,y2,z2);
  
  Double_t dZ = z2-z1;
  if(dZ == 0.) return kFALSE;
  Double_t dXdZ  = (x2-x1)/dZ;
  Double_t dYdZ  = (y2-y1)/dZ;

  Double_t layerPath = 2*halfCatDist*TMath::Sqrt(dXdZ*dXdZ + dYdZ*dYdZ + 1.); 
  Float_t  cl1 = calcWire(( halfCatDist-z1)*dXdZ+x1,( halfCatDist-z1)*dYdZ+y1) + 0.5;
  Float_t  cl2 = calcWire((-halfCatDist-z1)*dXdZ+x1,(-halfCatDist-z1)*dYdZ+y1) + 0.5;

  if(cl1>cl2) {
    Float_t c = cl1;
    cl1       = cl2;
    cl2       = c;
  }
  if(cl2 < 0. || Short_t(cl1) >= nCells) return kFALSE;
  
  firstCell     = cl1;
  lastCell      = cl2;
  firstCellPath = layerPath;
  midCellPath   = 0.;
  lastCellPath  = 0.;
  if(firstCell == lastCell) return kTRUE;  // return in one cell crossed only
  
  // Calc. path length in cells: 
  Double_t pathPerCell = layerPath/(cl2-cl1);
  firstCellPath = pathPerCell * (firstCell + 1. - cl1);
  lastCellPath  = pathPerCell * (cl2 - lastCell);
  
  // Check cell number boundaries:
  if(cl1<0.) {
    firstCell     = 0;
    firstCellPath = firstCell==lastCell ? lastCellPath : pathPerCell;
  }
  if(cl2>=nCells) {
    lastCell     = nCells-1;
    lastCellPath = pathPerCell;
  }
  if(firstCell == lastCell) lastCellPath = 0.;
  else if(lastCell - firstCell > 1) midCellPath = pathPerCell;
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::calcSegCrossCells(Double_t z,Double_t r,Double_t theta,Double_t phi,
    Int_t& firstCell, Int_t& lastCell, 
    Float_t &firstCellPath,Float_t &midCellPath,Float_t &lastCellPath) const {
  // calculation of cells crossed by line z,r,theta,phi (parameters of HMdcSeg)
  //    z,r [mm]   theta,phi [rad]
  //
  // return kTRUE if crossed cell number >=0 and < nCells_in_layer
  // return region of crossed cells: firstCell - lastCell
  // and track path in cells:
  // firstCellPath for cell=firstCell
  // midCellPath   for firstCell<cell<lastCell
  // lastCellPath  for cell=lastCell
  //
  // midCellPath  = 0 if one or two cells crossed only
  // lastCellPath = 0 if one cell crossed only
  //
  // Exmaple:
  //  HMdcSizesCells* pSizesCell;
  //  HMdcSizesCellsLayers &sizesCellsLayer = (*pSizesCell)[sector][module][layer];
  //  Int_t firstCell,lastCell; 
  //  Float_t firstCellPath,midCellPath,lastCellPath;
  //  if(sizesCellsLayer->calcCrossedCells(Zmin,Rmin,theta,phi,firstCell,lastCell, 
  //                                       firstCellPath,midCellPath,lastCellPath) {
  //    Float_t totalePathInLayer = 0.;
  //    for(Int_t c=firstCell;c<=lastCell;++c) {
  //      Float_t cellPath;
  //      if   (c == firstCell) cellPath = firstCellPath;
  //      else (c == lastCell)  cellPath = lastCellPath;
  //      else                  cellPath = midCellPath;
  //      totalePathInLayer += cellPath;
  //      .... 
  //    }
  //  }
  
  Double_t cosPhi = TMath::Cos(phi);
  Double_t sinPhi = TMath::Sin(phi);
  Double_t x      = -r*sinPhi;   
  Double_t y      =  r*cosPhi;
  Double_t dZ0    = TMath::Cos(theta);
  Double_t dxy    = TMath::Sqrt(1.-dZ0*dZ0);
  
  // Transform line to the layer coor.system:
  transTo(x,y,z);
  Double_t dX,dY,dZ;
  rotVectToLay(dxy*cosPhi,dxy*sinPhi,dZ0, dX,dY,dZ );
  
  if(dZ == 0.) return kFALSE;
  Double_t dXdZ  = dX/dZ;
  Double_t dYdZ  = dY/dZ;
  
  Double_t layerPath = 2*halfCatDist*TMath::Sqrt(dXdZ*dXdZ + dYdZ*dYdZ + 1.); 
  Float_t  cl1 = calcWire(( halfCatDist-z)*dXdZ+x,( halfCatDist-z)*dYdZ+y) + 0.5;
  Float_t  cl2 = calcWire((-halfCatDist-z)*dXdZ+x,(-halfCatDist-z)*dYdZ+y) + 0.5;
  
  if(cl1>cl2) {
    Float_t c = cl1;
    cl1       = cl2;
    cl2       = c;
  }
  if(cl2 < 0. || Short_t(cl1) >= nCells) return kFALSE;
  
  firstCell     = cl1;
  lastCell      = cl2;
  firstCellPath = layerPath;
  midCellPath   = 0.;
  lastCellPath  = 0.;
  if(firstCell == lastCell) return kTRUE;  // return in one cell crossed only 

  // Calc. path length in cells:  
  Double_t pathPerCell = layerPath/(cl2-cl1);
  firstCellPath = pathPerCell * (firstCell + 1. - cl1);
  lastCellPath  = pathPerCell * (cl2 - lastCell);
  
  // Check cell number boundaries:
  if(cl1<0.) {
    firstCell     = 0;
    firstCellPath = firstCell==lastCell ? lastCellPath : pathPerCell;
  }
  if(cl2>=nCells) {
    lastCell     = nCells-1;
    lastCellPath = pathPerCell;
  }
  
  if(firstCell == lastCell) lastCellPath = 0.;
  else if(lastCell - firstCell > 1) midCellPath = pathPerCell;
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::calcCrossedCellsPar(const HMdcLineParam& ln,Double_t extendCells,
  Int_t &c1,Int_t &c2,Int_t &c3,Double_t *al,Double_t *md,Double_t *st) const {

  // calculation of cells crossed by line "ln" 
  // ln - sector coordinate system
  Double_t dYZ;
  Double_t y     = calcImpactParam(ln,dYZ);
  Double_t cell  = (wireOffset + y)*invPitch + 0.5;
  Double_t dCell = halfCatDist*invPitch*dYZ + extendCells;
  
  if(cell + dCell < 0.) return kFALSE;                          // out of layer
  c1 = cell - dCell < 0. ? 0 : cell - dCell;
  if(c1 < firstCellPart2) {
    if(c1 >= nCells) return kFALSE;                             // out of layer
    Double_t p1s = 1./TMath::Sqrt(1.+dYZ*dYZ);
    st[0] = pitch*p1s;
    md[0] = (cellsArray[c1].getWirePos()-y)*p1s; 
    al[0] = calcAlphaImpact(dYZ); //TMath::ATan2(1.,dYZ)*TMath::RadToDeg();

    if(cell-dCell > 260.) return kFALSE;                        // out of layer
    c2 = cell + dCell  + 1;
    if(c2 <= firstCellPart2) {
      if(c2 > nCells) c2 = nCells;
      c3 = -1;
      return kTRUE;
    } else c2 = firstCellPart2;
  } else c1 = -1;
  
  y     = calcImpactParam(ln,dYZ,kFALSE);
  cell  = (wireOffsetPart2 + y)*invPitch + 0.5;
  dCell = halfCatDist*invPitch*dYZ + extendCells;
  
  if(c1 < 0) { // No crossed cells in first layer part:
    c2 = cell - dCell;
    if(c2 >=  nCells) return kFALSE;                            // out of layer
    if(c2 < firstCellPart2) c2 = firstCellPart2;
  }
  c3 = cell + dCell +1;
  if(c3 > nCells) c3 = nCells;
  
  Double_t p1s = 1./TMath::Sqrt(1.+dYZ*dYZ);
  st[1] = pitch*p1s;
  md[1] = (cellsArray[c2].getWirePos()-y)*p1s; 
  al[1] = calcAlphaImpact(dYZ); //TMath::ATan2(1.,dYZ)*TMath::RadToDeg();
  
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::calcCrossedCellsPar(const Double_t *v,Double_t extendCells,
     Int_t &c1,Int_t &c2,Int_t &c3,Double_t *al,Double_t *md,Double_t *st) const {
  // calculation of cells crossed by line "v"
  // v[0-2] = x,y,z - point of line
  //          dx,dy,dz - direction of line
  // v[3] = dx/dz;
  // v[4] = dy/dz;

  Double_t z   = v[2] - zModule;
  Double_t dYZ = cosWireOr*v[4] - sinWireOr*v[3];
  Double_t y   = cosWireOr*v[1] - sinWireOr*v[0] - z*dYZ;
  if(dYZ < 0.) dYZ = -dYZ;
  
  Double_t dCell = halfCDsDivPitch*dYZ + extendCells;
  if(dCell > 10.) return kFALSE;
  Double_t cell  = (wireOffset + y)*invPitch + 0.5;
  if(cell > 260. || cell < -20.) return kFALSE;

  Double_t maxCell = cell + dCell;
  if(maxCell < 0.) return kFALSE;                               // out of layer
  c1 = cell - dCell < 0. ? 0 : cell - dCell;
  if(c1 < firstCellPart2) {
    if(c1 >= nCells) return kFALSE;                             // out of layer
    Double_t p1s = 1./TMath::Sqrt(1.+dYZ*dYZ);
    st[0] = pitch*p1s;
    md[0] = (cellsArray[c1].getWirePos()-y)*p1s;
   
    al[0] = calcAlphaImpact(dYZ); //TMath::ATan2(1.,dYZ)*TMath::RadToDeg();
    c2 = maxCell  + 1;
    if(c2 <= firstCellPart2) {
      if(c2 >= nCells) c2 = nCells-1;
      c3 = -1;
      return kTRUE;
    } else c2 = firstCellPart2;
  } else c1 = -1;
  
  dYZ = cosWireOrPart2*v[4] - sinWireOrPart2*v[3];
  y   = cosWireOrPart2*v[1] - sinWireOrPart2*v[0] - z*dYZ;
  if(dYZ < 0.) dYZ = -dYZ;
  
  cell    = (wireOffsetPart2 + y)*invPitch + 0.5;
  dCell   = halfCDsDivPitch*dYZ + extendCells;
  maxCell = cell + dCell;
  
  if(c1 < 0) { // No crossed cells in first layer part:
    c2 = cell - dCell;
    if(c2 >=  nCells) return kFALSE;                            // out of layer
    if(c2 < firstCellPart2) c2 = firstCellPart2;
  }
  c3 = maxCell +1;
  if(c3 > nCells) c3 = nCells;
  
  Double_t p1s = 1./TMath::Sqrt(1.+dYZ*dYZ);
  st[1] = pitch*p1s;
  md[1] = (cellsArray[c2].getWirePos()-y)*p1s; 
  al[1] = calcAlphaImpact(dYZ); //TMath::ATan2(1.,dYZ)*TMath::RadToDeg();
  
  return kTRUE;
}

void HMdcSizesCellsLayer::transSecToRotLayer(Int_t c,HGeomVector& point) const {
  // transformation to coor. sys. of rotated layer from sector sys.
  point = getRotLaySysRSec(c).transTo(point);
}

Double_t HMdcSizesCellsLayer::getAlpha(Int_t c,const HGeomVector& p1,const HGeomVector& p2) const {
  // return the angle alpha (in degree) between projection of line p1-p2
  // on the Y-Z plane and Y axis in coor.sys. of wires in degree.
  // p1,p2 in coordinate system of sector.
  const HGeomTransform& trans = getRotLaySysRSec(c);
  HGeomVector tranP1(trans.transTo(p1));
  HGeomVector tranP2(trans.transTo(p2));
  return TMath::ATan2( TMath::Abs(tranP2.getZ()-tranP1.getZ()),
                       TMath::Abs(tranP2.getY()-tranP1.getY()) )*TMath::RadToDeg();
}

Double_t HMdcSizesCellsLayer::getDist(const HGeomVector& p1,const HGeomVector& p2,Int_t cell){
  // return the minimal distance from line (p1-p2) to wire.
  // p1,p2 are in coordinate system of sector.
  const HGeomTransform& trans = getRotLaySysRSec(cell);
  HGeomVector tranP1(trans.transTo(p1));
  HGeomVector tranP2(trans.transTo(p2));
  Double_t yWire = cellsArray[cell].getWirePos();
  Double_t y1    = tranP1.getY() - yWire;
  Double_t y2    = tranP2.getY() - yWire;
  Double_t dz    = tranP1.getZ() - tranP2.getZ();
  Double_t dy    = y1-y2;
  return TMath::Abs((tranP1.getZ()*y2-tranP2.getZ()*y1)/TMath::Sqrt(dz*dz+dy*dy));
}

void HMdcSizesCellsLayer::getImpact(const HGeomVector& p1,const HGeomVector& p2,Int_t cell,
                                    Double_t &alpha,Double_t &minDist) const {
  // calc. the angle alpha (in degree) between projection of line p1-p2
  // on the Y-Z plane and Y axis in coor.sys. of wire (y=z=0) and
  // the minimal distance from line (p1-p2) to wire.
  // p1,p2 - in sector coor.sys.
  const HGeomTransform& trans = getRotLaySysRSec(cell);
  HGeomVector tranP1(trans.transTo(p1));
  HGeomVector tranP2(trans.transTo(p2));
  Double_t yWire = cellsArray[cell].getWirePos();
  Double_t y1    = tranP1.getY() - yWire;
  Double_t y2    = tranP2.getY() - yWire;
  Double_t dz    = tranP2.getZ() - tranP1.getZ();
  Double_t dy    = y2-y1;
  Double_t lng   = 1./TMath::Sqrt(dz*dz+dy*dy);
  alpha          = TMath::ASin(TMath::Abs(dz)*lng)*TMath::RadToDeg();
  minDist        = TMath::Abs((tranP1.getZ()*y2-tranP2.getZ()*y1)*lng);
}

Double_t HMdcSizesCellsLayer::getImpactLSys(const HGeomVector& p1l,const HGeomVector& p2l,
                                            Int_t cell, Double_t &alpha,Int_t& distSign) const {
  // p1l,p2l - in ROTATED LAYER COOR.SYS.!!!
  // calc. the angle alpha (in degree) between projection of line p1-p2
  // on the Y-Z plane and Y axis in coor.sys. of wire (y=z=0) and
  // the minimal distance from line (p1-p2) to wire.
  Double_t dz = p2l.getZ() - p1l.getZ();
  Double_t dy = p2l.getY() - p1l.getY();
  Double_t y  = p1l.getY() - cellsArray[cell].getWirePos();
  Double_t z  = p1l.getZ();
  distSign    = y >= z*dy/dz ? +1 : -1;   // For derivatives calculation in alignment
  alpha       = calcAlphaImpact(dy/dz); //TMath::ATan2(TMath::Abs(dz),TMath::Abs(dy))*TMath::RadToDeg();
  return TMath::Abs((y*dz-z*dy)/TMath::Sqrt(dz*dz+dy*dy));
}

Double_t HMdcSizesCellsLayer::getImpact(
    Double_t x1,Double_t y1,Double_t z1,
    Double_t x2,Double_t y2,Double_t z2, Int_t cell,Double_t &alpha) const {
  // calc. the angle alpha (in degree) between projection of line
  // x1,y1,z1-x2,y2,z2 on the Y-Z plane and Y axis in coor.sys.
  // of wires and the minimal distance from line x1,y1,z1-x2,y2,z2
  // to wire.
  // x1,y1,z1,x2,y2,z2 - in sector coor.sys. 
  Double_t y,z,dy,dz;
  const Double_t *transArr = getRLSysRSec(cell);
  rotateTo(transArr,x1-transArr[9],y1-transArr[10],z1-transArr[11],y,z);
  y -= cellsArray[cell].getWirePos();
  rotateTo(transArr,x2-x1,y2-y1,z2-z1,dy,dz);
  alpha = calcAlphaImpact(dy/dz); //TMath::ATan2(TMath::Abs(dz),TMath::Abs(dy))*TMath::RadToDeg();
  return TMath::Abs((y*dz-z*dy)/TMath::Sqrt(dz*dz+dy*dy));
}

Bool_t HMdcSizesCellsLayer::getImpact(Double_t x1, Double_t y1, Double_t z1,
                                      Double_t x2, Double_t y2, Double_t z2,
    Int_t cell, Double_t &alpha, Double_t &minDist) const {
  // return kTRUE if line x1,y1,z1-x2,y2,z2 intersect cell or kFALSE
  // if not intersect and calc. the angle alpha (in degree) between projection
  // of the line  x1,y1,z1-x2,y2,z2 on the Y-Z plane and Y axis in coor.
  // sys. of wires and the minimal distance from line x1,y1,z1-x2,y2,z2
  // to wire.
  // x1,y1,z1,x2,y2,z2 - in sector coor.sys.
  Double_t y,z,dy,dz;
  const Double_t *transArr = getRLSysRSec(cell);
  rotateTo(transArr,x1-transArr[9],y1-transArr[10],z1-transArr[11],y,z);
  y -= cellsArray[cell].getWirePos();
  rotateTo(transArr,x2-x1,y2-y1,z2-z1,dy,dz);
  
  Double_t lng   = 1./TMath::Sqrt(dz*dz+dy*dy);
  Double_t yDist = TMath::Abs(y*dz-z*dy); // abs(Ywire-Ycross_track)=yDist/dz
  minDist = yDist*lng;
  dz      = TMath::Abs(dz);
  dy      = TMath::Abs(dy);
  alpha   = calcAlphaImpact(dy/dz); //TMath::ATan2(dz,dy)*TMath::RadToDeg();
  if(minDist*dz*lng > halfPitch) {
    if(dy==0.0) return kFALSE;
    if((yDist-halfPitch*dz)/dy > halfCatDist)  return kFALSE;
  } else if(minDist*dy*lng > halfCatDist &&    // dy*lng = cos(alpha)
      (yDist-halfCatDist*dy)/dz > halfPitch) return kFALSE;
  return kTRUE;
}

Double_t HMdcSizesCellsLayer::getXSign(Double_t x1, Double_t y1,
    Double_t z1, Double_t x2, Double_t y2, Double_t z2, Int_t cell) const {
  // return X position of the signal on the wire in coor. sys. of this wire
  // (Ywire=0, Zwire=0), x1,y1,z1 & x2,y2,z2 - track in sector coor.system
  Double_t x,y,z;
  const Double_t *transArr = getRLSysRSec(cell);
  rotateTo(transArr,x1-transArr[9],y1-transArr[10],z1-transArr[11],x,y,z);
  y -= cellsArray[cell].getWirePos();
  Double_t dx,dy,dz;
  rotateTo(transArr,x2-x1,y2-y1,z2-z1,dx,dy,dz);
  return x-(z*dz+y*dy)/(dz*dz+dy*dy)*dx;
}

Float_t HMdcSizesCellsLayer::getSignPath(Double_t x1, Double_t y1, Double_t z1,
    Double_t x2, Double_t y2, Double_t z2, Int_t cell) const {
  // return the path of signal along wire number "cell"
  // x1,y1,z1 & x2,y2,z2 - track in sector coor.system
  // Path can be <0. or > wire_length !!!
  // For GEANT data and don't connected wires return 0.
  HMdcSizesCellsCell& fCell = cellsArray[cell];
  if(!&fCell || fCell.getReadoutSide()=='0') return 0.;
  Float_t x = getXSign(x1, y1,z1, x2, y2, z2,cell);
  return fCell.getSignPath(x);
}

Double_t HMdcSizesCellsLayer::getXSign(const HMdcLineParam& ln,Int_t cell) const {
  // return X position of the signal on the wire in coor. sys. of this wire
  // (Ywire=0, Zwire=0), "ln" - track in sector coor.system
  Double_t x,y,z,dx,dy,dz;
  Int_t parSec = ln.getSec();
  if(parSec==sector || parSec<0) {
    const Double_t *trans = getRLSysRSec(cell);
    rotateTo(trans,ln.x1()-trans[9],ln.y1()-trans[10],ln.z1()-trans[11],x,y,z);
    y -= cellsArray[cell].getWirePos();
    rotateTo(trans,ln.getDir(),dx,dy,dz);
  } else {
    HGeomTransform rotLSysROtherSec;
    getRotLSysForOtherSecSys(parSec,cell,rotLSysROtherSec);
    const HGeomRotation& rot = rotLSysROtherSec.getRotMatrix();
    const HGeomVector&   trn = rotLSysROtherSec.getTransVector();
    HMdcSizesCells::rotateXYZ(rot,ln.x1()-trn(0),ln.y1()-trn(1),ln.z1()-trn(2),x,y,z);
    y -= cellsArray[cell].getWirePos();
    HMdcSizesCells::rotateDir(rot,ln.getDir(),dx,dy,dz);
  }
  return x-(z*dz+y*dy)/(dz*dz+dy*dy)*dx;
}

Double_t HMdcSizesCellsLayer::getSignPath(const HMdcLineParam& ln,
    Int_t cell) const {
  // return the path of signal along wire number "cell"
  // "ln" - track in sector coor.system
  // Path can be <0. or > wire_length !!!
  // For GEANT data and don't connected wires return 0.
  HMdcSizesCellsCell& fCell = cellsArray[cell];
  if(!&fCell || fCell.getReadoutSide()=='0') return 0.;
  Double_t x = getXSign(ln,cell);
  return fCell.getSignPath(x);
}

Float_t HMdcSizesCellsLayer::getSignPath(Double_t x1, Double_t y1, Double_t z1,
    Double_t x2, Double_t y2, Double_t z2, Int_t cell, Float_t& outside) const {
  // return the path of signal along wire number "cell"
  // x1,y1,z1 & x2,y2,z2 - track in sector coor.system
  // For GEANT data and don't connected wires return 0.
  //
  // outside:
  // if(path<0) outside=-path;
  // else if(path>wire_length) outside=path-wire_length;
  // else outside=0;
  //
  // path-outside give new path which >=0. and <=wire_length)

  HMdcSizesCellsCell& fCell = cellsArray[cell];
  if(!&fCell || fCell.getReadoutSide()=='0') {
    outside = 0.;
    return 0.;
  }
  
  Float_t x    = getXSign(x1, y1,z1, x2, y2, z2,cell);
  Float_t path = fCell.getSignPath(x);
  if(path<0.) outside = path;
  else if(path>fCell.getWireLength()) outside = path-fCell.getWireLength();
  else outside=0.;
  return path;
}

Bool_t HMdcSizesCellsLayer::getDriftDist(Double_t xP1,Double_t yP1,Double_t zP1,
    Double_t xP2, Double_t yP2, Double_t zP2,
    Int_t cell, Double_t &alphaDrDist, Double_t &driftDist) const {
  //
  // return kTRUE if line p1-p2 intersect cell or kFALSE if not intersect
  // calc. the angle driftAlpha (in degree) and drift distance in cell
  // from line (p1-p2) to wire.
  // p1,p2 - in sector coor.sys.
  // 1)
  //    Z^
  //     |  |------------*----|
  //     |  | cell        *   |
  //     |  |            / *  |
  //     |  |           /90 * |
  //     |  | driftDist/     *|
  //     |  |         /       *
  //   --|--|--------*^-----/-|*--------->
  //     |  |              /  | *        Y
  //     |  |      alphaDrDist|  *
  //        |                 |   *
  //        |                 |    *
  //        |-----------------|     * track
  //
  // 2)             *** 90
  //    Z^             ***
  //     |  |----------:--***-|
  //     |  | cell     :  /  ***
  //     |  |         :  /    | ***  track
  //     |  |         : /     |    ***
  //     |  | driftDist/      |
  //     |  |        :/       |
  //   --|--|--------*^-------|---------->
  //     |  |         |       |          Y
  //     |  |                 |  alphaDrDist ???
  //        |                 |
  //        |                 |
  //        |-----------------|
  //
  //
  // 3)                        *
  //                            *
  //    Z^                     / * <-- no eg. 90 deg. !
  //     |  |-----------------/   *
  //     |  | cell           /|    *
  //     |  |               / |     *
  //     |  |              /  |      *
  //     |  |   driftDist /   |       *
  //     |  |            /    |        *track
  //     |  |           /     |         *
  //     |  |          /      |          *
  //     |  |         /       |      Alpha*
  //   --|--|--------*^-------|------------->
  //     |  |         |       |          Y
  //     |  |      alphaDrDist|
  //        |                 |
  //        |                 |
  //        |                 |
  //        |                 |
  //        |                 |
  //        |                 |
  //        |-----------------|

  Double_t y1,y2,z1,z2;
  const Double_t *trans = getRLSysRSec(cell);
  rotateTo(trans,xP1-trans[9],yP1-trans[10],zP1-trans[11],y1,z1);
  rotateTo(trans,xP2-trans[9],yP2-trans[10],zP2-trans[11],y2,z2);
  Double_t yWire = cellsArray[cell].getWirePos();
  y1 -= yWire;
  y2 -= yWire;
      
  Double_t dz       = TMath::Abs(z1-z2);
  Double_t dy       = TMath::Abs(y1-y2);
  Double_t length   = TMath::Sqrt(dz*dz+dy*dy);
  Double_t ctgAlpha = dy/dz;
  Double_t sinAlpha = dz/length;
  Double_t yDist    = TMath::Abs((z1*y2-z2*y1)/dz);
  driftDist   = yDist*sinAlpha;
  alphaDrDist = TMath::ATan2(dz,dy)*TMath::RadToDeg();
  if(driftDist*sinAlpha > halfPitch) {
    Double_t z = (dy != 0.0) ? ((yDist-halfPitch)/ctgAlpha) : 1.E+10;
    if(z <= halfCatDist)  {
      driftDist   = TMath::Sqrt(halfPitch*halfPitch+z*z);
      alphaDrDist = TMath::ATan2((Double_t)halfPitch,z)*TMath::RadToDeg();
    } else {
      Double_t tgADrDist=halfCatDist/halfPitch;
      alphaDrDist = 90.-TMath::ATan(tgADrDist)*TMath::RadToDeg();
      driftDist   = yDist/(TMath::Abs(ctgAlpha*tgADrDist)+1.) * TMath::Sqrt(1+tgADrDist*tgADrDist);
      return kFALSE;
    }
  } else if(driftDist*dy/length > halfCatDist) { // dy/length = cos(alpha)
    Double_t y = yDist-halfCatDist*ctgAlpha;
    if(y <= halfPitch) {
      driftDist   = TMath::Sqrt(y*y+halfCatDist*halfCatDist);
      alphaDrDist = TMath::ATan2(y,(Double_t)halfCatDist)*TMath::RadToDeg();
    } else {
      Double_t tgADrDist = halfCatDist/halfPitch;
      alphaDrDist = 90.-TMath::ATan(tgADrDist)*TMath::RadToDeg();
      driftDist   = yDist/(TMath::Abs(ctgAlpha*tgADrDist)+1.) * TMath::Sqrt(1+tgADrDist*tgADrDist);
      return kFALSE;
    }
  }
  return kTRUE;
}

void HMdcSizesCellsLayer::calcInOtherSecSys(const HMdcLineParam& ln,Int_t cell,
    Double_t &y,Double_t &z,Double_t &dy,Double_t &dz) const {
  // if "ln"  is in another sector coor.sys
  Int_t parSec = ln.getSec();
  HGeomTransform rotLSysROtherSec;
  getRotLSysForOtherSecSys(parSec,cell,rotLSysROtherSec);

  const HGeomRotation& rot = rotLSysROtherSec.getRotMatrix();
  const HGeomVector&   trn = rotLSysROtherSec.getTransVector();
  HMdcSizesCells::rotateYZ(rot,ln.x1()-trn(0),ln.y1()-trn(1),ln.z1()-trn(2),y,z);
  y -= cellsArray[cell].getWirePos();
  HMdcSizesCells::rotateDir(rot,ln.getDir(),dy,dz);
}

void HMdcSizesCellsLayer::getRotLSysForOtherSecSys(Int_t othSec,Int_t cell,
                                                   HGeomTransform& trans) const {
  trans = cell<firstCellPart2 ? rotLaySysRSec : rotLaySysRSecPart2;
  
  HMdcSizesCells*    pSCells     = HMdcSizesCells::getObject();
  HMdcSizesCellsSec& pSCellsSec0 = (*pSCells)[sector];
  const HGeomTransform* tr0 = pSCellsSec0.getLabTrans();
  trans.transFrom(*tr0);  // trans. to lab.
  
  HMdcSizesCellsSec&    pSCellsSec1 = (*pSCells)[othSec];
  const HGeomTransform* tr1         = pSCellsSec1.getLabTrans();
  trans.transTo(*tr1);   // trans. to another sector
}

void HMdcSizesCellsLayer::print(void) const {
  printf("HMSCLayer: %iS %iM %iL, %i cells, cat.dist=%5.2f, pitch=%f, ang.=%6.3f\n",
      sector+1,module+1,layer+1,nCells,halfCatDist*2,pitch,
      TMath::ASin(sinWireOr)*TMath::RadToDeg());
  if(firstCellPart2<nCells) printf("  Part2 from %i cell ang.=%6.3f\n",firstCellPart2+1,
      TMath::ASin(sinWireOrPart2)*TMath::RadToDeg());
  printf("\n");
}

HMdcSizesCellsMod::HMdcSizesCellsMod(HMdcSizesCellsSec* pSec, Int_t mod) {
  // constructor
  sector = pSec->getSector();
  module = mod;
  pToSec = pSec;
  for (Int_t layer=0; layer<6; layer++) mdcSCLayers[layer].initLayer(this,layer);
}

HMdcSizesCellsMod::~HMdcSizesCellsMod(void) {
  // destructor
}

Bool_t HMdcSizesCellsMod::setSecTrans(const HGeomTransform * alignTrans,
    Int_t sysOfAlignTrans) {
  if(!HMdcGetContainers::getObject()->getSecTransMod(sysRSec,sector,module))
    return kFALSE;
  if(alignTrans) {
    if(sysOfAlignTrans==0) {
      sysRSec.transTo(*alignTrans);
      HMdcSizesCells::getExObject()->setGeomModifiedFlag();
    } else if(sysOfAlignTrans==1) {
      HGeomTransform alSys=(*alignTrans);
      alSys.transFrom(sysRSec);
      sysRSec=alSys;
      HMdcSizesCells::getExObject()->setGeomModifiedFlag();
    } else if(sysOfAlignTrans==2) {
      sysRSec.transFrom(pToSec->getLabTransform());
      sysRSec.transTo(*alignTrans);
      sysRSec.transTo(pToSec->getLabTransform());
      HMdcSizesCells::getExObject()->setGeomModifiedFlag();
    }
  }
  HMdcSizesCells::copyTransfToArr(sysRSec,tSysRSec);
  tSysRSecOp[ 0] = tSysRSec[0];
  tSysRSecOp[ 1] = tSysRSec[3];
  tSysRSecOp[ 2] = tSysRSec[6];
  tSysRSecOp[ 3] = tSysRSec[1];
  tSysRSecOp[ 4] = tSysRSec[4];
  tSysRSecOp[ 5] = tSysRSec[7];
  tSysRSecOp[ 6] = tSysRSec[2];
  tSysRSecOp[ 7] = tSysRSec[5];
  tSysRSecOp[ 8] = tSysRSec[8];
  tSysRSecOp[ 9] = -tSysRSec[0]*tSysRSec[ 9]-tSysRSec[3]*tSysRSec[10]-tSysRSec[6]*tSysRSec[11];
  tSysRSecOp[10] = -tSysRSec[1]*tSysRSec[ 9]-tSysRSec[4]*tSysRSec[10]-tSysRSec[7]*tSysRSec[11];
  tSysRSecOp[11] = -tSysRSec[2]*tSysRSec[ 9]-tSysRSec[5]*tSysRSec[10]-tSysRSec[8]*tSysRSec[11];
  setPlanePar(sysRSec);
  ct[0] = (tSysRSec[0]-tSysRSec[6]*parA);
  ct[1] = (tSysRSec[3]-tSysRSec[6]*parB);
  ct[2] = tSysRSec[6]*(parD-parB*tSysRSec[10]-tSysRSec[11]-parA*tSysRSec[9]);
  ct[3] = (tSysRSec[1]-tSysRSec[7]*parA);
  ct[4] = (tSysRSec[4]-tSysRSec[7]*parB);
  ct[5] = tSysRSec[7]*(parD-parB*tSysRSec[10]-tSysRSec[11]-parA*tSysRSec[9]);
  return kTRUE;
}

void HMdcSizesCellsMod::calcMdcHit(Double_t x1, Double_t y1, Double_t z1,
      Double_t x2, Double_t y2, Double_t z2,
      Double_t eX1, Double_t eY1, Double_t eZ1, Double_t eX2, Double_t eY2,
      Double_t dZ1dX1, Double_t dZ1dY1, Double_t dZ2dX2, Double_t dZ2dY2,
      Double_t& x, Double_t& eX, Double_t& y, Double_t& eY,
      Double_t& xDir, Double_t& eXDir, Double_t& yDir, Double_t& eYDir) const {
  // Calcul. a cross of the line with plane MDC (parA*x+parB*y+c*z=parD),
  // transform. this point to mdc coor.sys. (z=0) and calc.
  // hit direction in mdc coor.sys. and errors
  //
  // Input param:
  // x1,y1,z1 - target or point on plane
  // x2,y2,z2 - point on plane
  // eX1,eY1,eZ1, eX2,eY2 - errors (eZ2=0!)
  // dZ2dX2 = -A2  where A2 - plane param. (z2=D2-A2*x2+B2*y2)
  // dZ2dY2 = -B2  where B2 - plane param. (z2=D2-A2*x2+B2*y2)
  // If P1-on plane
  //   eZ1=0;
  //   dZ1dX1 = -A1  where A1 - plane param. (z1=D1-A1*x1+B1*y1)
  //   dZ1dY1 = -B1  where B1 - plane param. (z1=D1-A1*x1+B1*y1)
  // If P1-target:
  //   dZ1dX1 = dZ1/dY1 = 0;
  //---Cross-----------------------------------------------
  Double_t dX   = x2-x1;
  Double_t dY   = y2-y1;
  Double_t dZ   = z2-z1;
  Double_t cxz1 = parD-z1-parA*x1;
  Double_t cxz2 = parD-z2-parA*x2;
  Double_t cyz1 = parD-z1-parB*y1;
  Double_t cyz2 = parD-z2-parB*y2;
  Double_t del  = 1/(parA*dX+parB*dY+dZ);
  Double_t xt0  = (x2*cyz1 - x1*cyz2)*del;
  Double_t yt0  = (y2*cxz1 - y1*cxz2)*del;
  Double_t xt   = xt0-tSysRSec[ 9];
  Double_t yt   = yt0-tSysRSec[10];
  x = ct[0]*xt+ct[1]*yt+ct[2];
  y = ct[3]*xt+ct[4]*yt+ct[5];
  // - Errors - - - - - - - - - - - - - - - - -
  Double_t x1t    = x1-xt0;
  Double_t y1t    = y1-yt0;
  Double_t xt2    = xt0-x2;
  Double_t yt2    = yt0-y2;
  Double_t xtA    = xt0*parA;
  Double_t ytB    = yt0*parB;
  Double_t dXtdX2 = x1t*dZ2dX2+cyz1-xtA;
  Double_t dYtdY2 = y1t*dZ2dY2+cxz1-ytB;
  Double_t dXtdX1 = xt2*dZ1dX1-cyz2+xtA;
  Double_t dYtdY1 = yt2*dZ1dY1-cxz2+ytB;
  Double_t dXtdY2 = x1t*(dZ2dY2+parB);
  Double_t dYtdX2 = y1t*(dZ2dX2+parA);
  Double_t dXtdY1 = xt2*(dZ1dY1+parB);
  Double_t dYtdX1 = yt2*(dZ1dX1+parA);
  Double_t dXtdZ1 = xt2;
  Double_t dYtdZ1 = yt2;
  Double_t dXdX1  = (ct[0]*dXtdX1+ct[1]*dYtdX1)*eX1;
  Double_t dYdX1  = (ct[3]*dXtdX1+ct[4]*dYtdX1)*eX1;
  Double_t dXdY1  = (ct[0]*dXtdY1+ct[1]*dYtdY1)*eY1;
  Double_t dYdY1  = (ct[3]*dXtdY1+ct[4]*dYtdY1)*eY1;
  Double_t dXdX2  = (ct[0]*dXtdX2+ct[1]*dYtdX2)*eX2;
  Double_t dYdX2  = (ct[3]*dXtdX2+ct[4]*dYtdX2)*eX2;
  Double_t dXdY2  = (ct[0]*dXtdY2+ct[1]*dYtdY2)*eY2;
  Double_t dYdY2  = (ct[3]*dXtdY2+ct[4]*dYtdY2)*eY2;
  Double_t dXdZ1  = (ct[0]*dXtdZ1+ct[1]*dYtdZ1)*eZ1;
  Double_t dYdZ1  = (ct[3]*dXtdZ1+ct[4]*dYtdZ1)*eZ1;
  eX = TMath::Sqrt(dXdX1*dXdX1+dXdY1*dXdY1+dXdZ1*dXdZ1+dXdX2*dXdX2+dXdY2*dXdY2)*del;
  eY = TMath::Sqrt(dYdX1*dYdX1+dYdY1*dYdY1+dYdZ1*dYdZ1+dYdX2*dYdX2+dYdY2*dYdY2)*del;
  //---Hit direction----------------------------------------------------
  Double_t length = 1/TMath::Sqrt(dX*dX+dY*dY+dZ*dZ);
  dX  *= length; // unit vector
  dY  *= length;
  dZ  *= length;
  xDir = tSysRSec[0]*dX+tSysRSec[3]*dY+tSysRSec[6]*dZ;
  yDir = tSysRSec[1]*dX+tSysRSec[4]*dY+tSysRSec[7]*dZ;
  // - Errors - - - - - - - - - - - - - - - - -
  Double_t c0xx   = (tSysRSec[0]-xDir*dX);
  Double_t c3xy   = (tSysRSec[3]-xDir*dY);
  Double_t c6xz   = (tSysRSec[6]-xDir*dZ);
  Double_t c1yx   = (tSysRSec[1]-yDir*dX);
  Double_t c4yy   = (tSysRSec[4]-yDir*dY);
  Double_t c7yz   = (tSysRSec[7]-yDir*dZ);
  Double_t dXDdX2 = (c0xx+c6xz*dZ2dX2)*eX2;
  Double_t dXDdY2 = (c3xy+c6xz*dZ2dY2)*eY2;
  Double_t dXDdX1 = (c0xx+c6xz*dZ1dX1)*eX1; // *(-1)
  Double_t dXDdY1 = (c3xy+c6xz*dZ1dY1)*eY1; // *(-1)
  Double_t dXDdZ1 =       c6xz        *eZ1; // *(-1)
  Double_t dYDdX2 = (c1yx+c7yz*dZ2dX2)*eX2;
  Double_t dYDdY2 = (c4yy+c7yz*dZ2dY2)*eY2;
  Double_t dYDdX1 = (c1yx+c7yz*dZ1dX1)*eX1; // *(-1)
  Double_t dYDdY1 = (c4yy+c7yz*dZ1dY1)*eY1; // *(-1)
  Double_t dYDdZ1 =       c7yz        *eZ1; // *(-1)
  eXDir = TMath::Sqrt(dXDdX2*dXDdX2+dXDdY2*dXDdY2+
                      dXDdX1*dXDdX1+dXDdY1*dXDdY1+dXDdZ1*dXDdZ1)*length;
  eYDir = TMath::Sqrt(dYDdX2*dYDdX2+dYDdY2*dYDdY2+
                      dYDdX1*dYDdX1+dYDdY1*dYDdY1+dYDdZ1*dYDdZ1)*length;
}

HMdcSizesCellsSec::HMdcSizesCellsSec(HMdcSizesCells* pSC, Int_t sec) {
  // constructor creates an array of pointers of type
  // HMdcSizesCellsMod
  sector     = sec;
  array      = new TObjArray(4);
  mdcStatSec = pSC->modStatusArr(sector);
  pToSC      = pSC;
  targets    = 0;
  for (Int_t mod=0; mod<4; mod++) if(mdcStatSec[mod])
      (*array)[mod] = new HMdcSizesCellsMod(this,mod);
}

HMdcSizesCellsSec::~HMdcSizesCellsSec(void) {
  // destructor
  if(array) {
    array->Delete();
    delete array;
    array = 0;
  }
  if(targets) delete [] targets;
  targets=0;
}

Int_t HMdcSizesCellsSec::getSize(void) const {
  // return the size of the pointer array
  return array->GetEntries();
}

HMdcSizesCells* HMdcSizesCells::fMdcSizesCells=0;

HMdcSizesCells::HMdcSizesCells(void) {
  // constructor creates an array of pointers of type HMdcSizesCellsSec
  fGetCont       = HMdcGetContainers::getObject();
  fLayerGeomPar  = fGetCont->getMdcLayerGeomPar();
  fGeomPar       = fGetCont->getMdcGeomPar();
  fSpecGeomPar   = fGetCont->getSpecGeomPar();
  fMdcGeomStruct = fGetCont->getMdcGeomStruct();
  fMdcRawStruct  = fGetCont->getMdcRawStruct();
  fLayerCorrPar  = fGetCont->getMdcLayerCorrPar();
  if (!fMdcRawStruct->isStatic() || !HMdcGetContainers::isInited(fMdcRawStruct))
      ((HParSet*)fMdcRawStruct)->init(); //Needs explicit initialization 
  fMdcLookupRaw = (HMdcLookupRaw*)gHades->getRuntimeDb()->getContainer("MdcLookupRaw");
  changed      = kFALSE;
  geomModified = kFALSE;
  array        = new TObjArray(6);
  for(Int_t i=0;i<3;i++) verLayerGeomPar[i]=verGeomPar[i]=verLookupRaw[i]=-111111;
  numTargets   = 0;
  targets      = NULL;
  tarHalfThick = NULL;
  return;
}

HMdcSizesCells::~HMdcSizesCells(void) {
  // destructor
  if(array) {
    array->Delete();
    delete array;
    array = NULL;
  }
  HMdcGetContainers::deleteCont();
  if(targets) delete [] targets;
  if(tarHalfThick) delete [] tarHalfThick;
  targets      = NULL;
  tarHalfThick = NULL;
}

Bool_t HMdcSizesCells::initContainer(void) {
  if( !fMdcGeomStruct || !HMdcGetContainers::isInited(fLayerGeomPar)
      || !HMdcGetContainers::isInited(fGeomPar)
      || !HMdcGetContainers::isInited(fMdcLookupRaw)
      || !HMdcGetContainers::isInited(fMdcRawStruct) 
      || !HMdcGetContainers::isInited(fLayerCorrPar) ) return kFALSE;
  if( array==NULL || fLayerGeomPar->hasChanged() || fGeomPar->hasChanged() ||
      fMdcLookupRaw->hasChanged() || fLayerCorrPar->hasChanged()) {
    for(Int_t i=1; i<3; i++) {
      if( array!=NULL &&
         fLayerGeomPar->getInputVersion(i) == verLayerGeomPar[i] &&
         fGeomPar->     getInputVersion(i) == verGeomPar[i] &&
         fMdcLookupRaw->getInputVersion(i) == verLookupRaw[i]) continue;
      for(Int_t j=i; j<3; j++) {
        verLayerGeomPar[j] = fLayerGeomPar->getInputVersion(j);
        verGeomPar[j]      = fGeomPar->getInputVersion(j);
        verLookupRaw[j]    = fMdcLookupRaw->getInputVersion(j);
      }
      changed=kTRUE;
      for (Int_t sec = 0; sec < 6; sec++) {
        nModsSeg[sec][0]=nModsSeg[sec][1]=0;
        for(Int_t mod=0;mod<4;mod++) {
          mdcStatus[sec][mod] = fGetCont->isModActive(sec,mod);
          if(mdcStatus[sec][mod]) nModsSeg[sec][mod/2]++;
        }
        if( !fGetCont->isSecActive(sec) )  continue;
        if( !(*array)[sec] ) (*array)[sec] = new HMdcSizesCellsSec(this,sec);
        if( !fillCont(sec) )  return kFALSE;
      }
      fillTargetPos();
      break;
    }
  } else changed=kFALSE;
  if(geomModified) changed=kTRUE;
  return kTRUE;
}

HMdcSizesCells* HMdcSizesCells::getObject(void) {
  if(fMdcSizesCells == NULL) fMdcSizesCells = new HMdcSizesCells();
  return fMdcSizesCells;
}

void HMdcSizesCells::deleteCont(void) {
  if(fMdcSizesCells) {
    delete fMdcSizesCells;
    fMdcSizesCells=0;
  }
}

Int_t HMdcSizesCells::getSize(void) const {
  // return the size of the pointer array
  return array->GetEntries();
}

void HMdcSizesCells::clear(void) {
  // clears the container
  for(Int_t s=0;s<6;s++) {
    HMdcSizesCellsSec* sec=&(*this)[s];
    if( sec == NULL ) continue;
    for(Int_t m=0;m<4;m++) {
      HMdcSizesCellsMod* mod=&(*sec)[m];
      if( mod == NULL ) continue;
      for(Int_t l=0;l<6;l++) {
        HMdcSizesCellsLayer& mdcSCLay = (*mod)[l];
	Int_t nc = mdcSCLay.getNCells();
	for(Int_t c=0;c<nc;c++) mdcSCLay[c].clear();
      }
    }
  }
  changed=kFALSE;
}

Bool_t HMdcSizesCells::fillCont(Int_t sec) {
  // calculation of the container HMdcSizesCells
  // with wire's sizes (2-pint) in coordinate system of sector.
  HMdcSizesCellsSec& fSCSec = (*this)[sec];
  if(!fGetCont->getLabTransSec(fSCSec.getLabTransform(),sec)) return kFALSE;
  for(Int_t mod=0; mod<4; mod++) if(!fillModCont(sec,mod)) return kFALSE;
  return kTRUE;
}

Bool_t HMdcSizesCells::fillModCont(Int_t sec, Int_t mod, 
    HGeomTransform * alignTrans, Int_t sysOfAlignTrans) { 
  // filling HMdcSizesCellsMod, HMdcSizesCellsLayer, HMdcSizesCellsCell
  // objects for MDC module "mod" in sector "sec".
  // if alignTrans>0 - correction of mdc position (must be done after
  // filling HMdcSizesCells!)
  // sysOfAlignTrans =0 transform mdc coor.sys. by "alignTrans" respect 
  //                    coor.sys.of sector
  //                 =1 transform mdc coor.sys. by "alignTrans" respect 
  //                    coor.sys.of mdc module
  //                 =2 transform mdc coor.sys. by "alignTrans" respect 
  //                    lab.coor.sys.
  if(!secOk(sec) || !modOk(mod)) return kFALSE; 
  HMdcSizesCellsSec& fSizesCellsSec=(*this)[sec];
  if(&fSizesCellsSec==0) return kFALSE;
  if( !mdcStatus[sec][mod] ) return kTRUE;
  HMdcSizesCellsMod& fSCellsMod = fSizesCellsSec[mod];
  if(!fSCellsMod.setSecTrans(alignTrans,sysOfAlignTrans)) return kFALSE;
  for(Int_t layer=0; layer<6; layer++)
      if(!fSCellsMod[layer].fillLayerCont(fLayerCorrPar)) return kFALSE;
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::getParamCont(void) {
  HMdcGetContainers* pGetCont=HMdcGetContainers::getObject();
  HGeomCompositeVolume* pCV =pGetCont->getGeomCompositeVolume(module);
  if(!pCV) {
    Error("getParamCont","GeomCompositeVolume for MDC%i in sec.%i is absent.",
        module+1,sector+1);
    return kFALSE;
  }
  pGeomVol=pCV->getComponent(layer);
  if(pGeomVol==0) {
    Error("getParamCont",
        "HGeomVolume for lay. %i in sec.%i mod.%i doesn't exist!",layer+1,
        sector+1,module+1);
    return kFALSE;
  }
  pLayerGeomParLay=&(*(pGetCont->getMdcLayerGeomPar()))[sector][module][layer];
  if(pLayerGeomParLay==0) {
    Error("getParamCont",
        "HLayerGeomParLay for lay. %i in sec.%i mod.%i doesn't exist!",layer+1,
        sector+1,module+1);
    return kFALSE;
  }
  return kTRUE;
}

Bool_t HMdcSizesCells::fillModCont(Int_t sec, Int_t mod, Double_t * corr) { 
  // filling HMdcSizesCellsMod, HMdcSizesCellsLayer, HMdcSizesCellsCell
  // objects for MDC module "mod" in sector "sec"
  if(!secOk(sec) || !modOk(mod)) return kFALSE;
  HMdcSizesCellsSec& fSizesCellsSec=(*this)[sec];
  if(&fSizesCellsSec==0) return kFALSE;
  if( !mdcStatus[sec][mod] ) return kTRUE;
  HMdcSizesCellsMod& fSCellsMod=fSizesCellsSec[mod];
  if(!fSCellsMod.setSecTrans()) return kFALSE;
  for(Int_t layer=0; layer<6; layer++) 
      if(!fSCellsMod[layer].fillLayerCont(fLayerCorrPar,corr)) return kFALSE;
  return kTRUE;
}

Bool_t HMdcSizesCellsLayer::fillLayerCont(const HMdcLayerCorrPar* fLayCorrPar,
                                          const Double_t* corr) {
  if(!getParamCont()) return kFALSE;
  if(nCells<0) return kTRUE;
  pitch           = pLayerGeomParLay->getPitch();
  invPitch        = 1./pitch;
  halfPitch       = pitch*0.5;
  halfCatDist     = pLayerGeomParLay->getCatDist()*0.5;
  halfCDsDivPitch = halfCatDist*invPitch;
  maxDriftDist    = TMath::Sqrt(halfPitch*halfPitch + halfCatDist*halfCatDist);
  cellOffset      = pLayerGeomParLay->getCentralWireNr()-0.5;
  wireOffset      = (pLayerGeomParLay->getCentralWireNr()-1.)*pitch;
  wireOrient      = pLayerGeomParLay->getWireOrient();
  // Layer geometry correction:
  if(corr) {
    wireOrient += corr[layer];
    wireOffset += corr[layer + 6];
    cellOffset += corr[layer + 6]/pitch;
  }
  cosWireOr = TMath::Cos(wireOrient*TMath::DegToRad());
  sinWireOr = TMath::Sin(wireOrient*TMath::DegToRad());

  if(fLayCorrPar) {   // Must be after "if(corr)..."
    Float_t shift,orCorr;
    fLayCorrPar->getLayerCorrPar(sector,module,layer,firstCellPart2,shift,orCorr);
    cellOffsetPart2   = cellOffset - shift/pitch;
    wireOffsetPart2   = wireOffset - shift;
    wireOrientPart2   = wireOrient - orCorr;
    cosWireOrPart2    = TMath::Cos(wireOrientPart2*TMath::DegToRad());
    sinWireOrPart2    = TMath::Sin(wireOrientPart2*TMath::DegToRad());
  }

  if(!setSecTrans(corr ? corr[layer+12]:0.)) return kFALSE; // Must be abter if(fLayCorrPar) {...}!
  
  Int_t nPoint=pGeomVol->getNumPoints();
  if(nPoint != 8) {
    Error("fillCont","NumPoints for layer volume not eg. 8.");
    return kFALSE;
  }
  sensWiresThick = pGeomVol->getPoint(4)->getZ() - 
                   pGeomVol->getPoint(0)->getZ();
  if(sensWiresThick < 0.02) {
    Error("fillCont","Geometry of layer looks not reasonable");
    sensWiresThick = 0.;
    zGeantHit      = 0.;
    return kFALSE;
  }
  zGeantHit      = -sensWiresThick/2.;
  Double_t x[4],y[4];
  for(nPoint=0; nPoint<4;nPoint++) {
    const HGeomVector* pnt = pGeomVol->getPoint(nPoint);
    x[nPoint] = pnt->getX();
    y[nPoint] = pnt->getY();
  }

  // ----- calcBLines ----------------
  Double_t at[4],bt[4];
  for(Int_t i1=0; i1<4; i1++) {
    Int_t i2=i1+1;
    if(i2==4) i2=0;
    at[i1] = (y[i2]-y[i1])/(x[i2]-x[i1]);
    bt[i1] = y[i1]-at[i1]*x[i1];
  }
  //----------------------------------

  for(Int_t cell=0; cell<nCells; cell++) {
//---  Double_t a = TMath::Tan(wireOr*TMath::DegToRad());   // y=a*x+ln[1]
    Double_t a = getTanWireOr(cell);
    HMdcSizesCellsCell& fSizesCellsCell = cellsArray[cell];
    Double_t yWire = calcWireY(cell);
    Double_t b     = yWire/getCosWireOr(cell);
    fSizesCellsCell.setWirePos(yWire);
    HGeomVector& lineP1 = fSizesCellsCell.getWirePnt1();
    HGeomVector& lineP2 = fSizesCellsCell.getWirePnt2();
    //---------- Calculation of wire size ------------------
    //
    //
    //            Line 1
    //          ----------                 ^ (y)
    //          \ Layre  /                 |
    // Line 0 -> \ MDC  /  <- Line 2       |
    //            \____/                   |
    //            Line 3                   |
    //  (x) <--------|----------------------
    //              0,0
    //
    Double_t x1  = (bt[0]-b)/(a-at[0]);       //Xb    line 0
    Double_t x2  = (bt[2]-b)/(a-at[2]);       //Xe    line 2
    Double_t y1  = a*x1+b;                    // Yb
    Double_t y2  = a*x2+b;                    // Ye
    Int_t nLine1 = (x1>=x[0] && x1<=x[1] && y1>=y[0] && y1<=y[1]) ? 0:-1;
    Int_t nLine2 = (x2<=x[3] && x2>=x[2] && y2>=y[3] && y2<=y[2]) ? 2:-1;
    if(nLine1<0 && nLine2 <0) {
      if(cell<100) {
        Warning("fillCont","S%i M%i L%i: Cell %i is out of MDC.",
        sector+1,module+1,layer+1,cell+1);
        continue;
      } else {
        Warning("fillCont","S%i M%i L%i: Cells %i-%i are out of MDC.",
        sector+1,module+1,layer+1,cell+1,nCells);
        break;
      }
    }
    if(nLine1<0) {
      x1 = (bt[1]-b)/(a-at[1]);               // Xb    line 1
      if(x1<x[2]) x1 = (bt[3]-b)/(a-at[3]);   // Xb    line 3
      y1 = a*x1+b;                            // Yb
    } else if(nLine2<0) {
      x2 = (bt[1]-b)/(a-at[1]);               // Xe    line 1
      if(x2>x[1]) x2 = (bt[3]-b)/(a-at[3]);   // Xe    line 3
      y2 = a*x2+b;                            // Ye
    }
    
    HMdcSizesCells* pSC=HMdcSizesCells::getExObject();
    Char_t readOutSide=pSC->findReadOutSide(sector,module,layer,cell);
    if( readOutSide=='0'){
      if( cell>3 && cell < pSC->nCells(sector,module,layer)-4) {
        if(corr==0) Warning("fillCont",
          " %iS%iM%iL: Wire %i is not connected.",sector+1,module+1,layer+1,cell+1);
      }
    } else {
      if( (readOutSide=='L' && x1<x2) || (readOutSide=='R' && x1>x2) ||
          (readOutSide=='U' && y1<y2) ) {
        Double_t tmp;
        tmp=x1; x1=x2; x2=tmp;
        tmp=y1; y1=y2; y2=tmp;
      }
      if(readOutSide=='U') readOutSide=(x1>x2) ? 'L':'R';
    }
    fSizesCellsCell.setReadoutSide(readOutSide);
    fSizesCellsCell.setXReadout(getXinRotLay(cell,x1,y1));
//???    fSizesCellsCell.setWireLength(getXinRotLay(cell,x2-x1,y2-y1));
    fSizesCellsCell.setWireLength(TMath::Sqrt((x2-x1)*(x2-x1)+(y2-y1)*(y2-y1)));
    lineP1.setXYZ(x1,y1,0.);
    lineP2.setXYZ(x2,y2,0.);
    lineP1 = sysRSec.transFrom(lineP1);
    lineP2 = sysRSec.transFrom(lineP2);
    fSizesCellsCell.setStatus(kTRUE);
  }
  return kTRUE;
}

Double_t HMdcSizesCellsLayer::getXinRotLay(Int_t c, Double_t xi, Double_t yi)
    const {
  // Input:  xi,yi - layer coor.sys.
  // Return X in  rotated layer coor.sys.
  if(c < firstCellPart2) return xi*cosWireOr      + yi*sinWireOr;
  else                   return xi*cosWireOrPart2 + yi*sinWireOrPart2;
}

Double_t HMdcSizesCellsLayer::getYinRotLay(Int_t c, Double_t xi, Double_t yi) const {
  // Input:  xi,yi - layer coor.sys.
  // Return Y in  rotated layer coor.sys.
  if(c < firstCellPart2) return yi*cosWireOr      - xi*sinWireOr;
  else                   return yi*cosWireOrPart2 - xi*sinWireOrPart2;
}

Double_t HMdcSizesCellsLayer::getCosWireOr(Int_t c) const {
  return c<firstCellPart2 ? cosWireOr : cosWireOrPart2;
}

Double_t HMdcSizesCellsLayer::getSinWireOr(Int_t c) const {
  return c<firstCellPart2 ? sinWireOr : sinWireOrPart2;
}

Double_t HMdcSizesCellsLayer::getTanWireOr(Int_t c) const {
  return c<firstCellPart2 ? sinWireOr/cosWireOr : sinWireOrPart2/cosWireOrPart2;
}

Double_t HMdcSizesCellsLayer::getWireOffset(Int_t c) const  {
  return c<firstCellPart2 ? wireOffset : wireOffsetPart2;
}

Bool_t HMdcSizesCells::getCellVol(Int_t sec,Int_t mod,Int_t lay,Int_t cell,
    HMdcTrap& volCell, Double_t sizeFactor) const {
  // Return the cell volume as HMdcTrap (8 points) in sector(!) coordinate sys..
  // Use "sizeFactor" param. for drawing purpose only. 
  // This parameter increase Y and Z cell size on this factor.
  HGeomVector layerVol[8];  // Drift gas vol. (8-points)
  if( &((*this)[sec]) == 0 ) return kFALSE;
  HMdcSizesCellsMod& fSCellsMod=(*this)[sec][mod];
  if(!&fSCellsMod) return kFALSE;
  HGeomCompositeVolume* fComVol=fGetCont->getGeomCompositeVolume(mod);
  if(!fComVol) return kFALSE;
  HMdcSizesCellsLayer& fSCellsLay = fSCellsMod[lay];
  if(cell<0 || cell>=fSCellsLay.getNCells() || !fSCellsLay[cell].getStatus())
    return kFALSE;
  HGeomVolume* fGeomVol=fComVol->getComponent(lay);
  for(Int_t point=0; point<4; point++) {
    layerVol[point  ] = *(fGeomVol->getPoint(point));
    layerVol[point+4] = layerVol[point];
    layerVol[point  ].setZ(-fSCellsLay.getHalfCatDist());
    layerVol[point+4].setZ(+fSCellsLay.getHalfCatDist());
  }
  Double_t a = fSCellsLay.getTanWireOr(cell); // y=a*x+ln[1]
  // ----- calcBLines ----------------
  Double_t at[4],bt[4];
  for(Int_t i1=0; i1<4; i1++) {
    Int_t i2=i1+1;
    if(i2==4) i2=0;
    at[i1] = (layerVol[i2].getY()-layerVol[i1].getY())/
             (layerVol[i2].getX()-layerVol[i1].getX());
    bt[i1] = layerVol[i1].getY()-at[i1]*layerVol[i1].getX();
  }
  Double_t cosWrOr = fSCellsLay.getCosWireOr(cell);
  Double_t sinWrOr = fSCellsLay.getSinWireOr(cell);
  Double_t ymax = -1.e+30;
  Double_t ymin =  1.e+30;
  for(Int_t i=0; i<4; i++) {
    Double_t y = layerVol[i].getY()*cosWrOr - layerVol[i].getX()*sinWrOr;
    if(y>ymax) ymax = y;
    if(y<ymin) ymin = y;
  }
  Double_t b = fSCellsLay[cell].getWirePos()/cosWrOr;
  //---------- Calculation of wire size ------------------
  //
  //
  //            Line 1
  //          ----------                 ^ (y)
  //          \ Layre  /                 |
  // Line 0 -> \ MDC  /  <- Line 2       |
  //            \____/                   |
  //            Line 3                   |
  //  (x) <--------|----------------------
  //              0,0
  //
  Double_t x1     = (bt[0]-b)/(a-at[0]);       // Xb    line 0
  Double_t x2     = (bt[2]-b)/(a-at[2]);       // Xe    line 2
  Double_t y1     = a*x1+b;                    // Yb    line 0
  Double_t y2     = a*x2+b;                    // Ye    line 2
  Int_t    nLine1 = (x1>=layerVol[0].getX() && x1<=layerVol[1].getX() &&
                     y1>=layerVol[0].getY() && y1<=layerVol[1].getY()) ? 0:-1;
  Int_t    nLine2 = (x2<=layerVol[3].getX() && x2>=layerVol[2].getX() &&
                     y2>=layerVol[3].getY() && y2<=layerVol[2].getY()) ? 2:-1;
  if(nLine1<0 && nLine2 <0) return kFALSE;
  if(nLine1<0)       nLine1 = ((bt[1]-b)/(a-at[1]) >= layerVol[2].getX()) ? 1:3;
  else if(nLine2<0)  nLine2 = ((bt[1]-b)/(a-at[1]) <= layerVol[1].getX()) ? 1:3;

  Double_t halfPitch = fSCellsLay.getHalfPitch();
  if(sizeFactor>0.) halfPitch *= sizeFactor;
  Double_t bLine = fSCellsLay[cell].getWirePos() - halfPitch;
  Double_t tLine = fSCellsLay[cell].getWirePos() + halfPitch;
  if(tLine > ymax) tLine = ymax;
  if(bLine < ymin) bLine = ymin;
  bLine = bLine/cosWrOr;
  tLine = tLine/cosWrOr;

  Double_t z=layerVol[0](2);
  if(sizeFactor>0.) z*=sizeFactor;
  Double_t x=(bt[nLine1]-bLine)/(a-at[nLine1]);
  volCell[0].setXYZ(x,a*x+bLine,z);  
  x = (bt[nLine1]-tLine)/(a-at[nLine1]);
  volCell[1].setXYZ(x,a*x+tLine,z);
  x = (bt[nLine2]-tLine)/(a-at[nLine2]);
  volCell[2].setXYZ(x,a*x+tLine,z);
  x = (bt[nLine2]-bLine)/(a-at[nLine2]);
  volCell[3].setXYZ(x,a*x+bLine,z);
  z = layerVol[4](2);
  
  HGeomVector dif;
  volCell.setDbPointNum(-1);
  for(Int_t i=0;i<4;i++) {
    Int_t i2 = i<3 ? i+1 : 0;
    dif = volCell[i]-volCell[i2];
    if(dif.length()<0.01) {
      volCell[i2]   = volCell[i];
      volCell[i2+4] = volCell[i+4];
      volCell.setDbPointNum(i);
      break;               // not more then one double point
    }
  }
  
  if(sizeFactor>0.) z *= sizeFactor;
  for(Int_t i=0; i<4; i++) {
    volCell[i+4] = volCell[i];
    volCell[i+4].setZ(z);
  }
  volCell.transFrom(*(fSCellsLay.getSecTrans()));
  return kTRUE;
}

Char_t HMdcSizesCells::findReadOutSide(Int_t s,Int_t m,Int_t l,Int_t c) const {
  // Function to get the side from which the corresponding wire is read out.
  // Function return values are 'L' for left, 'R' for right, 'U' for top 
  // and '0' for GEANT data and not connected wires.
  if(!fMdcLookupRaw || !fMdcRawStruct) return '0';
  HMdcLookupCell &cell = (*fMdcLookupRaw)[s][m][l][c];
  if(!&cell) return '0';
  
  Char_t rdOutSide = cell.getReadoutSide();
  if(rdOutSide=='L' || rdOutSide=='R' || rdOutSide=='U') return rdOutSide;
  
  // Next code is important for reading parameters from root file which 
  // was created before adding ReadoutSide in HMdcLookupCell!:
  Int_t mbo = cell.getNMoth();
  if(mbo < 0) return '0';  // don't connected wires have mbo==-1
  HMdcRawMothStru& rMbo=(*fMdcRawStruct)[s][m][mbo];
  if(!&rMbo) return '0';
  Char_t mbn = rMbo.GetName()[0];
  if     (mbn == '1') return 'L';
  else if(mbn == '2') return 'R';
  else if(mbn == '3') return 'U';
  else                return '0';  
}

Bool_t HMdcSizesCells::fillTargetPos(HGeomVector* targetShift) {
  // Filling target parameters
  if(targets && numTargets<fSpecGeomPar->getNumTargets()) {
    delete [] targets;
    delete [] tarHalfThick;
    targets = 0;
  }
  numTargets = fSpecGeomPar->getNumTargets();
  if( numTargets < 1 ) {
    Error("calcTarget","Number of targets = %i!",numTargets);
    return kFALSE;
  }
  if(targets==0) {
    targets      = new HGeomVector [numTargets];
    tarHalfThick = new Double_t    [numTargets];
  }
  for(Int_t t=0;t<numTargets;t++) {
    HGeomVolume* tr = fSpecGeomPar->getTarget(t);
    if(!tr) {
      Error("calcTarget","GeomVolume for target absent!");
      return kFALSE;
    }
    targets[t]      = tr->getTransform().getTransVector();
    Double_t zV1    = tr->getPoint(0)->getZ();
    Double_t zV2    = tr->getPoint(2)->getZ();
    tarHalfThick[t] = TMath::Abs(zV1-zV2)/2.;
    targets[t].setZ(targets[t].getZ()+(zV1+zV2)/2.);
    // Targets sorting:
    Int_t t1 = t;
    for(Int_t t2=t1-1;t2>=0;t2--) {
      if(targets[t1].getZ()>targets[t2].getZ()) break;
      HGeomVector tmpVec(targets[t2]);
      targets[t2] = targets[t1];
      targets[t1] = tmpVec;
      Double_t tmpTarTh = tarHalfThick[t2];
      tarHalfThick[t2]  = tarHalfThick[t1];
      tarHalfThick[t1]  = tmpTarTh;
      t1 = t2;
    }
  }
  targ3p[0]  = targets[0];
  targ3p[2]  = targets[numTargets-1];
  targ3p[0].setZ(targ3p[0].getZ() - tarHalfThick[0]);
  targ3p[2].setZ(targ3p[2].getZ() + tarHalfThick[numTargets-1]);
  targ3p[1]  = targ3p[0]+targ3p[2];
  targ3p[1] *= 0.5;
 
  if(targetShift) {
    for(Int_t i=0;i<3;i++) targ3p[i] += *targetShift;
    for(Int_t i=0;i<numTargets;i++) targets[i] += *targetShift;
  }
    
  for(Int_t sec=0;sec<6;sec++) {
    HMdcSizesCellsSec* fSec=(HMdcSizesCellsSec*)((*array)[sec]);
    if(fSec == 0) continue;
    fSec->setNumOfTargets(targ3p,numTargets,targets);
  }
  return kTRUE;
}

Int_t HMdcSizesCells::calcTargNum(Double_t z,Double_t* dz) const {
  // Calculate the target numer which is 
  // Return the nearest to z target numer.
  // dz!=0 :
  //           *dz will eq. 0. if z inside target 
  // otherwise *dz will eq. Abs(Ztarg-z)
  Int_t    nt   = 0;
  Double_t dZnt = TMath::Abs(z-targets[0].getZ());
  if(dZnt <= tarHalfThick[0]+0.01) dZnt = 0;
  else {
    for(Int_t n=1;n<numTargets;n++) {
      Double_t dZnext = TMath::Abs(z-targets[n].getZ());
      if(dZnext<dZnt) {
        nt = n;
        dZnt = dZnext;
        if(dZnt > tarHalfThick[n]+0.01) continue;
        dZnt = 0.;
        break;
      }
    }
  }
  if(dz) *dz = dZnt;
  return nt;
}

void HMdcSizesCells::calcMdcSeg(Double_t x1, Double_t y1, Double_t z1,
                                Double_t x2, Double_t y2, Double_t z2,
    Double_t eX1, Double_t eY1, Double_t eZ1, Double_t eX2, Double_t eY2,
    Double_t dZ1dX1, Double_t dZ1dY1, Double_t dZ2dX2, Double_t dZ2dY2,
    Double_t& zm, Double_t& eZm, Double_t& r0,  Double_t& eR0,
    Double_t& theta, Double_t& eTheta, Double_t& phi, Double_t& ePhi) {
  // Calcul. a HMdcSeg hit.
  // Input and output are in sector coor.sys.
  // theta=atan(TMath::Sqrt(dX*dX+dY*dY)/dZ);  phi=atan(dY/dX)
  // zm= z1 - cos(theta)/sin(theta) * (x1*cos(phi)+y1*sin(phi))
  // r0=y1*cos(phi)-x1*sin(phi)
  //
  // If (x1,y1,z1)=(x2,y2,z2) dZ will eq.1.!
  // Input param:
  // x1,y1,z1 - target or point on plane
  // x2,y2,z2 - point on plane
  // eX1,eY1,eZ1, eX2,eY2 - errors (eZ2=0!)
  // dZ2dX2 = -A2  where A2 - plane param. (z2=D2-A2*x2+B2*y2)
  // dZ2dY2 = -B2  where B2 - plane param. (z2=D2-A2*x2+B2*y2)
  // If P1-on plane
  //   eZ1=0;
  //   dZ1dX1 = -A1  where A1 - plane param. (z1=D1-A1*x1+B1*y1)
  //   dZ1dY1 = -B1  where B1 - plane param. (z1=D1-A1*x1+B1*y1)
  // If P1-target:
  //   dZ1dX1 = dZ1/dY1 = 0;
  Double_t dX    = x2-x1;
  Double_t dY    = y2-y1;
  Double_t dZ    = z2-z1;
  Double_t lenXY = TMath::Sqrt(dX*dX+dY*dY);
  if(lenXY<2.E-5) {
    dX    = x2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    dY    = y2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    lenXY = 1.E-5;            // = dX*dX+dY*dY;
    dZ    = 1.;
  }
  dX /= lenXY;
  dY /= lenXY;
  dZ /= lenXY;
  phi   = TMath::ATan2(dY,dX);
  theta = TMath::ATan2(1.,dZ);
  Double_t xy2 = (x2*dX+y2*dY);
  Double_t xy1 = (x1*dX+y1*dY);
  zm = (z1*xy2-z2*xy1)/lenXY;  //= z1-dZ*(x1*dX+y1*dY);
  r0 = y1*dX-x1*dY;

  Double_t dR0dX1 = (y2-r0*dX)*eX1;  // *(-1)
  Double_t dR0dX2 = (y1-r0*dX)*eX2;
  Double_t dR0dY1 = (x2+r0*dY)*eY1;
  Double_t dR0dY2 = (x1+r0*dY)*eY2;  // *(-1)
  eR0 = TMath::Sqrt(dR0dX1*dR0dX1+dR0dY1*dR0dY1+dR0dX2*dR0dX2+dR0dY2*dR0dY2)/lenXY;

  Double_t zx12   = (z1*dX-x1*dZ)-2*zm*dX;
  Double_t zy12   = (z1*dY-y1*dZ)-2*zm*dY;
  Double_t dZmdZ1 = xy2*eZ1;
  Double_t dZmdX1 = (xy2*dZ1dX1-zx12-z2*dX)*eX1;
  Double_t dZmdX2 = (xy1*dZ2dX2-zx12-z1*dX)*eX2;  // *(-1)
  Double_t dZmdY1 = (xy2*dZ1dY1-zy12-z2*dY)*eY1;
  Double_t dZmdY2 = (xy1*dZ2dY2-zy12-z1*dY)*eY2;  // *(-1)
  eZm = TMath::Sqrt(dZmdZ1*dZmdZ1+dZmdX1*dZmdX1+dZmdX2*dZmdX2+dZmdY1*dZmdY1+
        dZmdY2*dZmdY2)/lenXY;

  Double_t drvTh  = 1./(1.+dZ*dZ);
  Double_t dThdX1 = (dZ*dX-dZ1dX1)*eX1; // *(-1)
  Double_t dThdY1 = (dZ*dY-dZ1dY1)*eY1; // *(-1)
  Double_t dThdX2 = (dZ*dX-dZ2dX2)*eX2;
  Double_t dThdY2 = (dZ*dY-dZ2dY2)*eY2;
  //  Double_t dThdZ1=eZ1;
  eTheta = TMath::Sqrt(dThdX1*dThdX1+dThdY1*dThdY1+eZ1*eZ1+dThdX2*dThdX2+dThdY2*dThdY2)*
           drvTh/lenXY;

  Double_t dPhdX1 = dY*eX1;
  Double_t dPhdX2 = dY*eX2; // *(-1)
  Double_t dPhdY1 = dX*eY1; // *(-1)
  Double_t dPhdY2 = dX*eY2;
  ePhi = TMath::Sqrt(dPhdX1*dPhdX1+dPhdX2*dPhdX2+dPhdY1*dPhdY1+dPhdY2*dPhdY2)/lenXY;
}

void HMdcSizesCells::calcRZToTargLine(Double_t x1, Double_t y1, Double_t z1, 
                                Double_t x2, Double_t y2, Double_t z2, 
    Double_t &zm, Double_t &r0) const {
  // Input and output are in lab.sys.
  calcRZToLineXY(x1,y1,z1,x2,y2,z2,targ3p[2].getX(),targ3p[2].getY(),zm,r0);
}

void HMdcSizesCellsSec::calcRZToTargLine(Double_t x1, Double_t y1, Double_t z1, 
                                Double_t x2, Double_t y2, Double_t z2, 
    Double_t &zm, Double_t &r0) const {
  // Input and output are in sector coor.sys.
  HMdcSizesCells::calcRZToLineXY(x1,y1,z1,x2,y2,z2,
      targ3p[2].getX(),targ3p[2].getY(),zm,r0);
}

void HMdcSizesCells::calcRZToLineXY(Double_t x1, Double_t y1, Double_t z1, 
                                Double_t x2, Double_t y2, Double_t z2, 
    Double_t xBeam, Double_t yBeam, Double_t &zm, Double_t &r0) {
  // zm= z1 - cos(theta)/sin(theta) * ((x1-xBeam)*cos(phi)+(y1-yBeam)*sin(phi))
  // r0=(y1-yBeam)*cos(phi)-(x1-xBeam)*sin(phi)
  //
  // If (x1,y1,z1)=(x2,y2,z2) dZ will eq.1.! 
  Double_t dX     = x2-x1;
  Double_t dY     = y2-y1;
  Double_t dZ     = z2-z1;
  Double_t lenXY2 = dX*dX+dY*dY;
  Double_t lenXY  = TMath::Sqrt(lenXY2);
  if(lenXY<2.E-5) {
    dX     = x2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    dY     = y2 * 1.E-5/TMath::Sqrt(x2*x2+y2*y2);
    lenXY  = 1.E-5;            //dX*dX+dY*dY;
    lenXY2 = lenXY*lenXY;
    dZ     = 1.;
  }
  x1 -= xBeam;  // Shifting to beam line
  y1 -= yBeam;  // Shifting to beam line
  
  zm = z1-dZ*(x1*dX+y1*dY)/lenXY2;
  r0 = (y1*dX-x1*dY)/lenXY;
}

void HMdcSizesCells::setTransform(Double_t* par, HGeomTransform& trans) {
  // Filling  HGeomTransform object by 6 parameters
  // par[0] - shift along X
  // par[1] - shift along Y
  // par[2] - shift along Z
  // par[3] - rotation around X (in degree)
  // par[4] - rotation around Y (in degree)
  // par[5] - rotation around Z (in degree)
  if(par==0) return;
  
  Double_t alpha    = par[3]*TMath::DegToRad();
  Double_t beta     = par[4]*TMath::DegToRad();
  Double_t gamma    = par[5]*TMath::DegToRad();

  Double_t cosAlpha = TMath::Cos(alpha);
  Double_t sinAlpha = TMath::Sin(alpha);
  Double_t cosBeta  = TMath::Cos(beta);
  Double_t sinBeta  = TMath::Sin(beta);
  Double_t cosGamma = TMath::Cos(gamma);
  Double_t sinGamma = TMath::Sin(gamma);

  Double_t rot[9];
  rot[0] = cosBeta*cosGamma;
  rot[1] = -cosBeta*sinGamma;
  rot[2] = sinBeta;
  rot[3] = sinAlpha*sinBeta*cosGamma+cosAlpha*sinGamma;
  rot[4] = -sinAlpha*sinBeta*sinGamma+cosAlpha*cosGamma;
  rot[5] = -sinAlpha*cosBeta;
  rot[6] = -cosAlpha*sinBeta*cosGamma+sinAlpha*sinGamma;
  rot[7] = cosAlpha*sinBeta*sinGamma+sinAlpha*cosGamma;
  rot[8] = cosAlpha*cosBeta;

  trans.setRotMatrix(rot);
  trans.setTransVector(par);
}

void HMdcSizesCells::copyTransfToArr(const HGeomTransform& trans,
    Double_t* arr) {
  // Copy trans to array
  // Array size must be eq. or more 12!
  const HGeomRotation& rot = trans.getRotMatrix();
  const HGeomVector&   trn = trans.getTransVector();
  for(Int_t i=0; i<9; i++) arr[i]=rot(i);
  arr[ 9] = trn(0);
  arr[10] = trn(1);
  arr[11] = trn(2);
}

void HMdcSizesCells::rotateYZ(const HGeomRotation& rot,
    Double_t xi,Double_t yi,Double_t zi,Double_t& yo, Double_t& zo) {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: yo,zo - rotated layer coor.sys.
  yo = rot(1)*xi + rot(4)*yi + rot(7)*zi;
  zo = rot(2)*xi + rot(5)*yi + rot(8)*zi;
}

void HMdcSizesCells::rotateXYZ(const HGeomRotation& rot,
    Double_t xi,Double_t yi,Double_t zi,
    Double_t& xo, Double_t& yo,Double_t& zo) {
  // Input:  xi,yi,zi - sector coor.sys.
  // Output: x0,yo,zo - rotated layer coor.sys.
  xo = rot(0)*xi + rot(3)*yi + rot(6)*zi;
  yo = rot(1)*xi + rot(4)*yi + rot(7)*zi;
  zo = rot(2)*xi + rot(5)*yi + rot(8)*zi;
}


void HMdcSizesCells::rotateDir(const HGeomRotation& rot,
    const HGeomVector& d,Double_t& dy, Double_t& dz) {
  // Input:  rot - rotation matrix; d - vector...
  // Output: yo,zo - rotated layer coor.sys.
  dy = rot(1)*d.getX()+rot(4)*d.getY()+rot(7)*d.getZ();
  dz = rot(2)*d.getX()+rot(5)*d.getY()+rot(8)*d.getZ();
}

void HMdcSizesCells::rotateDir(const HGeomRotation& rot,
    const HGeomVector& d,Double_t& dx, Double_t& dy, Double_t& dz) {
  // Input:  rot - rotation matrix; d - vector...
  // Output: x0,yo,zo - rotated layer coor.sys.
  dx = rot(0)*d.getX()+rot(3)*d.getY()+rot(6)*d.getZ();
  dy = rot(1)*d.getX()+rot(4)*d.getY()+rot(7)*d.getZ();
  dz = rot(2)*d.getX()+rot(5)*d.getY()+rot(8)*d.getZ();
}

void HMdcSizesCells::transLineToOtherSec(const HMdcLineParam& ln,Int_t sec,
    HGeomVector& p1,HGeomVector& p2) {
  Int_t parSec = ln.getSec();
  p1 = ln.getFisrtPoint().getVector();
  p2 = ln.getSecondPoint().getVector();
  if(parSec==sec || parSec<0) return;
  HMdcSizesCellsSec& pSCellsSec0 = (*this)[parSec];
  const HGeomTransform* tr0 = pSCellsSec0.getLabTrans();
  p1 = tr0->transFrom(p1);
  p2 = tr0->transFrom(p2);
  HMdcSizesCellsSec& pSCellsSec1 = (*this)[sec];
  const HGeomTransform* tr1 = pSCellsSec1.getLabTrans();
  p1 = tr1->transTo(p1);
  p2 = tr1->transTo(p2);
}

void HMdcSizesCells::transLineToAnotherSec(HMdcLineParam& ln,Int_t anotherSec) {
  // Transformation of track line to another sector coor.system (for cosmic!)
  // !!! Error matrix is not transformed !!!
  Int_t parSec = ln.getSec();
  if(parSec==anotherSec || parSec<0) return;
  const HGeomTransform* tr0 = (*this)[parSec].getLabTrans();
  const HGeomTransform* tr1 = (*this)[anotherSec].getLabTrans();
  ln.transFrom(tr0);
  ln.transTo(tr1,anotherSec);
}

Int_t HMdcSizesCells::nCells(Int_t s,Int_t m,Int_t l) const {
  return ((*fMdcGeomStruct)[s][m])[l];
}

void HMdcSizesCellsSec::setNumOfTargets(HGeomVector* targ3pLab,
    Int_t nt,HGeomVector* targetsLab) {
  for(Int_t i=0;i<3;i++) targ3p[i] = sysRLab.transTo(targ3pLab[i]);
  if(targets && numTargets<nt) {
    delete [] targets;
    targets = 0;
  }
  numTargets = nt;
  if(targets==0) targets = new HGeomVector[numTargets];
  for(Int_t i=0;i<numTargets;i++) targets[i] = sysRLab.transTo(targetsLab[i]);
}

Double_t HMdcSizesCells::getMaxTargHThick(void) const {
  Double_t maxThick=0;
  for(Int_t i=0;i<numTargets;i++) {
    if(maxThick<tarHalfThick[i]) maxThick = tarHalfThick[i];
  }
  return maxThick;
}
