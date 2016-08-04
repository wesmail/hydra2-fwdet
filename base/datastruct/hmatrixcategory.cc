using namespace std;
#include "hmatrixcategory.h"
#include "hmatrixcatiter.h"
#include "hades.h"
#include "TBrowser.h"
#include "TObject.h"
#include "TArrayI.h"
#include "TClonesArray.h"
#include "TClass.h"
#include "hlocation.h"
#include "htree.h"
#include "hfilter.h"
#include "hdebug.h"
#include <iostream> 
#include <iomanip>
//*-- Author : Manuel Sanchez
//*-- Modified : 22/02/02 by R. Holzmann
//*-- Modified : 18/05/98 by Manuel Sanchez
//*-- Modified:  05/12/98  by D.Bertini (problem with clear and multiple hits)
//*-- Modified:  29/02/00  by D.Bertini 
//*-- Copyright : GENP (Univ. Santiago de Compostela)


//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////
//HMatrixCategory
//
//  In this particular HCategory all the objects belonging to the category
// are stored in a matrix-like fashion; so before start using the category it 
// is needed to indicate the number of indexes in this matrix and the range of
// each index
//
//  When the matrix category is stored in a tree it creates one superbranch 
// holding one subbranch per data member in the objects held by the category. 
// All the objects held by the category are stored through that superbranch. 
// 
////////////////////////////////////

ClassImp(HMatrixCategory)

HMatrixCategory::HMatrixCategory(void)  {
  //Default constructor
 fData=NULL;
 fIndexTable=NULL;
}

HMatrixCategory::HMatrixCategory(const Text_t *className,
				 Int_t nSizes,
				 Int_t *sizes,
				 Float_t fillRate) 
{
  // Creates a HMatrixCategory holding objects of class "className" clasified
  //by nSizes indexes (giving their location) whose maximun values are the
  //indicated in the vector "sizes".
  //
  // However it only allocates memory for fillRate*(maximun number of objects)
  //data objects.
  fData=NULL;
  fIndexTable=new HIndexTable;
  setup(className,nSizes,sizes,fillRate);

}

HMatrixCategory::~HMatrixCategory(void) {
  //Destructor
  if (fData) {fData->Clear(); delete fData;}
  if (fIndexTable) delete fIndexTable;
}

void HMatrixCategory::setup(const Text_t *className,Int_t nSizes,Int_t *sizes,Float_t fillRate) {
  //This method sets up a category.
  //
  //Input:
  //  className ---> a string with the name of the object's class stored in 
  //                 the category
  //  nSizes ---> Number of indexes needed to access an object (fBranchingLevel)
  //  sizes ---> a vector of ints containing the maximum value for each of the
  //             indexes before.
  //  fillRate---> Expected fill factor in the category 
  //               (number of slots used)/(maximun number of slots)
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("MatrixCategory::setup");
#endif
 fBranchingLevel=nSizes;
 fIndexTable->setDimensions(nSizes,sizes);
 fNDataObjs=0;
#if DEBUG_LEVEL>2
 gDebuger->message("Allocation data clones array");
#endif
 if (fData) delete fData;
 fData=new TClonesArray(className,(Int_t)(fIndexTable->getEntries()*fillRate));
#if DEBUG_LEVEL>2
 gDebuger->leaveFunc("HMatrixCategory::setup");
#endif
}

const Text_t *HMatrixCategory::getClassName(void) {
  //Returns the class' name of the objects held by the category
 if (fData)
  return fData->GetClass()->GetName();
 return NULL;
}

TClass *HMatrixCategory::getClass(void) {
  if (fData)
    return fData->GetClass();
  return NULL;
}

TArrayI *HMatrixCategory::getSizes(void) {
  //returns an array of ints with the maximun value for the indexes used to 
  //get an object
 return fIndexTable->getDimensions();
}

Int_t HMatrixCategory::getSize(Int_t aIdx) {
  //Returns the maximun value of the index aIdx
 return (fIndexTable->getDimensions()->At(aIdx));
}

void HMatrixCategory::activateBranch(TTree *tree,Int_t splitLevel) {
  //Activates the branches in tree matching the appropiate names.
  TBranch *brHead=0;
  Char_t name[256];

  if (splitLevel)
    if (fHeader && tree) {
      sprintf(name,"%s.Header",getClassName());
      brHead=tree->GetBranch(name);
      if (brHead) { //Pre ROOT3 splitting
        tree->SetBranchAddress(name,&fHeader);
	tree->SetBranchStatus(name,1);
      }
    }
}

void HMatrixCategory::makeBranch(TBranch *parent) {
  //This function should not be called, since HMatrixCategory is not self
  //splittable
}

