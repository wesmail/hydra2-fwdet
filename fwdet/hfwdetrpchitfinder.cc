//*-- Author  : Rafal Lalik
//*-- Created : 10.01.2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcHitFinder
//
//  This class searches for hits in the Forward Rpc detector data
//
//  Produce HFwDetRpcHit
//
/////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdetrpchitfinder.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hdebug.h"
#include "hspectrometer.h"
#include "hevent.h"
#include "hcategory.h"
#include "fwdetdef.h"
#include "hgeantkine.h"
#include "hgeantfwdet.h"
#include "hfwdetdetector.h"
#include "hfwdetrpccalsim.h"
#include "hfwdetrpchitfinderpar.h"
#include "hfwdetrpcgeompar.h"
#include "hfwdetrpchit.h"

// #define VERBOSE_MODE 1

ClassImp(HFwDetRpcHitFinder);

struct RpcHit {
    Float_t x, y, z, tof;
    Int_t track;
};

HFwDetRpcHitFinder::HFwDetRpcHitFinder()
{
    // default constructor
    initVariables();
}

HFwDetRpcHitFinder::HFwDetRpcHitFinder(const Text_t *name, const Text_t *title)
                    : HReconstructor(name, title)
{
    // constructor
    initVariables();
}

HFwDetRpcHitFinder::~HFwDetRpcHitFinder()
{
}

void HFwDetRpcHitFinder::initVariables()
{
    // initialize the variables in constructor
    pFwDetRpcCalCat = 0;
    pFwDetRpcHitCat = 0;
    pFwDetRpcGeomPar = 0;
    fLoc.setNIndex(1);
    fLoc.set(1,0,0);
}

Bool_t HFwDetRpcHitFinder::init() {
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
    {
        Error("FwDetRpcDigitizer::init", "No Forward Detector found");
        return kFALSE;
    }

    // get the Cal category
    pFwDetRpcCalCat = gHades->getCurrentEvent()->getCategory(catFwDetRpcCal);
    if (!pFwDetRpcCalCat)
    {
        Error("HFwDetRpcHitFinder::init()", "Cal category not found");
        return kFALSE;
    }

    // build the Calibration category
    pFwDetRpcHitCat = pFwDet->buildCategory(catFwDetRpcHit, kTRUE);
    if (!pFwDetRpcHitCat)
    {
        Error("HFwDetRpcHitFinder::init()", "Hit category not created");
        return kFALSE;
    }

    // create the parameter container
    pFwDetRpcHitFinderPar = (HFwDetRpcHitFinderPar *)gHades->getRuntimeDb()->getContainer("FwDetRpcHitFinderPar");
    if (!pFwDetRpcHitFinderPar)
    {
        Error("HFwDetRpcHitFinder::init()", "Parameter container for hit finder not created");
        return kFALSE;
    }

    // create the parameter container
    pFwDetRpcGeomPar = (HFwDetRpcGeomPar *)gHades->getRuntimeDb()->getContainer("FwDetRpcGeomPar");
    if (!pFwDetRpcGeomPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for geometry not created");
        return kFALSE;
    }

    return kTRUE;
}

Bool_t HFwDetRpcHitFinder::reinit()
{
#ifdef VERBOSE_MODE
    pFwDetRpcHitFinderPar->print();
#endif

    fMatchRadius = pFwDetRpcHitFinderPar->getMatchRadius();
    fMatchTime = pFwDetRpcHitFinderPar->getMatchTime();

    avg_z = 0.0;
    Int_t mods = pFwDetRpcGeomPar->getModules();
    for (Int_t i = 0; i < mods; ++i)
    {
        mod_z[i] = pFwDetRpcGeomPar->getModuleZ(i);
        avg_z += mod_z[i];
    }
    avg_z /= mods;

    return kTRUE;
}

Int_t HFwDetRpcHitFinder::execute()
{
    // Digitization of GEANT hits and storage in HFwDetRpcCalSim

    HFwDetRpcCalSim * cal = 0;

#ifdef VERBOSE_MODE
int cnt = 0;
printf("<<--- RPC: VERBOSE_MODE ON ---------------------->>\n");
#endif

    std::vector<RpcHit> hits[FWDET_RPC_MAX_MODULES];
    Int_t has_pair[FWDET_RPC_MAX_MODULES][100];

    Float_t x = 0.0, y = 0.0, z = 0.0, tof = -1.0;      // for CalSim
    Int_t track = -1;

    Int_t entries = pFwDetRpcCalCat->getEntries();
    for(int i = 0; i < entries; ++i)
    {
        cal = (HFwDetRpcCalSim*)pFwDetRpcCalCat->getObject(i);
        if (cal)
        {
            cal->getAddress(module, layer, strip);

            Int_t hitsNum = cal->getHitsNum();

            z = pFwDetRpcGeomPar->getModuleZ(module);

            for (Int_t h = 0; h < hitsNum; ++h)
            {
                cal->getHit(h, x, y, tof);
                cal->getTrack(h, track);

                RpcHit rpch;
                rpch.x = x;
                rpch.y = y;
                rpch.z = z;
                rpch.tof = tof;
                rpch.track = track;

                hits[(Int_t)module].push_back(rpch);
            }
        }
    }

    Int_t mods = pFwDetRpcGeomPar->getModules();
    for (Int_t m = 0; m < mods; ++m)
    {
        for (UInt_t i = 0; i < hits[m].size(); ++i)
        {
            has_pair[m][i] = 0;
        }
    }

    for (UInt_t i = 0; i < hits[0].size(); ++i)
    {
        RpcHit h1 = hits[0][i];
        for (UInt_t j = 0; j < hits[1].size(); ++j)
        {
            RpcHit h2 = hits[1][j];

            Float_t dx = h2.x - h1.x;
            Float_t dy = h2.y - h1.y;

            Float_t radius = sqrt(dx*dx + dy*dy);
            Float_t dt = h2.tof - h1.tof;

            if (radius < fMatchRadius*10. and dt < fMatchTime)
            {
                Float_t n_x = (h1.x + h2.x)/2.0;
                Float_t n_y = (h1.y + h2.y)/2.0;
                Float_t n_z = (h1.z + h2.z)/2.0;//avg_z;
                Float_t n_tof = (h1.tof + h2.tof)/2.0;
                Int_t tr = (h1.track == h2.track) ? h1.track : -1;

                fillHit(n_x, n_y, n_z, n_tof, tr);

                has_pair[0][i] = 1;
                has_pair[1][j] = 1;
            }
        }
    }

    for (Int_t m = 0; m < mods; ++m)
    {
        for (UInt_t i = 0; i < hits[m].size(); ++i)
        {
            if (!has_pair[m][i])
            {
                RpcHit h = hits[m][i];
                fillHit(h.x, h.y, h.z, h.tof, h.track);
            }
        }
    }

    return 0;
}

bool HFwDetRpcHitFinder::fillHit(Float_t x, Float_t y, Float_t z, Float_t tof, Int_t track)
{
    Int_t h = pFwDetRpcHitCat->getEntries();
    fLoc[0] = h;    // hit nr

    HFwDetRpcHit * hit = (HFwDetRpcHit*)pFwDetRpcHitCat->getObject(fLoc);
    if (hit == nullptr)
    {
        hit = (HFwDetRpcHit*)pFwDetRpcHitCat->getSlot(fLoc);
        hit = new(hit) HFwDetRpcHit;
    }

    if (hit)
    {
        hit->setHit(x, y, z, tof);
        hit->setTrack(track);

#ifdef VERBOSE_MODE
        hit->print();
        printf("\n");
#endif
    }
    else
        return false;

    return true;
}
