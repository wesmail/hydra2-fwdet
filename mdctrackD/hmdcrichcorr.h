#ifndef HMDCRICHMATCH_H
#define HMDCRICHMATCH_H

#include "hreconstructor.h"
#include "TString.h"
#include "TF1.h"

class HCategory;
class HIterator;
class HMdcLookUpTb;
class HMdcCalibrater1;
class HMdcEvntListCells;
class TH1F;
class TFile;
class TLine;

class HMdcDGaus : public TF1 {
  private:
    Float_t condMean;   // if abs(new_mean-old_mean)<condMean*old_sigma -ok.!
    Float_t condSig1;   // if (new_sig-old_sig)<condSig1*old_sigma-ok.!
    Float_t condSig2;   // if (old_sig-new_sig)<condSig2*old_sigma-ok.!
  public:
    HMdcDGaus(void);
    HMdcDGaus(const Char_t* name, Double_t xmin, Double_t xmax);
    void setTestCond(Float_t cM,Float_t cS1,Float_t cS2) {
      condMean=cM;
      condSig1=cS1;
      condSig2=cS2;
    }
    Int_t histFit(TH1F* hst, Float_t  oMean, Float_t  oSig,
        Float_t& nMean, Float_t& nSig, Bool_t doTest=kTRUE);
    static Double_t dgaus(Double_t *x, Double_t *par);
    
  ClassDef(HMdcDGaus,0)
};

class HMdcRichCorr : public HReconstructor  {
  private:
    TString fileName;              // input file name with extension
    TString flNmWoExt;             // input file name without extension
    TString runId;
    TString eventDate;
    TString eventTime;

    HMdcCalibrater1* fCalib1;      // pointer to mdc calibrater1
    
    HMdcEvntListCells* pListCells; // list of fired wires in current event
    Bool_t isMdcLCellsOwn;         // =kTRUE if MdcEvntListCells is own

    HCategory* fClusCat;           // pointer to HMdcClus data category
    HIterator* iterMdcClus;        // iterator

    Int_t nMdcHits[6];             // num. of mdc hits in [sector]

    HCategory* fRichHitCat;        // pointer to HMatchURich or HRichHit categ.
    HIterator* iterRichHit;

    Int_t nRichHits[6];            // num. of rich hits in [sector]
    Float_t richTheta[6][400];     // rings param.
    Float_t richPhi[6][400];       //
    Int_t nMdcHitRing[6][400];     // kTRUE - ring

    Float_t radToDeg;              //

    HMdcLookUpTb* fLookUpTb;       // clus. finder for MDC1&2
    Int_t level4;                  // Level[seg] of 4-layers finding
    Int_t level5;                  // Level[seg] of 5-layers finding
    Int_t nLayers[6][4];           // Number of layers in modules [sec][mod]
    Int_t clFinderType;            // Cluster finder type

    // Matching cuts:
    Int_t nRingsCut;               // Cut for number of matched rings
    Float_t dThCuts[6][2][2];      // [sec][mod][] dTheta bounds (rich-mdc)
    Float_t dPhCuts[6][2][2];      // [sec][mod][] dPhi bounds (rich-mdc)
    Float_t numSigTh;              // +/-sig. cut for dTheta
    Float_t numSigPh;              // +/-sig. cut for dPhi
    
    Float_t planeMeanDTh[2];
    Float_t planeSigmaDTh[2];
    Float_t planeMeanDPh[2];
    Float_t planeSigmaDPh[2];

    // Statistics:
    Int_t nEvents;                 // Total number of events.
    Int_t nRingEvents;             // Number of events with rings
    Int_t nMatchedEvents;          // Number of matched events
    Int_t nMatchedEvents1R;        // Number of events with 1 matched rings
    
    //Histograms:
    TString histFileDir;
    TString histFileSuffix;
    TString histFileOption;
    TString histFile;
    TString psFile;
    TH1F* hDThAll[2];
    TH1F* hDPhAll[2];
    TH1F* hDTh[6][2];
    TH1F* hDPh[6][2];
    TH1F* hNRings;
    TH1F* hNMRings;
    TH1F* hNEvents;
    
    Bool_t saveHist;
    Bool_t saveCanv;
    Bool_t savePSFile;
    
    // Testins cuts param.:
    HMdcDGaus funDGaus;        // fit function
    Int_t fitStatDTh[6][2];    // 0-ok.!, 1-mean and/or sigma shifted, 
    Int_t fitStatDPh[6][2];    // 2-fit corrupted

  public:
    HMdcRichCorr(void);
    HMdcRichCorr(const Text_t *name,const Text_t *title, Int_t nRCut=1);
    ~HMdcRichCorr(void);

    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);
    Int_t  execute(void);
    void printStatus(void) const;

    void makeCalibrater1(Int_t vers=1, Int_t cut=1, Int_t domerge=0);

    void setNLayers(const Int_t *lst);
    void setTypeClFinder(Int_t type) {clFinderType=type;}
    void setLevel(Int_t l4s1, Int_t l5s1) {
      level4=l4s1;
      level5=l5s1;
    }

    void setDThetaCuts(Float_t dTh1, Float_t dTh2, Float_t nSig=3.);
    void setDPhiCuts(Float_t dPh1, Float_t dPh2, Float_t nSig=3.);
    void setDThetaCuts(const Float_t* dTh, Float_t nSig=3.);
    void setDPhiCuts(const Float_t* dPh, Float_t nSig=3.);
    
    void setDThetaCutsSig(Float_t mDTh,Float_t sDTh,Float_t nSig=3.);
    void setDPhiCutsSig(Float_t mDPh,Float_t sDPh,Float_t nSig=3.);
    void setDThetaCuts(const Float_t* mDTh,const Float_t* sDTh,Float_t nSig=3.);
    void setDPhiCuts(const Float_t* mDPh,const Float_t* sDPh,Float_t nSig=3.);
    
    void setHistFile(const Char_t* dir,const Char_t* suf,
                     const Char_t* option="NEW");
    
    void setTestCond(Float_t cM,Float_t cS1,Float_t cS2) {
      funDGaus.setTestCond(cM,cS1,cS2);
    }
    void setSaveFlags(Bool_t hist, Bool_t canv, Bool_t psfile) {
      saveHist=hist;
      saveCanv=canv;
      savePSFile=psfile;
    }

  private:
    void setup(void);
    void fillListByRichHit(void);
    Bool_t reinitMdcFinder(void);
    void findMdcClusters(void);
    void makeHist(void);
    TFile* openHistFile(const Char_t* flag);
    Float_t meanDTh(Int_t s, Int_t m) {
      return (dThCuts[s][m][0]+dThCuts[s][m][1])*0.5;
    }
    Float_t meanDPh(Int_t s, Int_t m) {
      return (dPhCuts[s][m][0]+dPhCuts[s][m][1])*0.5;
    }
    Float_t sigmaDTh(Int_t s, Int_t m) {
      return (numSigTh>0) ? (dThCuts[s][m][1]-dThCuts[s][m][0])/(2*numSigTh):0.;
    }
    Float_t sigmaDPh(Int_t s, Int_t m) {
      return (numSigPh>0) ? (dPhCuts[s][m][1]-dPhCuts[s][m][0])/(2*numSigPh):0.;
    }
    void calcAvMnSg(void);
    void drawCut(TLine* ln,Float_t mean,Float_t sigma,Float_t nSig,
        Int_t color, Double_t y);

  ClassDef(HMdcRichCorr,0)
};

#endif
