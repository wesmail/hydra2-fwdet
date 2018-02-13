#include "hparticlecand.h"
#include "henergylosscorrpar.h"
#include "hparticleanglecor.h"
#include "hphysicsconstants.h"

#include "TMath.h"
#include <iostream>
#include <iomanip>
using namespace std;

// ROOT's IO and RTTI stuff is added here
ClassImp(HParticleCand)

    //_HADES_CLASS_DESCRIPTION
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    // HParticleCand
    //
    // In this container matched hits from all detectors are stored.
    // The class does not define the algorithm how the matching is done.
    // This is defined in HMetaMatchF2 (coarse matching) and HParticleCandFiller.
    //
    // -------------------------------------------------------------------------
    // Track Cleaner flags
    // To clean the ParticleCand objects from unwanted fakes
    // the object carries a bitfield (32 bit Int_t). The different
    // qualitiy criteria can be used to select the best reconstructed
    // candidate in the case of multiple use of single detector hits.
    // The single bit numbers are defined by enum eFlagBits (see hparticledef.h)
    //
    //  The selection of tracks and setting of the corresponding flags is performed
    //  by HParticleCandCleaner and HParticleCandSorter classes (see the documentation).
    //
    //  For example void  setFlagBit(HParticle::kIsDoubleRICH) sets the bit flag that
    //  the RICH hit has been used in more than one HParticleCand object.
    //  Correspondingly one can ask for the flag with Bool_t isFlagBit(HParticle::kIsDoubleRICH)
    //  void   setFlagBit(Int_t bit) and  Bool_t isFlagBit(Int_t bit) works for the bitnumber (0-31) specified
    //  manually (if one knows what one is doing....)
    //  void   setFlagBit   (eFlagBits bit)
    //  void   unsetFlagBit (eFlagBits bit)
    //  Bool_t isFlagBit    (eFlagBits bit)
    //  void   setFlagBit   (Int_t bit);
    //  void   unsetFlagBit (Int_t bit);
    //  Bool_t isFlagBit    (Int_t bit);
    //  void   setFlagBitByValue(eFlagBits bit, Bool_t val) sets/unsets bit depending on val
    //  void   setFlagBitByValue(Int_t     bit, Bool_t val) sets/unsets bit depending on val
    //  Bool_t isFlagDoubleHit ()           check all Double_t hits flags simultanously (false if none set)
    //  Bool_t isFlagAllBestHit()           check all best hit flags simultanously    (true if all are set)
    //  Int_t  getDoubleHitsLeptons() returns the 4 bit Double_t hit word as Int_t
    //  Int_t  getDoubleHitsHadrons() returns the 3 bit Double_t hit word (excluding RICH) as Int_t
    //  Bool_t isFlagNoBestHit ()           check all best hit flags simultanously    (true if none set)
    //  Int_t  getFlagField()               do what you want with flags field
    //  Bool_t isFlagAND(Int_t num,Int_t fl, ...) check all flags in argument simultanously (false if none set)
    //  Bool_t isFlagOR(Int_t num,Int_t fl, ...)  check all flags in argument simultanously (true if any set)
    //  void   printFlags(TString comment="") prints all flags in binary representation.
    //  void   printInfo()                    prints indices of detector hits + quality criteria + flags
    //
    //  The function
    //  Bool_t select(Bool_t (*function)(HParticleCand* ))  { return  (*function)(this); }
    //  taking a pointer to function with argument HParticleCand* returning a Bool_t allows
    //  todo complex selection on the HParticleCand object as one has
    //  full access to all members of HParticleCand inside the test function.
    //  The function pointer can be a pointer to a global function or member function
    //  of an Object for example:
    //
    //  Bool_t test(HParticleCand* cand){               // global function
    //           if ( put all selection criteria here ) return kTRUE;
    //           else                                   return kFALSE;
    //  } or
    //
    //  static Bool_t dummy::test(HParticleCand* cand){   // member function of object dummy
    //                                                    // needs to be declared static !
    //           if ( put all selection criteria here ) return kTRUE;
    //           else                                   return kFALSE;
    //  }
    //  would be called in the code like
    //
    //  dummy d;
    //  HParticleCand* p= new HParticleCand() // just to get an object
    //  p->select(test))        // global function
    //  p->select(dummy::test)) // member function of object dummy (static call without object creation)
    //  p->select(d.test))      // member function of real object dummy
    //
    //
    ////////////////////////////////////////////////////////////////////////////////






