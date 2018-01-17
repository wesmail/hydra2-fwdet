//*-- Author : Jochen Markert 18.07.2007

#include "hparticletracksorter.h"
#include "hparticletool.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hparticlecand.h"
#include "hemccluster.h"
#include "hparticledef.h"
#include "emcdef.h"
#include "htool.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;
#define DEBUG 0

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleTrackSorter
//
// This Class loops over the HParticleCand category and
// analyze the objects. If the ECAL detector is used a loop over
// HEmcCluster is performed too to mark the clusters used by selected
// candidates. The Purpose of the procedure is to categorize
// the objects in the case of multiple usage of the single detector
// hits (RICH, InnerMDC, OuterMDC, META) as Double_t hit and to provide
// a hint which of the candidates is the best with respect to certain
// criteria (described by the bit flags below). If a HParticleCand
// object has been marked before by an other task with HParticleCand::kIsUsed it
// will be ignored by the sort process and stay unchanged. For the description
// of the differnt set/get functions of the HParticleCand objects see the
// documentation.
// To use HParticleTrackSorter without changing the flags of the HParticleCand objects
// (non persitent way inside a macro or reconstructor) one can call the
// backupFlags() function before and modification of the flags and
// restore the flags by calling restoreFlags() after the privat work has been
// done.
//
// For debugging purpose one cand set screen printouts to check the sorting
// (setDebug() + setPrintLevel(Int_t level)(level = 1-3)).
//
// The analysis of the HParticleCand objects is done in several steps:
//
// 1. Create the HParticleTrackSorter object and call the HParticleTrackSorter::Init()
//    To initialze the sorter object with category pointers etc ...
// 2. The internal structures are filled per event from the HParticleCand
//    category by calling the
//    Int_t HParticleTrackSorter::fill(Bool_t (*function)(HParticleCand* ))
//    function which expects a function pointer to a function taking an
//    HParticleCand pointer as argument and returning a Bool_t if the conditions on
//    the HParticleCand object are fullfilled (like for example in
//    Bool_t HParticleTrackSorter::selectHadrons(HParticleCand* pcand). For objects
//    failing the test the kIsRejected bit is set. The user can provide
//    his own selection functions to replace the build in selectLeptons(.....)
//    and selectHadrons(....) select functions.
//    The function pointer can be a pointer to a global function or member function
//    of an Object for example:
//
//    Bool_t select(HParticleCand* cand){             // global function
//             if ( put all selection criteria here ) return kTRUE;
//             else                                   return kFALSE;
//    } or
//
//    static Bool_t dummy::select(HParticleCand* cand){ // member function of object dummy
//                                                      // needs to be declared static !
//             if ( put all selection criteria here ) return kTRUE;
//             else                                   return kFALSE;
//    }
//    would be called in the code like
//
//    dummy d;
//    HParticleCand* p= new HParticleCand() // just to get an object
//    HParticleTrackSorter sorter;               // create the sorter object
//    // now we have 3 possibilities to call the
//    // test function
//    1. sorter.fill(select)        // global function
//    2. sorter.fill(dummy::select) // member function of object dummy (static call without object creation)
//    3. sorter.fill(d.select)      // member function of real object dummy
// 3. For all accepted objects the quality criteria and Double_t hit flags are
//    evaluated in within the selected sample and flagged to the HParticleCand objects.
//    The objects which fullfill the given minimum criteria the kIsAcceptedXXX bit is
//    flagged. For the RICH, InnerMDC, OuterMDC and META the kIsDoubleHitXXX bit are set
//    if the same detector hit index has been used more than once in the sample.
//    For the double hit rejection of Meta hits only the selected Meta hits
//    (ParticleCand::getSelectedMeta()) are taken into account. In the case of RPCClst+SHOWERHIT
//    The shower hit might be used by more than one candidate.
// 4. With the call of
//    Int_t HParticleTrackSorter::selectBest(HParticleTrackSorter::ESwitch byQuality, Int_t byParticle)
//    the final selection on the objects is performed. The by the fill() procedure
//    selected objects are sorted by the selected quality criteria. Starting from the
//    best object one goes down the list and marks all objects with the kIsUsed and
//    kIsBestXXX bit if they do not reuse an already used detector hit index
//    (Double_t hit rejection).
//    For leptons the kIsLepton bit is flagged in addition. Individual detector hits
//    can be ignored in the selection procedure with the setIgnoreXXX(kTRUE) functions
//    (by default all hits are taken into account).
//    HParticleTrackSorter::ESwitch byQuality can be used as quality criteria switch for
//
//          HParticleTrackSorter::kIsBestHitRich          (by number of pads)
//          HParticleTrackSorter::kIsBestHitInnerMdc      (by chi2, non fitted segments last)
//	    HParticleTrackSorter::kIsBestHitOuterMdc      (by chi2, non fitted segments last)
//          HParticleTrackSorter::kIsBestHitMeta          (by RK metamatch quality (or metaMatch quality if no RK , lower proirity))
//          HParticleTrackSorter::kIsBestRK               (by RK chi2, none fitted outer segments with lower priority)
//          HParticleTrackSorter::kIsBestRKRKMETA         (by RK chi2 * RK META match quality)
//          HParticleTrackSorter::kIsBestRKRKMETARadius   (by RK chi2 * RK META match radius)
//          HParticleTrackSorter::kIsBestUser             (by user provided sort function, no default function!)
//
//    user provide sort function:
//    static void   setUserSort(Bool_t    (*function)(candidateSort*, candidateSort*)
//    The user has to provide a function for sorting of the candidates:
//
//    Bool_t mysort(candidateSort* a, candidateSort* b) {
//      // this example does the same as kIsBestRKRKMETA
//      // sort in ascending order by RK_Chi2 * RK_META_match_Quality
//      // the function can use all data members of struct candidateSort defined in hparticletracksorter.h
//      return (a->RK_Chi2 * a->RK_META_match_Quality)  < (b->RK_Chi2 * b->RK_META_match_Quality);
//    }
//
//
// 5. The scheme allows for multiple selection following each other as
//    the already used objects are excluded from the next steps (for example first
//    select leptons and in the second iteration hadrons). The detector hit indicies
//    of the previous selection of kIsUsed marked objects are added to list of used
//    indicies by default not allowing to use a detector hit twice. To skip that
//    memory one can set setIgnorePreviousIndex(kTRUE).
// 6. After the analysis call HParticleTrackSorter::finalize(). After the selection
//    the user has to take into account only HParticleCand objects which  have
//    been flagged with kIsUsed. The objects slected by the Lepton selection will
//    be marked in addition with kIsLepton to identify them easily.
// 7. For lepton selections the internal selectLepton() function cand be manipulated.
//    With setRICHMatching(HParticleTrackSorter::EMatch ,Float_t window) the matching between
//    inner MDC and RICH hit can be selected (default: HParticleTrackSorter::kUseRKRICHWindow,4 degree ).
// 8. For Better suppression of background the build in select functions can apply
//    a spacial match to the meta cell in y coordinate. setUseYMatching(kTRUE) (default)
//    makes use of a match in y in within 3 mm. ( see HParticleTool::isGoodMetaCell(HParticleCand* c,Float_t bound))
// 9. For Better suppression of background the build in select functions can require a valid beta measurement (beta>0)
//    setUseBeta(kTRUE) (default), for leptons in addition a low cut in beta can be set setBetaLetonCut(0.9) (default)
//10. For Better suppression of background the build in select functions can suppress fake measurement
//    setUseFakeRejection(kTRUE) (default)
//
//
//    A selection of unique lepton and hadron tracks could look like
//    the following example :
//
// void myMacro(Int_t nEvents,TString inputDir, TString inFile)
// {
//    Hades* myHades = new Hades;
//    gHades->setQuietMode(2);
//
//    HRootSource* source = new HRootSource();
//    source->setDirectory(((Text_t *)inputDir.Data()));
//    source->addFile((Text_t *)inFile.Data());
//
//    myHades->setDataSource(source);
//
//    if(!myHades->init()){
//	cout<<"Hades Init() failed!"<<endl;
//	exit(1);
//    }
//
//
//    //------------------------------------------------------------------------
//    // get category ointers and iterators
//    HCategory* pidCat  = (HCategory*)gHades->getCurrentEvent()->getCategory(catParticleCand);
//    HIterator* iter    = NULL;
//    if(pidCat)
//    {
//         iter = (HIterator *)pidCat->MakeIterator("native");
//    } else {
//        cout<<"Category Pointer is NULL!"<<endl;
//	exit(1);
//    }
//    //------------------------------------------------------------------------
//
//    --------------------------------------------------------------------------------------------
//    //--------------------------CONFIGURATION---------------------------------------------------
//    //At begin of the program (outside the event loop)
//    HParticleTrackSorter sorter;
//    //sorter.setDebug();                                            // for debug
//    //sorter.setPrintLevel(3);                                      // max prints
//    //sorter.setRICHMatching(HParticleTrackSorter::kUseRKRICHWindow,4.); // select matching RICH-MDC for selectLeptons() function
//    //sorter.setUseYMatching(kTRUE);                                // use meta cell build in select functions
//    //sorter.setMetaBoundary(4.0);                                   // match in 4 mm to meta cell
//    //sorter.setUseBeta(kTRUE);                                     // require beta >0 in build in select functions
//    //sorter.setTOFMAXCut( 60.);                                    // maximum tof [ns] allowd for match ( if beta is used)
//    //sorter.setBetaLeptonCut(0.9);                                 // lower beta cut for lepton select
//    //sorter.setUseFakeRejection(kTRUE);                            // reject fakes in  build in select functions (default kTRUE)
//    //sorter.setUseMETAQA(kTRUE);                                   // metaqa in  build in select functions (default kTRUE)
//    //sorter.setMETAQACut(3.0);                                     // metaqa cut for ahdrons+leptons
//    //sorter.setIgnoreInnerMDC();                                   // do not reject Double_t inner MDC hits
//    //sorter.setIgnoreOuterMDC();                                   // do not reject Double_t outer MDC hits
//    //sorter.setIgnoreMETA();                                       // do not reject Double_t META hits
//    //sorter.setIgnorePreviousIndex();                              // do not reject indices from previous selctions
//    sorter.init();                                                  // get catgegory pointers etc...
//    --------------------------------------------------------------------------------------------
//
//
//    //------------------------------------------------------------------------
//    // Event loop
//    for(Int_t i = 1;i < nEvents; i ++)
//    {
//        //----------break if last event is reached-------------
//        if(!gHades->eventLoop(1)) break;
//
//        //------------------------------------------------------------------------
//        // run your selection and set all flags
//
//        //------------------------------------------------------------------------
//        // clean vectors and index arrays
//        sorter.cleanUp();
//        //------------------------------------------------------------------------
//
//        // if you do not want to modify the flags
//        // for the following tasks etc (needs call of
//        // restoreFlags() at the end of the eventloop)
//        //sorter.backupFlags();
//
//        sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);     // reset all flags for flags (0-28) ,reject,used,lepton
//        Int_t nCandLep     = sorter.fill(HParticleTrackSorter::selectLeptons);   // fill only good leptons
//        Int_t nCandLepBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
//        Int_t nCandHad     = sorter.fill(HParticleTrackSorter::selectHadrons);   // fill only good hadrons (already marked good leptons will be skipped)
//        Int_t nCandHadBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);
//        //------------------------------------------------------------------------
//
//        //------------------------------------------------------------------------
//        // analylize marked tracks (kIsUsed) ....
//    	  iter->Reset();
//        HParticleCand* pcand;
//        while((pcand = (HParticleCand*)iter->Next()) != NULL)
//	  {
//           // accept only objects marked as good
//	     if(!pcand->isFlagBit(HParticleCand::kIsUsed) ) continue;
//
//           // do other selection......
//        }
//        //------------------------------------------------------------------------
//
//        // if you do not want to modify the flags
//        // for the following tasks etc (needs call of
//        // backupFlags() at the beginning of the eventloop)
//        // sorter.restoreFlags();
//
//
//    }
//    //------------------------------------------------------------------------
//    //At the end of program
//    sorter.finalize();
// }
////////////////////////////////////////////////////////////////////////////////

