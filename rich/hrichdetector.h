//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichDetector
//
//
//
//////////////////////////////////////////////////////////////////////////////

#ifndef HRICHDETECTOR_H
#define HRICHDETECTOR_H

#include "haddef.h"
#include "hdetector.h"

class HParIo;
class HTask;

class HRichDetector : public HDetector {

public:
   HRichDetector();
   virtual ~HRichDetector() {}

   HCategory *buildLinearCat(const Text_t* classname, const Int_t   size = 1000);
   HCategory *buildMatrixCat(const Text_t* classname, const Float_t fillRate);
   HCategory *buildSimMatrixCat(const Text_t* classname, const Float_t fillRate, const Int_t size = 200);


   HTask*     buildTask(const Text_t name[], const Option_t* opt = "");
   HCategory* buildCategory(Cat_t cat);

   void     activateParIo(HParIo* io);
   Bool_t   write(HParIo* io);
   Bool_t   init(void) {
      return kTRUE;
   }

   ClassDef(HRichDetector, 1) // RICH detector class
};


#endif // HRICHDETECTOR_H
