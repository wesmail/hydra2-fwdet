using namespace std;
#include "htaskset.h"
#include "hdebug.h"
#include "hmessagemgr.h"
#include "hades.h"
#include "TROOT.h"
#include "TClass.h"
#include "haddef.h"
#include "TBrowser.h"

#include <iostream>
#include <iomanip>

//*-- Author : Manuel Sanchez
//*-- Modified: 13/02/2004 by R. Holzmann
//*-- Modified: 19/11/2001 by D. Magestro
//*-- Modified: 09/03/2000 by R. Holzmann
//*-- Modified: 26/5/1999 by D.Bertini
//*-- Modified: 9/11/1998

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HTaskSet
//
// This HTask is in fact a set of tasks arbitrarily connected among themselves.
//
// The tasks are connected such that when a task is finished a pointer to
// the next task is retrieved; note that the tasks must not necessarily be
// connected in a linear fashion.
///////////////////////////////////////////////////////////////////////////////

HTaskSet::HTaskSet(const Text_t name[],const Text_t title[]) : HTask(name,title) {
  // Constructor
  fFirstTask=0;
  fNextTask=0;
  fIsTimed=kFALSE;
  owner=0;
  for (Int_t i=0;i<8;i++) ids[i] = -1;
}


HTaskSet::HTaskSet(void) {
  // Default constructor
  fIsTimed=kFALSE;
  owner=0;
  for (Int_t i=0;i<8;i++) ids[i] = -1;
}

/*
// old code .... its not a copy and and not used ... so don"t allow it
 HTaskSet::HTaskSet(HTaskSet &ts) {
  // Copy constructor.
  fFirstTask=ts.fFirstTask;
  fNextTask=ts.fNextTask;
  fTasks.AddAll(&ts.fTasks);
  fIsTimed=ts.fIsTimed;
  owner=ts.owner;
  for (Int_t i=0;i<8;i++) ids[i] = ts.ids[i];
}
*/

HTaskSet::~HTaskSet(void) {
  // Destructor.
  fTasks.Delete();
}


HTask* HTaskSet::getTask(const Char_t *name) {
  // return pointer to first task of given name found
  if (strcmp(GetName(),name)==0) return this;

  HTask* task=NULL;
  HTask* found=NULL;
  TIterator *iter=fTasks.MakeIterator();
  while ( (task=(HTask*)iter->Next()) ) {
    if ( (found=task->getTask(name)) ) return found;
  }
  return NULL;
}


Bool_t HTaskSet::add( HTask* task ){
 // For the moment only elementary task are added

 if ( fTasks.IsEmpty() ) {
   fTasks.Add( task );
   fFirstTask=task;
   task->setOwner(this);
   return kTRUE;
 } else {
   HTask* pTask = (HTask*) fTasks.Last();
   pTask->connectTask(task,0);
   if (!(fTasks.FindObject(task))) fTasks.Add( task );
   task->setOwner(this);
   return kTRUE;
 }
}

Bool_t HTaskSet::connect(HTask *task) {
  // Connects "task" as the first task to be performed in the set.
  if (!fTasks.FindObject(task)) fTasks.Add(task);
  fFirstTask=task;
  task->setOwner(this);
  return kTRUE;
}


Bool_t HTaskSet::connect(HTask *task,HTask *where,Int_t n) {
  // connects task "task" to the task "where" with parameter "n"
  if(!task) return kTRUE;
  if (where) {
    if (fTasks.FindObject(where)) {
      where->connectTask(task,n);
      if (!(fTasks.FindObject(task))) fTasks.Add(task);
    } else return kFALSE;
  } else {
    fTasks.Add(task);
    fFirstTask=task;
  }
  task->setOwner(this);
  return kTRUE;
}


