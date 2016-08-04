//*-- AUTHOR : Jochen Markert

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcDeDx2Maker
//
// This transformation class calculates the pseudo dEdx from t2-t1 (time above threshold)
// of all fired drift cells included in one HMdcSeg. The transformation is performed using
// the parameter container HMdcDEdX2, which holds the parameters for normalization of the
// measured t2-t1 with impact angle and minimum distance to wire (CAL2 parametrization)
// and the algorithm to normalize the single measurements and average over all cells included
// in the segment. Inside the execute() function a loop over the category of HMdcTrkCand is
// performed and the dEdx calculation is done for each inner and outer HMdcSeg separately.
// The result is filled to HMdcTrkCand.
// Explanations can be looked up in HMdcDeDX2.
//
// Since the filling of dEdx is compute intensive, by default only the combined
// dEdx for inner+outer MDC segment is filled (used by dst production). Additional
// dEdx can be calculated per segment and Mdc module. All dEdx values will be stored
// in HMdcTrkCand.
//
// static setUseModule(Int_t module);   switch for caldedx()  = 0 first mod in seg,
//                                                            = 1 second mod in seg,
//                                                            = 2 both mods in seg (default)
// static setFillCase(In_t case);   0 = combined (default) ,
//                                  1 = combined + seg,
//                                  2 = combined + seg + mod
//
// The functions above have to called after creating the task.
///////////////////////////////////////////////////////////////////////////////
#include "hmdcdedx2maker.h"
#include "hmdcdedx2.h"
#include "hmdcseg.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrkcand.h"
#include "hmdcclusinf.h"
#include "hmdcclus.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hcategory.h"
#include "hmessagemgr.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
using namespace std;

ClassImp(HMdcDeDx2Maker)


Int_t   HMdcDeDx2Maker::module=2;
Int_t   HMdcDeDx2Maker::fillCase=2;


HMdcDeDx2Maker::HMdcDeDx2Maker(void) {
  // Default constructor .
  initParameters();
}
HMdcDeDx2Maker::HMdcDeDx2Maker(const Text_t* name,const Text_t* title)
                 :  HReconstructor(name,title) {
  // Constructor calls the constructor of class HReconstructor with the name
  // and the title as arguments.
  initParameters();
}

