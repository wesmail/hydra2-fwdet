//*-- Author : Anar Rustamov

#include "halignmentgnomag.h"
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

using namespace std;

ClassImp(HAlignmentGNoMag)
  
  
  HAlignmentGNoMag::HAlignmentGNoMag()
{
  alignTracks = new TClonesArray("HAlignmentGParticle",5000000);
  ResetCounts();
  isAligned=kFALSE;
  histoDiffYBeforeMod3=NULL;
  histoDiffYAfterMod3=NULL;
  histoDiffYBeforeVsPhiMod3=NULL;
  histoDiffYAfterVsPhiMod3=NULL;
  histoDiffYBeforeMod4=NULL;
  histoDiffYAfterMod4=NULL;
  histoDiffYBeforeVsPhiMod4=NULL;
  histoDiffYAfterVsPhiMod4=NULL;
  fitVersion = 0;
  
  AcceptRadius=0.;
  AcceptRadiusFour=0.;
  AcceptRadiusTarg=0.;
  AcceptDir=0.;
  AcceptDirFour=0.;
  
  isThirdChamber = kTRUE;
  isFouthChamber = kTRUE;

   
  
  for(Int_t ss=0; ss<6; ss++)
  for(Int_t mm=0; mm<4; mm++)
   mdcSetup[ss][mm]=-1;
}

void fcnMod(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  static int count = 1;
  count++;

  HAlignmentGNoMag *myObject=(HAlignmentGNoMag*)(gMinuit->GetObjectFit());
  HAlignmentGParticle *p;
  Float_t F=0;
  Float_t FF=0;
  
  HGeomTransform trans = myObject -> GetAlignRot() -> MakeTransMatrix(par[0], par[1], par[2], par[3], par[4], par[5]);
  HGeomVector inter;
  HGeomVector  diff;
  Int_t usedMod=myObject->GetUsedMod();
  Int_t usedSec=myObject->GetUsedSec();
  Int_t fitVersion = myObject->GetFitVersion();
  for(Int_t i=0;  i< myObject -> GetCloneSize(); i++)
    {
      p=(HAlignmentGParticle*)(myObject->GetAlignTracks())->At(i);
      //if(!p->GetIsGood()) continue;
      HGeomVector point1 = p->GetPoint(0); //in Mod
      HGeomVector point2 = p->GetPoint(1); //in Mod
      HGeomVector point3 = p->GetPoint(2); //in Mod
      HGeomVector point4 = p->GetPoint(3); //in Mod
      point1 = myObject -> GetAlignRot() -> TransMdc(point1,"FromModToSec",usedSec,0);
      point2 = myObject -> GetAlignRot() -> TransMdc(point2,"FromModToSec",usedSec,1);
      if(usedMod==2)
	point3= trans.transFrom(point3);
      if(usedMod == 3)
	point3= trans.transFrom(point4);
      inter=myObject -> GetAlignGeom() -> CalcIntersection(point1, point2, trans);
      diff=(inter-point3);
      F  +=  diff.getX()*diff.getX()/4. + diff.getY()*diff.getY() +  diff.getZ()*diff.getZ();
      FF += diff.getX()*diff.getX()     + diff.getY()*diff.getY() +  diff.getZ()*diff.getZ();
    }
    
      

  if(fitVersion == 0) 
  {     
  f = F;
  if( count%100 ==0 ) cout<<count<<"  the function value is  "<<f<<"  "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<endl;
  }
  else
  {
  f = sqrt(FF);
  if( count%100 ==0 ) cout<<count<<"  the function value is  "<<f<<"  "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<endl;
  }
}

void fcnMod11(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  static int count = 1;
  count++;
  
  
  HAlignmentGNoMag *myObject=(HAlignmentGNoMag*)(gMinuit->GetObjectFit());
  HAlignmentGParticle *p;
  Float_t errX = 0.450;
  Float_t errY = 0.140;
  Float_t F = 0;
  HGeomTransform trans = myObject -> GetAlignRot() -> MakeTransMatrix(par[0], par[1], par[2], par[3], par[4], par[5]);
  HGeomVector inter;
  HGeomVector  diff;
  Int_t usedMod=myObject->GetUsedMod();
  Int_t usedSec=myObject->GetUsedSec();
  HGeomTransform transDub = *myObject -> GetAlignRot()->GetTransMdc(usedSec, 1);
  transDub.transTo(*myObject -> GetAlignRot()->GetTransMdc(usedSec));
  for(Int_t i=0;  i< myObject -> GetCloneSize(); i++)
    {
      p = (HAlignmentGParticle*)(myObject->GetAlignTracks())->At(i);
      //if(!p->GetIsGood()) continue;
      HGeomVector point1 = p->GetPoint(0); //in Mod
      HGeomVector point2 = p->GetPoint(1); //in Mod
      HGeomVector point3 = p->GetPoint(2); //in Mod
      HGeomVector point4 = p->GetPoint(3); //in Mod
      point1= myObject -> GetAlignRot() -> TransMdc(point1,"FromModToSec",usedSec,0);
      //point2= myObject -> GetAlignRot() -> TransMdc(point2,"FromModToSec",usedSec,1);
      if(usedMod == 2)
	point3 = trans.transFrom(point3); // sector
      if(usedMod == 3)
	point3= trans.transFrom(point4); // sector
      //inter=myObject -> GetAlignGeom() -> CalcIntersection(point1, point2, trans);
      inter=myObject -> GetAlignGeom() -> CalcIntersection(point1, point3, transDub); //transDub---Sector syst
      inter=transDub.transTo(inter);
      //inter = myObject -> GetAlignRot() -> TransMdc(inter,"FromModToSec",usedSec,0);
      diff = (inter-point2);
      F += diff.getX()*diff.getX()/errX/errX+diff.getY()*diff.getY()/errY/errY;////+diff.getZ()*diff.getZ();
    }
  //f=sqrt(F);
  f=F;
  if( count%100 ==0 ) cout<<count<<"  the function value is  "<<f<<"  "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<endl;
  
}