ClassImp(HParticleTrackSorter)

Bool_t  HParticleTrackSorter::kDebug               = kFALSE;
Int_t   HParticleTrackSorter::printLevel           = 1;

Int_t   HParticleTrackSorter::kSwitchIndex         = 0;
Int_t   HParticleTrackSorter::kSwitchQuality       = 0;
Int_t   HParticleTrackSorter::kSwitchParticle      = 0;
Int_t   HParticleTrackSorter::kSwitchRICHMatching  = kUseRKRICHWindow;
Float_t HParticleTrackSorter::fRICHMDCWindow       = 4.;

Bool_t  HParticleTrackSorter::kIgnoreRICH          = kFALSE;
Bool_t  HParticleTrackSorter::kIgnoreInnerMDC      = kFALSE;
Bool_t  HParticleTrackSorter::kIgnoreOuterMDC      = kFALSE;
Bool_t  HParticleTrackSorter::kIgnoreMETA          = kFALSE;
Bool_t  HParticleTrackSorter::kIgnorePreviousIndex = kFALSE;
Bool_t  HParticleTrackSorter::kUseYMatching        = kTRUE;
Bool_t  HParticleTrackSorter::kUseYMatchingScaling = kTRUE;
Float_t HParticleTrackSorter::fMetaBoundary        = 4.0;
Bool_t  HParticleTrackSorter::kUseBeta             = kTRUE;
Float_t HParticleTrackSorter::fBetaLepCut          = 0.9;
Bool_t  HParticleTrackSorter::kUseFakeRejection    = kTRUE;
Bool_t  HParticleTrackSorter::kUseMETAQA           = kTRUE;
Float_t HParticleTrackSorter::fMETAQACut           = 3.0;
Float_t HParticleTrackSorter::fTOFMAXCut           = 60.0;

Bool_t (*HParticleTrackSorter::pUserSort)(candidateSort*, candidateSort*) = NULL;

HParticleTrackSorter::HParticleTrackSorter(void)
    : TNamed("ParticleSorter","PID track candidate sorter")
{
    clear();
}

HParticleTrackSorter::HParticleTrackSorter(TString name, TString title)
: TNamed(name, title)
{
    clear();
}

HParticleTrackSorter::~HParticleTrackSorter(void)
{
    if(nameIndex)           delete [] nameIndex;
    if(nameQuality)         delete [] nameQuality;
    if(iterParticleCandCat) delete iterParticleCandCat;
}

void HParticleTrackSorter::clear()
{
    // initialize the member variables
    nt                   = NULL;
    isSimulation         = kFALSE;
    nameIndex            = 0;
    nameQuality          = 0;
    pParticleCandCat     =  NULL;
    pEmcClusterCat       =  NULL;
    iterParticleCandCat  =  NULL;
    fill_Iteration       =  0;
    selectBest_Iteration =  0;
    currentEvent         = 100000000;
    fEvent               = NULL;
}

Bool_t HParticleTrackSorter::init(HRecEvent* evt)
{
    // get the pointer to the HParticleCand categegory and create the
    // corresponding iterator. HRecEvent* evt == 0 (default) the current event
    // will be used otherise the event provided.

    if(evt) { fEvent = evt; }
    else    { fEvent = (HRecEvent*)gHades->getCurrentEvent(); }

    pParticleCandCat = fEvent->getCategory(catParticleCand);
    if (!pParticleCandCat) {
	Error("HParticleTrackSorter::init()"," No HParticleCand Input -> Switching HParticleTrackSorter OFF");
    }
    else {
        if(iterParticleCandCat) delete iterParticleCandCat;
	iterParticleCandCat = (HIterator *) pParticleCandCat->MakeIterator();
    }
    pEmcClusterCat   = fEvent->getCategory(catEmcCluster);

    HCategory* catKine = fEvent->getCategory(catGeantKine);
    if(catKine) isSimulation = kTRUE;

    if(kDebug) {

	Int_t size;
	nameIndex   = HTool::parseString("kIsIndexRICH,kIsIndexInnerMDC,kIsIndexOuterMDC,"
					 "kIsIndexTOF,kIsIndexSHOWER,kIsIndexRPC,kIsIndexMETA",size,",",kFALSE);
        cout<<"-------------------------------------------"<<endl;
	cout<<"HParticleTrackSorter::init() :"<<endl;
	cout<<"index :: " <<endl;
	for(Int_t i = 0; i < size; i ++){
	    cout<<i <<" "<<nameIndex[i].Data()<<endl;
	}
	nameQuality = HTool::parseString("kIsBestHitRICH,kIsBestHitInnerMDC,kIsBestHitOuterMDC,kIsBestHitMETA,"
					 "kIsBestRK,kIsBestRKRKMETA,kIsBestRKRKMETARadius",size,",",kFALSE);
        for(Int_t i = 0; i < size; i ++){
	    cout<<i + SORTER_NUMBER_OF_INDEX<<" "<<nameQuality[i].Data()<<endl;
	}
	cout<<"-------------------------------------------"<<endl;
    }

     return kTRUE;
}

Bool_t HParticleTrackSorter::finalize(void)
{
    // clean up the memory and
    // write ntuple to file (if it exist)
    cleanUp(kTRUE);
    return kTRUE ;
}



Int_t HParticleTrackSorter::clearVector( vector<candidateSort*>& all_candidateSorts)
{
    // delete all candidate structs in vector
    // from heap and call the clear function of
    // vector afterwards.
    for(UInt_t i = 0; i < all_candidates.size() ; ++ i)
    {
	candidateSort* cand = all_candidates[i];
        delete cand;
    }
    all_candidates.clear();
    return 0;
}


