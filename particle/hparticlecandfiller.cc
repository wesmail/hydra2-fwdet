#include "hparticlecandfiller.h"
#include "hcategorymanager.h"
#include "hparticletool.h"
#include "hgeomvolume.h"
//---------- detector def ----------
#include "hgeantdef.h"
#include "hparticledef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "tofdef.h"
#include "showerdef.h"
#include "richdef.h"
#include "rpcdef.h"
#include "emcdef.h"
//---------- hades etc -------------
#include "hades.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hlocation.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
//--------- data objects -----------
#include "hmetamatch2.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdctrkcand.h"
#include "hsplinetrack.h"
#include "hrktrackB.h"
#include "hshowerhitsim.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hrpccluster.h"
#include "hrpcclustersim.h"

#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticlemdc.h"
#include "hparticlecandfillerpar.h"
#include "htofwalkpar.h"
#include "hparticleanglecor.h"

#include "hmdclayer.h"

#include "TVector2.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <algorithm>
#include <numeric>

using namespace std;
using namespace Particle;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// HParticleCandFiller
//
// This tasks loops over HMetaMatch2 objects and fills
// the output objects HParticleCand. The taks detects
// if it is running in simulation or read data mode automatically
// by the existence of the HGeantKine category in the input file.
// Different filter options can be applied (see description of
// setConditions() ).
///////////////////////////////////////////////////////////////////////////////

ClassImp(HParticleCandFiller)

HParticleCandFiller::HParticleCandFiller (const Option_t par[]):
    HReconstructor ()
{
    // Constructor calls the constructor of class HReconstructor without the name
    // and the title as arguments. Optional parameters are explained in setConditions().

    initVars ();
    setConditions(par);
}

HParticleCandFiller::HParticleCandFiller (const Text_t * name,
					  const Text_t * title,
					  const Option_t par[]):
HReconstructor (name, title)
{
    // Constructor calls the constructor of class HReconstructor with the name
    // and the title as arguments. Optional parameters are explained in setConditions().

    initVars ();
    setConditions(par);
}

HParticleCandFiller::~HParticleCandFiller (void)
{
    // destructor deletes the iterator
    if(fMetaMatchIter) delete fMetaMatchIter;

}
void HParticleCandFiller::setConditions(const Option_t par[])
{
    // Set parameters by names. Options (may be separated by comma or blank chars),
    // By default (no option specified) all values will be kFALSE:
    // GOODSEG0   - skip the track when inner seg chi2 < 0
    // GOODSEG1   - skip the track when outer seg chi2 < 0
    // GOODMETA   - skip the track when no META was fired
    // GOODRK     - skip the track when RK chi2 < 0
    // KALMAN     - switch from RK to Kalman for mom reco
    // GOODLEPTON - skip all candidates which have no RICH matching
    // DEBUG      - write out candidate objects for debugging
    // NORICHALIGN- dont align rich phi and theta
    // NORICHVERTEXCORR- dont do vertex correction in ring sorting
    // NOMETAQANORM - dont do normaization of RK dx
    // NOGEANTACCEPTANCE - dont fill geant acceptance and crop bits
    // NOFAKE     - skip fake marked candidates
    // ACCEPTFAKE - do not skip fake marked candidates

    TString s = par;
    s.ToUpper();

    fbgoodSeg0   = (strstr(s.Data(), "GOOGSEG0")  != NULL);
    fbgoodSeg1   = (strstr(s.Data(), "GOODSEG1")  != NULL);
    fbgoodMeta   = (strstr(s.Data(), "GOODMETA")  != NULL);
    fbgoodRK     = (strstr(s.Data(), "GOODRK")    != NULL);
    fbgoodLepton = (strstr(s.Data(), "GOODLEPTON")!= NULL);
    fmomSwitch   = (strstr(s.Data(), "KALMAN")    != NULL) ? Particle::kMomKalman : Particle::kMomRK;
    fbIsDebug    = (strstr(s.Data(), "DEBUG")     != NULL);
    fbdoRichAlign= (strstr(s.Data(), "NORICHALIGN")!= NULL)? kFALSE : kTRUE;
    fbdoRichVertexCorr = (strstr(s.Data(), "NORICHVERTEXCORR")!= NULL)? kFALSE : kTRUE;
    fbdoMETAQANorm     = (strstr(s.Data(), "NOMETAQANORM")!= NULL)? kFALSE : kTRUE;
    fbdoMomentumCorr   = (strstr(s.Data(), "NOMOMENTUMCORR")!= NULL)? kFALSE : kTRUE;
    fbdoPathLengthCorr = (strstr(s.Data(), "NOPATHLENGTHCORR")!= NULL)? kFALSE : kTRUE;
    fbdoGeantAcceptance= (strstr(s.Data(), "NOGEANTACCPETANCE")!= NULL)? kFALSE : kTRUE;
    fbnoFake     = (strstr(s.Data(), "NOFAKE")    != NULL);
    if(strstr(s.Data(), "ACCEPTFAKE")!= NULL)fbnoFake=kFALSE;
}

void HParticleCandFiller::initVars ()
{
    // set internal variables to start values

    fCatMdcTrkCand   = NULL;
    fCatMetaMatch    = NULL;

    fCatMdcSeg       = NULL;
    fCatTofHit       = NULL;
    fCatTofCluster   = NULL;
    fCatEmcCluster   = NULL;
    fCatShowerHit    = NULL;
    fCatRichHit      = NULL;
    fCatRpcCluster   = NULL;
    fCatSpline       = NULL;
    fCatRK           = NULL;
    fCatKalman       = NULL;
    fCatParticleCand = NULL;
    fCatGeantKine    = NULL;

    fMetaMatchIter   = NULL;

    fCatParticleCand = NULL;
    fCatParticleDebug= NULL;
    fCatParticleMdc  = NULL;

    fFillerPar       = NULL;
    fTofWalkPar      = NULL;


    fbIsSimulation   = kFALSE;
    fbIsDebug        = kFALSE;
    fbFillMdc        = kFALSE;
    fbgoodSeg0       = kFALSE;
    fbgoodSeg1       = kFALSE;
    fbgoodMeta       = kFALSE;
    fbgoodRK         = kFALSE;
    fbgoodLepton     = kFALSE;
    fmomSwitch       = Particle::kMomRK;
    fsortSwitch      = 0; // metaquality
    fbdoRichAlign    = kTRUE;
    fbdoRichVertexCorr=kTRUE;
    fbdoMETAQANorm    =kTRUE;
    fbdoMomentumCorr  =kTRUE;
    fbdoPathLengthCorr=kTRUE;
    fbdoGeantAcceptance=kTRUE;
    fbnoFake          =kFALSE;

    fMinWireGoodTrack=   5;
    fScaleGhostTrack = 0.1;
    fScaleGoodTrack  = 3.0;
    fAngleCloseTrack = 15.;

    all_candidates.resize( 2000, 0 );  // increase capacity to avoid later copying of data
    all_candidates.clear();            // capacity is still large

}



