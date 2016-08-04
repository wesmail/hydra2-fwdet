#include "hshowergeometry.h"
#include "hshowergeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hgeomcompositevolume.h"

ClassImp(HShowerGeometry)

HShowerGeometry::HShowerGeometry(const Char_t* name,
                                 const Char_t* title,
                                 const Char_t* context)
               :HParCond(name,title,context) {
  nSectors = 6;
  nModules = 3;
  nRows = 32;
  nColumns = 32;
  for(Int_t i = 0; i < nModules; i++) {
    frames[i].setModuleID(i);
    wires[i].setModuleID(i);
    pads[i].setModuleID(i);
    pads[i].setFrame(&frames[i]);
  }
  if (gHades) {
    pGeomPar=(HShowerGeomPar*)(gHades->getRuntimeDb()->getContainer("ShowerGeomPar"));
  } else {
    pGeomPar=0;
  }
}

HShowerGeometry::~HShowerGeometry() {
  clear();
}

Bool_t HShowerGeometry::init(HParIo* inp,Int_t* set) {
  Bool_t rc=fillFromGeomPar();
  if (rc) {
    HDetParIo* input=inp->getDetParIo("HCondParIo");
    if (input) rc=input->init(this,set);
  }
  return rc;
}

void HShowerGeometry::clear() {
  for(Int_t i=0; i<nModules; i++) {
    wires[i].reset();
    pads[i].reset();
  }
}

void HShowerGeometry::putParams(HParamList* l) {
  if (!l) return;
  for(Int_t i=0; i<nModules; i++) {
    wires[i].putParams(l);
    pads[i].putParams(l);
  }
}

Bool_t HShowerGeometry::getParams(HParamList* l) {
  if (!l) return kFALSE;
  for(Int_t i=0; i<nModules; i++) {
    if (!wires[i].getParams(l)) return kFALSE;
    if (!pads[i].getParams(l))  return kFALSE;
  }
#if DEBUG_LEVEL > 0
  printParams();
#endif
  return kTRUE;
}

Bool_t HShowerGeometry::fillFromGeomPar(){
  //Function for calculating the module centers and tranformation used in analysis
  //(corresponding to the PAD_FIBREs)
  if (!pGeomPar) return kFALSE;
  if (!pGeomPar->hasChanged()) return kTRUE;
  HGeomVector vm[3] = { HGeomVector(0.000 , 0.00 ,  -4.0),
                        HGeomVector(0.000 , 0.00 ,  67.6), 
                        HGeomVector(0.000 , 0.00 , 134.9)};
  for (Int_t s=0;s<nSectors;s++) {
    sectorTransform[s].clear();
    for (Int_t m=0;m<nModules;m++) {
      moduleTransform[s*nModules+m].clear();
    }
  }
  for (Int_t s=0;s<pGeomPar->getMaxSectors();s++) {
    sectorTransform[s]=pGeomPar->getModule(s,0)->getLabTransform();
    for(Int_t m=0;m<pGeomPar->getMaxModules();m++) {
      HGeomTransform trans=pGeomPar->getModule(s,m)->getLabTransform();
      HGeomVector v_mod = trans.getRotMatrix()*vm[m]+trans.getTransVector();
      trans.setTransVector(v_mod);
      moduleTransform[s*nModules+m]=trans;
    }  
  }
  HGeomVector *points[4];
  HShowerFrameCorner *pFC;
  for(Int_t m = 0; m < 3; m++) {
    HGeomCompositeVolume* refMod=pGeomPar->getRefVolume(m);
    HGeomVolume *comp = refMod->getComponent(0);//only one comp; wires plane
    points[0] = comp->getPoint(3);//geant coordinate system is left-handed!! 
    points[1] = comp->getPoint(2);//
    points[2] = comp->getPoint(1);//
    points[3] = comp->getPoint(0);//
    frames[m].reset();
    for (Int_t i=0;i<4;i++) {
      pFC = new HShowerFrameCorner;
      pFC->setX(points[i]->getX()*0.1);
      pFC->setY(points[i]->getY()*0.1);
      if(i==0 || i==3) pFC->setFlagArea(1);
      else pFC->setFlagArea(0);
      frames[m].addCorner(pFC);
    }
  }
  return kTRUE;
}

