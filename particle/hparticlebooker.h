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
class HEmcCluster;
class HMdcSeg;
class HRichHit;

class HTofHitSim;
class HTofClusterSim;
class HRpcClusterSim;
class HShowerHitSim;
class HEmcClusterSim;
class HMdcSegSim;
class HRichHitSim;
class HParticleCandSim;

class HCategory;

R__EXTERN HParticleBooker *gParticleBooker;

class HParticleBooker : public TObject
{

private:

    //-------------------------------------------------------
    // indices user in candidates
    map<Int_t, vector<HParticleCand*> > mTofHittoCand;       //!  TOF hit     lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mTofClsttoCand;      //!  TOF cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRpcClsttoCand;      //!  RPC cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mShowertoCand;       //!  SHOWER hit  lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mEmctoCand;          //!  EMC cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mInnerMdctoCand;     //!  inner Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mOuterMdctoCand;     //!  outer Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRichtoCand;         //!  RICH hit    lookup   detector hit ind -> list of candidates using this hit


    //-------------------------------------------------------
    // indices user in candidates
    vector<Int_t> vTofHitInd;       //!
    vector<Int_t> vTofClstInd;      //!
    vector<Int_t> vRpcClstInd;      //!
    vector<Int_t> vShowerInd;       //!
    vector<Int_t> vEmcInd;          //!
    vector<Int_t> vInnerMdcInd;     //!
    vector<Int_t> vOuterMdcInd;     //!
    vector<Int_t> vRichInd;         //!
    //-------------------------------------------------------

    //-------------------------------------------------------
    // objects user in candidates
    vector<HTofHit*>     vTofHit;   //!
    vector<HTofCluster*> vTofClst;  //!
    vector<HRpcCluster*> vRpcClst;  //!
    vector<HShowerHit*>  vShower;   //!
    vector<HEmcCluster*> vEmc;      //!
    vector<HMdcSeg*>     vInnerMdc; //!
    vector<HMdcSeg*>     vOuterMdc; //!
    vector<HRichHit*>    vRich;     //!
    //-------------------------------------------------------

    //-------------------------------------------------------
    // all objects in event
    vector<HTofHit*>     vTofHitAll;   //!
    vector<HTofCluster*> vTofClstAll;  //!
    vector<HRpcCluster*> vRpcClstAll;  //!
    vector<HShowerHit*>  vShowerAll;   //!
    vector<HEmcCluster*> vEmcAll;      //!
    vector<HMdcSeg*>     vInnerMdcAll; //!
    vector<HMdcSeg*>     vOuterMdcAll; //!
    vector<HRichHit*>    vRichAll;     //!
    //-------------------------------------------------------


    //-------------------------------------------------------
    // SIMULATION INFO
    //-------------------------------------------------------


    //-------------------------------------------------------
    map<Int_t, vector<HParticleCandSim*> > mTracktoCand;       //!  Geant Track lookup   geant track (assigned track)  -> list of candidates using this track
    map<Int_t, vector<HParticleCandSim*> > mTracktoAnyCand;    //!  Geant Track lookup   geant track (in any detector) -> list of candidates using this track
    map<Int_t, vector<HTofHitSim*> >       mTracktoTofHit;     //!  Geant Track lookup   geant track -> list of TofHits using this track
    map<Int_t, vector<HTofClusterSim*> >   mTracktoTofCluster; //!  Geant Track lookup   geant track -> list of TofCluster using this track
    map<Int_t, vector<HRpcClusterSim*> >   mTracktoRpcCluster; //!  Geant Track lookup   geant track -> list of RpcCluster using this track
    map<Int_t, vector<HShowerHitSim*> >    mTracktoShowerHit;  //!  Geant Track lookup   geant track -> list of ShowerHit using this track
    map<Int_t, vector<HEmcClusterSim*> >   mTracktoEmcCluster; //!  Geant Track lookup   geant track -> list of EmcCluster using this track
    map<Int_t, vector<HMdcSegSim*> >       mTracktoInnerMdc;   //!  Geant Track lookup   geant track -> list of inner MdcSeg using this track
    map<Int_t, vector<HMdcSegSim*> >       mTracktoOuterMdc;   //!  Geant Track lookup   geant track -> list of outer MdcSeg using this track
    map<Int_t, vector<HRichHitSim*> >      mTracktoRichHit;    //!  Geant Track lookup   geant track -> list of RichHit using this track

