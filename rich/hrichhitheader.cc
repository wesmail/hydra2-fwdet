//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Walter Karig <W.Karig@gsi.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichHitCandidate
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichhitheader.h"

ClassImp(HRichHitHeader)

HRichHitHeader::HRichHitHeader()
{
   setSector(-1);

   iEventNr = 0;
   iPadFiredNr = 0;
   iPadCleanedNr = 0;
   iClusterCleanedNr = 0;
   iClustersCleaned.Set(0);
   iRingNr = 0;

   iFakePad = 0;
   iFakeLocalMax4 = 0;
   iFakeLocalMax8 = 0;
   detCurrentSec = 0;
   iExecutedAnalysis = 0;

}