Bool_t HParticleCandFiller::init (void)
{
    // Get Categories and Parameters etc....

    HEvent *ev = gHades->getCurrentEvent ();
    if (ev) {

	fCatGeantKine = ev->getCategory (catGeantKine);
	if (fCatGeantKine) { fbIsSimulation = kTRUE; }
	else               { fbIsSimulation = kFALSE;}

	if(!(fCatMetaMatch  = HCategoryManager::getCategory(catMetaMatch ,kFALSE,"catMetaMatch" ))) return kFALSE;
	if(!(fCatMdcTrkCand = HCategoryManager::getCategory(catMdcTrkCand,kFALSE,"catMdcTrkCand"))) return kFALSE;
	if(!(fCatMdcSeg     = HCategoryManager::getCategory(catMdcSeg    ,kFALSE,"catMdcSeg"    ))) return kFALSE;

	fCatTofHit     = HCategoryManager::getCategory(catTofHit      ,kTRUE,"catTofHit");
	fCatTofCluster = HCategoryManager::getCategory(catTofCluster  ,kTRUE,"catTofCluster");
	fCatRichHit    = HCategoryManager::getCategory(catRichHit     ,kTRUE,"catRichHit");
	fCatRpcCluster = HCategoryManager::getCategory(catRpcCluster  ,kTRUE,"catRpcCluster");
	fCatSpline     = HCategoryManager::getCategory(catSplineTrack ,kTRUE,"catSplineTrack");
	if(fmomSwitch==Particle::kMomRK)    fCatRK         = HCategoryManager::getCategory(catRKTrackB    ,kTRUE,"catRKTrackB");
	if(fmomSwitch==Particle::kMomKalman)fCatKalman     = HCategoryManager::getCategory(catKalTrack    ,kTRUE,"catKalTrack");

	fCatShowerHit = HCategoryManager::getCategory(catShowerHit,kTRUE,"catShowerHit");
	fCatEmcCluster= HCategoryManager::getCategory(catEmcCluster,kTRUE,"catEmcCluster");

	if (fbIsSimulation) {
	    fCatParticleCand = HCategoryManager::addCategory(catParticleCand,"HParticleCandSim",5000,"Particle");
	} else {
	    fCatParticleCand = HCategoryManager::addCategory(catParticleCand,"HParticleCand"   ,5000,"Particle");
	}
	if (!fCatParticleCand) { return kFALSE; }

	if (fbIsDebug) {
	    fCatParticleDebug = HCategoryManager::addCategory(catParticleDebug,"candidate",5000,"Particle",kTRUE);
	    if (!fCatParticleDebug) { return kFALSE; }
	}
	if (fbFillMdc) {
	    fCatParticleMdc = HCategoryManager::addCategory(catParticleMdc,"HParticleMdc",5000,"Particle",kTRUE);
	    if (!fCatParticleMdc) { return kFALSE; }
	}



	fMetaMatchIter = (HIterator*) fCatMetaMatch->MakeIterator("native");
	if(!fMetaMatchIter){
	    Error ("init()", "Retrieve ZERO pointer for MetaMatch iter!");
	    return kFALSE;
	}

	fFillerPar =(HParticleCandFillerPar*) gHades->getRuntimeDb()->getContainer("ParticleCandFillerPar");
	if(!fFillerPar) {
 	    Error ("init()", "Retrieve ZERO pointer for HParticleCandFillerPar!");
	    return kFALSE;
	}

	fTofWalkPar =(HTofWalkPar*) gHades->getRuntimeDb()->getContainer("TofWalkPar");
	if(!fTofWalkPar) {
 	    Error ("init()", "Retrieve ZERO pointer for HTofWalkPar!");
	    return kFALSE;
	}

	fSizesCells = (HMdcSizesCells*)HMdcSizesCells::getObject();
	if(!fSizesCells)
	{
 	    Error ("init()", "Retrieve ZERO pointer for HMdcSizesCells!");
            return kFALSE;
	}

    } else {
	Error ("init()", "Retrieve ZERO pointer for fMetaMatchIter!");
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HParticleCandFiller::reinit(void)
{
    Bool_t result = fSizesCells->initContainer();

    cropLay = 0;
    if(result){
	cropLay = new HMdcLayer();

	cropLay->fillRKPlane    (fSizesCells);
	cropLay->fillLayerPoints(fSizesCells,kFALSE);
	gHades->addObjectToOutput(cropLay);
	//---------------------------------------------------

    }

    return result;
}

Int_t HParticleCandFiller::execute (void)
{
    // loop on HMetaMatch2 objects and fill an intermediate
    // working candidate object. Selections are performed on this
    // objects. All objects which are flagged to be used are
    // transported to the output HParticleCand category.
    clearVector();

    HMetaMatch2* metaMatch = 0;
    fMetaMatchIter->Reset();
    while( (metaMatch = (HMetaMatch2*)fMetaMatchIter->Next()) != 0){
	fillCand(metaMatch);
    }
    fillSingleProperties();
    fillCollectiveProperties();
    fillOutput();
    clearVector();
    fillGeantAcceptance();
    return 0;
}

Bool_t HParticleCandFiller::finalize (void)
{
    // jobs to be done after last event is finished.
    return kTRUE;
}

Int_t  HParticleCandFiller::findBestRich(HMetaMatch2* meta,HMdcSeg* mdcSeg1 )
{
    // returns the best rich slot in HMetaMatch2
    // corrections for RICH phi+theta are taken into
    // account


    if(!meta || !mdcSeg1 || !fCatRichHit || !fFillerPar) return -1;

    Int_t nrich = meta->getNCandForRich();
    Int_t slot  = -1;
    if(nrich >= 1){
        Float_t zVertex        = gHades->getCurrentEvent()->getHeader()->getVertexZ();
        HVertex& vertexClust   = gHades->getCurrentEvent()->getHeader()->getVertexCluster();

	Float_t chi2Clust = vertexClust.getChi2(); // -1 = no vertex found , 0 = ok


	Double_t qabest   = 100000;
        Double_t qa       = 0;

        HRichHit* richHit  = 0;
	for(Int_t i = 0; i < nrich; i ++){

	    richHit      = HCategoryManager::getObject(richHit   ,fCatRichHit   ,meta->getARichInd(i));

	    Double_t richCorr    = 0.;
            Double_t theta       = richHit->getTheta();
            Double_t phi         = richHit->getPhi();                                     // deg , lab
	    if(fbdoRichVertexCorr && chi2Clust > -0.5 && zVertex > -999.9F && zVertex < 100 ) richCorr = fFillerPar->getRichCorr(zVertex,theta,phi); // vertex z [mm] . theta [deg]
	    theta += richCorr;                                                      // first correct z position emission

	    Double_t thetaCor = theta;
            Double_t phiCor   = phi;

	    if(!fbIsSimulation && fbdoRichAlign){                                         // only align for real data
		HParticleAngleCor::alignRichRing(theta,phi,thetaCor,phiCor);              // second correct theta/phi
	    }
            qa = HParticleTool::calcRichQA(mdcSeg1, (Float_t) thetaCor,(Float_t) phiCor); // richTheta and richPhi in deg in lab system


	    if(qa >= 0 && qa < qabest) {
		slot   = i;
		qabest = qa;
	    }

	} // end richhit cand
    }

    return slot;

}


void HParticleCandFiller::fillCandNoMeta(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num)
{

    HMdcTrkCand*  mdcTrkCand = 0;
    HMdcSeg*      mdcSeg1    = 0;
    HMdcSeg*      mdcSeg2    = 0;
    HTofCluster*  tofClst    = 0;
    HTofHit*      tofHit1    = 0;
    HTofHit*      tofHit2    = 0;
    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;
    HRpcCluster*  rpcClst    = 0;
    HRichHit*     richHit    = 0;
    HSplineTrack* spline     = 0;
    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;

    cand.rkSuccess = rkSuccess;
    cand.nCand     = num;
    //--------------------------------------------------------------------
    // fill common info
    cand           .fillMeta(meta);
    cand.mdctrk    .fillMeta(meta);
    cand.spline    .fillMeta(meta);

    mdcTrkCand = HCategoryManager::getObject(mdcTrkCand,fCatMdcTrkCand,cand.mdctrk.ind);
    if(mdcTrkCand){
	cand.seg1.ind  = mdcTrkCand->getSeg1Ind();
	cand.seg2.ind  = mdcTrkCand->getSeg2Ind();
    }
    mdcSeg1    = HCategoryManager::getObject(mdcSeg1   ,fCatMdcSeg    ,cand.seg1.ind);
    mdcSeg2    = HCategoryManager::getObject(mdcSeg2   ,fCatMdcSeg    ,cand.seg2.ind);
    spline     = HCategoryManager::getObject(spline    ,fCatSpline    ,cand.spline.ind);

    Int_t slot = findBestRich(meta,mdcSeg1);
    cand.richhit.fillMeta(meta,slot);    // set richindex : only best matched rich ?
    richHit    = HCategoryManager::getObject(richHit   ,fCatRichHit   ,cand.richhit.ind);


    cand.mdctrk    .fill(mdcTrkCand);
    cand.seg1      .fill(mdcSeg1);
    cand.seg2      .fill(mdcSeg2);
    cand.spline    .fill(spline);
    cand.richhit   .fill(richHit);    // get all rich vars
    //--------------------------------------------------------------------

    cand.system = -1;
    cand.selectTof    = kNoUse;
    cand.usedMeta     = kNoUse;
    cand.rk.selectTof = kNoUse;
    cand.rk.usedMeta  = kNoUse;
    cand.kal.selectTof = kNoUse;
    cand.kal.usedMeta  = kNoUse;

    cand.objects.reset();
    cand.objects.pMdcTrk    =  mdcTrkCand;
    cand.objects.pSeg1      = (HMdcSegSim* )    mdcSeg1;
    cand.objects.pSeg2      = (HMdcSegSim* )    mdcSeg2;
    cand.objects.pRichHit   = (HRichHitSim*)    richHit;
    cand.objects.pTofClst   = (HTofClusterSim*) tofClst;
    cand.objects.pTofHit1   = (HTofHitSim*)     tofHit1;
    cand.objects.pTofHit2   = (HTofHitSim*)     tofHit2;
    cand.objects.pRpcClst   = (HRpcClusterSim*) rpcClst;
    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
    cand.objects.pEmcClst   = (HEmcClusterSim*) emcClst;
    cand.objects.pSpline    = spline;

    if     (fmomSwitch == Particle::kMomRK    ) {
	cand.rk.ind = meta->getRungeKuttaInd();
	rk = HCategoryManager::getObject(rk,fCatRK,cand.rk.ind);
	cand.rk.fill(rk);
	cand.objects.pRk = rk;
    }
    else if(fmomSwitch == Particle::kMomKalman) {
	cand.kal.ind = meta->getKalmanFilterInd();
	kal = HCategoryManager::getObject(kal,fCatKalman,cand.kal.ind);
	cand.kal.fill(kal);
	cand.objects.pKalman = kal;
    }
    else   {Error("fillCandNoMeta","Unknown momentum option");}

}

void  HParticleCandFiller::fillCandTof(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num, Int_t n)
{
    HMdcTrkCand*  mdcTrkCand = 0;
    HMdcSeg*      mdcSeg1    = 0;
    HMdcSeg*      mdcSeg2    = 0;
    HTofCluster*  tofClst    = 0;
    HTofHit*      tofHit1    = 0;
    HTofHit*      tofHit2    = 0;
    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;
    HRpcCluster*  rpcClst    = 0;
    HRichHit*     richHit    = 0;
    HSplineTrack* spline     = 0;
    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;

    cand.rkSuccess = rkSuccess;
    cand.nCand     = num;
    //--------------------------------------------------------------------
    // fill common info
    cand           .fillMeta(meta);
    cand.mdctrk    .fillMeta(meta);
    cand.spline    .fillMeta(meta);

    mdcTrkCand = HCategoryManager::getObject(mdcTrkCand,fCatMdcTrkCand,cand.mdctrk.ind);
    if(mdcTrkCand){
	cand.seg1.ind  = mdcTrkCand->getSeg1Ind();
	cand.seg2.ind  = mdcTrkCand->getSeg2Ind();
    }
    mdcSeg1    = HCategoryManager::getObject(mdcSeg1   ,fCatMdcSeg    ,cand.seg1.ind);
    mdcSeg2    = HCategoryManager::getObject(mdcSeg2   ,fCatMdcSeg    ,cand.seg2.ind);
    spline     = HCategoryManager::getObject(spline    ,fCatSpline    ,cand.spline.ind);

    Int_t slot = findBestRich(meta,mdcSeg1);
    cand.richhit.fillMeta(meta,slot);    // set richindex : only best matched rich ?
    richHit    = HCategoryManager::getObject(richHit   ,fCatRichHit   ,cand.richhit.ind);


    cand.mdctrk    .fill(mdcTrkCand);
    cand.seg1      .fill(mdcSeg1);
    cand.seg2      .fill(mdcSeg2);
    cand.spline    .fill(spline);
    cand.richhit   .fill(richHit);    // get all rich vars
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // fill META hits
    cand.tof[kTofClst].fillMeta(meta,n,kTofClst);
    cand.tof[kTofHit1].fillMeta(meta,n,kTofHit1);
    cand.tof[kTofHit2].fillMeta(meta,n,kTofHit2);

    tofClst   = HCategoryManager::getObject(tofClst   ,fCatTofCluster,cand.tof[kTofClst].ind);
    tofHit1   = HCategoryManager::getObject(tofHit1   ,fCatTofHit    ,cand.tof[kTofHit1].ind);
    tofHit2   = HCategoryManager::getObject(tofHit2   ,fCatTofHit    ,cand.tof[kTofHit2].ind);

    cand.tof[kTofClst].fill(tofClst,kTofClst);
    cand.tof[kTofHit1].fill(tofHit1,kTofHit1);
    cand.tof[kTofHit2].fill(tofHit2,kTofHit2);
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // define system for this candidate
    cand.system =  1;
    cand.selectTof    = kNoUse;
    cand.usedMeta     = kNoUse;
    cand.rk.usedMeta  = kNoUse;
    cand.rk.selectTof = kNoUse;
    cand.kal.usedMeta  = kNoUse;
    cand.kal.selectTof = kNoUse;
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // make decision what to fill
    // for rk. For different cases some preselection
    // has to be done


    //--------------------------------------------------------------------
    // selection from Meta
    Float_t    tofqua[3] = {-1,-1,-1};
    Float_t minTof   = 1e37;
    Int_t minIndMeta = kNoUse;

    if(tofClst||tofHit1||tofHit2) {

	// find best match TOF
	for(Int_t i = 0; i < 3; i ++) {
	    tofqua[i] = cand.tof[i].quality;
	    if (tofqua[i] >= 0 && tofqua[i] < minTof){ minTof = tofqua[i]; minIndMeta = i;}
	}
	cand.selectTof = minIndMeta; // best TOF
    }
    cand.usedMeta = minIndMeta;
    //--------------------------------------------------------------------

    cand.objects.reset();

    cand.objects.pMdcTrk    =  mdcTrkCand;
    cand.objects.pSeg1      = (HMdcSegSim* )    mdcSeg1;
    cand.objects.pSeg2      = (HMdcSegSim* )    mdcSeg2;
    cand.objects.pRichHit   = (HRichHitSim*)    richHit;
    cand.objects.pTofClst   = (HTofClusterSim*) tofClst;
    cand.objects.pTofHit1   = (HTofHitSim*)     tofHit1;
    cand.objects.pTofHit2   = (HTofHitSim*)     tofHit2;
    cand.objects.pRpcClst   = (HRpcClusterSim*) rpcClst;
    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
    cand.objects.pEmcClst   = (HEmcClusterSim*) emcClst;
    cand.objects.pSpline    = spline;

    //--------------------------------------------------------------------
    if(!rkSuccess){ // RK failed
	cand.rk.ind       = meta->getRungeKuttaInd();
	cand.rk.usedMeta  = kNoUse;
        cand.kal.ind      = meta->getKalmanFilterInd();
	cand.kal.usedMeta = kNoUse;

    } else {
	if(fmomSwitch == Particle::kMomRK    )
	{
	    //--------------------------------------------------------------------
	    // vars for tof selection with rk
	    HRKTrackB* rkhit [3] = { 0, 0, 0};
	    Float_t    rkqua [3] = {-1,-1,-1};
	    Float_t min  = 1e37;
	    Int_t minInd = kNoUse;

	    //--------------------------------------------------------------------

	    //--------------------------------------------------------------------
	    // select tof hits
	    // Strategy : select by best quality of rk matching
	    rkhit[kTofClst] = tofClst ? HCategoryManager::getObject(rkhit[kTofClst],fCatRK,meta->getRungeKuttaIndTofClst(n)): 0 ;
	    rkhit[kTofHit1] = tofHit1 ? HCategoryManager::getObject(rkhit[kTofHit1],fCatRK,meta->getRungeKuttaIndTofHit1(n)): 0 ;
	    rkhit[kTofHit2] = tofHit2 ? HCategoryManager::getObject(rkhit[kTofHit2],fCatRK,meta->getRungeKuttaIndTofHit2(n)): 0 ;

	    if(fsortSwitch==0){
		if(tofClst && rkhit[kTofClst]) { rkqua[kTofClst] = rkhit[kTofClst]->getQualityTof();}
		if(tofHit1 && rkhit[kTofHit1]) { rkqua[kTofHit1] = rkhit[kTofHit1]->getQualityTof();}
		if(tofHit2 && rkhit[kTofHit2]) { rkqua[kTofHit2] = rkhit[kTofHit2]->getQualityTof();}
	    } else if (fsortSwitch==1) {
		if(tofClst && rkhit[kTofClst]) { rkqua[kTofClst] = rkhit[kTofClst]->getMetaRadius();}
		if(tofHit1 && rkhit[kTofHit1]) { rkqua[kTofHit1] = rkhit[kTofHit1]->getMetaRadius();}
		if(tofHit2 && rkhit[kTofHit2]) { rkqua[kTofHit2] = rkhit[kTofHit2]->getMetaRadius();}
	    } else   { Error("fillCandTof","Unknown sort option");}

	    // find best match
	    for(Int_t i = 0; i < 3; i ++) {
		if (rkqua[i] > 1e37 ) rkqua[i] = 1e36;
		if (rkqua[i] >= 0 && rkqua[i] < min){ min = rkqua[i]; minInd = i;}
	    }
	    cand.rk.selectTof = minInd; // best TOF
	    //--------------------------------------------------------------------

	    //--------------------------------------------------------------------

	    if     (minInd == kTofClst) { cand.rk.ind = meta->getRungeKuttaIndTofClst(n); cand.rk.usedMeta = kTofClst;}
	    else if(minInd == kTofHit1) { cand.rk.ind = meta->getRungeKuttaIndTofHit1(n); cand.rk.usedMeta = kTofHit1;}
	    else if(minInd == kTofHit2) { cand.rk.ind = meta->getRungeKuttaIndTofHit2(n); cand.rk.usedMeta = kTofHit2;}
	    else {
		Error("fillCand()","(system == 1) No valid rk object!");
	    }
	    //--------------------------------------------------------------------

	}
	else if(fmomSwitch == Particle::kMomKalman)
	{
	    //--------------------------------------------------------------------
	    // vars for tof selection with rk
	    HKalTrack* rkhit [3] = { 0, 0, 0};
	    Float_t    rkqua [3] = {-1,-1,-1};
	    Float_t min  = 1e37;
	    Int_t minInd = kNoUse;

	    //--------------------------------------------------------------------

	    //--------------------------------------------------------------------
	    // select tof hits
	    // Strategy : select by best quality of rk matching
	    rkhit[kTofClst] = tofClst ? HCategoryManager::getObject(rkhit[kTofClst],fCatKalman,meta->getKalmanFilterIndTofClst(n)): 0 ;
	    rkhit[kTofHit1] = tofHit1 ? HCategoryManager::getObject(rkhit[kTofHit1],fCatKalman,meta->getKalmanFilterIndTofHit1(n)): 0 ;
	    rkhit[kTofHit2] = tofHit2 ? HCategoryManager::getObject(rkhit[kTofHit2],fCatKalman,meta->getKalmanFilterIndTofHit2(n)): 0 ;

	    if(fsortSwitch==0){
		if(tofClst && rkhit[kTofClst]) { rkqua[kTofClst] = rkhit[kTofClst]->getQualityTof();}
		if(tofHit1 && rkhit[kTofHit1]) { rkqua[kTofHit1] = rkhit[kTofHit1]->getQualityTof();}
		if(tofHit2 && rkhit[kTofHit2]) { rkqua[kTofHit2] = rkhit[kTofHit2]->getQualityTof();}
	    } else if(fsortSwitch==1){
		if(tofClst && rkhit[kTofClst]) { rkqua[kTofClst] = rkhit[kTofClst]->getMetaRadius();}
		if(tofHit1 && rkhit[kTofHit1]) { rkqua[kTofHit1] = rkhit[kTofHit1]->getMetaRadius();}
		if(tofHit2 && rkhit[kTofHit2]) { rkqua[kTofHit2] = rkhit[kTofHit2]->getMetaRadius();}
	    } else   { Error("fillCandTof","Unknown sort option");}
	    // find best match
	    for(Int_t i = 0; i < 3; i ++) {
		if (rkqua[i] > 1e37 ) rkqua[i] = 1e36;
		if (rkqua[i] >= 0 && rkqua[i] < min){ min = rkqua[i]; minInd = i;}
	    }
	    cand.kal.selectTof = minInd; // best TOF
	    //--------------------------------------------------------------------

	    //--------------------------------------------------------------------

	    if     (minInd == kTofClst) { cand.kal.ind = meta->getKalmanFilterIndTofClst(n); cand.kal.usedMeta = kTofClst;}
	    else if(minInd == kTofHit1) { cand.kal.ind = meta->getKalmanFilterIndTofHit1(n); cand.kal.usedMeta = kTofHit1;}
	    else if(minInd == kTofHit2) { cand.kal.ind = meta->getKalmanFilterIndTofHit2(n); cand.kal.usedMeta = kTofHit2;}
	    else {
		Error("fillCand()","(system == 1) No valid kaltrack object!");
	    }
	    //--------------------------------------------------------------------

	} else   {
	    Error("fillCandTof","Unknown momentum option");

	}
    }

    if(fmomSwitch == Particle::kMomRK)
    {
	//--------------------------------------------------------------------
	rk = HCategoryManager::getObject(rk,fCatRK,cand.rk.ind);
	cand.rk.fill(rk);
	cand.objects.pRk    = rk;
	//--------------------------------------------------------------------
    }    else if(fmomSwitch == Particle::kMomKalman) {
	//--------------------------------------------------------------------
	kal = HCategoryManager::getObject(kal,fCatKalman,cand.kal.ind);
	cand.kal.fill(kal);
	cand.objects.pKalman = kal;
	//--------------------------------------------------------------------
    } else   {
	Error("fillCandTof","Unknown momentum option");
    }

    

}

void  HParticleCandFiller::fillCandRpc(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num, Int_t n)
{

    HMdcTrkCand*  mdcTrkCand = 0;
    HMdcSeg*      mdcSeg1    = 0;
    HMdcSeg*      mdcSeg2    = 0;
    HTofCluster*  tofClst    = 0;
    HTofHit*      tofHit1    = 0;
    HTofHit*      tofHit2    = 0;
    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;
    HRpcCluster*  rpcClst    = 0;
    HRichHit*     richHit    = 0;
    HSplineTrack* spline     = 0;
    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;

    cand.rkSuccess = rkSuccess;
    cand.nCand     = num;
    //--------------------------------------------------------------------
    // fill common info
    cand           .fillMeta(meta);
    cand.mdctrk    .fillMeta(meta);
    cand.spline    .fillMeta(meta);

    mdcTrkCand = HCategoryManager::getObject(mdcTrkCand,fCatMdcTrkCand,cand.mdctrk.ind);
    if(mdcTrkCand){
	cand.seg1.ind  = mdcTrkCand->getSeg1Ind();
	cand.seg2.ind  = mdcTrkCand->getSeg2Ind();
    }
    mdcSeg1    = HCategoryManager::getObject(mdcSeg1   ,fCatMdcSeg    ,cand.seg1.ind);
    mdcSeg2    = HCategoryManager::getObject(mdcSeg2   ,fCatMdcSeg    ,cand.seg2.ind);
    spline     = HCategoryManager::getObject(spline    ,fCatSpline    ,cand.spline.ind);

    Int_t slot = findBestRich(meta,mdcSeg1);
    cand.richhit.fillMeta(meta,slot);    // set richindex : only best matched rich ?
    richHit    = HCategoryManager::getObject(richHit   ,fCatRichHit   ,cand.richhit.ind);


    cand.mdctrk    .fill(mdcTrkCand);
    cand.seg1      .fill(mdcSeg1);
    cand.seg2      .fill(mdcSeg2);
    cand.spline    .fill(spline);
    cand.richhit   .fill(richHit);    // get all rich vars
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // fill META hits
    cand.rpcclst      .fillMeta(meta,n);   // automatic match between best RPC and SHOWER ?
    rpcClst   = HCategoryManager::getObject(rpcClst   ,fCatRpcCluster,cand.rpcclst.ind);
    cand.rpcclst      .fill(rpcClst);
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // define system for this candidate
    cand.system = 0;
    cand.selectTof    = kRpcClst;
    cand.usedMeta     = kRpcClst;

    if(rkSuccess) {
	cand.rk.usedMeta  = kRpcClst;
	cand.rk.selectTof = kRpcClst;
	cand.rk.ind       = meta->getRungeKuttaIndRpcClst(n);
	cand.kal.usedMeta  = kRpcClst;
	cand.kal.selectTof = kRpcClst;
	cand.kal.ind       = meta->getKalmanFilterIndRpcClst(n);

    } else {
	cand.rk.usedMeta  = kNoUse;
	cand.rk.selectTof = kNoUse;
	cand.rk.ind       = meta->getRungeKuttaInd();
	cand.kal.usedMeta  = kNoUse;
	cand.kal.selectTof = kNoUse;
	cand.kal.ind       = meta->getKalmanFilterInd();
    }
    //--------------------------------------------------------------------

    cand.objects.reset();

    cand.objects.pMdcTrk    =  mdcTrkCand;
    cand.objects.pSeg1      = (HMdcSegSim* )    mdcSeg1;
    cand.objects.pSeg2      = (HMdcSegSim* )    mdcSeg2;
    cand.objects.pRichHit   = (HRichHitSim*)    richHit;
    cand.objects.pTofClst   = (HTofClusterSim*) tofClst;
    cand.objects.pTofHit1   = (HTofHitSim*)     tofHit1;
    cand.objects.pTofHit2   = (HTofHitSim*)     tofHit2;
    cand.objects.pRpcClst   = (HRpcClusterSim*) rpcClst;
    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
    cand.objects.pEmcClst   = (HEmcClusterSim*) emcClst;
    cand.objects.pSpline    = spline;

    if(fmomSwitch == Particle::kMomRK)
    {
	//--------------------------------------------------------------------
	rk = HCategoryManager::getObject(rk,fCatRK,cand.rk.ind);
	cand.rk.fill(rk);
	cand.objects.pRk    = rk;
	//--------------------------------------------------------------------
    }    else if(fmomSwitch == Particle::kMomKalman) {
	//--------------------------------------------------------------------
	kal = HCategoryManager::getObject(kal,fCatKalman,cand.kal.ind);
	cand.kal.fill(kal);
	cand.objects.pKalman = kal;
	//--------------------------------------------------------------------
    } else   {
	Error("fillCandRpc","Unknown momentum option");
    }

}

void  HParticleCandFiller::fillCandShower(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num, Int_t n)
{
    HMdcTrkCand*  mdcTrkCand = 0;
    HMdcSeg*      mdcSeg1    = 0;
    HMdcSeg*      mdcSeg2    = 0;
    HTofCluster*  tofClst    = 0;
    HTofHit*      tofHit1    = 0;
    HTofHit*      tofHit2    = 0;
    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;
    HRpcCluster*  rpcClst    = 0;
    HRichHit*     richHit    = 0;
    HSplineTrack* spline     = 0;
    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;

    cand.rkSuccess = rkSuccess;
    cand.nCand     = num;
    //--------------------------------------------------------------------
    // fill common info
    cand           .fillMeta(meta);
    cand.mdctrk    .fillMeta(meta);
    cand.spline    .fillMeta(meta);

    mdcTrkCand = HCategoryManager::getObject(mdcTrkCand,fCatMdcTrkCand,cand.mdctrk.ind);
    if(mdcTrkCand){
	cand.seg1.ind  = mdcTrkCand->getSeg1Ind();
	cand.seg2.ind  = mdcTrkCand->getSeg2Ind();
    }
    mdcSeg1    = HCategoryManager::getObject(mdcSeg1   ,fCatMdcSeg    ,cand.seg1.ind);
    mdcSeg2    = HCategoryManager::getObject(mdcSeg2   ,fCatMdcSeg    ,cand.seg2.ind);
    spline     = HCategoryManager::getObject(spline    ,fCatSpline    ,cand.spline.ind);

    Int_t slot = findBestRich(meta,mdcSeg1);
    cand.richhit.fillMeta(meta,slot);    // set richindex : only best matched rich ?
    richHit    = HCategoryManager::getObject(richHit   ,fCatRichHit   ,cand.richhit.ind);


    cand.mdctrk    .fill(mdcTrkCand);
    cand.seg1      .fill(mdcSeg1);
    cand.seg2      .fill(mdcSeg2);
    cand.spline    .fill(spline);
    cand.richhit   .fill(richHit);    // get all rich vars
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // fill META hits
    cand.showerhit    .fillMeta(meta,n);
    showerHit = HCategoryManager::getObject(showerHit ,fCatShowerHit ,cand.showerhit.ind);
    cand.showerhit    .fill(showerHit);
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // define system for this candidate
    cand.system = 0;
    cand.selectTof    = kShowerHit;
    cand.usedMeta     = kShowerHit;

    if(rkSuccess) {
	cand.rk.usedMeta  = kShowerHit;
	cand.rk.selectTof = kShowerHit;
	cand.rk.ind       = meta->getRungeKuttaIndShowerHit(n);
	cand.kal.usedMeta  = kShowerHit;
	cand.kal.selectTof = kShowerHit;
	cand.kal.ind       = meta->getKalmanFilterIndShowerHit(n);

    } else {
	cand.rk.usedMeta  = kNoUse;
	cand.rk.selectTof = kNoUse;
	cand.rk.ind       = meta->getRungeKuttaInd();
	cand.kal.usedMeta  = kNoUse;
	cand.kal.selectTof = kNoUse;
	cand.kal.ind       = meta->getKalmanFilterInd();
    }
    //--------------------------------------------------------------------

    cand.objects.reset();

    cand.objects.pMdcTrk    =  mdcTrkCand;
    cand.objects.pSeg1      = (HMdcSegSim* )    mdcSeg1;
    cand.objects.pSeg2      = (HMdcSegSim* )    mdcSeg2;
    cand.objects.pRichHit   = (HRichHitSim*)    richHit;
    cand.objects.pTofClst   = (HTofClusterSim*) tofClst;
    cand.objects.pTofHit1   = (HTofHitSim*)     tofHit1;
    cand.objects.pTofHit2   = (HTofHitSim*)     tofHit2;
    cand.objects.pRpcClst   = (HRpcClusterSim*) rpcClst;
    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
    cand.objects.pEmcClst   = (HEmcClusterSim*) emcClst;
    cand.objects.pSpline    = spline;

    if(fmomSwitch == Particle::kMomRK)
    {
	//--------------------------------------------------------------------
	rk = HCategoryManager::getObject(rk,fCatRK,cand.rk.ind);
	cand.rk.fill(rk);
	cand.objects.pRk    = rk;
	//--------------------------------------------------------------------
    }    else if(fmomSwitch == Particle::kMomKalman) {
	//--------------------------------------------------------------------
	kal = HCategoryManager::getObject(kal,fCatKalman,cand.kal.ind);
	cand.kal.fill(kal);
	cand.objects.pKalman = kal;
	//--------------------------------------------------------------------
    } else   {
	Error("fillCandRpc","Unknown momentum option");
    }

}

void  HParticleCandFiller::fillCandEmc(Bool_t rkSuccess,HMetaMatch2* meta,candidate& cand,Int_t num, Int_t n)
{
    HMdcTrkCand*  mdcTrkCand = 0;
    HMdcSeg*      mdcSeg1    = 0;
    HMdcSeg*      mdcSeg2    = 0;
    HTofCluster*  tofClst    = 0;
    HTofHit*      tofHit1    = 0;
    HTofHit*      tofHit2    = 0;
    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;
    HRpcCluster*  rpcClst    = 0;
    HRichHit*     richHit    = 0;
    HSplineTrack* spline     = 0;
    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;

    cand.rkSuccess = rkSuccess;
    cand.nCand     = num;
    //--------------------------------------------------------------------
    // fill common info
    cand           .fillMeta(meta);
    cand.mdctrk    .fillMeta(meta);
    cand.spline    .fillMeta(meta);

    mdcTrkCand = HCategoryManager::getObject(mdcTrkCand,fCatMdcTrkCand,cand.mdctrk.ind);
    if(mdcTrkCand){
	cand.seg1.ind  = mdcTrkCand->getSeg1Ind();
	cand.seg2.ind  = mdcTrkCand->getSeg2Ind();
    }
    mdcSeg1    = HCategoryManager::getObject(mdcSeg1   ,fCatMdcSeg    ,cand.seg1.ind);
    mdcSeg2    = HCategoryManager::getObject(mdcSeg2   ,fCatMdcSeg    ,cand.seg2.ind);
    spline     = HCategoryManager::getObject(spline    ,fCatSpline    ,cand.spline.ind);

    Int_t slot = findBestRich(meta,mdcSeg1);
    cand.richhit.fillMeta(meta,slot);    // set richindex : only best matched rich ?
    richHit    = HCategoryManager::getObject(richHit   ,fCatRichHit   ,cand.richhit.ind);


    cand.mdctrk    .fill(mdcTrkCand);
    cand.seg1      .fill(mdcSeg1);
    cand.seg2      .fill(mdcSeg2);
    cand.spline    .fill(spline);
    cand.richhit   .fill(richHit);    // get all rich vars
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // fill META hits
    cand.emcclst    .fillMeta(meta,n);
    emcClst = HCategoryManager::getObject(emcClst ,fCatEmcCluster ,cand.emcclst.ind);
    cand.emcclst    .fill(emcClst);
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // define system for this candidate
    cand.system = 0;
    cand.selectTof    = kEmcClst;
    cand.usedMeta     = kEmcClst;

    if(rkSuccess) {
	cand.rk.usedMeta  = kEmcClst;
	cand.rk.selectTof = kEmcClst;
	cand.rk.ind       = meta->getRungeKuttaIndEmcCluster(n);
	cand.kal.usedMeta  = kShowerHit;
	cand.kal.selectTof = kShowerHit;
	cand.kal.ind       = meta->getKalmanFilterIndEmcCluster(n);

    } else {
	cand.rk.usedMeta  = kNoUse;
	cand.rk.selectTof = kNoUse;
	cand.rk.ind       = meta->getRungeKuttaInd();
	cand.kal.usedMeta  = kNoUse;
	cand.kal.selectTof = kNoUse;
	cand.kal.ind       = meta->getKalmanFilterInd();
    }
    //--------------------------------------------------------------------

    cand.objects.reset();

    cand.objects.pMdcTrk    =  mdcTrkCand;
    cand.objects.pSeg1      = (HMdcSegSim* )    mdcSeg1;
    cand.objects.pSeg2      = (HMdcSegSim* )    mdcSeg2;
    cand.objects.pRichHit   = (HRichHitSim*)    richHit;
    cand.objects.pTofClst   = (HTofClusterSim*) tofClst;
    cand.objects.pTofHit1   = (HTofHitSim*)     tofHit1;
    cand.objects.pTofHit2   = (HTofHitSim*)     tofHit2;
    cand.objects.pRpcClst   = (HRpcClusterSim*) rpcClst;
    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
    cand.objects.pEmcClst   = (HEmcClusterSim*) emcClst;
    cand.objects.pSpline    = spline;

    if(fmomSwitch == Particle::kMomRK)
    {
	//--------------------------------------------------------------------
	rk = HCategoryManager::getObject(rk,fCatRK,cand.rk.ind);
	cand.rk.fill(rk);
	cand.objects.pRk    = rk;
	//--------------------------------------------------------------------
    }    else if(fmomSwitch == Particle::kMomKalman) {
	//--------------------------------------------------------------------
	kal = HCategoryManager::getObject(kal,fCatKalman,cand.kal.ind);
	cand.kal.fill(kal);
	cand.objects.pKalman = kal;
	//--------------------------------------------------------------------
    } else   {
	Error("fillCandRpc","Unknown momentum option");
    }

}



void HParticleCandFiller::fillCand(HMetaMatch2* meta){
    // create candidate meta objects for all selected matches
    // in HMetaMatch2. In Simulation the GEANT information is
    // filled too. This function is the heart of the task.
    // Selections which META hit to use are performed here.

    if(fbnoFake) if(meta->isFake()) { return; }

    Bool_t isEMC = meta->isEmcCluster();

    //--------------------------------------------------------------------
    // was rk succesful ?  (rk indedx has to taken from another place ....)
    // also quality parameters ...
    // caution : meta->getRungeKuttaInd();
    //          is filled in case a.) no METAHIT b.) rk failed

    HRKTrackB*    rk         = 0;
    HKalTrack*    kal        = 0;
    Bool_t  rkSuccess    = kTRUE;
    Float_t rkchi2       = -1;

    if(fmomSwitch == Particle::kMomRK)
    {
	Int_t   rkReplaceInd = meta->getRungeKuttaInd();
	if ( (rk = HCategoryManager::getObject(rk,fCatRK,rkReplaceInd)) != 0 ) {
	    rkchi2 = rk->getChiq();
            if (rkchi2 < 0)  { rkSuccess = kFALSE; }
	}
    } else if(fmomSwitch == Particle::kMomKalman) {
	Int_t   rkReplaceInd = meta->getKalmanFilterInd();
	if ( (kal = HCategoryManager::getObject(kal,fCatKalman,rkReplaceInd)) != 0 ) {
	    rkchi2 = kal->getChi2();
            if (rkchi2 < 0)  { rkSuccess = kFALSE; }
	}
    } else   {
	Error("fillCand","Unknown momentum option");
    }
    //--------------------------------------------------------------------

    Int_t       bestShrInd  = -1;
    HShowerHit* bestShr     = 0;
    HRKTrackB*  bestRKShr   = 0;
    HKalTrack*  bestKalShr  = 0;

    Int_t       bestEmcInd  = -1;
    HEmcCluster* bestEmc    = 0;
    HRKTrackB*  bestRKEmc   = 0;
    HKalTrack*  bestKalEmc  = 0;
 
    if(isEMC){
	//--------------------------------------------------------------------
	// find best EmcCluster match
	Float_t     emcQA       = 10000.;
	HEmcCluster* hit =0;
	Float_t qa      =-1;
	for(UInt_t i=0; i < meta->getNEmcClusters();i++){
	    Int_t ind = meta->getEmcClusterInd (i);
	    hit = HCategoryManager::getObject(hit,fCatEmcCluster,ind);
	    if(hit){
		if(!rkSuccess){
		    qa=meta->getEmcClusterQuality(i);
		    if(qa>=0 && qa < emcQA) {
			emcQA      = qa;
			bestEmc    = hit;
			bestEmcInd = i;
		    }
		} else {
		    HRKTrackB* rkEmc=0;
		    HKalTrack* kalEmc=0;
		    if     (fmomSwitch == Particle::kMomRK)     {
			rkEmc  = HCategoryManager::getObject(rkEmc ,fCatRK    ,meta->getRungeKuttaIndEmcCluster(i));
			if     (fsortSwitch==0)qa=rkEmc->getQualityEmc();
			else if(fsortSwitch==1)qa=rkEmc->getMetaRadius();
			else   { Error("fillCand","Unknown sort option");}
		    }
		    else if(fmomSwitch == Particle::kMomKalman) {
			kalEmc = HCategoryManager::getObject(kalEmc,fCatKalman,meta->getKalmanFilterIndEmcCluster(i));
			if     (fsortSwitch==0)qa=kalEmc->getQualityEmc();
			else if(fsortSwitch==1)qa=kalEmc->getMetaRadius();
			else   { Error("fillCand","Unknown sort option");}
		    }
		    else   { Error("fillCand","Unknown momentum option");}

		    if(qa>=0 && qa < emcQA) {
			emcQA      = qa;
			bestEmc    = hit;
			bestEmcInd = i;
			bestRKEmc  = rkEmc;
			bestKalEmc = kalEmc;
		    }
		}
	    }
	}
	//--------------------------------------------------------------------
    } else {
	//--------------------------------------------------------------------
	// find best ShowerHit match
	Float_t     shrQA       = 10000.;
	HShowerHit* hit =0;
	Float_t qa      =-1;
	for(UInt_t i=0; i < meta->getNShrHits();i++){
	    Int_t ind = meta->getShowerHitInd (i);
	    hit = HCategoryManager::getObject(hit,fCatShowerHit,ind);
	    if(hit){
		if(!rkSuccess){
		    qa=meta->getShowerHitQuality(i);
		    if(qa>=0 && qa < shrQA) {
			shrQA      = qa;
			bestShr    = hit;
			bestShrInd = i;
		    }
		} else {
		    HRKTrackB* rkShr=0;
		    HKalTrack* kalShr=0;
		    if     (fmomSwitch == Particle::kMomRK)     {
			rkShr  = HCategoryManager::getObject(rkShr ,fCatRK    ,meta->getRungeKuttaIndShowerHit(i));
			if     (fsortSwitch==0)qa=rkShr->getQualityShower();
			else if(fsortSwitch==1)qa=rkShr->getMetaRadius();
			else   { Error("fillCand","Unknown sort option");}
		    }
		    else if(fmomSwitch == Particle::kMomKalman) {
			kalShr = HCategoryManager::getObject(kalShr,fCatKalman,meta->getKalmanFilterIndShowerHit(i));
			if     (fsortSwitch==0)qa=kalShr->getQualityShower();
			else if(fsortSwitch==1)qa=kalShr->getMetaRadius();
			else   { Error("fillCand","Unknown sort option");}
		    }
		    else   { Error("fillCand","Unknown momentum option");}

		    if(qa>=0 && qa < shrQA) {
			shrQA      = qa;
			bestShr    = hit;
			bestShrInd = i;
			bestRKShr  = rkShr;
			bestKalShr = kalShr;
		    }
		}
	    }
	}
	//--------------------------------------------------------------------
    }

    HShowerHit*   showerHit  = 0;
    HEmcCluster*  emcClst    = 0;

    //--------------------------------------------------------------------
    if(meta->getSystem()==-1)    // nothing to do for TOF,RPC,SHOWER
    {
	Int_t num = 1;
	candidate& cand = *(new candidate);
	cand.reset();
	cand.isEMC = isEMC;

	fillCandNoMeta(rkSuccess,meta,cand,num);

	//--------------------------------------------------------------------
        // simulation part
	if(fbIsSimulation){
	    fillCandSim(cand);
	}
	//--------------------------------------------------------------------

        all_candidates.push_back(&cand);
    }
    //####################################################################
    else if(meta->getSystem()==1)  // TOF case : select the best TOF for TOF hit or TOF Cluster
    {
	Int_t numTof = meta->getNTofHits();

	//--------------------------------------------------------------------
	// fill the different candiates each selected TOF row
	for(Int_t n = 0; n < numTof; n ++)
	{
	    candidate& cand = *(new candidate);
	    cand.reset();
	    cand.isEMC = isEMC;

	    fillCandTof(rkSuccess,meta,cand,numTof,n);

	    //--------------------------------------------------------------------
	    // simulation part
	    if(fbIsSimulation){
		fillCandSim(cand);
	    }
	    //--------------------------------------------------------------------

	    all_candidates.push_back(&cand);

	} // end ntof loop

    }
    //####################################################################
    else if(meta->getSystem()==0)  // RPC + SHOWER/EMC
    {
	Int_t numRpc = meta->getNRpcClusters();

	//--------------------------------------------------------------------
	// fill the different candiates for each selected RPC row
	for(Int_t n = 0; n < numRpc; n ++)
	{
	    candidate& cand = *(new candidate);
	    cand.reset();
	    cand.isEMC = isEMC;

	    fillCandRpc(rkSuccess,meta,cand,numRpc,n);
	    if(isEMC){
		if(bestEmc){
		    if     (fmomSwitch == Particle::kMomRK)     cand.emcclst     .fillMeta(meta,bestEmcInd,bestRKEmc);
		    else if(fmomSwitch == Particle::kMomKalman) cand.emcclst     .fillMetaKal(meta,bestEmcInd,bestKalEmc);
		    else   { Error("fillCand","Unknown momentum option");}
		    emcClst = HCategoryManager::getObject(emcClst ,fCatEmcCluster ,cand.emcclst.ind);
		    cand.emcclst    .fill(emcClst);
		    cand.objects.pEmcClst = (HEmcClusterSim*)  emcClst;
		}
	    } else {
		if(bestShr){
		    if     (fmomSwitch == Particle::kMomRK)     cand.showerhit    .fillMeta(meta,bestShrInd,bestRKShr);
		    else if(fmomSwitch == Particle::kMomKalman) cand.showerhit    .fillMetaKal(meta,bestShrInd,bestKalShr);
		    else   { Error("fillCand","Unknown momentum option");}
		    showerHit = HCategoryManager::getObject(showerHit ,fCatShowerHit ,cand.showerhit.ind);
		    cand.showerhit    .fill(showerHit);
		    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
		}
	    }
	    //--------------------------------------------------------------------
	    // simulation part
	    if(fbIsSimulation){
		fillCandSim(cand);
	    }
	    //--------------------------------------------------------------------

	    all_candidates.push_back(&cand);

	} // end loop RpcClst
	//--------------------------------------------------------------------

	//--------------------------------------------------------------------
	if(numRpc <= 0 && (bestShr||bestEmc)){ // if there was no RPC : fill best showerHit/emcClst
	    //--------------------------------------------------------------------
	    // fill the different candiates for each selected RPC row

	    candidate& cand = *(new candidate);
	    cand.reset();
	    cand.isEMC = isEMC;

	    if(isEMC){
		Int_t numEmc=meta->getNEmcClusters();
		fillCandEmc(rkSuccess,meta,cand,numEmc,bestEmcInd );  // RKSHOWER ?????
		if     (fmomSwitch == Particle::kMomRK)     cand.emcclst    .fillMeta(meta,bestEmcInd,bestRKEmc);
		else if(fmomSwitch == Particle::kMomKalman) cand.emcclst    .fillMetaKal(meta,bestEmcInd,bestKalEmc);
		else   { Error("fillCand","Unknown momentum option");}
	    } else {
		Int_t numShr=meta->getNShrHits();
		fillCandShower(rkSuccess,meta,cand,numShr,bestShrInd );  // RKSHOWER ?????
		if     (fmomSwitch == Particle::kMomRK)     cand.showerhit    .fillMeta(meta,bestShrInd,bestRKShr);
		else if(fmomSwitch == Particle::kMomKalman) cand.showerhit    .fillMetaKal(meta,bestShrInd,bestKalShr);
		else   { Error("fillCand","Unknown momentum option");}
	    }
	    //--------------------------------------------------------------------
	    // simulation part
	    if(fbIsSimulation){
		fillCandSim(cand);
	    }
	    //--------------------------------------------------------------------

	    all_candidates.push_back(&cand);

	}
	//--------------------------------------------------------------------


    }
    //####################################################################
    else if(meta->getSystem()==2)  //  RPC/SHOWER/EMC + TOF
    {
	Int_t numTof = meta->getNTofHits();
        Int_t numRpc = meta->getNRpcClusters();


	//--------------------------------------------------------------------
	// fill the different candiates for each selected TOF row
	for(Int_t n = 0; n < numTof; n ++)
	{
	    candidate& cand = *(new candidate);
	    cand.reset();
	    cand.isEMC = isEMC;

	    fillCandTof(rkSuccess,meta,cand,numTof+numRpc,n);
	    cand.system = 2;

	    if(isEMC){
		if(bestEmc){
		    if     (fmomSwitch == Particle::kMomRK)     cand.emcclst   .fillMeta(meta,bestEmcInd,bestRKEmc);
		    else if(fmomSwitch == Particle::kMomKalman) cand.emcclst   .fillMetaKal(meta,bestEmcInd,bestKalEmc);
		    else   { Error("fillCand","Unknown momentum option");}
		    emcClst = HCategoryManager::getObject(emcClst ,fCatEmcCluster ,cand.emcclst.ind);
		    cand.emcclst    .fill(emcClst);
		    cand.objects.pEmcClst = (HEmcClusterSim*)  emcClst;
		}
	    } else {
		if(bestShr){
		    if     (fmomSwitch == Particle::kMomRK)     cand.showerhit    .fillMeta(meta,bestShrInd,bestRKShr);
		    else if(fmomSwitch == Particle::kMomKalman) cand.showerhit    .fillMetaKal(meta,bestShrInd,bestKalShr);
		    else   { Error("fillCand","Unknown momentum option");}
		    showerHit = HCategoryManager::getObject(showerHit ,fCatShowerHit ,cand.showerhit.ind);
		    cand.showerhit    .fill(showerHit);
		    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
		}
	    }
	    //--------------------------------------------------------------------
	    // simulation part
	    if(fbIsSimulation){
		fillCandSim(cand);
	    }
	    //--------------------------------------------------------------------

	    all_candidates.push_back(&cand);

	} // end ntof loop


	//--------------------------------------------------------------------
	// fill the different candiates for each selected RPC row
	for(Int_t n = 0; n < numRpc; n ++)
	{
	    candidate& cand = *(new candidate);
	    cand.reset();
	    cand.isEMC = isEMC;

	    fillCandRpc(rkSuccess,meta,cand,numTof+numRpc,n);
	    cand.system=2;

	    if(isEMC){
		if(bestEmc){
		    if     (fmomSwitch == Particle::kMomRK)     cand.emcclst    .fillMeta(meta,bestEmcInd,bestRKEmc);
		    else if(fmomSwitch == Particle::kMomKalman) cand.emcclst    .fillMetaKal(meta,bestEmcInd,bestKalEmc);
		    else   { Error("fillCand","Unknown momentum option");}
		    emcClst = HCategoryManager::getObject(emcClst ,fCatEmcCluster ,cand.emcclst.ind);
		    cand.emcclst    .fill(emcClst);
		    cand.objects.pEmcClst = (HEmcClusterSim*) emcClst;
		}
	    } else {
		if(bestShr){
		    if     (fmomSwitch == Particle::kMomRK)     cand.showerhit    .fillMeta(meta,bestShrInd,bestRKShr);
		    else if(fmomSwitch == Particle::kMomKalman) cand.showerhit    .fillMetaKal(meta,bestShrInd,bestKalShr);
		    else   { Error("fillCand","Unknown momentum option");}
		    showerHit = HCategoryManager::getObject(showerHit ,fCatShowerHit ,cand.showerhit.ind);
		    cand.showerhit    .fill(showerHit);
		    cand.objects.pShowerHit = (HShowerHitSim*)  showerHit;
		}
	    }
	    //--------------------------------------------------------------------
	    // simulation part
	    if(fbIsSimulation){
		fillCandSim(cand);
	    }
	    //--------------------------------------------------------------------

	    all_candidates.push_back(&cand);

	} // end loop RpcClst
	//--------------------------------------------------------------------


    } else {
	Error("fillCand()","metamatch system other than -1 to 2 = %i!",meta->getSystem());
        return;
    }

}


void HParticleCandFiller::fillCandSim(candidate& cand)
{
    //--------------------------------------------------------------------
    // for simulation we have to fill HGEANT infos
    // in addition. The GEANT track infos are collected
    // in trackVec objects for each detector and for
    // all detectors together. The tracks in the common
    // vector are sorted by 3 criteria:
    // 1. number of detectors which have seen this track
    // 2. sum of weights of the track
    // 3. correlation flag
    // The best track number will be transported
    // to HParticleCandSim output object.


    Int_t tr = -1;

    pointers& objects = cand.objects;

    //--------------------------------------------------------------------
    // MDC

    //--------------------------------------------------------------------
    // search for "good tracks" (ghosts identified by MDC tracking)
    vector<Int_t> goodTracks;  // good inner+outer or inner (if no outer segment)

    Int_t nGoodInnerTracks = objects.pSeg1->getNNotFakeTracks();
    Int_t nGoodOuterTracks = 0;
    //--------------------------------------------------------------------
    // global decision on ghost done on MDC
    Int_t nwInnerCut = objects.pSeg1->getSumWires()/2 + 1;

    Int_t ghostFlag=0;
    if( nGoodInnerTracks == 0) {
	ghostFlag = kIsGhost|kIsInnerGhost ;
    } else {
	if(objects.pSeg1->getTrack(0) == gHades->getEmbeddingRealTrackId() )                          ghostFlag |= kIsGhost|kIsInnerGhost ;
	if(nwInnerCut > objects.pSeg1->getNTimes(0) || objects.pSeg1->getNTimes(0)<fMinWireGoodTrack) ghostFlag |= kIsGhost|kIsInnerGhost;
    }

    Int_t nwOuterCut = 0;
    if(objects.pSeg2){

	nGoodOuterTracks = objects.pSeg2->getNNotFakeTracks();
	if( nGoodOuterTracks == 0) {
	    ghostFlag |= kIsGhost|kIsOuterGhost ;
	} else {
	    nwOuterCut = objects.pSeg2->getSumWires()/2 + 1;
	    if(nwOuterCut > objects.pSeg2->getNTimes(0) || objects.pSeg2->getNTimes(0)<fMinWireGoodTrack) ghostFlag |= kIsGhost|kIsOuterGhost;
	    if(objects.pSeg1->getTrack(0) != objects.pSeg2->getTrack(0))                                  ghostFlag |= kIsGhost;
	    if(objects.pSeg2->getTrack(0) == gHades->getEmbeddingRealTrackId() )                          ghostFlag |= kIsGhost|kIsOuterGhost ;
	}
    }
    //--------------------------------------------------------------------

    

    for(Int_t i = 0; i < nGoodInnerTracks; i ++) {
 	tr =  objects.pSeg1->getTrack(i);
	if(objects.pSeg2) tr = objects.pSeg1->getGoodTrack(i,objects.pSeg2,fMinWireGoodTrack); // if out seg, check if it matches
	if(tr > 0 ) { // good track!
	    goodTracks.push_back(tr);
	}
    }
    //--------------------------------------------------------------------
    Int_t ghost = 0;
    if(objects.pSeg1) { //we found an inner mdc segment
	for(Int_t i = 0; i < objects.pSeg1->getNTracks(); i++) { //loop over all contributing tracks ...
	    ghost = kIsInnerGhost;
	    if(i < nGoodInnerTracks && objects.pSeg1->getNTimes(i) >= fMinWireGoodTrack) ghost = 0; // good track

	    tr = objects.pSeg1->getTrack(i);
	    if(tr < 1 && tr != gHades->getEmbeddingRealTrackId() ) continue; // skip cells which contain no valid hit
	    if(tr > 0 && find(goodTracks.begin(),goodTracks.end(),tr) == goodTracks.end()) { // ghost
		cand.mdc1Tracks.addTrack(tr,objects.pSeg1->getNTimes(i),kIsInInnerMDC|kIsGhost|ghost,fScaleGhostTrack);
	    } else {                                                                         // good or real track
                if(tr > 0 ) cand.mdc1Tracks.addTrack(tr,objects.pSeg1->getNTimes(i),kIsInInnerMDC,fScaleGoodTrack); // GEANT
                else        cand.mdc1Tracks.addTrack(tr,objects.pSeg1->getNTimes(i),kIsInInnerMDC);                 // REAL
	    }
	}
	cand.mdc1Tracks.calcWeights();

    }
    if(objects.pSeg2) { //we found an outer mdc segment

	for(Int_t i = 0; i < objects.pSeg2->getNTracks(); i++) { //loop over all contributing tracks ...
	    ghost = kIsOuterGhost;
	    if(i < nGoodOuterTracks && objects.pSeg2->getNTimes(i) >= fMinWireGoodTrack) ghost = 0; // good track

	    tr = objects.pSeg2->getTrack(i);
	    if(tr < 1 && tr != gHades->getEmbeddingRealTrackId() ) continue; // skip cells which contain no valid hit
            if(tr > 0 && find(goodTracks.begin(),goodTracks.end(),tr) == goodTracks.end()) { // ghost
		cand.mdc2Tracks.addTrack(tr,objects.pSeg2->getNTimes(i),kIsInOuterMDC|kIsGhost|ghost,fScaleGhostTrack);
	    } else {                                                                         // good or real track
                if(tr > 0 ) cand.mdc2Tracks.addTrack(tr,objects.pSeg2->getNTimes(i),kIsInOuterMDC,fScaleGoodTrack); // GEANT
                else        cand.mdc2Tracks.addTrack(tr,objects.pSeg2->getNTimes(i),kIsInOuterMDC);                // REAL
	    }
	}
	cand.mdc2Tracks.calcWeights();
    }
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // RICH
    //Check for up to three possibly contributing leptons in this ring
    if(objects.pRichHit){
	if(objects.pRichHit->weigTrack1 > 0.0 && (objects.pRichHit->track1 > 0 || objects.pRichHit->track1 == gHades->getEmbeddingRealTrackId())) {
	    cand.richTracks.addTrack(objects.pRichHit->track1,objects.pRichHit->weigTrack1,kIsInRICH);
	}
	if(objects.pRichHit->weigTrack2 > 0.0 && (objects.pRichHit->track2 > 0 || objects.pRichHit->track2 == gHades->getEmbeddingRealTrackId())) {
	    cand.richTracks.addTrack(objects.pRichHit->track2,objects.pRichHit->weigTrack2,kIsInRICH);
	}
	if(objects.pRichHit->weigTrack3 > 0.0 && (objects.pRichHit->track3 > 0 || objects.pRichHit->track3 == gHades->getEmbeddingRealTrackId())) {
	    cand.richTracks.addTrack(objects.pRichHit->track3,objects.pRichHit->weigTrack3,kIsInRICH);
	}
	cand.richTracks.calcWeights();
    }
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // META
    //
    // we have to copy the GEANT infos of the selected objects!


    //--------------------------------------------------------------------
    // RK success or failed ?
    Int_t selMetaHit = kNoUse;
    if(cand.rkSuccess) {
	if     (fmomSwitch == Particle::kMomRK)     selMetaHit = cand.rk.usedMeta;
	else if(fmomSwitch == Particle::kMomKalman) selMetaHit = cand.kal.usedMeta;
        else   Error("fillCandSim()","Unknown momswitch !");
    } else { selMetaHit = cand.usedMeta;    }  // from segment matching
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    if       (selMetaHit  == kTofClst)  {

	if(objects.pTofClst) {
	    Int_t nTr = objects.pTofClst ->getNParticipants();

	    for(Int_t i = 0; i < nTr && i < 3; i ++)
	    {
		//Get track numbers
		tr = objects.pTofClst->getNTrack1(i);
		tr = HParticleTool::findFirstHitInTof(tr,2);
                Bool_t good = kFALSE;
		if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
		    cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		    good = kTRUE;
		}
		if(objects.pTofClst->getNTrack1(i) != objects.pTofClst->getNTrack2(i) ){  // skip same track
		    tr = objects.pTofClst ->getNTrack2(i);
		    tr = HParticleTool::findFirstHitInTof(tr,2);
		    if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
			cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		    }
		} else { if (good) cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA); } // same track accepeted
		cand.tofTracks.calcWeights();
	    }
	}

	//--------------------------------------------------------------------
    } else if(selMetaHit  == kTofHit1)  {

	if(objects.pTofHit1){
	    tr = objects.pTofHit1 ->getNTrack1();
	    tr = HParticleTool::findFirstHitInTof(tr,2);
	    Bool_t good = kFALSE;
	    if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
		cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		good = kTRUE;
	    }
	    if(objects.pTofHit1 ->getNTrack1() != objects.pTofHit1 ->getNTrack2() ){  // skip same track
		tr = objects.pTofHit1 ->getNTrack2();
		tr = HParticleTool::findFirstHitInTof(tr,2);
		if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
		    cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		}
	    } else { if (good) cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA); } // same track accepeted
	    cand.tofTracks.calcWeights();
	}

	//--------------------------------------------------------------------
    } else if(selMetaHit  == kTofHit2)  {

	if(objects.pTofHit2){
	    tr = objects.pTofHit2 ->getNTrack1();
	    tr = HParticleTool::findFirstHitInTof(tr,2);
            Bool_t good = kFALSE;
	    if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
		cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		good = kTRUE;
	    }

	    if(objects.pTofHit2 ->getNTrack1() != objects.pTofHit2 ->getNTrack2() ){  // skip same track
		tr = objects.pTofHit2 ->getNTrack2();
		tr = HParticleTool::findFirstHitInTof(tr,2);
		if( tr > 0 || tr == gHades->getEmbeddingRealTrackId() ) {
		    cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA);
		}
	    } else { if (good) cand.tofTracks.addTrack(tr,1,kIsInTOF|kIsInMETA); } // same track accepeted
	    cand.tofTracks.calcWeights();
	}

	//--------------------------------------------------------------------
    } else if(selMetaHit  == kRpcClst)  {

	if(objects.pRpcClst){
	    Int_t tracks[4];
	    Bool_t atbox[4];
	    objects.pRpcClst->getTrackList  (tracks);
	    objects.pRpcClst->getIsAtBoxList(atbox);
	    Int_t nTr = objects.pRpcClst->howManyTracks();
	    if(nTr > 4) {
		Warning("fillCandSim()","RPC number of tracks > 4! Should not happen.");
                nTr = 4;
	    }
	    for(Int_t i = 0; i < nTr; i ++) {
		tr = tracks[i];
		if( tr >= 0 || tr == gHades->getEmbeddingRealTrackId()){
		    if(tr >= 0 ) { // sim tracks
			if(atbox[i]) { cand.rpcTracks.addTrack(tr,1,kIsInRPC|kIsInMETA);}
			else         {
			    Warning("fillCandSim()","RPC track not at box! Should not happen.");
			}
		    } else {     // embedded tracks
			cand.rpcTracks.addTrack(tr,1,kIsInRPC|kIsInMETA);
		    }
		}
	    }
	    cand.rpcTracks.calcWeights();
	}

	//--------------------------------------------------------------------
    } else if(selMetaHit  == kShowerHit){

	//--------------------------------------------------------------------
    } else if(selMetaHit  == kEmcClst){

	//--------------------------------------------------------------------

    } else { ; }  // NO META
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // Always fill SHOWER/EMC if existing
    if(objects.pEmcClst){
	Int_t nTr = objects.pEmcClst->getNTracks();
	for(Int_t i = 0; i < nTr; i ++) {
	    tr = objects.pEmcClst->getTrack(i);
	    if( tr >= 0 || tr == gHades->getEmbeddingRealTrackId()){
		cand.emcTracks.addTrack(tr,1,kIsInEMC|kIsInMETA);
	    }
	}
	cand.emcTracks.calcWeights();
    }

    if(objects.pShowerHit){
	Int_t nTr = objects.pShowerHit->getNTracks();
	for(Int_t i = 0; i < nTr; i ++) {
	    tr = objects.pShowerHit->getTrack(i);
	    if( tr >= 0 || tr == gHades->getEmbeddingRealTrackId()){
		cand.showerTracks.addTrack(tr,1,kIsInSHOWER|kIsInMETA);
	    }
	}
	cand.showerTracks.calcWeights();
    }
    //--------------------------------------------------------------------






    //--------------------------------------------------------------------
    // now add all detector tracks to commonTracks
    // and sort them by 1. flag and 2. weight
    cand.commonTracks.addTrackWeight(cand.mdc1Tracks);
    cand.commonTracks.addTrackWeight(cand.mdc2Tracks);
    cand.commonTracks.addTrackWeight(cand.richTracks);
    cand.commonTracks.addTrackWeight(cand.tofTracks);
    cand.commonTracks.addTrackWeight(cand.rpcTracks);
    if(cand.isEMC)cand.commonTracks.addTrackWeight(cand.emcTracks);
    else          cand.commonTracks.addTrackWeight(cand.showerTracks);
    cand.commonTracks.sortWeightNdet();

    if(cand.commonTracks.tracks.size()>0)cand.commonTracks.tracks[0]->flag |= ghostFlag; // make sure MDC decision is transported
    //--------------------------------------------------------------------

}
void HParticleCandFiller::fillSingleProperties()
{

    // selection for single tracks done in this loop
    // 1. RICH-MDC matching flags are set
    // 2. all cuts (GOODSEG0 .... ) are performed here.
    // Rejected candidate objects will have use == 0.

    Float_t zVertex        = gHades->getCurrentEvent()->getHeader()->getVertexZ();
    HVertex& vertexClust   = gHades->getCurrentEvent()->getHeader()->getVertexCluster();

    Float_t chi2Clust = vertexClust.getChi2(); // -1 = no vertex found , 0 = ok

    for(UInt_t i = 0; i < all_candidates.size(); i ++){
	candidate& cand = *(all_candidates[i]);

	//-----------------------------------------------------
	// RICH - MDC matching
	// 1. correct rings for vertex position
	// 2. check matching for RK
	if(cand.richhit.ind >= 0){
	    Float_t richCorr    = 0;

	    if(fbdoRichVertexCorr && chi2Clust > -0.5 && zVertex > -999.9F && zVertex < 100 ) richCorr = fFillerPar->getRichCorr(zVertex,cand.richhit.theta,cand.richhit.phi);  // vertex z [mm], deg

	    cand.corrThetaRich  = richCorr;
	    cand.richhit.theta += richCorr;
	    if(!fbIsSimulation && fbdoRichAlign){ // for real data do alignment of rich
		Double_t thetaCor = cand.richhit.theta;
		Double_t phiCor   = cand.richhit.phi;

		HParticleAngleCor::alignRichRing(cand.richhit.theta,cand.richhit.phi,thetaCor,phiCor);   // second correct theta/phi

		cand.alignThetaRich  = thetaCor - cand.richhit.theta;
                cand.alignPhiRich    = phiCor - cand.richhit.phi;
		cand.richhit.theta = (Float_t)thetaCor;
                cand.richhit.phi   = (Float_t)phiCor;
            }

	    Float_t dTheta = cand.deltaThetaSeg1(); // delta theta
	    Float_t dPhi   = cand.deltaPhiSeg1();   // delta phi
	    Float_t mom = cand.spline.p;            // initial mom from spline

	    if     (fmomSwitch == Particle::kMomRK )    { if( cand.rk.chi2  > 0) mom = cand.rk.p; }      //
	    else if(fmomSwitch == Particle::kMomKalman) { if( cand.kal.chi2 > 0) mom = cand.kal.p;}      //
	    else   Error("fillSingleProperties()","Unknown momswitch !");

	    cand.richMdcQuality     = sqrt( dTheta * dTheta + dPhi * dPhi);
	    cand.richRkQuality      = -1;
	    cand.hasRingCorrelation =  0;
	    if(fFillerPar->acceptPhiTheta(cand.sector,mom,dPhi,dTheta)){
		cand.hasRingCorrelation = kIsRICHMDC;
	    }

	    if( (fmomSwitch == Particle::kMomRK     && cand.rk.chi2  > 0 ) ||
                (fmomSwitch == Particle::kMomKalman && cand.kal.chi2 > 0 )
	      ){
		if(fmomSwitch == Particle::kMomRK){
		    dTheta = cand.deltaThetaRk(); // delta theta
		    dPhi   = cand.deltaPhiRk();   // delta phi
		} else if(fmomSwitch == Particle::kMomKalman){
		    dTheta = cand.deltaThetaKal(); // delta theta
		    dPhi   = cand.deltaPhiKal();   // delta phi
		} else Error("fillSingleProperties()","Unknown momswitch !");

		cand.richRkQuality = sqrt( dTheta * dTheta + dPhi * dPhi);
		if(fFillerPar->acceptPhiTheta(cand.sector,mom,dPhi,dTheta)){
		    cand.hasRingCorrelation = cand.hasRingCorrelation | kIsRICHRK;  // add flag
		}
	    }
	}
	//-----------------------------------------------------

	//-----------------------------------------------------
	// filter candidates by options. cand.used=0 will be skipped
	// when creating output objects
	if(fbgoodSeg0   && cand.seg1.chi2 < 0)          { cand.used = 0; }
	if(fbgoodSeg1   && cand.seg2.chi2 < 0)          { cand.used = 0; }
	if(fbgoodMeta   && cand.system    < 0)          { cand.used = 0; }
	if(fbgoodLepton && cand.hasRingCorrelation == 0){ cand.used = 0; }
	if(fbgoodRK ){
	    if     (fmomSwitch == Particle::kMomRK     ) { if( cand.rk.chi2   < 0) { cand.used = 0; }}
	    else if(fmomSwitch == Particle::kMomKalman ) { if( cand.kal.chi2  < 0) { cand.used = 0; }}
	    else Error("fillSingleProperties()","Unknown momswitch !");
	}
	//-----------------------------------------------------

    }
}

