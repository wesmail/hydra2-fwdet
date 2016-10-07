//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetStrawDigitizer
//
//  This class digitizes the Forward Straw detector data
//
//  Produce calibrated Time and energy loss and the straw number
//
/////////////////////////////////////////////////////////////

using namespace std;

#include <cstring>

using namespace std;

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
#include "TError.h"


const double rot_mat[2][2] = {
    { cos(M_PI / 4.), -sin(M_PI / 4.) },
    { sin(M_PI / 4.), cos(M_PI / 4.) }
};

// dx..dz - track direction vector
// mod_xref -- reference coordinate in x/y
// mod_yref -- reference coordinate in z
// straw_diam -- diameter of the straw
// straw_n -- number of fired straw
// xh..xz -- geant hit coordinate
// Mu -- unitary momentum vector
// double DIGI(int dx, int dy, int dz, float mod_xref, float mod_zref, float straw_diam, double xh, double yh, double zh, int straw_n, TVector3 Mu);

enum CELL_DIR { deg0, deg90, deg45m, deg45p };

uint find_straw(double xh, double yh, double zh, float mod_xref, float straw_diam, CELL_DIR cd);
double find_radius(int dx, int dy, int dz, float mod_xref, float mod_zref, float straw_diam, double xh, double yh, double zh, int straw_n, const TVector3 & Mu);

int fill_straw_hit(HCategory* fFwDetStrawCalCat, HLocation & fLoc, float x_ref, float z_ref, float straw_diam, int straw, int straw_ref, float x, float y, float z, const TVector3 & mom, float tofHit, float eHit, int track);

ClassImp(HFwDetStrawDigitizer);

const size_t HFwDetStrawDigitizer::nstraws_Tx[HFwDetStrawDigitizer::nstations] =
{
    160, // number of straws in station 1
    226  // number of straws in module 2
};
const Float_t HFwDetStrawDigitizer::Tx_x_a[HFwDetStrawDigitizer::nstations] =
{
    -396.15, // x coord. of the begining of the 1st cell in module 1 A1SV1)
    -558.025 // the same like above, but for module 2 A2SV1
};
const Float_t HFwDetStrawDigitizer::Tx_x_b[HFwDetStrawDigitizer::nstations] =
{
    -401.19, // x-coord. of the begining of the 2nd cell in module 1 A1SV1)
    -563.075
};

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
    fGeantFwDetCat    = 0;
    fFwDetStrawCalCat = 0;
    fStrawDigiPar     = 0;
    fLoc.setNIndex(2);
    fLoc.set(2,0,0);
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
    fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fGeantFwDetCat)
    {
        Error("HFwDetStrawDigitizer::init()","HGeant FwDet input missing");
        return kFALSE;
    }

    // build the Calibration category
    fFwDetStrawCalCat=pFwDet->buildCategory(catFwDetStrawCal);
    if (!fFwDetStrawCalCat)
    {
        Error("HFwDetStrawDigitizer::init()","Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    fStrawDigiPar = (HFwDetStrawDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetStrawDigiPar");
    if (!fStrawDigiPar)
    {
        Error("HFwDetStrawDigitizer::init()","Parameter container for digitizer not created");
        return kFALSE;
    }

    return kTRUE;
}

Int_t HFwDetStrawDigitizer::execute(void)
{
    // Digitization of GEANT hits and storage in HFwDetStrawCalSim
    gErrorIgnoreLevel = kFatal;

    HGeantFwDet* ghit = 0;

    TVector3 Mom, Mom_unit;

    int mod = 0;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(module, cell);
            mod = (int)module;

            if(mod > FWDET_STRAW_MAX_MODULES)
                continue; // skip the other detectors of the FwDet

            ghit->getHit(xHit, yHit,  zHit, pxHit, pyHit, pzHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();

            Mom.SetXYZ(pxHit, pyHit, pzHit);
            Mom_unit = Mom.Unit();

            float x_rot = xHit;
            float y_rot = yHit;

            // find the reference straw number
            uint fstraw_a = find_straw(x_rot, 0, 0, Tx_x_a[mod], straw_diam, deg0);
            uint fstraw_b = find_straw(x_rot, 0, 0, Tx_x_b[mod], straw_diam, deg0);

            fLoc[0] = module;
            fLoc[1] = cell;

            // fill straw for subcell a for a reference straw
            fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_a[mod], T12_z_a,
                           straw_diam, fstraw_a, fstraw_a,
                           x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);

            // if momentum vector is not 0, check also neighbour straws
            if (Mom_unit.Mag() != 0)
            {
                // to the left from reference
                for (int i = fstraw_a-1; i >= 0; --i)
                {
                    int res = fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_a[mod], T12_z_a,
                                straw_diam, i, fstraw_a,
                                x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);
                    if (res == -1)
                        break;
                }
                // to the right from reference
                for (int i = fstraw_a+1; i < (int)nstraws_Tx[mod]; ++i)
                {
                    int res = fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_a[mod], T12_z_a,
                                straw_diam, i, fstraw_a,
                                x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);
                    if (res == -1)
                        break;
                }
            }

            // same like above
            fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_b[mod], T12_z_b,
                           straw_diam, fstraw_a, fstraw_a,
                           x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);

            if (Mom_unit.Mag() != 0)
            {
                for (int i = fstraw_b-1; i >= 0; --i)
                {
                    int res = fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_b[mod], T12_z_b,
                                straw_diam, i, fstraw_a,
                                x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);
                    if (res == -1)
                        break;
                }
                for (int i = fstraw_b+1; i < (int)nstraws_Tx[mod]; ++i)
                {
                    int res = fill_straw_hit(fFwDetStrawCalCat, fLoc, Tx_x_b[mod], T12_z_b,
                                straw_diam, i, fstraw_a,
                                x_rot, y_rot, zHit, Mom_unit, tofHit, eHit, trackNumber);
                    if (res == -1)
                        break;
                }
            }
        }
    }
    return 0;
}

