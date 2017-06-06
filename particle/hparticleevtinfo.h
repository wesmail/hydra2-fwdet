
#ifndef __HPARTICLEEVTINFO_H__
#define __HPARTICLEEVTINFO_H__


#include "TObject.h"

#define MAXSECTOR 6
#define MAXIOSEG 2
// ----------------------------------------------------------------------------
class HParticleEvtInfo : public TObject
{
private:

    // hit multiplicity per detector (sector wise)
    Int_t aRichMult                [MAXSECTOR];            // RICH rings
    Int_t aRichCorrMult            [MAXSECTOR];            // correlated RICH rings with a RK track
    Int_t aRichClusMult            [MAXSECTOR];            // RICH charged particle clusters
    Int_t aMdcClusMult             [MAXSECTOR][MAXIOSEG];  // MDC cluster (segment wise)
    Int_t aMdcSegFittedMult        [MAXSECTOR][MAXIOSEG];  // fitted MDC segments
    Int_t aMdcSegUnfittedMult      [MAXSECTOR][MAXIOSEG];  // unfitted MDc segments
    Int_t aTofMult                 [MAXSECTOR];            // TOF hit multiplicity
    Int_t aShowerMult              [MAXSECTOR];            // Shower/TOFino hit or Emc cluster multiplicity
    Int_t aRpcMult                 [MAXSECTOR];            // RPC cluster multiplicity
    Int_t aRpcMultHit              [MAXSECTOR];            // RPC hit multiplicity
    Int_t aTofMultCut              [MAXSECTOR];            // TOF hit multiplicity after cut < 35 ns
    Int_t aRpcMultCut              [MAXSECTOR];            // RPC cluster multiplicity  after cut < 25 ns
    Int_t aRpcMultHitCut           [MAXSECTOR];            // RPC hit multiplicity      after cut < 25 ns
    Int_t aParticleCandMult        [MAXSECTOR];            // Particle Candidate entries
    Int_t aSelectedParticleCandMult[MAXSECTOR];            // selected ParticleTrack Candidate entries (flagged kIsUsed)
    Int_t aPrimaryParticleCandMult [MAXSECTOR];            // selected ParticleTrack Candidate from primary vertex entries (flagged kIsUsed)
    Short_t aMdcWires              [MAXSECTOR][4][6];      // all wire of MDC fired in the event (sec,mod,lay)
    Short_t aMdcWiresUnused        [MAXSECTOR][4][6];      // all wire of MDC fired in the event which are not used inside segments (sec,mod,lay)
    Short_t aMdcWiresRaw           [MAXSECTOR][4][6];      // all wire of MDC fired before timecuts (sec,mod,lay)
    // reaction plane information
    Int_t    nFWallMult;                                   // forward wall hits
    Double_t fFWallVector; 	                           // not yet implemented: vector size of all FW positions
    Double_t fRPlanePhi; 	                           // not yet implemented: phi of reaction plane
    Float_t  fwallPhiA;                                    // wall sample A phi
    Float_t  fwallPhiB;                                    // wall sample B phi
    Float_t  fwallPhiAB;                                   // delta phi sample A-B
    Float_t  fwallNA;                                      // n hits in sample A
    Float_t  fwallNB;                                      // n hits in sample B
    UInt_t   fEventSelector;                               // flags for eventselection (hparticeldef.h eEventSelect)

    Float_t  fMeanMult;                                    // floating mean for all selected cands
    Float_t  fMeanMIPSMult;                                // floating mean for all selected mips cands
    Float_t  fMeanLEPMult;                                 // floating mean for all selected lep cands
    Float_t  fMeanMultSec    [6];                          // floating mean for all selected cands per sector
    Float_t  fMeanMIPSMultSec[6];                          // floating mean for all selected mips cands per sector
    Float_t  fMeanLEPMultSec [6];                          // floating mean for all selected mips cands per sector
    Float_t  fSigmaMult;                                    // floating sigma for all selected cands
    Float_t  fSigmaMIPSMult;                                // floating sigma for all selected mips cands
    Float_t  fSigmaLEPMult;                                 // floating sigma for all selected lep cands
    Float_t  fSigmaMultSec    [6];                          // floating sigma for all selected cands per sector
    Float_t  fSigmaMIPSMultSec[6];                          // floating sigma for all selected mips cands per sector
    Float_t  fSigmaLEPMultSec [6];                          // floating sigma for all selected mips cands per sector


public:

