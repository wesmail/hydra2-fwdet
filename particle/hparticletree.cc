#include "hparticletree.h"
#include "hades.h"
#include "hdatasource.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hcategorymanager.h"
#include "hparticletool.h"

//-----------------------
#include "hparticledef.h"
#include "haddef.h"
#include "hstartdef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "rpcdef.h"
#include "tofdef.h"
#include "showerdef.h"
#include "walldef.h"
#include "hpiontrackerdef.h"
#include "hgeantdef.h"


//-----------------------
#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticleevtinfo.h"
#include "hparticlemdc.h"
#include "hmetamatch2.h"

#include "hstart2hit.h"
#include "hstart2cal.h"
#include "htboxchan.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hrichdirclus.h"
#include "hrichcal.h"
#include "hrichcalsim.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "htofcluster.h"
#include "htofclustersim.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"
#include "hshowerhit.h"
#include "hshowerhitsim.h"
#include "hwallhit.h"
#include "hwallhitsim.h"

#include "hpiontrackertrack.h"
#include "hpiontrackerhit.h"
#include "hpiontrackercal.h"
#include "hpiontrackerraw.h"

#include "hmdctrkcand.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdchit.h"
#include "hmdchitsim.h"
#include "hmdcclus.h"
#include "hmdcclussim.h"
#include "hmdcclusinf.h"
#include "hmdcclusinfsim.h"
#include "hmdcclusfit.h"
#include "hmdcclusfitsim.h"
#include "hmdcwirefit.h"
#include "hmdcwirefitsim.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"

#include "hgeantkine.h"
#include "hgeantrich.h"
#include "hgeantmdc.h"
#include "hgeanttof.h"
#include "hgeantrpc.h"
#include "hgeantshower.h"
#include "hgeantwall.h"
#include "hgeantstart.h"
#include "hgeantemc.h"

//-----------------------
#include "TBranch.h"
#include "TSystem.h"
#include "TObjArray.h"

//-----------------------
#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
using namespace std;

// ROOT's IO and RTTI stuff is added here
ClassImp(HParticleTree)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleTree
//
// Reconstructor to create an filtered root output.
// The output is generated independend for each HParticleTree
// reconstructor and das not affect Hades. The purpose
// is to create different output files with different event
// structure in parallel and allow to reduce the data volume
// for special analysis tasks (for example rare dilepton decays
// in Au+Au collisions).
// The filtering starts from HParticleCand. Only objects
// which are flagged kIsUsed are copied to the output.
// The user can specify selectLeptons() and selectHadrons()
// functions. By default the functions of HParticleTracksorter
// are used (for the documentation of the selection functions see
// HParticleTrackSorter). In addition the user can specify a selectEvent()
// function function which allows to check the full event.
//
// The program works the following way:
//
// I.    The original flags of the HParticleCand objects and HParticleSorter
//       are backupted. This alows to run the task without affecting other tasks.
// II.   In the input HParticleCand objects are flagged using the user specified
//       selection functions or default functions.
// III.  If the user provides a Bool_t selectEvent(TObjArray*) function the
//       properties of the full event can be evaluated. If the function returns
//       kFALSE an empty event ist stored to preserve the 1 to 1 correlation
//       with the dst file.
//       With setSkipEmptyEvents (kTRUE) skipping of not selected events can be forced
// III.  From HParticleCand objects which are flagged kIsUsed the selection
//       procedure starts. Only flagged objects are considered and the detector
//       hits belonging to the candidates are stored in the output too if enabled.
//       With setSkipTracks(kFALSE) all tracks will forced to the output.
//       HParticleEvtInfo, HStart2Cal, HStartHit and HWallHit are fully copied
//       if enabled (see list below).
//       Alternatively (not at the same time!) to using flagged candidates as
//       selection the user can provide a function to select the kept particles by
//       setUserkeepTrack(Bool_t (*function)(HParticleCand* )). The function
//       has to return kTRUE for selected candidates.
//       All indices are resorted. The new event will look like the old one,
//       but containing only "good" candidates. The full features of the analysis
//       of the DSTs are preserved.
//
//       SUPPORTED CATEGORIES:
//
//       catParticleCand
//       catParticleEvtInfo (fullcopy)
//       catStart2Hit       (fullcopy)
//       catStart2Cal       (fullcopy)
//       catTBoxChan        (fullcopy)
//       catWallHit         (fullcopy)
//       catPionTrackerRaw  (fullcopy)
//       catPionTrackerCal  (fullcopy)
//       catPionTrackerHit  (fullcopy)
//       catPionTrackerTrack(fullcopy)
//       catTofHit
//       catTofCluster
//       catRpcCluster
//       catShowerHit
//       catRichHit
//       catRichDirClus     (fullcopy
//       catRichCal         (fullcopy
//       catMdcSeg
//       catMdcHit
//       catMdcCal1
//       catMdcClus
//       catMdcClusInf
//       catMdcClusFit
//       catMdcWireFit
//       catGeantKine             (fullcopy)
//       catMdcGeantRaw           (fullcopy)
//       catTofGeantRaw           (fullcopy)
//       catRpcGeantRaw           (fullcopy)
//       catShowerGeantRaw        (fullcopy)
//       catWallGeantRaw          (fullcopy)
//       catRichGeantRaw (+1,+2)  (fullcopy)
//
//
// create output:
// by
// 1. set outputfilename       ( full name including path, dir or suffix not used)
// 2. by hades output name     ( dir / file stem + suffix + _cyclecount.root)
// 3. by datasource input name ( dir / file stem + suffix + _cyclecount.root)
//
// Example: Filter dilepton events :
//
// I.   use the standard selectLeptons() from HParticleTrackSorter
// II.  ignore hadrons : take simple user defined function below
// III. select events which have at least 1 positron and 1 electron candidate
// IV.  create the event structure you need
//
//  //------------------------------------------------------
//  Bool_t selectLeptons(HParticleCand* pcand){
//
//    //  selection function for lepton candidates.
//    Bool_t select = kFALSE;
//    if(pcand->isFlagAND(5,
//			Particle::kIsAcceptedHitRICH,
//			Particle::kIsAcceptedHitInnerMDC,
//			Particle::kIsAcceptedHitOuterMDC,
//			Particle::kIsAcceptedHitMETA,
//			Particle::kIsAcceptedRK)
//       &&
//       pcand->getInnerSegmentChi2() > 0
//       &&
//       pcand->getChi2() < 1000
//       &&
//       pcand->getBeta() > 0.9
//    ) select = kTRUE;
//
//   return select;
//  }
//  //------------------------------------------------------
//  //------------------------------------------------------
//  Bool_t selectNoHadron(HParticleCand*){
//      // do not use this selection
//      return kFALSE;
//  }
//
//  //------------------------------------------------------
//  Bool_t selectEvent(TObjArray* ar){
//    // select events which have at least
//    // one lepton canddate pair . TObjArray* can
//    // be used to pas parameters into the function
//
//    HCategory* catCand = gHades->getCurrentEvent()->getCategory(catParticleCand);
//    if(catCand){
//    Int_t n=catCand->getEntries();
//    HParticleCand* cand=0;
//    Int_t nEle = 0;
//    Int_t nPos = 0;
//        for(Int_t i = 0; i < n; i ++){
//    	cand = HCategoryManager::getObject(cand,catCand,i);
//    	if(cand){
//    	    if(cand->isFlagBit(kIsLepton)){
//    		if      (cand->getCharge() == -1 ) nEle++;
//    		else if (cand->getCharge() ==  1 ) nPos++;
//    	    }
//    	}
//        }
//        if( (nPos < 1 && nEle < 2) || (nPos < 2 && nEle < 1) ) return kFALSE; // accept 2likesign or 1 unlikesign pair
//     } else return kFALSE;
//     return kTRUE;
//  }
//  //------------------------------------------------------
//
//  HParticleTree* parttree = new HParticleTree("leptontree","leptontree");
//  // 1. set outputfilename       ( full name including path, dir or suffix not used)
//  // 2. by hades output name     ( dir / file stem + suffix + _cyclecount.root)
//  // 3. by datasource input name ( dir / file stem + suffix + _cyclecount.root)
//
//  // case 1:
//  // parttree->setOutputFile ("/lustre/hades/filter_tree.root","Filter","RECREATE",2 ); // set outfile by hand
//  // case 2+3:
//  partree->setOutputFileSuffix("filter_tree"); // take outfile name from Hades output or data input and add _filter_tree at the end
//  parttree->setOutputDir("/lustre/hades");     // take outfile name from Hades output or data input and change the dir
//
//  parttree->setSkipEmptyEvents(kTRUE);   (default kFALSE)
//  parttree->setSkipTracks(kTRUE);        (default kTRUE)
//
//  //-------CONFIGURE HParticleTrackSorter-----------------
//  parttree->setIgnoreRICH         (kTRUE);
//  parttree->setIgnoreInnerMDC     (kTRUE);
//  parttree->setIgnoreOuterMDC     (kTRUE);
//  parttree->setIgnoreMETA         (kTRUE);
//  parttree->setIgnorePreviousIndex(kTRUE);
//  parttree->setRICHMatching(HParticleTrackSorter::kUseRKRICHWindow match, 4.); // matching type and window
//  //------------------------------------------------------
//
//  //-------CONFIGURE EventStructure-----------------------
//  Cat_t PersistentCat[] = {
//    catParticleCand,
//    catParticleMdc,
//    catParticleEvtInfo,
//    catStart2Hit,
//    catStart2Cal,
//    catTBoxChan,
//    catWallHit,
//    catPionTrackerTrack,
//    //catPionTrackerRaw,
//    //catPionTrackerCal,
//    //catPionTrackerHit,
//    //catTofHit,
//    //catTofCluster,
//    //catRpcCluster,
//    //catShowerHit,
//    //catRichHit,
//    //catRichDirClus,
//    //catRichCal,
//    //catMdcSeg,
//    //catMdcHit,
//    //catMdcCal1,
//    //catMdcClus,
//    //catMdcClusInf,
//    //catMdcClusFit,
//    //catMdcWireFit
//    //catGeantKine,
//    //catMdcGeantRaw,
//    //catTofGeantRaw,
//    //catRpcGeantRaw,
//    //catShowerGeantRaw,
//    //catWallGeantRaw,
//    //catRichGeantRaw,
//    //catRichGeantRaw+1,
//    //catRichGeantRaw+2,
//    //catEmcGeantRaw,
//    //catStartGeantRaw
//   };
//
//  // list of categories which should be fully copied
//  // do not use categories twice!
//
//    Cat_t PersistentCatFullCopy[] = {
//    catParticleEvtInfo,
//    catStart2Hit,
//    catStart2Cal,
//    catTBoxChan,
//    catWallHit,
//    catPionTrackerTrack,
//    //catPionTrackerRaw,
//    //catPionTrackerCal,
//    //catPionTrackerHit,
//    //catTofHit,
//    //catTofCluster,
//    //catRpcCluster,
//    //catShowerHit,
//    //catRichHit,
//    //catRichDirClus,
//    //catRichCal,
//    //catGeantKine,
//    //catMdcGeantRaw,
//    //catTofGeantRaw,
//    //catRpcGeantRaw,
//    //catShowerGeantRaw,
//    //catWallGeantRaw,
//    //catRichGeantRaw,
//    //catRichGeantRaw+1,
//    //catRichGeantRaw+2,
//    //catEmcGeantRaw,
//    //catStartGeantRaw
//   };
//
//  parttree->setEventStructure(sizeof(PersistentCat)        /sizeof(Cat_t),PersistentCat,kFALSE); // filter these cats
//  parttree->setEventStructure(sizeof(PersistentCatFullCopy)/sizeof(Cat_t),PersistentCat,kTRUE);  // copy these cats witt all entries
//  //------------------------------------------------------
//  //-------CONFIGURE SELECTIONS-----------------------
//  parttree->setUserSelectionLeptons(selectLeptons);   // use own lepton selector
//  parttree->setUserSelectionHadrons(selectNoHadron); // use only standard leptons from HParticlesorter
//  parttree->setUserSelectionEvent(selectEvent,NULL); // no params used
//  //------------------------------------------------------
//
////////////////////////////////////////////////////////////////////////////////




    HParticleTree::HParticleTree(const Text_t *name,const Text_t *title)
    : HReconstructor(name,title)
{
    fCurrentEvent      = 0;
    fOutputFile        = 0;
    fTree              = 0;
    fCycleNumber       = 0;
    fOutputFileName    = "";
    fOutputTitle       = "";
    fOutputOption      = "RECREATE";
    fOutputCompression = 0;
    fOutputFileSuffix  = "_filter_tree";
    fOutputDir         = "";
    fOutFound          = kFALSE;

    kSkipEmptyEvents   = kFALSE;
    kSkipTracks        = kTRUE;

    Cat_t supported [] = {
	catParticleCand,
	catParticleMdc,
	catParticleEvtInfo,
	catStart2Hit,
	catStart2Cal,
        catTBoxChan,
	catWallHit,
	catPionTrackerRaw,
	catPionTrackerCal,
	catPionTrackerHit,
	catPionTrackerTrack,
	catTofHit,
	catTofCluster,
	catRpcCluster,
	catShowerHit,
	catRichHit,
	catRichDirClus,
	catRichCal,
	catMdcSeg,
	catMdcHit,
	catMdcCal1,
	catMdcClus,
	catMdcClusInf,
	catMdcClusFit,
	catMdcWireFit,
	catGeantKine,
	catMdcGeantRaw,
	catTofGeantRaw,
	catRpcGeantRaw,
	catShowerGeantRaw,
	catWallGeantRaw,
	catRichGeantRaw,
	catRichGeantRaw+1,
	catRichGeantRaw+2,
	catEmcGeantRaw,
        catStartGeantRaw
    };


    for(UInt_t i = 0 ; i < sizeof(supported) / sizeof(Cat_t); i++){
	fmCatNumSupport.push_back(supported[i]);
    }
    sort(fmCatNumSupport.begin(),fmCatNumSupport.end());


    Cat_t supportFullCopy [] ={
	catGeantKine,
	catMdcGeantRaw,
	catTofGeantRaw,
	catRpcGeantRaw,
	catShowerGeantRaw,
	catWallGeantRaw,
	catRichGeantRaw,
	catRichGeantRaw+1,
	catRichGeantRaw+2,
	catEmcGeantRaw,
        catStartGeantRaw,
	catParticleEvtInfo,
	catStart2Hit,
	catStart2Cal,
        catTBoxChan,
	catWallHit,
	catPionTrackerRaw,
	catPionTrackerCal,
	catPionTrackerHit,
	catPionTrackerTrack,
        catRichHit,
	catRichDirClus,
	catRichCal,
	catShowerHit,
	catTofHit,
	catTofCluster,
	catRpcCluster
    };

    for(UInt_t i = 0 ; i < sizeof(supportFullCopy) / sizeof(Cat_t); i++){
	fmCatNumFullCopySupport.push_back(supportFullCopy[i]);
    }
    sort(fmCatNumFullCopySupport.begin(),fmCatNumFullCopySupport.end());



    kSwitchRICHMatching  = HParticleTrackSorter::kUseRKRICHWindow;
    fRICHMDCWindow       = 4.  ;
    kIgnoreRICH          = kFALSE;
    kIgnoreInnerMDC      = kFALSE;
    kIgnoreOuterMDC      = kFALSE;
    kIgnoreMETA          = kFALSE;
    kIgnorePreviousIndex = kFALSE;

    pUserSelectEvent   = NULL;
    pUserSelectLeptons = NULL;
    pUserSelectHadrons = NULL;
    pUserKeepTrack     = NULL;
    fParamSelectEvent  = NULL;

    sorter.backupSetup();

}

