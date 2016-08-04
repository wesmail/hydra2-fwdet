//
//
// Last change: 30.9.10  M. Weber

#include "htrb2unpacker.h"
#include "hdatasource.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
using namespace std;

#define REF_DIFF 100

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//                     HTrb2Unpacker
//            Base class for TRB data unpacking.
//  Any detector connected to TRB board should use this class to decode the subevent. 
//  Decode() method fills the arrays with information about: 
//  channel number (0-127), 
//  measured time, 
//  measured width.
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
//  and width is set to -1. 
//
//  The information in Arrays is ordered in following way:
//  1. Leading Time has to exist -> fill trbLeadingTime[ TrbChanNb ][ hitNb ];
//  2. If Trailing time is found we try to marge it with last Leading Time.
//     If Trailing time does not exist trbLeadingTime[ TrbChanNb ][ hitNb ] = -1000000
//     If more than 1 Trailing time exist for one Leading Time the last one is used.
//  3. Width is calculated only if Trailing time exist. 
//
//  Max. number of hits per channel is currently set to 10. 
//
//  Decode:
//  The decode function returns dfferent values for different cases:
//    100  - end of subevent, stop decoding by purpose
//     10  - CTS information -> go to next subsubevt
//      0  - error, decoding failed
//      1  - data  decode 
//
//
//  Special :
//
//  Try recover wrong blocksize information ( like corrupted data beam apr12, day 116)
//  To fixes are implemented and active by default.
//  The fixes (tryRecover_1,tryRecover_2, see decode())
//  can be enabled/disabled by  :   void tryRecover(Bool_t case1=kTRUE,Bool_t case2=kTRUE);
//
//
//  In addition this class should perform basic data corrections and error handling.
//  (in preparation)
/////////////////////////////////////////////////////////////////////////////////


ClassImp(HTrb2Unpacker)

HTrb2Unpacker::HTrb2Unpacker(UInt_t id)  {

  subEvtId=id;

  debugFlag       = 0;
  uTrbNetAdress   = 0;
  trbDataVer      = 0;
  trbinlcorr      = 0;
  trbaddressmap   = 0;
  uStartPosition  = 0;
  uSubBlockSize   = 0;
  nCountWords     = 0;

  correctINL      = kFALSE;
  correctINLboard = kFALSE;
  highResModeOn   = kFALSE;
  quietMode       = kFALSE;
  reportCritical  = kFALSE;
  trbDataPairFlag = kFALSE;

  tryRecover_1=kTRUE;
  tryRecover_2=kTRUE;

  clearAll();
}

void HTrb2Unpacker::clearAll(void) {

  for(Int_t i=0; i<128; i++){
    for(Int_t k=0; k<10; k++){
      
      trbLeadingTime[i][k]     = -1000000;
      trbTrailingTime[i][k]    = -1000000;
      trbADC[i][k]           = -1; 
      
     }  
    
    trbDataExtension[i]     = 0;
    trbLeadingMult[i]       = 0;  
    trbTrailingMult[i]      = 0;  
    trbTrailingTotalMult[i] = 0;
  }
  
  highResModeOn    = kFALSE;
  correctINLboard  = kFALSE;

}

