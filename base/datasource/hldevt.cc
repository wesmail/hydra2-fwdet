// File: hldevt.cc
// Modified bt D.Bertini 19/03/99
//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////
//HldEvent
//
//  Class used for reading LMD events from file
//
/////////////////////////////////////////////////////
using namespace std;
#include <iomanip>
#include <iostream>


#include "hldevt.h"
#include "hldsubevt.h"

Bool_t HldEvt::scanHdr(void) {
  //Checks event header. Returns kTRUE if correct.
  Bool_t r=kTRUE;
  if (getId()==0 || getSize()>100000) {
    r=kFALSE;
    printf("Event Corrupted\n");
    dumpHdr();
  }
  
  return r;
}

Int_t HldEvt::appendSubEvtIdx(HldUnpack& unpack) {
  subEvtTable[lastSubEvtIdx].id = unpack.getSubEvtId();
  subEvtTable[lastSubEvtIdx].p = unpack.getpSubEvt();
  if (lastSubEvtIdx == maxSubEvtIdx - 1) {
    printf("\nMax. nb of unpackers (%d) exceeded!\n\n",(Int_t)maxSubEvtIdx);
    return 0;
  } else return ++lastSubEvtIdx;
}


void HldEvt::dumpHdr() const {
  cout.fill('0');
  cout << "Event: "
       << "RunNr: " << getRunNr() 
       << "\tDate: "
       << setw(2) << getYear() << '/'
       << setw(2) << getMonth() << '/'
       << setw(2) << getDay() << ' '
       << setw(2) << getHour() << ':'
       << setw(2) << getMinute() << ':'
       << setw(2) << getSecond() << endl;

  cout << "Header: "   
       << "SeqNr: " << getSeqNr()
       << "\tId: " << getId()
       << "\tSize: " << getSize() 
       << "\tDecoding: " << getDecoding() << endl;
  

}

//ClassImp(HldEvt)  





























