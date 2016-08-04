#include "hparticlegeantevent.h"


#include "hgeomvector.h"
#include "hcategory.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hgeantheader.h"
#include "hades.h"
#include "hphysicsconstants.h"
#include "hcategorymanager.h"
#include "hparticletool.h"

#include "hgeantkine.h"
#include "hparticlegeant.h"
#include "hparticlegeantdecay.h"


#include <vector>
#include <map>
#include <iostream>
#include <iomanip>
#include <algorithm>

ClassImp(HParticleGeantEvent)


//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleGeantEvent
//
// Simulation object keeping HGeant pointers and decays to ease
    // the geant use.
    //
    //
    //##########################################################################
    // USAGE:
    //
    //
    //  HParticleGeantEvent geantevent; // create the object out side event loop
    //
    //  ...
    //
    //  for (Int_t i=0; i < entries; i++) {
    //    Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
    //    if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
    //
    //
    //    geantevent.nextEvent(); // fill the geant event structure
    //
    //    cout<<"event "<<i<<"------------------------------------------"<<endl;
    //    cout<<"impact : "<<geantevent.getImpactParam()<<", beam E: "<< geantevent.getBeamEnergy()<< ", event plane: "<<geantevent.getEventPlane()<<endl;
    //    cout<<"mult charged prim 4pi "<<geantevent.getMultiplicity(-1,0,0,0)             // id,generation,charge,type,detbits
    //        <<" acc "<< geantevent.getMultiplicity(-1,0,0,1)
    //        <<" reco true iMDC+oMDC+META "<<geantevent.getMultiplicity(-1,0,0,2,kIsInInnerMDC|kIsInOuterMDC|kIsInMETA)
    //        <<" reco true all "<<geantevent.getMultiplicity(-1,0,0,2,0)
    //        <<endl;
    //
    //    //----------------------------------------------------
    //    // example 1:
    //    for(UInt_t j = 0; j < geantevent.getDecays().size(); j++ ){
    //        HParticleGeantDecay* decay = geantevent.getDecays()[j];
    //        if(decay->getDistFromVertex()<100 ) decay->print(); // print all decays max 10cm arround 0,0,0
    //    }
    //
    //    //----------------------------------------------------
    //    // example 2 :
    //    vector<HParticleGeantDecay*> decays;      // a decay keeps pointer to mother and daughters and the mother decay
    //    geantevent.isDecay(decays,52,-1);         // all omgegas from PLUTO (external : generation = -1) filter params :  motherid, generation, med, dist
    //    for(UInt_t j = 0; j < decays.size(); j++) {
    //       decays[j] ->print();
    //    }
    //    //----------------------------------------------------
    //    // example 3 :
    //    geantevent.isDecay(decays,18,0);          // all primary lambda decays (urqmd+PLUTO(is not decayed outside GEANT))
    //    vector<Int_t > pids;
    //    pids.push_back(14);  // proton
    //    pids.push_back(9);   // pion
    //    for(UInt_t j = 0; j < decays.size(); j++) {
    //       HParticleGeantDecay* lambda = decays[j];
    //       if(!lambda->isExternalSource()) continue;  // keep only PLUTO
    //       if(lambda->isDaughter(pids)) // both particles found
    //
    //       vector<HGeantKine*>& daughters = lambda->getDaughters();
    //       for(UInt_t l = 0; l < daughters.size(); j++){
    //           HParticleGeant* p = geantevent.getParticle(daughters[l]);
    //           if(p->isInAcceptance()){
    //             Int_t nReco p->getNRecoCand(); // number of HParticleCandSim keeping this geant track
    //             for(Int_t k=0; k < nReco; k++){
    //                 if(!p->getRecoCand(k)->isGhostTrack() && p->getRecoCand(k)->isIndDetectors(kIsInInnerMDC||kIsInOuterMDC|kIsInMETA)) { // true reco
    //
    //                 }
    //             }
    //           }
    //        }
    //
    //    }
    //
    //    //----------------------------------------------------
    //    // example 4 :
    //    vector<HParticleGeant*> particles;
    //    geantevent.isParticle(particles,2,-2,0); // all primary positrons  possible filter params : id,motherid,generation,med,dist
    //    for(UInt_t j = 0; j < particles.size(); j++) {
    //        particles[j]->print();
    //        HParticleGeantDecay* d = particles[j]->getMotherDecay(); // will even work for PLUTO decayed particles
    //        if(d) d->print();
    //    }
    //    //----------------------------------------------------
    //    // example 5 :
    //    HParticleCandSim* cand1=0;
    //    for(Int_t j = 0; j < particleCat->getEntries(); j ++){
    //        cand1 = HCategoryManager::getObject(cand1,particleCat,j);
    //        if(!cand1->isFlagBit(Particle::kIsLepton)) continue;
    //
    //        Int_t tr = cand1->getGeantTrack();
    //        HParticleGeant* p      = geantevent.getParticle(tr);
    //        HParticleGeantDecay* d = p->getMotherDecay();    // mother decay
    //        Int_t nDaughter        = d ->getNDaughters();
    //        HGeantKine* sister = 0;
    //        for(Int_t k=0; k < nDaughter; k++){              // find sister track
    //            sister = d->getDaughter(k);
    //            if(sister->getTrack() != p->getParticle()->getTrack()) break;
    //        }
    //        HParticleGeant* pSister = geantevent.getParticle(sister);
    //
    //        Int_t nReco p->getNRecoCand(); // number of HParticleCandSim keeping this geant track
    //        for(Int_t k=0; k < nReco; k++){
    //           p->getRecoCand(k)->print(); // all candidates sharing the same geant track
    //        }
    //        nReco pSister->getNRecoCand(); // number of HParticleCandSim keeping this geant track
    //        for(Int_t k=0; k < nReco; k++){
    //           p->getRecoCand(k)->print(); // all candidates sharing the same geant track
    //        }
    //    }
    //  } // end eventloop
