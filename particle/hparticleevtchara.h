#ifndef  HPARTICLEEVTCHARA_H
#define  HPARTICLEEVTCHARA_H

#define SIZEOFARRAY(x)  (sizeof(x) / sizeof((x)[0]))
#define RESETARRAY(x)  memset(x, 0, sizeof(x));  // reset array to null


#include "TString.h"
#include "TFile.h"
#include "TROOT.h"
#include "TSystem.h"
#include <TNamed.h>
#include <TH1F.h>
#include <TString.h>
#include <TFile.h>
#include <TMath.h>
#include <TROOT.h>
#include <TH2F.h>
#include <TF1.h>
#include <TStyle.h>
#include <TGraphErrors.h>
#include <TTree.h>	 
#include <vector>
#include <TNtuple.h>
//#include <vector>
//#include <list>
#include "hades.h"
#include "htool.h"
#include "hphysicsconstants.h"
#include "hrootsource.h"
#include "hiterator.h"
#include "hloop.h"
#include "htime.h"

#include "haddef.h"
#include "heventheader.h"
#include "hreconstructor.h"
#include "hparticledef.h"
#include "hparticlestructs.h"
#include "hparticlecand.h"
#include "hparticlecandsim.h"

#include "hcategory.h"
#include "hcategorymanager.h"
#include "hparticleevtinfo.h"

#include "walldef.h"
#include "hwallhit.h"
//#include "hwallraw.h"


using namespace std;
class HParticleCand;
class HCategory;

class HParticleEvtChara : public HReconstructor {

public:

    HParticleEvtChara(const Text_t* name="HParticleEvtChara",const Text_t* title="HParticleEvtChara"); /// constructor
    ~HParticleEvtChara();  /// destructor
    Int_t   execute(void);
    Bool_t  init(void);
    Bool_t  finalize(){ return kTRUE;}

    void    setBeamTime(TString beamTime);
    void    setGeneration(TString generation);
    Bool_t  setParameterFile(TString ParameterFile);
    void    setEstimator(TString estimator)             {fCentralityEstimator = estimator;}
    void    useEstimator(TString estimator)             {fCentralityEstimator = estimator;}   // FIXME

    Int_t   getCentralityClass() const;                      // Return centrality classes with fixed Cuts
    Int_t   getCentralityClass(TString estimator) const;     // Return centrality classes with fixed Cuts
    Int_t   getCentralityClass(UInt_t buffer, TString estimator) const;

    void    printCentralityClass();
    void    printCentralityClass(TString estimator);


    Int_t   getNbins();
    Int_t   getNbins(TString estimator);
    
    Float_t getCentralityPercentile() const;
    Float_t getCentralityPercentile(TString estimator) const;   // Return centrality percentile [0.0-100.0%] of total cross section - Error is 101%

    Int_t   getCentralityClass5();                               // Return centrality classes in 5%  bins of total cross section
    Int_t   getCentralityClass5(TString estimator);              // Return centrality classes in 5%  bins of total cross section
    Int_t   getCentralityClass10();
    Int_t   getCentralityClass10(TString estimator);             // Return centrality classes in 10% bins of total cross section

    Float_t getFWSumChargeSpec(){return fFWSumChargeSpec;}
    Bool_t  calcFWSumChargeSpec(); 
    
    void    setReferenceMean(Float_t referenceMean)       {fReferenceMeanSelTrack = referenceMean;}
    void    setFWSumChargeSpec(Int_t x)                   {fFWSumChargeSpec=x;}
    void    setDirectivity(Float_t x)                     {fDirectivity=x;}
    void    setRatioEtEz(Float_t x)                       {fRatioEtEz=x;}
    void    setEt(Float_t x)                              {fEt=x;}
    
    void    Reset();
    Int_t   print();
    
private:

    Bool_t  loadParameterFile();

    HCategory *fParticleEvtInfoCat; // output category
    HCategory *fCatWallHit;

    
    TString fParameterFile;   // ParameterFile
    TFile *fFile;
    
    // -------------------------------------------------------------------------
    // flags
    Bool_t  fUseParameterFile;    //
    TString fRun;                 // beamtime
    TString fGen;                 // generation of data set
    TString fCentralityEstimator; // used CentralityEstimator
    Bool_t  isSimulation;         // for simulation if catGeantKine is available
    Int_t   fQuality;             // Quality of centrality determination  not Valid == -1, best == 0
    Float_t fFWSumChargeSpec;
    Float_t fDirectivity;
    Float_t fRatioEtEz;
    Float_t fEt;

    // -------------------------------------------------------------------------
    //  flags for HParticleEvtChara, CentralityEsitimator
    enum eCentralityEsitimator { 
        kTOFRPC                = 0x01,        // 1: 1     
        kTOF                   = 0x02,        // 2: 2     
        kRPC                   = 0x03,        // 3: 1     
        kTOFRPCtot             = 0x04,        // 4: 1     
        kTOFtot                = 0x05,        // 2: 2     
        kRPCtot                = 0x06,        // 1: 1     
        kSelectedTrack         = 0x07,        // 3: 4     
        kSelectedTrackCorr     = 0x08,        // 4: 8     
        kSelectedTrackNorm     = 0x09,        // 4: 8     
        kFWSumChargeSpec       = 0x10,        // 5: 16    
        kEt                    = 0x11,        // 6: 32    
    };
    //  CentralityClass
    enum eCentralityClass { 
        k5percentPT3           = 0x01,        // 1: 1     
        kpercentPT3            = 0x02,        // 2: 2     
        kFOPIPT3               = 0x03,        // 1: 1     
        k5percentPT2           = 0x04,        // 1: 1     
        kpercentPT2            = 0x05,        // 2: 2     
        kFOPIPT2               = 0x06,        // 1: 1     
    };


    // track running mean reference point
    Float_t fReferenceMeanSelTrack;
    
    TH1F    *fHtempTOFtimecut;      // histogram with centrality vs multiplicity using TOF
    TH1F    *fHtempRPC;             // histogram with centrality vs multiplicity using RPC
    TH1F    *fHtempTOFRPC;          // histogram with centrality vs multiplicity using TOFRPC
    TH1F    *fHtempTOFRPCtimecut;   // histogram with centrality vs multiplicity using TOFRPCtimecut
    TH1F    *fHtempSelecTrack;      // histogram with centrality vs multiplicity using SelectedTrack
    TH1F    *fHtempSelecTrackCorr;  // histogram with centrality vs multiplicity using SelectedTrackCorr
    TH1F    *fHtempSelecTrackNorm;
    TH1F    *fHtempFWSumChargeSpec;
    TH1F    *fHtempDirectivity;
    TH1F    *fHtempRatioEtEz;
    TH1F    *fHtempEt;
    
    TH1F    *hMultTOFRPCtimecut_5percent_fixedCuts;
    TH1F    *hMultTOFRPCtimecut_10percent_fixedCuts;
    TH1F    *hMultTOFRPCtimecut_FOPI_fixedCuts;
    
    ClassDef(HParticleEvtChara,0)

};

#endif
