/**  HFwDetStrawVector.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

#include "hfwdetstrawvector.h"
#include "hmdcsizescells.h"

// -----   Default constructor   -------------------------------------------
HFwDetStrawVector::HFwDetStrawVector() : TObject(),
    fFlag(0), fNhits(0), fNDF(0), fZ(0.0), fChi2(0.0)
{
    for (Int_t i = 0; i < 4; ++i) fParams[i] = 0.0;
    for (Int_t i = 0; i < 16; ++i) fHitInds[i] = 0;
    for (Int_t i = 0; i < 10; ++i) fCovar[i] = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
HFwDetStrawVector::~HFwDetStrawVector()
{
}
// -------------------------------------------------------------------------

// -----   Public method AddHit   ------------------------------------------
Int_t HFwDetStrawVector::AddHit(Int_t indx)
{
    // Add hit with index indx to the track

    fHitInds[fNhits++] = indx;
    return fNhits;
}
// -------------------------------------------------------------------------

// -----   Public method GetCovarMatr   ------------------------------------
TMatrixDSym HFwDetStrawVector::GetCovarMatr()
{
  // Return covariance matrix as TMatrixDSym

    TMatrixDSym cov(4);
    Int_t ipos = 0;
    for (Int_t i = 0; i < 4; ++i)
    {
        for (Int_t j = 0; j <= i; ++j)
        {
            cov(i,j) = fCovar[ipos++];
            cov(j,i) = cov(i,j);
        }
    }
    return cov;
}
// -------------------------------------------------------------------------

// -----   Public method SetCovar   ----------------------------------------
void HFwDetStrawVector::SetCovar(TMatrixDSym cov)
{
  // Set covariance matrix

    Int_t ipos = 0;
    for (Int_t i = 0; i < 4; ++i)
    {
        for (Int_t j = 0; j <= i; ++j)
        {
            fCovar[ipos++] = cov(i,j);
        }
    }
}
// -------------------------------------------------------------------------

// -----   Public method HadesParams   -------------------------------------
void HFwDetStrawVector::HadesParams(Double_t *params)
{
    // Convert to HADES track parameters

    Double_t x1 = fParams[0], y1 = fParams[1], z1 = fZ;
    Double_t dz = 100.0;
    Double_t z2 = z1 + dz, x2 = x1 + fParams[2] * dz, y2 = y1 + fParams[3] * dz;
    HMdcSizesCells::calcMdcSeg(x1, y1, z1, x2, y2, z2, params[0], params[1], params[2], params[3]);
}
// -------------------------------------------------------------------------

// -----   Private method HadesParam   -------------------------------------
Double_t HFwDetStrawVector::HadesParam(Int_t ipar)
{
    // Convert (at first call) and return HADES track parameters

    Double_t params[4];
    HadesParams(params);
    return params[ipar];
}
// -------------------------------------------------------------------------
ClassImp(HFwDetStrawVector);
