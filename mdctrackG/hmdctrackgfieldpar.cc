
using namespace std;
#include "hmdctrackgfieldpar.h"
#include "hmdctrackgfield.h"
#include "hparamlist.h"
#include <iostream>
#include "stdlib.h"

ClassImp(HMdcTrackGFieldPar);


HMdcTrackGFieldPar::HMdcTrackGFieldPar(const Char_t* name, const Char_t* title, const Char_t* context)
    :HParCond(name,title,context)
{
    field = new HMdcTrackGField("Fieldmap","Fieldmap");
}

HMdcTrackGFieldPar::~HMdcTrackGFieldPar()
{
    if(field)  delete field;
}

void HMdcTrackGFieldPar::putParams(HParamList* l)
{
    // Puts all params of HMdcDigitPar to the parameter list of
    // HParamList (which ist used by the io);
    if (!l) return;
    l->addObject("field",field);
}
Bool_t HMdcTrackGFieldPar::getParams(HParamList* l)
{
    if (!l) return kFALSE;
    if(!( l->fillObject("field",field))) return kFALSE;
    return kTRUE;
}
void HMdcTrackGFieldPar::clear()
{
    field->clear();
}
void HMdcTrackGFieldPar::initFieldMap(TString input)
{
field->init(input);
}
void HMdcTrackGFieldPar::calcField(Double_t *a,Double_t *b,Double_t i){ field->calcField(a,b,i);}

