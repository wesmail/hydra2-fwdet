#include "hparticlegeantdecay.h"


#include <vector>
#include <iostream>
#include <iomanip>
#include <algorithm>

using namespace std;

ClassImp(HParticleGeantDecay)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleGeantDecay
//
// Simulation object keeping HGeant pointers mother and pointers to daughters and
// mother decays. For documentation of use case see the documentation of
// HParticleGeantEvent.
////////////////////////////////////////////////////////////////////////////////

HParticleGeantDecay::HParticleGeantDecay()
{
    clear();
}

HParticleGeantDecay::~HParticleGeantDecay()
{
    ;
}

Bool_t HParticleGeantDecay::isDaughterPID(Int_t pid)
{
    // return kTRUE if a daughters with the given id
    // is among the list of daughters.

    for(UInt_t i = 0; i < fdaughters.size(); i ++ ){
      if(fdaughters[i]->getID() == pid) return kTRUE;
    }
    return kFALSE;
}

Bool_t HParticleGeantDecay::isDaughterPID(vector<Int_t>& pids)
{
    // checks if all pids in vector
    // are contained in daughters. This function
    // can be used to make sure that a decay has all
    // all needed daughters in the list.
    // example : lambda -> p + piminus
    //           if both daughters should be in acceptance found
    //           one can ask with ids 14+9
    // example 2 :   pi0 -> e+ + e- + gamma
    //               2+3 will make sure both leptons are found

    if(fdaughters.size()==0 ) return kFALSE;
    vector<Int_t> ind;
    ind.assign(pids.size(),-1);

    for(UInt_t i  = 0; i < fdaughters.size(); i ++ ){
	for(UInt_t j  = 0; j < pids.size(); j ++ ){
	    if(fdaughters[i]->getID() == pids[j] && ind[j] < 0) {
		ind[j] = i;
                break;
	    }
	}
    }

    for(UInt_t j  = 0; j < ind.size(); j ++ ){
       if (ind[j] < 0 ) return kFALSE;
    }
    return kTRUE;
}

void HParticleGeantDecay::addDaughter(HGeantKine* d)
{
    // adds a daughter to the list of daughters
    // if the daughters is not already in the list
    if(!isDaughter(d)) fdaughters.push_back(d);
}

void  HParticleGeantDecay::sortDaughters()
{
    // sorts the daughters ascending by track number
    sort(fdaughters.begin(),fdaughters.end(),compareTrackNr);
}
Float_t  HParticleGeantDecay::getDistFromVertex(HGeomVector* primVer)
{
    // returns the distance of the decay from primary vertex (if provided)
    // otherwise the distance from 0,0,0 in mm

    if(!primVer){
	return fdecayVertex.length();
    } else {
	HGeomVector diff = *primVer - fdecayVertex;
        return diff.length();
    }
}

void HParticleGeantDecay::clear()
{
    // clears all internal variables
    fmother     = 0 ;
    fdaughters.clear();
    fgeneration = 0;
    fmotherDecay= 0;
    fdecayVertex.setXYZ(-1000,-1000,-1000);
}

Bool_t HParticleGeantDecay::isDaughter(HGeantKine* d)
{
    // return kTRUE if the particle is inside the list of
    // daughters of the decay
    if( find(fdaughters.begin(),fdaughters.end(),d) == fdaughters.end()) return kFALSE;
    else                                                                 return kTRUE;

}

Bool_t HParticleGeantDecay::isDaughter(vector<HGeantKine*> daughters)
{
    // returns kTRUE if all particles are found
    // in the list of daughters

    if(fdaughters.size()==0 ) return kFALSE;
    if(daughters.size() ==0 ) return kFALSE;

    for(UInt_t i  = 0; i < daughters.size(); i ++ ){
      if(!isDaughter(daughters[i])) return kFALSE;
    }
    return kTRUE;
}

void HParticleGeantDecay::print()
{
    // print some basic infos about the decay
    if(fmother) {
	cout<<"mother : generation "<<setw(3)<<fgeneration<< " ##################################"<<endl;
	cout<<"    mother     tr  "<<setw(5)<<fmother->getTrack()<<" parent tr "<<setw(5)<<fmother->getParentTrack()<<" id "<<setw(8)<<HPhysicsConstants::pid(fmother->getID())<<" vertex : "<<setw(10)<<fdecayVertex.X()<<" "<<setw(10)<<fdecayVertex.Y()<<" "<<setw(10)<<fdecayVertex.Z()<<endl;
	cout<<"               med "<<setw(4)<<getMedium()            <<" mechanism "<<setw(5)<<HPhysicsConstants::geantProcess(getMechanism())<<endl;
	cout<<"    generator info "<<setw(10)<<getGeneratorInfo()<<" info1 "<<setw(10)<<getGeneratorInfo1()<<" info2 "<<setw(10)<<getGeneratorInfo2()<<endl;
	for(UInt_t i=0;i<fdaughters.size(); i++){
	    HGeantKine* kine = fdaughters[i];
	    cout<<"    daughter "<<i<<" tr "<<setw(5)<<kine->getTrack()<<" parent tr "<<setw(5)<<kine->getParentTrack()<<" id "<<setw(8)<<HPhysicsConstants::pid(kine->getID())<<endl;
	}
    }
    if(fmotherDecay){
	cout<<"    parent Decay : generation "<<setw(3)<< fmotherDecay->getGeneration() <<" mother id "<<setw(8)<< (fmotherDecay->getMother() ? HPhysicsConstants::pid(fmotherDecay->getMotherID()): 0 )<<setw(5)<<" tr "<<(fmotherDecay->getMother() ? fmotherDecay->getMother()->getTrack(): 0 )<<endl;
    } else {
	cout<<"    parent Decay : none"<<endl;
    }
}
