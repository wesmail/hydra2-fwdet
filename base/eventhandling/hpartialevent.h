#pragma interface
#ifndef HPARTIALEVENT_H
#define HPARTIALEVENT_H

#include "hevent.h"
#include "TTree.h"
#include "TBranch.h"
#include "TObjArray.h"

class HCategory;
class HTree;

class HPartialEvent : public HEvent
{
protected:
  Int_t fRecLevel; //Reconstruction level of the event
  Cat_t fBaseCategory; //Identifier for the first category stored in this partial event
  TObject *fHeader; // Sub event's header.
  TObjArray *fCategories; //! Categories 
public:
  HPartialEvent(void);
  HPartialEvent(const Text_t *aName,const Text_t *aTitle,Cat_t aBaseCat);
  ~HPartialEvent(void);
  Cat_t getBaseCat(void) {return fBaseCategory;}
  void makeBranch(TBranch *parent,HTree* tree);
  void activateBranch(TTree *tree,Int_t splitLevel);
  void addCategory(Cat_t aCat,HCategory *category);
  TObjArray *getCategories(void);
  HCategory *getCategory(Cat_t aCat);
  Bool_t addCategory(Cat_t aCat,HCategory *cat,Option_t opt[]);
  Bool_t removeCategory(Cat_t aCat);
  void setRecLevel(Int_t aRecLevel);
  Int_t getRecLevel(void);
  void setSubHeader(TObject *header) {fHeader=header;}
  TObject *getSubHeader(void) {return fHeader;}
  virtual void Clear(Option_t *opt="");
  void clearAll(Int_t level);
  Bool_t isEmpty(void) {return fCategories->IsEmpty();}
  Bool_t IsFolder(void) const {return kFALSE;}
  void setExpandedStreamer(Bool_t t) { SetBit(32,t); }
  Bool_t hasExpandedStreamer(void) {return TestBit(32); }
  void Browse(TBrowser *b);
  void setPersistency(Bool_t fPersistency);
  ClassDef(HPartialEvent,2) //Event data corresponding to each main detector
};

#endif /* !HPARTIALEVENT_H */
