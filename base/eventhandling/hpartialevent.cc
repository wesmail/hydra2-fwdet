#pragma implementation
using namespace std;
#include "hpartialevent.h"

#include "hades.h"
#include "hmatrixcategory.h"
#include "htree.h"
#include "TBrowser.h"
#include "TBuffer.h"
#include "TClass.h"
#include <stdlib.h>
#include "TROOT.h"
#include "hdebug.h"
#include <iostream> 
#include <iomanip>

//*-- Author : Manuel Sanchez
//*-- Modified : 20/01/2002 by Manuel Sanchez
//*-- Modified : 16/06/2000 by R. Holzmann
//*-- Modified : 7/10/98 by Manuel Sanchez
//*-- Modified : 16/04/98 9:17 by Manuel Sanchez 
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////
//HPartialEvent Begin_Html 
//<a href="HPartialEvent.gif">(UML Diagram)</a> 
//End_Html
// 
//   Each partial events holds the data (i.e. the HDataObjects) corresponding 
// to each main detection system; like Mdcs, Rich...
//
//   These data in a partial event are organized in categories, which stand
// for the different types of data in the partial event. For example, in the
// partial event for the Mdcs there will be a category holding the objects with
// raw data (i.e. the objects instatiating a HMdcRaw). So conceptually the
// categories are "kind of data" and they hold the objects of a particular
// kind (instatiating a particular class).
//
//   The categories are the responsible of giving access to the objects they 
// hold (see HCategory), but a partial event must also allow the user to 
// access the data stored into it; that's done using the function getCategory()
// which returns a particular category, so the user can ask that category for
// the data object he wants. For example, let's say we have a HLocation object
// loc pointing to any location, and a HPartialEvent par with the Mdc's data; 
// if we want to get the raw data object pointed by loc we should use:
//
//    par->getCategory(catMdcRaw)->getObject(loc);
//
//    And this function will return the pursued data object.
//
//////////////////////////////////////////////////////////////////////////

ClassImp(HPartialEvent)

HPartialEvent::HPartialEvent(void) {
  //Default constructor
  fHeader=NULL;
  fRecLevel=1;
  fCategories=new TObjArray(32);
  setExpandedStreamer(kFALSE);
}

HPartialEvent::HPartialEvent(const Text_t *aName,
			     const Text_t *aTitle,
			     Cat_t aBaseCat) : HEvent(aName,aTitle) 
{
  //Allocates a new HPartial event with name aName and title aTitle
  //
  // aBaseCat is the identifier of the lower category that will be stored in
  // this partial event, for example: if the user wants to allocate a partial
  // event holding the Mdc data, then aBaseCat should be: catMdc; which is a
  // constant defined in HadDef.h
  fHeader=NULL;
  fBaseCategory=aBaseCat;
  fRecLevel=rlRaw;
  fCategories=new TObjArray(32);
  setExpandedStreamer(kFALSE);
}
   
HPartialEvent::~HPartialEvent(void) {
  //Clears the event 
 if (fCategories) {
   fCategories->Delete();
   delete fCategories;
 }
}

