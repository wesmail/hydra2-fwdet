#include "hparticlepairmaker.h"
#include "hparticlepairdraw.h"
#include "hcategorymanager.h"
#include "hphysicsconstants.h"
#include "hades.h"
#include "hrecevent.h"


#include "TCanvas.h"
#include "TH1F.h"
#include "TLatex.h"
#include "TEllipse.h"
#include "TLine.h"

// ROOT's IO and RTTI stuff is added here
ClassImp(HParticlePairMaker)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticlePairMaker
//
// Build vectors of freference candidates, fothers candidates and fpairs
// for the current event. As reference candidates marked as kIsLepton
// will be used.
//
// nectEvent() to be called inside eventloop after HParticleSorter selection.
// 1. clear all internal structures
// 2. fill vector freference by candidates which are flagged kIsLepton ( or kIsUsed if it is setup)
//    All other candidates (disregarding kIsLepton or kIsUsed) are added to vector fothers
//    To avoid unneeded pairs the user can set a userFilter by set setUserFilter(Bool_t (*userfilter)(HParticleCand*))
//    In this case only candidates passing this function (return kTRUE) will be taken into account
//    for freference and fothers.
// 3. created all pair combinations between freference x freference (no double counting) and freference x fothers.
//       If setDoSkippedFullCandPairs(kTRUE) is  set (default) in addition all pairs of full reconstructed
//       particles ( inner+outer MDC + META) of fOthers are build (ffullrecoOthers x ffullrecoOthers +
//       ffullrecoOthers x fnofullrecoOthers ). All pairs with full reco candidates of the event are build
//       this way for investigation, even the ones which are skipped from reference by any criteria or double hit
//       rejection.
//    a. pass the selectPID1 and selectPID2 functions
//       will get pid1 or pid2 assigned. Otherwise pid is -2(fake+), -1(fake-) (see HPhysicsConstants).
//       if pid2 fails it will get pid assigned opposite to reference cand in pairCase10.
//       fakes for pid2 will be assigned by polarity or opposite to reference cand.
//    b. eClosePairSelect flags will be set for each pair (see hparticledef.h).
//       This flags can be used later to classify the pairs.
//    c. pairs are inserted in fpairs vector. With static HParticlePair::setDoMomentumCorrection(Bool_t doit)
//       the enegryloss correction can be swithced on/off
//    d. secondary vertex and vertex cut vars are calulated. Which  primary ebevnt vertex
//       should be used has to be set by the user (default = kVertexParticle, see eVertex
//       in hparticledef.h). The user can provide an own event vertex per event by calling setVertex().
//
//
// Bool_t  HParticleTool::evalPairsFlags(UInt_t flag,HParticlePair& pair)
// can be used to classify the pair according to the predefined pair cases
// in ePairCase + eClosePairSelect (hparticledef.h). There are dedicated filter
// functions provided to select from the pairs vector
//
//BEGIN_HTML
// <img src="http://web-docs.gsi.de/~halo/docs/hydra/classDocumentation/docu_pics/hydra2/particle/lepton_pair_cases_1.png" alt="Lepton Pair Cases" width="750" >
// <img src="http://web-docs.gsi.de/~halo/docs/hydra/classDocumentation/docu_pics/hydra2/particle/lepton_pair_cases_2.png" alt="Lepton Pair Cases" width="750" >
//END_HTML
//   Fig 1. All predefined pairCases for Leptons
//
//BEGIN_HTML
// <img src="http://web-docs.gsi.de/~halo/docs/hydra/classDocumentation/docu_pics/hydra2/particle/hadron_pair_cases.png" alt="Hadron Pair Cases" width="750" >
//END_HTML
//   Fig 2. All predefined pairCases for Hadrons
//
//BEGIN_HTML
// <img src="http://web-docs.gsi.de/~halo/docs/hydra/classDocumentation/docu_pics/hydra2/particle/lepton_hadron_pair_cases.png" alt="Mixed Lepton Hadron Pair Cases" width="525" >
//END_HTML
//   Fig 2. All predefined pairCases for mixed Lepton Hadrons
//
    //
    //-----------------------------------------------------------------------------
    // USAGE:
    // #include  "hparticlepairmaker.h"
    // #include  "hparticledef.h"
    // #include  "hparticletool.h"
    // #include  <vector>
    // using namespace std;
    // ....
    //
    //
    // {
    // // setup HLoop etc ....
    //
    // ....
    //  HEnergyLossCorrPar enLossCorr;
    //  enLossCorr.setDefaultPar("apr12"); //  "jan04","nov02","aug04","apr12"
    //
    // //---------------------------------------------
    // // HPARTICLEPAIRMAKER SETUP :
    // HParticlePairMaker pairmaker;
    // // pairmaker.setPIDs(2,3,51) // default
    // // pairmaker.setPIDsSelection(mySelectionPID1,mySelectionPID2) // set your own selection function for pid1+pid2 (signature :  Bool_t myfunction(HParticleCand*) )
    // // pairmaker.setUseLeptons(kTRUE);// kTRUE : use kIsLepton, kFALSE: kIsUsed for selection of reference  (default kTRUE)
    // // pairmake.setRequireRich(kFALSE); // do not ask for rich Hit in selection functions ( default = kTRUE)
    // // pairmaker.setVertexCase(Particle::kVertexParticle); // select which event vertex to use (default = kVertexParticle)
    // // HParticlePair::setDoMomentumCorrection(kTRUE); // default : kTRUE
    // //---------------------------------------------
    //
    // vector<HParticlePair*> pairs;
    //
    // for (Int_t i=0; i < entries; i++) {
    //    Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
    //    if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
    //
    //
    //    //--------------------------------------------------------------------------
    //    // HPARTICLETRACKSORTER SETUP
    //    sorter.cleanUp();
    //    sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);     // reset all flags for flags (0-28) ,reject,used,lepton
    //    Int_t nCandLep     = sorter.fill(HParticleTrackSorter::selectLeptons);   // fill only good leptons
    //    Int_t nCandLepBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
    //    Int_t nCandHad     = sorter.fill(HParticleTrackSorter::selectHadrons);   // fill only good hadrons (already marked good leptons will be skipped)
    //    Int_t nCandHadBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);
    //    //--------------------------------------------------------------------------
    //
    //
    //    //###########################################################################
    //    // HPARTICLEPAIRMAKER ACTION:
    //    pairmaker.nextEvent();  // call it after your HParticleSorter action!
    //
    //
    //
    //    //--------------------------------------------------------------------------
    //    // example  1:
    //    // fill list of pairs for pairCase1 and require
    //    // fitted inner+out+RK for second leg in addition
    //    pairmaker.filterPairsVector(pairs,kPairCase1|kFittedInnerMDC2|kFittedInnerMDC2|kRK2);
    //
    //    for(UInt_t j = 0; j < pairs.size(); j++ ){ // print the pairs
    //       pairs[j]->print();
    //    }
    //    //--------------------------------------------------------------------------
    //
    //
    //    //--------------------------------------------------------------------------
    //    // example 2:
    //    // get list of all pairs and loop over the list
    //    // plot different pairCases
    //
    //    vector<HParticlePair>& pairsVec =  pairmaker.getPairsVector();
    //    for(UInt_t l = 0; l < pairsVec.size(); l++ ){
    //        HParticlePair& pair = pairsVec[l];
    //
    //        if(HParticleTool::evalPairsFlags(kPairCase1,pair)){
    //    	cout<<"case1"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase2,pair)){
    //    	cout<<"case2"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase3,pair)){
    //    	cout<<"case3"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase4,pair)){
    //    	cout<<"case4"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase5,pair)){
    //    	cout<<"case5"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase6,pair)){
    //    	cout<<"case6"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase7,pair)){
    //    	cout<<"case7"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase8,pair)){
    //    	cout<<"case8"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase9,pair)){
    //    	cout<<"case9"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase10,pair)){
    //    	cout<<"case10"<<endl;
    //        } else if(HParticleTool::evalPairsFlags(kPairCase11,pair)){
    //   	cout<<"case11"<<endl;
    //        }
    //    }
    //    //--------------------------------------------------------------------------
    //    //###########################################################################
    //   }  // end event loop
    // }