HGeomTransform HAlignmentGNoMag::AlignMod()
{
  Int_t ierflg = 0;
  if(!gMinuit)
  fMinuit=new TMinuit(6);
  gMinuit->SetFCN(fcnMod11);
  gMinuit->SetObjectFit(this);
  Double_t arglist[6];
  Double_t vstart[6],low[6],up[6];
  Double_t step[13];
  Double_t out[6];
  Double_t errOut[6];
  HGeomTransform fromModToSec = transBefore[UsedMod];
  fromModToSec.transTo(*fAlignRot->GetTransMdc(UsedSec));
  fAlignRot -> GetEulerAngles (fromModToSec, vstart[0], vstart[1], vstart[2]);
  fAlignRot -> GetTransVector (fromModToSec, vstart[3], vstart[4], vstart[5]);
  for(Int_t i=0; i<6; i++)
    {
      //step[i] = vstart[i] / 1000.;
      step[i] = 0.01;
      low[i]  = vstart[i] - .9;
      up[i]   = vstart[i] + .9;
    }
   low[3]  = vstart[3] - 10.;
   low[4]  = vstart[4] - 10.;
   low[5]  = vstart[5] - 10.;
   up[3]   = vstart[3] + 10.;
   up[4]   = vstart[4] + 10.;
   up[5]   = vstart[5] + 10.;
  
   step[3] = 0.05;
   step[4] = 0.5;
   step[5] = 0.5;
  
  gMinuit->mnparm(0, "phi", vstart[0],step[0],low[0],up[0],ierflg);
  gMinuit->mnparm(1, "psi",vstart[1], step[1],low[1],up[1],ierflg);
  gMinuit->mnparm(2, "theta", vstart[2], step[2],low[2],up[2],ierflg);
  gMinuit->mnparm(3, "x", vstart[3], step[3], low[3],up[3],ierflg);
  gMinuit->mnparm(4, "y",vstart[4], step[4],low[4],up[4],ierflg);
  gMinuit->mnparm(5, "z", vstart[5], step[5], low[5],up[5],ierflg);
  
/*
    gMinuit->FixParameter(0);
    gMinuit->FixParameter(1);
    gMinuit->FixParameter(2);


    gMinuit->FixParameter(3);
    gMinuit->FixParameter(4);
    gMinuit->FixParameter(5);
*/
  
   gMinuit->SetPrintLevel(-1);
  //     arglist[0] =1;
  //     gMinuit->mnexcm("SET NOW", arglist,1,ierflg);
  gMinuit->SetErrorDef(0.5);
//  gMinuit->SetErrorDef(1);
  arglist[0]=2;
  gMinuit->mnexcm("SET STR", arglist,1,ierflg);
  arglist[0] = 1;
  gMinuit->mnexcm("SET ERR", arglist ,1,ierflg);
  arglist[1] = 0.001;
  arglist[0] = 100000;
  gMinuit->mnexcm("SIMPLEX",arglist,1,ierflg);
 // gMinuit->mnexcm("MINIMIZE",arglist,1,ierflg);
  ////////gMinuit->mnexcm("MINOS",arglist,1,ierflg);
  for (Int_t ii=0; ii<6; ii++)
    {
      gMinuit->GetParameter(ii,out[ii],errOut[ii]);
    }
  transAfter[UsedMod]=fAlignRot->MakeTransMatrix(out[0], out[1], out[2], out[3], out[4], out[5]);
  transAfter[UsedMod].transFrom(*fAlignRot->GetTransMdc(UsedSec));
  return transAfter[UsedMod];
}

HAlignmentGNoMag::~HAlignmentGNoMag()
{
  if(alignTracks)
    {
      alignTracks -> Delete();
      delete alignTracks;
    }
}

