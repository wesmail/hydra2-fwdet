



#include "TString.h"
#include "TROOT.h"


#include <iostream>
using namespace std;

#include "pluto_embedded.C"   //Int_t thermal_omega_loop(TString outdir="",TString outfile="", nevents=1000)



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
    if(argc>4) cout<<"arg5 ="<<argv[5]<<endl;



    TString nevts ;
    switch (argc)
    {
    case 4:                   // outdir + outfile + nEvents + ntuplefile (including path)
        nevts  = argv[3];
	return pluto_embedded(TString(argv[1]),TString(argv[2]),nevts.Atoi());
	break;
    case 5:                   // outdir + outfile + nEvents + ntuplefile (including path)
        nevts  = argv[3];
	return pluto_embedded(TString(argv[1]),TString(argv[2]),nevts.Atoi(),argv[4]);
	break;
    case 6:                   // outdir + outfile + nEvents + ntuplefile (including path) + particletype
        nevts  = argv[3];
	return pluto_embedded(TString(argv[1]),TString(argv[2]),nevts.Atoi(),argv[4],argv[5]);
	break;

    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! TString outdir="",TString outfile="", nevents=1000 [ ntuplefile ] [particle type]"<<endl;

	return 1; // fail
    }
}
#endif