Int_t HTrb2Unpacker::correctRefTimeCh(Int_t refChan) {

  ///////////////////////////////////////////
  // Reference signal from channel <refCh> of TDC chip
  // is used for correction of corresponding TDC chns 
  //
  // Example:
  // for <refChan> = 32 (low resolution mode)
  // correct chns 0-31   with chn 31
  // correct chns 31-63  with chn 63
  // correct chns 64-95  with chn 95
  // correct chns 96-127 with chn 127
  //
  //
  // for <refChan> = 8 (high resolution mode)
  // correct chns 0-7   with chn 7
  // correct chns 8-15  with chn 15
  // correct chns 16-23 with chn 23
  // correct chns 24-31 with chn 31
  //  
  //
  // Also physical timing signal on reference channel
  // --> compare all 4 reference channels and get 
  // timing signal which is in all channels (+- <REF_DIFF> bins)
  //
  // Call this function only if channel <refCh> contains
  // reference time.
  //
  // If now refernce time is found all arrays will be set to default
  ///////////////////////////////////////////

  Int_t nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

  Float_t corrTime[4][10];
  Float_t refTime[4] = {- 1000000,- 1000000,- 1000000,- 1000000};

  Int_t refCh        = -1;
  Int_t jo           = -1; 

  Bool_t bRefTime    = kFALSE;
  Bool_t flag        = kFALSE;

  Int_t refDiff      = REF_DIFF;
  
  // veto needs bigger window
  // --> Parameter container ???
  if(refChan==8) refDiff = 1000;

  // scan all arrays which contain time data and do correction
  for(Int_t jj=0; jj<4; jj++) {  

    // First check if there is ANY data on a TDC, if not, skip it
    flag   = kFALSE;
    jo     = jj*refChan;
    for(Int_t ll=0; ll<refChan; ll++) { 
      if( trbLeadingMult[jo+ll]>0){
	flag = kTRUE; 
	break;
      }
    }
    if(!flag) continue;

    //If there is data, check reference channel
    flag  = kFALSE;
    refCh = jo + refChan-1;

    for(Int_t kk=0; kk<trbLeadingMult[refCh]; kk++) {

	corrTime[jj][kk] = trbLeadingTime[refCh][kk];

      // Now find the common correction time (only for j > 0)
      //
      // first we have to compare all leading times of TDC chip 0 
      // with TDC chip 1 to get the reference in 0
      if(jj == 1 ){
	for(Int_t mm=0; mm<trbLeadingMult[refChan-1]; mm++) {
	  
	  refTime[0] = corrTime[0][mm];

	  // same time if inside +- <REF_DIFF> bins
	  if( refTime[0] - corrTime[jj][kk] > -refDiff &&
	      refTime[0] - corrTime[jj][kk] < refDiff){
	    
	    refTime[jj]   =  corrTime[jj][kk];
	    
	    flag = kTRUE;
	    break;
	  }
	}
      }
      
      // for TDC chip 2 and 3 we compare to found reference time in chip 0
      else if(jj > 1){
	if( refTime[0] - corrTime[jj][kk] > -refDiff &&
	    refTime[0] - corrTime[jj][kk] < refDiff){
	
	  refTime[jj]   =  corrTime[jj][kk];
	  flag = kTRUE;
	}
      }
    }
  


    // if one TDC chip fails, there is no reference time for this event
    if(jj > 0 ){ 
      if(flag) bRefTime = kTRUE;
      else{
	bRefTime = kFALSE;
	break;
      }
    }
    
  }


  // If NO reference time -> set all to default values
  if(bRefTime == kFALSE ){
      if(!quietMode) {
	  if( !gHades->isCalibration() ){
	      Warning("correctRefTimeCh","Event %d --> No Ref Time! SubEvtID 0x%x, TRBid 0x%x",nEvt,subEvtId,uTrbNetAdress);
	      Warning("reference times:","%f %f %f %f",refTime[0],refTime[1],refTime[2],refTime[3]);
	  }
      }
    clearAll();
  }

  // ELSE do the correction 
  else{
    for(Int_t jj=0; jj<4; jj++) {  
      jo=jj*refChan;
      for(Int_t ll=0; ll<refChan; ll++) {  
	Int_t ii;
	ii=jo+ll;
	for(Int_t kk=0; kk<trbLeadingMult[ii]; kk++) {

	  trbLeadingTime[ii][kk]  = trbLeadingTime[ii][kk] - refTime[jj];   
	  trbTrailingTime[ii][kk] = trbTrailingTime[ii][kk] - refTime[jj];  

	}
      }
    }
  }

  return 0;
}

Int_t HTrb2Unpacker::correctRefTimeCh31(void) {

  ///////////////////////////////////////////
  // Reference signal from channel 31 of TDC
  // is used for correction of corresponding TDC chns
  // this is not the final version of
  // hardware design
  // call this function only if channel 31 contains
  // reference time
  ///////////////////////////////////////////

  Bool_t flag      = kFALSE;
  Int_t refCh      = -1; 
  Int_t jo         = -1;
  Float_t corrTime = 0.;


  // scan all arrays which contain time data and do correction
  for(Int_t jj=0; jj<4; jj++) {      

    // First check if there is ANY data on a TDC, if not, skip it
    flag  = kFALSE;
    jo    = jj*32;
    for(Int_t ll=0; ll<32; ll++) {  
      if( trbLeadingMult[jo+ll]>0){
	flag = kTRUE;
	break;
      }
    }
    if(!flag) continue;

    //If there is data, check reference channel
    refCh    = jo + 31;
    corrTime = trbLeadingTime[refCh][0];


    // If NO reference time -> set all to default values
    if(corrTime <= -1000000 ){
      if(!quietMode ) {
	if( !gHades->isCalibration() )Info("correctRefTimeCh","No Ref Time! SubEvtID 0x%x, TRBid 0x%x",subEvtId,uTrbNetAdress);
      }
      clearAll();
    }
    
    // ELSE do the correction 
    else{
      for(Int_t ll=0; ll<32; ll++) {  // For all TDC channels
	Int_t ii;
	ii=jo+ll;
	for(Int_t kk=0; kk<10; kk++) {
	  trbLeadingTime[ii][kk]  = trbLeadingTime[ii][kk]  - corrTime + 40000;// We want poitive times here; 20000 ~ 2us
	  trbTrailingTime[ii][kk] = trbTrailingTime[ii][kk] - corrTime + 40000;// thats bigger than max time window of TDC
	}
      }
    }
  }
  
  return 0;
}

