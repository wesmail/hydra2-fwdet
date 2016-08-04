#ifndef __HPARTICLETREE_H__
#define __HPARTICLETREE_H__

#include "TObject.h"
#include "TObjArray.h"

#include "hreconstructor.h"
#include "htree.h"
#include "hrecevent.h"
#include "hparticletracksorter.h"

#include <TFile.h>

#include <map>
#include <vector>
using namespace std;

class HParticleCand;

class HParticleTree : public HReconstructor
{
private:

    HRecEvent*         fCurrentEvent;      //! Event under reconstruction
    TFile*             fOutputFile;        //! File used to store the output tree
    HTree*             fTree;              //! Output tree
    Int_t              fCycleNumber;       //! cycle number of output file
    TString            fOutputFileName;    //! output file name (derived from hades output name)
    TString            fOutputTitle;       //! output file title
    TString            fOutputOption;      //! output file option
    Int_t              fOutputCompression; //! output file compression
    TString            fOutputFileSuffix;  //! replace .root by fOutputFileSuffix.root
    TString            fOutputDir;         //! outputdir
    Bool_t             fOutFound;          //!

    Bool_t             kSkipEmptyEvents;   //! kTRUE = skip not selected events (default = kFALSE)
    Bool_t             kSkipTracks;        //! kTRUE = skip not selected tracks (default = kTRUE)

    Int_t              kSwitchRICHMatching;
    Float_t            fRICHMDCWindow  ;

    Bool_t             kIgnoreRICH;
    Bool_t             kIgnoreInnerMDC;
    Bool_t             kIgnoreOuterMDC;
    Bool_t             kIgnoreMETA;
    Bool_t             kIgnorePreviousIndex;

    HParticleTrackSorter sorter;

    vector <Int_t>              fCatNums;           //! list of persistent output categories
    map    <TString,Int_t>      fmCatNameToNum;     //!
    map    <Int_t,TString>      fmCatNumToName;     //!
    map    <Int_t,HCategory*>   fmCatNumToPointer;  //!
    map    <TString,HCategory*> fmCatNameToPointer; //!
    map    <Int_t,Int_t>        fmCatNumToFullCopy; //!
    vector <Int_t>              fmCatNumSupport;    //!
    vector <Int_t>              fmCatNumFullCopySupport; //!

    Bool_t (*pUserSelectEvent)  (TObjArray* );     //! user provided function pointer event selection (TObjArray for parameters)
    Bool_t (*pUserSelectLeptons)(HParticleCand* ); //! user provided function pointer to lepton selection
    Bool_t (*pUserSelectHadrons)(HParticleCand* ); //! user provided function pointer to hadron selection
    Bool_t (*pUserKeepTrack)    (HParticleCand* ); //! user provided function pointer to keep/discard tracks independent of sorterflags

    TObjArray* fParamSelectEvent;                  //! object array for optional parameters of event selection function

    Bool_t  makeTree       (void);         // create output tree
    void    recreateOutput (void);         // open new output file if max filesize is reached
    void    closeOutput    (void);         // close output file
    void    setEvent       (void);         // setup the event layout
    void    extractMdcSeg(HParticleCand* cand, Bool_t isSim,Int_t segInd1,Int_t&segInd2);
    void    extractMdcCal1(Bool_t isSim,Int_t segInd);
    void    extractMdcCal1FromClus(Bool_t isSim,Int_t segInd);

    Int_t ctMdcSeg [6][2];                     //!
    Int_t ctMdcClus[6][2];                     //!
    Int_t ctMdcHit [6][4];                     //!
    Int_t ctRpcClus[6]   ;                     //!

    Bool_t doFullCopy(Cat_t cat);

public:

    HParticleTree(const Text_t *name = "",const Text_t *title ="");
    virtual ~HParticleTree();

    Bool_t  init();
    Int_t   execute();
    Bool_t  finalize();
    void    setSkipEmptyEvents (Bool_t skip) { kSkipEmptyEvents = skip; }
    void    setSkipTracks      (Bool_t skip) { kSkipTracks      = skip; }

    void    setOutputFile      (TString fname  = "filter_tree.root",
                                TString ftitle = "Filter",
                                TString fopt   = "RECREATE",
                                Int_t   fcomp  = 2
			       );
    void    setOutputFileSuffix(TString suff  = "filter_tree") { fOutputFileSuffix = suff;}
    void    setOutputDir       (TString dir   = "")            { fOutputDir        = dir ;}
    void    setEventStructure  (Int_t n,Cat_t PersistentCat[],Bool_t fullCopy=kFALSE);

    void    setIgnoreRICH         (Bool_t ignore = kTRUE) { kIgnoreRICH          = ignore;}
    void    setIgnoreInnerMDC     (Bool_t ignore = kTRUE) { kIgnoreInnerMDC      = ignore;}
    void    setIgnoreOuterMDC     (Bool_t ignore = kTRUE) { kIgnoreOuterMDC      = ignore;}
    void    setIgnoreMETA         (Bool_t ignore = kTRUE) { kIgnoreMETA          = ignore;}
    void    setIgnorePreviousIndex(Bool_t ignore = kTRUE) { kIgnorePreviousIndex = ignore;}
    void    setRICHMatching(HParticleTrackSorter::ERichMatch match, Float_t window = 4.) {
	kSwitchRICHMatching = match;
	fRICHMDCWindow      = window;
    }
    void    setUserSelectionEvent  (Bool_t (*function)(TObjArray*),TObjArray* ar) { pUserSelectEvent   = function; fParamSelectEvent = ar;}
    void    setUserSelectionLeptons(Bool_t (*function)(HParticleCand* )){ pUserSelectLeptons = function;}
    void    setUserSelectionHadrons(Bool_t (*function)(HParticleCand* )){ pUserSelectHadrons = function;}

    void    setUserkeepTrack(Bool_t (*function)(HParticleCand* )){ pUserKeepTrack = function;}

    ClassDef(HParticleTree,0)  // A task to create an fitered  root output
};


#endif // __HPARTICLETREE_H__
