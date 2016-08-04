//*-- Author : Anar Rustamov

#include "halignmentgcosmics.h"
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
ClassImp(HAlignmentGCosmics)
  HAlignmentGCosmics::HAlignmentGCosmics()
{
  fAlignGeom  = new HAlignmentGGeom();
  fAlignRot   = new HAlignmentGRotations();
  fMinuit     = NULL;////new TMinuit(6);
  for(Int_t i = 0; i < 6; i++)
    {
      refTrans[i] = *(fAlignRot -> GetTransMdc(i,0)); 
      //      if(i==0) {cout<< "reftrans" <<endl; refTrans[i].print();}
      //gsi modulunun matrisi
      transGSIBefore[i] = refTrans[i];
      //yuxaridaki kimi    
      refTrans[i].transTo(transGSIBefore[0]); 
      transGSIAfter[i] =  refTrans[i];  
      transDubna[i] = *(fAlignRot -> GetTransMdc(i,1));
    }
    
  accRange = 10.;
  accRange3 = 10;    
  localAlign =0;    
    
  histoX0 = new TH1F("histoX0","histoX0",100,-20.,20.);
  histoY0 = new TH1F("histoY0","histoY0",100,-10.,10.);
  histoX0After = new TH1F("histoX0After","histoX0After",100,-20.,20.);
  histoY0After = new TH1F("histoY0After","histoY0After",100,-10.,10.);
  histoX1 = new TH1F("histoX1","histoX1",100,-20.,20.);
  histoY1 = new TH1F("histoY1","histoY1",100,-10.,10.);
  histoX1After = new TH1F("histoX1After","histoX1After",100,-20.,20.);
  histoY1After = new TH1F("histoY1After","histoY1After",100,-10.,10.);
  histoX2 = new TH1F("histoX2","histoX2",100,-20.,20.);
  histoY2 = new TH1F("histoY2","histoY2",100,-10.,10.);
  histoX2After = new TH1F("histoX2After","histoX2After",100,-20.,20.);
  histoY2After = new TH1F("histoY2After","histoY2After",100,-10.,10.);

  histoX03 = new TH1F("histoX03","histoX03",100,-20.,20.);
  histoY03 = new TH1F("histoY03","histoY03",100,-10.,10.);
  histoX0After3 = new TH1F("histoX0After3","histoX0After3",100,-20.,20.);
  histoY0After3 = new TH1F("histoY0After3","histoY0After3",100,-10.,10.);
  histoX13 = new TH1F("histoX13","histoX13",100,-20.,20.);
  histoY13 = new TH1F("histoY13","histoY13",100,-10.,10.);
  histoX1After3 = new TH1F("histoX1After3","histoX1After3",100,-20.,20.);
  histoY1After3 = new TH1F("histoY1After3","histoY1After3",100,-10.,10.);
  histoX23 = new TH1F("histoX23","histoX23",100,-20.,20.);
  histoY23 = new TH1F("histoY23","histoY23",100,-10.,10.);
  histoX2After3 = new TH1F("histoX2After3","histoX2After3",100,-20.,20.);
  histoY2After3 = new TH1F("histoY2After3","histoY2After3",100,-10.,10.);
}

void HAlignmentGCosmics::SetNtuple(TNtuple *_nt)
{
 nt = _nt;
 nt -> SetBranchAddress("x1",&x1); // yuxari dubna 
 nt -> SetBranchAddress("y1",&y1); // yuxari dubna
 nt -> SetBranchAddress("x2",&x2); // yuxari gsi 
 nt -> SetBranchAddress("y2",&y2); // yuxari gsi
 nt -> SetBranchAddress("x3",&x3); // ashagi gsi 
 nt -> SetBranchAddress("y3",&y3); // ashagi gsi
 nt -> SetBranchAddress("x4",&x4); // ashagi dubna 
 nt -> SetBranchAddress("y4",&y4); // ahsagi dubna
 nt -> SetBranchAddress("sec1",&sec1);
 nt -> SetBranchAddress("sec2",&sec2);
 nt -> SetBranchAddress("missing",&missing);
}

HAlignmentGCosmics::~HAlignmentGCosmics()
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



