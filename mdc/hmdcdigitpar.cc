//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : J. Markert
////////////////////////////////////////////////////////////////////////////
// HMdcDigitPar
//
// Container class for Mdc digitization parameters
//
////////////////////////////////////////////////////////////////////////////
#include "hmdcdigitpar.h"
#include "hmessagemgr.h"
#include "hparamlist.h"
#include <stdlib.h>

ClassImp(HMdcDigitPar)

HMdcDigitPar::HMdcDigitPar(const Char_t* name,const Char_t* title,
                       const Char_t* context)
    : HParCond(name,title,context)
{
    //
    layEff.Set(144);
    layEffScale.Set(144);
    layEffThickness.Set(144);
    scaleTime1Err.Set(4);
    scaleTime1ErrMIPS.Set(4);
    clear();
}
HMdcDigitPar::~HMdcDigitPar()
{
  // destructor
}
void HMdcDigitPar::clear()
{
    for(Int_t i=0;i<144;i++)
    {
      layEff[i]=-99;
      layEffScale[i]=-99;
      layEffThickness[i]=-99;
    }
    for(Int_t i=0;i<4;i++)
    {
        scaleTime1Err[i]=-99;
        scaleTime1ErrMIPS[i]=-99;
    }
    cellEffScale=-99;
    signalspeed=-99;
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HMdcDigitPar::printParam(void)
{
    // prints the parameters of HMdcDigitPar to the screen.
    SEPERATOR_msg("#",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcDigitPar:");
    INFO_msg(10,HMessageMgr::DET_MDC,"LayerEfficiency:");
    for(Int_t s=0;s<6;s++){
	for(Int_t m=0;m<4;m++){
	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				 "s %i m %i %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
				 s,m,
				 layEff[(s*24)+(m*6)+0],layEff[(s*24)+(m*6)+1],layEff[(s*24)+(m*6)+2],
				 layEff[(s*24)+(m*6)+3],layEff[(s*24)+(m*6)+4],layEff[(s*24)+(m*6)+5]);
	}
    }
    INFO_msg(10,HMessageMgr::DET_MDC,"LayerEfficiencyScale:");
    for(Int_t s=0;s<6;s++){
	for(Int_t m=0;m<4;m++){
	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				 "s %i m %i %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
				 s,m,
				 layEffScale[(s*24)+(m*6)+0],layEffScale[(s*24)+(m*6)+1],layEffScale[(s*24)+(m*6)+2],
				 layEffScale[(s*24)+(m*6)+3],layEffScale[(s*24)+(m*6)+4],layEffScale[(s*24)+(m*6)+5]);
	}
    }
    INFO_msg(10,HMessageMgr::DET_MDC,"LayerEfficiencyThickness:");
    for(Int_t s=0;s<6;s++){
	for(Int_t m=0;m<4;m++){
	  gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				 "s %i m %i %5.4f %5.4f %5.4f %5.4f %5.4f %5.4f",
				 s,m,
				 layEffThickness[(s*24)+(m*6)+0],layEffThickness[(s*24)+(m*6)+1],layEffThickness[(s*24)+(m*6)+2],
				 layEffThickness[(s*24)+(m*6)+3],layEffThickness[(s*24)+(m*6)+4],layEffThickness[(s*24)+(m*6)+5]);
	}
    }
    INFO_msg(10,HMessageMgr::DET_MDC,"Time1ErrScale:");
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
			   "%5.4f %5.4f %5.4f %5.4f", scaleTime1Err[0],scaleTime1Err[1],scaleTime1Err[2],scaleTime1Err[3]);

    INFO_msg(10,HMessageMgr::DET_MDC,"Time1ErrScaleMIPS:");
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
			   "%5.4f %5.4f %5.4f %5.4f", scaleTime1ErrMIPS[0],scaleTime1ErrMIPS[1],scaleTime1ErrMIPS[2],scaleTime1ErrMIPS[3]);

    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"cellEffScale: %6.4f ns/mm",cellEffScale);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"SignalSpeed: %6.4f ns/mm",signalspeed);
    SEPERATOR_msg("#",60);
}
void HMdcDigitPar::putParams(HParamList* l)
{
    // Puts all params of HMdcDigitPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("layEff",     layEff);
    l->add("layEffScale",layEffScale);
    l->add("layEffThickness",layEffThickness);
    l->add("scaleTime1Err",scaleTime1Err);
    l->add("scaleTime1ErrMIPS",scaleTime1ErrMIPS);
    l->add("cellEffScale",cellEffScale);
    l->add("signalspeed",signalspeed);

}
Bool_t HMdcDigitPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("layEff",     &layEff)))       return kFALSE;
    if(!( l->fill("layEffScale",&layEffScale)))  return kFALSE;
    if(!( l->fill("layEffThickness",&layEffThickness)))  return kFALSE;
    if(!( l->fill("scaleTime1Err",&scaleTime1Err)))  return kFALSE;
    if(!( l->fill("scaleTime1ErrMIPS",&scaleTime1ErrMIPS)))  return kFALSE;
    if(!( l->fill("cellEffScale",&cellEffScale)))return kFALSE;
    if(!( l->fill("signalspeed",&signalspeed)))  return kFALSE;

    return kTRUE;
}
