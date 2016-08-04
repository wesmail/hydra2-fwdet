#include "hparticlegeantpair.h"
#include "hparticletool.h"
#include "hphysicsconstants.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(HParticleGeantPair)

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticleGeantPair
//
// a pair build of 2 HGeantKine objects. Opening Angle + Vertex variables
// are calulated when the pair is set.
//
////////////////////////////////////////////////////////////////////////////////
Bool_t HParticleGeantPair::fbCheckAcceptance=kTRUE;

HParticleGeantPair::HParticleGeantPair()
{
    clear();
}

HParticleGeantPair::~HParticleGeantPair()
{

}

Bool_t HParticleGeantPair::isTruePair()
{
    // true if daughters are from source
    //
    if(fstatusFlags&0x01) return kTRUE;
    return kFALSE;
}

Bool_t HParticleGeantPair::isInAcceptance(Int_t ver)
{
    if(ver<0)        { if( (fstatusFlags&6) == 6 ) return kTRUE; }  // both candidates
    else if(ver==0)  { if( (fstatusFlags&2) == 2 ) return kTRUE; }  // candidate 1
    else if(ver==1)  { if( (fstatusFlags&4) == 4 ) return kTRUE; }  // candidate 2
    return kFALSE;
}

Int_t HParticleGeantPair::isFakePair()
{

    // checkes the assigned pids (not GEANT!)
    // return 2 if both cands are fake, 1 if one is
    // fake and 0 if none is fake

    if     (fPID[0] < 0 && fPID[1] < 0 ) return 2;
    else if(fPID[0] < 0 || fPID[1] < 0 ) return 1;
    else                                 return 0;
}

Bool_t  HParticleGeantPair::calcVertex()
{

    if(!fcand[0] || !fcand[1]) {
	Error("calcVertex()","Candidates not yet filled! Skipped!");
	return kFALSE;
    }
    HGeomVector base1,dir1,base2,dir2,dirMother;
    Float_t x,y,z;
    fcand[0]->getVertex(x,y,z);
    base1.setXYZ(x,y,z);
    fcand[1]->getVertex(x,y,z);
    base2.setXYZ(x,y,z);

    dir1.setXYZ(fc[0].X(),fc[0].Y(),fc[0].Z());
    dir2.setXYZ(fc[1].X(),fc[1].Y(),fc[1].Z());

    fDecayVertex  = HParticleTool::calcVertexAnalytical(base1,dir1,base2,dir2); // vertex of the two tracks

    dirMother.setXYZ((*this).X(),(*this).Y(),(*this).Z());

    fVerMinDistCand[0]   = HParticleTool::calculateMinimumDistanceStraightToPoint(base1,dir1,fEventVertex); //distance first secondary particle from primary vertex
    fVerMinDistCand[1]   = HParticleTool::calculateMinimumDistanceStraightToPoint(base2,dir2,fEventVertex); //distance second secondary particle from primary vertex
    fVerMinDistMother    = HParticleTool::calculateMinimumDistanceStraightToPoint(fDecayVertex,dirMother,fEventVertex);//?? distance decay vertex from primary vertex
    fVerDistMother       = (fDecayVertex-fEventVertex).length(); // decay vertex <-> global vertex
    fMinDistCandidates   = HParticleTool::calculateMinimumDistance(base1,dir1,base2,dir2);

    return kTRUE;
}

Bool_t  HParticleGeantPair::calcVectors(Int_t pid1,Int_t pid2,Int_t motherpid,HGeomVector& vertex)
{
    // sets fPID1,fPID2,fMotherPID and fills lorentz vectors
    // opening Angle etc.

    if(!fcand[0] || !fcand[1]) { Error("calcVectors()","Candidates not yet filled! Skipped!"); return kFALSE; }

    fPID[0]    = pid1;
    fPID[1]    = pid2;
    fMotherPID = motherpid;
    fEventVertex = vertex;

    HParticleTool::getTLorentzVector(fcand[0],fc[0],fPID[0]);
    HParticleTool::getTLorentzVector(fcand[1],fc[1],fPID[1]);

    (*((TLorentzVector*)this)) =  fc[0] + fc[1];

    foAngle = fc[0].Angle(fc[1].Vect()) * TMath::RadToDeg();

    calcVertex();

    return kTRUE;
}


