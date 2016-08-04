//*****************************************************************************
// File: $RCSfile: $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//
//*****************************************************************************
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HMdcUnpackePar
//
// Parameter container for the MDC unapcker.
// This container keeps all parameters that are needed for the unpacker of
// the MDC data.
//
// The most important one is the decodingVersion. This defines the structure
// in the raw data read from the hld file.
// The chamberInfo array tells the unpacker in which chamber to expect data,
// that is malformed and needs special treatment.
// The array excludeBlackListMask, excludeBlacklistAddress, recoverBlackListMask
// and recoverBlackListAddress contain information about the malformed datawords.
// The dataword is masked and the result is compared to the address (testXXX functions).
// The recoveryMethode tells the unpacker which function to call for data recovery,
// once the full subevent has been unpacked.
//
//*****************************************************************************
#include "hmdcunpackerpar.h"
#include "hades.h"
#include "hmessagemgr.h"
#include "hevent.h"
#include "heventheader.h"
#include "hpario.h"
#include "hdetpario.h"
#include "hparamlist.h"

ClassImp(HMdcUnpackerPar)

    HMdcUnpackerPar::HMdcUnpackerPar(const char* name, const char* title, const char* context)
    : HParCond(name,title,context)
{
    // Default constructor
    strcpy(detName,"Mdc");
    decodeVersion = -1;
    tdcMode = -1;
    doAddrCorr = -1;
	mapOutUnusedChannels = -1;
}

HMdcUnpackerPar::~HMdcUnpackerPar(void)
{
    //Destructor
}

Bool_t HMdcUnpackerPar::getParams(HParamList *l)
{
    // Retrive parameters from HParamLis,

    if (!l) return kFALSE;
    if(!(l->fill("decodeVersion", &decodeVersion))) return kFALSE;
    if(!(l->fill("tdcMode",&tdcMode))) return kFALSE;
    if(!(l->fill("doAddrCorr",&doAddrCorr))) return kFALSE;
    if(!(l->fill("doMapOutUnusedChannels",&mapOutUnusedChannels))) return kFALSE;
    return kTRUE;
}

Bool_t HMdcUnpackerPar::init(HParIo* inp,Int_t* set)
{
    // intitializes the container from an input
    HDetParIo* input=inp->getDetParIo("HCondParIo");
    if (input) return (input->init(this,set));
    return kFALSE;
}

void HMdcUnpackerPar::printParam(void)
{
    // Print out all parameters.
    SEPERATOR_msg("#",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcUnpackerPar:");
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"decodeVersion:\t\t%d",decodeVersion);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"tdcMode:\t\t%d",tdcMode);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"addr. corr:\t\t%d",doAddrCorr);
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"map unused:\t\t%d",mapOutUnusedChannels);
    SEPERATOR_msg("#",60);
}

void HMdcUnpackerPar::putParams(HParamList *l)
{
    // Put the parameters to the list.
    if (!l) return;
    l->add("decodeVersion",decodeVersion);
    l->add("tdcMode",tdcMode);
    l->add("doAddrCorr",doAddrCorr);
	l->add("doMapOutUnusedChannels",mapOutUnusedChannels);
}

void HMdcUnpackerPar::setParamContext(const char *context)
{
    // Sets the context (normally done via the constructor)
    paramContext=context;
}

Int_t HMdcUnpackerPar::write(HParIo* output)
{
    // writes the container to an output
    HDetParIo* out=output->getDetParIo("HCondParIo");
    if (out) return out->write(this);
    return -1;
}

Int_t HMdcUnpackerPar::getDecodeVersion(void)
{
    // Return the decoding version to be used for the actual run.
    return decodeVersion;
}

Int_t HMdcUnpackerPar::getTdcMode(void)
{
    // Return the mode the TDC chis wher triggering:
    // The TDC can be operated in 2 different modes:
    // mode 0 (default) : trigger on leading and trailing edge
    // mode 1           : trigger on 2 leading edges:
    return tdcMode;
}

Int_t HMdcUnpackerPar::getDoAddrCorr(void)
{
    // Enable address correction of OEP dataword
    return doAddrCorr;
}

Int_t HMdcUnpackerPar::getMapUnusedChannels(void)
{
  // Return wether mapping out of unused channels should be done
  return mapOutUnusedChannels;
}
