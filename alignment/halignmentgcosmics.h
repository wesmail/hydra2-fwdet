#ifndef HALIGNMENTGCOSMICS_H
#define HALIGNMENTGCOSMICS_H
#include "TObject.h"
#include "hsymmat.h"
#include "fstream"
#include "iostream"
#include "TString.h"
#include "halignmentgparticle.h"
#include "hgeomtransform.h"


class TClonesArray;
class HAlignmentGGeom;
class HAlignmentGRotations;
class TH2F;
class TH1F;
class TMinuit;
class TFile;
class SplineGeantMy;
class TNtuple;

class HAlignmentGCosmics:public TObject
{
 public:
  HAlignmentGCosmics();
  virtual ~HAlignmentGCosmics();
  void SetNtuple(TNtuple *_nt);// { nt = _nt;} 
  TNtuple *GetNtuple()         {return nt;} 
  HAlignmentGGeom*  GetAlignGeom()      {return fAlignGeom;}
  HAlignmentGRotations*  GetAlignRot()  {return fAlignRot  ;}
  HGeomTransform GetRefTrans(Int_t i)   {return refTrans[i];}
  HGeomTransform GetTransGSI(TString,Int_t);
  //  void  SetRefTrans(HGeomTransform  *trans);
  void Align();
  void AlignLocal();
  void checkAlignment();
  void Yoxlama();
  Float_t GetMinFunction(HGeomTransform *, Bool_t test = kFALSE, TString option="");
  void changename(TH1F *, TString);
  void fillHistograms(HGeomVector &, HGeomVector &, Int_t, TString);
  Float_t GetMinFunction3(HGeomVector *,HGeomTransform *,Bool_t test = kFALSE, TString = "");
  void fillHistograms3(HGeomVector &, Int_t , TString);
  Int_t GetFixedSector() {return fixedSector;}
  void SetFixedSector(Int_t _fixedSector ) {fixedSector = _fixedSector;}

 private:
  Float_t accRange;
  Float_t accRange3;
  Int_t localAlign;
  Int_t fixedSector;
  
  TMinuit *fMinuit;
  TNtuple *nt;
  HGeomTransform refTrans[6]       ;   //gsi 0-ci modulun o biri sectoralara nisbeten
  HGeomTransform transGSIBefore[6] ;   //gsi modules before alignment
  HGeomTransform transGSIAfter[6]  ;
  HGeomTransform transDubna[6]     ;
  HAlignmentGGeom      *fAlignGeom ;
  HAlignmentGRotations *fAlignRot  ;
  TH1F *histoX0;
  TH1F *histoY0;
  TH1F *histoX0After;
  TH1F *histoY0After;
  
  TH1F *histoX1;
  TH1F *histoY1;
  TH1F *histoX1After;
  TH1F *histoY1After;
  
  TH1F *histoX2;
  TH1F *histoY2;
  TH1F *histoX2After;
  TH1F *histoY2After;


  //////////////

  TH1F *histoX03;
  TH1F *histoY03;
  TH1F *histoX0After3;
  TH1F *histoY0After3;
  
  TH1F *histoX13;
  TH1F *histoY13;
  TH1F *histoX1After3;
  TH1F *histoY1After3;
  
  TH1F *histoX23;
  TH1F *histoY23;
  TH1F *histoX2After3;
  TH1F *histoY2After3;

  
  
  Float_t x1, y1, x2, y2, x3, y3, x4, y4;
  Float_t sec1, sec2;
  Float_t missing;  
  ClassDef(HAlignmentGCosmics,0)
};   
#endif
    