Int_t HTrb2Unpacker::correctRefTimeCh127(void) {

  ///////////////////////////////////////////
  // Reference signal from channel 127 (last in TRB)
  // ist used for correction of ALL TDCs
  // this is not the final version of
  // hardware design
  // call this function only if channel 127 contains
  // reference time
  ///////////////////////////////////////////

  Float_t corrTime = 0.;;

  corrTime = trbLeadingTime[127][0];

  // If NO reference time -> set all to default values
  if(corrTime <= -1000000 ){
    if(!quietMode) {
      if( !gHades->isCalibration() )Info("correctRefTimeCh","No Ref Time! SubEvtID 0x%x, TRBid 0x%x",subEvtId,uTrbNetAdress);
    }
    clearAll();
  }
  
  // ELSE do the correction 
  else{
    for(Int_t jj=0; jj<128; jj++) {  // For all channels
      for(Int_t kk=0; kk<10; kk++) {
	trbLeadingTime[jj][kk]  = trbLeadingTime[jj][kk] - corrTime + 40000;// We want poitive times here; 20000 ~ 2us
	trbTrailingTime[jj][kk] = trbTrailingTime[jj][kk] - corrTime + 40000;// thats bigger than max time window of TDC
      }
    }
  }
  return 0;
}

Int_t HTrb2Unpacker::correctRefTimeStartDet23(void) {

  ///////////////////////////////////////////
  // Reference signal from channel 23 of TDC chip
  // is used for correction of corresponding TDC chns 
  //
  ///////////////////////////////////////////

  Float_t corrTime = trbLeadingTime[23][0];
  
  // If NO reference time -> set all to default values
  if(corrTime <= -1000000 ){
    if(!quietMode) {
      if( !gHades->isCalibration() )Info("correctRefTimeStartDet23","No Ref Time! SubEvtID 0x%x, TRBid 0x%x",subEvtId,uTrbNetAdress);
    }
    clearAll();
  }
  
  // ELSE do the correction 
  else{
    for(Int_t ii=0; ii<32; ii++) {  
      for(Int_t kk=0; kk<trbLeadingMult[ii]; kk++) {
	trbLeadingTime[ii][kk]  = trbLeadingTime[ii][kk] - corrTime;   
	trbTrailingTime[ii][kk] = trbTrailingTime[ii][kk] - corrTime;  
      }
    }
  }
  
  return 0;
}

Int_t HTrb2Unpacker::correctOverflow(void) {

  ///////////////////////////////////////////
  // TDC overflow has to be corrected
  ///////////////////////////////////////////

  // This is not necessary if the substract window bit is set in the TDCs

  return 0;
}


Bool_t HTrb2Unpacker::fill_pair(Int_t ch,Float_t time,Float_t length) {

  ///////////////////////////////////////////
  // Stores the given time in the next data element
  // and sets the multiplicity.
  // Return kFALSE if 10 hits are already stored.
  ///////////////////////////////////////////

  if( trbLeadingMult[ch]<10) {
    trbLeadingTime[ch][trbLeadingMult[ch]]=time;
    trbTrailingTime[ch][trbLeadingMult[ch]]=time+length;
  }
  trbLeadingMult[ch]++;
  trbTrailingMult[ch]=trbLeadingMult[ch];
  return (trbLeadingMult[ch]<=10);

}

Bool_t HTrb2Unpacker::fill_lead(Int_t ch, Float_t time) {

  ///////////////////////////////////////////
  // Stores the given time in the next data element
  // and sets the multiplicity.
  // Return kFALSE if 10 hits are already stored.
  ///////////////////////////////////////////

  if( trbLeadingMult[ch]<10){
    trbLeadingTime[ch][trbLeadingMult[ch]]=time;
  }
  trbLeadingMult[ch]++;
  return(trbLeadingMult[ch]<=10);
}


