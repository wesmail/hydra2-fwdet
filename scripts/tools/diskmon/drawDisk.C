#ifndef ___DRAWDISK___
#define ___DRAWDISK___


#include "hdiskspace.h"
#include "TFile.h"
#include "TString.h"
#include "TH1D.h"
#include "TCanvas.h"
#include "TStyle.h"

#include <vector>


#include <iostream>
#include <fstream>
#include <streambuf>

using namespace std;



void setAxisUser(TH1D* h,vector<HDiskDir*>& daughters,Float_t titleoffset)
{
    for(UInt_t i = 0; i < daughters.size(); i ++ ){
	HDiskDir* d = daughters[i];
	TString n = d->GetName();
	n = gSystem->BaseName(n.Data());
	h->GetXaxis()->SetBinLabel(i+1,n.Data());
    }
    h->GetXaxis()->LabelsOption("v");
    h->GetYaxis()->SetTitleOffset(titleoffset);
}


void drawDisk(HDiskCatalog* catalog,
              HDiskStat*    catalogstat,
	      TString outdir   = "/misc/kempter/projects/diskspace/web",
              Int_t level=1
	      )
{
    if(catalog ==0) {
	cout<<"ERROR : drawDisk() : catalog ==NULL "<<endl;
	return;
    }

    if(catalogstat ==0) {
	cout<<"ERROR : drawDisk() : catalogstat ==NULL "<<endl;
	return;
    }
    ULong64_t        U = HFileSys::getUnitT();
    UInt_t    lastdays = 100;

    gStyle->SetOptStat(0);
    gStyle->SetPadTopMargin(0.08);
    gStyle->SetPadBottomMargin(0.2);
    gStyle->SetPadRightMargin(0.11);
    gStyle->SetPadLeftMargin(0.08);

    Float_t titleoffset=1.;


    TString fname = catalog->getDiskName();
    fname.ReplaceAll("/","_");
    fname = Form("dirdata%s",fname.Data());

    cout<<"READING "<<Form("%s/%s.root",outdir.Data(),fname.Data())<<endl;

    ofstream fout(Form("%s/%s_dir_summary.log",outdir.Data(),fname.Data()));
    if(1)
    {   // scope for redirect
	HRedirect log(&cout,&fout);// cout -> summary logfile
	catalog->printDisk(level,"size",8,"-");
    }
    fout.close();


    ofstream fout1(Form("%s/%s_dir_summary_1.log",outdir.Data(),fname.Data()));

    if(1)
    {   // scope for redirect
	HRedirect log1(&cout,&fout1);// cout -> summary logfile
	catalog->printDisk(1,"size",8,"-");
    }
    fout1.close();

    ofstream fout2(Form("%s/%s_dir_summary_2.log",outdir.Data(),fname.Data()));
    if(1)
    {   // scope for redirect
	HRedirect log2(&cout,&fout2);// cout -> summary logfile
	catalog->printDisk(2,"size",8,"-");
    }
    fout2.close();

    ofstream fout3(Form("%s/%s_dir_summary_3.log",outdir.Data(),fname.Data()));
    if(1)
    {   // scope for redirect
	HRedirect log3(&cout,&fout3);// cout -> summary logfile
	catalog->printDisk(3,"size",8,"-");
    }
    fout3.close();

    HDiskDir* base = catalog->getDir(catalog->getDiskName());

    if(base)
    {
        TString user;
	vector<HDiskDir*> daughters;

	cout<<"size---------------------------------------------------------"<<endl;
	catalog->sortDirs(base,daughters,"size");

	TCanvas* clevel1 = new TCanvas("clevel1","direcories of 1st level",1500,800);
        //clevel1->Divide(2,2);
        //-------------------------------------------------------------------------
	TH1D*    hlevel1_size = new TH1D("hlevel1_size","",daughters.size(),0,daughters.size());
	hlevel1_size ->SetFillColor(kOrange+1);
	hlevel1_size ->SetYTitle("diskspace [TB]");

	setAxisUser(hlevel1_size,daughters,titleoffset);

	for(UInt_t i = 0; i < daughters.size(); i ++ ){
	    HDiskDir* level1 = daughters[i];
	    if(level1){
		hlevel1_size->SetBinContent(i+1,level1->getSize()/(Double_t)HFileSys::getUnitT()) ;
	    } else {cout<<"mist"<<endl;}
	}
        //-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
        cout<<"nfiles-------------------------------------------------------"<<endl;
	daughters.clear();
	catalog->sortDirs(base,daughters,"nfiles");
	TH1D* hlevel1_nfiles = new TH1D("hlevel1_nfiles","",daughters.size(),0,daughters.size());
	hlevel1_nfiles ->SetFillColor(kOrange+1);
	hlevel1_nfiles ->SetYTitle("number of files");

	setAxisUser(hlevel1_nfiles,daughters,titleoffset);

	for(UInt_t i = 0; i < daughters.size(); i ++ ){
	    HDiskDir* level1 = daughters[i];
	    if(level1){
                hlevel1_nfiles->SetBinContent(i+1,level1->getNFilesTotal()) ;
	    } else {cout<<"mist"<<endl;}
	}
        //-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
        cout<<"smallfiles---------------------------------------------------"<<endl;
	daughters.clear();
	catalog->sortDirs(base,daughters,"nsmallfiles");
	TH1D*  hlevel1_nsmallfiles = new TH1D("hlevel1_nsmallfiles","",daughters.size(),0,daughters.size());
	hlevel1_nsmallfiles ->SetFillColor(kOrange+1);
	hlevel1_nsmallfiles ->SetYTitle("number of small files");

	setAxisUser(hlevel1_nsmallfiles,daughters,titleoffset);

	for(UInt_t i = 0; i < daughters.size(); i ++ ){
	    HDiskDir* level1 = daughters[i];
	    if(level1){
		catalog->getUser(level1->getOwner(),user);
                hlevel1_nsmallfiles->SetBinContent(i+1,level1->getNSmallFilesTotal()) ;
	    } else {cout<<"mist"<<endl;}
	}
        //-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
        cout<<"ratio--------------------------------------------------------"<<endl;
	daughters.clear();
	catalog->sortDirs(base,daughters,"filesratio");

	TH1D*  hlevel1_nsmallfiles_ratio = new TH1D("hlevel1_nsmallfiles_ratio","",daughters.size(),0,daughters.size());
	hlevel1_nsmallfiles_ratio ->SetFillColor(kOrange+1);
	hlevel1_nsmallfiles_ratio ->SetYTitle("number of small files / number of files");

	setAxisUser(hlevel1_nsmallfiles_ratio,daughters,titleoffset);

	for(UInt_t i = 0; i < daughters.size(); i ++ ){
	    HDiskDir* level1 = daughters[i];
	    if(level1){
		catalog->getUser(level1->getOwner(),user);
		if(level1->getNFilesTotal()>0){
		    hlevel1_nsmallfiles_ratio->SetBinContent(i+1,level1->getNSmallFilesTotal()/(Double_t)level1->getNFilesTotal()) ;
		}
	    } else {cout<<"mist"<<endl;}
	}
        //-------------------------------------------------------------------------

        cout<<"draw---------------------------------------------------------"<<endl;



        cout<<"draw size----------------------------------------------------"<<endl;
	clevel1->cd();
	gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetLogy();
	hlevel1_size->Draw();
        gPad->SaveAs(Form("%s/%s_size.png",outdir.Data(),fname.Data()));


        cout<<"draw nfiles--------------------------------------------------"<<endl;
	clevel1->cd();
	gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetLogy();
	hlevel1_nfiles->Draw();
        gPad->SaveAs(Form("%s/%s_nfiles.png",outdir.Data(),fname.Data()));

        cout<<"draw nsmallfiles---------------------------------------------"<<endl;
	clevel1->cd();
	gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetLogy();
	hlevel1_nsmallfiles->Draw();
        gPad->SaveAs(Form("%s/%s_nsmallfiles.png",outdir.Data(),fname.Data()));

        cout<<"draw ratio---------------------------------------------------"<<endl;
	clevel1->cd();
	gPad->SetGridx();
        gPad->SetGridy();
        gPad->SetLogy();
	hlevel1_nsmallfiles_ratio->Draw();
        gPad->SaveAs(Form("%s/%s_ratio.png",outdir.Data(),fname.Data()));


    }
    TString select = "daughters";

    TCanvas* c = catalogstat->draw("size",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_size.png",outdir.Data(),fname.Data()));

    c = catalogstat->draw("nfiles",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_nfiles.png",outdir.Data(),fname.Data()));

    c = catalogstat->draw("nsmallfiles",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_nsmallfiles.png",outdir.Data(),fname.Data()));

    select = "mother";

    c = catalogstat->draw("size",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_%s_size.png",outdir.Data(),fname.Data(),select.Data()));

    c = catalogstat->draw("nfiles",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_%s_nfiles.png",outdir.Data(),fname.Data(),select.Data()));

    c = catalogstat->draw("nsmallfiles",lastdays,U,select);
    if(c) c->SaveAs(Form("%s/history_%s_%s_nsmallfiles.png",outdir.Data(),fname.Data(),select.Data()));

    cout<<"done---------------------------------------------------------"<<endl;


}
#endif