void HParticleCandFiller::fillCollectiveProperties()
{

    // collect information of the environment of a
    // candidate
    // 1. calculation openening angles etc. to fitted
    //    and not fitted other candidates (in inner MDC)

    UInt_t  size = all_candidates.size();

    if(size < 2) return;

    Float_t phi1=0.,phi2=0.,theta1=0.,theta2=0.;
    Float_t oAngle = -1;
    Int_t ind      =  0;

    //----------------------------------------------------------
    for(UInt_t i = 0; i < size ; i ++){                   // outer loop
	candidate& cand = *(all_candidates[i]);

	if(cand.rkSuccess){
	    if(fmomSwitch == Particle::kMomRK){
		phi1   = cand.rk.phi;
		theta1 = cand.rk.theta;
	    } else if(fmomSwitch == Particle::kMomKalman){
		phi1   = cand.kal.phi;
		theta1 = cand.kal.theta;
	    } else Error("fillCollectiveProperties()","Unknown momswitch !");

	} else {
	    phi1   = cand.seg1.phi;
	    theta1 = cand.seg1.theta;
	}

	//----------------------------------------------------------
	for(UInt_t j = 0; j < size; j ++){              // inner loop
	    candidate& cand2 = *(all_candidates[j]);
            if(cand.ind      == cand2.ind)      continue;   // skip candidate from same MetaMatch
	    if(cand.sector   != cand2.sector)   continue;   // only in same sector
            if(cand.seg1.ind == cand2.seg1.ind) continue;   // skip cands from same inner segment

	    if(cand2.rkSuccess){
		if(fmomSwitch == Particle::kMomRK){
		    phi2   = cand2.rk.phi;
		    theta2 = cand2.rk.theta;
		} else if(fmomSwitch == Particle::kMomKalman){
		    phi1   = cand.kal.phi;
		    theta1 = cand.kal.theta;
		} else Error("fillCollectiveProperties()","Unknown momswitch !");

	    } else {
		phi2   = cand2.seg1.phi;
		theta2 = cand2.seg1.theta;
	    }

	    oAngle = HParticleTool::getOpeningAngle(phi1,theta1,phi2,theta2);
	    if(oAngle < fAngleCloseTrack){                   // only for small angle
		cand.closeTracks.addTrack(j,oAngle);
	    }
	} // end inner

	//----------------------------------------------------------
        // find closest fitted/nonfitted tracks
	UInt_t ntr  = (cand.closeTracks).tracks.size();
	for(UInt_t l = 0; l < ntr; l ++){
	    ind    = (cand.closeTracks).tracks[l].ind;
	    oAngle = (cand.closeTracks).tracks[l].oAngle;
	    candidate& c = *(all_candidates[ind]);
	    if(c.seg1.chi2 > 0){
		if(oAngle < fabs(cand.oAFitted)){
		    cand.oAFitted = oAngle;
		    if( c.hasRingCorrelation == 0) { cand.oAFitted *= -1; }  // hadron case
		}
	    } else {
		if(oAngle < fabs(cand.oANoFitted)){
		    cand.oANoFitted = oAngle;
		    if( c.hasRingCorrelation == 0) { cand.oANoFitted *= -1; }  // hadron case
		}
	    }
	}
        //----------------------------------------------------------

    } // end outer
    //----------------------------------------------------------
}


