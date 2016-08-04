#ifndef HGEOMBASICSHAPE_H
#define HGEOMBASICSHAPE_H
using namespace std;
#include "hgeomtransform.h"
#include "TNamed.h"
#include "TString.h"
#include "TArrayD.h"
#include "TShape.h"
#include <fstream> 

class HGeomVolume;

class HGeomBasicShape : public TNamed {
protected:
  Int_t nPoints;            // number of points describing the shape 
  Int_t nParam;             // number of parameters needed to create the ROOT shape
  TArrayD* param;           // parameters needed to create the ROOT shape
  HGeomTransform* center;   // position of the volume center in the technical KS
  HGeomTransform* position; // position of the ROOT volume in its ROOT mother
public:
  HGeomBasicShape();
  virtual ~HGeomBasicShape();
  Int_t getNumPoints() {return nPoints;}
  Int_t getNumParam() {return nParam;}
  TArrayD* getParam() {return param;}
  HGeomTransform* getCenterPosition() {return center;}
  HGeomTransform* getVoluPosition() {return position;}
  virtual Int_t readPoints(fstream*,HGeomVolume*);   
  virtual Bool_t writePoints(fstream*,HGeomVolume*);   
  virtual void printPoints(HGeomVolume* volu);
  virtual TArrayD* calcVoluParam(HGeomVolume*) {return 0;} 
  virtual void calcVoluPosition(HGeomVolume*,const HGeomTransform&);
  virtual void calcVoluPosition(HGeomVolume*,const HGeomTransform&,
			  const HGeomTransform&) {return;}
  void printParam();
protected:
  void posInMother(const HGeomTransform&,const HGeomTransform&);
  ClassDef(HGeomBasicShape,0) // base class for all shapes
};

#endif  /* !HGEOMBASICSHAPE_H */
