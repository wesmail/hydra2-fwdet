#include "hparticlebooker.h"
#include "hades.h"
#include "hcategory.h"
#include "hparticledef.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "showerdef.h"
#include "hmdcdef.h"
#include "richdef.h"

#include "hparticlecand.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hrpccluster.h"
#include "hshowerhit.h"
#include "hmdcseg.h"
#include "hrichhit.h"
#include "hparticletool.h"
#include "hcategorymanager.h"

#include <iostream>
#include <iomanip>

HParticleBooker* gParticleBooker;

ClassImp(HParticleBooker)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleBooker
//
// Creates maps form each Detctor hit index to the list
// of HParticleCands in the current event who are using
// a given hit
//
//-----------------------------------------------------------------------------
// USAGE:
// #include  "hparticlebooker.h"
// #include  "hcategorymanager.h"
// #include  "hcategory.h"
// #include  "hades.h"
// #include  "hparticlecand.h"
// #include  "hparticledef.h"
// #include  <vector>
// using namespace std;
// ....
//
//
// {
// // setup HLoop etc ....
//
// ....
// //---------------------------------------------
// // HPARTICLEBOOKER SETUP :
// HParticleBooker booker;
// //---------------------------------------------
//
// vector<HParticleCand*> vCandidates;
//
// HCategory* catCand = HCategoryManager::getCategory(catParticleCand);
// if(!catCand) { exit(1); }
//
// for (Int_t i=0; i < entries; i++) {
//    Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
//    if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
//
//
//    //###########################################################################
//    // HPARTICLEPAIRMAKER ACTION:
//    booker.nextEvent();  // fills all tables
//
//	  if(catCand){
//	      HParticleCand* cand=0;
//	      for(Int_t j=0; j<catCand->getEntries();j++){
//	          cand = HCategoryManager::getObject(cand,catCand,j);
//                  // do something  ....
//             Int_t n =booker.getSameMeta(cand,vCandidates); // fill a list of all cand sharing the same metahit
//
//
//       } // end loop cand
//	  } //catCand
// }
////////////////////////////////////////////////////////////////////////////////

HParticleBooker::HParticleBooker()
{
    gParticleBooker = this;
}

HParticleBooker::~HParticleBooker()
{

    mTofHittoCand  .clear();
    mTofClsttoCand .clear();
    mRpcClsttoCand .clear();
    mShowertoCand  .clear();
    mInnerMdctoCand.clear();
    mOuterMdctoCand.clear();
    mRichtoCand    .clear();

    vTofHitInd .clear();
    vTofClstInd.clear();
    vRpcClstInd.clear();
    vShowerInd  .clear();
    vInnerMdcInd.clear();
    vOuterMdcInd.clear();
    vRichInd    .clear();

    gParticleBooker = NULL;
}



