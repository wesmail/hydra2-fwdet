#ifndef __HPARTICLEPAIR_H__
#define __HPARTICLEPAIR_H__

#include "hparticlecand.h"
#include "hparticlecandsim.h"
#include "hparticletool.h"
#include "hgeomvector.h"
#include "hgeantkine.h"
#include "TLorentzVector.h"




class HParticlePair : public TLorentzVector
{
protected:


    HParticleCand* fcand[2];    //! pointer to cand1 (reference: should be a lepton candidate) and cand2
    HParticlePair* fpair[2];    //! pointer to cand1 and cand2  from pairs
    TLorentzVector    fc[2];    // local copy of lorentz vector of cand1 and cand2

    UInt_t fpairFlags;          // pair classifier
    UInt_t fstatusFlags;        //  bit  0x01   true cand1
                                //       0x02   true cand2

    Int_t fPID[2];              // assigned PID for cand1 and cand2
    Int_t fMotherPID;           // assigned PID for Mother
    Float_t foAngle;            // opening angle of pair [deg]
    Bool_t  fIsSimulation ;     // kTRUE if HParticleCandSim is used


    HGeomVector  fEventVertex;  // used event vertex
    HGeomVector  fDecayVertex;  // secondary vertex of both candidates
    Float_t fVerMinDistCand[2]; // vertex cut vars : mindist cand1 and cand2 to event vertex
    Float_t fVerMinDistMother;  // vertex cut vars : mindist mother to event vertex
    Float_t fVerDistMother;     // vertex cut vars
    Float_t fMinDistCandidates; // vertex cut vars : mindist between cand1 and cand2

    static Bool_t fDoMomCorrection; //!


    Bool_t          calcVertex();
    Bool_t          isSimulation();
    void            setTruePair();
public:
    HParticlePair();
    ~HParticlePair();


    HParticleCand*     getCand    (Int_t ind)       { return fcand[ind];  }
    HParticlePair*     getPair    (Int_t ind)       { return fpair[ind];  }
    HParticleCandSim*  getCandSim (Int_t ind)       { return (HParticleCandSim*) fcand[ind];  }
    TLorentzVector&    getCandVect(Int_t ind)       { return fc[ind];}
    Int_t              index(Int_t pid)             { return fPID[0] == pid ? 0 : 1; }
    Float_t            getOpeningAngle()            { return foAngle; }
    Float_t            getPhi()                     { return (Phi() < 0 ? Phi()*TMath::RadToDeg()+360 : Phi()*TMath::RadToDeg());}
    Float_t            getTheta()                   { return Theta()*TMath::RadToDeg();}

    Int_t              getCandPID(Int_t ind)        { return fPID[ind];}
    Int_t              getMotherPID()               { return fMotherPID;}
    UInt_t             getPairFlags()               { return fpairFlags ;}
    Int_t              isFakePair();
    Bool_t             isTruePair();
    HParticleCandSim*  getFirstDaughter();
    Bool_t             isGeantDecay();
    Bool_t             isSameExternalSource();
    void               getSourceInfo(Int_t index,Int_t& parentTr,Int_t& grandparenttr,Int_t& geninfo,Int_t& geninfo1,Int_t& geninfo2);
    Bool_t             getIsSimulation()            { return fIsSimulation;}

    Float_t            getVerMinDistCand(Int_t ind) { return fVerMinDistCand[ind];}
    Float_t            getVerMinDistMother()        { return fVerMinDistMother;}
    Float_t            getMinDistCandidates()       { return fMinDistCandidates;}
    Float_t            getVerDistMother()           { return fVerDistMother;}
    HGeomVector&       getEventVertex()             { return fEventVertex; }
    HGeomVector&       getDecayVertex()             { return fDecayVertex; }

    void               printFlags();
    void               print(UInt_t selection=63);
    void               clear();

    UInt_t             getStatusFlags() { return fstatusFlags; }
    Bool_t             checkStatus(UInt_t flag)  { return  (fstatusFlags|flag) == flag ? kTRUE: kFALSE; }

    Bool_t             calcVectors(Int_t pid1,Int_t pid2,Int_t motherpid,HGeomVector& vertex);
    Bool_t             setPair(HParticleCand* cnd1,Int_t pid1,HParticleCand* cnd2,Int_t pid2,Int_t motherpid,UInt_t pairflags,HGeomVector& vertex);
    Bool_t             setPair(HParticlePair* cnd1,Int_t pid1,HParticleCand* cnd2,Int_t pid2,Int_t motherpid,UInt_t pairflags,HGeomVector& vertex);
    Bool_t             setPair(HParticlePair* cnd1,Int_t pid1,HParticlePair* cnd2,Int_t pid2,Int_t motherpid,UInt_t pairflags,HGeomVector& vertex);
    static void        setDoMomentumCorrection(Bool_t doit) { fDoMomCorrection = doit;}
    static Bool_t      getDoMomentumCorrection()            { return fDoMomCorrection ;}
    ClassDef(HParticlePair,1)

};
#endif // __HPARTICLEPAIR_H__

