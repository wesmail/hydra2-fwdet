//*-- Author  : Luis silva
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcDigitizer
//
//  This class digitizes the Forward Rpc detector data
//
//  Produce calibrated Time and energy loss and the rpc number
//
/////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdetrpcdigitizer.h"
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
#include "hfwdetrpcdigipar.h"
#include "hfwdetrpcgeompar.h"

// #define VERBOSE_MODE 1

ClassImp(HFwDetRpcDigitizer);

HFwDetRpcDigitizer::HFwDetRpcDigitizer()
{
    // default constructor
    initVariables();
}

HFwDetRpcDigitizer::HFwDetRpcDigitizer(const Text_t *name, const Text_t *title)
                    : HReconstructor(name, title)
{
    // constructor
    initVariables();
}

HFwDetRpcDigitizer::~HFwDetRpcDigitizer()
{
}

void HFwDetRpcDigitizer::initVariables()
{
    // initialize the variables in constructor
    pGeantFwDetCat  = 0;
    pFwDetRpcCalCat = 0;
    pFwDetRpcDigiPar     = 0;
    pFwDetRpcGeomPar     = 0;
    fLoc.setNIndex(4);
    fLoc.set(4,0,0);
}

Bool_t HFwDetRpcDigitizer::init() {
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
    {
        Error("FwDetRpcDigitizer::init", "No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    pGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!pGeantFwDetCat)
    {
        Error("HFwDetRpcDigitizer::init()", "HGeant FwDet input missing");
        return kFALSE;
    }

    // build the Calibration category
    pFwDetRpcCalCat = pFwDet->buildCategory(catFwDetRpcCal, kTRUE);
    if (!pFwDetRpcCalCat)
    {
        Error("HFwDetRpcDigitizer::init()", "Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    pFwDetRpcDigiPar = (HFwDetRpcDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetRpcDigiPar");
    if (!pFwDetRpcDigiPar)
    {
        Error("HFwDetRpcDigitizer::init()", "Parameter container for digitizer not created");
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

Bool_t HFwDetRpcDigitizer::reinit()
{
    Int_t mods = pFwDetRpcGeomPar->getModules();

    for (Int_t m = 0; m < mods; ++m)
    {
        Float_t phi = pFwDetRpcGeomPar->getModulePhi(m);
        sina[m] = sin(TMath::DegToRad() * phi);
        cosa[m] = cos(TMath::DegToRad() * phi);
        Int_t lays = pFwDetRpcGeomPar->getLayers(m);
        for (Int_t l = 0; l < lays; ++l)
        {
            offset[m][l] = pFwDetRpcGeomPar->getLayerY(m, l);
        }
    }
#ifdef VERBOSE_MODE
pFwDetRpcGeomPar->print();
#endif

    return kTRUE;
}

Int_t HFwDetRpcDigitizer::execute()
{
    // Digitization of GEANT hits and storage in HFwDetRpcCalSim

    HGeantFwDet* ghit = 0;

#ifdef VERBOSE_MODE
int cnt = 0;
printf("<<--- RPC: VERBOSE_MODE ON ---------------------->>\n");
#endif

    c_tr = -1;      // current (tracked) variables
    c_mod = -1;
    c_lay = -1;

    RpcTrackHits rpc_track_hit;
    rpc_track_hit.hits_num = 0;

    Int_t entries = pGeantFwDetCat->getEntries();
    for(int i = 0; i < entries; ++i)
    {
        ghit = (HGeantFwDet*)pGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(geantModule, geantLayer, geantCell);

            if(geantModule < FWDET_RPC_MODULE_MIN or geantModule > FWDET_RPC_MODULE_MAX)
                continue; // skip the other detectors of the FwDet

            // calculate rpc cell number and set location indexes
            module = geantModule - FWDET_RPC_MODULE_MIN;

            ghit->getHit(xHit, yHit, zHit, pxHit, pyHit, pzHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();

            if (trackNumber != c_tr or geantModule != c_mod or geantLayer != c_lay)
            {
                // new track, but do something with the old one
                if (rpc_track_hit.hits_num > 0)
                    processHits(rpc_track_hit);

                c_tr = trackNumber;
                c_mod = geantModule;
                c_lay = geantLayer;

                rpc_track_hit.mod = module;
                rpc_track_hit.lay = geantLayer;
                rpc_track_hit.hits_num = 0;
            }

            Int_t m = (Int_t)module;
            Int_t l = (Int_t)geantLayer;

            Float_t offy = offset[m][l];

#ifdef VERBOSE_MODE
printf("(%d,%d)  cosa=%f, sina=%f, off=%f   ", m, l, cosa[m], sina[m], offset[m][l]);
#endif

            Float_t xx = xHit;
            Float_t yy = yHit + offy;

            x = xx*cosa[m] - yy*sina[m];
            y = xx*sina[m] + yy*cosa[m];

#ifdef VERBOSE_MODE
printf("%f,%f -> %f,%f -> %f,%f\n", xHit, yHit, xx, yy, x, y);
#endif


#ifdef VERBOSE_MODE
// printf("(%2d) gm=%d  gl=%d  gc=%d tn=%d -> hit=(%f,%f,%f)  el=%f  tof=%f  p=%f (%f,%f,%f)\n", ++cnt, geantModule, geantLayer, geantCell, trackNumber, xHit, yHit, zHit, eHit, tofHit, sqrt(pxHit*pxHit + pyHit*pyHit + pzHit*pzHit), pxHit, pyHit, pzHit);
#endif

            Int_t hn = rpc_track_hit.hits_num;
            rpc_track_hit.cell[hn] = geantCell;
            rpc_track_hit.x[hn] = xHit;
            rpc_track_hit.y[hn] = yHit;
            rpc_track_hit.tof[hn] = tofHit;
            ++rpc_track_hit.hits_num;
        }
    }

    if (rpc_track_hit.hits_num > 0)
        processHits(rpc_track_hit);

    return 0;
}

void HFwDetRpcDigitizer::processHits(const RpcTrackHits & rpc_track_hit)
{
    // here you process the data, e.g.

#ifdef VERBOSE_MODE
    rpc_track_hit.print();
    printf("\n");
#endif

    Int_t fired_cells = rpc_track_hit.hits_num;

    Float_t avg_x = 0.0;
    Float_t avg_y = 0.0;
    Float_t avg_t = 0.0;

    Int_t cnt_cells = 0;
    Int_t tracked_strip = -1;
    Int_t current_strip = -1;

    // loop over all cells
    for (Int_t i = 0; i < fired_cells; ++i)
    {
        // get current strip number (project hit coordinate to strip)
        // y (in local geometry) is used as a coordinate to reflect strip
        current_strip = findStrip(rpc_track_hit.y[i]);

        // if another strip that before, process the strip's hit
        if (current_strip != tracked_strip)
        {
            if (cnt_cells > 0)
            {
                // calculate and save hit from strip
                // avg_x is used to reflect position along strip
                calculateHit(rpc_track_hit.mod, rpc_track_hit.lay,
                    current_strip, avg_x / cnt_cells, avg_t / cnt_cells);
            }

            // reset variables
            cnt_cells = 0;
            tracked_strip = current_strip;
        }

        avg_x += rpc_track_hit.x[i];
        avg_y += rpc_track_hit.y[i];
        avg_t += rpc_track_hit.tof[i];
        ++cnt_cells;
    }

    if (cnt_cells > 0)
    {
        // calculate and save hit from strip
        calculateHit(rpc_track_hit.mod, rpc_track_hit.lay,
            current_strip, avg_y / cnt_cells, avg_t / cnt_cells);
    }
}

bool HFwDetRpcDigitizer::calculateHit(Int_t mod, Int_t lay, Int_t s, Float_t a_y, Float_t a_tof)
{
    Float_t strip_length = 900.;    // should go to params
    Float_t dt_slope = 0.2;         // should go to params
    Float_t dt_offset = 0.;         // should go to params

    Float_t strip_half_len = strip_length/2.;

    // estimation of time
    Float_t tl = (a_y + strip_half_len) * dt_slope + dt_offset;
    Float_t tr = (strip_half_len - a_y) * dt_slope + dt_offset;

    // fake charge
    Float_t ql = (a_y + strip_half_len) * dt_slope + dt_offset;
    Float_t qr = (strip_half_len - a_y) * dt_slope + dt_offset;

    // save hit from strip

    return fillHit(mod, lay, s, tl, tr, ql, qr);
}

Int_t HFwDetRpcDigitizer::findStrip(Float_t x)
{
    Int_t strips_num = 30;  // go to params
    Float_t strip_gap = 1.0;    // go to params
    Float_t strip_width = 30.0;

    // quick estimations
    // no gap included
    // center of layer is (0,0) coordinate
    Int_t strip = (x / strip_width) + strips_num/2;

    return strip;
}

bool HFwDetRpcDigitizer::fillHit(Int_t m, Int_t l, Int_t s, Float_t tl, Float_t tr, Float_t ql, Float_t qr)
{
    fLoc[0] = m;    // module
    fLoc[1] = l;    // layer
    fLoc[2] = 0;    // dummy
    fLoc[3] = s;    // strip

    HFwDetRpcCalSim * cal = (HFwDetRpcCalSim*)pFwDetRpcCalCat->getObject(fLoc);
    if (cal == nullptr)
    {
        cal = (HFwDetRpcCalSim*)pFwDetRpcCalCat->getSlot(fLoc);
        cal = new(cal) HFwDetRpcCalSim;
        cal->setAddress(m, l, s);
    }

    if (cal)
    {
        Bool_t ret = cal->addHit(tl, tr, ql, qr);
        if (ret)
        {
            Int_t n = cal->getHitsNum();
            cal->setTrack(n-1, c_tr);
            cal->setHit(n-1, x, y, tofHit);
        }

#ifdef VERBOSE_MODE
        cal->print();
        printf("\n");
#endif
    }
    else
        return false;

    return true;
}
