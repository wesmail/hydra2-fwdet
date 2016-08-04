#ifndef HMDCTRACKINGEFF_H
#define HMDCTRACKINGEFF_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "TRandom.h"
#include "TIterator.h"

class TNtuple;
class TFile;
class HMatrixCategory;
class HLinearCategory;
class HMdcSegSim;
class HMdcTrkCand;
class HGeantKine;
class TLorentzVector;

class HMdcTrackingEff : public HReconstructor {
protected:
    TString fNameRoot;   //! file name of root output file
    TFile*   out;        //! file pointer
    TNtuple* single;
    TNtuple* pairs;

    TIterator* iterTrkCand;     //!
    TIterator* iterTrkCandIdeal;//!
    TIterator* iterGeantTof;    //!
    TIterator* iterGeantShower; //!


    HMatrixCategory* catmHMdcTrkCand;     //!
    HMatrixCategory* catmHMdcTrkCandIdeal;//!

    HMatrixCategory* catmHMdcSegSim;      //!
    HMatrixCategory* catmHMdcSegIdeal;    //!

    HMatrixCategory* catmHGeantTof;       //!
    HMatrixCategory* catmHGeantShower;    //!
    HLinearCategory* catlHGeantKine;      //!


    //trkCandIdeal
    Int_t     indSegId[2];               //!
    HMdcSegSim* pSegId[2];               //!

    //trkCand
    Int_t     indSeg[2];                 //!
    HMdcSegSim* pSeg[2];                 //!

    //segmentId
    Int_t trkNrId;                       //!





    static const Int_t nEvnt  =1 ;     // number of event   indexes stored in data word
    static const Int_t nKine  =10;     // number of kine    indexes stored in data word
    static const Int_t nSeg   =24;     // number of segment indexes stored in data word
    static const Int_t nChi2  =2;
    static const Int_t npair  =25;
    static const Int_t nsingle=2;

    static const Int_t maxTrks=600;   //!
    static const Int_t nSingle=40;    //! nEvnt + nkine + nSeg + nChi2 + nsingle
    static const Int_t nPairs =98;    //! nEvnt + nkine + nSeg + nChi2 + nkine + nSeg + nChi2 + npair
    static const Int_t sizeInd=10;    //!

    Int_t indexTable[maxTrks*sizeInd]; //! maxtrks tracks : ind kine, ind seg1_id,ind seg2_id, ind seg1,ind seg2,ind used
                                       //                   0         1           2            3        4        5
                                       //                   parent trk, geninfo geninfo1 geninfo2
                                       //                   6           7       8        9

    Float_t dSingle[nSingle];  //!
    Float_t dPairs [nPairs];   //!

    Int_t offsetSeg1Kine;  //! starting index in data word for particle 1 (kine)
    Int_t offsetSeg2Kine;  //! starting index in data word for particle 2 (kine)
    Int_t offsetSeg1    ;  //! starting index in data word for seg1       (seg)
    Int_t offsetSeg2    ;  //! starting index in data word for seg2       (seg)
    Int_t offsetChi2    ;  //! starting index in data word for chi2 relative to seg

    Int_t offsetPair    ;  //! starting index in data word for pair   infos
    Int_t offsetSingle  ;  //! starting index in data word for single infos

    void         initVariables();
    void         resetDataArray()
    {
	for (Int_t i=0;i<nSingle;i++)dSingle[i]=-999;
	for (Int_t i=0;i<nPairs;i++) dPairs [i]=-999;
    }
    void         getSegPointers(Int_t*,HMdcSegSim**,HMdcTrkCand*,HMatrixCategory*,Int_t mode=0);
    HGeantKine*  getKineInfo(Int_t,Int_t&,Int_t&,Float_t&,Float_t&,Float_t&,HLinearCategory*);
    Bool_t       sameTrkSegments(Int_t,HMdcSegSim**,Int_t mode=0);
    HMdcTrkCand* findSameRealCand(Int_t,Int_t*,HMdcSegSim**,TIterator*,HMatrixCategory*,HMatrixCategory*,HLinearCategory*);
    TString      getProcessString(Float_t);
    Bool_t       sameDecay(Int_t,Int_t,Float_t,Float_t,Float_t,Float_t);
    void         resetIndexTable(Int_t*,Int_t);
    Bool_t       fillIndexTable(Int_t*,Int_t&,Int_t,Int_t*,Int_t*,HLinearCategory*);
    void         printIndexTable(Int_t*,Int_t);
    Int_t        findPartner(Int_t*,Int_t,Int_t);
    Bool_t       hasPartner(Int_t*,Int_t,Int_t);
    void         fillSegData(Int_t,Int_t*,Float_t*,Int_t,HMatrixCategory*,HMatrixCategory*);
    void         fillKineData(Int_t&,Int_t,Int_t*,Float_t*,Int_t,HMatrixCategory*,HLinearCategory*);
    Int_t        findGeantMeta(Int_t,Int_t,TIterator*,HMatrixCategory*,TIterator*,HMatrixCategory*);
    void         fillNTuple(Int_t,Int_t*,Int_t,HMatrixCategory*,HMatrixCategory*,HLinearCategory*,TIterator*,HMatrixCategory*,TIterator*,HMatrixCategory*,TNtuple*,TNtuple*);
    Float_t      getLabPhiDeg(Int_t sector, Float_t phi);
    Float_t      getThetaPairDeg(TLorentzVector& v);
    Float_t      getPhiPairDeg(TLorentzVector& v);
    void         kineToSegPhiThetaDeg(HGeantKine* kine,Float_t& theta,Float_t& phi);
    Float_t      pairPhiToLabDeg(Float_t phi);
    Float_t      pairThetaToLabDeg(Float_t theta);
    void         fillVectorFromSeg(TLorentzVector& v, Int_t slot,Int_t type);
    void         fillVectorFromKine(TLorentzVector& v, Int_t slot);
public:
    HMdcTrackingEff(void);
    HMdcTrackingEff(const Text_t* name,const Text_t* title);
    ~HMdcTrackingEff(void);
    void    setOutputRoot (TString="ntuple_tracking_eff.root");
    Bool_t  init(void);
    Bool_t  finalize();
    Int_t   execute();
    ClassDef(HMdcTrackingEff,0) // fills ntuple for efficiency of tracking
};

#endif /* !HMDCTRACKINGEFF_H */