Bool_t HParticleTrackSorter::cmpIndex(candidateSort* a, candidateSort* b)
{
    // sorts the candidate list ascending by the index
    // of the detector hit of interest.

    switch (kSwitchIndex) {
    case kIsIndexRICH:
	// sort in ascending order by Rich index
	return a->ind_RICH     < b->ind_RICH;
    case kIsIndexInnerMDC:
	// sort in ascending order by inner MDC segment index
	return a->ind_innerMDC < b->ind_innerMDC;
    case kIsIndexOuterMDC:
	// sort in ascending order by outer MDC segment index
	return a->ind_outerMDC < b->ind_outerMDC;
    case kIsIndexTOF:
	// sort in ascending order by TOF index
	return a->ind_TOF      < b->ind_TOF;
    case kIsIndexSHOWER:
	// sort in ascending order by SHOWER index
	return a->ind_SHOWER   < b->ind_SHOWER;
    case kIsIndexRPC:
	// sort in ascending order by RPC index
	return a->ind_RPC      < b->ind_RPC;

    default:
	::Error("HParticleTrackSorter::CmpIndex()","Unknown switch statement %i!",kSwitchIndex);
	return kFALSE;

    }
}

Bool_t HParticleTrackSorter::cmpQuality(candidateSort* a, candidateSort* b)
{
    // sorts the candidate list by the quality critera
    // of interest (ascending or decending depending on
    // the quantity). The best object is always the first.

    switch (kSwitchQuality) {
    case kIsBestHitRICH:
	// sort in ascending order by inner MDC chi2
	return a->nPadsRich             < b->nPadsRich;
    case kIsBestHitInnerMDC:
	// sort in ascending order by inner MDC chi2
	return a->innerMDCChi2          < b->innerMDCChi2;
    case kIsBestHitOuterMDC:
	// sort in ascending order by outer MDC chi2
	return a->outerMDCChi2          < b->outerMDCChi2;
    case kIsBestHitMETA:
	// sort in ascending order by RK METAMATCH QUALITY
	return a->RK_META_match_Quality < b->RK_META_match_Quality;
    case kIsBestRK:
        // sort in ascending order by RK chi2
	return a->RK_Chi2               < b->RK_Chi2;
    case kIsBestRKRKMETA:
	// sort in ascending order by RK_Chi2 * RK_META_match_Quality
        return (a->RK_Chi2 * a->RK_META_match_Quality)  < (b->RK_Chi2 * b->RK_META_match_Quality);
    case kIsBestRKRKMETARadius:
	// sort in ascending order by RK_Chi2 * RK_META_match_Quality
        return (a->RK_Chi2 * a->RK_META_match_Radius)  < (b->RK_Chi2 * b->RK_META_match_Radius);
    case kIsBestUser:
	// sort in ascending order by user function
	if(pUserSort == NULL) {
            ::Error("HParticleTrackSorter::CmpQuality()","kIsBestUser is set but user function pointer i NULL!");
	    return kTRUE;
	}
        else return (*pUserSort)(a,b);
    default:
	::Error("HParticleTrackSorter::CmpQuality()","Unknown switch statement %i!",kSwitchQuality);
        return kFALSE;

    }
}


Bool_t HParticleTrackSorter::rejectIndex(candidateSort* a, HParticleTrackSorter::ESwitch byIndex, Int_t& index)
{
    // if function return kTRUE the Object will be rejected
    // rejects candidates from the actual list, where the
    // detector hit of interest is not defined.

    switch (byIndex) {
    case kIsIndexRICH:
	// NO RICH
	return ( index = a->ind_RICH )     < 0;
    case kIsIndexInnerMDC:
	// NO inner MDC segment
	return ( index = a->ind_innerMDC ) < 0;
    case kIsIndexOuterMDC:
	// NO outer MDC segment
	return ( index = a->ind_outerMDC ) < 0;
    case kIsIndexTOF:
	// NO TOF
	return ( index = a->ind_TOF )      < 0;
    case kIsIndexSHOWER:
	// NO SHOWER
	return ( index = a->ind_SHOWER )   < 0;
    case kIsIndexRPC:
	// NO RPC
	return ( index = a->ind_RPC )      < 0;
    default:
	::Error("HParticleTrackSorter::rejectIndex()","Unknown switch statement %i!",byIndex);
        return kFALSE;
    }
}
Bool_t HParticleTrackSorter::rejectQuality(candidateSort* a, HParticleTrackSorter::ESwitch byQuality)
{
    // if function return kTRUE the Object will be rejected
    // rejects the candidates from the actual list if the quality
    // of interest does not match the mimnum requirement.

    switch (byQuality) {
    case kIsBestHitRICH:
	// RICH ring
	return a->ind_RICH < 0;
    case kIsBestHitInnerMDC:
	// NO fitted inner MDC segments
	return a->innerMDCChi2 < 0;
    case kIsBestHitOuterMDC:
	// NO fitted outer MDC segments
	return (a->outerMDCChi2 < 0 || a->ind_outerMDC < 0);
    case kIsBestHitMETA:
	// NO META
	return (a->ind_TOF < 0 && a->ind_SHOWER < 0 && a->ind_RPC < 0);
    case kIsBestRK:
	// NO outer MDC segment or NO RK
	return (a->RK_Chi2 < 0);
    case kIsBestRKRKMETA:
	// NO META or NO RK
        return (a->ind_TOF < 0 && a->ind_SHOWER < 0 && a->ind_RPC < 0) || (a->RK_Chi2 < 0);
    case kIsBestRKRKMETARadius:
	// NO META or NO RK
        return (a->ind_TOF < 0 && a->ind_SHOWER < 0 && a->ind_RPC < 0) || (a->RK_Chi2 < 0);
    case kIsBestUser:
	// NO META or NO RK
        return (a->ind_TOF < 0 && a->ind_SHOWER < 0 && a->ind_RPC < 0) || (a->RK_Chi2 < 0);

    default:
	::Error("HParticleTrackSorter::rejectQuality()","Unknown switch statement %i!",byQuality);
        return kFALSE;

    }
}

Int_t HParticleTrackSorter::flagAccepted(vector <candidateSort*>& all_candidates, HParticleTrackSorter::ESwitch byQuality)
{
    // flag all candidates which does satisfy the quality
    // criteria from input with kIsAcceptedXXX bit. If kDebug
    // is set and printLevel >=2 the statistics for the accepted
    // objects is printed.


    Int_t ctAll           = 0;
    Int_t ctAcceptQuality = 0;
    HParticleCand* pcand;
    for(UInt_t i = 0; i < all_candidates.size(); ++ i)
    {
        ++ ctAll;
	candidateSort* cand = all_candidates[i];
	if(rejectQuality(cand, byQuality))      continue;
	++ ctAcceptQuality;

	pcand = (HParticleCand*) pParticleCandCat->getObject(cand->ind_Cand);
	if(!pcand) {
	    Error("HParticleTrackSorter::flagAccepted()","NULL pointer retrieved for HParticleCand at index %i",cand->ind_Cand);
	    continue;
	}
	switch (byQuality) {
	case kIsBestHitRICH:
	    pcand -> setFlagBit(Particle::kIsAcceptedHitRICH);
            //------------------------------------------------
            // check the ring correlation
	    if(pcand->getChi2() >= 0){
		if( pcand->getRingCorr() == Particle::kIsRICHRK ||
		    pcand->getRingCorr() == (Particle::kIsRICHRK|Particle::kIsRICHMDC)
		  ) pcand->setFlagBit(Particle::kIsAcceptedRKRICH);
	    }
	    if( pcand->getRingCorr() == Particle::kIsRICHMDC ||
	        pcand->getRingCorr() == (Particle::kIsRICHRK|Particle::kIsRICHMDC)
	      ) pcand->setFlagBit(Particle::kIsAcceptedHitRICHMDC);
            //------------------------------------------------
	    break;
	case kIsBestHitInnerMDC:
	    pcand -> setFlagBit(Particle::kIsAcceptedHitInnerMDC);
	    break;
	case kIsBestHitOuterMDC:
	    pcand -> setFlagBit(Particle::kIsAcceptedHitOuterMDC);
	    break;
	case kIsBestHitMETA:
	    pcand -> setFlagBit(Particle::kIsAcceptedHitMETA);
            //------------------------------------------------
            // check if it was accepted for RK
	    if(pcand->getChi2() >= 0) {
		pcand-> setFlagBit(Particle::kIsAcceptedRKMETA);
	    }
            //------------------------------------------------
	    break;
	case kIsBestRK:
	    pcand -> setFlagBit(Particle::kIsAcceptedRK);
	    break;
	case kIsBestRKRKMETA:
	    pcand -> setFlagBit(Particle::kIsAcceptedRKRKMETA);
	    break;
	case kIsBestRKRKMETARadius:
	    pcand -> setFlagBit(Particle::kIsAcceptedRKRKMETARadius);
	    break;
	case kIsBestUser:
	    pcand -> setFlagBit(Particle::kIsAcceptedUser);
	    break;
	default :

	    Error("HParticleTrackSorter::flagAccepted()","Unknown switch statement : %i!",byQuality);
	    exit(1);
	    break;
	}

    }
    if(kDebug && printLevel >= 2){
	cout<<"flagAccepted(): "
	    <<"s: "                  <<selectBest_Iteration
	    <<", f: "                <<fill_Iteration
	    <<", kSwitchParticle: "  <<kSwitchParticle
	    <<", quality "           <<nameQuality[byQuality-SORTER_NUMBER_OF_INDEX].Data()
	    <<", candidate= "        <<ctAll
	    <<", acc. quality = "    <<ctAcceptQuality
	    <<endl;

    }
    return 0;
}