void HParticleBooker::bookHits(HParticleCand* cand1)
{
    // add candidate hits to maps

    if(cand1->getRichInd() !=-1 ) {
	if(mRichtoCand.find(cand1->getRichInd()) == mRichtoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mRichtoCand[cand1->getRichInd()] = v;
            vRichInd.push_back(cand1->getRichInd());
	} else {
	    mRichtoCand[cand1->getRichInd()].push_back(cand1);
	}
    }

    if(cand1->getInnerSegInd() !=-1 ) {
	if(mInnerMdctoCand.find(cand1->getInnerSegInd()) == mInnerMdctoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mInnerMdctoCand[cand1->getInnerSegInd()] = v;
            vInnerMdcInd.push_back(cand1->getInnerSegInd());
	    HMdcSeg* hit = HParticleTool::getMdcSeg(cand1->getInnerSegInd());
	    if(hit) vInnerMdc.push_back(hit);
	} else {
	    mInnerMdctoCand[cand1->getInnerSegInd()].push_back(cand1);
	}
    }

    if(cand1->getOuterSegInd() !=-1 ) {
	if(mOuterMdctoCand.find(cand1->getOuterSegInd()) == mOuterMdctoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mOuterMdctoCand[cand1->getOuterSegInd()] = v;
            vOuterMdcInd.push_back(cand1->getOuterSegInd());
	    HMdcSeg* hit = HParticleTool::getMdcSeg(cand1->getOuterSegInd());
	    if(hit) vOuterMdc.push_back(hit);
	} else {
	    mOuterMdctoCand[cand1->getOuterSegInd()].push_back(cand1);
	}
    }

    if(cand1->getSystemUsed() != -1){
	Int_t meta = cand1->getMetaHitInd();
        Int_t showerInd = cand1->getShowerInd();
	if(cand1->isTofHitUsed()){
	    if(mTofHittoCand.find(meta) == mTofHittoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mTofHittoCand[meta] = v;
		vTofHitInd.push_back(cand1->getTofHitInd());

		HTofHit* hit = HParticleTool::getTofHit(cand1->getTofHitInd());
		if(hit) vTofHit.push_back(hit);
	    } else {
		mTofHittoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isTofClstUsed()){
	    if(mTofClsttoCand.find(meta) == mTofClsttoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mTofClsttoCand[meta] = v;
		vTofClstInd.push_back(cand1->getTofClstInd());
		HTofCluster* hit = HParticleTool::getTofCluster(cand1->getTofClstInd());
		if(hit) vTofClst.push_back(hit);
	    } else {
		mTofClsttoCand[meta].push_back(cand1);
	    }
	} else if (cand1->isRpcClstUsed()){
	    if(mRpcClsttoCand.find(meta) == mRpcClsttoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mRpcClsttoCand[meta] = v;
		vRpcClstInd.push_back(cand1->getRpcInd());
		HRpcCluster* hit = HParticleTool::getRpcCluster(cand1->getRpcInd());
		if(hit) vRpcClst.push_back(hit);
	    } else {
		mRpcClsttoCand[meta].push_back(cand1);
	    }
	}
	if (showerInd!=-1){
	    if(mShowertoCand.find(showerInd) == mShowertoCand.end()){
		vector<HParticleCand*> v;
		v.push_back(cand1);
		mShowertoCand[showerInd] = v;
		vShowerInd.push_back(showerInd);
		HShowerHit* hit = HParticleTool::getShowerHit(showerInd);
		if(hit) vShower.push_back(hit);
	    } else {
		mShowertoCand[showerInd].push_back(cand1);
	    }
	}
    }


    HCategory* tofhitCat = HCategoryManager::getCategory(catTofHit,2);
    if(tofhitCat)
    {
	Int_t size = tofhitCat->getEntries();
	HTofHit* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,tofhitCat,j);
	    if(hit)
	    {
                vTofHitAll.push_back(hit);
	    }
	}
    }

    HCategory* tofclstCat = HCategoryManager::getCategory(catTofCluster,2);
    if(tofclstCat)
    {
	Int_t size = tofclstCat->getEntries();
	HTofCluster* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,tofclstCat,j);
	    if(hit)
	    {
		vTofClstAll.push_back(hit);
	    }
	}
    }

    HCategory* rpcclstCat = HCategoryManager::getCategory(catRpcCluster,2);
    if(rpcclstCat)
    {
	Int_t size = rpcclstCat->getEntries();
	HRpcCluster* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,rpcclstCat,j);
	    if(hit)
	    {
		vRpcClstAll.push_back(hit);
	    }
	}
    }

    HCategory* showerhitCat = HCategoryManager::getCategory(catShowerHit,2);
    if(showerhitCat)
    {
	Int_t size = showerhitCat->getEntries();
	HShowerHit* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,showerhitCat,j);
	    if(hit)
	    {
		vShowerAll.push_back(hit);
	    }
	}
    }

    HCategory* mdcsegCat = HCategoryManager::getCategory(catMdcSeg,2);
    if(mdcsegCat)
    {
	Int_t size = mdcsegCat->getEntries();
	HMdcSeg* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,mdcsegCat,j);
	    if(hit)
	    {

		if(hit->getIOSeg() == 0) vInnerMdcAll.push_back(hit);
		if(hit->getIOSeg() == 1) vOuterMdcAll.push_back(hit);
	    }
	}
    }

    HCategory* richhitCat = HCategoryManager::getCategory(catRichHit,2);
    if(richhitCat)
    {
	Int_t size = richhitCat->getEntries();
	HRichHit* hit=0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,richhitCat,j);
	    if(hit)
	    {
		vRichAll.push_back(hit);
	    }
	}
    }
}

void HParticleBooker::nextEvent()
{
    mTofHittoCand  .clear();
    mTofClsttoCand .clear();
    mRpcClsttoCand .clear();
    mShowertoCand  .clear();
    mInnerMdctoCand.clear();
    mOuterMdctoCand.clear();
    mRichtoCand    .clear();

    vTofHitInd  .clear();
    vTofClstInd .clear();
    vRpcClstInd .clear();
    vShowerInd  .clear();
    vInnerMdcInd.clear();
    vOuterMdcInd.clear();
    vRichInd    .clear();

    vTofHit  .clear();
    vTofClst .clear();
    vRpcClst .clear();
    vShower  .clear();
    vInnerMdc.clear();
    vOuterMdc.clear();
    vRich    .clear();

    vTofHitAll  .clear();
    vTofClstAll .clear();
    vRpcClstAll .clear();
    vShowerAll  .clear();
    vInnerMdcAll.clear();
    vOuterMdcAll.clear();
    vRichAll    .clear();

    HCategory* candCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catParticleCand);
    if(candCat){
	UInt_t n = candCat->getEntries();
	HParticleCand* cand1 = 0 ;
	//-------------------------------------------------------
	// fill lists for reference and others
	for(UInt_t i=0; i < n; i++){
	    cand1 = HCategoryManager::getObject(cand1,candCat,i);

	    //-------------------------------------------------------
	    //  book the hits -> candiate lists
	    bookHits(cand1);
	    //-------------------------------------------------------
	}
    }
}


