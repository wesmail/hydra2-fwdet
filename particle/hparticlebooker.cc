#include "hparticlebooker.h"
#include "hades.h"
#include "hcategory.h"
#include "hparticledef.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "hmdcdef.h"
#include "richdef.h"

#include "hparticlecand.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hrpccluster.h"
#include "hshowerhit.h"
#include "hemccluster.h"
#include "hmdcseg.h"
#include "hrichhit.h"
#include "hparticlecandsim.h"
#include "htofhitsim.h"
#include "htofclustersim.h"
#include "hrpcclustersim.h"
#include "hshowerhitsim.h"
#include "hemcclustersim.h"
#include "hmdcsegsim.h"
#include "hrichhitsim.h"
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
    mEmctoCand     .clear();
    mInnerMdctoCand.clear();
    mOuterMdctoCand.clear();
    mRichtoCand    .clear();

    vTofHitInd  .clear();
    vTofClstInd .clear();
    vRpcClstInd .clear();
    vShowerInd  .clear();
    vEmcInd     .clear();
    vInnerMdcInd.clear();
    vOuterMdcInd.clear();
    vRichInd    .clear();

    vTofHit  .clear();
    vTofClst .clear();
    vRpcClst .clear();
    vShower  .clear();
    vEmc     .clear();
    vInnerMdc.clear();
    vOuterMdc.clear();
    vRich    .clear();

    vTofHitAll  .clear();
    vTofClstAll .clear();
    vRpcClstAll .clear();
    vShowerAll  .clear();
    vEmcAll     .clear();
    vInnerMdcAll.clear();
    vOuterMdcAll.clear();
    vRichAll    .clear();

    mTracktoCand      .clear();
    mTracktoAnyCand   .clear();
    mTracktoTofHit    .clear();
    mTracktoTofCluster.clear();
    mTracktoRpcCluster.clear();
    mTracktoShowerHit .clear();
    mTracktoEmcCluster.clear();
    mTracktoInnerMdc  .clear();
    mTracktoOuterMdc  .clear();
    mTracktoRichHit   .clear();

    mTracktoCandInd      .clear();
    mTracktoAnyCandInd   .clear();
    mTracktoTofHitInd    .clear();
    mTracktoTofClusterInd.clear();
    mTracktoRpcClusterInd.clear();
    mTracktoShowerHitInd .clear();
    mTracktoEmcClusterInd.clear();
    mTracktoInnerMdcInd  .clear();
    mTracktoOuterMdcInd  .clear();
    mTracktoRichHitInd   .clear();

    gParticleBooker = NULL;
}



