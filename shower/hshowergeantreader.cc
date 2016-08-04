using namespace std;
#include "hshowergeantreader.h"
#include "hgeantshower.h"
#include "hgeantmaxtrk.h"
#include "showerdef.h"
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
/////////////////////////////////////////////////////
//  HShowerGeantReader for HGeant Root SHOWER branch
//
/////////////////////////////////////////////////////

ClassImp(HShowerGeantReader)

HShowerGeantReader::HShowerGeantReader(void) {
  fEventId = 0;
}

HShowerGeantReader::~HShowerGeantReader(void) {
}


Bool_t HShowerGeantReader::init(void) {
//  Set up categories and HGeant input tree
//  (this still needs splitlevel=2 on input file)
//
  fGeantShowCat = (HMatrixCategory*)(gHades->getCurrentEvent()
                          ->getCategory(catShowerGeantRaw));  
  if (!fGeantShowCat) {
     if (!fInputFile) {
        t = NULL;
        return kFALSE;   // no category, no input file !
     }
     if (fInputFile->cd("dirSimul")) { // change dir, read category definition
        fGeantShowCat = (HMatrixCategory*)(gDirectory->Get("HGeantShower"));
        fInputFile->cd("..");        // go back up one level
     } else {
        Int_t ini[2] = {6,MAXTRKSHOW};      // use old default definition 
        fGeantShowCat = new HMatrixCategory("HGeantShower",2,ini,1.0);
     }
     gHades->getCurrentEvent()->
               addCategory(catShowerGeantRaw,fGeantShowCat,"Simul");

     printf("\n%s(%d,%d) matrix category created for HGeant input\n",
              fGeantShowCat->getClassName(),
              fGeantShowCat->getSize(0), fGeantShowCat->getSize(1));

  }
  if (!fInputFile) {
     t = NULL;
     return kTRUE;   // category exists, but no input file (called from HGeant)
  }

  if ((t=(TTree*)fInputFile->Get("T")) == NULL) return kFALSE;

  t->SetBranchAddress("HGeantShower",&fGeantShowCat); // connect SHOWER category
  t->SetBranchStatus("HGeantShower",kTRUE);
  fGeantShowCat->activateBranch(t,2);
  
  if (gHades->getOutputFile() != NULL) gHades->getOutputFile()->cd(); // !!!
  return kTRUE;
}


Bool_t HShowerGeantReader::execute(void) {
//  Read one entry from tree 
//
  if(t) {   // input tree does exist
     if(t->GetEntry(fEventId)==0) return kFALSE;  // read 1 entry from tree
  }

  fEventId++;

  return kTRUE;  // return type is Bool_t
}