void   HParticleCand::setFlagBit   (Int_t bit)
{
    // set given bit in flag (0-32)
    if(bit >= 0 && bit < 32 ) fFlags |=  ( 0x01 << bit );
    else {
	Error("HParticleCand::setFlagBit(Int_t)","Bit number out of range : %i!",bit);
    }
}

void   HParticleCand::unsetFlagBit (Int_t bit)
{
    // unset given bit in flag (0-32)
    if(bit >= 0 && bit < 32 ) fFlags &= ~( 0x01 << bit );
    else {
	Error("HParticleCand::unsetFlagBit(Int_t)","Bit number out of range : %i!",bit);
    }
}

Bool_t HParticleCand::isFlagBit    (Int_t bit)
{
    // check given bit in flag (0-32)
    // return kTRUE if bit is set
    if (bit >= 0 && bit < 32 )  return (fFlags >> bit ) & 0x01;
    else {
	Error("HParticleCand::isFlagBit(Int_t)","Bit number out of range : %i!",bit);
	return kFALSE;
    }
}

Bool_t HParticleCand::isFlagOR(Int_t num,...)
{
    // check given number of bits in argument in flag (0-32)
    // return kTRUE if any bit is set
    va_list ap;
    va_start(ap,num);

    for(Int_t i=0;i<num;i++){

	Int_t bit=va_arg(ap,Int_t);
	if(bit < 0 || bit > 31)
	{
	    Error("HParticleCand::isFlagOR()","Bit number out of range : i=%i ,%i!",i,bit);
	    va_end(ap);
	    return kFALSE;
	}
	if(isFlagBit(bit)) {
	    va_end(ap);
	    return kTRUE;
	}
    }
    va_end(ap);
    return kFALSE;
}
Bool_t HParticleCand::isFlagAND(Int_t num, ...)
{
    // check given number of bits in argument in flag (0-32)
    // return kTRUE if all bits are set
    va_list ap;
    va_start(ap,num);

    for(Int_t i=0;i<num;i++){
	Int_t bit=va_arg(ap,Int_t);
	if(bit < 0 || bit > 31)
	{
	    Error("HParticleCand::isFlagAND()","Bit number out of range : i=%i ,%i!",i,bit);
	    va_end(ap);
	    return kFALSE;
	}
	if(!isFlagBit(bit)) {
	    va_end(ap);
	    return kFALSE;
	}
    }
    va_end(ap);
    return kTRUE;
}

