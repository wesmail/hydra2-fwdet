#include "hvirtualcand.h"
#include "hphysicsconstants.h"

#include "TBuffer.h"

// ROOT's IO and RTTI stuff is added here
ClassImp(HVirtualCand)

    //_HADES_CLASS_DESCRIPTION
    ////////////////////////////////////////////////////////////////////////////////
    //
    //
    // HVirtualCand
    //
    // Virtual class for Candidates
    //
    //
    ////////////////////////////////////////////////////////////////////////////////


HVirtualCand::~HVirtualCand() {}

void HVirtualCand::print(UInt_t selection)
{
}

Float_t HVirtualCand::getMomentumPID(Int_t pid)
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

void HVirtualCand::Streamer(TBuffer &R__b)
{
   // Stream an object of class HVirtualCand.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TLorentzVector::Streamer(R__b);
      R__b >> fCharge;
      R__b >> fTofRec;
      R__b >> fBeta;
      R__b >> fMomentum;
      R__b >> fMass2;
      R__b >> fPhi;
      R__b >> fTheta;
      R__b >> fR;
      R__b >> fZ;
      R__b >> fChi2;
      R__b.CheckByteCount(R__s, R__c, HVirtualCand::IsA());
   } else {
      R__c = R__b.WriteVersion(HVirtualCand::IsA(), kTRUE);
      TLorentzVector::Streamer(R__b);
      R__b << fCharge;
      R__b << fTofRec;
      R__b << fBeta;
      R__b << fMomentum;
      R__b << fMass2;
      R__b << fPhi;
      R__b << fTheta;
      R__b << fR;
      R__b << fZ;
      R__b << fChi2;
      R__b.SetByteCount(R__c, kTRUE);
   }
}
