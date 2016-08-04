//*-- AUTHOR : Ilse Koenig
//*-- Modified last : 09/05/2002 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HMdcContFact
//
//  Factory for the parameter containers in libMdc
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hmdccontfact.h"
#include "hruntimedb.h"
#include "hmdccal2par.h"
#include "hmdccal2parsim.h"
#include "hmdccalparraw.h"
#include "hmdctdcthreshold.h"
#include "hmdctdcchannel.h"
#include "hmdccelleff.h"
#include "hmdcwirestat.h"
#include "hmdcdigitpar.h"
#include "hmdcgeomstruct.h"
#include "hmdcgeompar.h"
#include "hmdclookupgeom.h"
#include "hmdclookupraw.h"
#include "hmdcrawstruct.h"
#include "hmdctimecut.h"
#include "hmdcsetup.h"
#include "hmdclayergeompar.h"
#include "hmdcbitflipcorpar.h"
#include "hmdcunpackerpar.h"
#include "hmdcoepaddrcorrpar.h"

#include <iostream>
#include <iomanip>

ClassImp(HMdcContFact)

static HMdcContFact gHMdcContFact;

HMdcContFact::HMdcContFact() {
  // Constructor (called when the library is loaded)
  fName="MdcContFact";
  fTitle="Factory for parameter containers in libMdc";
  setAllContainers();
  HRuntimeDb::instance()->addContFactory(this);
}

void HMdcContFact::setAllContainers() {
  // Creates the Container objects with all accepted contexts and adds them to
  // the list of containers for the MDC library.
    HContainer* cal2par= new HContainer("MdcCal2Par",
                   "cal2 calibration parameters for Mdc [time->distance]","MdcCal2ParSimProduction");
    //cal2par->addContext("");
    containers->Add(cal2par);

    HContainer* cal2parsim=new HContainer("MdcCal2ParSim",
                   "cal2 calibration parameters for Mdc [distance->time]","MdcCal2ParSimProduction");
    containers->Add(cal2parsim);

    HContainer* calparraw=new HContainer("MdcCalParRaw",
                   "raw calibration parameters for Mdc",
                   "MdcCalParRawProduction");
    containers->Add(calparraw);

    HContainer* tdcthreshold=new HContainer("MdcTdcThreshold",
                   "threshold parameters for Mdc",
                   "MdcTdcThresholdProduction");
    containers->Add(tdcthreshold);

    HContainer* tdcchannelmask=new HContainer("MdcTdcChannel",
                   "TDC channel masks for Mdc",
                   "MdcTdcChannelProduction");
    containers->Add(tdcchannelmask);

    HContainer* celleff= new HContainer("MdcCellEff",
                   "cell efficiency parameters for MdcDigitizer",
                   "MdcCellEffProduction");
    containers->Add(celleff);

    HContainer* wirestat= new HContainer("MdcWireStat",
                   "wire status parameters for MdcDigitizer",
                   "MdcWireStatProduction");
    containers->Add(wirestat);

    HContainer* digitpar=new HContainer("MdcDigitPar",
                   "digitisation parameters for Mdc","MdcDigitParProduction");
    containers->Add(digitpar);

    HContainer* geompar=new HContainer("MdcGeomPar",
                   "Mdc geometry parameters","GeomProduction");
    containers->Add(geompar);

    HContainer* geomstruct=new HContainer("MdcGeomStruct",
                   "Mdc parameters for geometry structure","");
    containers->Add(geomstruct);

    HContainer* layergeompar=new HContainer("MdcLayerGeomPar",
                   "geometry parameters for Mdc layers","MdcLayerGeomParProduction");
    containers->Add(layergeompar);

    HContainer* lookupgeom=new HContainer("MdcLookupGeom",
                   "Mdc lookup table from raw to cal1","");
    containers->Add(lookupgeom);

    HContainer* lookupraw=new HContainer("MdcLookupRaw",
                   "Mdc lookup table from cal1 to raw","");
    containers->Add(lookupraw);

    HContainer* rawstruct= new HContainer("MdcRawStruct",
                   "Mdc parameters for hardware structure","");
    containers->Add(rawstruct);

   HContainer* timecut= new HContainer("MdcTimeCut",
                   "cut on time1, time2 & time2-time1","MdcTimeCutProduction");
    timecut->addContext("MdcTimeCutProductionSim");
    containers->Add(timecut);

    HContainer* setup=new HContainer("MdcSetup",
                   "Setup parameters for HMdcTaskSet","MdcSetupProduction");
    containers->Add(setup);

    HContainer* bitflipcorpar=new HContainer("MdcBitFlipCorPar",
                   "Setup parameters for HMdcBitFlipCorPar","MdcBitFlipCorParProduction");
    containers->Add(bitflipcorpar);

    HContainer* unpackerpar = new HContainer("MdcUnpackerPar","Mdc unpacker lookuptable for data recovery","MdcUnpackerParProduction");
    containers->Add(unpackerpar);

    HContainer* oepAddrCorrPar = new HContainer("MdcOepAddrCorrPar","MDC OEP address correction lookup table","MdcOepAddrCorrParProduction");
    containers->Add(oepAddrCorrPar);
}

HParSet* HMdcContFact::createContainer(HContainer* c) {
  // Calls the constructor of the corresponding parameter container.
  // For an actual context, which is not an empty string and not the default context
  // of this container, the name is concatinated with the context.
  const Char_t* name=c->GetName();
  if (strcmp(name,"MdcCal2Par")==0)
    return new HMdcCal2Par(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcCal2ParSim")==0)
    return new HMdcCal2ParSim(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcCalParRaw")==0)
    return new HMdcCalParRaw(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcTdcThreshold")==0)
    return new HMdcTdcThreshold(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcTdcChannel")==0)
    return new HMdcTdcChannel(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcCellEff")==0)
    return new HMdcCellEff(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcWireStat")==0)
    return new HMdcWireStat(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcDigitPar")==0)
    return new HMdcDigitPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcGeomPar")==0)
    return new HMdcGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcGeomStruct")==0)
    return new HMdcGeomStruct(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcLayerGeomPar")==0)
    return new HMdcLayerGeomPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcLookupGeom")==0)
    return new HMdcLookupGeom(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcLookupRaw")==0)
    return new HMdcLookupRaw(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcRawStruct")==0)
    return new HMdcRawStruct(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcTimeCut")==0)
    return new HMdcTimeCut(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcSetup")==0)
    return new HMdcSetup(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if (strcmp(name,"MdcBitFlipCorPar")==0)
    return new HMdcBitFlipCorPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  if(strcmp(name,"MdcUnpackerPar")==0)
    return new HMdcUnpackerPar("MdcUnpackerPar","Mdc unpacker lookuptable for data recovery","MdcUnpackerParProduction");
  if(strcmp(name,"MdcOepAddrCorrPar")==0)
    return new HMdcOepAddrCorrPar(c->getConcatName().Data(),c->GetTitle(),c->getContext());
  return 0;
}