void fcnCosmicsLocal(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  //bir sector birlikde chevrilir;
  static int count = 1;
  count++;
  HAlignmentGCosmics   *myObject   = (HAlignmentGCosmics*)(gMinuit -> GetObjectFit());
  HAlignmentGRotations *fAlignRot  = myObject -> GetAlignRot();
  HGeomTransform trans =  fAlignRot -> MakeTransMatrix(par[0], par[1], par[2], par[3], par[4], par[5]);
  HGeomTransform transMod[6];
  
  Int_t fixedSector = myObject -> GetFixedSector();
  
  for(Int_t i = 0; i < 6; ++i)
    {
      transMod[i] = myObject -> GetTransGSI("Old",i);
      if(i==fixedSector || i==(fixedSector+3))
      {
       transMod[i].transTo(trans);
      }
      if(fixedSector == -1) transMod[i].transTo(trans);
    }
  f =  myObject -> GetMinFunction(transMod);
  if(count%10 == 0) 
    cout<<"iter:  "<<count<<"  function  "<<f<<" "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "
                      <<par[4]<<"  "<<par[5]<<endl;
}



void fcnCosmics(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag)
{
  static int count = 1;
  count++;
  HAlignmentGCosmics   *myObject   = (HAlignmentGCosmics*)(gMinuit -> GetObjectFit());
  HAlignmentGRotations *fAlignRot  = myObject -> GetAlignRot();
  HGeomTransform trans =  fAlignRot -> MakeTransMatrix(par[0], par[1], par[2], par[3], par[4], par[5]);
  HGeomTransform transMod[6];
  for(Int_t i = 0; i < 6; ++i)
    {
      transMod[i] = myObject -> GetRefTrans(i);
      transMod[i].transFrom(trans);
    }
  f =  myObject -> GetMinFunction(transMod);
  if(count%10 == 0) 
    cout<<"iter:  "<<count<<"  function  "<<f<<" "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<"  "<<par[3]<<"  "
                      <<par[4]<<"  "<<par[5]<<endl;
}

void HAlignmentGCosmics::Align()
{
  Int_t ierflg = 0;
  if(!gMinuit) fMinuit=new TMinuit(6);
  gMinuit->SetFCN(fcnCosmics);
  gMinuit->SetObjectFit(this);
  Double_t arglist[6];
  Double_t vstart[6];
  Double_t step[13];
  Double_t out[6];
  Double_t errOut[6];
 // Double_t low[6] ,up[6];

  fAlignRot -> GetEulerAngles (transGSIBefore[0], vstart[0], vstart[1], vstart[2]);
  fAlignRot -> GetTransVector (transGSIBefore[0], vstart[3], vstart[4], vstart[5]);
  for(Int_t i=0; i<6; i++)
    {
      step[i] = 0.001;

      //low[i]  = vstart[i] - .009;
      //up[i]   = vstart[i] + .009;
    }
  /*
  low[3]  = vstart[3] - 15.;
  low[4]  = vstart[4] - 15.;
  low[5]  = vstart[5] - 15.;
  up[3]   = vstart[3] + 15.;
  up[4]   = vstart[4] + 15.;
  up[5]   = vstart[5] + 15.;
  */
  step[3] = 0.05;
  step[4] = 0.5;
  step[5] = 0.5;
  /*
  gMinuit->mnparm(0,"phi", vstart[0],step[0],low[0],up[0],ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],low[1],up[1],ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],low[2],up[2],ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], low[3],up[3],ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],low[4],up[4],ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], low[5],up[5],ierflg);
 */


  gMinuit->mnparm(0,"phi", vstart[0],step[0],0,0,ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],0,0,ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],0,0,ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], 0,0,ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],0,0,ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], 0,0,ierflg);


  gMinuit->SetPrintLevel(-1);
  //     arglist[0] =1;
  //     gMinuit->mnexcm("SET NOW", arglist,1,ierflg);
  //gMinuit->SetErrorDef(0.5);
  gMinuit->SetErrorDef(1);
  arglist[0]=2;
  gMinuit->mnexcm("SET STR",arglist,1,ierflg);
  //arglist[0] = 1;
  //gMinuit->mnexcm("SET ERR",arglist ,1,ierflg);
  arglist[1] = 0.01;
  arglist[0] = 10000;
  
    gMinuit->mnexcm("SIMPLEX",arglist,1,ierflg);
    //gMinuit->mnexcm("MIGRAD",arglist,1,ierflg);
  ////////gMinuit->mnexcm("MINOS",arglist,1,ierflg);
  for (Int_t ii = 0; ii < 6; ii++)
    {
      gMinuit->GetParameter(ii,out[ii],errOut[ii]);
    }
  
  HGeomTransform transT = fAlignRot->MakeTransMatrix(out[0], out[1], out[2], out[3], out[4], out[5]);
  
  for (Int_t ii = 0; ii < 6; ii++)
    {
      transGSIAfter[ii].transFrom(transT);
    }
}