HParticleTree::~HParticleTree()
{
    //closeOutput();
    //if (fTree)             delete fTree;
    //if (fCurrentEvent)     delete fCurrentEvent;
}

Bool_t HParticleTree::init()
{
   sorter.init();

   return kTRUE;
}

Int_t  HParticleTree::execute()
{
    //-------------------------------------------
    // create output:
    // by
    // 1. set outputfilename       ( full name including path, dir or suffix not used)
    // 2. by hades output name     ( dir / file stem + suffix + _cyclecount.root)
    // 3. by datasource input name ( dir / file stem + suffix + _cyclecount.root)
    //
    //

    memset(ctMdcSeg ,0,6*2*sizeof(Int_t));
    memset(ctMdcClus,0,6*2*sizeof(Int_t));
    memset(ctMdcHit ,0,6*4*sizeof(Int_t));
    memset(ctRpcClus,0,6*sizeof(Int_t));

    if(!fOutFound)
    {   //  only once!

	sorter.backupSetup();

	const HRecEvent& event = *((HRecEvent*)gHades->getCurrentEvent());

	if(fOutputFileName != ""){ // output file name has been set by hand

            Info("execute()","CREATING OUTPUT FILE FROM SPECIFIED FILENAME= %s !",fOutputFileName.Data());
	    fOutputFile = new TFile(fOutputFileName.Data(),"RECREATE",fOutputTitle.Data(),fOutputCompression);
	    fOutFound = kTRUE;
            setEvent();
	    makeTree();
	} else {
            if( fOutputDir == "" )         fOutputDir = gSystem->WorkingDirectory();        // -> absolute path
            if( fOutputDir.EndsWith("/") ) fOutputDir.Replace(fOutputDir.Length() -1,1,""); // remove trailing "/"


	    TFile* f  = gHades->getOutputFile();

	    if(f){  // derive filename from hades output file

		fOutputOption      = f->GetOption();
		fOutputTitle       = f->GetTitle();
		fOutputCompression = f->GetCompressionLevel();


		TString fname = f->GetName();
		fname = gSystem->BaseName(fname.Data());  // remove path
                fname.ReplaceAll(".root","");

                fOutputFileName = Form("%s/%s%s.root",fOutputDir.Data(),fname.Data(),fOutputFileSuffix.Data());

		Info("execute()","CREATING OUTPUT FILE FROM HADES OUTPUT= %s !",fOutputFileName.Data());
		fOutputFile = new TFile(fOutputFileName.Data(),"RECREATE",fOutputTitle.Data(),fOutputCompression);
		fOutFound = kTRUE;

		fCurrentEvent = new HRecEvent(event);

		setEvent();

		makeTree();

	    } else { // derive filename from datasource input file

		HDataSource* source = gHades->getDataSource();
		if(source){

		    TString fname = source->getCurrentFileName();

		    if(fname.EndsWith(".root"))
		    { // root file input

			fOutputOption      = "RECREATE";
			fOutputTitle       = "Filter";
			fOutputCompression = 2;

			fname = gSystem->BaseName(fname.Data());  // remove path
			fname.ReplaceAll(".root","");

			fOutputFileName = Form("%s/%s%s.root",fOutputDir.Data(),fname.Data(),fOutputFileSuffix.Data());

			Info("","CREATING OUTPUT FILE FROM ROOT INPUT = %s !",fOutputFileName.Data());
			fOutputFile = new TFile(fOutputFileName.Data(),"RECREATE",fOutputTitle.Data(),fOutputCompression);
			fOutFound = kTRUE;
			setEvent();
		        makeTree();
		    } else { // hld input

			fOutputOption      = "RECREATE";
			fOutputTitle       = "Filter";
			fOutputCompression = 2;


			fname = gSystem->BaseName(fname.Data());  // remove path
			fname.ReplaceAll(".hld","");

			fOutputFileName = Form("%s/%s%s.root",fOutputDir.Data(),fname.Data(),fOutputFileSuffix.Data());

			Info("","CREATING OUTPUT FILE FROM HLD INPUT = %s !",fOutputFileName.Data());
			fOutputFile = new TFile(fOutputFileName.Data(),fOutputOption.Data(),fOutputTitle.Data(),fOutputCompression);
			fOutFound = kTRUE;

			setEvent();
			makeTree();
		    } // hld input
		} // data source
	    } // from input
	} // from input or output
    }
    //-------------------------------------------


    //-------------------------------------------
    // save output if file is too big and open a new one
    if (fOutputFile) {
	if (fOutputFile->GetBytesWritten() > gHades->getOutputSizeLimit()) {
	    recreateOutput();
	}
    }
    //-------------------------------------------



    if(fTree)
    {
	Bool_t goodEvent = kTRUE;

	fCurrentEvent->Clear();

	HEventHeader* header      = fCurrentEvent->getHeader();
	HEventHeader* hadesheader = gHades->getCurrentEvent()->getHeader(); // copy full event header

	new (header) HEventHeader(*hadesheader);

	if(fCurrentEvent->getCategory(catParticleCand))
	{

	    //-------------------------------------------
            sorter.backupFlags(kTRUE); // backup only flags of HParticleCand, not setup of HParticleSorter

	    sorter.setIgnoreRICH         (kIgnoreRICH) ;
	    sorter.setIgnoreInnerMDC     (kIgnoreInnerMDC);
	    sorter.setIgnoreOuterMDC     (kIgnoreOuterMDC);
	    sorter.setIgnoreMETA         (kIgnoreMETA) ;
	    sorter.setIgnorePreviousIndex(kIgnorePreviousIndex);
            if(kSwitchRICHMatching == HParticleTrackSorter::kUseRICHIndex)         sorter.setRICHMatching(HParticleTrackSorter::kUseRICHIndex        ,fRICHMDCWindow);
            if(kSwitchRICHMatching == HParticleTrackSorter::kUseRKRICHCorrelation) sorter.setRICHMatching(HParticleTrackSorter::kUseRKRICHCorrelation,fRICHMDCWindow);
	    if(kSwitchRICHMatching == HParticleTrackSorter::kUseRKRICHWindow)      sorter.setRICHMatching(HParticleTrackSorter::kUseRKRICHWindow     ,fRICHMDCWindow);

            sorter.cleanUp();  // clear vector of candidates + indices of hits

	    // clean all flags !
	    sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);     // flags,reject,used,lepton

	    //-------LEPTONS---------------------------
	    if(pUserSelectLeptons){
		sorter.fill(pUserSelectLeptons);                  // fill only good leptons (user provided)
	    } else {
		sorter.fill(HParticleTrackSorter::selectLeptons); // fill only good leptons
	    }
	    sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
	    //-------HADRONS---------------------------
	    if(pUserSelectHadrons){
		sorter.fill(pUserSelectHadrons);                  // fill only good hadrons (user provided) (already marked good leptons will be skipped)
	    } else {
		sorter.fill(HParticleTrackSorter::selectHadrons); // fill only good hadrons (already marked good leptons will be skipped)
	    }
	    sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);
	    //-------------------------------------------

            goodEvent = kTRUE;
	    if(pUserSelectEvent){
                goodEvent = (*pUserSelectEvent)(fParamSelectEvent);
	    }

	    if(goodEvent)
	    {
		Bool_t isSim = fmCatNumToName[catParticleCand] == "HParticleCandSim" ? kTRUE : kFALSE;

		//-------------------------------------------
		// First copy all categories which are not connected
		// with HParticleCand. Full categories are copied if
		// set in the event layout


		//###########################################
		//###########################################
		//  GEANT
		//-------------------------------------------
		if(fCurrentEvent->getCategory(catGeantKine))  // linear
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catGeantKine);
		    HCategory* catOut = fCurrentEvent->getCategory(catGeantKine);
		    Int_t n = catIn->getEntries();
		    HGeantKine* kine1 = 0;
		    HGeantKine* kine2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			kine1 = HCategoryManager::getObject    (kine1,catIn,i);
			kine2 = HCategoryManager::newObjectCopy(kine2,kine1,catOut,index);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catMdcGeantRaw)) // matrix
		{

		    HCategory* catIn  = HCategoryManager::getCategory(catMdcGeantRaw);
		    HLocation loc;
		    loc.set(4,0,0,0,0); // 6,4,7,195


		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catMdcGeantRaw);

		    Int_t n = catIn->getEntries();
		    HGeantMdc* hit1 = 0;
		    HGeantMdc* hit2 = 0;

		    Int_t ind[6][4][7] ;
		    for(Int_t s=0;s<6;s++){
			for(Int_t m=0;m<4;m++){
			    for(Int_t l=0;l<7;l++){
                              ind[s][m][l]=0;
			    }
			}
		    }


		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
			    loc[1] = hit1->getModule();
			    loc[2] = hit1->getLayer();
                            loc[3] = ind[loc[0]][loc[1]][loc[2]];

			    hit2 =(HGeantMdc*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantMdc(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());
                            ind[loc[0]][loc[1]][loc[2]]++;
			}
		    }

		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catTofGeantRaw)) // matrix
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catTofGeantRaw);
		    HLocation loc;
		    loc.set(2,0,0); // 6,200

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catTofGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantTof* hit1 = 0;
		    HGeantTof* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantTof*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantTof(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRpcGeantRaw))  // matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catRpcGeantRaw);
		    HLocation loc;
		    loc.set(2,0,0); // 6,2500

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRpcGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantRpc* hit1 = 0;
		    HGeantRpc* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantRpc*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantRpc(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catShowerGeantRaw)) // matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catShowerGeantRaw);
		    HLocation loc;
		    loc.set(2,0,0); // 6,600

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catShowerGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantShower* hit1 = 0;
		    HGeantShower* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantShower*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantShower(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catEmcGeantRaw)) // matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catShowerGeantRaw);
		    HLocation loc;
		    loc.set(2,0,0); // 6,600

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catEmcGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantEmc* hit1 = 0;
		    HGeantEmc* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantEmc*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantEmc(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catWallGeantRaw))  // linear
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catWallGeantRaw);
		    HCategory* catOut = fCurrentEvent->getCategory(catWallGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantWall* wall1 = 0;
		    HGeantWall* wall2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			wall1 = HCategoryManager::getObject    (wall1,catIn,i);
			wall2 = HCategoryManager::newObjectCopy(wall2,wall1,catOut,index);
			wall2->setNextHitIndex(wall1->getNextHitIndex());

		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catStartGeantRaw))  // linear
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catStartGeantRaw);
		    HCategory* catOut = fCurrentEvent->getCategory(catStartGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantStart* start1 = 0;
		    HGeantStart* start2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			start1 = HCategoryManager::getObject    (start1,catIn,i);
			start2 = HCategoryManager::newObjectCopy(start2,start1,catOut,index);
			start2->setNextHitIndex(start1->getNextHitIndex());

		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichGeantRaw)) // matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catRichGeantRaw);
		    HLocation loc;
		    loc.set(2,0,0); // 6,4000

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw);
		    Int_t n = catIn->getEntries();
		    HGeantRichPhoton* hit1 = 0;
		    HGeantRichPhoton* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantRichPhoton*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantRichPhoton(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichGeantRaw+1)) //matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catRichGeantRaw+1);
		    HLocation loc;
		    loc.set(2,0,0); // 6,4000

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw+1);
		    Int_t n = catIn->getEntries();
		    HGeantRichDirect* hit1 = 0;
		    HGeantRichDirect* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantRichDirect*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantRichDirect(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichGeantRaw+2)) // matrix
		{
	            HCategory* catIn  = HCategoryManager::getCategory(catRichGeantRaw+2);
		    HLocation loc;
		    loc.set(2,0,0); // 6,4000

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw+2);
		    Int_t n = catIn->getEntries();
		    HGeantRichMirror* hit1 = 0;
		    HGeantRichMirror* hit2 = 0;

		    Int_t ind[6] ;
		    for(Int_t s=0;s<6;s++){
			ind[s]=0;
		    }

		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			if(hit1){

			    loc[0] = hit1->getSector();
                            loc[1] = ind[loc[0]];

			    hit2 =(HGeantRichMirror*) catOut->getSlot(loc,&index);
			    new (hit2) HGeantRichMirror(*hit1);
			    hit2->setNextHitIndex(hit1->getNextHitIndex());

                            ind[loc[0]]++;
			}
		    }
		}
		//-------------------------------------------
 		//###########################################
		//###########################################

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catParticleEvtInfo))
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catParticleEvtInfo);
		    HCategory* catOut = fCurrentEvent->getCategory(catParticleEvtInfo);
		    Int_t n = catIn->getEntries();
		    HParticleEvtInfo* info1 = 0;
		    HParticleEvtInfo* info2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			info1 = HCategoryManager::getObject    (info1,catIn,i);
			info2 = HCategoryManager::newObjectCopy(info2,info1,catOut,index);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catStart2Hit))
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catStart2Hit);
		    HCategory* catOut = fCurrentEvent->getCategory(catStart2Hit);
		    Int_t n = catIn->getEntries();
		    HStart2Hit* hit1 = 0;
		    HStart2Hit* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catStart2Cal))
		{   // MatrixCat!
		    HCategory* catIn  = HCategoryManager::getCategory(catStart2Cal);

		    HLocation loc;
		    loc.set(2,0,0);

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catStart2Cal);
		    Int_t n = catIn->getEntries();
		    HStart2Cal* hit1 = 0;
		    HStart2Cal* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject(hit1,catIn,i);
			loc[0] = hit1->getModule();
			loc[1] = hit1->getStrip();
			hit2 =(HStart2Cal*) catOut->getSlot(loc,&index);
			new (hit2) HStart2Cal(*hit1);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catTBoxChan))
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catTBoxChan);
		    HCategory* catOut = fCurrentEvent->getCategory(catTBoxChan);
		    Int_t n = catIn->getEntries();
		    HTBoxChan* hit1 = 0;
		    HTBoxChan* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catWallHit))
		{   // Matrixcat!
		    HCategory* catIn  = HCategoryManager::getCategory(catWallHit);
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catWallHit);

		    HLocation loc;
		    loc.set(1,0);

		    Int_t n = catIn->getEntries();
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){

			if(isSim){
			    HWallHitSim* hit1 = 0;
			    HWallHitSim* hit2 = 0;

			    hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			    loc[0] = hit1->getCell();
			    hit2 =(HWallHitSim*) catOut->getSlot(loc,&index);
			    new (hit2) HWallHitSim(*hit1);

			} else {
			    HWallHit* hit1 = 0;
			    HWallHit* hit2 = 0;

			    hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			    loc[0] = hit1->getCell();
			    hit2 =(HWallHit*) catOut->getSlot(loc,&index);
			    new (hit2) HWallHit(*hit1);
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichCal))
		{   // Matrixcat!
		    HCategory* catIn  = HCategoryManager::getCategory(catRichCal);
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRichCal);

		    HLocation loc;
		    loc.set(3,0,0,0);   // matrix cat ,  but used as linearcat ???

		    Int_t n = catIn->getEntries();
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			if(isSim){
			    HRichCalSim* hit1 = 0;
			    HRichCalSim* hit2 = 0;

			    hit1 = HCategoryManager::getObject(hit1,catIn,i);
                            loc[0] = hit1->getSector();
			    loc[1] = hit1->getRow();
			    loc[2] = hit1->getCol();

			    hit2 =(HRichCalSim*) catOut->getSlot(loc,&index);
			    new (hit2) HRichCalSim(*hit1);

			} else {

			    HRichCal* hit1 = 0;
			    HRichCal* hit2 = 0;

			    hit1 = HCategoryManager::getObject(hit1,catIn,i);
                            loc[0] = hit1->getSector();
			    loc[1] = hit1->getRow();
			    loc[2] = hit1->getCol();

			    hit2 =(HRichCal*) catOut->getSlot(loc,&index);
			    new (hit2) HRichCal(*hit1);
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichDirClus))
		{   // Matrixcat!
		    HCategory* catIn  = HCategoryManager::getCategory(catRichDirClus);
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRichDirClus);

		    HLocation loc;
		    loc.set(1,0);   // matrix cat ,  but used as linearcat ???

		    Int_t n = catIn->getEntries();
		    HRichDirClus* hit1 = 0;
		    HRichDirClus* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 =(HRichDirClus*) catOut->getNewSlot(loc,&index);
			new (hit2) HRichDirClus(*hit1);
		    }
		}
		//-------------------------------------------


		//-------------------------------------------
		if(fCurrentEvent->getCategory(catPionTrackerTrack))
		{
		    HCategory* catIn  = HCategoryManager::getCategory(catPionTrackerTrack);
		    HCategory* catOut = fCurrentEvent->getCategory(catPionTrackerTrack);
		    Int_t n = catIn->getEntries();
		    HPionTrackerTrack* hit1 = 0;
		    HPionTrackerTrack* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catPionTrackerCal))
		{   // Matrixcat!
		    HCategory* catIn  = HCategoryManager::getCategory(catPionTrackerCal);
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catPionTrackerCal);

		    HLocation loc;
		    loc.set(2,0,0);   // matrix cat ,  but used as linearcat ???

		    Int_t n = catIn->getEntries();
		    HPionTrackerCal* hit1 = 0;
		    HPionTrackerCal* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 =(HPionTrackerCal*) catOut->getNewSlot(loc,&index);
			new (hit2) HPionTrackerCal(*hit1);
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catPionTrackerHit))
		{   // Matrixcat!
		    HCategory* catIn  = HCategoryManager::getCategory(catPionTrackerHit);
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catPionTrackerHit);

		    HLocation loc;
		    loc.set(2,0,0);   // matrix cat ,  but used as linearcat ???

		    Int_t n = catIn->getEntries();
		    HPionTrackerHit* hit1 = 0;
		    HPionTrackerHit* hit2 = 0;
		    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			hit1 = HCategoryManager::getObject    (hit1,catIn,i);
			hit2 =(HPionTrackerHit*) catOut->getNewSlot(loc,&index);
			new (hit2) HPionTrackerHit(*hit1);
		    }
		}
		//-------------------------------------------






		//-------------------------------------------
                // now the categories with option to full copy
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRichHit) && doFullCopy(catRichHit)){

		    HLinearCategory* catOut = (HLinearCategory*)fCurrentEvent->getCategory(catRichHit);
                    HCategory* catIn  = HCategoryManager::getCategory(catRichHit);

		    Int_t n = catIn->getEntries();
                    Int_t index;
		    for(Int_t i = 0; i < n; i++){
			if(isSim)
			{
			    HRichHitSim* hit1 = 0;
			    HRichHitSim* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catIn,i);
			    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);

			} else {

			    HRichHit* hit1 = 0;
			    HRichHit* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catIn,i);
			    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);
			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catTofHit) && doFullCopy(catTofHit)){
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catTofHit);
                    HCategory* catIn  = HCategoryManager::getCategory(catTofHit);

		    HLocation loc;
		    loc.set(3,0,0,0);

		    Int_t n = catIn->getEntries();
                    Int_t index;

		    for(Int_t i = 0; i < n; i++){

			if(isSim)
			{
			    HTofHitSim* hit1 = 0;
			    HTofHitSim* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catTofHit,i);

			    loc[0] = hit1->getSector();
			    loc[1] = hit1->getModule();
			    loc[2] = hit1->getCell();

			    hit2 =(HTofHitSim*) catOut->getSlot(loc,&index);
			    new (hit2) HTofHitSim(*hit1);

			} else {

			    HTofHit* hit1 = 0;
			    HTofHit* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catTofHit,i);

			    loc[0] = hit1->getSector();
			    loc[1] = hit1->getModule();
			    loc[2] = hit1->getCell();

			    hit2 =(HTofHit*) catOut->getSlot(loc,&index);
			    new (hit2) HTofHit(*hit1);

			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catTofCluster) && doFullCopy(catTofCluster)){
		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catTofCluster);
                    HCategory* catIn  = HCategoryManager::getCategory(catTofCluster);

		    HLocation loc;
		    loc.set(3,0,0,0);

		    Int_t n = catIn->getEntries();
                    Int_t index;

		    for(Int_t i = 0; i < n; i++){

			if(isSim)
			{
			    HTofClusterSim* hit1 = 0;
			    HTofClusterSim* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catTofCluster,i);

			    loc[0] = hit1->getSector();
			    loc[1] = hit1->getModule();
			    loc[2] = hit1->getCell();

			    hit2 =(HTofClusterSim*) catOut->getSlot(loc,&index);
			    new (hit2) HTofClusterSim(*hit1);

			} else {

			    HTofCluster* hit1 = 0;
			    HTofCluster* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catTofCluster,i);

			    loc[0] = hit1->getSector();
			    loc[1] = hit1->getModule();
			    loc[2] = hit1->getCell();

			    hit2 =(HTofCluster*) catOut->getSlot(loc,&index);
			    new (hit2) HTofCluster(*hit1);

			}
		    }
		}
		//-------------------------------------------


		//-------------------------------------------
		if(fCurrentEvent->getCategory(catRpcCluster) && doFullCopy(catRpcCluster)){


		    HLocation loc;
		    loc.set(2,0,0);

		    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRpcCluster);
		    HCategory* catIn  = HCategoryManager::getCategory(catRpcCluster);

		    Int_t n = catIn->getEntries();
                    Int_t index;

		    for(Int_t i = 0; i < n; i++){
			if(isSim)
			{
			    HRpcClusterSim* hit1 = 0;
			    HRpcClusterSim* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catRpcCluster,i);

			    loc[0] = hit1->getSector();
			    loc[1] = ctRpcClus[hit1->getSector()];

			    hit2 =(HRpcClusterSim*) catOut->getSlot(loc,&index);
			    new (hit2) HRpcClusterSim(*hit1);
			    hit2->setAddress(loc[0],index);

			    ctRpcClus[hit1->getSector()] ++;

			} else {

			    HRpcCluster* hit1 = 0;
			    HRpcCluster* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catRpcCluster,i);

			    loc[0] = hit1->getSector();
			    loc[1] = ctRpcClus[hit1->getSector()];

			    hit2 =(HRpcCluster*) catOut->getSlot(loc,&index);
			    new (hit2) HRpcCluster(*hit1);
			    hit2->setAddress(loc[0],index);

			    ctRpcClus[hit1->getSector()] ++;

			}
		    }
		}
		//-------------------------------------------

		//-------------------------------------------
		if(fCurrentEvent->getCategory(catShowerHit) && doFullCopy(catShowerHit)){

		    HCategory* catOut = fCurrentEvent->getCategory(catShowerHit);
		    HCategory* catIn  = HCategoryManager::getCategory(catShowerHit);

		    Int_t n = catIn->getEntries();
                    Int_t index;

		    for(Int_t i = 0; i < n; i++){
			if(isSim)
			{
			    HShowerHitSim* hit1 = 0;
			    HShowerHitSim* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catShowerHit,i);
			    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);

			} else {

			    HShowerHit* hit1 = 0;
			    HShowerHit* hit2 = 0;
			    hit1 = HCategoryManager::getObject(hit1,catShowerHit,i);
			    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,index);
			}
		    }
		}
		//-------------------------------------------
		//-------------------------------------------






		//-------------------------------------------
		// now fill all categeories which can be reached
		// form HParticleCand. Only objects which are inside
		// candidates which are flagged kIsUsed are taken into
		// account

		HCategory* catIn  = HCategoryManager::getCategory(catParticleCand);
		HCategory* catOut = fCurrentEvent->getCategory(catParticleCand);

		Int_t n = catIn->getEntries();
		HParticleCand *cand, *cand1, *cand2;  cand=cand1=cand2=0;
		HParticleCandSim* cands1, *cands2;    cands1=cands2=0;
		Int_t indexCand2; // use this for all objects stored parallel to candidate2

                Int_t particleMdcInd1;
		Int_t richInd1;
		Int_t seg0Ind1;
		Int_t seg1Ind1;
		Int_t rpcInd1;
		Int_t showerInd1;
		Int_t tofInd1;
		Int_t tofClstInd1;

                Int_t particleMdcInd2;
		Int_t richInd2;
		Int_t seg0Ind2;
		Int_t seg1Ind2;
		Int_t rpcInd2;
		Int_t showerInd2;
		Int_t tofInd2;
		Int_t tofClstInd2;


                // remember already copied hits
		map<Int_t,Int_t>mPartMdc;
		map<Int_t,Int_t>mSeg;
                map<Int_t,Int_t>mRich;
                map<Int_t,Int_t>mTofHit;
                map<Int_t,Int_t>mTofClst;
                map<Int_t,Int_t>mRpcClst;
                map<Int_t,Int_t>mShrHit;



		for(Int_t i = 0; i < n; i++){

		    //-------------------------------------------
		    // take only candidates which are marked as used
		    // from those objects retieve the corresponding hit
		    // objects. Indices have to be adopted in the
		    // output object.

		    if(isSim){
			cands1 = HCategoryManager::getObject    (cands1,catIn,i);
			if(kSkipTracks ){

			    if(pUserKeepTrack){
				if(!(*pUserKeepTrack)(cands1)) continue;
			    } else {
				if(!cands1->isFlagBit(Particle::kIsUsed)) continue;
			    }
			}
			cands2 = HCategoryManager::newObjectCopy(cands2,cands1,catOut,indexCand2);
			cands2->setIndex(indexCand2);

			cand = cands2;

                        particleMdcInd1 = cands1->getIndex();
			richInd1        = cands1->getRichInd();
			seg0Ind1        = cands1->getInnerSegInd();
			seg1Ind1        = cands1->getOuterSegInd();
			rpcInd1         = cands1->getRpcInd();
			showerInd1      = cands1->getShowerInd();
			tofInd1         = cands1->getTofHitInd();
			tofClstInd1     = cands1->getTofClstInd();

		    } else {
			cand1 = HCategoryManager::getObject    (cand1,catIn,i);
			if(kSkipTracks){
			    if(pUserKeepTrack){
				if(!(*pUserKeepTrack)(cand1)) continue;
			    } else {
				if(!cand1->isFlagBit(Particle::kIsUsed)) continue;
			    }
			}
			cand2 = HCategoryManager::newObjectCopy(cand2,cand1,catOut,indexCand2);
			cand2->setIndex(indexCand2);

			cand = cand2;

                        particleMdcInd1 = cand1->getIndex();
			richInd1        = cand1->getRichInd();
			seg0Ind1        = cand1->getInnerSegInd();
			seg1Ind1        = cand1->getOuterSegInd();
			rpcInd1         = cand1->getRpcInd();
			showerInd1      = cand1->getShowerInd();
			tofInd1         = cand1->getTofHitInd();
			tofClstInd1     = cand1->getTofClstInd();

		    }




		    //-------------------------------------------

                    particleMdcInd2 = -1;
		    richInd2        = -1;
		    seg0Ind2        = -1;
		    seg1Ind2        = -1;
		    rpcInd2         = -1;
		    showerInd2      = -1;
		    tofInd2         = -1;
		    tofClstInd2     = -1;

		    //-------------------------------------------

		    if(!doFullCopy(catParticleMdc))
		    {
			if(particleMdcInd1 !=-1 && fCurrentEvent->getCategory(catParticleMdc))
			{
			    if(mPartMdc.find(particleMdcInd1) == mPartMdc.end())
			    { // new object
                                HLinearCategory* catOut = (HLinearCategory*)fCurrentEvent->getCategory(catParticleMdc);


                                HParticleMdc* particleMdc1 = 0;
                                HParticleMdc* particleMdc2 = 0;
                                particleMdc1 = HCategoryManager::getObject(particleMdc1,catParticleMdc,particleMdcInd1);
                                particleMdc2 = HCategoryManager::newObjectCopy(particleMdc2,particleMdc1,catOut,particleMdcInd2);

                                particleMdcInd2 = indexCand2;
                                particleMdc2->setIndex(particleMdcInd2);

                                mPartMdc[particleMdcInd1] = particleMdcInd2;

			    }

			}
		    }
		    //-------------------------------------------

		    //-------------------------------------------
		    if(!doFullCopy(catRichHit))
		    {
			if(richInd1 !=-1 && fCurrentEvent->getCategory(catRichHit))
			{
			    if(mRich.find(richInd1) == mRich.end())
			    { // new object
				HLinearCategory* catOut = (HLinearCategory*)fCurrentEvent->getCategory(catRichHit);

				if(isSim)
				{
				    HRichHitSim* hit1 = 0;
				    HRichHitSim* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catRichHit,richInd1);
				    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,richInd2);

				} else {

				    HRichHit* hit1 = 0;
				    HRichHit* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catRichHit,richInd1);
				    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,richInd2);
				}

				mRich[richInd1] = richInd2;

			    } else {  // object was already copied
				richInd2 = mRich[richInd1];
			    }

			    cand->setRichInd(richInd2);
			}
		    } else { cand->setRichInd(richInd1); }
		    //-------------------------------------------

		    //-------------------------------------------
		    if(!doFullCopy(catTofHit))
		    {
			if(tofInd1 !=-1 && fCurrentEvent->getCategory(catTofHit))
			{

			    if(mTofHit.find(tofInd1) == mTofHit.end())
			    { // new object
				HLocation loc;
				loc.set(3,0,0,0);

				HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catTofHit);

				if(isSim)
				{
				    HTofHitSim* hit1 = 0;
				    HTofHitSim* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catTofHit,tofInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = hit1->getModule();
				    loc[2] = hit1->getCell();

				    hit2 =(HTofHitSim*) catOut->getSlot(loc,&tofInd2);
				    new (hit2) HTofHitSim(*hit1);

				} else {

				    HTofHit* hit1 = 0;
				    HTofHit* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catTofHit,tofInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = hit1->getModule();
				    loc[2] = hit1->getCell();

				    hit2 =(HTofHit*) catOut->getSlot(loc,&tofInd2);
				    new (hit2) HTofHit(*hit1);

				}
				mTofHit[tofInd1] = tofInd2;

			    } else {
				// object already copied
				tofInd2 = mTofHit[tofInd1];
			    }
			    cand->setTofHitInd(tofInd2);
			}
		    } else { cand->setTofHitInd(tofInd1); }

		    //-------------------------------------------

		    //-------------------------------------------
		    if(!doFullCopy(catTofCluster))
		    {
			if(tofClstInd1 !=-1 && fCurrentEvent->getCategory(catTofCluster))
			{
			    if(mTofClst.find(tofClstInd1) == mTofClst.end())
			    {

				HLocation loc;
				loc.set(3,0,0,0);

				HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catTofCluster);

				if(isSim)
				{
				    HTofClusterSim* hit1 = 0;
				    HTofClusterSim* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catTofCluster,tofClstInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = hit1->getModule();
				    loc[2] = hit1->getCell();

				    hit2 =(HTofClusterSim*) catOut->getSlot(loc,&tofClstInd2);
				    new (hit2) HTofClusterSim(*hit1);

				} else {

				    HTofCluster* hit1 = 0;
				    HTofCluster* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catTofCluster,tofClstInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = hit1->getModule();
				    loc[2] = hit1->getCell();

				    hit2 =(HTofCluster*) catOut->getSlot(loc,&tofClstInd2);
				    new (hit2) HTofCluster(*hit1);

				}

				mTofClst[tofClstInd1] = tofClstInd2;

			    } else {
				// object already copied
				tofClstInd2 = mTofClst[tofClstInd1];
			    }
			    cand->setTofClstInd(tofClstInd2);
			}
		    } else { cand->setTofClstInd(tofClstInd1); }
		    //-------------------------------------------

		    //-------------------------------------------
		    if(!doFullCopy(catRpcCluster))
		    {
			if(rpcInd1 !=-1 && fCurrentEvent->getCategory(catRpcCluster))
			{
			    if(mRpcClst.find(rpcInd1) == mRpcClst.end())
			    {  // new object

				HLocation loc;
				loc.set(2,0,0);

				HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catRpcCluster);

				if(isSim)
				{
				    HRpcClusterSim* hit1 = 0;
				    HRpcClusterSim* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catRpcCluster,rpcInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = ctRpcClus[hit1->getSector()];

				    hit2 =(HRpcClusterSim*) catOut->getSlot(loc,&rpcInd2);
				    new (hit2) HRpcClusterSim(*hit1);
				    hit2->setAddress(loc[0],rpcInd2);

				    ctRpcClus[hit1->getSector()] ++;

				} else {

				    HRpcCluster* hit1 = 0;
				    HRpcCluster* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catRpcCluster,rpcInd1);

				    loc[0] = hit1->getSector();
				    loc[1] = ctRpcClus[hit1->getSector()];

				    hit2 =(HRpcCluster*) catOut->getSlot(loc,&rpcInd2);
				    new (hit2) HRpcCluster(*hit1);
				    hit2->setAddress(loc[0],rpcInd2);

				    ctRpcClus[hit1->getSector()] ++;

				}

				mRpcClst[rpcInd1] = rpcInd2;

			    } else {
				// object already copied
				rpcInd2 = mRpcClst[rpcInd1];
			    }

			    cand->setRpcInd(rpcInd2);
			}
		    } else { cand->setRpcInd(rpcInd1); }

		    //-------------------------------------------

		    //-------------------------------------------
		    if(!doFullCopy(catShowerHit))
		    {
			if(showerInd1 !=-1 && fCurrentEvent->getCategory(catShowerHit))
			{
			    if(mShrHit.find(showerInd1) == mShrHit.end())
			    { // new object

				HCategory* catOut = fCurrentEvent->getCategory(catShowerHit);

				if(isSim)
				{

				    HShowerHitSim* hit1 = 0;
				    HShowerHitSim* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catShowerHit,showerInd1);
				    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,showerInd2);

				} else {

				    HShowerHit* hit1 = 0;
				    HShowerHit* hit2 = 0;
				    hit1 = HCategoryManager::getObject(hit1,catShowerHit,showerInd1);
				    hit2 = HCategoryManager::newObjectCopy(hit2,hit1,catOut,showerInd2);
				}

				mShrHit[showerInd1] = showerInd2;

			    } else {
				// object already copied
				showerInd2 = mShrHit[showerInd1];
			    }

			    cand->setShowerInd(showerInd2);
			}
		    } else { cand->setShowerInd(showerInd1); }
		    //-------------------------------------------

		    //-------------------------------------------
		    if(seg0Ind1 !=-1 && fCurrentEvent->getCategory(catMdcSeg))
		    {
			if(mSeg.find(seg0Ind1) == mSeg.end()){
                            // new object
			    extractMdcSeg(cand,isSim,seg0Ind1,seg0Ind2);
			    mSeg[seg0Ind1] = seg0Ind2;
			} else {
			    //object already copied
			    cand->setInnerSegInd(mSeg[seg0Ind1]);
			}
		    }
		    //-------------------------------------------

		    //-------------------------------------------
		    if(seg1Ind1 !=-1 && fCurrentEvent->getCategory(catMdcSeg))
		    {
			if(mSeg.find(seg1Ind1) == mSeg.end()){
			    // new object
			    extractMdcSeg(cand,isSim,seg1Ind1,seg1Ind2);
                            mSeg[seg1Ind1] = seg1Ind2;
			} else {
			    //object already copied
			    cand->setOuterSegInd(mSeg[seg1Ind1]);
			}
		    }
		    //-------------------------------------------

		} // loop particlecand


	    } // good event

	    sorter.restoreFlags(kTRUE); // restore only flags of HParticleCand, not setup of HParticleSorter
	    sorter.restoreSetup();
	} // cat particle cand

	if( (goodEvent && kSkipEmptyEvents) || !kSkipEmptyEvents ) fTree->Fill();
    } // fTree

    sorter.restoreSetup();

    return 0;
}

