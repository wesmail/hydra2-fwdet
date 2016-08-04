#ifndef HTOFHITFSIM_H
#define HTOFHITFSIM_H

#include "htofhitf.h"
#include "tofdef.h"

class HTofHit;
class HTofRaw;
class HCategory;
class HIterator;

class HTofHitFSim : public HTofHitF {
  protected:
    HTofHit *makeHit(TObject *address);
    void fillHit(HTofHit *, HTofRaw *);
  public:
    HTofHitFSim(void);
    HTofHitFSim(const Text_t* name,const Text_t* title);
    ~HTofHitFSim(void);
    Bool_t init();
    ClassDef(HTofHitFSim,0) // Tof hit finder for simulated data.
};

#endif /* !HTOFHITFSIM_H */