void HAlignmentGCosmics::AlignLocal()
{
  localAlign = 1;
  Int_t ierflg = 0;
  if(!gMinuit) fMinuit=new TMinuit(6);
  gMinuit->SetFCN(fcnCosmicsLocal);
  gMinuit->SetObjectFit(this);
  Double_t arglist[6];
  Double_t vstart[6] = {0.};
  
  vstart[0] = acos(-1.)/2.;
  vstart[2] = -1.*vstart[0];
  
  //Double_t low[6],up[6];
  Double_t step[13];
  Double_t out[6];
  Double_t errOut[6];

  for(Int_t i=0; i<6; i++)
    {
      step[i] = 0.001;
      //low[i]  = vstart[i] - .009;
      //up[i]   = vstart[i] + .009;
    }
  /*
  low[3]  = vstart[3] - 15.;
  low[4]  = vstart[4] - 15.;
  low[5]  = vstart[5] - 15.;
  up[3]   = vstart[3] + 15.;
  up[4]   = vstart[4] + 15.;
  up[5]   = vstart[5] + 15.;
  */
  step[3] = 0.005;
  step[4] = 0.005;
  step[5] = 0.005;
  /*
  gMinuit->mnparm(0,"phi", vstart[0],step[0],low[0],up[0],ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],low[1],up[1],ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],low[2],up[2],ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], low[3],up[3],ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],low[4],up[4],ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], low[5],up[5],ierflg);
 */


  gMinuit->mnparm(0,"phi", vstart[0],step[0],0,0,ierflg);
  gMinuit->mnparm(1,"psi",vstart[1], step[1],0,0,ierflg);
  gMinuit->mnparm(2,"theta", vstart[2], step[2],0,0,ierflg);
  gMinuit->mnparm(3,"x", vstart[3], step[3], 0,0,ierflg);
  gMinuit->mnparm(4,"y",vstart[4], step[4],0,0,ierflg);
  gMinuit->mnparm(5,"z", vstart[5], step[5], 0,0,ierflg);


  gMinuit->SetPrintLevel(-1);
  //     arglist[0] =1;
  //     gMinuit->mnexcm("SET NOW", arglist,1,ierflg);
  //gMinuit->SetErrorDef(0.5);
  gMinuit->SetErrorDef(1);
  arglist[0]=2;
  gMinuit->mnexcm("SET STR",arglist,1,ierflg);
  //arglist[0] = 1;
  //gMinuit->mnexcm("SET ERR",arglist ,1,ierflg);
  arglist[1] = 0.01;
  arglist[0] = 10000;
  
    gMinuit->mnexcm("SIMPLEX",arglist,1,ierflg);
    //gMinuit->mnexcm("MIGRAD",arglist,1,ierflg);
  ////////gMinuit->mnexcm("MINOS",arglist,1,ierflg);
  for (Int_t ii = 0; ii < 6; ii++)
    {
      gMinuit->GetParameter(ii,out[ii],errOut[ii]);
    }
  
  HGeomTransform transT = fAlignRot->MakeTransMatrix(out[0], out[1], out[2], out[3], out[4], out[5]);
  
  for (Int_t ii = 0; ii < 6; ii++)
    {
      transGSIAfter[ii] = transGSIBefore[ii];
      if(ii == fixedSector || ii == (fixedSector+3))
      transGSIAfter[ii].transTo(transT);
      if(fixedSector == -1 ) transGSIAfter[ii].transTo(transT);
    }
}

 

