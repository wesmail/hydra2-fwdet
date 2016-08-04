//*-- Author   : Patrick Sellheim, Georgy Kornakov
//*-- Created  : 09/01/2014

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HParticleBt
//
//  This class contains functions to search rings in 1 event based on data
//  provided by HBtMatrix.
// General strategy for RICH back tracking:
//  - Use theta and phi angle information of particle candidate
//    to point to ring center on RICH pad plane
//  - Predict fired pads with the help of ring shape parametrization
//    function
//  - If at least one predicted pad is fired cluster information will
//    be calculated
//  - Fired pads are allocated to clusters of rings
//  - Local maxima of clusters are searched and used to calculate the
//    ring quality observable chi2
//  - Number of all shared maxima per ring and shared maxima with a
//    specific track are identified
//  - User output is stored.
//
//////////////////////////////////////////////////////////////////////////////

#include "hparticlebt.h"
#include "hcategorymanager.h"
#include "richdef.h"
#include "hparticledef.h"
#include "hruntimedb.h"
#include "hparticlebtpar.h"
#include "hparticletool.h"


  Float_t  HParticleBt::fBetaRPCLimit = 0.8;
  Float_t  HParticleBt::fBetaTOFLimit = 0.8;
  Float_t  HParticleBt::fMdcdEdxLimit = 50.;
  Float_t  HParticleBt::foAngleLimit  = 4.;

// ----------------------------------------------------------------
ClassImp(HParticleBt)

// ----------------------------------------------------------------

HParticleBt::HParticleBt(void)
{
    fRing         = NULL;
    fClus         = NULL;
    fCandCat      = NULL;
    fRichCal      = NULL;
    fBtPar        = NULL;
    fDebugInfo    = kFALSE;
}

HParticleBt::HParticleBt(const Text_t* name,const Text_t* title,const TString beamtime)
: HReconstructor(name,title)
{
    fDebugInfo    = kFALSE;
    fRing         = NULL;
    fClus         = NULL;
    fCandCat      = NULL;
    fRichCal      = NULL;
    fBtPar        = NULL;
    fDebugInfo    = kFALSE;
    cout << "--------------------------------------------------------------------------------------------" << endl;
    cout << "HParticleBtRing: "<< endl;

    if(beamtime=="apr12"){
	cout << "Beamtime: '" << beamtime << "' Angle corrections for leptons are applied." << endl;
	fdoAngleCorr = kTRUE;
    }else if(beamtime=="jul14" || beamtime=="aug14" ){
	fdoAngleCorr = kFALSE;
	cout << "Beamtime: '" << beamtime << "' Angle corrections for leptons are not applied." << endl;
    }else{
	cout << "WARNING: Unknown beamtime " << beamtime << "! Angle corrections for leptons are applied by default." << endl;
	fdoAngleCorr = kFALSE;
    }
    cout << endl;
}

HParticleBt::~HParticleBt(void)
{

}

void HParticleBt::setDebugInfo(Bool_t debugInfo)
{
    //Set kTRUE to produce RingInfo output
    fDebugInfo = debugInfo;
}


Bool_t HParticleBt::init(void)
{

    fCandCat = gHades->getCurrentEvent()->getCategory(catParticleCand);
    if(!fCandCat) {
        Error("init","No ParticleCand Category");
	return kFALSE;
    }

   

    fRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if(!fRichCal) {
        Error("init","No RichCalCategory");
	return kFALSE;
    }

    fBtPar =(HParticleBtPar*) gHades->getRuntimeDb()->getContainer("ParticleBtPar");
    if(!fBtPar) {
        Error ("init()", "Retrieve ZERO pointer for ParticleBtPar!");
        return kFALSE;
    }
    
    fAngleCor.setDefaults("apr12");
    fSorter.init();

    fRing = new HParticleBtRingF();
    fClus = new HParticleBtClusterF();
    fClus->setRingF(fRing);

    // make output category
    fParticleBtOutCat = HCategoryManager::getCategory(catParticleBtRing,-1,"catParticleBtRing" );
    if(!fParticleBtOutCat) fParticleBtOutCat = HCategoryManager::addCategory(catParticleBtRing,"HParticleBtRing",1,"Particle");

    if(!fParticleBtOutCat) {
	Error( "init", "Cannot make catParticleBtOut category" );
	return kFALSE;
    }

    // make ring info category
    if(fDebugInfo){
	fParticleBtRingInfoCat = HCategoryManager::getCategory(catParticleBtRingInfo,-1,"catParticleBtRingInfo" );
	if(!fParticleBtRingInfoCat) fParticleBtRingInfoCat = HCategoryManager::addCategory(catParticleBtRingInfo,"HParticleBtRingInfo",1,"Particle");

	if(!fParticleBtRingInfoCat) {
	Error( "init", "Cannot make catParticleBtRingInfo category" );
	return kFALSE;
	}
    }
    return kTRUE;
}