Int_t HParticleTrackSorter::flagDouble(vector <candidateSort*>& all_candidates, HParticleTrackSorter::ESwitch byIndex)
{
    // sort vector in ascending order by the particular index byindex. Loops
    // over the vector and extract objects with same detetector index and marks them
    // as Double_t hits in HParticleCand (kIsDoubleHitXXX bit). Non valid detecor
    // hits (index -1) are not taken into account. If kDebug
    // is set and printLevel >=2 the statistics for the accepted
    // objects is printed.

    kSwitchIndex   = byIndex;
    sort(all_candidates.begin(), all_candidates.end(), this->cmpIndex);


    Int_t index;
    Int_t old_index =-99;
    vector <candidateSort*> daughter;

    Int_t ctAll          = 0;
    Int_t ctAcceptIndex  = 0;
    Int_t ctAcceptDoubles= 0;
    Int_t ctCheck        = 0;

    for(UInt_t i = 0; i < all_candidates.size(); ++ i)
    {
        ++ ctAll;
	candidateSort* cand = all_candidates[i];
	if(rejectIndex  (cand, byIndex, index)) continue;
        ++ ctAcceptIndex;
	if(index == old_index || old_index == -99)
	{ // first time  or  still same index
	    daughter.push_back(cand);
	} else {
            // finalize list
	    if(!daughter.empty())
	    {
                setFlagsDouble(daughter,byIndex);
		if(daughter.size() >= 2){
		    ctAcceptDoubles += daughter.size();
		}
		ctCheck += daughter.size();
		daughter.clear();
	    }
	    daughter.push_back(cand);
	}
	old_index = index;
    }
    // finalize list last group
    if(!daughter.empty())
    {
	setFlagsDouble(daughter, byIndex);
	if(daughter.size() >= 2){
	    ctAcceptDoubles += daughter.size();
	}
	ctCheck += daughter.size();
    }
    if(kDebug && printLevel >= 2){
	cout<<"flagDouble(): "
	    <<"s: "                <<selectBest_Iteration
	    <<", f: "              <<fill_Iteration
	    <<", kSwitchParticle: "<<kSwitchParticle
	    <<", index "           <<nameIndex  [byIndex].Data()
	    <<", candidate= "      <<ctAll
	    <<", acc. index = "    <<ctAcceptIndex
            <<", acc. doubles = "  <<ctAcceptDoubles
            <<", check = "         <<ctCheck
	    <<endl;

    }
    return 0;

}

Int_t HParticleTrackSorter::setFlagsDouble(vector<candidateSort*>& daughter, HParticleTrackSorter::ESwitch byIndex)
{
    // flag the HParticleCand objects with the rank of the Double_t hit criteria as output
    // form the sorting algorithms. Expects as input a vector of candidates with the
    // same hit index.

    if(daughter.empty()   ) return 0;    // nothing to do
    if(daughter.size() < 2) return 0;    // no Double_t hit


    HParticleCand* cand;
    for(UInt_t i = 0; i < daughter.size(); ++ i)
    {  // loop over list and set flags
	cand = (HParticleCand*) pParticleCandCat->getObject(daughter[i]->ind_Cand);
	if(!cand) {
	    Error("HParticleTrackSorter::setFlagsDouble()","NULL pointer retrieved for HParticleCand at index %i",daughter[i]->ind_Cand);
	    continue;
	}
	switch (byIndex)
	{
	case kIsIndexRICH:
	    cand -> setFlagBit(Particle::kIsDoubleHitRICH);
	    break;
	case kIsIndexInnerMDC:
	    cand -> setFlagBit(Particle::kIsDoubleHitInnerMDC);
	    break;
	case kIsIndexOuterMDC:
	    cand -> setFlagBit(Particle::kIsDoubleHitOuterMDC);
	    break;
	case kIsIndexTOF:
	    cand -> setFlagBit(Particle::kIsDoubleHitMETA);
	    break;
	case kIsIndexSHOWER:
	    cand -> setFlagBit(Particle::kIsDoubleHitMETA);
	    break;
	case kIsIndexRPC:
	    cand -> setFlagBit(Particle::kIsDoubleHitMETA);
	    break;
	default :

	    Error("HParticleTrackSorter::setFlagsDouble()","Unknown switch statement : %i!",byIndex);
	    exit(1);
	    break;

	}
    }

    return 0;
}

Bool_t HParticleTrackSorter::flagEmcClusters()
{
    // loop over EmcCluster cat and reset IsUsedInParticleCand.
    // loop over all candidates and flag emc clusters used in
    // selected candidates only
    if(pEmcClusterCat&&pParticleCandCat)
    {
        HEmcCluster* emc = 0;
	Int_t n = pEmcClusterCat->getEntries();
	for(Int_t i = 0; i < n; i ++){
            emc = (HEmcCluster*) pEmcClusterCat->getObject(i);
            if(emc) emc -> unsetIsUsedInParticleCand();
	}

        HParticleCand* cand = 0;
	n = pParticleCandCat->getEntries();
	for(Int_t i = 0; i < n; i ++){
	    cand = (HParticleCand*) pParticleCandCat->getObject(i);
	    if(cand->isFlagBit(Particle::kIsUsed) ){
		Int_t ind = cand->getEmcInd();
		if(ind>=0){
		    emc = (HEmcCluster*) pEmcClusterCat->getObject(ind);
                    if(emc)  emc->setIsUsedInParticleCand();
		}
	    }
	}
    }
     return kTRUE;
}


