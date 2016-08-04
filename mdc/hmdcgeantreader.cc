#include "hmdcgeantreader.h"
#include "hgeantmdc.h"
#include "hgeantmaxtrk.h"
#include "hmdcdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hmessagemgr.h"
#include "TDirectory.h"

//#include <iostream>
//#include <iomanip>
using namespace std;

//*-- Author : R. Holzmann
//*-- Modified: 05/12/2008 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////
//  HMdcGeantReader for HGeant Root MDC branch
//
///////////////////////////////////////////////////

ClassImp(HMdcGeantReader)

HMdcGeantReader::HMdcGeantReader(void) {
  fEventId = 0;
}

HMdcGeantReader::~HMdcGeantReader(void) {
}


Bool_t HMdcGeantReader::init(void) {
//  Set up categories and HGeant input tree
//  (this still needs splitlevel=2 on input file)
//
  fGeantMdcCat = (HMatrixCategory*)(gHades->getCurrentEvent()
                          ->getCategory(catMdcGeantRaw));  
  if (!fGeantMdcCat) {
     if (!fInputFile) {
        t = NULL;
        return kFALSE;   // no category, no input file !
     }
     if (fInputFile->cd("dirSimul")) { // change dir, read category definition
        fGeantMdcCat = (HMatrixCategory*)(gDirectory->Get("HGeantMdc"));
        fInputFile->cd("..");        // go back up one level
     } else {
        Int_t ini[4] = {6,4,7,MAXTRKMDC};      // use old default definition 
        fGeantMdcCat = new HMatrixCategory("HGeantMdc",4,ini,1.0);
     }
     gHades->getCurrentEvent()->
               addCategory(catMdcGeantRaw,fGeantMdcCat,"Simul");

     gHades->getMsg()->info(10,HMessageMgr::DET_MDC,this->GetName(),
			    "%s(%d,%d,%d,%d) matrix category created for HGeant input",
			    fGeantMdcCat->getClassName(),
			    fGeantMdcCat->getSize(0), fGeantMdcCat->getSize(1),
			    fGeantMdcCat->getSize(2), fGeantMdcCat->getSize(3));

  }
  if (!fInputFile) {
     t = NULL;
     return kTRUE;   // category exists, but no input file (called from HGeant)
  }

  if ((t=(TTree*)fInputFile->Get("T")) == NULL) return kFALSE;

  t->SetBranchAddress("HGeantMdc",&fGeantMdcCat);   // connect to MDC category
  t->SetBranchStatus("HGeantMdc",kTRUE);
  fGeantMdcCat->activateBranch(t,2);
  
  if (gHades->getOutputFile() != NULL) gHades->getOutputFile()->cd(); // !!!
  return kTRUE;
}


Bool_t HMdcGeantReader::execute(void) {
//  Read one entry from tree 
//
  if(t) {   // input tree does exist
     if(t->GetEntry(fEventId)==0) return kFALSE;  // read 1 entry from tree
  }

  fEventId++;

  return kTRUE;  // return type is Bool_t
}