Bool_t HTaskSet::connect(HTask *task,const Text_t *where,Int_t n) {
  // Connects task "task" to the task with name "where" using "n" as parameter.
  if(!task) return kTRUE;
  if(where) {
    HTask *wh=NULL;
    wh=(HTask *)fTasks.FindObject(where);
    if (wh) {
      if(wh->connectTask(task,n)) {
        if (!(fTasks.FindObject(task))) fTasks.Add(task);
      } else {
        WARNING_msg(10,HMessageMgr::DET_ALL,"Problem in connecting task!\n");
        return kFALSE;
      }
    } else return kFALSE;
    task->setOwner(this);
    return kTRUE;
  }
  return kFALSE;
}


Bool_t HTaskSet::connect(const Text_t task[],const Text_t where[],Int_t n) {
  // Connects the task named "task" to the one named "where" using "n" as
  // parameter.
  HTask *ta=NULL,*wh=NULL;
  wh=(HTask *)fTasks.FindObject(where);
  ta=(HTask *)fTasks.FindObject(task);
    if (ta && wh) {
      wh->connectTask(ta,n);
      if (!(fTasks.FindObject(task))) fTasks.Add(ta);
    } else return kFALSE;
  ta->setOwner(this);
  return kTRUE;
}


void HTaskSet::Clear(Option_t *opt) {

//FIXME: "HTaskSet::Clear should clear not delete..."
  fTasks.Delete();
  fFirstTask=NULL;
  fNextTask=NULL;
}


Bool_t HTaskSet::init(void) {
  // Calls the init function for each of the tasks in the task set.
    TIter next(&fTasks);
  HTask *task=NULL;
  if(fIsTimed) resetTimer();
  while ( (task=(HTask *)next())!=NULL) {
    if (!task->init()) {
	Error("init","%s initialization failed!",task->GetName());
	return kFALSE;
    }
  }
  return kTRUE;
}


Bool_t HTaskSet::reinit(void) {
  // Calls the init function for each of the tasks in the task set.
  TIter next(&fTasks);
  HTask *task=NULL;
  while ( (task=(HTask *)next())!=NULL) {
      if (!(task->reinit())) { Error("reinit()","Error returned from %s",task->GetName()); return kFALSE; }
  }
  return kTRUE;
}


Bool_t HTaskSet::finalize(void) {
  // Calls the finalize function for each of the tasks in the set
  TIter next(&fTasks);
  HTask *task=NULL;
  while ( (task=(HTask *)next())!=NULL) {
    if (!task->finalize()) return kFALSE;
  }
  return kTRUE;
}


void HTaskSet::print(void) {
  // Dumps dependencies for each of the tasks in the set
    TIter next(&fTasks);
    HTask *task=NULL;

    if(owner==NULL) {
	gHades->getMsg()->info(10,HMessageMgr::DET_ALL,GetName(),"=== TASK SET: %-15s ========================\n",
			       GetName());
    }
    else if(owner->getOwner()==NULL) {
	gHades->getMsg()->info(10,HMessageMgr::DET_ALL,GetName(),"--- Task Set: %-15s -------------------\n",
			       GetName());
    }
    else {
	gHades->getMsg()->info(10,HMessageMgr::DET_ALL,GetName(),"- - Task set: %-15s - - - - - - - -\n",
			       GetName());
    }
    while ( (task=(HTask *)next())!=NULL) {
	if(strcmp((task->IsA())->GetName(),"HTaskSet")==0) {
	    ((HTaskSet*) task)->print();
	    SEPERATOR_msg("-",63);
	    //printf("\n");
	}
	else { task->getConnections(); }
    }

    if(owner==NULL) { SEPERATOR_msg("=",63); }
    //printf("================================================================\n\n");
//  else
//  printf("-----------------------------------------------------------\n");
}


void HTaskSet::setIds(Int_t i0, Int_t i1, Int_t i2, Int_t i3,  Int_t i4,
                      Int_t i5, Int_t i6, Int_t i7) {
  // set event ids for which this task set should execute
  //
  ids[0] = i0;
  ids[1] = i1;
  ids[2] = i2;
  ids[3] = i3;
  ids[4] = i4;
  ids[5] = i5;
  ids[6] = i6;
  ids[7] = i7;

}

