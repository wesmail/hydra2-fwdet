//
//
// Last change: 19.4.06  B. Spruck

#include "htrbbaseunpacker.h"
#include "htrblookup.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
using namespace std;

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//                     HTrbBaseUnpacker
//            Base class for TRB data unpacking.
//  Any detector connected to TRB board should use this class to decode the subevent. 
//  Decode() method fills the arrays with information about: 
//  channel number (0-127), 
//  measured time, 
//  measured ADC.
//
//  The goal is to provide data which can be directly used by subsystems 
//  (tof, start, hodo, wall) in order to directly fill raw categories.
//
//  The most important information is stored in:	   
//  trbLeadingTime[ TrbChanNb ][ hitNb ],
//  trbADC[ TrbChanNb ][ hitNb ]. 
//  trbLeadingMult[ TrbChanNb ]
//
//  In case of Leading and Trailing mode the leading time and trailing time 
//  are delivered in separate data words. In this mode it can happen that 
//  a trailing time is missing. In such case trailing time is set to -1000000 
//  and ADC is set to -1. 
//
//  The information in Arrays is ordered in following way:
//  1. Leading Time has to exist -> fill trbLeadingTime[ TrbChanNb ][ hitNb ];
//  2. If Trailing time is found we try to marge it with last Leading Time.
//     If Trailing time does not exist trbLeadingTime[ TrbChanNb ][ hitNb ] = -1000000
//     If more than 1 Trailing time exist for one Leading Time the last one is used.
//  3. ADC is calculated only if Trailing time exist. 
//
//  Max. number of hits per channel is currently set to 10. 
//
//
//  In addition this class should perform basic data corrections and error handling.
//  (in preparation)
/////////////////////////////////////////////////////////////////////////////////


ClassImp(HTrbBaseUnpacker)

HTrbBaseUnpacker::HTrbBaseUnpacker(UInt_t id)  {
  // constructor
  debugFlag=0;
  subEvtId=id;
  trbDataVer=0;
  quietMode=false;
  reportCritical=false;
  lookup=0;
  clearAll();
}

void HTrbBaseUnpacker::clearAll(void){
  for(Int_t i=0; i<128; i++){
    for(Int_t k=0; k<10; k++){
      trbLeadingTime[i][k]  = -1000000;
      trbTrailingTime[i][k] = -1000000;
      trbADC[i][k]=-1; 
    }    
  //--  trbDataExtension[i]=0;
    trbLeadingMult[i]=0;  
    trbTrailingMult[i]=0;  
    trbTrailingTotalMult[i]=0;
  }
}

Int_t HTrbBaseUnpacker::correctRefTimeCh31(void){
  ///////////////////////////////////////////
  //
  // Reference signal from channel 31 of TDC
  // ist used for correction of corresponding TDC chns
  // this is not the final version of
  // hardware design
  // call this function only if channel 31 contains
  // reference time
  ///////////////////////////////////////////
  Int_t refCh; //= (Int_t)(chan / 32); // this will give one of the channels
  Int_t corrTime;
  // now do correction
  // scann all arrays which contain time data and do correction
  // Corr. Times are  stored in:
  // trbLeadingTime[refCh][0]  first hit !!!
  // where refCh = 31, 63, 95, 127;

  for(Int_t jj=0; jj<4; jj++){// For all TDC chips
    Int_t jo;// j offset
    Bool_t flag;
    // First check if there is ANY data on a TDC, if not, skip it
    flag=false;
    jo=jj*32;
    for(Int_t ll=0; ll<32; ll++){// For all TDC channels
      if( trbLeadingMult[jo+ll]>0){ flag=true; break;}
    }
    if(!flag) continue;

    // Yes there is data, now check reference
    refCh = jo+31;// gives ref Channel back

    corrTime = trbLeadingTime[refCh][0];

    // Think about it! no ref time means -> reject, other suggestions?
    if(corrTime <= -1000000 ){
//#warning "This Info message should be active for the beamtime!!!!!"
      if(!quietMode){
        Info("correctRefTimeCh31","No Ref Time! TRBid 0x%x TDC %d",subEvtId,jj);
      }
      corrTime=-1000000;// very big negative ... this means we do not have ref. time
//      continue; // this means we do not have ref. time
    }

    for(Int_t ll=0; ll<32; ll++){// For all TDC channels
      Int_t ii;
      ii=jo+ll;
      for(Int_t kk=0; kk<10; kk++){
        trbLeadingTime[ii][kk]  = trbLeadingTime[ii][kk] - corrTime + 40000;// We want poitive times here; 20000 ~ 2us
        trbTrailingTime[ii][kk] = trbTrailingTime[ii][kk] - corrTime + 40000;// thats bigger than max time window of TDC
      }
    }
  }

  return 0;
}