void HAlignmentGNoMag::Init(TString _inName, TString _outName, Int_t _numberOfTracks, Bool_t _isAligned)
{
  numberOfTracks=_numberOfTracks;
  isAligned=_isAligned;
  isAligned=kFALSE;
  histoVertex = new TH1F("histoVertex","histoVertex",100,-100,10);
  if(histoDiffYBeforeMod3)
    delete histoDiffYBeforeMod3;
  histoDiffYBeforeMod3 = new TH1F("histoDiffYBeforeMod3","histoDiffYBeforeMod3",100,-10,10);
  if(histoDiffYAfterMod3)
    delete histoDiffYAfterMod3;
  histoDiffYAfterMod3  = new TH1F("histoDiffYAfterMod3", "histoDiffYAfterMod3",100,-10,10);
  if(histoDiffYBeforeMod4)
    delete histoDiffYBeforeMod4;
  histoDiffYBeforeMod4 = new TH1F("histoDiffYBeforeMod4","histoDiffYBeforeMod4",100,-10,10);
  if(histoDiffYAfterMod4)
    delete histoDiffYAfterMod4;
  histoDiffYAfterMod4  = new TH1F("histoDiffYAfterMod4", "histoDiffYAfterMod4",100,-10,10);
  if(histoDiffYBeforeVsPhiMod3)
    delete histoDiffYBeforeVsPhiMod3;
  histoDiffYBeforeVsPhiMod3 = new TH2F("histoDiffYBeforeVsPhiMod3","histoDiffYBeforeVsPhiMod3",100,60,120,100,-10,10);
  if(histoDiffYAfterVsPhiMod3)
    delete histoDiffYAfterVsPhiMod3;
  histoDiffYAfterVsPhiMod3  = new TH2F("histoDiffYAfterVsPhiMod3", "histoDiffYAfterVsPhiMod3",100,60,120,100,-10,10);
  if(histoDiffYBeforeVsThetaMod3)
    delete histoDiffYBeforeVsThetaMod3;
  histoDiffYBeforeVsThetaMod3 = new TH2F("histoDiffYBeforeVsThetaMod3","histoDiffYBeforeVsThetaMod3",100,10,90,100,-10,10);
  if(histoDiffYAfterVsThetaMod3)
    delete histoDiffYAfterVsThetaMod3;
  histoDiffYAfterVsThetaMod3  = new TH2F("histoDiffYAfterVsThetaMod3", "histoDiffYAfterVsThetaMod3",100,10,90,100,-10,10);
  if(histoDiffYBeforeVsPhiMod4)
    delete histoDiffYBeforeVsPhiMod4;
  histoDiffYBeforeVsPhiMod4 = new TH2F("histoDiffYBeforeVsPhiMod4","histoDiffYBeforeVsPhiMod4",100,60,120,100,-10,10);
  if(histoDiffYAfterVsPhiMod4)
    delete histoDiffYAfterVsPhiMod4;
  histoDiffYAfterVsPhiMod4  = new TH2F("histoDiffYAfterVsPhiMod4", "histoDiffYAfterVsPhiMod4",100,60,120,100,-10,10);
  if(histoDiffYBeforeVsThetaMod4)
    delete histoDiffYBeforeVsThetaMod4;
  histoDiffYBeforeVsThetaMod4 = new TH2F("histoDiffYBeforeVsThetaMod4","histoDiffYBeforeVsThetaMod4",100,10,90,100,-10,10);
  if(histoDiffYAfterVsThetaMod4)
    delete histoDiffYAfterVsThetaMod4;
    histoDiffYAfterVsThetaMod4  = new TH2F("histoDiffYAfterVsThetaMod4", "histoDiffYAfterVsThetaMod4",100,10,90,100,-10,10);
    histoTheta=new TH1F("histoTheta", "histoTheta", 80,10,90);
    histoTheta->GetXaxis()->SetTitle("#theta [^{o}]");
    
    //RasterPlotBefore= new TH2F("RasterPlotBefore", "RasterPlotBefore",100,-500,500,100,100,1000);
    //RasterPlotAfter= new TH2F("RasterPlotAfter", "RasterPlotAfter",100,-500,500,100,100,1000);
    
    if(RasterPlotBefore3) delete RasterPlotBefore3;
      RasterPlotBefore3= new TH2F("RasterPlotBefore3", "RasterPlotBefore3",200,-420,420,200,220,900);
    if(RasterPlotBefore4) delete RasterPlotBefore4;
      RasterPlotBefore4 = new TH2F("RasterPlotBefore4", "RasterPlotBefore4",200,-420,420,200,220,900);

    if(RasterPlotAfter3) delete RasterPlotAfter3;
      RasterPlotAfter3= new TH2F("RasterPlotAfter3", "RasterPlotAfter3",200,-420,420,200,220,900);
    if(RasterPlotAfter4) delete RasterPlotAfter4;
      RasterPlotAfter4 = new TH2F("RasterPlotAfter4", "RasterPlotAfter4",200,-420,420,200,220,900);


    //histoVertex = new TH1F("histoVertex","histoVertex",100,-100,20);
    
    //declare histograms and so on
    SetIname(_inName);
    outName=_outName;
    cout<<"We are reading the file  "<<_inName<<endl;
    ////in.open(inName);
}

void HAlignmentGNoMag::CollectTracks()
{
    cout<<"numEvents==  "<<numberOfTracks<<endl;
    Int_t counter, counter1;
    counter=0;
    cout<<"Start hist distribution"<<endl;
    /////while(!in.eof())
    //for(Int_t f=0; f<936634; f++)
    for(Int_t f=0; f<numberOfTracks; f++)
      {
        if(counter%10000==0)
	{
	  cout<<"the Event Number is  "<<counter<<" sector ===  "<<sector<<"  module==  "<<UsedMod<<"  evt==  "<<FirstEvent<<endl;
	  if(counter!=0 && FirstEvent == 0) {cout<<"end is reached stop everything "<<endl;break;}
	}

	if(counter==0)
	  {
            in>>nEvent>>sector>>mod>>X>>Y>>Xdir>>Ydir;
            FirstEvent=nEvent;
            FirstMod=mod;
            FirstX=X;
            FirstXdir=Xdir;
            FirstY=Y;
            FirstYdir=Ydir;
	  }
        counter1=0;
        for(Int_t m=0; m<50; m++)
	  {
            in>>nEvent>>sector>>mod>>X>>Y>>Xdir>>Ydir;
            if(nEvent==FirstEvent)
	      {
                if(counter1==0)
		  {
                    AddData(FirstX,FirstXdir,FirstY, FirstYdir, FirstMod);
		  }
                AddData(X,Xdir,Y,Ydir,mod);
                counter1++;
	      }
            else
            {
	      FirstEvent=nEvent;
	      FirstMod=mod;
	      FirstX=X;
	      FirstXdir=Xdir;
	      FirstY=Y;
	      FirstYdir=Ydir;
	      break;
            }
	  }
        AddToClones();
	
        counter++;
      }
    in.close();
    in.clear();
}

