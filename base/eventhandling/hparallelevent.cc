#include "hparallelevent.h"
#include "hades.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hcategorymanager.h"

//-----------------------
#include "haddef.h"
#include "hgeantdef.h"


//-----------------------

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
ClassImp(HParallelEvent)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParallelEvent
//
// This class holds an full event structure connected
// to an root input file and the corresponding tree.
// It provides the function to merge 2 geant events.
// The events have to have the same structure.
// The class is used by HGeantMergeSource.
//
////////////////////////////////////////////////////////////////////////////////




HParallelEvent::HParallelEvent(const Text_t *name,const Text_t *title)
: TNamed(name,title)
{
    fCurrentEvent      = 0;
    fTree              = 0;
    fFile              = 0;
}

HParallelEvent::~HParallelEvent()
{
    if(fCurrentEvent) delete fCurrentEvent;
}

void HParallelEvent::activateTree(TTree *tree)
{
    // Sets the right branch address and branch status (=1) for all the branches
    // in tree which correspond to the branches which would eventually be
    // generated for the output Root tree of events if the function makeTree() is
    // called.
    //
    // This mechanism allows to only read those branches in "tree" which are
    // neccesary to fill an event as defined in setEventLayout()
    Int_t splitLevel = 2;
    TBranch *brEvent = 0;

    Char_t sl = *(strchr(tree->GetTitle(),'.') + 1);
    switch (sl) {
    case '0' : splitLevel = 0;
    break;
    case '1' : splitLevel = 1;
    break;
    case '2' : splitLevel = 2;
    break;
    default : splitLevel  = 2;
    }
    if (fCurrentEvent)
    {
	tree->SetBranchStatus("*",1);   // needed since Root 303-04 because of
	// change in TBranchElement::SetAddress()
	if (splitLevel == 0)
	{
	    tree->SetBranchAddress("Event",&fCurrentEvent);
	    tree->SetBranchStatus("Event",1);
	} else {
	    brEvent = tree->GetBranch("Event.");
	    if (brEvent) { //Post ROOT3 splitting
		brEvent->SetAddress(&fCurrentEvent);
		tree->SetBranchStatus("Event.",1);
		tree->SetBranchStatus("Event.*",1);
	    } else { //Pre ROOT3 splitting
		brEvent = tree->GetBranch("Event");
		if (brEvent) {
		    tree->SetBranchAddress("Event",&fCurrentEvent);
		    tree->SetBranchStatus("Event",1);
		    tree->SetBranchStatus("Event.*",1);
		} else {
		    Warning("activateBranch","Event branch not found in tree");
		}
	    }
	    fCurrentEvent->activateBranch(tree,splitLevel);
	}
    }
}

Bool_t   HParallelEvent::setInputFile(TString name,Bool_t print)
{
    // set the input file, reads the eventstructure and
    // activates the tree

    if(gSystem->AccessPathName(name.Data()) == 0) {
	fFile = new TFile(name.Data(),"READ");
	if(fFile){
	    fTree = (HTree*)fFile->Get("T");
	    if(!fTree){
		Error("setInputFile()","File = %s contains no Tree \"T\"!",name.Data());
                return kFALSE;
	    }
	    fCurrentEvent = (HRecEvent*) fFile->Get("Event");
	    if(!fCurrentEvent){
		Error("setInputFile()","File = %s contains no EventStructure !",name.Data());
                return kFALSE;
	    }
	} else return kFALSE;

	SetName(name.Data());


	activateTree(fTree);
        if(print)Info("setInputFile()","Open %s",name.Data());
	return kTRUE;
    } else {
        Error("setInputFile()","File = %s not found!",name.Data());
        return kFALSE;
    }
}

void     HParallelEvent::closeInputFile()
{
    if(fFile) {
        Info("closeInputFile()","Close %s",GetName());
	fFile->Close();
	fFile = 0;
	fTree = 0;

    }
}


