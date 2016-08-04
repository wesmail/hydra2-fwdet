#ifndef HTRACK_H
#define HTRACK_H

#include "hrecobject.h"

class HTrack : public HRecObject
{
private:
  Float_t fP; // Particle momentum in MeV/c
public:
  HTrack(void);
  HTrack(HTrack &aTrack);
  ~HTrack(void);
  void setMomentum(Float_t aP);
  ClassDef(HTrack,1) // Track class (= dummy)
};

#endif /* !HTRACK_H */