void HAlignmentGNoMag::AddData(Float_t x,Float_t xd,Float_t y,Float_t yd,Int_t m)
{
  XValue[m][nCount[m]]=x;
  XdirValue[m][nCount[m]]=xd;
  YValue[m][nCount[m]]=y;
  YdirValue[m][nCount[m]]=yd;
  nCount[m]++;
}


/*
Bool_t HAlignmentGNoMag::AddToClones()
{
  for(Int_t m=0; m<4; m++)
    {
      if(nCount[m]==0)
        {
	    ResetCounts();
            return kFALSE;
        }
    }
*/    
Bool_t HAlignmentGNoMag::AddToClones()
{


 if(nCount[0] == 0 || nCount[1] == 0 )
 {
            ResetCounts();
            return kFALSE;
 } 
 
 if(nCount[2] == 0 && nCount[3] == 0 )
 {
            ResetCounts();
            return kFALSE;
 } 
 
 

  if(nCount[2] == 0 && nCount[3]!= 0)
  {
    setDefault(XValue[2][0], XdirValue[2][0], YValue[2][0], YdirValue[2][0]);
    //setDefault(XValue[2][1], XdirValue[2][1], YValue[2][1], YdirValue[2][1]);
    
    nCount[2] = 1;
    //cout<<"No Modele 2"<<endl;
  }
  
  if(nCount[3] == 0 && nCount[2] != 0 )
  {
    setDefault(XValue[3][0], XdirValue[3][0], YValue[3][0], YdirValue[3][0]);
    nCount[3] = 1;
  }

//   cout<<nCount[0]<<"  "<<nCount[1]<<"  "<<nCount[2]<<"  "<<nCount[3]<<endl;

  for(Int_t i=0; i<nCount[0]; i++)
    for(Int_t j=0; j<nCount[1]; j++)
      for(Int_t k=0; k<nCount[2];k++)
	for(Int_t l=0; l<nCount[3]; l++)
	  {
	    
	    
	    fAlignParticle.SetX(       XValue[0][i],    XValue[1][j],    XValue[2][k],    XValue[3][l] );
	    fAlignParticle.SetXdir( XdirValue[0][i], XdirValue[1][j], XdirValue[2][k], XdirValue[3][l] );
	    fAlignParticle.SetY(       YValue[0][i],    YValue[1][j],    YValue[2][k],    YValue[3][l] );
	    fAlignParticle.SetYdir( YdirValue[0][i], YdirValue[1][j], YdirValue[2][k], YdirValue[3][l] );
	    fAlignParticle.SetPoints();
	    
	    if(Selected(fAlignParticle))
	      {
		if(cloneSize>=2000000)
		  {
		    cout<<"Increase clone size"<<endl;
		    ResetCounts();
		    return kFALSE;
		  }
		testP = new ((*alignTracks)[cloneSize++]) HAlignmentGParticle(fAlignParticle);
		
	      }
	    
	  }
  ResetCounts();
  return kTRUE;
}

void HAlignmentGNoMag::ResetCounts()
{
  
  for(Int_t i=0; i<4; i++)
    {
      nCount[i]=0;
    }
  
}

