#include "TROOT.h"
#include "TString.h"
#include "TFile.h"
#include "TTree.h"
#include "TSystem.h"
#include "htool.h"


#include <fstream>
#include <iostream>
using namespace std;

Int_t testFiles(TString name,Int_t nevents)
{

    if(gSystem->AccessPathName(name.Data()) !=0 ){
	cout<<"Error : file \""<<name.Data()<<"\"does not exist!"<<endl;
        return 1;
    }

    
    TFile* in = new TFile(name.Data(),"READ");
    if(!in){
	cout<<name.Data()<<" : ERROR : Could not open file"<<endl;
	in->Close();
	delete in;
	return 2;
    }
    TTree* T = (TTree*)in->Get("T");
    if(!T){
	cout<<name.Data()<<" : ERROR : Could not find Tree"<<endl;
	in->Close();
	delete in;
	return 3;
    }
    TKey* hades = in->FindKey("Hades");
    if(!hades){
	cout<<name.Data()<<" : ERROR : Could not find Hades object"<<endl;
	in->Close();
	delete in;
	return 4;
    }
    if(nevents!=-1){
	Int_t n = T->GetEntries();
	if(n<nevents){
	    cout<<name.Data()<<" : ERROR : Not enough Events in Tree : "<<n<<" ("<<nevents<<")"<<endl;
	    in->Close();
	    delete in;
	    return 5;
	}
    }
    cout<<"Check : file \""<<name.Data()<<" \"is OK!"<<endl;

    in->Close();

    return 0;
}
int main(int argc, char **argv)
{
    TROOT AnalysisDST("AnalysisDST","test macro");


    TString nevents = "-1";
    switch (argc)
    {
    case 2:
	return testFiles(TString(argv[1]),nevents.Atoi()); // inputfile + outdir
	break;
    case 3:

        nevents=argv[2];
	return testFiles(TString(argv[1]),nevents.Atoi()); // inputfile + outdir
	break;
    default:  // inputfile + outdir + nevents
	cout<<"usage : "<<argv[0]<<" file "<<endl;

        return 1;
    }
}
