//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Ilse Koenig <i.koenig@gsi.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichContFact
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHCONTFACT_H
#define HRICHCONTFACT_H

#include "hcontfact.h"

class HParSet;
class HContainer;

class HRichContFact : public HContFact {
private:
   void setAllContainers();
public:
   HRichContFact();
   ~HRichContFact() {}
   HParSet* createContainer(HContainer*);
   ClassDef(HRichContFact, 0) // Factory for parameter containers in libRich
};

#endif  /* !HRICHCONTFACT_H */
