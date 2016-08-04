#include "htofcalibrater.h"

#include "hcategory.h"
#include "hlocation.h"
#include  "htofraw.h"
#include "htofcal.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "htofcalpar.h"
#include "tofdef.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////
//HTofCalibrater  (not operational !!!)
//
// adapted from /u/halo/packages/abase/new by Manuel Sanchez (17/06/98)
////////////////////////////////////////////////////////////////////////

ClassImp(HTofCalibrater)

void HTofCalibrater::initParContainer() {
  fCalPar=(HTofCalPar *)gHades->getRuntimeDb()->getContainer("TofCalPar");
}

Bool_t HTofCalibrater::init(void) {
  fRawCat=gHades->getCurrentEvent()->getCategory(catTofRaw);
  if (!fRawCat) {
    fRawCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofRaw);
    if (!fRawCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofRaw,fRawCat,"Tof");
  }
  fCalCat=gHades->getCurrentEvent()->getCategory(catTofCal);
  if (!fCalCat) {
    fCalCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofCal);
    if (!fCalCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catTofCal,fCalCat,"Tof");
  }
  fIter=(HIterator *)fRawCat->MakeIterator();
  fLoc.set(3,0,0,0);
  fActive=kTRUE;
  initParContainer();
  return kTRUE;
}

HTofCalibrater::HTofCalibrater(void) {
  fCalPar=NULL;
  fRawCat=fCalCat=NULL;
  fIter=NULL;
}

HTofCalibrater::HTofCalibrater(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
  fCalPar=NULL;
  fRawCat=fCalCat=NULL;
  fIter=NULL;  
}


HTofCalibrater::~HTofCalibrater(void) {
  if (fCalPar) delete fCalPar;
}

HTofCalibrater &HTofCalibrater::operator=(HTofCalibrater &c) {
  fCalPar=c.fCalPar;
  fRawCat=c.fRawCat;
  fCalCat=c.fCalCat;
  return c;
}

Int_t HTofCalibrater::execute(void) {  // not operational !!
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("TofCalibrater::execute");
  gDebuger->message("Raw cat points to %p",fRawCat);
#endif
  /*
  HTofRaw *raw = NULL;
  HTofCal *cal = NULL;
  HTofCalParCell *cell = NULL;
  fIter->Reset();
  while ( (raw=(HTofRaw *)fIter->Next()) != NULL) {
    fLoc[0] = raw->getSector();
    fLoc[1] = raw->getModule();
    fLoc[2] = raw->getCell();
    cal=(HTofCal *)fCalCat->getSlot(fLoc);
    if (cal != NULL) {
      cal = new(cal) HTofCal;
      cal->setNHit(raw->getNHit());
      if (raw->getNHit()>0) {
	cell=fCalPar->getCell(fLoc);
	cal->setTimeRight(raw->getRightTime()*
			  cell->getSlopeTimeRight()+
			  cell->getOffsetTimeRight());
	cal->setChargeRight(raw->getRightCharge()*
			    cell->getSlopeChargeRight()+
			    cell->getOffsetChargeRight());
	if (raw->getNHit()>1) {
	  cal->setTimeLeft(raw->getLeftTime()*
			   cell->getSlopeTimeLeft()+
			   cell->getOffsetTimeLeft());
	  cal->setChargeLeft(raw->getLeftCharge()*
			     cell->getSlopeChargeLeft()+
			     cell->getOffsetChargeLeft());
	  cal->setSector(raw->getSector());
	  cal->setModule(raw->getModule());
	  cal->setCell(raw->getCell());
	}
      }
    }
  }
#if DEBUG_LEVEL>2 
  gDebuger->leaveFunc("HTofCalibrater::execute");
#endif
  */
  return 0;
}
      

