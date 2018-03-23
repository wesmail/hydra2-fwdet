#ifndef __HPARTICLECANDSIM_H__
#define __HPARTICLECANDSIM_H__

#include "hparticlecand.h"
#include "hvirtualcandsim.h"


class HParticleCandSim : public HParticleCand, public HVirtualCandSim
{
private:

    // sim info
    Int_t   fGeantTrackRich  [3];     // GEANT tracks in rich
    Int_t   fGeantTrackMeta  [4];     // GEANT tracks in tof or rpc
    Int_t   fGeantTrackShower[4];     // GEANT tracks in Shower | ECAL
    Int_t   fGeantTrackInnerMdc[2];   // GEANT tracks in inner MDC seg
    Int_t   fGeantTrackOuterMdc[2];   // GEANT tracks in outer MDC seg
    Char_t  fGeantTrackInnerMdcN[2];  // GEANT number of wires contributing for tracks in inner MDC seg
    Char_t  fGeantTrackOuterMdcN[2];  // GEANT number of wires contributing for tracks in outer MDC seg
public:
    HParticleCandSim()
    {
	for(Int_t i=0;i<4;i++){
	    fGeantTrackMeta[i]=-1;
	    fGeantTrackShower[i]=-1;
	}
	for(Int_t i=0;i<3;i++){
	    fGeantTrackRich[i]=-1;
	}
	for(Int_t i=0;i<2;i++){
	    fGeantTrackInnerMdc[i]=-1;
	    fGeantTrackOuterMdc[i]=-1;
	    fGeantTrackInnerMdcN[i]=0;
	    fGeantTrackOuterMdcN[i]=0;
	}

    }
	~HParticleCandSim() {}

	void     setGeantTrackRich(Int_t a,Int_t i)   { if(i<3 && i>=0) fGeantTrackRich  [i] = a; }
	void     setGeantTrackMeta(Int_t a,Int_t i)   { if(i<4 && i>=0) fGeantTrackMeta  [i] = a; }
	void     setGeantTrackShower(Int_t a,Int_t i) { if(i<4 && i>=0) fGeantTrackShower[i] = a; }
	void     setGeantTrackEmc   (Int_t a,Int_t i) { if(i<4 && i>=0) fGeantTrackShower[i] = a; }  // shared with shower
	void     setGeantTrackInnerMdc(Int_t a,Int_t i,Int_t n) { if(i<2 && i>=0) { fGeantTrackInnerMdc[i] = a; fGeantTrackInnerMdcN[i] = n; }}
	void     setGeantTrackOuterMdc(Int_t a,Int_t i,Int_t n) { if(i<2 && i>=0) { fGeantTrackOuterMdc[i] = a; fGeantTrackOuterMdcN[i] = n; }}

	Int_t    getGeantTrackRich(Int_t i)           { return (i<3 && i>=0)?  fGeantTrackRich  [i] :-1; }
	Int_t    getGeantTrackMeta(Int_t i)           { return (i<4 && i>=0)?  fGeantTrackMeta  [i] :-1; }
	Int_t    getGeantTrackShower(Int_t i)         { return (i<4 && i>=0)?  fGeantTrackShower[i] :-1; }
	Int_t    getGeantTrackEmc   (Int_t i)         { return (i<4 && i>=0)?  fGeantTrackShower[i] :-1; }  // shared with shower
	Int_t    getGeantTrackInnerMdc(Int_t i)       { return (i<2 && i>=0)?  fGeantTrackInnerMdc[i] :-1; }
        Int_t    getGeantTrackOuterMdc(Int_t i)       { return (i<2 && i>=0)?  fGeantTrackOuterMdc[i] :-1; }
	Int_t    getGeantTrackInnerMdcN(Int_t i)      { return (i<2 && i>=0)?  fGeantTrackInnerMdcN[i] : 0; }
        Int_t    getGeantTrackOuterMdcN(Int_t i)      { return (i<2 && i>=0)?  fGeantTrackOuterMdcN[i] : 0; }
	Float_t  getGeantTrackInnerMdcWeight(Int_t i) { return (i<2 && i>=0 && getNWSeg(0) !=0 )?  Float_t(fGeantTrackInnerMdcN[i])/getNWSeg(0) : 0; }
        Float_t  getGeantTrackOuterMdcWeight(Int_t i) { return (i<2 && i>=0 && getNWSeg(0) !=0 )?  Float_t(fGeantTrackOuterMdcN[i])/getNWSeg(1) : 0; }

        Bool_t   isGhostTrack()                const  { return (getGeantCorrTrackIds()&kIsGhost) == 0 ? kFALSE:kTRUE; }
        Bool_t   isInDetector (eDetBits det)        const  { return (getGeantCorrTrackIds()&det) == 0 ? kFALSE:kTRUE; }
	Bool_t   isInDetectors(UInt_t flagdetbits)  const  { return (getGeantCorrTrackIds()&flagdetbits) == flagdetbits ? kTRUE:kFALSE; }
        Int_t    getNDetector()                     const  {
             Int_t n=0;
             if(getGeantCorrTrackIds()&kIsInRICH)      n+=1;
             if(getGeantCorrTrackIds()&kIsInInnerMDC)  n+=1;
             if(getGeantCorrTrackIds()&kIsInOuterMDC)  n+=1;
             if(getGeantCorrTrackIds()&kIsInTOF)       n+=1;
             if(getGeantCorrTrackIds()&kIsInRPC)       n+=1;
             if(getGeantCorrTrackIds()&kIsInSHOWER||getGeantCorrTrackIds()&kIsInEMC)    n+=1;
             return n;
        }
	Bool_t   isGoodMDCWires (Int_t tr,Int_t   minInner=7  ,Int_t   minOuter=7  ,                                    Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);
        Bool_t   isGoodMDCWeight(Int_t tr,Float_t minInner=0.5,Float_t minOuter=0.5,                                    Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);
        Bool_t   isGoodMDC      (Int_t tr,Float_t minInner=0.5,Float_t minOuter=0.5,Int_t minInnerW=5,Int_t minOuterW=5,Bool_t checkall=kFALSE,Bool_t requireOuter=kTRUE);

        virtual void   print(UInt_t selection=31);

    void OldStreamer(TBuffer &R__b, Int_t version);
	ClassDef(HParticleCandSim,4)  // A simple Sim track of a particle
};


#endif // __HPARTICLECANDSIM_H__