    map<Int_t, vector<Int_t> > mTracktoCandInd;       //!  Geant Track lookup   geant track (assigned track)  -> list of candidates indices using this track
    map<Int_t, vector<Int_t> > mTracktoAnyCandInd;    //!  Geant Track lookup   geant track (in any detector) -> list of candidates indicesusing this track
    map<Int_t, vector<Int_t> > mTracktoTofHitInd;     //!  Geant Track lookup   geant track -> list of TofHits indices using this track
    map<Int_t, vector<Int_t> > mTracktoTofClusterInd; //!  Geant Track lookup   geant track -> list of TofCluster indices using this track
    map<Int_t, vector<Int_t> > mTracktoRpcClusterInd; //!  Geant Track lookup   geant track -> list of RpcCluster indices using this track
    map<Int_t, vector<Int_t> > mTracktoShowerHitInd;  //!  Geant Track lookup   geant track -> list of ShowerHit indices using this track
    map<Int_t, vector<Int_t> > mTracktoEmcClusterInd; //!  Geant Track lookup   geant track -> list of EmcCluster indices using this track
    map<Int_t, vector<Int_t> > mTracktoInnerMdcInd;   //!  Geant Track lookup   geant track -> list of inner MdcSeg indices using this track
    map<Int_t, vector<Int_t> > mTracktoOuterMdcInd;   //!  Geant Track lookup   geant track -> list of outer MdcSeg indices using this track
    map<Int_t, vector<Int_t> > mTracktoRichHitInd;    //!  Geant Track lookup   geant track -> list of RichHit using indices this track
    //-------------------------------------------------------

    //-------------------------------------------------------
    // helpers
    HCategory* richhitCat;           //!
    HCategory* tofhitCat;            //!
    HCategory* tofclstCat;           //!
    HCategory* rpcclstCat;           //!
    HCategory* showerhitCat;         //!
    HCategory* emcclusterCat;        //!
    HCategory* mdcsegCat;            //!
    HCategory* candCat;              //!

    //-------------------------------------------------------
   void bookHits(HParticleCand* cand1);

public:

    HParticleBooker();
    ~HParticleBooker();



    void  nextEvent();

    vector<Int_t>& getTofHitIndices()    { return  vTofHitInd; }
    vector<Int_t>& getTofClstIndices()   { return  vTofClstInd; }
    vector<Int_t>& getRpcClstIndices()   { return  vRpcClstInd; }
    vector<Int_t>& getShowerIndices()    { return  vShowerInd; }
    vector<Int_t>& getEmcIndices()       { return  vEmcInd; }
    vector<Int_t>& getInnerMdcIndices()  { return  vInnerMdcInd; }
    vector<Int_t>& getOuterMdcIndices()  { return  vOuterMdcInd; }
    vector<Int_t>& getRichIndices()      { return  vRichInd; }


    map<Int_t, vector<HParticleCand*> >& getTofHitMap()   { return  mTofHittoCand; }
    map<Int_t, vector<HParticleCand*> >& getTofClstMap()  { return  mTofClsttoCand; }
    map<Int_t, vector<HParticleCand*> >& getRpcClstMap()  { return  mRpcClsttoCand; }
    map<Int_t, vector<HParticleCand*> >& getShowerHitMap(){ return  mShowertoCand; }
    map<Int_t, vector<HParticleCand*> >& getEmcClusterMap(){ return  mEmctoCand; }
    map<Int_t, vector<HParticleCand*> >& getInnerMdcMap() { return  mInnerMdctoCand; }
    map<Int_t, vector<HParticleCand*> >& getOuterMdcMap() { return  mOuterMdctoCand; }
    map<Int_t, vector<HParticleCand*> >& getRichMap()     { return  mRichtoCand; }


