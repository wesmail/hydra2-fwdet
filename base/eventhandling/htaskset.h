#ifndef HTASKSET_H
#define HTASKSET_H

#include "THashList.h"
#include "THashTable.h"
#include "TOrdCollection.h"
#include "htask.h"

class HTaskSet : public HTask {
protected:
  TOrdCollection fTasks; // Tasks in this set
  HTask *fNextTask;
  HTask *fFirstTask;
  Int_t ids[8];

private:
    HTaskSet(HTaskSet &ts);
public:
  HTaskSet(void);
  HTaskSet(const Text_t name[],const Text_t title[]);
  ~HTaskSet(void);
  
  // initialisation
  Bool_t init(void);
  Bool_t finalize(void);
  Bool_t reinit(void);
  
  // connection via macros
  Bool_t add(HTask *);
  Bool_t connect(HTask *);
  Bool_t connect(HTask *,HTask *,Int_t n = kGOFORWARD);
  Bool_t connect(HTask *,const Text_t *where,Int_t n = kGOFORWARD);
  Bool_t connect(const Text_t task[],const Text_t where[],Int_t n=0);
  Bool_t connectTask(HTask *,Int_t n);  
  // clearing of table
  void Clear(Option_t *opt="");
  
  // function to be used for derived task sets (HRichTaskSet, etc.)
  HTask *make(const Char_t *select="",const Option_t *option="") {return 0;}

  //Control execution flow
  HTask *next(Int_t &errCode);
  HTask *next(Int_t &errCode, Int_t id);
  HTask *operator()(Int_t &errCode) {return next(errCode);}
  //characteristics
  HTask* getComposite() {return this;}
  TOrdCollection* getSetOfTask() {return &fTasks;}
  HTask* getTask(const Char_t *name);
  //Dump of connections
  void print();

  //Browsing
  Bool_t IsFolder(void) const;
  void Browse(TBrowser *b);
  void isTimed(Bool_t flag=kTRUE);
  void resetTimer(void);
  void printTimer(void);

  //Event ids
  void    setIds(Int_t i0, Int_t i1=-1, Int_t i2=-1, Int_t i3=-1, Int_t i4=-1,
              Int_t i5=-1, Int_t i6=-1, Int_t i7=-1);
  Int_t*  getIds(Int_t &size){size=sizeof(ids)/sizeof(Int_t);return &ids[0];}
  ClassDef(HTaskSet,2) // Set of tasks
};

#endif /* !HTASKSET_H */

