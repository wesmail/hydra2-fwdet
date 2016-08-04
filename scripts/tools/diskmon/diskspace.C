#include "hdiskspace.h"

#include "TString.h"
#include "TH1F.h"
#include "TFile.h"
#include "TSystem.h"
#include "TROOT.h"
#include <iostream>

#include "drawDisk.C"

void diskspace(TString dir    = "/hera/hades/user/kempter",
	       TString outdir = "/misc/kempter/projects/diskspace/web",
               Int_t level = 1
	      )
{
    gROOT->SetBatch();
    HDiskCatalog* catalog = 0;
    HDiskStat*       stat = 0;
    Bool_t create         = kTRUE;

    TString fname = dir;
    fname.ReplaceAll("/","_");
    TString objname=fname;
    fname = Form("dirdata%s",fname.Data());

    if(gSystem->AccessPathName(Form("%s/%s.root",outdir.Data(),fname.Data()))==0) create = kFALSE;

    cout<<"before input : -----------------------------------"<<endl;
    if(create) {
	cout<<"No input file "<<Form("%s/%s.root",outdir.Data(),fname.Data())<<" found, will create input!"<<endl;
    } else {
	cout<<"Input file "<<Form("%s/%s.root",outdir.Data(),fname.Data())<<" found, will use as input!"<<endl;
    }

    if(!create)
    {
	TFile* fin  = new TFile(Form("%s/%s.root",outdir.Data(),fname.Data()),"READ");
	fin->cd();
	catalog = (HDiskCatalog*) gDirectory->Get(objname.Data());
        stat    = (HDiskStat*)    gDirectory->Get(Form("stat%s",objname.Data()));
	if(catalog && catalog->diskname.CompareTo(dir)==0 ){
	    cout<<"\n\n\n"<<endl;
            cout<<"old catalog summary : ------------------------"<<endl;
            catalog->printDisk(level,"size",8,"-");

	} else {
	    cout<<"Calatlog not found"<<endl;
	}
    } else {
	cout<<"\n\n\n"<<endl;
	cout<<"creating new Catalog: ------------------------"<<endl;
	catalog = new HDiskCatalog(dir);
        catalog->SetName(objname.Data());
	stat    = new HDiskStat(dir);
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
    HDiskDir* base = catalog->getDir(catalog->getDiskName());

    if(base){
	vector<HDiskDir*> daughters;
        stat->addEntry(catalog->getLastScanStart(),base);
	catalog->getDaughterDirs(base,daughters);
	for(UInt_t i=0;i<daughters.size();i++){
            stat->addEntry(catalog->getLastScanStart(),daughters[i]);
	}
    }
    //----------------------------------------------------------------

    cout<<"drawing  disk     : ------------------------"<<endl;
    drawDisk(catalog,stat,outdir,level);

    fout->cd();
    catalog->Write();
    stat   ->Write();
    fout->Save();
    fout->Close();

    cout<<"\n\n\n"<<endl;

    cout<<"moving files : --------------------------------"<<endl;
    if(!create) gSystem->Exec(Form("mv %s/%s.root %s/%s_old.root",outdir.Data(),fname.Data(),outdir.Data(),fname.Data()));
    gSystem->Exec(Form("mv %s/%s_new.root %s/%s.root",outdir.Data(),fname.Data(),outdir.Data(),fname.Data()));
    cout<<"finished  : -----------------------------------"<<endl;

}