    Int_t getCandidatesForTofHit    (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForTofCluster(Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForRpcCluster(Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForShower    (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForEmc       (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForInnerMdc  (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForOuterMdc  (Int_t index,vector<HParticleCand*>& cands);
    Int_t getCandidatesForRich      (Int_t index,vector<HParticleCand*>& cands);

    Int_t getCandidatesAnyDetectorForTrack(Int_t track,vector<HParticleCandSim*>& cands);
    Int_t getCandidatesForTrack           (Int_t track,vector<HParticleCandSim*>& cands);
    Int_t getTofHitForTrack               (Int_t track,vector<HTofHitSim*>&       cands);
    Int_t getTofClusterForTrack           (Int_t track,vector<HTofClusterSim*>&   cands);
    Int_t getRpcClusterForTrack           (Int_t track,vector<HRpcClusterSim*>&   cands);
    Int_t getShowerHitForTrack            (Int_t track,vector<HShowerHitSim*>&    cands);
    Int_t getEmcClusterForTrack           (Int_t track,vector<HEmcClusterSim*>&   cands);
    Int_t getInnerMdcSegForTrack          (Int_t track,vector<HMdcSegSim*>&       cands);
    Int_t getOuterMdcSegForTrack          (Int_t track,vector<HMdcSegSim*>&       cands);
    Int_t getRichHitForTrack              (Int_t track,vector<HRichHitSim*>&      cands);

    Int_t getCandidatesIndAnyDetectorForTrack(Int_t track,vector<Int_t>& cands);
    Int_t getCandidatesIndForTrack           (Int_t track,vector<Int_t>& cands);
    Int_t getTofHitIndForTrack               (Int_t track,vector<Int_t>& cands);
    Int_t getTofClusterIndForTrack           (Int_t track,vector<Int_t>& cands);
    Int_t getRpcClusterIndForTrack           (Int_t track,vector<Int_t>& cands);
    Int_t getShowerHitIndForTrack            (Int_t track,vector<Int_t>& cands);
    Int_t getEmcClusterIndForTrack           (Int_t track,vector<Int_t>& cands);
    Int_t getInnerMdcSegIndForTrack          (Int_t track,vector<Int_t>& cands);
    Int_t getOuterMdcSegIndForTrack          (Int_t track,vector<Int_t>& cands);
    Int_t getRichHitIndForTrack              (Int_t track,vector<Int_t>& cands);


    vector<HTofHit*>&     getTofHits     () { return vTofHit;  }
    vector<HTofCluster*>& getTofClusters () { return vTofClst; }
    vector<HRpcCluster*>& getRpcClusters () { return vRpcClst; }
    vector<HShowerHit*>&  getShowerHits  () { return vShower;  }
    vector<HEmcCluster*>& getEmcClusters () { return vEmc;  }
    vector<HMdcSeg*>&     getInnerMdcSegs() { return vInnerMdc;}
    vector<HMdcSeg*>&     getOuterMdcSegs() { return vOuterMdc;}
    vector<HRichHit*>&    getRichHits    () { return vRich;    }

    vector<HTofHit*>&     getAllTofHits     () { return vTofHitAll;  }
    vector<HTofCluster*>& getAllTofClusters () { return vTofClstAll; }
    vector<HRpcCluster*>& getAllRpcClusters () { return vRpcClstAll; }
    vector<HShowerHit*>&  getAllShowerHits  () { return vShowerAll;  }
    vector<HEmcCluster*>& getAllEmcClusters () { return vEmcAll;  }
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
