
#include "hparticlepair.h"
#include "hparticletool.h"
#include "hphysicsconstants.h"

#include <iostream>
#include <iomanip>

using namespace std;

ClassImp(HParticlePair)


//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticlePair
//
// a pair build of 2 HParticleCand objects. Opening Angle + Vertex variables
// are calulated when the pair is set.  With static HParticlePair::setDoMomentumCorrection(Bool_t doit)
// Particle momenta are corrected for energyloss (default: kTRUE).
//
////////////////////////////////////////////////////////////////////////////////


Bool_t HParticlePair::fDoMomCorrection = kTRUE;

HParticlePair::HParticlePair()
{
    clear();
}

HParticlePair::~HParticlePair()
{

}

Bool_t HParticlePair::isSimulation()
{
    // checks if pair is created by sim objects

    HParticleCand* c = 0;

    if     (fcand[0]) c = fcand[0];
    else if(fcand[1]) c = fcand[1];

    if(c){
	TString n = c->ClassName();
	if(n.CompareTo("HParticleCandSim") == 0){ return kTRUE; }
    }

    if(fpair[0] && fpair[0]->getIsSimulation()) return kTRUE;
    if(fpair[1] && fpair[1]->getIsSimulation()) return kTRUE;

    return kFALSE;
}


Int_t HParticlePair::isFakePair() {

    // checkes the assigned pids (not GEANT!)
    // return 2 if both cands are fake, 1 if one is
    // fake and 0 if none is fake

    if     (fPID[0] < 0 && fPID[1] < 0 ) return 2;
    else if(fPID[0] < 0 || fPID[1] < 0 ) return 1;
    else                                 return 0;
}

Bool_t HParticlePair::isTruePair() {

    // return kTRUE if both candidates have the same
    // geant pid as assigned pid and candidate is not
    // a ghost. Should be called on simulation only.
    if(fIsSimulation){
	if( (fstatusFlags&3) == 3) return kTRUE;  // 2 bits for true candidates
    }
    return kFALSE;
}

Bool_t  HParticlePair::calcVertex()
{

    if( (!fcand[0] && !fpair[0]) || (!fcand[1] && !fpair[1])) {
	Error("calcVertex()","Candidates not yet filled! Skipped!");
	return kFALSE;
    }
    HGeomVector base1,dir1,base2,dir2,dirMother;

    if(fcand[0]) HParticleTool::calcSegVector(fcand[0]->getZ(),fcand[0]->getR(),(TMath::DegToRad()*fcand[0]->getPhi()),(TMath::DegToRad()*fcand[0]->getTheta()),base1,dir1);
    else {
	base1 = fpair[0]->getDecayVertex();
        dir1  .setXYZ((*fpair[0]).X(),(*fpair[0]).Y(),(*fpair[0]).Z());
    }

    if(fcand[1]) HParticleTool::calcSegVector(fcand[1]->getZ(),fcand[1]->getR(),(TMath::DegToRad()*fcand[1]->getPhi()),(TMath::DegToRad()*fcand[1]->getTheta()),base2,dir2);
    else {
	base2 = fpair[1]->getDecayVertex();
        dir2  .setXYZ((*fpair[1]).X(),(*fpair[1]).Y(),(*fpair[1]).Z());
    }

    fDecayVertex  = HParticleTool::calcVertexAnalytical(base1,dir1,base2,dir2); // vertex of the two tracks

    dirMother.setXYZ((*this).X(),(*this).Y(),(*this).Z());

    fVerMinDistCand[0]   = HParticleTool::calculateMinimumDistanceStraightToPoint(base1,dir1,fEventVertex); //distance first secondary particle from primary vertex
    fVerMinDistCand[1]   = HParticleTool::calculateMinimumDistanceStraightToPoint(base2,dir2,fEventVertex); //distance second secondary particle from primary vertex
    fVerMinDistMother    = HParticleTool::calculateMinimumDistanceStraightToPoint(fDecayVertex,dirMother,fEventVertex);//?? distance decay vertex from primary vertex
    fVerDistMother       = (fDecayVertex-fEventVertex).length(); // decay vertex <-> global vertex
    fMinDistCandidates   = HParticleTool::calculateMinimumDistance(base1,dir1,base2,dir2);

    return kTRUE;
}

