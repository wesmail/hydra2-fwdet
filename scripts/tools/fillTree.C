#include "TStyle.h"
#include "TTree.h"
#include "TString.h"
#include "TObjString.h"
#include "TRandom.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TDatime.h"
#include "TObjArray.h"

#include "htool.h"

#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

ULong64_t timeVal;
Int_t njobsRun;
Int_t njobsPend;

TString cluster;
TString user;
TString group;
TString project;

Int_t   loadnRun;
Int_t   loadnAvail;
Int_t   loadnMax;
Float_t loadAvail;
Float_t loadMax;

Bool_t readLog(TString userDat,TTree* tree)
{
    //-----------------------------------------------------------
    if(gSystem->AccessPathName(userDat.Data())==0){
	ifstream in;
	in.open(userDat.Data());
	Char_t line[5000];
	TString l="";
        time_t timeStamp=0;

	while(in.good() && !in.eof())
	{
	    in.getline(line,5000);
	    l=line;
	    TString word;
	    if(l != "" ){
		timeStamp=0;

		njobsRun  =-1;
		njobsPend =-1;
		cluster   ="noCluster";
		user      ="noUser";
		group     ="noGroup";
		project   ="noProject";
		loadnRun  =-1;
		loadnAvail=-1;
		loadnMax  =-1;
		loadMax   =-1;
		loadAvail =-1;


		//-----------------------------------------------------------
		// tokenize the line: format "timestamp user1:njobs:njobstotal user2:njobs:njobstotal user3:njobs:njobstotal"
		TObjArray* a = l.Tokenize(" ");
		for(Int_t i=0; i<a->GetEntries();i++){
		    word = ((TObjString*)a->At(i))->GetString();
		    if(i==0) {
			timeStamp = word.Atoi();
			continue;
		    }

		    //-----------------------------------------------------------
		    // get user and number of jobs
		    TObjArray* a2 = word.Tokenize(":");
		    if(a2->GetEntries()==4){        // user log
			timeVal=timeStamp;
			user     = ((TObjString*)a2->At(0))->GetString();
			njobsRun = ((TObjString*)a2->At(1))->GetString().Atoi();
			njobsPend= ((TObjString*)a2->At(2))->GetString().Atoi() - njobsRun;
			group    = ((TObjString*)a2->At(3))->GetString();
			//project= ((TObjString*)a2->At(4))->GetString();
			tree->Fill();

		    } else if(a2->GetEntries()==3){ // group log
			timeVal=timeStamp;
			user     = ((TObjString*)a2->At(0))->GetString();
			njobsRun = ((TObjString*)a2->At(1))->GetString().Atoi();
			njobsPend= ((TObjString*)a2->At(2))->GetString().Atoi() - njobsRun;
			group    ="sum";
			project  ="sum";
			tree->Fill();

		    } else if(a2->GetEntries()==6){ // load log  : mainCluster:njobsRun:njobsAvail:njobsMaximum:loadAvail:loadMaximum
			timeVal=timeStamp;
			cluster = ((TObjString*)a2->At(0))->GetString();

                        if(cluster!="mainCluster") continue;

			loadnRun     = ((TObjString*)a2->At(1))->GetString().Atoi();
			loadnAvail   = ((TObjString*)a2->At(2))->GetString().Atoi();
			loadnMax     = ((TObjString*)a2->At(3))->GetString().Atoi();
			loadAvail    = ((TObjString*)a2->At(4))->GetString().Atof();
			loadMax      = ((TObjString*)a2->At(5))->GetString().Atof();

			tree->Fill();

		    } else {
			cout<<"wrong : "<<word.Data()<<endl;
		    }
		    a2->Delete();
		    delete a2;
		    //-----------------------------------------------------------
		}
		a->Delete();
		delete a;
		//-----------------------------------------------------------

	    }
	}
	in.close();

    } else {
	cout<<"userfile not found : "<<userDat.Data()<<endl;
    }
    //-----------------------------------------------------------
    return kTRUE;
}

void fillTree(TString userDat="archive/logUser_*.dat",TString groupDat="archive/logGroup_*.dat",TString loadDat="archive/logLoad_*.dat")
{
    gStyle->SetOptStat(0);
    TFile* out=new TFile("testTree.root","RECREATE");

    // Create a ROOT Tree
    TTree *tree = new TTree("T","userDat",99);

    time_t tnow=0;
    time(&tnow);
    struct tm* t_now = localtime(&tnow);
    TDatime today(t_now->tm_year+1900,t_now->tm_mon,t_now->tm_mday,0,00,00);
    gStyle->SetTimeOffset(today.Convert());

    time_t toffset = today.Convert();
    time_t timeStamp=0;


    Int_t bufsize=5000;
    tree->Branch("user"     ,&user     , bufsize,99);
    tree->Branch("group"    ,&group    , bufsize,99);
    tree->Branch("project"  ,&project  , bufsize,99);
    tree->Branch("timeVal"  ,&timeVal  ,"timeVal/l"  ,bufsize);
    tree->Branch("njobsRun" ,&njobsRun ,"njobsRun/I" ,bufsize);
    tree->Branch("njobsPend",&njobsPend,"njobsPend/I",bufsize);

    tree->Branch("cluster"   ,&cluster   , bufsize       ,99);
    tree->Branch("loadnRun"  ,&loadnRun  ,"loadnRun/I"  ,bufsize);
    tree->Branch("loadnAvail",&loadnAvail,"loadnAvail/I",bufsize);
    tree->Branch("loadnMax"  ,&loadnMax  ,"loadnMax/I"  ,bufsize);
    tree->Branch("loadAvail" ,&loadAvail ,"loadAvail/F" ,bufsize);
    tree->Branch("loadMax"   ,&loadMax   ,"loadMax/F"   ,bufsize);

    TObjArray* filesUser  = HTool::glob(userDat);
    TObjArray* filesGroup = HTool::glob(groupDat);
    TObjArray* filesLoad  = HTool::glob(loadDat);

    if(filesUser && filesGroup && filesLoad){

	if(filesUser->GetEntries()!=filesGroup->GetEntries()){
	    cout<<"Not same number of log files"<<endl;
	    return;
	}

	for(Int_t i=0; i< filesUser->GetEntries();i++)
	{
	    TString fileUser = ((TObjString*)filesUser ->At(i))->GetString();
	    TString fileGroup= ((TObjString*)filesGroup->At(i))->GetString();
	    TString fileLoad = ((TObjString*)filesLoad ->At(i))->GetString();
	    cout<<"add File :"<<fileUser<<endl;
	    readLog(fileUser,tree);
	    cout<<"add File :"<<fileGroup<<endl;
	    readLog(fileGroup,tree);
	    cout<<"add File :"<<fileLoad<<endl;
	    readLog(fileLoad,tree);
	}


	filesUser->Delete();
	delete filesUser;
	filesGroup->Delete();
	delete filesGroup;
	filesLoad->Delete();
	delete filesLoad;
    }

    tree->Write();
    out->Save();
    out->Close();

}
