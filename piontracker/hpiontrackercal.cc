//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HPionTrackerCal
//
//  Calibrated data of the PionTracker detector
//
//  The class accepts up to 4 hits per channel, each with time and charge.
//  If the number of hits exceeds 4, the hit counter will be incremented,
//  but no data are stored for these hits.
//
/////////////////////////////////////////////////////////////////////////////

#include "hpiontrackercal.h"

ClassImp(HPionTrackerCal)