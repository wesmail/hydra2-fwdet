//*-- Author  : A.Zinchenko <alexander.zinchenko@jinr.ru>
//*-- Created : 2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HVectorCand
//
//  Segments / tracks in the Forward Straw tracker.
//
/////////////////////////////////////////////////////////////

#include "hvectorcand.h"
#include "hfwdetrpchit.h"
#include "hmdcsizescells.h"

// -----   Default constructor   -------------------------------------------
HVectorCand::HVectorCand() : TLorentzVector(),
    fTrack(-1), fRpcTrack(-1), fNhits(0), fNDF(0), fTof(-1.0), fChi2(0.0),
    dirVec(0., 0., 1.0), refVec(0., 0., 0.)
{
    for (Int_t i = 0; i < 16; ++i) fHitInds[i] = 0;
    for (Int_t i = 0; i < 10; ++i) fCovar[i] = 0;
}
// -------------------------------------------------------------------------

// -----   Destructor   ----------------------------------------------------
HVectorCand::~HVectorCand()
{
}
// -------------------------------------------------------------------------

// -----   Public method AddHit   ------------------------------------------
Int_t HVectorCand::addHit(Int_t indx)
{
    // Add hit with index indx to the track

    fHitInds[fNhits++] = indx;
    return fNhits;
}
// -------------------------------------------------------------------------

// -----   Public method GetCovarMatr   ------------------------------------
TMatrixDSym HVectorCand::getCovarMatr() const
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
void HVectorCand::setCovar(TMatrixDSym cov)
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
void HVectorCand::getHadesParams(Double_t *params) const
{
    // Convert to HADES track parameters

    Double_t x1 = refVec.X(), y1 = refVec.Y(), z1 = refVec.Z();
    Double_t dz = 100.0;
    Double_t z2 = z1 + dz, x2 = x1 + dirVec.X() * dz, y2 = y1 + dirVec.Y() * dz;
    HMdcSizesCells::calcMdcSeg(x1, y1, z1, x2, y2, z2, params[0], params[1], params[2], params[3]);
}
// -------------------------------------------------------------------------

// -----   Private method HadesParam   -------------------------------------
Double_t HVectorCand::getHadesParam(Int_t ipar) const
{
    // Convert (at first call) and return HADES track parameters

    Double_t params[4];
    getHadesParams(params);
    return params[ipar];
}
// -------------------------------------------------------------------------

void HVectorCand::print() const
{
    printf("----- VECTOR -----\n");
    printf("   track=%d   nhits=%d   ndf=%d\n", fTrack, fNhits, fNDF);
    printf("   indexes=");
    for (Int_t i = 0; i < fNhits; ++i)
        printf("%2d,", fHitInds[i]);
    printf("\n");
    printf("      bits=");
    for (Int_t i = 0; i < fNhits; ++i)
        printf(" %c,", fLRbits[i] ? '+' : '-');
    printf("\n");
    printf("   ref=(%f,%f)   dir=(%f,%f)   z=%f   tof=%f\n",
           refVec.X(), refVec.Y(), dirVec.X(), dirVec.Y(), refVec.z(), fTof);
//     Double32_t fCovar[10];    // covar. matrix
    printf("   QA chi2=%f\n", fChi2);
}

void HVectorCand::getParams(Double_t* pars) const
{
    pars[0] = refVec.X();
    pars[1] = refVec.Y();
    pars[2] = dirVec.X();
    pars[3] = dirVec.Y();
}

void HVectorCand::setParams(Double_t* pars)
{
    refVec.SetX(pars[0]);
    refVec.SetY(pars[1]);
    dirVec.SetX(pars[2]);
    dirVec.SetY(pars[3]);
}

void HVectorCand::calc4vectorProperties(Double_t p, Double_t m)
{
    TVector3 v3(dirVec);
    v3.SetMag(p);

    SetVectM(v3, m);
}

void HVectorCand::calc4vectorProperties(Double_t m)
{
    Float_t mom = HFwDetRpcHit::calcMomentum(fTofL, fTof, m);

    calc4vectorProperties(mom, m);
}

ClassImp(HVectorCand);
