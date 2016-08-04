#ifndef __HPARTICLEBOOKER_H__
#define __HPARTICLEBOOKER_H__


#include "hparticlecand.h"

#include <vector>
#include <map>

using namespace std;

class HParticleBooker;
class HTofHit;
class HTofCluster;
class HRpcCluster;
class HShowerHit;
class HMdcSeg;
class HRichHit;

R__EXTERN HParticleBooker *gParticleBooker;

class HParticleBooker : public TObject
{

private:

    map<Int_t, vector<HParticleCand*> > mTofHittoCand;       //!  TOF hit     lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mTofClsttoCand;      //!  TOF cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRpcClsttoCand;      //!  RPC cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mShowertoCand;       //!  SHOWER hit  lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mInnerMdctoCand;     //!  inner Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mOuterMdctoCand;     //!  outer Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRichtoCand;         //!  RICH hit    lookup   detector hit ind -> list of candidates using this hit


    vector<Int_t> vTofHitInd;
    vector<Int_t> vTofClstInd;
    vector<Int_t> vRpcClstInd;
    vector<Int_t> vShowerInd;
    vector<Int_t> vInnerMdcInd;
    vector<Int_t> vOuterMdcInd;
    vector<Int_t> vRichInd;

    vector<HTofHit*>     vTofHit;
    vector<HTofCluster*> vTofClst;
    vector<HRpcCluster*> vRpcClst;
    vector<HShowerHit*>  vShower;
    vector<HMdcSeg*>     vInnerMdc;
    vector<HMdcSeg*>     vOuterMdc;
    vector<HRichHit*>    vRich;

    vector<HTofHit*>     vTofHitAll;
    vector<HTofCluster*> vTofClstAll;
    vector<HRpcCluster*> vRpcClstAll;
    vector<HShowerHit*>  vShowerAll;
    vector<HMdcSeg*>     vInnerMdcAll;
    vector<HMdcSeg*>     vOuterMdcAll;
    vector<HRichHit*>    vRichAll;

    void bookHits(HParticleCand* cand1);

public:

    HParticleBooker();
    ~HParticleBooker();



    void  nextEvent();

    vector<Int_t>& getTofHitIndices()    { return  vTofHitInd; }
    vector<Int_t>& getTofClstIndices()   { return  vTofClstInd; }
    vector<Int_t>& getRpcClstIndices()   { return  vRpcClstInd; }
    vector<Int_t>& getShowerIndices()    { return  vShowerInd; }
    vector<Int_t>& getInnerMdcIndices()  { return  vInnerMdcInd; }
    vector<Int_t>& getOuterMdcIndices()  { return  vOuterMdcInd; }
    vector<Int_t>& getRichIndices()      { return  vRichInd; }


    map<Int_t, vector<HParticleCand*> >& getTofHitMap()   { return  mTofHittoCand; }
    map<Int_t, vector<HParticleCand*> >& getTofClstMap()  { return  mTofClsttoCand; }
    map<Int_t, vector<HParticleCand*> >& getRpcClstMap()  { return  mRpcClsttoCand; }
    map<Int_t, vector<HParticleCand*> >& getShowerHitMap(){ return  mShowertoCand; }
    map<Int_t, vector<HParticleCand*> >& getInnerMdcMap() { return  mInnerMdctoCand; }
    map<Int_t, vector<HParticleCand*> >& getOuterMdcMap() { return  mOuterMdctoCand; }
    map<Int_t, vector<HParticleCand*> >& getRichMap()     { return  mRichtoCand; }


    Int_t getCandidatesForTofHit    (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForTofCluster(Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForRpcCluster(Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForShower    (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForInnerMdc  (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForOuterMdc  (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForRich      (Int_t index,vector<HParticleCand*>& cands);


    vector<HTofHit*>&     getTofHits     () { return vTofHit;  }
    vector<HTofCluster*>& getTofClusters () { return vTofClst; }
    vector<HRpcCluster*>& getRpcClusters () { return vRpcClst; }
    vector<HShowerHit*>&  getShowerHits  () { return vShower;  }
    vector<HMdcSeg*>&     getInnerMdcSegs() { return vInnerMdc;}
    vector<HMdcSeg*>&     getOuterMdcSegs() { return vOuterMdc;}
    vector<HRichHit*>&    getRichHits    () { return vRich;    }

    vector<HTofHit*>&     getAllTofHits     () { return vTofHitAll;  }
    vector<HTofCluster*>& getAllTofClusters () { return vTofClstAll; }
    vector<HRpcCluster*>& getAllRpcClusters () { return vRpcClstAll; }
    vector<HShowerHit*>&  getAllShowerHits  () { return vShowerAll;  }
    vector<HMdcSeg*>&     getAllInnerMdcSegs() { return vInnerMdcAll;}
    vector<HMdcSeg*>&     getAllOuterMdcSegs() { return vOuterMdcAll;}
    vector<HRichHit*>&    getAllRichHits    () { return vRichAll;    }

    Int_t getSameRich    (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference=kTRUE);
    Int_t getSameInnerMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference=kTRUE);
    Int_t getSameOuterMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference=kTRUE);
    Int_t getSameMeta    (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference=kTRUE);
    Int_t getSameAny     (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,UInt_t detswitch=0,Bool_t isReference=kTRUE);

    ClassDef(HParticleBooker,0)

};


#endif
