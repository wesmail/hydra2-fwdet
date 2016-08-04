#ifndef HMDCPPEVTFILTER_H
#define HMDCPPEVTFILTER_H

#include "hevent.h"
#include "hreconstructor.h"
#include "TString.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "hmdcseg.h"
#include "hades.h"
#include "haddef.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hmdcgeompar.h"
#include "hspecgeompar.h"
#include "hmdcdetector.h"
#include "hcategory.h"
#include "hruntimedb.h"
#include "heventheader.h"
#include "hmdcclus.h"

class TH1F;
class TFile;
class TLine;



class HMdcPPEvtFilter : public HReconstructor  {
  protected:
  HRuntimeDb*        rtdb;
  HCategory*         catmHMdcSeg;   //! HMdcSeg Category
  HMdcSeg*           mdcseg;         //! seg data level
  HIterator*         mdcsegiter;     //! seg iterator

  HCategory*         catmHMdcClus;   //! HMdcClus Category
  HMdcClus*          mdcclus;         //! clus data level
  HIterator*         mdcclusiter;     //! clus iterator


  Char_t title[300];          //! histrogram title
  Char_t name[300];           //! histogram name
  Char_t xtitle[300];         //! histogram xaxis title
  Char_t ytitle[300];         //! histogram yaxis title
  Char_t ztitle[300];         //! histogram zaxis title

  TH1F*              histtanthetamult;
  TH1F*              histphidiff;
  TH1F*              histphidiffcut;
  TH1F*              histtanthetamultcut;
  TH2F*              histchithetacut;
  TH2F*              histchitheta;
  TH2F*              histchiphicut;
  TH2F*              histchiphi;

  TCanvas*           canv1; //! canvas for histogram
  TCanvas*           canv2; //! canvas for histogram
  TCanvas*           canv3; //! canvas for histogram

  TString histFileDir;
  TString histFileSuffix;
  TString histFileOption;
  TString histFile;
  TString psFile;
  TString flNmWoExt;         // input file name without extension

  Bool_t saveHist;
  Bool_t saveCanv;
  Bool_t savePSFile;

  Int_t flagcategory;
  Int_t sector[200];
  Float_t phishift[6];
  Float_t theta[200], phi[200];
  Float_t phidiff, tanthetamult;
  Float_t phimin, phimax;
  Float_t tanthetamin, tanthetamax;
  Float_t phiwidth, tanthetawidth;
  Float_t tanthetamean, phimean;
  Int_t multiplicity;       // multiplicity counter
  Int_t IsElasticFlag;
  Int_t TrigBit;
  Float_t chi2[200];

 void resetCounters()
 {
   multiplicity=0;
 }

 void resetArrays()
   {
     for(Int_t a=0; a<200; a++)
       {
	 phi[a]=0;
	 theta[a]=0;
	 chi2[a]=0;
	 sector[a]=0;
       }
   }


  private:

 TFile* openHistFile(const Char_t* flag);

  public:

    HMdcPPEvtFilter(const Text_t *name,const Text_t *title, Int_t flagcat=0);
    HMdcPPEvtFilter(void);
    ~HMdcPPEvtFilter(void);

    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);
    Int_t  execute(void);

    void createHists();
    void initVariables();
    void setThetaCuts(Float_t Theta, Float_t DTheta);
    void setPhiCuts(Float_t Phi, Float_t DPhi);
                void setSaveHists(Bool_t s = kTRUE){saveHist = s;};
                void setSaveCanv(Bool_t s = kTRUE){saveCanv = s;};
                void setSavePSFile(Bool_t s = kTRUE){savePSFile = s;};
                void setTrigBit(Int_t bit){TrigBit = bit;};
    void setHistFile(const Char_t* dir,const Char_t* suf,const Char_t* option="NEW");

    ClassDef(HMdcPPEvtFilter,0)
};

#endif
