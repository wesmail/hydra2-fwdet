#include "hkalmetamatch.h"

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
//

ClassImp(HKalMetaMatch)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalMetaMatch::HKalMetaMatch() {

    clear();
}

//  -----------------------------------
//  Public Methods
//  -----------------------------------

void HKalMetaMatch::clear() {
    beta        = -1.F;
    ind         = -1;
    mass2       = -1.F;
    metaEloss   = -1.F;
    Double_t defPos = 1.e4;
    metaHit. SetXYZ(defPos, defPos, defPos);
    metaHitLocal.SetXYZ(defPos, defPos, defPos);
    metaReco.SetXYZ(-defPos, -defPos, -defPos);
    metaRecoLocal.SetXYZ(-defPos, -defPos, -defPos);
    metaSys     = -1;
    quality     = -1.F;
    tof         = -1.F;
    trackLength = -1.F;
}
