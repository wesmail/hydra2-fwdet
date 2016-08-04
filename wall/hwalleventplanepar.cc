//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HWallEventPlanePar
//
// Container class for Wall EventPlane parameters
//
/////////////////////////////////////////////////////////////

#include "hwalleventplanepar.h"
#include "hparamlist.h"

ClassImp(HWallEventPlanePar)

    HWallEventPlanePar::HWallEventPlanePar(const Char_t* name,const Char_t* title,
					   const Char_t* context)
    : HParCond(name,title,context)
{
    // constructor
    clear();
}

void HWallEventPlanePar::clear() {
    // clears the container
    fT1_cut = 0.F;
    fT2_cut = 0.F;
    fX_shift = 0.F;
    fY_shift = 0.F;
    fR0_cut = 0.F;
    fZ1_cut_s = 0.F;
    fZ1_cut_m = 0.F;
    fZ1_cut_l = 0.F;

    status   = kFALSE;
    resetInputVersions();
}

void HWallEventPlanePar::putParams(HParamList* l) {
    // puts all parameters to the parameter list, which is used by the io
    if (!l) return;
    l->add("fT1_cut",fT1_cut);
    l->add("fT2_cut",fT2_cut);
    l->add("fX_shift",fX_shift);
    l->add("fY_shift",fY_shift);
    l->add("fR0_cut",fR0_cut);
    l->add("fZ1_cut_s",fZ1_cut_s);
    l->add("fZ1_cut_m",fZ1_cut_m);
    l->add("fZ1_cut_l",fZ1_cut_l);
}

Bool_t HWallEventPlanePar::getParams(HParamList* l) {
    // gets all parameters from the parameter list, which is used by the io
    if (!l) return kFALSE;
    if (!(l->fill("fT1_cut",&fT1_cut))) return kFALSE;
    if (!(l->fill("fT2_cut",&fT2_cut))) return kFALSE;
    if (!(l->fill("fX_shift",&fX_shift))) return kFALSE;
    if (!(l->fill("fY_shift",&fY_shift))) return kFALSE;
    if (!(l->fill("fR0_cut",&fR0_cut))) return kFALSE;
    if (!(l->fill("fZ1_cut_s",&fZ1_cut_s))) return kFALSE;
    if (!(l->fill("fZ1_cut_m",&fZ1_cut_m))) return kFALSE;
    if (!(l->fill("fZ1_cut_l",&fZ1_cut_l))) return kFALSE;

    return kTRUE;
}
