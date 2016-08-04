//*-- AUTHOR : M.Golubeva
//*-- Created : 08/07/2010
//*-- based on htoftrb2unpacker (P.Tlusty)

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HWallTrb2Unpacker
//
//  Class for unpacking TRB data and filling
//    the Wall Raw category
//  Base subevent decoding is performed by decode function
//  see: htrbbaseunpacker.h (/base/datasource/ )
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hwalltrb2unpacker.h"
#include "walldef.h"
#include "hwalltrb2lookup.h"
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
#include "hwallraw.h"
#include "hwallrawsim.h"
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

#include "TCanvas.h"
#include "TStyle.h"

ClassImp (HWallTrb2Unpacker)

HWallTrb2Unpacker::HWallTrb2Unpacker (UInt_t id):HTrb2Unpacker (id)
{
  // constructor
  pRawCat = NULL;
  timeRef = kTRUE;
  timeRefRemove = kFALSE;
  timeShift = 7200.;
}

Bool_t HWallTrb2Unpacker::init (void)
{

  //  cout<<"====wall init ...0"<<endl;
  // creates the raw category and gets the pointer to the TRB lookup table
  pRawCat = gHades->getCurrentEvent ()->getCategory (catWallRaw);
  if (!pRawCat) {
    pRawCat = gHades->getSetup ()->getDetector ("Wall")->buildCategory (catWallRaw);
    if (!pRawCat)
      return kFALSE;
    gHades->getCurrentEvent ()->addCategory (catWallRaw, pRawCat, "Wall");
  }
  loc.set(1,0);

  lookup = (HWallTrb2Lookup *) (gHades->getRuntimeDb ()->getContainer ("WallTrb2Lookup"));
  if (!lookup) return kFALSE;

  
  if (correctINL) {
    trbaddressmap=(HTrbnetAddressMapping*)(gHades->getRuntimeDb()->getContainer("TrbnetAddressMapping"));
    if (!trbaddressmap) {
      Error("init","No Pointer to parameter container TrbnetAddressMapping.");
      return kFALSE;
    }
  }
  

  return kTRUE;
}

Bool_t HWallTrb2Unpacker::finalize(void)
{

  if(!quietMode &&  debugFlag > 0 ){
    Info("finalize","No control histograms implemented");
  }
  return kTRUE;
}

