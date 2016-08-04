// @(#)$Id: dummyRec.cc,v 1.3 2008-09-18 13:05:56 halo Exp $
// -----------------------------------------------------------------------------
// A simple example how to use HSUDummyRec class

#include "hsudummyrec.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hlocation.h"

#include "hgeantdef.h"
#include "showerdef.h"
#include "hgeantkine.h"
#include "hshowerhittrack.h"
#include "hmdchitsim.h"

#include "hphysicsconstants.h"

#include "TNtuple.h"
#include "TMath.h"

// -----------------------------------------------------------------------------

class HDummyRec : public HSUDummyRec
{
public:

    HDummyRec(const Char_t *pOutFileName);

    // -------------------------------------------------------------------------

    virtual Bool_t init(void);
    virtual Int_t  execute(void);
    virtual Bool_t finalize(void);

    // -------------------------------------------------------------------------

private:

    HCategory *pGeantCat;
    HIterator *pShowerIt;
    HIterator *pMdcIt;

    TNtuple   *pOut;

    TString    sFileName;

    HLocation  loc;
};

// -----------------------------------------------------------------------------

HDummyRec::HDummyRec(const Char_t *pOutFileName)
{
    sFileName = pOutFileName;
    pGeantCat = NULL;
    pShowerIt = NULL;
    pMdcIt    = NULL;
    pOut      = NULL;

    loc.set(2, 0, 0);
}

// -----------------------------------------------------------------------------

Bool_t HDummyRec::init(void)
{
TFile *pFile = gFile;

    setInitOk(kFALSE);

    if((pGeantCat = getCategory(catGeantKine)) == NULL)
    {
        Error("init", "Cannot get catGeantKine cat");
        goto lab_End;
    }

    if((pShowerIt = getIterator(catShowerHitTrack)) == NULL)
    {
        Error("init", "Cannot get catShowerHitTrack iterator");
        goto lab_End;
    }

    if((pMdcIt = getIterator("HMdcHitSim")) == NULL)
    {
        Error("init", "Cannot get HMdcHitSim iterator");
        goto lab_End;
    }

    if(openOutFile(sFileName.Data()) == NULL)
        return kFALSE;

    if((pOut = new TNtuple("res", "Shower sim hits",
                    "pid:mom:beta:theta:sum0:sum1:sum2:ch:row:col")) == NULL)
    {
        Error("init", "Cannot create NTuple");
        goto lab_End;
    }

    if(pFile != NULL)
        pFile->cd();

    setInitOk();

lab_End:

    return getInitOk();
}

// -----------------------------------------------------------------------------

Bool_t HDummyRec::finalize(void)
{
    return writeAndCloseOutFile();
}

// -----------------------------------------------------------------------------

Int_t HDummyRec::execute(void)
{
HShowerHitTrack *pHit;
HGeantKine      *pKine;
Float_t          fX, fY, fZ, fP, fTheta, fBeta, fM;
Int_t            iTrack, iPid;
HMdcHitSim      *pMdc;
Int_t            iSector, i;

    pShowerIt->Reset();

    while((pHit = (HShowerHitTrack *) pShowerIt->Next()) != NULL)
    {
        if((pHit->getModule() != 0) || (pHit->getTrack() < 1))
            continue;

        iTrack = pHit->getTrack();

        if((pKine = (HGeantKine *)pGeantCat->getObject(iTrack - 1)) == NULL)
        {
            Error("execute", "No HGeantKine %d", pHit->getTrack() - 1);
            continue;
        }

        pKine->getVertex(fX, fY, fZ);
        if((fabs(fZ) > 2.5) || (fX * fX + fY * fY > 4.0))
            continue;

        // checking, wheather the track was seen in the MDC
        for(iSector = 0; iSector < 6; iSector++)
        {
            loc[0] = iSector;
            pMdcIt->Reset();
            pMdcIt->gotoLocation(loc);

            while((pMdc = (HMdcHitSim *)pMdcIt->Next()) != NULL)
            {
                for(i = 0; i < pMdc->getNTracks(); i++)
                {
                    if(pMdc->getTrack(i) == iTrack)
                        goto lab_Ok;
                }
            }
        }

        continue;

lab_Ok:
        pKine->getMomentum(fX, fY, fZ);
        fP = pKine->getTotalMomentum();
        fTheta = TMath::RadToDeg() * TMath::ACos(fZ / fP);

        pKine->getParticle(iTrack, iPid);

        if((fM = HPhysicsConstants::mass(iPid)) <= 0.0f)
            continue;

        fBeta = fP / TMath::Sqrt(fP * fP + fM * fM);

        pOut->Fill(iPid, fP, fBeta, fTheta,
                    pHit->getSum(0), pHit->getSum(1), pHit->getSum(2),
                    pHit->getCharge(),
                    pHit->getRow(), pHit->getCol());
    }

    return 0;
}
