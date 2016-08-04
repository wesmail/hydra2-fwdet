//*-- AUTHOR : Ilse Koenig, Tomasz Wojcik, Wolfgang Koenig
//*-- Created : 19/01/2005

//Modified 24/08/2005 Filip Krizek
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HWallTaskSet
//
// This HTaskSet contains the tasks for the Forward Wall
// The tasklist can be be configured via make(const Char_t* select,const Option_t* option)
//
///////////////////////////////////////////////////////////////////////////////

#include "hwalltaskset.h"
#include "hwalldigitizer.h"
#include "hwallcalibrater.h"
#include "hwallonehitf.h"
#include "hwallhitf.h"
#include "hwallhitfsim.h"
#include "hwalleventplanef.h"


#include "TObjString.h"

using namespace std;


HWallTaskSet::HWallTaskSet(const Text_t name[],const Text_t title[])
: HTaskSet(name,title)
{
    fdoEventPlane = kTRUE;
}
HWallTaskSet::~HWallTaskSet(void)
{

}

HTask* HWallTaskSet::make(const Char_t* select,const Option_t* option) {
    // Returns a pointer to the task or taskset specified by 'select'
    // select :   real or simulation
    // option :   noeventplane = do not run HWallEventPlaneF (runs by default)

    HTaskSet *tasks    = new HTaskSet("Wall","List of Forward Wall tasks");

    TString sel = select;
    TString opt = option;
    sel.ToLower();
    opt.ToLower();
    parseArguments(opt);

    if(sel.CompareTo("") == 0){
	Error("make()","You have to specify \"real\" or \"simulation\" in \"select\"! (has been moved from \"option\")");
	exit(1);
    }


    if(sel.CompareTo("simulation") == 0){
	tasks->add( new HWallDigitizer("wall.digi","wall.digi"));
	tasks->add( new HWallHitFSim("wall.hitfs","wall.hitfs"));
    }

    if(sel.CompareTo("real") == 0){
	tasks->add( new HWallCalibrater("wall.cal","wall.cal") );
	tasks->add( new HWallOneHitF("wall.onehitf","wall.onehitf"));
	tasks->add( new HWallHitF("wall.hitf","wall.hitf"));
    }

    if(fdoEventPlane) tasks->add(new HWallEventPlaneF("wall.eventplane","wall.eventplane")) ;

    return tasks;
}
void HWallTaskSet::parseArguments(TString s1)
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
	    if (0 == argument.CompareTo("noeventplane")) {
		fdoEventPlane = kFALSE;
	    } else {
               Warning("parseArguments()","Unknown option \"%s\" will be ignored!",argument.Data());
	    }
	}
	delete myiter;
	myiter = NULL;

	myarguments ->Delete();
	delete myarguments;

    }
}

ClassImp(HWallTaskSet)










