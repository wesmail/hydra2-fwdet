//*-- Author : A.Rustamov

// Task set to All momentum Reconstruction methods
// The arguments:
// metamatching is connected by default!!!!
// all other options are off by default!
// spline -connect spline  task
// runge -connect runge kutta task
// docluser - connect Tof clustering
// rejectunmatched - do not store unmatched tracks

#include "hsplinetaskset.h"
#include "hmessagemgr.h"
#include "hsplinetrackF2.h"
#include "hmetamatchF2.h"
#include "hsplinetofclF2.h"
#include "hrktrackBF2.h"

#include <cstdlib>
using namespace std;

ClassImp(HSplineTaskSet)

Bool_t HSplineTaskSet::isSpline     = kFALSE;
Bool_t HSplineTaskSet::isClustering = kFALSE;
Bool_t HSplineTaskSet::isRunge      = kFALSE;
Bool_t HSplineTaskSet::isRejectUnMatched = kFALSE;

HSplineTaskSet::HSplineTaskSet() {
    isSplinePar = kFALSE;
}

HSplineTaskSet::HSplineTaskSet(const Text_t name[],const Text_t title[]):HTaskSet(name,title){
    isSplinePar = kFALSE;
}

HSplineTaskSet::~HSplineTaskSet()
{

}

void HSplineTaskSet::setSplinePar()
{
    isSplinePar = kTRUE;
}

HTask* HSplineTaskSet::make(const Char_t* select,const Option_t *options)
{
    HTaskSet *tasks = new HTaskSet("Spline","Spline Task Sets");
    TString opt = options;
    parseArguments(opt);
    HMetaMatchF2* metaF = new HMetaMatchF2("MetaMAtching","MetaMAtching");
    tasks->add(metaF);
    if(isRejectUnMatched) metaF->storeNotMatchedTracks(kFALSE);
    else                  metaF->storeNotMatchedTracks(kTRUE);
    if(isSpline)
    {
	HSplineTrackF2* pSplineTrackF2 = new HSplineTrackF2("SplineTrack","SplineTrack");
	if(isSplinePar) pSplineTrackF2->makeSplinePar();
	tasks->add(pSplineTrackF2);
    }


    if(isRunge)
    {
	tasks->add(new HRKTrackBF2("RK-tracking2", 2));
    }


    if(isClustering)
    {
	tasks->add(new HSplineTofClF2("tofClustering","tofClustering",1));
    }
    else
    {
	//tasks->add(new HSplineTofClF2("tofClustering","tofClustering",0));
    }

    return tasks;
}

void HSplineTaskSet::parseArguments(TString a)
{
    a.ReplaceAll(" ","");
    if(a.CompareTo("") == 0) return;
    a.ToLower();
    TObjArray* myarguments = a.Tokenize(",");
    TObjString *stemp;
    TString argument;
    TIterator* myiter = myarguments->MakeIterator();
    while ((stemp = (TObjString*)myiter->Next()) != 0) {
	argument = stemp->GetString();
	Info("parseArguments()","option: %s", argument.Data() );
	if(argument.CompareTo     ("spline")       == 0){ isSpline     = kTRUE; }
	else if(argument.CompareTo("runge")        == 0){ isRunge      = kTRUE; }
	else if(argument.CompareTo("doclustering") == 0){ isClustering = kTRUE; }
	else if(argument.CompareTo("rejectunmatched") == 0){ isRejectUnMatched = kTRUE; }
	else {
	    Error("parseArguments()","unknown option = %s detected! fix your macro!",argument.Data());
            exit(1);
	}
    }
    delete myiter;
    myarguments->Delete();
    delete myarguments;

}


