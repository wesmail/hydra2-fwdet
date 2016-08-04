using namespace std;
#include "hshowertaskset.h"
#include "htask.h"
#include "htaskset.h"
#include "hdebug.h"
#include "TROOT.h"
#include "TClass.h"
#include "haddef.h"
#include "hades.h"
#include "hshowercalibrater.h"
#include "hshowerhitfinder.h"
#include "hshowercopy.h"
#include "hshowerhitdigitizer.h"
#include "hshowerpaddigitizer.h"
#include "hshowerhittrackmatcher.h"
#include "TBrowser.h"
#include "TString.h"
#include "TObjString.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

//*-- Author : Dan Magestro
//*-- Modified: 05/04/2003  by J.Otwinowski
//	    simulation flag set for hshowertofinocorrealtor
//*-- Modified: 19/11/2001  by D. Magestro
//*-- Modified: 14/02/2002  by J.Otwinowski
//*-- Modified: 02/06/2006  by J.Otwinowski
//   	 lowshowerefficiency flag added
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HShowerTaskSet
//
// This HTaskSet contains the tasks for the Shower - Tofino detector
//
// BEGIN_HTML<b><font color="maroon">For more information, see <a target="_top" href="http://webdb.gsi.de:8010/pls/hades_webdbs/hanal.hal_posting_query.show_posting_text?p_id=436">this analysis logbook entry<a>.</font></b> END_HTML
//
///////////////////////////////////////////////////////////////////////////////

Int_t HShowerTaskSet::ianalysisSimOrReal=0;
Int_t HShowerTaskSet::ianalysisLevel=0;
Int_t HShowerTaskSet::imagnet=0;
Int_t HShowerTaskSet::analysisSimOrReal=0;
Int_t HShowerTaskSet::analysisLevel=3;
Bool_t HShowerTaskSet::isCoilOff=kFALSE;
Bool_t HShowerTaskSet::isSimulation=kFALSE;
Bool_t HShowerTaskSet::isLowShowerEfficiency=kTRUE;

HShowerTaskSet::HShowerTaskSet(const Text_t name[],const Text_t title[]) : HTaskSet(name,title) {
    // Constructor
}


HShowerTaskSet::HShowerTaskSet(void) : HTaskSet() {
    // Default constructor
}


HShowerTaskSet::~HShowerTaskSet(void) {
    // Destructor.
}


HTask* HShowerTaskSet::make(const Char_t *select,const Option_t *option) {
    // Returns a pointer to the Shower task or taskset specified by 'select'


    HTaskSet *tasks = new HTaskSet("Shower","List of Shower tasks");

    TString sel = select;
    sel.ToLower() ;
    passArguments(sel);

    TString opt = option;

    passArguments(opt);

    if(isSimulation&&gHades->getEmbeddingMode()>0)
    {
	Error("HShowerTaskSet::make()","Embedding is not implemented in Simulation! Use Real for embedding!");
	exit(1);
    }


    //if( isLowShowerEfficiency == kFALSE) pShTofCorr->setLowShowerEfficiencyFlag(kFALSE);
    //else pShTofCorr->setLowShowerEfficiencyFlag(kTRUE);


    if(!isSimulation)
    {
	// real or embedding
	if(analysisLevel >= 1 )
	{
	    tasks->add( new HShowerCalibrater("sho.cal","sho.cal") );

	    if(gHades->getEmbeddingMode()>0)
	    {   // embedding
		tasks->add( new HShowerHitDigitizer("shohit.digi","shohit.digi") );
		tasks->add( new HShowerPadDigitizer("shopad.digi","shopad.digi") );
		tasks->add( new HShowerCopy("sho.copy","sho.copy"));
	    }
	}
	if(analysisLevel >= 2)
	{
	    tasks->add( new HShowerHitFinder("sho.hitf","sho.hitf") );
	    if(gHades->getEmbeddingMode()>0)
	    {   // embedding
		tasks->add( new HShowerHitTrackMatcher("sho.htmatch","sho.htmatch") );
	    }
	}
    }
    else
    {
	// simulation
	if(analysisLevel >=1 )
	{
	    tasks->add( new HShowerHitDigitizer("shohit.digi","shohit.digi") );
	    tasks->add( new HShowerPadDigitizer("shopad.digi","shopad.digi") );
	    tasks->add( new HShowerCopy("sho.copy","sho.copy"));
	}
	if(analysisLevel >=2)
	{
	    tasks->add( new HShowerHitFinder("sho.hitf","sho.hitf") );
	    tasks->add( new HShowerHitTrackMatcher("sho.htmatch","sho.htmatch") );
	}
    }
    return tasks;
}

void HShowerTaskSet::setAnalysisSimOrReal(Option_t* optrealorsim="") {
    TString optRealOrSim=optrealorsim;

    optRealOrSim.ToLower();
    if(optRealOrSim.CompareTo("real")==0)
    {
	isSimulation=kFALSE;
    }
    else if(optRealOrSim.CompareTo("simulation")==0)
    {
	isSimulation=kTRUE;
    }
    else
    {
	cerr<<"HShowerTaskSet:setRealOrSim() unknown argument :use real or simulation" << endl;
	exit(1);
    }
}

