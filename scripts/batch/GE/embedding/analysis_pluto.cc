



#include "TString.h"
#include "TROOT.h"


#include <iostream>
using namespace std;

#include "pluto_embedded.C"   //Int_t pluto_embedded(TString outdir="",TString outfile="",TString type="pi-",Int_t nEvents=1000,Int_t source = 1,TString vertexntuple="")



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
    if(argc>7) cout<<"arg7 ="<<argv[7]<<endl;

    cout<<argc-1<<" parameter"<<endl;

    TString nevts ;
    TString source ;
    switch (argc)
    {
    case 7:                   // outdir + outfile + type +nEvents sourceswitch + ntuplefile (including path)
        nevts  = argv[4];
        source = argv[5];

	return pluto_embedded(TString(argv[1]),TString(argv[2]), argv[3],nevts.Atoi(),source.Atoi(),argv[6]);
	break;

    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! outdir, outfile, particle_type nevents source(0:white embedding,1:thermal) [ ntuplefile ]"<<endl;
	cerr<<"available types :"<<endl;
	cerr<<"white  : any single particle"<<endl;
	cerr<<"thermal: hadrons lambda,Xi-,K0S,K-,K+,phiKK,p,d,pi-,pi+ leptons:pi0,eta,w,wdalitz,phi,rho0,D+"<<endl;
	return 1; // fail
    }
}
#endif


