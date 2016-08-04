

#include "hparticlecand.h"
#include "hcategorymanager.h"
#include "hcategory.h"

#include "hparticletree.h"
#include "htime.h"

#include "hparticledef.h"
#include "hstartdef.h"
#include "walldef.h"
#include "richdef.h"
#include "showerdef.h"
#include "hstartdef.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"


#include "TObjArray.h"
#include "TSystem.h"

Bool_t selectHadronsTreeFilter(HParticleCand*){
   // we want only leptons in this case
   return kFALSE;
}

Bool_t selectLeptonsTreeFilter(HParticleCand* pcand)
{
    // build in selection function for lepton candidates.
    // Requires besides an RICH hit, RK + META and fitted
    // inner+outer segment.

    Bool_t test = kFALSE;

    

    Double_t beta = pcand->getBeta();
    Double_t mom  = pcand->getMomentum();
    Int_t system  = pcand->getSystemUsed();

    if (mom > 1500) return test;

    if((system == 0 && beta > 0.94 &&  mom < 200) ||
       (system == 1 && beta > 0.96 &&  mom < 250)
      )
    {   // for low momenta do not use RICH
	test = pcand->isFlagAND(4,
				Particle::kIsAcceptedHitInnerMDC,
				Particle::kIsAcceptedHitOuterMDC,
				Particle::kIsAcceptedHitMETA,
				Particle::kIsAcceptedRK
			       );
    } else { // for large momenta check RICH

	test = pcand->isFlagAND(5,
				Particle::kIsAcceptedHitRICH,
				Particle::kIsAcceptedHitInnerMDC,
				Particle::kIsAcceptedHitOuterMDC,
				Particle::kIsAcceptedHitMETA,
				Particle::kIsAcceptedRK
			       );

	if(test) test = fabs(pcand->getDeltaTheta()) < 4 ? kTRUE : kFALSE ;
	if(test) test = fabs(pcand->getDeltaPhi())   < 4 ? kTRUE : kFALSE ;
    }

    if(test) test = pcand->getChi2() < 1000 ? kTRUE: kFALSE;

    if(!test) return kFALSE;

    if(test) test = pcand->getBeta() > 0.9          ? kTRUE : kFALSE ;
    if(test) test = pcand->getMetaMatchQuality() < 3 ? kTRUE : kFALSE ;

    return test;
}

Bool_t selectLeptonsTreeFilter_old(HParticleCand* pcand){

    //  selection function for lepton candidates.
    Bool_t select = kFALSE;
    if(pcand->isFlagAND(5,
			Particle::kIsAcceptedHitRICH,
			Particle::kIsAcceptedHitInnerMDC,
			Particle::kIsAcceptedHitOuterMDC,
			Particle::kIsAcceptedHitMETA,
			Particle::kIsAcceptedRK)
       &&
       pcand->getBeta() > 0.9
       &&
       pcand->getChi2() < 1000) select = kTRUE;

    if(select) select = fabs(pcand->getDeltaTheta()) < 4    ? kTRUE : kFALSE ;
    if(select) select = fabs(pcand->getDeltaPhi())   < 4    ? kTRUE : kFALSE ;
    if(select) select = pcand->getMomentum()         < 1500 ? kTRUE : kFALSE ;


   return select;
}

Bool_t selectEventTreeFilter(TObjArray* ar){

    HCategory* catCand = gHades->getCurrentEvent()->getCategory(catParticleCand);
    if(catCand){
	Int_t n=catCand->getEntries();
	HParticleCand* cand=0;
	Int_t nLepton = 0;
	Int_t nEle = 0;
	Int_t nPos = 0;
	    for(Int_t i = 0; i < n; i ++){
		cand = HCategoryManager::getObject(cand,catCand,i);
		if(cand){
		    if(cand->isFlagBit(kIsLepton)){
			nLepton++;
			if      (cand->getCharge() == -1 ) nEle++;
			else if (cand->getCharge() ==  1 ) nPos++;
		    }
		}
	    }
	    if(nPos < 1 && nEle < 1) return kFALSE;  // select single lepton

    } else return kFALSE;

    return kTRUE;
}

void addFilter(HTaskSet *masterTaskSet, TString inFile,TString outDir)
{

    TString dir    = gSystem->DirName (inFile);
    //TString file   = HTime::stripFileName(inFile);
    TString file   = gSystem->BaseName (inFile);

    TString fileWoPath = file;
    fileWoPath.ReplaceAll(".hld","");
    Int_t evtBuild = HTime::getEvtBuilderFileName(fileWoPath,kFALSE);

    HParticleTree* parttree = new HParticleTree("particletree_small","particletree_small");


//       SUPPORTED CATEGORIES:
//
//       catParticleCand
//       catParticleEvtInfo (fullcopy)
//       catStart2Hit       (fullcopy)
//       catStart2Cal       (fullcopy)
//       catTBoxChan        (fullcopy)
//       catWallHit         (fullcopy)
//       catTofHit          (opt full)
//       catTofCluster      (opt full)
//       catRpcCluster      (opt full)
//       catShowerHit       (opt full)
//       catRichHit         (opt full)
//       catRichDirClus     (fullcopy)
//       catRichCal         (fullcopy)
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


    //------Long-List-Of-Categories-which-are-needed-in-this-DST-production------//
    //----------------------------------------------------------------------------//
    Cat_t pPersistentCatAll[] =
    {
	//catRichDirClus,catRichHit, catRichCal,  // full copy
	//catMdcCal1,
	catShowerHit,
	catTofHit, catTofCluster,
	catRpcHit, catRpcCluster,
	catParticleCand, catParticleEvtInfo,
	catWallHit,
	catStart2Cal, catStart2Hit,
	catTBoxChan
    };

    //------Short-List-Of-Categories-which-are-needed-in-this-DST-production------//
    //----------------------------------------------------------------------------//
    Cat_t pPersistentCat[] =
    {
	//catRichDirClus,catRichHit, catRichCal,  // full copy
	//catMdcCal1,
	//catShowerHit,
	//catTofHit, catTofCluster,
	//catRpcHit, catRpcCluster,
	catParticleCand, catParticleEvtInfo,
	catWallHit,
	catStart2Cal, catStart2Hit,
	catTBoxChan
    };

    //------lsit of full copy categories------------------------------------------//
    //----------------------------------------------------------------------------//
    Cat_t pPersistentCatFull[] =
    {
	catRichDirClus,catRichHit,catRichCal
    };


    if(evtBuild == 1) {
	parttree->setEventStructure(sizeof(pPersistentCatAll)/sizeof(Cat_t) ,pPersistentCatAll);
        parttree->setEventStructure(sizeof(pPersistentCatFull)/sizeof(Cat_t),pPersistentCatFull,kTRUE);     // add more
    } else {
	parttree->setEventStructure(sizeof(pPersistentCat)/sizeof(Cat_t)    ,pPersistentCat);
        //parttree->setEventStructure(sizeof(pPersistentCatFull)/sizeof(Cat_t),pPersistentCatFull,kTRUE);
    }
    parttree->setSkipEmptyEvents(kTRUE);
    parttree->setUserSelectionHadrons(selectHadronsTreeFilter);
    parttree->setUserSelectionLeptons(selectLeptonsTreeFilter);
    TString outfile = Form("%s/%s_filter_tree.root",outDir.Data(),file.Data());
    outfile.ReplaceAll("//","/");
    parttree->setUserSelectionEvent(selectEventTreeFilter,NULL);
    parttree->setOutputFile(outfile,"Filter","RECREATE",2 );
    masterTaskSet->add(parttree);
}