void HParticleCand::printFlags(TString comment)
{
    // print the flag field in binary representation
    // Comment will be printed at the end of line

    TString out="";
    for(Int_t i=32;i>0;i--){
	if(i%4==0) out+=" ";
	out+= ( (fFlags>>(i-1)) & 0x1 );
    }
    cout<<"bin "<<out.Data()<<" "<<comment.Data()<<endl;
    cout<<endl;
    cout<<"kIsDoubleHitRICH        " <<(Int_t) isFlagBit(kIsDoubleHitRICH)         <<" bit "<<kIsDoubleHitRICH<<endl;
    cout<<"kIsDoubleHitInnerMDC    " <<(Int_t) isFlagBit(kIsDoubleHitInnerMDC)     <<" bit "<<kIsDoubleHitInnerMDC<<endl;
    cout<<"kIsDoubleHitOuterMDC    " <<(Int_t) isFlagBit(kIsDoubleHitOuterMDC)     <<" bit "<<kIsDoubleHitOuterMDC<<endl;
    cout<<"kIsDoubleHitMETA        " <<(Int_t) isFlagBit(kIsDoubleHitMETA)         <<" bit "<<kIsDoubleHitMETA<<endl;
    cout<<"kIsBestHitRICH          " <<(Int_t) isFlagBit(kIsBestHitRICH)           <<" bit "<<kIsBestHitRICH<<endl;
    cout<<"kIsBestHitInnerMDC      " <<(Int_t) isFlagBit(kIsBestHitInnerMDC)       <<" bit "<<kIsBestHitInnerMDC<<endl;
    cout<<"kIsBestHitOuterMDC      " <<(Int_t) isFlagBit(kIsBestHitOuterMDC)       <<" bit "<<kIsBestHitOuterMDC<<endl;
    cout<<"kIsBestHitMETA          " <<(Int_t) isFlagBit(kIsBestHitMETA)           <<" bit "<<kIsBestHitMETA<<endl;
    cout<<"kIsBestRK               " <<(Int_t) isFlagBit(kIsBestRK)                <<" bit "<<kIsBestRK<<endl;
    cout<<"kIsBestRKRKMETA         " <<(Int_t) isFlagBit(kIsBestRKRKMETA)          <<" bit "<<kIsBestRKRKMETA<<endl;
    cout<<"kIsBestRKRKMETARadius   " <<(Int_t) isFlagBit(kIsBestRKRKMETARadius)    <<" bit "<<kIsBestRKRKMETARadius<<endl;
    cout<<"kIsBestUser             " <<(Int_t) isFlagBit(kIsBestUser)              <<" bit "<<kIsBestUser<<endl;

    cout<<"kIsAcceptedHitRICH      " <<(Int_t) isFlagBit(kIsAcceptedHitRICH)       <<" bit "<<kIsAcceptedHitRICH<<endl;
    cout<<"kIsAcceptedHitRICHMDC   " <<(Int_t) isFlagBit(kIsAcceptedHitRICHMDC)    <<" bit "<<kIsAcceptedHitRICHMDC<<endl;
    cout<<"kIsAcceptedHitInnerMDC  " <<(Int_t) isFlagBit(kIsAcceptedHitInnerMDC)   <<" bit "<<kIsAcceptedHitInnerMDC<<endl;
    cout<<"kIsAcceptedHitOuterMDC  " <<(Int_t) isFlagBit(kIsAcceptedHitOuterMDC)   <<" bit "<<kIsAcceptedHitOuterMDC<<endl;
    cout<<"kIsAcceptedHitMETA      " <<(Int_t) isFlagBit(kIsAcceptedHitMETA)       <<" bit "<<kIsAcceptedHitMETA<<endl;
    cout<<"kIsAcceptedRKMETA       " <<(Int_t) isFlagBit(kIsAcceptedRKMETA)        <<" bit "<<kIsAcceptedRKMETA<<endl;
    cout<<"kIsAcceptedRKRICH       " <<(Int_t) isFlagBit(kIsAcceptedRKRICH)        <<" bit "<<kIsAcceptedRKRICH<<endl;
    cout<<"kIsAcceptedRK           " <<(Int_t) isFlagBit(kIsAcceptedRK)            <<" bit "<<kIsAcceptedRK<<endl;
    cout<<"kIsAcceptedRKMETA       " <<(Int_t) isFlagBit(kIsAcceptedRKRKMETA)      <<" bit "<<kIsAcceptedRKRKMETA<<endl;
    cout<<"kIsAcceptedRKMETARadius " <<(Int_t) isFlagBit(kIsAcceptedRKRKMETARadius)<<" bit "<<kIsAcceptedRKRKMETARadius<<endl;
    cout<<"kIsLepton               " <<(Int_t) isFlagBit(kIsLepton)                <<" bit "<<kIsLepton<<endl;
    cout<<"kIsUsed                 " <<(Int_t) isFlagBit(kIsUsed)                  <<" bit "<<kIsUsed<<endl;
    cout<<"kIsRejected             " <<(Int_t) isFlagBit(kIsRejected)              <<" bit "<<kIsRejected<<endl;

}