void HPartialEvent::activateBranch(TTree *tree,Int_t splitLevel) {
  // Activates those branches in the TTree tree whose names are equal to those
  //of the branches that would be generated with the current event structure if
  //the function makeBranch() is called, being the split level=splitLevel
  //
  // This way if we have a TTree with a lot of branches and we only need some
  //of them to fill data in the event structure we are using at a given moment,
  //then we only activate those branches and the others are not read
  TString cad;
  Text_t name[255];
  
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HPartialEvent::activateBranch");
#endif
  if (fCategories) {
    if (splitLevel==2) {
      TObjArray &vector=*fCategories;
      HCategory **cat;
      Int_t i;
      TBranch *brHead=0;
      TBranch *brCategory=0;

      for (i=0;i<=vector.GetLast();i++) {
	cat=(HCategory **)&vector[i];
	if ( (*cat)!=NULL && ((*cat)->IsPersistent())) {
	  if ( (*cat)->isSelfSplitable()) 
	    (*cat)->activateBranch(tree,splitLevel);
	  else {
	    cad=(*cat)->getClassName();
	    cad += ".";
	    brCategory = tree->GetBranch(cad.Data());
	    if (brCategory) { //Post ROOT3 splitting
	      tree->SetBranchAddress(cad.Data(),cat);
	      (*cat)->activateBranch(tree,splitLevel);
	      tree->SetBranchStatus(cad.Data(),1);
	      cad=(*cat)->getClassName(); cad+="*";
	      tree->SetBranchStatus(cad.Data(),1);
	    } else {
	      cad=(*cat)->getClassName();
	      brCategory = tree->GetBranch(cad.Data());
	      if (brCategory) { //Pre ROOT3 splitting
		tree->SetBranchAddress(cad.Data(),cat);
		(*cat)->activateBranch(tree,splitLevel);
		tree->SetBranchStatus(cad.Data(),1);
		cad=(*cat)->getClassName(); cad+=".*";
		tree->SetBranchStatus(cad.Data(),1);
	      } else {
		Warning("activateBranch","Category %s not found in tree",
			cad.Data());
	      }
	    }
    	  }
	}
      }
      sprintf(name,"%s.Header",GetName());
      brHead=tree->GetBranch(name);
      if (brHead) { //Pre ROOT3 splitting
	tree->SetBranchAddress(name,&fHeader);
	tree->SetBranchStatus(name,1);
      }
    }
  }
#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("return HPartialEvent::activateBranch");
#endif
}
 
void HPartialEvent::makeBranch(TBranch *parent,HTree* tree) {
  // Makes branches for each category in the partial event and for the split
  //tree, then those branches are added to the list of subbranches of "parent"
  //
  // This method is called by Hades::makeTree() 
  TString cad;
  Text_t name[255];

  if (fCategories) {
    if (gHades->getSplitLevel()==2) {
      TObjArray &vector=*fCategories;
      HCategory **cat;
      Int_t i;
      for (i=0;i<=vector.GetLast();i++) {
	cat=(HCategory **)&vector[i];
	if ( (*cat)!=NULL && ((*cat)->IsPersistent())) {
	  if ( (*cat)->isSelfSplitable()) {
	      tree->Branch((*cat)->getClassName(),
			   (*cat)->ClassName(),
			   cat,gHades->getTreeBufferSize(),0);
	      //parent->GetListOfBranches()->Add(b);
	      //(*cat)->makeBranch(b);
	  } else {
	      sprintf(name,"%s.",(*cat)->getClassName());
	      tree->Branch(name,
			   (*cat)->ClassName(),
			   cat,gHades->getTreeBufferSize(),99);
	      //parent->GetListOfBranches()->Add(b);
	  }
	}
      }
 
    }
  }
}

Bool_t HPartialEvent::addCategory(Cat_t aCat,HCategory *cat,Option_t *) {
  // Adds the category "cat", identified by "aCat" to the partial event.
  addCategory(aCat,cat);
  return kTRUE;
}

void HPartialEvent::addCategory(Cat_t aCat,HCategory *category) {
  // Adds the category "category", which is identified by aCat, to the partial
  //event
  //
  // Before using this function the user should setup the HCategory "category"
  //
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HPartialEvent::addCategory");
#endif
  fCategories->AddAtAndExpand(category,aCat-fBaseCategory);
  category->setCategory(aCat);
#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HPartialEvent::addCategory");
#endif
}

Bool_t HPartialEvent::removeCategory(Cat_t aCat) {
  //Removes the category aCat from this partial event and deletes it.
  HCategory *category=0;
  category=(HCategory *)fCategories->At(aCat-fBaseCategory);
  if (category==0) return kFALSE;
  fCategories->RemoveAt(aCat-fBaseCategory);
  delete category;
  return kTRUE;
}

TObjArray *HPartialEvent::getCategories(void) {
  // Returns an array with all the categories held by this event
  //
  // To add categories to the partial event use HPartialEvent::addCategory()
  return fCategories;
}

