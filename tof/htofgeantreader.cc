using namespace std;
#include "htofgeantreader.h"
#include "hgeanttof.h"
#include "hgeantmaxtrk.h"
#include "tofdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "TDirectory.h"
#include <iostream> 
#include <iomanip>

//*-- Author : R. Holzmann
//*-- Modified: 05/12/2008 by I. Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////
//  HTofGeantReader for HGeant Root TOF branch
//
///////////////////////////////////////////////////

ClassImp(HTofGeantReader)

HTofGeantReader::HTofGeantReader(void) {
  fEventId = 0;
}

HTofGeantReader::~HTofGeantReader(void) {
}


Bool_t HTofGeantReader::init(void) {
//  Set up categories and HGeant input tree
//  (this still needs splitlevel=2 on input file)
//
  fGeantTofCat = (HMatrixCategory*)(gHades->getCurrentEvent()
                          ->getCategory(catTofGeantRaw));  
  if (!fGeantTofCat) {
     if (!fInputFile) {
        t = NULL;
        return kFALSE;   // no category, no input file !
     }
     if (fInputFile->cd("dirSimul")) { // change dir, read category definition
        fGeantTofCat = (HMatrixCategory*)(gDirectory->Get("HGeantTof"));
        fInputFile->cd("..");        // go back up one level
     } else {
        Int_t ini[2] = {6,MAXTRKTOF};      // use old default definition 
        fGeantTofCat = new HMatrixCategory("HGeantTof",2,ini,1.0);
     }
     gHades->getCurrentEvent()->
               addCategory(catTofGeantRaw,fGeantTofCat,"Simul");

     printf("\n%s(%d,%d) matrix category created for HGeant input\n",
              fGeantTofCat->getClassName(),
              fGeantTofCat->getSize(0), fGeantTofCat->getSize(1));

  }
  if (!fInputFile) {
     t = NULL;
     return kTRUE;   // category exists, but no input file (called from HGeant)
  }

  if ((t=(TTree*)fInputFile->Get("T")) == NULL) return kFALSE;

  t->SetBranchAddress("HGeantTof",&fGeantTofCat);   // connect to TOF category
  t->SetBranchStatus("HGeantTof",kTRUE);
  fGeantTofCat->activateBranch(t,2);
  
  if (gHades->getOutputFile() != NULL) gHades->getOutputFile()->cd(); // !!!
  return kTRUE;
}


Bool_t HTofGeantReader::execute(void) {
//  Read one entry from tree 
//
  if(t) {   // input tree does exist
     if(t->GetEntry(fEventId)==0) return kFALSE;  // read 1 entry from tree
  }

  fEventId++;

  return kTRUE;  // return type is Bool_t
}



