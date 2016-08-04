//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//  HPionTrackerTaskSet
//
//  This HTaskSet contains the tasks for the PionTracker detector
//
//  Options:
//  - nopiontrackerhit: runs the task list only up to cal level (default: kTRUE)
//  - skipevents:       options for HPionTrackerHitF task (default: kFALSE)
//                      controls skipping of all events without successful
//                      time reconstruction.
//
///////////////////////////////////////////////////////////////////////////////

#include "hpiontrackertaskset.h"
#include "hpiontrackercalibrater.h"
#include "hpiontrackerhitf.h"
#include "hpiontrackertrackf.h"
#include "TObjString.h"
#include "TObjArray.h"

using namespace std;

ClassImp(HPionTrackerTaskSet)

HPionTrackerTaskSet::HPionTrackerTaskSet(void) : HTaskSet() {
  // default constructor sets the default options
  fSkipEvent = kFALSE;
  fUsePionTrackerHit = kTRUE;
  fUsePionTrackerTrack = kTRUE;
}

HPionTrackerTaskSet::HPionTrackerTaskSet(const Text_t name[], const Text_t title[])
                    : HTaskSet(name, title) {
  // constructor sets the default options
  fSkipEvent = kFALSE;
  fUsePionTrackerHit = kTRUE;
  fUsePionTrackerTrack = kTRUE;
}

HTask* HPionTrackerTaskSet::make(const Char_t *select, const Option_t *option) {
  // creates the task list
  // the parameter "select" accepts NULL or "real"
  // possible options: nopiontrackerhit, skipevents (see class docu)
  // Returns a pointer to the PionTracker task or taskset specified by 'select'
  HTaskSet *tasks = new HTaskSet("PionTracker", "List of PionTracker tasks");
  TString sel = select;
  TString opt = option;
  sel.ToLower();
  opt.ToLower();
  parseArguments(opt);

  if (sel.IsNull() || sel.CompareTo("real")==0) {
    tasks->add(new HPionTrackerCalibrater("piontracker.cal", "PionTracker cal"));
    if (kTRUE == fUsePionTrackerHit) {
      tasks->add(new HPionTrackerHitF("piontracker.hitF", "PionTracker hit finder", fSkipEvent));
    }
    if (kTRUE == fUsePionTrackerTrack) {
      tasks->add(new HPionTrackerTrackF("piontracker.trackF", "PionTracker track finder", fSkipEvent));
    }
  }
  return tasks;
}

void HPionTrackerTaskSet::parseArguments(TString s1) {
  // parses the list of options
  s1.ToLower();
  s1.ReplaceAll(" ", "");

  if (0 != s1.Length()) {
    TIterator*  myiter      = NULL;
    TObjString* stemp       = NULL;
    TString     argument;
    TObjArray*  myarguments = s1 .Tokenize(",");

    // iterate over the list of arguments and compare it
    // to the known key words.
    myiter = myarguments->MakeIterator();
    myiter->Reset();
    while (0 != (stemp = static_cast<TObjString*>(myiter->Next()))) {
      argument = stemp->GetString();
      if (0 == argument.CompareTo("skipevents")) {
        fSkipEvent = kTRUE;
      }
      if (0 == argument.CompareTo("nopiontrackerhit")) {
        fUsePionTrackerHit = kFALSE;
      }
      if (0 == argument.CompareTo("nopiontrackertrack")) {
        fUsePionTrackerTrack = kFALSE;
      }
    }

    delete myiter;
    myiter = NULL;
    myarguments->Delete();
    delete myarguments;
  }
}