Bool_t  HParticleGeantPair::setPair(HGeantKine* cnd1,Int_t pid1,
			    HGeantKine* cnd2,Int_t pid2,
			    HGeantKine* mother,Int_t motherpid,HGeomVector& vertex)
{
    // sets candidates and calls
    // calcvectors. Supposed to be called at initial
    // filling. uses provided pids for mass calculation.
    // if  fmother!=0 && motherpid > 0    fMotherPID = motherpid
    // if  fmother!=0 && motherpid <= 0   fMotherPID = GeantmotherPID
    // if  fmother==0 && motherpid > 0    fMotherPID = motherid
    // vertex = event vertex
    fcand[0]     = cnd1;
    fcand[1]     = cnd2;
    fmother      = mother;

    if(fmother){
	if(motherpid <= 0  ) fMotherPID = fmother->getID();
	else                 fMotherPID = motherpid;
    } else {
	if(motherpid > 0)    fMotherPID = motherpid;
	else {
	    Warning("setPair()","HGeantKine pointer for mother == 0 and motherpid not valid");
	}
    }

    if(fcand[0] && fcand[1]){
       if( fcand[0]->getParentTrack()!=0 && fcand[0]->getParentTrack() == fcand[1]->getParentTrack() ) fstatusFlags = fstatusFlags|0x01;
       if(fbCheckAcceptance && fcand[0]->isInAcceptance()) fstatusFlags = fstatusFlags|0x02;
       if(fbCheckAcceptance && fcand[1]->isInAcceptance()) fstatusFlags = fstatusFlags|0x04;
    }

    return calcVectors(pid1,pid2,motherpid,vertex);
}

Bool_t  HParticleGeantPair::setPair(HGeantKine* cnd1,HGeantKine* cnd2,
			    HGeantKine* mother, Int_t motherpid,HGeomVector& vertex)
{
    // sets candidates and calls
    // calcvectors. Supposed to be called at initial
    // filling. uses Geant id for mass calculation
    // if  fmother!=0 && motherpid > 0    fMotherPID = motherpid
    // if  fmother!=0 && motherpid <= 0   fMotherPID = GeantmotherPID
    // if  fmother==0 && motherpid > 0    fMotherPID = motherid
    // vertex = event vertex

    fcand[0]     = cnd1;
    fcand[1]     = cnd2;
    fmother      = mother;

    if(fmother){
	if(motherpid <= 0  ) fMotherPID = fmother->getID();
	else                 fMotherPID = motherpid;
    } else {
	if(motherpid > 0)    fMotherPID = motherpid;
        else {
	    Warning("setPair()","HGeantKine pointer for mother == 0 and motherpid not valid");
	}
    }

    Int_t pid1 =  cnd1->getID();
    Int_t pid2 =  cnd2->getID();

    if(fcand[0] && fcand[1]){
       if( fcand[0]->getParentTrack()!=0 && fcand[0]->getParentTrack() == fcand[1]->getParentTrack() ) fstatusFlags = fstatusFlags|0x01;
       if(fbCheckAcceptance && fcand[0]->isInAcceptance()) fstatusFlags = fstatusFlags|0x02;
       if(fbCheckAcceptance && fcand[1]->isInAcceptance()) fstatusFlags = fstatusFlags|0x04;
    }

    return calcVectors(pid1,pid2,motherpid,vertex);
}


void  HParticleGeantPair::print(UInt_t selection)
{
    // print option bits
    // bit   1 : print particle infos                      (selection == 1)
    //       2 : print pids, oAngle and polarities         (selection == 2)
    //       3 : print vertex infos                        (selection == 4)
    // default : print all


    cout<<"HParticleGeantPair::print() --------------------------------------"<<endl;

    if( (selection>>0) & 0x01){
	if(fcand[0]) fcand[0]->print();
	if(fcand[1]) fcand[1]->print();
        if(fmother)  fmother->print();

    }

    if( (selection>>1) & 0x01){
	cout<<"    pid1 = "   <<fPID[0]
	    <<", pid2 = "     <<fPID[1]
	    <<", motherpid = "<<fMotherPID
	    <<", oAngle = "<<foAngle
	    <<", chrg1 = "  << HPhysicsConstants::charge(fPID[0])
	    <<", chrg2 = "  << HPhysicsConstants::charge(fPID[1])
	    <<endl;
    }

    if( (selection>>2) & 0x01){
	cout<<"primary vertex    : "<<setw(12)<<fEventVertex.X()   <<", "<<setw(12)<<fEventVertex.Y()<<", "<<setw(12)<<fEventVertex.Z()<<endl;
	cout<<"decay   vertex    : "<<setw(12)<<fDecayVertex.X()   <<", "<<setw(12)<<fDecayVertex.Y()<<", "<<setw(12)<<fDecayVertex.Z()<<endl;
	cout<<"vermindist cand1  : "<<setw(12)<<fVerMinDistCand[0] <<", vermindist cand2 : "<<setw(12)<<fVerMinDistCand[1]<<", mindist candidates : "<<setw(12)<<fMinDistCandidates<<endl;
	cout<<"vermindist mother : "<<setw(12)<<fVerMinDistMother  <<", verdist mother   : "<<setw(12)<<fVerDistMother<<endl;
    }

}


void  HParticleGeantPair::clear()
{
    // clears all data elements but not vectors!
    fcand[0]         = 0;
    fcand[1]         = 0;
    fmother          = 0;
    fPID[0]          = -10;
    fPID[1]          = -10;
    fMotherPID       = -10;
    foAngle          = -1;
    fstatusFlags     = 0;

    fVerMinDistCand[0]   = 0.;
    fVerMinDistCand[1]   = 0.;
    fVerMinDistMother    = 0.;
    fVerDistMother       = 0.;
    fMinDistCandidates   = 0.;
}
