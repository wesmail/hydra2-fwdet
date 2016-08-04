//*-- Author : Ilse Koenig
//*-- Created: 21/02/2003

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HLdFileOutput
//
// Writes HLD events to a file
//
///////////////////////////////////////////////////////////////////////////////

#include "hldfileoutput.h"
#include "hldsource.h"
#include "hldevt.h"
#include <unistd.h>

ClassImp(HldFileOutput)

HldFileOutput::HldFileOutput(void) {
  // Default constructor
  fOption="NEW";
  fileSuffix="f_";
  evt=0;
  numTotal=0;
  numFiltered=0;
  UInt_t i[2]={0,0};
  fout=0;
  memcpy(padding,(UChar_t*)i,64);
}

HldFileOutput::HldFileOutput(HldSource* pSource,const Text_t* pDir,const Text_t* pSuffix,
                             const Option_t* pOption) {
  // Constructor accepts the data source, the file directory and the file option
  setHldSource(pSource);
  setDirectory(pDir);
  setFileSuffix(pSuffix);
  setFileOption(pOption);
  numTotal=0;
  numFiltered=0;
  UInt_t i[2]={0,0};
  fout=0;
  memcpy(padding,(UChar_t*)i,64);
}

HldFileOutput::~HldFileOutput(void) {
  // Destructor
  close();
}

void HldFileOutput::setHldSource(HldSource* pSource) {
  // Sets the pointer to the event in the datasource
  if (!pSource) {
    evt=0;    
    Error("setHldSource","HldSource not defined");
  } else {
    evt=pSource->fReadEvent;
    evt->setWritable();
  }
}

void HldFileOutput::setDirectory(const Text_t* pDir) {
  // Sets the output directory
  fDir=pDir;
  if (fDir.Length()>0 && !fDir.EndsWith("/")) fDir+='/'; 
}

void HldFileOutput::setFileSuffix(const Text_t* pSuffix) {
  // Sets the file suffix (default f_)
  if (strlen(pSuffix)>0) fileSuffix=pSuffix;
  else fileSuffix="f_";
}

void HldFileOutput::setFileOption(const Option_t* option) {
  // Sets the file option:
  // "CREATE" or "NEW": does not overwrite an existing file
  // "RECREATE":        overwrites an existing file
  if (strcmp(option,"CREATE")==0 || strcmp(option,"NEW")==0
        || strcmp(option,"RECREATE")==0) fOption=option;
  else {
    Error("setFileOption(Text_t*,Option_t*)","Invalid option!");
    fOption="NEW";
  }
}

Bool_t HldFileOutput::open(const Text_t* filename) {
  // Opens the output file
  close();
  numTotal=0;
  numFiltered=0;

  if(!fileSuffix.BeginsWith("."))
  {
      fileSuffix="."+fileSuffix;
  }
  TString fileName=filename;
  TString fn=fDir+filename;

  if(!fileName.EndsWith(fileSuffix.Data()))
  {
      fn+=fileSuffix;
  }

  if (fOption.CompareTo("CREATE")==0 || fOption.CompareTo("NEW")==0) {
    if (access(fn.Data(),F_OK)==0) {
      Error("open(Text_t*)","File %s exists already",fn.Data());
      return kFALSE;
    }
  }
  fout=new ofstream(fn.Data());
  if (!fout->is_open())
    Error("open(Text_t*)","File %s cannot be created",fn.Data());
  return fout->is_open() ;  
}

void HldFileOutput::close() {
  // Closes the output file
  if (fout) {
    if(fout->is_open()) fout->close();
    delete fout;
    fout=0;
  }
}

void HldFileOutput::writeEvent() {
  // Writes the event to the output file and increments the counters
  if (evt && fout && fout->is_open()) {
      if(evt->getData()) {
	  fout->write((Char_t*)evt->getHeader(),evt->getHdrSize());
	  fout->write((Char_t*)evt->getData(),evt->getDataSize());
	  fout->write((Char_t*)padding,evt->getPaddedSize()-evt->getSize());
	  numTotal++;
	  if ((evt->getId() & 0xf)==1) numFiltered++;
      } else {
        Warning("HldFileOutput::writeEvent()","NULL pointer for Data received, skipped!");
      }
  }
}