void HShowerGeometry::printParams() {
  HParCond::printParams();
  for (Int_t s=0; s < nSectors; s++) {
    cout<<"Lab transform of wire plane 0 in sector "<<s<<endl; 
    sectorTransform[s].print();
    for (Int_t m=0; m < nModules; m++) {
      cout<<"Lab transform pad plane sector "<<s<<" module "<<m<<endl; 
      moduleTransform[s*nModules+m].print();
    }
  }
  for (Int_t m=0; m < nModules; m++) {
    frames[m].printParams();
  }
};

void HShowerGeometry::getLocalCoord(HLocation& loc, HGeomVector& v) {
//retrieve local coordinates of center of pads in position loc

  Float_t fX, fY;
  Float_t scale = 10.0; //transform cm to mm

  HShowerPadTab* pPads = getPadParam(loc[1]);
  pPads->getPad(loc[2], loc[3])->getPadCenter(&fX, &fY);

  v.setX(scale*fX);
  v.setY(scale*fY);
  v.setZ(0.0);  //z==0 local coordinates are in plain
}

void HShowerGeometry::transVectToLab(HLocation& loc, HGeomVector vLocal, HGeomVector2& vLab) {
// transform X_loc, Y_Loc -> X,Y,Z Lab

  HGeomTransform tt = getTransform(loc[0], loc[1]);
  vLab = tt.transFrom(vLocal);
}

void HShowerGeometry::getLabCoord(HLocation& loc, HGeomVector2& v) {
// retrieve lab coordinates of center of pads in position loc
// Local coordinates are transformed to lab using HGeomTransform class
// There is data for transformation this in HShowerModuleGeom object
// addressed by loc[0], loc[1]

  HGeomVector vLoc;
  getLocalCoord(loc, vLoc);
  HGeomTransform tt = getTransform(loc[0], loc[1]);
  v = tt.transFrom(vLoc);
}

void HShowerGeometry::getSphereCoord(HLocation& loc, HGeomVector2& v, HGeomVector *pvTarget){
// Sphere coordinate are calculated from lab coordinates
  Float_t r,theta,phi;
  HGeomVector2 vLoc;

  getLabCoord(loc,vLoc);

  if(pvTarget != NULL)
    vLoc -= *pvTarget;

  vLoc.sphereCoord(r,theta,phi); //calculation of sphere coord

  v.setRad(r);
  v.setTheta(theta);
  v.setPhi(phi);
}

void HShowerGeometry::transLabToSphereCoord(HGeomVector2& labIn, HGeomVector2& sphereOut, HGeomVector *pvTarget){
// transform X_Lab,Y_Lab,Z_Lab -> Phi,Theta,R
//Sphere coordinate are calculated from lab coordinates
  Float_t r,theta,phi;

  if(pvTarget != NULL)
    //vLoc -= *pvTarget;
    labIn -= *pvTarget;

  labIn.sphereCoord(r,theta,phi); //calculation of sphere coord

  sphereOut.setRad(r);
  sphereOut.setTheta(theta);
  sphereOut.setPhi(phi);
}

Bool_t HShowerGeometry::transPadToLab() {
  HGeomTransform tt;
  HGeomVector2 vLoc,vLab;
  Float_t fX,fY,r,theta,phi;
  Float_t scale = 10.0; //transform cm to mm
  FILE *fp;
  fp = fopen("test.txt","w");

  for(Int_t i=0;i<nSectors;i++) {
    for(Int_t j=0;j<nModules;j++) {
      for (Int_t k=0;k < nRows; k++) {
        for (Int_t l=0;l < nColumns; l++) {

          HShowerPadTab* pPads = getPadParam(j);
          pPads->getPad(k,l)->getPadCenter(&fX, &fY);
          vLoc.setX(scale*fX);
          vLoc.setY(scale*fY);
          vLoc.setZ(0.0);

          tt = getTransform(i,j);
          vLab = tt.transFrom(vLoc);

          vLab.sphereCoord(r,theta,phi);
          if(!j) fprintf(fp,"%3d%3d%3d %10.3f%10.3f\n",i,k,l,phi,theta);

        }
      }
    }
  }
  fclose(fp);
  return kTRUE;
}
