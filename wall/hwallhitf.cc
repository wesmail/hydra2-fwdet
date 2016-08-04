using namespace std;
#include "TRandom.h"
#include <time.h>
#include <iostream>
#include <iomanip>
#include "hwallhitf.h" //FK//
#include "walldef.h"   //FK//
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hwalldetector.h" //FK//
#include "hwallraw.h"   //FK//
#include "hevent.h"
#include "hpartialevent.h"
#include "hcategory.h"
#include "hmatrixcategory.h"
#include "hrecevent.h"
#include "hlocation.h"
#include "hwallcalpar.h" //FK//
#include "hwallhit.h"   //FK//
#include "hwallonehit.h"   //FK//
#include "hwallonehitf.h"   //FK//
#include "hstartdef.h"
#include "hstart2hit.h"
#include "hwallgeompar.h"        //FK//
#include "hdetgeompar.h"         //FK//
#include "hgeomvector.h"         //FK//
#include "hspecgeompar.h"        //FK//
#include "hgeomvolume.h"           //FK//
#include "hgeomcompositevolume.h"  //FK//
#include "hgeomtransform.h"        //FK//

//*-- Created : 24/06/2000 by M.Golubeva
//*-- Modified : 27/04/2001 by T.Wojcik
//*-- Modified: 11/8/2005 by F. Krizek
// Modified by M.Golubeva 01.11.2006
// Modified by A.Sadovsky 02.12.2007

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////
//
//  HWallHitf calibrates raw Wall data and puts output
//  values into cal data category
//
/////////////////////////////////////////////////////////////////////

void HWallHitF::initParContainer() {

    pWallGeometry = (HWallGeomPar *)gHades->getRuntimeDb()->getContainer("WallGeomPar");
    pSpecGeometry = (HSpecGeomPar *)gHades->getRuntimeDb()->getContainer("SpecGeomPar");
}

HWallHitF::HWallHitF(void) {

    fHitCat       = NULL;
    fRawCat       = NULL;
    fOneHitCat    = NULL;
    fStartHitCat  = NULL;
    pWallGeometry = NULL;
    fLoc.set(1,-1);
    iter          = NULL;

}

HWallHitF::HWallHitF(const Text_t *name,const Text_t *title) :
HReconstructor(name,title) {

    fHitCat       = NULL;
    fRawCat       = NULL;
    fOneHitCat    = NULL;
    fStartHitCat  = NULL;
    pWallGeometry = NULL;
    pSpecGeometry = NULL;
    fLoc.set(1,-1);
    iter          = NULL;
}

HWallHitF::~HWallHitF(){
    if(iter){
	delete iter;
	iter = NULL;
    }
}

void HWallHitF::fillHit(HWallHit *hit, HWallOneHit *onehit)
{

}

Bool_t HWallHitF::init(void)
{

    initParContainer();

    fOneHitCat=gHades->getCurrentEvent()->getCategory(catWallOneHit);
    if (!fOneHitCat) return kFALSE;

    fHitCat = gHades->getCurrentEvent()->getCategory(catWallHit);
    if (!fHitCat) {
	fHitCat = gHades->getSetup()->getDetector("Wall")
	    ->buildCategory(catWallHit);

	if (!fHitCat)
	    return kFALSE;
	else
	    gHades->getCurrentEvent()->addCategory(catWallHit,fHitCat,"Wall");
    }

    //-- Try Start Hit Category, if exists, use the start time --//
    //-- if it does not exist assume that start time iz zero ----//

    fStartHitCat = gHades->getCurrentEvent()->getCategory(catStart2Hit);
    if (!fStartHitCat){
	Warning("init","Start Hit level not defined: setting start time to 0");
	fStartHitCat = 0;
    }

    iter = (HIterator *)fOneHitCat->MakeIterator("native");

    return kTRUE;
}

void HWallHitF::fillGeometry(HWallHit *hit)
{
    // Fills in the LAB position parameters for the given HTofHit.
    //
    // This is done by transforming the point (x,0,0) in the ROD coordinate
    // system to the LAB coordinate system. Where x is the reconstructed
    // x position inside the hit.

    HGeomVector rLocal,r;
    Float_t d,phi,theta,xx,yy,zz;
    Float_t rad2deg = 180./TMath::Pi();

    HModGeomPar *module       = pWallGeometry->getModule(0);
    HGeomTransform &trans     = module->getLabTransform();
    HGeomVolume *cellVol      = module->getRefVolume()->getComponent(hit->getCell());
    HGeomTransform &cellTrans = cellVol->getTransform();

    // Fill r with the hit coordinates in the cell coordinate system
    // Since we do not have information about x,y,z coordinates of impact
    //  hi occures in centre thus z=0, x=0 and y=0.
    //  Note that (0,0,0) corresponds to the rod center.

    r.setX(0.);
    r.setY(0.);
    r.setZ(0.);

    rLocal = cellTrans.transFrom(r);  // transform to module coordinate system
    r      = trans.transFrom(rLocal); // transform from module to LAB system

    HGeomVolume *tv = pSpecGeometry->getTarget(0);
    if (tv) r      -= tv->getTransform().getTransVector();   // correct for target position

    // Fill output
    d = r.length();
    theta = (d>0.) ? (rad2deg * TMath::ACos(r.getZ() / d)) : 0.;
    phi   = rad2deg * TMath::ATan2( r.getY(), r.getX());
    if (phi < 0.) phi += 360.;
    if (tv)         r += tv->getTransform().getTransVector(); // correct for target position

    hit->setXYZLab(r.getX(), r.getY(), r.getZ());
    hit->setDistance( d );
    hit->setTheta(theta);
    hit->setPhi(phi);
    hit->getXYZLab(xx,yy,zz);

}

Int_t HWallHitF::execute(void)
{
    HWallOneHit* onehit = NULL;
    HWallHit* hit       = NULL;
    Bool_t  startFlag = kFALSE;
    Float_t startTime = 0, calTime = 0;
    fLoc.set(1,-1);

    if (fStartHitCat) {
	HStart2Hit *h = NULL;
	if((h=(HStart2Hit *)fStartHitCat->getObject(0))!=NULL){
	    startTime   = h->getTime();
	    startFlag   = h->getFlag();
	}
    }

    iter->Reset();   // this works only in split mode=2
    // (for 0 and 1 the iterator must be recreated)
    while ((onehit=(HWallOneHit *)iter->Next()) != 0) {

	fLoc[0] = onehit->getCell();
	if(fLoc[0] == -1) continue;

	if(onehit->getTime() > -399.) {
	    fLoc[0] = onehit->getCell();

	    calTime =  onehit->getTime();

	    if(startFlag) calTime -=  startTime;

	    hit = (HWallHit*) fHitCat->getSlot(fLoc);
	    if(hit != NULL) {
		hit = new(hit) HWallHit;
		hit->setTime(calTime);
		hit->setCharge(onehit->getCharge());

		hit->setAddress(fLoc[0]);
		fillHit(hit,onehit);
		fillGeometry(hit);  //FK// TADY TO KRACHUJE
	    }

	}

    }

    return 0;

}


ClassImp(HWallHitF)










