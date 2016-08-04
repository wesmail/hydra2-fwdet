//*-- AUTHOR : Denis Bertini
//*-- Last modified : 21/05/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HParAsciiFileIo
//
// Interface class for parameter I/O from ASCII file
// derived from the interface base class HParIo
// 
// It contains pointers to the ASCII file and to the interface classes for all
// detectors defined in the actual setup.
//
// This class was created by renaming the former class HParAsciFileIo.
// 
///////////////////////////////////////////////////////////////////////////////

#include "hparasciifileio.h"
#include "hdetpario.h"
#include "hades.h"
#include "hspectrometer.h"
#include <string.h>

ClassImp(HParAsciiFileIo)

HParAsciiFileIo::HParAsciiFileIo() {
  // default destructor
  file=0;
}

HParAsciiFileIo::~HParAsciiFileIo() {
  // default destructor closes an open file and deletes list of I/Os
  close();
}

Bool_t HParAsciiFileIo::open(const Text_t* fname,const Text_t* status) {
  // opens file
  // if a file is already open, this file will be closed
  // activates detector I/Os
  close();
  if(!((strcmp(status,"in")==0) || (strcmp(status,"out")==0))){
    printf(" put the right stream option for file :%2s\n",fname);
    printf(" writing state : out \n");
    printf(" reading state : in  \n");
    printf(" open  aborted \n");
    return kFALSE;
  }

  file=new fstream();
  if(strcmp(status,"in")==0){file->open( fname, ios::in);};
  if(strcmp(status,"out")==0){file->open( fname, ios::out);};

  filebuf* buf = file->rdbuf();
  if (file && (buf->is_open()==1)) {
    inputName=fname;
    gHades->getSetup()->activateParIo(this);
    return kTRUE;
  }
  return kFALSE;
}

void HParAsciiFileIo::close() {
  // closes the file and deletes the detector I/Os
  if (file) {
    file->close();
    delete file;
    file=0;
    inputName="";
  }
  if (detParIoList) detParIoList->Delete();
}

void HParAsciiFileIo::print() {
  // prints information about the file and the detector I/Os
  if (check()) {
    printf("HParAsciiFile*          %s\n",inputName.Data());
    TIter next(detParIoList);
    HDetParIo* io;
    cout<<"detector I/Os: ";
    while ((io=(HDetParIo*)next())) {
      cout<<" "<<io->GetName();
    }
    cout<<'\n';
  }
  else printf("No pointer to file\n");
}

fstream* HParAsciiFileIo::getFile() {
  // returns the file pointer
  return file;
}
