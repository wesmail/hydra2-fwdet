#ifndef HGEOMCOMPOSITEVOLUME_H
#define HGEOMCOMPOSITEVOLUME_H

#include "hgeomvolume.h"

class HGeomCompositeVolume : public HGeomVolume {
protected:
  TObjArray* components;    // array of components (type HGeomVolume)
public:
  HGeomCompositeVolume(Int_t nComp=0);
  ~HGeomCompositeVolume();
  Int_t getNumComponents();
  HGeomVolume* getComponent(const Int_t);
  void createComponents(const Int_t);
  void setComponent(HGeomVolume*,const Int_t);
  void clear();
  void print();
  ClassDef(HGeomCompositeVolume,1) // basic geometry of a volume with components
};

#endif /* !HGEOMCOMPOSITEVOLUME_H */
