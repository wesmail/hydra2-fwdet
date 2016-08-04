//*-- Author : Anar Rustamov

#include "halignmentgbase.h"
#include "TClonesArray.h"
#include "halignmentggeom.h"
#include "halignmentgrotations.h"
#include "hgeomtransform.h"
#include "TH2F.h"
#include "TH1F.h"
#include "TMinuit.h"
#include "iostream"
#include "TMath.h"
#include "TFile.h"
#include "TCanvas.h"
#include "TNtuple.h"

using namespace std;

ClassImp(HAlignmentGBase)
  
  HAlignmentGBase::HAlignmentGBase()
{
  fAlignGeom=new HAlignmentGGeom();
  fAlignRot=new HAlignmentGRotations();
  fMinuit=NULL;
  cloneSize=0;
  histoNumbers =0;
  alignTracks = NULL;
}

HAlignmentGBase::~HAlignmentGBase()
{
  if(fAlignGeom)
    {
      delete fAlignGeom;
      fAlignGeom=NULL;
    }
  if(fAlignRot)
    {
      delete fAlignRot;
      fAlignRot=NULL;
    }
  
  if(fMinuit)
    {
      delete fMinuit;
      fMinuit=NULL;
    }
}

void HAlignmentGBase::GetPoints(void (*pFunc)(Float_t& , Float_t&, Float_t& , Float_t& , HGeomVector& , HGeomVector&),Float_t& r, Float_t& phi, Float_t& theta, Float_t& z, HGeomVector& p1, HGeomVector& p2)
{
  pFunc(r,phi, theta, z, p1,p2);
}