Bool_t HParticleTree::finalize()
{
    closeOutput();
    return kTRUE;

}
void HParticleTree::setEventStructure(Int_t n,Cat_t PersistentCat[],Bool_t fullCopy)
{

    // user definition of the event structure by an array
    // of category numbers. The categories will be created
    // only if they are already present in the input

    for(Int_t i = 0; i < n; i++){

	if(find(fmCatNumSupport.begin(),fmCatNumSupport.end(),PersistentCat[i]) != fmCatNumSupport.end()){ // supported ?

	    if(find(fCatNums.begin(),fCatNums.end(),PersistentCat[i]) == fCatNums.end()){ // not yet added ?
		fCatNums.push_back(PersistentCat[i]); //fmCatNumToFullCopy;
	    } else {
		Warning("setEventStructure()","Cat %i already in event structure ! Skipped second time.",PersistentCat[i]);
                continue;
	    }
	    if(fullCopy ){
		if(find(fmCatNumFullCopySupport.begin(),fmCatNumFullCopySupport.end(),PersistentCat[i]) != fmCatNumFullCopySupport.end() ){
                   fmCatNumToFullCopy [PersistentCat[i]] = 1;
		} else {
                   Warning("setEventStructure()","Cat %i not supported for fullCopy ! added in filtered copy mode.",PersistentCat[i]);
                   fmCatNumToFullCopy [PersistentCat[i]] = 0;
		}
	    } else {
		fmCatNumToFullCopy [PersistentCat[i]] = 0;
	    }
	} else {
	    Warning("setEventStructure()","Cat %i not supported, will be skipped!",PersistentCat[i]);
	}
    }
    sort(fCatNums.begin(),fCatNums.end());

}

