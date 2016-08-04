#include "TObject.h"
#include "hades.h"
#include "hreconstructor.h"
#include "TClass.h"
//*-- Author : Manuel Sanchez
//*-- Modified : 22/04/2002 by R. Holzmann
//*-- Modified : 9/10/1998 by Manuel Sanchez
//*-- Modified : 27/04/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////
//HReconstructor
//
//  Base class for any reconstruction method or algoritm. See HTask for
// more information.
//
////////////////////////////////////////////////////////////////////////

ClassImp(HReconstructor)

HReconstructor::HReconstructor(void) :
                             HTask("Reconstructor","Generic reconstructor") {
  // Default reconstructor
  fActive=kTRUE;
  fHistograms=NULL;
  fIsTimed=kFALSE;
}

HReconstructor::HReconstructor(const Text_t *name,const Text_t *title) : HTask(name,title){
  // Allocates a new reconstructor with name=name and Title=title
  fActive=kTRUE;
  fHistograms=NULL;
  fIsTimed=kFALSE;
}

HReconstructor::~HReconstructor(void) {
  //Destructor
}

HTask *HReconstructor::next(Int_t &errCode) {
  //Returns next task to be performed according to the return value of the
  //execute() function
  if(fIsTimed) fTimer.Start(kFALSE);
  Int_t e=(fActive) ? execute() : 0;
  if(fIsTimed) fTimer.Stop();
  if (e<0) {
    errCode=e;
    //    printf("task :%s ",this->GetName());
    //  printf("exited with errCode:%i \n",errCode);
    return NULL;
  }
  errCode=0;
  return (HTask *)fOutputs[e];
}


HTask *HReconstructor::getTask(const Char_t *name) {
  if (strcmp(GetName(),name)==0) return this;
  else return NULL;
}


Bool_t HReconstructor::connectTask(HTask *task,Int_t n) {
  // Connects the return value "n" to the task "task"
  if (!fOutputs[n]) 
    fOutputs.AddAt(task,n);
  else { 
    printf("Overwritten fOutputs!\n"); 
    return kFALSE;
  }
  return kTRUE;
}


void HReconstructor::getConnections(void){

Int_t entries = fOutputs.GetEntries();
  printf(".................................................\n");
  printf("Tasks connected to %s:\n\n",this->GetName());
  if( entries == 0 ) {
    printf("   None!\n");
    printf(".................................................\n");
  } else {
    for(Int_t i=0; i<=fOutputs.GetLast();i++){ 
      if (fOutputs[i]) printf("   %s is connected via 'return %3i' \n",
                                                 fOutputs[i]->GetName(), i); 
    }
  }
}


Bool_t HReconstructor::IsFolder(void) const {
  // Returns true. This tells the Root browser to show HReconstructor as a 
  // folder holding other objects
  return kTRUE;
}


void HReconstructor::Browse(TBrowser *b) {
  // Browse function to see the subnodes and histograms supported by a
  // particular reconstructor in a Root Browser.

  if (fHistograms) b->Add(fHistograms,"Control histograms");
}


void HReconstructor::resetTimer(void) {
  // Resets task timer
  //
  if(fIsTimed) fTimer.Reset(); 
}


void HReconstructor::printTimer(void) {
  // Print task timer output
  //
  if(!fActive) {
    printf("%15s:  not active\n",this->GetName());
    return;
  } 
  if(fIsTimed) {
    Float_t realTime = fTimer.RealTime();
    Float_t cpuTime = fTimer.CpuTime();
    printf("%15s:  Real time: %10.2f sec   CPU time: %10.2f sec\n",
                                            this->GetName(),realTime,cpuTime);
  }
  else printf("%15s:  not timed\n",this->GetName());
}