////////////////////////////////////////////////////////////////////////////////


HParticleGeantEvent::HParticleGeantEvent()
{
    vParticles    .clear();
    vParticles    .resize(1000);
    vDecays       .clear();
    vDecays       .resize(1000);
    fBeamEnergy = 0;
    fEventPlane = 0;
    fImpactParam= 0;
    bConvertExtThermal = kTRUE;
    bCorrectInfo       = kTRUE;
}

HParticleGeantEvent::~HParticleGeantEvent()
{

    clear();
}

void  HParticleGeantEvent::clear(){

    for(UInt_t i = 0 ; i < vexternalKine.size(); i++ ) delete vexternalKine[i];
    for(UInt_t i = 0 ; i < vDecays.size();       i++ ) delete vDecays[i];
    for(UInt_t i = 0 ; i < vParticles.size();    i++ ) delete vParticles[i];
    vexternalKine.clear();
    vexternalParticles.clear();
    vDecays      .clear();
    vParticles   .clear();
    mToDaughters.clear();
    mToParticle .clear();
    feventVertex.setXYZ(0,0,0);
    fBeamEnergy = 0;
    fEventPlane = 0;
    fImpactParam= 0;

}

void HParticleGeantEvent::nextEvent()
{
    clear();

    vector<HGeantKine*> mothers;
    vector<HGeantKine*> daughters;

    HCategory* kineCat = HCategoryManager::getCategory(catGeantKine,0,"catGeantKine");

    if(!kineCat) return;


    HPartialEvent* simul =((HRecEvent*) gHades->getCurrentEvent())->getPartialEvent(catSimul);

    if(simul) {
	HGeantHeader* header = (HGeantHeader*) simul->getSubHeader();
	if(header){
	    fBeamEnergy  = header->getBeamEnergy();
	    fEventPlane  = header->getEventPlane();
	    fImpactParam = header->getImpactParameter();
	}

    } else cout<<"no catSimul"<<endl;

    //----------------------------------------------------
    // loop over kinecat and book all particles
    Int_t n = kineCat->getEntries();
    HGeantKine* kine=0;
    Bool_t vertexFound = kFALSE;
    for(Int_t j = 0 ;j < n ; j ++){
	kine = HCategoryManager::getObject(kine,kineCat,j);
	if(kine){
	    if(!vertexFound && kine->getParentTrack() == 0){
		kine->getVertex(feventVertex);
		vertexFound = kTRUE;
	    }

	    if(bCorrectInfo && kine->isBugInfo()) {
               kine->setGenerator(0,0,0);
	    }


	    UInt_t gen = HGeantKine::getMothers(kine,mothers);  // get all mothers of current object
	    // returns the actual generation of the particle

	    UInt_t ndaughters = HGeantKine::getDaughters(kine,daughters);

	    if(ndaughters > 0){
		mToDaughters[kine] = daughters;
	    }

	    if(kine->isExternalSource() && kine->getParentTrack() == 0){
		vexternalParticles.push_back(kine);
	    }

	    //----------------------------------------------------
	    // add all particles to working array
	    HParticleGeant* p =  new HParticleGeant();
	    p->setGeneration(gen);
	    p->setParticle(kine);
	    if(mothers.size() > 0) p->setMother(mothers[0]);
	    vParticles.push_back(p);

	    mToParticle[kine->getTrack()] = p;

	    //----------------------------------------------------

	} // if kine
    } // end loop kine

    //----------------------------------------------------
    // map sources of external particles
    // CAUTION : in case parentIndex of the
    // particle is not set, one can not distinguish
    // multiple source of the same type per event.
    // they will end up in the same source

    map<Long64_t,vector<HGeantKine*> > mexternalSource;
    for(UInt_t j=0; j < vexternalParticles.size(); j++){
	HGeantKine* daughter = vexternalParticles[j];

	if(daughter->getGeneratorInfo() == daughter->getGeneratorInfo1() )  continue;  // particle from PLUTO, but decay inside GEANT

	Long64_t index       = daughter->getGeneratorInfo2() + kMaxInt + kMaxInt*daughter->getGeneratorInfo2();
	if(mexternalSource.find(index) == mexternalSource.end()) {
	    // new source
	    vector<HGeantKine*> daughters;
	    daughters.push_back(daughter);
	    mexternalSource[index] = daughters;
	} else { // add daughter to existing source
	    mexternalSource[index].push_back(daughter);
	}

    }

    // for external source we can reconstruct the
    // decay including an fake HGeantKine object
    for(map <Long64_t,vector<HGeantKine*> >::iterator itex = mexternalSource.begin(); itex != mexternalSource.end(); ++itex){

	vector<HGeantKine*>& daughters = itex->second;

	HParticleGeantDecay* decay = new HParticleGeantDecay() ;

	TLorentzVector v1,v2;
	for(UInt_t j = 0; j <daughters.size(); j++){ // reconstruct momentum of mother from daughters
	    // will work for outside GEANT decayed particles
	    HParticleTool::getTLorentzVector(daughters[j],v1);
	    v2 += v1;
	}

	Int_t     sourceID  = daughters[0]->getGeneratorInfo();
	Bool_t    isThermal = daughters[0]->isThermalSource();


	HGeantKine* kine = new HGeantKine();
	if(isThermal && bConvertExtThermal) kine->setID(sourceID-500);
	else                                kine->setID(sourceID);

	kine->setGenerator(daughters[0]->getGeneratorInfo(),daughters[0]->getGeneratorInfo1(),daughters[0]->getGeneratorInfo2());
	kine->setMomentum(v2.Px(),v2.Py(),v2.Pz());
	kine->setVertex(feventVertex.X(),feventVertex.Y(),feventVertex.Z());
	kine->setMechanism(5); // decay
	kine->setMedium(-1); //


	vexternalKine.push_back(kine);

	decay->setMother(kine);
	decay->setDecayVertex(feventVertex.X(),feventVertex.Y(),feventVertex.Z());
	decay->setGeneration(-1);
	decay->setDaughters (daughters);
	decay->sortDaughters();

	vDecays.push_back(decay);
    }
    //----------------------------------------------------



    //----------------------------------------------------
    // build all decays others than external
    for(map <HGeantKine*,vector<HGeantKine*> >::iterator it = mToDaughters.begin(); it != mToDaughters.end(); ++it){
	HGeantKine* mother             = it->first;
	vector<HGeantKine*>& daughters = it->second;

	Float_t x,y,z;
	Int_t   aPar,aMed,aMech;
	mother->getVertex(x,y,z);
	mother->getCreator(aPar,aMed,aMech);
	UInt_t gen = HGeantKine::getGeneration(mother);  // get all mothers of current object

	HParticleGeantDecay* decay = new HParticleGeantDecay();
	decay->setMother(mother);
	decay->setDecayVertex(x,y,z);
	decay->setGeneration(gen) ;
	decay->setDaughters(daughters);
	decay->sortDaughters();


	vDecays.push_back(decay);
    }
    //----------------------------------------------------

    //----------------------------------------------------
    //  create decays for external particles which did
    //  not decay outside HGEANT and did not
    //  yet create (because maybe no daughter is in acceptance)
    //  a decay
    for(UInt_t i = 0; i < vexternalParticles.size(); i++){
	HGeantKine* part = vexternalParticles[i];

	if(part->getParentTrack() == 0 && part->isExternalSource() &&
	   part->getGeneratorInfo() == part->getGeneratorInfo1()  )
	{

	    HParticleGeantDecay* d = isMotherOfDecay(part);
	    if(d == 0){  // not yet in the list of decays
		HGeomVector vert;
		part->getVertex(vert);

		HParticleGeantDecay* decay = new HParticleGeantDecay();
		decay->setMother(part);
		decay->setDecayVertex(vert);
		decay->setGeneration(0);
		vDecays.push_back(decay);
	    }
	}

    }
    //----------------------------------------------------


    //----------------------------------------------------
    //  map decays for mother decays
    if(vDecays.size() > 0){
	for(UInt_t i=vDecays.size()-1; i > 0; i--){
	    HGeantKine* daughter = vDecays[i]->getMother();
	    if(daughter){
		HParticleGeantDecay* dmother = isDaughterOfDecay(daughter);
		if(dmother) vDecays[i]->setMotherDecay(dmother);

	    }
	}
    }
    //----------------------------------------------------

    //----------------------------------------------------
    //  map particles to decays
    for(UInt_t i = 0; i < vParticles.size(); i ++){
	HParticleGeantDecay* decay = isDaughterOfDecay(vParticles[i]->getParticle());
	if(decay) vParticles[i]->setMotherDecay(decay);
    }
    //----------------------------------------------------



    //----------------------------------------------------
    //  map reconstructed particles
    HCategory* candCat = HCategoryManager::getCategory(catParticleCand,2,"catParticleCand");

    if(candCat){
	n = candCat->getEntries();
	HParticleCandSim* cand = 0 ;
	for(Int_t i = 0; i < n ;i ++){
	    cand = HCategoryManager::getObject(cand,candCat,i);
	    if(cand){
		Int_t tr = cand->getGeantTrack();
		if(tr > 0){
		    map<Int_t,HParticleGeant*>::iterator it = mToParticle.find(tr);
		    if(it != mToParticle.end()) {
			HParticleGeant* gP = it->second;
			gP->addRecoCand(cand);
		    }
		}
	    }
	}
    }
    //----------------------------------------------------


}