Bool_t  HParticlePair::calcVectors(Int_t pid1,Int_t pid2,Int_t motherpid,HGeomVector& vertex)
{
    // sets fPID1,fPID2,fMotherPID and fills lorentz vectors
    // opening Angle etc.

    if( (!fcand[0] && !fpair[0]) || (!fcand[1] && !fpair[1])    ) { Error("calcVectors()","Candidates not yet filled! Skipped!"); return kFALSE; }

    fPID[0]    = pid1;
    fPID[1]    = pid2;
    fMotherPID = motherpid;
    fEventVertex = vertex;
    Float_t mass =  HPhysicsConstants::mass(fPID[0]);
    if(mass == -1) mass = HPhysicsConstants::mass(2);

    if(fcand[0]) HParticleTool::fillTLorentzVector(fc[0],fcand[0],fPID[0],HParticlePair::getDoMomentumCorrection());
    else         fc[0]         = (*((TLorentzVector*)fpair[0]));

    mass =  HPhysicsConstants::mass(fPID[1]);
    if(mass == -1) mass = HPhysicsConstants::mass(2);


    if(fcand[1]) HParticleTool::fillTLorentzVector(fc[1],fcand[1],fPID[1],HParticlePair::getDoMomentumCorrection());
    else         fc[1]         = (*((TLorentzVector*)fpair[1]));

    (*((TLorentzVector*)this)) =  fc[0] + fc[1];

    foAngle = fc[0].Angle(fc[1].Vect()) * TMath::RadToDeg();

    calcVertex();

    return kTRUE;
}

HParticleCandSim* HParticlePair::getFirstDaughter()
{
    // returns the pointer to the first valid daughter
    // returns 0 if no valid daughter is found
    HParticleCandSim* c =0;
    if(getCandSim(0)) return getCandSim(0);
    if(getCandSim(1)) return getCandSim(1);

    return c;
}

