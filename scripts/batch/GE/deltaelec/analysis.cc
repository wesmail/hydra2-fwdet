



#include "TString.h"
#include "TROOT.h"


#include <iostream>
using namespace std;

#include "targ_delta_emission.C"   //Int_t targ_delta_emission(Int_t maxEvents, TString shell, TString inputDir,TString outputDir,TString outputFile, Int_t fileNumber)


#ifndef __CINT__
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
    if(argc>4) cout<<"arg4 ="<<argv[4]<<endl;
    if(argc>5) cout<<"arg5 ="<<argv[5]<<endl;
    if(argc>6) cout<<"arg6 ="<<argv[6]<<endl;


    TString nevts ;
    TString filenum ;
    switch (argc)
    {
    case 7:         // Int_t maxEvents, TString shell, TString inputDir,TString outputDir ,TString outputFile, Int_t fileNumber
        nevts        = argv[1];
        filenum      = argv[6];
	return targ_delta_emission (nevts.Atoi(),argv[2],argv[3],argv[4],argv[5],filenum.Atoi());
	break;
    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! Int_t maxEvents, TString shell, TString inputDir, TString outputDir, TString outputFile, Int_t fileNumber"<<endl;

	return 1; // fail
    }
}
#endif