Bool_t HTrb2Unpacker::fill_trail(Int_t ch, Float_t time) {

  ///////////////////////////////////////////
  // Calculates the time between trailing and LAST(!) leading hit.
  // No other check if its really the right one,
  // i am depending on the TDC to deliver the right order
  // Return kFALSE if no leading yet or more than 4 Hits
  ///////////////////////////////////////////

  trbTrailingTotalMult[ch]++;
   
  Int_t m = -1;     // Leading Multiplicity
  m=trbLeadingMult[ch];
  if(m==0) return kFALSE;

  if( m<=10) {
    if( trbTrailingMult[ch]!=m) {
      trbTrailingTime[ch][m-1]=time;
      trbADC[ch][m-1] = time - trbLeadingTime[ch][m-1];
      if(trbADC[ch][m-1]<0) { // either a big error or an overflow because off too big time windows
	trbADC[ch][m-1]+=0x80000; // correct for overflow
      }
    } else {
      return kFALSE; // In this case we already have one trailing
    }
  }
  trbTrailingMult[ch]=m;

  return(trbTrailingMult[ch]<=10);
}


// correct INL of HPTDC for a single DATA word  
Float_t HTrb2Unpacker::doINLCorrection(Int_t nTrbCh, Int_t nRawTime)
{  
  Int_t nTimeHigh, nTimeLow;
  Float_t fCorrectedTime;

  if(correctINL == kTRUE  && correctINLboard == kTRUE ){
    if( highResModeOn == kTRUE ){            //correct INL for 25ps/channel
      
      nTimeHigh = ( nRawTime & 0x1FFC00);  // 11 MSB
      nTimeLow  = ( nRawTime & 0x003ff );  // 10 LSB 
      
      fCorrectedTime = (Float_t)( (Float_t)nTimeLow  +
				  ( trbinlcorr->getCorrection( (nTrbCh*4)+(Int_t)(nTimeLow / 256.0), (nTimeLow % 256) ) ));
      fCorrectedTime = (Float_t)( (Float_t)nTimeHigh + fCorrectedTime);
      
      if(debugFlag > 0){
	cout<<"==TRB_Res_25ps, chan: "<<nTrbCh<<", measured time: "<<nRawTime<<", corrected:"<<fCorrectedTime<<
	  ", correction:"<<( trbinlcorr->getCorrection( (nTrbCh*4)+(Int_t)(nTimeLow / 256.0), (nTimeLow % 256) ) )
	    <<"  --BIN: "<<nTimeLow<<" newCh: "<<(nTrbCh*4)+(Int_t)(nTimeLow / 256.0)<<" binInParHist: "<<( (nTimeLow % 256)  )<< endl;
      }
    } 
    
    else{      //correct INL for 100ps/channel
      
      nTimeHigh = ( nRawTime & 0x7ff00 );
      nTimeLow  = ( nRawTime & 0x000ff );  
      fCorrectedTime = (Float_t)( (Float_t)nTimeLow  +
				  ( trbinlcorr->getCorrection( nTrbCh , nTimeLow ) ));
      fCorrectedTime = (Float_t)( (Float_t)nTimeHigh + fCorrectedTime);
      
      if(debugFlag > 0){
	cout<<"==chan: "<<nTrbCh<<", measured time: "<<nRawTime<<", corrected:"<<fCorrectedTime<<
	  ", correction:"<<( trbinlcorr->getCorrection( nTrbCh , nTimeLow ) )<<endl;
      }
    }
  }
  
  //no INL correction will be done
  else{  
    fCorrectedTime = (Float_t)nRawTime;
  }
  
  return fCorrectedTime;
}