Int_t HParticleTrackSorter::fillInput(vector < candidateSort* >& all_candidates)
{
    // loop over HParticleCand category and fill a vector of temporary objects
    // which will be used for sorting and flagging of the objects in the end.
    // The candidate objects are initialzied with values of interest in a well
    // defined way, which makes the sorting and rejecting of objects later on easier.

    HParticleCand* pCand ;

    candidateSort* cand;
    iterParticleCandCat->Reset();
    while ((pCand = (HParticleCand *)iterParticleCandCat->Next()) != 0 )
    {

	if(pCand->isFlagBit(Particle::kIsUsed))
	{
	    if(!kIgnorePreviousIndex && fill_Iteration == 2)
	    {
		//-------------------------------------------------------------------
		// add detector indices of already used objects to
		// the current index list
		if(pCand->getRichInd()     >= 0 &&
		   (find(index_RICH.begin(),index_RICH.end()        ,pCand->getRichInd())     == index_RICH.end()) ){
		    index_RICH.push_back(pCand->getRichInd());
		}
		if(pCand->getInnerSegInd() >= 0 &&
		   (find(index_InnerMDC.begin(),index_InnerMDC.end(),pCand->getInnerSegInd()) == index_InnerMDC.end()) ){
		    index_InnerMDC.push_back(pCand->getInnerSegInd());
		}
		if(pCand->getOuterSegInd() >= 0 &&
		   (find(index_OuterMDC.begin(),index_OuterMDC.end(),pCand->getOuterSegInd()) == index_OuterMDC.end()) ){
		    index_OuterMDC.push_back(pCand->getOuterSegInd());
		}

		//-------------------------------------------------------------------
                // TOF Clst + TOF hit special case
		if(pCand->getSelectedMeta() == Particle::kTofClst && pCand->getTofClstInd()      >= 0 &&
		   (find(index_TOFClst.begin(),index_TOFClst.end()  ,pCand->getTofClstInd())  == index_TOFClst.end()) ){
		    index_TOFClst.push_back(pCand->getTofClstInd());
		}
		if((pCand->getSelectedMeta() == Particle::kTofHit1 || pCand->getSelectedMeta() == Particle::kTofHit2) &&
		   pCand->getTofHitInd()       >= 0 &&
		   (find(index_TOFHit.begin(),index_TOFHit.end()    ,pCand->getTofHitInd()+1000)  == index_TOFHit.end()) ){
		    index_TOFHit.push_back(pCand->getTofHitInd()+1000);
		}
		//-------------------------------------------------------------------
		if(pCand->getSelectedMeta() == Particle::kShowerHit && pCand->getShowerInd()   >= 0 &&
		   (find(index_SHOWER.begin(),index_SHOWER.end()    ,pCand->getShowerInd())   == index_SHOWER.end()) ){
		    index_SHOWER.push_back(pCand->getShowerInd());
		}

		if(pCand->getSelectedMeta() == Particle::kRpcClst && pCand->getRpcInd()      >= 0 &&		   (find(index_RPC.begin(),index_RPC.end()          ,pCand->getRpcInd())      == index_RPC.end()) ){
		    index_RPC.push_back(pCand->getRpcInd());
		}

	    }
	    //-------------------------------------------------------------------

	    continue; // skip objects which are marked
	}
	if(pCand->isFlagBit(Particle::kIsRejected))  continue; // skip objects which are marked

	for(Int_t i = 0; i < 29; i ++) { pCand->unsetFlagBit(i);  }  // clean condition, only reject/used/lepton bit stays!

        //-------------------------------------------------------------------
	// fill the structur objects with needed
        // information

	cand = new candidateSort;
        cand->cand                  = pCand;
	cand->ind_Cand              = pCand->getIndex();

	cand->ind_RICH              = pCand->getRichInd();
	cand->ind_innerMDC          = pCand->getInnerSegInd();
	cand->ind_outerMDC          = pCand->getOuterSegInd();
	cand->ind_TOF               = (pCand->getTofClstInd() >= 0)? pCand->getTofClstInd() : (pCand->getTofHitInd() >= 0)? pCand->getTofHitInd() + 1000 : -1 ; // special case tof
	cand->ind_SHOWER            = pCand->getShowerInd();
	cand->ind_RPC               = pCand->getRpcInd();
	cand->selectedMeta          = pCand->getSelectedMeta();
        cand->nPadsRich             = pCand->getRingNumPads();
	cand->innerMDCChi2          = pCand->getInnerSegmentChi2();
	cand->outerMDCChi2          = pCand->getOuterSegmentChi2();
                                      
	cand->sec                   =  pCand->getSector();
	cand->beta                  =  pCand->getBeta();
	cand->mom                   =  pCand->getMomentum();
	cand->MDC_dEdx              =  pCand->getMdcdEdx();
	cand->nMdcLayerInnerSeg     =  pCand->getNLayer(0);
        cand->nMdcLayerOuterSeg     =  pCand->getNLayer(1);
	cand->RK_Chi2               = (pCand->getOuterSegmentChi2() < 0 && pCand->getChi2() >= 0 )? pCand->getChi2() + 100 : pCand->getChi2(); // lower priority sorting
        cand->RICH_RK_Corr          =  pCand->getRingCorr();
	cand->RK_RICH_match_Quality = (pCand->getRichInd() >= 0)?  pCand->getRichMatchingQuality(): -1;
	cand->RK_META_match_Quality = (finite(pCand->getMetaMatchQuality()))? pCand->getMetaMatchQuality() : -1;
	cand->RK_META_match_Radius  = (finite(pCand->getMetaMatchRadius()))? pCand->getMetaMatchRadius() : -1;
	cand->RK_META_dx            = (finite(pCand->getRkMetaDx()))? pCand->getRkMetaDx() : -1;
	cand->RK_META_dy            = (finite(pCand->getRkMetaDy()))? pCand->getRkMetaDy() : -1;
	cand->RK_META_match_Shr_Quality = pCand->getMetaMatchQualityShower();

	cand->userSort1=0;
	cand->userSort2=0;
	cand->userSort3=0;


	if(pCand->getSelectedMeta() == kTofClst ||
	   pCand->getSelectedMeta() == kTofHit1 ||
	   pCand->getSelectedMeta() == kRpcClst
	  ) {
	    cand->MetaModule = pCand->getMetaModule(0);
	    cand->MetaCell   = pCand->getMetaCell(0);
	} else if (pCand->getSelectedMeta() == kTofHit2){
	    cand->MetaModule = pCand->getMetaModule(1);
	    cand->MetaCell   = pCand->getMetaCell(1);
	} else {
	    cand->MetaModule = -1;
            cand->MetaCell   = -1;
	}

	if(cand->RK_Chi2 < 0){
	    cand->RK_META_match_Quality += 100; // lower priority in sorting
	    cand->RK_META_match_Radius  += 1000; // lower priority in sorting
	}

	//-------------------------------------------------------------------


	all_candidates.push_back(cand);
    }
    return 0;
}

void HParticleTrackSorter::printCand(candidateSort* cand, Int_t i,TString spacer)
{
    // print one candidate object and the flags which have been
    // already set to HParticleCand
    cout<<spacer.Data()<<i<<" --------------------------------------------"<<endl;
    cout<<spacer.Data()<<"cand " <<cand->ind_Cand
	<<", R: "  <<cand->ind_RICH
	<<", iM: " <<cand->ind_innerMDC
	<<", oM: " <<cand->ind_outerMDC
	<<", T: "  <<cand->ind_TOF
	<<", S: "  <<cand->ind_SHOWER
	<<", RPC: "<<cand->ind_RPC
	<<", selMeta: "<<cand->selectedMeta
	<<endl;
    cout<<spacer.Data()
	<<"Npads:"    <<cand->nPadsRich
	<<", ichi2: " <<cand->innerMDCChi2
	<<", ochi2: " <<cand->outerMDCChi2
	<<", RKM: "   <<cand->RK_META_match_Quality
	<<", RKMSHR: "<<cand->RK_META_match_Shr_Quality
	<<", RK: "    <<cand->RK_Chi2
	<<", RKRKMETA: "  <<(cand->RK_Chi2 != -1 && cand->RK_META_match_Quality != -1 ? cand->RK_Chi2*cand->RK_META_match_Quality : -1)
	<<", RKRKMETARadius: "  <<(cand->RK_Chi2 != -1 && cand->RK_META_match_Radius != -1 ? cand->RK_Chi2*cand->RK_META_match_Radius : -1)
	<<", isRingRK: "<<(Int_t) cand->RICH_RK_Corr
	<<endl;
    HParticleCand* pcand = (HParticleCand*) pParticleCandCat->getObject(cand->ind_Cand);
    if(cand)
    {
	cout<<spacer.Data()<<flush;
	pcand->printFlags();
    } else {
	::Error("HParticleTrackSorter::printCand()","NULL pointer retrieved for HParticleCand Index %i",cand->ind_Cand);

    }

}
void HParticleTrackSorter::printEvent(TString comment)
{
    Int_t ctcand = 0;

    cout<<"----------------------------------------"<<endl;
    cout<<"Event kSwitchParticle: "<<kSwitchParticle<<" "
 	<<setw(6)                  <<fEvent->getHeader()->getEventSeqNumber()
	<<"s: "                    <<selectBest_Iteration
	<<", f: "                  <<fill_Iteration
	<<" "                      <<comment.Data()
	<<endl;
    HParticleCand* pcand;
    iterParticleCandCat->Reset();
    while ((pcand = (HParticleCand *)iterParticleCandCat->Next()) != 0 )
    {
	cout <<setw(6)<<ctcand<<" "<<flush;
	pcand->print();
	++ ctcand;
    }
}

void HParticleTrackSorter::resetFlags(Bool_t flag, Bool_t reject, Bool_t used, Bool_t lepton)
{
    // reset all flags of HParticleCand by looping over the
    // HParticleCand category. The flags kIsRejected, kIsUsed
    // and kIsLepton are special for selection and handles independend
    // from the other flags. The lower flags of objects marked
    // with kIsUsed cand be only reste if used == kRTUE.

    if(!iterParticleCandCat) return;
    HParticleCand* pCand;
    iterParticleCandCat->Reset();
    while ((pCand = (HParticleCand *) iterParticleCandCat->Next()) != 0) // begin of ParticleCand iterator
    {
	if(flag){
	    if((pCand->isFlagBit(Particle::kIsUsed) && used)  ||
	       !pCand->isFlagBit(Particle::kIsUsed) ){
		// we want to keep the flags from previous selection
		// clean condition, only reject,used and lepton bit stays!
		for(Int_t i=0;i<Particle::kIsLepton;i++) { pCand->unsetFlagBit(i);  }  
	    }
	}
	if(reject) pCand->unsetFlagBit(Particle::kIsRejected);
	if(used)   pCand->unsetFlagBit(Particle::kIsUsed);
	if(lepton) pCand->unsetFlagBit(Particle::kIsLepton);
    }
}
void HParticleTrackSorter::selection(Bool_t (*function)(HParticleCand* ))
{
    // loops over the HParticleCand category and marks all objects
    // which does not fullfill the selection criteria with the reject flag.
    // Objects marked as kIsUsed are skipped.
    if(!iterParticleCandCat) return;
    HParticleCand* pCand;
    iterParticleCandCat->Reset();
    while ((pCand = (HParticleCand *) iterParticleCandCat->Next()) != 0) // begin of ParticleCand iterator
    {
	if( pCand->isFlagBit(Particle::kIsUsed)) { continue;} // do not touch already used objects!
	if(!pCand->select(function)) {pCand->setFlagBit(Particle::kIsRejected); }
    }
}
void HParticleTrackSorter::cleanUp(Bool_t final)
{
    // delete temporary candidate objects from
    // heap and clear the candidate vector. clear
    // all index vectors.

    clearVector(all_candidates);

    if(kIgnorePreviousIndex || final){
	index_RICH    .clear();
	index_InnerMDC.clear();
	index_OuterMDC.clear();
	index_SHOWER  .clear();
	index_TOFHit  .clear();
	index_TOFClst .clear();
	index_RPC     .clear();
    }
}

