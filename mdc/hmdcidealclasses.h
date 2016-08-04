#ifndef HMDCIDEALCLASSES_H
#define HMDCIDEALCLASSES_H

#include "hmdchitsim.h"
#include "hmdcsegsim.h"
#include "hmdctrkcand.h"

class HMdcHitIdeal : public HMdcHitSim {
  public:
  ClassDef(HMdcHitIdeal,1) // simulated hit on a MDC
};

class HMdcSegIdeal : public HMdcSegSim {
  public:
  ClassDef(HMdcSegIdeal,1) // simulated segment on a MDC
};
class HMdcTrkCandIdeal : public HMdcTrkCand {
  public:
  ClassDef(HMdcTrkCandIdeal,1) // simulated track candidate on a MDC
};

#endif  /* HMDCIDEALCLASSES_H */
