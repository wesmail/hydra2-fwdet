//*-- Author : Anar Rustamov

#include "halignmentgparticle.h"
#include "hgeomvector.h"

using namespace std;

ClassImp(HAlignmentGParticle)
  
  HAlignmentGParticle::HAlignmentGParticle()
{
  
  for(Int_t i=0; i<4; i++)
    {
      X[i]=0.;
      Xdir[i]=0.;
      Y[i]=0.;
      Ydir[i]=0.;
    }
  isGood=kTRUE;
}

HAlignmentGParticle::HAlignmentGParticle(HAlignmentGParticle &p) :TObject(p)
{
  for(Int_t i=0; i<4; i++)
    {
      X[i] =      p.GetX(i);
      Xdir[i]=    p.GetXdir(i);
      Y[i]=       p.GetY(i);
      Ydir[i]=    p.GetYdir(i);
    }
  SetPoints();
  isGood=kTRUE;
}

void HAlignmentGParticle::SetX(Float_t x1, Float_t x2, Float_t x3, Float_t x4)
{
  X[0]=x1;
  X[1]=x2;
  X[2]=x3;
  X[3]=x4;
}

void HAlignmentGParticle::SetXdir(Float_t x1, Float_t x2, Float_t x3, Float_t x4)
{
  Xdir[0]=x1;
  Xdir[1]=x2;
  Xdir[2]=x3;
  Xdir[3]=x4;
}

void HAlignmentGParticle::SetY(Float_t x1, Float_t x2, Float_t x3, Float_t x4)
{
  Y[0]=x1;
  Y[1]=x2;
  Y[2]=x3;
  Y[3]=x4;
}

void HAlignmentGParticle::SetYdir(Float_t x1, Float_t x2, Float_t x3, Float_t x4)
{
  Ydir[0]=x1;
  Ydir[1]=x2;
  Ydir[2]=x3;
  Ydir[3]=x4;
}

void HAlignmentGParticle::SetPoints()
{
  Point[0].setXYZ(X[0],Y[0],0.);
  Point[1].setXYZ(X[1],Y[1],0.);
  Point[2].setXYZ(X[2],Y[2],0.);
  Point[3].setXYZ(X[3],Y[3],0.);
  Dir[0].setXYZ(Xdir[0], Ydir[0], sqrt(fabs(1.-Xdir[0]*Xdir[0]-Ydir[0]*Ydir[0])));
  Dir[1].setXYZ(Xdir[1], Ydir[1], sqrt(fabs(1.-Xdir[1]*Xdir[1]-Ydir[1]*Ydir[1])));
  Dir[2].setXYZ(Xdir[2], Ydir[2], sqrt(fabs(1.-Xdir[2]*Xdir[2]-Ydir[2]*Ydir[2])));
  Dir[3].setXYZ(Xdir[3], Ydir[3], sqrt(fabs(1.-Xdir[3]*Xdir[3]-Ydir[3]*Ydir[3])));
}
