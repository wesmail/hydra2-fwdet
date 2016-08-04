//*-- Author  : Georgy Kornakov
//*-- Created : 27.01.2016

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetTaskSet
//
//  This class contains the tasks for the Forward Detector
//
/////////////////////////////////////////////////////////////

using namespace std;

#include "hfwdettaskset.h"
#include "hdebug.h"
#include "haddef.h"
#include "hades.h"
#include "hfwdetstrawdigitizer.h"
#include "hfwdetscindigitizer.h"
#include "hfwdetrpcdigitizer.h"
#include <iostream> 
#include <iomanip>
#include "TObjString.h"

HFwDetTaskSet::HFwDetTaskSet(void) : HTaskSet() {
    // Default constructor
    doStraw = kFALSE;
    doScin  = kFALSE;
    doRpc   = kFALSE;
}

HFwDetTaskSet::HFwDetTaskSet(const Text_t name[],const Text_t title[]) : HTaskSet(name,title) {
    // Constructor
    doStraw = kFALSE;
    doScin  = kFALSE;
    doRpc   = kFALSE;
}

void HFwDetTaskSet::parseArguments(TString s) {
    // parses arguments (straw, scin, rpc) to the make tasks function

    s.ToLower();
    s.ReplaceAll(" ","");

    Ssiz_t len=s.Length();

    if(len!=0) {

	TObjArray* myarguments = s.Tokenize(",");
	TObjString *stemp;
	TString argument;
	TIterator* myiter=myarguments->MakeIterator();
	// go over list of arguments and compare the known keys

	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();

	    Info("parseArguments()","option: %s", argument.Data() );

	    if (argument.CompareTo("straw")   == 0) {
                doStraw  = kTRUE;
            }
            else if( argument.CompareTo("scin")    == 0) {
                doScin   = kTRUE;
            }
            else if (argument.CompareTo("rpc")     == 0) {
                doRpc    = kTRUE;
            }
            else {
		Error("parseArguments()","Unknown option = %s",argument.Data());
	    }
	}
	delete myiter;

	myarguments->Delete();
	delete myarguments ;
    }
}

HTask* HFwDetTaskSet::make(const Char_t *select,const Option_t *option) {
    // Returns a pointer to the FwDet task or taskset specified by 'select'
    // OPTIONS: see parseArguments()

    HTaskSet *tasks = new HTaskSet("FwDet","List of FwDet tasks");
    TString simulation="simulation";
    TString real ="real";

    TString sel = select;
    TString opt = option;

    sel.ToLower();
    opt.ToLower();
    parseArguments(opt);

    if(sel.CompareTo(simulation)==0 || gHades->getEmbeddingMode()>0)
    {
	if(doStraw)
	    tasks->add( new HFwDetStrawDigitizer("fwdetstraw.digi","fwdetstraw.digi") );
	if(doScin)
	    tasks->add( new HFwDetScinDigitizer("fwdetscin.digi","fwdetscin.digi") );
	if(doRpc)
	    tasks->add( new HFwDetRpcDigitizer("fwdetrpc.digi","fwdetrpc.digi") );
    }
    if(sel.CompareTo(real)==0 || gHades->getEmbeddingMode()>0)
    {
    }
    return tasks;
}

ClassImp(HFwDetTaskSet)