void HParticleBooker::bookHits(HParticleCand* cand1)
{
    // add candidate hits to maps
    HParticleCandSim* candS = dynamic_cast<HParticleCandSim*>(cand1);

    if(cand1->getRichInd() !=-1 ) {
	if(mRichtoCand.find(cand1->getRichInd()) == mRichtoCand.end()){
	    vector<HParticleCand*> v;
	    v.push_back(cand1);
	    mRichtoCand[cand1->getRichInd()] = v;
            vRichInd.push_back(cand1->getRichInd());
	} else {
	    mRichtoCand[cand1->getRichInd()].push_back(cand1);
	}

	if(candS)   // SIM ONLY
	{
	    for(Int_t i = 0; i < 3; i ++){
		Int_t tr = candS->getGeantTrackRich(i);
		if(tr > 0)
		{
		    if(mTracktoRichHitInd.find(tr) == mTracktoRichHitInd.end()){  // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getRichInd());
			mTracktoRichHitInd[tr] = vI;
			if(richhitCat){
			    HRichHitSim* hitS;
			    hitS = HCategoryManager::getObject(hitS,richhitCat,cand1->getRichInd());
			    vector<HRichHitSim*> v;
			    v.push_back(hitS);
			    mTracktoRichHit[tr] = v;
			}
		    } else {
			if(find(mTracktoRichHitInd[tr].begin(),mTracktoRichHitInd[tr].end(),cand1->getRichInd()) == mTracktoRichHitInd[tr].end()){
			    mTracktoRichHitInd[tr].push_back(cand1->getRichInd());
			    if(richhitCat){
				HRichHitSim* hitS;
				hitS = HCategoryManager::getObject(hitS,richhitCat,cand1->getRichInd());
				mTracktoRichHit[tr].push_back(hitS);
			    }
			}
		    }

                    // map all tracks of all hits to candidates
		    if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){  // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getIndex());
			mTracktoAnyCandInd[tr] = vI;
			vector<HParticleCandSim*> v;
			v.push_back(candS);
			mTracktoAnyCand[tr] = v;
		    } else {
			if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
			    mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
			    mTracktoAnyCand[tr].push_back(candS);
			}
		    }
		}
	    }
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

	if(candS)  // SIM ONLY
	{
	    for(Int_t i = 0; i < 2; i ++){
		Int_t tr = candS->getGeantTrackInnerMdc(i);
		if(tr > 0)
		{
		    if(mTracktoInnerMdcInd.find(tr) == mTracktoInnerMdcInd.end()){  // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getInnerSegInd());
			mTracktoInnerMdcInd[tr] = vI;
			if(mdcsegCat){
			    HMdcSegSim* hitS;
			    hitS = HCategoryManager::getObject(hitS,mdcsegCat,cand1->getInnerSegInd());
			    vector<HMdcSegSim*> v;
			    v.push_back(hitS);
			    mTracktoInnerMdc[tr] = v;
			}
		    } else {
			if(find(mTracktoInnerMdcInd[tr].begin(),mTracktoInnerMdcInd[tr].end(),cand1->getInnerSegInd()) == mTracktoInnerMdcInd[tr].end()){
			    mTracktoInnerMdcInd[tr].push_back(cand1->getInnerSegInd());
			    if(mdcsegCat){
				HMdcSegSim* hitS;
				hitS = HCategoryManager::getObject(hitS,mdcsegCat,cand1->getInnerSegInd());
				mTracktoInnerMdc[tr].push_back(hitS);
			    }
			}
		    }

                    // map all tracks of all hits to candidates
		    if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){    // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getIndex());
			mTracktoAnyCandInd[tr] = vI;
			vector<HParticleCandSim*> v;
			v.push_back(candS);
			mTracktoAnyCand[tr] = v;
		    } else {
			if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
			    mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
			    mTracktoAnyCand[tr].push_back(candS);
			}
		    }
		}
	    }
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

	if(candS)  // SIM ONLY
	{
	    for(Int_t i = 0; i < 2; i ++){
		Int_t tr = candS->getGeantTrackOuterMdc(i);
		if(tr > 0)
		{
		    if(mTracktoOuterMdcInd.find(tr) == mTracktoOuterMdcInd.end()){   // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getOuterSegInd());
			mTracktoOuterMdcInd[tr] = vI;
			if(mdcsegCat){
			    HMdcSegSim* hitS;
			    hitS = HCategoryManager::getObject(hitS,mdcsegCat,cand1->getOuterSegInd());
			    vector<HMdcSegSim*> v;
			    v.push_back(hitS);
			    mTracktoOuterMdc[tr] = v;
			}
		    } else {
			if(find(mTracktoOuterMdcInd[tr].begin(),mTracktoOuterMdcInd[tr].end(),cand1->getOuterSegInd()) == mTracktoOuterMdcInd[tr].end()){
			    mTracktoOuterMdcInd[tr].push_back(cand1->getOuterSegInd());
			    if(mdcsegCat){
				HMdcSegSim* hitS;
				hitS = HCategoryManager::getObject(hitS,mdcsegCat,cand1->getOuterSegInd());
				mTracktoOuterMdc[tr].push_back(hitS);
			    }
			}
		    }

                    // map all tracks of all hits to candidates
		    if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){    // new Track
			vector<Int_t> vI;
			vI.push_back(cand1->getIndex());
			mTracktoAnyCandInd[tr] = vI;
			vector<HParticleCandSim*> v;
			v.push_back(candS);
			mTracktoAnyCand[tr] = v;
		    } else {
			if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
			    mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
			    mTracktoAnyCand[tr].push_back(candS);
			}
		    }
		}
	    }
	}
    }

    if(cand1->getSystemUsed() != -1){
	Int_t meta = cand1->getMetaHitInd();
        Int_t showerInd = cand1->getShowerInd();
        Int_t emcInd    = cand1->getEmcInd();
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

	    if(candS) // SIM ONLY
	    {
		for(Int_t i = 0; i < 4; i ++){
		    Int_t tr = candS->getGeantTrackMeta(i);
		    if(tr > 0)
		    {
			if(mTracktoTofHitInd.find(tr) == mTracktoTofHitInd.end()){     // new Track
			    vector<Int_t> vI;
			    vI.push_back(meta);
			    mTracktoTofHitInd[tr] = vI;
			    if(tofhitCat){
				HTofHitSim* hitS;
				hitS = HCategoryManager::getObject(hitS,tofhitCat,meta);
				vector<HTofHitSim*> v;
				v.push_back(hitS);
				mTracktoTofHit[tr] = v;
			    }
			} else {
			    if(find(mTracktoTofHitInd[tr].begin(),mTracktoTofHitInd[tr].end(),meta) == mTracktoTofHitInd[tr].end()){
				mTracktoTofHitInd[tr].push_back(meta);
				if(tofhitCat){
				    HTofHitSim* hitS;
				    hitS = HCategoryManager::getObject(hitS,tofhitCat,meta);
				    mTracktoTofHit[tr].push_back(hitS);
				}
			    }
			}

			// map all tracks of all hits to candidates
			if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){   // new Track
			    vector<Int_t> vI;
			    vI.push_back(cand1->getIndex());
			    mTracktoAnyCandInd[tr] = vI;
			    vector<HParticleCandSim*> v;
			    v.push_back(candS);
			    mTracktoAnyCand[tr] = v;
			} else {
			    if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
				mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
				mTracktoAnyCand[tr].push_back(candS);
			    }
			}
		    }
		}
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

	    if(candS)  // SIM ONLY
	    {
		for(Int_t i = 0; i < 4; i ++){
		    Int_t tr = candS->getGeantTrackMeta(i);
		    if(tr > 0)
		    {
			if(mTracktoTofClusterInd.find(tr) == mTracktoTofClusterInd.end()){  // new Track
			    vector<Int_t> vI;
			    vI.push_back(meta);
			    mTracktoTofClusterInd[tr] = vI;
			    if(tofclstCat){
				HTofClusterSim* hitS;
				hitS = HCategoryManager::getObject(hitS,tofclstCat,meta);
				vector<HTofClusterSim*> v;
				v.push_back(hitS);
				mTracktoTofCluster[tr] = v;
			    }
			} else {
			    if(find(mTracktoTofClusterInd[tr].begin(),mTracktoTofClusterInd[tr].end(),meta) == mTracktoTofClusterInd[tr].end()){
				mTracktoTofClusterInd[tr].push_back(meta);
				if(tofclstCat){
				    HTofClusterSim* hitS;
				    hitS = HCategoryManager::getObject(hitS,tofclstCat,meta);
				    mTracktoTofCluster[tr].push_back(hitS);
				}
			    }
			}

			// map all tracks of all hits to candidates
			if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){        // new Track
			    vector<Int_t> vI;
			    vI.push_back(cand1->getIndex());
			    mTracktoAnyCandInd[tr] = vI;
			    vector<HParticleCandSim*> v;
			    v.push_back(candS);
			    mTracktoAnyCand[tr] = v;
			} else {
			    if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
				mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
				mTracktoAnyCand[tr].push_back(candS);
			    }
			}
		    }
		}
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

	    if(candS)  // SIM ONLY
	    {
		for(Int_t i = 0; i < 4; i ++){
		    Int_t tr = candS->getGeantTrackMeta(i);
		    if(tr > 0)
		    {
			if(mTracktoRpcClusterInd.find(tr) == mTracktoRpcClusterInd.end()){   // new Track
			    vector<Int_t> vI;
			    vI.push_back(meta);
			    mTracktoRpcClusterInd[tr] = vI;
			    if(rpcclstCat){
				HRpcClusterSim* hitS;
				hitS = HCategoryManager::getObject(hitS,rpcclstCat,meta);
				vector<HRpcClusterSim*> v;
				v.push_back(hitS);
				mTracktoRpcCluster[tr] = v;
			    }
			} else {
			    if(find(mTracktoRpcClusterInd[tr].begin(),mTracktoRpcClusterInd[tr].end(),meta) == mTracktoRpcClusterInd[tr].end()){
				mTracktoRpcClusterInd[tr].push_back(meta);
				if(rpcclstCat){
				    HRpcClusterSim* hitS;
				    hitS = HCategoryManager::getObject(hitS,rpcclstCat,meta);
				    mTracktoRpcCluster[tr].push_back(hitS);
				}
			    }
			}

			// map all tracks of all hits to candidates
			if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){        // new Track
			    vector<Int_t> vI;
			    vI.push_back(cand1->getIndex());
			    mTracktoAnyCandInd[tr] = vI;
			    vector<HParticleCandSim*> v;
			    v.push_back(candS);
			    mTracktoAnyCand[tr] = v;
			} else {
			    if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
				mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
				mTracktoAnyCand[tr].push_back(candS);
			    }
			}
		    }
		}
	    }
	}

	if(cand1->isEmc())
	{
	    if (emcInd!=-1){
		if(mEmctoCand.find(emcInd) == mEmctoCand.end()){
		    vector<HParticleCand*> v;
		    v.push_back(cand1);
		    mEmctoCand[emcInd] = v;
		    vEmcInd.push_back(emcInd);
		    HEmcCluster* hit = HParticleTool::getEmcCluster(emcInd);
		    if(hit) vEmc.push_back(hit);
		} else {
		    mEmctoCand[emcInd].push_back(cand1);
		}

		if(candS)  // SIM ONLY
		{
		    for(Int_t i = 0; i < 4; i ++){
			Int_t tr = candS->getGeantTrackMeta(i);
			if(tr > 0)
			{
			    if(mTracktoEmcClusterInd.find(tr) == mTracktoEmcClusterInd.end()){    // new Track
				vector<Int_t> vI;
				vI.push_back(meta);
				mTracktoEmcClusterInd[tr] = vI;
				if(emcclusterCat){
				    HEmcClusterSim* hitS;
				    hitS = HCategoryManager::getObject(hitS,emcclusterCat,meta);
				    vector<HEmcClusterSim*> v;
				    v.push_back(hitS);
				    mTracktoEmcCluster[tr] = v;
				}
			    } else {
				if(find(mTracktoEmcClusterInd[tr].begin(),mTracktoEmcClusterInd[tr].end(),meta) == mTracktoEmcClusterInd[tr].end()){
				    mTracktoEmcClusterInd[tr].push_back(meta);
				    if(emcclusterCat){
					HEmcClusterSim* hitS;
					hitS = HCategoryManager::getObject(hitS,emcclusterCat,meta);
					mTracktoEmcCluster[tr].push_back(hitS);
				    }
				}
			    }

			    // map all tracks of all hits to candidates
			    if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){      // new Track
				vector<Int_t> vI;
				vI.push_back(cand1->getIndex());
				mTracktoAnyCandInd[tr] = vI;
				vector<HParticleCandSim*> v;
				v.push_back(candS);
				mTracktoAnyCand[tr] = v;
			    } else {
				if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
				    mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
				    mTracktoAnyCand[tr].push_back(candS);
				}
			    }
			}
		    }
		}
	    }

	} else {
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

		if(candS)  // SIM ONLY
		{
		    for(Int_t i = 0; i < 4; i ++){
			Int_t tr = candS->getGeantTrackMeta(i);
			if(tr > 0)
			{
			    if(mTracktoShowerHitInd.find(tr) == mTracktoShowerHitInd.end()){    // new Track
				vector<Int_t> vI;
				vI.push_back(meta);
				mTracktoShowerHitInd[tr] = vI;
				if(showerhitCat){
				    HShowerHitSim* hitS;
				    hitS = HCategoryManager::getObject(hitS,showerhitCat,meta);
				    vector<HShowerHitSim*> v;
				    v.push_back(hitS);
				    mTracktoShowerHit[tr] = v;
				}
			    } else {
				if(find(mTracktoShowerHitInd[tr].begin(),mTracktoShowerHitInd[tr].end(),meta) == mTracktoShowerHitInd[tr].end()){
				    mTracktoShowerHitInd[tr].push_back(meta);
				    if(showerhitCat){
					HShowerHitSim* hitS;
					hitS = HCategoryManager::getObject(hitS,showerhitCat,meta);
					mTracktoShowerHit[tr].push_back(hitS);
				    }
				}
			    }

			    // map all tracks of all hits to candidates
			    if(mTracktoAnyCandInd.find(tr) == mTracktoAnyCandInd.end()){      // new Track
				vector<Int_t> vI;
				vI.push_back(cand1->getIndex());
				mTracktoAnyCandInd[tr] = vI;
				vector<HParticleCandSim*> v;
				v.push_back(candS);
				mTracktoAnyCand[tr] = v;
			    } else {
				if(find(mTracktoAnyCandInd[tr].begin(),mTracktoAnyCandInd[tr].end(),cand1->getIndex()) == mTracktoAnyCandInd[tr].end()){
				    mTracktoAnyCandInd[tr].push_back(cand1->getIndex());
				    mTracktoAnyCand[tr].push_back(candS);
				}
			    }
			}
		    }
		}
	    }

	}
    }


    // map global track to candidates
    if(candS)  // SIM ONLY
    {
	Int_t tr = candS->getGeantTrack();
	if(tr > 0)
	{
	    if(mTracktoCandInd.find(tr) == mTracktoCandInd.end()){       // new Track
		vector<Int_t> vI;
		vI.push_back(cand1->getIndex());
		mTracktoCandInd[tr] = vI;
		vector<HParticleCandSim*> v;
		v.push_back(candS);
		mTracktoCand[tr] = v;
	    } else {
		if(find(mTracktoCandInd[tr].begin(),mTracktoCandInd[tr].end(),cand1->getIndex()) == mTracktoCandInd[tr].end()){
		    mTracktoCandInd[tr].push_back(cand1->getIndex());
		    mTracktoCand[tr].push_back(candS);
		}
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
    mEmctoCand     .clear();
    mInnerMdctoCand.clear();
    mOuterMdctoCand.clear();
    mRichtoCand    .clear();

    vTofHitInd  .clear();
    vTofClstInd .clear();
    vRpcClstInd .clear();
    vShowerInd  .clear();
    vEmcInd     .clear();
    vInnerMdcInd.clear();
    vOuterMdcInd.clear();
    vRichInd    .clear();

    vTofHit  .clear();
    vTofClst .clear();
    vRpcClst .clear();
    vShower  .clear();
    vEmc     .clear();
    vInnerMdc.clear();
    vOuterMdc.clear();
    vRich    .clear();

    vTofHitAll  .clear();
    vTofClstAll .clear();
    vRpcClstAll .clear();
    vShowerAll  .clear();
    vEmcAll     .clear();
    vInnerMdcAll.clear();
    vOuterMdcAll.clear();
    vRichAll    .clear();

    mTracktoCand      .clear();
    mTracktoAnyCand   .clear();
    mTracktoTofHit    .clear();
    mTracktoTofCluster.clear();
    mTracktoRpcCluster.clear();
    mTracktoShowerHit .clear();
    mTracktoEmcCluster.clear();
    mTracktoInnerMdc  .clear();
    mTracktoOuterMdc  .clear();
    mTracktoRichHit   .clear();

    mTracktoCandInd      .clear();
    mTracktoAnyCandInd   .clear();
    mTracktoTofHitInd    .clear();
    mTracktoTofClusterInd.clear();
    mTracktoRpcClusterInd.clear();
    mTracktoShowerHitInd .clear();
    mTracktoEmcClusterInd.clear();
    mTracktoInnerMdcInd  .clear();
    mTracktoOuterMdcInd  .clear();
    mTracktoRichHitInd   .clear();


    tofhitCat = HCategoryManager::getCategory(catTofHit,2);
    if(tofhitCat)
    {
	Int_t size = tofhitCat->getEntries();
	HTofHit* hit = 0;
	HTofHitSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,tofhitCat,j);
	    if(hit)
	    {
		vTofHitAll.push_back(hit);
		hitS = dynamic_cast<HTofHitSim*>(hit);
		if(hitS)
		{
		    Int_t tr = hitS->getNTrack1();
		    if(tr > 0){
			if(mTracktoTofHit.find(tr) == mTracktoTofHit.end()){
			    vector<HTofHitSim*> v;
			    vector<Int_t> vI;
			    v.push_back(hitS);
			    vI.push_back(j);
			    mTracktoTofHit[tr] = v;
			    mTracktoTofHitInd[tr] = vI;
			} else {
			    if(find(mTracktoTofHit[tr].begin(),mTracktoTofHit[tr].end(),hitS) == mTracktoTofHit[tr].end()){
				mTracktoTofHit[tr].push_back(hitS);
				mTracktoTofHitInd[tr].push_back(j);
			    }
			}
		    }
		    tr = hitS->getNTrack2();
		    if(tr > 0){
			if(mTracktoTofHit.find(tr) == mTracktoTofHit.end()){
			    vector<HTofHitSim*> v;
			    vector<Int_t> vI;
			    v.push_back(hitS);
			    vI.push_back(j);
			    mTracktoTofHit[tr] = v;
			    mTracktoTofHitInd[tr] = vI;
			} else {
			    if(find(mTracktoTofHit[tr].begin(),mTracktoTofHit[tr].end(),hitS) == mTracktoTofHit[tr].end()){
				mTracktoTofHit[tr].push_back(hitS);
				mTracktoTofHitInd[tr].push_back(j);
			    }
			}
		    }
		}
	    }
	}
    }

    tofclstCat = HCategoryManager::getCategory(catTofCluster,2);
    if(tofclstCat)
    {
	Int_t size = tofclstCat->getEntries();
	HTofCluster* hit = 0;
	HTofClusterSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,tofclstCat,j);
	    if(hit)
	    {
		vTofClstAll.push_back(hit);
		hitS = dynamic_cast<HTofClusterSim*>(hit);
		if(hitS)
		{

		    for(Int_t i = 0; i < hit->getClusterSize()&&i<3; i ++)
		    {

			Int_t tr = hitS->getNTrack1(i);
			if(tr > 0){
			    if(mTracktoTofCluster.find(tr) == mTracktoTofCluster.end()){
				vector<HTofClusterSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoTofCluster[tr] = v;
				mTracktoTofClusterInd[tr] = vI;
			    } else {
				if(find(mTracktoTofCluster[tr].begin(),mTracktoTofCluster[tr].end(),hitS) == mTracktoTofCluster[tr].end()){
				    mTracktoTofCluster[tr].push_back(hitS);
				    mTracktoTofClusterInd[tr].push_back(j);
				}
			    }
			}
			tr = hitS->getNTrack2(i);
			if(tr > 0){
			    if(mTracktoTofCluster.find(tr) == mTracktoTofCluster.end()){
				vector<HTofClusterSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoTofCluster[tr] = v;
				mTracktoTofClusterInd[tr] = vI;
			    } else {
				if(find(mTracktoTofCluster[tr].begin(),mTracktoTofCluster[tr].end(),hitS) == mTracktoTofCluster[tr].end()){
				    mTracktoTofCluster[tr].push_back(hitS);
				    mTracktoTofClusterInd[tr].push_back(j);
				}
			    }
			}
		    }
		}
	    }
	}
    }

    rpcclstCat = HCategoryManager::getCategory(catRpcCluster,2);
    if(rpcclstCat)
    {
	Int_t size = rpcclstCat->getEntries();
	HRpcCluster* hit = 0;
	HRpcClusterSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,rpcclstCat,j);
	    if(hit)
	    {
		vRpcClstAll.push_back(hit);
		hitS = dynamic_cast<HRpcClusterSim*>(hit);
		if(hitS)
		{
		    Int_t TrackList[4];
		    hitS->getTrackList(TrackList);
		    for(Int_t i = 0;i < 4; i ++)
		    {
			Int_t tr = TrackList[i];
			if(tr > 0){
			    if(mTracktoRpcCluster.find(tr) == mTracktoRpcCluster.end()){
				vector<HRpcClusterSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoRpcCluster[tr] = v;
				mTracktoRpcClusterInd[tr] = vI;
			    } else {
				if(find(mTracktoRpcCluster[tr].begin(),mTracktoRpcCluster[tr].end(),hitS) == mTracktoRpcCluster[tr].end()){
				    mTracktoRpcCluster[tr].push_back(hitS);
				    mTracktoRpcClusterInd[tr].push_back(j);
				}
			    }
			}
		    }
		}
	    }
	}
    }

    showerhitCat = HCategoryManager::getCategory(catShowerHit,2);
    if(showerhitCat)
    {
	Int_t size = showerhitCat->getEntries();
	HShowerHit* hit = 0;
	HShowerHitSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,showerhitCat,j);
	    if(hit)
	    {
		vShowerAll.push_back(hit);
		hitS = dynamic_cast<HShowerHitSim*>(hit);
		if(hitS)
		{
		    for(Int_t i = 0; i <hitS->getNTracks() ; i ++)
		    {
			Int_t tr = hitS->getTrack(i);
			if(tr > 0){
			    if(mTracktoShowerHit.find(tr) == mTracktoShowerHit.end()){
				vector<HShowerHitSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoShowerHit[tr] = v;
				mTracktoShowerHitInd[tr] = vI;
			    } else {
				if(find(mTracktoShowerHit[tr].begin(),mTracktoShowerHit[tr].end(),hitS) == mTracktoShowerHit[tr].end()){
				    mTracktoShowerHit[tr].push_back(hitS);
				    mTracktoShowerHitInd[tr].push_back(j);
				}
			    }
			}
		    }
		}
	    }
	}
    }

    emcclusterCat = HCategoryManager::getCategory(catEmcCluster,2);
    if(emcclusterCat)
    {
	Int_t size = emcclusterCat->getEntries();
	HEmcCluster* hit = 0;
	HEmcClusterSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,emcclusterCat,j);
	    if(hit)
	    {
		vEmcAll.push_back(hit);
		hitS = dynamic_cast<HEmcClusterSim*>(hit);
		if(hitS)
		{
		    for(Int_t i = 0; i <hitS->getNTracks() ; i ++)
		    {
			Int_t tr = hitS->getTrack(i);
			if(tr > 0){
			    if(mTracktoEmcCluster.find(tr) == mTracktoEmcCluster.end()){
				vector<HEmcClusterSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoEmcCluster[tr] = v;
				mTracktoEmcClusterInd[tr] = vI;
			    } else {
				if(find(mTracktoEmcCluster[tr].begin(),mTracktoEmcCluster[tr].end(),hitS) == mTracktoEmcCluster[tr].end()){
				    mTracktoEmcCluster[tr].push_back(hitS);
				    mTracktoEmcClusterInd[tr].push_back(j);
				}
			    }
			}
		    }
		}
	    }
	}
    }

    mdcsegCat = HCategoryManager::getCategory(catMdcSeg,2);
    if(mdcsegCat)
    {
	Int_t size = mdcsegCat->getEntries();
	HMdcSeg* hit = 0;
	HMdcSegSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,mdcsegCat,j);
	    if(hit)
	    {

		if(hit->getIOSeg() == 0)
		{
		    vInnerMdcAll.push_back(hit);
		    hitS = dynamic_cast<HMdcSegSim*>(hit);
		    if(hitS)
		    {
			for(Int_t i = 0; i <hitS->getNTracks() ; i ++)
			{
			    Int_t tr = hitS->getTrack(i);
			    if(tr > 0){
				if(mTracktoInnerMdc.find(tr) == mTracktoInnerMdc.end()){
				    vector<HMdcSegSim*> v;
				    vector<Int_t> vI;
				    v.push_back(hitS);
				    vI.push_back(j);
				    mTracktoInnerMdc[tr] = v;
				    mTracktoInnerMdcInd[tr] = vI;
				} else {
				    if(find(mTracktoInnerMdc[tr].begin(),mTracktoInnerMdc[tr].end(),hitS) == mTracktoInnerMdc[tr].end()){
					mTracktoInnerMdc[tr].push_back(hitS);
					mTracktoInnerMdcInd[tr].push_back(j);
				    }
				}
			    }
			}
		    }
		}
		if(hit->getIOSeg() == 1) {
		    vOuterMdcAll.push_back(hit);
		    hitS = dynamic_cast<HMdcSegSim*>(hit);
		    if(hitS)
		    {
			for(Int_t i = 0; i <hitS->getNTracks() ; i ++)
			{
			    Int_t tr = hitS->getTrack(i);
			    if(tr > 0){
				if(mTracktoOuterMdc.find(tr) == mTracktoOuterMdc.end()){
				    vector<HMdcSegSim*> v;
				    vector<Int_t> vI;
				    v.push_back(hitS);
				    vI.push_back(j);
				    mTracktoOuterMdc[tr] = v;
				    mTracktoOuterMdcInd[tr] = vI;
				} else {
				    if(find(mTracktoOuterMdc[tr].begin(),mTracktoOuterMdc[tr].end(),hitS) == mTracktoOuterMdc[tr].end()){
					mTracktoOuterMdc[tr].push_back(hitS);
					mTracktoOuterMdcInd[tr].push_back(j);
				    }
				}
			    }
			}
		    }
		}
	    }
	}
    }

    richhitCat = HCategoryManager::getCategory(catRichHit,2);
    if(richhitCat)
    {
	Int_t size = richhitCat->getEntries();
	HRichHit* hit = 0;
	HRichHitSim* hitS = 0;
	for(Int_t j = 0; j < size; j++) {
	    hit = HCategoryManager::getObject(hit,richhitCat,j);
	    if(hit)
	    {
		vRichAll.push_back(hit);
		hitS = dynamic_cast<HRichHitSim*>(hit);
		if(hitS)
		{
		    for(Int_t i = 0; i < 3 ; i ++)
		    {
			Int_t tr = 0;
			if     (i==0)tr = hitS->track1;
			else if(i==1)tr = hitS->track2;
			else         tr = hitS->track3;

			if(tr > 0){
			    if(mTracktoRichHit.find(tr) == mTracktoRichHit.end()){
				vector<HRichHitSim*> v;
				vector<Int_t> vI;
				v.push_back(hitS);
				vI.push_back(j);
				mTracktoRichHit[tr] = v;
				mTracktoRichHitInd[tr] = vI;
			    } else {
				if(find(mTracktoRichHit[tr].begin(),mTracktoRichHit[tr].end(),hitS) == mTracktoRichHit[tr].end()){
				    mTracktoRichHit[tr].push_back(hitS);
				    mTracktoRichHitInd[tr].push_back(j);
				}
			    }
			}
		    }
		}
	    }
	}
    }

    candCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catParticleCand);
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