Bool_t HAlignmentGNoMag::Selected(HAlignmentGParticle &p)
{
  //return kTRUE;

  //if(isThirdChamber==kFALSE) {cout<<"track is added to clonde"<<endl;return kTRUE;}
 

  HGeomVector point1=p.GetPoint(0);
  HGeomVector dir1=  p.GetDir(0);
  HGeomVector point2=p.GetPoint(1);
  HGeomVector dir2=  p.GetDir(1);
  HGeomVector point3=p.GetPoint(2);
  HGeomVector dir3=  p.GetDir(2);
  HGeomVector point4=p.GetPoint(3);
  HGeomVector dir4=  p.GetDir(3);
  
  // cout<<"before "<<point1.getZ()<<endl;
  point1=fAlignRot->TransMdc(point1,"FromModToLab",UsedSec,0);
  point2=fAlignRot->TransMdc(point2,"FromModToLab",UsedSec,1);
  // point3=fAlignRot->TransMdc(point3,"FromModToLab",0,2);
  // point4=fAlignRot->TransMdc(point4,"FromModToLab",0,3);
  dir1=fAlignRot->TransMdc(dir1,"FromModToLab",UsedSec,0)-(fAlignRot->GetTransMdc(UsedSec,0)->getTransVector());
  dir2=fAlignRot->TransMdc(dir2,"FromModToLab",UsedSec,1)-(fAlignRot->GetTransMdc(UsedSec,1)->getTransVector());
  // dir3=fAlignRot->TransMdc(dir3,"FromModToLab",0,2)-(fAlignRot->GetTransMdc(0,2)->getTransVector());
    // dir4=fAlignRot->TransMdc(dir4,"FromModToLab",0,3)-(fAlignRot->GetTransMdc(0,3)->getTransVector());
  
    //if(UsedMod==2)
    // {

     

    if(isThirdChamber)
    {
    point3 = transBefore[2].transFrom(point3);
    dir3   =   transBefore[2].transFrom(dir3)-transBefore[2].getTransVector();
    }
    //}

    //if(UsedMod==3)
    // {
    if(isFouthChamber)
    {
    point4 = transBefore[3].transFrom(point4);
    dir4   = transBefore[3].transFrom(dir4)-transBefore[3].getTransVector();
    }
    //}

    //cout<<point1.getZ()<<endl;

    HGeomVector NullPoint(0.,0.,0);
    HGeomVector NullPointDir(0.,0.,1.);

    if(isThirdChamber){
    if(fAlignGeom->CalcDistanceToLine(point2,point1,point3)    > AcceptRadius    )
        return kFALSE;
	}
    
    if(isFouthChamber){
    if(fAlignGeom->CalcDistanceToLine(point2,point1,point4)    > AcceptRadiusFour)
        return kFALSE;
	}
    
    if(isThirdChamber){
    if(fAlignGeom->CalcDistanceToLine(NullPoint,point1,point3) > AcceptRadiusTarg)
        return kFALSE;
	}
    
    if(isThirdChamber){
    if(fAlignGeom->CalcVectorDistance(dir1,point1,point3)      > AcceptDir       )
        return kFALSE;
	}
    
    if(isThirdChamber){
    if(fAlignGeom->CalcVectorDistance(dir2,point1,point3)      > AcceptDir       )
        return kFALSE;
	}
    
    if(fAlignGeom->CalcVectorDistance(dir2,point1,point2)      > AcceptDir       )
        return kFALSE;
    
    if(isFouthChamber){
    if(fAlignGeom->CalcVectorDistance(dir4,point1,point2)      > AcceptDirFour   )
        return kFALSE;
	}

    HGeomVector zCoord = fAlignGeom->CalcVertex(point1,point2-point1,NullPoint,NullPointDir);


    Float_t theta = acos((point2.getZ()-point1.getZ())/(point2-point1).length())*180./acos(-1.);

    
    

    //if(zCoord.getZ()<-60) return kFALSE;
    //if(theta < 20) return kFALSE;
    
    if(zCoord.getZ()<-65 || zCoord.getZ()>-10 ) return kFALSE;
    if(theta < 18) return kFALSE;
    
    
    histoTheta->Fill(theta);
    histoVertex->Fill(zCoord.getZ());

    return kTRUE;
}

void HAlignmentGNoMag::FillHistograms()
{
    cout<<"Number of tracks in clone"<<endl;
    cout<<cloneSize<<endl;

    HAlignmentGParticle *p=NULL;
    for(Int_t i=0;  i<cloneSize; i++)
    {

        p=(HAlignmentGParticle*)alignTracks->At(i);
        HGeomVector point1=p->GetPoint(0);
        HGeomVector point2=p->GetPoint(1);
        HGeomVector point3=p->GetPoint(2);



        point1=fAlignRot->TransMdc(point1,"FromModToLab",0,0);
	
        //point3=fAlignRot->TransMdc(point3,"FromModToLab",0,2);


        HGeomVector inter=fAlignGeom->CalcIntersection(point1, point3, *fAlignRot->GetTransMdc(0,1));
        inter=fAlignRot->TransMdc(inter,"FromLabToMod",0,1);

        HGeomVector NullPoint(0.,0.,0);
        HGeomVector NullPointDir(0.,0.,1.);


        HGeomVector Vertex=fAlignGeom->CalcVertex(point1,point2,NullPoint,NullPointDir);

        //histoVertex->Fill(Vertex.getZ());
        //histoDiffYBefore->Fill((point2-inter).getY());

    }
}


void HAlignmentGNoMag::JustPlot(Int_t sec, Int_t mod)
{
    ResetCounts();

    UsedMod=mod;
    UsedSec=sec;
    
    if(!isAligned)
    {
     if(isThirdChamber)
    transBefore[2] = *fAlignRot->GetTransMdc(UsedSec,2);
    }
    if(isFouthChamber)
    transBefore[3] = *fAlignRot->GetTransMdc(UsedSec,3);
    
    alignTracks->Clear();
    in.clear();
    in.open(inName);
    cloneSize=0;
    CollectTracks();

    cout<<"clone size == "<<cloneSize<<endl;

    HAlignmentGParticle *p=NULL;
    for(Int_t i=0;  i<cloneSize; i++)
    {
        p=(HAlignmentGParticle*)alignTracks->At(i);
        HGeomVector point1=p->GetPoint(0);
        HGeomVector point2=p->GetPoint(1);
        point1=fAlignRot->TransMdc(point1,"FromModToLab",UsedSec,0);
        point2=fAlignRot->TransMdc(point2,"FromModToLab",UsedSec,1);
        //Float_t theta=acos((point2.getZ()-point1.getZ())/(point2-point1).length())*180./acos(-1.);
        //histoTheta->Fill(theta);
    }
    alignTracks->Clear();
    ff=new TFile(outName+".root","recreate");
    histoTheta->Write();
}

