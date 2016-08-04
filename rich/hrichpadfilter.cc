//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <laura.fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichPadFilter
//
//  HRichPadFilter is a HFilter to reduce the size
//  of the HRichCalSim container. After the cut-off
//  threshold for each pad is applied all the pad
//  that are under threshold are deleted.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichcalsim.h"
#include "hrichpadfilter.h"

#include <fstream>

using namespace std;

ClassImp(HRichPadFilter)

Bool_t
HRichPadFilter::check(TObject* hit)
{
   // returns kFALSE if the pad has to be deleted.
   // otherwise, returns kTRUE

   if (0 != static_cast<HRichCalSim*>(hit)->getCharge()) {
      return kTRUE;
   }
   return kFALSE;
}