HTask* HTaskSet::next(Int_t &errCode, Int_t evtId) {
  // do tasks only if evtId matches with one of the 8 tabulated ids
  //
  if (fFirstTask) {
    for (Int_t i=0;i<8; i++) if (evtId==ids[i]) return next(errCode);
  }
  return NULL;
}

HTask *HTaskSet::next(Int_t &errCode) {
  // Iterates throught the task set. When the iteration is finished it
  // returns a pointer to the next task as set by connectTask()
  HTask *task;
  if (fFirstTask==NULL) return fNextTask;   // task set is empty
  Int_t err=0;

  //loop over task in the list
  task=fFirstTask;
  while (task) {
    task=task->next(err);
  }
  errCode=err;
     // check the errcode value stop and skipevent
     // (when skipping an event in a multi-tasklist
     // layout, the next tasklist is correctly skipped

  if (errCode ==  kSTOP || errCode == kSkipEvent) return NULL;
  return fNextTask;
}


Bool_t HTaskSet::connectTask(HTask *task,Int_t) {
  // Connects "task" as the next task to be performed; n is ignored.
  fNextTask=task;
  return kTRUE;
}


Bool_t HTaskSet::IsFolder(void) const {
  // Returns true. This tells the Root browser to show HTaskSet as a folder
  // holding other objects
  return kTRUE;
}


void HTaskSet::Browse(TBrowser *b) {
  // Adds the tasks in the this taskset to the current ROOT browser.
  //
  // This function is called by Root when browsing gHades with the Root browser
  //

  TIterator *next=fTasks.MakeIterator();
  TObject *task=0;

  while ( (task=(HTask *)next->Next()) != 0) {
    b->Add(task);
  }
  delete next;
}


void HTaskSet::isTimed(Bool_t flag) {
  // Set timing flag for all tasks in set
  TIter next(&fTasks);
  HTask *task=NULL;
  while ( (task=(HTask *)next()) != NULL ) task->isTimed(flag);
  fIsTimed = flag;
}


void HTaskSet::resetTimer(void) {
  // Calls the reset function for each of the task timers in the task set.
  if(fIsTimed) {
    TIter next(&fTasks);
    HTask *task=NULL;
    while ( (task=(HTask *)next()) != NULL ) task->resetTimer();
  }
}


void HTaskSet::printTimer(void) {
  // Calls the print function for each of the task timers in the task set.
  if(fIsTimed) {
    TIter next(&fTasks);
    HTask *task=NULL;
    gHades->getMsg()->infoB(10,HMessageMgr::DET_ALL,"Timing information for taskset %s:\n",this->GetName());
    while ( (task=(HTask *)next()) != NULL ) task->printTimer();
  }
  else gHades->getMsg()->infoB(10,HMessageMgr::DET_ALL,"No timing information has been collected for taskset %s!\n",
                                                             this->GetName());
}

void HTaskSet::Streamer(TBuffer &R__b)
{
   // Stream an object of class HTaskSet.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      HTask::Streamer(R__b);
      fTasks.Streamer(R__b);
      R__b >> fNextTask;
      R__b >> fFirstTask;
      if (R__v > 1 )R__b.ReadStaticArray(ids);
      else for (Int_t i=0; i<8; i++) ids[i] = 0;
      R__b.CheckByteCount(R__s, R__c, HTaskSet::IsA());
   } else {
      R__c = R__b.WriteVersion(HTaskSet::IsA(), kTRUE);
      HTask::Streamer(R__b);
      fTasks.Streamer(R__b);
      R__b << fNextTask;
      R__b << fFirstTask;
      R__b.WriteArray(ids, 8);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

ClassImp(HTaskSet)
































