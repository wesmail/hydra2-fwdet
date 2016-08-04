#ifndef HGEOMVECTOR2_H
#define HGEOMVECTOR2_H
using namespace std;
#include "TObject.h"
#include "TMath.h"
#include <math.h>
#include <iostream> 
#include <iomanip>
#include "hgeomvector.h"


class HGeomVector2 : public HGeomVector {
protected:
  Double_t Rad;
  Double_t Phi;
  Double_t Theta;
public:
  HGeomVector2(Double_t dx=0,Double_t dy=0,Double_t dz=0):  
      HGeomVector(dx,dy,dz)
  {
      Rad=Phi=Theta=0.0;
  }
  HGeomVector2(const HGeomVector& v) : HGeomVector(v)
  {
      Rad=Phi=Theta=0.0;
  }
  ~HGeomVector2() {}
  Double_t getRad() const {return Rad;}
  Double_t getPhi() const {return Phi;}
  Double_t getTheta() const {return Theta;}
  void setRad(const Double_t a) {Rad=a;}
  void setPhi(const Double_t a) {Phi=a;}
  void setTheta(const Double_t a) {Theta=a;}
  void sphereCoord(Float_t& rad, Float_t& theta, Float_t& phi);
//  friend ostream& operator << (ostream& put,const HGeomVector& v);
//  friend istream& operator >> (istream& get,HGeomVector& v);

  ClassDef(HGeomVector2,1) // vector with 3 spherical components
};

// -------------------- inlines ---------------------------

inline void HGeomVector2::sphereCoord(Float_t& rad, Float_t& theta, Float_t& phi) {
  Double_t r,k;

  rad = length();
  k = 180./(TMath::Pi());

  theta = k*(TMath::ACos(z/rad));

  r = TMath::Sqrt(x*x+y*y);
  phi = k*(TMath::ACos(x/r));
  if (y<0) phi = 360. - phi;
}

/*
inline ostream& operator << (ostream& put,const HGeomVector& v) {
  return put<<v(0)<<"  "<<v(1)<<"  "<<v(2)<<'\n';
}

inline istream& operator >> (istream& get,HGeomVector& v) {
  Double_t x[3];
  get>>x[0]>>x[1]>>x[2];
  v.setVector(x);
  return get;
}
*/

#endif /* !HGEOMVECTOR_H */

