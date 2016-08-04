#ifndef HTOFCLUSTERFSIM_H
#define HTOFCLUSTERFSIM_H

#include "htofclusterf.h"
#include "tofdef.h"

class HTofCluster;
class HTofHit;
class HCategory;
class HIterator;

class HTofClusterFSim : public HTofClusterF {

public:
  HTofClusterFSim(void) { }
  HTofClusterFSim(const Text_t* name,const Text_t* title) : HTofClusterF(name,title) { }
  ~HTofClusterFSim(void);
  Bool_t init();
  Int_t execute();
  ClassDef(HTofClusterFSim,0) // Tof cluster finder for simulated data.
};

#endif /* !HTOFCLUSTERFSIM_H */

