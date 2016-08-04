//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////
//*-- AUTHOR : Y.C. Pachmayer
////////////////////////////////////////////////////////////////////////////
// HMdcBitFlipCorPar
//
// Container class for Mdc mbo bitflip correction parameters
//
////////////////////////////////////////////////////////////////////////////
#include "hmdcbitflipcorpar.h"
#include "hmessagemgr.h"
#include "hparamlist.h"
#include <stdlib.h>

ClassImp(HMdcBitFlipCorPar)

  HMdcBitFlipCorPar::HMdcBitFlipCorPar(const Char_t* name,const Char_t* title, 
				       const Char_t* context)
    : HParCond(name,title,context)
{
    //
    bitshiftcorfactor.Set(3456);
    clear();
}

HMdcBitFlipCorPar::~HMdcBitFlipCorPar()
{
  // destructor
}
void HMdcBitFlipCorPar::clear()
{
    for(Int_t i=0;i<3456;i++)
    {
      bitshiftcorfactor[i]=-99;
    }
    status=kFALSE;
    resetInputVersions();
    changed=kFALSE;
}
void HMdcBitFlipCorPar::printParam(void)
{
    // prints the parameters of HMdcBitFlipCorPar to the screen.
    SEPERATOR_msg("#",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcBitFlipCorPar:");
    INFO_msg(10,HMessageMgr::DET_MDC,"BitFlipCor:");
    for(Int_t s=0;s<6;s++)
      {
	for(Int_t m=0;m<4;m++)
	  {
	  for(Int_t mbo=0; mbo<16; mbo++)
	    {
	      gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),
				     "s %i m %i mbo %i %i %i %i %i %i %i %i %i %i",
				     s,m, mbo, bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+0],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+1],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+2],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+3],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+4],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+5],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+6],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+7],
				     bitshiftcorfactor[(s*576)+(m*144)+(mbo*9)+8]);
	    }
	  }
      }
    SEPERATOR_msg("#",60);
}
void HMdcBitFlipCorPar::putParams(HParamList* l)
{
    // Puts all params of HMdcBitFlipCorPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->add("bitflipcor",     bitshiftcorfactor.GetArray(),3456);
}
Bool_t HMdcBitFlipCorPar::getParams(HParamList* l)
{
  Int_t arraycp[3456];
  if (!l) return kFALSE;
  if(!( l->fill("bitflipcor",     arraycp, 3456)))      return kFALSE;
  else bitshiftcorfactor.Set(3456,arraycp);
    return kTRUE;
}
