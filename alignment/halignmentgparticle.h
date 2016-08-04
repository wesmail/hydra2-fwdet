#ifndef HALIGNMENTGPARTICLE_H
#define HALIGNMENTGPARTICLE_H

#include "TObject.h"
#include "hgeomvector.h"

class HAlignmentGParticle:public TObject
{
 public:
  HAlignmentGParticle();
  HAlignmentGParticle(HAlignmentGParticle&);
  ~HAlignmentGParticle() {;}
  void SetX(Float_t , Float_t, Float_t , Float_t );
  void SetXdir(Float_t , Float_t , Float_t ,Float_t );
  void SetY(Float_t ,Float_t ,Float_t ,Float_t );
  void SetYdir(Float_t ,Float_t ,Float_t, Float_t);
  void SetPoints();
  HGeomVector GetPoint(Int_t i) {return Point[i];}
  HGeomVector GetDir(Int_t i) {return Dir[i];}
  void SetIsGood(Bool_t _isGood) {isGood=_isGood;}
  Bool_t GetIsGood() {return isGood;}
  Float_t  GetX(Int_t i)    {return X[i];}
  Float_t  GetXdir(Int_t i) {return Xdir[i];}
  Float_t  GetY(Int_t i)    {return Y[i];}  
  Float_t  GetYdir(Int_t i) {return Ydir[i];}
 private:
  Float_t X[4];
  Float_t Xdir[4];
  Float_t Y[4];
  Float_t Ydir[4];
  HGeomVector Point[4];
  HGeomVector Dir[4];
  Bool_t isGood;
  ClassDef(HAlignmentGParticle,0)
    };   
#endif
    
