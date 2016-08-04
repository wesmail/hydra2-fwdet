//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : G. Kornakov
//
// HParticlePathLengthCorr
//
// Reconstructor to recalulate the path length for the event vertex.
// Beta and Mass of the particle are recalutated. HParticlePathLengthCorr
// can be run by  HParticleT0Reco automatically if enabled.
//
//  setBeamTime() has to be set for the correct data set (default "apr12")
//
//--------------------------------------------------------------------------
// USAGE:
//
//  HParticlePathLengthCorr pathCorr;
//  pathCorr.setBeamTime("apr12");
//
//  // 2 possible ways :
//  a. As Reconstructor add it to standard task list
//  b. With HLoop :
//
//     before eventLoop :  pathCorr.init();
//     inside eventloop :  pathCorr.execute();
//
//
////////////////////////////////////////////////////////////////////////////
#include "hparticlepathlengthcorr.h"
#include "hparticlecand.h"
#include "heventheader.h"
#include "hades.h"
#include "hevent.h"
#include "hgeomvector.h"
#include "hcategory.h"
#include "TMath.h"

ClassImp(HParticlePathLengthCorr)


HParticlePathLengthCorr::HParticlePathLengthCorr(const Text_t* name,const Text_t* title)
    : HReconstructor(name,title)
{
    setBeamTime("apr12");
    candCat = NULL;
}

HParticlePathLengthCorr::~HParticlePathLengthCorr()
{

}

Bool_t HParticlePathLengthCorr::init()
{
    candCat = gHades->getCurrentEvent()->getCategory(catParticleCand); //
    if(!candCat){
	Error("init()","Could not retrieve catParticleCand category!");
	return kFALSE;
    }
    return kTRUE;
}

Bool_t HParticlePathLengthCorr::setBeamTime(TString beamtime)
{
    start = 0.0;
    beamtime.ToLower();
    if(beamtime == "apr12") {
	zVm = -2.9462e+01;
	xVm = 1.03215e-01;
	yVm = 6.80308e-01;
	Double_t tempA[6] = {-0.00244573,0.00203701, 0.00486586,0.00375363, 0.00273255, -0.00214376};
	Double_t tempB[6] = {0.983468,0.983118,0.978343,0.968608,0.971367,0.977932};
	Double_t tempD[6] = {54.1157,55.1195,59.732,66.2281,65.0598,63.026};
	Double_t tempan[6]={0.*TMath::DegToRad(),60.*TMath::DegToRad(),120.*TMath::DegToRad(),180.*TMath::DegToRad(),240.*TMath::DegToRad(),300*TMath::DegToRad()};
	beamVelocity = 0.9015114 ;
	for(Int_t sector=0;sector<6;sector++) {
	    A[sector]  = tempA[sector];
	    B[sector]  = tempB[sector];
	    D[sector]  = tempD[sector];
	    an[sector] = tempan[sector];
	    hPlane[sector].setPlanePar(A[sector]*TMath::Cos(an[sector])-B[sector]*TMath::Sin(an[sector]),A[sector]*TMath::Sin(an[sector])+B[sector]*TMath::Cos(an[sector]),1.0,D[sector]);

	}
	return kTRUE;
    }
    Info("setBeamTime()","Beam time %s is unknown!",beamtime.Data());
    return kFALSE;
}

Int_t HParticlePathLengthCorr::execute()
{
    Float_t newMass2;
    HParticleCand* pCand=0;
    pVertex = (gHades->getCurrentEvent()->getHeader()->getVertexReco()).getPos();
    if(pVertex.X()==-1000. && pVertex.Y()==-1000. && pVertex.Z()==-1000. ) return 0;

    start = (zVm-pVertex.getZ()) /beamVelocity/299.792458;

    if(candCat) {
	for(Int_t i=0;i<candCat->getEntries();i++) {
	    pCand = (HParticleCand*)candCat->getObject(i);
	    if(!pCand) continue;
	    if(pCand->getBeta()!=-1 && pCand->isFlagAND(4,
							Particle::kIsAcceptedHitInnerMDC,
							Particle::kIsAcceptedHitOuterMDC,
							Particle::kIsAcceptedHitMETA,
							Particle::kIsAcceptedRK) ) {
		sector      = pCand->getSector();
		Float_t mom = pCand->getMomentum();
		Float_t newPath = pCand->getDistanceToMetaHit();
		Float_t newBeta = pCand->getBeta();
		Float_t tof     = newPath/newBeta/299.792458; //in (ns)
		Float_t xP, yP, zP;
		hPlane[sector].calcSegIntersec(pCand->getZ(), pCand->getR(), pCand->getTheta()*TMath::DegToRad(), pCand->getPhi()*TMath::DegToRad(), xP, yP, zP );
		Float_t rktl = sqrt((xP-xVm)*(xP-xVm)+(yP-yVm)*(yP-yVm)+(zP-zVm)*(zP-zVm) );
		// And the real was:
		Float_t rktl_real = sqrt(  (xP-pVertex.getX())*(xP-pVertex.getX())
					 + (yP-pVertex.getY())*(yP-pVertex.getY())
					 + (zP-pVertex.getZ())*(zP-pVertex.getZ())   );
		// And the new PL is....:
		newPath -=  rktl ;
		newPath +=  rktl_real;
		//Tof correction (ToF of the incident Ion to the center of the target.
		tof += (zVm-pVertex.getZ()) /beamVelocity/299.792458;
		newBeta  =  newPath/tof/299.792458;
		newMass2 =  mom*mom*(1.-newBeta*newBeta)/newBeta/newBeta;
		pCand->setMass2(newMass2);
		pCand->setBeta(newBeta);
		pCand->setDistanceToMetaHit(newPath);
	    }
	}
    }
    return 0;
}



