//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
// HParticleEvtInfoFiller
//
// Reconstructor calculates detector hit multiplicity, track (hparticlecand)
// multiplicity and raction plane informations for each event.
// At the moment Forward Wall and Reaction Plane information is not calculated.
//
// These variables are stored in the HParticleEvtInfo output category.
//
////////////////////////////////////////////////////////////////////////////

#include "TString.h"
#include "TIterator.h"
#include "TArrayI.h"

#include "hparticleevtinfo.h"
#include "hparticleevtinfofiller.h"
#include "hcategorymanager.h"


#include "hmdclookupgeom.h"
#include "hmdccalparraw.h"
#include "hgeomvector.h"
#include "hparticletool.h"

#include "richdef.h"
#include "walldef.h"
#include "tofdef.h"
#include "hstartdef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "rpcdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hgeantdef.h"

#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hevent.h"
#include "hlocation.h"
#include "hlinearcategory.h"

#include "hshowerhitsim.h"
#include "hrichdirclus.h"
#include "hrichhit.h"
#include "hmdcclus.h"
#include "hmdcseg.h"
#include "hmdccal1.h"
#include "hmdcraw.h"
#include "hparticlecand.h"
#include "htofhit.h"
#include "hrpccluster.h"
#include "hrpchit.h"
#include "hemccluster.h"
#include "hwallhit.h"
#include "hwalleventplane.h"
#include "hstart2hit.h"


ClassImp(HParticleEvtInfoFiller)


//-----------------------------------------------------------------------------
HParticleEvtInfoFiller::HParticleEvtInfoFiller(const Text_t* name,const Text_t* title, TString beam)
: HReconstructor(name, title)
{

    beam.ToLower();
    beamtime=beam;
    clearMembers();

    ftimeTofCut      = 35.;
    ftimeRpcCut      = 25.;
    fprimary_mindist = 10.;

    fvertexzCut      = -65; // default apr12
    fMetaNPILEUPCut  = 5;
    fMetaPILEUPCut   =  60;

    fMdcPILEUPCut[0] = 200;     // default t1 > 200 (inner) > 400 (outer)
    fMdcPILEUPCut[1] = 200;     // default t1 > 200 (inner) > 400 (outer)
    fMdcPILEUPCut[2] = 400;     // default t1 > 200 (inner) > 400 (outer)
    fMdcPILEUPCut[3] = 400;     // default t1 > 200 (inner) > 400 (outer)
    fMdcPILEUPNCut   = 5*7;     // number of wires in cut region  (7 pion beam 35 apr12)

    fMeanMult        =  40;
    fMeanMIPSMult    =  12;
    fMeanLEPMult     =  1;
    fSigmaMult       =  22;
    fSigmaMIPSMult   =  7;
    fSigmaLEPMult    =  1;

    fMeanMIPSBetaCut =  0.8;

    fMeanLEPBetaCut  =  0.9;
    fMeanLEPMomCut   =  500;
    fMeanLEPRichQACut=  2;

    fminEvts         = 100;
    fmaxEvts         = 2000;
    fminLEPEvts      = 100;
    fmaxLEPEvts      = 4000;
    fSecToAll        = 6;
    fMinMult         = 1;
    fMaxFacToMean    = sqrt(12.);
    fMinStartCorrFlag= 0;
    ftriggerBit      = 13;


    // no veto
    fvetoMin = -15;
    fvetoMax =  15;

    // startMetacorrelation
    fminStartMeta = 80;
    fmaxStartMeta = 350;
    fthreshMeta   = 4;
    fwindowMeta   = 5;
    foffsetMeta   = 7;

    // startVetoCorrelation
    fminStartVeto = 15;
    fmaxStartVeto = 350;
    fwindowVeto   = 2.;


    if(beamtime == "jul14" || beamtime == "aug14") { // START at -400/-300
	fmaxLEPEvts      = 4000;
	fSecToAll        = 6;
	fMinMult         = 0;
	fMaxFacToMean    = 1000;
	fvertexzCut      = -160; //
	fMetaNPILEUPCut  = 0;
	fMdcPILEUPNCut   = 1*7;
	fMeanMult        = 1.4;
        fMeanMIPSMult    = 0.8;
	fMeanLEPMult     = 0.3;
	fSigmaMult       =  1;
	fSigmaMIPSMult   =  1;
	fSigmaLEPMult    =  1;
	fMeanLEPBetaCut  = 0.9;
	fMeanLEPMomCut   = 500;
	ftriggerBit      = 11;
	fMinStartCorrFlag= -1;
    } else if(beamtime == "apr12"){
	fmaxLEPEvts      = 4000;
	fSecToAll        = 6;
	fMinMult         = 0.;
	fMaxFacToMean    = 2.95;
	fvertexzCut      = -65; // default apr12 (START at -80)
	fMetaNPILEUPCut  = 5;
	fMdcPILEUPNCut   = 5*7;
	fMeanMult        = 40;
	fMeanMIPSMult    = 12;
	fMeanLEPMult     = 1;
	fSigmaMult       =  22;
	fSigmaMIPSMult   =  7;
	fSigmaLEPMult    =  1;
	fMeanLEPBetaCut  = 0.9;
	fMeanLEPMomCut   = 500;
	ftriggerBit      = 13;
	fMinStartCorrFlag= 0;
    } else {
       Error("HParticleEvtInfoFiller()","Unknown beam time %s! will use apr12 default!",beamtime.Data());
    }

}

