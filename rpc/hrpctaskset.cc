//*-- Author  : D. Gonzalez-Diaz
//*-- Created : 14/06/2006
//*-- Modified: 24/08/2007 by P.Cabanelas
//*-- Modified: 04/12/2013 by G. Kornakov Added RPC embedding mode

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// HRpcTaskSet
//
// This HRpcSet contains the tasks for the Rpc detector
//
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hrpctaskset.h"
#include "hdebug.h"
#include "haddef.h"
#include "hades.h"
#include "hrpcdigitizer.h"
#include "hrpccalibrater.h"
#include "hrpchitf.h"
#include "hrpcclusterf.h"
#include <iostream> 
#include <iomanip>
#include "TObjString.h"

HRpcTaskSet::HRpcTaskSet(const Text_t name[],const Text_t title[]) : HTaskSet(name,title) {
    // Constructor
}


HRpcTaskSet::HRpcTaskSet(void) : HTaskSet() {
    // Default constructor
}


HRpcTaskSet::~HRpcTaskSet(void) {
    // Destructor.
}

void HRpcTaskSet::parseArguments(TString s, Int_t &doCluster) {
    // parses arguments to the make tasks function
    //
    // OPTIONS:
    // noclust    : (default kFALSE) switch clusterfinder to fill only size=1 objects (copy hits)

    doCluster = 1;  // default

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

	    if( argument.CompareTo("noclust")   == 0)  doCluster = kFALSE;
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

HTask* HRpcTaskSet::make(const Char_t *select,const Option_t *option) {
    // Returns a pointer to the Rpc task or taskset specified by 'select'
    // OPTIONS: see parseArguments()

    HTaskSet *tasks = new HTaskSet("Rpc","List of Rpc tasks");
    TString simulation="simulation";
    TString real ="real";

    TString sel = select;
    TString opt = option;

    sel.ToLower();
    opt.ToLower();
    Int_t doCluster = 1;

    parseArguments(opt,doCluster);

    if(sel.CompareTo(simulation)==0 || gHades->getEmbeddingMode()>0)
    {
	tasks->add( new HRpcDigitizer("rpc.digi","rpc.digi") );
    }
    if(sel.CompareTo(real)==0 || gHades->getEmbeddingMode()>0)
    {
	tasks->add( new HRpcCalibrater("rpc.cal","rpc.cal") );
    }

    // --------------------------------------------------
    // Commom tasks for real, sim and embedding
    tasks->add( new HRpcHitF("rpc.hitf","rpc.hitf") );
    HRpcClusterF* clusterf = new HRpcClusterF("rpc.clusterf","rpc.clusterf") ;
    if(doCluster == 0) {
        clusterf ->setDoSingleClusters(kTRUE);
    }
    tasks->add( clusterf );

    return tasks;
}

ClassImp(HRpcTaskSet)










