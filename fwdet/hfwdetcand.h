#ifndef HFWDETCAND_H
#define HFWDETCAND_H

#include "Rtypes.h"

#include "haddef.h"
#include "fwdetdef.h"

#include "hvirtualcand.h"

#include "TBits.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"
#include "TMath.h"
#include "TObject.h"

class HFwDetCand: public HVirtualCand
{
public:
    HFwDetCand();
    virtual ~HFwDetCand();

    Int_t    getNofHits() const { return fNhits; }
    Int_t    getNDF()     const { return fNDF; }
    Double_t getPointX()  const { return refVec.X(); }
    Double_t getPointY()  const { return refVec.Y(); }
    Double_t getPointZ()  const { return refVec.Z(); }
    Double_t getDirTx()   const { return dirVec.X(); }
    Double_t getDirTy()   const { return dirVec.Y(); }

    Float_t  getDistance()     const { return fDistance; }

    Int_t getHitIndex(Int_t ihit) const { return fHitInds[ihit]; }
    const TBits &getLR() const { return fLRbits; }
    void getParams(Double_t *pars) const;
    TMatrixDSym getCovarMatr() const;

    void  setNDF     (Int_t ndf)  { fNDF = ndf; }
    void  setPointZ  (Double_t z) { refVec.SetZ(z); }
    void  setDistance(Double_t l) { fDistance = l; }
    void  setLRbit   (Int_t indx) { fLRbits.SetBitNumber(indx); }

    Int_t addHit(Int_t indx);
    void  setCovar(TMatrixDSym matr);
    void  setParams(Double_t *pars);

    void  calc4vectorProperties(Double_t p, Double_t m);
    void  calc4vectorProperties(Double_t m);
    const TVector3 & getReferenceVector() const { return refVec; }

    void  print() const;

    void  setHadesParams();

private:
    Int_t fNhits;           // number of hits
    Int_t fNDF;             // number of degrees of freedom
    Int_t fHitInds[FWDET_STRAW_MAX_VPLANES];  //[fNhits] hit indices in planes

    Float_t fDistance;      // distance of flight

    Double32_t fCovar[10];  // covar. matrix

    TVector3 dirVec;        // direction vector (momentum)
    TVector3 refVec;        // reference vector (anchor)

    TBits fLRbits;          // bit pattern for left-right drift direction choice (L "-", R "+")

    ClassDef(HFwDetCand, 2);
};

#endif