HParticleEvtInfoFiller::~HParticleEvtInfoFiller()
{
    delete pWallHitIter;
    delete pRichHitIter;
    delete pRichClusIter;
    delete pMdcClusIter;
    delete pMdcSegIter;
    delete pTofHitIter;
    delete pRpcClusterIter;
    delete pRpcHitIter;
    delete pShowerHitIter;
    delete pParticleCandIter;
    delete pEmcClusIter;
    clearMembers();
}
// ---------------------------------------------------------------------------
void HParticleEvtInfoFiller::clearMembers()
{
    zeroLoc.set( 0 );

    pWallHitIter      = 0;
    pRichHitIter      = 0;
    pRichClusIter     = 0;
    pMdcClusIter      = 0;
    pMdcSegIter       = 0;
    pTofHitIter       = 0;
    pRpcClusterIter   = 0;
    pRpcHitIter       = 0;
    pShowerHitIter    = 0;
    pParticleCandIter = 0;
    pEmcClusIter      = 0;

    pParticleEvtInfoCat = 0;



}


// ---------------------------------------------------------------------------
Bool_t HParticleEvtInfoFiller::finalize(void)
{
    return kTRUE;
}


// ---------------------------------------------------------------------------
Bool_t HParticleEvtInfoFiller::init(void)
{
    HCategory* cat   = 0;

    // create iterators to input categories

    Int_t doErrorMessage = 0 ; // 0=warning, 1=Error , -1 silent

    cat = HCategoryManager::getCategory(catWallHit     ,doErrorMessage,"catWallHit, from HParticleEvtInfoFiller::init()");
    if (cat) pWallHitIter      = cat->MakeIterator("native");


    cat = HCategoryManager::getCategory(catRichHit     ,doErrorMessage,"catRichHit, from HParticleEvtInfoFiller::init()");
    if (cat) pRichHitIter      = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catRichDirClus ,doErrorMessage,"catRichDirClus, from HParticleEvtInfoFiller::init()");
    if (cat) pRichClusIter     = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catMdcClus     ,doErrorMessage,"catMdcClus, from HParticleEvtInfoFiller::init()");
    if (cat) pMdcClusIter      = cat->MakeIterator("native");


    cat = HCategoryManager::getCategory(catMdcSeg      ,doErrorMessage,"catMdcSeg, from HParticleEvtInfoFiller::init()");
    if (cat) pMdcSegIter       = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catTofHit      ,doErrorMessage,"catTofHit, from HParticleEvtInfoFiller::init()");
    if (cat) pTofHitIter       = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catRpcCluster  ,doErrorMessage,"catRpcCluster, from HParticleEvtInfoFiller::init()");
    if (cat)  pRpcClusterIter  = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catRpcHit      ,doErrorMessage,"catRpcHit, from HParticleEvtInfoFiller::init()");
    if (cat)  pRpcHitIter  = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catParticleCand,doErrorMessage,"catParticleCand, from HParticleEvtInfoFiller::init()");
    if (cat) pParticleCandIter = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catShowerHit,doErrorMessage,"catShowerHit, from HParticleEvtInfoFiller::init()");
    if (cat) pShowerHitIter  = cat->MakeIterator("native");

    cat = HCategoryManager::getCategory(catEmcCluster,doErrorMessage,"catEmcCluster, from HParticleEvtInfoFiller::init()");
    if (cat) pEmcClusIter  = cat->MakeIterator("native");

    pWallEventPlaneCat       = HCategoryManager::getCategory(catWallEventPlane,doErrorMessage,"catWallEventPlane, from HParticleEvtInfoFiller::init()");

    // make output category
    pParticleEvtInfoCat = HCategoryManager::getCategory(catParticleEvtInfo,-1,"catParticleEvtInfo" );
    if(!pParticleEvtInfoCat) pParticleEvtInfoCat = HCategoryManager::addCategory(catParticleEvtInfo,"HParticleEvtInfo",1,"Particle");

    if(!pParticleEvtInfoCat) {
	Error( "init", "Cannot make catParticleEvtInfo category" );
	return kFALSE;
    }

    HCategory* mdcrawCat = HCategoryManager::getCategory(catMdcRaw,-1,"");

    lookupMdcGeom = (HMdcLookupGeom*) gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
    if(mdcrawCat)calparMdc     = (HMdcCalParRaw*)  gHades->getRuntimeDb()->getContainer("MdcCalParRaw");
    else         calparMdc = NULL;

    meanALL .initParam(fmaxEvts   ,fminEvts   ,fMaxFacToMean ,fMeanMult    ,fSigmaMult    ,fMinMult);
    meanMIPS.initParam(fmaxEvts   ,fminEvts   ,fMaxFacToMean ,fMeanMIPSMult,fSigmaMIPSMult,fMinMult);
    meanLEP .initParam(fmaxLEPEvts,fminLEPEvts,fMaxFacToMean ,fMeanLEPMult ,fSigmaLEPMult ,fMinMult);
    for(Int_t s=0;s<6;s++) {
	meanALLSec [s].initParam(fmaxEvts   ,fminEvts   ,fMaxFacToMean ,fMeanMult    /fSecToAll,fSigmaMult    ,fMinMult);
	meanMIPSSec[s].initParam(fmaxEvts   ,fminEvts   ,fMaxFacToMean ,fMeanMIPSMult/fSecToAll,fSigmaMIPSMult,fMinMult);
	meanLEPSec [s].initParam(fmaxLEPEvts,fminLEPEvts,fMaxFacToMean ,fMeanLEPMult /fSecToAll,fSigmaLEPMult ,fMinMult);
    }

    return kTRUE;
}

