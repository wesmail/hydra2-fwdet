//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016
//*-- Modified : 29/07/2016 by V.Pechenov

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawDigitizer
//
//  This class digitizes the Forward Straw detector data
//
//  Produce calibrated Time and energy loss and the straw number
//
//
//
//  !!! There digitization is not implemented yet !!!
//  Programm store to the output category catFwDetStrawCal tof from geant hit
//  and minimal distance from geant track to the wire.
//
//  Geant stores three values: module, layer, cell.
//  Each module contains fixed number of layers. Each layer is composed of
/// several double-modules. Each double-module is built of single plane blocks.
//  Each block is built out of straws.
//
//  The first double-module is most left (negative x), the last is most right
//  (positive x).
//  Odd block number is for the front plane, even for the back plane. Block
//  numbers increas from left (negative x) to right (positive x).
//  Straws in a block goes from left (negative x) to right (positive x).
//
//  Cell value encodes position of single straw in a layer in a following way:
//    cell=double_module*100 + block*10 + straw
//  e.g. cell=437 -> double_module 4, block 3 (back plane), straw 7
//
//  Geometry of a single straw module for station T1(T2) is following:
//  Number of layers in a module:               4 (4)
//  Number of double-modules in a layer:        5 (7)
//  Number of blocks in a double-module:        4 (4)
//  Number of straws in a block:                8 (8)
//
//  While module and layer numbers go from 0..n, double_module, block and straw
//  numbers encoded in the cell value go 1..n
/////////////////////////////////////////////////////////////

#include "hfwdetstrawdigitizer.h"
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
#include "hfwdetstrawcalsim.h"
#include "hfwdetstrawdigipar.h"
#include "hfwdetstrawgeompar.h"
#include "hfwdetgeompar.h"

#include "TRandom3.h"
#include "TVector2.h"

#include <cstring>
using namespace std;

// #define VERBOSE_MODE 1

ClassImp(HFwDetStrawDigitizer);

HFwDetStrawDigitizer::HFwDetStrawDigitizer()
{
   // default constructor
   initVariables();
}

HFwDetStrawDigitizer::HFwDetStrawDigitizer(const Text_t *name, const Text_t *title) :
   HReconstructor(name, title)
{
   // constructor
   initVariables();
}

HFwDetStrawDigitizer::~HFwDetStrawDigitizer()
{
}

void HFwDetStrawDigitizer::initVariables(void)
{
    // initialize the variables in constructor
    pGeantFwDetCat = nullptr;
    pStrawCalCat = nullptr;
    pStrawDigiPar = nullptr;
    fLoc.setNIndex(4);
    fLoc.set(4,0,0,0,0);

    rand = nullptr;
}

