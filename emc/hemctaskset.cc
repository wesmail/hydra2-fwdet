//*-- Author  : K. Lapidus
//*-- Created :

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HEmcTaskSet
//
// This HTaskSet contains the tasks for the Emc
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hemctaskset.h"
#include "hemccalibrater.h"
#include "hemcdigitizer.h"
#include "hemcclusterf.h"
#include "hdebug.h"
#include "haddef.h"
#include <iostream> 
#include <iomanip>

HTask* HEmcTaskSet::make(const Char_t* select, Option_t* option) {
  // Returns a pointer to the task or taskset specified by 'select'
  HTaskSet *tasks = new HTaskSet("Emc","List of Emc tasks");
  TString simulation="simulation"; 
  TString real ="real";            

  TString sel = select;
  TString opt = option;
  sel.ToLower();
  opt.ToLower();
  
  if(sel.CompareTo(simulation)==0){
    tasks->add(new HEmcDigitizer("emc.digi","emc.digi"));
    tasks->add(new HEmcClusterF("emc.clusf","emc.clusf"));
  }
  
  if(sel.CompareTo(real)==0){  	 	
    tasks->add(new HEmcCalibrater("emc.cal", "Emc calibrater"));
    tasks->add(new HEmcClusterF("emc.clusf","emc.clusf"));
  }

  return tasks;
}

ClassImp(HEmcTaskSet)