Bool_t HParticleEvtInfoFiller::isLepton(HParticleCand* cand)
{

    if(!cand->isFlagBit(kIsUsed)) return kFALSE;
    Float_t mom  =  cand->getMomentum() ;
    Float_t beta =  cand->getBeta() ;

    if(beta<0)               return kFALSE;
    if(mom >fMeanLEPMomCut)  return kFALSE;
    if(cand->getRichInd()<0) return kFALSE;
    if(cand->getRichMatchingQuality()>fMeanLEPRichQACut) return kFALSE;
    if(beta  <  (((1.-fMeanLEPBetaCut)/fMeanLEPMomCut)*mom + fMeanLEPBetaCut)) return kFALSE;   // wedge (together with momcut)
    return kTRUE;
}

// ---------------------------------------------------------------------------
Int_t HParticleEvtInfoFiller::execute(void)
{
    HWallHit           *wall_hit         = 0;
    HRichHit           *rich_hit         = 0;
    HRichDirClus       *rich_clus        = 0;
    HMdcClus           *mdc_clus         = 0;
    HMdcSeg            *mdc_seg          = 0;
    HShowerHit         *shower_hit       = 0;
    HEmcCluster        *emc_clus         = 0;
    HTofHit            *tof_hit          = 0;
    HRpcCluster        *rpc_cluster      = 0;
    HRpcHit            *rpc_hit          = 0;
    HParticleCand      *particle_cand    = 0;
    HParticleEvtInfo   *pParticleEvtInfo = 0;

    Int_t wall_hits                                     = 0;
    Int_t rings                                         = 0;
    Int_t rich_rings             [MAXSECTOR]            = { 0 };
    Int_t rich_corr_rings        [MAXSECTOR]            = { 0 };
    Int_t rich_clusters          [MAXSECTOR]            = { 0 };
    Int_t mdc_clusters           [MAXSECTOR][MAXIOSEG]  = { { 0 }, { 0 } };
    Int_t mdc_segments_fitted    [MAXSECTOR][MAXIOSEG]  = { { 0 }, { 0 } };
    Int_t mdc_segments_unfitted  [MAXSECTOR][MAXIOSEG]  = { { 0 }, { 0 } };
    Int_t tof_hits               [MAXSECTOR]            = { 0 };
    Int_t rpc_clst               [MAXSECTOR]            = { 0 };
    Int_t rpc_hits               [MAXSECTOR]            = { 0 };
    Int_t tof_hits_cut           [MAXSECTOR]            = { 0 };
    Int_t rpc_clst_cut           [MAXSECTOR]            = { 0 };
    Int_t rpc_hits_cut           [MAXSECTOR]            = { 0 };
    Int_t shower_hits            [MAXSECTOR]            = { 0 };
    Int_t particle_cands         [MAXSECTOR]            = { 0 };
    Int_t selected_particle_cands[MAXSECTOR]            = { 0 };
    Int_t primary_particle_cands [MAXSECTOR]            = { 0 };

    Int_t ctMETAPileUp = 0;
    Int_t ctMDCPileUp  = 0;


    TArrayI  pUsedRichHit;
    Int_t    idxRich;







    // fill counters
    if (pWallHitIter) {
	pWallHitIter->Reset();
	while ((wall_hit = (HWallHit*)pWallHitIter->Next())) { ++ wall_hits; }
    }

    if (pRichHitIter) {
	pRichHitIter->Reset();
	while ((rich_hit = (HRichHit*)pRichHitIter->Next())) {
	    ++ rings;
	    ++ rich_rings[rich_hit->getSector()];
	}
    }

    if (pRichClusIter) {
	pRichClusIter->Reset();
	while ((rich_clus = (HRichDirClus*)pRichClusIter->Next())){
	    ++ rich_clusters[rich_clus->getSector()];
	}
    }

    if (pMdcClusIter) {
	pMdcClusIter->Reset();
	while ((mdc_clus = (HMdcClus*)pMdcClusIter->Next())) {
	    ++ mdc_clusters[mdc_clus->getSec()][mdc_clus->getIOSeg()];
	}
    }

    if (pMdcSegIter) {
	pMdcSegIter->Reset();
	while ((mdc_seg = (HMdcSeg*)pMdcSegIter->Next())) {
	    if (mdc_seg->getChi2() == -1) ++ mdc_segments_unfitted[mdc_seg->getSec()][mdc_seg->getIOSeg()];
	    else	                  ++ mdc_segments_fitted  [mdc_seg->getSec()][mdc_seg->getIOSeg()];
	}
    }


    //-----------------------------------------------------------------
    Int_t wires   [6][4][6][220]; // wire address -> ntimes
    Int_t wiresRaw[6][4][6][220]; // wire address from raw before timecut
    Int_t wiresPileup[6][4][6][220]; // wire address from raw before timecut
    // clear the count table per wire
    for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
		    wires[s][m][l][c]    = -1;
		    wiresRaw[s][m][l][c] = -1;
		    wiresPileup[s][m][l][c] = -1;
		}
	    }
	}
    }
    // loop over Mdc Raw cat and make a list of all
    // wires in the current event. the initial count
    // of the wire is set to 0

    HCategory* mdcRawCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcRaw);
    if(mdcRawCat&&lookupMdcGeom&&calparMdc){
	HStart2Hit* starthit =0;
        starthit = HCategoryManager::getObject(starthit,catStart2Hit,0,kTRUE);
	Float_t starttime = 0;
        if(starthit) starttime = starthit->getTime();

	HMdcRaw* raw;
        Int_t s,m,l,c,mb,tdc;
	Int_t size = mdcRawCat->getEntries();
	for(Int_t i = 0; i < size; i ++){
	    raw = HCategoryManager::getObject(raw,mdcRawCat,i);
	    if(raw){
		raw->getAddress(s,m,mb,tdc);

		if (s<0 || m<0) continue;
		HMdcLookupMoth& lookGMoth = (*lookupMdcGeom)[s][m][mb];
		if(&lookGMoth == NULL || lookGMoth.getSize() == 0) continue;
		HMdcLookupChan& chan=(*lookupMdcGeom)[s][m][mb][tdc];
		l=chan.getNLayer();
		c=chan.getNCell();
                if(l<0 || c<0) continue;
		wiresRaw[s][m][l][c] ++;
		HMdcCalParTdc& t = (*calparMdc)[s][m][mb][tdc];
		Float_t offset = t.getOffset();
		Float_t slope  = t.getSlope();
                if(offset<0) continue;
		Float_t Time1 = (offset - (raw->getTime(1) * slope) + starttime);
                Float_t Time2 = (offset - (raw->getTime(2) * slope) + starttime);

		if(Time1>fMdcPILEUPCut[m] && Time2-Time1 > 20) wiresPileup[s][m][l][c] ++;

	    }
	}
    }
    // loop over Mdc Cal1 cat and make a list of all
    // wires in the current event. the initial count
    // of the wire is set to 0

    HCategory* mdcCal1Cat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcCal1);
    if(mdcCal1Cat){
	HMdcCal1* cal;
        Int_t s,m,l,c;
	Int_t size = mdcCal1Cat->getEntries();
	for(Int_t i = 0; i < size; i ++){
	    cal = HCategoryManager::getObject(cal,mdcCal1Cat,i);
	    if(cal){
		cal->getAddress(s,m,l,c);
		wires[s][m][l][c] ++;
	    }
	}
    }
    // loop over MdcSegment cat and count up
    // each wire which has been used in segments.
    // Non fitted wires are ignored.

    HCategory* mdcSegCat = (HCategory*) gHades->getCurrentEvent()->getCategory(catMdcSeg);
    if(mdcSegCat){
	HMdcSeg* seg;
	Int_t s,m=0,l,c,io,ncell;
	Int_t size = mdcSegCat->getEntries();
	for(Int_t i = 0; i < size; i ++){
	    seg = HCategoryManager::getObject(seg,mdcSegCat,i);
	    if(seg){
		s  = seg->getSec();
		io = seg->getIOSeg();
		for(Int_t lay = 0; lay < 12; lay ++){
		    if(io == 0 ){ m = ( lay < 6 ) ? 0 : 1;}
		    if(io == 1 ){ m = ( lay < 6 ) ? 2 : 3;}

		    if(lay >= 6 ) l = lay - 6;
		    else          l = lay;

		    ncell = seg->getNCells(lay);

		    for(Int_t n = 0; n < ncell; n ++){
			c = seg->getCell(lay,n);
			wires[s][m][l][c] ++;
		    }
		}
	    }
	}
     }
     // pileup
     for(Int_t s = 0; s < 6; s ++){
	for(Int_t m = 0; m < 4; m ++){
	    for(Int_t l = 0; l < 6; l ++){
		for(Int_t c = 0; c < 220; c ++){
		    if(wires[s][m][l][c] > 0 && wiresPileup[s][m][l][c] == 0) ctMDCPileUp++;
		}
	    }
	}
    }

    //-----------------------------------------------------------------

    if (pTofHitIter) {
	pTofHitIter->Reset();
	while ((tof_hit = (HTofHit*)pTofHitIter->Next())) {
	    ++ tof_hits[(Int_t)tof_hit->getSector()];
	    if(tof_hit->getTof() < ftimeTofCut && tof_hit->getTof() > 0 )  ++ tof_hits_cut[(Int_t)tof_hit->getSector()];

	    if(tof_hit->getTof() < 0 || tof_hit->getTof() > fMetaPILEUPCut ) ctMETAPileUp++;
	}
    }

    if (pRpcClusterIter) {
	pRpcClusterIter->Reset();
	while ((rpc_cluster = (HRpcCluster*)pRpcClusterIter->Next())) {
	    ++ rpc_clst[(Int_t)rpc_cluster->getSector()];
	    if(rpc_cluster->getTof() < ftimeRpcCut && rpc_cluster->getTof() > 0 )  ++ rpc_clst_cut[(Int_t)rpc_cluster->getSector()];
	    if(rpc_cluster->getTof() < 0 || rpc_cluster->getTof() > fMetaPILEUPCut ) ctMETAPileUp++;

	}
    }

    if (pRpcHitIter) {
	pRpcHitIter->Reset();
	while ((rpc_hit = (HRpcHit*)pRpcHitIter->Next())) {
	    ++ rpc_hits[(Int_t)rpc_hit->getSector()];
            if(rpc_hit->getTof() < ftimeRpcCut && rpc_hit->getTof() > 0 )  ++ rpc_hits_cut[(Int_t)rpc_hit->getSector()];
	}
    }

    if (pShowerHitIter) {
	pShowerHitIter->Reset();
	while ((shower_hit = (HShowerHit*) pShowerHitIter->Next())) {
	    if(shower_hit->getModule() == 0) ++ shower_hits[(Int_t)shower_hit->getSector()];
	}
    }
    if (pEmcClusIter) {
	pEmcClusIter->Reset();
	while ((emc_clus = (HEmcCluster*) pEmcClusIter->Next())) {
	    ++ shower_hits[(Int_t)emc_clus->getSector()];
	}
    }

    pUsedRichHit.Set(rings);
    pUsedRichHit.Reset();


    //-----------------------------------------------------------------

    Bool_t isSimulation = kFALSE;
    if(gHades->getEmbeddingMode()>0) isSimulation = kFALSE; // embedding is treated as real
    else {
	HCategory* kinecat = HCategoryManager::getCategory(catGeantKine,-1); // silent
	if(kinecat) isSimulation = kTRUE;
    }


    Bool_t kGOODRECOVERTEX  = kFALSE;
    Bool_t kGOODCLUSTVERTEX = kFALSE;
    Bool_t kGOODSTART       = kFALSE;
    Bool_t kGOODSTART2      = kFALSE;
    Bool_t kGOODTRIGGER     = kFALSE;
    Bool_t kNOPILEUPMETA    = kFALSE;
    Bool_t kNOPILEUPMDC     = kFALSE;
    Bool_t kNOPILEUPSTART   = kFALSE;
    Bool_t kNOVETO          = kFALSE;
    Bool_t kGOODSTARTVETO   = kFALSE;
    Bool_t kGOODSTARTMETA   = kFALSE;





    if(isSimulation){ // switch on all flags which need START + VETO + TRIGGER
	kGOODSTART      = kTRUE;
	kGOODSTART2     = kTRUE;
	kNOPILEUPSTART  = kTRUE;
	kNOVETO         = kTRUE;
	kGOODSTARTVETO  = kTRUE;
	kGOODSTARTMETA  = kTRUE;
	kGOODTRIGGER    = kTRUE;

    }  else {

	if(gHades->getCurrentEvent()->getHeader()->isTBit(ftriggerBit))       { kGOODTRIGGER     = kTRUE; }
	kNOPILEUPSTART = HParticleTool::isNoSTARTPileUp();
        kGOODSTART     = HParticleTool::isGoodSTART(fMinStartCorrFlag);
        kGOODSTART2    = HParticleTool::isGoodSTART(2);
	kNOVETO        = HParticleTool::isNoVETO(fvetoMin,fvetoMax);
	kGOODSTARTVETO = HParticleTool::isGoodSTARTVETO(fminStartVeto,fmaxStartVeto,fwindowVeto);
	kGOODSTARTMETA = HParticleTool::isGoodSTARTMETA(fminStartMeta,fmaxStartMeta,fthreshMeta,fwindowMeta,foffsetMeta);

    }

    kGOODCLUSTVERTEX = HParticleTool::isGoodClusterVertex(fvertexzCut);
    kGOODRECOVERTEX  = HParticleTool::isGoodRecoVertex   (fvertexzCut);
    if(ctMETAPileUp <= fMetaNPILEUPCut)                                   { kNOPILEUPMETA    = kTRUE; }
    if(ctMDCPileUp  <= fMdcPILEUPNCut )                                   { kNOPILEUPMDC     = kTRUE; }
    //-----------------------------------------------------------------

    if (pParticleCandIter)
    {
        HGeomVector base, dir;
	HGeomVector vertex = HParticleTool::getGlobalVertex(Particle::kVertexParticle,kFALSE);

	Int_t ctNCand    [7] = {0,0,0,0,0,0,0};
        Int_t ctNCandMIPS[7] = {0,0,0,0,0,0,0};
        Int_t ctNCandLEP [7] = {0,0,0,0,0,0,0};


	pParticleCandIter->Reset();
	while ((particle_cand = (HParticleCand*)pParticleCandIter->Next()))
	{
	    ++ particle_cands[particle_cand->getSector()];
	    if(particle_cand->isFlagBit(Particle::kIsUsed))
	    {
		++ selected_particle_cands[particle_cand->getSector()];

		HParticleTool::calcSegVector(particle_cand->getZ(),particle_cand->getR(),particle_cand->getPhi(), particle_cand->getTheta(),base,dir);
		Double_t mindist = HParticleTool::calculateMinimumDistanceStraightToPoint(base,dir,vertex);
		if(mindist<fprimary_mindist) { ++ primary_particle_cands[particle_cand->getSector()]; particle_cand->setPrimaryVertex(); }

		//-----------------------------------------------------------------
                // running mean collection
		if(kGOODTRIGGER && kGOODCLUSTVERTEX && kGOODSTART){  // some minimal conditions for relevant events
		    if(isLepton(particle_cand)){
			ctNCandLEP[particle_cand->getSector()]++;
			ctNCandLEP[6]++;
		    }

		    Float_t beta = particle_cand->getBeta();
		    if(beta>0){
			ctNCand[particle_cand->getSector()]++;
			ctNCand[6]++;
			if(beta>fMeanMIPSBetaCut){
			    ctNCandMIPS[particle_cand->getSector()]++;
			    ctNCandMIPS[6]++;
			}
		    }
		}
		//-----------------------------------------------------------------

	    }
	    if ( (idxRich = particle_cand->getRichInd()) > -1 &&
		 (particle_cand->getRingCorr() == Particle::kIsRICHRK || particle_cand->getRingCorr() == (Particle::kIsRICHRK|Particle::kIsRICHMDC) )  &&
		pUsedRichHit.At(idxRich) == 0 )
	    {
		pUsedRichHit.AddAt(1, idxRich);
		++ rich_corr_rings[particle_cand->getSector()];
	    }
	}

	//-----------------------------------------------------------------
        // calculate running mean
	if(ctNCand[6]>0){ // skipp empty events

	    meanALL .calcMean(ctNCand    [6]);
	    meanMIPS.calcMean(ctNCandMIPS[6]);
	    meanLEP .calcMean(ctNCandLEP [6]);
	    for(Int_t s=0;s<6;s++) {
		meanALLSec [s].calcMean(ctNCand    [s]);
		meanMIPSSec[s].calcMean(ctNCandMIPS[s]);
        	meanLEPSec[s] .calcMean(ctNCandLEP [s]);
            }
	}
	//-----------------------------------------------------------------

    } // end particle loop



    // fill output category
    Int_t index = -1;
    if((pParticleEvtInfo = HCategoryManager::newObject(pParticleEvtInfo,pParticleEvtInfoCat,index)) != 0 )
    {
	pParticleEvtInfo = new (pParticleEvtInfo) HParticleEvtInfo;
	pParticleEvtInfo->setFWallMult               ( wall_hits );
	pParticleEvtInfo->setRichMult                ( rich_rings );
	pParticleEvtInfo->setRichCorrMult            ( rich_corr_rings );
	pParticleEvtInfo->setRichClusMult            ( rich_clusters );
	pParticleEvtInfo->setTofMult                 ( tof_hits );
	pParticleEvtInfo->setRpcMult                 ( rpc_clst );
	pParticleEvtInfo->setRpcMultHit              ( rpc_hits );
	pParticleEvtInfo->setTofMultCut              ( tof_hits_cut );
	pParticleEvtInfo->setRpcMultCut              ( rpc_clst_cut );
	pParticleEvtInfo->setRpcMultHitCut           ( rpc_hits_cut );
	pParticleEvtInfo->setShowerMult              ( shower_hits );     // EMC too
	pParticleEvtInfo->setParticleCandMult        ( particle_cands );
	pParticleEvtInfo->setSelectedParticleCandMult( selected_particle_cands );
	pParticleEvtInfo->setPrimaryParticleCandMult ( primary_particle_cands );
	pParticleEvtInfo->setMdcClusMult             ( mdc_clusters );
	pParticleEvtInfo->setMdcSegFittedMult        ( mdc_segments_fitted );
	pParticleEvtInfo->setMdcSegUnfittedMult      ( mdc_segments_unfitted );
	for(Int_t s = 0; s < 6; s ++){
	    for(Int_t m = 0; m < 4; m ++){
		for(Int_t l = 0; l < 6; l ++){
		    Int_t sumLayUnused = 0;
                    Int_t sumLay       = 0;
                    Int_t sumLayRaw    = 0;
		    for(Int_t c = 0; c < 220; c ++){
			if(wires   [s][m][l][c] == 0) sumLayUnused ++;
                        if(wires   [s][m][l][c] >  0) sumLay       ++;
	                if(wiresRaw[s][m][l][c] == 0) sumLayRaw    ++;
	            }
                    pParticleEvtInfo->setMdcWires      (s,m,l,sumLay);
                    pParticleEvtInfo->setMdcWiresUnused(s,m,l,sumLayUnused);
                    pParticleEvtInfo->setMdcWiresRaw   (s,m,l,sumLayRaw);
		}
	    }
	}

	if(pWallEventPlaneCat){
	    HWallEventPlane* evtplane = 0;
	    evtplane = HCategoryManager::getObject(evtplane,pWallEventPlaneCat,0,kTRUE);
	    if(evtplane){
		pParticleEvtInfo->setRPlanePhi  (evtplane->getPhi());
		pParticleEvtInfo->setPhiA       (evtplane->getPhiA() );
		pParticleEvtInfo->setPhiB       (evtplane->getPhiB());
		pParticleEvtInfo->setPhiAB      (evtplane->getPhiAB());
		pParticleEvtInfo->setNA         (evtplane->getNA());
		pParticleEvtInfo->setNB         (evtplane->getNB());

	    }
	}


	if(kGOODCLUSTVERTEX)pParticleEvtInfo->setSelectorBit(kGoodVertexClust);
	if(kGOODRECOVERTEX) pParticleEvtInfo->setSelectorBit(kGoodVertexCand);
	if(kGOODSTART)      pParticleEvtInfo->setSelectorBit(kGoodSTART);
	if(kGOODSTART2)     pParticleEvtInfo->setSelectorBit(kGoodSTART2);
	if(kGOODTRIGGER)    pParticleEvtInfo->setSelectorBit(kGoodTRIGGER);
	if(kNOPILEUPMETA)   pParticleEvtInfo->setSelectorBit(kNoPileUpMETA);
	if(kNOPILEUPMDC)    pParticleEvtInfo->setSelectorBit(kNoPileUpMDC);
	if(kNOPILEUPSTART)  pParticleEvtInfo->setSelectorBit(kNoPileUpSTART);
        if(kNOVETO)         pParticleEvtInfo->setSelectorBit(kNoVETO);
        if(kGOODSTARTVETO)  pParticleEvtInfo->setSelectorBit(kGoodSTARTVETO);
	if(kGOODSTARTMETA)  pParticleEvtInfo->setSelectorBit(kGoodSTARTMETA);

        pParticleEvtInfo->setMeanMult    (meanALL .getMean());
        pParticleEvtInfo->setMeanMIPSMult(meanMIPS.getMean());
        pParticleEvtInfo->setMeanLEPMult (meanLEP .getMean());
        pParticleEvtInfo->setSigmaMult    (meanALL .getSigma());
        pParticleEvtInfo->setSigmaMIPSMult(meanMIPS.getSigma());
        pParticleEvtInfo->setSigmaLEPMult (meanLEP .getSigma());
        for(Int_t s = 0; s < 6; s ++){
	    pParticleEvtInfo->setMeanMult    (s,meanALLSec [s].getMean());
	    pParticleEvtInfo->setMeanMIPSMult(s,meanMIPSSec[s].getMean());
	    pParticleEvtInfo->setMeanLEPMult (s,meanLEPSec [s].getMean());
	    pParticleEvtInfo->setSigmaMult    (s,meanALLSec [s].getSigma());
	    pParticleEvtInfo->setSigmaMIPSMult(s,meanMIPSSec[s].getSigma());
	    pParticleEvtInfo->setSigmaLEPMult (s,meanLEPSec [s].getSigma());
	}

    }
    else
    {
	Error( "execute", "Couldn't get a new slot from 'catParticleEvtInfo'!" );
	exit( 2 );
    }

    return 0;
}
