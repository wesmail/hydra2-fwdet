#ifndef __HPARTICLEPAIRMAKER_H__
#define __HPARTICLEPAIRMAKER_H__


#include "hparticledef.h"
#include "hparticlepair.h"
#include "hparticlecand.h"
#include "hparticletool.h"
#include "hphysicsconstants.h"
#include "heventheader.h"


#include "TObject.h"
#include "TLorentzVector.h"


#include <vector>
#include <map>
#include <iostream>
#include <iomanip>

using namespace std;

class HParticlePairMaker : public TObject
{
private:

    vector<HParticleCand*> freference;                    //! reference candidates (kIsLepton flagged)
    vector<HParticleCand*> fothers;                       //! other candidates (not KIsLepton flagged)
    vector<HParticleCand*> ffullrecoOthers;               //! full reco cands (inner/outer MDC + META) inside others
    vector<HParticleCand*> fnofullrecoOthers;             //! not full reco cands (inner/outer MDC or META missing) inside others
    vector<HParticlePair>  fpairs;                        //! all pair combinations freference x fothers


    map<Int_t, vector<HParticleCand*> > mTofHittoCand;       //!  TOF hit     lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mTofClsttoCand;      //!  TOF cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRpcClsttoCand;      //!  RPC cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mShowertoCand;       //!  SHOWER hit  lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mEmctoCand;          //!  EMC Cluster lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mInnerMdctoCand;     //!  inner Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mOuterMdctoCand;     //!  outer Seg   lookup   detector hit ind -> list of candidates using this hit
    map<Int_t, vector<HParticleCand*> > mRichtoCand;         //!  RICH hit    lookup   detector hit ind -> list of candidates using this hit
    map<HParticleCand*,vector<HParticlePair*> > mCandtoPair; //!  candidate   lookup   candidate        -> list of pairs using this candidate



    Bool_t (*fselectPID1)(HParticleCand*);                 //! selection function pid1 (default positrons)
    Bool_t (*fselectPID2)(HParticleCand*);                 //! selection function pid2 (default electrons)
    Bool_t (*fuserFilter)(HParticleCand*);                 //! user filter function to avoid unneeded combinatorics

    Int_t fPID1;                                           //!  pid1 (default positrons)
    Int_t fPID2;                                           //!  pid2 (default electrons)
    Int_t fMotherPID;                                      //!  default dilepton

    Bool_t fuse_kIsLepton;                                 //! == kTRUE use kIsLepton as refererence selection (default)
    Bool_t fdoSkippedFullCandPairs;                        //! == kTRUE build also pairs of skipped full reco cands (inner/outer MDC+META) with others
    static Bool_t frequireRich;                            //! ask for rich index in selctPos/selectNeg function

    Int_t         fVertexCase;                            //! which eventvertex to use (see eVertex in hparticledef.h)
    HGeomVector   fVertex;                                // vertex for  current event

    vector<UInt_t>  fCaseCt;                              //! counter array for cases
    vector<UInt_t>  fCaseVec;                             //! vector for pair cases
    Int_t           richCandCt;                           //! counter for all pair cases with both candidates matching a Rich (check)

    void clearVectors();
    void bookHits(HParticleCand* cand1);
    void selectPID(HParticleCand* cand1,Int_t& pid1,Bool_t warn=kTRUE);
public:

    HParticlePairMaker();
    ~HParticlePairMaker();

    //--------------------------------------------------------------------------
    // setters
    void setPIDs(Int_t pid1,Int_t pid2,Int_t motherpid) {
	fPID1      = pid1;
	fPID2      = pid2;
	fMotherPID = motherpid;
    }

    void setPIDsSelection(Bool_t (*selPID1)(HParticleCand*), Bool_t (*selPID2)(HParticleCand*)) {
	fselectPID1=selPID1;
        fselectPID2=selPID2;
    }
    void setUserFilter(Bool_t (*userfilter)(HParticleCand*)) {
	fuserFilter=userfilter;
    }
    void          setDoSkippedFullCandPairs(Bool_t doit) { fdoSkippedFullCandPairs = doit; }
    void          setUseLeptons (Bool_t use) { fuse_kIsLepton = use;}
    static void   setRequireRich(Bool_t use) { frequireRich = use;}
    static Bool_t getRequireRich()           { return frequireRich ;}
    void          setVertexCase(Particle::eVertex vertexCase) { fVertexCase =  vertexCase; };
    void          setVertex(HGeomVector& v)  { fVertex = v; fVertexCase = kVertexUser;}
    void          setVertex(HVertex& v)      { fVertex = v.getPos(); fVertexCase = kVertexUser;}

    static Bool_t selectPos(HParticleCand*);
    static Bool_t selectNeg(HParticleCand*);

    //--------------------------------------------------------------------------
    //  event action
    void nextEvent();

    vector<HParticleCand*>& getReferenceVector() { return freference; }
    vector<HParticleCand*>& getOthersVector   () { return fothers;    }
    vector<HParticlePair>&  getPairsVector    () { return fpairs;     }

    //--------------------------------------------------------------------------
    // filter functions
    void  filterPairsVector(vector<HParticlePair*>& filterpairs,UInt_t flag=0);
    void  filterPairsVector(vector<HParticlePair*>& filterpairs,vector<UInt_t>& flags);
    Int_t filterCandidates (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Float_t oAngle=-1);
    Int_t filterCandidates (HParticleCand* cand,vector<HParticlePair*>& filterpairs,UInt_t flag=0,Float_t oAngle=-1);

    //--------------------------------------------------------------------------
    // lookup functions
    Int_t getSameRich    (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference = kTRUE);
    Int_t getSameInnerMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference = kTRUE);
    Int_t getSameOuterMdc(HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference = kTRUE);
    Int_t getSameMeta    (HParticleCand* cand,vector<HParticleCand*>& candidates,UInt_t flag=0,Bool_t isReference = kTRUE);

    void plotPairCaseStat();
    ClassDef(HParticlePairMaker,0)
};


#endif // __HPARTICLEPAIRMAKER_H__
