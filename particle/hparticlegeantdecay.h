#ifndef  __HPARTICLEGEANTDECAY_H__
#define __HPARTICLEGEANTDECAY_H__


#include "hgeomvector.h"
#include "hgeantkine.h"

#include "TObject.h"

#include <vector>

using namespace std;

class HParticleGeantDecay : public TObject {

private:

    HGeantKine* fmother;
    vector <HGeantKine*> fdaughters;
    HGeomVector fdecayVertex;
    Int_t  fgeneration;
    HParticleGeantDecay*  fmotherDecay;


    static  Bool_t compareTrackNr(HGeantKine* kine1,HGeantKine* kine2){
	return kine1->getTrack()     < kine2->getTrack();
    }

public:

    HParticleGeantDecay();
    ~HParticleGeantDecay();

    Int_t                  getMotherID()       { return (fmother) ? fmother->getID()    : -10;}
    HGeantKine*            getMother()         { return fmother;}
    vector<HGeantKine*>&   getDaughters()      { return fdaughters;}
    UInt_t                 getNDaughters()     { return fdaughters.size();}
    HGeantKine*            getDaughter(UInt_t i) { return (i < fdaughters.size()) ? fdaughters[i] : 0;}
    Bool_t                 isDaughterPID(Int_t pid);
    Bool_t                 isDaughterPID(vector<Int_t>& pids);

    HGeomVector&           getDecayVertex()    { return fdecayVertex;}
    Int_t                  getMedium()         { return (fmother) ? fmother->getMedium()    : -1;}
    Int_t                  getMechanism()      { return (fmother) ? fmother->getMechanism() : 0;}
    Int_t                  getGeneration()     { return fgeneration;}
    HParticleGeantDecay*   getMotherDecay()    { return fmotherDecay;}
    Int_t                  getGeneratorInfo()  { return (fmother) ? fmother->getGeneratorInfo() : 0;}
    Int_t                  getGeneratorInfo1() { return (fmother) ? fmother->getGeneratorInfo1(): 0;}
    Int_t                  getGeneratorInfo2() { return (fmother) ? fmother->getGeneratorInfo2(): 0;}

    void                   setGeneration(Int_t gen)                      { fgeneration = gen;}
    void                   setDecayVertex(Float_t x,Float_t y,Float_t z) { fdecayVertex.setXYZ(x,y,z);}
    void                   setDecayVertex(HGeomVector& ver)              { fdecayVertex = ver;}
    void                   setMotherDecay(HParticleGeantDecay* mdecay)   { fmotherDecay = mdecay;}
    void                   setMother  (HGeantKine* mother)               { fmother = mother;}
    void                   addDaughter(HGeantKine* d);
    void                   setDaughters(vector<HGeantKine*>& d)          { fdaughters = d;}
    void                   sortDaughters();
    Float_t                getDistFromVertex(HGeomVector* primVer=0);
    Bool_t                 isExternalSource()  { return (fmother) ? fmother->isExternalSource() : kFALSE; }
    void                   clear();
    Bool_t                 isDaughter(HGeantKine* d);
    Bool_t                 isDaughter(vector<HGeantKine*> d);
    void                   print();

    ClassDef(HParticleGeantDecay,0)

};

#endif