void HAlignmentGCosmics::checkAlignment()
{
  cout<<"checking the alignment results "<<endl;
  cout<<"checking the alignment results "<<endl;
  cout<<"checking the alignment results "<<endl;
  cout<<"checking the alignment results "<<endl;
  cout<<"checking the alignment results "<<endl;
  GetMinFunction(transGSIBefore, kTRUE);
  GetMinFunction(transGSIAfter,  kTRUE,"After");
    TFile f("checkalignment.root","recreate");
    histoX0 -> Write();
    histoY0 -> Write();
    histoX0After -> Write();
    histoY0After -> Write();
    
    histoX1 -> Write();
    histoY1 -> Write();
    histoX1After -> Write();
    histoY1After -> Write();
    
    histoX2 -> Write();
    histoY2 -> Write();
    histoX2After -> Write();
    histoY2After -> Write();
    
    histoX0After -> SetLineColor(2);
    histoY0After -> SetLineColor(2);
    histoX1After -> SetLineColor(2);
    histoY1After -> SetLineColor(2);
    histoX2After -> SetLineColor(2);
    histoY2After -> SetLineColor(2);
    
    
    TCanvas *c1 = new TCanvas();
    c1 -> Divide(2,3);
    c1->cd(1);
    histoX0 -> Draw();
    histoX0After -> Draw("sames");
    c1->cd(2);
    histoY0 -> Draw();
    histoY0After -> Draw("sames");
    
    c1->cd(3);
    histoX1 -> Draw();
    histoX1After -> Draw("sames");
    c1->cd(4);
    histoY1 -> Draw();
    histoY1After -> Draw("sames");
    
    c1->cd(5);
    histoX2 -> Draw();
    histoX2After -> Draw("sames");
    c1->cd(6);
    histoY2 -> Draw();
    histoY2After -> Draw("sames");
    
     TCanvas *c2 = new TCanvas();
    c2 -> Divide(2,3);
    c2->cd(1);
    histoX0After -> Draw();
    c2->cd(2);
    histoY0After -> Draw();
    
    c2->cd(3);
    histoX1After -> Draw();
    c2->cd(4);
    histoY1After -> Draw();
    
    c2->cd(5);
    histoX2After -> Draw();
    c2->cd(6);
    histoY2After -> Draw();

    c1 -> Write();
    c2 -> Write(); 



    //////////

    histoX03 -> Write();
    histoY03 -> Write();
    histoX0After3 -> Write();
    histoY0After3 -> Write();
    
    histoX13 -> Write();
    histoY13 -> Write();
    histoX1After3 -> Write();
    histoY1After3 -> Write();
    
    histoX23 -> Write();
    histoY23 -> Write();
    histoX2After3 -> Write();
    histoY2After3 -> Write();
    
    histoX0After3 -> SetLineColor(2);
    histoY0After3 -> SetLineColor(2);
    histoX1After3 -> SetLineColor(2);
    histoY1After3 -> SetLineColor(2);
    histoX2After3 -> SetLineColor(2);
    histoY2After3 -> SetLineColor(2);
    
    
    TCanvas *c3 = new TCanvas();
    c3 -> Divide(2,3);
    c3->cd(1);
    histoX03 -> Draw();
    histoX0After3 -> Draw("sames");
    c3->cd(2);
    histoY03 -> Draw();
    histoY0After3 -> Draw("sames");
    
    c3->cd(3);
    histoX13 -> Draw();
    histoX1After3 -> Draw("sames");
    c3->cd(4);
    histoY13 -> Draw();
    histoY1After3 -> Draw("sames");
    
    c3->cd(5);
    histoX23 -> Draw();
    histoX2After3 -> Draw("sames");
    c3->cd(6);
    histoY23 -> Draw();
    histoY2After3 -> Draw("sames");
    
    c3 -> Write();

   

 }

HGeomTransform HAlignmentGCosmics::GetTransGSI(TString option,Int_t mod)
{
  if (option=="Old")
     return transGSIBefore[mod];
     else if(option=="New")
       return transGSIAfter[mod]; 
      else 
        { 
	  HGeomTransform test;
	  return test;
	}
}

