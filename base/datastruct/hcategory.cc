#pragma implementation
using namespace std;
#include <hcategory.h>
#include <hiterator.h>
#include <hdebug.h>
#include "hfilter.h"
#include <iostream> 
#include <iomanip>
#include <stdio.h>

//*-- Author : Manuel Sanchez manuel@fpddv1.usc.es
//*-- Modified : 25/09/98 by Manuel Sanchez
//*-- Modified : 29/02/00 by Denis Bertini
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////
//HCategory (ABC)
// 
//   The HCategory class is an abstract base class. So the real work is 
// made by the derived classes. These derived classes correspond to different
// strategies to store the data objects in memory and in file. The classes
// derived from HCategory can also indicate the way they want to be stored in
// a Root tree (the TBranch layout), for that purpose the makeBranch() function
// can be overloaded.
//
//   A HCategory encapsulates one category of data; that is one kind of data
// (mdc raw data,rich rings...), and it is responsible of giving the user 
// access to the objects held by that category. The category is also able to
// hold one header with common information for all the objects in the category.
//
//   The class provides functions to access the objects in the category:
//
//   Each data object in the category is stored in a particular location 
// (see HLocation), so to access an object you must give its location and use
// the method: getObject(HLocation &aLoc). This method returns one only object,
// if you want a collection with all the objects corresponding to a particular 
// location (i.e, all the raw data in the second chamber of the first sector...)
// then use query(TCollection *col,HLocation &aLoc) with col being the 
// collection where to store the result. You can also iterate on all the 
// objects in the category or the objects corresponding to a particular location
// using the HIterator created by MakeIterator() or MakeReverseIterator()
//
//   The category is also responsible of allocating new objects, this can be 
// done through the functions getSlot(HLocation &aLoc) and 
// getNewSlot(HLocation &aLoc)  which return a place in memory where to 
// allocate the new object (a slot) corresponding to the location aLoc. The
// strategy of letting the HCategory to manage the memory has the advantage 
// that it allows the category to have the memory preallocated.
//
//   Example:
//    {
//     HLocation loc; //Allocates loc pointing to a location
//     HMdcRaw *raw; //Declaration of a pointer to a TObject
//     HCategory *cat; //Pointer to a generic category
//     ...
//     //loc is set to point to the location (2,2,1); this could be something
//     //like: sector 2, mdc 2, plane 1.
//     loc.set(3,2,2,1); 
//     //Ask for the slot at location "loc" and stores its address in raw
//     //if there is not such a slot, the getSlot() method will return NULL
//     raw=cat->getSlot(loc);
//     //If we have a valid slot (raw!=NULL) then allocate a new object of
//     //class HMdcRaw at the addres given by raw using the operator 
//     //"new with placement" 
//     if (raw!=NULL) raw=new(raw) HMdcRaw;
//    }
//     
//   Each category can be persistent or not; so if it's persistent it
// will be stored in the output file (if any) and if it's not, then it won't
// be stored in the output file. To control the persistency of a category
// the setPersistency() method is provided
//
//   Here follows a description of the common methods every class inherited 
// from HCategory provides:
//
//  TObject *&getNewSlot(HLocation &aLoc)
//    Returns a memory slot for location aLoc where you can place a new object
//   aLoc gives the indexes for the location of the new object except the last 
//   one. So the function returns the first empty slot in aLoc.
//
//  TObject *&getSlot(HLocation &aLoc)
//     The same as before, but now aLoc gives the complete location of the new
//    object 
//  TObject *getObject(HLocation &aLoc)
//    Returns the object at the location aLoc
//  
//  TClonesArray *getClones(HLocation &aLoc)
//    Returns a clones array with the objects corresponding to the HLocation 
//    aLoc.
//
//  Bool_t query(TCollection *aCol,HLocation &aLoc,HFilter &aFilter)
//    Adds to aCol all objects in the category corresponding to the location 
//    aLoc and passing the filter aFilter. There are also functions without 
//    aFilter or without aLoc
//
//  Bool_t filter(HLocation &aLoc,HFilter &aFilter)
//  Bool_t filter(HFilter &aFilter)
//    The same as before but now the objects not verifying the conditions are 
//   deleted off the category
//
////////////////////////////////

ClassImp(HCategory)

 TObject *gNullObjectP = 0;

HCategory::HCategory(void)
{
  //Default constructor 
 fCat=catInvalid;
 fBranchingLevel=0;
 fPersistency=kTRUE;
 fHeader = 0;
}

HCategory::~HCategory(void) {
  //Destructor. 
}

void HCategory::activateBranch(TTree *tree,Int_t splitLevel) {
  // If a category generates its own branches in a different way than that
  //provided with HTree::makeBranch (see isSelfSplitable()) then it must 
  //override the makeBranch function as well as the activateBranch function..
  //
  // The activate branch is intended to activate those branches in the TTree
  //tree whose names correspond to the names of the branches that would be 
  //created by makeBranch() in this category. To do such a thing the Root 
  //methods: TTree::SetBranchAddress() and TTree::SetBranchStatus() need to
  //be called.
  //
  // As a default activateBranch() does nothing
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HCategory::activateBranch");
#endif
  if (tree==NULL) Warning("HCategory::activateBranch","tree is NULL");
  if (splitLevel<0) Warning("HCategory::activateBranch","invalid split level");
#if DEBUG_LEVEL>2
  gDebuger->leaveFunc("HCategory::activateBranch");
#endif
}

