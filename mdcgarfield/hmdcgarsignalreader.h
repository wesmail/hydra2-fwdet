#ifndef HMDCGARSIGNALREADER_H
#define HMDCGARSIGNALREADER_H
#include "TNamed.h"
#include "TObject.h"
#include "TH1.h"
#include "TPaveText.h"
#include "htool.h"

class TFile;
class TDirectory;
class TH1F;
class TString;

class HMdcGarSignalReader : public TNamed {
protected:

    FILE*    inputAscii;                  //! file pointer to input
    TString  fNameAsciiIn;                //! file name of root input file
    TFile*   outputRoot;                  //! root file pointer
    TString  fNameRootOut;                //! root file pointer
    Int_t    nSignals;                    //! number of signals per sample point
    Int_t    nBinSignal;                  //! number of bins of signal hist
    Float_t  rangeSignal;                 //! upperrange signal hist
    Int_t    nBinDist;                    //! number of bins of hist drift time vs distance
    Float_t  binSizeDist;                 //! size of bin hist drift time vs distance (mm)
    Int_t    nStep;                       //! number of threshold steps
    Float_t  stepsize;                    //! size of threshold step
    Float_t  minSignal[4];                // maximum charge per module type
    Int_t    version;                     //! read different GARFIELD versions (704,708 (skip))
    TH1F*    hdist_1[10];                 //! distribution of time1 per threshold step
    TH1F*    hdist_2[10];                 //! distribution of time2 per threshold step
    TH1F*    htime1 [10];                 //! hist for time1 vs distance per threshold step
    TH1F*    htime2 [10];                 //! hist for time2 vs distance per threshold step
    TH1F*    hminCharge   [4];            //! hist for minimum charge per mdc type
    TH1F*    hminChargeSum[4];            //! hist for minimum charge from sum hist per mdc type
    TPaveText* myconfig;                  //! configuration discription
    Int_t    nmake;                       //! counter for calling make
    TH1F*    hresponse;                   //! response hist
    Bool_t   simResponse;                 //! switch simulate response of adc / simple integration
    Float_t sigma1;                       //! sigma 1 of reponse function of adc
    Float_t sigma2;                       //! sigma 2 of reponse function of adc

    Bool_t   writehists[6];               //! switch write on/off for signal

    void    initVariables();
    void    initHistArrays()
    {
	for(Int_t i=0;i<getNStep();i++)
	{
	    htime1[i] =0;
            htime2[i] =0;
            hdist_1[i]=0;
            hdist_2[i]=0;
	}
    }
    void    createHistArrays(Int_t,Int_t,Int_t,Int_t);
    void    writeHistArrays (Int_t type)
    {
	for(Int_t i=0;i<getNStep();i++)
	{
	    if(type==1 || type==3)
	    {
		writeHist(htime1 [i],4);
		writeHist(htime2 [i],4);
	    }
	    if(type==2 || type==3)
	    {
		writeHist(hdist_1[i],3);
		writeHist(hdist_2[i],3);
	    }
	}
    }
    void    deleteHistArrays(Int_t type)
    {
	for(Int_t i=0;i<getNStep();i++)
	{
            if(type==1 || type==3)
	    {
		HTool::deleteObject(htime1 [i]);
		HTool::deleteObject(htime2 [i]);
	    }
	    if(type==2 || type==3)
	    {
		HTool::deleteObject(hdist_1[i]);
		HTool::deleteObject(hdist_2[i]);
	    }
	}
    }
    void    writeHist     (TH1F* h,Int_t i)
    {
	if(h&&writehists[i])
	{
	    h->Write();
	}
    }
    void    readInput     (Int_t,Int_t);
    void    integrateHists(Int_t,Int_t,Int_t,Int_t,TH1F*,TH1F*);
    void    findThresholds(Int_t,TH1F*);
    void    fillDriftTimeVersusDistance(Int_t,Int_t,Int_t);
    Bool_t  check(Int_t,Int_t,Int_t,Int_t,TH1F*);
    void    createHists(Int_t,Int_t,Int_t,TH1F**,TH1F**,TH1F**,TH1F**);
    void    createChargeHists();
    void    fillChargeHists(Int_t,Int_t,Int_t,Int_t,Int_t,TH1F*,TH1F*);
    void    writeChargeHists();
    void    deleteChargeHists();
    void    response(Float_t mean,Float_t scale);
    void    simulateResponse(TH1F*,TH1F*);
public:
    HMdcGarSignalReader(const Char_t* name="",const Char_t* title="");
    ~HMdcGarSignalReader  ();
    void    setVersion(Int_t vers){if(vers==704||vers==708){version=vers;}else{Warning("serVersion()","Unknown version! Use 704 (old,default) or 708 (skip)!");}}
    void    setFileNameOut(TString);
    void    setFileNameIn (TString);
    void    setSigma1(Float_t sig){sigma1=sig;}
    void    setSigma2(Float_t sig){sigma2=sig;}
    void    setSimResponse(Bool_t sim){simResponse=sim;}
    Float_t getSigma1(){return sigma1;}
    Float_t getSigma2(){return sigma2;}
    void    setNSignals   (Int_t i)   {nSignals=i;}
    Int_t   getNSignals   ()          {return nSignals;}
    void    setNBinSignal (Int_t i)   {nBinSignal=i;}
    Int_t   getNBinSignal ()          {return nBinSignal;}
    void    setRangeSignal(Float_t i) {rangeSignal=i;}
    Float_t getRangeSignal()          {return rangeSignal;}
    void    setNBinDist   (Int_t i)   {nBinDist=i;}
    Int_t   getNBinDist   ()          {return nBinDist;}
    void    setBinSizeDist(Float_t i) {binSizeDist=i;}
    Float_t getBinSizeDist()          {return binSizeDist;}
    void    setNStep      (Int_t i)   {nStep=i;}
    Int_t   getNStep      ()          {return nStep;}
    void    setStepSize   (Float_t i) {stepsize=i;}
    Float_t getStepSize   ()          {return stepsize;}
    void    setMinSignal  (Float_t i,Float_t j,Float_t k,Float_t l)
    {
	minSignal[0]=i;
        minSignal[1]=j;
        minSignal[2]=k;
        minSignal[3]=l;
    }
    Float_t getMinSignal  (Int_t i)   {return minSignal[i];}
    void    setWriteHists (Bool_t signal,Bool_t signalint,Bool_t sum,Bool_t dist,Bool_t timedistance,Bool_t mincharge)
    {
	writehists[0]=signal;
	writehists[1]=signalint;
	writehists[2]=sum;
        writehists[3]=dist;
        writehists[4]=timedistance;
        writehists[5]=mincharge;
    }
    void    printStatus   (void);
    void    make          (Int_t,Int_t,TString);
    void    closeOutput   ();
    ClassDef(HMdcGarSignalReader,1) // Class for producing the MDC Cal2 parameters
};
#endif  /*!HMDCGARSIGNALREADER_H*/
