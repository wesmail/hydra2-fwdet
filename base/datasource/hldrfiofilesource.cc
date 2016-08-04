//*-- Author : J.Wuestenfeld
//*-- Modified : 11/19/2003 by J.Wuestenfeld

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////
//HLdRFIOFileSource
//
//  This "data source" reads the HLD data from files on the taperobot
//  Based on HldFileSource
//
//////////////////////////////////////////////////////////
using namespace std;
#include "hldrfiofilesource.h"
#include "hldrfiofilevt.h"
#include "hevent.h"
#include "hrecevent.h"
#include <unistd.h>

ClassImp(HldRFIOFileSource)

HldRFIOFileSource::HldRFIOFileSource(void)
	: HldFileSource()
{
	// Default constructor for a Lmd file data source.
	if(fReadEvent) delete fReadEvent;
  fReadEvent=new HldRFIOFilEvt();
	fCurrentRobot = "lxgstore";
	fCurrentArchive = "hadesraw";
}


Int_t HldRFIOFileSource::getRunId(const Text_t *fileName)
{
	// gets the runId of the specified file
  HldRFIOFilEvt evt;
  Int_t r=-1;

  if (fCurrentFile && strcmp(fileName,fCurrentFile->GetName())==0) {
    r=fCurrentFile->getRunId();
  } else {
    if (!evt.setFile(fileName)) r=-1;
    else {
      evt.read();
      r=evt.getRunNr();
    }
  }
  return r;
}

void HldRFIOFileSource::addFile(const Text_t *name,Int_t refId)
{
	// adds the file with the reference runId to the list of files, if it can be accessed
  HldFileDesc *desc=0;
  HldFileDesc *newDesc=0;
  TString fileName;
  fileName=fCurrentRobot+":/"+fCurrentArchive+"/"+fCurrentDir+"/"+name;
  desc=(HldFileDesc *)fFileList.FindObject(fileName);
  if (desc) {
    if (refId==-1)
      newDesc=new HldFileDesc(fileName.Data(),desc->getRunId(),desc->getRefId());
    else
      newDesc=new HldFileDesc(fileName.Data(),desc->getRunId(),refId);
    fFileList.Add(newDesc);
    Warning("addFile","File %s added more than once\n",fileName.Data());
  } else {
    if (HRFIOFile::access(fileName.Data(),F_OK)!=0) { // File does not exist
      Error("addFile","File %s not found\n",fileName.Data());
    } else {
      newDesc=new HldFileDesc(fileName.Data(),getRunId(fileName),refId);
      fFileList.Add(newDesc);
    }
  }
}

EDsState HldRFIOFileSource::getNextEvent(Bool_t doUnpack) {
  // loops over the event files in the runtime database
  // Tries to read a new event from the LMD file and put the info into the
  // HEvent structure:

  if(isDumped) {
    if (dumpEvt()==kDsError) {
      if (getNextFile()==kFALSE) return kDsEndData;
      else return kDsEndFile;
    }
    return kDsOk;
  }

  if (isScanned) {
    if (scanEvt()==kDsError) {
      if (getNextFile()==kFALSE) return kDsEndData;
      else return kDsEndFile;
    }
    return kDsOk;
  }

  fEventNr++;

  if ( !((HldRFIOFilEvt*)fReadEvent)->execute() || !(fEventNr<fEventLimit)) {
    //End of current file
    fEventNr = 0;
    if (getNextFile()==kFALSE) return kDsEndData;
    else return kDsEndFile;
  }

  if(doUnpack){
      decodeHeader((*fEventAddr)->getHeader());
      iter->Reset();
      HldUnpack *unpacker;
      while ( (unpacker=(HldUnpack *)iter->Next())!=NULL) {
	  Int_t ret=unpacker->execute();
	  if(!ret) return kDsError;
	  if(ret==kDsSkip) return kDsSkip;
      }
  }
  return kDsOk;
}


Bool_t HldRFIOFileSource::getNextFile(void )
{
  if (!fCurrentFile) {
    if (fFileList.IsEmpty()) return kFALSE;
    else fCurrentFile=(HldFileDesc *)fFileList.First();
  } else {
    fCurrentFile=(HldFileDesc *)fFileList.After(fCurrentFile);
    if (!fCurrentFile) return kFALSE;
  }

  ((HldRFIOFilEvt *)fReadEvent)->setFile(fCurrentFile->GetName());

  return kTRUE;
}


Bool_t HldRFIOFileSource::rewind()
{
  if (fFileList.IsEmpty()) return kFALSE;
  else fCurrentFile=(HldFileDesc *)fFileList.First();
  return ((HldRFIOFilEvt *)fReadEvent)->setFile(fCurrentFile->GetName());
}

void HldRFIOFileSource::setRobot(const Char_t *robotName)
{
  TString *tmp;

  tmp = new TString(robotName);
  tmp->ToUpper();
  if(tmp->CompareTo("LXGSTORE")==0)
    fCurrentRobot = "lxgstore.gsi.de";
}
