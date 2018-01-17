//*-- Author : Jochen Markert  18.07.2007

#ifndef  __HPARTICLETRACKSORTER_H__
#define  __HPARTICLETRACKSORTER_H__

#include "hcategory.h"
#include "hiterator.h"
#include "hrecevent.h"

#include <vector>
#include "TString.h"
#include "TFile.h"
#include "TNtuple.h"


using  std::vector;
using  std::iostream;

# define SORTER_NUMBER_OF_INDEX 7
# define SORTER_NUMBER_OF_QUALITY_CRITERIA 7

class HParticleCand;

typedef struct {
    HParticleCand* cand;
    Short_t  ind_Cand;
    Short_t  ind_RICH   ;
    Short_t  ind_innerMDC;
    Short_t  ind_outerMDC;
    Short_t  ind_TOF;
    Short_t  ind_SHOWER;
    Short_t  ind_RPC;
    Short_t  selectedMeta;
    Int_t    nPadsRich;
    Short_t  sec;
    Short_t  MetaModule;
    Short_t  MetaCell;
    Float_t  mom;
    Float_t  beta;
    Float_t  MDC_dEdx;
    Float_t  innerMDCChi2;
    Float_t  outerMDCChi2;
    Short_t  nMdcLayerInnerSeg;
    Short_t  nMdcLayerOuterSeg;
    Float_t  RK_META_match_Quality;
    Float_t  RK_META_match_Radius;
    Float_t  RK_META_dx;
    Float_t  RK_META_dy;
    Float_t  RK_META_match_Shr_Quality;
    Float_t  RK_RICH_match_Quality;
    Float_t  RK_Chi2;
    Bool_t   RICH_RK_Corr;
    Int_t    userSort1;
    Int_t    userSort2;
    Int_t    userSort3;

} candidateSort;

class HParticleTrackSorter : public TNamed {

public:

    enum ERichMatch {
	kUseRICHIndex         = 0,   // require RICH valid index
	kUseRKRICHCorrelation = 1,   // require RK-RICH correlation (from HPidTrackCand)
	kUseRKRICHWindow      = 2    // require RK-RICH matching inside +- window
    };

    enum ESelect {
	kIsLepton = 0,
	kIsHadron = 1
    };

    enum ESwitch {
	kIsIndexRICH         =  0,    //
	kIsIndexInnerMDC     =  1,    //
	kIsIndexOuterMDC     =  2,    //
	kIsIndexTOF          =  3,    //
	kIsIndexSHOWER       =  4,    //
        kIsIndexRPC          =  5,    //
        kIsIndexMETA         =  6,    //

	kIsBestHitRICH       =  7,    //  (by chi2)
	kIsBestHitInnerMDC   =  8,    //  (by chi2)
	kIsBestHitOuterMDC   =  9,    //  (by chi2)
        kIsBestHitMETA       = 10,    //  (by RK META match quality)
	kIsBestRK            = 11,    //  (by RK chi2, none fitted outer segments with lower priority)
        kIsBestRKRKMETA      = 12,    //  (by RK chi2 * RK META match quality)
        kIsBestRKRKMETARadius= 13,    //  (by RK chi2 * RK META match radius)
        kIsBestUser          = 14     //  (by userFunction)
    };


protected:

    static Bool_t  kDebug;
    static Int_t   printLevel;             //! higher -> more prints (1,2,3)
    static Int_t   kSwitchIndex;           //! switch to select sort by index
    static Int_t   kSwitchQuality;         //! switch to select sort by quality algorithms
    static Int_t   kSwitchParticle;        //! switch to select leptons/hadrons
    static Int_t   kSwitchRICHMatching;    //! switch to select RICH/MDC matching in lepton selection
    static Float_t fRICHMDCWindow  ;       //! matching window RICH/MDC in phi/theta (symmetric, degree)

