//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <Laura.Fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichAnalysisSim
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHANALYSISSIM_H
#define HRICHANALYSISSIM_H

#include "hrichanalysis.h"

class HCategory;
class HRichCalSim;
class HRichHitSim;
class HRichRingFindSim;


class HRichAnalysisSim: public HRichAnalysis {
public:
   HRichAnalysisSim(void);
   HRichAnalysisSim(const Text_t *name, const Text_t *title, Bool_t kSkip = kFALSE);
   ~HRichAnalysisSim(void);

   Bool_t init(void);
   Bool_t reinit(void);
   Int_t  execute();

   Int_t  getPadsNTrack1(Int_t padx, Int_t pady, Int_t sec);
   Int_t  getPadsNTrack2(void);
   Int_t  getTrack(Int_t index);
   Int_t  getFlag(Int_t index);

   HRichHitSim *pRings;

private:
   void   sortTracks(HRichHitSim *);
   void   updateHits(Int_t nSec);
   Bool_t initParameters(void);

   Bool_t kSkipEvtIfNoRing;

   HRichRingFindSim*  pRingFindSim;  // Pointer to ringfinder
   HCategory*         m_pTrackCat;   // Pointer to the track container
   HRichCalSim*       pRichCalSim;   // Pointer to cal container

   ClassDef(HRichAnalysisSim, 0)

};

#endif /* !HRICHANALYSISSIM_H */

