using namespace std;
#include "htoftaskset.h"
#include "hdebug.h"
#include "haddef.h"
#include "hades.h"
#include "htofdigitizer.h"
#include "htofhitf.h"
#include "htofhitf2.h"
#include "htofhitfsim.h"
#include "htofclusterf.h"
#include "htofclusterfsim.h"

#include "TObjArray.h"
#include "TIterator.h"
#include "TObjString.h"

using namespace std;
#include <iostream>
#include <iomanip>

//*-- Author : Dan Magestro
//*-- Modified: 04/12/2001  by R. Holzmann
//*-- Modified: 19/11/2001  by D. Magestro

//*-- Modified: 30/06/2004  by J. Otwinowski
// TofCluster category is filled by default

//*-- Modified: 06/07/2004  by J. Otwinowski
//	Come back to the optional TofCluster (better for cosmics) 

//*-- Modified: 07/01/2005  by P. Tlusty
//	TofClusterFSim called for sim data instead of TofClusterF 

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HTofTaskSet
//
// This HTaskSet contains the tasks for the Tof detector
//
// BEGIN_HTML<b><font color="maroon">For more information, see <a target="_top" href="http://webdb.gsi.de:8010/pls/hades_webdbs/hanal.hal_posting_query.show_posting_text?p_id=436">this analysis logbook entry<a>.</font></b> END_HTML
//
///////////////////////////////////////////////////////////////////////////////

HTofTaskSet::HTofTaskSet(const Text_t name[],const Text_t title[]) : HTaskSet(name,title) {
    // Constructor
    fhitfinderVersion = 2;
    fdoCluster         = kTRUE;
}


HTofTaskSet::HTofTaskSet(void) : HTaskSet() {
    // Default constructor
    fhitfinderVersion = 2;
    fdoCluster         = kTRUE;
}


HTofTaskSet::~HTofTaskSet(void) {
  // Destructor.
}

void HTofTaskSet::parseArguments(TString s1) {
    // parses arguments to the make tasks function
    //
    // OPTIONS:
    // hitfinder1 : switch hitfinder to old version 1. default is 2;
    // noclust    : siwtch off clusterfinder. default is run clusterfinder


    TString s2 = s1;

    s1.ToLower();
    s1.ReplaceAll(" ","");

    Ssiz_t len=s1.Length();

    if(len!=0) {

	TObjArray* myarguments = s1.Tokenize(",");
	TObjString *stemp;
	TString argument;

	TIterator* myiter=myarguments->MakeIterator();
	// go over list of arguments and compare the known keys

	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();

	    Info("parseArguments()","option: %s", argument.Data() );

	    if( argument.CompareTo("hitfinder1") == 0) fhitfinderVersion = 1;
	    if( argument.CompareTo("noclust")   == 0)  fdoCluster        = kFALSE;
	    else
	    {
		Error("parseArguments()","Unknown option = %s",argument.Data());
	    }
	}
	delete myiter;

	myarguments->Delete();
	delete myarguments ;
    }
}


HTask* HTofTaskSet::make(const Char_t *select,const Option_t *option) {
  // Returns a pointer to the Tof task or taskset specified by 'select'

  HTaskSet *tasks = new HTaskSet("Tof","List of Tof tasks");

  TString sel = select;
  TString opt = option;
  sel.ToLower();
  opt.ToLower();
  parseArguments(opt);

  if(sel.CompareTo("simulation")==0&&gHades->getEmbeddingMode()!=0)
  {
      Error("HTofTaskSet::make()","embeddingMode > 0 in simulation NOT ALLOWED!");
      exit(1);
  }

  if( sel.CompareTo("simulation")==0 ||
     (sel.CompareTo("real")      ==0 && gHades->getEmbeddingMode()!=0))
  {
      tasks->add( new HTofDigitizer("tof.digi","tof.digi") );
      if(fhitfinderVersion ==1) tasks->add( new HTofHitFSim("tof.hitfs","tof.hitfs") );
      if(fhitfinderVersion ==2) tasks->add( new HTofHitF2("tof.hitfs","tof.hitfs") );

      if(fdoCluster) {
	  tasks->add( new HTofClusterFSim("tof.clusf","tof.clusf") );
      }
  }
  if(sel.CompareTo("real")==0&&gHades->getEmbeddingMode()==0)
  {
      tasks->add( new HTofHitF("tof.hitf","tof.hitf") );
      if(fhitfinderVersion ==1) tasks->add( new HTofHitF("tof.hitfs","tof.hitfs") );
      if(fhitfinderVersion ==2) tasks->add( new HTofHitF2("tof.hitfs","tof.hitfs") );

      if(fdoCluster) {
	  tasks->add( new HTofClusterF("tof.clusf","tof.clusf") );
      }
  }
  return tasks;
}

ClassImp(HTofTaskSet)