    static Bool_t kIgnoreRICH;             //! switch to ignore RICH hits for Double_t hit counting
    static Bool_t kIgnoreInnerMDC;         //! switch to ignore InnerMDC hits for Double_t hit counting
    static Bool_t kIgnoreOuterMDC;         //! switch to ignore OuterMDC hits for Double_t hit counting
    static Bool_t kIgnoreMETA;             //! switch to ignore META hits for Double_t hit counting
    static Bool_t kIgnorePreviousIndex;    //! switch to ignore indices from previoius marked ued objects
    static Bool_t kUseYMatching;           //! switch to use Y matching to meta cell (default kTRUE)
    static Bool_t kUseYMatchingScaling;    //! switch to use 1/p scaling of Y matching to meta cell (default kTRUE)
    static Bool_t kUseBeta;                //! switch to use Beta (in lepton select use fBetaLepCut, in hadron beta>0) (default kTRUE)
    static Bool_t kUseFakeRejection;       //! switch to use fakerejection in build in select functions  (default kTRUE)
    static Bool_t kUseMETAQA;              //! switch to use metaqa (normed dx) in build in select functions  (default kTRUE)
    static Float_t fMetaBoundary;          //! matching window when using Y matching with metacell [mm] (default 3): fabs(dy) < (halfcell+metaBoundary)
    static Float_t fBetaLepCut;            //! lower beta cut for lepton selection (default 0.9)
    static Float_t fMETAQACut;             //! meta qa (normed dx) cut (default 3.)
    static Float_t fTOFMAXCut;             //! max tof [ns] allowed to match (when beta is used)

    Int_t   kSwitchRICHMatchingBackup;    //! switch to select RICH/MDC matching in lepton selection
    Float_t fRICHMDCWindowBackup  ;       //! matching window RICH/MDC in phi/theta (symmetric, degree)

    Bool_t kIgnoreRICHBackup;             //! switch to ignore RICH hits for Double_t hit counting
    Bool_t kIgnoreInnerMDCBackup;         //! switch to ignore InnerMDC hits for Double_t hit counting
    Bool_t kIgnoreOuterMDCBackup;         //! switch to ignore OuterMDC hits for Double_t hit counting
    Bool_t kIgnoreMETABackup;             //! switch to ignore META hits for Double_t hit counting
    Bool_t kIgnorePreviousIndexBackup;    //! switch to ignore indices from previoius marked ued objects
    Bool_t kUseYMatchingBackup;           //! switch to use Y matching to meta cell (default kTRUE)
    Bool_t kUseYMatchingScalingBackup;    //! switch to use 1/p sclaing of Y matching to meta cell (default kTRUE)
    Bool_t kUseBetaBackup;                //! switch to use Beta (in lepton select use fBetaLepCut, in hadron beta>0)  (default kTRUE)
    Bool_t kUseFakeRejectionBackup;       //! switch to use Beta (in lepton select use fBetaLepCut, in hadron beta>0)  (default kTRUE)
    Bool_t kUseMETAQABackup;              //! switch to use metaqa (normed dx) in build in select functions  (default kTRUE)
    Float_t fMetaBoundaryBackup;          //! matching window when using Y matching with metacell [mm] (default 3): fabs(dy) < (halfcell+metaBoundary)
    Float_t fBetaLepCutBackup;            //! lower beta cut for lepton selection  (default 0.9)
    Float_t fMETAQACutBackup;             //! meta qa (normed dx) cut (default 3.)
    Float_t fTOFMAXCutBackup;             //! max tof [ns] allowed to match (when beta is used)

    Bool_t (*pUserSortBackup)(candidateSort*, candidateSort*); //! user provided function pointer to sort algo

    Int_t  fill_Iteration;                 //! remember the number of fill() calls
    Int_t  selectBest_Iteration;           //! remember the number of selectBest() calls
    UInt_t  currentEvent;                   //! find new event

    vector < candidateSort *> all_candidates;  //!

    vector < Int_t > index_RICH;           //!
    vector < Int_t > index_InnerMDC;       //!
    vector < Int_t > index_OuterMDC;       //!
    vector < Int_t > index_SHOWER;         //!
    vector < Int_t > index_TOFHit;         //!
    vector < Int_t > index_TOFClst;        //!
    vector < Int_t > index_RPC;            //!
    vector < Int_t > old_flags;            //! remember the old flags of HParticleCand for restoring
    vector < Int_t > old_flags_emc;        //! remember the old flags of HEmcCluster for restoring

    TString*   nameIndex  ;                //!
    TString*   nameQuality;                //!

    Bool_t     isSimulation;               //!
    TFile*     fout;                       //!
    TNtuple*   nt;                         //!
    HCategory* pParticleCandCat;           //! HParticleCand category
    HCategory* pEmcClusterCat;             //! HEmcCluster category
    HIterator* iterParticleCandCat;        //! iterator on HParticleCand
    HRecEvent* fEvent;                     //! pointer to local event under reconstruction


    static Bool_t (*pUserSort)(candidateSort*, candidateSort*); //! user provided function pointer to sort algo

