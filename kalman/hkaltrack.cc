//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// 
// Data container holding all information from the Kalman filter.
//
///////////////////////////////////////////////////////////////////////////////

#include "hkaltrack.h"

ClassImp(HKalTrack)

HKalTrack::HKalTrack() {
    ndf            = -1.F;
    trackLength    = -1.F;
    pid            = -1.F;
    isWireHit      = kFALSE;
    idxFirst       = -1;
    idxLast        = -1;
    dxMeta         = -10000.F;
    dyMeta         = -10000.F;
    dzMeta         = -10000.F;
    qualityRpc     = -1.F;
    qualityShower  = -1.F;
    qualityTof     = -1.F;
}