void HParticleCand::print(UInt_t selection)
{
    // print one candidate object and the flags which have been
    // already set to HParticleCand
    //
    // print option bits
    // bit   1 : print hit indices                         (selection == 1)
    //       2 : print hit indices                         (selection == 2)
    //       3 : print chi2s and matching vars             (selection == 4)
    //       4 : print selection flags                     (selection == 8)
    // default : print all

    cout<<"    "<<" --------------------------------------------"<<endl;
    if( (selection>>0) & 0x01){
	if(isEmc()){
	    cout<<"    "
		<<"sec: "    <<fSector
		<<" cand ind : "<<setw(4)<<fIndex
		<<", R: "    <<setw(3)<<fRichInd
		<<", iM: "   <<setw(4)<<fInnerSegInd
		<<", oM: "   <<setw(4)<<fOuterSegInd
		<<", Tclst: "<<setw(3)<<fTofClstInd
		<<", Thit: " <<setw(3)<<fTofHitInd
		<<", E: "  <<setw(3)<<fShowerInd
		<<", RPC: "  <<setw(3)<<fRpcInd
		<<endl;

	} else {
	    cout<<"    "
		<<"sec: "    <<fSector
		<<" cand ind : "<<setw(4)<<fIndex
		<<", R: "    <<setw(3)<<fRichInd
		<<", iM: "   <<setw(4)<<fInnerSegInd
		<<", oM: "   <<setw(4)<<fOuterSegInd
		<<", Tclst: "<<setw(3)<<fTofClstInd
		<<", Thit: " <<setw(3)<<fTofHitInd
		<<", S: "  <<setw(3)<<fShowerInd
		<<", RPC: "  <<setw(3)<<fRpcInd
		<<endl;
	}
    }
    if( (selection>>1) & 0x01){
	cout<<"    "
	    <<"ichi2    : " <<setw(8)<<fInnerSegmentChi2
	    <<",ochi2   : " <<setw(8)<<fOuterSegmentChi2
	    <<endl;
	if(isEmc())
	{
	    cout<<"    "
		<<"RKM      : "   <<setw(8)<<fMetaMatchQuality
		<<",RKMECA  : "   <<setw(8)<<fMetaMatchQualityShower
		<<",RKMR    : "   <<setw(8)<<fMetaMatchRadius
		<<",RKMRECA : "   <<setw(8)<<fMetaMatchRadiusShower
		<<",RKMdx   : "   <<setw(8)<<fRkMetaDx
		<<",RKMdy   : "   <<setw(8)<<fRkMetaDy <<endl;
	} else {
	    cout<<"    "
		<<"RKM      : "   <<setw(8)<<fMetaMatchQuality
		<<",RKMShr  : "   <<setw(8)<<fMetaMatchQualityShower
		<<",RKMR    : "   <<setw(8)<<fMetaMatchRadius
		<<",RKMRShr : "   <<setw(8)<<fMetaMatchRadiusShower
		<<",RKMdx   : "   <<setw(8)<<fRkMetaDx
		<<",RKMdy   : "   <<setw(8)<<fRkMetaDy <<endl;
	}
	cout<<"    "
	    <<"RKRKM    : "   <<setw(8)<<((fMetaMatchQuality >= 0 && fChi2 > 0 ) ? fMetaMatchQuality*fChi2 : -1.f)
	    <<",RKRKMR  : "   <<setw(8)<<((fMetaMatchRadius  >= 0 && fChi2 > 0 ) ? fMetaMatchRadius*fChi2 : -1.f)
	    <<",RKR     : "   <<setw(8)<<((fRichInd >= 0) ? getRichMatchingQuality() : -1.f)
	    <<",RK      : "   <<setw(8)<<fChi2
	    <<endl;

	cout<<"    "
	    <<"isRingRK : " <<setw(8)<<(((kIsRICHRK&fRingCorr) == kIsRICHRK)? 1 : 0)
	    <<",used    : "   <<setw(8)<<(Int_t)isFlagBit(kIsUsed)
	    <<",lepton  : "   <<setw(8)<<(Int_t)isFlagBit(kIsLepton)
	    <<",fake    : "   <<setw(8)<<(Int_t)isFakeRejected()
	    <<",offVert : "   <<setw(8)<<(Int_t)isOffVertexClust()
	    <<endl;
	
    }

    if( (selection>>2) & 0x01){
	cout<<"PID            : pid   : " <<setw(12)<<(Int_t)fPID <<", charge    : "<<setw(12)<<fCharge                       <<", beta      : "<<setw(12)<<fBeta                 <<", mom       : "<<setw(12)<<fMomentum<<endl;
	cout<<"               : mass2 : " <<setw(12)<<fMass2      <<", MDC dEdx  : "<<setw(12)<<fMdcdEdx                      <<", TOF dEdx  : "<<setw(12)<<fTofdEdx<<endl;
	cout<<"RK seg         : phi   : " <<setw(12)<<fPhi        <<", theta     : "<<setw(12)<<fTheta                <<", r         : "<<setw(12)<<fR                    <<", z         : "<<setw(12)<<fZ<<endl;

	if(isNewRich()){
	    cout<<"RICH           : ncals : " <<setw(12)<<(Int_t)fRingNumPads<<", chi2      : "<<setw(12)<<fRingAmplitude<<", av. chrge : "<<setw(12)<<getAverageRingCharge()<<", centroid   "<<setw(12)<<fRingCentroid<<endl;
	} else {
	    cout<<"RICH           : npads : " <<setw(12)<<(Int_t)fRingNumPads<<", amplitude : "<<setw(12)<<fRingChi2     <<", radius    : "<<setw(12)<<fRingCentroid<<endl;
	}
	cout<<"               : phi   : " <<setw(12)<<fRichPhi    <<", theta     : "<<setw(12)<<fRichTheta    <<", dphi      : "<<setw(12)<<getDeltaPhi()         <<", dtheta    : "<<setw(12)<<getDeltaTheta()<<endl;
	cout<<"SHOWER         : sum0  : " <<setw(12)<<fShowerSum0 <<", sum1      : "<<setw(12)<<fShowerSum1   <<", sum2      : "<<setw(12)<<fShowerSum2           <<", deltasum  : "<<setw(12)<<getShowerDeltaSum()<<endl;
    }

    cout<<"    "<<flush;
    if( (selection>>3) & 0x01) printFlags();
}
 
