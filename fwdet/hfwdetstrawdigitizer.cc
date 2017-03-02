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
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"

#include "TError.h"
#include "TRandom.h"
#include "TVector2.h"

#include <cstring>
using namespace std;

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
    pGeantFwDetCat    = NULL;
    pFwDetStrawCalCat = NULL;
    pStrawDigiPar     = NULL;
    fLoc.setNIndex(4);
    fLoc.set(4,0,0,0,0);
}

Bool_t HFwDetStrawDigitizer::init(void)
{
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
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
    pFwDetStrawCalCat = pFwDet->buildCategory(catFwDetStrawCal);
    if (!pFwDetStrawCalCat)
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
    return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void)
{
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim
    // gErrorIgnoreLevel = kFatal;

    Float_t time_reso = pStrawDigiPar->getTimeReso();
    Float_t eloss_reso = pStrawDigiPar->getElossReso();
    Float_t drift_reso = pStrawDigiPar->getDriftReso();

    Int_t entries = pGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        HGeantFwDet* ghit = (HGeantFwDet*)pGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(geantModule, geantLayer, geantCell);

            Int_t mod = geantModule;
            Int_t layer = geantLayer;
            Int_t plane = -1;
            Int_t cell = -1;

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

            if(mod > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            // straws are rotated in the geo by 90 degrees around x axis
            // y and z must be swap
            ghit->getHit(xHit, zHit,  yHit, pxHit, pzHit, pyHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();

            fLoc[0] = mod;
            fLoc[1] = layer;
            fLoc[2] = plane;
            fLoc[3] = cell;

            Float_t cell_x = pStrawGeomPar->getOffsetX(mod, layer, plane) + cell*pStrawGeomPar->getStrawPitch(mod, layer);
            Float_t cell_z = pStrawGeomPar->getOffsetZ(mod, layer, plane);

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

            if (time_reso > 0.0)
            {
                tof = tof + gRandom->Gaus() * time_reso;
                tof = TMath::Max(Float_t(0), tof);
            }

            if (eloss_reso > 0.0)
            {
                eloss = eloss + gRandom->Gaus() * eloss_reso;
                eloss = TMath::Max(Float_t(0), eloss);
            }

            if (drift_reso > 0.0)
            {
                radius = radius + gRandom->Gaus() * drift_reso;
                radius = TMath::Max(Float_t(0), radius);
                radius = TMath::Min(radius, pStrawGeomPar->getStrawRadius(mod, layer));
            }

            Bool_t res =  fillStrawCalSim(tof, eloss, radius, cell_x, cell_z, cell);
            if (!res)
            {
                Error("HFwDetStrawDigitizer::execute()",
                "Can't fill from %d for m=%d l=%d p=%d s=%d", geantCell, mod, layer, plane, cell);
//                 return kFALSE;
            }
        }
    }
    return 0;
}

Bool_t HFwDetStrawDigitizer::fillStrawCalSim(Float_t tof, Float_t eloss, Float_t radius, Float_t posX, Float_t posZ, Int_t straw)
{
    // function creat and fill object HFwDetStrawCalSim
    // return kFALSE if data was not stored to the category

    Int_t first = 1;

    HFwDetStrawCalSim * cal = (HFwDetStrawCalSim*)pFwDetStrawCalCat->getObject(fLoc);
    if (cal != NULL)
    {                                     // straw tube ocupaed by another track:
        if (cal->getDrift() <= radius)
        {
            cal->setTrack(trackNumber);
            return kFALSE;
        }
        first = 0;
    }
    else
    {
        cal = (HFwDetStrawCalSim*)pFwDetStrawCalCat->getSlot(fLoc);
    }
    if (cal)
    {
        if (first) cal = new(cal) HFwDetStrawCalSim;
        cal->setAddress(fLoc[0], fLoc[1], fLoc[2], fLoc[3]);
//         cal->setHit(tofHit, eHit, radius, posX, posZ, 0);
        cal->setHit(tofHit, eHit, radius, posX, posZ, straw);
        cal->setTrack(trackNumber);
    }
    else
        return kFALSE;

    return kTRUE;
}