void HParticleTree::setEvent()
{
    // analyzes the current event of Hades and
    // sets the own event structure arccording
    // to the selected categories.



    HRecEvent* event = ((HRecEvent*) gHades->getCurrentEvent());


    fCurrentEvent = new HRecEvent();
    TObjArray* pevts = event->getPartialEvents();

    map<TString , Int_t> mpOut;

    cout<<"#########################################################"<<endl;
    cout<<"From task : name = "<< this->GetName()<<", title = "<<this->GetTitle()<<endl;
    cout<<"Partial events found in input = "<<pevts->GetEntries()<<endl;
    cout<<"Creating output event structure :"<<endl;
    Bool_t doAll = fCatNums.size() == 0 ? kTRUE : kFALSE;

    //------------------------------------------------------
    // find all active partial events in output
    for(Int_t i = 0;i < pevts->GetSize(); i++ ) {

	HPartialEvent* p = (HPartialEvent*)pevts->At(i);
	if(p){
	    if(mpOut.find(p->GetName()) == mpOut.end()){

		TObjArray* cats = p->getCategories( );
                //------------------------------------------------------
		// loop categories of partial event
		Int_t catnum;
		for(Int_t j = 0;j < cats->GetSize(); j++ ) {
                    HCategory* cat = (HCategory*)cats->At(j);
		    if(cat) catnum = cat->getCategory( );
		    if(cat  &&
		       ( doAll || find(fCatNums.begin(),fCatNums.end(),catnum) != fCatNums.end() ) &&
		       mpOut.find(p->GetName()) == mpOut.end())
		    {
			mpOut[p->GetName()] = i;
		    } // cat in list
		} // for cat
	    } // new p
	} // p
    }
    //------------------------------------------------------




    map<TString , Int_t> mp;

    for(Int_t i = 0;i < pevts->GetSize(); i++ ) {

	HPartialEvent* p = (HPartialEvent*)pevts->At(i);
	if(p ){

	    Cat_t catbasenum = p->getBaseCat();

	    if(mp   .find(p->GetName()) == mp   .end() &&
               mpOut.find(p->GetName()) != mpOut.end()
	      ){

		cout<<setw(2)<<i<<" partial "<<setw(15)<<p->GetName()<<" title "<<setw(15)<<p->GetTitle()<<" catbase = "<<setw(5)<<catbasenum<<endl;
		fCurrentEvent->addPartialEvent(new HPartialEvent(p->GetName(),p->GetTitle(),p->getBaseCat()));

		TObjArray* cats = p->getCategories( );

		//------------------------------------------------------
                // loop categories of partial event
                Int_t catnum = 0;
		for(Int_t j = 0;j < cats->GetSize(); j++ ) {
		    HCategory* cat = (HCategory*) cats->At(j);
		    if(cat) catnum = cat->getCategory( );
		    if(cat && (doAll || find(fCatNums.begin(),fCatNums.end(),catnum) != fCatNums.end() ) )
		    {
                        TString type = cat->GetName() ;

			fmCatNameToNum[cat->getClassName()] = catnum;
			fmCatNumToName[catnum] = cat->getClassName();


			if        (type == "HLinearCategory"){
			    cout<<"\t"<<setw(2)<<j<<" cat "<<setw(15)<<cat->GetName()<<" "<<setw(20)<<cat->getClassName()<<" catnum "<<setw(5)<<catnum<<endl;

                            HLinearCategory* catL = ((HLinearCategory*)cat);
			    const TObjArray* ar = catL->getData();
			    Int_t size = ar->GetSize();
                            HLinearCategory* catNew = new HLinearCategory(cat->getClassName(),size);
			    catNew->setDynamicObjects(catL->getDynamicObjects());
			    fCurrentEvent->addCategory(cat->getCategory( ),catNew,p->GetName());

			    fmCatNumToPointer [catnum] = catNew ;
                            fmCatNameToPointer[cat->getClassName()] = catNew ;

			} else if (type == "HMatrixCategory"){
                            HMatrixCategory* catM = ((HMatrixCategory*)cat);
			    TArrayI* ar = catM->getSizes();

			    cout<<"\t"<<setw(2)<<j<<" cat "<<setw(15)<<cat->GetName()<<" "<<setw(20)<<cat->getClassName()<<" catnum "<<setw(5)<<catnum
				<<" sizes: "<<ar->GetSize()<<" : "<<flush;
			    for(Int_t l=0; l < ar->GetSize(); l ++){
				cout<<ar->At(l)<<" "<<flush;
			    }
			    cout<<endl;



			    HMatrixCategory* catNew = new HMatrixCategory(cat->getClassName(),ar->GetSize(),ar->GetArray(),1.);
                            fCurrentEvent->addCategory(cat->getCategory( ),catNew,p->GetName());
 			    fmCatNumToPointer [catnum] = catNew ;
                            fmCatNameToPointer[cat->getClassName()] = catNew ;
			}
		    } // cat
		}
                //------------------------------------------------------
		mp[p->GetName()]=i;
	    } // new partial event
	} // p
    }
    //------------------------------------------------------
    cout<<"#########################################################"<<endl;

}


