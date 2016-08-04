//*-- Author : Anar Rustamov

#include "halignmentgshower.h"
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
ClassImp(HAlignmentGShower)
  HAlignmentGShower::HAlignmentGShower()
{
  fAlignGeom  = new HAlignmentGGeom();
  fAlignRot   = new HAlignmentGRotations();
  fMinuit     = NULL;////new TMinuit(6);
  alignMode   = 0;
}

void HAlignmentGShower::SetNtuple(TNtuple *_nt)
{
 nt = _nt;
 nt -> SetBranchAddress("x1s",&x1); // yuxari dubna 
 nt -> SetBranchAddress("y1s",&y1); // yuxari dubna
 nt -> SetBranchAddress("z1s",&z1);
 nt -> SetBranchAddress("x2s",&x2); // yuxari gsi 
 nt -> SetBranchAddress("y2s",&y2); // yuxari gsi
 nt -> SetBranchAddress("z2s",&z2);
 nt -> SetBranchAddress("xsh",&xsh); // ashagi gsi 
 nt -> SetBranchAddress("ysh",&ysh); // ashagi gsi
 nt -> SetBranchAddress("zsh",&zsh);
 nt -> SetBranchAddress("xshl",&xshl); // ashagi gsi
 nt -> SetBranchAddress("yshl",&yshl);
 nt -> SetBranchAddress("zshl",&zshl);




 nt -> SetBranchAddress("s",&sec);
}

HAlignmentGShower::~HAlignmentGShower()
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



void fcnShower(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  static int count = 1;
  count++;
  HAlignmentGShower       *myObject   = (HAlignmentGShower*)(gMinuit -> GetObjectFit());
  HAlignmentGRotations    *fAlignRot  = myObject -> GetAlignRot();
  HGeomTransform trans =  fAlignRot -> MakeTransMatrix(par[0], par[1], par[2], par[3], par[4], par[5]);
 // HGeomTransform transOldT = myObject -> getTransOld();
 // transOldT.transTo(trans);

  
  f =  myObject -> GetMinFunction(trans);
  if(count%100 == 0) 
  
  cout<<"iter:  "<<count<<"  function  "<<f<<" "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "
                      <<par[4]<<"  "<<par[5]<<" alignmode" <<myObject -> getAlignMode() << endl;
}


void HAlignmentGShower::AlignShower(Int_t sec)
{

  alignSec = sec;

  Int_t ierflg = 0;
  if(!gMinuit) fMinuit=new TMinuit(6);
  gMinuit->SetFCN(fcnShower);
  gMinuit->SetObjectFit(this);
  Double_t arglist[6];
  Double_t vstart[6]={0.};
  Double_t low[6],up[6];
  Double_t step[13];
  Double_t out[6];
  Double_t errOut[6];
  
  
  HGeomTransform transOldB     =  transOld;
  transOldB.transTo(*fAlignRot -> GetTransMdc(alignSec));
  
  fAlignRot -> GetEulerAngles (transOldB, vstart[0], vstart[1], vstart[2]);
  fAlignRot -> GetTransVector (transOldB, vstart[3], vstart[4], vstart[5]);
//  vstart[0] = acos(-1.)/180.;
//  vstart[1]=0.; 
//  vstart[2] = -1.*vstart[0];
 
  for(Int_t i=0; i<6; i++)
    {
       step[i] = 0.001;
       low[i]  = vstart[i] - .09;
       up[i]   = vstart[i] + .09;
    }
  low[3]  = vstart[3] - 55.;
  low[4]  = vstart[4] - 55.;
  low[5]  = vstart[5] - 55.;
  up[3]   = vstart[3] + 55.;
  up[4]   = vstart[4] + 55.;
  up[5]   = vstart[5] + 55.;
  step[3] = 1.;
  step[4] = 5.;
  step[5] = 5.;

  
  gMinuit->mnparm(0,"phi", vstart[0],step[0],low[0],up[0],ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],low[1],up[1],ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],low[2],up[2],ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], low[3],up[3],ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],low[4],up[4],ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], low[5],up[5],ierflg);
 
/*
  gMinuit->mnparm(0,"phi", vstart[0],step[0],0,0,ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],0,0,ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],0,0,ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], 0,0,ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],0,0,ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], 0,0,ierflg);
*/

 
  gMinuit->SetPrintLevel(-1);
  gMinuit->SetErrorDef(1);
  arglist[0]=2;
  gMinuit->mnexcm("SET STR",arglist,1,ierflg);
  arglist[1] = 0.01;
  arglist[0] = 20000;
  gMinuit->mnexcm("SIMPLEX",arglist,1,ierflg);
  
  for (Int_t ii = 0; ii < 6; ii++)
    {
      gMinuit->GetParameter(ii,out[ii],errOut[ii]);
    }
  //transNew = transOld;
  transNew = fAlignRot->MakeTransMatrix(out[0], out[1], out[2], out[3], out[4], out[5]);
  transNew.transFrom(*fAlignRot->GetTransMdc(alignSec));
 // transNew.transTo(transS);
}



