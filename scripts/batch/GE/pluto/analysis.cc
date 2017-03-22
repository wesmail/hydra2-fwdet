



#include "TString.h"
#include "TROOT.h"


#include <iostream>
using namespace std;

#include "thermal_loop.C"    //int loop    (TString outdir="",TString outfile="",TString type="w" , Int_t nEvents=1000)
#include "embedding_loop.C"  //int loop_emb(TString outdir="",TString outfile="",TString type="e+", Int_t nEvents=1000,TString vertex_ntuple = "")



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


    TString number ;
    TString nevts ;
    TString sourceSwitch ;
    Int_t sSwitch = 0;  // default 0 = thermal, 1  = white emb
    switch (argc)
    {
    case 6:       // outdir,outfile,type nevents switch  :  thermal or embedding without vertex
        nevts        = argv[4];
	sourceSwitch = argv[5];
        sSwitch      = sourceSwitch.Atoi();
        if     (sSwitch == 0 ) return loop    (TString(argv[1]),TString(argv[2]),TString(argv[3]),nevts.Atoi());  // outdir,outfile,type nevents
        else if(sSwitch == 1)  return loop_emb(TString(argv[1]),TString(argv[2]),TString(argv[3]),nevts.Atoi());  // outdir,outfile,type nevents
	else  { cerr<<"ERROR: analysis() wrong source switch "<<sSwitch<<endl; return 1;}
	break;
    case 7:       // outdir,outfile,type nevents, switch vertexfile :  embedding with vertex
        nevts        = argv[4];
	sourceSwitch = argv[5];
        sSwitch      = sourceSwitch.Atoi();
	if(sSwitch == 0 )      return loop    (TString(argv[1]),TString(argv[2]),TString(argv[3]),nevts.Atoi(),TString(argv[6]));
	else if(sSwitch == 1 ) return loop_emb(TString(argv[1]),TString(argv[2]),TString(argv[3]),nevts.Atoi(),TString(argv[6]));
        else  { cerr<<"ERROR: analysis() wrong source switch "<<sSwitch<<endl; return 1;}
	break;
    default:
	cerr<<"ERROR: analysis() : WRONG NUMBER OF ARGUMENTS! TString outdir="",TString outfile="",TString type="", nevents=1000  sourceSwitch(0=thermal,1=white) [vertexntuple for embedding]"<<endl;

	return 1; // fail
    }
}
#endif


