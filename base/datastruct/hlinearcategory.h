#ifndef HLINEARCATEGORY_H
#define HLINEARCATEGORY_H

#include "hcategory.h"

class HLinearCatIter;

class HLinearCategory : public HCategory {
friend class HLinearCatIter;
protected:
  TClonesArray *fData; // Data array of the category
  Int_t fNDataObjs;
  Bool_t hasDynamicObjects;
public:
  HLinearCategory(void);
  HLinearCategory(const Text_t *className,Int_t size=1000);
  ~HLinearCategory(void);
  const Text_t *getClassName(void);
  TClass *getClass(void) {return fData->GetClass();}
  void makeBranch(TBranch *parent);
  void activateBranch(TTree *tree,Int_t splitLevel);
  Int_t getEntries() {return fData->GetEntriesFast();}
  TObject *&getNewSlot(HLocation &aLoc,Int_t* pIndex=0);
  TObject *&getSlot(HLocation &aLoc,Int_t* pIndex=0);
  TObject *&getSlot(Int_t index);
  TObject *getObject(HLocation &aLoc);
  TObject *getObject(Int_t index);
  const TObjArray* getData() { return fData;}
  Bool_t filter(HFilter &aFilter);
  Bool_t filter(HLocation &aLoc,HFilter &aFilter);
  void Clear(const Option_t *opt="");
  void Compress(void) {    // compresss the TClonesArray
     fData->Compress();
     fNDataObjs = fData->GetEntries();
  }
  void setDynamicObjects(Bool_t isDyn=kTRUE);
  Bool_t getDynamicObjects() { return hasDynamicObjects; }
  void freeDynamicMemory();
  
  inline Bool_t isSelfSplitable(void);
  TIterator *MakeIterator(const Option_t* opt="catIter",Bool_t dir=kIterForward);
  Int_t getIndex(HLocation &aLoc) {return aLoc[0];}
  Int_t getIndex(TObject* pObj) {return fData->IndexOf(pObj);}
  void  sort(void) {fData->Sort(fNDataObjs);}
  const TCollection* getCollection(){return fData;} 
  ClassDef(HLinearCategory,2) // Generic data category (1-dim array of objects)
};
  
//--------------------inlines--------------------------------
inline Bool_t HLinearCategory::isSelfSplitable(void) {
 return kFALSE;
}

#endif /* !HLINEARCATEGORY_H */