HCategory *HPartialEvent::getCategory(Cat_t aCat) {
  // Returns a pointer to the category identified by "aCat"
  HCategory *cat;
  cat=(HCategory *)fCategories->At(aCat-fBaseCategory);
  return cat;
}
      
void HPartialEvent::Clear(Option_t *) {
  // Clears the partial event,i.e. deletes all objects in the partial event
  //but keeping its structure.
  //
  // After the reconstruction of each event, the HEvent::Clear function must
  //be called in order to make place for the new data. 
 TIter nextCat(fCategories);
 HCategory *cat;

 while ((cat=(HCategory *)nextCat())!=NULL) {
    cat->Clear();
 }
}

void HPartialEvent::clearAll(Int_t level) {
  //See HEvent
  if (level>1) {
    Clear();
  } else {
    if (fCategories) fCategories->Delete();
  }
}

void HPartialEvent::setRecLevel(Int_t aRecLevel) {
  // Sets the reconstruction level of the event, this is useful to know which
  //categories in the event are already reconstructed.
  fRecLevel=aRecLevel;
}

void HPartialEvent::setPersistency(Bool_t fPersistency) {
  Int_t nCat = fCategories->GetEntriesFast();
  for (Int_t i=0; i<nCat; i++) { // loop over categories
    HCategory* cat = (HCategory*)(fCategories->At(i));
    if (cat) cat->HCategory::setPersistency(fPersistency);
  }  
}

Int_t HPartialEvent::getRecLevel(void) {
  // Returns the reconstruction level of the partial event.
  return fRecLevel;
}

void HPartialEvent::Browse(TBrowser *b) {
  // Method used to browse a partial event with a Root browser.
  //
  // This function will be called by a Root browser, not by the user
  if (!fCategories) return;
  TIter next(fCategories);
  HCategory *cat;

  while ((cat=(HCategory *)next())!=NULL) {
     if (cat->InheritsFrom("HSplitCategory")) b->Add(cat,cat->getClassName());
     else cat->Browse(b);
  }
}

void HPartialEvent::Streamer(TBuffer &R__b)
{
  // Stream an object of class HPartialEvent.
  TObjArray *catNames=0;
  Int_t nCategories=0;
  HCategory *category=0;
  Char_t buffer[255];

  if (R__b.IsReading()) {
    Version_t R__v = R__b.ReadVersion(); if (R__v) { }
    HEvent::Streamer(R__b);
    R__b >> fRecLevel;
    R__b >> fBaseCategory;
    R__b >> fHeader;
    if (R__v == 2 && !hasExpandedStreamer() ) {
      if (fCategories) fCategories->Delete();
      R__b >> fCategories;
    } else {
      R__b >> catNames;
      nCategories=catNames->GetEntriesFast();
      for (Int_t i=0;i<nCategories;i++) {
	if (catNames->At(i)!=NULL) {
	  strcpy(buffer,((TObjString *)catNames->At(i))->GetString().Data());
	  if (gROOT->GetClass(buffer)) {
	    category=(HCategory *)gDirectory->Get(buffer);
	    fCategories->AddAtAndExpand(category,i);
	  }
	}
      }
      delete catNames;
    }
  } else {
    R__b.WriteVersion(HPartialEvent::IsA());
    HEvent::Streamer(R__b);
    R__b << fRecLevel;
    R__b << fBaseCategory;
    R__b << fHeader;
    if (!hasExpandedStreamer()) {
      R__b << fCategories;
    } else {
      nCategories=fCategories->GetEntriesFast(); 
      catNames=new TObjArray(nCategories);
      for (Int_t i=0;i<nCategories;i++) {
	category=(HCategory *)fCategories->At(i);
	if (category) { 
	 if (category->IsPersistent())
	  catNames->AddAt(new TObjString(category->getClassName()),i);
	}
      }
      R__b << catNames;
      for (Int_t i=0;i<nCategories;i++) { 
	category=(HCategory *)fCategories->At(i);
	if (category) { 
	  if (category->IsPersistent())
	    category->Write(category->getClassName());
	}
      }
      delete catNames;
    }
  }
}  