    HParticleEvtInfo(void);
    ~HParticleEvtInfo(void) {}


    void setRichMult                (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRichMult                [s] = m[s]; }
    void setRichCorrMult            (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRichCorrMult            [s] = m[s]; }
    void setRichClusMult            (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRichClusMult            [s] = m[s]; }
    void setTofMult                 (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aTofMult                 [s] = m[s]; }
    void setRpcMult                 (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRpcMult                 [s] = m[s]; }
    void setRpcMultHit              (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRpcMultHit              [s] = m[s]; }
    void setTofMultCut              (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aTofMultCut              [s] = m[s]; }
    void setRpcMultCut              (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRpcMultCut              [s] = m[s]; }
    void setRpcMultHitCut           (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aRpcMultHitCut           [s] = m[s]; }
    void setShowerMult              (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aShowerMult              [s] = m[s]; }
    void setEmcMult                 (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aShowerMult              [s] = m[s]; }
    void setParticleCandMult        (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aParticleCandMult        [s] = m[s]; }
    void setSelectedParticleCandMult(const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aSelectedParticleCandMult[s] = m[s]; }
    void setPrimaryParticleCandMult (const Int_t m[MAXSECTOR]) { for (Int_t s = 0; s < MAXSECTOR; ++s) aPrimaryParticleCandMult [s] = m[s]; }
    void setMdcClusMult(const Int_t m[MAXSECTOR][MAXIOSEG]) {
	for (Int_t s = 0; s < MAXSECTOR; ++s)
	    for (Int_t io = 0; io < MAXIOSEG; ++io)
		aMdcClusMult[s][io] = m[s][io];
    }
    void setMdcSegFittedMult(const Int_t m[MAXSECTOR][MAXIOSEG]) {
	for (Int_t s = 0; s < MAXSECTOR; ++s)
	    for (Int_t io = 0; io < MAXIOSEG; ++io)
		aMdcSegFittedMult[s][io] = m[s][io];
    }
    void setMdcSegUnfittedMult(const Int_t m[MAXSECTOR][MAXIOSEG]) {
	for (Int_t s = 0; s < MAXSECTOR; ++s)
	    for (Int_t io = 0; io < MAXIOSEG; ++io)
		aMdcSegUnfittedMult[s][io] = m[s][io];
    }

    void setMdcWires      (Int_t s,Int_t m,Int_t l,Int_t nw) { aMdcWires      [s][m][l] = nw; }
    void setMdcWiresUnused(Int_t s,Int_t m,Int_t l,Int_t nw) { aMdcWiresUnused[s][m][l] = nw; }
    void setMdcWiresRaw   (Int_t s,Int_t m,Int_t l,Int_t nw) { aMdcWiresRaw   [s][m][l] = nw; }

    void setFWallMult  (Int_t m)    { nFWallMult   = m; }
    void setFWallVector(Double_t d) { fFWallVector = d; }
    void setRPlanePhi  (Double_t d) { fRPlanePhi   = d; }
    void setPhiA       (Float_t val){ fwallPhiA    = val; }
    void setPhiB       (Float_t val){ fwallPhiB    = val; }
    void setPhiAB      (Float_t val){ fwallPhiAB   = val; }
    void setNA         (Float_t val){ fwallNA      = val; }
    void setNB         (Float_t val){ fwallNB      = val; }

    void setMeanMult    (Float_t mean)          { fMeanMult        = mean; }
    void setMeanMIPSMult(Float_t mean)          { fMeanMIPSMult    = mean; }
    void setMeanLEPMult (Float_t mean)          { fMeanLEPMult     = mean; }
    void setMeanMult    (Int_t s,Float_t mean)  { fMeanMultSec[s]     = mean; }
    void setMeanMIPSMult(Int_t s,Float_t mean)  { fMeanMIPSMultSec[s] = mean; }
    void setMeanLEPMult (Int_t s,Float_t mean)  { fMeanLEPMultSec [s] = mean; }
    void setSigmaMult    (Float_t sig)          { fSigmaMult        = sig; }
    void setSigmaMIPSMult(Float_t sig)          { fSigmaMIPSMult    = sig; }
    void setSigmaLEPMult (Float_t sig)          { fSigmaLEPMult     = sig; }
    void setSigmaMult    (Int_t s,Float_t sig)  { fSigmaMultSec[s]     = sig; }
    void setSigmaMIPSMult(Int_t s,Float_t sig)  { fSigmaMIPSMultSec[s] = sig; }
    void setSigmaLEPMult (Int_t s,Float_t sig)  { fSigmaLEPMultSec [s] = sig; }