Int_t HParticleBooker::getCandidatesForEmc(Int_t index,vector<HParticleCand*>& hits)
{
    // fills vector of candidates using this hit index. returns the size of the vector
    hits.clear();
    map<Int_t,vector<HParticleCand*> > :: iterator it;
    it = mEmctoCand.find(index);
    if(it!=mEmctoCand.end()){
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

Int_t HParticleBooker::getCandidatesAnyDetectorForTrack     (Int_t track,vector<HParticleCandSim*>& cands)
{
    // fills vector of candidates using this track in any detector hit.
    // returns the size of the vector
    cands.clear();
    map<Int_t,vector<HParticleCandSim*> > :: iterator it;
    it = mTracktoAnyCand.find(track);
    if(it!=mTracktoAnyCand.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesForTrack     (Int_t track,vector<HParticleCandSim*>& cands)
{
    // fills vector of candidates using this global track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HParticleCandSim*> > :: iterator it;
    it = mTracktoCand.find(track);
    if(it!=mTracktoCand.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}


Int_t HParticleBooker::getTofHitForTrack         (Int_t track,vector<HTofHitSim*>&       cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HTofHitSim*> > :: iterator it;
    it = mTracktoTofHit.find(track);
    if(it!=mTracktoTofHit.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getTofClusterForTrack     (Int_t track,vector<HTofClusterSim*>&   cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HTofClusterSim*> > :: iterator it;
    it = mTracktoTofCluster.find(track);
    if(it!=mTracktoTofCluster.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getRpcClusterForTrack     (Int_t track,vector<HRpcClusterSim*>&   cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HRpcClusterSim*> > :: iterator it;
    it = mTracktoRpcCluster.find(track);
    if(it!=mTracktoRpcCluster.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getShowerHitForTrack      (Int_t track,vector<HShowerHitSim*>&    cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HShowerHitSim*> > :: iterator it;
    it = mTracktoShowerHit.find(track);
    if(it!=mTracktoShowerHit.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getEmcClusterForTrack      (Int_t track,vector<HEmcClusterSim*>&    cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HEmcClusterSim*> > :: iterator it;
    it = mTracktoEmcCluster.find(track);
    if(it!=mTracktoEmcCluster.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getInnerMdcSegForTrack    (Int_t track,vector<HMdcSegSim*>&       cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HMdcSegSim*> > :: iterator it;
    it = mTracktoInnerMdc.find(track);
    if(it!=mTracktoInnerMdc.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getOuterMdcSegForTrack    (Int_t track,vector<HMdcSegSim*>&       cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HMdcSegSim*> > :: iterator it;
    it = mTracktoOuterMdc.find(track);
    if(it!=mTracktoOuterMdc.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getRichHitForTrack        (Int_t track,vector<HRichHitSim*>&      cands)
{
    // fills vector of hit using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<HRichHitSim*> > :: iterator it;
    it = mTracktoRichHit.find(track);
    if(it!=mTracktoRichHit.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesIndAnyDetectorForTrack     (Int_t track,vector<Int_t>& cands)
{
    // fills vector of candidates indices using this track in any detector hit.
    // returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoAnyCandInd.find(track);
    if(it!=mTracktoAnyCandInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getCandidatesIndForTrack     (Int_t track,vector<Int_t>& cands)
{
    // fills vector of candidates indices using this global track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoCandInd.find(track);
    if(it!=mTracktoCandInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}


Int_t HParticleBooker::getTofHitIndForTrack         (Int_t track,vector<Int_t>&       cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoTofHitInd.find(track);
    if(it!=mTracktoTofHitInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getTofClusterIndForTrack     (Int_t track,vector<Int_t>&   cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoTofClusterInd.find(track);
    if(it!=mTracktoTofClusterInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getRpcClusterIndForTrack     (Int_t track,vector<Int_t>&   cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoRpcClusterInd.find(track);
    if(it!=mTracktoRpcClusterInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getShowerHitIndForTrack      (Int_t track,vector<Int_t>&    cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoShowerHitInd.find(track);
    if(it!=mTracktoShowerHitInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getEmcClusterIndForTrack      (Int_t track,vector<Int_t>&    cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoEmcClusterInd.find(track);
    if(it!=mTracktoEmcClusterInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getInnerMdcSegIndForTrack    (Int_t track,vector<Int_t>&       cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoInnerMdcInd.find(track);
    if(it!=mTracktoInnerMdcInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getOuterMdcSegIndForTrack    (Int_t track,vector<Int_t>&       cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoOuterMdcInd.find(track);
    if(it!=mTracktoOuterMdcInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
    } else {
	return 0;
    }
}

Int_t HParticleBooker::getRichHitIndForTrack        (Int_t track,vector<Int_t>&      cands)
{
    // fills vector of hit indices using this track. returns the size of the vector
    cands.clear();
    map<Int_t,vector<Int_t> > :: iterator it;
    it = mTracktoRichHitInd.find(track);
    if(it!=mTracktoRichHitInd.end()){
	cands.assign(it->second.begin(),it->second.end());
	return cands.size();
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