HGeomTransform HAlignmentGNoMag::Align(const Int_t &sec,const Int_t &mod )
{
   if(fMinuit) delete fMinuit;
   fMinuit=new TMinuit(6);
      


    UsedMod = mod;
    UsedSec = sec;

    isThirdChamber = kTRUE;
    isFouthChamber = kTRUE;
    
    cout<<"M D C    S E T U P !!!!!"<<endl;
    for(Int_t ss = 0; ss < 6; ss++)
    {
      for(Int_t mm = 0; mm < 4; mm++)
      {
       cout<< mdcSetup[ss][mm] <<" ";
      }
      cout<<endl;
    }
    
    //for(Int_t ss = 0; ss < 6; ss++)
    //{
        if(!isMdc(sec,2)) 
	{
	cout<<"in this sector 3rd chamber does not exist"<<endl;
	isThirdChamber = kFALSE;
	}
	else {isThirdChamber = kTRUE;}
        
	if(!isMdc(sec,3))
	{ 
	cout<<"in this sector 4th chamber does not exist"<<endl;
	isFouthChamber = kFALSE;
	}
	else {isFouthChamber = kTRUE;}
//    }  
    
    //if(UsedSec ==2){isThirdChamber = kFALSE;}
    ////if(UsedSec == 5) {isFouthChamber = kFALSE;}

    AcceptRadius = 4.;
    AcceptRadiusFour = 4.;
    AcceptRadiusTarg = 35000000.;
    AcceptDir = 0.3;
    AcceptDirFour = 0.3;
    fitVersion = 0;


    ResetCounts();
    cout<<"THE ALINMENT OF SECTOR   "<< sec <<"  MODULE  "<<mod<<"  STARTED"<<endl;
    cout<<"The fit version is "<<fitVersion<<"  radius is == "<<AcceptRadius<<endl;
    
    if(!isAligned)
    {
     cout<<"get trans"<<endl;
    
     if(isThirdChamber)
     transBefore[2] = *fAlignRot->GetTransMdc(UsedSec,2);
    }
    cout<<"got trans"<<endl;
    
    if(isFouthChamber)
    transBefore[3] = *fAlignRot->GetTransMdc(UsedSec,3);
    
    
    
    alignTracks->Clear();
    in.clear();
//    in.close();
    in.open(inName);
    cloneSize=0;
    CollectTracks();
    cout<<"clone size= "<<cloneSize<<endl;
    ResetRaster();    
    transStarting[UsedMod]=transBefore[UsedMod];
    transAfter[UsedMod]=AlignMod();
    transBefore[UsedMod]=transAfter[UsedMod];
    isAligned=kTRUE;
    in.close();
/*    
    /////////////////////////////////////////////////////////
    cout<<"Second iteration"<<endl;
    if(fMinuit) delete fMinuit;
    fMinuit=new TMinuit(6);
    AcceptRadius = 4.;
    AcceptRadiusFour = 4.;
    AcceptRadiusTarg = 3500000.;
    AcceptDir = 0.3;
    AcceptDirFour = 0.3;
    fitVersion = 0;
    cout<<"The fit version is "<<fitVersion<<"  radius is == "<<AcceptRadius<<endl;
    alignTracks->Clear();
    in.clear();
    in.open(inName);
    cloneSize = 0;
    CollectTracks();
    cout<<"clone size= "<<cloneSize<<endl;
    transAfter[UsedMod]=AlignMod();
    transBefore[UsedMod]=transAfter[UsedMod];
*/
    /////////////////////////////////////////////////////////
    /*
      cout<<"Third iteration"<<endl;
      alignTracks->Clear();
      in.clear();
      in.open(inName);
     
      cloneSize=0;
      CollectTracks();
      cout<<"clone size= "<<cloneSize<<endl;
      ///////Raster();
      transAfter[UsedMod]=AlignMod();
    /////////////////////////////////////////////////////////  
    */

    cout<<"before"<<endl;
    transStarting[mod].print();
    cout<<"after"<<endl;
    transAfter[mod].print();

    return transAfter[UsedMod];
}