void HParticleCandFiller::fillOutput()
{
    // From the candidate objects the output HParticleCand
    // will be filled if the candidate flage useed == 1
    // All additional categories objects will be filled
    // in the same order so that the objects later can be
    // easily matched.

    HParticleCand*    part  =  0;
    HParticleCandSim* partS =  0;
    Int_t index             = -1;




    HVertex vertex = (gHades->getCurrentEvent()->getHeader()->getVertexReco());
    Float_t sweights= vertex.getSumOfWeights();
    HGeomVector vvertex = vertex.getPos();
    Int_t nmin = 8;
    for(UInt_t i = 0; i < all_candidates.size(); i ++){
	candidate* cand = all_candidates[i];
	if(!cand->used) continue;
	pointers& objects = cand->objects;

	if(fbIsSimulation) {
	    if((partS = HCategoryManager::newObject(partS,fCatParticleCand,index)) != 0 )
	    {
		cand->fillParticleCand   (partS,index,fmomSwitch);
		cand->fillParticleCandSim(partS,fCatGeantKine);
                if(fbdoMETAQANorm)     HParticleTool::normDX(partS,fTofWalkPar);
                if(fbdoMomentumCorr)   HParticleTool::setCorrectedMomentum(partS);
		if(fbdoPathLengthCorr&&sweights>=nmin) HParticleTool::correctPathLength(partS,vvertex,cropLay->getMdcPlanes(),cropLay->getTargetMidPoint());
		checkCropLayer(partS);
	    }
	} else {
	    if((part = HCategoryManager::newObject(part,fCatParticleCand,index)) != 0 )
	    {
		cand->fillParticleCand   (part,index,fmomSwitch);
                if(fbdoMETAQANorm)     HParticleTool::normDX(part,fTofWalkPar);
		if(fbdoMomentumCorr)   HParticleTool::setCorrectedMomentum(part);
		if(fbdoPathLengthCorr&&sweights>=nmin) HParticleTool::correctPathLength(part,vvertex,cropLay->getMdcPlanes(),cropLay->getTargetMidPoint());
		checkCropLayer(part);
           }
	}

	if(fbIsDebug){
	    candidate* c;

	    c = HCategoryManager::newSlot(c,fCatParticleDebug,index);
	    if(c){
		c =  new (c) candidate(*cand);
	    }

	}

	if(fbFillMdc) {
	    HParticleMdc* partMdc = 0;
	    partMdc = HCategoryManager::newSlot(partMdc,fCatParticleMdc,index);

	    if(partMdc){
		partMdc =  new (partMdc) HParticleMdc();
                partMdc->setIndex(index);
		partMdc->fill(objects.pSeg1);
                partMdc->fill(objects.pSeg2);
                partMdc->fill(objects.pMdcTrk);
	    }


	}


	

    }
}
void  HParticleCandFiller::fillGeantAcceptance()
{
    if(fCatGeantKine&&fbdoGeantAcceptance){

        Int_t n = fCatGeantKine->getEntries();
        HGeantKine* kine = 0;
	Bool_t cropped [4] ;
	for(Int_t i = 0 ; i < n ; i ++){
	    kine = HCategoryManager::getObject(kine,fCatGeantKine,i);
	    if(kine){
		if(!kine->isAcceptanceFilled()) kine->fillAcceptanceBit();
		if(cropLay){
		    for(Int_t j = 0 ; j < 4 ; j ++) cropped[j] = kFALSE;
		    HParticleTool::checkCropedLayer(kine,cropLay,cropped);
		    for(Int_t j = 0 ; j < 4 ; j ++) {
			kine->unsetAtMdcEdge(j);
			if(cropped[j]) kine->setAtMdcEdge(j);
		    }
                    kine->setCropedFilled();
		}
	    }
	}
    }
}