Float_t HParticleCand::getZprime(Float_t xBeam,Float_t yBeam,Float_t& rPrime)
{
    // xBeam, yBeam - position of beam(target) line in lab.system
    // returns zPrime coordinate value, rPrime as function argument
    Float_t phi    = fPhi * TMath::DegToRad();
    Float_t cosPhi = TMath::Cos(phi);
    Float_t sinPhi = TMath::Sin(phi);
    Float_t tanTh  = TMath::Tan(fTheta * TMath::DegToRad());
    rPrime = fR + xBeam * sinPhi - yBeam * cosPhi;
    if(TMath::Abs(tanTh) > 1.e-10) {
	return fZ + (xBeam * cosPhi + yBeam * sinPhi) / tanTh;
    } else {
	return fZ;
    }
}

Float_t HParticleCand::getZprime(Float_t xBeam,Float_t yBeam) {
    // xBeam, yBeam - position of beam(target) line in lab.system
    // returns zPrime coordinate value. if rPrime is needed too
    // use Float_t HParticleCand::getZprime(Float_t xBeam,Float_t yBeam,Float_t& rPrime)
    // to save some calculations
    Float_t tanTh  = TMath::Tan(fTheta * TMath::DegToRad());
    if(TMath::Abs(tanTh) < 1.e-10) return fZ;
    Float_t phi    = fPhi * TMath::DegToRad();
    return fZ + (xBeam * TMath::Cos(phi) + yBeam * TMath::Sin(phi)) / tanTh;
}

