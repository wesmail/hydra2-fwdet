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

#include "TRandom.h"
#include "TVector2.h"

#include <cstring>
using namespace std;

// #define VERBOSE_MODE1
// #define VERBOSE_MODE2

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

HFwDetStrawDigitizer::~HFwDetStrawDigitizer() { }

void HFwDetStrawDigitizer::initVariables(void)
{
    // Initialize the variables in constructor
    pGeantFwDetCat = nullptr;
    pStrawCalCat = nullptr;
    pStrawDigiPar = nullptr;
    fLoc.setNIndex(5);
    fLoc.set(5,0,0,0,0);
}

Bool_t HFwDetStrawDigitizer::init(void)
{
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
//     adc_reso = pStrawDigiPar->getAnalogReso();
//     eloss_slope = pStrawDigiPar->getElossSlope();
//     eloss_offset = pStrawDigiPar->getElossOffset();
//
//     time_reso = pStrawDigiPar->getTimeReso();
    dt_p[0] = pStrawDigiPar->getDriftTimePar(0);
    dt_p[1] = pStrawDigiPar->getDriftTimePar(1);
    dt_p[2] = pStrawDigiPar->getDriftTimePar(2);
    dt_p[3] = pStrawDigiPar->getDriftTimePar(3);
    dt_p[4] = pStrawDigiPar->getDriftTimePar(4);
//
//     start_offset = pStrawDigiPar->getStartOffset();
//     threshold = pStrawDigiPar->getThreshold();
//     efficiency = pStrawDigiPar->getEfficiency();

    for (Int_t m = 0; m < FWDET_STRAW_MAX_MODULES; ++m)
        for (Int_t l = 0; l < FWDET_STRAW_MAX_LAYERS; ++l)
        {
            Float_t a = pStrawGeomPar->getLayerRotation(m, l) * TMath::DegToRad();
            cosa[m][l] = TMath::Cos(a);
            sina[m][l] = TMath::Sin(a);
        }

#ifdef VERBOSE_MODE1
    pStrawGeomPar->printParams();
    pStrawDigiPar->printParams();
#endif
    return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void)
{
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim

    Char_t  geaModule;      // module number (0...8); straw: 0,1
    Char_t  geaLayer;       // layer number (0..8): straw: 0-4
    Int_t   geaCell;        // cell number
    Char_t  geaSubCell;     // sub cell number

    // get all params
    Float_t adc_reso = pStrawDigiPar->getAnalogReso();
    Float_t eloss_slope = pStrawDigiPar->getElossSlope();
    Float_t eloss_offset = pStrawDigiPar->getElossOffset();

    Float_t dtime_reso = pStrawDigiPar->getDriftTimeReso();

    Float_t start_offset = pStrawDigiPar->getStartOffset();
    Float_t start_reso = pStrawDigiPar->getStartReso();

    Float_t threshold = pStrawDigiPar->getThreshold();
    Float_t efficiency = pStrawDigiPar->getEfficiency();

#ifdef VERBOSE_MODE1
int cnt = -1;
printf("<<----------------------------------------------->>\n");
#endif

    std::map<Int_t, Int_t> track_hit_cnt;

    Int_t entries = pGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        HGeantFwDet* ghit = (HGeantFwDet*)pGeantFwDetCat->getObject(i);
        if (ghit)
        {
#ifdef VERBOSE_MODE1
++cnt;
#endif
            ghit->getAddress(geaModule, geaLayer, geaCell, geaSubCell);

            if (geaModule > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            // detection efficiency
            Float_t rnd = gRandom->Rndm();
#ifdef VERBOSE_MODE1
printf("(%2d) det eff: rand=%.2f  <?<  eff=%.2f\n", cnt, rnd, efficiency);
#endif
            if (rnd > efficiency)
                continue;

            DigiFields df;
            GeantFields gf;

            df.mod = geaModule;
            df.lay = geaLayer;

            // Straw number calculation
            // Straw and offsets are organized as follow (l: long, s: short)
            //
            // llllllllssssllllllll
            // llllllllssssllllllll
            // llllllllssssllllllll
            // llllllllssssllllllll
            // llllllll    llllllll
            // llllllll    llllllll
            // llllllll    llllllll
            // llllllll    llllllll
            // llllllllssssllllllll
            // llllllllssssllllllll
            // llllllllssssllllllll
            // llllllllssssllllllll
            // |
            // |- offset 0 (o0)
            //         |- offset 1 (o1)
            //             |- offset 2 (o2)
            //                    |- offset 3 (03)
            //
            // If abs(straw_number) < 256: long straws, otherwsie short
            // For long:
            //   if straw_number >= o1: straw_number + o2;
            // For short: abs(straw_number) - 256 + o1

            df.plane = geaCell & 0x1;
            df.straw = geaCell >> 1;
            df.udconf = geaSubCell;

            // straws are rotated in the geo by 90 degrees around x axis
            // y and z must be swap
            ghit->getHit(gf.xHit, gf.zHit, gf.yHit,
                         gf.pxHit, gf.pzHit, gf.pyHit,
                         gf.tofHit, gf.trackLength, gf.eHit);
            gf.trackNumber = ghit->getTrackNumber();

            // in the sim, z-axis is inverted
            gf.pzHit = -gf.pzHit;
            gf.zHit = -gf.zHit;

            Float_t cell_x = pStrawGeomPar->getOffsetX(df.mod, df.lay, df.plane)
                + df.straw*pStrawGeomPar->getStrawPitch(df.mod, df.lay);
            Float_t cell_z = pStrawGeomPar->getOffsetZ(df.mod, df.lay, df.plane);

            Float_t hit_x = cell_x + gf.xHit;
            Float_t hit_y = gf.yHit;
            Float_t hit_z = cell_z + gf.zHit;

            gf.lab_x = hit_x*cosa[df.mod][df.lay] - hit_y*sina[df.mod][df.lay];
            gf.lab_y = hit_x*sina[df.mod][df.lay] + hit_y*cosa[df.mod][df.lay];
            gf.lab_z = hit_z;

            TVector2 v_n(gf.pzHit, gf.pxHit);   // track vector dir.  n
            v_n = v_n/v_n.Mod();
            TVector2 p_a(hit_z, hit_x);         // track hit position a
            TVector2 p_p(cell_z, cell_x);       // wire position      p

            // calculations:
            // dist = | (a-p) - ((a-p)*n)n|
            TVector2 a_p_diff = p_a - p_p;
            TVector2 v_proj_n_diff = (a_p_diff * v_n) * v_n;
            TVector2 v_dist = a_p_diff - v_proj_n_diff;
            df.radius = v_dist.Mod();

            df.time = gf.tofHit + calcDriftTime(df.radius);
            df.adc = gf.eHit*eloss_slope + eloss_offset;

#ifdef VERBOSE_MODE1
printf("     g (m=%d l=%d c=%03d ud=%d, tr=%d) -> p=%d s=%03d  u=%f  el=%.4f tof=%.3f dr=%f  adc=%.2f time=%f  p=%f (%f,%f) loc=(%f,%f,%f)\n", geaModule, geaLayer, geaCell, geaSubCell, gf.trackNumber, df.plane, df.straw, cell_x, gf.eHit, gf.tofHit, df.radius, df.adc, df.time, sqrt(gf.pxHit*gf.pxHit + gf.pyHit*gf.pyHit + gf.pzHit*gf.pzHit), gf.pxHit/gf.pzHit, gf.pyHit/gf.pzHit, gf.lab_x, gf.lab_y, gf.lab_z);
#endif

            if (adc_reso > 0.0)
            {
                df.adc += + gRandom->Gaus() * adc_reso;
                df.adc = TMath::Max(Float_t(0), df.adc);
            }

            if (df.adc < threshold)
                continue;

            // resolution of the ToT in the PASTREC
            if (dtime_reso > 0.0)
            {
                df.time += gRandom->Gaus() * dtime_reso;
                df.time = TMath::Max(Float_t(0), df.time);
            }

            // resolution of the start detector
            if (start_reso > 0.0)
            {
                df.time += gRandom->Gaus() * start_reso;
                df.time = TMath::Max(Float_t(0), df.time);
            }

            df.time += start_offset;

#ifdef VERBOSE_MODE1
printf("     resolution effects                                                             adc=%.2f time=%f\n", df.adc, df.time);
#endif

            df.posU = cell_x;
            df.posZ = cell_z;

            df.hitnr = ++track_hit_cnt[gf.trackNumber];

            Int_t res = fillStrawCalSim(df, gf);
            if (res == 1)
            {
                Error("HFwDetStrawDigitizer::execute()",
                "Can't fill from %d for m=%d l=%d p=%d s=%d ud=%d", geaCell, df.mod, df.lay, df.plane, df.straw, df.udconf);
//                 return kFALSE;
            }
//             if (res == 2)
//             {
//                 Error("HFwDetStrawDigitizer::execute()",
//                 "Overwrite fill from %d for m=%d l=%d p=%d s=%d ud=%d", geaCell, df.mod, df.lay, df.plane, df.straw, df.udconf);
// //                 return kFALSE;
//             }
        }
    }

#ifdef VERBOSE_MODE1
printf("<<- done ---------------------------------------->>\n");
#endif

    return 0;
}

