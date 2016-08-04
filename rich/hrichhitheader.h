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


#ifndef __HRichHitHeader_H
#define __HRichHitHeader_H

#include "TArrayI.h"
#include "TObject.h"

class HRichHitHeader: public TObject {
public:
   HRichHitHeader();
   virtual ~HRichHitHeader() {}

   Float_t getCurrentSec() {
      return detCurrentSec;
   }
   Int_t   getEventNr() {
      return iEventNr;
   }
   Int_t   getExecutedAnalysis(void) {
      return iExecutedAnalysis;
   }
   Int_t   getSector(void) {
      return m_nSector;
   }
   void    setCurrentSec(Float_t Charge) {
      detCurrentSec = Charge;
   }
   void    setEventNr(Int_t iNr) {
      iEventNr = iNr;
   }
   void    setExecutedAnalysis(Int_t i) {
      iExecutedAnalysis = i;
   }
   void    setSector(Int_t s) {
      m_nSector = s;
   }

public:
   Int_t iEventNr;
   Int_t iPadFiredNr;
   Int_t iPadCleanedNr;
   Int_t iClusterCleanedNr;
   TArrayI iClustersCleaned; //! get rid of this little bugger
   Int_t iRingNr;
   Int_t iExecutedAnalysis;

   Int_t iFakePad;
   Int_t iFakeLocalMax4;
   Int_t iFakeLocalMax8;

   Int_t m_nSector;
   Float_t detCurrentSec;

   ClassDef(HRichHitHeader, 1)
};

#endif /* !__HRichHitHeader_H */



