

#include "TString.h"
#include "TRandom.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <algorithm>
using namespace std;

void makeLongFileList()
{

    Int_t nList  = 1500;
    Int_t nFiles = 100;
    TString fileName="/hera/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/Au_Au_1230MeV_1000evts";

    vector<Int_t> numbers;
    vector<Int_t> usednumbers;



    //-----------------------------------
    // read txt file with 1 number per line
    //
    //find /hera/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/ -name "*.root" | sed 's/_1\.root//' | sed 's/\/hera\/hades\/dstsim\/apr12\/hgeant\/bmax10\/new\/deltaelectron\/Au_Au_1230MeV_1000evts_//' | sort > numbers.txt
    Int_t number;
    ifstream input;
    input.open("numbers.txt");

    while(!input.eof()){
	input>>number;
	if(!input.fail()){
	    numbers.push_back(number);
	}
    }
    input.close();
    cout<<" nfiles "<<numbers.size()<<endl;
    //-----------------------------------

    ofstream out;
    out.open(Form("fileList.list"));

    for(Int_t n=0;n<nList;n++){ // create nFilelists
        TString list="";
        TString fname;
        usednumbers.clear();
	for(Int_t f=0;f<nFiles;f++){ // with nfiles added in each list

	    UInt_t nr = gRandom->Rndm()*numbers.size();
	    if(nr>numbers.size()-1) nr=numbers.size()-1;

	    while(find(usednumbers.begin(),usednumbers.end(),nr)!=usednumbers.end()){  // use each file only once in a list
		nr = gRandom->Rndm()*numbers.size();
		if(nr>numbers.size()-1) nr=numbers.size()-1;
	    }

	    if(f<nFiles-1) fname = Form("%s_%i_1.root,",fileName.Data(),nr);
            else           fname = Form("%s_%i_1.root" ,fileName.Data(),nr);
            list+=fname;
	}

	cout<<list.Data()<<endl;
	out<<list.Data()<<endl;


    }
    out.close();

}