void HParticleTree::setOutputFile (TString fname,TString ftitle,TString fopt,Int_t fcomp)
{
    // fname = filename (including path)
    // if this function is called the output filename
    // will not be derived from Hades output (if exists)
    // or the data input

    fOutputFileName    = fname;
    fOutputTitle       = ftitle;
    fOutputOption      = fopt;
    fOutputCompression = fcomp;
}

Bool_t HParticleTree::makeTree(void)
{
    // Creates an output tree from the information in the event structure
    // and according to the splitLevel (see setSplitLevel() )
    TBranch *b = NULL;
    Bool_t r   = kFALSE;
    Text_t treeTitle[255];

    if (fTree) delete fTree;

    sprintf(treeTitle,"T.%i",gHades->getSplitLevel());

    if (fOutputFile) { fOutputFile->cd(); } // Make sure output file is current file

    fTree = new HTree("T",treeTitle);
    //gTree = fTree;    //???
    if (fCurrentEvent && fTree)
    {
	if (gHades->getSplitLevel() == 0) {
	    b = fTree->Branch("Event",
			      fCurrentEvent->ClassName(),
			      &fCurrentEvent,64000,0);
	} else {
	    b = fTree->Branch("Event.",
			      fCurrentEvent->ClassName(),
			      &fCurrentEvent,gHades->getTreeBufferSize(),99);
	    fCurrentEvent->makeBranch(b,fTree);
	}
	if (b) {
	    r = kTRUE;
	}
    }
    return r;
}