void HShowerTaskSet::setAnalysisLevel(Option_t* optanalysislevel="") {
    // Sets maximum analysis levels leprecognition
    TString optanalLavel = optanalysislevel;

    optanalLavel.ToLower();
    if(optanalLavel.CompareTo("leprecognition")==0)
    {
	analysisLevel=1;
    }
    else
    {
	cerr<<"HShowerTaskSet::setAnalysisLevel(Option_t*): unknown argument use leprecognition" << endl;
	exit(1);
    }
}

void HShowerTaskSet::passArguments(TString s1) {
    // pass arguments to the make tasks function

    TString ssimulation = "simulation" ;
    TString sreal = "real" ;
    TString smagneton = "magneton" ;
    TString smagnetoff = "magnetoff" ;
    TString sraw = "raw" ;
    TString scal = "cal" ;
    TString shit = "hit" ;
    TString slowshowerefficiency = "nolowshowerefficiency" ;

    TString s2 = s1;

    s1.ToLower();
    s1.ReplaceAll(" ","");

    Ssiz_t len=s1.Length();

    if(len!=0) {

	TObjArray* myarguments = s1.Tokenize(",");
	TObjString *stemp;
	TString argument;

	ianalysisSimOrReal=0;
	ianalysisLevel=0;
	imagnet=0;

	TIterator* myiter=myarguments->MakeIterator();
	// go over list of arguments and compare the known keys

	while ((stemp=(TObjString*)myiter->Next())!= 0)
	{
	    argument=stemp->GetString();

	    Info("parseArguments()","option: %s", argument.Data() );

	    if( argument.CompareTo(slowshowerefficiency) == 0) isLowShowerEfficiency = kFALSE;

	    if(argument.CompareTo(sraw) == 0)
	    {
		ianalysisLevel++;
		if(ianalysisLevel>1)
		{
		    cerr<<"HShowerTaskSet: overlap with other argument for analysis level:RAW!" << endl;

		    cout << "input option string: " << s2  << endl;
		    exit(1);
		}

		analysisLevel=0;
	    }

	    else if(argument.CompareTo(scal) == 0)
	    {
		ianalysisLevel++;
		if(ianalysisLevel>1)
		{
		    cerr<<"HShowerTaskSet: overlap with other argument for analysis level:CAL!" << endl;

		    cout << "input option string: " << s2  << endl;
		    exit(1);
		}

		analysisLevel=1;
	    }

	    else if(argument.CompareTo(shit) == 0)
	    {
		ianalysisLevel++;
		if(ianalysisLevel>1)
		{
		    cerr<<"HShowerTaskSet: overlap with other argument for analysis level:HIT!" << endl;

		    cout << "input option string: " << s2  << endl;
		    exit(1);
		}

		analysisLevel=2;
	    }

	    else if(argument.CompareTo(ssimulation) == 0) {

		ianalysisSimOrReal++;
		if(ianalysisSimOrReal>1)
		{
		    cerr<<"HShowerTaskSet: ovrlap with other argument for analysis level:SIMULATION!"<< endl;
		    cout << "input option string: " << s2  << endl;
		    exit(1);
		}
		isSimulation=kTRUE;
	    }

	    else if(argument.CompareTo(sreal)==0)
	    {
		ianalysisSimOrReal++;
		if(ianalysisSimOrReal>1)
		{
		    cerr<<"HShowerTaskSet: overlap with other argument for analysis level:REAL!" << endl;
		    cout << "input option string: " << s2  << endl;
		    exit(1);
		}
		isSimulation=kFALSE;
	    }

	    else if(argument.CompareTo(smagnetoff)==0)
	    {
		imagnet++;
		if(imagnet>1)
		{
		    cerr<< "HShowerTaskSet: overlap with other argument for magnet status:MagnetOff!" << endl;
		    cout<<"INPUT OPTIONSTRING: "<<s2<<endl;
		    printOptions();
		    exit(1);
		}
		isCoilOff=kTRUE;
	    }

	    else if(argument.CompareTo(smagneton)==0)
	    {
		imagnet++;
		cout<<"INPUT OPTIONSTRING: "<<s2<<endl;
		if(imagnet>1)
		{
		    cerr<<"HShowerTaskSet: overlap with other argument for magnet status:MagnetOn!" << endl;
		    cout<<"INPUT OPTIONSTRING: "<<s2<<endl;
		    printOptions();
		    exit(1);
		}
		isCoilOff=kFALSE;
	    }
	}
	delete myiter;

	myarguments->Delete();
	delete myarguments ;
    }
}

void HShowerTaskSet::printOptions(void) {

    printf("*********************************************************** \n\n");

    printf("THESE ARE POSSIBLE ANALYSIS OPTIONS FOR SHOWER - TOFINO DETECTOR \n\n");
    printf("real - REAL DATA ANALYSIS / DEFAULT \n");
    printf("simulation - SIMULATION DATA ANALYSIS \n");
    printf("magnetoff - MAGNETIC FIELD SWITCH OFF \n");
    printf("magneton - MAGNETIC FIELD SWITCH ON / DEFAULT \n");
    printf("raw - ANALYSIS UP TO THE RAW LEVEL");
    printf("cal - ANALYSIS UP TO THE CAL LEVEL  \n");
    printf("hit - ANALYSIS UP TO THE HIT LEVEL  \n");

    printf("*********************************************************** \n");
}

ClassImp(HShowerTaskSet)