void HCategory::setBranchingLevel(Int_t nLevel) {
  //Sets the branching level.
  //
  // The branching level is directly related with the number of indexes needed
  //to identify an object in the category (the number of indexes in the object's
  //location). For example, in the HMatrixCategory those two numbers are equal;
  //however in HSplitCategory the branching level is equal to the number of
  //indexes needed to unambiguosly identify an object in the category minus 1.
 fBranchingLevel=nLevel;
}

void HCategory::setCategory(Cat_t aCat) {
  //Sets the identifier for this particular category
 fCat=aCat;
}

Cat_t HCategory::getCategory(void) {
  //Returns the identifier of this particular category
 return fCat;
}

Int_t HCategory::getBranchingLevel(void) {
  //Returns the branching level for this category.
  //
  // The branching level is directly related with the number of indexes needed
  //to identify an object in the category (the number of indexes in the object's
  //location). For example, in the HMatrixCategory those two numbers are equal;
  //however in HSplitCategory the branching level is equal to the number of
  //indexes needed to unambiguosly identify an object in the category minus 1.
 return fBranchingLevel;
}

Bool_t HCategory::IsPersistent(void) {
  //Returns kTRUE if the category is persistent and kFALSE if it is not.
  return fPersistency;
}

void HCategory::setPersistency(Bool_t per) {
  // Sets the persistency of the category
  //
  // Input:
  //   per=kTRUE  --> The category is persistent
  //   per=kFALSE --> The category is not persistent.
  fPersistency=per;
}

Bool_t HCategory::IsFolder(void) const {
  return kTRUE;
}

TIterator *HCategory::MakeReverseIterator(void) {
  //Makes an HIterator which iterates backwards
  return MakeIterator("catIter",kIterBackward);
}

Bool_t HCategory::query(TCollection *aCol,HFilter &aFilter) {
  // Stores in the collection aCol pointers to all the objects in the category
  //verifying the condition given by the filter aFilter.
  //
  // Returns kTRUE if everything works Ok, kFALSE in other case
  HIterator *iter=(HIterator *)MakeIterator();
  TObject *data=NULL;
  iter->Reset();
  while ( (data=(TObject *)iter->Next())!=NULL) {
    if (aFilter.check(data)) aCol->Add(data);
  }
  return kTRUE;
}

Bool_t HCategory::query(TCollection *aCol,HLocation &aLoc) {
  // Stores in the collection aCol pointers to all the objects in the category
  //corresponding to the location aLoc.
  //
  // Returns kTRUE if everything works Ok, kFALSE in other case
  HIterator *iter=(HIterator *)MakeIterator();
  TObject *data=NULL;
  iter->gotoLocation(aLoc);
  while ( (data=(TObject *)iter->Next())!=NULL) {
    aCol->Add(data);
  }
  return kTRUE;
}

Bool_t HCategory::query(TCollection *aCol,HLocation &aLoc,HFilter &aFilter) {
  // Stores in the collection aCol pointers to all the objects in the category
  //verifying the condition given by the filter aFilter and corresponding to 
  //the location aLoc
  //
  // Returns kTRUE if everything works Ok, kFALSE in other case.
  HIterator *iter=(HIterator *)MakeIterator();
  TObject *data=NULL;
  iter->gotoLocation(aLoc);
  while ( (data=(TObject *)iter->Next())!=NULL) {
    if (aFilter.check(data)) aCol->Add(data);
  }
  return kTRUE;
}

const Text_t *HCategory::getClassName(void) {
  AbstractMethod("getClassName");
  return "";
}

void HCategory::makeBranch(TBranch *parent) { 
  AbstractMethod("makeBranch"); 
}

TObject *&HCategory::getNewSlot(HLocation &aLoc,Int_t* pIndex) { 
  AbstractMethod("getNewSlot");
  return gNullObjectP;
}

TObject *&HCategory::getSlot(HLocation &aLoc,Int_t* pIndex) {
  AbstractMethod("getSlot");
  return gNullObjectP;
}

TObject *HCategory::getObject(HLocation &aLoc) {
  AbstractMethod("getObject");
  return 0;
}

Bool_t HCategory::filter(HFilter &aFilter) {
  AbstractMethod("filter");
  return kFALSE;
}

Bool_t HCategory::filter(HLocation &aLoc,HFilter &aFilter) {
  AbstractMethod("filter");
  return kFALSE;
}

void HCategory::Clear(Option_t *opt) {
  AbstractMethod("Clear");
}

Bool_t HCategory::isSelfSplitable(void) {
  AbstractMethod("isSelfSplitable");
  return kFALSE;
}

TIterator *HCategory::MakeIterator(Option_t *opt, Bool_t dir) {
  AbstractMethod("MakeIterator");
  return 0;
}

void HCategory::Streamer(TBuffer &R__b)
{
   // Stream an object of class HCategory.
   Bool_t temp; 
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      if (R__v<2) R__b >> temp;
      R__b >> fCat;
      R__b >> fBranchingLevel;
      R__b >> fHeader;
   } else {
      R__b.WriteVersion(HCategory::IsA());
      TObject::Streamer(R__b);
//      R__b << fPersistency;
      R__b << fCat;
      R__b << fBranchingLevel;
      R__b << fHeader;
   }
}


