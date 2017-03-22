
#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"


#include <iostream>
using namespace std;

#include "writeVertex.C"

#ifndef __CINT__

Int_t analysis(TString infile,TString outfile,Int_t nEvents)
{
    //  infileList : comma seprated file list "file1.root,file2.root"
    //  outfile    : optional (not used here) , used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    return writeVertex(infile,outfile,nEvents);
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
	return analysis(TString(argv[1]),TString(argv[2]),nevts.Atoi());
	break;
    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! TString infile="",TString outfile="", nevents=1000"<<endl;

	return 1; // fail
    }
}
#endif


