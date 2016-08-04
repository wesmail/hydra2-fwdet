#ifndef HPARTICLEANGLECOR_H
#define HPARTICLEANGLECOR_H

#include "hades.h"
#include "hevent.h"
#include "hparticlecand.h"
#include "heventheader.h"
#include "hgeomvector.h"
#include "hparticledef.h"

#include "TObject.h"
#include "TMath.h"
#include "TString.h"


#include <queue>

using namespace std;

class HParticleAngleCor : public TObject {
protected:
    Float_t meanZTarget;   // typical shift of target centroid in lab coordinates (in mm)
    Float_t zRichCenter;   // typical shift of rich as compared to design value. Determined via meanZTarget (in mm)
    Float_t zMirror;       // center of Rich mirror in lab coordinates. Depends on zRichCenter and RICH geometry.
    Float_t rMirror;       // effective radius for dominant multiple scattering source (in between mirror and shell radius).
    Float_t drShield;      // additional distance from target center to effective multiple scattering point due to delta electron shield
    Float_t maxRhoShield;  // end of delta electron shield (perpendicular to beam axis projected onto mirror/shell sphere)
    Bool_t  useMeanXY;
    Bool_t  useShield;     // delta electron shield provides significant contribution to multiple scattering
    Int_t   vertexType;    // switch cluster,segment,particle global vertex (see Particle:VertexXXX)
    Bool_t  bDoWarn;
    Float_t sumXVertex;
    Float_t sumYVertex;
    UInt_t  maxAverage;

    queue<Float_t> qxVertex;
    queue<Float_t> qyVertex;

public:
    HParticleAngleCor(void);
    ~HParticleAngleCor(void) {};
    void        initParam(void);
    void        resetMeanVertex();
    Bool_t      recalcEmission(const Double_t z, const Double_t rho, const Double_t theta, const Double_t phi,
			       Double_t & zCor, Double_t & rhoCor, Double_t & thetaCor, Double_t & phiCor);
    Bool_t      recalcEmission(const HParticleCand * pCand,
			  Double_t & zCor, Double_t & rhoCor, Double_t & thetaCor,Double_t & phiCor);
    Bool_t      recalcAngles     (const HParticleCand * pCand, Double_t & thetaCor, Double_t & phiCor);
    Bool_t      recalcSetAngles  (HParticleCand* pCand);
    Bool_t      recalcSetEmission(HParticleCand* pCand);
    static Bool_t alignRichRing(const Double_t theta, const Double_t phi, Double_t & thetaCor, Double_t & phiCor);
    static Bool_t alignRichRing(HParticleCand* pCand);
    static Bool_t realignRichRing(const HParticleCand* pCand, Double_t & thetaCor, Double_t & phiCor);
    static Bool_t realignRichRing(HParticleCand* pCand);
    static Float_t matchRingTrack(HParticleCand* cand);
    void        calcSegVector(const Double_t z, const Double_t rho, const Double_t phi, const Double_t theta,
			      HGeomVector &base, HGeomVector &dir);
    void        setMeanZTarget(const Float_t zValue);
    void        setShield(const Bool_t shieldFlag);
    Bool_t      setDefaults(const TString beamtime);
    void        setMaxAverage(UInt_t n) { if (n > 1 ) maxAverage = n; }
    void        setUseMeanXYVertex(void);
    void        setUseEventXYVertex(void);
    void        setDoWarning(Bool_t warn) { bDoWarn = warn; }
    Bool_t      getMeanXYVertex(Float_t & xValue, Float_t & yValue);
    Float_t     getMeanZTarget(void) {return meanZTarget;}; // center of segmented target
    void        setVertexType(Int_t v=Particle::kVertexSegment) { vertexType = v ;}
    Int_t       getVertexType(){ return vertexType ;}
    HGeomVector getGlobalVertex(Int_t v=Particle::kVertexParticle,Bool_t warn=kFALSE);
    ClassDef(HParticleAngleCor,0) //Correct emission angles by assuming a multiple scattering kick at the Rich mirror/housing
};
#endif //HPARTICLEANGLECOR_H
