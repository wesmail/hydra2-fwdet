using namespace std;
#include "hsimulgeantreader.h"
#include "hgeantheader.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "hlinearcategory.h"
#include "TDirectory.h"
#include <iostream> 
#include <iomanip>

//*-- Author : R. Holzmann
//*-- Modified: 17/02/2000 by R. Holzmann

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////
//  HSimulGeantReader for HGeant Root event
//
//  connect complete event + simul header
////////////////////////////////////////////

ClassImp(HSimulGeantReader)

HSimulGeantReader::HSimulGeantReader(void) {
  fEventId = 0;
}

HSimulGeantReader::~HSimulGeantReader(void) {
}


Bool_t HSimulGeantReader::init(void) {
//  Set up event and HGeant input tree
//
  fEventId = 0;
  if (fInputFile == NULL) {
     t = NULL;
     return kFALSE;   // no input file !
  }

  fSimEv = (HRecEvent*)fInputFile->Get("Event");  // read event definition
  gHades->setEvent(fSimEv);

  if ((t = (TTree*)fInputFile->Get("T")) == NULL) return kFALSE;

  gHades->activateTree(t);  // this should connect everything, but does not
                            // therefore we need to handle the header extra...

  HGeantHeader* fSimHead=(HGeantHeader*)(fSimEv->getPartialEvent(catSimul)
                                               ->getSubHeader());
  if(fSimHead == NULL) {
     fSimEv->getPartialEvent(catSimul)->setSubHeader(new HGeantHeader());
     fSimHead = (HGeantHeader*)(fSimEv->getPartialEvent(catSimul)
                                      ->getSubHeader());
  }
  t->SetBranchAddress("Simul.Header",&fSimHead);  // connect header explicitly
  t->SetBranchStatus("Simul.Header",1);

  if (gHades->getOutputFile() != NULL) gHades->getOutputFile()->cd();
  return kTRUE;
}


Bool_t HSimulGeantReader::execute(void) {
//  Read one entry from tree
//
  if(t) {   // input tree does exist
     if(t->GetEntry(fEventId)==0) return kFALSE;  // read 1 entry from tree
  }

  fEventId++;

  return kTRUE;  // return type is Bool_t
}




