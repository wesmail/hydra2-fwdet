// File: hldsubevt.cc
//
//*-- Author : Walter Karig <W.Karig@gsi.de>
//*-- Modified : 97/12/10 15:24:45 by Walter Karig
//*-- Modified : 25/03/98 by Manuel Sanchez Garcia
//*-- Modified : 19/03/99 by D.Bertini

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////
//HldSubEvent
//
//  This class contains the data of a LMD event which are not stored in
//  its header. That is the actual data for the mdc,rich,tof and shower
//
//////////////////////////////////////////////////////////
using namespace std;
#include "hldsubevt.h"
#include <iostream> 
#include <iomanip>
#include <stdlib.h>


void HldSubEvt::swapData() {
    if (wasSwapped()) {
	switch (getWordSize()) {
	case 4:
	    swap4((UInt_t*) getData(), getDataLen());
	    break;
	case 2:
	    swap2((UShort_t*) getData(), getDataLen());
	    break;
	case 1:
	    break;
	default:
	    cerr << "Word Size of " << getWordSize()
		 << " not supported" << endl;
	    exit(1);
	}
        resetSwapFlag();   // make sure, subevent is not swapped a second time
    }
} 


Bool_t HldSubEvt::scanIt(void) {
  //Checks the subevent for corruption. Returns kTRUE if correct.
  Bool_t r=kTRUE;
  
  if (getId()>699 || getId()<1) {
    r=kFALSE;
    printf("SubEvent corrupted\n");
  }
  return r;
}

void HldSubEvt::dumpIt(FILE* output) const{

  fprintf(output,"===========================================================\n");
  fprintf(output,"SubEvent: Id:%8i Size:%8i Decoding:%8i \n",  
	  getId(),
	  (Int_t)getSize(),
	  getDecoding()); 
  fprintf(output,"TrigNr:%8i\n", getTrigNr()); 
  fprintf(output,"===========================================================\n");
 
  UInt_t *data;
  data= getData();
  for(UInt_t i=0;i<getDataSize()/sizeof(UInt_t);i++){
    if(i % 4 == 0 ){ // newline with offset 
     fprintf(output,"\n0x%08x:",(UInt_t) ((Char_t *) data - (Char_t *) getData()));
    }
    fprintf(output,"  0x%08x", data[i]);
  }

  fprintf(output,"\n");
  fprintf(output,"\n");
}



