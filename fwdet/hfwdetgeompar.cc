//*-- AUTHOR : Ilse Koenig
//*-- Created : 16/11/2015

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
// HFwDetGeomPar
//
// Geometry parameter container for the Forward Detector
//////////////////////////////////////////////////////////////////////////////

#include "hfwdetgeompar.h"
#include "hgeomcompositevolume.h"
#include "hpario.h"
#include "hdetpario.h"

ClassImp(HFwDetGeomPar);

HFwDetGeomPar::HFwDetGeomPar(const Char_t* name,const Char_t* title,
                                const Char_t* context) :
                                HDetGeomPar(name, title, context, "FwDet")
{
    // constructor calling the base class constructor with the detector name
}

HFwDetGeomPar::~HFwDetGeomPar()
{
}

Bool_t HFwDetGeomPar::init(HParIo* inp, Int_t* set)
{
    // intitializes the container from an input
    HDetParIo* input=inp->getDetParIo("HFwDetParIo");
    if (input) return (input->init(this,set));
    return kFALSE;
}

Int_t HFwDetGeomPar::write(HParIo* output)
{
    // writes the container to an output
    HDetParIo* out=output->getDetParIo("HFwDetParIo");
    if (out) return out->write(this);
    return -1;
}
