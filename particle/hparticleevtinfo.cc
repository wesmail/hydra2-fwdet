//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
// HParticleEvtInfo
//
// A container keeping characteristic information about event
// (detector hit multiplicity, particle track multiplicity,
//  reaction plane)
//
////////////////////////////////////////////////////////////////////////////


#include "hparticleevtinfo.h"

#include "TBuffer.h"

// ----------------------------------------------------------------------------

ClassImp(HParticleEvtInfo)
  
// ----------------------------------------------------------------------------
HParticleEvtInfo::HParticleEvtInfo(void)
{
   // Default HParticleEvtInfo constructor

   clearMembers();
}

// ----------------------------------------------------------------------------
void HParticleEvtInfo::clearMembers(void)
{
   // Just set all member variables to 0.

   for (Int_t s = 0; s < MAXSECTOR; ++s)
   {
      aRichMult[s]         = 0;
      aRichCorrMult[s]     = 0;
      aRichClusMult[s]     = 0;
      aTofMult[s]          = 0;
      aRpcMult[s]          = 0;
      aRpcMultHit[s]       = 0;
      aTofMultCut[s]          = 0;
      aRpcMultCut[s]          = 0;
      aRpcMultHitCut[s]       = 0;
      aShowerMult[s]       = 0;
      aParticleCandMult[s] = 0;
      aSelectedParticleCandMult[s] = 0;
      aPrimaryParticleCandMult [s] = 0;
      fMeanMultSec    [s] = 0;
      fMeanMIPSMultSec[s] = 0;
      fMeanLEPMultSec [s] = 0;
      fSigmaMultSec    [s] = 0;
      fSigmaMIPSMultSec[s] = 0;
      fSigmaLEPMultSec [s] = 0;

   for (Int_t m = 0; m < 4; ++m)
   {
       for (Int_t l = 0; l < 6; ++l)
       {
         aMdcWires      [s][m][l]=0;
         aMdcWiresUnused[s][m][l]=0;
         aMdcWiresRaw   [s][m][l]=0;
       }
   }


      for (Int_t io = 0; io < MAXIOSEG; ++io)
      {
	 aMdcClusMult[s][io]        = 0;
	 aMdcSegFittedMult[s][io]   = 0;
	 aMdcSegUnfittedMult[s][io] = 0;
      }
   }

   nFWallMult   = 0;
   fFWallVector = 0.0;    
   fRPlanePhi   = 0.0;

   fwallPhiA = 0.;
   fwallPhiB = 0.;
   fwallPhiAB= 0.;
   fwallNA   = 0.;
   fwallNB   = 0.;

   fEventSelector = 0;
   fMeanMult      = 0;                                    // floating mean for all selected cands
   fMeanMIPSMult  = 0;
   fMeanLEPMult   = 0;
   fSigmaMult      = 0;                                    // floating sigma for all selected cands
   fSigmaMIPSMult  = 0;
   fSigmaLEPMult   = 0;

}

