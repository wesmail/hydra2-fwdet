#ifndef  __HPARTICLEGEANT_H__
#define __HPARTICLEGEANT_H__

#include "hgeomvector.h"
#include "hparticledef.h"
#include "hparticlecandsim.h"
#include "hgeantkine.h"

#include "TObject.h"

#include <vector>

using namespace std;
using namespace Particle;

class HParticleGeantDecay;
class HParticleCandSim;

class HParticleGeant : public TObject {

private:

    HGeantKine*               fparticle;         // pointer to kine object
    HGeantKine*               fmother;           // pointer to mother kine object
    HParticleGeantDecay*      fmotherDecay;      // pointer to the mother decay
    vector<HParticleCandSim*> vReco;             // list of reconstructed candidates keeping the same Geant track
    Bool_t                    fInAcceptance;     // = kTRUE if particle is detected in Acceptance
    Int_t                     fgeneration;       // generation of this particle (number of mothers)
    HGeomVector               fdecayVertex;      // vertex of this particle

public:

    HParticleGeant() ;
    ~HParticleGeant();

    void                      setParticle(HGeantKine* part);
    HGeantKine*               getParticle()                                 { return fparticle;}
    HGeantKine*               getMother()                                   { return fmother;}
    void                      setMother  (HGeantKine* moth)                 { fmother = moth; }
    Int_t                     getID()                                       { return (fparticle ) ? fparticle->getID() : -1;}
    HGeomVector&              getDecayVertex()                              { return fdecayVertex;}
    Int_t                     getMedium()                                   { return (fparticle ) ? fparticle->getMedium() : -1;}
    Int_t                     getMechanism()                                { return (fparticle ) ? fparticle->getMechanism() : 0;}
    Int_t                     getGeneration()                               { return fgeneration;}
    HParticleGeantDecay*      getMotherDecay()                              { return fmotherDecay;}
    void                      setGeneration(Int_t gen)                      { fgeneration = gen;}
    void                      setDecayVertex(Float_t x,Float_t y,Float_t z) { fdecayVertex.setXYZ(x,y,z);}
    void                      setDecayVertex(HGeomVector& ver)              { fdecayVertex = ver;}
    void                      setMotherDecay(HParticleGeantDecay* mdecay)   { fmotherDecay = mdecay;}
    Float_t                   getDistFromVertex(HGeomVector* primVer=0);
    vector<HParticleCandSim*> getRecoCand()                                 { return vReco;}
    HParticleCandSim*         getRecoCand(UInt_t i)                         { return (i < vReco.size() ) ? vReco[i] : 0 ; }
    void                      addRecoCand(HParticleCandSim* c);
    UInt_t                    getNRecoCand();
    UInt_t                    getNRecoUsedCand();
    UInt_t                    getNGhosts(Bool_t isUsed=kFALSE);
    UInt_t                    getNTrueReco(Bool_t isUsed=kTRUE,UInt_t detbits=kIsInInnerMDC|kIsInOuterMDC|kIsInMETA);
    Bool_t                    isInAcceptance()                              {return fInAcceptance;}
    void                      print();
    void                      clear();

    ClassDef(HParticleGeant,0)

};

#endif