Int_t  HParticleTrackSorter::fillAndSetFlags()
{
    // clean up the memory and index vectors and fill the
    // candidate vector new. Flag the kIsAcceptedXXX criteria
    // and kIsDoubleHitXXX criteria of the HParticleCand objects of
    // the selection (skipp objects which are marked kIsUsed and
    // kIsRejected) returns the number of candidates.


    if(!pParticleCandCat) return 0; // nothing to do
    fill_Iteration++;

    cleanUp(kFALSE);
    //------------------------------------------------------------------
    // fill HParticleCand Info to temorary objects
    // which will be jused for sorting
    // all flags in HParticleCand will be reset (besides kIsRejected)
    fillInput(all_candidates);


    // list of thing to to
    // for each Double_t hit
    HParticleTrackSorter::ESwitch myIndex  [] = {
	kIsIndexRICH,
	kIsIndexInnerMDC,
	kIsIndexOuterMDC,
	kIsIndexTOF,
	kIsIndexSHOWER,
        kIsIndexRPC
    };

    // and for different quality criteria
    HParticleTrackSorter::ESwitch myQuality[] = {
        kIsBestHitRICH,
        kIsBestHitInnerMDC,
        kIsBestHitOuterMDC,
        kIsBestHitMETA,
        kIsBestRK,
	kIsBestRKRKMETA,
	kIsBestRKRKMETARadius,
        kIsBestUser
   };

    kSwitchIndex   = kIsIndexRICH;
    kSwitchQuality = kIsBestHitRICH;

    // check all objects for validity with respect to
    // quality criteria and flag them in HParticleCand
    for(UInt_t i = 0; i < (sizeof(myQuality) / sizeof(Int_t)); i ++){
	flagAccepted(all_candidates,myQuality[i]);
    }

    // vector is resorted by index! flag all Double_t used
    // detector hits in HParticleCand
    for(UInt_t i = 0; i < (sizeof(myIndex) / sizeof(Int_t)); i ++){
        flagDouble(all_candidates,myIndex[i]);
    }

    return all_candidates.empty() ? 0 : all_candidates.size();
}

void  HParticleTrackSorter::backupFlags(Bool_t onlyFlags)
{
    // backup the flags of HParticleCand objects. The flags cand be
    // restored to the objects by calling restoreFlags. The flags
    // which have been stored previously will be cleared.

    if(!onlyFlags){
	kSwitchRICHMatchingBackup  = kSwitchRICHMatching;
	fRICHMDCWindowBackup       = fRICHMDCWindow  ;

	kIgnoreRICHBackup          = kIgnoreRICH;
	kIgnoreInnerMDCBackup      = kIgnoreInnerMDC;
	kIgnoreOuterMDCBackup      = kIgnoreOuterMDC;
	kIgnoreMETABackup          = kIgnoreMETA;
	kIgnorePreviousIndexBackup = kIgnorePreviousIndex;
	pUserSortBackup            = pUserSort;

	kUseYMatchingBackup        = kUseYMatching;
	kUseYMatchingScalingBackup = kUseYMatchingScaling;
        fMetaBoundaryBackup        = fMetaBoundary;
	kUseBetaBackup             = kUseBeta;
        fBetaLepCutBackup          = fBetaLepCut;
        kUseFakeRejectionBackup    = kUseFakeRejection;
        kUseMETAQABackup           = kUseMETAQA;

	fMetaBoundaryBackup        = fMetaBoundary;
	fBetaLepCutBackup          = fBetaLepCut;
	fMETAQACutBackup           = fMETAQACut;
	fTOFMAXCutBackup           = fTOFMAXCut;

    }
    old_flags.clear();
    old_flags_emc.clear();

    if(!iterParticleCandCat) return;

    HParticleCand* pCand ;
    iterParticleCandCat->Reset();

    while ((pCand = (HParticleCand *)iterParticleCandCat->Next()) != 0 )
    {
         old_flags.push_back(pCand->getFlagField());
    }
    if(pEmcClusterCat){
	HEmcCluster* emc = 0;
	Int_t n = pEmcClusterCat->getEntries();
	for(Int_t i = 0; i < n; i ++){
	    emc = (HEmcCluster*) pEmcClusterCat->getObject(i);
	    if(emc) {
		Int_t fl = emc -> isUsedInParticleCand();
		old_flags_emc.push_back(fl);
	    }
	}
    }

}

Bool_t  HParticleTrackSorter::restoreFlags(Bool_t onlyFlags)
{
    // restore the flags of HParticleCand objects since the last call of
    // backupFlags(). Returns kFALSE if size of vector old_flags does
    // not match the number of objects in HParticleCand category

    if(!onlyFlags){
	kSwitchRICHMatching  = kSwitchRICHMatchingBackup;
	fRICHMDCWindow       = fRICHMDCWindowBackup  ;

	kIgnoreRICH          = kIgnoreRICHBackup;
	kIgnoreInnerMDC      = kIgnoreInnerMDCBackup;
	kIgnoreOuterMDC      = kIgnoreOuterMDCBackup;
	kIgnoreMETA          = kIgnoreMETABackup;
	kIgnorePreviousIndex = kIgnorePreviousIndexBackup;
        pUserSort            = pUserSortBackup;
	kUseYMatching        = kUseYMatchingBackup;
	kUseYMatchingScaling = kUseYMatchingScalingBackup;
	fMetaBoundary        = fMetaBoundaryBackup;
	kUseBeta             = kUseBetaBackup;
        fBetaLepCut          = fBetaLepCutBackup;
        kUseFakeRejection    = kUseFakeRejectionBackup;
        kUseMETAQA           = kUseMETAQABackup;

	fMetaBoundary        = fMetaBoundaryBackup;
	fBetaLepCut          = fBetaLepCutBackup;
	fMETAQACut           = fMETAQACutBackup;
	fTOFMAXCut           = fTOFMAXCutBackup;
    }

    if(!iterParticleCandCat) return kFALSE;

    HParticleCand* pCand ;
    iterParticleCandCat->Reset();

    vector<Int_t>::iterator iter;
    iter = old_flags.begin();
    while ((pCand = (HParticleCand *)iterParticleCandCat->Next()) != 0 )
    {
	if(iter != old_flags.end()){
	    pCand->setFlagField(*iter);
	    iter ++;
	} else {
            Error("restoreFlags()","Size of backuped flags does not match number of HParticleCand objects!");
	    return kFALSE;
	}
    }

    if(pEmcClusterCat){
	HEmcCluster* emc = 0;
	Int_t n = pEmcClusterCat->getEntries();
	for(Int_t i = 0; i < n; i ++){
	    emc = (HEmcCluster*) pEmcClusterCat->getObject(i);
	    if(emc) {
		emc -> unsetIsUsedInParticleCand();
                if(old_flags_emc[i] == 1 ) emc -> isUsedInParticleCand();
	    }
	}
    }

    return kTRUE;
}
void  HParticleTrackSorter::backupSetup()
{
    // backup the setup of HParticleTrackSorter. The setup cand be
    // restored to the objects by calling restoreSetup.

    kSwitchRICHMatchingBackup  = kSwitchRICHMatching;
    fRICHMDCWindowBackup       = fRICHMDCWindow  ;

    kIgnoreRICHBackup          = kIgnoreRICH;
    kIgnoreInnerMDCBackup      = kIgnoreInnerMDC;
    kIgnoreOuterMDCBackup      = kIgnoreOuterMDC;
    kIgnoreMETABackup          = kIgnoreMETA;
    kIgnorePreviousIndexBackup = kIgnorePreviousIndex;
    pUserSortBackup            = pUserSort;
    kUseYMatchingBackup        = kUseYMatching;
    kUseYMatchingScalingBackup = kUseYMatchingScaling;
    fMetaBoundaryBackup        = fMetaBoundary;
    kUseBetaBackup             = kUseBeta;
    fBetaLepCutBackup          = fBetaLepCut;
    kUseFakeRejectionBackup    = kUseFakeRejection;
    kUseMETAQABackup           = kUseMETAQA;

    fMetaBoundaryBackup        = fMetaBoundary;
    fBetaLepCutBackup          = fBetaLepCut;
    fMETAQACutBackup           = fMETAQACut;
    fTOFMAXCutBackup           = fTOFMAXCut;

}

