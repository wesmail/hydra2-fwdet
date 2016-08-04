//*-- AUTHOR : Pavel Tlusty
//*-- Created : 15/06/2010
//*-- based on hstart2trb2unpacker

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HTofTrbUnpacker
//
//  Class for unpacking TRB data and filling
//    the Tof Raw category
//  Base subevent decoding is performed by decode function
//  see: htrbbaseunpacker.h (/base/datasource/ )
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "htoftrb2unpacker.h"
#include "tofdef.h"
#include "htofdetector.h"
#include "htoftrb2lookup.h"
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
#include "htofraw.h"
#include "htofrawsim.h"
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

ClassImp (HTofTrb2Unpacker)

HTofTrb2Unpacker::HTofTrb2Unpacker (UInt_t id):HTrb2Unpacker (id)
{
  // constructor
  pRawCat = NULL;
  timeRef = kTRUE;
  timeRefRemove = kFALSE;
  timeShift = 7200.;
}

Bool_t HTofTrb2Unpacker::init (void)
{

//  cout<<"====tof init ...0"<<endl;
  // creates the raw category and gets the pointer to the TRB lookup table
  pRawCat = gHades->getCurrentEvent ()->getCategory (catTofRaw);
  if (!pRawCat) {

      if(gHades->getEmbeddingMode()==0) {
	  pRawCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofRaw);
      } else {
	  pRawCat=((HTofDetector*)(gHades->getSetup()->getDetector("Tof")))->buildMatrixCategory("HTofRawSim",0.5F);
      }
      if (!pRawCat) return kFALSE;

      gHades->getCurrentEvent ()->addCategory (catTofRaw, pRawCat, "Tof");
  }
  loc.set(3,0,0,0);

  lookup = (HTofTrb2Lookup *) (gHades->getRuntimeDb ()->getContainer ("TofTrb2Lookup"));
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

Bool_t HTofTrb2Unpacker::finalize(void)
{

  if(!quietMode && debugFlag > 0 ){
    Info("finalize","No control histograms implemented");
  }
  return kTRUE;
}

Int_t HTofTrb2Unpacker::execute (void)
{
   HTofRaw *pRaw = 0;    // pointer to Raw category
   Int_t nEvt = 0;        // Evt Seq. Nb.


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
    //      0  - error, decoding failed
    //      1  - data  decode     
    // decode all subsubevents until subevent is finished - even if there is an error 
    Int_t DecodeOutput = 1; 
    while( 1 ) { 
      DecodeOutput = decode();
      // go to next TRB
      uStartPosition = uStartPosition + uSubBlockSize + 1; //start TRB decoding at the correct start position
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

//		cout << "***** TOF: trbDataExtension *****"<<endl;
//		cout << trbDataVer <<": "<< trbExtensionSize <<endl;


      //for (Int_t i = 0; i < 128; i++) {
      //  if(trbLeadingTime[i][0]>0) printf("%d %.0f %d\n",i,trbLeadingTime[i][0],trbLeadingMult[i]);
      //}

      //Ref Time Subtraction
      //cout <<"timeRef "<<timeRef<<endl;
      if(timeRef)  correctRefTimeCh(32);
    

      // get the part of the lookup table which belongs to the subevent ID
      // in order to check which TRBchannel belongs to the TofDetector

      HTofTrb2LookupBoard *board = lookup->getBoard (uTrbNetAdress);
      if (!board) {
         Warning ("execute", "TrbNetAdress: %x (%i) unpacked but TRB Board not in lookup table",uTrbNetAdress,uTrbNetAdress);
         return 1;
      }

      //fill RAW category for Tof detector
      for (Int_t i=0; i<board->getSize(); i++) {

	if (trbLeadingMult[i] < 1 ) continue;        //Leading Time data for this channel exist        

	if ( debugFlag > 0 ) cout <<"timeRefRemove "<<timeRefRemove<<" timeShift "<<timeShift<<endl;

        if(timeRefRemove) {  //remove reference times
          if(TMath::Abs(trbLeadingTime[i][0])<0.1) continue;
	}
        trbLeadingTime[i][0]=trbLeadingTime[i][0]+timeShift;  

	 if ( debugFlag > 0 ) printf("chan %d time %.0f adc %.0f\n",i,trbLeadingTime[i][0],trbADC[i][0]);

         // fill tof raw category
         HTofTrb2LookupChan *chan = board->getChannel (i);


         loc[0] = (Int_t)chan->getSector();
         loc[1] = (Int_t)chan->getModule();
         loc[2] = (Int_t)chan->getCell();
         Char_t side   = (Char_t)chan->getSide();

	 if ( debugFlag > 0 ) printf("chan %d sector %d module %d cell %d side %d\n",i,loc[0],loc[1],loc[2],side);

         if(loc[0]>=0) {

            pRaw=(HTofRaw *)pRawCat->getObject(loc);
            if (!pRaw){
              pRaw=(HTofRaw *)pRawCat->getSlot(loc);
	      if (pRaw) {

		  if(gHades->getEmbeddingMode()==0) {
		  pRaw=new (pRaw) HTofRaw;
	          }else{
		    pRaw=new (pRaw) HTofRawSim;
	          }
                  pRaw->setSector((Char_t)loc[0]);
                  pRaw->setModule((Char_t)loc[1]);
                  pRaw->setCell((Char_t)loc[2]);
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


            // fill only one time info !!!! 
            // todo: convert invalid time from unpacker -> invalid time in tof class ... all negative are invalid?
            Int_t m;
            m=trbLeadingMult[i];
            if(m>10) m=10;// no more than 10 Times stored
            
	    if(side=='r') {
                pRaw->setRightTime(trbLeadingTime[i][0]);
                pRaw->setRightCharge(trbADC[i][0]);
                }
	    else {
                pRaw->setLeftTime(trbLeadingTime[i][0]);
                pRaw->setLeftCharge(trbADC[i][0]);
                } 

	 } //sector>=0

      }// for(Int_t i=0; i<128; i++)
    } // while loop
   }//if(pSubEvt)

   return 1;
}