Bool_t HParticlePair::isGeantDecay()
{
    // returns true if both daughters
    // have a parentTrack > 0 (mother is inside GEANT)
    // both parent tracks are the same
    //

    if(getCandSim(0) && getCandSim(1))
    {
        // decay in GEANT
	if(getCandSim(0)->getGeantParentTrackNum() > 0 &&
	   getCandSim(1)->getGeantParentTrackNum() > 0 &&
           getCandSim(0)->getGeantParentTrackNum() == getCandSim(1)->getGeantParentTrackNum()
	  ) return kTRUE;
    }
    Int_t geninfo_1         = -1;
    Int_t geninfo1_1        = -1;
    Int_t geninfo2_1        = -1;
    Int_t parentTrack1      = -1;
    Int_t grandparentTrack1 = -1;
    Int_t geninfo_2         = -1;
    Int_t geninfo1_2        = -1;
    Int_t geninfo2_2        = -1;
    Int_t parentTrack2      = -1;
    Int_t grandparentTrack2 = -1;

    if(getCandSim(0) && fpair[1])
    {
	          getSourceInfo( 0,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
        fpair[1]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

        if(fpair[1]->isTruePair() && parentTrack1 > 0 && grandparentTrack2 == parentTrack1 ) return kTRUE;

	return kFALSE;
    }

    if(getCandSim(1) && fpair[0])
    {
	          getSourceInfo( 1,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
        fpair[0]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

        if(fpair[0]->isTruePair() && parentTrack1 > 0 && grandparentTrack2 == parentTrack1 ) return kTRUE;

	return kFALSE;
    }

    if(fpair[0] && fpair[1])
    {
	fpair[0]->getSourceInfo(-1,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
        fpair[1]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

	if(fpair[0]->isTruePair() &&
	   fpair[1]->isTruePair() &&
	   grandparentTrack1 > 0 && grandparentTrack2 == grandparentTrack1 ) return kTRUE;

	return kFALSE;
    }

    return kFALSE;
}

Bool_t HParticlePair::isSameExternalSource()
{
    // returns true if both daughters
    // have a parentTrack = 0 (mother is outside GEANT)
    // and the stem form the same source (geninfo > 0,
    // geninfo and geninfo2 are equal)

    Int_t geninfo_1         = -1;
    Int_t geninfo1_1        = -1;
    Int_t geninfo2_1        = -1;
    Int_t parentTrack1      = -1;
    Int_t grandparentTrack1 = -1;
    Int_t geninfo_2         = -1;
    Int_t geninfo1_2        = -1;
    Int_t geninfo2_2        = -1;
    Int_t parentTrack2      = -1;
    Int_t grandparentTrack2 = -1;

    if(getCandSim(0) && getCandSim(1))
    {
	getSourceInfo( 0,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
        getSourceInfo( 1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);


	// decay in PLUTO
	if(  (( parentTrack1 == 0 && parentTrack2 == 0 )             ||      // decay outside GEANT
	      ( parentTrack1  > 0 && parentTrack2 == parentTrack1 )          // decay inside  GEANT
	     )  &&      // decay inside  GEANT
	   geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  )
	{
	    return kTRUE;
	}
	return kFALSE;
    }

    Bool_t truepair=kFALSE;

    if(getCandSim(0) && fpair[1])
    {
                  getSourceInfo( 0,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
	fpair[1]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

	truepair=fpair[1]->isTruePair();

	if(truepair &&
          (
	   (parentTrack1 == 0 && grandparentTrack2 == 0 && parentTrack2 > 0) || // decay outside GEANT
	   (parentTrack1 >  0 && grandparentTrack2 == parentTrack1)             // decay inside  GEANT
	  ) &&
	   geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) return kTRUE;

    } else if (getCandSim(1) && fpair[0]){
	          getSourceInfo( 1,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
	fpair[0]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

	truepair=fpair[0]->isTruePair();

	if(truepair &&
	   (
	    (parentTrack2 == 0 && grandparentTrack1 == 0 && parentTrack1 > 0) || // decay outside GEANT
	    (parentTrack2 >  0 && grandparentTrack1 == parentTrack2)             // decay inside  GEANT
	   )
	   &&
	   geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) return kTRUE;

    } else if (fpair[0] && fpair[1]){
	fpair[0]->getSourceInfo(-1,parentTrack1,grandparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
	fpair[1]->getSourceInfo(-1,parentTrack2,grandparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);

	truepair=fpair[0]->isTruePair();
        if(truepair) truepair=fpair[1]->isTruePair();

	if(truepair &&
           (
	    (grandparentTrack1 == 0 && grandparentTrack2 == 0 && parentTrack1 > 0 && parentTrack2 > 0 ) ||  // decay outside GEANT
	    (grandparentTrack1 < 0 &&  grandparentTrack1 == grandparentTrack2 )                             // decay inside  GEANT
	   )
           &&
	   geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) return kTRUE;
    }

    return kFALSE;
}

void HParticlePair::getSourceInfo(Int_t index,Int_t& parentTrack,Int_t& grandParentTrack,Int_t& geninfo,Int_t& geninfo1,Int_t& geninfo2)
{
    // returns parent track, grand parent track, geninfo and
    // geninfo2 of the daughter with index (first valid daughter if index==-1).


    geninfo =-1;
    geninfo1=-1;
    geninfo2=-1;
    parentTrack = 0;
    grandParentTrack = 0;

    if(!fIsSimulation) return;

    HParticleCandSim* c = 0;

    if     (index == -1)              c = getFirstDaughter();
    else if(index >= 0 && index < 3 ) c = getCandSim(index);

    if(c){
	geninfo  = c->getGeantGeninfo();
	geninfo1 = c->getGeantGeninfo1();
	geninfo2 = c->getGeantGeninfo2();
        parentTrack = c->getGeantParentTrackNum();
        grandParentTrack = c->getGeantGrandParentTrackNum();
    }
}


void    HParticlePair::setTruePair()
{
    if(!fIsSimulation) return;

    if(getCandSim(0) && getCandSim(1))
    {
	if(getCandSim(0)->getGeantParentTrackNum() > 0 && getCandSim(0)->getGeantParentTrackNum() == getCandSim(1)->getGeantParentTrackNum()) {    // decay in GEANT
	    if(!getCandSim(0)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
            if(!getCandSim(1)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x02;
	    return;
	}

	Int_t geninfo_1  = getCandSim(0)->getGeantGeninfo();
        Int_t geninfo2_1 = getCandSim(0)->getGeantGeninfo2();
        Int_t geninfo_2  = getCandSim(1)->getGeantGeninfo();
        Int_t geninfo2_2 = getCandSim(1)->getGeantGeninfo2();

        if(geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) {
	    if(!getCandSim(0)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
	    if(!getCandSim(1)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x02;
	}

	return;
    }

    if(getCandSim(0) && fpair[1])
    {
	Int_t geninfo_1        = -1;
	Int_t geninfo1_1       = -1;
        Int_t geninfo2_1       = -1;
        Int_t parentTrack      = -1;
        Int_t grandparentTrack = -1;


	fpair[1]->getSourceInfo(-1,parentTrack,grandparentTrack,geninfo_1,geninfo1_1,geninfo2_1);

	if(grandparentTrack > 0 && getCandSim(0)->getGeantParentTrackNum() == grandparentTrack) {    // decay in GEANT
	    if(!getCandSim(0)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
            if(fpair[1]->isTruePair())          fstatusFlags = fstatusFlags|0x02;
	    return;
	}

	Int_t geninfo_2  = getCandSim(0)->getGeantGeninfo();
        Int_t geninfo2_2 = getCandSim(0)->getGeantGeninfo2();

        if(geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) {
	    if(!getCandSim(0)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
	    if(fpair[1]->isTruePair())          fstatusFlags = fstatusFlags|0x02;
	}

	return;
    }

    if(getCandSim(1) && fpair[0])
    {
	Int_t geninfo_1        = -1;
	Int_t geninfo1_1       = -1;
        Int_t geninfo2_1       = -1;
        Int_t parentTrack      = -1;
        Int_t grandparentTrack = -1;


	fpair[0]->getSourceInfo(-1,parentTrack,grandparentTrack,geninfo_1,geninfo1_1,geninfo2_1);

	if(grandparentTrack > 0 && getCandSim(1)->getGeantParentTrackNum() == grandparentTrack) {    // decay in GEANT
	    if(!getCandSim(1)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
            if(fpair[0]->isTruePair())          fstatusFlags = fstatusFlags|0x02;
	    return;
	}

	Int_t geninfo_2  = getCandSim(0)->getGeantGeninfo();
        Int_t geninfo2_2 = getCandSim(0)->getGeantGeninfo2();

        if(geninfo_1 > 0                                &&  // external source
	   geninfo_1  == geninfo_2                      &&  // sourceID
	   geninfo2_1 == geninfo2_2                         // parentindex if more than 1 decay of the same source is in the event
	  ) {
	    if(!getCandSim(1)->isGhostTrack() ) fstatusFlags = fstatusFlags|0x01;
	    if(fpair[0]->isTruePair())          fstatusFlags = fstatusFlags|0x02;
	}

	return;
    }




    if(fpair[0] && fpair[1])
    {
	Int_t gparentTrack1 =-1;
	Int_t gparentTrack2 =-1;
	Int_t parentTrack1  =-1;
	Int_t parentTrack2  =-1;
	Int_t geninfo_1  = -1;
	Int_t geninfo1_1 = -1;
	Int_t geninfo2_1 = -1;
	Int_t geninfo_2  = -1;
	Int_t geninfo1_2 = -1;
	Int_t geninfo2_2 = -1;

	fpair[0]->getSourceInfo(-1,parentTrack1,gparentTrack1,geninfo_1,geninfo1_1,geninfo2_1);
	fpair[1]->getSourceInfo(-1,parentTrack2,gparentTrack2,geninfo_2,geninfo1_2,geninfo2_2);


	if(gparentTrack1 > 0 && gparentTrack2 > 0 && gparentTrack1 == gparentTrack2)   { // decay in GEANT
	    fstatusFlags = fstatusFlags|0x01;
	    fstatusFlags = fstatusFlags|0x02;
	    return;
	}

	if(gparentTrack1 == 0 && gparentTrack2 == 0 &&
	   geninfo_1 > 0  &&
	   geninfo_1  == geninfo_2                  &&  // sourceID
	   geninfo2_1 == geninfo2_2                     // parentindex if more than 1 decay of the same source is in the event

	  )   { // decay in PLUTO
	    fstatusFlags = fstatusFlags|0x01;
	    fstatusFlags = fstatusFlags|0x02;
	    return;
	}

    } // end both pair

}

Bool_t  HParticlePair::setPair(HParticleCand* cnd1,Int_t pid1,
			       HParticleCand* cnd2,Int_t pid2,
			       Int_t motherpid,UInt_t pairflags,HGeomVector& vertex)
{
    // sets pairflags , candidates and calls
    // calcvectors. Supposed to be called at initial
    // filling.

    fpairFlags    = pairflags;
    fpair[0]      = 0;
    fpair[1]      = 0;
    fcand[0]      = cnd1;
    fcand[1]      = cnd2;
    fIsSimulation = isSimulation();

    setTruePair();

    return calcVectors(pid1,pid2,motherpid,vertex);
}

Bool_t  HParticlePair::setPair(HParticlePair* cnd1,Int_t pid1,
			       HParticleCand* cnd2,Int_t pid2,
			       Int_t motherpid,UInt_t pairflags,HGeomVector& vertex)
{
    // sets pairflags , candidates and calls
    // calcvectors. Supposed to be called at initial
    // filling. candidate1 will be filled from
    // mother + decay vertex from the pair.

    fpairFlags    = pairflags;
    fpair[0]      = cnd1;
    fpair[1]      = 0;
    fcand[0]      = 0;
    fcand[1]      = cnd2;
    fIsSimulation = isSimulation();


    setTruePair();

    return calcVectors(pid1,pid2,motherpid,vertex);
}

Bool_t  HParticlePair::setPair(HParticlePair* cnd1,Int_t pid1,
			       HParticlePair* cnd2,Int_t pid2,
			       Int_t motherpid,UInt_t pairflags,HGeomVector& vertex)
{
    // sets pairflags , candidates and calls
    // calcvectors. Supposed to be called at initial
    // filling. candidate1 and candidate2 will be filled from
    // mother + decay vertex from the pair.

    fpairFlags    = pairflags;
    fpair[0]      = cnd1;
    fpair[1]      = cnd2;
    fcand[0]      = 0;
    fcand[0]      = 0;
    fIsSimulation = isSimulation();



    setTruePair();

    return calcVectors(pid1,pid2,motherpid,vertex);
}


void HParticlePair::printFlags()
{
    // print pairs flags

    TString out="";
    for(Int_t i=32;i>0;i--){
	if(i%4==0) out+=" ";
	out+= ( (fpairFlags>>(i-1)) & 0x1 );
    }
    cout<<"    bin "<<out.Data()<<" pairflags "<<endl;
    cout<<endl;


    cout<<"kSameRICH               1: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSameRICH)<<endl;
    cout<<"kSameInnerMDC           2: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSameInnerMDC)<<endl;
    cout<<"kSameOuterMDC           3: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSameOuterMDC)<<endl;
    cout<<"kSameMETA               4: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSameMETA)<<endl;
    cout<<"kSamePosPolarity        5: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSamePosPolarity)<<endl;
    cout<<"kSameNegPolarity        6: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSameNegPolarity)<<endl;
    cout<<"kSamePolarity           7: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kSamePolarity)<<endl;

    cout<<"kRICH2                  8: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kRICH2)<<endl;
    cout<<"kFittedInnerMDC2        9: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kFittedInnerMDC2)<<endl;
    cout<<"kFittedOuterMDC2       10: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kFittedOuterMDC2)<<endl;
    cout<<"kOuterMDC2             11: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kOuterMDC2)<<endl;
    cout<<"kRK2                   12: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kRK2)<<endl;
    cout<<"kMETA2                 13: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kMETA2)<<endl;
    cout<<"kIsLepton2             14: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kIsLepton2)<<endl;
    cout<<"kIsUsed2               15: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kIsUsed2)<<endl;

    cout<<"kNoSameRICH            16: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSameRICH)<<endl;
    cout<<"kNoSameInnerMDC        17: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSameInnerMDC)<<endl;
    cout<<"kNoSameOuterMDC        18: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSameOuterMDC)<<endl;
    cout<<"kNoSameMETA            19: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSameMETA)<<endl;
    cout<<"kNoSamePosPolarity     20: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSamePosPolarity)<<endl;
    cout<<"kNoSameNegPolarity     21: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSameNegPolarity)<<endl;
    cout<<"kNoSamePolarity        22: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoSamePolarity)<<endl;

    cout<<"kNoRICH2               23: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoRICH2)<<endl;
    cout<<"kNoFittedInnerMDC2     24: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoFittedInnerMDC2)<<endl;
    cout<<"kNoFittedOuterMDC2     25: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoFittedOuterMDC2)<<endl;
    cout<<"kNoOuterMDC2           26: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoOuterMDC2)<<endl;
    cout<<"kNoRK2                 27: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoRK2)<<endl;
    cout<<"kNoMETA2               28: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoMETA2)<<endl;
    cout<<"kNoIsLepton2           29: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoIsLepton2)<<endl;
    cout<<"kNoIsUsed2             30: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoIsUsed2)<<endl;
    cout<<"kNoUseRICH             31: "<< HParticleTool::isPairsFlagsBit(fpairFlags,kNoUseRICH)<<endl;


}

void  HParticlePair::print(UInt_t selection)
{
    // print option bits
    // bit   1 : print particle infos                      (selection == 1)
    //       2 : print pids, oAngle and polarities         (selection == 2)
    //       3 : print hit indices and META selection info (selection == 4)
    //       4 : print vertex infos                        (selection == 8)
    //       4 : print pair flags                          (selection == 16)
    // default : print all


    cout<<"HParticlePair::print() --------------------------------------"<<endl;

    if( (selection>>0) & 0x01){
	if(fcand[0]) fcand[0]->print();
	if(fcand[1]) fcand[1]->print();
        if(fpair[0]) fpair[0]->print(1);
	if(fpair[1]) fpair[1]->print(1);

    }

    if( (selection>>1) & 0x01){
	cout<<"    pid1 = "   <<fPID[0]
	    <<", pid2 = "     <<fPID[1]
	    <<", motherpid = "<<fMotherPID
	    <<", oAngle = "<<foAngle
	    <<", pol1 = "  << (Int_t) ( (fcand[0]) ?  fcand[0]->getCharge() : 0 )
	    <<", pol2 = "  << (Int_t) ( (fcand[1]) ?  fcand[1]->getCharge() : 0 )
	    <<endl;
    }

    if( ( (selection>>2) & 0x01) && fcand[0] && fcand[1]){
	cout<<"    RICH     : " <<setw(3)<<fcand[0]->getRichInd()    <<" "<<setw(3)<< fcand[1]->getRichInd()<<endl;
	cout<<"    InnerMDC : " <<setw(3)<<fcand[0]->getInnerSegInd()<<" "<<setw(3)<< fcand[1]->getInnerSegInd()<<endl;
	cout<<"    OuterMDC : " <<setw(3)<<fcand[0]->getOuterSegInd()<<" "<<setw(3)<< fcand[1]->getOuterSegInd()<<endl;
	cout<<"    META     : " <<setw(3)<<fcand[0]->getMetaHitInd() <<" "<<setw(3)<<fcand[1]->getMetaHitInd()<<" "<<setw(3)<<fcand[0]->getSelectedMeta()<<" "<<setw(3)<<fcand[1]->getSelectedMeta()<<endl;
	cout<<" systemUsed1 : "<<setw(3)<<fcand[0]->getSystemUsed()        <<", systemUsed2 : "<<setw(3)<<fcand[1]->getSystemUsed()<<endl;
	cout<<" isTofClst1  : "<<setw(3)<<(Int_t)fcand[0]->isTofClstUsed() <<", isTofClst2  : "<<setw(3)<<(Int_t)fcand[1]->isTofClstUsed()<<endl;
	cout<<" isTofHit1   : "<<setw(3)<<(Int_t)fcand[0]->isTofHitUsed()  <<", isTofHit2   : "<<setw(3)<<(Int_t)fcand[1]->isTofHitUsed()<<endl;
	cout<<" isRpcClst1  : "<<setw(3)<<(Int_t)fcand[0]->isRpcClstUsed() <<", isRpcClst2  : "<<setw(3)<<(Int_t)fcand[1]->isRpcClstUsed()<<endl;
	cout<<" isShower1   : "<<setw(3)<<(Int_t)fcand[0]->isShowerUsed()  <<", isShower2   : "<<setw(3)<<(Int_t)fcand[1]->isShowerUsed()<<endl;
    }

    if( (selection>>3) & 0x01){
	cout<<"primary vertex    : "<<setw(12)<<fEventVertex.X() <<", "<<setw(12)<<fEventVertex.Y()<<", "<<setw(12)<<fEventVertex.Z()<<endl;
	cout<<"decay   vertex    : "<<setw(12)<<fDecayVertex.X() <<", "<<setw(12)<<fDecayVertex.Y()<<", "<<setw(12)<<fDecayVertex.Z()<<endl;
	cout<<"vermindist cand1  : "<<setw(12)<<fVerMinDistCand[0] <<", vermindist cand2 : "<<setw(12)<<fVerMinDistCand[1]<<", mindist candidates : "<<setw(12)<<fMinDistCandidates<<endl;
	cout<<"vermindist mother : "<<setw(12)<<fVerMinDistMother<<", verdist mother   : "<<setw(12)<<fVerDistMother<<endl;
    }


    if( (selection>>4) & 0x01) printFlags();

}





void  HParticlePair::clear()
{
    // clears all data elements but not vectors!
    fcand[0]         = 0;
    fcand[1]         = 0;
    fpair[0]         = 0;
    fpair[1]         = 0;
    fpairFlags       = 0;
    fstatusFlags     = 0;
    fPID[0]          = -10;
    fPID[1]          = -10;
    fMotherPID       = -10;
    foAngle          = -1;
    fIsSimulation    = kFALSE;

    fVerMinDistCand[0]   = 0.;
    fVerMinDistCand[1]   = 0.;
    fVerMinDistMother    = 0.;
    fVerDistMother       = 0.;
    fMinDistCandidates   = 0.;
}

