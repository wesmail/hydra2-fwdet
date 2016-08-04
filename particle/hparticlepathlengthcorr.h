#ifndef HPARTICLEPATHLENGTHCORR_H
#define HPARTICLEPATHLENGTHCORR_H

#include "hmdcgeomobj.h"
#include "hreconstructor.h"

#include "TString.h"


class HParticleCand;
class HCategory;
class HGeomVector;

class HParticlePathLengthCorr : public HReconstructor {
private:
    HCategory*   candCat;
    HGeomVector  pVertex;
    Int_t sector;
    Double_t A[6];
    Double_t B[6];
    Double_t D[6];
    Double_t an[6];
    HMdcPlane hPlane[6];
    Float_t zVm;
    Float_t xVm;
    Float_t yVm;
    Float_t start;
    Float_t beamVelocity;

public:

    HParticlePathLengthCorr(const Text_t* name="ParticlePathLengthCorr",const Text_t* title="ParticlePathLengthCorr");
    ~HParticlePathLengthCorr();

    Bool_t  init(void);
    Int_t   execute(void);
    Bool_t  finalize(){return kTRUE;}
    Bool_t  setBeamTime(TString beamtime);
    Float_t getStart() { return start;}

    ClassDef(HParticlePathLengthCorr,0)
};



#endif
