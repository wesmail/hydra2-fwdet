#include "hdirectsource.h"
#include "TROOT.h"
#include "hgeantreader.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hcategory.h"

//*-- Author : R. Holzmann
//*-- Modified : 02/03/2000 by R. Holzmann

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////
// HDirectSource
//
//  This class is a data source to operate directly under HGeant control
//
//  HDirectSource acts like HGeantSource, but instead of taking the data from
//  HGeant Root file(s) it is launched from within HGeant and gets the data
//  directly from the simulation categories in memory
//////////////////////////////

HDirectSource::HDirectSource(void) {
  //Default constructor
  fCurrentRunId = 0;
}

HDirectSource::HDirectSource(HDirectSource &s) {
  fReaderList.Delete();
  fReaderList.AddAll(&s.fReaderList);
  fCurrentRunId = s.fCurrentRunId;
}

HDirectSource::~HDirectSource(void) {
  //Destructor. Deletes the readers
  fReaderList.Delete();
}

Bool_t HDirectSource::addGeantReader(HGeantReader *r) {
  //This method is provided to stay compatible with readers
  //like HMdcGeantRootReader, which do part of the digitization
  //The input is the actual reader to be used.

  r->setInput(NULL);   // the file input of the reader is set to NULL
  fReaderList.Add(r);
  return kTRUE;
}

Bool_t HDirectSource::finalize(void) {
 TIterator *iter=fReaderList.MakeIterator();
 HGeantReader *reader=0;
 while ( (reader=(HGeantReader *)iter->Next())!= 0 ) {
  if (!reader->finalize()) return kFALSE;
 }
 return kTRUE;
}

Bool_t HDirectSource::init(void) {
  // calls the init() functions of all readers
  TIter next(&fReaderList);
  HGeantReader *fReader;

  HRecEvent* fEv = (HRecEvent*)(*fEventAddr);   // pointer to event
  if(fEv == NULL) {
     printf("\n*** No HRecEvent exists.\n");
     return kFALSE;   
  }
  HPartialEvent* fSimul = fEv->getPartialEvent(catSimul);
  if(fSimul == NULL) {
     printf("\n*** No Simul partial event exists.\n");
     return kFALSE;
  }

  while ( (fReader=(HGeantReader*)next())!=NULL) {  // initialize readers
    if (!fReader->init()) return kFALSE;
  }
  return kTRUE;
}

EDsState HDirectSource::getNextEvent(void) {
  //Produce a new event and fill into the HEvent structure
  //
  TIter next(&fReaderList);
  HGeantReader *fReader;
  Int_t returnCode;
  gROOT->ProcessLine("doGeant(\"trigger 1\");");  // make 1 event here
  while ( (fReader=(HGeantReader *)next())!=NULL) {
     returnCode = fReader->execute();
     if(returnCode != kTRUE) return kDsEndData;   // error or end of data
  }
  return kDsOk;
}  

ClassImp(HDirectSource)

//extern void doGeant(Char_t*);   // this is an HGeant function