    Float_t getMeanMult    ()         { return fMeanMult          ; }
    Float_t getMeanMIPSMult()         { return fMeanMIPSMult      ; }
    Float_t getMeanLEPMult ()         { return fMeanLEPMult       ; }
    Float_t getMeanMult    (Int_t s)  { return fMeanMultSec[s]    ; }
    Float_t getMeanMIPSMult(Int_t s)  { return fMeanMIPSMultSec[s]; }
    Float_t getMeanLEPMult (Int_t s)  { return fMeanLEPMultSec [s]; }

    Float_t getSigmaMult    ()         { return fSigmaMult          ; }
    Float_t getSigmaMIPSMult()         { return fSigmaMIPSMult      ; }
    Float_t getSigmaLEPMult ()         { return fSigmaLEPMult       ; }
    Float_t getSigmaMult    (Int_t s)  { return fSigmaMultSec[s]    ; }
    Float_t getSigmaMIPSMult(Int_t s)  { return fSigmaMIPSMultSec[s]; }
    Float_t getSigmaLEPMult (Int_t s)  { return fSigmaLEPMultSec [s]; }




    Int_t getRichMult                (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRichMult                [s]  :  -1; }
    Int_t getRichCorrMult            (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRichCorrMult            [s]  :  -1; }
    Int_t getRichClusMult            (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRichClusMult            [s]  :  -1; }
    Int_t getTofMult                 (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aTofMult                 [s]  :  -1; }
    Int_t getRpcMult                 (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRpcMult                 [s]  :  -1; }
    Int_t getRpcMultHit              (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRpcMultHit              [s]  :  -1; }
    Int_t getTofMultCut              (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aTofMultCut              [s]  :  -1; }
    Int_t getRpcMultCut              (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRpcMultCut              [s]  :  -1; }
    Int_t getRpcMultHitCut           (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aRpcMultHitCut           [s]  :  -1; }
    Int_t getShowerMult              (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aShowerMult              [s]  :  -1; }
    Int_t getEmcMult                 (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aShowerMult              [s]  :  -1; }
    Int_t getParticleCandMult        (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aParticleCandMult        [s]  :  -1; }
    Int_t getSelectedParticleCandMult(Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aSelectedParticleCandMult[s]  :  -1; }
    Int_t getPrimaryParticleCandMult (Int_t s)           const { return ( s >= 0 && s < MAXSECTOR )  ?  aPrimaryParticleCandMult [s]  :  -1; }
    Int_t getMdcClusMult             (Int_t s, Int_t io) const { return ( s >= 0 && s < MAXSECTOR  &&  io >= 0 &&  io < MAXIOSEG ) ? aMdcClusMult       [s][io]  :  -1; }
    Int_t getMdcSegFittedMult        (Int_t s, Int_t io) const { return ( s >= 0 && s < MAXSECTOR  &&  io >= 0 &&  io < MAXIOSEG ) ? aMdcSegFittedMult  [s][io]  :  -1; }
    Int_t getMdcSegUnfittedMult      (Int_t s, Int_t io) const { return ( s >= 0 && s < MAXSECTOR  &&  io >= 0 &&  io < MAXIOSEG)  ? aMdcSegUnfittedMult[s][io]  :  -1; }