Bool_t HParticleBt::reinit(void)
{

    fClus -> init();
    fRing -> init();
    fClus -> setRingF(fRing);

    fBetaRPCLimit = fBtPar->getBetaRPCLimit();
    fBetaTOFLimit = fBtPar->getBetaTOFLimit();
    fMdcdEdxLimit = fBtPar->getMDCdEdxLimit();
    foAngleLimit  = 4.;

    return kTRUE;
}

Bool_t HParticleBt::isGoodLepton(HParticleCand* cand)
{
    //Good Particle cand selection
    Int_t sys       = cand->getSystemUsed();
    Float_t beta    = cand->getBeta();
    Float_t dEdx    = cand->getMdcdEdx();

    if(dEdx > fMdcdEdxLimit)
	return kFALSE;
    else{
	if( (sys == 0 && beta > fBetaRPCLimit) || (sys == 1 && beta > fBetaTOFLimit) )
	    return kTRUE;
	else
            return kFALSE;
    }
    return kFALSE;
}

Bool_t HParticleBt::selectLeptonsBeta(HParticleCand* pcand)
{
    //Track selection function for lepton candidates.

    Bool_t selectEpEm = kFALSE;
    if(pcand->isFlagAND(4,
			Particle::kIsAcceptedHitInnerMDC,
			Particle::kIsAcceptedHitOuterMDC,
			Particle::kIsAcceptedHitMETA,
			Particle::kIsAcceptedRK)&&

       pcand->getInnerSegmentChi2() > 0
       &&
       isGoodLepton(pcand)
       &&
       pcand->getChi2() < 500) selectEpEm=kTRUE;
    if(selectEpEm)
	return kTRUE;
    else
	return kFALSE;
}