Int_t HWallTrb2Unpacker::execute (void)
{
  HWallRaw *pRaw = 0;    // pointer to Raw category
  Int_t nEvt = 0;        // Evt Seq. Nb.
  Int_t m=0;
  
  if (gHades->isCalibration ()) {
    //calibration event
    return 1;
  }

   if (gHades->getCurrentEvent()->getHeader()->getId() == 0xe) {
      //scaler event
      return 1;
   }

  // decode subevent data
  if (pSubEvt) {        // pSubEvt - make sure that there is something for decoding
    nEvt = gHades->getCurrentEvent ()->getHeader ()->getEventSeqNumber ();
    
    // decodes the subevent and fill arrays, see: htrb2unpacker.h
    // decode() return values: 
    //    100  - end of subevent, stop decoding by purpose
    //     10  - CTS are these Data in FW TDC?   Now inplemented/ Should be checked //BK
    //      0  - error, decoding failed
    //      1  - data  decode     
    // decode all subsubevents until subevent is finished - even if there is an error 
    Int_t DecodeOutput = 1; 
    while( 1 ) { 
      DecodeOutput = decode();
      // go to next TRB
      uStartPosition = uStartPosition + uSubBlockSize + 1; //start TRB decoding at the correct start position
      if (DecodeOutput==10) continue; //stop decoding CTS Event go to next //BK
      if (DecodeOutput==0) { //stop decoding of current TRB and go to next
	Error("decode","subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x",nEvt,subEvtId);
	continue;
      }
      if (DecodeOutput==100) { //End of subevebt is reached
        uStartPosition=0;
	if ( debugFlag > 0 ) Info("decode","DecodeOutput : %i",DecodeOutput);
	break;
      }
      
      // if(!decode()) return(-1); <--- for old data structure
      
      //		cout << "***** WALL: trbDataExtension *****"<<endl;
      //		cout << trbDataVer <<": "<< trbExtensionSize <<endl;
      
      
      //for (Int_t i = 0; i < 128; i++) {
      //  if(trbLeadingTime[i][0]>0) printf("%d %.0f %d\n",i,trbLeadingTime[i][0],trbLeadingMult[i]);
      //}
      
      //Ref Time Subtraction
      if(timeRef)  correctRefTimeCh(32);
      
      
      // get the part of the lookup table which belongs to the subevent ID
      // in order to check which TRBchannel belongs to the Wall detector
      
      HWallTrb2LookupBoard *board = lookup->getBoard (uTrbNetAdress);
      if (!board) {
         Warning ("execute", "TrbNetAdress: %x (%i) unpacked but TRB Board not in lookup table",uTrbNetAdress,uTrbNetAdress);
         return 1;
      }

      if(board->getSize() < 0) continue;   //BK
      //fill RAW category for Wall detector
      for (Int_t i=0; i<board->getSize(); i++) {
	//cout <<"MULT " <<trbLeadingMult[i] <<endl;
	if (trbLeadingMult[i] < 1 ) continue;        //Leading Time data for this channel exist        

	if ( debugFlag > 0 ) cout <<"timeRefRemove "<<timeRefRemove<<" timeShift "<<timeShift<<endl;

        if(timeRefRemove) {  //remove reference times
	  //cout <<"MARINA " <<trbLeadingTime[i][0] <<endl;
          if(TMath::Abs(trbLeadingTime[i][0])<0.1) continue;
	}
        trbLeadingTime[i][0]=trbLeadingTime[i][0]+timeShift;  

	if ( debugFlag > 0 ) printf("chan %d time %.0f adc %.0f\n",i,trbLeadingTime[i][0],trbADC[i][0]);

	// fill wall raw category
	if(i == -1) continue; 
	HWallTrb2LookupChan *chan = board->getChannel (i);
	
	loc[0] = (Int_t)chan->getCell();
	if ( debugFlag > 0 ) printf("chan %d cell %d\n",i,loc[0]);
	
	if(loc[0]>=0) {
	  //cout <<"CELL " <<loc[0] <<" " <<board->getSize() <<endl;
	  pRaw=(HWallRaw *)pRawCat->getObject(loc);
	  if (!pRaw){
	    pRaw=(HWallRaw *)pRawCat->getSlot(loc);
	    if (pRaw) {
	      
	      if(gHades->getEmbeddingMode()==0) {
		pRaw=new (pRaw) HWallRaw;
	      }else{
		pRaw=new (pRaw) HWallRawSim;
	      }
	      
	      pRaw->setCell(loc[0]);
	      
	      m=trbLeadingMult[i];
	      //cout <<"MARINA " <<m <<" " <<loc[0] <<endl; 
	      //if(loc[0]>225 && loc[0]<245) cout <<"MARINA " <<m <<" " <<trbTrailingMult[i] <<" " <<trbTrailingTime[i][0] <<" " <<trbLeadingTime[i][0] <<" " <<trbADC[i][0] <<endl;
	      
	      if(m>4) m=4; //no more than 4 Times stored
	      for(Int_t chmult=0; chmult<m; chmult++){
		//if(trbLeadingTime[i][chmult]>=lower &&  trbLeadingTime[i][chmult]<=upper){
		pRaw->fill( trbLeadingTime[i][chmult], trbADC[i][chmult]);
		//if(loc[0]>225 && loc[0]<245) cout <<"MARINA " <<chmult <<" " <<trbTrailingTime[i][chmult] <<" " <<trbLeadingTime[i][chmult] <<" " <<trbADC[i][chmult] <<endl;
		
		//}
	      }//for(Int_t chmult=0; chmult< trbLeadingMult[i]; chmult++)
	    }//if (pRaw)
	    else {
	      Error("execute()",
		    "Can't get slot\n");
	      return -1;
	    }
	  }//if (!pRaw)
	  else {
            #if DEBUG_LEVEL>4
	    Info("execute()",
		 "Slot already exists!\n");
            #endif
            }
	  
	 }//if(loc[0]>=0)  //sector>=0 

      }// for(Int_t i=0; i<128; i++)
    } // while loop
   }//if(pSubEvt)

   return 1;
}