Float_t HAlignmentGCosmics::GetMinFunction(HGeomTransform *trans, Bool_t isFill, TString option)
{


  HGeomVector point1, Point1;
  HGeomVector point2;
  HGeomVector point3;
  HGeomVector point4, Point4;
  HGeomVector diff1, diff2;
  HGeomVector POINTS[4];  

  Float_t diff = 0;
    
  Int_t nentries = nt -> GetEntries();
  //    nentries  = 5000;
  for(Int_t i = 0; i < nentries; i++)  
    {
    /*
      if(!isFill)
      {
       //if(sec1 == 0) continue;
       if(sec1 == 0)  continue;
      }
    */  
      //      if(i%1000000 ==0) cout<<"reading ntuple finished "<<i<<" of "<<nentries<<endl;
      nt->GetEntry(i);

      point1.setXYZ(x1, y1, 0.); // module sisteminde;
      point2.setXYZ(x2, y2, 0.); // module sisteminde;
      point3.setXYZ(x3, y3, 0.); // module sisteminde;
      point4.setXYZ(x4, y4, 0.); // module sisteminde;

      POINTS[0] = point1;
      POINTS[1] = point2;
      POINTS[2] = point3;
      POINTS[3] = point4;


      if(localAlign && fixedSector!= -1 && ((Int_t)sec1 != fixedSector && (Int_t)sec2 != fixedSector)) continue; 
      
      if(missing !=0 ) 
      {/*cout<<"check!!! missing == "<<missing<<endl;*/
      diff += GetMinFunction3(POINTS,trans,isFill,option); continue;
      }
     

      point2 = trans[(Int_t)sec1].transFrom(point2); // in lab
      point3 = trans[(Int_t)sec2].transFrom(point3); // in lab
      Point1 = fAlignGeom ->  CalcIntersection(point3,point2,transDubna[(Int_t)sec1]); //in lab
      Point4 = fAlignGeom ->  CalcIntersection(point2,point3,transDubna[(Int_t)sec2]); //in lab

      
      HGeomVector point1Lab = transDubna[(Int_t)sec1].transFrom(point1); // in lab
      HGeomVector point4Lab = transDubna[(Int_t)sec2].transFrom(point4); // in lab
      
      
      
      if(!isFill)
      {   
      if(fAlignGeom ->  CalcDistanceToLine(point2,point1Lab,point4Lab) > accRange ) continue;
      if(fAlignGeom ->  CalcDistanceToLine(point3,point1Lab,point4Lab) > accRange ) continue;
      }
       
      if(isFill)
	{
	  Point1 = transDubna[(Int_t)sec1].transTo(Point1); //in module
	  Point4 = transDubna[(Int_t)sec2].transTo(Point4); //in module
	}
      
      else
	{ 
	  //do the same as before
	  Point1 = transDubna[(Int_t)sec1].transTo(Point1); //in module
	  Point4 = transDubna[(Int_t)sec2].transTo(Point4); //in module
	}
       	
      /*
      else
	{

    
	//  point1 =  point1Lab;//transDubna[(Int_t)sec1].transFrom(point1); // in lab
	//  point4 =  point4Lab; //transDubna[(Int_t)sec2].transFrom(point4); // in lab
	  
	  Point1 = transDubna[(Int_t)sec1].transTo(Point1); //in module
          Point4 = transDubna[(Int_t)sec2].transTo(Point4); //in module
	  point1 =  fAlignRot ->  TransMdc(point1,"FromModToSec",(Int_t)sec1,1); //in sec
	  point4 =  fAlignRot ->  TransMdc(point4,"FromModToSec",(Int_t)sec2,1); //in sec                
	  Point1 =  fAlignRot ->  TransMdc(Point1,"FromModToSec",(Int_t)sec1,1); //in sec
	  Point4 =  fAlignRot ->  TransMdc(Point4,"FromModToSec",(Int_t)sec2,1); //in sec
	}
      */
      diff1  = point1 - Point1;
      diff2  = point4 - Point4;
      if(isFill) fillHistograms(diff1, diff2, (Int_t)sec1, option);

      diff += diff1.getX()*diff1.getX()/6. + diff1.getY()*diff1.getY();// + diff1.getZ()*diff1.getZ();
      diff += diff2.getX()*diff2.getX()/6. + diff2.getY()*diff2.getY();// + diff2.getZ()*diff2.getZ();
 
    }

  return diff;
}

