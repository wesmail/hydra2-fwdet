#include "hparticlegeant.h"
#include "hparticlegeantdecay.h"
#include "hparticlecandsim.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

using namespace std;


ClassImp(HParticleGeant)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleGeant
//
// Simulation object keeping HGeant pointers and pointers to decays and
// reconstructed HParticleCand objects for this geant track.
// For documentation of use case see the documentation of
// HParticleGeantEvent.
////////////////////////////////////////////////////////////////////////////////

HParticleGeant::HParticleGeant()
{
    clear();
}
HParticleGeant::~HParticleGeant()
{
    ;
}

void  HParticleGeant::setParticle(HGeantKine* part)
{
    // sets kine pointer of particle
    // the vertex and the acceptance flag
    fparticle = part;
    part->getVertex(fdecayVertex);
    fInAcceptance = part->isInAcceptance();
}
Float_t HParticleGeant::getDistFromVertex(HGeomVector* primVer)
{
    if(!primVer) return fdecayVertex.length();
    else {
	HGeomVector diff = *primVer + fdecayVertex;
        return diff.length();
    }
}

void HParticleGeant::addRecoCand(HParticleCandSim* c)
{
    // adds a candidate to the list of recontructed
    // candidates if it is not already in the list
    if(find(vReco.begin(),vReco.end(),c)==vReco.end()) vReco.push_back(c);
}

UInt_t  HParticleGeant::getNRecoCand()
{
    return vReco.size();
}

UInt_t  HParticleGeant::getNRecoUsedCand()
{

    // return the number of reconstructed candidates matching
    // this geant object which are flagged kIsUsed
    Int_t ct = 0 ;
    for(UInt_t i =0; i < vReco.size(); i++){
	if(!vReco[i]->isFlagBit(kIsUsed)) continue;    // only count used particles
	ct++;
    }
    return ct;
}

UInt_t  HParticleGeant::getNGhosts(Bool_t isUsed)
{
    // return the number of reconstructed candidates matching
    // this geant object which are flagged kIsUsed (if isUsed=kTRUE, default=kFALSE)
    // and are marked as Ghosts
    Int_t ct = 0 ;
    for(UInt_t i =0; i < vReco.size(); i++){
	if(isUsed && !vReco[i]->isFlagBit(kIsUsed)) continue;    // only count used particles
	if(vReco[i]->isGhostTrack()) ct++;
    }
    return ct;
}

UInt_t HParticleGeant::getNTrueReco(Bool_t isUsed,UInt_t detbits)
{
    // return the number of reconstructed candidates matching
    // this geant object which are flagged kIsUsed (if isUsed=kTRUE, default=kTRUE)
    // and are not marked as Ghosts and fullfill the detector bits
    // (default == kIsInInnerMDC|kIsInOuterMDC|kIsInMETA, = 0 will ignore)
    Int_t ct = 0 ;
    for(UInt_t i =0; i < vReco.size(); i++){
	if(isUsed && !vReco[i]->isFlagBit(kIsUsed)) continue;    // only count used particles
	if( (detbits==0 || vReco[i]->isInDetectors(detbits)) && !vReco[i]->isGhostTrack()) ct++;
    }
    return ct;
}

void HParticleGeant::print()
{
    // prints some basic infos about this particle
    cout<<"HParticleGeant: generation "<< setw(3)<<fgeneration<< " ##################################"<<endl;
    if(fparticle) fparticle->print();
    if(fmotherDecay)  cout<<"mother ID  "<< setw(3)<<fmotherDecay->getMotherID() ;
    cout<<"n reco     "<< setw(3)<<vReco.size()<<"n reco used "<<setw(3)<<getNRecoUsedCand()<<" n true "<<setw(3)<<getNTrueReco()<<" n ghost "<<setw(3)<<getNGhosts()<<endl;
}

void HParticleGeant::clear()
{
    // resets all internal varibales
    fparticle   = 0;
    fmother     = 0;
    fgeneration = 0;
    fmotherDecay= 0;
    fdecayVertex.setXYZ(-1000,-1000,-1000);
    vReco.clear();
    fInAcceptance = kFALSE;

}
