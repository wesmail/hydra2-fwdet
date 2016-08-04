//*-- Author : Walter Karig
//*-- Modified : 08/09/2003 by Tomasz Wojcik
//*-- Modified : 05/10/2001 by Wolfgang Koenig
//*-- Modified : 14/12/2000 by Ilse Koenig
//*-- Modified : 05/06/98 by Manuel Sanchez
//*-- Modified : 97/11/13 15:24:59 by Walter Karig
//
using namespace std;
#include "htofunpacker.h"
#include "tofdef.h"
#include "htoflookup.h"
#include "htofraw.h"
#include "htofrawsim.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "htofdetector.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include "hdatasource.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////
//HTofUnpacker
//
//  This is the unpacker used to read HTof data from LMD files
//
//////////////////////////////////

HTofUnpacker::HTofUnpacker(UInt_t id) {
  subEvtId = id;
  pRawCat=NULL;
  lookup=0;
}


Bool_t HTofUnpacker::init(void) {
  evCounter=0;
  errorCounter=0;
  pRawCat=gHades->getCurrentEvent()->getCategory(catTofRaw);
  if (!pRawCat) {
      if(gHades->getEmbeddingMode()==0) {
	  pRawCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofRaw);
      } else {
	  pRawCat=((HTofDetector*)(gHades->getSetup()->getDetector("Tof")))->buildMatrixCategory("HTofRawSim",0.5F);
      }
    if (!pRawCat) return kFALSE;
    gHades->getCurrentEvent()->addCategory(catTofRaw,pRawCat,"Tof");
  }
  loc.set(3,0,0,0);
  lookup=(HTofLookup*)(gHades->getRuntimeDb()
                             ->getContainer("TofLookup"));
  return kTRUE;
}


