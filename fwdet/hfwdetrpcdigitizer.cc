//*-- Author  : Georgy Kornakov
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
//#include "TRandom.h"

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
    fGeantFwDetCat  = 0;
    fFwDetRpcCalCat = 0;
    fRpcDigiPar     = 0;
    fLoc.setNIndex(2);
    fLoc.set(2,0,0);
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
    fGeantFwDetCat = gHades->getCurrentEvent()->getCategory(catFwDetGeantRaw);
    if (!fGeantFwDetCat)
    {
        Error("HFwDetRpcDigitizer::init()", "HGeant FwDet input missing");
        return kFALSE;
    }

    // build the Calibration category
    fFwDetRpcCalCat=pFwDet->buildCategory(catFwDetRpcCal);
    if (!fFwDetRpcCalCat)
    {
        Error("HFwDetRpcDigitizer::init()", "Cal category not created");
        return kFALSE;
    }

    // create the parameter container
    fRpcDigiPar = (HFwDetRpcDigiPar *)gHades->getRuntimeDb()->getContainer("FwDetRpcDigiPar");
    if (!fRpcDigiPar)
    {
        Error("HFwDetRpcDigitizer::init()", "Parameter container for digitizer not created");
        return kFALSE;
    }

    return kTRUE;
}

Int_t HFwDetRpcDigitizer::execute()
{
    // Digitization of GEANT hits and storage in HFwDetRpcCalSim

    HGeantFwDet* ghit = 0;
    HFwDetRpcCalSim* cal = 0;

    Int_t entries = fGeantFwDetCat->getEntries();
    for(int i = 0; i < entries; ++i)
    {
        ghit = (HGeantFwDet*)fGeantFwDetCat->getObject(i);
        if (ghit)
        {
            ghit->getAddress(geantModule, geantCell);
            if(geantModule < 6)
                continue; // skip the other detectors of the FwDet

            ghit->getHit(xHit, yHit,  zHit, pxHit, pyHit, pxHit, tofHit, trackLength, eHit);
            trackNumber = ghit->getTrackNumber();

            // calculate rpc cell number and set location indexes
            module = geantModule - 6;
            rpcNum = geantCell;  // preliminary!!!!!!!!!
            if (module >= FWDET_RPC_MAX_MODULES || rpcNum >= FWDET_RPC_MAX_CELLS)
                continue;

            fLoc[0] = module;
            fLoc[1] = rpcNum;
            // do something with the Geant hit...
            //
            // fill the cal hit:
            // check if the object is already existing in the cal category

            cal = (HFwDetRpcCalSim*)fFwDetRpcCalCat->getSlot(fLoc);
            if (cal)
            {
                cal = new(cal) HFwDetRpcCalSim;
                cal->setAddress(module, geantCell, rpcNum);
                cal->setHit(tofHit, eHit);
                cal->setTrack(trackNumber);
            }
        }
    }

    return 0;
}
