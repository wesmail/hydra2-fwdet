//*****************************************************************************
// File: $RCSfile: htrbnetunpacker.cc,v $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//
//*****************************************************************************
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HTrbNetUnpacker
//
//
//*****************************************************************************

#include "htrbnetunpacker.h"
#include "htrbnetdebuginfo.h"
#include "hades.h"
#include "hevent.h"
#include "hruntimedb.h"
#include "hmessagemgr.h"
#include "hlinearcategory.h"
#include "hlocation.h"

ClassImp(HTrbNetUnpacker)

HTrbNetUnpacker::HTrbNetUnpacker(void)
{
   initialized = kFALSE;
   debugInfo = NULL;
}

HTrbNetUnpacker::HTrbNetUnpacker(HTrbNetUnpacker &unp) : TObject(unp)
{
}

HTrbNetUnpacker::~HTrbNetUnpacker(void)
{
}

Int_t HTrbNetUnpacker::execute(void)
{
   return 1;
}

Bool_t HTrbNetUnpacker::init(void)
{
   // Initialize category to store information
   if (!initialized) {
      rtdb = gHades->getRuntimeDb();
      if (!rtdb) {
         ERROR_msg(HMessageMgr::DET_ALL, "Could not get pointer tu runtime database!");
         return kFALSE;
      }

      // create HTrbNetDebugInfo category.
      debugInfo = (HLinearCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catTrbNet));

      if (!debugInfo) {
         debugInfo = new HLinearCategory("HTrbNetDebugInfo", 20);
         debugInfo->setPersistency(kFALSE);
      }
      if (!((HEvent*)(gHades->getCurrentEvent()))->addCategory(catTrbNet, debugInfo, "TrbNet")) {
         return kFALSE;
      }
      initialized = kTRUE;
   }
   return kTRUE;
}

Bool_t HTrbNetUnpacker::reinit(void)
{
   return kTRUE;
}

Bool_t HTrbNetUnpacker::finalize(void)
{
   return kTRUE;
}

Int_t HTrbNetUnpacker::unpackData(UInt_t *data, Int_t subEventId)
{
   // Unpack data. If subEventId=0 put with trbNetadress, else take subEventId as adress.
   UInt_t size = 0;
   UInt_t address = 0;
   HTrbNetDebugInfo *infoObj;
   HLocation loc;

   size = (*data & 0xffff0000) >> 16;
   if (subEventId == 0) {
      address = (*data & 0xffff);
   } else {
      address = subEventId;
   }
   data++;
   loc.set(1, 0);
   for (UInt_t i = 0; i < size; i++) {
      infoObj = (HTrbNetDebugInfo*)debugInfo->getNewSlot(loc);
      if (infoObj) {
         infoObj = new(infoObj) HTrbNetDebugInfo();
         infoObj->setAddress(address & 0xffff);
         infoObj->setStatus(*data);
      } else return -1;
   }
   return 0;
}
