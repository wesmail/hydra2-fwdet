#ifndef HMATRIXCATEGORY_H
#define HMATRIXCATEGORY_H

#include "hcategory.h"
#include "hindextable.h"

class TArrayI;
class TClonesArray;
class HMatrixCatIter;
class TClass;

class HMatrixCategory : public HCategory {
friend class HMatrixCatIter;
protected:
  HIndexTable *fIndexTable; // Table of the position in fData of data objects
  Int_t fNDataObjs; //Number of data objects actually stored.
  TClonesArray *fData; // Container for the data in the Matrix category;
public:
   HMatrixCategory(void);					        
   HMatrixCategory(const Text_t *className,Int_t nSizes,Int_t *sizes,Float_t fillRate=0.1);
   ~HMatrixCategory(void);
   void setup(const Text_t *className,Int_t nSizes,Int_t *sizes,Float_t fillRate=0.1);
   const Text_t *getClassName(void);
   const TCollection* getCollection(){return fData;} 
   TClass *getClass(void);
   TArrayI *getSizes(void);
   Int_t getSize(Int_t aIdx);
   Int_t getEntries() {return fData->GetEntriesFast();}
   void activateBranch(TTree *tree,Int_t splitLevel);
   void makeBranch(TBranch *parent);
   Bool_t isSelfSplitable(void);
   TObject *&getNewSlot(HLocation &aLoc,Int_t *pIndex=0);
   TObject *&getSlot(HLocation &aLoc,Int_t *pIndex=0);
   TObject *getObject(HLocation &aLoc);
   TObject *getObject(Int_t index);
   Int_t getIndex(HLocation &aLoc) {return fIndexTable->getIndex(aLoc);}
   Int_t getIndex(TObject* pObj) {return fData->IndexOf(pObj);}
   Bool_t query(TCollection *aCol,HFilter &aFilter);
   Bool_t query(TCollection *aCol,HLocation &aLoc);
   Bool_t query(TCollection *aCol,HLocation &aLoc,HFilter &aFilter);
   Bool_t filter(HFilter &aFilter);
   Bool_t filter(HLocation &aLoc,HFilter &aFilter);
   TIterator *MakeIterator(const Option_t* opt="catIter",Bool_t dir=kIterForward);
   void Clear(const Option_t *opt="");
   void Browse(TBrowser *b);
   ClassDef(HMatrixCategory,1) //Category where data are stored in a vector
};

#endif /* !HMATRIXCATEGORY_H */




