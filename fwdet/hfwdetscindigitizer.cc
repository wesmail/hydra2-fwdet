//*-- Author   : Ilse Koenig
//*-- Created  : 19.02.2016

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////
//  HFwDetScinDigitizer
//
//  This class digitizes the Forward Scintillator detector data
//
//  Produces calibrated time and energy loss for each Scintillator cell
//
///////////////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdetscindigitizer.h"
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
#include "hfwdetscincalsim.h"
#include "hfwdetscindigipar.h"
//#include "TRandom.h"

ClassImp(HFwDetScinDigitizer);

HFwDetScinDigitizer::HFwDetScinDigitizer()
{
    // default constructor
    initVariables();
}

HFwDetScinDigitizer::HFwDetScinDigitizer(const Text_t *name, const Text_t *title) :
    HReconstructor(name, title)
{
    // constructor
    initVariables();
}

HFwDetScinDigitizer::~HFwDetScinDigitizer()
{
}

void HFwDetScinDigitizer::initVariables()
{
    // initialize the variables in constructor
    fGeantFwDetCat   = 0;
    fFwDetScinCalCat = 0;
    fScinDigiPar     = 0;
    fLoc.setNIndex(2);
    fLoc.set(2,0,0);
}

Bool_t HFwDetScinDigitizer::init()
{
    // initializes the task

    // find the Forward detector in the HADES setup
    HFwDetDetector* pFwDet = (HFwDetDetector*)(gHades->getSetup()->getDetector("FwDet"));
    if (!pFwDet)
    {
        Error("FwDetScinDigitizer::init", "No Forward Detector found");
        return kFALSE;
    }

    // GEANT input data
    fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fGeantFwDetCat)
    {
        Error("HFwDetScinDigitizer::init()", "HGeant FwDet input missing");
        return kFALSE;
    }

    // build the Calibration category
    fFwDetScinCalCat=pFwDet->buildCategory(catFwDetScinCal);
    if (!fFwDetScinCalCat)
    {
        Error("HFwDetScinDigitizer::init()", "Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    fScinDigiPar = (HFwDetScinDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetScinDigiPar");
    if (!fScinDigiPar)
    {
        Error("HFwDetScinDigitizer::init()", "Parameter container for digitizer not created");
        return kFALSE;
    }

    return kTRUE;
}

Int_t HFwDetScinDigitizer::execute()
{
    // Digitization of GEANT hits and storage in HFwDetScinCalSim

    HGeantFwDet* ghit = 0;
    HFwDetScinCalSim* cal = 0;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(Int_t i = 0; i < entries; ++i)
    {
        ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
        if (ghit)
        {
            Char_t  geantModule;  // GEANT FwDet module number (0...8)
            Char_t  geantLayer;   // GEANT FwDet layer number (0...8)
            Int_t   geantCell;    // GEANT cell number inside module (0...8)
            Char_t  geantSubCell;

            ghit->getAddress(geantModule, geantLayer, geantCell, geantSubCell);
            if(geantModule < 4 || geantModule > 5) continue; // skip the other detectors of the FwDet

            DigiFields df;
            GeantFields gf;

            ghit->getHit(gf.xHit, gf.yHit,  gf.zHit,
                         gf.pxHit, gf.pyHit, gf.pxHit,
                         gf.tofHit, gf.trackLength, gf.eHit);
            gf.trackNumber = ghit->getTrackNumber();

            // calculate scintillator cell number and set location indexes
            df.mod = geantModule - 4;
            df.cell = geantCell;  // preliminary!!!!!!!!!

            if (df.mod >= FWDET_SCIN_MAX_MODULES || df.cell >= FWDET_SCIN_MAX_CELLS)
                continue;

            fLoc[0] = df.mod;
            fLoc[1] = df.cell;

            // do something with the Geant hit...
            //
            // fill the cal hit:
            // check if the object is already existing in the cal category

            cal = (HFwDetScinCalSim*)fFwDetScinCalCat->getSlot(fLoc);
            if (cal)
            {
                cal = new(cal) HFwDetScinCalSim;
                cal->setAddress(df.mod, 0, df.cell);
                cal->setHit(gf.tofHit, gf.eHit);
                cal->setTrack(gf.trackNumber);
            }
        }
    }

    return 0;
}
