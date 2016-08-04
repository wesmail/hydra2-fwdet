//

//////////////////////////////////////////////////////
//  HldFileEvent
//
//  Class for reading events from files
//
//////////////////////////////////////////////////////


#ifndef HLDFILEVT_H
#define HLDFILEVT_H

#include <fstream>
#include "hldevt.h"
#include "hldsubevt.h"
#include "hldunpack.h"
#include <iostream>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
// from times
#include <time.h>


class HldFilEvt: public HldEvt {

private:
std::istream* file;

struct stat status; //! status struct of hld file

public:
 HldFilEvt() {                    // No input file
   file = 0;
   init();
 }
 HldFilEvt(const Char_t* name) {    // read from file
   file = new std::ifstream(name);
   init();
 }
 ~HldFilEvt() {
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
 public:
//ClassDef(HldFilEvt, 0)           // HADES LMD Event
};
#endif /* !HLDFILEVT_H */  