Float_t HParticleCand::getRprime(Float_t xBeam,Float_t yBeam) {
    // xBeam, yBeam - position of beam(target) line in lab.system
    // returns zPrime coordinate value. if zPrime is needed too
    // use Float_t HParticleCand::getZprime(Float_t xBeam,Float_t yBeam,Float_t& rPrime)
    // to save some calculations

    Float_t phi = fPhi * TMath::DegToRad();
    return fR + xBeam * TMath::Sin(phi) - yBeam * TMath::Cos(phi);
}

Float_t HParticleCand::getMomentumPID(Int_t pid)
{
    // returns the momentum of the candidate taking
    // into account the charge of the provided PID  ( mom*charge)
    // In the track reconstruction by default all
    // momenta are recontructed using charge = 1

    if(fMomentum == -1) return fMomentum;

    Float_t mom      = fMomentum;
    Int_t chrg       = TMath::Abs(HPhysicsConstants::charge(pid));
    if(chrg > 0) mom = mom*chrg;

    return mom;
}

Float_t HParticleCand::getMomentumCorrectionPID(Int_t pid)
{
    // return the momentum correction for the candidate
    // due to material budget in target + rich + absorbers.
    // requires HEnergyLossCorrPar. The correction takes
    // into acount the charge of the particle ( > 1). Units
    // are MeV/c

    if(fMomentum == -1) return 0;

    static Bool_t doError = kTRUE;

    HEnergyLossCorrPar* corrpar = HEnergyLossCorrPar::getObject();
    Float_t corr = 0.;

    if(corrpar){
	Double_t mom = fMomentum;
	Int_t chrg   = TMath::Abs(HPhysicsConstants::charge(pid));
        if(chrg > 0) mom = mom*chrg;
	corr         = (Float_t) corrpar->getDeltaMom(pid,mom,fTheta);
    } else {
	if(doError) Error("getMomCorr()","HEnergyLossCorrPar not initialized!");
	doError = kFALSE;
    }

    return corr;
}

Float_t HParticleCand::getCorrectedMomentumPID(Int_t pid)
{
    // return the corrected momentum for the candidate
    // due to material budget in target + rich + absorbers.
    // requires HEnergyLossCorrPar. The correction takes
    // into acount the charge of the particle ( > 1). Units
    // are MeV/c

    if(fMomentum == -1) return fMomentum;

    Float_t mom      = fMomentum;
    Int_t chrg       = TMath::Abs(HPhysicsConstants::charge(pid));
    if(chrg > 0) mom = mom*chrg;

    Float_t corr = getMomentumCorrectionPID(pid);
    return  mom + corr;
}

Float_t HParticleCand::getMass2PID(Int_t pid)
{
    // returns the mass2 of the candidate taking
    // into account the charge of the provided PID  ( mom*charge)
    // In the track reconstruction by default all
    // momenta are recontructed using charge = 1

    if(fMass2 == -1) return fMass2;

    Float_t mom      = fMomentum;
    Int_t chrg       = TMath::Abs(HPhysicsConstants::charge(pid));
    if(chrg > 0) mom = mom*chrg;
    return (mom*mom)/(1-fBeta*fBeta)/(fBeta*fBeta);
}

Float_t HParticleCand::getMass2CorrectionPID(Int_t pid)
{
    // return the mass2 correction (mass2_corr-mass2) for the candidate
    // due to material budget in target + rich + absorbers.
    // requires HEnergyLossCorrPar. The correction takes
    // into acount the charge of the particle ( > 1). Units
    // are MeV/c

    if(fMass2 == -1) return 0;
    Float_t mass2     = getMass2PID(pid);
    Float_t mass2corr = getCorrectedMass2PID(pid);

    return mass2corr-mass2;
}