uint find_straw(double xh, double yh, double zh, float mod_xref, float straw_diam, CELL_DIR cd)
{
    // Find straw number by calculating the distance from the edge
    // of the detector, and dividing by the straw radius
    float r = 0.0;
    switch (cd)
    {
        case deg0:
            r = xh;
            break;
        case deg90:
            r = yh;
            break;
        case deg45m:
        case deg45p:
            r = sqrt(xh*xh + yh*yh);
            break;
    }

    float dist_ = (r - mod_xref);
    float found_cell_f = dist_/straw_diam;

    return (int)round(found_cell_f);
}

double find_radius(int dx, int dy, int dz, float mod_xref, float mod_zref, float straw_diam, double xh, double yh, double zh, int straw_n, const TVector3 & Mu)
{
    // Calculate distance between straw wire with directions (cos) dx,dy,dz
    // and coordinates 2 coordinates in straw plane (thrid coordinate is 0)
    // and 3rd line in space crossing sensitive volume at xh, yh, zh and
    // directions given by Mu vctor

    float dX = -xh;
    float dY = -yh;

    if (dx)
    {
        dY += mod_xref + straw_diam*straw_n;
    }

    if (dy)
    {
        dX += mod_xref + straw_diam*straw_n;
    }

    TMatrixD D(3,3), D1(2,2), D2(2,2), D3(2,2);

    D(0,0) = dX;
    D(0,1) = dY;
    D(0,2) = mod_zref + straw_diam/2. - zh;
    D(1,0) = Mu.X();
    D(1,1) = Mu.Y();
    D(1,2) = Mu.Z();
    D(2,0) = dx;  // 0
    D(2,1) = dy;  // 1
    D(2,2) = dz;  // 0

    D1(0,0) = Mu.Y();
    D1(0,1) = Mu.Z();
    D1(1,0) = dy; //1
    D1(1,1) = dz; //0

    D2(0,0) = Mu.Z();
    D2(0,1) = Mu.X();
    D2(1,0) = dz; //0
    D2(1,1) = dx; //0

    D3(0,0) = Mu.X();
    D3(0,1) = Mu.Y();
    D3(1,0) = dx; //0
    D3(1,1) = dy; //1

    float r = D.Determinant() / TMath::Sqrt(
        D1.Determinant()*D1.Determinant()+D2.Determinant()*D2.Determinant()+D3.Determinant()*D3.Determinant());
    return TMath::Abs(r);
}

int fill_straw_hit(HCategory* fFwDetStrawCalCat, HLocation & fLoc, float x_ref, float z_ref, float straw_diam, int straw, int straw_ref, float x, float y, float z, const TVector3 & mom, float tofHit, float eHit, int track)
{
    int ret = 0;

    float posZ = z_ref + straw_diam/2.;
    float posX = x_ref + straw_diam*straw;

    float radius = straw_diam/2.;

    if (mom.Mag() != 0)
        radius = find_radius(0, 1, 0, x_ref, z_ref, straw_diam, x, y, z, straw, mom);

    if (radius > straw_diam/2.)
        return -1;

    HFwDetStrawCalSim * cal = (HFwDetStrawCalSim*)fFwDetStrawCalCat->getSlot(fLoc);
    if (cal)
    {
        cal = new(cal) HFwDetStrawCalSim;
        cal->setAddress(fLoc[0], fLoc[1], straw); // FIXME straw here and below
        cal->setHit(tofHit, eHit, radius, posX, posZ, straw - straw_ref);
        cal->setTrack(track);

        ret = 1;
    }

    return ret;
}
