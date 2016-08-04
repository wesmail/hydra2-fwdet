#ifndef HTASK_H
#define HTASK_H

#include "TNamed.h"

 
typedef Int_t TaskControl; 
const TaskControl  kSTOP = -1;
const TaskControl  kGOFORWARD  = 0; 
const TaskControl  kGOBACKWARD = 1;



class HTask : public TNamed {
protected:
  HTask* owner;
  Bool_t manual;
  Bool_t isInitialised;
  Bool_t isConnected; 
  Bool_t fIsTimed;     // timer flag

public:
  virtual Bool_t connectTask(HTask *task,Int_t n)=0;

public:
  HTask(void) {}
  HTask(const Text_t name[],const Text_t title[]): TNamed(name,title), manual(kFALSE),
  isInitialised(kFALSE), isConnected(kFALSE) {}
  virtual ~HTask(void) {}
  virtual HTask *next(Int_t &errCode)=0;
  virtual HTask *getTask(const Char_t *name)=0;
  virtual Bool_t init(void)=0;
  virtual Bool_t reinit(void) {return kTRUE;}

  virtual Bool_t finalize(void)=0;
  virtual void getConnections(){;}
  virtual HTask* getComposite()=0; 
  virtual HTask* getOwner(){return owner;}
  virtual void setOwner(HTask* atask){owner=atask;}
  virtual Bool_t setConnections(){ return kTRUE;}
  void setManual(){ manual=kTRUE;}
  //void error(void);
  virtual void isTimed(Bool_t)=0;
  virtual void resetTimer()=0;
  virtual void printTimer()=0;
  ClassDef(HTask,1) //ABC for a task
};


#endif /* !HTASK_H */





