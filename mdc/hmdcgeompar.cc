//*-- AUTHOR : Ilse Koenig
//*-- Modified : 17/01/2002 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HMdcGeomPar
//
// Container class for the basic MDC geometry parameters
//
//////////////////////////////////////////////////////////////////////////////

#include "hmdcgeompar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hgeomcompositevolume.h"

ClassImp(HMdcGeomPar)

HMdcGeomPar::HMdcGeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Mdc") {
  // constructor calling the base class constructor with the detector name
}

Bool_t HMdcGeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  Bool_t rc=kFALSE;
  HDetParIo* input=inp->getDetParIo("HMdcParIo");
  if (input) {
    rc= (input->init(this,set));
    if (rc&&changed) {
      shiftLayerTransToCenter();
    }
  }
  return rc;
}

void HMdcGeomPar::shiftLayerTransToCenter() {
  // Shifts the points to be symmetric in z.
  // The translation vector is changed accordingly to place the intrisic coodinate
  // system in the center of the volume
  for (Int_t m=0;m<refVolumes->GetSize();m++) {
    HGeomCompositeVolume* refMod=(HGeomCompositeVolume*)(refVolumes->At(m));
    if (refMod) {
      for(Int_t c=0;c<refMod->getNumComponents();c++) {
        HGeomVolume* vol=refMod->getComponent(c);
        if (vol) {
          Double_t zShift=(vol->getPoint(4)->getZ()+vol->getPoint(0)->getZ())/2.;
          zShift=round(zShift*1000.)/1000.;
          if (fabs(zShift)>=0.001) {
            for(Int_t k=0;k<vol->getNumPoints();k++) {
              HGeomVector* point=vol->getPoint(k);
              point->setZ(point->getZ()-zShift);
            }
            HGeomTransform& trans=vol->getTransform();
            HGeomVector pos=trans.getTransVector();
            pos.setZ(pos.getZ()+zShift);
            trans.setTransVector(pos);
          }
        }
      } 
    }
  }
}

Int_t HMdcGeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HMdcParIo");
  if (out) return out->write(this);
  return -1;
}