Bool_t HMatrixCategory::isSelfSplitable(void) {
  //Determines if the category can be automatically split or it has
  //its own splitting algorithm (in this last case the function makeBranch is 
  //called for splitting).
  //
  //returns kFALSE
  return kFALSE;
}

TObject *&HMatrixCategory::getNewSlot(HLocation &aLoc,Int_t *pIndex) {
  //Should not be used for HMatrixCategory
  Int_t la=0,lae=0,pos=0;
  TClonesArray &data=*fData;
  la=aLoc.getLinearIndex(fIndexTable->getDimensions());
  lae=la+fIndexTable->getDimensions()->At(fBranchingLevel-1);
  while (la<lae) {
    if (fIndexTable->getIndex(la)==-1) {
      pos=fNDataObjs;
      fNDataObjs++;
      fIndexTable->setIndex(la,pos);
      if(pIndex) *pIndex = pos;
      return data[pos];
    }
    la++;
  }
  if(pIndex) *pIndex = -1;
  return gNullObjectP;
}

TObject *&HMatrixCategory::getSlot(HLocation &aLoc,Int_t *pIndex) {
  //Returns a memory address corresponding to the HLocation aLoc, where a new
  //object can be instantiated
 Int_t la=0,pos=0,idx;
 TClonesArray &data=*fData;

 if (aLoc.getNIndex()<fBranchingLevel) return gNullObjectP;
 if (!fIndexTable->checkLocation(aLoc)) return gNullObjectP;

 la=aLoc.getLinearIndex(fIndexTable->getDimensions());
 idx=fIndexTable->getIndex(la);
 if (idx==-1) {
   pos=fNDataObjs;
   fNDataObjs++;
   fIndexTable->setIndex(la,pos);
 } else {
   pos=idx;
 }
 if(pIndex) *pIndex = pos;
 return data[pos];
}

TObject *HMatrixCategory::getObject(HLocation &aLoc) {
  //Returns the object at the HLocation aLoc (see HCategory)
 Int_t pos=0;
 if (!fIndexTable->checkLocation(aLoc)) return NULL;
 pos=fIndexTable->getIndex(aLoc);

 if (pos==-1) return NULL;
 return ((TObject *)fData->At(pos));
}

TObject *HMatrixCategory::getObject(Int_t index) {
  //Returns the object at index in internal TClonesArray
 if(index < 0 || index >= fNDataObjs) return NULL;
 else return ((TObject *)fData->At(index));
}

Bool_t HMatrixCategory::query(TCollection *aCol,HFilter &aFilter) {
  //Puts all objects in the category which pass the HFilter aFilter in the
  //collection pointed by aCol.
  TIter next(fData);
  TObject *data;

  if (!fData) return kFALSE;
  
  while ((data=(TObject *)next())!=NULL) {
    if (aFilter.check(data)) aCol->Add(data);
  }
  return kTRUE;
}

Bool_t HMatrixCategory::query(TCollection *aCol,HLocation &aLoc) {
  //Puts all objects in the category corresponding to the HLocation aLoc in 
  //the collection pointed by aCol

  Int_t i=0,n=0,pos=0;
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HMatrixCategory::query");
  gDebuger->message("entries=%i",fData->GetEntries());
#endif

  if (!fData) return kFALSE;

  n=fIndexTable->gotoLocation(aLoc);
 
  for (i=0;i<n;i++) {
    if ((pos=fIndexTable->next())!=-1) {
      aCol->Add(fData->At(pos));
    }
  }
  return kTRUE;
#if DEBUG_LEVEL > 2
  gDebuger->leaveFunc("HMatrixCategory::query");
#endif
}

Bool_t HMatrixCategory::query(TCollection *aCol,
			      HLocation &aLoc,
			      HFilter &aFilter) {
  //Puts all the objects in the category passing the HFilter aFilter and 
  //corresponding to the HLocation aLoc into the collection pointed by aCol
  Int_t i=0,n=0,pos=0;
  TObject *data=NULL;
 
  if (!fData) return kFALSE;

  n=fIndexTable->gotoLocation(aLoc);

  for (i=0;i<n;i++) {
   if ((pos=fIndexTable->next())!=-1) { 
      data=(TObject *)fData->At(pos);
      if (aFilter.check(data)) aCol->Add(data);
   }
  }
  return kTRUE;
}