    Int_t getSumRichMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRichMult[s];
	return sum;
    }
    Int_t getSumRichCorrMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRichCorrMult[s];
	return sum;
    }
    Int_t getSumRichClusMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRichClusMult[s];
	return sum;
    }
    Int_t getSumTofMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aTofMult[s];
	return sum;
    }
    Int_t getSumRpcMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRpcMult[s];
	return sum;
    }
    Int_t getSumRpcMultHit() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRpcMultHit[s];
	return sum;
    }
    Int_t getSumTofMultCut() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aTofMultCut[s];
	return sum;
    }
    Int_t getSumRpcMultCut() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRpcMultCut[s];
	return sum;
    }
    Int_t getSumRpcMultHitCut() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aRpcMultHitCut[s];
	return sum;
    }
    Int_t getSumShowerMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aShowerMult[s];
	return sum;
    }
    Int_t getSumEmcMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aShowerMult[s];
	return sum;
    }
    Int_t getSumParticleCandMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aParticleCandMult[s];
	return sum;
    }
    Int_t getSumSelectedParticleCandMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aSelectedParticleCandMult[s];
	return sum;
    }
    Int_t getSumPrimaryParticleCandMult() const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aPrimaryParticleCandMult[s];
	return sum;
    }
    Int_t getSumMdcClusMult(Int_t io) const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aMdcClusMult[s][io];
	return sum;
    }
    Int_t getSumMdcSegFittedMult(Int_t io) const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aMdcSegFittedMult[s][io];
	return sum;
    }
    Int_t getSumMdcSegUnfittedMult(Int_t io) const {
	Int_t sum = 0;
	for (Int_t s = 0; s < MAXSECTOR; ++s ) sum += aMdcSegUnfittedMult[s][io];
	return sum;
    }

    Int_t getMdcWiresLay   (Int_t s,Int_t m,Int_t l) {   return aMdcWires[s][m][l]; }
    Int_t getMdcWiresMod   (Int_t s,Int_t m)         {   Int_t sum = 0;                                                for(Int_t l=0;l<6;l++) sum += aMdcWires[s][m][l];  return sum; }
    Int_t getMdcWiresSec   (Int_t s)                 {   Int_t sum = 0;                         for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWires[s][m][l];  return sum; }
    Int_t getMdcWires      ()                        {   Int_t sum = 0;  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWires[s][m][l];  return sum; }

    Int_t getMdcWiresUnusedLay   (Int_t s,Int_t m,Int_t l) {   return aMdcWiresUnused[s][m][l]; }
    Int_t getMdcWiresUnusedMod   (Int_t s,Int_t m)         {   Int_t sum = 0;                         for(Int_t l=0;l<6;l++)                        sum += aMdcWiresUnused[s][m][l];  return sum; }
    Int_t getMdcWiresUnusedSec   (Int_t s)                 {   Int_t sum = 0;                         for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWiresUnused[s][m][l];  return sum; }
    Int_t getMdcWiresUnused      ()                        {   Int_t sum = 0;  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWiresUnused[s][m][l];  return sum; }

    Int_t getMdcWiresRawLay   (Int_t s,Int_t m,Int_t l) {   return aMdcWiresRaw[s][m][l]; }
    Int_t getMdcWiresRawMod   (Int_t s,Int_t m)         {   Int_t sum = 0;                                                for(Int_t l=0;l<6;l++) sum += aMdcWiresRaw[s][m][l];  return sum; }
    Int_t getMdcWiresRawSec   (Int_t s)                 {   Int_t sum = 0;                         for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWiresRaw[s][m][l];  return sum; }
    Int_t getMdcWiresRaw      ()                        {   Int_t sum = 0;  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) sum += aMdcWiresRaw[s][m][l];  return sum; }



    Int_t    getFWallMult  (void) const { return nFWallMult;   }
    Double_t getFWallVector(void) const { return fFWallVector; }
    Double_t getRPlanePhi  (void) const { return fRPlanePhi;   }
    Float_t  getPhiA       (void) const { return fwallPhiA;  }
    Float_t  getPhiB       (void) const { return fwallPhiB;  }
    Float_t  getPhiAB      (void) const { return fwallPhiAB; }
    Float_t  getNA         (void) const { return fwallNA; }
    Float_t  getNB         (void) const { return fwallNB; }

    //--------------------------------------------------------------
    // handling flags for eventselection (hparticledef.h : eEventSelect)
    UInt_t getEventSelector(void) const { return fEventSelector;}
    void   setEventSelector(UInt_t flag){ fEventSelector=flag;}
    Bool_t isGoodEvent     (UInt_t flag){ return (flag==(fEventSelector&flag));}
    void   setSelectorBit  (UInt_t flag){ fEventSelector=(fEventSelector|flag);}
    void   unsetSelectorBit(UInt_t flag){ fEventSelector&=~flag;}
    //--------------------------------------------------------------

    void clearMembers(void);
    ClassDef(HParticleEvtInfo, 6) // Event characteristic info
};

#endif //__HPARTICEEVTINFO_H__
