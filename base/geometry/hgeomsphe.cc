//*-- AUTHOR : Ilse Koenig
//*-- Modified : 11/11/2003 by Ilse Koenig
//*-- Modified : 29/06/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HGeomSphe
//
// class for the GEANT shape SPHE
// 
// The size of a SPHE is defined by 3 'points' (The z-components are not used)
//   point 0:   inner radius of the shell
//              outer radius of the shell;
//   point 1:   starting polar angle of the shell,
//              ending polar angle of the shell;
//   point 2:   starting azimuthal angle of the shell,
//              ending azimuthal angle of the shell; )
//
// The intrinsic coordinate system of a SPHE, which sits in the CAVE and is
// not rotated, is identical with the laboratory system.
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeomsphe.h"
#include "hgeomvolume.h"
#include "hgeomvector.h"

ClassImp(HGeomSphe)

HGeomSphe::HGeomSphe() {
  // constructor
  fName="SPHE";
  nPoints=3;
  nParam=6;
  param=new TArrayD(nParam);
}


HGeomSphe::~HGeomSphe() {
  // default destructor
  if (param) {
    delete param;
    param=0;
  }
  if (center) {
    delete center;
    center=0;
  }
  if (position) {
    delete position;
    position=0;
  }
}


Int_t HGeomSphe::readPoints(fstream* pFile,HGeomVolume* volu) {
  // reads the 3 'points' decribed above from ascii file
  // if the array of points is not existing in the volume it is created and
  // the values are stored inside
  // returns the number of points
  if (!pFile) return 0;
  if (volu->getNumPoints()!=nPoints) volu->createPoints(nPoints);
  Double_t x,y;
  const Int_t maxbuf=155;
  Text_t buf[maxbuf];
  for(Int_t i=0;i<nPoints;i++) {
    pFile->getline(buf,maxbuf);
    sscanf(buf,"%lf%lf",&x,&y);
    volu->setPoint(i,x,y,0.0);
  }
  return nPoints;
}


Bool_t HGeomSphe::writePoints(fstream* pFile,HGeomVolume* volu) {
  // writes the 3 'points' decribed above to ascii file
  if (!pFile) return kFALSE;  
  Text_t buf[155];
  for(Int_t i=0;i<nPoints;i++) {
    HGeomVector& v=*(volu->getPoint(i));
    sprintf(buf,"%9.3f%10.3f\n",v(0),v(1));
    pFile->write(buf,strlen(buf));
  }
  return kTRUE;
}


void HGeomSphe::printPoints(HGeomVolume* volu) {
  // prints volume points to screen
  for(Int_t i=0;i<nPoints;i++) {
    HGeomVector& v=*(volu->getPoint(i));
    printf("%9.3f%10.3f\n",v(0),v(1));
  }
}


TArrayD* HGeomSphe::calcVoluParam(HGeomVolume* volu) {
  // calculates the parameters needed to create the shape SPHE 
  Double_t fac=10.;
  HGeomVector& v0=*(volu->getPoint(0));
  HGeomVector& v1=*(volu->getPoint(1));
  HGeomVector& v2=*(volu->getPoint(2));
  param->AddAt(v0(0)/fac,0);
  param->AddAt(v0(1)/fac,1);
  param->AddAt(v1(0),2);
  param->AddAt(v1(1),3);
  param->AddAt(v2(0),4);
  param->AddAt(v2(1),5);
  return param;
} 


void HGeomSphe::calcVoluPosition(HGeomVolume*,
          const HGeomTransform& dTC,const HGeomTransform& mTR) {
  // calls the function posInMother(...) to calculate the position of the
  // volume in its mother 
  center->clear();
  posInMother(dTC,mTR);
}