Float_t HParticleCand::getCorrectedMass2PID(Int_t pid)
{
    // return the corrected mass2 for the candidate
    // due to material budget in target + rich + absorbers.
    // requires HEnergyLossCorrPar. The correction takes
    // into acount the charge of the particle ( > 1). Units
    // are MeV/c

    if(fMass2 == -1) return fMass2;

    Float_t mom = getCorrectedMomentumPID(pid);

    return (mom*mom)/(1-fBeta*fBeta)/(fBeta*fBeta);
}
Float_t HParticleCand::getRichMatchingQualityNorm()
{
    // norm delta theta phi by width of distributions,
    // do some correction for theta,phi and momentum
    // (W. Koenig, see HParticleAngleCor::matchRingTrack() )
    return HParticleAngleCor::matchRingTrack(this);
}

Int_t HParticleCand::getNLayer(UInt_t io)
{
    // i0 :  0 = inner seg , 1 = outer seg, 2 = both segs
    Int_t sum=0;
    if(io<2){
	for(UInt_t i=0;i<12;i++){
	    sum+= getLayer(io,i);
	}
    } else {
	for(UInt_t i=0;i<12;i++){
	    sum+= getLayer(0,i);
	    sum+= getLayer(1,i);
	}
    }

    return sum;
}

Int_t HParticleCand::getNLayerMod(UInt_t mod)
{
    Int_t sum=0;
    UInt_t io =0;
    if(mod>1) io =1;
    UInt_t first = 0;
    UInt_t last  = 12;
    if(mod==1||mod==3)first=6;
    if(mod==0||mod==2)last =6;

    for(UInt_t i=first;i<last;i++){
        sum+= getLayer(io,i);
    }
   return sum;
}

void HParticleCand::printLayers()
{
    TString out="";
    for(Int_t i=32;i>0;i--){
	if(i<25&&i%6==0) out+=" ";
	out+= ( (fLayers>>(i-1)) & 0x1 );
    }
    cout<<"    layers "<<out.Data()<<endl;
}