Float_t HAlignmentGCosmics::GetMinFunction3(HGeomVector *POINTS,HGeomTransform *trans, Bool_t isFill,
 TString option)
{
  HGeomVector point1, point2, point3, point4;
  HGeomVector Point1, Point4;
  point1 = POINTS[0];
  point2 = POINTS[1];
  point3 = POINTS[2];
  point4 = POINTS[3];
  HGeomVector DIFF;

  if(missing == 1)
    {
      //top dubna is missing
      point2 = trans[(Int_t)sec1].transFrom(point2); // in lab
      point3 = trans[(Int_t)sec2].transFrom(point3); // in lab
      Point4 = fAlignGeom ->  CalcIntersection(point2,point3,transDubna[(Int_t)sec2]); //in lab
      HGeomVector point4Lab = transDubna[(Int_t)sec2].transFrom(point4); // in lab
      if(!isFill)
	{   
	  if(fAlignGeom ->  CalcDistanceToLine(point2,point3,point4Lab) > accRange3 ) return 0;
	}
      Point4 = transDubna[(Int_t)sec2].transTo(Point4); //in module    
      
      DIFF  = point4 - Point4;
      if(isFill) fillHistograms3(DIFF,(Int_t)sec1, option);
      return (DIFF.getX()*DIFF.getX()/6. + DIFF.getY()*DIFF.getY());
    }
  else 
    if(missing == 4)
      {
	//bottom dubna is missing
	point2 = trans[(Int_t)sec1].transFrom(point2); // in lab
	point3 = trans[(Int_t)sec2].transFrom(point3); // in lab
	Point1 = fAlignGeom ->  CalcIntersection(point3,point2,transDubna[(Int_t)sec1]); //in lab  
	HGeomVector point1Lab = transDubna[(Int_t)sec1].transFrom(point1); // in lab
	if(!isFill)
	  {   
	    if(fAlignGeom ->  CalcDistanceToLine(point3,point2,point1Lab) > accRange3 ) return 0;
	  }
	Point1 = transDubna[(Int_t)sec1].transTo(Point1); //in module
	DIFF  = point1 - Point1;
        if(isFill) fillHistograms3(DIFF,(Int_t)sec1, option);
	return (DIFF.getX()*DIFF.getX()/6. + DIFF.getY()*DIFF.getY());
      }
    else if(missing == 2)
      {
	//missing top gsi
	 point3 = trans[(Int_t)sec2].transFrom(point3); // in lab
	 HGeomVector point1Lab = transDubna[(Int_t)sec1].transFrom(point1); // in lab
         HGeomVector point4Lab = transDubna[(Int_t)sec2].transFrom(point4); // in lab
	 Point1 = fAlignGeom ->  CalcIntersection(point4Lab,point3,transDubna[(Int_t)sec1]); //in lab 
	 if(!isFill)
	   {   
	     if(fAlignGeom ->  CalcDistanceToLine(point4Lab,point3,point1Lab) > accRange3 ) return 0;
	   }	
        Point1 = transDubna[(Int_t)sec1].transTo(Point1); //in module
        DIFF  = point1 - Point1;
        if(isFill) fillHistograms3(DIFF,(Int_t)sec1, option);
        return (DIFF.getX()*DIFF.getX()/6. + DIFF.getY()*DIFF.getY());
      }
    else if(missing ==3)
      {
	//missing bottom gsi
        point2 = trans[(Int_t)sec1].transFrom(point2); // in lab
	HGeomVector point1Lab = transDubna[(Int_t)sec1].transFrom(point1); // in lab
	HGeomVector point4Lab = transDubna[(Int_t)sec2].transFrom(point4); // in lab
	Point4 = fAlignGeom ->  CalcIntersection(point1Lab,point2,transDubna[(Int_t)sec2]); //in lab
        if(!isFill)
	  {   
	     if(fAlignGeom ->  CalcDistanceToLine(point1Lab,point2,point4Lab) > accRange3 ) return 0;
	  }	     
        
	Point4 = transDubna[(Int_t)sec2].transTo(Point4); //in module    
	DIFF  = point4 - Point4;
        if(isFill) fillHistograms3(DIFF,(Int_t)sec1, option);
	return (DIFF.getX()*DIFF.getX()/6. + DIFF.getY()*DIFF.getY());
  }
    else 
      { 
        cout<<"missing === "<<missing<<endl;
	return 0;
      }
  
}