Float_t HAlignmentGShower::GetMinFunction(HGeomTransform &trans)
{
  Float_t dist = 0;
  Float_t dist1;
  Int_t nentries = nt -> GetEntries();
  for(Int_t i = 0; i < nentries; i++)
    {
      nt->GetEntry(i);
      
      if(sec != alignSec) continue;
      
      point1.setXYZ(x1, y1, z1); //lab
      point2.setXYZ(x2, y2, z2);      
      
      if(alignMode == 0)
      {
        pointSh.setXYZ(xshl,yshl,zshl); 
        pointSh = trans.transFrom(pointSh);//lab
        dist1 = fAlignGeom -> CalcDistanceToLine(pointSh,point1,point2);
        if(dist1 > 150) continue;
      }     
      else
      { 
	pointSh.setXYZ(xsh,ysh,zsh);//lab
	point1  =   trans.transTo(point1); 
	point2  =   trans.transTo(point2);
	pointSh =   trans.transTo(pointSh);
	point1.setZ(point1.getZ() - zshl);
	point2.setZ(point2.getZ() - zshl);
	diffZ = point2.getZ() - point1.getZ();
	diffX = point2.getX() - point1.getX();
	diffY = point2.getY() - point1.getY();
	xCr   = (point1.getX()*diffZ - point1.getZ()*diffX)/diffZ;
	yCr   = (point1.getY()*diffZ - point1.getZ()*diffY)/diffZ;
	dist1 = sqrt((xCr - pointSh.getX())*(xCr - pointSh.getX()) + 
		     (yCr - pointSh.getY())*(yCr - pointSh.getY()));
	if(dist1 > 180) continue; 
      }       
      
      dist += dist1;
    }
  return dist;
}

  void HAlignmentGShower::CheckAlignment(HGeomTransform transNew, HGeomTransform transOld, TFile *f)
  {
   
     transNew.transTo(*fAlignRot->GetTransMdc(alignSec));
     transOld.transTo(*fAlignRot->GetTransMdc(alignSec));
    
  
    Char_t nnn[256];
    
    sprintf(nnn,"%s%i%s","align",alignSec,".root");
    TString nn = nnn;
    
    
    
    if(!f) f = new TFile(nn.Data(),"recreate");
  
    TNtuple *chnt = new TNtuple("chnt","chnt","xold:yold:xnew:ynew:xcrold:ycrold:xcrnew:ycrnew:sec");  
    
    Int_t nentries = nt -> GetEntries();
    for(Int_t i = 0; i < nentries; i++)
    {
      nt->GetEntry(i);
      
      if(sec != alignSec) continue;
      
      point1.setXYZ(x1, y1, z1); 
      point2.setXYZ(x2, y2, z2);
      
      ////////////////////
      pointSh.setXYZ(xshl,yshl,zshl); 
      //pointSh = trans.transFrom(pointSh);//lab
      //pointSh.setXYZ(xsh,ysh,zsh);
      Float_t coordOld[4];
      Float_t coordNew[4];
      
      getPoints(point1, point2, pointSh, transOld, coordOld);
      getPoints(point1, point2, pointSh, transNew, coordNew);
      chnt -> Fill(coordOld[0], coordOld[1], coordNew[0], coordNew[1], coordOld[2], coordOld[3], 
                  coordNew[2], coordNew[3],sec);
     }
     f->cd();
     chnt -> Write();
   }
   
  void HAlignmentGShower::getPoints(HGeomVector &p1, HGeomVector &p2, HGeomVector &psh, 
                                    HGeomTransform &trans, Float_t *coord)
  {
    
      HGeomVector pointSh;
      pointSh = trans.transFrom(psh);
      HGeomVector testCr = fAlignGeom -> CalcIntersection(p1,p2,trans);
       
//      point1  =   trans.transTo(p1);
//      point2  =   trans.transTo(p2);
//      pointSh =   trans.transTo(psh);

//      diffZ = point2.getZ() - point1.getZ();
//      diffX = point2.getX() - point1.getX();
//      diffY = point2.getY() - point1.getY();
//      xCr   = (point1.getX()*diffZ - point1.getZ()*diffX)/diffZ;
//      yCr   = (point1.getY()*diffZ - point1.getZ()*diffY)/diffZ;
      
      coord[0] = pointSh.getX();
      coord[1] = pointSh.getY();
      coord[2] = testCr.getX();
      coord[3] = testCr.getY();

        
  
  }
        
