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
#include "TObjString.h"

HTask* HEmcTaskSet::make(const Char_t* select, Option_t* option) {
  // Returns a pointer to the task or taskset specified by 'select'
  HTaskSet *tasks = new HTaskSet("Emc","List of Emc tasks");
  TString simulation="simulation"; 
  TString real ="real";            

  TString sel = select;
  TString opt = option;
  sel.ToLower();
  opt.ToLower();

  parseArguments(opt);
  
  if(sel.CompareTo(simulation)==0){
    if (doEmcCal)
      tasks->add(new HEmcDigitizer("emc.digi","emc.digi"));
    if (doEmcClusF)
      tasks->add(new HEmcClusterF("emc.clusf","emc.clusf"));
  }
  
  if(sel.CompareTo(real)==0){  	 	
    if (doEmcCal)
      tasks->add(new HEmcCalibrater("emc.cal", "Emc calibrater"));
    if (doEmcClusF)
      tasks->add(new HEmcClusterF("emc.clusf","emc.clusf"));
  }

  return tasks;
}

void HEmcTaskSet::parseArguments(TString s)
{
    // parses arguments (straw, rpc) to the make tasks function

    s.ToLower();
    s.ReplaceAll(" ", "");

    Ssiz_t len = s.Length();

    if (len!=0)
    {
        doEmcCal = kFALSE;
        doEmcClusF = kFALSE;

        TObjArray* myarguments = s.Tokenize(",");
        TObjString *stemp = 0;
        TString argument;
        TIterator* myiter = myarguments->MakeIterator();
        // go over list of arguments and compare the known keys

        while ((stemp=(TObjString*)myiter->Next()) != 0)
        {
            argument=stemp->GetString();
            Info("parseArguments()", "option: %s", argument.Data());

            if (argument.CompareTo("cal") == 0)
            {
                doEmcCal   = kTRUE;
            }
            else if (argument.CompareTo("clusf") == 0)
            {
                doEmcCal   = kTRUE;
                doEmcClusF = kTRUE;
            }
            else
            {
                Error("parseArguments()", "Unknown option = %s", argument.Data());
            }
        }
        delete myiter;

        myarguments->Delete();
        delete myarguments;
    }
}

ClassImp(HEmcTaskSet)
