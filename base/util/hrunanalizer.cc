//*-- AUTHOR : Manuel Sanchez
//*-- Modified : 13/19/1999 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
//  HRunAnalizer
//
//  Reconstructor to analyze the event header and to retrieve the information
//  stored in the Oracle database table hades_oper.runs:
//       hld file name
//       run id
//       run start (date and time of first event)
//       run stop (date and time of last event, must not be a stop event)
//       total number of events in the file
//       number of calibration events
//  The information is written to an Ascii file (default name: "run_info.txt").
//
///////////////////////////////////////////////////////////////////////////////

#include "hrunanalizer.h"
#include "hevent.h"
#include "heventheader.h"
#include "hades.h"
#include "hruntimedb.h"
#include <unistd.h>

ClassImp(HRunAnalizer)

HRunAnalizer::HRunAnalizer(Text_t *name,Text_t *title,Text_t *fileName)
  : HReconstructor(name,title) {
  // constructor
  // default name of output file: run_info.txt
  Char_t defaultFileName[]="run_info.txt";
  fEventHeader=0;
  fFirstRun=kTRUE;
  fTotEvents=0;
  fCalEvents=0;
  if (!fileName) fileName=defaultFileName;
  if (access(fileName,F_OK)==0) { //If file exists
    fOut=fopen(fileName,"a"); 
  } else {
    fOut=fopen(fileName,"w"); 
    writeHeader();
  }
}
/*
// old code ... mase copy constructor private
 HRunAnalizer::HRunAnalizer(HRunAnalizer &analizer) {
  // copy constructor (not implemented!)
  Error("HRunAnalizer","Copy constructor not defined");
}
*/
HRunAnalizer::~HRunAnalizer(void) {
  // destructor
  fclose(fOut);
}

Int_t HRunAnalizer::execute(void) {
  // reads the start event -> run id, run start
  // extracts the date and time of the last event, the total number of
  // events and the number of calibration events in the file
  UInt_t fId=fEventHeader->getId();
  if (fId==65538) { // start event
    extractStartInfo();
    fFirstEvent=kFALSE;
  } else {
    fRunStopTime=fEventHeader->getTime();
    fRunStopDate=fEventHeader->getDate();
    if (fId!=65539) fTotEvents++;  //no increment for stop event
    if (fId==4) fCalEvents++;
  }
  return 0;
}

Bool_t HRunAnalizer::init(void) {
  fEventHeader=gHades->getCurrentEvent()->getHeader();
  fFirstRun=kTRUE;
  fTotEvents=0;
  fCalEvents=0;
  return kTRUE;
}

Bool_t HRunAnalizer::reinit(void) {
  if (fFirstRun)
    fFirstRun=kFALSE;
  else
    writeInfo(); // Write info of previous file.
  fTotEvents=0;
  fCalEvents=0;
  fFirstEvent=kTRUE;
  return kTRUE;
}

Bool_t HRunAnalizer::finalize(void) {
  if (!fFirstRun) writeInfo();
  return kTRUE;
}

void HRunAnalizer::extractStartInfo(void) {
  fRunStartTime=fEventHeader->getTime();
  fRunStartDate=fEventHeader->getDate();
  fRunId=fEventHeader->getEventRunNumber();
  fFileName=gHades->getRuntimeDb()->getCurrentFileName();
}

void HRunAnalizer::writeHeader(void) {
  fprintf(fOut,"########################################################\n");
  //  fprintf(fOut,"#Run Id\t\tRun Start Time\tNr Events\tFile name\n");
  fprintf(fOut,"#Run Id   File name   Total nr of events   Nr of cal events\n");
  fprintf(fOut,"#    Run Start  Run Stop Time\n");
  fprintf(fOut,"########################################################\n");
}

void HRunAnalizer::writeInfo(void) {
  UInt_t db=fRunStartDate;
  UInt_t tb=fRunStartTime;
  UInt_t de=fRunStopDate;
  UInt_t te=fRunStopTime;
  fprintf(fOut,"%u  %s  %i  %i\n",fRunId,fFileName.Data(),fTotEvents,fCalEvents);
  fprintf(fOut,"    %02i-%02i-%4i::%02i:%02i:%02i  %02i-%02i-%4i::%02i:%02i:%02i\n", 
         (db & 0xFF),1+( (db>>8) & 0xFF),1900+((db>>16) & 0xFF),
         ((tb>>16) & 0xFF),( (tb>>8) & 0xFF),( tb & 0xFF),
         (de & 0xFF),1+( (de>>8) & 0xFF),1900+((de>>16) & 0xFF),
	 ((te>>16) & 0xFF),( (te>>8) & 0xFF),( te & 0xFF));
}

