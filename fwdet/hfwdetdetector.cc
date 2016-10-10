//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/11/2015

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HFwDetDetector
//
//  The detector class of the Forward Detector
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hfwdetdetector.h"
#include "fwdetdef.h"
#include "hcategory.h"
#include "hlinearcategory.h"
#include "hmatrixcategory.h"
#include "hgeantmaxtrk.h"
#include "hades.h"
#include "hevent.h"
#include "hpario.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"
#include "hdetpario.h"
#include "hfwdetparrootfileio.h"
#include "hfwdetparasciifileio.h"
#include "TClass.h"
#include <iostream> 
#include <iomanip>

ClassImp(HFwDetDetector); // FwDet detector class

HFwDetDetector::HFwDetDetector() : HDetector("FwDet", "The FwDet detector")
{
    // constructor
    fName = "FwDet";
    maxSectors    = FWDET_MAX_SECTORS;
    maxModules    = FWDET_MAX_MODULES;
    maxComponents = FWDET_MAX_COMPONENTS; // needed for geometry parameter container
    modules       = new TArrayI(maxModules);
}

HFwDetDetector::~HFwDetDetector()
{
    delete modules;
    modules = 0;
}

void HFwDetDetector::activateParIo(HParIo* io)
{
    // activates the input/output class for the FwDet parameters
    // no I/O from Oracle for the preliminary detector(s)
    if (strcmp(io->IsA()->GetName(),"HParRootFileIo") == 0)
    {
        HFwDetParRootFileIo* p=new HFwDetParRootFileIo(((HParRootFileIo*)io)->getParRootFile());
        io->setDetParIo(p);
    }
    if (strcmp(io->IsA()->GetName(),"HParAsciiFileIo") == 0)
    {
        HFwDetParAsciiFileIo* p=new HFwDetParAsciiFileIo(((HParAsciiFileIo*)io)->getFile());
        io->setDetParIo(p);
    }
}

Bool_t HFwDetDetector::write(HParIo* output)
{
    // writes the FwDet setup to output
    HDetParIo* out = output->getDetParIo("HFwDetParIo");
    if (out) return out->write(this);
    return kFALSE;
}

HCategory* HFwDetDetector::buildMatrixCategory(const Text_t* className, Int_t maxMod,
                                               Int_t maxCell, Float_t fillRate)
{
    Int_t* sizes = new Int_t[2]; // 2 levels Module - Cell
    sizes[0] = maxMod;
    sizes[1] = maxCell; 
    HMatrixCategory* category = new HMatrixCategory(className, 2, sizes, fillRate);
    delete [] sizes;
    return category;
}

HCategory* HFwDetDetector::buildMatrixCategory(const Text_t* className, Int_t maxMod,
                                               Int_t maxDbLay, Int_t nLays, Int_t maxCell, Float_t fillRate)
{
    Int_t* sizes = new Int_t[4]; // 4 levels Module - DoubleLayer - Layer - Cell(straw tube)
    sizes[0] = maxMod;
    sizes[1] = maxDbLay; 
    sizes[2] = nLays;
    sizes[3] = maxCell;
    HMatrixCategory* category = new HMatrixCategory(className, 4, sizes, fillRate);
    delete [] sizes;
    return category;
}

HCategory* HFwDetDetector::buildLinearCategory(const Text_t* className, Int_t size)
{
    if (size > 0)
    {
        HLinearCategory* category = new HLinearCategory(className, size);
        return category;
    }
    return 0;
}

HCategory* HFwDetDetector::buildCategory(Cat_t cat)
{
    // gets the category if existing
    // builts and adds if not existing
    // returns the pointer to the category or zero
    HCategory *pcat;
    pcat = gHades->getCurrentEvent()->getCategory(cat);
    if (pcat) return (pcat);  // already existing
    Int_t maxModInSetup = getMaxModInSetup();
    if (maxModInSetup == 0) return 0;

    switch (cat)
    {
        case catFwDetStrawCal:
            pcat = buildMatrixCategory("HFwDetStrawCalSim", FWDET_STRAW_MAX_MODULES, FWDET_STRAW_MAX_DOUBLELAYS, 2, FWDET_STRAW_MAX_CELLS); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
            break;
        case catFwDetScinCal:
            if (maxModInSetup >= 4)
            {
                pcat = buildMatrixCategory("HFwDetScinCalSim", FWDET_SCIN_MAX_MODULES, FWDET_SCIN_MAX_CELLS);
            }
            break;
        case catFwDetRpcCal:
            if (maxModInSetup >= 6)
            {
                pcat = buildMatrixCategory("HFwDetRpcCalSim", FWDET_RPC_MAX_MODULES, FWDET_RPC_MAX_CELLS);
            }
            break;
    default:
        return NULL;
    }

    if (pcat) gHades->getCurrentEvent()->addCategory(cat, pcat, "FwDet");

    return pcat;
}

Int_t HFwDetDetector::getMaxModInSetup()
{
    Int_t maxMod = -1;
    for (Int_t i = 0; i < maxModules; i++)
    {
        if (modules->At(i)) maxMod = (i > maxMod) ? i : maxMod;
    }

    ++maxMod;
    return maxMod;
}
