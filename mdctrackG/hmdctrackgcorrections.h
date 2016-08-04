#ifndef HMDCTRACKGCORRECTIONS_H
#define HMDCTRACKGCORRECTIONS_H
#include "TString.h"
#include "TObject.h"
#include "TNamed.h"

//CORRECTION PARAMETERS FOR MOMENTUM RECONSTRUCTION

class HMdcTrackGCorrections:public TNamed
{
friend class HMdcTrackGSpline;
private:
Double_t phiI[16];
Double_t tetaI[34];
Double_t precI[60];
Double_t corrE[34][16][60];
Double_t corrP[34][16][60];
Double_t corrE1[34][16][60];
Double_t corrP1[34][16][60];
//Double_t corrPNew[34][16][60];
//Double_t corrENew[34][16][60];


public:
  HMdcTrackGCorrections(const Char_t* name="",const Char_t* title="");
  HMdcTrackGCorrections(HMdcTrackGCorrections &corr);
  ~HMdcTrackGCorrections();
  void init(TString,TString,TString,TString);
  void clear();
  Double_t getPhiI(Int_t i) {return phiI[i];}
  Double_t getThetaI(Int_t i) {return tetaI[i];}
  Double_t getPrecI(Int_t i) {return precI[i];}
  Double_t getCorrE(Int_t i,Int_t j, Int_t k) {return corrE[i][j][k];}
  Double_t getCorrP(Int_t i,Int_t j, Int_t k) {return corrP[i][j][k];}
  Double_t getCorrE1(Int_t i,Int_t j, Int_t k) {return corrE1[i][j][k];}
  Double_t getCorrP1(Int_t i,Int_t j, Int_t k) {return corrP1[i][j][k];}
    
  ClassDef(HMdcTrackGCorrections,2)
};


#endif
