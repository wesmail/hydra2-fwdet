#ifndef __HSelectedEvents__
#define __HSelectedEvents__

#include "hades.h"
#include "htime.h"
#include "hreconstructor.h"
#include "hdatasource.h"

#include "TString.h"


#include <fstream>
#include <iostream>

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////
// HSelectedEvents
//
// Helper class to read just selected events form hld files.
// Needs a list of squence numbers from hld file as ascii input.
// Format :  hldfilename eventsequencenumber per line
// ( example : be1210817112103 27 )
// All events not contained in the list will be skipped.
// If no input file is set all events will be taken.
// Add this task first in the task list. The task will analyze
// the eventhader only, no complicated event reconstruction is
// needed.
//
//-------------------------------------------------------------
//    CREATE a list of sequence numbers using HLoop:
// {
//    HLoop loop(kTRUE);
//
//    Bool_t ret = loop.addMultFiles(infileList);
//    if(ret == 0) {
//	cout<<"READBACK: ERROR : cannot find inputfiles : "<<infileList.Data()<<endl;
//	return 1;
//    }
//
//    if(!loop.setInput("")) {
//	cout<<"READBACK: ERROR : cannot read input !"<<endl;
//	exit(1);
//    } // read all categories
//    loop.printCategories();
//    loop.printChain();
//
//    ofstream output;
//    output.open("seqNumber.txt");
//
//    TString fname; fnewname;
//
//    for (Int_t i=0; i < entries; i++) {
//        Int_t nbytes =  loop.nextEvent(i);            // get next event. categories will be cleared before
//        if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
//
//        if(loop.isNewFile(fnewname)){ fname = HTime::stripFileName(fnewname);}
//
//        Bool_t condition = kFALSE;
//        //---------------------------------------------------
//        // ... now select events and set condition=kTRUE ...
//
//
//        if(condition){
//	     cout  <<fname<<" "<<loop.getEventHeader()->getEventSeqNumber()<<endl;
// 	     output<<fname<<" "<<loop.getEventHeader()->getEventSeqNumber()<<endl;
//        }
//     }
//
//   output.close();
// }
//-------------------------------------------------------------
// usage:
//
// HSelectedEvents* selection = new HSelectedEvents("")
// HSelectedEvents* selection = new HSelectedEvents("HSeletedEvents","HSelectedEvents");
// selection->setInputFile("seqNumber.txt");  // comment this line for no selection
//
// HTaskSet *masterTaskSet = gHades->getTaskSet("all");
// masterTaskSet->add(selection);  // put it first in tasklist to skip discarded events from the beginning

//
///////////////////////////////////////////////////////////////

using namespace std;

class HSelectedEvents : public HReconstructor
{
protected:
    // put all vars here which are not
    // local to a function
    ifstream input;
    TString fname;
    TString bename;
    Int_t seqNum;
public:

HSelectedEvents(const Text_t *name = "",const Text_t *title ="")
	: HReconstructor(name,title)
{
    // init your vars
    fname = "";
}



virtual ~HSelectedEvents()
{
    // clean up all dynamically created objects

}

Bool_t init()
{
    // this function is called once in the beginning
    // create histograms or get pointer to param containers
    // or data containers here. Tip: Create first your
    // output file and and after that your histograms/ ntuples.
    // In this way you make shure the object will end up in your
    // root file and not any other one.


    if(fname.CompareTo("")!=0) input.open(fname);
    else {
	Warning("init()","Inputfile not set!");
	return kTRUE;
    }

    if(input.good() ) input >> bename >> seqNum;

    return kTRUE;
}
Bool_t reinit()
{   // this function is called for each file
    // after init()
    // use this place if you need already initialized
    // containers

    return kTRUE;
}

Int_t execute()
{
    // this function is called once per event.
    // if the function returns kSkipEvent the event
    // will be skipped a. for all following tasks
    // and b. not appear in output (hades eventLoop())

    Bool_t print =kTRUE;


    if(input.good()){

	Int_t num = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
	TString currentfile;

	HDataSource* source = gHades->getDataSource();
	if(source)  {
	    currentfile = source->getCurrentFileName();
	    currentfile = HTime::stripFileName(currentfile);
	}

	if(currentfile.CompareTo(bename.Data()) != 0){
	    if(print) cout<<"wind forward to find correct file name : current "<<currentfile<<" list : "<< bename<<endl;

	    while(currentfile.CompareTo(bename.Data()) == 0 && input.good()) {
		if(input.good() ) {
		    input >> bename >> seqNum;
		    if(print)cout<<"file name : current "<<currentfile<<" list : "<< bename<<" while new seqnum >= current "<<num<<" request "<< seqNum <<endl;
		} else {
		    cout<<"input broken!"<<endl;
		    break;
		}
	    }

	}


	if(num < seqNum) {  // discard this event
	    if(print)cout<<"file name : current "<<currentfile<<" list : "<< bename<<" skip : current "<<num<<" request "<< seqNum <<endl;
	    return kSkipEvent;
	}
	if(num == seqNum) { // get new seqnumber from file
	    if(print)cout<<"file name : current "<<currentfile<<" list : "<< bename<<" get new seqnum :  current "<<num<<" request "<< seqNum <<endl;
	    if(input.good() ) input >> bename >> seqNum;
	    return 0;
	}
	if(num > seqNum) {  // wind forward seqnumber from file
	    if(print)cout<<"file name : current "<<currentfile<<" list : "<< bename<<" wind new seqnum : current "<<num<<" request "<< seqNum <<endl;
	    while(currentfile.CompareTo(bename.Data()) == 0 && seqNum <= num && input.good()) {
		if(input.good() ) {
		    input >> bename >> seqNum;
		    if(print)cout<<"file name : current "<<currentfile<<" list : "<< bename<<" while new seqnum >= current "<<num<<" request "<< seqNum <<endl;
		} else {
		    cout<<"input broken!"<<endl;
		    break;
		}
	    }
	    if(num < seqNum) return kSkipEvent;   // discard this event
	}
    }
    return 0;
}

Bool_t finalize()
{   // this function is called once in the end of the
    // runtime of the program. Save histograms to file
    // etc.
    //input.close();
    return kTRUE;
}

void setInputFile(TString name) {fname = name;}

//ClassDef(HSelectedEvents,0) // no streamer, do not use if you include blank header in ACLIC
};

#endif
