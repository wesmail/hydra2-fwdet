#ifndef HALIGNMENTGNOMAG_H
#define HALIGNMENTGNOMAG_H

#include "TObject.h"
#include "hsymmat.h"
#include "fstream"
#include "iostream"
#include "TString.h"
#include "halignmentgparticle.h"
#include "hgeomtransform.h"
#include "halignmentgbase.h"

class TClonesArray;
class HAlignmentGGeom;
class HAlignmentGRotations;
class TH2F;
class TH1F;
class TMinuit;
class TFile;

class HAlignmentGNoMag:public HAlignmentGBase
{
 
public:
  HAlignmentGNoMag();
  virtual ~HAlignmentGNoMag();  
  void    SetIname(TString _inName) {inName=_inName;}
  TString GetIname() {return inName;}
  void    Init(TString, TString, Int_t, Bool_t);
  virtual void    CollectTracks();
  virtual void    ResetCounts();
  virtual Bool_t  AddToClones();
  virtual HGeomTransform Align(const Int_t &, const Int_t &);    
  void  CheckAlignment();
  //virtual void CheckAlignment(HGeomTransform trans[6][4], Int_t s) {;}
  virtual void  FillHistograms();
  virtual void SetNtBranches() {;}
  void AddData(Float_t , Float_t , Float_t, Float_t, Int_t);
  Bool_t Selected(HAlignmentGParticle &);  
  TH1F* GetTargetHisto() { return histoVertex;}
  //TH2F* GetRasterPlotBefore() {return RasterPlotBefore;}
  //TH2F* GetRasterPlotAfter() {return RasterPlotAfter;}
  HGeomTransform AlignMod();
  void Raster(TH2F*, TH2F*);
  void JustPlot(Int_t , Int_t );

  Int_t GetFitVersion() {return fitVersion;}
  void  SetFitVersion(Int_t _fitVersion) {fitVersion = _fitVersion;}
  void setDefault(Float_t& , Float_t& , Float_t& , Float_t& );
 
 void setMdcSetup(Int_t a[6][4]);
 Bool_t isMdc(Int_t , Int_t );
 
 void ResetRaster();
 
private:
  Bool_t isThirdChamber;
  Bool_t isFouthChamber;

  Float_t AcceptRadius;
  Float_t AcceptRadiusFour;
  Float_t AcceptRadiusTarg;
  Float_t AcceptDir;
  Float_t AcceptDirFour;

  Int_t fitVersion;
  Int_t numberOfTracks;
  Bool_t isAligned;
  ifstream in;
  TString inName;
  TString outName;
  Int_t nEvent;
  Int_t mod;
  Int_t sector;
  Float_t X,Y;
  Float_t Xdir, Ydir;
  Int_t FirstEvent;
  Int_t FirstMod;
  Float_t FirstX;
  Float_t FirstXdir;
  Float_t FirstY;
  Float_t FirstYdir;
  Int_t nCount[4];
  HAlignmentGParticle *testP;
  HAlignmentGParticle fAlignParticle;
  Float_t XValue   [4][50];
  Float_t XdirValue[4][50];
  Float_t YValue   [4][50];
  Float_t YdirValue[4][50];
  TH1F *histoVertex; 
  TH1F *histoDiffYBeforeMod3;
  TH1F *histoDiffYAfterMod3;
  TH2F *histoDiffYBeforeVsPhiMod3;
  TH2F *histoDiffYAfterVsPhiMod3;
  TH2F *histoDiffYBeforeVsThetaMod3;
  TH2F *histoDiffYAfterVsThetaMod3;
  TH1F *histoDiffYBeforeMod4;
  TH1F *histoDiffYAfterMod4;
  TH2F *histoDiffYBeforeVsPhiMod4;
  TH2F *histoDiffYAfterVsPhiMod4;
  TH2F *histoDiffYBeforeVsThetaMod4;
  TH2F *histoDiffYAfterVsThetaMod4;
  TH1F *histoTheta;
  TH2F *RasterPlotBefore3;
  TH2F *RasterPlotAfter3;
  TH2F *RasterPlotBefore4;
  TH2F *RasterPlotAfter4;
  
  ///TH1F *histoVertex;

  HGeomTransform transBefore[4];
  HGeomTransform transAfter[4];
  HGeomTransform transStarting[4];
  
  Int_t mdcSetup[6][4];
  
ClassDef(HAlignmentGNoMag,0)

};   
#endif