void HParticleTree::closeOutput()
{
    // close the output file and write
    // the Hades event structure
    if (fOutputFile)
    {
	fOutputFile->cd();

	if (fCurrentEvent)
	{
	    if (fCurrentEvent->InheritsFrom("HRecEvent")) { //!!!!
		Bool_t expand = ((HRecEvent *)fCurrentEvent)->hasExpandedStreamer();
		( (HRecEvent *)fCurrentEvent)->setExpandedStreamer(kTRUE);
		fCurrentEvent->Clear();
		fCurrentEvent->Write("Event");
		( (HRecEvent *)fCurrentEvent)->setExpandedStreamer(expand);
	    } else {
		fCurrentEvent->Clear();
		fCurrentEvent->Write("Event");
	    }
	}

	fOutputFile->Write();
	delete fOutputFile;
        fOutputFile = NULL;
	fTree       = 0;
    }
}

void HParticleTree::recreateOutput(void)
{
    // Create new output file in case a split file is
    // needed. Takes care of the special Hades tree
    // features

    fCycleNumber ++;
    Bool_t createTree = (fTree != 0);

    closeOutput();

    TString fname = fOutputFileName;
    fname.ReplaceAll(".root","");

    fOutputFileName = Form("%s_%i.root",fname.Data(),fCycleNumber);

    fOutputFile = new TFile(fOutputFileName.Data(),fOutputOption.Data(),fOutputTitle.Data(),fOutputCompression);
    if (createTree) { makeTree(); }

}