void HParticleEvtInfo::Streamer(TBuffer &R__b)
{
   // Stream an object of class HParticleEvtInfo.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b.ReadStaticArray((int*)aRichMult);
      R__b.ReadStaticArray((int*)aRichCorrMult);
      R__b.ReadStaticArray((int*)aRichClusMult);
      R__b.ReadStaticArray((int*)aMdcClusMult);
      R__b.ReadStaticArray((int*)aMdcSegFittedMult);
      R__b.ReadStaticArray((int*)aMdcSegUnfittedMult);
      R__b.ReadStaticArray((int*)aTofMult);
      R__b.ReadStaticArray((int*)aShowerMult);
      R__b.ReadStaticArray((int*)aRpcMult);
      if(R__v > 1){
	  R__b.ReadStaticArray((int*)aRpcMultHit);
      } else {
	  for (Int_t s = 0; s < MAXSECTOR; ++s) aRpcMultHit[s] = 0;
      }
      if(R__v > 2){
	  R__b.ReadStaticArray((int*)aTofMultCut);
	  R__b.ReadStaticArray((int*)aRpcMultCut);
	  R__b.ReadStaticArray((int*)aRpcMultHitCut);
      } else {
	  for (Int_t s = 0; s < MAXSECTOR; ++s) {
	      aTofMultCut[s] = 0;
	      aRpcMultCut[s] = 0;
	      aRpcMultHitCut[s] = 0;
	  }
      }
      R__b.ReadStaticArray((int*)aParticleCandMult);
      R__b.ReadStaticArray((int*)aSelectedParticleCandMult);
      if(R__v > 4){
	  R__b.ReadStaticArray((int*)aPrimaryParticleCandMult);
      }


      if(R__v > 3){
	  R__b.ReadStaticArray((short*)aMdcWires);
	  R__b.ReadStaticArray((short*)aMdcWiresUnused);
      } else {
	  for (Int_t s = 0; s < MAXSECTOR; ++s){
	      for (Int_t m = 0; m < 4; ++m){
		  for (Int_t l = 0; l < 6; ++l){
                          aMdcWires      [s][m][l]=0;
                          aMdcWiresUnused[s][m][l]=0;
		  }
	      }
	  }
      }
      if(R__v > 4){
	  R__b.ReadStaticArray((short*)aMdcWiresRaw);
      } else {
	  for (Int_t s = 0; s < MAXSECTOR; ++s){
	      for (Int_t m = 0; m < 4; ++m){
		  for (Int_t l = 0; l < 6; ++l){
		      aMdcWiresRaw[s][m][l]=0;
		  }
	      }
	  }
      }


      R__b >> nFWallMult;
      R__b >> fFWallVector;
      R__b >> fRPlanePhi;
      if(R__v > 2){
	  R__b >> fwallPhiA ;
	  R__b >> fwallPhiB ;
	  R__b >> fwallPhiAB;
	  R__b >> fwallNA   ;
	  R__b >> fwallNB   ;
      } else {
	  fwallPhiA = 0.;
	  fwallPhiB = 0.;
	  fwallPhiAB= 0.;
	  fwallNA   = 0.;
	  fwallNB   = 0.;
      }


      if(R__v > 4){
	  R__b >> fEventSelector;
      } else {
	  fEventSelector = 0 ;
      }

      if(R__v > 4){

	  R__b >> fMeanMult ;
	  R__b >> fMeanMIPSMult ;
	  R__b >> fMeanLEPMult ;
	  R__b.ReadStaticArray((float*)fMeanMultSec);
	  R__b.ReadStaticArray((float*)fMeanMIPSMultSec);
	  R__b.ReadStaticArray((float*)fMeanLEPMultSec);
      } else {
	  fMeanMult     = 0;
          fMeanMIPSMult = 0;
          fMeanLEPMult  = 0;
	  for (Int_t s = 0; s < MAXSECTOR; ++s){
	      fMeanMultSec    [s] = 0;
	      fMeanMIPSMultSec[s] = 0;
	      fMeanLEPMultSec [s] = 0;
	  }
      }
      if(R__v > 5){

	  R__b >> fSigmaMult ;
	  R__b >> fSigmaMIPSMult ;
	  R__b >> fSigmaLEPMult ;
	  R__b.ReadStaticArray((float*)fSigmaMultSec);
	  R__b.ReadStaticArray((float*)fSigmaMIPSMultSec);
	  R__b.ReadStaticArray((float*)fSigmaLEPMultSec);
      } else {
	  fSigmaMult     = 0;
          fSigmaMIPSMult = 0;
          fSigmaLEPMult  = 0;
	  for (Int_t s = 0; s < MAXSECTOR; ++s){
	      fSigmaMultSec    [s] = 0;
	      fSigmaMIPSMultSec[s] = 0;
	      fSigmaLEPMultSec [s] = 0;
	  }
      }
      R__b.CheckByteCount(R__s, R__c, HParticleEvtInfo::IsA());
   } else {
      R__c = R__b.WriteVersion(HParticleEvtInfo::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b.WriteArray(aRichMult, 6);
      R__b.WriteArray(aRichCorrMult, 6);
      R__b.WriteArray(aRichClusMult, 6);
      R__b.WriteArray((int*)aMdcClusMult, 12);
      R__b.WriteArray((int*)aMdcSegFittedMult, 12);
      R__b.WriteArray((int*)aMdcSegUnfittedMult, 12);
      R__b.WriteArray(aTofMult, 6);
      R__b.WriteArray(aShowerMult, 6);
      R__b.WriteArray(aRpcMult, 6);
      R__b.WriteArray(aRpcMultHit, 6);
      R__b.WriteArray(aTofMultCut, 6);
      R__b.WriteArray(aRpcMultCut, 6);
      R__b.WriteArray(aRpcMultHitCut, 6);
      R__b.WriteArray(aParticleCandMult, 6);
      R__b.WriteArray(aSelectedParticleCandMult, 6);
      R__b.WriteArray(aPrimaryParticleCandMult , 6);
      R__b.WriteArray((short*)aMdcWires, 6*4*6);
      R__b.WriteArray((short*)aMdcWiresUnused, 6*4*6);
      R__b.WriteArray((short*)aMdcWiresRaw   , 6*4*6);
      R__b << nFWallMult;
      R__b << fFWallVector;
      R__b << fRPlanePhi;
      R__b << fwallPhiA ;
      R__b << fwallPhiB ;
      R__b << fwallPhiAB;
      R__b << fwallNA   ;
      R__b << fwallNB   ;
      R__b << fEventSelector;
      R__b << fMeanMult ;
      R__b << fMeanMIPSMult ;
      R__b << fMeanLEPMult ;
      R__b.WriteArray((float*)fMeanMultSec    ,6);
      R__b.WriteArray((float*)fMeanMIPSMultSec,6);
      R__b.WriteArray((float*)fMeanLEPMultSec ,6);
      R__b << fSigmaMult ;
      R__b << fSigmaMIPSMult ;
      R__b << fSigmaLEPMult ;
      R__b.WriteArray((float*)fSigmaMultSec    ,6);
      R__b.WriteArray((float*)fSigmaMIPSMultSec,6);
      R__b.WriteArray((float*)fSigmaLEPMultSec ,6);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

