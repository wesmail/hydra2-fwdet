//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700RingFinderPar
//
//  This class handles the parameters for HRich700RingFinderHough
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700ringfinderpar.h"
#include "richdef.h"
#include "hparamlist.h"

#include "TRandom.h"


#include <iostream>
#include <sstream>
#include <string>

using namespace std;


ClassImp(HRich700RingFinderPar)



HRich700RingFinderPar::HRich700RingFinderPar(const Char_t* name,const Char_t* title,
					     const Char_t* context)
    : HParCond(name,title,context)
{
    clear();
}

HRich700RingFinderPar::~HRich700RingFinderPar()
{
  // destructor
}

void HRich700RingFinderPar::clear()
{
    fNofParts         = 0;
    fMaxDistance      = 0.f;
    fMinDistance      = 0.f;
    fMinRadius        = 0.f;
    fMaxRadius        = 0.f;
    fNofBinsX         = 0;
    fNofBinsY         = 0;
    fHTCut            = 0;
    fNofBinsR         = 0;
    fHTCutR           = 0;
    fMinNofHitsInArea = 0;
    fUsedHitsAllCut   = 0.f;
    fRmsCoeffCOP      = 0.f;
    fMaxCutCOP        = 0.f;

}

void HRich700RingFinderPar::printParam(void)
{

    printf("----------------------------------------------------------------------\n");
    printf("HRich700RingFinderPar : \n");
    printf("fNofParts         = %hi \n", fNofParts );
    printf("fMaxDistance      = %f \n" , fMaxDistance );
    printf("fMinDistance      = %f \n" , fMinDistance);
    printf("fMinRadius        = %f \n" , fMinRadius);
    printf("fMaxRadius        = %f \n" , fMaxRadius);
    printf("fNofBinsX         = %hi \n", fNofBinsX);
    printf("fNofBinsY         = %hi \n", fNofBinsY);
    printf("fHTCut            = %hi \n", fHTCut);
    printf("fNofBinsR         = %hi \n", fNofBinsR );
    printf("fHTCutR           = %hi \n", fHTCutR );
    printf("fMinNofHitsInArea = %hi \n", fMinNofHitsInArea);
    printf("fUsedHitsAllCut   = %f \n" , fUsedHitsAllCut);
    printf("fRmsCoeffCOP      = %f \n" , fRmsCoeffCOP);
    printf("fMaxCutCOP        = %f \n" , fMaxCutCOP);
    printf("----------------------------------------------------------------------\n");

}


void HRich700RingFinderPar::putParams(HParamList* l)
{
    if (!l) return;

    l->add("fNofParts"        , fNofParts );
    l->add("fMaxDistance"     , fMaxDistance );
    l->add("fMinDistance"     , fMinDistance);
    l->add("fMinRadius"       , fMinRadius);
    l->add("fMaxRadius"       , fMaxRadius);
    l->add("fNofBinsX"        , fNofBinsX);
    l->add("fNofBinsY"        , fNofBinsY);
    l->add("fHTCut"           , fHTCut);
    l->add("fNofBinsR"        , fNofBinsR );
    l->add("fHTCutR"          , fHTCutR );
    l->add("fMinNofHitsInArea", fMinNofHitsInArea);
    l->add("fUsedHitsAllCut"  , fUsedHitsAllCut);
    l->add("fRmsCoeffCOP"     , fRmsCoeffCOP);
    l->add("fMaxCutCOP"       , fMaxCutCOP);
}

Bool_t HRich700RingFinderPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fill("fNofParts"        , &fNofParts        ))) return kFALSE;
    if(!( l->fill("fMaxDistance"     , &fMaxDistance     ))) return kFALSE;
    if(!( l->fill("fMinDistance"     , &fMinDistance     ))) return kFALSE;
    if(!( l->fill("fMinRadius"       , &fMinRadius       ))) return kFALSE;
    if(!( l->fill("fMaxRadius"       , &fMaxRadius       ))) return kFALSE;
    if(!( l->fill("fNofBinsX"        , &fNofBinsX        ))) return kFALSE;
    if(!( l->fill("fNofBinsY"        , &fNofBinsY        ))) return kFALSE;
    if(!( l->fill("fHTCut"           , &fHTCut           ))) return kFALSE;
    if(!( l->fill("fNofBinsR"        , &fNofBinsR        ))) return kFALSE;
    if(!( l->fill("fHTCutR"          , &fHTCutR          ))) return kFALSE;
    if(!( l->fill("fMinNofHitsInArea", &fMinNofHitsInArea))) return kFALSE;
    if(!( l->fill("fUsedHitsAllCut"  , &fUsedHitsAllCut  ))) return kFALSE;
    if(!( l->fill("fRmsCoeffCOP"     , &fRmsCoeffCOP     ))) return kFALSE;
    if(!( l->fill("fMaxCutCOP"       , &fMaxCutCOP       ))) return kFALSE;
    return kTRUE;
}

std::string HRich700RingFinderPar::getStringForParTxtFile()
{
    stringstream ss;

    ss << "##############################################################################" << endl;
    ss << "# Class:   HRich700RingFinderPar" << endl;
    ss << "# Context: Rich700RingFinderParProduction" << endl;
    ss << "##############################################################################" << endl;
    ss << "[Rich700RingFinderPar]" << endl;
    ss << "//----------------------------------------------------------------------------" << endl;
    ss << "fNofParts:  Int_t  " << fNofParts << endl;
    ss << "fMaxDistance:  Float_t " << fMaxDistance << endl;
    ss << "fMinDistance:  Float_t  " << fMinDistance << endl;
    ss << "fMinRadius:  Float_t  " << fMinRadius << endl;
    ss << "fMaxRadius:  Float_t  " << fMaxRadius << endl;
    ss << "fNofBinsX:  Int_t  " << fNofBinsX << endl;
    ss << "fNofBinsY:  Int_t  " << fNofBinsY << endl;
    ss << "fHTCut:  Int_t  " << fHTCut << endl;
    ss << "fNofBinsR:  Int_t  " << fNofBinsR << endl;
    ss << "fHTCutR:  Int_t  " << fHTCutR << endl;
    ss << "fMinNofHitsInArea:  Int_t  " << fMinNofHitsInArea << endl;
    ss << "fUsedHitsAllCut:  Float_t  " << fUsedHitsAllCut << endl;
    ss << "fRmsCoeffCOP:  Float_t  " << fRmsCoeffCOP << endl;
    ss << "fMaxCutCOP:  Float_t  " << fMaxCutCOP << endl;
    ss << "##############################################################################" << endl;

    return ss.str();
}
