//*-- Author : Anar Rustamov 
//*-- Last modified : 10/08/2005 by Ilse Koenig

#include "hsplinepar.h"
#include <iostream>
ClassImp(HSplinePar)
using namespace std;

HSplinePar::HSplinePar() 
{
for(Int_t i=0; i<52; i++)
    {
      xPoints[i]=-1000.;
      yPoints[i]=-1000.;
      zPoints[i]=-1000.;
    }
}

HSplinePar::~HSplinePar() 
{
  ;
}

void HSplinePar::init(Int_t N, Int_t NN)
{
  ; //will be implemented
}

void HSplinePar::clear()
{
  ; //will be implemented
}

void HSplinePar::remove(Double_t *pD)
{
  delete [] pD; pD=0;
}

void HSplinePar::remove(Float_t *pF)
{
  delete [] pF; pF=0;
}

void HSplinePar::setSplinePoints(Float_t *x, Float_t *y, Float_t *z)
{

  for(Int_t i=0; i<52; i++)
    {
      xPoints[i]=x[i];
      yPoints[i]=y[i];
      zPoints[i]=z[i];
    }

}

void HSplinePar::getSplinePoints(Float_t *x, Float_t *y, Float_t *z)
{

  for(Int_t i=0; i<52; i++)
    {
      x[i]=xPoints[i];
      y[i]=yPoints[i];
      z[i]=zPoints[i];
    }

}