Int_t HTrbBaseUnpacker::correctRefTimeCh127(void){
  ///////////////////////////////////////////
  //
  // Reference signal from channel 127 (last in TRB)
  // ist used for correction of ALL TDCs
  // this is not the final version of
  // hardware design
  // call this function only if channel 127 contains
  // reference time
  ///////////////////////////////////////////
  Int_t corrTime;
  // now do correction
  // trbLeadingTime[127][0]  first hit !!!

  corrTime = trbLeadingTime[127][0];
  
  // Think about it! no ref time means -> reject, other suggestions?
  if(corrTime <= -1000000 ){
//#warning "This Info message should be active for the beamtime!!!!!"
    if(!quietMode){
      Info("correctRefTimeCh127","No Ref Time! TRBid 0x%x",subEvtId);
    }
      corrTime=-1000000;// very big negative ... this means we do not have ref. time
//    return 0; // this means we do not have ref. time
  }

  for(Int_t jj=0; jj<128; jj++){// For all channels
    for(Int_t kk=0; kk<10; kk++){
      trbLeadingTime[jj][kk]  = trbLeadingTime[jj][kk] - corrTime + 40000;// We want poitive times here; 20000 ~ 2us
      trbTrailingTime[jj][kk] = trbTrailingTime[jj][kk] - corrTime + 40000;// thats bigger than max time window of TDC
    }
  }

  return 0;
}

Int_t HTrbBaseUnpacker::correctOverflow(void){
  ///////////////////////////////////////////
  // TDC overflow has to be corrected
  ///////////////////////////////////////////
  // This is not necessary if the substract window bit is set in the TDCs
  return 0;
}


Bool_t HTrbBaseUnpacker::fill_pair(Int_t ch,Int_t time,Int_t length)
{
   ///////////////////////////////////////////
   // Stores the given time in the next data element
   // and sets the multiplicity.
   // Return false if 10 hits are already stored.
   ///////////////////////////////////////////

   if( trbLeadingMult[ch]<10){
      trbLeadingTime[ch][trbLeadingMult[ch]]=time;
      trbTrailingTime[ch][trbLeadingMult[ch]]=time+length;
   }
   trbLeadingMult[ch]++;
   trbTrailingMult[ch]=trbLeadingMult[ch];

   return(trbLeadingMult[ch]<=10);
}

Bool_t HTrbBaseUnpacker::fill_lead(Int_t ch,Int_t time)
{
   ///////////////////////////////////////////
   // Stores the given time in the next data element
   // and sets the multiplicity.
   // Return false if 10 hits are already stored.
   ///////////////////////////////////////////

   if( trbLeadingMult[ch]<10){
      trbLeadingTime[ch][trbLeadingMult[ch]]=time;
   }
   trbLeadingMult[ch]++;

   return(trbLeadingMult[ch]<=10);
}

Bool_t HTrbBaseUnpacker::fill_trail(Int_t ch,Int_t time)
{
   ///////////////////////////////////////////
   // Calculates the time between trailing and LAST(!) leading hit.
   // No other check if its really the right one,
   // i am depending on the TDC to deliver the right order
   // Return kFALSE if no leading yet or more than 4 Hits
   ///////////////////////////////////////////

   trbTrailingTotalMult[ch]++;
   
   Int_t m;// Leading Multiplicity
   m=trbLeadingMult[ch];
   if(m==0) return kFALSE;

   if( m<=10){
      if( trbTrailingMult[ch]!=m){
         trbTrailingTime[ch][m-1]=time;
         trbADC[ch][m-1] = time - trbLeadingTime[ch][m-1];
			if(trbADC[ch][m-1]<0){
				// cerr << "!!!!!!!!!!!!!!!!! WIDTH < 0 can not be!!! "<<time<<" - "<< trbLeadingTime[ch][m-1]<<endl;
				// this is eigther an big error or oan overflow because off too big time windows...
				// we guess the best
				trbADC[ch][m-1]+=0x80000;// correct for overflow
			}
      }else{
         return kFALSE;// In this case we already have one trailing
      }
   }
   trbTrailingMult[ch]=m;

   return(trbTrailingMult[ch]<=10);
}

