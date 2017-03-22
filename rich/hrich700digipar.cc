//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700DigiPar
//
//  This class handles the parameters for HRich700Digitizer
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700digipar.h"
#include "hparamlist.h"


using namespace std;


ClassImp(HRich700DigiPar)



HRich700DigiPar::HRich700DigiPar(const Char_t* name,const Char_t* title,
				 const Char_t* context)
    : HParCond(name,title,context)
{
    clear();
}

HRich700DigiPar::~HRich700DigiPar()
{
  // destructor
}

void HRich700DigiPar::clear()
{
    fCrossTalkProbability  = 0;
    fNofNoiseHits          = 0;

}

void HRich700DigiPar::printParam(void)
{

    printf("----------------------------------------------------------------------\n");
    printf("HRich700DigiPar : \n");
    printf("fCrossTalkProbability         = %f \n", fCrossTalkProbability );
    printf("fNofNoiseHits                 = %i \n", fNofNoiseHits);
    printf("----------------------------------------------------------------------\n");

}


void HRich700DigiPar::putParams(HParamList* l)
{
    if (!l) return;

    l->add("fCrossTalkProbability" , fCrossTalkProbability );
    l->add("fNofNoiseHits"         , fNofNoiseHits );
}

Bool_t HRich700DigiPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fCrossTalkProbability", &fCrossTalkProbability ))) return kFALSE;
    if(!( l->fill("fNofNoiseHits"        , &fNofNoiseHits         ))) return kFALSE;
    return kTRUE;
}
