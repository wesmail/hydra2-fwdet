/**  HVectorCand.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

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

    inline Int_t getFlag() const { return fFlag; }
    inline Int_t getNofHits() const { return fNhits; }
    inline Int_t getNDF() const { return fNDF; }
    inline Double_t getChi2() const { return fChi2; }
    inline Double_t getX() const { return refVec.X(); }
    inline Double_t getY() const { return refVec.Y(); }
    inline Double_t getTx() const { return dirVec.X(); }
    inline Double_t getTy() const { return dirVec.Y(); }
    inline Double_t getZ() const { return refVec.Z(); }
    inline Double_t getZH() const { return getHadesParam(0); }
    inline Double_t getRH() const { return getHadesParam(1); }
    inline Double_t getThetaH() const { return getHadesParam(2); }
    inline Double_t getPhiH() const { return getHadesParam(3); }
    void getHadesParams(Double_t *params) const;

    inline Int_t getHitIndex(Int_t ihit) const { return fHitInds[ihit]; }
    inline const TBits &getLR() const { return fLRbits; }
    void getParams(Double_t *pars) const;
    TMatrixDSym getCovarMatr() const;

    inline void setFlag(Int_t flag) { fFlag = flag; }
    inline void setNDF(Int_t ndf) { fNDF = ndf; }
    inline void setChi2(Double_t c2) { fChi2 = c2; }
    inline void setZ(Double_t z) { refVec.SetZ(z); }
    inline void setLRbit(Int_t indx) { fLRbits.SetBitNumber(indx); }

    Int_t addHit(Int_t indx);
    void setCovar(TMatrixDSym matr);
    void setParams(Double_t *pars);

    void calc4vectorProperties(Double_t p, Double_t m);
    inline const TVector3 & getReferenceVector() const { return refVec; }

    void print() const;
private:
    Double_t getHadesParam(Int_t ipar) const;

private:
    Int_t fFlag;            // flag TODO what flag?
    Int_t fNhits;           // number of hits
    Int_t fNDF;             // number of degrees of freedom
    Int_t fHitInds[FWDET_STRAW_MAX_VPLANES];  // hit indices in planes

    Double32_t fCovar[10];  // covar. matrix
    Double32_t fChi2;       // chi2

    TVector3 dirVec;        // direction vector (momentum)
    TVector3 refVec;        // reference vector (anchor)

    TBits fLRbits;            // bit pattern for left-right drift direction choice (false "-", true "+")

    ClassDef(HVectorCand, 1);
};

#endif