HParticleGeantDecay* HParticleGeantEvent::isDaughterOfDecay(HGeantKine* daughter)
{
    // returns the decay for this daughter particle
    // returns 0 if no matching decay is found

    if(!daughter) return NULL;
    for(UInt_t i=0; i < vDecays.size(); i++){
	if(vDecays[i]->isDaughter(daughter)) return  vDecays[i];
    }
    return NULL;
}

HParticleGeantDecay* HParticleGeantEvent::isDaughterOfDecay(Int_t track)
{
    // returns the decay for this kine daughter particle with tracknumber track
    // returns 0 if no matching decay is found
    HGeantKine* moth = getKine(track);
    return  isDaughterOfDecay(moth);
}

HParticleGeantDecay* HParticleGeantEvent::isMotherOfDecay(HGeantKine* moth)
{
    // returns the decay for this mother particle
    // returns 0 if no matching decay is found
    if(!moth) return NULL;
    for(UInt_t i=0; i < vDecays.size(); i++){
	if(vDecays[i]->getMother() && vDecays[i]->getMother() == moth) return  vDecays[i];
    }
    return NULL;
}

HParticleGeantDecay* HParticleGeantEvent::isMotherOfDecay(Int_t track)
{
    // returns the decay for this kine mother particle with tracknumber track
    // returns 0 if no matching decay is found
    HGeantKine* moth = getKine(track);
    return  isMotherOfDecay(moth);
}

