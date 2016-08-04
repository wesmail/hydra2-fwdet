//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <laura.fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichPadFilter
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHPADFILTER_H
#define HRICHPADFILTER_H

#include "hfilter.h"

class HRichPadFilter : public HFilter {
public:
   HRichPadFilter(void)  {}
   ~HRichPadFilter(void) {}

   Bool_t check(TObject* padCal);

   ClassDef(HRichPadFilter, 0)
};


#endif /* !HRICHPADFILTER_H */




