// Created by M.Golubeva 21.11.2006
//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HWallRefWinPar
//
// Container class for Wall RefWin parameters
//
/////////////////////////////////////////////////////////////

#include "hwallrefwinpar.h"
#include "hparamlist.h"

ClassImp(HWallRefWinPar)

HWallRefWinPar::HWallRefWinPar(const Char_t* name,const Char_t* title,
                           const Char_t* context)
             : HParCond(name,title,context)
{
  // constructor
    clear();
}

void HWallRefWinPar::clear() {
  // clears the container
  time_ref_win_low =0.F;
  time_ref_win_high=0.F;

  status=kFALSE;
  resetInputVersions();
}

void HWallRefWinPar::putParams(HParamList* l) {
  // puts all parameters to the parameter list, which is used by the io
  if (!l) return;
  l->add("time_ref_win_low",time_ref_win_low);
  l->add("time_ref_win_high",time_ref_win_high);
}

Bool_t HWallRefWinPar::getParams(HParamList* l) {
 // gets all parameters from the parameter list, which is used by the io
  if (!l) return kFALSE;
  if (!(l->fill("time_ref_win_low",&time_ref_win_low))) return kFALSE;
  if (!(l->fill("time_ref_win_high",&time_ref_win_high))) return kFALSE;

  return kTRUE;
}