Int_t HParticleBooker::getCandidatesForTofHit(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mTofHittoCand.find(index);
    if(it!=mTofHittoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
        return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForTofCluster(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mTofClsttoCand.find(index);
    if(it!=mTofClsttoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForRpcCluster(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mRpcClsttoCand.find(index);
    if(it!=mRpcClsttoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForShower(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mShowertoCand.find(index);
    if(it!=mShowertoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForInnerMdc(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mInnerMdctoCand.find(index);
    if(it!=mInnerMdctoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForOuterMdc(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mOuterMdctoCand.find(index);
    if(it!=mOuterMdctoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForRich(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mRichtoCand.find(index);
    if(it!=mRichtoCand.end()){
	hits.assign(it->second.begin(),it->second.end());
	return hits.size();
    } else {
	return 0;
    }
}



Int_t HParticleBooker::getSameRich(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
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
			Bool_t res = kFALSE;
			if(!isReference) res=HParticleTool::evalPairsFlags(flag,cand,v[i]);
			else             res=HParticleTool::evalPairsFlags(flag,v[i],cand);
			if(res){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticleBooker::getSameInnerMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
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
			Bool_t res = kFALSE;
			if(!isReference) res=HParticleTool::evalPairsFlags(flag,cand,v[i]);
			else             res=HParticleTool::evalPairsFlags(flag,v[i],cand);
			if(res){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticleBooker::getSameOuterMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
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
			Bool_t res = kFALSE;
			if(!isReference) res=HParticleTool::evalPairsFlags(flag,cand,v[i]);
			else             res=HParticleTool::evalPairsFlags(flag,v[i],cand);
			if(res){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticleBooker::getSameMeta(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,Bool_t isReference)
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
			Bool_t res = kFALSE;
			if(!isReference) res=HParticleTool::evalPairsFlags(flag,cand,v[i]);
			else             res=HParticleTool::evalPairsFlags(flag,v[i],cand);
			if(res){
			    candidates.push_back(v[i]);
			}
		    }
		}
	    } //end loop
	} else return -1;
    }
    return candidates.size();
}

Int_t HParticleBooker::getSameAny(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag,UInt_t detswitch,Bool_t isReference)
{
    // fills list of candidates which share any detector hit. Detectors can be selected by
    // detswitch setting the bit using eClosePairSelect (default is 0. in this case all
    // detectors are filled).
    // The number of candidatefound for the hit is returned or -1 if the hit is not found at all..
    // The input candidate will be not included in candidates. Candidates can be filtered
    // by flag ( 0 == no filter, see flags eClosePairSelect+ ePairCase (hparticledef.h)).
    // The input candidate will be treated as refererence if isReference == kTRUE.

    candidates.clear();
    vector<HParticleCand*> vtemp;
    vector<HParticleCand*> vtemp2;

    if(detswitch == 0 || HParticleTool::isPairsFlagsBit(detswitch,kSameRICH)){
	getSameRich(cand,vtemp,0,kTRUE);
	vtemp2.insert(vtemp2.end(),vtemp.begin(),vtemp.end());
    }
    if(detswitch == 0 || HParticleTool::isPairsFlagsBit(detswitch, kSameInnerMDC)){
	getSameInnerMdc(cand,vtemp,0,kTRUE);
	vtemp2.insert(vtemp2.end(),vtemp.begin(),vtemp.end());
    }
    if(detswitch == 0 || HParticleTool::isPairsFlagsBit(detswitch, kSameOuterMDC)){
	getSameOuterMdc(cand,vtemp,0,kTRUE);
	vtemp2.insert(vtemp2.end(),vtemp.begin(),vtemp.end());
    }
    if(detswitch == 0 || HParticleTool::isPairsFlagsBit(detswitch, kSameMETA)){
	getSameMeta(cand,vtemp,0,kTRUE);
	vtemp2.insert(vtemp2.end(),vtemp.begin(),vtemp.end());
    }
    sort(vtemp2.begin(),vtemp2.end());           // needs sort before unique

    vector<HParticleCand*>::iterator it;
    it = unique(vtemp2.begin(),vtemp2.end());    // remove doubles
    vtemp2.resize(distance(vtemp2.begin(),it));  // shrink it to real size

    for(UInt_t i=0;i<vtemp2.size();i++){
        HParticleCand* c = vtemp2[i] ;
	if(flag == 0 )  candidates.push_back(c);
	else {
	    Bool_t res = kFALSE;
	    if(!isReference) res=HParticleTool::evalPairsFlags(flag,cand,c);
	    else             res=HParticleTool::evalPairsFlags(flag,c,cand);
	    if(res){
		candidates.push_back(c);
	    }
	}
    }

    return candidates.size()==0 ? -1 : candidates.size();
}