Int_t HTofUnpacker::execute() {
// This function fills the TOF raw data structure.
//
//---------------------------------------------------------------------
/*
November 2000 Subevent Format for TOF, TOFINO and START


The new subevent format for TOF and START/TOFINO will look like this.

The first four words are the usual subevent header:

 subEvtSize             size of subevent including header (bytes)
 subEvtDecoding         0x00020001
 subEvtId               TOF: 411   START: 511
 subEvtTrigNr           trigger number

The next 32 bit word contains the data word, which was sent by the trigger
distribution system to the readout board. It contains information about
the trigger type, which produced the event (normal, calibration...).

 dtuTrigWord            0x a1 02 00 01 (example)
                                    \/
                                     \current trigger tag (should always
                                      be equal to the 8 LSB of the
                                      subEvtTrigNr!)
                                 \/
                                  \always 0x00 (for future use)
                              \/
                               \trigger tag, which was distributed with
                                this trigger (will be the current trigger
                                tag of the following event, when triggered
                                positive)
                           \/
                            \trigger type, 4 MSB are always 0xa
                             4 LSB are: 0x1=normal trigger
                                        0x4=calibration
                             currently only these two trigger types are
                             in use, might be extended during this beamtime

Now come the "real" data, which is equivalent to the corresponding part of
the old subevent format.

Example for TOF:

 1st TDC:
 0x32002000             V878 header word (see v878 doku)
 0x30000fd7             V878 data word (channel 0)
 0x30010fb7             V878 data word (channel 1)
 0x30020f97             V878 data word (channel 2)
 ...
 0x301e0fb0             V878 data word (channel 30)
 0x301f0fb4             V878 data word (channel 31)
 0x34000002             V878 end-of-block word

 2nd TDC:
 0x3a002000             see above and see V878 doku for details
 0x38000fcb
 ...
 0x381f0f93
 0x3c000002

The number of TDC "blocks" depends on the number of TDC/ADCs mounted to
a certain crate. The number of V878 data words per TDC depends on the
number of channels fired (zero and overflow are suppressed). Anyhow, every
TDC/ADC will send at least the V878 header word and the end-of-block word
independently of the number of fired channels.

That's it, there is no end-of-event word or something like this. After the
last V878 end-of-block word the next subevent starts.

Here is a memory dump of my readout board. It contains one subevent marked
with [ ]. It's a subevent containing three TDCs.

TOF-RTU memory dump
------------------------------------------------------------
start address : 0x1000000
length [byte] : 0x100000
---------------------------------------------
 1000000:[000001ac 00020001 00000191 00000001
 1000010: a1020001 32002000 30000fd7 30010fb7
 1000020: 30020f97 30030f96 30040f89 30050f9f
 1000030: 30060f90 30070f7c 30080f89 30090f8e
 1000040: 300a0f8f 300b0f8a 300c0f97 300d0f91
 1000050: 300e0f96 300f0f87 30100f93 30110f8e
 1000060: 30120f94 30130f8e 30140f9c 30150f95
 1000070: 30160fa8 30170f97 30180fa8 30190fa9
 1000080: 301a0fad 301b0fa6 301c0fb5 301d0fae
 1000090: 301e0fb0 301f0fb4 34000002 3a002000
 10000a0: 38000fcb 38010fad 38020f86 38030f6b
 10000b0: 38040f7f 38050f80 38060f90 38070f6e
 10000c0: 38080f81 38090f80 380a0f8f 380b0f9a
 10000d0: 380c0f7a 380d0f7e 380e0f8a 380f0f74
 10000e0: 38100f84 38110f83 38120f82 38130f76
 10000f0: 38140f8a 38150f81 38160f89 38170f84
 1000100: 38180f90 38190f90 381a0f90 381b0f87
 1000110: 381c0f93 381d0f8e 381e0f8b 381f0f93
 1000120: 3c000002 42002000 40000fcf 40010fb1
 1000130: 40020f87 40030f7f 40040f88 40050f8d
 1000140: 40060f8c 40070f88 40080f86 40090f87
 1000150: 400a0f8c 400b0f97 400c0f84 400d0f87
 1000160: 400e0f8b 400f0f86 40100f8d 40110f92
 1000170: 40120f97 40130f8e 40140f94 40150f90
 1000180: 40160f9d 40170f9a 40180f97 40190fa6
 1000190: 401a0fa2 401b0f93 401c0f9b 401d0f94
 10001a0: 401e0fa6 401f0f93 44000002]000001ac
 10001b0: 00020001 00000191 00000002 a1030002
 10001c0: 32002000 30000fec 30010fd1 30020fae
 10001d0: 30030f94 30040f88 30050f9e 30060f97
 10001e0: 30070f7f 30080f89 30090f89 300a0f8d
 10001f0: 300b0f8c 300c0f97 300d0f90 300e0f96

*/
//---------------------------------------------------------------------
//Raw type (bits 24,25,26) in the header
//header = 2, trailer = 4, 0=valid dataword, 1=known bug, 6=invalid data
// !!!!!!!!!!!!!!!!!!!!!! Watch out !!!!!!!!!!!!!!!!!!!!!!
// The 1st bit of the raw type (bits 24,25,26) is undefined and
// should not be checked. The bug is in the manual, not in the data.
//
// A 'filler' word can show up in between a trailer and a header or at the end
// of the data transfer. It is identified by raw type 11?,
// where the ? refers to the undefined bit.
    HTofRaw* pRaw=0;
    Int_t nCrate=-1, nChannel=0,  nSector=-1, nMod=-1, nCell=-1;
    Int_t nEvt        = 0;    //Evt Id
    UInt_t   nSlot    = 0;
    UInt_t uWordCount = 0;    //auxiliary
    UInt_t uBlockSize = 0;    //Number of data words in one block
    UInt_t uBoardType = 0;    // 1 - Struck, 0 - TDC
    UInt_t *uTmp      = NULL; //auxiliary

    Char_t dcType='U', side='U';
	

    if( gHades->isCalibration()){
        //calibration event
        //Nothing to do for tof
        return 1;
    }
    if(pSubEvt) {
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
            if( ((*data >>24) & 0x6)==0 ){
                uBlockSize = (*data) & 0x0000003ff;
                uBoardType = (*data >> 12 ) & 0x1;
                //latch or scaler
                if(uBoardType==1){
                    data+=uBlockSize;
                    continue;
                }
                //next Should (can) be tdc header
                else if(uBoardType==0){
                    //Do a normal TDC scanning
                    #if DEBUG_LEVEL>2
                        printf("TIP-block 0x%08x Size %i Board  %i\n",
                                *data,uBlockSize,uBoardType);
                    #endif
                    continue;
                }
                else{
                    #if DEBUG_LEVEL>2
                        Warning("execute","Uknown BoardType=%i",uBoardType);
                    #endif
                }
            }
            ///Scan for TDC header
            else if ( ((*data>>24) & 0x6)==2 ){
                //Found valid TDC header
                uBlockSize = ( (*data) >> 8   ) & 0xff;
                nCrate     = ( (*data) >> 16  ) & 0xff;
                nSlot      = ( (*data) >> 27  ) & 0x1f;
                //Check if also trailer exists
                if ((*(data + uBlockSize+1) >>24 & 0x6)!=4){
                    Warning("execute","Header found but no trailer!!!");
                    continue;
                }
                //Check trigger tag in the trailer
                if ((*(data+uBlockSize+1)&0xff)!=( ((UInt_t)pSubEvt->getTrigNr() )&0xff ) ){
		    Error("execute","Trigger tag mismatch: EventSeqNumber= %i! Skip event....",nEvt);
                    if(!gHades->getForceNoSkip()) return kDsSkip;
                }
                //Scan all data words
                #if DEBUG_LEVEL>4
                    printf("TDC header 0x%08x.Size: %i.Crate: %i.Slot: %i.\n",
                        *data,uBlockSize,nCrate,nSlot);
                #endif
                uTmp=data;
                HTofLookupSlot* dc=lookup->getSlot(nCrate-1,nSlot-1);
                if (dc && (dcType=dc->getType())!='U') {
                    uWordCount=0;
                    while( ++data && data<(uTmp + uBlockSize+1) && *data!=0x0 ){
                        uWordCount++;
                        if(nSlot!=((*data>>27)&0x1f)){
                            Warning("execute",
                                "Slot (%d) different that in header(%d)",
                                nSlot,(*data>>27)&0x1f);
                            continue;
                        }
                        nChannel = (*(data) >> 16 ) & 0x003f;
                        HTofLookupChan* dcs=dc->getChannel(nChannel);
                        if (dcs){
                            dcs->getAddress(nSector,nMod,nCell,side);
                            if (nSector>=0 && nSector<6) {
                                loc[0]=nSector;
                                loc[1]=nMod;
                                loc[2]=nCell;
                                pRaw=(HTofRaw *)pRawCat->getObject(loc);
                                if (!pRaw){
                                    pRaw=(HTofRaw *)pRawCat->getSlot(loc);
				    if (pRaw) {

					if(gHades->getEmbeddingMode()==0) {
					    pRaw=new (pRaw) HTofRaw;
					}else{
					    pRaw=new (pRaw) HTofRawSim;
					}
                                        pRaw->setSector((Char_t)nSector);
                                        pRaw->setModule((Char_t)nMod);
                                        pRaw->setCell((Char_t)nCell);
                                    }
                                    else {
                                        Error("execute()",
                                                "Can't get slot\n");
                                        return -1;
                                    }
                                }
                                else {
                                    #if DEBUG_LEVEL>4
                                        Info("execute()",
                                            "Slot already exists!\n");
                                    #endif
                                }
                                Float_t val = (*data & 0xfff);
                                if (dcType=='T'){
                                    if(side=='R')
                                        pRaw->setRightTime(val);
                                    else
                                        pRaw->setLeftTime(val);

                                }
                                else {
                                    if(side=='R')
                                        pRaw->setRightCharge(val);
                                    else
                                        pRaw->setLeftCharge(val);
                                }
                            }
                            else {
                                #if DEBUG_LEVEL>4
                                    Warning("execute()",
                                        "Wrong address! Sector %i",
                                        nSector);
                                #endif
                            }
                        }//end dcs
                        else {
                            Info("execute()",
                                    "TDC channel: %i not found",nChannel);
                        }
                    }
                    if(uWordCount!=uBlockSize){
                        //Corrupted block
                        Warning("execute()",
                                "Found %i but expecting %i words!!!",
                                uWordCount,uBlockSize);
                        continue;
                    }
                }//TDC
                else{
                    //Skip this block
                    data+=uBlockSize+1;
                    #if DEBUG_LEVEL>4
                        Info("execute()",
                            "Can not get TDC for create %i and slot %i",
                            nCrate-1,nSlot-1);
                    #endif
                }
            }
            else if ( ((*data>>24) & 0x6)==6 ){
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
            }
            else if ( ((*data>>24) & 0x6)==0 ){
                Warning("execute()",
                        "Evt: %i, Data follow, but no header",nEvt);
            }
            else {
                Warning("execute()",
                        "Evt: %i, Unknow raw data type %i",
                        nEvt,((*data>>24) & 0x6));
            }
        }//end of data
    }
    return 1;
}

ClassImp(HTofUnpacker)





