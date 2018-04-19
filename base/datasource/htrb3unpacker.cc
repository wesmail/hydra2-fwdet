//*-- AUTHOR Sergey Linev
//*-- Modified : 17/04/2014 by I. Koenig
//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//  HTrb3Unpacker
//  Base class for TRB data unpacking.
//
//  Any detector connected to TRB3 boards should use this class to decode the subevent. 
//  Decode() method fills the arrays with information about: 
//  channel number (0-127), 
//  measured time, 
//  measured width.
//
//  The goal is to provide data which can be directly used by subsystems 
//  in order to directly fill raw categories.
//
//  The most important information is stored in:	   
//  trbLeadingTime[ TrbChanNb ][ hitNb ],
//  trbADC[ TrbChanNb ][ hitNb ]. 
//  trbLeadingMult[ TrbChanNb ]
//
//  Max. number of hits per channel is currently set to 10. 
//
/////////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "htrb3unpacker.h"
#include "hdatasource.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include "htrb3tdciterator.h"
#include "htrb3calpar.h"
#include "hruntimedb.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

ClassImp(HTrb3Unpacker)

HTrb3Unpacker::HTrb3Unpacker(UInt_t id) :
  HldUnpack(),
  subEvtId(id),
  uHUBId(0x9000),
  uCTSId(0x8000),
  debugFlag(0),
  quietMode(kFALSE),
  reportCritical(kFALSE)
{
  // Constructor, sets the subevent id
}

HTrb3Unpacker::~HTrb3Unpacker()
{
}


void HTrb3Unpacker::clearAll(void)
{
}

Bool_t HTrb3Unpacker::decode(void)
{
  // unpacks trb3 data into tdc hits
  clearAll();

  if (pSubEvt==0) return kFALSE;
  
  // ensure that data swapped once
  // pSubEvt->swapData();

  UInt_t* raw_data = pSubEvt->getData();
  UInt_t* raw_end  = pSubEvt->getEnd();
  UInt_t nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

  if(debugFlag > 0) {
     cout<<"-0-EvNb "<<nEvt<<", "<<hex<<"data word: "<<*raw_data<<dec<<"  --NetSubevent(TRB) Header"<<endl;
  }

  // all sub-sub events will be scanned from at once
  // data = data + uStartPosition;

  if(debugFlag > 0){
    cout<<"-0a-EvNb "<<nEvt<<", data word: "<<hex<<*raw_data<<dec<<"  --"<<endl;
  }

  UInt_t ix = 0;
  UInt_t trbSubEvSize = raw_end - raw_data;

  // from here code very similar to TrbProcessor

  while (ix < trbSubEvSize) {

     UInt_t data = raw_data[ix++];

     UInt_t datalen = (data >> 16) & 0xFFFF;
     UInt_t trbaddr = data & 0xFFFF;

     if ((trbaddr & 0xFF00) == (uHUBId & 0xFF00)) {
        if(debugFlag > 0)
           printf("   HUB header: 0x%08x, size=%u (ignore)\n", (UInt_t) data, datalen);

        // TODO: formally we should analyze HUB subsubevent as real subsubevent but
        // we just skip header and continue to analyze data

        // ix+=datalen;  // WORKAROUND !!!

        continue;
     }

     //! ==================== CTS header ================
     if ((trbaddr & 0xFF00) == (uCTSId & 0xFF00)) {
        if(debugFlag > 0)
           printf("   CTS header: 0x%x, size=%d\n", (UInt_t) data, datalen);
        //hTrbTriggerCount->Fill(5);          //! TRB - CTS
        //hTrbTriggerCount->Fill(0);          //! TRB TOTAL

        // ignore CTS header
        ix+=datalen;
        continue;
     }

     //! ==================  Dummy header  ==========================
     if (trbaddr == 0x5555) {
        if(debugFlag > 0)
           printf("   Dummy header: 0x%x, size=%d\n", (UInt_t) data, datalen);
        //hTrbTriggerCount->Fill(4);          //! TRB - DUMMY
        //hTrbTriggerCount->Fill(0);          //! TRB TOTAL
        while (datalen-- > 0) {
           //! In theory here must be only one word - termination package with the status
           data = raw_data[ix++];
           if(debugFlag > 0)
              printf("      word: 0x%08x\n", (UInt_t) data);
           //UInt_t fSubeventStatus = data;
           //if (fSubeventStatus != 0x00000001) { bad events}
        }

        continue;
     }

     if (decodeData(trbaddr, datalen, raw_data + ix))
     {
         ix+=datalen;
        continue; // go to next block
     }

     if(debugFlag > 0)
        printf("Unknown header %x length %u in TDC subevent\n", trbaddr, datalen);

     ix+=datalen;
  }

  // decoding and filling

  return kTRUE;
}