Int_t HParticleBt::execute(void)
 {
    //Backtracking main class
    //Sorting the particle candidates in order to fill the internally used vector
    vector <HParticleCand> candidates;
    HVertex& vertex = gHades->getCurrentEvent()->getHeader()->getVertex();
    HParticleCand* cand1 = 0;
    Int_t counter        = 0;
    Bool_t trackInSec[6] = {kFALSE,kFALSE,kFALSE,kFALSE,kFALSE,kFALSE};

  
    //------------------------------------------------------------------------
    // clean vectors and index arrays
    fSorter.backupFlags(kTRUE); // abckup only flags not setup
    fSorter.cleanUp();
    //------------------------------------------------------------------------
    fSorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);            // reset all flags for flags (0-28) ,reject,used,lepton
    fSorter.setIgnoreInnerMDC();
    fSorter.setIgnoreOuterMDC();
    fSorter.setIgnoreMETA();
   
    fSorter.fill(selectLeptonsBeta);   // fill only good leptons
    fSorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
    // Loop over particle candidates for finding reasanoble tracks (beta, chi2, mmqa,...) for further matching with RICH

    for(Int_t i = 0; i < fCandCat->getEntries() ; i ++){
	cand1 = HCategoryManager::getObject(cand1,fCandCat,i);
	if(!cand1->isFlagBit(Particle::kIsLepton)) {
	    continue;
	}
	candidates.push_back(*cand1);
    }

    // Loop for filling prediction and correction for MS
    Int_t size = candidates.size();
    for(Int_t i = 0; i < size; i ++){
	if(fdoAngleCorr)
	    fAngleCor.recalcSetAngles(&candidates[i]);
        fRing->fillPrediction(&candidates[i], vertex,kTRUE,fdoAngleCorr);
    }

    fRing->fillRichCal(fRichCal);


    //Search for close Track parts around good rings. Search area is restricted by foAngleLimit.
    Int_t candidatesSize = candidates.size();
    for(Int_t i = 0; i < fCandCat->getEntries() ; i ++){
	cand1 = HCategoryManager::getObject(cand1,fCandCat,i);
        Float_t oAngle = 99;
	for(Int_t j = 0; j < candidatesSize; j ++){
	    if(cand1->getSector() != candidates[j].getSector()) continue;
	    oAngle = HParticleTool::getOpeningAngle(&candidates[j],cand1);
	    if(oAngle < foAngleLimit && fClus->getPadsRing(j) > 0) {
		Bool_t isInList = kFALSE;
		for(Int_t k = 0 ; k < candidatesSize ; k++){
		    if(candidates[k].getIndex() == cand1->getIndex())
			isInList = kTRUE;
		}
		if(!isInList){ //Candidate is not included in list of good candidates
		    candidates.push_back(*cand1);
		    if(fdoAngleCorr)
			fAngleCor.recalcSetAngles(&candidates[candidates.size()-1]);
		    fRing->fillPrediction(&candidates[candidates.size()-1], vertex,kFALSE,fdoAngleCorr);
		
		}
	    }
	}
    }

    // Fills rich cal hits for further calculations
    for(Int_t i = 0; i < size; i++){
	counter = fClus->getPadsRing(i);

	if(counter>0) {
	    // Calculate cluster information only if at least one hit was found
	    fClus -> calcCluster(i);
	    trackInSec[candidates[i].getSector()] = kTRUE;
        }
    }


    fSorter.restoreFlags(kTRUE); // restore only flags not backup

    if(!fRing->hasNoisyRichSeg(trackInSec))
	fillOutput(candidates);
    fRing -> clearData();
    fClus -> clearCluster();

    return EXIT_SUCCESS;
}

