#ifndef HRECEVENT_H
#define HRECEVENT_H

#include "hevent.h"
#include "TClonesArray.h"
#include "heventheader.h"
#include "TObjArray.h"

class HTrack;
class HPartialEvent;
class HTree;

class HRecEvent : public HEvent {
protected:
  Int_t fRecLevel;        //  Reconstruction level for this event
  Int_t fNTracks;         //  Number of reconstructed tracks in this event (not used!)
  HEventHeader *fHeader;  //! Event header
  TClonesArray *fTracks;  //  Reconstructed tracks for this event (not used!)
  TObjArray *fPartialEvs; //! List of partial events in this event

public:
  HRecEvent(void);
  ~HRecEvent(void);
  void makeBranch(TBranch *parent,HTree* tree);
  void activateBranch(TTree *tree,Int_t splitLevel);
  HTrack *newTrack(void);
  void addTrack(HTrack &aTrack);
  HTrack *getTrack(UInt_t aId);
  void clearTracks(void);
  HEventHeader *getHeader(void) const {return fHeader;} 
  void setRecLevel(Int_t aRecLevel);
  Int_t getRecLevel(void); 
  HPartialEvent *addPartialEvent(Cat_t eventCat,const Text_t *name,
				 const Text_t *title);
  void addPartialEvent(HPartialEvent*);
  TObjArray * getPartialEvents() { return fPartialEvs;}
  HPartialEvent *getPartialEvent(Cat_t idx);
  Bool_t removePartialEvent(Cat_t cat);
  void merge(HRecEvent* ev);
  HCategory *getCategory(Cat_t aCat);
  Bool_t addCategory(Cat_t aCat,HCategory *cat,Option_t opt[]);
  Bool_t removeCategory(Cat_t aCat);
  Bool_t IsFolder(void) const {return kFALSE;}
  void Browse(TBrowser *b);
  void Clear(Option_t *opt="");
  void clearAll(Int_t level);
  void setExpandedStreamer(Bool_t t);
  Bool_t hasExpandedStreamer(void) {return TestBit(32); }
  
  ClassDef(HRecEvent,2) //A HADES event under reconstruction
};

#endif /* !HRECEVENT_H */ 


 