void HParticleTree::extractMdcSeg(HParticleCand* cand, Bool_t isSim,Int_t segInd1,Int_t& segInd2)
{
    // extract HMdcSeg. HMdcHit,HMdcCal1,HMdcClus,HMdcClusInf,
    // HMdcClusFit,HMdcWireFit are extracted for this segment
    // it enabled.


    HLocation loc;
    loc.set(3,0,0,0);

    HMatrixCategory* catOut = (HMatrixCategory*)fCurrentEvent->getCategory(catMdcSeg);
    HMdcSeg* seg = 0;
    if(isSim)
    {
	HMdcSegSim* hit1 = 0;
	HMdcSegSim* hit2 = 0;
	hit1 = HCategoryManager::getObject(hit1,catMdcSeg,segInd1);

	loc[0] = hit1->getSec();
	loc[1] = hit1->getIOSeg();
	loc[2] = ctMdcSeg[hit1->getSec()][hit1->getIOSeg()];

	hit2 =(HMdcSegSim*) catOut->getSlot(loc,&segInd2);
	new (hit2) HMdcSegSim(*hit1);

        seg = hit2;

    } else {

	HMdcSeg* hit1 = 0;
	HMdcSeg* hit2 = 0;
	hit1 = HCategoryManager::getObject(hit1,catMdcSeg,segInd1);

	loc[0] = hit1->getSec();
	loc[1] = hit1->getIOSeg();
	loc[2] = ctMdcSeg[hit1->getSec()][hit1->getIOSeg()];

	hit2 =(HMdcSeg*) catOut->getSlot(loc,&segInd2);
	new (hit2) HMdcSeg(*hit1);

	seg = hit2;
    }

    if(seg->getIOSeg()==0) cand->setInnerSegInd(segInd2);
    else                   cand->setOuterSegInd(segInd2);

    ctMdcSeg[seg->getSec()][seg->getIOSeg()]++;

    extractMdcCal1        (isSim,segInd1);
    extractMdcCal1FromClus(isSim,segInd1);


    //--------------------------------------------------------
    // input objects
    HMdcClus* clus         = HParticleTool::getMdcClus(segInd1);
    HMdcHit* mdchit0       = HParticleTool::getMdcHit(segInd1,0);
    HMdcHit* mdchit1       = HParticleTool::getMdcHit(segInd1,1);
    HMdcClusInf* clusinf0  = HParticleTool::getMdcClusInf(segInd1,0);
    HMdcClusInf* clusinf1  = HParticleTool::getMdcClusInf(segInd1,1);
    HMdcClusFit* clusfit   = HParticleTool::getMdcClusFit(segInd1);
    //--------------------------------------------------------

    //--------------------------------------------------------
    // output objects
    HCategory* wireFitCat = fCurrentEvent->getCategory(catMdcWireFit);
    HCategory* clusFitCat = fCurrentEvent->getCategory(catMdcClusFit);
    HCategory* clusInfCat = fCurrentEvent->getCategory(catMdcClusInf);
    HMatrixCategory* hitCat  = (HMatrixCategory*) fCurrentEvent->getCategory(catMdcHit);
    HMatrixCategory* clusCat = (HMatrixCategory*) fCurrentEvent->getCategory(catMdcClus);


    HMdcClus*    clus2      = 0;
    HMdcClusFit* clusfit2   = 0;
    HMdcClusInf* clusinf0_2 = 0;
    HMdcClusInf* clusinf1_2 = 0;
    HMdcHit* mdchit0_2      = 0;
    HMdcHit* mdchit1_2      = 0;
    Int_t clusInd2     = -1;
    Int_t clusfitInd2  = -1;
    Int_t clusinf0Ind2 = -1;
    Int_t clusinf1Ind2 = -1;
    Int_t mdchit0Ind2  = -1;
    Int_t mdchit1Ind2  = -1;
    //--------------------------------------------------------

    //--------------------------------------------------------
    // copy HMdcClus  (ownindex: need in HMdcClusFit and HMdcClus)
    if(clus && clusCat){
	HLocation loc;
        loc.set(3,0,0,0);
        loc[0] = clus->getSec();
        loc[1] = clus->getIOSeg();
        loc[2] = ctMdcClus[clus->getSec()][clus->getIOSeg()];

	clus2 =(HMdcClusSim*) clusCat->getSlot(loc,&clusInd2);
	if(isSim){
	    new (clus2) HMdcClusSim(*((HMdcClusSim*)clus));
	} else {
	    new (clus2) HMdcClus(*clus);
	}
	clus2->setOwnIndex(clusInd2);
        clus2->setSegIndex(segInd2);

	ctMdcClus[clus->getSec()][clus->getIOSeg()]++;
    }
    //--------------------------------------------------------

    //--------------------------------------------------------
    // copy HMdcClusFit  (need in HMdcClus index and first+last HMdcWireFit)
    // ownindex needed by HMdcClusInf
    if(clusfit && clusFitCat){
	if(isSim){
	    HMdcClusFitSim* clusfits2 = 0;
	    clusfits2 = HCategoryManager::newObjectCopy(clusfits2,(HMdcClusFitSim*)clusfit,clusFitCat,clusfitInd2);
            clusfit2  = clusfits2;
	} else {
	    clusfit2 = HCategoryManager::newObjectCopy(clusfit2,clusfit,clusFitCat,clusfitInd2);
	}
	clusfit2->setClustCatIndex(clusInd2);
    }
    //--------------------------------------------------------

    //--------------------------------------------------------
    // copy HMdcClusInf  (need in HMdcClus index and HMdcClusFit)
    // ownindex needed by HMdcHit
    if(clusinf0 && clusInfCat){
	if(isSim){
	    HMdcClusInfSim* clusinfs2 = 0;
	    clusinfs2 = HCategoryManager::newObjectCopy(clusinfs2,(HMdcClusInfSim*)clusinf0,clusInfCat,clusinf0Ind2);
	    clusinf0_2 = clusinfs2;
	} else {
	    clusinf0_2 = HCategoryManager::newObjectCopy(clusinf0_2,clusinf0,clusInfCat,clusinf0Ind2);
	}
	clusinf0_2->setClusIndex(clusInd2);
        clusinf0_2->setClusFitIndex(clusfitInd2);
    }
    if(clusinf1 && clusInfCat){
	if(isSim){
	    HMdcClusInfSim* clusinfs2 = 0;
	    clusinfs2 = HCategoryManager::newObjectCopy(clusinfs2,(HMdcClusInfSim*)clusinf1,clusInfCat,clusinf1Ind2);
	    clusinf1_2 = clusinfs2;
	} else {
	    clusinf1_2 = HCategoryManager::newObjectCopy(clusinf1_2,clusinf1,clusInfCat,clusinf1Ind2);
	}
	clusinf1_2->setClusIndex(clusInd2);
        clusinf1_2->setClusFitIndex(clusfitInd2);
    }
    //--------------------------------------------------------

    //--------------------------------------------------------
    // copy HMdcHit  (need in HMdcClusInf index)
    if( mdchit0 && hitCat){
	HLocation loc;
        loc.set(3,0,0,0);
        loc[0] = mdchit0->getSector();
        loc[1] = mdchit0->getModule();
        loc[2] = ctMdcHit[mdchit0->getSector()][mdchit0->getModule()];

	mdchit0_2 =(HMdcHit*) hitCat->getSlot(loc,&mdchit0Ind2);
	if(isSim){
	    new (mdchit0_2) HMdcHitSim(*((HMdcHitSim*)mdchit0));
	} else {
	    new (mdchit0_2) HMdcHit(*mdchit0);
	}

	mdchit0_2->setClusInfIndex(clusinf0Ind2);

	ctMdcHit[mdchit0->getSector()][mdchit0->getModule()]++;
    }
    if( mdchit1 && hitCat){
	HLocation loc;
        loc.set(3,0,0,0);
        loc[0] = mdchit1->getSector();
        loc[1] = mdchit1->getModule();
        loc[2] = ctMdcHit[mdchit1->getSector()][mdchit1->getModule()];

	mdchit1_2 =(HMdcHit*) hitCat->getSlot(loc,&mdchit1Ind2);
	if(isSim){
	    new (mdchit1_2) HMdcHitSim(*((HMdcHitSim*)mdchit1));
	} else {
	    new (mdchit1_2) HMdcHit(*mdchit1);
	}

	mdchit1_2->setClusInfIndex(clusinf1Ind2);

	ctMdcHit[mdchit1->getSector()][mdchit1->getModule()]++;
    }

    if(seg) {
        seg->setHitInd(0,mdchit0Ind2);
        seg->setHitInd(1,mdchit1Ind2);
    }
    //--------------------------------------------------------




    if(wireFitCat)
    {
	Int_t first = -1;
	Int_t last  = -1;

	if(clusfit) {
	    first = clusfit->getFirstWireFitInd();
	    last  = clusfit->getLastWireFitInd();
	}

	//--------------------------------------------------------
	// copy wirefit objects (indices : first and last needed by HMdcClusFit )
	TObjArray* awire = HParticleTool::getMdcWireFitSeg(segInd1);
        Int_t ind2   =  0;

	Int_t first2 = -1;
        Int_t last2  = -1;

	for(Int_t i = first; i <= last; i ++){

	    if(isSim){
		HMdcWireFitSim* hit1 = 0;
		HMdcWireFitSim* hit2 = 0;
		hit1 = HCategoryManager::getObject(hit1,catMdcWireFit,i);
		hit2 = HCategoryManager::newObjectCopy(hit2,hit1,wireFitCat,ind2);

	    } else {

		HMdcWireFit* hit1 = 0;
		HMdcWireFit* hit2 = 0;
		hit1 = HCategoryManager::getObject(hit1,catMdcWireFit,i);
		hit2 = HCategoryManager::newObjectCopy(hit2,hit1,wireFitCat,ind2);
	    }

	    if(i == first) first2 = ind2;
            last2 = ind2;
	}
        //--------------------------------------------------------

	if(clusfit2) {
	    clusfit2->setFirstWireFitInd(first2);
	    clusfit2->setLastWireFitInd(last2);
	}

	delete awire;
    }
}

