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
//  HRichRingFindSim
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHRINGFINDSIM_H
#define HRICHRINGFINDSIM_H

#include "hrichringfind.h"

class HRichAnalysisSim;
class HRichHitSim;


class HRichRingFindSim: public HRichRingFind {

public:
   HRichRingFindSim();
   virtual ~HRichRingFindSim();

   Bool_t init(HRichAnalysisSim* showMe);
   Int_t  Execute(HRichAnalysisSim *giveMe);

private:
   void LookForTrack(HRichAnalysisSim *showMe);

   HRichHitSim* pRings;

   ClassDef(HRichRingFindSim, 0)

};

#endif /* !HRICHRINGFINDSIM_H */