Int_t HParticleGeantEvent::isDecay(vector<HParticleGeantDecay*>& decays,
				   Int_t motherid,Int_t generation,Int_t med,Float_t dist)
{
    // fills the vector of decays matching the conditions (vector is cleared automatically
    // before filling).
    // possible filters :   motherid (ID of the mother), ignored if < 0
    //                      generation of the decay (-1 : arteficial ext, 0 : primay mother ....), ignored if < -1
    //                      med (medium where the decay was created),  ignored if < 0
    //                      dist (distance of decay from primary vertex), ignored if ==0 ,
    //                            dist > 0 all decays < dist will be selected,
    //                            dist < 0 all decays > dist will be selected

    decays.clear();
    Float_t di = TMath::Abs(dist);
    for(UInt_t i=0; i < vDecays.size(); i++){

	Float_t d=0;
        if(dist!=0) d = vDecays[i]->getDistFromVertex(&feventVertex);

	if(( (motherid < 0 ) ||  ( vDecays[i]->getMother() && vDecays[i]->getMother()->getID()     == motherid   ) ) &&
	   ( (generation< -1) || ( vDecays[i]->getGeneration()                                     == generation ) ) &&
	   ( (med      < 0 ) ||  ( vDecays[i]->getMother() && vDecays[i]->getMother()->getMedium() == med        ) ) &&
	   ( (dist    == 0 ) ||  ( ( dist > 0 ? d < di : d > di) ) )
	  )
	{
	    decays.push_back(vDecays[i]);
	}
    }

    return decays.size();
}

