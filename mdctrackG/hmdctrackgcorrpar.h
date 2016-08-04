#ifndef HMDCTRACKGCORRPAR_H
#define HMDCTRACKGCORRPAR_H
#include "TString.h"
#include "TObject.h"
#include "TNamed.h"
#include "hparcond.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
//CORRECTION PARAMETERS FOR MOMENTUM RECONSTRUCTION
class HMdcTrackGField;
class HMdcTrackGCorrections;
class HMdcTrackGSpline;
class HMdcTrackGFieldPar;
class HGeomVector;

class HMdcTrackGCorrPar:public HParCond {
protected:
    Int_t doCorrections;
   //    HMdcTrackGField* field; //!
   HMdcTrackGCorrections* corr;
   HMdcTrackGCorrections* corr1;
   HMdcTrackGCorrections* corrScan0, *corrScan1, *corrScan2;
   
    HMdcTrackGSpline* spline;  //!
    HMdcTrackGFieldPar *fieldmap; //!
public:
   
   HMdcTrackGCorrPar(const Char_t* name="MdcTrackGCorrPar",
		     const Char_t* title="Correction parameters for Spline fitting",
		     const Char_t* context="MdcTrackGCorrParProduction");
    ~HMdcTrackGCorrPar();
    void    putParams(HParamList*);
    Bool_t  getParams(HParamList*);//{return kTRUE;}
    void    clear();
   void    setCorrectionFlag(Int_t flag){ doCorrections=flag;}
   Int_t   getCorrectionFlag(){ return doCorrections;}
   void    initCorrections(TString, TString,TString,TString);
   void    initCorrections1(TString, TString,TString,TString);
   void    initCorrectionsScan(TString *, TString *,TString *,TString *);
   Double_t calcMomentum(HGeomVector*,Bool_t cond=kTRUE,Double_t targetz=0.);
   Double_t calcMomentum(HMdcSeg *seg[2],Bool_t cond=kTRUE,Double_t targetz=0.);
   
   Double_t calcMomentum123(HMdcSeg *seg[2],Bool_t cond=kTRUE,Double_t targetz=0.);
   void getDistance(Double_t *);
   Double_t getMetaDistance(Double_t,Double_t,Double_t);
   //    void    calcField(Double_t *a,Double_t *b,Int_t i);
   
   //  HMdcTrackGField* getField(){return field;}
   HMdcTrackGCorrections* getCorr(){return corr;}
   HMdcTrackGSpline* getSPline() {return spline;}
   HMdcTrackGCorrections* getCorr1(){return corr1;}
   
   void  getCorrScan(HMdcTrackGCorrections *corrScanAll[]);
   
   ClassDef(HMdcTrackGCorrPar,3)
      };
#endif
