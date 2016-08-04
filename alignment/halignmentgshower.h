#ifndef HALIGNMENTGSHOWER_H
#define HALIGNMENTGSHOWER_H
#include "TObject.h"
#include "hsymmat.h"
#include "fstream"
#include "iostream"
#include "TString.h"
#include "halignmentgparticle.h"
#include "hgeomtransform.h"


class TClonesArray;
class HAlignmentGGeom;
class HAlignmentGRotations;
class TH2F;
class TH1F;
class TMinuit;
class TFile;
class SplineGeantMy;
class TNtuple;

class HAlignmentGShower:public TObject
   {
   public:
     HAlignmentGShower();
     virtual ~HAlignmentGShower();
     void SetNtuple(TNtuple *_nt); 
     TNtuple *GetNtuple() {return nt;}
     HAlignmentGGeom*  GetAlignGeom()      {return fAlignGeom;}
     HAlignmentGRotations*  GetAlignRot()  {return fAlignRot  ;}
     Float_t GetMinFunction(HGeomTransform &trans);
     
     void seTrans(HGeomTransform &tr) {transOld = tr;}
     HGeomTransform getTransOld() {return transOld; }
     HGeomTransform getTransNew() {return transNew; }
     
     void getPoints(HGeomVector&, HGeomVector&,HGeomVector &, HGeomTransform &, Float_t *);
    
     void CheckAlignment(HGeomTransform , HGeomTransform , TFile *);				    

     Int_t getAlignSec() {return alignSec;} 
     void  setAlignMode( Int_t _alignMode) {alignMode = _alignMode;}
     Int_t getAlignMode() {return alignMode;}
     
     
     void AlignShower(Int_t);
   private:
   
     HGeomVector point1, point2, pointSh;
     Int_t alignSec;
     
     
      Float_t diffZ;
      Float_t diffX;
      Float_t diffY;
      Float_t xCr ;
      Float_t yCr ;

      Int_t alignMode;     
     
   
     HAlignmentGGeom      *fAlignGeom ;
     HAlignmentGRotations *fAlignRot  ;
     HGeomTransform transOld;
     HGeomTransform transNew;
   
     TMinuit *fMinuit;
     TNtuple *nt;
     
     Float_t x1, y1, z1, x2, y2, z2;
     Float_t xsh, ysh, zsh,xshl, yshl, zshl;
     Float_t sec;
     
     ClassDef(HAlignmentGShower,0)
    };   
#endif
    