Bool_t  HParticleTrackSorter::restoreSetup()
{
    // restore the setup of HParticleTrackSorter since the last call of
    // backupSetup().

    kSwitchRICHMatching  = kSwitchRICHMatchingBackup;
    fRICHMDCWindow       = fRICHMDCWindowBackup  ;

    kIgnoreRICH          = kIgnoreRICHBackup;
    kIgnoreInnerMDC      = kIgnoreInnerMDCBackup;
    kIgnoreOuterMDC      = kIgnoreOuterMDCBackup;
    kIgnoreMETA          = kIgnoreMETABackup;
    kIgnorePreviousIndex = kIgnorePreviousIndexBackup;
    pUserSort            = pUserSortBackup;
    kUseYMatching        = kUseYMatchingBackup;
    kUseYMatchingScaling = kUseYMatchingScalingBackup;
    fMetaBoundary        = fMetaBoundaryBackup;
    kUseBeta             = kUseBetaBackup;
    fBetaLepCut          = fBetaLepCutBackup;
    kUseFakeRejection    = kUseFakeRejectionBackup;
    kUseMETAQA           = kUseMETAQABackup;

    fMetaBoundary        = fMetaBoundaryBackup;
    fBetaLepCut          = fBetaLepCutBackup;
    fMETAQACut           = fMETAQACutBackup;
    fTOFMAXCut           = fTOFMAXCutBackup;

    return kTRUE;
}

Int_t HParticleTrackSorter::fill(Bool_t (*function)(HParticleCand* ))
{
    // fill the input candidate vector. The kIsRejected flag and
    // lower flags (below kIsLepton) is reseted for all objects
    // not marked as kIsUsed. The filling is done twice once for
    // all objects and second after the application of the user
    // provide test function. Returns the number of candidates after
    // the selection. If kDebug is set and printLevel >= 1 the number
    // of candidates before and after selection will be printed.

    resetFlags(kTRUE,kTRUE,kFALSE,kFALSE);  // need to reset reject/double hit bits (already used objects will stay)!
    Int_t first  = fillAndSetFlags();       // fill all candidates
    selection(function);                    // flag candidates rejected if the they fail the test
    Int_t second = fillAndSetFlags();       // fill only good candidates
    if(kDebug && printLevel >= 1) {
	cout<<"fill(): "
	    <<"s: "                <<selectBest_Iteration
	    <<", f: "              <<fill_Iteration
	    <<", kSwitchParticle: "<<kSwitchParticle
	    <<", nCands first: "   <<first
	    <<", nCands second :"  <<second
	    <<endl;
    }
    return all_candidates.empty() ?  0 : all_candidates.size();
}

Int_t HParticleTrackSorter::selectBest(HParticleTrackSorter::ESwitch byQuality, Int_t byParticle)
{
    // perfomes the selection of the best tracks (with respect to the chosen quality
    // criteria), which makes shure, that each detector hit is used only once. If
    // In case Int_t byParticle == kIsLepton the RICH index
    // is taken into account, otherwise not. For leptons the kIsLepton bit is flagged
    // in addition. Individual detector hits can be ignored in the selection procedure with
    // the setIgnoreXXX(kTRUE) functions (by default all hits are taken into account).
    // Returns the number of selected best HParticleCand objects.
    // if kDebug is set and  printLevel >= 1 the number of sected objects and
    // the statistics of the rejection on Double_t hits i the snhgle detectors is printed.
    // if printLevel >=3 the properties of the candidates (accepted and rejected) are
    // printed.

    if(fEvent->getHeader()->getEventSeqNumber() != currentEvent){
	currentEvent         = fEvent->getHeader()->getEventSeqNumber();
        selectBest_Iteration = 0;
    }
    selectBest_Iteration++;

    kSwitchQuality  = byQuality;
    kSwitchParticle = byParticle;
    Int_t ct        = 0;
    Int_t ctdouble[4] = {0};

    if(kSwitchQuality==HParticleTrackSorter::kIsBestUser && pUserSort==NULL){
	::Error ("selectBest()","qualtiy is kIsBestUser, but pUserSort==NULL. No sort done.");
    } else {
	sort(all_candidates.begin(),all_candidates.end(),this->cmpQuality);
    }
    for(UInt_t i = 0; i < all_candidates.size(); ++ i)
    {
        candidateSort* cand = all_candidates[i];

        //-------------------------------------------------------
	// skip the candidates which contain a single detector hit
        // which has been used before
        Int_t isDoubleHit = 0;
        /*
	// disabled :: HParticleCand objects are not created for all comb.
	// of innerMDC  and RICH ! This caused problems with close pairs
	// (2 good tracks matched to same RICH ring).
	if( (kSwitchParticle == kIsLepton && !kIgnoreRICH ) ) {
            // look to RICH hits only in case of leptons
	    if(cand->ind_RICH     >= 0 &&
	       (find(index_RICH.begin(),index_RICH.end()        ,cand->ind_RICH)     != index_RICH.end()) ){
		isDoubleHit |= 0x1 << 0;
		ctdouble[0] ++;
	    }
	} */
	if( !kIgnoreInnerMDC ) {
	    if(cand->ind_innerMDC >= 0 &&
	       (find(index_InnerMDC.begin(),index_InnerMDC.end(),cand->ind_innerMDC) != index_InnerMDC.end()) ){
		isDoubleHit |= 0x1 << 1;
		ctdouble[1] ++;
	    }
	}
	if( !kIgnoreOuterMDC ) {
	    if(cand->ind_outerMDC >= 0 &&
	       (find(index_OuterMDC.begin(),index_OuterMDC.end(),cand->ind_outerMDC) != index_OuterMDC.end()) ){
		isDoubleHit |= 0x1 << 2;
		ctdouble[2] ++;
	    }
	}
	if( !kIgnoreMETA ) {
	    if( (cand->selectedMeta == Particle::kTofHit1 || cand->selectedMeta == Particle::kTofHit2)  &&
	       cand->ind_TOF      >= 0 &&
	       (find(index_TOFHit.begin(),index_TOFHit.end()    ,cand->ind_TOF)      != index_TOFHit.end()) ){
		isDoubleHit |= 0x1 << 3;
		ctdouble[3] ++;

	    }

	    if(cand->selectedMeta == Particle::kTofClst && cand->ind_TOF      >= 0 &&
	       (find(index_TOFClst.begin(),index_TOFClst.end()   ,cand->ind_TOF)     != index_TOFClst.end()) ){
		isDoubleHit |= 0x1 << 3;
		ctdouble[3] ++;

	    }
            /*
	    // disabled :: HParticleCand objects are not created for all comb.
	    // of outerMDC  and SHOWER!
	    if(cand->selectedMeta == Particle::kShowerHit && cand->ind_SHOWER   >= 0 &&
	       (find(index_SHOWER.begin(),index_SHOWER.end()    ,cand->ind_SHOWER)   != index_SHOWER.end()) ){
		isDoubleHit |= 0x1 << 3;
		ctdouble[3] ++;
	    }
            */
	    if(cand->selectedMeta == Particle::kRpcClst   && cand->ind_RPC      >= 0 &&
	       (find(index_RPC.begin(),index_RPC.end()          ,cand->ind_RPC)      != index_RPC.end()) ){
		isDoubleHit |= 0x1 << 3;
		ctdouble[3] ++;
	    }
	}
	if(isDoubleHit){
            if(kDebug && printLevel >= 1){
		TString out="";
		for(Int_t j = 3; j >= 0; j --){
		    out += ( (isDoubleHit>>j) & 0x1 );
		}
		cout<<"--selectBest(): skip Double_t hit with pattern : "<<out.Data()<<endl;
		printCand(cand,i);
	    }
	    continue;
	}
        //-------------------------------------------------------

        //-------------------------------------------------------
	// mark the candidate as best and add the single detector hit indices
	// to the lookup

	/*
	// disabled :: HParticleCand objects are not created for all comb.
	// of innerMDC and RICH ! This caused problems with close pairs
	// (2 good tracks matched to same RICH ring).
	if(cand->ind_RICH     >= 0 ){
	    index_RICH.push_back(cand->ind_RICH);
	}
	*/

	if(cand->ind_innerMDC >= 0 ){
	    index_InnerMDC.push_back(cand->ind_innerMDC);
	}
	if(cand->ind_outerMDC >= 0 ){
	    index_OuterMDC.push_back(cand->ind_outerMDC);
	}
	if( (cand->selectedMeta == Particle::kTofHit1 || cand->selectedMeta == Particle::kTofHit2) &&
	   cand->ind_TOF      >= 0 ){
	    index_TOFHit.push_back(cand->ind_TOF);
	}
	if(cand->selectedMeta == Particle::kTofClst &&
	   cand->ind_TOF      >= 0 ){
	    index_TOFClst.push_back(cand->ind_TOF);
	}
        /*
        // disabled :: HParticleCand objects are not created for all comb.
	// of outerMDC and SHOWER !
	if(cand->selectedMeta == Particle::kShowerHit &&
	   cand->ind_SHOWER   >= 0 ){
	    index_SHOWER.push_back(cand->ind_SHOWER);
	}
	*/
	if(cand->selectedMeta == Particle::kRpcClst &&
	   cand->ind_RPC      >= 0 ){
	    index_RPC.push_back(cand->ind_RPC);
	}

	HParticleCand* pcand = (HParticleCand*) pParticleCandCat->getObject(cand->ind_Cand);
	if(!pcand) {
	    Error("HParticleTrackSorter::selectBest()","NULL pointer retrieved for HParticleCand at index %i",cand->ind_Cand);
	    continue;
	}
	switch (byQuality)
	{
	case kIsBestHitRICH:
	    pcand -> setFlagBit(Particle::kIsBestHitRICH);
	    break;
	case kIsBestHitInnerMDC:
	    pcand -> setFlagBit(Particle::kIsBestHitInnerMDC);
	    break;
	case kIsBestHitOuterMDC:
	    pcand -> setFlagBit(Particle::kIsBestHitOuterMDC);
	    break;
	case kIsBestHitMETA:
	    pcand -> setFlagBit(Particle::kIsBestHitMETA);
	    break;
	case kIsBestRK:
	    pcand -> setFlagBit(Particle::kIsBestRK);
	    break;
	case kIsBestRKRKMETA:
	    pcand -> setFlagBit(Particle::kIsBestRKRKMETA);
	    break;
	case kIsBestRKRKMETARadius:
	    pcand -> setFlagBit(Particle::kIsBestRKRKMETARadius);
	    break;
	case kIsBestUser:
	    pcand -> setFlagBit(Particle::kIsBestUser);
	    break;
	default :

	    Error("HParticleTrackSorter::selectBest()","Unknown switch statement : %i!",byQuality);
	    exit(1);
	    break;

	}
        ++ ct;
	pcand ->setFlagBit(Particle::kIsUsed);
	if(byParticle == kIsLepton){
	    pcand ->setFlagBit(Particle::kIsLepton);
	}
	if(kDebug && printLevel >= 1){
	    cout<<"++selectBest(): select hit : "<<endl;
	    printCand(cand,i);
	}
	//-------------------------------------------------------

    }
    if(kDebug && printLevel >= 1)
    {
	cout<<"selectBest(): "
	    <<"s: "                  <<selectBest_Iteration
	    <<", f: "                <<fill_Iteration
	    <<", kSwitchParticle: "  <<kSwitchParticle
	    <<", nBest cand: "       <<ct
	    <<", Double_t RICH: "    <<ctdouble[0]
            <<", InnerMDC: "         <<ctdouble[1]
            <<", Double_t OuterMDC: "<<ctdouble[2]
            <<", Double_t META: "    <<ctdouble[3]
	    <<endl;
    }
    fill_Iteration = 0;
    flagEmcClusters();
  return ct;
}

