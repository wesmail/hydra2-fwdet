//*-- Author : Walter Karig 
//*-- Modified : 05/06/98 by Manuel Sanchez
//*-- Modified : 28/5/2000 by R. Holzmann
//
using namespace std;
#include "htofunpacker99.h"
#include "htofraw.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include <iostream> 
#include <iomanip>

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////
//HTofUnpacker99
//
//  This is the HUnpacker used to read Nov 99 TOF data from LMD files
//
//////////////////////////////////

HTofUnpacker99::HTofUnpacker99(void) {
  pRawCat=NULL;
  pTofData = NULL;
}

HTofUnpacker99::~HTofUnpacker99(void) {
}

Bool_t HTofUnpacker99::init(void) {
  pRawCat=gHades->getCurrentEvent()->getCategory(catTofRaw);
  if (!pRawCat) {
    pRawCat=gHades->getSetup()->getDetector("Tof")->buildCategory(catTofRaw);
    if (!pRawCat) {
      return kFALSE;
    } else {
      gHades->getCurrentEvent()->addCategory(catTofRaw,pRawCat,"Tof");
    }
  }
  return kTRUE;
}

Int_t HTofUnpacker99::execute() {

  Int_t i = fill();
  return i;
}


Int_t HTofUnpacker99::fill() {

  //  cout << "In HTofUnpacker99::fill()" << endl;

  if(pTofData) {
    delete [] pTofData;
    pTofData = NULL;
  }

  //  if(pSubEvt) cout << "pSubEvt exists" << endl;
  //  else cout << "pSubEvt doesn't exist" << endl;


  if(pSubEvt) {

    UInt_t channels,geo;
    UInt_t tdcCh,time;
    Int_t cursor;
    Int_t j;
    UInt_t tdcCounter;

    HLocation loc;
    loc.set(3,0,0,0);

    Int_t length = pSubEvt->getEnd() - pSubEvt->getData();

    //    printf(" length = %i \n",length);  

    pTofData = new UInt_t[length];
    UInt_t *pointer = pSubEvt->getData();
    for(j=0;j<length;j++) pTofData[j] = pointer[j];
    cursor = 0;

//
// start of loop over TOF TDC's inside TOF subevent
//

    tdcCounter = 0;

    while(cursor<length) {
      geo = (UInt_t) byte(1,pTofData[cursor]) >> 3;
      channels = (UInt_t) byte(3,pTofData[cursor++]);

      //   cout<<"geo = "<<geo<<"  channels = "<<channels<<"  chanEnd ="<<
      //   	(UInt_t)(byte(3,pTofData[cursor+channels]))<<endl;


   /*if(channels!=((UInt_t)(byte(3,pTofData[cursor+channels])))) {
	cout << "Error in TDC" << geo << " has happened" << endl;
	return 0;
      }*/

      tdcCh = 0;
      time = 0;

//
//start of loop over data words inside TOF TDC
//

      for(j=cursor; j < (Int_t)(cursor+channels); j++) {
	tdcCh = (tdcCh | (pTofData[j] & 0x001f0000)) >> 16;
        time = (pTofData[j] & 0xfff ) % 0xfff;


	//	loc[0] = 3;


//for September test
	
	if(geo==3||geo==4||geo==5||geo==6) loc[0] = 2; 
	if(geo==8||geo==9||geo==10||geo==11) loc[0] = 5;

	if(geo==3||geo==5||geo==8||geo==10) loc[1] = tdcCh/8;
	if(geo==4||geo==6||geo==9||geo==11) loc[1] = tdcCh/8 + 4;

	loc[2] = tdcCh%8;

	//      	cout<<"tdcCh = "<<tdcCh<<"   time = "<<time<<endl;

/*
//        if(geo==5||geo==7) loc[1] = tdcCh/8;

	if(geo==7) {
	  if(tdcCh/8==0 || tdcCh/8==2) loc[1] = 2;
	  if(tdcCh/8==1 || tdcCh/8==3) loc[1] = 3;
	}

	if(geo==6||geo==8) loc[1] = tdcCh/8 + 4;

	loc[2] = tdcCh%8;
*/


	//	if(HTofRaw *pCell= (HTofRaw*) pRawCat->getSlot(loc)) {

	HTofRaw *pCell = (HTofRaw*) pRawCat->getObject(loc);

        if (pCell == NULL) { 
	  pCell = (HTofRaw*) pRawCat->getSlot(loc);
	  if (pCell != NULL) {
	    pCell = new(pCell) HTofRaw;   // needed to get a streamer
	                                    // for HTree::Fill() 
	    pCell->setRightTime(0.0);
	    pCell->setLeftTime(0.0);
	    pCell->setRightCharge(0.0);
	    pCell->setLeftCharge(0.0);
	    pCell->setSector((Char_t) loc[0]);
	    pCell->setModule((Char_t)loc[1]);
	    pCell->setCell((Char_t)loc[2]);
	  } else return kFALSE;
        }

	if(pCell != NULL) {

/*	  if(geo==6) {
	    pCell->setLeftTime((Float_t) time);
            pCell->setModule((Char_t)loc[1]);
            pCell->setCell((Char_t)loc[2]);
	  }

	  if(geo==8){
	    pCell->setRightTime((Float_t) time);
            pCell->setModule((Char_t)loc[1]);
            pCell->setCell((Char_t)loc[2]);
	  }

	  if(geo==7) {

	    if(tdcCh/8==0||tdcCh/8==1) {
	      pCell->setLeftTime((Float_t) time);
	      pCell->setModule((Char_t)loc[1]);
	      pCell->setCell((Char_t)loc[2]);
	    }

	    if(tdcCh/8==2||tdcCh/8==3) {
	      pCell->setRightTime((Float_t) time);
	      pCell->setModule((Char_t)loc[1]);
	      pCell->setCell((Char_t)loc[2]);
	    }
	  }*/

	  if(geo==3||geo==4||geo==8||geo==9) {
	    pCell->setRightTime((Float_t) time);
	    pCell->setSector((Char_t) loc[0]);
	    pCell->setModule((Char_t) loc[1]);
	    pCell->setCell((Char_t) loc[2]);
	  }

	  if(geo==5||geo==6||geo==10||geo==11) {
	    pCell->setLeftTime((Float_t) time);
	    pCell->setSector((Char_t) loc[0]);
	    pCell->setModule((Char_t) loc[1]);
	    pCell->setCell((Char_t) loc[2]);
	  }

	}

//
//end of loop over data words inside TOF TDC
//

      }	

      cursor += channels + 1;
      tdcCounter++;

//
//end of loop over TOF TDC's inside TOF subevent
//

    }

  }  

return kTRUE;

}           

ClassImp(HTofUnpacker99) 








