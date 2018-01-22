//*-- Author  : Rafal Lalik
//*-- Created : 03.05.2017

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

#include "TRandom.h"

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
    pFwDetRpcDigiPar = 0;
    pFwDetRpcGeomPar = 0;
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
        rot[m] = TMath::DegToRad() * phi;
        sina[m] = sin(rot[m]);
        cosa[m] = cos(rot[m]);
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

    Char_t  geaModule;      // module number (0...8); straw: 0,1
    Char_t  geaLayer;       // layer number (0..8): straw: 0-4
    Int_t   geaCell;        // cell number
    Char_t  geaSubCell;     // sub cell number

    Float_t time_reso = pFwDetRpcDigiPar->getTimeReso();
    Float_t time_prop = pFwDetRpcDigiPar->getTimeProp();
    Float_t time_offset = pFwDetRpcDigiPar->getTimeOffset();

    Int_t entries = pGeantFwDetCat->getEntries();

#ifdef VERBOSE_MODE
printf("<<-- RPC: VERBOSE_MODE ON - process %3d entries ->>\n", entries);
#endif

    for(int i = 0; i < entries; ++i)
    {
        ghit = (HGeantFwDet*)pGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(geaModule, geaLayer, geaCell, geaSubCell);

            if(geaModule < FWDET_RPC_MODULE_MIN or geaModule > FWDET_RPC_MODULE_MAX)
                continue; // skip the other detectors of the FwDet

            Char_t module;  // rpc module number
            Char_t layer;   // rpc layer number

            GeantFields gf;
            // calculate rpc cell number and set location indexes
            module = geaModule - FWDET_RPC_MODULE_MIN + (geaLayer >> 1);
            layer = geaLayer & 0x1;

            ghit->getHit(gf.xHit, gf.yHit, gf.zHit, gf.pxHit, gf.pyHit, gf.pzHit, gf.tofHit, gf.trackLength, gf.eHit);
            gf.trackNumber = ghit->getTrackNumber();

            Int_t m = (Int_t)module;
            Int_t l = (Int_t)layer;

            Float_t offy = offset[m][l];

            Float_t xx = gf.xHit;
            Float_t yy = gf.yHit + offy;

            Float_t x_lab = xx*cosa[m] - yy*sina[m];
            Float_t y_lab = xx*sina[m] + yy*cosa[m];

            Float_t strip_half_len = pFwDetRpcGeomPar->getStripLength(m, l)/2.;

            Float_t base_time = gf.tofHit + time_offset;
            // estimation of time
            Float_t tr = (strip_half_len + gf.xHit) / time_prop + base_time;
            Float_t tl = (strip_half_len - gf.xHit) / time_prop + base_time;

            if (time_reso > 0.0)
            {
                tl += gRandom->Gaus(0.0, time_reso);
                tl = TMath::Max(Float_t(0), tl);

                tr += gRandom->Gaus(0.0, time_reso);
                tr = TMath::Max(Float_t(0), tr);
            }

            // fake charge
            Float_t ql = (gf.xHit + strip_half_len) * time_prop + time_offset;
            Float_t qr = (strip_half_len - gf.xHit) * time_prop + time_offset;

            Int_t strip = findStrip(m, l, gf.yHit);
            Int_t max_strips = pFwDetRpcGeomPar->getStrips(m, l);
            if (strip < 0 || strip >= max_strips)
            {
                Error("HFwDetRpcDigitizer::execute()","Strip number %d,%d,%f -> %d outside allowed range %d,%d", m, l, gf.yHit, strip, 0, max_strips);
                continue;
            }

            ClusterFields cf;
            cf.m = m;
            cf.l = l;
            cf.s = strip;
            cf.tl = tl;
            cf.tr = tr;
            cf.ql = ql;
            cf.qr = qr;
            cf.tof = gf.tofHit;
            cf.track = gf.trackNumber;
            cf.x = x_lab;
            cf.y = y_lab;

#ifdef VERBOSE_MODE
printf("[%2d] (%d,%d,%d,%d) -> (%d,%d,%d,%d)\n", gf.trackNumber,
       geaModule, geaLayer, geaCell, geaSubCell, module, layer, geaCell, geaSubCell);
printf("(%d,%d)  cosa=%f, sina=%f, off=%f   ", m, l, cosa[m], sina[m], offset[m][l]);
printf("%f,%f -> %f,%f -> %f,%f\n", gf.xHit, gf.yHit, xx, yy, cf.x, cf.y);
#endif

            fillHit(cf);
        }
    }

    return 0;
}

Int_t HFwDetRpcDigitizer::findStrip(Int_t m, Int_t l, Float_t x)
{
    Int_t strips_num = pFwDetRpcGeomPar->getStrips(m, l);  // go to params
    Float_t strip_width = pFwDetRpcGeomPar->getStripWidth(m, l);
    Float_t strip_gap = pFwDetRpcGeomPar->getStripGap(m, l);
    Float_t ax = abs(x);

    if ((strips_num & 0x1) == 1) // odd strips calc
    {
        Int_t limit = (strips_num + 1)/2;
        Int_t i = 0;
        for (i = 0; i < limit; ++i)
        {
            Float_t upper_x = strip_width/2.0 + (strip_gap+strip_width)*i;
//             Float_t lower_x = -strip_width/2.0 + (strip_gap+strip_width)*i;

            if (ax < upper_x/* && ax > lower_x*/) // FIXME gap hit is assigned to a strip
            {
                if (x < 0)
                    return strips_num/2 - i;
                else
                    return strips_num/2 + i;
            }
        }
    }
    else // even strips calc
    {
        Int_t limit = strips_num/2;
        Int_t i = 0;
        for (i = 0; i < limit; ++i)
        {
            Float_t upper_x = -strip_gap/2.0 + (strip_gap+strip_width)*(i+1);
//             Float_t lower_x = strip_gap/2.0 + (strip_gap+strip_width)*i;

            if (ax < upper_x/* && ax > lower_x*/) // FIXME gap hit is assigned to a strip
            {
                if (x < 0)
                    return limit - 1 - i;
                else
                    return limit + i;
            }
        }
    }

    return -1;
}

bool HFwDetRpcDigitizer::fillHit(const ClusterFields & cf)
{
    fLoc[0] = cf.m;    // module
    fLoc[1] = cf.l;    // layer
    fLoc[2] = 0;       // dummy
    fLoc[3] = cf.s;    // strip

    HFwDetRpcCalSim * cal = (HFwDetRpcCalSim*)pFwDetRpcCalCat->getObject(fLoc);
    if (cal == nullptr)
    {
        cal = (HFwDetRpcCalSim*)pFwDetRpcCalCat->getSlot(fLoc);
        cal = new(cal) HFwDetRpcCalSim;
        cal->setAddress(cf.m, cf.l, cf.s);
    }

    if (cal)
    {
        Bool_t ret = cal->addHit(cf.tl, cf.tr, cf.ql, cf.qr);
        if (ret)
        {
            Int_t n = cal->getHitsNum();
            cal->setTrack(n-1, cf.track);
            cal->setHit(n-1, cf.x, cf.y, cf.tof);

            cal->reconstructHits(pFwDetRpcDigiPar->getTimeProp(),
                                 pFwDetRpcGeomPar->getStripLength(cf.m, cf.l),
                                 pFwDetRpcGeomPar->getStripWidth(cf.m, cf.l),
                                 pFwDetRpcGeomPar->getStripGap(cf.m, cf.l),
                                 pFwDetRpcGeomPar->getStrips(cf.m, cf.l),
                                 offset[cf.m][cf.l], rot[cf.m]);
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
