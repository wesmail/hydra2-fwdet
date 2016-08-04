#ifndef __HPARTICLEGEANTEVENT_H__
#define __HPARTICLEGEANTEVENT_H__

#include "hgeomvector.h"
#include "hparticledef.h"

#include "TObject.h"

#include <vector>
#include <map>

using namespace std;
using namespace Particle;

class HParticleGeant;
class HParticleGeantDecay;
class HGeantKine;

class HParticleGeantEvent : public TObject {

private:

    map<HGeantKine*, vector<HGeantKine*> >  mToDaughters;        // kine    -> vector of daughters
    map<Int_t, HParticleGeant* >            mToParticle;         // trackNr -> particle

    vector <HParticleGeantDecay*>           vDecays;             // vector of all decays
    vector<HGeantKine*>                     vexternalParticles;  // vector of all external (PLUTO) particles
    vector<HGeantKine*>                     vexternalKine;       // vector of all arteficial kine mother objects of external particle
    vector<HParticleGeant*>                 vParticles;          // vector of all particles

    HGeomVector   feventVertex;       // [mm]
    Float_t       fBeamEnergy;        // [MeV]
    Float_t       fEventPlane;        // [deg]
    Float_t       fImpactParam;       // [fm]
    Bool_t        bConvertExtThermal; // (default: kTRUE) convert id for external thermal source (sourceID-500)
    Bool_t        bCorrectInfo;       // (default: kTRUE) correct geninfo bug from HGeant2 <= 3.2
    void          clear();
public:

    HParticleGeantEvent();
    ~HParticleGeantEvent();
    void                           setConvertExtThermal(Bool_t convert) { bConvertExtThermal = convert; }
    void                           setCorrectGenInfo   (Bool_t cor)     { bCorrectInfo       = cor; }


    vector<HGeantKine*>&           getExternalParticles() { return vexternalParticles;}
    HGeomVector&                   getEventVertex()       { return feventVertex; }
    vector<HParticleGeantDecay*>&  getDecays()            { return vDecays;}
    vector<HParticleGeant*>&       getParticles()         { return vParticles;}
    Float_t                        getBeamEnergy()        { return fBeamEnergy;}
    Float_t                        getEventPlane()        { return fEventPlane;}
    Float_t                        getImpactParam()       { return fImpactParam;}


    void                           nextEvent();

    HParticleGeantDecay*           isMotherOfDecay  (Int_t track);
    HParticleGeantDecay*           isMotherOfDecay  (HGeantKine* moth);
    HParticleGeantDecay*           isDaughterOfDecay(HGeantKine* daughter);
    HParticleGeantDecay*           isDaughterOfDecay(Int_t track);
    Int_t                          isDecay   (vector<HParticleGeantDecay*>& decays,Int_t motherid = -2, Int_t generation = -2,Int_t med = -2,Float_t dist=0);
    Int_t                          isParticle(vector<HGeantKine*>&     particles,Int_t id  = -2 ,Int_t motherid = -2, Int_t generation = -2, Int_t med = -2 ,Float_t dist   = 0);
    Int_t                          isParticle(vector<HParticleGeant*>& particles,Int_t id  = -2 ,Int_t motherid = -2, Int_t generation = -2, Int_t med = -2 ,Float_t dist   = 0);
    HParticleGeant*                getParticle(Int_t track);
    HParticleGeant*                getParticle(HGeantKine*);
    HGeantKine*                    getKine    (Int_t track);
    UInt_t                         getMultiplicity(Int_t id=-10,Int_t generation=0,Int_t charge=0,UInt_t type=1,UInt_t detbits=kIsInInnerMDC|kIsInOuterMDC|kIsInMETA);

    ClassDef(HParticleGeantEvent,0)

};




#endif
