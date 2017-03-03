#ifndef HVECTORCAND_H
#define HVECTORCAND_H

#include "Rtypes.h"

#include "haddef.h"
#include "fwdetdef.h"

#include "TBits.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"
#include "TObject.h"

class HVectorCand: public TLorentzVector
{
public:
    HVectorCand();
    virtual ~HVectorCand();

    Int_t getNofHits() const { return fNhits; }
    Int_t getNDF() const { return fNDF; }
    Double_t getChi2() const { return fChi2; }
    Double_t getX() const { return refVec.X(); }
    Double_t getY() const { return refVec.Y(); }
    Double_t getZ() const { return refVec.Z(); }
    Double_t getTx() const { return dirVec.X(); }
    Double_t getTy() const { return dirVec.Y(); }
    Float_t getTof() const { return fTof; }
    Float_t getDistance() const { return fDistance; }
    Double_t getHadesZ() const { return getHadesParam(0); }
    Double_t getHadesR() const { return getHadesParam(1); }
    Double_t getHadesTheta() const { return getHadesParam(2); }
    Double_t getHadesPhi() const { return getHadesParam(3); }
    void getHadesParams(Double_t *params) const;

    Int_t getHitIndex(Int_t ihit) const { return fHitInds[ihit]; }
    const TBits &getLR() const { return fLRbits; }
    void getParams(Double_t *pars) const;
    TMatrixDSym getCovarMatr() const;

    void setNDF(Int_t ndf) { fNDF = ndf; }
    void setChi2(Double_t c2) { fChi2 = c2; }
    void setZ(Double_t z) { refVec.SetZ(z); }
    void setTof(Double_t t) { fTof = t; }
    void setDistance(Double_t l) { fDistance = l; }
    void setLRbit(Int_t indx) { fLRbits.SetBitNumber(indx); }

    Int_t addHit(Int_t indx);
    void setCovar(TMatrixDSym matr);
    void setParams(Double_t *pars);

    void calc4vectorProperties(Double_t p, Double_t m);
    void calc4vectorProperties(Double_t m);
    const TVector3 & getReferenceVector() const { return refVec; }

    void print() const;

private:
    Double_t getHadesParam(Int_t ipar) const;

private:
    Int_t fNhits;           // number of hits
    Int_t fNDF;             // number of degrees of freedom
    Int_t fHitInds[FWDET_STRAW_MAX_VPLANES];  // hit indices in planes

    Float_t fTof;           // associated Tof, -1.0 if none
    Float_t fDistance;      // distance of flight

    Double32_t fCovar[10];  // covar. matrix
    Double32_t fChi2;       // chi2

    TVector3 dirVec;        // direction vector (momentum)
    TVector3 refVec;        // reference vector (anchor)

    TBits fLRbits;          // bit pattern for left-right drift direction choice (L "-", R "+")

    ClassDef(HVectorCand, 1);
};

#endif