void HParticleBt::fillOutput(vector <HParticleCand> &candidates)
{
    Int_t size = candidates.size();
    HParticleBtRing*     particleBtOut      = 0;
    HParticleBtRingInfo* particleBtRingInfo = 0;
    HParticleCand*       cand               = 0;
    Int_t                trackCounter       = 0;

    //Fill user output
    const   Int_t shareLimit                    = 10;
    Int_t   sharedMaxima[shareLimit]            = {0,0,0,0,0,0,0,0,0,0};
    Int_t   sharedMaximaBad[shareLimit]         = {0,0,0,0,0,0,0,0,0,0};
    Float_t sharedMaximaCharge[shareLimit]      = {0,0,0,0,0,0,0,0,0,0};
    Float_t sharedMaximaChargeBad[shareLimit]   = {0,0,0,0,0,0,0,0,0,0};
    Int_t   sharedMaximaNearby[shareLimit]      = {0,0,0,0,0,0,0,0,0,0};

    Int_t   sharedMaximaIdx[shareLimit]         = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    Int_t   sharedMaximaBadIdx[shareLimit]      = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    Int_t   sharedMaximaChargeIdx[shareLimit]   = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    Int_t   sharedMaximaChargeBadIdx[shareLimit]= {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
    Int_t   sharedMaximaNearbyIdx[shareLimit]   = {-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};

    Int_t   sharedTmp                           = 0;
    Int_t   sharedTmpBad                        = 0;
    Float_t sharedTmpCharge                     = 0;
    Float_t sharedTmpChargeBad                  = 0;
    Int_t   sharedTmpNearby                     = 0;
    Int_t   counterShared                       = 0;
    Int_t   counterSharedBad                    = 0;
    Int_t   counterSharedCharge                 = 0;
    Int_t   counterSharedChargeBad              = 0;
    Int_t   counterSharedNearby                 = 0;





    for(Int_t trackNo=0; trackNo < size; trackNo++){
	if(fClus->getPadsRing(trackNo) > 0 ) {
	    for(Int_t i = 0; i < shareLimit; i++){
		sharedMaxima[i]            = 0;
		sharedMaximaBad[i]         = 0;
                sharedMaximaCharge[i]      = 0.;
		sharedMaximaChargeBad[i]   = 0.;
	        sharedMaximaNearby[i]      = 0;

		sharedMaximaIdx[i]         = -1;
	        sharedMaximaBadIdx[i]      = -1;
	        sharedMaximaChargeIdx[i]   = -1;
	        sharedMaximaChargeBadIdx[i]= -1;
	        sharedMaximaNearbyIdx[i]   = -1;
	    }
	    sharedTmp               = 0;
	    sharedTmpBad            = 0;
	    sharedTmpCharge         = 0.;
            sharedTmpChargeBad      = 0.;
	    sharedTmpNearby         = 0;

	    counterShared           = 0;
	    counterSharedBad        = 0;
	    counterSharedCharge     = 0;
	    counterSharedChargeBad  = 0;
	    counterSharedNearby     = 0;

	    for(Int_t i = 0; i < size; i ++){
		if(i == trackNo)
                    continue;
	        sharedTmp           = fClus->getMaximaSharedTrack(trackNo,i);
	        sharedTmpBad        = fClus->getMaximaSharedBadTrack(trackNo,i);
	        sharedTmpCharge     = fClus->getMaximaChargeSharedTrack(trackNo,i);
	        sharedTmpChargeBad  = fClus->getMaximaChargeSharedBadTrack(trackNo,i);
	        sharedTmpNearby     = fClus->getNearbyMaximaSharedTrack(trackNo,i);
		if(sharedTmp > 0){
		    if(counterShared  < shareLimit){
			sharedMaxima[counterShared]    = sharedTmp;
			sharedMaximaIdx[counterShared] = candidates[i].getIndex();
		    
		    }
		    counterShared++;
		}
                if(sharedTmpBad > 0){
		    if(counterSharedBad  < shareLimit){
			sharedMaximaBad[counterSharedBad]    = sharedTmpBad;
			sharedMaximaBadIdx[counterSharedBad] = candidates[i].getIndex();
		    }
		    counterSharedBad++;
		}
		if(sharedTmpCharge > 0){
		    if(counterSharedCharge  < shareLimit){
                	sharedMaximaCharge[counterSharedCharge]    = sharedTmpCharge;
			sharedMaximaChargeIdx[counterSharedCharge] = candidates[i].getIndex();
	            }
		    counterSharedCharge++;
		}
	        if(sharedTmpChargeBad > 0){
		    if(counterSharedChargeBad  < shareLimit){
			sharedMaximaChargeBad[counterSharedChargeBad]    = sharedTmpChargeBad;
			sharedMaximaChargeBadIdx[counterSharedChargeBad] = candidates[i].getIndex();
		    }
		    counterSharedChargeBad++;
		}
	        if(sharedTmpNearby > 0){
		    if( counterSharedNearby < shareLimit){
			sharedMaximaNearby[counterSharedNearby]    = sharedTmpNearby;
			sharedMaximaNearbyIdx[counterSharedNearby] = candidates[i].getIndex();
		    }
		    counterSharedNearby++;
		}
	    }
	    //Set -1 if Maxima are shared with more than 5 tracks --> Bad event
	    if(counterShared>shareLimit){
		for(Int_t i=0; i < shareLimit; i++){
		    sharedMaxima[i]    = -1;
		    sharedMaximaIdx[i] = -1;
		}
	    }
	    if(counterSharedBad>shareLimit){
		for(Int_t i=0; i < shareLimit; i++){
		    sharedMaximaBad[i]    = -1;
		    sharedMaximaBadIdx[i] = -1;
		}
	    }
	    if(counterSharedCharge>shareLimit){
		for(Int_t i=0; i < shareLimit; i++){
		    sharedMaximaCharge[i]    = -1;
		    sharedMaximaChargeIdx[i] = -1;
		}
	    }
	    if(counterSharedChargeBad>shareLimit){
		for(Int_t i=0; i < shareLimit; i++){
		    sharedMaximaChargeBad[i]    = -1;
		    sharedMaximaChargeBadIdx[i] = -1;
		}
	    }
	    if(counterSharedNearby>shareLimit){
		for(Int_t i=0; i < shareLimit; i++){
		    sharedMaximaNearby[i]    = -1;
		    sharedMaximaNearbyIdx[i] = -1;
		}
	    }

	    // fill output category
	    Int_t index = -1;
	    if((particleBtOut = HCategoryManager::newObject(particleBtOut,fParticleBtOutCat,index)) != 0 )
	    {
		particleBtOut = new (particleBtOut) HParticleBtRing;

		particleBtOut->setPadsRing(     fClus->getPadsRing(trackNo)  );
		particleBtOut->setPadsClus(      fClus->getPadsSum(trackNo)   );
		particleBtOut->setChargeRing(   fClus->getChargeRing(trackNo));
		particleBtOut->setChargeClus(    fClus->getChargeSum(trackNo) );

		particleBtOut->setClusters(     fClus->getNClusters(trackNo)   );
		particleBtOut->setMaxima(       fClus->getMaxima(trackNo)      );
		particleBtOut->setMaximaCharge( fClus->getMaximaCharge(trackNo));
		particleBtOut->setNearbyMaxima( fClus->getNearbyMaxima(trackNo));

		particleBtOut->setChi2(         fClus->getChi2Value(trackNo)       );
	        particleBtOut->setRingMatrix(   fRing->getRingMatrix(trackNo)      );
		particleBtOut->setMeanDist(     fClus->getMeanDist(trackCounter)   );
         
                particleBtOut->setMaximaShared(                       fClus->getMaximaShared(trackNo)      );
	        particleBtOut->setMaximaSharedFragment(               fClus->getMaximaSharedBad(trackNo)      );
	        particleBtOut->setMaximaChargeShared(                 fClus->getMaximaChargeShared(trackNo)      );
	        particleBtOut->setMaximaChargeSharedFragment(         fClus->getMaximaChargeSharedBad(trackNo)      );
	        particleBtOut->setNearbyMaximaShared(                 fClus->getNearbyMaximaShared(trackNo));
	        particleBtOut->setMaximaSharedTrack(                  sharedMaxima         );
                particleBtOut->setMaximaSharedTrackIdx(               sharedMaximaIdx      );
	        particleBtOut->setMaximaSharedFragmentTrack(          sharedMaximaBad      );
                particleBtOut->setMaximaSharedFragmentTrackIdx(       sharedMaximaBadIdx   );
	        particleBtOut->setMaximaChargeSharedTrack(            sharedMaximaCharge   );
                particleBtOut->setMaximaChargeSharedTrackIdx(         sharedMaximaChargeIdx);
	        particleBtOut->setMaximaChargeSharedFragmentTrack(    sharedMaximaChargeBad   );
                particleBtOut->setMaximaChargeSharedFragmentTrackIdx( sharedMaximaChargeBadIdx);
	        particleBtOut->setNearbyMaximaSharedTrack(            sharedMaximaNearby   );
                particleBtOut->setNearbyMaximaSharedTrackIdx(         sharedMaximaNearbyIdx);

		cand = HCategoryManager::getObject(cand,fCandCat,candidates[trackNo].getIndex());
                cand->setRichBTInd(index);
                trackCounter++;
	    }
	    else
	    {
		Error( "execute", "Couldn't get a new slot from 'catParticleBtRing'!" );
		exit( 2 );
	    }
	}
    }

    if(fDebugInfo){
	//Fill RingInfo category
	Int_t index2 = -1;
	Bool_t clusterInfoFilled = kFALSE;
	Bool_t ringInfoFilled = kFALSE;
	HParticleBtRingInfo ringInfoTest;
	ringInfoFilled      = fRing->fillRingInfo(&ringInfoTest);
        clusterInfoFilled   = fClus->fillRingInfo(&ringInfoTest);

	if((particleBtRingInfo = HCategoryManager::newObject(particleBtRingInfo,fParticleBtRingInfoCat,index2)) != 0 ){
	    particleBtRingInfo = new (particleBtRingInfo) HParticleBtRingInfo(ringInfoTest);
	}
	else
	{
	    Warning( "execute", "Couldn't get a new slot from 'catParticleBtRingInfo'!" );
	    exit( 2 );
	}

      
	if(clusterInfoFilled || ringInfoFilled )
	{
	    Warning( "execute", "'catParticleBtRingInfo' array limit is reached! Ring information is not stored." );

	}
    }
}






