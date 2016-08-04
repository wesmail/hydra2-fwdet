#ifndef  __HPARTICLEGEANTPAIR_H__
#define  __HPARTICLEGEANTPAIR_H__

#include "hgeomvector.h"
#include "hgeantkine.h"
#include "TLorentzVector.h"

class HParticleGeantPair : public TLorentzVector
{
protected:


    HGeantKine*    fcand[2];    //! pointer to cand1 (reference: should be a lepton candidate) and cand2
    HGeantKine*    fmother;     //! pointer to other particle
    TLorentzVector    fc[2];    // local copy of lorentz vector of cand1 and cand2
    Int_t fPID[2];              // assigned PID for cand1 and cand2
    Int_t fMotherPID;           // assigned PID for Mother
    Float_t foAngle;            // opening angle of pair [deg]
    UInt_t fstatusFlags;        //  bit  0x01  true pair
                                //  bit  0x02  cand1 in acceptance
                                //  bit  0x04  cand2 in acceptance
                                //  bit  0x08  cand1 reconstuctable
                                //  bit  0x10  cand2 reconstuctable

    HGeomVector  fEventVertex;  // used event vertex
    HGeomVector  fDecayVertex;  // secondary vertex of both candidates
    Float_t fVerMinDistCand[2]; // vertex cut vars : mindist cand1 and cand2 to event vertex
    Float_t fVerMinDistMother;  // vertex cut vars : mindist mother to event vertex
    Float_t fVerDistMother;     // vertex cut vars
    Float_t fMinDistCandidates; // vertex cut vars : mindist between cand1 and cand2

    static Bool_t fbCheckAcceptance; //! run acceptance check (default: kTRUE)


    Bool_t          calcVertex();

public:
    HParticleGeantPair();
    ~HParticleGeantPair();

    HGeantKine*     getCand(Int_t ind)           { return fcand[ind];  }
    HGeantKine*     getMother()                  { return fmother;  }
    Float_t         getOpeningAngle()            { return foAngle; }
    Float_t         getPhi()                     { return (Phi() < 0 ? Phi()*TMath::RadToDeg()+360 : Phi()*TMath::RadToDeg());}
    Float_t         getTheta()                   { return Theta()*TMath::RadToDeg();}
    TLorentzVector& getCandVect(Int_t ind)       { return fc[ind];}

    Int_t           getCandPID(Int_t ind)        { return fPID[ind];}
    Int_t           getMotherPID()               { return fMotherPID;}

    Float_t         getVerMinDistCand(Int_t ind) { return fVerMinDistCand[ind];}
    Float_t         getVerMinDistMother()        { return fVerMinDistMother;}
    Float_t         getMinDistCandidates()       { return fMinDistCandidates;}
    Float_t         getVerDistMother()           { return fVerDistMother;}
    HGeomVector&    getEventVertex()             { return fEventVertex; }
    HGeomVector&    getDecayVertex()             { return fDecayVertex; }

    Int_t           isFakePair();
    Bool_t          isTruePair();
    Bool_t          isInAcceptance(Int_t ver=-1);
    void            print(UInt_t selection=31);
    void            clear();
    UInt_t          getStatusFlags() { return fstatusFlags; }
    Bool_t          checkStatus(UInt_t flag)  { return  (fstatusFlags|flag) == flag ? kTRUE: kFALSE; }

    Bool_t          calcVectors(Int_t pid1,Int_t pid2,Int_t motherpid,HGeomVector& vertex);
    Bool_t          setPair(HGeantKine* cnd1,Int_t pid1,HGeantKine* cnd2,Int_t pid2,HGeantKine* mother,Int_t motherpid,HGeomVector& vertex);
    Bool_t          setPair(HGeantKine* cnd1,HGeantKine* cnd2,HGeantKine* mother,Int_t motherpid,HGeomVector& vertex);

    static void     setAcceptanceCheck(Bool_t doit) { fbCheckAcceptance = doit;}

    ClassDef(HParticleGeantPair,1)

};

#endif