void HParticleTrackSorter::setRICHMatching(HParticleTrackSorter::ERichMatch match, Float_t window)
{
    // switch between the matching cut RICH-MDC inside the selectLeptons() function. The method
    // is selected by the options defined in HParticleTrackSorter::ERichMatch.
	kSwitchRICHMatching = match;
	fRICHMDCWindow      = window;
}


Bool_t HParticleTrackSorter::selectLeptons(HParticleCand* pcand)
{
    // build in selection function for lepton candidates.
    // Requires besides an RICH hit, RK + META and fitted
    // inner+outer segment. The RICH-MDC matching cut depends on the
    // settings of the matching method and window (set by
    // HParticleTrackSorter::setRICHMatching . default is
    // kUseRKRICHWindow, window = 4. degree)

    if(kUseFakeRejection&&pcand->isFakeRejected()) return kFALSE;
    Bool_t test = kFALSE;
    if( pcand->isFlagAND(5,
			 Particle::kIsAcceptedHitRICH,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitOuterMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(!test) return kFALSE;

    if(kUseMETAQA && (pcand->getMetaMatchQuality() > fMETAQACut || pcand->getMetaMatchQuality() < 0)) return kFALSE;
    if(kUseYMatching && !HParticleTool::isGoodMetaCell(pcand,fMetaBoundary,kUseYMatchingScaling)) return kFALSE;
    if(kUseBeta && pcand->getBeta()<fBetaLepCut) return kFALSE;

    if(test)
    {   // rich match only!
	if( kSwitchRICHMatching == kUseRICHIndex)
	{
	    return kTRUE;
	}
	else if ( kSwitchRICHMatching == kUseRKRICHCorrelation	&&  pcand -> getRingCorr() )
	{
	    return kTRUE;
	}
	else if ( kSwitchRICHMatching == kUseRKRICHWindow
		 &&
		 fabs(pcand->getDeltaTheta()) < fRICHMDCWindow
		 &&
		 fabs(pcand->getDeltaPhi())   < fRICHMDCWindow
		)
	{
	    return kTRUE;
	}
	else return kFALSE;

    }
    else return kFALSE;
}
Bool_t HParticleTrackSorter::selectLeptonsNoOuterFit(HParticleCand* pcand)
{
    // build in selection function for lepton candidates.
    // Requires besides an RICH hit, RK + META and fitted
    // inner. Fit for outer segment not reuired. The
    // RICH-MDC matching cut depends on the
    // settings of the matching method and window (set by
    // HParticleTrackSorter::setRICHMatching . default is
    // kUseRKRICHWindow, window = 4. degree)

    if(kUseFakeRejection&&pcand->isFakeRejected()) return kFALSE;
    Bool_t test = kFALSE;
    if( pcand->isFlagAND(4,
			 Particle::kIsAcceptedHitRICH,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getOuterSegInd()      > 0
       &&
       pcand->getChi2()             < 1000      // RK
      ) test = kTRUE;

    if(!test) return kFALSE;

    if(kUseMETAQA && (pcand->getMetaMatchQuality() > fMETAQACut || pcand->getMetaMatchQuality() < 0)) return kFALSE;
    if(kUseYMatching && !HParticleTool::isGoodMetaCell(pcand,fMetaBoundary,kUseYMatchingScaling)) return kFALSE;
    if(kUseBeta && pcand->getBeta()<fBetaLepCut) return kFALSE;

    if(test)
    {   // rich match only!
	if( kSwitchRICHMatching == kUseRICHIndex)
	{
	    return kTRUE;
	}
	else if ( kSwitchRICHMatching == kUseRKRICHCorrelation	&&  pcand -> getRingCorr() )
	{
	    return kTRUE;
	}
	else if ( kSwitchRICHMatching == kUseRKRICHWindow
		 &&
		 fabs(pcand->getDeltaTheta()) < fRICHMDCWindow
		 &&
		 fabs(pcand->getDeltaPhi())   < fRICHMDCWindow
		)
	{
	    return kTRUE;
	}
	else return kFALSE;
    }
    else return kFALSE;
}
Bool_t HParticleTrackSorter::selectHadrons(HParticleCand* pcand )
{
    // build in selection function for hadron candidates.
    // Requires besides RK + META and fitted inner+outer MDC.

    if(kUseFakeRejection&&pcand->isFakeRejected()) return kFALSE;
    Bool_t test=kFALSE;
    if( pcand->isFlagAND(4,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitOuterMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getChi2()             < 1000
      ) test=kTRUE;
    if(!test) return kFALSE;

    if(kUseMETAQA && (pcand->getMetaMatchQuality() > fMETAQACut || pcand->getMetaMatchQuality() < 0)) return kFALSE;
    if(kUseYMatching && !HParticleTool::isGoodMetaCell(pcand,fMetaBoundary,kUseYMatchingScaling)) return kFALSE;
    if(kUseBeta && (pcand->getBeta()<0 || pcand->getTof()>fTOFMAXCut)) return kFALSE;

    return test;
}
Bool_t HParticleTrackSorter::selectHadronsNoOuterFit(HParticleCand* pcand )
{
    // build in selection function for hadron candidates.
    // Requires besides RK + META and fitted inner MDC. Fit
    // for outer MDC not required.

    if(kUseFakeRejection&&pcand->isFakeRejected()) return kFALSE;
    Bool_t test=kFALSE;
    if( pcand->isFlagAND(3,
			 Particle::kIsAcceptedHitInnerMDC,
			 Particle::kIsAcceptedHitMETA,
			 Particle::kIsAcceptedRK
			)
       &&
       pcand->getInnerSegmentChi2() > 0
       &&
       pcand->getOuterSegInd()      > 0
       &&
       pcand->getChi2()             < 1000
      ) test=kTRUE;

    if(!test) return kFALSE;

    if(kUseMETAQA && (pcand->getMetaMatchQuality() > fMETAQACut || pcand->getMetaMatchQuality() < 0)) return kFALSE;
    if(kUseYMatching && !HParticleTool::isGoodMetaCell(pcand,fMetaBoundary,kUseYMatchingScaling)) return kFALSE;
    if(kUseBeta && (pcand->getBeta()<0 || pcand->getTof()>fTOFMAXCut)) return kFALSE;

    return test;
}

