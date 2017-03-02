/**  HFwDetStrawVector.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

#ifndef HFWDETSTRAWVECTOR_H
#define HFWDETSTRAWVECTOR_H

#include "TObject.h"
#include "TBits.h"
#include "TMatrixDSym.h"

class HFwDetStrawVector: public TObject
{
public:
    HFwDetStrawVector();
    virtual ~HFwDetStrawVector();

    Int_t getFlag() const { return fFlag; }
    Int_t getNofHits() const { return fNhits; }
    //Int_t getNDF() const { return (fNhits > 5) ? fNhits - 4 : 1; }
    Int_t getNDF() const { return fNDF; }
    Double_t getChi2() const { return fChi2; }
    Double_t getX() const { return fParams[0]; }
    Double_t getY() const { return fParams[1]; }
    Double_t getTx() const { return fParams[2]; }
    Double_t getTy() const { return fParams[3]; }
    Double_t getZ() const { return fZ; }
    Double_t getZH() { return hadesParam(0); }
    Double_t getRH() { return hadesParam(1); }
    Double_t getThetaH() { return hadesParam(2); }
    Double_t getPhiH() { return hadesParam(3); }
    void hadesParams(Double_t *params);

    Int_t getHitIndex(Int_t ihit) const { return fHitInds[ihit]; }
    TBits &getLR() { return fLRbits; }
    void params(Double_t *pars) const { for (Int_t i = 0; i < 4; ++i) pars[i] = fParams[i]; }
    TMatrixDSym getCovarMatr();

    void setFlag(Int_t flag) { fFlag = flag; }
    void setNDF(Int_t ndf) { fNDF = ndf; }
    void setChi2(Double_t c2) { fChi2 = c2; }
    void setZ(Double_t z) { fZ = z; }
    void setLRbit(Int_t indx) { fLRbits.SetBitNumber(indx); }

    Int_t addHit(Int_t indx);
    void setCovar(TMatrixDSym matr);
    void setParams(Double_t *pars) { for (Int_t i = 0; i < 4; ++i) fParams[i] = pars[i]; }

private:
    Double_t hadesParam(Int_t ipar);

private:
    Int_t fFlag;              // flag TODO what flag?
    Int_t fNhits;             // number of hits
    Int_t fNDF;               // number of degrees of freedom
    Int_t fHitInds[16];       // hit indices in planes
    Double32_t fParams[4];    // track parameters (x, y, Tx, Ty)
    Double32_t fZ;            // z
    Double32_t fCovar[10];    // covar. matrix
    Double32_t fChi2;         // chi2
    TBits fLRbits;            // bit pattern for left-right drift direction choice (false "-", true "+")

    ClassDef(HFwDetStrawVector, 1);
};

#endif