void  HParticleCandFiller::checkCropLayer(HParticleCand* c)
{
    if(!cropLay) return;
    Int_t sec = c->getSector();
    if(!fSizesCells->modStatus(sec,0) && !fSizesCells->modStatus(sec,1)) return;

    Int_t layers[2] = {0,5};
    HMdcSeg* seg[2] = {0,0} ;

    Double_t x,y,z;
    Double_t xmin,xmax,ymin,ymax;

    for(Int_t s = 0; s < 2; s++){ // segment
	if(s==0) seg[0] = HParticleTool::getMdcSeg(c->getInnerSegInd());
	else     seg[1] = HParticleTool::getMdcSeg(c->getOuterSegInd());
        if(!seg[s]) continue;
	for(Int_t m = 0; m < 2; m++){ // module in segment
	    Int_t mod = 2*s+m;        // real module
	    if(!fSizesCells->modStatus(sec,mod)) continue;
	    Int_t lay = layers[m];
	    (*fSizesCells)[sec][mod][lay].calcSegIntersec(seg[s]->getZ(),seg[s]->getR(),seg[s]->getTheta(),seg[s]->getPhi(),x,y,z);
	    (*fSizesCells)[sec][mod][lay].transTo(x,y,z);
	    TVector2  p(x,y);
	    if(!cropLay->getCropedLayersEdge(p, sec,mod,lay, xmin,xmax,ymin,ymax)){
		c->setAtMdcEdge(s*2+m);
	    }
	}
    }
    
}

void HParticleCandFiller::clearVector(void)
{
    // clear the working objects at the end of each event.
    UInt_t nt = all_candidates.size();
    for(UInt_t i = 0; i < nt; i ++){
	candidate* cand = all_candidates[i];
	delete cand;
    }
    all_candidates.clear();
}