void HAlignmentGCosmics::changename(TH1F *histo, TString option)
{
  TString name(histo->GetName());
  name.Append(option.Data());
  histo -> SetName(name.Data());
}

void HAlignmentGCosmics::Yoxlama()
{
  for(Int_t i=0; i<6; i++)
    {
      cout<<"gsi bir basha"<<endl;
      transGSIBefore[i].print();
      cout<<"gsi chevirmeden sonra "<<endl;
      HGeomTransform  transMod = refTrans[i];
      transMod.transFrom(transGSIBefore[0]);
      transMod.print();
    }
}

void HAlignmentGCosmics::fillHistograms(HGeomVector &diff1, HGeomVector &diff2,Int_t sec1, TString option)
{
  if(sec1 == 0)
    {
      if(option == "After")
	{
	  histoX0After -> Fill(diff1.getX());
          histoX0After -> Fill(diff2.getX());
	  histoY0After -> Fill(diff1.getY());
	  histoY0After -> Fill(diff2.getY());
	}
      else
	{
	  histoX0 -> Fill(diff1.getX());
          histoX0 -> Fill(diff2.getX());
	  histoY0 -> Fill(diff1.getY());
	  histoY0 -> Fill(diff2.getY());
	}
    }
  else  
    if(sec1 == 1)
      {
      if(option == "After")
	{
	  histoX1After -> Fill(diff1.getX());
          histoX1After -> Fill(diff2.getX());
	  histoY1After -> Fill(diff1.getY());
	  histoY1After -> Fill(diff2.getY());
	}
      else
	{
	  histoX1 -> Fill(diff1.getX());
          histoX1 -> Fill(diff2.getX());
	  histoY1 -> Fill(diff1.getY());
	  histoY1 -> Fill(diff2.getY());
	  
	  //histoX0VsX -> Fill();
	  
	}
      }
    else
      if(sec1 != 2) {cout<<"this can not be"<<endl;}
      else
        {
	  if(option == "After")
	    {
	      histoX2After -> Fill(diff1.getX());
	      histoX2After -> Fill(diff2.getX());
	      histoY2After -> Fill(diff1.getY());
	      histoY2After -> Fill(diff2.getY());
	    }
	  else
	    {
	      histoX2 -> Fill(diff1.getX());
	      histoX2 -> Fill(diff2.getX());
	      histoY2 -> Fill(diff1.getY());
	      histoY2 -> Fill(diff2.getY());
	    }
	}
  
} 

void HAlignmentGCosmics::fillHistograms3(HGeomVector &diff, Int_t sec1, TString option)
{
  if(sec1 == 0)
    {
      if(option == "After")
	{
	  histoX0After3 -> Fill(diff.getX());
          histoY0After3 -> Fill(diff.getY());
	}
      else
	{
	  histoX03 -> Fill(diff.getX());
	  histoY03 -> Fill(diff.getY());
	}
    }
  else  
    if(sec1 == 1)
      {
      if(option == "After")
	{
	  histoX1After3 -> Fill(diff.getX());
	  histoY1After3 -> Fill(diff.getY());
	}
      else
	{
	  histoX13 -> Fill(diff.getX());
	  histoY13 -> Fill(diff.getY());
	}
      }
    else
      if(sec1 != 2) {cout<<"this can not be"<<endl;}
      else
        {
	  if(option == "After")
	    {
	      histoX2After3 -> Fill(diff.getX());
	      histoY2After3 -> Fill(diff.getY());
	    }
	  else
	    {
	      histoX23 -> Fill(diff.getX());
	      histoY23 -> Fill(diff.getY());
	    }
	}
} 
