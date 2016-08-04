#ifndef HBASECONTFACT_H
#define HBASECONTFACT_H

#include "hcontfact.h"

class HParSet;

class HBaseContFact : public HContFact {
private:
  void setAllContainers();
public:
  HBaseContFact();
  ~HBaseContFact() {}
  HParSet* createContainer(HContainer*); 
  ClassDef(HBaseContFact,0) // Factory for parameter containers in lib Hydra
};

#endif  /* !HBASECONTFACT_H */
