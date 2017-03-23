//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/11/2015

//_HADES_CLASS_DESCRIPTION
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////////
// HFwDetParAsciiFileIo
//
// Class for parameter input/output from/into Ascii file for the Forward detector
//
/////////////////////////////////////////////////////////////////////////////////

#include "hfwdetparasciifileio.h"
#include "hparset.h"
#include "hfwdetgeompar.h"

ClassImp(HFwDetParAsciiFileIo);

HFwDetParAsciiFileIo::HFwDetParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f)
{
    // constructor calls the base class constructor
    fName = "HFwDetParIo";
}

HFwDetParAsciiFileIo::~HFwDetParAsciiFileIo()
{
}

Bool_t HFwDetParAsciiFileIo::init(HParSet* pPar, Int_t* set)
{
    // calls the appropriate read function for the container
    const  Text_t* name=pPar->GetName();
    if (pFile)
    {
        if (0 == strncmp(name,"FwDetGeomPar", strlen("FwDetGeomPar")))
        {
            return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
        }
        Error("init(HParSet*,Int_t*)","Initialization of %s not possible from ASCII file",name);
        return kFALSE;
    }
    Error("init(HParSet*,Int_t*)","No input file open");
    return kFALSE;
}

Int_t HFwDetParAsciiFileIo::write(HParSet* pPar)
{
    // calls the appropriate write function for the container
    if (pFile)
    {
        const  Text_t* name = pPar->GetName();
        if (0 == strncmp(name, "FwDetGeomPar", strlen("FwDetGeomPar")))
        {
            return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
        }
        Error("write(HParSet*)", "%s could not be written to ASCII file",name);
        return -1;
    }
    Error("write(HParSet*)", "No output file open");
    return -1;
}
