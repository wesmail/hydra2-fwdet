#ifndef HALIGNMENTGBASE_H
#define HALIGNMENTGBASE_H
#include "TObject.h"
#include "hsymmat.h"
#include "fstream"
#include "iostream"
#include "TString.h"
#include "halignmentgparticle.h"
#include "hgeomtransform.h"
#include "TNtuple.h"
class TClonesArray;
class HAlignmentGGeom;
class HAlignmentGRotations;
class TH2F;
class TH1F;
class TMinuit;
class TFile;


class HAlignmentGBase:public TObject
{
 public:
  
  HAlignmentGBase();
  virtual  ~HAlignmentGBase();  
  ////////////////////////////////////////////////////////////////
  virtual void    CollectTracks()                            = 0;
  virtual void    ResetCounts()                              = 0;
  virtual Bool_t  AddToClones()                              = 0;
  virtual void    FillHistograms()                           = 0;
  virtual HGeomTransform Align(const Int_t &, const Int_t &) = 0;
  //virtual void CheckAlignment(HGeomTransform trans[6][4], Int_t s)= 0;
  
  virtual void SetNtBranches()                                    = 0;
  /////////////////////////////////////////////////////////////////  
  HAlignmentGGeom*       GetAlignGeom() const     {return  fAlignGeom; }
  HAlignmentGRotations*  GetAlignRot () const     {return  fAlignRot ; }
  Int_t GetCloneSize () const {return cloneSize;}
  Int_t GetHistoNumbers () const {return histoNumbers;}
  
  TClonesArray* GetAlignTracks() const {return alignTracks;}
  TClonesArray* GetHistoClones() const {return histoClones;}
  
  Int_t GetUsedMod() const {return UsedMod;}
  Int_t GetUsedSec() const {return UsedSec;}
  void  SetNtuple(TNtuple *_nt) {nt=_nt; ntEntries = nt-> GetEntries();}
  TNtuple* GetNtuple() const {return nt;}
  
  void GetPoints(void (*pFunc)(Float_t &, Float_t &, Float_t &, Float_t &, HGeomVector &, HGeomVector &),Float_t &, Float_t &, Float_t &, Float_t &, HGeomVector &, HGeomVector &);
  
  
  ///////////////////////////////////////////////////////////////// 
  
 protected:
  
  TClonesArray* alignTracks;
  TClonesArray* histoClones;
  Int_t histoNumbers;
  
  TFile *ff;
  TNtuple *nt;
  TMinuit *fMinuit;
  Int_t UsedMod;
  Int_t UsedSec;
  HAlignmentGGeom *fAlignGeom;
  HAlignmentGRotations *fAlignRot;
  HGeomTransform transBefore, transAfter;
  Int_t cloneSize;
  Int_t ntEntries;
  ClassDef(HAlignmentGBase,0)
    };   
#endif
    
    
