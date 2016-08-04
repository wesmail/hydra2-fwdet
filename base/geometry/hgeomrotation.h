#ifndef HGEOMROTATION_H
#define HGEOMROTATION_H
using namespace std;
#include "TObject.h"
#include "hgeomvector.h"
#include "TRotMatrix.h"
#include <iostream> 
#include <iomanip>

class HGeomRotation : public TObject {
protected:
  Double_t rot[9];
public:
  inline HGeomRotation();
  inline HGeomRotation(const Double_t*);
  HGeomRotation(const Double_t,const Double_t,const Double_t);
  ~HGeomRotation() {}
  inline void setMatrix(const Double_t*);
  inline void setMatrix(const Float_t*);
  void setEulerAngles(const Double_t,const Double_t,const Double_t);
  inline void setElement(const Double_t,const Int_t);
  inline Double_t operator () (Int_t) const;
  inline HGeomRotation& operator = (const HGeomRotation&);
  inline Bool_t operator == (const HGeomRotation&);
  inline Bool_t operator != (const HGeomRotation&);
  inline HGeomVector operator * (const HGeomVector&) const;
  inline HGeomRotation operator * (const HGeomRotation&) const;
  inline HGeomRotation& operator *= (const HGeomRotation&);
  inline HGeomRotation& transform(const HGeomRotation&);
  inline Bool_t isUnitMatrix();
  inline HGeomRotation inverse() const;
  inline HGeomRotation& invert();
  inline Double_t determinant() const;
  Double_t diff2(const HGeomRotation&) const;
  inline Double_t getElement(Int_t i,Int_t j) const;
  inline void setUnitMatrix();
  inline void setZero();
  inline void print() const;
  TRotMatrix* createTRotMatrix(const Text_t* name="",const Text_t* title="");

  ClassDef(HGeomRotation,1) // rotation matrix
};

// -------------------- inlines ---------------------------

inline HGeomRotation::HGeomRotation() {
  rot[0]=rot[4]=rot[8]=1.;
  rot[1]=rot[2]=rot[3]=rot[5]=rot[6]=rot[7]=0.;
}

inline Double_t HGeomRotation::operator () (Int_t i) const {
  if (i>=0 && i<9) return rot[i];
  Error("operator()","bad index");
  return 0;
}

inline HGeomRotation::HGeomRotation(const Double_t* a) {
  for(Int_t i=0;i<9;i++) rot[i]=a[i];
}
  
inline void HGeomRotation::setMatrix(const Double_t* a) {
  for(Int_t i=0;i<9;i++) rot[i]=a[i];
}
  
inline void HGeomRotation::setMatrix(const Float_t* a) {
  for(Int_t i=0;i<9;i++) rot[i]=a[i];
}
  
inline void HGeomRotation::setElement(const Double_t a, const Int_t i) {
  if (i<9) rot[i]=a;
}

inline Double_t HGeomRotation::getElement(Int_t i,Int_t j) const {
  return rot[i*3+j];
}

inline HGeomRotation& HGeomRotation::operator = (const HGeomRotation& r) {
  for(Int_t i=0;i<9;i++) rot[i]=r(i);
  return *this;
}

inline Bool_t HGeomRotation::operator == (const HGeomRotation& r) {
  Int_t i=0;
  while (i<9) {
    if (rot[i]!=r(i)) return kFALSE;
    i++;
  }
  return kTRUE;
}

inline Bool_t HGeomRotation::operator != (const HGeomRotation& r) {
  Int_t i=0;
  while (i<9) {
    if (rot[i]!=r(i)) return kTRUE;
    i++;
  }
  return kFALSE;
}
 
inline HGeomVector HGeomRotation::operator * (const HGeomVector& v) const {
  return HGeomVector(rot[0]*v(0)+rot[1]*v(1)+rot[2]*v(2),
                     rot[3]*v(0)+rot[4]*v(1)+rot[5]*v(2),
                     rot[6]*v(0)+rot[7]*v(1)+rot[8]*v(2));
}

inline HGeomRotation HGeomRotation::operator * (const HGeomRotation& r) const {
  Double_t a[9];
  for(Int_t i=0;i<9;i++) a[i]=0;
  for(Int_t i=0;i<3;i++) {
    for(Int_t j=0;j<3;j++) {
      Int_t n=3*i+j;
      for(Int_t k=0;k<3;k++) a[n]+=rot[3*i+k]*r(3*k+j);
    }
  }
  return HGeomRotation(&a[0]);
}

inline HGeomRotation& HGeomRotation::operator *= (const HGeomRotation& r) {
  return *this=operator * (r);  
}

inline   HGeomRotation& HGeomRotation::transform(const HGeomRotation& r) {
  return *this=r*(*this);
}

inline Bool_t HGeomRotation::isUnitMatrix() {
  return (rot[0]==1. && rot[1]==0. && rot[2]==0. &&
          rot[3]==0. && rot[4]==1. && rot[5]==0. &&
          rot[6]==0. && rot[7]==0. && rot[8]==1.)  ? kTRUE : kFALSE;
}

inline HGeomRotation HGeomRotation::inverse() const {
  Double_t a[9];
  for(Int_t i=0;i<3;i++) {
    for(Int_t j=0;j<3;j++) a[j+3*i]=rot[i+3*j];
  }
  return HGeomRotation(a); 
}

inline HGeomRotation& HGeomRotation::invert() {
  return *this=inverse();
}

inline Double_t HGeomRotation::determinant() const {
  return rot[0]*(rot[4]*rot[8]-rot[7]*rot[5])
        -rot[3]*(rot[1]*rot[8]-rot[7]*rot[2])
        +rot[6]*(rot[1]*rot[5]-rot[4]*rot[2]);
}

inline void HGeomRotation::setUnitMatrix(){
  rot[0]=rot[4]=rot[8]=1.;
  rot[1]=rot[2]=rot[3]=rot[5]=rot[6]=rot[7]=0.;
}

inline void HGeomRotation::setZero(){
  for(Int_t i=0;i<9;i++) rot[i]=0.;
}

inline void HGeomRotation::print() const {
  for(Int_t i=0;i<9;i++) cout<<rot[i]<<"  ";
  cout<<'\n';
}

#endif /* !HGEOMROTATION_H */
