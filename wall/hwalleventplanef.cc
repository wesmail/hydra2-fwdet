#include "hwalleventplanef.h"
#include "walldef.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hrecevent.h"
#include "hwallhit.h"
#include "hwalleventplane.h"
#include "hwalleventplanepar.h"

#include "TRandom.h"
#include "TFile.h"
#include "TH1F.h"
#include "TMath.h"
#include "TVector2.h"

using namespace std;

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////
//
//  HWallEventPlanef resonstructes the eventplane by HWallHits
//
/////////////////////////////////////////////////////////////////////

ClassImp(HWallEventPlaneF)

Bool_t HWallEventPlaneF::fUseCorrection = kTRUE;

HWallEventPlaneF::HWallEventPlaneF(const Text_t *name,const Text_t *title)
    : HReconstructor(name,title) {
	fWallHitCat         = NULL;
	fWallEventPlaneCat  = NULL;
	fWallEventPlanePar  = NULL;
}

HWallEventPlaneF::~HWallEventPlaneF()
{

}

Bool_t HWallEventPlaneF::init(void)
{
    fWallHitCat = HCategoryManager::getCategory(catWallHit,kTRUE,"catWallHit");
    if (!fWallHitCat) {
	Warning("init()","catWallHit not available!");
    }
    fWallEventPlaneCat = HCategoryManager::addCategory(catWallEventPlane,"HWallEventPlane",10,"Wall",kFALSE);

    if(fUseCorrection) fWallEventPlanePar = (HWallEventPlanePar*)gHades->getRuntimeDb()->getContainer("WallEventPlanePar");
    else               fWallEventPlanePar = NULL;


    return kTRUE;
}


Int_t HWallEventPlaneF::execute(void)
{

    if(!fWallHitCat) return 0;

    Float_t T1_cut   = 0.0;
    Float_t T2_cut   = 0.0;
    Float_t X_shift  = 0.0;
    Float_t Y_shift  = 0.0;
    //Float_t R0_cut   = 0.0;
    Float_t Z1_cut_s = 0.0;
    Float_t Z1_cut_m = 0.0;
    Float_t Z1_cut_l = 0.0;

    //------------------------------------------------------------
    // retrieve correction parameters if available
    if(fWallEventPlanePar) {
	T1_cut   = fWallEventPlanePar->getT1Cut();
	T2_cut   = fWallEventPlanePar->getT2Cut();
	X_shift  = fWallEventPlanePar->getXShift();
	Y_shift  = fWallEventPlanePar->getYShift();
	//R0_cut   = fWallEventPlanePar->getR0Cut();
	Z1_cut_s = fWallEventPlanePar->getZ1_cut_s();
	Z1_cut_m = fWallEventPlanePar->getZ1_cut_m();
	Z1_cut_l = fWallEventPlanePar->getZ1_cut_l();
    }
    //------------------------------------------------------------

    //------------------------------------------------------------
    // loop over wall hits and select good cells for eventplane
    Int_t   multWall   =   0;
    Int_t   cellNum    =   0;
    Float_t cellTime   = 0.0;
    Float_t cellCharge = 0.0;
    Float_t dEdxCut    = 0.0;
    Float_t wallX,wallY,wallZ;
    Float_t wallXc, wallYc; //corrected reCentered
    TVector2 vect(0.,0.);
    TVector2 vsum(0.,0.);
    TVector2   eX(1.,0.);

    fcellsVect.reset(); //

    HWallHit* pWallHit  = NULL;
    for(Int_t i = 0; i < fWallHitCat->getEntries(); i ++ ){
	pWallHit   = HCategoryManager::getObject(pWallHit,fWallHitCat,i);
	cellNum    = pWallHit->getCell();
	cellTime   = pWallHit->getTime();
	cellCharge = pWallHit->getCharge();

	if(cellNum <  144                  ){ dEdxCut = Z1_cut_s; }
	if(cellNum >= 144 && cellNum <= 207){ dEdxCut = Z1_cut_m; }
	if(cellNum >  207                  ){ dEdxCut = Z1_cut_l; }

	if(cellTime > T1_cut && cellTime < T2_cut && cellCharge > dEdxCut){
	    // spectators selected
	    multWall++;

	    pWallHit->getXYZLab(wallX,wallY,wallZ);
	    wallXc = wallX - X_shift;
	    wallYc = wallY - Y_shift;


	    vect.Set(wallXc, wallYc);
	    vect =  vect.Unit();
	    vsum += vect;

	    fcellsVect.setCellVect(vect);

	}
    }
    //------------------------------------------------------------


    //------------------------------------------------------------
    // now we go over spectators and make calculations for A/B subevents
    Int_t NA      = 0;
    Int_t NB      = 0;
    Int_t choiceA = 1;  //
    Int_t choiceB = 1;  //
    Int_t multFWcells = fcellsVect.getNumbOfCells();

    Float_t choice;
    TVector2 vectA(0.,0.), vectB(0.,0.);
    TVector2 vsumA(0.,0.), vsumB(0.,0.);

    for(Int_t ic = 0; ic < multFWcells; ic++){
	choice = gRandom->Rndm();
	// I can be proud that my algorithm has more flat distribution :)
	Float_t levelA = (multFWcells/2.-choiceA+1.)/Float_t(multFWcells);
	Float_t levelB = (multFWcells/2.-choiceB+1.)/Float_t(multFWcells);
	if(choice < levelA/(levelA+levelB)){
	    vectA = fcellsVect.getCellVector(ic);
	    vsumA += vectA;
	    choiceA++;
	    NA++;
	} else {
	    vectB = fcellsVect.getCellVector(ic);
	    vsumB += vectB;
	    choiceB++;
	    NB++;
	}
    }
    //------------------------------------------------------------


    //------------------------------------------------------------
    // calculating eventplane angles
    Float_t phiEP  =  vsum.DeltaPhi(eX)   *TMath::RadToDeg();
    Float_t phiA   = vsumA.DeltaPhi(eX)   *TMath::RadToDeg();
    Float_t phiB   = vsumB.DeltaPhi(eX)   *TMath::RadToDeg();
    Float_t phiAB  = vsumA.DeltaPhi(vsumB)*TMath::RadToDeg();

    if(multFWcells == 0)   phiEP = -1000.;
    if(NA == 0)            phiA  = -1000.;
    if(NB == 0)            phiB  = -1000.;
    if(NB == 0 || NA == 0) phiAB = -1000.;


    //------------------------------------------------------------

    //------------------------------------------------------------
    // fill output object
    if(fWallEventPlaneCat){
	HWallEventPlane* eventplane = 0;
	Int_t index = 0;
	eventplane = HCategoryManager::newObject(eventplane,fWallEventPlaneCat,index);
	if(eventplane){
	    eventplane->setPhi ( phiEP);
	    eventplane->setPhiA( phiA );
	    eventplane->setPhiB( phiB );
	    eventplane->setPhiAB(phiAB);
	    eventplane->setNA(NA);
	    eventplane->setNB(NB);
	}
    }
    //------------------------------------------------------------


    return 0;
}











