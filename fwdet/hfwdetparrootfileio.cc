//_HADES_CLASS_DESCRIPTION
//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/11/2015

/////////////////////////////////////////////////////////////////////////////////
//  HFwDetParRootFileIo
//
//  Class for parameter input/output from/into ROOT file for the Forward detector
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////////////////////////

#include "hfwdetparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hfwdetdetector.h"
#include "hfwdetgeompar.h"

ClassImp(HFwDetParRootFileIo);

HFwDetParRootFileIo::HFwDetParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f)
{
    // constructor sets the name of the detector I/O "HFwDetParIo"
    fName = "HFwDetParIo";
    HDetector* det = gHades->getSetup()->getDetector("FwDet");
    Int_t n = det->getMaxSectors();
    initModules = new TArrayI(n);
}

HFwDetParRootFileIo::~HFwDetParRootFileIo()
{
    // destructor
    if (initModules)
    {
        delete initModules;
        initModules = 0;
    }
}

Bool_t HFwDetParRootFileIo::init(HParSet* pPar, Int_t* set)
{
    // initializes a container called by name, but only the modules
    // defined in the array 'set'
    // calls the special read function for this container
    // If it is called the first time it reads the setup found in the file
    if (!isActiv) readModules("FwDet");
    const Text_t* name=pPar->GetName();
    if (pFile)
    {
        if (0 == strncmp(name,"FwDetGeomPar", strlen("FwDetGeomPar")))
        {
            return HDetParRootFileIo::read((HFwDetGeomPar*)pPar, set);
        }
    }
    Error("init(HParSet*,Int_t*)", "Initialization of %s not possible from ASCII file",name);
    return kFALSE;
}