void HAlignmentGNoMag::CheckAlignment()
{
    HAlignmentGParticle *p=NULL;
    HGeomVector point3Before, point3After;
    HGeomVector point4Before, point4After;

    HGeomVector interBefore3, interAfter3;
    HGeomVector interBefore4, interAfter4;

    HGeomVector point2Lab;
    HGeomVector point2Sec;
    for(Int_t i=0;  i<cloneSize; i++)
    {

        p=(HAlignmentGParticle*)alignTracks->At(i);
        HGeomVector point1=p->GetPoint(0);
        HGeomVector point2=p->GetPoint(1);
        HGeomVector point3=p->GetPoint(2);
        HGeomVector point4=p->GetPoint(3);
	
	if(p -> GetXdir(2) == -1000 && UsedMod == 2 )
	{ 
	cout<<"Something is Wrong from module 2"<<endl;
	cout<<"Something is Wrong!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	cout<<"Something is Wrong from module 2"<<endl;
	cout<<"Something is Wrong from module 2!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	}
	
	
	if(p -> GetXdir(3) == -1000 && UsedMod == 3 )
	{ 
	cout<<"Something is Wrong from module 3"<<endl;
	cout<<"Something is Wrong from module 3   !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	cout<<"Something is Wrong from module 3"<<endl;
	cout<<"Something is Wrong from module 3!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"<<endl;
	}
	
	
	

        point1=fAlignRot->TransMdc(point1,"FromModToLab",UsedSec,0);
        point2Lab=fAlignRot->TransMdc(point2,"FromModToLab",UsedSec,1);

        point2Sec=fAlignRot->TransMdc(point2,"FromModToSec",UsedSec,1);

        if(isThirdChamber){
        point3Before = transStarting[2].transFrom(point3);
        point3After  = transAfter[2].transFrom(point3);
           }
	else {point3Before.setXYZ(-100,-1000,-1000);
	      point3After.setXYZ(-100,-1000,-1000);}   
	 
	 if(isFouthChamber){ 
        point4Before = transStarting[3].transFrom(point4);
        point4After  = transAfter[3].transFrom(point4);
          }
	  else
	  {
	  point4Before.setXYZ(-100,-1000,-1000);
	  point4After.setXYZ(-100,-1000,-1000);
	  }

        interBefore3=fAlignGeom->CalcIntersection(point1, point3Before, *fAlignRot->GetTransMdc(UsedSec,1));
        interAfter3=fAlignGeom->CalcIntersection(point1, point3After, *fAlignRot->GetTransMdc(UsedSec,1));
           
        interBefore4=fAlignGeom->CalcIntersection(point1, point4Before, *fAlignRot->GetTransMdc(UsedSec,1));
        interAfter4=fAlignGeom->CalcIntersection(point1, point4After, *fAlignRot->GetTransMdc(UsedSec,1));

        interBefore3=fAlignRot->TransMdc(interBefore3,"FromLabToMod",UsedSec,1);
        interAfter3=fAlignRot->TransMdc(interAfter3,"FromLabToMod",UsedSec,1);

        interBefore4=fAlignRot->TransMdc(interBefore4,"FromLabToMod",UsedSec,1);
        interAfter4=fAlignRot->TransMdc(interAfter4,"FromLabToMod",UsedSec,1);

        histoDiffYBeforeMod3->Fill(point2.getY()-interBefore3.getY());
        histoDiffYAfterMod3->Fill(point2.getY()-interAfter3.getY());

         
        histoDiffYBeforeMod4->Fill(point2.getY()-interBefore4.getY());
        histoDiffYAfterMod4->Fill(point2.getY()-interAfter4.getY());


        Float_t phi=atan2(point2Lab.getY()-point1.getY(), point2Lab.getX()-point1.getX())*180./acos(-1.);

        Float_t theta=acos((point2Lab.getZ()-point1.getZ())/(point2Lab-point1).length())*180./acos(-1.);

        //////histoTheta->Fill(theta);


        if(UsedSec!=5)
        {
            if(phi<0)
                phi+=360;
            histoDiffYBeforeVsPhiMod3->Fill(phi-60.*UsedSec, point2.getY()-interBefore3.getY());
            histoDiffYAfterVsPhiMod3->Fill( phi-60.*UsedSec, point2.getY()-interAfter3.getY());

            histoDiffYBeforeVsPhiMod4->Fill(phi-60.*UsedSec, point2.getY()-interBefore4.getY());
            histoDiffYAfterVsPhiMod4->Fill( phi-60.*UsedSec,  point2.getY()-interAfter4.getY());


        }
        if(UsedSec==5)
        {
            histoDiffYBeforeVsPhiMod3->Fill(phi+60., point2.getY()-interBefore3.getY());
            histoDiffYAfterVsPhiMod3->Fill( phi+60., point2.getY()-interAfter3.getY());

            histoDiffYBeforeVsPhiMod4->Fill(phi+60.,  point2.getY()-interBefore4.getY());
            histoDiffYAfterVsPhiMod4->Fill( phi+60.,  point2.getY()-interAfter4.getY());

        }


        histoDiffYBeforeVsThetaMod3->Fill(theta, point2.getY()-interBefore3.getY());
        histoDiffYAfterVsThetaMod3-> Fill(theta,  point2.getY()-interAfter3.getY());



        histoDiffYBeforeVsThetaMod4->Fill(theta, point2.getY()-interBefore4.getY());
        histoDiffYAfterVsThetaMod4->Fill( theta,  point2.getY()-interAfter4.getY());



        //if(p->GetIsGood())
        //{
        //////    RasterPlotAfter->Fill( point2Sec.getX(), point2Sec.getY() );
    //    }

    }

    ff=new TFile(outName+".root","recreate");
    //TCanvas *diffYMod3=new TCanvas("diffYMod3","diffYMod3",500,500);
    //diffYMod3->Divide(1,2);
    //diffYMod3->cd(1);
    histoTheta->Write();

    histoDiffYBeforeMod3->Write();
    //diffYMod3->cd(2);
    histoDiffYAfterMod3->Write();

    RasterPlotAfter3->Write();
    RasterPlotAfter4->Write();
    RasterPlotBefore3->Write();
    RasterPlotBefore4->Write();


    //TCanvas *diffYMod4=new TCanvas("diffYMod4","diffYMod4",500,500);
    //diffYMod4->Divide(1,2);
    //diffYMod4->cd(1);
    histoDiffYBeforeMod4->Write();
    //    diffYMod4->cd(2);
    histoDiffYAfterMod4->Write();

    //    TCanvas *diffYVsPhiMod3=new TCanvas("diffYVsPhiMod3","diffYVsPhiMod3",500,500);
    //    diffYVsPhiMod3->Divide(1,2);
    //    diffYVsPhiMod3->cd(1);
    histoDiffYBeforeVsPhiMod3->Write();
    histoDiffYAfterVsPhiMod3->Write();
    histoDiffYBeforeVsThetaMod3->Write();
    histoDiffYAfterVsThetaMod3->Write();

    //    TCanvas *diffYVsPhiMod4=new TCanvas("diffYVsPhiMod3","diffYVsPhiMod3",500,500);
    //    diffYVsPhiMod4->Divide(1,2);
    //    diffYVsPhiMod4->cd(1);
    histoDiffYBeforeVsPhiMod4->Write();
    histoDiffYAfterVsPhiMod4->Write();
    histoDiffYBeforeVsThetaMod4->Write();
    histoDiffYAfterVsThetaMod4->Write();

    histoVertex->Write();

    //
    //    diffYVsPhiMod3->Write();
    //    diffYVsPhiMod4->Write();
    //    diffYMod3->Write();
    //    diffYMod4->Write();
    //ff->Close();


}


void HAlignmentGNoMag::Raster(TH2F* RasterBefore, TH2F* RasterAfter)
{
    Int_t countRaster=0;
    cout<<"Startting the Raster"<<endl;
    HAlignmentGParticle *p=NULL;
    Float_t binWidthX = RasterBefore->GetXaxis()->GetBinWidth(1);
    Float_t binWidthY = RasterBefore->GetYaxis()->GetBinWidth(1);
    Int_t binX;
    Int_t binY;
    Int_t checkBin[201][201];
    for(Int_t m=1; m < 201; m++)
        for(Int_t n=1; n < 201; n++)
            checkBin[m][n]=0;
    for(Int_t i=0;  i<cloneSize; i++)
    {
        if(i%10000==0)
            cout<<"raster finished "<<i<<"  of "<<cloneSize<<endl;
        p=(HAlignmentGParticle*)alignTracks->At(i);
        HGeomVector point2=p->GetPoint(1);
        point2=fAlignRot->TransMdc(point2,"FromModToSec",UsedSec,1);
        RasterBefore->Fill(point2.getX(), point2.getY());
        binX=(Int_t)((point2.getX() + 420.)/binWidthX)+1;
        binY=(Int_t)((point2.getY() - 220.)/binWidthY)+1;
        if(binX > 201 || binY > 201)
        {
            continue;
        }
        if(checkBin[binX][binY]==1)
        {
            p->SetIsGood(kFALSE);
            continue;
        }
        RasterAfter->Fill(point2.getX(), point2.getY());
        checkBin[binX][binY]=1;
        countRaster++;

    }
    cout<<"Raster selected  "<<countRaster<<"  tracks"<<endl;
}

/*
void HAlignmentGNoMag::Raster()
{
    Int_t countRaster=0;
    cout<<"Startting the Raster"<<endl;
    HAlignmentGParticle *p=NULL;
    Float_t binWidthX=RasterPlotBefore->GetXaxis()->GetBinWidth(1);
    Float_t binWidthY=RasterPlotBefore->GetYaxis()->GetBinWidth(1);
    Int_t binX;
    Int_t binY;
    Int_t checkBin[101][101];
    for(Int_t m=1; m<101; m++)
        for(Int_t n=1; n<101; n++)
            checkBin[m][n]=0;
    for(Int_t i=0;  i<cloneSize; i++)
    {
        if(i%10000==0)
            cout<<"raster finished "<<i<<"  of "<<cloneSize<<endl;
        p=(HAlignmentGParticle*)alignTracks->At(i);
        HGeomVector point2=p->GetPoint(1);
        point2=fAlignRot->TransMdc(point2,"FromModToSec",UsedSec,1);
        RasterPlotBefore->Fill(point2.getX(), point2.getY());
        binX=(Int_t)((point2.getX()+500)/binWidthX)+1;
        binY=(Int_t)(point2.getY()/binWidthY)+1;
        if(binX>1001 || binY>1001)
        {
            continue;
        }
        if(checkBin[binX][binY]==1)
        {
            p->SetIsGood(kFALSE);
            continue;
        }
        //RasterPlotAfter->Fill(point2.getX(), point2.getY());
        checkBin[binX][binY]=1;
        countRaster++;

    }
    cout<<"Raster selected  "<<countRaster<<"  tracks"<<endl;
}
*/
void HAlignmentGNoMag::setDefault(Float_t& a, Float_t& b, Float_t& c, Float_t& d)
{
  a=b=c=d=-1000.;
}

Bool_t HAlignmentGNoMag::isMdc(Int_t sec, Int_t mod)
{
   if(mdcSetup[sec][mod]==0) return kFALSE;
   return kTRUE;
}

void HAlignmentGNoMag::setMdcSetup(Int_t a[6][4])
{
  for(Int_t i=0; i<6; i++)
   for(Int_t j=0; j<4; j++)
   {
    mdcSetup[i][j] = a[i][j];
   }
}

void HAlignmentGNoMag::ResetRaster()
{
/* 
    if(RasterPlotBefore3) delete RasterPlotBefore3;
      RasterPlotBefore3= new TH2F("RasterPlotBefore3", "RasterPlotBefore3",100,-420,420,100,220,900);
    if(RasterPlotBefore4) delete RasterPlotBefore4;
      RasterPlotBefore4 = new TH2F("RasterPlotBefore4", "RasterPlotBefore4",100,-420,420,100,220,900);

    if(RasterPlotAfter3) delete RasterPlotAfter3;
      RasterPlotAfter3= new TH2F("RasterPlotAfter3", "RasterPlotAfter3",100,-420,420,100,220,900);
    if(RasterPlotAfter4) delete RasterPlotAfter4;
      RasterPlotAfter4 = new TH2F("RasterPlotAfter4", "RasterPlotAfter4",100,-420,420,100,220,900);
 */ 
   
    if(UsedMod == 2)
    { 
    Raster(RasterPlotBefore3, RasterPlotAfter3);
    }
    if(UsedMod == 3)
    {
    Raster(RasterPlotBefore4, RasterPlotAfter4);
    }
 
}
