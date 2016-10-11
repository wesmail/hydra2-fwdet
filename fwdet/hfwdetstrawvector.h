/**  HFwDetStrawVector.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

#ifndef HFWDETSTRAWVECTOR_H
#define HFWDETSTRAWVECTOR_H

#include <TObject.h>
#include <TBits.h>
#include <TMatrixDSym.h>

class HFwDetStrawVector: public TObject
{
public:
    /** Default constructor **/
    HFwDetStrawVector();

    /** Destructor **/
    virtual ~HFwDetStrawVector();

    Int_t GetFlag() const { return fFlag; }
    Int_t GetNofHits() const { return fNhits; }
    //Int_t GetNDF() const { return (fNhits > 5) ? fNhits - 4 : 1; }
    Int_t GetNDF() const { return fNDF; }
    Double_t GetChi2() const { return fChi2; }
    Double_t GetX() const { return fParams[0]; }
    Double_t GetY() const { return fParams[1]; }
    Double_t GetTx() const { return fParams[2]; }
    Double_t GetTy() const { return fParams[3]; }
    Double_t GetZ() const { return fZ; }
    Double_t GetZH() { return HadesParam(0); }
    Double_t GetRH() { return HadesParam(1); }
    Double_t GetThetaH() { return HadesParam(2); }
    Double_t GetPhiH() { return HadesParam(3); }
    void HadesParams(Double_t *params);

    Int_t GetHitIndex(Int_t ihit) const { return fHitInds[ihit]; }
    TBits &GetLR() { return fLRbits; }
    void Params(Double_t *pars) const { for (Int_t i = 0; i < 4; ++i) pars[i] = fParams[i]; }
    TMatrixDSym GetCovarMatr();

    void SetFlag(Int_t flag) { fFlag = flag; }
    void SetNDF(Int_t ndf) { fNDF = ndf; }
    void SetChi2(Double_t c2) { fChi2 = c2; }
    void SetZ(Double_t z) { fZ = z; }
    void SetLRbit(Int_t indx) { fLRbits.SetBitNumber(indx); }

    Int_t AddHit(Int_t indx);
    void SetCovar(TMatrixDSym matr);
    void SetParams(Double_t *pars) { for (Int_t i = 0; i < 4; ++i) fParams[i] = pars[i]; }

private:
    Double_t HadesParam(Int_t ipar);

private:
    Int_t fFlag;              // flag
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
