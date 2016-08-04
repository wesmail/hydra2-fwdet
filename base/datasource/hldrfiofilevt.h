//////////////////////////////////////////////////////
//  HldRfioFileEvent
//
//  Class for reading events from files on taperobot
//
//////////////////////////////////////////////////////

#ifndef HLDRFIOFILEVT_H
#define HLDRFIOFILEVT_H

#include "hldevt.h"
#include "hrfiofile.h"

class HldRFIOFilEvt: public HldEvt {

private:
		HRFIOFile *file; //! poiter to RFIO file

public:
 HldRFIOFilEvt() {                    // No input file
   file = 0;
   init();
 }
 HldRFIOFilEvt(const Char_t* name) {    // read from file
   file = new HRFIOFile(name,ios::in);
   init();
 }
 ~HldRFIOFilEvt() {
   if (file) delete file;
   delete[] pData;
 }
 void init() {
   pHdr = (UInt4*) &hdr;
   pData = 0;
   lastSubEvtIdx = 0;
 }
 Bool_t setFile(const Text_t *name);

 Bool_t readSubEvt(size_t i);
 Bool_t read();
 Bool_t execute();
 Bool_t swap();
 
 ClassDef(HldRFIOFilEvt,0)   // RFIO file event
};

#endif
