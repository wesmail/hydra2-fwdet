//*-- Author  : A.Zinchenko <alexander.zinchenko@jinr.ru>
//*-- Created : 2016
//*-- Modified: R. Lalik <Rafal.Lalik@ph.tum.de>

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetCand
//
//  Reconstructed vector
//
/////////////////////////////////////////////////////////////

#include "hfwdetcand.h"
#include "hfwdetrpchit.h"
#include "hmdcsizescells.h"

ClassImp(HFwDetCand);

HFwDetCand::HFwDetCand() :
    fNhits(0), fNDF(0), fDistance(0.0),
    dirVec(0., 0., 1.0), refVec(0., 0., 0.)
{
    for (Int_t i = 0; i < 16; ++i) fHitInds[i] = 0;
    for (Int_t i = 0; i < 10; ++i) fCovar[i] = 0;
}

HFwDetCand::~HFwDetCand() {}

Int_t HFwDetCand::addHit(Int_t indx)
{
    // Add hit with index indx to the track
    fHitInds[fNhits++] = indx;
    return fNhits;
}

TMatrixDSym HFwDetCand::getCovarMatr() const
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

void HFwDetCand::setCovar(TMatrixDSym cov)
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

void HFwDetCand::setHadesParams()
{
    // Convert to HADES track parameters
    Double_t x1 = refVec.X(), y1 = refVec.Y(), z1 = refVec.Z();
    Double_t dz = 100.0;
    Double_t z2 = z1 + dz, x2 = x1 + dirVec.X() * dz, y2 = y1 + dirVec.Y() * dz;

    Double_t p0, p1, p2, p3;
    HMdcSizesCells::calcMdcSeg(x1, y1, z1, x2, y2, z2, p0, p1, p2, p3);
    if (p3 < 0.) p3 += TMath::Pi()*2.0;

    setZ(p0);
    setR(p1);
    setTheta(p2*TMath::RadToDeg());
    setPhi(p3*TMath::RadToDeg());
}

void HFwDetCand::print() const
{
    printf("----- VECTOR -----\n");
    printf("   nhits=%d   ndf=%d   indexes=", fNhits, fNDF);
    for (Int_t i = 0; i < fNhits; ++i)
        printf("%2d,", fHitInds[i]);
    printf("\n");
    printf("      bits=");
    for (Int_t i = 0; i < fNhits; ++i)
        printf(" %c,", fLRbits[i] ? '+' : '-');
    printf("\n");
    printf("   ref=(%f,%f)   dir=(%f,%f)   z=%f   tof=%i\n",
           refVec.X(), refVec.Y(), dirVec.X(), dirVec.Y(), refVec.z(), getTofRec());
//     Double32_t fCovar[10];    // covar. matrix
    printf("   QA chi2=%f   chi2/ndf=%f\n", fChi2, fChi2/fNDF);
}

void HFwDetCand::getParams(Double_t* pars) const
{
    // get X, Y, Tx, Ty
    pars[0] = refVec.X();
    pars[1] = refVec.Y();
    pars[2] = dirVec.X();
    pars[3] = dirVec.Y();
}

void HFwDetCand::setParams(Double_t* pars)
{
    // set X, Y, Tx, Ty
    refVec.SetX(pars[0]);
    refVec.SetY(pars[1]);
    dirVec.SetX(pars[2]);
    dirVec.SetY(pars[3]);
}

void HFwDetCand::calc4vectorProperties(Double_t p, Double_t m)
{
    // Calc TLorentzVector for given momentum and mass
    TVector3 v3(dirVec);
    v3.SetMag(p);
    SetVectM(v3, m);
}

void HFwDetCand::calc4vectorProperties(Double_t m)
{
    // Calc TLorentzVector for given mass, momentum is calculated from TOF
    Float_t mom = HFwDetRpcHit::calcMomentum(fDistance, getTofRec(), m);
    calc4vectorProperties(mom, m);
}

void HFwDetCand::Streamer(TBuffer &R__b)
{
// Stream an object of class HFwDetCand.

    UInt_t R__s, R__c;
    if (R__b.IsReading()) {
        Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
        HVirtualCand::Streamer(R__b);
        R__b >> fNhits;
        R__b >> fNDF;
        R__b.WriteArray(fHitInds, FWDET_STRAW_MAX_VPLANES);
        R__b >> fDistance;
        R__b.WriteArray(fCovar, 10);
        dirVec.Streamer(R__b);
        refVec.Streamer(R__b);
        fLRbits.Streamer(R__b);
        R__b.CheckByteCount(R__s, R__c, HFwDetCand::IsA());
    } else {
        R__c = R__b.WriteVersion(HFwDetCand::IsA(), kTRUE);
        HVirtualCand::Streamer(R__b);
        R__b << fNhits;
        R__b << fNDF;
        R__b.ReadStaticArray((Int_t*)fHitInds);
        R__b << fDistance;
        R__b.ReadStaticArray((Double32_t*)fCovar);
        dirVec.Streamer(R__b);
        refVec.Streamer(R__b);
        fLRbits.Streamer(R__b);
        R__b.SetByteCount(R__c, kTRUE);
    }
}
