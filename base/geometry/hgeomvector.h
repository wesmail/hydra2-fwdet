#ifndef HGEOMVECTOR_H
#define HGEOMVECTOR_H
using namespace std;
#include "TObject.h"
#include "TMath.h"
#include <math.h>
#include <iostream>
#include <iomanip>

class HGeomVector : public TObject {
protected:
  Double_t x;
  Double_t y;
  Double_t z;
  inline void round(Double_t d,Int_t n);
public:
  HGeomVector(Double_t dx=0,Double_t dy=0,Double_t dz=0) : x(dx),y(dy),z(dz) {}
  ~HGeomVector() {}
  Double_t &X() {return x;}
  Double_t &Y() {return y;}
  Double_t &Z() {return z;}
  Double_t getX() const {return x;}
  Double_t getY() const {return y;}
  Double_t getZ() const {return z;}
  void setXYZ(const Double_t xx,const Double_t yy,const Double_t zz) {
    x=xx; y=yy; z=zz;
  }
  void setX(const Double_t a) {x=a;}
  void setY(const Double_t a) {y=a;}
  void setZ(const Double_t a) {z=a;}
  inline void setVector(const Double_t* a);
  inline void setVector(const Float_t* a);
  inline Double_t operator() (const Int_t i) const;
  inline HGeomVector operator - () const;
  inline HGeomVector& operator = (const HGeomVector& v);
  inline Bool_t operator == (const HGeomVector& v) const;
  inline Bool_t operator != (const HGeomVector& v) const;
  inline Bool_t operator < (const Double_t a);
  inline Bool_t operator <= (const Double_t a);
  inline Bool_t operator > (const Double_t a);
  inline Bool_t operator >= (const Double_t a);
  inline HGeomVector& operator += (const Double_t a);
  inline HGeomVector& operator -= (const Double_t a);
  inline HGeomVector& operator *= (const Double_t a);
  inline HGeomVector& operator /= (const Double_t a);
  inline HGeomVector& operator += (const HGeomVector& v);
  inline HGeomVector& operator -= (const HGeomVector& v);
  inline HGeomVector operator + (const HGeomVector& v) const;
  inline HGeomVector operator - (const HGeomVector& v) const;
  inline HGeomVector& abs();
  inline Double_t scalarProduct(const HGeomVector& v) const;
  inline HGeomVector vectorProduct(const HGeomVector& v) const;
  Double_t length() const {return sqrt(x*x+y*y+z*z);}
  void clear() {x=y=z=0.;}
  void print() const {printf("%10.3f%10.3f%10.3f\n",x,y,z);}
  inline void round(Int_t n);
  inline friend ostream& operator << (ostream& put,const HGeomVector& v);
  inline friend istream& operator >> (istream& get,HGeomVector& v);
  ClassDef(HGeomVector,1) // vector with 3 components
};

// -------------------- inlines ---------------------------

inline void HGeomVector::setVector(const Double_t* a) {
  x=a[0];
  y=a[1];
  z=a[2];
}

inline void HGeomVector::setVector(const Float_t* a) {
  x=a[0];
  y=a[1];
  z=a[2];
}

inline Double_t HGeomVector::operator() (const Int_t i) const {
  switch (i) {
    case 0: return x;
    case 1: return y;
    case 2: return z;
    default: Error("operator()","bad index");
  }
  return 0;
}

inline HGeomVector HGeomVector::operator - () const {
  HGeomVector p(-x,-y,-z);
  return p;
}

inline HGeomVector& HGeomVector::operator = (const HGeomVector& v) {
  x=v.getX();
  y=v.getY();
  z=v.getZ();
  return *this;
}

inline Bool_t HGeomVector::operator == (const HGeomVector& v) const {
  return ((v.getX()!=x || v.getY()!=y || v.getZ()!=z) ? kFALSE : kTRUE); 
}

inline  Bool_t HGeomVector::operator != (const HGeomVector& v) const {
  return (v.getX()!=x || v.getY()!=y || v.getZ()!=z)  ? kTRUE : kFALSE; 
}

inline Bool_t HGeomVector::operator < (const Double_t a) {
  return (x>=a || y>=a || z>=a) ? kFALSE : kTRUE;
}

inline Bool_t HGeomVector::operator <= (const Double_t a) {
  return (x>a || y>a || z>a) ? kFALSE : kTRUE;
}

inline Bool_t HGeomVector::operator > (const Double_t a) {
  return (x<=a || y<=a || z<=a) ? kFALSE : kTRUE;
}

inline Bool_t HGeomVector::operator >= (const Double_t a) {
  return (x<a || y<a || z<a) ? kFALSE : kTRUE;
}

inline HGeomVector& HGeomVector::operator += (const Double_t a) {
  x+=a;
  y+=a;
  z+=a;
  return *this;
}

inline HGeomVector& HGeomVector::operator -= (const Double_t a) {
  x-=a;
  y-=a;
  z-=a;
  return *this;
}

inline HGeomVector& HGeomVector::operator *= (const Double_t a) {
  x*=a;
  y*=a;
  z*=a;
  return *this;
}

inline HGeomVector& HGeomVector::operator /= (const Double_t a) {
  x/=a;
  y/=a;
  z/=a;
  return *this;
}

inline HGeomVector& HGeomVector::operator += (const HGeomVector& v) {
  x+=v.getX();
  y+=v.getY();
  z+=v.getZ();
  return *this;
}

inline HGeomVector& HGeomVector::operator -= (const HGeomVector& v) {
  x-=v.getX();
  y-=v.getY();
  z-=v.getZ();
  return *this;
}

inline HGeomVector HGeomVector::operator + (const HGeomVector& v) const {
  HGeomVector p(*this);
  p+=v;
  return p;
}

inline HGeomVector HGeomVector::operator - (const HGeomVector& v) const {
  HGeomVector p(*this);
  p-=v;
  return p;
}

inline HGeomVector& HGeomVector::abs() {
  x=TMath::Abs(x);
  y=TMath::Abs(y);
  z=TMath::Abs(z);
  return *this;
}

inline Double_t HGeomVector::scalarProduct(const HGeomVector& v) const {
  return (x*v.getX()+y*v.getY()+z*v.getZ());
}

inline HGeomVector HGeomVector::vectorProduct(const HGeomVector& v) const {
  HGeomVector p(y*v.getZ()-z*v.getY(),z*v.getX()-x*v.getZ(),
                x*v.getY()-y*v.getX());
  return p;
}

inline void HGeomVector::round(Double_t d,Int_t n) {
  // rounds d to a precision with n digits
    if(d>0) d=floor(d*pow(10.,n)+0.5)/pow(10.,n);
  else d=-floor((-d)*pow(10.,n)+0.5)/pow(10.,n);
}

inline void HGeomVector::round(Int_t n) {
  // rounds every component to a precision with n digits
  round(x,n);
  round(y,n);
  round(z,n);
}

inline ostream& operator << (ostream& put,const HGeomVector& v) {
  return put<<v(0)<<"  "<<v(1)<<"  "<<v(2)<<'\n';
}

inline istream& operator >> (istream& get,HGeomVector& v) {
  Double_t x[3];
  get>>x[0]>>x[1]>>x[2];
  v.setVector(x);
  return get;
}

#endif /* !HGEOMVECTOR_H */













