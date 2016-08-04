

#include "hades.h"
#include "hloop.h"

#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"


#include <iostream>
using namespace std;



#ifndef __CINT__

Int_t readBack(TString infile,TString outfile,Int_t nEvents)
{

    if(gSystem->AccessPathName(infile.Data()) != 0){
	cout<<"READBACK: ERROR : cannot find inputfile : "<<infile.Data()<<endl;
        return 1;
    }

    HLoop loop(kTRUE);

    Bool_t ret = loop.addFile(infile);
    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfile : "<<infile.Data()<<endl;
	return 1;
    }
    if(!loop.setInput("")) {
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    } // read all categories
    loop.printCategories();
    loop.printChain();



    Int_t entries=loop.getEntries();
    if(nEvents < entries) entries = nEvents;



    for (Int_t i=0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);            // get next event. categories will be cleared before
	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
        if(i%1000 == 0) cout<<"event "<<i<<endl;
    }

    delete gHades;
    return 0;
}
int main(int argc, char **argv)
{
    TROOT Analysis("Analysis","compiled analysis macro");

    // argc is the number of arguments in char* array argv
    // CAUTION: argv[0] contains the progname
    // argc has to be nargs+1
    cout<<argc<<" arguments "<<endl;
    if(argc>1) cout<<"arg1 ="<<argv[1]<<endl;
    if(argc>2) cout<<"arg2 ="<<argv[2]<<endl;
    if(argc>3) cout<<"arg3 ="<<argv[3]<<endl;

    TString number ;
    TString nevts ;
    switch (argc)
    {
    case 4:       // just inputfile name + nEvents
        nevts  = argv[3];
	return readBack(TString(argv[1]),TString(argv[2]),nevts.Atoi());
	break;
    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! TString infile="",TString outfile="", nevents=1000"<<endl;

	return 1; // fail
    }
}
#endif


