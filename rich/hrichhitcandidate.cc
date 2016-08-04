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
//  HRichHitCandidate
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichhitcandidate.h"

ClassImp(HRichHitCandidate)


HRichHitCandidate::HRichHitCandidate()
{
   fX = 0;
   fY = 0;
   fA = 0;
   fPadLabel = 0;
   fMaxLabel = 0;
}

HRichHitCandidate::HRichHitCandidate(const Int_t x, const Int_t y, const Int_t a,
                                     const Int_t p, const Int_t l)
{
   fX = x;
   fY = y;
   fA = a;
   fPadLabel = p;
   fMaxLabel = l;
}

HRichHitCandidate::~HRichHitCandidate() {}

void HRichHitCandidate::Reset()
{
   fX = 0;
   fY = 0;
   fA = 0;
   fPadLabel = 0;
   fMaxLabel = 0;
}
