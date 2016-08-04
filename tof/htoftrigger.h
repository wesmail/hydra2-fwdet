#ifndef  HTOFTRIGGER_H
#define  HTOFTRIGGER_H

#include "hreconstructor.h"
#include "hkinesim.h"

class HCategory;
class HTofCalPar;

class HTofTrigger : public HReconstructor {
protected:  
  HCategory *fHitCat;  //! Pointer to the hits category
  HIterator *iterator; //! Iterator on raw data
  Int_t  nThreshold;

public:
  HTofTrigger(void);
  HTofTrigger(const Text_t *name,const Text_t *title,Int_t multi);
  ~HTofTrigger(void) {delete kineGeant;}
  HKineSim * kineGeant;
  Bool_t init(void);
  Bool_t finalize(void) {return kTRUE;}
  Int_t execute(void);
  void checkTofHit(Int_t nTrack ,Int_t nSecTof);
  Bool_t checkTofMult();
  Int_t nSec2, nSec5, nMult, nEvCounter;
  ClassDef(HTofTrigger,0) // Select simulated TOF hits
};

#endif /* !HTOFTRIGGER_H */






