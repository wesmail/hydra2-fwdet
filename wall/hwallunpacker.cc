//*-- AUTHOR : Ilse Koenig, Tomasz Wojcik, Wolfgang Koenig
//*-- Created : 17/01/2005

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HWallRaw
//
//  Class for the raw data class of the Forward Wall
//  (see TOF unpacker for subevent information)
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hwallunpacker.h"
#include "walldef.h"
#include "hwalllookup.h"
#include "hwallraw.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

ClassImp(HWallUnpacker)

HWallUnpacker::HWallUnpacker(UInt_t id) {
  // constructor
  subEvtId=id;
  pRawCat=NULL;
  lookup=0;
}

Bool_t HWallUnpacker::init(void) {
  // creates the raw category and gets the pointer to the lookup table
  pRawCat=gHades->getCurrentEvent()->getCategory(catWallRaw);
  if (!pRawCat) {
    pRawCat=gHades->getSetup()->getDetector("Wall")->buildCategory(catWallRaw);
    if (!pRawCat) return kFALSE;
    gHades->getCurrentEvent()->addCategory(catWallRaw,pRawCat,"Wall");
  }
  loc.set(1,0);
  lookup=(HWallLookup*)(gHades->getRuntimeDb()->getContainer("WallLookup"));
  return kTRUE;
}


Int_t HWallUnpacker::execute(void) {
// This function fills the Forward Wall raw data structure.
// see TOF unpacker for subevent information
  HWallRaw* pRaw=0;
  Int_t nCrate=-1, nChannel=0, nCell=-1;
  Int_t nEvt        = 0;    //Evt Id
  UInt_t nSlot      = 0;
  UInt_t uWordCount = 0;    //auxiliary
  UInt_t uBlockSize = 0;    //Number of data words in one block
  UInt_t uBoardType = 0;    // 1 - Struck, 0 - TDC
  UInt_t *uTmp      = NULL; //auxiliary
  Char_t dcType='U';
  if (gHades->isCalibration()){
    //calibration event
    //nothing to do for forward wall
    return 1;
  }
  if (pSubEvt) {
    UInt_t* data = pSubEvt->getData();
    UInt_t* end  = pSubEvt->getEnd();
    nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
    //Loop over all data in this subevt
    while ( ++data<end && *data!=0x0 ) {
      #if DEBUG_LEVEL>4
         printf("Data word: %p\n",*data);
      #endif
      //Scan for TIP-block header ( TYPE 0 )
      //TIP-block appears only in the new data structure
      if ( ((*data >>24) & 0x6)==0 ) {
        uBlockSize = (*data) & 0x0000003ff;
        uBoardType = (*data >> 12 ) & 0x1;
        //latch or scaler
        if (uBoardType==1) {
          data+=uBlockSize;
          continue;
        }
        //next Should (can) be tdc header
        else if (uBoardType==0) {
          //Do a normal TDC scanning
          #if DEBUG_LEVEL>2
             printf("TIP-block 0x%08x Size %i Board  %i\n",
                    *data,uBlockSize,uBoardType);
          #endif
          continue;
        } else {
          #if DEBUG_LEVEL>2
             Warning("execute","Uknown BoardType=%i",uBoardType);
          #endif
        }
      }
      //Scan for TDC header
      else if ( ((*data>>24) & 0x6)==2 ){
        //Found valid TDC header
        uBlockSize = ( (*data) >> 8   ) & 0xff;
        nCrate     = ( (*data) >> 16  ) & 0xff;
        nSlot      = ( (*data) >> 27  ) & 0x1f;
        //Check if also trailer exists
        if ((*(data + uBlockSize+1) >>24 & 0x6)!=4) {
          Warning("execute","Header found but no trailer!!!");
          continue;
        }
        //Check trigger tag in the trailer
        if ((*(data+uBlockSize+1)&0xff)!=((UInt_t)pSubEvt->getTrigNr()&0xFF)) {
          Error("execute","Trigger tag mismatch!");
          exit(EXIT_FAILURE);
        }
        //Scan all data words
        #if DEBUG_LEVEL>4
           printf("TDC header 0x%08x.Size: %i.Crate: %i.Slot: %i.\n",
                  *data,uBlockSize,nCrate,nSlot);
        #endif
        uTmp=data;
        HWallLookupSlot* dc=lookup->getSlot(nCrate-1,nSlot-1);
        if (dc && (dcType=dc->getType())!='U') {
          uWordCount=0;
          while( ++data && data<(uTmp + uBlockSize+1) && *data!=0x0 ) {
            uWordCount++;
            if (nSlot!=((*data>>27)&0x1f)) {
              Warning("execute",
                      "Slot (%d) different that in header(%d)",
                      nSlot,(*data>>27)&0x1f);
              continue;
            }
            nChannel = (*(data) >> 16 ) & 0x003f;
            HWallLookupChan* dcs=dc->getChannel(nChannel);
            if (dcs) {
              nCell=dcs->getCell();
              loc[0]=nCell;
              pRaw=(HWallRaw *)pRawCat->getObject(loc);
              if (!pRaw) {
                pRaw=(HWallRaw *)pRawCat->getSlot(loc);
                if (pRaw) {
                  pRaw=new (pRaw) HWallRaw;
                  pRaw->setCell(nCell);
                } else {
                  Error("execute()","Can't get slot\n");
                  return -1;
                }
              } else {
                #if DEBUG_LEVEL>4
                   Info("execute()","Slot already exists!\n");
                #endif
              }
              Float_t val = (*data & 0xfff);
              if (dcType=='T') pRaw->setTime(val);
              else pRaw->setCharge(val);
            } else Info("execute()","TDC channel: %i not found",nChannel);
          } // end while
          if (uWordCount!=uBlockSize) {
            //Corrupted block
            Warning("execute()",
                    "Found %i but expecting %i words!!!",
                    uWordCount,uBlockSize);
            continue;
          }
        } else {
          //Skip this block
          data+=uBlockSize+1;
          #if DEBUG_LEVEL>4
             Info("execute()",
                  "Can not get TDC for crate %i and slot %i",
                  nCrate-1,nSlot-1);
          #endif
        }
      } else if ( ((*data>>24) & 0x6)==6 ) {
        //invalid data word
        // skip this filler word
        #if DEBUG_LEVEL>2
           Warning("execute()",
                   "Evt: %i, Invalid Data Word! Type: %d,Skipping 0x%08x",
                   nEvt,((*data>>24) & 0x6),*data);
          #if DEBUG_LEVEL>4
             pSubEvt->dumpIt();
          #endif
        #endif
      } else if ( ((*data>>24) & 0x6)==0 ) {
        Warning("execute()",
                "Evt: %i, Data follow, but no header",nEvt);
      } else {
        Warning("execute()",
                "Evt: %i, Unknow raw data type %i",
                nEvt,((*data>>24) & 0x6));
      }
    } //end of data
  }
  return 1;
}






