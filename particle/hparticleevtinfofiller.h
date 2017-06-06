#ifndef __HPARTICLEEVTINFOFILLER_H__
#define __HPARTICLEEVTINFOFILLER_H__

#include "hreconstructor.h"
#include "hparticlerunningmean.h"

#include <cmath>

class TIterator;
class HCategory;
class HLocation;

class HMdcLookupGeom;
class HMdcCalParRaw;
class HParticleCand;
//-----------------------------------------------------------------------------
class HParticleEvtInfoFiller : public HReconstructor
{
private:
    HLocation zeroLoc;

    // iterators to the input categories
    TIterator *pWallHitIter;
    TIterator *pRichHitIter;
    TIterator *pRichClusIter;
    TIterator *pMdcClusIter;
    TIterator *pMdcSegIter;
    TIterator *pTofHitIter;
    TIterator *pRpcClusterIter;
    TIterator *pRpcHitIter;
    TIterator *pEmcClusIter;
    TIterator *pShowerHitIter;
    TIterator *pParticleCandIter;

    // output category
    HCategory *pParticleEvtInfoCat;
    HCategory *pWallEventPlaneCat;

    TString beamtime;
    HMdcLookupGeom*  lookupMdcGeom;
    HMdcCalParRaw*  calparMdc;

    // cuts used during filling
    Float_t ftimeTofCut;           // default 35
    Float_t ftimeRpcCut;           // default 25
    Float_t fprimary_mindist;      // default 10
    Float_t fvertexzCut ;          // default -65 (apr12) -200 /jul14+aug14
    Float_t fMetaPILEUPCut ;       // default 60 ns
    Int_t   fMetaNPILEUPCut ;      // number of meta hits <0 || > fMetaPILEUPCut
    Float_t fMdcPILEUPCut[4] ;     // default t1 > 200 (inner) > 400 (outer)
    Float_t fMdcPILEUPNCut ;       // number of wires in cut region  (7 pion beam 35 apr12)
    Float_t fMeanMult;             // mean mult of all candidates
    Float_t fMeanMIPSMult;         // mean mult of all mips candidates (beta>fMeanMIPSbetaCut)
    Float_t fMeanLEPMult;          // mean mult of all lep candidates (beta:mom inside wedge cut + richqa <2)
    Float_t fSigmaMult;            // sigma mult of all candidates
    Float_t fSigmaMIPSMult;        // sigma mult of all mips candidates (beta>fMeanMIPSbetaCut)
    Float_t fSigmaLEPMult;         // sigma mult of all lep candidates (beta:mom inside wedge cut + richqa <2)
    Int_t   fminEvts;              // min number of events
    Int_t   fmaxEvts;              // min number of events
    Int_t   fminLEPEvts;           // min number of events
    Int_t   fmaxLEPEvts;           // min number of events
    Float_t fMeanMIPSBetaCut;      // MIPS selection
    Float_t fMeanLEPBetaCut;       // lep selection
    Float_t fMeanLEPMomCut;        // lep selection
    Float_t fMeanLEPRichQACut;     // lep selection
    Int_t   ftriggerBit;           // trigger for physics
    Int_t   fMinStartCorrFlag;     // corr flag from hparticlestart2hitf
    Float_t fSecToAll;             // av sect per evt
    Int_t   fMinMult;              // minimum allow mult per evt
    Int_t   fMaxFacToMean;         // cutoff for big values larger mean*fMaxFacToMean


    //------------------------------------------------------------------
    // event pileup rejection
    // no veto ----------------------------
    Float_t fvetoMin ;
    Float_t fvetoMax ;
    // startMetacorrelation ---------------
    Float_t fminStartMeta ;    // [ns] min start time to look for correlation
    Float_t fmaxStartMeta ;   // [ns] min start time to look for correlation
    Int_t     fthreshMeta ;     // min number of meta hist needed
    Float_t   fwindowMeta ;     // [ns] integration window
    Float_t   foffsetMeta ;     // [ns] sfift fastest particle to zero
    // startVetoCorrelation ---------------
    Float_t fminStartVeto ;    // [ns] min start time to look for correlation
    Float_t fmaxStartVeto ;   // [ns] min start time to look for correlation
    Float_t   fwindowVeto ;    // [ns] +- matching window
    //------------------------------------------------------------------

    //------------------------------------------------------------------
    // running mean mult
    HParticleRunningMeanI meanALL;
    HParticleRunningMeanI meanMIPS;
    HParticleRunningMeanI meanLEP;
    HParticleRunningMeanI meanALLSec [6];
    HParticleRunningMeanI meanMIPSSec[6];
    HParticleRunningMeanI meanLEPSec [6];
    //------------------------------------------------------------------



    Bool_t isLepton(HParticleCand* cand);
    void clearMembers(void);

public:
    HParticleEvtInfoFiller(const Text_t* name="EvtInfoFiller",const Text_t* title="EvtInfoFiller",TString beamtime="apr12");
    ~HParticleEvtInfoFiller();

    void setTofCut       (Float_t rpccutoff,Float_t tofcutoff) { ftimeTofCut = tofcutoff; ftimeRpcCut = rpccutoff;}
    void setVertexCut    (Float_t z,Float_t mindist)           { fvertexzCut = z ; fprimary_mindist =mindist;}
    void setMetaPileUpCut(Float_t tof,Int_t n)                 { fMetaPILEUPCut = tof ;fMetaNPILEUPCut = n;}
    void setMdcPileupCut (Float_t t0,Float_t t1,Float_t t2,Float_t t3,Int_t n){fMdcPILEUPCut[0]=t0;fMdcPILEUPCut[1]=t1;fMdcPILEUPCut[2]=t2;fMdcPILEUPCut[3]=t3;fMdcPILEUPNCut=n; }
    void setMeanMult     (Float_t mean,Float_t meanmips,Float_t sigmean,Float_t sigmeanmips,Float_t betacut,Int_t minEvt,Int_t maxEvt) { fMeanMult = mean; fMeanMIPSMult = meanmips; fSigmaMult = sigmean; fSigmaMIPSMult = sigmeanmips; fMeanMIPSBetaCut = betacut; fminEvts=minEvt; fmaxEvts=maxEvt; }
    void setMeanLepMult  (Float_t mean,Float_t sigma,Float_t betacut,Float_t momcut,Float_t richcut, Int_t minEvt,Int_t maxEvt) { fMeanLEPMult = mean;fSigmaLEPMult=sigma; fMeanLEPBetaCut = betacut; fMeanLEPMomCut = momcut; fMeanLEPRichQACut =richcut; fminLEPEvts=minEvt; fmaxLEPEvts=maxEvt; }
    void setNoVETOCut    (Float_t min=-15,Float_t max=15) { fvetoMin = min; fvetoMax = max; }
    void setSTARTVETOCUT (Float_t mintstart=15,Float_t maxstart=350,Float_t window=1.) { fminStartVeto = mintstart ; fmaxStartVeto = maxstart; fwindowVeto = window; }
    void setSTARTMETACUT (Float_t mintstart=80,Float_t maxstart=350,Float_t window=5., Float_t offset = 7.) { fminStartMeta = mintstart ; fmaxStartMeta = maxstart; fwindowMeta = window; foffsetMeta = offset; }

    // all Hydra reconstuructor classes must have this
    virtual Bool_t init    (void);
    virtual Bool_t finalize(void);
    virtual Int_t  execute (void);
    virtual Bool_t reinit  (void) { return kTRUE; }


    ClassDef(HParticleEvtInfoFiller, 0)
};

#endif // __HPARTICLEEVTINFOFILLER_H__

