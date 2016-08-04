#ifndef HMDCGAREFFMAKER_H
#define HMDCGAREFFMAKER_H
#include "TNamed.h"
#include "TObject.h"
#include "TH1.h"

class TFile;
class TDirectory;
class TH1D;
class TString;

class HMdcGarEffMaker : public TNamed {
protected:

    FILE*    outputAscii;        //! file pointer to output
    TString  fNameAsciiOut;      //! file name ascii out
    TFile*   inputRoot;          //! root file pointer
    TString  fNameRootIn;        //! file name root un file pointer
    TFile*   outputRoot;         //! root file pointer
    TString  fNameRootOut;       //! file name root un file pointer

    Float_t  minSignal[4];       //! minimum charge per mdc type
    static Int_t colors[10];     //! colors of hists
    Int_t    nSamples;           //! number of sample points per angle
    Int_t    nSteps;             //! number of threshold steps
    TH1D*    heff_time1;         //! [mdc][angle]collected charge as function of distance to wire
    TH1D*    heff_max;           //! [mdc] maximum collected charge as function of angle
    TH1D*    heff_max_distance;  //! [mdc] distance of maximum charge as function of angle
    TH1D*    heff_cut[10];       //! [step][mdc] maximum distance for a given threshold charge as function of angle
    TH1D*    heff_cut_trend;     //! [mdc][angle] distance as function of threshold charge per angle
    Double_t fitpars[4][18][2];  //! fitparameters for hists
    Int_t version;               //! old/new format
    void    initVariables ();
    void    createHists   (Int_t,Int_t,Int_t);
    void    writeHists    (Int_t);
    void    deleteHists   (Int_t);
    TH1F*   getHist       (TFile*,TString);
    void    fillEffHist   (Int_t,Int_t);
    void    fillCutHist   (Int_t,Int_t);
    void    fillMaxHists  (Int_t);
    void    fillTrendHists(Int_t);
    void    fitTrendHists (Int_t,Int_t);
    void    readInput     ();
    void    writeAscii    ();
    void    resetParams   ()
    {
	for(Int_t mdc=0;mdc<4;mdc++){
	    for(Int_t angle=0;angle<18;angle++){
		for(Int_t pars=0;pars<2;pars++){
                 fitpars[mdc][angle][pars]=0;
		}
	    }
	}
    }
public:
    HMdcGarEffMaker(const Char_t* name="",const Char_t* title="");
    ~HMdcGarEffMaker  ();
    void    setFileNameRootOut (TString);
    void    setFileNameAsciiOut(TString);
    void    setFileNameRootIn  (TString);
    void    setMinSignal  (Float_t i,Float_t j,Float_t k,Float_t l)
    {
	minSignal[0]=i;
	minSignal[1]=j;
	minSignal[2]=k;
	minSignal[3]=l;
    }
    Float_t getMinSignal  (Int_t i) {return minSignal[i];}
    void    setVersion(Int_t vers)  {version=vers;}
    Int_t   getVersion()            {return version;}
    void    setNSamples(Int_t i)    {nSamples=i;}
    Int_t   getNSamples()           {return nSamples;}
    void    setNSteps(Int_t i)      {nSteps=i;}
    Int_t   getNSteps()             {return nSteps;}
    void    printStatus   (void);
    void    make          ();
    ClassDef(HMdcGarEffMaker,1) // Class for producing the MDC Cal2 parameters
};
#endif  /*!HMDCGAREFFMAKER_H*/