    void   printCand        (candidateSort*, Int_t, TString spacer = "    ");
    static Bool_t cmpIndex  (candidateSort*, candidateSort*);
    static Bool_t cmpQuality(candidateSort*, candidateSort*);
    Bool_t rejectIndex      (candidateSort*, HParticleTrackSorter::ESwitch, Int_t&);
    Bool_t rejectQuality    (candidateSort*, HParticleTrackSorter::ESwitch);
    Int_t  flagAccepted     (vector<candidateSort*>&, HParticleTrackSorter::ESwitch);
    Int_t  flagDouble       (vector<candidateSort*>&, HParticleTrackSorter::ESwitch);
    Int_t  setFlagsDouble   (vector<candidateSort*>&, HParticleTrackSorter::ESwitch);
    Int_t  clearVector      (vector<candidateSort*>&);
    Int_t  fillInput        (vector<candidateSort*>&);
    void   selection(Bool_t (*function)(HParticleCand* ));
    Int_t  fillAndSetFlags  ();
    Bool_t flagEmcClusters();

    void   clear(void);
public:
    HParticleTrackSorter(void);
    HParticleTrackSorter(TString name,TString title);
    ~HParticleTrackSorter(void);
    Bool_t init     (HRecEvent* evt=0);
    Bool_t finalize (void);
    // setup selections
    static void   setDebug              (Bool_t debug = kTRUE)  { kDebug               = debug;}
    static void   setIgnoreRICH         (Bool_t ignore = kTRUE) { kIgnoreRICH          = ignore;}
    static void   setIgnoreInnerMDC     (Bool_t ignore = kTRUE) { kIgnoreInnerMDC      = ignore;}
    static void   setIgnoreOuterMDC     (Bool_t ignore = kTRUE) { kIgnoreOuterMDC      = ignore;}
    static void   setIgnoreMETA         (Bool_t ignore = kTRUE) { kIgnoreMETA          = ignore;}
    static void   setIgnorePreviousIndex(Bool_t ignore = kTRUE) { kIgnorePreviousIndex = ignore;}
    static void   setUseYMatching       (Bool_t use    = kTRUE) { kUseYMatching        = use   ;}
    static void   setUseYMatchingScaling(Bool_t use    = kTRUE) { kUseYMatchingScaling = use   ;}
    static void   setUseBeta            (Bool_t use    = kTRUE) { kUseBeta             = use   ;}
    static void   setUseFakeRejection   (Bool_t use    = kTRUE) { kUseFakeRejection    = use   ;}
    static void   setUseMETAQA          (Bool_t use    = kTRUE) { kUseMETAQA           = use   ;}
    static void   setPrintLevel         (Int_t level)           { printLevel           = level;}
    static void   setUserSort(Bool_t    (*function)(candidateSort*, candidateSort*)){ pUserSort = function;}
    static void   setRICHMatching       (HParticleTrackSorter::ERichMatch match, Float_t window = 4.);
    static void   setMetaBoundary       (Float_t savety = 3.5) { fMetaBoundary = savety;}
    static void   setBetaLeptonCut      (Float_t cut = .9) { fBetaLepCut = cut;}
    static void   setMETAQACut          (Float_t cut = 3.) { fMETAQACut = cut;}
    static void   setTOFMAXCut          (Float_t cut = 60.){ fTOFMAXCut = cut;}
    static Float_t getMetaBoundary()    {return fMetaBoundary;}
    static Float_t getBetaLeptonCut()   {return fBetaLepCut;}

    // control selections
    void          resetFlags(Bool_t flag = kTRUE,Bool_t reject = kTRUE,Bool_t used = kTRUE,Bool_t lepton = kTRUE);
    void          cleanUp(Bool_t final = kTRUE);
    Int_t         fill(Bool_t (*function)(HParticleCand* ));
    void          backupFlags(Bool_t onlyFlags=kTRUE);
    Bool_t        restoreFlags(Bool_t onlyFlags=kTRUE);
    void          backupSetup();
    Bool_t        restoreSetup();
    Int_t         selectBest(HParticleTrackSorter::ESwitch byQuality, Int_t byParticle);
    static Bool_t selectLeptons          (HParticleCand* pcand = 0);
    static Bool_t selectHadrons          (HParticleCand* pcand = 0);
    static Bool_t selectLeptonsNoOuterFit(HParticleCand* pcand = 0);
    static Bool_t selectHadronsNoOuterFit(HParticleCand* pcand = 0);
    vector < candidateSort *>& getCandidates() { return  all_candidates;}
    void  printEvent(TString comment);
    ClassDef(HParticleTrackSorter,0);
};
#endif /* !__HPARTICLETRACKSORTER_H__ */