void HParticleTree::extractMdcCal1(Bool_t isSim,Int_t segInd)
{
    // extract all HMdcCal1 objects for a given HMdcSeg

    HMatrixCategory* catOut = (HMatrixCategory*) fCurrentEvent->getCategory(catMdcCal1) ;
    if(catOut){

	TObjArray* ar = HParticleTool::getMdcCal1Seg(segInd);

	if(ar){

	    HLocation loc;
            loc.set(4,0,0,0,0);
	    Int_t n = ar->GetSize();
	    for(Int_t i = 0; i < n ; i++){
		HMdcCal1* hit1 = (HMdcCal1*)ar->At(i);
		if(hit1){
		    loc[0] = hit1 ->getSector();
                    loc[1] = hit1 ->getModule();
                    loc[2] = hit1 ->getLayer();
                    loc[3] = hit1 ->getCell();

		    if(!catOut->getObject(loc)){ // object does not exist
                        Int_t index=0;
			if(isSim){
			    HMdcCal1Sim* hit2 = 0;
			    hit2 =(HMdcCal1Sim*) catOut->getSlot(loc,&index);
			    new (hit2) HMdcCal1Sim(*((HMdcCal1Sim*)hit1));
			} else {
			    HMdcCal1* hit2 = 0;
			    hit2 =(HMdcCal1*) catOut->getSlot(loc,&index);
			    new (hit2) HMdcCal1(*hit1);
			}
		    }
		}
	    }
	}

        delete ar;
    }
}
void HParticleTree::extractMdcCal1FromClus(Bool_t isSim,Int_t segInd)
{
    // extract all HMdcCal1 objects for a given HMdcClus

    HMatrixCategory* catOut = (HMatrixCategory*) fCurrentEvent->getCategory(catMdcCal1) ;
    if(catOut){

	TObjArray* ar = HParticleTool::getMdcCal1Cluster(segInd);

	if(ar){

	    HLocation loc;
            loc.set(4,0,0,0,0);
	    Int_t n = ar->GetSize();
	    for(Int_t i = 0; i < n ; i++){
		HMdcCal1* hit1 = (HMdcCal1*)ar->At(i);
		if(hit1){
		    loc[0] = hit1 ->getSector();
                    loc[1] = hit1 ->getModule();
                    loc[2] = hit1 ->getLayer();
                    loc[3] = hit1 ->getCell();

		    if(!catOut->getObject(loc)){ // object does not exist
                        Int_t index=0;
			if(isSim){
			    HMdcCal1Sim* hit2 = 0;
			    hit2 =(HMdcCal1Sim*) catOut->getSlot(loc,&index);
			    new (hit2) HMdcCal1Sim(*((HMdcCal1Sim*)hit1));
			} else {
			    HMdcCal1* hit2 = 0;
			    hit2 =(HMdcCal1*) catOut->getSlot(loc,&index);
			    new (hit2) HMdcCal1(*hit1);
			}
		    }
		}
	    }
	}

        delete ar;
    }
}

Bool_t HParticleTree::doFullCopy(Cat_t cat)
{
    // returns kTRUE if the catgeory is set to full copy
    // else or if the catgeory is not known kFALSE.
    map<Int_t,Int_t>::iterator iter = fmCatNumToFullCopy.find(cat);
    if(iter != fmCatNumToFullCopy.end() ) {
	return (Bool_t)iter->second;
    }

    return kFALSE;
}