void HTrb2Unpacker::printTdcError(UInt_t e, UInt_t subEvntId) {
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

Int_t HTrb2Unpacker::decode(void) {

  clearAll();

  UInt_t nEvt         = 0;  // Evt SeqNumber
  UInt_t nSizeCounter = 0;  // should go to the class member !!!!!
  UInt_t nTdcEvtId    = 0;
  UInt_t TdcDataLen   = 0;

  Int_t TdcId         = 0;

  uSubBlockSize    = 0;
  uTrbNetAdress    = 0;
  trbDataVer       = 0;
  trbExtensionSize = 0;
  trbDataPairFlag  = kFALSE;

  Bool_t wasRecovered=kFALSE;


  UInt_t* data = pSubEvt->getData();
  UInt_t* end  = pSubEvt->getEnd();
  nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();


  if(debugFlag > 0) {
    cout<<"-0-EvNb "<<nEvt<<" -nSizeCounter: "<<nSizeCounter<<", "<<hex<<"data word: "<<*data<<dec<<"  --NetSubevent(TRB) Header"<<endl;
  }

  // jump to current SubSubEvent
  data = data + uStartPosition; 
  UInt_t* data_c1 = data; // copy for TRB extension loop
  UInt_t* data_c2 = data; // copy for TDC loop

  if(debugFlag > 0){
    cout<<"-0a-EvNb "<<nEvt<<" -uStartPosition: "<<uStartPosition<<", "<<hex<<"data word: "<<*data<<dec<<"  --"<<endl;
  }

  uSubBlockSize=(*data>>16)&0xFFFF;   // No. of the data words from one TRB board
  uTrbNetAdress=*data&0xFFFF;         // TRB net Adddress (Old TRB subevent id)

  if(uSubBlockSize > (UInt_t)(end-data)){
      if(tryRecover_1)Error("decode()","Event %d --> SubBlkSize out of subevt. Blksize=%d. Try to recover.",nEvt,uSubBlockSize);
      else            Error("decode()","Event %d --> SubBlkSize out of subevt. Blksize=%d.",nEvt,uSubBlockSize);

      if(!tryRecover_1) {
	  uStartPosition=0;
	  return 0;
      }

      //-----------------------------------------------------------------------------
      // try to recover the data by looping forward until next
      // valid address is found. (apr12 beam, day 116)
      Int_t        ct= 0;
      Bool_t reCover = kFALSE;

      while(data<end){ // still inside subevent
	  ct++;
          data +=1;
	  uSubBlockSize=(*data>>16)&0xFFFF;   // No. of the data words from one TRB board
          uTrbNetAdress=*data&0xFFFF;         // TRB net Adddress (Old TRB subevent id)
	  uStartPosition+=1;

	  if( ((   uTrbNetAdress>=0x4000
	        && uTrbNetAdress<=0x5000 )
                || uTrbNetAdress==0x5555)
	     && uSubBlockSize>0
	     && uSubBlockSize<200){

	      data_c1 = data;
              data_c2 = data;
	      reCover = kTRUE;
              wasRecovered=kTRUE;
	      break;
	  }
      }
      if(!reCover) { // bad luck
	  cout<<"Recover failed."<<endl;
	  uStartPosition = 0;
	  return 0;
      }
      //-----------------------------------------------------------------------------

    }


  // check TRBNetAdress
  // IF TRBNet debug information
  if ( uTrbNetAdress == 0x5555 ) {
    decodeTrbNet(data,subEvtId);
    data+=(uSubBlockSize)+1;

    uStartPosition = 0; // reset the Start position for the next Event
    if( debugFlag > 0){
      cout << "++++> stopping decode(), dataword: " << hex << *data << dec << endl;
    }
    return 100;
  }

  // IF CTS -> skip & 
  // skip if its a hub in the 0x8000 range (added by JanM 20140430
  if(uTrbNetAdress <= 0x00FF || ((uTrbNetAdress & 0xF000) == 0x8000)){
    return 10;
  }

  // ELSE unpack TRB - TDC data
  // ------------------------- 
  // 1. Should INL correction be done ?
  // -------------------------
  if (correctINL == kTRUE){

    trbinlcorr = trbaddressmap->getBoard(uTrbNetAdress);
    
    // if there is no mapping table it is not possible to decide if the INL correction should be done
    if(trbinlcorr == NULL ){
      Error("decode()","Event %d -->  No mapping table data for TRB net adress 0x%x",nEvt,uTrbNetAdress);
      return kFALSE;
    }
    
    else{
      
      // decide if INL correction should be done for this board
      if (trbinlcorr->getNChannels() > 0  ){
	correctINLboard = kTRUE;
      }
      else{
	if( debugFlag > 0) Warning("decode()","Event %d -->  No INL corr data for TRB net adress 0x%x",nEvt,uTrbNetAdress);
	correctINLboard = kFALSE;
      }
    }
  }

  // ------------------------- 
  // 2. control the SubSubEvent size
  // -------------------------
  UInt_t uSubBlockSizeTest = (*(data+1)&0xFFFF);
  if(uSubBlockSize != uSubBlockSizeTest){
    if(tryRecover_2)Error("decode","Event %d -->  SubBlkSize from TRBnet ( %04x ) != SubBlkSize from subsubevt ( %04x ). Try to recover.",nEvt,uSubBlockSize,*(data+1)&0xFFFF);
    else            Error("decode","Event %d -->  SubBlkSize from TRBnet ( %04x ) != SubBlkSize from subsubevt ( %04x )",nEvt,uSubBlockSize,*(data+1)&0xFFFF);

    if(!tryRecover_2) {
	uStartPosition=0;
	return 0;
    }


    //-----------------------------------------------------------------------------
    // try to recover the data by looping forward until next
    // valid address is found and correct the uSubBlockSize  (apr12 beam, day 116)
    UInt_t        ct= 0;
    Bool_t reCover = kFALSE;

    UInt_t* data2=data;
    UInt_t  uSubBlockSize2;
    UInt_t  uTrbNetAdress2;
    UInt_t  uStartPosition2=uStartPosition;
    while(data2<end){ // still inside subevent
	ct++;
	data2 +=1;
	uSubBlockSize2=(*data2>>16)&0xFFFF;   // No. of the data words from one TRB board
	uTrbNetAdress2=*data2&0xFFFF;         // TRB net Adddress (Old TRB subevent id)
	uStartPosition2+=1;

	if( ((   uTrbNetAdress2>=0x4000
	      && uTrbNetAdress2<=0x5000 )
	     || uTrbNetAdress2==0x5555)
	   && uSubBlockSize2>0
	   && uSubBlockSize2<200){

	    if(ct == uSubBlockSizeTest+1) {

		uSubBlockSize =uSubBlockSizeTest;
		reCover = kTRUE;
		wasRecovered=kTRUE;
                break;
	    }
	    if(ct == uSubBlockSize+1) {

		reCover = kTRUE;
		wasRecovered=kTRUE;
                break;
	    }
	}
    }
    if(!reCover) {
	cout<<"Recover failed."<<endl;
	uStartPosition = 0;
	return 0;
    }
    //-----------------------------------------------------------------------------
  }
  
  if( debugFlag > 0){
    cout<<"-1a-EvNb "<<nEvt<<" -nSizeCounter: "<<nSizeCounter<<", "<<hex<<"data word: "<<*data<<dec
	<<" SubSize: "<<uSubBlockSize<< hex << " TrbNetAdress "<<uTrbNetAdress<<dec<<endl;
  }
  
  // ------------------------- 
  // 3. Loop over the TRB extension
  // ------------------------- 
  data_c1+=2;// go to the dataword where TRB extension is defined
  trbDataVer = (*data_c1>>24)&0xFF;
  trbDataPairFlag = (*data_c1>>16)&0x1;
  trbExtensionSize = *data_c1&0x0000FFFF;

  if((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode)) {
    Info("decode()","Event %d -->  Unpacker TrbHeader: %08x  Ver:%d  Ext:%d Pair:%d \n",nEvt,*data_c1,trbDataVer,trbExtensionSize,(Int_t)trbDataPairFlag);
  }
  if( trbExtensionSize != 0 ) {
    for ( UInt_t ii = 0; ii < trbExtensionSize; ii++ ) {

      data_c1++;    
      if(trbExtensionSize < 128) {
	trbDataExtension[ii] = *data_c1; 
      } else{
	Error("decode()","Event %d -->  SubEventId = 0x%x (TRB ID 0x%x ) too many data words (%d), limit is 128",nEvt,subEvtId,uTrbNetAdress,trbExtensionSize);
	return 0;
      }
    }
  }
  
  // ------------------------- 
  // 4. set resolution mode 
  // data_c1 =  AATTXXX
  // AA = 0   high resolution (100 ps)
  // AA = 1   very  high resolution (25 ps)
  // AA = 2   very  high resolution (25 ps) - calibration
  // ------------------------- 
  if( trbDataVer == 0 ){
    highResModeOn = kFALSE;
  }
  else if( trbDataVer == 1 ){
    highResModeOn = kTRUE;
  }    
  else{
    Warning("decode()","Event %d -->  Unknown trbDataVer ( 0x%x ) for TRB net adress 0x%x",nEvt,trbDataVer,uTrbNetAdress);
  }
  
  if( debugFlag > 0 ) cout<<" High Resolution mode: "<<highResModeOn<< hex << " for TRB net adress: "<<uTrbNetAdress<<dec<<endl;



  // ------------------------- 
  // 5. Loop over TDCs
  // ------------------------- 
  data_c2+=(2+trbExtensionSize);// jump to last TrbHeader dataword
  for ( UInt_t ii = 0; ii < uSubBlockSize-trbExtensionSize-2; ii++ ) { 
    data_c2++;// start decoding after TRB extension datawords
    nSizeCounter++;

    UInt_t dataword = *data_c2;
    if(debugFlag > 0) cout<<"--EvNb "<<nEvt<<" data_"<<nSizeCounter<<": "<<hex<<dataword<<dec<<endl;

    if(dataword==0xDEADFACE) {
      if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
        Info("decode()","Event %d -->  Found DEADFACE -> break %08X %08X\n",nEvt,*data,*end);
      }
      break;
    }

    TdcId=(dataword>>24)&0xF;      // might be wrong for TRB board

    if(TdcDataLen>0) TdcDataLen++;
    
    // ------------------------- 
    // 6. check data word TYPE
    // ------------------------- 
    switch(dataword>>28) { 
      case 0: { // Group Header
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Event %d --> Found GLOBAL Header $%08X\n",nEvt,dataword);
        } 
        if(!quietMode) Error("decode()","Event %d --> Global Header not expected!",nEvt);
        if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic

	break;
      }

      case 1: { // Group Trailer
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Event %d -->  Found GLOBAL Trailer $%08X\n",nEvt,dataword);
        }
	if(!quietMode) Error("decode()","Event %d --> Global Trailer not expected!",nEvt);
	if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic
        break;
      }

      case 2: { // TDC Header
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Event %d -->  Found TDC %d Header $%04X $%04X\n",nEvt,TdcId,(dataword>>12)&0xFFF,dataword&0xFFF);
        }
        if( TdcId>0 && nTdcEvtId!=((dataword>>12)&0xFFF)) {
          if(!quietMode) {
            Error("TRB unpack","Event %d -->  TDCs have different EventIds ******* Event Mixing *******",nEvt);
            printf("nTdcEvtId: %06X   dataword:  %06X  nEvt: %02X\n" , nTdcEvtId ,((dataword>>12)&0xFFF),nEvt);
	  }
          if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic
	}
        if( nEvt!=((dataword>>12)&0xFF)) {
          if(!quietMode) {
	    //      TDC Trigger TAG problem, To be checked !!!!
	    //      Error("TRB unpack","Event %d -->  TDC EventIds != Main EventId ******* Event Mixing *******");
	    //      printf("SubEvtId: %06X nTdcEvtId: %06X   dataword:  %06X  nEvt: %02X\n" ,subEvtId, nTdcEvtId ,((dataword>>12)&0xFFF),nEvt);
          }
	  //          exit();
	  //          return(kFALSE);
        }
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","nTdcEvtId: %06X   dataword:  %06X  nEvt: %02X\n" , nTdcEvtId ,((dataword>>12)&0xFFF),nEvt);
        }
        nTdcEvtId=(dataword>>12)&0xFFF;

        TdcDataLen=1;
        break;
      }

      case 3: { // TDC Trailer
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Found TDC %d Trailer $%04X $%04X\n",TdcId,(dataword>>12)&0xFFF,dataword&0xFFF);
        }
        if(TdcDataLen!=(dataword&0xFFF)) {
	    if(!quietMode) Error("decode()","Event %d -->  TRB unpack : TdcDataLen!= length in Trailer!",nEvt);
           if(!quietMode) printf("TRB unpack: TdcDataLen %d != %d ",TdcDataLen,dataword&0xFFF);
           if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic
	}
        TdcDataLen=0;
        if(nTdcEvtId!=((dataword>>12)&0xFFF)) {
          if(!quietMode) Error("TRB unpack","Event %d -->  TDC Header and Trailer have different EventIds",nEvt);
//          exit();
//          return(kFALSE);
        }
        break;
      }

      case 4: { // TDC DATA Leading
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Event %d -->  Found TDC %d Lead Data $%08X\n",nEvt,TdcId,dataword);
        }
        Int_t nData, nChannel;
        Int_t nDataLow, nDataHigh;            

        if(!highResModeOn) { // 100ps binning
          nChannel=(dataword>>19)&0x1f; // decode channel
          nChannel+=TdcId*32;
          nData=dataword&0x7ffff;   // decode 19bit data
          if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
            Info("decode()","Event %d --> (Chan,Data,InlCorrData) %3d, %d, %f \n",nEvt,nChannel,nData, doINLCorrection(nChannel, nData));
	  }
	  if(nChannel > 127){
	      Error("decode","Event %d -->  channel number bigger than 128",nEvt);
	      if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic
	      return 0;
	  }
        } 
	else {  // 25ps binning mode
          nChannel=(dataword>>21)&0x7; // decode channel
          nChannel+=TdcId*8;
          nDataHigh = (dataword&0x7ffff)<<2;
          nDataLow  = (dataword>>19)&0x3;
          nData     = nDataHigh + nDataLow;  
	  
          if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
            Info("decode()","Event %d -->  (Chan,Data) %3d, %d\n",nEvt,nChannel,nData);
          }
	  if(nChannel > 31){
	      Error("decode","Event %d -->  channel number bigger than 31",nEvt);
	      if(wasRecovered) return kDsSkip; // in case of a repaired event we skip if data are problematic
	      return 0;
	  }
        }
	
        // this is for SINGLE LEADING/TRAILING EDGE measurements only!!!
        // No check if the order is correct!!!
        // i am depending on the TDC to deliver the right order
        if(trbDataPairFlag) {
          if(!fill_pair(nChannel,(Float_t)(nData&0xFFFF),(Float_t)((nData>>12)&0x7F))) {
	  }
        } 
	else { // Leading as usual
          // fill INL corrected time 
          if(!fill_lead(nChannel, doINLCorrection(nChannel, nData))) {
          }
        }
        break;
      }
    case 5: { // TDC DATA Trailing
        if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
          Info("decode()","Event %d --> Found TDC %d Trail Data $%08X\n",nEvt,TdcId,dataword);
        } 
        Int_t nData, nChannel;
        Int_t nDataLow, nDataHigh;
        if(!highResModeOn){ //100ps binning
          nChannel=(dataword>>19)&0x1f; // decode channel
          nChannel+=TdcId*32;
          nData=dataword&0x7ffff;   // decode 19bit data
          if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
            Info("decode()","Event %d --> (Chan,Data,InlCorrData) %3d, %d, %f \n",nEvt,nChannel,nData, doINLCorrection(nChannel, nData));
          }
        } else {  // 25ps binning mode
          nChannel=(dataword>>21)&0x7; // decode channel
          nChannel+=TdcId*8;
          nDataHigh = (dataword&0x7ffff)<<2;
          nDataLow  = (dataword>>19)&0x3;
          nData     = nDataHigh + nDataLow;
          if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
            Info("decode()","Event %d --> (Chan,Data) %3d, %d\n",nEvt,nChannel,nData);
          }
        }
	
        // this is for SINGLE LEADING/TRAILING EDGE measurements only!!!
        // No check if the order is correct!!!
        // i am depending on the TDC to deliver the right order
	
        // fill INL corrected time 
        if(!fill_trail(nChannel, doINLCorrection(nChannel, nData) )) {
	  
        }
        break;
    }
    case 6:{// TDC ERROR
        if((dataword&0x7FFF)==0x1000) {// special case for non fatal errors
          if(!quietMode)Error("TRB unpack","Event %d -->  TDC Event Size Limit exceeded!\n",nEvt);
          if(!quietMode)printf("(TDC %d Error Event Size Limit: $%08X)\n",TdcId,dataword);
        } else {
          if(!quietMode) {
            Error("TRB unpack","Event %d -->  Found TDC Error(s)!\n",nEvt);
            printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
            printTdcError(dataword&0x7FFF,subEvtId);
          } else if(reportCritical && (dataword&0x6000)!=0) {
            Error("TRB unpack","Event %d -->  Found CRITICAL error",nEvt);
            if((dataword&0x2000)!=0) {
              Error("TRB unpack","Event %d -->  Event lost (trigger FIFO overflow)",nEvt);
              printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
            } else if((dataword&0x4000)!=0) {
              Error("TRB unpack","Event %d -->  Internal fatal chip error has been detected",nEvt);  
              printf("TDC %d Error $%04X ($%08X)\n",TdcId,dataword&0x7FFF,dataword);
            }
          }
        }
        break;
    }
    case 7: { // Debug Info
      if(!quietMode)Error("TRB unpack","Event %d -->  Found DEBUG Info",nEvt);
      if(!quietMode)printf("TRB unpack: TDC %d: Found Debug Info $%08X",TdcId,dataword);
      break;
    }
    default: {// not defined!
      if(!quietMode) Error("TRB unpack","Event %d -->  Found UNDEFINED data",nEvt);
      if(!quietMode) printf("TRB unpack: TDC %d: Found undefined $%08X",TdcId,dataword);
      break;
    }
    }
  }// end of for-loop over TRB datawords


  if(uSubBlockSize-trbExtensionSize-2!=nSizeCounter) {
    cout<<" --Block size ="<<uSubBlockSize<<" , counter Size: "<<nSizeCounter <<endl; 
    if(!quietMode)Error("TRB unpack","Event %d -->  Blocksize!=Counted words!!!",nEvt);
  }
  if( ((debugFlag > 0 || DEBUG_LEVEL>4) && (!quietMode))) {
    Info("decode()","Event %d --> ===Unpacker end (SubEvtID 0x%x , TRB ID 0x%x ) ===================\n",nEvt,subEvtId,uTrbNetAdress);
  }
  return(kTRUE);
}
