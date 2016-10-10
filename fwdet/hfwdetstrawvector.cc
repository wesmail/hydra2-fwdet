/**  HFwDetStrawVector.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

#include "hfwdetstrawvector.h"

// -----   Default constructor   -------------------------------------------
HFwDetStrawVector::HFwDetStrawVector()
  : TObject(),
    fFlag(0),
    fNhits(0),
    fNDF(0),
    fChi2(0.0)
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
  for (Int_t i = 0; i < 4; ++i) {
    for (Int_t j = 0; j <= i; ++j) {
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
  for (Int_t i = 0; i < 4; ++i) {
    for (Int_t j = 0; j <= i; ++j) {
      fCovar[ipos++] = cov(i,j);
    }
  }
}
// -------------------------------------------------------------------------

ClassImp(HFwDetStrawVector);