void HTrbBaseUnpacker::PrintTdcError(UInt_t e, UInt_t subEvntId)
{
   const Char_t *e_str[15]={
      "Hit lost in group 0 from read-out FIFO overflow",
      "Hit lost in group 0 from L1 buffer overflow",
      "Hit error have been detected in group 0",
      "Hit lost in group 1 from read-out FIFO overflow",
      "Hit lost in group 1 from L1 buffer overflow",
      "Hit error have been detected in group 1",
      "Hit lost in group 2 from read-out FIFO overflow",
      "Hit lost in group 2 from L1 buffer overflow",
      "Hit error have been detected in group 2",
      "Hit lost in group 3 from read-out FIFO overflow",
      "Hit lost in group 3 from L1 buffer overflow",
      "Hit error have been detected in group 3",
      "Hits rejected because of programmed event size limit",
      "Event lost (trigger FIFO overflow)",
      "Internal fatal chip error has been detected"
   };

   if(e==0) return;// No Error

   cout << "=== TRB/TDC Error analysis: TRB id = " <<subEvntId<< endl;
   for(Int_t i=0; i<15; i++){
      if( e&0x1){
         cout << e_str[i] << endl;
      }
      e>>=1;
   }
   cout << "===" << endl;
}

Int_t HTrbBaseUnpacker::decode(void)
{
   clearAll();


   Int_t TdcId;
   Int_t nCountTDC = 0;

   UInt_t nEvtNr = 0;

   UInt_t nSizeCounter = 0;
   UInt_t nTdcEvtId = 0;
   UInt_t TdcDataLen=0;

   UInt_t uBlockSize=0;

   UInt_t* data = pSubEvt->getData();
   UInt_t* end  = pSubEvt->getEnd();

   if(debugFlag > 0){

      //Loop over all data in this subevt
      cout<<hex<<"data word: "<<*data<<dec<<"  --Subevent Header"<<endl;
   }

   // first word in subevent contains info about subevent size 0xbeef_6a_20 (magic,EvtId,length)
   // should be used for checking the subevent.

   if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
      printf("=== Unpacker start SubEvtId: %d\n===Header: %X  Nr:%d  Len:%d\n",subEvtId,*data>>16,nEvtNr,uBlockSize);
   }

   trbDataVer=0;
   trbExtensionSize=0;
   trbDataPairFlag=false;

   if(((*data >> 24) & 0xFF)!=0xBE) { // WE HAVE NEW DATA FORMAT 
                                      // check which data version we have (0xBE - old version)
     nSizeCounter++; // first header
     
     /* limit the size information from the TRB to 16bit length */ 
     uBlockSize=*data&0xFFFF;          
     nEvtNr=(*data>>16)&0xFF;
     data ++;
     nSizeCounter++;
     Int_t nData;
     trbDataVer = (*data>>24)&0xFF;
     trbDataPairFlag = (*data>>16)&0x1;
     trbExtensionSize = *data&0x0000FFFF;
     if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
       printf("=== Unpacker NewHeader: %08X  Ver:%d  Ext:%d Pair:%d\n",*data,trbDataVer,trbExtensionSize,(Int_t)trbDataPairFlag);
     }
     if(trbExtensionSize != 0){
       for(Int_t ext_cycle=0; ext_cycle<trbExtensionSize; ext_cycle++){
          data++;
          nSizeCounter++;
          nData = *data;
          if(trbExtensionSize < 128) {
             trbDataExtension[ext_cycle] = nData; 
          }
          else{
             cout<<"--TRBBASEUNPACKER-SubEventId= "<<subEvtId<<" too many data words, limit is 128"<<endl;
          }
       }
     }
   }
   else { // HERE WE HAVE OLD DATA FORMAT
   if((*data&0xFFFF0000)==0xBE010000){ 
      if(!quietMode)Error("TRB unpack","TRB Buffer overflow, Data truncated. No lethal error, but please report if error is persistend!!!");
      return(kFALSE);
   }

   // BEEF has been replaced ny BE00 before the Beamtime, I only need it for
   // analysis of test files ... remove after Beamtime
   if((*data&0xFFFF0000)!=0xBEEF0000 && (*data&0xFFFF0000)!=0xBE000000){
      if(!quietMode)Error("TRB unpack","NO $BEEF FOUND!!!");
//      exit();
   }
   uBlockSize=*data&0xFF;
   nEvtNr=(*data>>8)&0xFF;

   if(nEvtNr!=(UInt_t)pSubEvt->getTrigNr()){
       if(!quietMode){
	   Error("TRB unpack","TRB EvtNr!=pSubEvt->getTrigNr() ********* Event Mixing *********");
	   printf("=== %d != %d \n",nEvtNr,(UInt_t)pSubEvt->getTrigNr());
       }
      return(kFALSE);
   }

//   if(uBlockSize<10){
#warning "Change value to 10 if all TRBs have 4 TDCs after beamtime may06"
   if(uBlockSize<8){
      if(!quietMode)printf("Error in TRB unpack: Suspicious length (too small for header/trailer of tdcs) %d , might be a overflow!\n",uBlockSize);
   }

     nSizeCounter++;// First one already processed
   } // end of the OLD data format
   while ( ++data<end && *data!=0x0 )
   {
      UInt_t dataword;
      dataword=*data;//[ii];

      nSizeCounter++;

      if(dataword==0xDEADFACE){
         if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
            printf("TRB unpack: Found DEADFACE -> break %08X %08X\n",*data,*end);
         }
         break;
      }

      TdcId=(dataword>>24)&0xF;// might be wrong for TRB board
      TdcId=nCountTDC;


      if(TdcDataLen>0) TdcDataLen++;

      switch(dataword>>28){// Raw TDC Data
         case 0:{// Group Header
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found GLOBAL Header $%08X\n",dataword);
            } 
            if(!quietMode)Info("TRB unpack","Global Header not expected!");
            break;
         }
         case 1:{// Group Trailer
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found GLOBAL Trailer $%08X\n",dataword);
            }
            if(!quietMode)Info("TRB unpack","Global Trailer not expected!");
            break;
         }
         case 2:{// TDC Header
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found TDC %d Header $%04X $%04X\n",TdcId,(dataword>>12)&0xFFF,dataword&0xFFF);
            }
            if( nCountTDC>0 && nTdcEvtId!=((dataword>>12)&0xFFF)){
               if(!quietMode)Error("TRB unpack","TDCs have different EventIds ******* Event Mixing *******");
               if(!quietMode)printf("nTdcEvtId: %06X   dataword:  %06X  nEvtNr: %02X\n" , nTdcEvtId ,((dataword>>12)&0xFFF),nEvtNr);
//               exit();
//               return(kFALSE);
            }
            if( nEvtNr!=((dataword>>12)&0xFF)){
               if(!quietMode)Error("TRB unpack","TDC EventIds != Main EventId ******* Event Mixing *******");
               if(!quietMode)printf("nTdcEvtId: %06X   dataword:  %06X  nEvtNr: %02X\n" , nTdcEvtId ,((dataword>>12)&0xFFF),nEvtNr);
//               exit();
//               return(kFALSE);
            }
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("nTdcEvtId: %06X   dataword:  %06X  nEvtNr: %02X\n" , nTdcEvtId ,((dataword>>12)&0xFFF),nEvtNr);
            }
            nTdcEvtId=(dataword>>12)&0xFFF;

            TdcDataLen=1;
            break;
         }
         case 3:{// TDC Trailer
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found TDC %d Trailer $%04X $%04X\n",TdcId,(dataword>>12)&0xFFF,dataword&0xFFF);
            }
            if(TdcDataLen!=(dataword&0xFFF)){
               if(!quietMode)Error("TRB unpack","TdcDataLen!= length in Trailer!");
               if(!quietMode)printf("TRB unpack: TdcDataLen %d != %d ",TdcDataLen,dataword&0xFFF);
            }
            TdcDataLen=0;
            if( nTdcEvtId!=((dataword>>12)&0xFFF)){
               if(!quietMode)Error("TRB unpack","TDC Header and Trailer have different EventIds");
//               exit();
//               return(kFALSE);
            }
            nCountTDC++;
            break;
         }
         case 4:{// TDC DATA Leading
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found TDC %d Lead Data $%08X\n",TdcId,dataword);
            }
            Int_t nData, nChannel;
            nChannel=(dataword>>19)&0x1f; // decode channel
            nChannel+=TdcId*32;
            nData=dataword&0x7ffff;   // decode 19bit data
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("(Chan,Data) %3d, %d\n",nChannel,nData);
            }
            // this is for SINGLE LEADING/TRAILING EDGE measurements only!!!
            // No check if the order is correct!!!
            // i am depending on the TDC to deliver the right order
            if(trbDataPairFlag){
               if(!fill_pair(nChannel,nData&0xFFFF,(nData>>12)&0x7F)){
                  //printf("Too many Hits\n");
               }
            }else{// Leading as usual
               if(!fill_lead(nChannel,nData)){
                  //printf("Too many Hits\n");
               }
            }
            break;
         }
         case 5:{// TDC DATA Trailing
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("TRB unpack: Found TDC %d Trail Data $%08X\n",TdcId,dataword);
            } 
            Int_t nData, nChannel;
            nChannel=(dataword>>19)&0x1f; // decode channel
            nChannel+=TdcId*32;
            nData=dataword&0x7ffff;   // decode 19bit data
            if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
               printf("(Chan,Data) %3d, %d\n",nChannel,nData);
            }
            // this is for SINGLE LEADING/TRAILING EDGE measurements only!!!
            // No check if the order is correct!!!
            // i am depending on the TDC to deliver the right order
            if(!fill_trail(nChannel,nData)){
               //printf("Trailing without Leading or Too many Hits\n");
            }
            break;
         }
         case 6:{// TDC ERROR
            if((dataword&0x7FFF)==0x1000){// special case for non fatal errors
               if(!quietMode)Info("TRB unpack","TDC Event Size Limit exceeded!\n");
               if(!quietMode)printf("(TDC %d Error Event Size Limit: $%08X)\n",TdcId,dataword);
            }else{
               if(!quietMode)Info("TRB unpack","Found TDC Error(s)!\n");
               if(!quietMode)printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
               if(!quietMode){
                 PrintTdcError(dataword&0x7FFF,subEvtId);
               }else if(reportCritical && (dataword&0x6000)!=0){
                  Error("TRB unpack","Found CRITICAL error");
                  if((dataword&0x2000)!=0){
                    Error("TRB unpack","Event lost (trigger FIFO overflow)");
                    printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
                  }else if((dataword&0x4000)!=0){
                    printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
                    Error("TRB unpack","Internal fatal chip error has been detected");                    
                  }
               }
            }
            break;
         }
         case 7:{// Debug Info
            if(!quietMode)Error("TRB unpack","Found DEBUG Info");
            if(!quietMode)printf("TRB unpack: TDC %d: Found Debug Info $%08X",TdcId,dataword);
            break;
         }
         default:{// not defined!
            if(!quietMode)Error("TRB unpack","Found UNDEFINED data");
            if(!quietMode)printf("TRB unpack: TDC %d: Found undefined $%08X",TdcId,dataword);
            break;
         }
      }
   }

   if(nCountTDC!=4){
      if( nCountTDC<4){
#warning "Comment this in after may06 beamtime"
//         Error("TRB unpack","TDC count <4 -> TDC data missing!!!");
      }else{
         if(!quietMode)Error("TRB unpack","TDC count >4 -> additional TDC data!!!");
      }
   }

   if(uBlockSize!=nSizeCounter){
      cout<<" --Blok size ="<<uBlockSize<<" , counter Size: "<<nSizeCounter <<endl; 
      if(!quietMode)Error("TRB unpack","Blocksize!=Counted words!!!");
   }
   if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))){
      printf("==== Unpacker end (%d)\n\n",subEvtId);
   }
   return(kTRUE);
}
