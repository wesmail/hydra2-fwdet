using namespace std;
#include "hdetector.h"
#include <iostream> 
#include <iomanip>

//*-- Author : I. Koenig, D. Bertini, M. Sanchez
//*-- Modified : 6/11/1998 by Manuel Sanchez 
//*-- Modified : 17/02/2000 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//  HDetector
//
//  class to store the actual setup of a detector
//
//  The setup is defined in the macro via the memberfuntion
//  setModules(Int_t sector,Int_t* modules). The array must
//  have the size of the maximum mumber of modules in a
//  sector for this detector. If the module if not active the
//  according number is 0.
//  e.g. for Mdc:   Int_t mod[4]={1,2,0,0};
//                  mdc.setModules(0,mod);
//       This activates the modules 1 and 2 in sector 0
//  Detectors without a sector wise segmentation as e.g. the Start detector
//  use sector = -1.
//
//  It also holds the intelligence to build tasks related to this particular
//  detector as well as to create data structures for it.
//
//  This intelligence is stored in the functions: buildTask() and 
//  buildCategory() and has the form of compiled C++ code.
//
//  The function HTask *HDetector::buildTask(Text_t *task, Option_t *opt) 
//  builds a task object corresponding to the task identified by the string
//  "task". This can be an atomic task (a HReconstructor) or a set of them, in
//  this case the tasks will be packed in a HTaskSet so that buildTask() returns
//  a pointer to that HTaskSet. "opt" is a string where extra options can be 
//  given; the available options differ from detector to detector.
//
//  The function HCategory* HDetector::buildCategory(Cat_t cat) uses the
//  setup information in the HDetector to build a category object. The category
//  class and it's setup will be defined by "cat"; i.e. the HDetector has built
//  in the information of which category subclass is recommended for each kind
//  of data.
////////////////////////////////////////////////////////////////////////////////


HDetector::HDetector(void) {
  //Constructor
  maxSectors=6;
  maxModules=0;
  modules=0;
}

HDetector::HDetector(const Text_t* name,const Text_t* title) : TNamed(name,title) {
  //Constructor with name
  maxSectors=6;
  maxModules=0;
  modules=0;
}

HDetector::~HDetector(void) {
  //Destructor
  if (modules) delete modules;
}
  
void HDetector::setModules(Int_t s,Int_t* m){
  // stores the modules givven in 'm' as active modules in sector 's'
  // may be called with s=-1 for detectors not belonging to a sector
  if (!modules) return;
  if (s<0) s=0;
  for(Int_t i=0;i<maxModules;i++) {
    modules->AddAt(m[i],(s*maxModules+i));
  }
}

Int_t* HDetector::getModules(){
  // returns a linear array of all modules
  if (modules) return modules->GetArray();
  return 0;
}

Int_t HDetector::getModule(Int_t s,Int_t m){
  // returns the number of the module in a sector
  // may be called with s=-1 for detectors not belonging to a sector
  // returns 0 if this module is not active
  if (m>=maxModules) return 0;
  if (s<0) return (*modules)[m];
  if (modules && s<maxSectors) return (*modules)[(s*maxModules+m)];
  return 0;
}

Bool_t HDetector::isSectorActive(Int_t sector) {
  // Returns true if any module in the sector "sector" is set active.
  Int_t begin = sector * maxModules;
  Int_t end = begin + maxModules;
  for (Int_t i=begin; i<end; i++) 
    if ( modules->At(i) > 0 ) return kTRUE;
  return kFALSE;
}

void HDetector::print() {
  // prints the detector setup
  if (!modules) return;
  cout<<fName<<'\n';
  Int_t s=(maxSectors<0) ? 1 : maxSectors;
  for(Int_t i=0;i<s;i++) {
    cout<<"      ";
    for(Int_t j=0;j<maxModules;j++) {
      Int_t k=i*maxModules+j;
      cout<<(*modules)[k]<<" ";
    }
    cout<<'\n';
  }
}

ClassImp(HDetector)
