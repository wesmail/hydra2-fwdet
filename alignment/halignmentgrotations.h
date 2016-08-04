#ifndef HALIGNMENTGROTATIONS_H
#define HALIGNMENTGROTATIONS_H

#include "TObject.h"
#include "iostream"
#include "fstream"

class HMdcGetContainers;
class HGeomTransform;
class HGeomVector;
class TString;
class HAlignmentGRotations:public TObject
{
 public:
  HAlignmentGRotations();
  ~HAlignmentGRotations() {;}
  HGeomVector TransMdc(HGeomVector &, TString, Int_t,Int_t);
  HGeomVector TransMdc(HGeomVector &, TString, Int_t);
  
  HGeomTransform* GetTransMdc(Int_t,Int_t);
  HGeomTransform* GetTransMdc(Int_t);
  

  HGeomTransform MakeTransMatrix(Double_t , Double_t, Double_t , Double_t , Double_t , Double_t );
  void           GetEulerAngles(HGeomTransform &, Double_t &, Double_t &, Double_t &);
  void           GetTransVector(HGeomTransform &, Double_t &, Double_t &, Double_t &);
 private:
 HGeomTransform *transMdcLabToMod[6][4];
 HGeomTransform *transMdcSecToMod[6][4];
 HGeomTransform *transMdcLabToSec[6];
 HMdcGetContainers *fGetCont; 

ClassDef(HAlignmentGRotations,0)
    };   
#endif
    
