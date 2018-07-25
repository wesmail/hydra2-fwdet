//*-- Modified : 03.12.2009 Ilse Koenig, adopted for rich700 08.06.2017 JAM

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////////////
//
//  HRich700Raw
//
//  Unpacked TRB raw data of the START detector using the TRB for readout
//
//  The class accepts up to 4 (START2_MAX_MULT) hits per channel, each with
//    time and width. If the number of hits exceeds 4, the hit counter
//    will be incremeted, but no data are stored for these hits.
//
/////////////////////////////////////////////////////////////////////////////

#include "hrich700raw.h"

ClassImp(HRich700hit_t)
ClassImp(HRich700Raw)