Int_t HParticleGeantEvent::isParticle(vector<HGeantKine*>& particles,
				      Int_t id,Int_t motherid, Int_t generation,
				      Int_t med,Float_t dist)
{
    // fills the vector of particles matching the conditions (vector is cleared automatically
    // before filling).
    // possible filters :   id (id of the particle), ignored if < 0
    //                      motherid (ID of the mother), ignored if < 0
    //                      generation of the decay (0 : primay mother ....), ignored if < 0
    //                      med (medium where the decay was created),  ignored if < 0
    //                      dist (distance of decay from primary vertex), ignored if ==0 ,
    //                            dist > 0 all decays < dist will be selected,
    //                            dist < 0 all decays > dist will be selected

    particles.clear();

    Float_t di = TMath::Abs(dist);

    for(UInt_t i=0; i < vParticles.size(); i++){

	HParticleGeant* p          = vParticles[i];
	HParticleGeantDecay* decay = p->getMotherDecay();
	HGeantKine* mother = 0;
	if(decay) mother = decay->getMother();

	Float_t d=0;
        if(dist!=0) d = p->getDistFromVertex(&feventVertex);

	if(( (id < 0 )       ||  ( p->getParticle()->getID()             == id ) ) &&
	   ( (generation< 0 )||  ( p->getGeneration() == generation            ) ) &&
	   ( (motherid < 0 ) ||  ( decay && mother && mother->getID()    == motherid ) ) &&
	   ( (med      < 0 ) ||  ( p->getParticle()->getMedium()         == med      ) ) &&
	   ( (dist    == 0 ) ||  ( ( dist > 0 ? d < di : d > di) ) )
	  )
	{
	    particles.push_back(p->getParticle());
	}
    }
    return particles.size();
}