HMdcDeDx2Maker::~HMdcDeDx2Maker(void) {
  // destructor deletes the iterator
  if (trkcanditer) delete trkcanditer;
  trkcanditer = 0;
}
void HMdcDeDx2Maker::initParameters()
{
    trkcandCat = 0;
    segCat     = 0;
    clsCat     = 0;
    clsInfCat  = 0;
    cal1Cat    = 0;
    trkcanditer= 0;
    mdcdedx    = 0;
    hasPrinted = kFALSE;
    module     = 2;
    fillCase   = 2;
}
void HMdcDeDx2Maker::setParContainers() {
  // creates the parameter containers MdcDeDx if they do not
  // exist and adds them to the list of parameter containers in the runtime
  // database
    mdcdedx = (HMdcDeDx2*)(((HRuntimeDb*)(gHades->getRuntimeDb()))->getContainer("MdcDeDx2"));
    if(!mdcdedx) {
	    Error("HMdcDeDx2Maker:setParContainers()","RETRIEVED 0 POINTER FOR HMDCDEDX!");
	    exit(1);
    }else{
	Bool_t ok = mdcdedx->initContainer();
	if(!ok){
	    Error("HMdcDeDx2Maker:setParContainers()","INIT OF HMDCDEDX FAILED!");
	    exit(1);
	}
    }
}
Bool_t HMdcDeDx2Maker::init(void) {
    // gets the pointers to HMdcTrkC and HMdcSeg categories
    // calls the function setParContainers()
    setParContainers();
    trkcandCat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcTrkCand));
    if (!trkcandCat) {
	Error("HMdcDeDx2Maker:init()","HMdcTrkCand Category not available!");
	exit(1);
    }
    segCat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcSeg));
    if (!segCat) {
	Error("HMdcDeDx2Maker:init()","HMdcSeg Category not available!");
	exit(1);
    }
    clsCat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcClus));
    if (!clsCat) {
	Error("HMdcDeDx2Maker:init()","HMdcClus Category not available!");
	exit(1);
    }
    clsInfCat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcClusInf));
    if (!clsInfCat) {
	Error("HMdcDeDx2Maker:init()","HMdcClusInf Category not available!");
	exit(1);
    }
    cal1Cat = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcCal1));
    if (!cal1Cat) {
	Error("HMdcDeDx2Maker:init()","HMdcCal1 Category not available!");
	exit(1);
    }

    trkcanditer = (HIterator *)((HCategory*)trkcandCat)->MakeIterator("native");

    if(!hasPrinted)printStatus();
    fActive = kTRUE;
    return kTRUE;
}
void HMdcDeDx2Maker::printStatus()
{
    // prints the parameters to the screen
    SEPERATOR_msg("-",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcDeDx2MakerSetup:");

    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"Input Mode      =  %i :  0 = from 1. MDC in seg, 1 = from 2. MDC in seg , 2 = from both MDC in seg",module);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"Fill Mode       =  %i :  0 = combined, 1 = combined+seg, 2 = combined+seg+mod",fillCase);

    SEPERATOR_msg("-",60);
    hasPrinted = kTRUE;
}
Int_t HMdcDeDx2Maker::execute(void)
{

    // calculates dEdx for all Segments contained in HMdcTrkCand.
    // The result is filled to HMdctrk.

    if(trkcandCat->getEntries() < 0) return 0;
    if(segCat    ->getEntries() < 0) return 0;
    if(clsCat    ->getEntries() < 0) return 0;
    if(clsInfCat ->getEntries() < 0) return 0;
    if(cal1Cat   ->getEntries() < 0) return 0;

    HMdcTrkCand* trkcand;

    trkcanditer->Reset();

    Float_t dedx   ;
    Float_t dedxold;
    Float_t sig    ;
    Float_t sigold ;
    UChar_t nw     ;
    UChar_t nwcut  ;

    HMdcSeg* seg[2] = {0};

    Int_t n = trkcandCat->getEntries();
    for(Int_t i=0;i<n;i++){
	trkcand = (HMdcTrkCand *) trkcandCat->getObject(i);

	seg[0] = seg[1] = 0;
	if(trkcand->getSeg1Ind() != -1) { seg[0] = (HMdcSeg*)segCat->getObject(trkcand->getSeg1Ind()); }
	if(trkcand->getSeg2Ind() != -1) { seg[1] = (HMdcSeg*)segCat->getObject(trkcand->getSeg2Ind()); }

	// fill case 0 (always) : combined
        dedx = mdcdedx->calcDeDx(seg,&dedxold,&sigold,&nw,&sig,&nwcut,2,module,kTRUE,-99.);

	if(dedx > -98.){
	    trkcand->setdedxSeg        (2,dedx );
	    trkcand->setSigmadedxSeg   (2,sig  );
	    trkcand->setNWirededxSeg   (2,nw   );
	    trkcand->setNWireCutdedxSeg(2,nwcut);
	}

	if(fillCase > 0){ // case 1: combined + seg
	    //-----------------------------------------------------
	    // inner seg
	    dedx = mdcdedx->calcDeDx(seg,&dedxold,&sigold,&nw,&sig,&nwcut,0,module,kTRUE,-99.);

	    if(dedx > -98.){
		trkcand->setdedxSeg        (0,dedx );
		trkcand->setSigmadedxSeg   (0,sig  );
		trkcand->setNWirededxSeg   (0,nw   );
		trkcand->setNWireCutdedxSeg(0,nwcut);
	    }
	    // outer seg
	    dedx = mdcdedx->calcDeDx(seg,&dedxold,&sigold,&nw,&sig,&nwcut,1,module,kTRUE,-99.);

	    if(dedx > -98.){
		trkcand->setdedxSeg        (1,dedx );
		trkcand->setSigmadedxSeg   (1,sig  );
		trkcand->setNWirededxSeg   (1,nw   );
		trkcand->setNWireCutdedxSeg(1,nwcut);
	    }
	}

	if(fillCase > 1 ) // case 2: combined + seg + module
	{
	    // per module
	    Float_t dedxM    [4];
	    Float_t dedxOldM [4];
	    Float_t sigM     [4];
	    Float_t sigOldM  [4];
	    UChar_t nwM      [4];
	    UChar_t nwCutM   [4];

	    for(Int_t m = 0; m < 4; m ++){
		if(seg[0] != 0 && m < 2){  // inner seg
		    dedxM[m] = mdcdedx->calcDeDx(seg,&dedxOldM[m],&sigOldM[m],&nwM[m],&sigM[m],&nwCutM[m],0,m%2,kTRUE,-99.);
		    if(dedx > -98.){
			trkcand->setdedx        (m,dedxM [m]);
			trkcand->setSigmadedx   (m,sigM  [m]);
			trkcand->setNWirededx   (m,nwM   [m]);
			trkcand->setNWireCutdedx(m,nwCutM[m]);
		    }
		}
		if(seg[1] != 0 && m > 1){ // outer seg
		    dedxM[m] = mdcdedx->calcDeDx(seg,&dedxOldM[m],&sigOldM[m],&nwM[m],&sigM[m],&nwCutM[m],1,m%2,kTRUE,-99.);
		    if(dedx > -98.){
			trkcand->setdedx        (m,dedxM [m]);
			trkcand->setSigmadedx   (m,sigM  [m]);
			trkcand->setNWirededx   (m,nwM   [m]);
			trkcand->setNWireCutdedx(m,nwCutM[m]);
		    }
		}
	    }
	}
	//-----------------------------------------------------


    }

    return 0;
}
Bool_t HMdcDeDx2Maker::finalize()
{
    return kTRUE;
}