void     HParallelEvent::mergeGeantEvent(HRecEvent*  targetEvent)
{
    // merge in internal event into target event. the content is appended
    // track numbers and indices are takes care of.

    HCategory* kineCat             = targetEvent->getCategory(catGeantKine);
    HMatrixCategory* richCatCerenk = (HMatrixCategory*)targetEvent->getCategory(catRichGeantRaw);
    HMatrixCategory* richCatDirect = (HMatrixCategory*)targetEvent->getCategory(catRichGeantRaw+1);
    HMatrixCategory* richCatMirror = (HMatrixCategory*)targetEvent->getCategory(catRichGeantRaw+2);
    HMatrixCategory* mdcCat        = (HMatrixCategory*)targetEvent->getCategory(catMdcGeantRaw);
    HMatrixCategory* tofCat        = (HMatrixCategory*)targetEvent->getCategory(catTofGeantRaw);
    HMatrixCategory* rpcCat        = (HMatrixCategory*)targetEvent->getCategory(catRpcGeantRaw);
    HMatrixCategory* shrCat        = (HMatrixCategory*)targetEvent->getCategory(catShowerGeantRaw);
    HCategory* startCat            = targetEvent->getCategory(catStartGeantRaw);
    HCategory* wallCat             = targetEvent->getCategory(catWallGeantRaw);
    HMatrixCategory* emcCat        = (HMatrixCategory*)targetEvent->getCategory(catEmcGeantRaw);



    HCategory* kineCatLoc             = fCurrentEvent->getCategory(catGeantKine);
    HMatrixCategory* richCatCerenkLoc = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw);
    HMatrixCategory* richCatDirectLoc = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw+1);
    HMatrixCategory* richCatMirrorLoc = (HMatrixCategory*)fCurrentEvent->getCategory(catRichGeantRaw+2);
    HMatrixCategory* mdcCatLoc        = (HMatrixCategory*)fCurrentEvent->getCategory(catMdcGeantRaw);
    HMatrixCategory* tofCatLoc        = (HMatrixCategory*)fCurrentEvent->getCategory(catTofGeantRaw);
    HMatrixCategory* rpcCatLoc        = (HMatrixCategory*)fCurrentEvent->getCategory(catRpcGeantRaw);
    HMatrixCategory* shrCatLoc        = (HMatrixCategory*)fCurrentEvent->getCategory(catShowerGeantRaw);
    HCategory* startCatLoc            = fCurrentEvent->getCategory(catStartGeantRaw);
    HCategory* wallCatLoc             = fCurrentEvent->getCategory(catWallGeantRaw);
    HMatrixCategory* emcCatLoc        = (HMatrixCategory*)fCurrentEvent->getCategory(catEmcGeantRaw);




    if(!kineCat||!kineCatLoc)   { Error("mergeGeantEvent()","catKine or catKineLoc missing!"); cout<<kineCat<<" "<<kineCatLoc<<endl;return; }
    if( (!richCatCerenk&&richCatCerenkLoc)|| (richCatCerenk&&!richCatCerenkLoc))  { Error("mergeGeantEvent()","catRich Cerenkov has different setting!"); return; }
    if( (!richCatDirect&&richCatDirectLoc)|| (richCatDirect&&!richCatDirectLoc))  { Error("mergeGeantEvent()","catRich Direct has different setting!");   return; }
    if( (!richCatMirror&&richCatMirrorLoc)|| (richCatMirror&&!richCatMirrorLoc))  { Error("mergeGeantEvent()","catRich Mirror has different setting!");   return; }
    if( (!mdcCat       &&mdcCatLoc       )|| (mdcCat       &&!mdcCatLoc       ))  { Error("mergeGeantEvent()","catMdc has different setting!");           return; }
    if( (!tofCat       &&tofCatLoc       )|| (tofCat       &&!tofCatLoc       ))  { Error("mergeGeantEvent()","catTof has different setting!");           return; }
    if( (!rpcCat       &&rpcCatLoc       )|| (rpcCat       &&!rpcCatLoc       ))  { Error("mergeGeantEvent()","catRpc has different setting!");           return; }
    if( (!shrCat       &&shrCatLoc       )|| (shrCat       &&!shrCatLoc       ))  { Error("mergeGeantEvent()","catShower has different setting!");        return; }
    if( (!startCat     &&startCatLoc     )|| (startCat     &&!startCatLoc     ))  { Error("mergeGeantEvent()","catStart has different setting!");         return; }
    if( (!wallCat      &&wallCatLoc      )|| (wallCat      &&!wallCatLoc      ))  { Error("mergeGeantEvent()","catWall has different setting!");          return; }
    if( (!emcCat       &&emcCatLoc       )|| (emcCat       &&!emcCatLoc       ))  { Error("mergeGeantEvent()","catEmc has different setting!");           return; }

    HLocation loc;//dummy
    Int_t index ;

    HGeantKine* kine            = 0;
    HGeantRichDirect* grichdir  = 0;
    HGeantRichPhoton* grichphot = 0;
    HGeantRichMirror* grichmir  = 0;
    HGeantMdc*        gmdc      = 0;
    HGeantTof*        gtof      = 0;
    HGeantShower*     gshr      = 0;
    HGeantRpc*        grpc      = 0;
    HGeantWall*       gwall     = 0;
    HGeantEmc*        gemc      = 0;
    HGeantStart*      gstart    = 0;

    Int_t lastRichCer = (richCatCerenk)? richCatCerenk->getEntries()-1 : -1;
    Int_t lastRichDir = (richCatDirect)? richCatDirect->getEntries()-1 : -1;
    Int_t lastRichMir = (richCatMirror)? richCatMirror->getEntries()-1 : -1;
    Int_t lastMdc     = (mdcCat)       ? mdcCat       ->getEntries()-1 : -1;
    Int_t lastTof     = (tofCat)       ? tofCat       ->getEntries()-1 : -1;
    Int_t lastShr     = (shrCat)       ? shrCat       ->getEntries()-1 : -1;
    Int_t lastRpc     = (rpcCat)       ? rpcCat       ->getEntries()-1 : -1;
    Int_t lastWall    = (wallCat)      ? wallCat      ->getEntries()-1 : -1;
    Int_t lastEmc     = (emcCat)       ? emcCat       ->getEntries()-1 : -1;
    Int_t lastStart   = (startCat)     ? startCat     ->getEntries()-1 : -1;


    Int_t kineLastTr  = 0;

    if(kineCat && kineCat->getEntries()>0){
	kine       = (HGeantKine* )kineCat->getObject(kineCat->getEntries()-1);
	kineLastTr  = kine->getTrack();
    }

    if(kineCatLoc && kineCatLoc->getEntries()>0){
        loc.set(0);

	for(Int_t i=0;i<kineCatLoc->getEntries();i++){
	    HGeantKine* kineLoc = (HGeantKine* )kineCatLoc->getObject(i);
            Int_t track          = kineLoc->getTrack();
            Int_t parentTrack    = kineLoc->getParentTrack();
	    Int_t firstRichLoc   = kineLoc->getFirstRichHit();
	    Int_t firstMdcLoc    = kineLoc->getFirstMdcHit();
	    Int_t firstTofLoc    = kineLoc->getFirstTofHit();
	    Int_t firstRpcLoc    = kineLoc->getFirstRpcHit();
	    Int_t firstShrLoc    = kineLoc->getFirstShowerHit();
	    Int_t firstWallLoc   = kineLoc->getFirstWallHit();
	    Int_t firstEmcLoc    = kineLoc->getFirstEmcHit();
	    Int_t firstStartLoc  = kineLoc->getFirstStartHit();


	    kine = (HGeantKine*) kineCat->getNewSlot(loc,&index); // new slot in target cat
            kine = new (kine) HGeantKine(*kineLoc);               // copy local object to target cat




            //--------------------------------------------------
            // shifting tracknumbers and indices
	    kine->setTrack         (track  + kineLastTr);
	    kine->setParentTrack   ((parentTrack  > 0)? parentTrack  + kineLastTr     :  0);
	    kine->setFirstRichHit  ((firstRichLoc >-1)? firstRichLoc + lastRichCer + 1: -1);
	    kine->setFirstMdcHit   ((firstMdcLoc  >-1)? firstMdcLoc  + lastMdc     + 1: -1);
	    kine->setFirstTofHit   ((firstTofLoc  >-1)? firstTofLoc  + lastTof     + 1: -1);
	    kine->setFirstRpcHit   ((firstRpcLoc  >-1)? firstRpcLoc  + lastRpc     + 1: -1);
	    kine->setFirstShowerHit((firstShrLoc  >-1)? firstShrLoc  + lastShr     + 1: -1);
	    kine->setFirstWallHit  ((firstWallLoc >-1)? firstWallLoc + lastWall    + 1: -1);
	    kine->setFirstEmcHit   ((firstEmcLoc  >-1)? firstEmcLoc  + lastEmc     + 1: -1);
	    kine->setFirstStartHit ((firstStartLoc>-1)? firstStartLoc+ lastStart   + 1: -1);

	    kine->setRichCategory(0);
	    kine->setMdcCategory(0) ;
            kine->setTofCategory(0) ;
            kine->setRpcCategory(0) ;
            kine->setShowerCategory(0) ;
            kine->setWallCategory(0) ;
            kine->setEmcCategory(0) ;
            kine->setStartCategory(0) ;
            kine->resetRichIter();
            kine->resetMdcIter();
            kine->resetTofIter();
            kine->resetRpcIter();
            kine->resetShowerIter();
            kine->resetWallIter();
            kine->resetEmcIter();
            kine->resetStartIter();
	    //--------------------------------------------------

	}
    }

    if(richCatCerenkLoc && richCatCerenkLoc->getEntries()>0){
	loc.set(2,0,0); // 6,4000
	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ ind[s]=0; }
	for(Int_t i=0;i<richCatCerenk->getEntries();i++){
	    HGeantRichPhoton* grichphotLoc = (HGeantRichPhoton* ) richCatCerenk->getObject(i);
            ind[grichphotLoc->getSector()]++;
	}

	for(Int_t i=0;i<richCatCerenkLoc->getEntries();i++){
	    HGeantRichPhoton* grichphotLoc = (HGeantRichPhoton* ) richCatCerenkLoc->getObject(i);
	    Int_t track  = grichphotLoc->getTrack();
	    Int_t nexhit = grichphotLoc->getNextHitIndex();
	    loc[0]       = grichphotLoc->getSector();
            loc[1]       = ind[loc[0]];
	    grichphot    = (HGeantRichPhoton*) richCatCerenk->getSlot(loc,&index); // new slot in target cat
	    grichphot    = new (grichphot) HGeantRichPhoton(*grichphotLoc);           // copy local object to target cat
            ind[loc[0]]++;
	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    grichphot->setTrack         (track        +kineLastTr);
	    grichphot->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastRichCer + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(richCatDirectLoc && richCatDirectLoc->getEntries()>0){
	loc.set(2,0,0); // 6,4000
	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ ind[s]=0; }
	for(Int_t i=0;i<richCatDirect->getEntries();i++){
	    HGeantRichDirect* grichdirLoc = (HGeantRichDirect* ) richCatDirect->getObject(i);
	    ind[grichdirLoc->getSector()]++;
	}
	for(Int_t i=0;i<richCatDirectLoc->getEntries();i++){
	    HGeantRichDirect* grichdirLoc = (HGeantRichDirect* ) richCatDirectLoc->getObject(i);
	    Int_t track  = grichdirLoc->getTrack();
	    Int_t nexhit = grichdirLoc->getNextHitIndex();
	    loc[0]       = grichdirLoc->getSector();
            loc[1]       = ind[loc[0]];
	    grichdir     = (HGeantRichDirect*) richCatDirect->getSlot(loc,&index); // new slot in target cat
	    grichdir     = new (grichdir) HGeantRichDirect(*grichdirLoc);             // copy local object to target cat
            ind[loc[0]]++;

	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    grichdir->setTrack         (track        +kineLastTr);
	    grichdir->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastRichDir + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(richCatMirrorLoc && richCatMirrorLoc->getEntries()>0){
       loc.set(2,0,0); // 6,4000
       Int_t ind[6] ;
       for(Int_t s=0;s<6;s++){ ind[s]=0; }
       for(Int_t i=0;i<richCatMirror->getEntries();i++){
	   HGeantRichMirror* grichmirLoc = (HGeantRichMirror* ) richCatMirror->getObject(i);
	   ind[grichmirLoc->getSector()]++;
       }
       for(Int_t i=0;i<richCatMirrorLoc->getEntries();i++){
	   HGeantRichMirror* grichmirLoc = (HGeantRichMirror* ) richCatMirrorLoc->getObject(i);
	   Int_t track  = grichmirLoc->getTrack();
	   Int_t nexhit = grichmirLoc->getNextHitIndex();
	   loc[0]       = grichmirLoc->getSector();
	   loc[1]       = ind[loc[0]];
	   grichmir     = (HGeantRichMirror*) richCatMirror->getSlot(loc,&index); // new slot in target cat
	   grichmir     = new (grichmir) HGeantRichMirror(*grichmirLoc);             // copy local object to target cat
	   ind[loc[0]]++;

	   //--------------------------------------------------
	   // shifting tracknumbers and indices
	   grichmir->setTrack         (track        +kineLastTr);
	   grichmir->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastRichMir + 1   : -1);
	   //--------------------------------------------------
       }
    }

    if(mdcCatLoc && mdcCatLoc->getEntries()>0){
	loc.set(4,0,0,0,0);
	Int_t ind[6][4][7] ;
	for(Int_t s=0;s<6;s++){
	    for(Int_t m=0;m<4;m++){
		for(Int_t l=0;l<7;l++){
		    ind[s][m][l]=0;
		}
	    }
	}
	for(Int_t i=0;i<mdcCat->getEntries();i++){
	    HGeantMdc* gmdcLoc = (HGeantMdc* ) mdcCat->getObject(i);
            loc[0] = gmdcLoc->getSector();
            loc[1] = gmdcLoc->getModule();
            loc[2] = gmdcLoc->getLayer();
	    ind[loc[0]][loc[1]][loc[2]]++;
	}

	for(Int_t i=0;i<mdcCatLoc->getEntries();i++){
	    HGeantMdc* gmdcLoc = (HGeantMdc* ) mdcCatLoc->getObject(i);
	    Int_t track   = gmdcLoc->getTrack();
	    Int_t nexhit  = gmdcLoc->getNextHitIndex();
            loc[0] = gmdcLoc->getSector();
            loc[1] = gmdcLoc->getModule();
            loc[2] = gmdcLoc->getLayer();
            loc[3] = ind[loc[0]][loc[1]][loc[2]];
	    gmdc          = (HGeantMdc*) mdcCat->getSlot(loc,&index); // new slot in target cat
	    gmdc          = new (gmdc) HGeantMdc(*gmdcLoc);           // copy local object to target cat
            ind[loc[0]][loc[1]][loc[2]]++;
	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    gmdc->setTrack         (track        +kineLastTr);
	    gmdc->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastMdc + 1   : -1);
	    //--------------------------------------------------
	}
    }



    if(tofCatLoc && tofCatLoc->getEntries()>0){
	loc.set(2,0,0); // 6,200
	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ind[s]=0;}
	for(Int_t i=0;i<tofCat->getEntries();i++){
	    HGeantTof* gtofLoc = (HGeantTof* ) tofCat->getObject(i);
            ind[(Int_t)gtofLoc->getSector()]++;
	}

	for(Int_t i=0;i<tofCatLoc->getEntries();i++){
	    HGeantTof* gtofLoc = (HGeantTof* ) tofCatLoc->getObject(i);
	    Int_t track  = gtofLoc->getTrack();
	    Int_t nexhit = gtofLoc->getNextHitIndex();
            loc[0]       = gtofLoc->getSector();
            loc[1]       = ind[loc[0]];
	    gtof         = (HGeantTof*) tofCat->getSlot(loc,&index); // new slot in target cat
	    gtof         = new (gtof) HGeantTof(*gtofLoc);           // copy local object to target cat
            ind[loc[0]]++;
	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    gtof->setTrack         (track        +kineLastTr);
	    gtof->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastTof + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(shrCatLoc && shrCatLoc->getEntries()>0){
	loc.set(2,0,0); // 6,600
	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ ind[s]=0; }
	for(Int_t i=0;i<shrCat->getEntries();i++){
	    HGeantShower* gshrLoc = (HGeantShower* ) shrCat->getObject(i);
	    ind[(Int_t)gshrLoc->getSector()]++;
	}

	for(Int_t i=0;i<shrCatLoc->getEntries();i++){
	    HGeantShower* gshrLoc = (HGeantShower* ) shrCatLoc->getObject(i);
	    Int_t track  = gshrLoc->getTrack();
	    Int_t nexhit = gshrLoc->getNextHitIndex();
            loc[0]       = gshrLoc->getSector();
            loc[1]       = ind[loc[0]];
	    gshr         = (HGeantShower*) shrCat->getSlot(loc,&index); // new slot in target cat
	    gshr         = new (gshr) HGeantShower(*gshrLoc);           // copy local object to target cat
            ind[loc[0]]++;
	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    gshr->setTrack         (track        +kineLastTr);
	    gshr->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastShr + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(rpcCatLoc && rpcCatLoc->getEntries()>0){
	loc.set(2,0,0); // 6,2500

	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ind[s]=0;}
	for(Int_t i=0;i<rpcCat->getEntries();i++){
	    HGeantRpc* grpcLoc = (HGeantRpc* ) rpcCat->getObject(i);
            ind[grpcLoc->getSector()]++;
	}
	for(Int_t i=0;i<rpcCatLoc->getEntries();i++){
	    HGeantRpc* grpcLoc = (HGeantRpc* ) rpcCatLoc->getObject(i);
	    Int_t track  = grpcLoc->getTrack();
	    Int_t nexhit = grpcLoc->getNextHitIndex();
            loc[0]       = grpcLoc->getSector();
            loc[1]       = ind[loc[0]];
	    grpc         = (HGeantRpc*) rpcCat->getSlot(loc,&index); // new slot in target cat
	    grpc         = new (grpc) HGeantRpc(*grpcLoc);           // copy local object to target cat
	    ind[loc[0]]++;

	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    grpc->setTrack         (track        +kineLastTr);
	    grpc->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastRpc + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(wallCatLoc && wallCatLoc->getEntries()>0){
        loc.set(0);
	for(Int_t i=0;i<wallCatLoc->getEntries();i++){
	    HGeantWall* gwallLoc = (HGeantWall* ) wallCatLoc->getObject(i);
	    Int_t track  = gwallLoc->getTrack();
	    Int_t nexhit = gwallLoc->getNextHitIndex();
	    gwall        = (HGeantWall*) wallCat->getNewSlot(loc,&index); // new slot in target cat
	    gwall        = new (gwall) HGeantWall(*gwallLoc);             // copy local object to target cat

	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    gwall->setTrack         (track        +kineLastTr);
	    gwall->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastWall + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(startCatLoc && startCatLoc->getEntries()>0){
	loc.set(0);
	for(Int_t i=0;i<startCatLoc->getEntries();i++){
	    HGeantStart* gstartLoc = (HGeantStart* ) startCatLoc->getObject(i);
	    Int_t track  = gstartLoc->getTrack();
	    Int_t nexhit = gstartLoc->getNextHitIndex();
	    gstart       = (HGeantStart*) startCat->getNewSlot(loc,&index); // new slot in target cat
	    gstart       = new (gstart) HGeantStart(*gstartLoc);            // copy local object to target cat

	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    gstart->setTrack         (track        +kineLastTr);
	    gstart->setNextHitIndex  ((nexhit>-1)  ? nexhit  + lastStart + 1   : -1);
	    //--------------------------------------------------
	}
    }
    if(emcCatLoc && emcCatLoc->getEntries()>0){
	loc.set(2,0,0); // 6,2500
	Int_t ind[6] ;
	for(Int_t s=0;s<6;s++){ind[s]=0;}
	for(Int_t i=0;i<emcCat->getEntries();i++){
	    HGeantEmc* gemcLoc = (HGeantEmc* ) emcCat->getObject(i);
	    ind[gemcLoc->getSector()]++;
	}

	for(Int_t i=0;i<emcCatLoc->getEntries();i++){
	    HGeantEmc* gemcLoc = (HGeantEmc* ) emcCatLoc->getObject(i);
	    Int_t track  = gemcLoc->getTrack();
	    Int_t nexhit = gemcLoc->getNextHitIndex();
            loc[0]       = gemcLoc->getSector();
            loc[1]       = ind[loc[0]];
	    gemc         = (HGeantEmc*) emcCat->getNewSlot(loc,&index); // new slot in target cat
	    gemc         = new (gemc) HGeantEmc(*gemcLoc);              // copy local object to target cat
	    ind[loc[0]]++;

	    //--------------------------------------------------
	    // shifting tracknumbers and indices
	    if(track>0) gemc->setTrack (track        +kineLastTr);  // special track Num -777 in Ecal shold not be shifted
	    gemc->setNextHitIndex      ((nexhit>-1)  ? nexhit  + lastEmc + 1   : -1);
	    //--------------------------------------------------
	}
    }

}



