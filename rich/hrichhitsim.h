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
//  HRichHitSim
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHHITSIM_H
#define HRICHHITSIM_H

#include "hrichhit.h"

class HRichHitSim: public HRichHit {
public:
   HRichHitSim();
   virtual ~HRichHitSim() {}
//LF
   Short_t iRingPadNTrack1[256];//!
   Short_t iRingPadNTrack2[256];//!
   Short_t iRingTrack[256];//!
   Char_t iRingFlag[256];//!
// for each ring maximally 3 tracks are stored.
   Int_t track1, track2, track3;
   Int_t weigTrack1, weigTrack2, weigTrack3;
// flag that distinguishes Photons from IP
   Int_t flag1, flag2, flag3;
   HRichHitSim& operator=(const HRichHitSim& source) {
      HRichHit::operator=(source);
      return *this;
   }
   HRichHitSim& operator=(const HRichHit& source) {
      HRichHit::operator=(source);
      return *this;
   }
//LF
   ClassDef(HRichHitSim, 1)
};

#endif /* !HRICHHITSIM_H */
