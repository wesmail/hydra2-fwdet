// File: hparasciifileiio.h

/////////////////////////////////////////////////////////////////////////
// Interface class ascii file                                          //
// derived from "abstact" interface class HParIo                       //
//                                                                     //
// contains pointers to interface classes for every detector and to    //
// the file                                                            //
/////////////////////////////////////////////////////////////////////////

#ifndef HPARASCIFILEIIO_H
#define HPARASCIFILEIIO_H
using namespace std;
#include <iostream> 
#include <iomanip>
#include <string.h>
#include "hpario.h"
#include "TList.h"
#include <fstream> 

class HParAsciiFileIo : public HParIo {
protected:
  fstream* file;  // pointer to a file
public:
  HParAsciiFileIo();

  // default destructor closes an open file and deletes list of I/Os
  ~HParAsciiFileIo();

  // opens file
  // if a file is already open, this file will be closed
  // activates detector I/Os
  Bool_t open(const Text_t* fname,const Text_t* status="in");

  // closes file
  void close();

  // returns kTRUE if file is open
  Bool_t check() {
    if (file) return (file->rdbuf()->is_open()==1);
    else return kFALSE;
  }
   
  // prints information about the file and the detector I/Os
  void print();

  fstream* getFile();

  ClassDef(HParAsciiFileIo,0) // Parameter I/O from Ascii files
};

#endif  /* !HPARASCIIFILEIO_H */

