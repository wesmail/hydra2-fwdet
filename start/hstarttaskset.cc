//*-- Author : Dan Magestro
//*-- Modified: 19/11/2001  by D. Magestro

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HStartTaskSet
//
// This HTaskSet contains the tasks for the Start detector
//
// Active options for HStart2HitF task:
//    - skipevents: controls skipping of all events without successful
//                  start time reconstruction.
//
// BEGIN_HTML<b><font color="maroon">For more information, see <a target="_top" href="http://webdb.gsi.de/pls/hades_webdbs/hanal.hal_posting_query.show_posting_text?p_id=436">this analysis logbook entry<a>.</font></b> END_HTML
//
///////////////////////////////////////////////////////////////////////////////

#include "TObjString.h"

#include "hstart2calibrater.h"
#include "hstart2hitf.h"
#include "hstart2hitfsim.h"
#include "hstarttaskset.h"
#include "htaskset.h"
#include "hades.h"

using namespace std;

ClassImp(HStartTaskSet)

HTask* HStartTaskSet::make(const Char_t *select, const Option_t *option)
{
   // Returns a pointer to the Start task or taskset specified by 'select'
   // select :  real or simulation
   //
   // Active options for HStart2HitF task:
   //    - skipevents: controls skipping of all events without successful
   //                  start time reconstruction.
   //    - nostarthit: do not use hitfinder

   HTaskSet *tasks = new HTaskSet("Start", "List of Start tasks");

   TString sel = select;
   TString opt = option;
   opt.ToLower();
   fUseStartHit = kTRUE;
   parseArguments(opt);
   sel.ToLower();

   if (sel.CompareTo("real") == 0 || gHades->getEmbeddingMode() > 0) {
       tasks->add(new HStart2Calibrater("start.cal", "Start cal"));
      if (kTRUE == fUseStartHit) {
         tasks->add(new HStart2HitF("start.hitF", "Start hit finder", fSkipEvent));
      }
   }
   if (sel.CompareTo("simulation") == 0 || gHades->getEmbeddingMode() > 0) {
       tasks->add(new HStart2HitFSim("start.hitF", "Start hit finder"));
   }
   return tasks;
}

void HStartTaskSet::parseArguments(TString s1)
{

   s1.ToLower();
   s1.ReplaceAll(" ", "");

   if (0 != s1.Length()) {
      TIterator*   myiter     = NULL;
      TObjString*  stemp      = NULL;
      TString      argument;
      TObjArray* myarguments = s1 .Tokenize(",");

      // iterate over the list of arguments and compare it
      // to the known key words.
      myiter = myarguments->MakeIterator();
      myiter->Reset();
      while (0 != (stemp = static_cast<TObjString*>(myiter->Next()))) {
         argument = stemp->GetString();
	 Info("parseArguments()","option: %s", argument.Data() );
         if (0 == argument.CompareTo("skipevents")) {
            fSkipEvent = kTRUE;
         }
         if (0 == argument.CompareTo("nostarthit")) {
            fUseStartHit = kFALSE;
	 }
      }
      delete myiter;
      myiter = NULL;

      myarguments ->Delete();
      delete myarguments;

   }
}
