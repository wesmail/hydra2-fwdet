//*-- Author : Anar Rustamov

#include "halignmentgrotations.h"
#include "hmdcgetcontainers.h"
#include "hgeomtransform.h"
#include "TString.h"
#include "hgeomvector.h"
#include "TMath.h"


using namespace std;

ClassImp(HAlignmentGRotations)

  HAlignmentGRotations::HAlignmentGRotations()
{
  
  for(Int_t s = 0; s < 6; s++)
  {
    transMdcLabToSec[s]=NULL;
    for(Int_t m = 0; m < 4; m++)
      {
	transMdcLabToMod[s][m] = NULL;
	transMdcSecToMod[s][m] = NULL;
      }
  }
  fGetCont = HMdcGetContainers::getObject();
  fGetCont -> getMdcGeomPar();
  fGetCont -> getSpecGeomPar();
}

HGeomTransform*  HAlignmentGRotations::GetTransMdc(Int_t s, Int_t m)
{
  //return transformation matrix 
  if(!transMdcLabToMod[s][m])
    {
      transMdcLabToMod[s][m] = new HGeomTransform();
      if(!fGetCont->getLabTransMod(*(transMdcLabToMod[s][m]),s,m))
	{
	  return NULL;
	}
    }
  return transMdcLabToMod[s][m];
}

HGeomTransform*  HAlignmentGRotations::GetTransMdc(Int_t s)
{
  //return transformation matrix
  if(!transMdcLabToSec[s])
    {
      transMdcLabToSec[s]=new HGeomTransform();
      if(!fGetCont->getLabTransSec(*(transMdcLabToSec[s]),s))
	{
	  return NULL;
	}
    }
  return transMdcLabToSec[s];
}


HGeomVector HAlignmentGRotations::TransMdc(HGeomVector &p, TString Option, Int_t s, Int_t m)
{
  HGeomVector pDummy(-10000.,-10000,-10000.);
  if(!transMdcLabToMod[s][m])
    {
      transMdcLabToMod[s][m]=new HGeomTransform();
      if(!fGetCont->getLabTransMod(*(transMdcLabToMod[s][m]),s,m))
	{
	   return pDummy;
	}
    }
  
  if(Option=="FromModToLab")
    {
      
      return transMdcLabToMod[s][m]->transFrom(p);
    }
  if(Option=="FromLabToMod")
    {
      return transMdcLabToMod[s][m]->transTo(p);
    }
    
 if(!transMdcSecToMod[s][m])
    {
      transMdcSecToMod[s][m]=new HGeomTransform();
      if(!fGetCont->getSecTransMod(*(transMdcSecToMod[s][m]),s,m))
	{
	  return pDummy;
	}
    }    
    
    
  if(Option=="FromModToSec")
    {
      return transMdcSecToMod[s][m]->transFrom(p);
    }
  if(Option=="FromSecToMod")
    {
      return transMdcSecToMod[s][m]->transTo(p);
    }
    return pDummy;
}


HGeomTransform HAlignmentGRotations::MakeTransMatrix(Double_t phi, Double_t psi, Double_t theta, 
                                                     Double_t x, Double_t y, Double_t z)
{
   phi*=180./acos(-1.);
   psi*=180./acos(-1.);
   theta*=180./acos(-1.);
 
   HGeomRotation rot(phi, psi, theta);
   HGeomVector shift(x,y,z);
   HGeomTransform trans;
   trans.setRotMatrix(rot);
   trans.setTransVector(shift);
   return trans;
}

void HAlignmentGRotations::GetEulerAngles(HGeomTransform &trans, Double_t &phi, Double_t &psi, Double_t &theta)
{
  const HGeomRotation &rot=trans.getRotMatrix();
  psi=acos(rot(8));
  Float_t sinPsi=sin(psi);
  phi=atan2(rot(5)/sinPsi, rot(2)/sinPsi);
  theta=atan2(rot(7)/sinPsi,-1.*rot(6)/sinPsi);
}

void HAlignmentGRotations::GetTransVector(HGeomTransform &trans, Double_t &x, Double_t &y, Double_t &z)
{
  const HGeomVector &shift= trans.getTransVector();
  x=shift.getX();
  y=shift.getY();
  z=shift.getZ();
}

HGeomVector HAlignmentGRotations::TransMdc(HGeomVector &p, TString Option, Int_t s)
{
  HGeomVector pDummy(-10000.,-10000,-10000.);
  if(!transMdcLabToSec[s])
    {
      transMdcLabToSec[s]=new HGeomTransform();
      if(!fGetCont->getLabTransSec(*(transMdcLabToSec[s]),s))
	{
	   return pDummy;
	}
    }
  
  if(Option=="FromSecToLab")
    {
      
      return transMdcLabToSec[s]->transFrom(p);
    }
  if(Option=="FromLabToSec")
    {
      return transMdcLabToSec[s]->transTo(p);
    }
 return pDummy;    
}