////////////////////////////////////////////////////////////////////////////////

Bool_t HParticlePairMaker::frequireRich = kTRUE;

HParticlePairMaker::HParticlePairMaker()
{
    fPID1      = HPhysicsConstants::pid("e+");
    fPID2      = HPhysicsConstants::pid("e-");
    fMotherPID = HPhysicsConstants::pid("dilepton");
    fselectPID1 = 0;
    fselectPID2 = 0;
    fuserFilter = 0;
    fuse_kIsLepton = kTRUE;
    fVertexCase = kVertexParticle ;
    fdoSkippedFullCandPairs = kTRUE;
    fpairs    .resize(10000);
    fothers   .resize(1000);
    ffullrecoOthers   .resize(100);
    fnofullrecoOthers .resize(1000);
    freference.resize(100);


    fCaseVec.push_back(kPairCase1);
    fCaseVec.push_back(kPairCase2);
    fCaseVec.push_back(kPairCase3);
    fCaseVec.push_back(kPairCase4);
    fCaseVec.push_back(kPairCase5);
    fCaseVec.push_back(kPairCase6);
    fCaseVec.push_back(kPairCase7);
    fCaseVec.push_back(kPairCase8);
    fCaseVec.push_back(kPairCase9);
    fCaseVec.push_back(kPairCase10);
    fCaseVec.push_back(kPairCase11);
    fCaseVec.push_back(kPairCase12);
    fCaseVec.push_back(kPairCase13);
    fCaseVec.push_back(kPairCase14);
    fCaseVec.push_back(kPairCase15);

    for(UInt_t i=0;i<fCaseVec.size();i++) fCaseCt.push_back(0);
    richCandCt = 0 ;
}