Bool_t HMatrixCategory::filter(HFilter &aFilter) {
  //see HCategory
    Int_t i=0,j=0,nEntries=0;
  Int_t dest=-1;
  TObject *obj=NULL;
  nEntries=fData->GetEntriesFast();
  Int_t *changes= new Int_t[nEntries];

  if (changes==NULL) return kFALSE;
  //Pass filter
  for (i=0;i<nEntries;i++) {
    obj=(TObject *)fData->At(i);
    if (obj!=NULL)
      if (!aFilter.check(obj)) {(*fData)[i] = 0; fNDataObjs--;}
  }
  //Eliminate gaps in fData
  for (i=0,dest=0;i<nEntries;i++) {
    if (fData->At(i)!=NULL) {
      changes[i]=dest;
      dest++;
    } else changes[i]=-1;
  }
  //Compress array
  fData->Compress();
  //Update index table
  nEntries=fIndexTable->gotoBegin();
  fIndexTable->getCompactTable()->clear(); // reset properly index list
  for (i=0;i<nEntries;i++) {
    if ((j=fIndexTable->next())!=-1) fIndexTable->setIndex(i,changes[j]);
  }
  delete[] changes;
  return kTRUE;
  //Version 2
//   HIterator *p=NULL;
//   Int_t i=0,n=0;
//   TObject *obj=NULL;
//   p=(HIterator *)MakeIterator();
//   while ( (obj=(TObject *)p->Next())!=NULL) {
//     if (aFilter.check(obj)) {
//       if (!obj->HasIdentifier()) obj->setLocation(p->getLocation());
//     } else {
//       fData->RemoveAt(fIndexTable->getIndex(p->getLocation()));
//     }
//   }
//   fData->Compress();
//   fIndexTable->Clear();
//   n=fData->GetEntriesFast();
//   for (i=0;i<n;i++) {
//     fIndexTable->setIndex((*obj->getLocation()),i);
//   }
//   return kTRUE;
}

Bool_t HMatrixCategory::filter(HLocation &aLoc,HFilter &aFilter) {
  //see HCategory
  Int_t *changes=NULL,i=0,j=0,nEntries=0;
  Int_t nLocs=0;
  Int_t dest=-1;
  TObject *obj=NULL;
  nEntries=fData->GetEntriesFast();
  changes = new Int_t[nEntries];
  if (changes==NULL) return kFALSE;
  //Pass filter
  nLocs=fIndexTable->gotoLocation(aLoc);
  for (i=0;i<nLocs;i++) {
    j=fIndexTable->next();
    if (j!=-1) {
      obj=(TObject *)fData->At(j);
      if (obj!=NULL)
	if (!aFilter.check(obj)) {(*fData)[j] = 0; fNDataObjs--;}
    }
  }
  //Eliminate gaps in fData
  for (i=0,dest=0;i<nEntries;i++) {
    if (fData->At(i)!=NULL) {
      changes[i]=dest;
      dest++;
    } else changes[i]=-1;
  }
  //Compress array
  fData->Compress();
  //Update index table
  nEntries=fIndexTable->gotoBegin();
  fIndexTable->getCompactTable()->clear(); // reset properly index list 
  for (i=0;i<nEntries;i++) {
    if ((j=fIndexTable->next())!=-1) fIndexTable->setIndex(i,changes[j]);
  }
  delete[] changes;
  return kTRUE;
}

TIterator *HMatrixCategory::MakeIterator(const Option_t* opt,Bool_t dir) {
  //Returns an iterator, which iterates in the direction dir and (as default)
  //throuhg the whole category.
  TString option = opt; 
  option.ToLower();
  if(option.Contains("native")){ return fData->MakeIterator(dir);}
  HMatrixCatIter *iterator=NULL;
  iterator=new HMatrixCatIter(this,dir);
  return iterator;
}

void HMatrixCategory::Clear(const Option_t *opt) {
  // see HCategory
 
 fNDataObjs=0;
 fData->Clear(opt);
 fIndexTable->Clear();
}

void HMatrixCategory::Browse(TBrowser *b) {
  //Browse objects in this category
  if (fData) b->Add(fData,fData->GetClass()->GetName());
}

void HMatrixCategory::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMatrixCategory.
   Char_t clase[200];
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      HCategory::Streamer(R__b);
      R__b >> fIndexTable;
      R__b >> fNDataObjs;
      R__b.ReadString(clase,200);
      if ( fData && strcmp(clase,fData->GetClass()->GetName())==0)
       fData->Clear();
      else {
          delete fData;
	   fData=new TClonesArray(clase);
           fData->Clear();
      }
      fData->Streamer(R__b); 
   } else {
       R__b.WriteVersion(HMatrixCategory::IsA());
       HCategory::Streamer(R__b);
       R__b << fIndexTable;
       R__b << fNDataObjs;
       strcpy(clase,fData->GetClass()->GetName());
       R__b.WriteString(clase);
       fData->Streamer(R__b);
   }
}

