#include "hdiskspace.h"

#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include "TRegexp.h"
#include "TObjString.h"

#include "drawDisk.C"

#include <iostream>
#include <vector>
using namespace std;
void diskspaceComposite(
			TString dir    = "/hera/hades",
			TString outdir = "/misc/kempter/projects/diskmon/out/hera/hades",
			Int_t level    = 3,
                        TString exclude="",      // default ""
                        TString include=".*"    // default ".*"
		       )
{
    // composite dir    :  select subdirs inside mother dir by
    // include,exclude (commasepared list of TRegexp, exclude evaluated first) :
    //            default include all ".*" , exact matching "^dir1$,^dir2$,^dir#$"
    //            default exclude none ""
    // for each subdir a discatalog and diskstat object will be created in an own file.
    // A master catalog is build by number of subleveldirs copied from subdir catalogs

    dir.ReplaceAll("//","/");
    outdir.ReplaceAll("//","/");

    if(outdir.EndsWith("/")){
       outdir.Replace(outdir.Length()-1,1,"");
    }
    if(dir.EndsWith("/")){
       dir.Replace(dir.Length()-1,1,"");
    }


    gROOT->SetBatch();

    vector<TString> vlevel1names;
    vector<TString> vlevel1final;
    vector<TRegexp> excludenames;
    vector<TRegexp> includenames;

    //----------------------------------------------------------------
    // retrieve a full list of level1 subdirs of dir
    // this list will be filtered by exclude later
    HFileSys::lsDirectory(dir,vlevel1names);
    //----------------------------------------------------------------


    //----------------------------------------------------------------
    // get list of excludes : comma separated. each entry can be a TRegexp
    TObjArray* ar = exclude.Tokenize(",");
    if(ar){
	cout<<"adding exclude :------------------------------"<<endl;

	for(Int_t i = 0 ; i < ar->GetEntries(); i++){
	    TString name = ((TObjString*)ar->At(i))->GetString();
            cout<<name<<endl;
	    TRegexp expr(name);
            excludenames.push_back(expr);
	}
	ar->Delete();
        delete ar;
    }
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // get list of includes : comma separated. each entry can be a TRegexp
    ar = include.Tokenize(",");
    if(ar){
	cout<<"adding include :------------------------------"<<endl;

	for(Int_t i = 0 ; i < ar->GetEntries(); i++){
	    TString name = ((TObjString*)ar->At(i))->GetString();
            cout<<name<<endl;
	    TRegexp expr(name);
            includenames.push_back(expr);
	}
	ar->Delete();
        delete ar;
    }
    //----------------------------------------------------------------

    //----------------------------------------------------------------
    // filter the dir list by excludes and put them into final list
    // of subdirs
    cout<<"analyzing include/exclude : ------------------"<<endl;
    for(UInt_t i = 0 ; i < vlevel1names.size() ; i++){
	TString name = vlevel1names[i];
        Bool_t skip=kFALSE;
	for(UInt_t i = 0 ; i < excludenames.size() ; i++){
            TRegexp& expr = excludenames[i];
	    if(name(expr) != "") {
		skip = kTRUE;
                cout<<"skip exclude "<<name<<endl;
                break;
	    }
	}
        Bool_t keep = kFALSE;
	for(UInt_t i = 0 ; i < includenames.size() ; i++){
            TRegexp& expr = includenames[i];
	    if(name(expr) != "") {
		keep = kTRUE;
                //cout<<"keep include "<<name<<endl;
                break;
	    }
	}
	if(skip == kTRUE || keep == kFALSE) continue;
	vlevel1final.push_back(name);
        cout<<"final "<<name<<endl;
    }
    //----------------------------------------------------------------

    TString supername = dir;
    supername.ReplaceAll("/","_");

    TFile* fsuper = new TFile(Form("%s/dirdata%s_new.root",outdir.Data(),supername.Data()),"RECREATE");
    fsuper->cd();

    Bool_t createSuper = kTRUE;
    HDiskCatalog* catalogsuper = new HDiskCatalog(dir);
    catalogsuper->SetName(supername.Data());
    HDiskDir basesuper(dir);
    basesuper.scan(0);   // get list of files in base dir
    catalogsuper->addDir(&basesuper);

    time_t start = 0;
    time_t stop  = 0;


    HDiskStat* statsuper    = 0;
    if(gSystem->AccessPathName(Form("%s/dirdata%s.root",outdir.Data(),supername.Data()))==0) createSuper = kFALSE;

    cout<<"before input master :-----------------------------"<<endl;

    if(createSuper) {
	cout<<"No input file master "<<Form("%s/dirdata%s.root",outdir.Data(),supername.Data())<<" found, will create input!"<<endl;
	statsuper    = new HDiskStat(dir);
	statsuper    ->SetName(Form("stat%s",supername.Data()));
    } else {
	cout<<"Input file master "<<Form("%s/dirdata%s.root",outdir.Data(),supername.Data())<<" found, will use as input!"<<endl;
	TFile* fin  = new TFile(    Form("%s/dirdata%s.root",outdir.Data(),supername.Data()),"READ");
	fin->cd();
	statsuper    = (HDiskStat*) gDirectory->Get(Form("stat%s",supername.Data()));
    }

    //----------------------------------------------------------------
    // loop over subdirs and a catalog for each entry
    TString mothername = dir;
    mothername.ReplaceAll("/","_");
    for(UInt_t i = 0 ; i < vlevel1final.size() ; i++)
    {
	HDiskCatalog* catalog = 0;
	HDiskStat*       stat = 0;
	Bool_t create         = kTRUE;
	TString level1name = vlevel1final[i];
	level1name.ReplaceAll("/","_");
	TString objname=Form("%s_%s",mothername.Data(),level1name.Data());
	TString fname = Form("dirdata%s",objname.Data());
        TString scandir=Form("%s/%s",dir.Data(),vlevel1final[i].Data());
        TFile* fin=0;
	cout<<"PROCESSING : "<<dir<<"/"<<level1name<<endl;
	if(gSystem->AccessPathName(Form("%s/%s.root",outdir.Data(),fname.Data()))==0) create = kFALSE;

	cout<<"before input : -----------------------------------"<<endl;
	if(create) {
	    cout<<"No input file "<<Form("%s/%s.root",outdir.Data(),fname.Data())<<" found, will create input!"<<endl;
	} else {
	    cout<<"Input file "<<Form("%s/%s.root",outdir.Data(),fname.Data())<<" found, will use as input!"<<endl;
	}
	if(!create)
	{
	    fin  = new TFile(Form("%s/%s.root",outdir.Data(),fname.Data()),"READ");
	    fin->cd();
	    catalog = (HDiskCatalog*) gDirectory->Get(objname.Data());
	    stat    = (HDiskStat*)    gDirectory->Get(Form("stat%s",objname.Data()));
	    if(catalog && catalog->getDiskName().CompareTo(scandir)==0 ){
		cout<<"\n\n\n"<<endl;
		cout<<"old catalog summary : ------------------------"<<endl;
		catalog->printDisk(level,"size",8,"-");

	    } else {
		cout<<"Calatlog not found"<<endl;
	    }
            fin->Close();
            delete fin;
	} else {
	    cout<<"\n\n\n"<<endl;
	    cout<<"creating new Catalog: ------------------------"<<endl;
	    catalog = new HDiskCatalog(scandir);
	    catalog->SetName(objname.Data());
	    stat    = new HDiskStat(scandir);
	    stat    ->SetName(Form("stat%s",objname.Data()));
	}



	cout<<"running scan : -------------------------------"<<endl;
	TFile* fout = new TFile(Form("%s/%s_new.root",outdir.Data(),fname.Data()),"RECREATE");
	fout->cd();
	cout<<"\n\n\n"<<endl;

	catalog->scan();

	cout<<"\n\n\n"<<endl;

	cout<<"new catalog summary : ------------------------"<<endl;
	catalog->printDisk(level,"size",8,"-");

	cout<<"adding to history   : ------------------------"<<endl;

	//----------------------------------------------------------------
	// add new entries to the stat object
        Int_t oldindex =0;
	HDiskDir* base = catalog->getDir(catalog->getDiskName(),&oldindex);

        if (start == 0)                        start = catalog->getLastScanStart();
        if (catalog->getLastScanStop() > stop) stop  = catalog->getLastScanStop();


	if(base){
	    vector<HDiskDir*> daughters;
	    stat   ->addEntry(catalog->getLastScanStart(),base);
	    catalog->getDaughterDirs(base,daughters);

	    for(UInt_t i=0;i<daughters.size();i++){
		stat->addEntry(catalog->getLastScanStart(),daughters[i]);
	    }

	    //----------------------------------------------------------------
            // master stat and catalog
	    cout<<"ADD TO MASTER CATALOG :"<<base->GetName()<<endl;
	    statsuper   ->addEntry(start,base);  // master catalog
	    TObjArray* list = catalog->getList() ;
	    for(Int_t i=0;i<list->GetEntries();i++){
		HDiskDir* dold = (HDiskDir*) list->At(i);
		if(dold->getLevel() < level){
		    dold->print();
		    catalogsuper->addDir(dold);
		}
	    }
	    //----------------------------------------------------------------
	}
	//----------------------------------------------------------------

	cout<<"drawing  disk     : ------------------------"<<endl;
	drawDisk(catalog,stat,outdir,level);

	fout->cd();
	catalog->Write();
	stat   ->Write();
	fout->Save();
	fout->Close();
        delete fout;
	delete stat;
        delete catalog;

	cout<<"\n\n\n"<<endl;

	cout<<"moving files : --------------------------------"<<endl;
	if(!create) gSystem->Exec(Form("mv %s/%s.root %s/%s_old.root",outdir.Data(),fname.Data(),outdir.Data(),fname.Data()));
	gSystem->Exec(Form("mv %s/%s_new.root %s/%s.root",outdir.Data(),fname.Data(),outdir.Data(),fname.Data()));
	cout<<"finished  : -----------------------------------"<<endl;

    } // end loop level1 dirs
    //----------------------------------------------------------------

    cout<<"writing master :-------------------------------"<<endl;

    cout<<"new catalog summary : ------------------------"<<endl;
    catalogsuper->setLastScanStart(start);
    catalogsuper->setLastScanStop(stop);
    catalogsuper->setUserMap (HFileSys::getUserMap());
    catalogsuper->setGroupMap(HFileSys::getGroupMap());
    catalogsuper->updateDirIndices();


    HDiskDir* base = catalogsuper->getDir(dir);
    statsuper   ->addEntry(start,base);

    catalogsuper->printDisk(level,"size",8,"-");
    drawDisk(catalogsuper,statsuper,outdir,level);

    ofstream fout(Form("%s/dirdata%s_dir_summary_1.log",outdir.Data(),catalogsuper->GetName()));
    if(1)
    {   // scope for redirect
	HRedirect log(&cout,&fout);// cout -> base logfile
	catalogsuper->printDisk(1,"size",8,"-");
    }
    fout.close();

    ofstream fout2(Form("%s/dirdata%s_dir_summary_2.log",outdir.Data(),catalogsuper->GetName()));
    if(1)
    {   // scope for redirect
	HRedirect log(&cout,&fout2);// cout -> base logfile
	catalogsuper->printDisk(2,"size",8,"-");
    }
    fout2.close();

    ofstream fout3(Form("%s/dirdata%s_dir_summary_3.log",outdir.Data(),catalogsuper->GetName()));
    if(1)
    {   // scope for redirect
	HRedirect log(&cout,&fout3);// cout -> base logfile
	catalogsuper->printDisk(3,"size",8,"-");
    }
    fout3.close();

    ofstream fout4(Form("%s/scanned_dirs.log",outdir.Data()));
    for(UInt_t i = 0; i < vlevel1final.size(); i++){
	fout4<<vlevel1final[i]<<endl;
    }
    fout4.close();

    fsuper->cd();
    catalogsuper->Write();
    statsuper   ->Write();
    fsuper->Save();
    fsuper->Close();

    cout<<"\n\n\n"<<endl;

    cout<<"moving master files :--------------------------"<<endl;
    if(!createSuper) gSystem->Exec(Form("mv %s/dirdata%s.root %s/dirdata%s_old.root",outdir.Data(),supername.Data(),outdir.Data(),supername.Data()));
                     gSystem->Exec(Form("mv %s/dirdata%s_new.root %s/dirdata%s.root",outdir.Data(),supername.Data(),outdir.Data(),supername.Data()));
    cout<<"finished  : -----------------------------------"<<endl;
}