HParticlePairMaker::~HParticlePairMaker()
{
     clearVectors();
}

Bool_t HParticlePairMaker::selectPos(HParticleCand* cand)
{
    // standard function for positrons
    // require richmatch (broad window,  frequireRich =kTRUE)
    // Select positive polatity

    if(getRequireRich() && cand->getRichInd() == -1) return kFALSE;

    if( cand->getCharge() > 0 ) return kTRUE;
    else                        return kFALSE;
}

Bool_t HParticlePairMaker::selectNeg(HParticleCand* cand)
{
    // standard function for electrons
    // require richmatch (broad window,  frequireRich =kTRUE)
    // Select negative polatity

    if(getRequireRich() && cand->getRichInd() == -1) return kFALSE;

    if( cand->getCharge() < 0 ) return kTRUE;
    else                        return kFALSE;
}

void HParticlePairMaker::bookHits(HParticleCand* cand1)
{
    // add candidate hits to maps

    if(cand1->getRichInd() !=-1 ) {
	if(mRichtoCand.find(cand1->getRichInd()) == mRichtoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mRichtoCand[cand1->getRichInd()] = v;
	} else {
	    mRichtoCand[cand1->getRichInd()].push_back(cand1);
	}
    }

    if(cand1->getInnerSegInd() !=-1 ) {
	if(mInnerMdctoCand.find(cand1->getInnerSegInd()) == mInnerMdctoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mInnerMdctoCand[cand1->getInnerSegInd()] = v;
	} else {
	    mInnerMdctoCand[cand1->getInnerSegInd()].push_back(cand1);
	}
    }

    if(cand1->getOuterSegInd() !=-1 ) {
	if(mOuterMdctoCand.find(cand1->getOuterSegInd()) == mOuterMdctoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mOuterMdctoCand[cand1->getOuterSegInd()] = v;
	} else {
	    mOuterMdctoCand[cand1->getOuterSegInd()].push_back(cand1);
	}
    }

    if(cand1->getSystemUsed() != -1){
	Int_t meta = cand1->getMetaHitInd();
	if(cand1->isTofHitUsed()){
	    if(mTofHittoCand.find(meta) == mTofHittoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mTofHittoCand[meta] = v;
	    } else {
		mTofHittoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isTofClstUsed()){
	    if(mTofClsttoCand.find(meta) == mTofClsttoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mTofClsttoCand[meta] = v;
	    } else {
		mTofClsttoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isRpcClstUsed()){
	    if(mRpcClsttoCand.find(meta) == mRpcClsttoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mRpcClsttoCand[meta] = v;
	    } else {
		mRpcClsttoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isShowerUsed()){
	    if(mShowertoCand.find(meta) == mShowertoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mShowertoCand[meta] = v;
	    } else {
		mShowertoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isEmcUsed()){
	    if(mEmctoCand.find(meta) == mEmctoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mEmctoCand[meta] = v;
	    } else {
		mEmctoCand[meta].push_back(cand1);
	    }
	}
    }
}
void HParticlePairMaker::selectPID(HParticleCand* cand1,Int_t& pid1,Bool_t warn)
{
    if(       (*fselectPID1)(cand1) ) { pid1 = fPID1;}    // one of the 2 functions should set the PID1 , take care about others!
    else if  ((*fselectPID2)(cand1) ) { pid1 = fPID2;}
    else     {
	if(warn){
	    Warning("nextEvent()","Reference particle not labeled with PID! Should not happen. Check your selection functions. frequireRich = kTRUE but kIsLepton is not used as referencce this might cause empty pids.");
	    cand1->print();
	}
    }
}

void HParticlePairMaker::nextEvent()
{
    // to be called inside eventloop after HParticleSorter selection.
    // 1. clear all internal structures
    // 2. fill vector freference by candidates which are flagged kIsLepton
    //    All other candidates are added to vector fothers
    // 3. created all pair combinations between freference and fothers which
    //    a. pass the selectPID1 and selectPID2 functions
    //       will get pid1 or pid2 assigned. Otherwise pid is -2(fake+), -1(fake-) (see HPhysicsConstants).
    //       if pid2 fails it will get pid assigend opposite to reference cand in pairCase10.
    //       fakes for pid2 will be assigned by polarity or opposite to reference cand.
    //    b. eClosePairSelect flags will be set for each pair.
    //       This flags can be used later to classify the pairs.
    //    c. pairs are inserted in fpairs vector.


    clearVectors();

    if(!fselectPID1) fselectPID1 = HParticlePairMaker::selectPos;
    if(!fselectPID2) fselectPID2 = HParticlePairMaker::selectNeg;

    HRecEvent* event = (HRecEvent*)gHades->getCurrentEvent();
    if(event){
	if(fVertexCase != kVertexUser){
	    fVertex = HParticleTool::getGlobalVertex(fVertexCase,kFALSE);
	}

	HCategory* candCat = (HCategory*) event->getCategory(catParticleCand);
	if(candCat){
	    UInt_t n = candCat->getEntries();
	    HParticleCand* cand1 = 0 ;
	    HParticleCand* cand2 = 0 ;
	    //-------------------------------------------------------
	    // fill lists for reference and others
	    for(UInt_t i=0; i < n; i++){
		cand1 = HCategoryManager::getObject(cand1,candCat,i);

		//-------------------------------------------------------
		//  book the hits -> candiate lists
		bookHits(cand1);
		//-------------------------------------------------------

		if(cand1){

		    if(fuserFilter && !(*fuserFilter)(cand1)) continue;


		    if(( fuse_kIsLepton && cand1->isFlagBit(Particle::kIsLepton)) ||
		       (!fuse_kIsLepton && cand1->isFlagBit(Particle::kIsUsed))
		      ){
			freference.push_back(cand1);
		    } else {
			fothers.push_back(cand1);
		    }
		}
	    } // end fill list loop
	    //-------------------------------------------------------


	    //-------------------------------------------------------
	    //  create pair combinations
	    Int_t pid1,pid2;
	    pid1 = pid2 = -10;

	    //-------------------------------------------------------
	    //  first do good pairs from reference x reference candidates

	    n = freference.size();
	    if(n > 1){
		for(UInt_t i = 0 ; i < n ; i++){
		    cand1 = freference[i];
		    pid1 = -10;

		    selectPID(cand1,pid1,kTRUE);

		    for(UInt_t j = i+1 ; j < n; j++){
			cand2 = freference[j];
			if(cand2->getRichInd()!=-1) richCandCt++;
			UInt_t flag = 0;
			HParticleTool::setPairFlags(flag,cand2,cand1);

			if(!fuse_kIsLepton) flag=flag|kNoUseRICH;

			pid2 = -10;

			selectPID(cand2,pid2,kTRUE);

			HParticlePair pair;
			pair.setPair(cand1,pid1,cand2,pid2,fMotherPID,flag,fVertex);
			fpairs.push_back(pair);

		    } // end loop others
		} // end loop reference x reference
	    }
	    //-------------------------------------------------------

	    //-------------------------------------------------------
	    // now do all pairs reference x other candidates
	    for(UInt_t i = 0 ; i < freference.size(); i++){
		cand1 = freference[i];
		pid1 = -10;

		selectPID(cand1,pid1,kTRUE);

		for(UInt_t j = 0 ; j < fothers.size(); j++){
		    cand2 = fothers[j];
		    if(cand2->getRichInd()!=-1) richCandCt++;
		    UInt_t flag = 0;
		    HParticleTool::setPairFlags(flag,cand2,cand1);

		    if(!fuse_kIsLepton) flag=flag|kNoUseRICH;

		    pid2 = -10;
		    if(        pid1 == fPID2 && (*fselectPID1)(cand2) ) { pid2 = fPID1;}  // PID2 could fail no polarity is defined
		    else if  ( pid1 == fPID1 && (*fselectPID2)(cand2) ) { pid2 = fPID2;}

		    if(pid2 == -10){

			if((flag&kNoOuterMDC2) == kNoOuterMDC2) {
			    // in this case polarity is not defined
			    // selectPID will fail, but we want to set
			    // pid2 in this case

			    if(pid1 == fPID1) pid2 = fPID2;
			    else              pid2 = fPID1;
			}
		    }

		    if(pid2 == -10) { // all cases with no RICH
			if(cand2->getCharge() != 0){
			    if(cand2->getCharge() > 0) pid2 =  HPhysicsConstants::pid("fake+");
			    else                       pid2 =  HPhysicsConstants::pid("fake-");
			} else {
			    if(HPhysicsConstants::charge(pid1) < 0 ) pid2 =  HPhysicsConstants::pid("fake+");
			    else                                     pid2 =  HPhysicsConstants::pid("fake-");
			}

		    }

		    HParticlePair pair;
		    pair.setPair(cand1,pid1,cand2,pid2,fMotherPID,flag,fVertex);
		    fpairs.push_back(pair);
		    //#define dbug
#ifdef  dbug
		    Int_t ct = 0 ;
		    for(UInt_t k = 0; k < fCaseVec.size(); k ++){   // fill statistics for pair cases
			if(HParticleTool::evalPairsFlags(fCaseVec[k],pair) || cand2->getRichInd()==-1) {
			    good2=kTRUE;
			    if(cand2->getRichInd()!=-1) ct++;
			}
		    }
		    if(!good2 || ct>1) pair.print();
#endif
		} // end loop others
	    } // end loop reference
	    //-------------------------------------------------------




	    if(fdoSkippedFullCandPairs)
	    {
                
		//-------------------------------------------------------
		// now do all pairs full reco of others x no full reco other candidates and fullreco others X full reco others
                for(UInt_t j = 0 ; j < fothers.size(); j++){
                    HParticleCand* c = fothers[j];
                    if(c->isFlagAND(4,                                 // Fully reconstructed
				    Particle::kIsAcceptedHitInnerMDC,
				    Particle::kIsAcceptedHitOuterMDC,
				    Particle::kIsAcceptedHitMETA,
				    Particle::kIsAcceptedRK )){
                          ffullrecoOthers.push_back(c);
		    } else {
                          fnofullrecoOthers.push_back(c);
		    }
		}

		//-------------------------------------------------------
                //fullreco others X full reco others
		for(UInt_t i = 0 ; i < ffullrecoOthers.size(); i++){
		    cand1 = ffullrecoOthers[i];
		    pid1 = -10;

		    selectPID(cand1,pid1,kTRUE);

		    for(UInt_t j = i+1 ; j < ffullrecoOthers.size(); j++){
			cand2 = ffullrecoOthers[j];
			UInt_t flag = 0;
			HParticleTool::setPairFlags(flag,cand2,cand1);

			if(!fuse_kIsLepton) flag=flag|kNoUseRICH;

			pid2 = -10;
			if(        pid1 == fPID2 && (*fselectPID1)(cand2) ) { pid2 = fPID1;}  // PID2 could fail no polarity is defined
			else if  ( pid1 == fPID1 && (*fselectPID2)(cand2) ) { pid2 = fPID2;}

			if(pid2 == -10){

			    if((flag&kNoOuterMDC2) == kNoOuterMDC2) {
				// in this case polarity is not defined
				// selectPID will fail, but we want to set
				// pid2 in this case

				if(pid1 == fPID1) pid2 = fPID2;
				else              pid2 = fPID1;
			    }
			}

			if(pid2 == -10) { // all cases with no RICH
			    if(cand2->getCharge() != 0){
				if(cand2->getCharge() > 0) pid2 =  HPhysicsConstants::pid("fake+");
				else                       pid2 =  HPhysicsConstants::pid("fake-");
			    } else {
				if(HPhysicsConstants::charge(pid1) < 0 ) pid2 =  HPhysicsConstants::pid("fake+");
				else                                     pid2 =  HPhysicsConstants::pid("fake-");
			    }

			}

			HParticlePair pair;
			pair.setPair(cand1,pid1,cand2,pid2,fMotherPID,flag,fVertex);
			fpairs.push_back(pair);

		    } // end loop fullreco
		} // end loop fullreco
		//-------------------------------------------------------

		//-------------------------------------------------------
                //fullreco others X no full reco others
		for(UInt_t i = 0 ; i < ffullrecoOthers.size(); i++){
		    cand1 = ffullrecoOthers[i];
		    pid1 = -10;

		    selectPID(cand1,pid1,kTRUE);

		    for(UInt_t j = 0 ; j < fnofullrecoOthers.size(); j++){
			cand2 = fnofullrecoOthers[j];
			UInt_t flag = 0;
			HParticleTool::setPairFlags(flag,cand2,cand1);

			if(!fuse_kIsLepton) flag=flag|kNoUseRICH;

			pid2 = -10;
			if(        pid1 == fPID2 && (*fselectPID1)(cand2) ) { pid2 = fPID1;}  // PID2 could fail no polarity is defined
			else if  ( pid1 == fPID1 && (*fselectPID2)(cand2) ) { pid2 = fPID2;}

			if(pid2 == -10){

			    if((flag&kNoOuterMDC2) == kNoOuterMDC2) {
				// in this case polarity is not defined
				// selectPID will fail, but we want to set
				// pid2 in this case

				if(pid1 == fPID1) pid2 = fPID2;
				else              pid2 = fPID1;
			    }
			}

			if(pid2 == -10) { // all cases with no RICH
			    if(cand2->getCharge() != 0){
				if(cand2->getCharge() > 0) pid2 =  HPhysicsConstants::pid("fake+");
				else                       pid2 =  HPhysicsConstants::pid("fake-");
			    } else {
				if(HPhysicsConstants::charge(pid1) < 0 ) pid2 =  HPhysicsConstants::pid("fake+");
				else                                     pid2 =  HPhysicsConstants::pid("fake-");
			    }

			}

			HParticlePair pair;
			pair.setPair(cand1,pid1,cand2,pid2,fMotherPID,flag,fVertex);
			fpairs.push_back(pair);

		    } // end loop fullnoreco
		} // end loop fullreco
		//-------------------------------------------------------

	    }  // end fdoSkippedFullCandPairs


	    //-------------------------------------------------------
	    //  book the candidate -> pair lists
	    for(UInt_t i = 0; i < fpairs.size(); i++){
		HParticlePair* pair = &(fpairs[i]);
		cand1 = pair->getCand(0);
		cand2 = pair->getCand(1);


		//-------------------------------------------------------
		// fill statistics for pair cases
		for(UInt_t k = 0; k < fCaseVec.size(); k ++){
		    if(HParticleTool::evalPairsFlags(fCaseVec[k],*pair)) {
			fCaseCt[k]++;
		    }
		}
		//-------------------------------------------------------


		if(mCandtoPair.find(cand1) == mCandtoPair.end()){
		    vector<HParticlePair*> pairs;
		    pairs.push_back(pair);
		    mCandtoPair[cand1] = pairs;
		} else {
		    mCandtoPair[cand1].push_back(pair);
		}
		if(mCandtoPair.find(cand2) == mCandtoPair.end()){
		    vector<HParticlePair*> pairs;
		    pairs.push_back(pair);
		    mCandtoPair[cand2] = pairs;
		} else {
		    mCandtoPair[cand2].push_back(pair);
		}
	    }
	    //-------------------------------------------------------

	}

    }
}

void HParticlePairMaker::clearVectors()
{
    // clear all internal vectors etc
    freference     .clear();
    fothers        .clear();
    fpairs         .clear();
    ffullrecoOthers.clear();
    fnofullrecoOthers.clear();

    mTofHittoCand  .clear();
    mTofClsttoCand .clear();
    mRpcClsttoCand .clear();
    mShowertoCand  .clear();
    mEmctoCand     .clear();
    mInnerMdctoCand.clear();
    mOuterMdctoCand.clear();
    mRichtoCand    .clear();
    mCandtoPair    .clear();
}

void HParticlePairMaker::filterPairsVector(vector<HParticlePair*>& filterpairs,UInt_t flag)
{
    // fill all pairs which fullfill the flag to filterspairs.
    // filterpairs will be cleared automatically before filling.
    filterpairs.clear();
    for(UInt_t i = 0; i < fpairs.size(); i++){
        HParticlePair& pair = fpairs[i];
	if(HParticleTool::evalPairsFlags(flag,pair)){
	    filterpairs.push_back(&pair);
	}
    }
}
void HParticlePairMaker::filterPairsVector(vector<HParticlePair*>& filterpairs,vector<UInt_t>& flags)
{
    // fill all pairs which fullfill the flags to filterspairs.
    // filterpairs will be cleared automatically before filling.
    filterpairs.clear();
    for(UInt_t i = 0; i < fpairs.size(); i++){
        HParticlePair& pair = fpairs[i];
	if(HParticleTool::evalPairsFlags(flags,pair)){
	    filterpairs.push_back(&pair);
	}
    }
}

Int_t HParticlePairMaker::filterCandidates(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Float_t oAngle)
{
    // vector candidates with all candidates which have been combined into pairs.
    // if flag == 0 no filtering will be done. Filtering takes place on pairs.
    // Flag can be used to filter for candidates which share 1 or more detector
    // hits with the candidate (see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // In addition a filter of opening Angle of the track can be applied (oAngle < 0 does no selection)
    // The function returns the number of found candidates or -1 if the candidate
    // has not been found at all (should not happen).

    candidates.clear();

    map<HParticleCand*,vector<HParticlePair*> >::iterator it = mCandtoPair.find(cand);
    if(it != mCandtoPair.end()){
	vector<HParticlePair*>& v = it->second;

	for(UInt_t i = 0 ; i < v.size(); i++){
            HParticlePair& pair = *(v[i]);
	    if(HParticleTool::evalPairsFlags(flag,pair) && (oAngle < 0 || pair.getOpeningAngle() < oAngle )){

		if(pair.getCand(0) == cand) candidates.push_back(pair.getCand(1));
                else                        candidates.push_back(pair.getCand(0));
	    }
	}

       return candidates.size();
    }
    return -1;
}

Int_t HParticlePairMaker::filterCandidates (HParticleCand* cand,vector<HParticlePair*>& filterpairs,UInt_t flag,Float_t oAngle)
{
    // Fills vector filterpairs with all pairs which share cand.
    // if flag == 0 no filtering will be done. Filtering takes place on pairs.
    // Flag can be used to filter for candidates which share 1 or more detector
    // hits with the candidate (see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // In addition a filter of opening Angle of the track can be applied (oAngle < 0 does no selection)
    // The function returns the number of found pairs or -1 if the candidate
    // has not been found at all (should not happen).

    filterpairs.clear();
    map<HParticleCand*,vector<HParticlePair*> >::iterator it = mCandtoPair.find(cand);
    if(it != mCandtoPair.end()){
	vector<HParticlePair*>& v = it->second;
        for(UInt_t i = 0 ; i < v.size(); i++){

            if(HParticleTool::evalPairsFlags(flag,*(v[i])) && ( oAngle < 0 || v[i]->getOpeningAngle() < oAngle) ){
		filterpairs.push_back(v[i]);
	    }
	}

    } else return -1;

    return filterpairs.size();

}


Int_t HParticlePairMaker::getSameRich(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
{
    // fills list of candidates which share the same RICH. If the RICH is not found
    // at all -1 is returned, other wise the number of candidates found for the hit.
    // The input candidate will be not included in candidates. Candidates can be filtered
    // by flag ( 0 == no filter, see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // The input candidate will be treated as refererence if isReference == kTRUE.

    candidates.clear();
    if(cand->getRichInd() !=-1 ){
	map<Int_t,vector<HParticleCand*> >::iterator it = mRichtoCand.find(cand->getRichInd());
        if(it != mRichtoCand.end()){
	    vector<HParticleCand*>& v = it->second;
	    for(UInt_t i = 0 ; i < v.size(); i++){

		if(v[i] != cand) {  // skip same cand
		    if(flag == 0 ) candidates.push_back(v[i]);
		    else {
			Bool_t fl = kFALSE;
			if(!isReference) fl=HParticleTool::evalPairsFlags(fl,cand,v[i]);
			else             fl=HParticleTool::evalPairsFlags(fl,v[i],cand);
			if(fl){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticlePairMaker::getSameInnerMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
{
    // fills list of candidates which share the same inner MDC seg. If the seg is not found
    // at all -1 is returned, other wise the number of candidates found for the hit.
    // The input candidate will be not included in candidates. Candidates can be filtered
    // by flag ( 0 == no filter, see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // The input candidate will be treated as refererence if isReference == kTRUE.

    candidates.clear();
    if(cand->getInnerSegInd() !=-1 ){
	map<Int_t,vector<HParticleCand*> >::iterator it = mInnerMdctoCand.find(cand->getInnerSegInd());
        if(it != mInnerMdctoCand.end()){
	    vector<HParticleCand*>& v = it->second;
	    for(UInt_t i = 0 ; i < v.size(); i++){

		if(v[i] != cand) {  // skip same cand
		    if(flag == 0 ) candidates.push_back(v[i]);
		    else {
			Bool_t fl = kFALSE;
			if(!isReference) fl=HParticleTool::evalPairsFlags(fl,cand,v[i]);
			else             fl=HParticleTool::evalPairsFlags(fl,v[i],cand);
			if(fl){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticlePairMaker::getSameOuterMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
{
    // fills list of candidates which share the same outer MDC seg. If the seg is not found
    // at all -1 is returned, other wise the number of candidates found for the hit.
    // The input candidate will be not included in candidates. Candidates can be filtered
    // by flag ( 0 == no filter, see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // The input candidate will be treated as refererence if isReference == kTRUE.

    candidates.clear();
    if(cand->getOuterSegInd() !=-1 ){
	map<Int_t,vector<HParticleCand*> >::iterator it = mOuterMdctoCand.find(cand->getOuterSegInd());
        if(it != mOuterMdctoCand.end()){
	    vector<HParticleCand*>& v = it->second;
	    for(UInt_t i = 0 ; i < v.size(); i++){

		if(v[i] != cand) {  // skip same cand
		    if(flag == 0 ) candidates.push_back(v[i]);
		    else {
			Bool_t fl = kFALSE;
			if(!isReference) fl=HParticleTool::evalPairsFlags(fl,cand,v[i]);
			else             fl=HParticleTool::evalPairsFlags(fl,v[i],cand);
			if(fl){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticlePairMaker::getSameMeta(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
{
    // fills list of candidates which share the same META hit. The number of candidates
    // found for the hit is returned or -1 if the hit is not found at all..
    // The input candidate will be not included in candidates. Candidates can be filtered
    // by flag ( 0 == no filter, see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // The input candidate will be treated as refererence if isReference == kTRUE.

    candidates.clear();

    Int_t metaind = cand->getMetaHitInd();
    Int_t sel     = cand->getSelectedMeta();

    if(metaind != -1 ){
        map<Int_t,vector<HParticleCand*> >* mp =0;

	if     (sel == kTofClst)           mp = &mTofClsttoCand;
        else if(sel == kTofHit1||sel==kTofHit2) mp = &mTofHittoCand;
        else if(sel == kRpcClst)           mp = &mRpcClsttoCand;
        else if(sel == kShowerHit)         mp = &mShowertoCand;
        else if(sel == kEmcClst)           mp = &mEmctoCand;
	else  {
	    Error("getSameMeta()","unknown Meta case!"); return 0;
	}
        map<Int_t,vector<HParticleCand*> >::iterator it = mp->find(metaind);

	if(it != mp->end()){
	    vector<HParticleCand*>& v = it->second;
	    for(UInt_t i = 0 ; i < v.size(); i++){

		if(v[i] != cand) {  // skip same cand
		    if(flag == 0 ) candidates.push_back(v[i]);
		    else {
			Bool_t fl = kFALSE;
			if(!isReference) fl=HParticleTool::evalPairsFlags(fl,cand,v[i]);
			else             fl=HParticleTool::evalPairsFlags(fl,v[i],cand);
			if(fl){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    } 
    return candidates.size();
}

void  HParticlePairMaker::plotPairCaseStat()
{
    // draws the fraction of pair cases for
    // the default pairCases1...12.

    TCanvas* c = new TCanvas("pairCase","pairCase",1500,800);
    c->SetGridx();
    c->SetGridy();
    c->SetBottomMargin(0.15);

    UInt_t bins = fCaseVec.size();

    TH1F* h = new TH1F("h","",bins,0,bins);
    h->SetLineColor(kRed);
    h->SetLineWidth(3);
    h->SetYTitle("Fraction of pairs [%]");
    h->GetXaxis()->SetNdivisions(100+bins);
    h->GetXaxis()->SetLabelSize(0);

    h->GetYaxis()->SetRangeUser(0.,50.);
    Float_t sum = 0;
    for(UInt_t i=0;i<bins;i++)  sum+=fCaseCt[i];

    cout<<"Pairs Cases for Leptons :"<<endl;
    for(UInt_t i=0;i<bins;i++){
	Float_t frac = 0;
	if(sum != 0) frac = (fCaseCt[i]/sum)*100.;
        if(sum !=0 ) h->Fill(i,frac);
	cout<<Form("case%2i : ",i+1)<<setw(15)<< frac << " cts "<<setw(15)<<fCaseCt[i]<<endl;

    }
    h->Draw();

    Double_t offsetX  = 0.25;
    Double_t offsetY  =   -8;
    Double_t scaleX   = 1.;
    Double_t scaleY   = 1.;

    HParticlePairDraw pairdraw;

    for(UInt_t i=0; i < bins; i++){
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,scaleY,fCaseVec[i],Form("Case%i",i+1),"");
    }
    cout<<"sum "<<sum<< " check " <<richCandCt<<endl;
}

