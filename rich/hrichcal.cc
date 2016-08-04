//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichCal
//
//  This class contains one hit in one cell of the RICH.
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichcal.h"

using namespace std;

ClassImp(HRichCal)

HRichCal::HRichCal()
   : HLocatedDataObject()
{
   fCharge         = 0.0;
   fSector         = -1;
   fRow            = -1;
   fCol            = -1;
   fEventNr        = -1;
   isCleanedSingle = kFALSE;
   isCleanedHigh   = kFALSE;
   isCleanedSector = kFALSE;
}

HRichCal::HRichCal(Float_t q)
   : HLocatedDataObject()
{
   fCharge         = q;
   fSector         = -1;
   fRow            = -1;
   fCol            = -1;
   fEventNr        = -1;
   isCleanedSingle = kFALSE;
   isCleanedHigh   = kFALSE;
   isCleanedSector = kFALSE;
}

HRichCal::HRichCal(Int_t s, Int_t r, Int_t c)
   : HLocatedDataObject()
{
   fCharge         = 0.0;
   fSector         = s;
   fRow            = r;
   fCol            = c;
   fEventNr        = -1;
   isCleanedSingle = kFALSE;
   isCleanedHigh   = kFALSE;
   isCleanedSector = kFALSE;
}