Bool_t HFwDetStrawDigitizer::init(void)
{
    rand = new TRandom3(0); 

    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* p = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!p)
    {
        Error("FwDetStrawDigitizer::init","No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    pGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!pGeantFwDetCat)
    {
        Error("HFwDetStrawDigitizer::init()","HGeant FwDet input missing");
        return kFALSE;
    }

    // build the Calibration category
    pStrawCalCat = p->buildCategory(catFwDetStrawCal, kTRUE);
    if (!pStrawCalCat)
    {
        Error("HFwDetStrawDigitizer::init()","Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    pStrawDigiPar = (HFwDetStrawDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
    if (!pStrawDigiPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for digitizer not created");
        return kFALSE;
    }

    // create the parameter container
    pStrawGeomPar = (HFwDetStrawGeomPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawGeomPar");
    if (!pStrawGeomPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for geometry not created");
        return kFALSE;
    }

    return kTRUE;
}

Bool_t HFwDetStrawDigitizer::reinit()
{
    adc_reso = pStrawDigiPar->getAnalogReso();
    eloss_slope = pStrawDigiPar->getElossSlope();
    eloss_offset = pStrawDigiPar->getElossOffset();

    time_reso = pStrawDigiPar->getTimeReso();
    dt_p[0] = pStrawDigiPar->getDriftTimePar(0);
    dt_p[1] = pStrawDigiPar->getDriftTimePar(1);
    dt_p[2] = pStrawDigiPar->getDriftTimePar(2);
    dt_p[3] = pStrawDigiPar->getDriftTimePar(3);
    dt_p[4] = pStrawDigiPar->getDriftTimePar(4);

    start_offset = pStrawDigiPar->getStartOffset();
    threshold = pStrawDigiPar->getThreshold();
    efficiency = pStrawDigiPar->getEfficiency();

    for (Int_t m = 0; m < FWDET_STRAW_MAX_MODULES; ++m)
        for (Int_t l = 0; l < FWDET_STRAW_MAX_LAYERS; ++l)
        {
            Float_t a = pStrawGeomPar->getLayerRotation(m, l) * TMath::DegToRad();
            cosa[m][l] = TMath::Cos(a);
            sina[m][l] = TMath::Sin(a);
        }

#ifdef VERBOSE_MODE
    pStrawDigiPar->print();
#endif

    return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void)
{
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim
    // gErrorIgnoreLevel = kFatal;

#ifdef VERBOSE_MODE
int cnt = -1;
printf("<<----------------------------------------------->>\n");
#endif

    Int_t entries = pGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        HGeantFwDet* ghit = (HGeantFwDet*)pGeantFwDetCat->getObject(i);
        if (ghit)
        {
#ifdef VERBOSE_MODE
            ++cnt;
#endif
            ghit->getAddress(geantModule, geantLayer, geantCell);

            Int_t mod = geantModule;

            if(mod > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            // detection efficiency
            Float_t rnd = rand->Rndm();
#ifdef VERBOSE_MODE
printf("(%2d) det eff: rand=%.2f  <?<  eff=%.2f\n", cnt, rnd, efficiency);
#endif
            if (rnd > efficiency)
                continue;

            Int_t lay = geantLayer;
            Int_t plane = -1;
            Int_t cell = -1;

            if(mod > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            Int_t l_panel = (Int_t) geantCell/100 - 1;
            Int_t l_block = (Int_t) (geantCell%100)/10 - 1;
            Int_t l_straw = (Int_t) geantCell%10 - 1;

            if (l_block % 2 == 0)
                plane = 0; // TODO add module and layer dep
            else
                plane = 1; // TODO as above

            Int_t n_blocks = pStrawGeomPar->getBlocks(mod);
            Int_t n_straws = pStrawGeomPar->getStraws(mod);

            // for a single panel, blocks/2 blocks for a plane =>  n_blocks >> 1
            // 
            //     | no of straws in a plane of panel | * panel number +
            //     | number of straw in current panel |
            cell = ((n_blocks >> 1) * n_straws ) * l_panel +
                (l_block >> 1) * n_straws + l_straw;

            // straws are rotated in the geo by 90 degrees around x axis
            // y and z must be swap
            ghit->getHit(xHit, zHit,  yHit, pxHit, pzHit, pyHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();

            // in the sim, z-axis is inverted
            pzHit = -pzHit;
            zHit = -zHit;

            fLoc[0] = mod;
            fLoc[1] = lay;
            fLoc[2] = plane;
            fLoc[3] = cell;

            Float_t cell_x = pStrawGeomPar->getOffsetX(mod, lay, plane) + cell*pStrawGeomPar->getStrawPitch(mod, lay);
            Float_t cell_z = pStrawGeomPar->getOffsetZ(mod, lay, plane);

            Float_t hit_x = cell_x + xHit;
            Float_t hit_y = yHit;
            Float_t hit_z = cell_z + zHit;

            Float_t loc_x = hit_x*cosa[mod][lay] - hit_y*sina[mod][lay];
            Float_t loc_y = hit_x*sina[mod][lay] + hit_y*cosa[mod][lay];
            Float_t loc_z = hit_z;

            hit_x = xHit + cell_x;
            hit_y = yHit;
            hit_z = zHit + cell_z;

            TVector2 v_n(pzHit, pxHit);                 // track vector dir.  n
            v_n = v_n/v_n.Mod();
            TVector2 p_a(zHit + cell_z, xHit + cell_x); // track hit position a
            TVector2 p_p(cell_z, cell_x);               // wire position      p

            // calculations:
            // dist = | (a-p) - ((a-p)*n)n|
            TVector2 a_p_diff = p_a - p_p;
            TVector2 v_proj_n_diff = (a_p_diff * v_n) * v_n;
            TVector2 v_dist = a_p_diff - v_proj_n_diff;
            Float_t radius = v_dist.Mod();

            Float_t tof = tofHit;
            Float_t eloss = eHit;
            Float_t time = tof + calcDriftTime(radius);
            Float_t adc = eloss*eloss_slope + eloss_offset;

#ifdef VERBOSE_MODE
printf("     g (m=%d l=%d c=%d) -> p=%d s=%03d  u=%f  el=%.4f tof=%.3f dr=%f  adc=%.2f time=%f  p=%f (%f,%f) loc=(%f,%f,%f)\n", geantModule, geantLayer, geantCell, plane, cell, cell_x, eloss, tof, radius, adc, time, sqrt(pxHit*pxHit + pyHit*pyHit + pzHit*pzHit), pxHit/pzHit, pyHit/pzHit, loc_x, loc_y, loc_z);
#endif

            if (adc_reso > 0.0)
            {
                adc += + rand->Gaus() * adc_reso;
                adc = TMath::Max(Float_t(0), adc);
            }

            if (adc < threshold)
                continue;

            if (time_reso > 0.0)
            {
                time += rand->Gaus() * time_reso;
                time = TMath::Max(Float_t(0), time);
            }

            time += start_offset;

#ifdef VERBOSE_MODE
printf("     resolution effects                                                             adc=%.2f time=%f\n", adc, time);
#endif

            Bool_t res = fillStrawCalSim(time, adc, tof, eloss, radius, cell_x, cell_z, cell, loc_x, loc_y, loc_z);
            if (!res)
            {
                Error("HFwDetStrawDigitizer::execute()",
                "Can't fill from %d for m=%d l=%d p=%d s=%d", geantCell, mod, lay, plane, cell);
//                 return kFALSE;
            }
        }
    }
    return 0;
}

Bool_t HFwDetStrawDigitizer::fillStrawCalSim(Float_t time, Float_t adc, Float_t tof, Float_t eloss, Float_t radius, Float_t posX, Float_t posZ, Int_t straw, Float_t lx, Float_t ly, Float_t lz)
{
    // function creat and fill object HFwDetStrawCalSim
    // return kFALSE if data was not stored to the category

    Int_t first = 1;

    HFwDetStrawCalSim * cal = (HFwDetStrawCalSim*)pStrawCalCat->getObject(fLoc);
    if (cal != nullptr)    // straw tube ocuppied by another track
    {
        if (cal->getDrift() <= radius)
        {
            cal->setTrack(trackNumber);
            return kFALSE;
        }
        first = 0;
    }
    else
    {
        cal = (HFwDetStrawCalSim*)pStrawCalCat->getSlot(fLoc);
    }
    if (cal)
    {
        if (first) cal = new(cal) HFwDetStrawCalSim;
        cal->setAddress(fLoc[0], fLoc[1], fLoc[2], fLoc[3]);

        cal->setHit(time, adc, posX, posZ, straw);

        cal->setToF(tof);
        cal->setDrift(radius);
        cal->setEloss(eloss);
        cal->setTrack(trackNumber);
        cal->setP(pxHit, pyHit, pzHit);
        cal->setHitPos(lx, ly, lz);
    }
    else
        return kFALSE;

    return kTRUE;
}

Float_t HFwDetStrawDigitizer::calcDriftTime(Float_t x) const
{
    Float_t x2 = x * x;
    Float_t x3 = x2 * x;
    Float_t x4 = x3 * x;

    Float_t dt = dt_p[0] + dt_p[1]*x + dt_p[2]*x2 + dt_p[3]*x3 + dt_p[4]*x4;
    return dt;
}