void HParticleCand::Streamer(TBuffer &R__b)
{
   // Stream an object of class HParticleCand.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TLorentzVector::Streamer(R__b);
      R__b >> fFlags;
      R__b >> fPID;
      R__b >> fSector;
      R__b >> fSystem;
      R__b >> fCharge;
      R__b >> fTofRec;
      R__b >> fRingCorr;
      R__b >> fIndex;
      R__b >> fBeta;
      R__b >> fMomentum;
      R__b >> fMass2;
      R__b >> fPhi;
      R__b >> fTheta;
      R__b >> fR;
      R__b >> fZ;
      R__b >> fChi2;
      R__b >> fDistanceToMetaHit;
      R__b >> fMdcdEdx;
      R__b >> fTofdEdx;
      R__b >> fInnerSegmentChi2;
      R__b >> fOuterSegmentChi2;
      R__b >> fAngleToNearbyFittedInner;
      R__b >> fAngleToNearbyUnfittedInner;
      R__b >> fRingNumPads;
      R__b >> fRingAmplitude;
      R__b >> fRingHouTra;
      R__b >> fRingPatternMatrix;
      R__b >> fRingCentroid;
      R__b >> fRichPhi;
      R__b >> fRichTheta;
      if(R__v > 8) R__b >> fRingChi2;
      else                 fRingChi2 =-1000;
      R__b >> fMetaMatchQuality;
      if(R__v > 1) R__b >> fMetaMatchQualityShower;
      else                 fMetaMatchQualityShower = -1;
      if(R__v > 3){
	  R__b >> fMetaMatchRadius;
	  R__b >> fMetaMatchRadiusShower;
      } else {
            fMetaMatchRadius=-1;
	    fMetaMatchRadiusShower=-1;
      }
      if(R__v > 6){
	  R__b >> fRkMetaDx;
	  R__b >> fRkMetaDy;
      } else {
            fRkMetaDx=-1000;
	    fRkMetaDy=-1000;
      }

      if(R__v > 7){
	  R__b >> fBetaOrg;
	  R__b >> fMomentumOrg;
	  R__b >> fDistanceToMetaHitOrg;
      } else {
	  fBetaOrg = fBeta;
          fMomentumOrg = fMomentum;
          fDistanceToMetaHitOrg = fDistanceToMetaHit;
      }
      R__b >> fShowerSum0;
      R__b >> fShowerSum1;
      R__b >> fShowerSum2;

      if(R__v > 9) R__b >> fEmcFlags;
      else                 fEmcFlags=0;

      if(R__v > 1) R__b >> fSelectedMeta;
      else                 fSelectedMeta =-1;

      R__b >> fMetaInd;

      if(R__v == 5 ) {
	  Short_t tmpInd=-1;
	  R__b >> tmpInd;
	  fRichInd  = (tmpInd&(0xFF))-1;
	  fRichBTInd= (tmpInd>>8&(0xFF))-1;
      } else {
	  if(R__v > 5){
	      R__b >> fRichInd;
              R__b >> fRichBTInd;
	  } else {
	      R__b >> fRichInd;
	      fRichBTInd =-1;
	  }
      }

      R__b >> fInnerSegInd;
      R__b >> fOuterSegInd;
      R__b >> fRpcInd;
      R__b >> fShowerInd;
      R__b >> fTofHitInd;
      R__b >> fTofClstInd;
      if(R__v > 2) R__b >> fLayers; else fLayers = 0;
      if(R__v > 4) R__b >> fWires;  else fWires  = 0;
      if(R__v > 4) R__b >> fmetaAddress;  else fmetaAddress = 0;

      R__b.CheckByteCount(R__s, R__c, HParticleCand::IsA());
   } else {
      R__c = R__b.WriteVersion(HParticleCand::IsA(), kTRUE);
      TLorentzVector::Streamer(R__b);
      R__b << fFlags;
      R__b << fPID;
      R__b << fSector;
      R__b << fSystem;
      R__b << fCharge;
      R__b << fTofRec;
      R__b << fRingCorr;
      R__b << fIndex;
      R__b << fBeta;
      R__b << fMomentum;
      R__b << fMass2;
      R__b << fPhi;
      R__b << fTheta;
      R__b << fR;
      R__b << fZ;
      R__b << fChi2;
      R__b << fDistanceToMetaHit;
      R__b << fMdcdEdx;
      R__b << fTofdEdx;
      R__b << fInnerSegmentChi2;
      R__b << fOuterSegmentChi2;
      R__b << fAngleToNearbyFittedInner;
      R__b << fAngleToNearbyUnfittedInner;
      R__b << fRingNumPads;
      R__b << fRingAmplitude;
      R__b << fRingHouTra;
      R__b << fRingPatternMatrix;
      R__b << fRingCentroid;
      R__b << fRichPhi;
      R__b << fRichTheta;
      R__b << fRingChi2;
      R__b << fMetaMatchQuality;
      R__b << fMetaMatchQualityShower;
      R__b << fMetaMatchRadius;
      R__b << fMetaMatchRadiusShower;
      R__b << fRkMetaDx;
      R__b << fRkMetaDy;
      R__b << fBetaOrg;
      R__b << fMomentumOrg;
      R__b << fDistanceToMetaHitOrg;
      R__b << fShowerSum0;
      R__b << fShowerSum1;
      R__b << fShowerSum2;
      R__b << fEmcFlags;
      R__b << fSelectedMeta;
      R__b << fMetaInd;
      R__b << fRichInd;
      R__b << fRichBTInd;
      R__b << fInnerSegInd;
      R__b << fOuterSegInd;
      R__b << fRpcInd;
      R__b << fShowerInd;
      R__b << fTofHitInd;
      R__b << fTofClstInd;
      R__b << fLayers;
      R__b << fWires;
      R__b << fmetaAddress;
      R__b.SetByteCount(R__c, kTRUE);
   }
}