Int_t HParticleGeantEvent::isParticle(vector<HParticleGeant*>& particles,
				      Int_t id,Int_t motherid, Int_t generation,
				      Int_t med ,Float_t dist)
{
    // fills the vector of particles matching the conditions (vector is cleared automatically
    // before filling).
    // possible filters :   id (id of the particle), ignored if < 0
    //                      motherid (ID of the mother), ignored if < 0
    //                      generation of the decay (0 : primay mother ....), ignored if < 0
    //                      med (medium where the decay was created),  ignored if < 0
    //                      dist (distance of decay from primary vertex), ignored if ==0 ,
    //                            dist > 0 all decays < dist will be selected,
    //                            dist < 0 all decays > dist will be selected

    particles.clear();

    Float_t di = TMath::Abs(dist);

    for(UInt_t i=0; i < vParticles.size(); i++){

	HParticleGeant* p          = vParticles[i];
	HParticleGeantDecay* decay = p->getMotherDecay();
	HGeantKine* mother = 0;
	if(decay) mother = decay->getMother();

	Float_t d=0;
        if(dist!=0) d = p->getDistFromVertex(&feventVertex);

	if(( (id < 0 )       ||  ( p->getParticle()->getID()             == id ) ) &&
	   ( (generation< 0 )||  ( p->getGeneration() == generation            ) ) &&
	   ( (motherid < 0 ) ||  ( decay && mother && mother->getID()    == motherid ) ) &&
	   ( (med      < 0 ) ||  ( p->getParticle()->getMedium()         == med      ) ) &&
	   ( (dist    == 0 ) ||  ( ( dist > 0 ? d < di : d > di) ) )
	  )
	{
	    particles.push_back(p);
	}
    }
    return particles.size();
}

HParticleGeant* HParticleGeantEvent::getParticle(Int_t track)
{
    // return the particle belonging to the tracknumber track
    // return 0 if no matching particle is found

    map<Int_t,HParticleGeant*>::iterator it = mToParticle.find(track);
    if(it != mToParticle.end()) return it->second;
    else return 0;
}

HParticleGeant* HParticleGeantEvent::getParticle(HGeantKine* kine)
{
    // return the particle belonging to the kine track
    // return 0 if no matching particle is found
    if(!kine) return 0;
    return  getParticle(kine->getTrack());
}

HGeantKine*   HParticleGeantEvent::getKine(Int_t track)
{
    // return the kine particle belonging to the track
    // return 0 if no matching particle is found
    HParticleGeant* p = getParticle(track);
    if(p) return p->getParticle();
    else return 0;

}

UInt_t  HParticleGeantEvent::getMultiplicity(Int_t id,Int_t generation,Int_t charge,UInt_t type,UInt_t detbits)
{
    // get particle multiplity
    // id    :   id      < 0  -> all IDs,  (default)
    //           id      >= 0 -> particles of the given id
    // gen   :   gen     < 0  -> all generations
    //           gen     = 0  -> primaries (default)
    // charge  : charge  = 0  -> all charged particles (default)
    //           charge  = 1  -> all positive charged particles
    //           charge  =-1  -> all negative charged particles
    //           charge  =-2  -> all particles
    // type  :   type    = 0  -> 4pi
    //           type    = 1  -> acceptance (default)
    //           type    = 2  -> reconstructed (requires kIsUsed + !isGhostTrack() + isInInnerMDC|isInOuterMDC|isInMETA)
    // detbits : detbits = 0  -> ignored
    //           detbits = kIsInInnerMDC|kIsInOuterMDC|kIsInMETA  default

    UInt_t mult = 0;
    for(UInt_t i=0; i < vParticles.size(); i++){

	HParticleGeant* p          = vParticles[i];

	Bool_t acc  = p->isInAcceptance();
	Bool_t reco = (p->getNTrueReco(kTRUE,detbits) > 0) ? kTRUE : kFALSE;

	Int_t chrg  = HPhysicsConstants::charge(p->getParticle()->getID());


	if(( (id         <  0 ) ||  ( p->getParticle()->getID()  == id         ) ) &&
	   ( (generation <  0 ) ||  ( p->getGeneration()         == generation ) ) &&
           ( (type       == 0 ) ||  (type   == 1 && acc)     || (type   == 2 && reco)     ) &&
           ( (charge     ==-2 ) ||  (charge == 0 && chrg!=0) || (charge == 1 && chrg > 0) || (charge ==-1 && chrg < 0)  )
	  )
	{
	    mult++;
	}
    }
    return mult;



}

