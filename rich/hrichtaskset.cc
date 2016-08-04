//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Dan Magestro
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichTaskSet
//
//  This HTaskSet contains the task sets for the Rich detector.
//  It can be distinguished between "real" and "simulation" task set by
//  passing the corresponding string to the first argument of the make()
//  function. Embedding is automatically recognized by return value of
//  gHades->getEmbeddingMode(). For embedding ONLY the "real" task set
//  has to be selected, otherwise the program will finish with an error.
//
//  To control the operation of the RICH tasks, HRichTaskSet has following
//  switches with preset default values:
//    - inoiseOn   = kTRUE;
//    - iSkipEvent = kFALSE;
//
//  The simulation of noise in HRichDigitizer behaves in pure
//  simulation differently as compared to the embedding modus.
//     * pure simulation: simulation of both correlated and random noise is ON
//     * embedding:       only simulation of correlated noise is ON
//
//  Default values can be changed by passing corresponding strings
//  to the second argument of make() function. For more details,
//  please read the documentation for this function below.
//
//  BEGIN_HTML For more information, see
//  <a target="_top" href="http://webdb.gsi.de/pls/hades_webdbs/hanal.hal_posting_query.show_posting_text?p_id=436">
//  this analysis logbook entry<a>. END_HTML
//
//////////////////////////////////////////////////////////////////////////////


#include "TObjString.h"

#include "hades.h"
#include "hrichanalysis.h"
#include "hrichanalysissim.h"
#include "hrichdigitizer.h"
#include "hrichtaskset.h"

using namespace std;

ClassImp(HRichTaskSet)


HRichTaskSet::HRichTaskSet(void)
   : HTaskSet()
{
   // Default constructor
   fNoiseOn   = kTRUE;
   fSkipEvent = kFALSE;

}

HRichTaskSet::HRichTaskSet(const Text_t name[], const Text_t title[])
   : HTaskSet(name, title)
{
   // Constructor
   fNoiseOn   = kTRUE;
   fSkipEvent = kFALSE;
}

HRichTaskSet::~HRichTaskSet(void)
{
   // Destructor.
}


HTask*
HRichTaskSet::make(const Char_t *select, const Option_t *option)
{
// Returns a pointer to the Rich task or taskset specified by 'select'.
// There are two possibilities to be passed:
//   - "real" for real data analysis and embedding
//   - "simulation" for simulation only
//
// Following options can be passed to the HRichTaskSet to control the
// behavior of the RICH analysis and simulation:
//   - "skipevent" HRichAnalysis task will return kSkipEvent for each
//                 event w/o found ring
//   - "noiseoff"  Turns OFF noise simulation in HRichDigitizer.
// For more information see BEGIN_HTML
// <a target="_top" href="http://forum.gsi.de/index.php?t=rview&th=2193&rid=334">
// this forum entry<a>. END_HTML

   HTaskSet* tasks      = new HTaskSet("Rich", "List of Rich tasks");
   TString   sel        = select;
   TString   opt        = option;

   sel.ToLower();
   if (0 == sel.CompareTo("simulation") &&
       0 != gHades->getEmbeddingMode()) {
      Error("make", "embeddingMode > 0 in simulation NOT ALLOWED!");
      return NULL;
   }


   parseArguments(opt);
   if (0 == sel.CompareTo("simulation") &&
       0 == gHades->getEmbeddingMode()) {
      tasks->add(new HRichDigitizer("rich.digi",  "Rich digitizer", fNoiseOn));
      tasks->add(new HRichAnalysisSim("rich.ana", "Ring finder",    fSkipEvent));
   }

   if (0 == sel.CompareTo("real") &&
       0 == gHades->getEmbeddingMode()) {
      tasks->add(new HRichAnalysis("rich.ana", "Ring finder", fSkipEvent));
   }

   if (0 == sel.CompareTo("real") &&
       0 != gHades->getEmbeddingMode()) {
      tasks->add(new HRichDigitizer("rich.digi",  "Rich digitizer", fNoiseOn));
      tasks->add(new HRichAnalysisSim("rich.ana", "Ring finder",    fSkipEvent));
   }
   return tasks;
}

void
HRichTaskSet::parseArguments(TString s1)
{

   s1.ToLower();
   s1.ReplaceAll(" ", "");


   if (0 != s1.Length()) {

      // iterate over the list of arguments and compare it
       // to the known key words.

       TObjArray* myarguments = s1.Tokenize(",");
       TIterator*   myiter     = NULL;
       TObjString*  stemp      = NULL;
       TString      argument;

       myiter = myarguments->MakeIterator();
       myiter->Reset();
       while (0 != (stemp = static_cast<TObjString*>(myiter->Next()))) {
	   argument = stemp->GetString();
	   Info("parseArguments()","option: %s", argument.Data() );
	   if (0 == argument.CompareTo("skipevent")) {
	       fSkipEvent = kTRUE;
	   } else if (0 == argument.CompareTo("noiseoff")) {
	       fNoiseOn  = kFALSE;
	   }
       }
       delete myiter;
       myiter = NULL;

       myarguments->Delete();
       delete myarguments;

   }
}