Int_t HFwDetStrawDigitizer::fillStrawCalSim(const DigiFields & df, const GeantFields & gf)
{
    // Function creat and fill object HFwDetStrawCalSim
    // return:
    //  0 - if stored properly
    //  1 - if data was not stored to the category
    //  2 - if multiple hit, and data were not owerwritten
    fLoc[0] = df.mod;
    fLoc[1] = df.lay;
    fLoc[2] = df.plane;
    fLoc[3] = df.straw;
    fLoc[4] = df.udconf;

    Int_t first = 1;

    HFwDetStrawCalSim * cal = (HFwDetStrawCalSim*)pStrawCalCat->getObject(fLoc);
    if (cal != nullptr)    // straw tube ocuppied by another track
    {
        if (cal->getTime() < df.time)
        {
//             cal->setTrack(gf.trackNumber);   // FIXME should we have multiple track numbers?
            return 2;
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
        cal->setAddress(fLoc[0], fLoc[1], fLoc[2], fLoc[3], fLoc[4]);

        cal->setHit(df.time, df.adc, df.posU, df.posZ);
        cal->setDriftRadius(df.radius);

        cal->setToF(gf.tofHit);
        cal->setEloss(gf.eHit);
        cal->setTrack(gf.trackNumber);
        cal->setP(gf.pxHit, gf.pyHit, gf.pzHit);
        cal->setHitPos(gf.lab_x, gf.lab_y, gf.lab_z);

        cal->setHitNumber(df.hitnr);
    }
    else
        return 1;

    return 0;
}

Float_t HFwDetStrawDigitizer::calcDriftTime(Float_t x) const
{
    // Calculate dirft time using simulated drift radius
    Float_t x2 = x * x;
    Float_t x3 = x2 * x;
    Float_t x4 = x3 * x;

    Float_t dt = dt_p[0] + dt_p[1]*x + dt_p[2]*x2 + dt_p[3]*x3 + dt_p[4]*x4;
    return dt;
}
