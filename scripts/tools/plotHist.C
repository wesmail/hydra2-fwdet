
#include "TSystem.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#include "TGraph.h"
#include "TH2F.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TLegend.h"

#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>

using namespace std;

static Int_t colors[]=
{
    kRed  ,kMagenta  ,kBlue  ,kCyan  ,kGreen  ,kOrange,
    kRed-6,kMagenta-6,kBlue-6,kCyan-6,kGreen-6,kOrange-6,
    kRed-7,kMagenta-7,kBlue-7,kCyan-7,kGreen-7,kOrange-7,
    kRed-2,kMagenta-2,kBlue-2,kCyan-2,kGreen-2,kOrange-2,kBlack
};
static    Int_t markers[]={
    20,21,22,23,24,25,26,27,28
};

static Int_t styles[]={
    1,2
};
static    Int_t nColors  = sizeof(colors) /sizeof(Int_t);
static    Int_t nMarkers = sizeof(markers)/sizeof(Int_t);
static    Int_t nStyles  = sizeof(styles)/sizeof(Int_t);


Bool_t findMinMaxGraph(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,TGraph* g)
{
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    Double_t x,y;

    for(Int_t i=0; i<g->GetN();i++){
	g->GetPoint(i,x,y);
	if(x<xmin || xmin==0) xmin = x;
	if(x>xmax || xmax==0) xmax = x;
        if(y<ymin || ymin==0) ymin = y;
	if(y>ymax || ymax==0) ymax = y;

    }
    return kTRUE;
}


Bool_t findMinMax(Double_t& xmin,Double_t& xmax,Double_t& ymin,Double_t& ymax,map<TString,TGraph*>& mymap)
{
    xmax = 0;
    xmin = 0;
    ymax = 0;
    ymin = 0;
    Double_t x1,y1,x2,y2;
    for(map<TString,TGraph*>::iterator iter = mymap.begin(); iter != mymap.end(); ++iter ) {
	TGraph*g  = iter->second;
	findMinMaxGraph(x1,x2,y1,y2,g);

	if(x1<xmin || xmin==0) xmin = x1;
	if(x2>xmax || xmax==0) xmax = x2;

	if(y1<ymin || ymin==0) ymin = y1;
	if(y2>ymax || ymax==0) ymax = y2;
    }

    if(ymin==ymax && ymin!=0) ymin=0;
    if(ymin==ymax && ymin==0) { ymin=0; ymax = 10;}

    return kTRUE;
}


void addToLegend(TLegend* l,map<TString,TGraph*>& mymap)
{
    for(map< TString, TGraph*>::iterator iter = mymap.begin(); iter != mymap.end(); ++iter ) {
	TGraph* g  = iter->second;
	l->AddEntry(g,g->GetName(),"lp");
    }
}

void divideUser(Int_t joblimit,
		map<TString,TGraph*>& mymap,
	        map<TString,TGraph*>& mymapPend,
	        map<TString,TGraph*>& mymapBig,
		map<TString,TGraph*>& mymapSmall,
		map<TString,TGraph*>& mymapBigPend,
		map<TString,TGraph*>& mymapSmallPend
	       )
{
    Double_t x1,y1,x2,y2;
    for(map<TString,TGraph*>::iterator iter = mymap.begin(); iter != mymap.end(); ++iter ) {
	TString user =  iter->first;
	TGraph* g    = iter->second;
	findMinMaxGraph(x1,x2,y1,y2,g);
	if(y2<joblimit){
	    mymapSmall    [user] = g;
	    mymapSmallPend[user] = mymapPend[user];
	} else {
	    mymapBig    [user] = g;
	    mymapBigPend[user] = mymapPend[user];
	}
    }
}


void draw(map<TString,TGraph*>& mymap)
{
    for(map< TString, TGraph*>::iterator iter = mymap.begin(); iter != mymap.end(); ++iter ) {
	TGraph* g  = iter->second;
	g->Draw("L");
        g->Draw("Psame");
    }
}

void  addUserPoint(TString user,
		   Int_t njobs,time_t timeStamp,time_t toffset,
		   map<TString,TGraph*>& mymap)
{
    if(mymap.find(user) == mymap.end()){ // new user
	TGraph* g = new TGraph;
	g->SetName(user.Data());
	g->SetPoint(0,timeStamp-toffset,njobs);
	g->SetLineColor(colors[(mymap.size()+1)%nColors]);
	g->SetMarkerColor(colors[(mymap.size()+1)%nColors]);
	g->SetMarkerStyle(markers[(mymap.size()+1)%nMarkers]);
	g->SetLineStyle(styles[(mymap.size()+1)%nStyles]);
	mymap[user] = g;

    } else {
	TGraph* g = mymap[user];
	g->SetPoint(g->GetN(),timeStamp-toffset,njobs);
    }
}

void plotHist(TString userDat="/misc/kempter/GE_mon_log_dev/logUser_2014-01-20.dat",
	      TString groupDat="/misc/kempter/GE_mon_log_dev/logGroup_2014-01-20.dat",
	      TString outdir="/misc/kempter/GE_mon_log_dev",
	      TString stampDate="",
	      TString stampTime="",
              UInt_t range1=0,
              UInt_t range2=0
	     )
{

    gStyle->SetOptStat(0);
    gStyle->SetPadTopMargin(0.1);
    gStyle->SetPadBottomMargin(0.1);
    gStyle->SetPadRightMargin(0.11);
    gStyle->SetPadLeftMargin(0.15);

    time_t tnow=0;
    time(&tnow);
    struct tm* t_now = localtime(&tnow);
    TDatime today(t_now->tm_year,t_now->tm_mon,t_now->tm_mday,0,00,00);   // year 95 <= xx <= 158 (158 being 2058).
    gStyle->SetTimeOffset(today.Convert());
    time_t toffset = today.Convert();
    if(range1 > toffset && range1 < tnow) {;/*toffset = range1;*/ }
    else                                  {range1 = tnow-toffset;}

    if(range2 > toffset && range2 < tnow) {;/*toffset = range2;*/ }
    else                                  {range2 = tnow;}

    time_t timeStamp=0;
    TString user;
    TString group;
    Int_t njobs=0;
    Int_t njobsTotal=0;
    TString groupname;
    TString projectname;
    map<TString,TGraph* > mUser;
    map<TString,TGraph* > mGroup;
    map<TString,TGraph* > mUserBig;
    map<TString,TGraph* > mUserSmall;
    map<TString,TGraph* > mUserPend;
    map<TString,TGraph* > mUserBigPend;
    map<TString,TGraph* > mUserSmallPend;
    map<TString,TGraph* > mGroupPend;


    Int_t jobLimit=1100;


    //-----------------------------------------------------------
    if(gSystem->AccessPathName(userDat.Data())==0){
	ifstream in;
	in.open(userDat.Data());
        Char_t line[5000];
        TString l="";

        
	while(in.good() && !in.eof())
	{
	    in.getline(line,5000);
	    l=line;
	    TString word;
	    if(l != "" ){
		timeStamp=0;


		//-----------------------------------------------------------
		// tokenize the line: format "timestamp user1:njobs:njobstotal user2:njobs:njobstotal user3:njobs:njobstotal"
		TObjArray* a = l.Tokenize(" ");
		for(Int_t i=0; i<a->GetEntries();i++){
		    word = ((TObjString*)a->At(i))->GetString();
		    if(i==0) {
			timeStamp = word.Atoi();
			continue;
		    }
                    if(timeStamp<range1) continue;
		    //-----------------------------------------------------------
                    // get user and number of jobs
		    TObjArray* a2 = word.Tokenize(":");
		    if(a2->GetEntries()==5){
			user = ((TObjString*)a2->At(0))->GetString();
			njobs= ((TObjString*)a2->At(1))->GetString().Atoi();
                        njobsTotal= ((TObjString*)a2->At(2))->GetString().Atoi();
                        njobsTotal= ((TObjString*)a2->At(2))->GetString().Atoi();
			groupname= ((TObjString*)a2->At(3))->GetString();
                        projectname= ((TObjString*)a2->At(4))->GetString();

			addUserPoint(user,njobs,timeStamp,toffset,mUser);
                        addUserPoint(user,njobsTotal-njobs,timeStamp,toffset,mUserPend);

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
 

    //-----------------------------------------------------------
    if(gSystem->AccessPathName(groupDat.Data())==0){
	

	ifstream in;
	in.open(groupDat.Data());
	Char_t line[5000];
	TString l="";


	while(in.good() && !in.eof())
	{
	    in.getline(line,5000);
	    l=line;
	    TString word;
	    if(l != "" ){
		timeStamp=0;


		//-----------------------------------------------------------
		// tokenize the line: format "timestamp user1:njobs:njobstotal:group:project user2:njobs:njobstotal:group:project user3:njobs:njobstotal:group:project"
		TObjArray* a = l.Tokenize(" ");
		for(Int_t i=0; i<a->GetEntries();i++){
		    word = ((TObjString*)a->At(i))->GetString();
		    if(i==0) {
			timeStamp = word.Atoi();
			continue;
		    }
		    if(timeStamp<range1) continue;

		    //-----------------------------------------------------------
		    // get user and number of jobs
		    TObjArray* a2 = word.Tokenize(":");
		    if(a2->GetEntries()==3){
			user = ((TObjString*)a2->At(0))->GetString();
			njobs= ((TObjString*)a2->At(1))->GetString().Atoi();
                        njobsTotal= ((TObjString*)a2->At(2))->GetString().Atoi();

                        //if(njobs == 0) continue;
                        addUserPoint(user,njobs,timeStamp,toffset,mGroup);
                        addUserPoint(user,njobsTotal-njobs,timeStamp,toffset,mGroupPend);

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


    }   else {
	cout<<"groupfile not found : "<<groupDat.Data()<<endl;
    }
    //-----------------------------------------------------------

    divideUser(jobLimit,mUser,mUserPend,
	       mUserBig,mUserSmall,
	       mUserBigPend,mUserSmallPend);

    //-----------------------------------------------------------
    TCanvas* cuser = new TCanvas("cuser","user stat",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    Double_t xmax = 0;
    Double_t xmin = 0;
    Double_t ymax = 0;
    Double_t ymin = 0;
    findMinMax(xmin,xmax,ymin,ymax,mUser);




    TH2F* huser = new TH2F("huser",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huser->GetXaxis()->SetTimeDisplay(1);
    huser->GetXaxis()->SetTimeFormat("%H:%M");
    huser->GetXaxis()->SetLabelSize(0.02);
    huser->GetXaxis()->LabelsOption("v");
    huser->GetXaxis()->SetTickLength(0);
    huser->SetYTitle("running jobs");
    huser->GetYaxis()->SetTitleOffset(2);

    huser->Draw();

    TLegend* luser = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luser,mUser);
    draw(mUser);
    luser->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cuserSmall = new TCanvas("cuserSmall","user stat Small",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mUserSmall);

    TH2F* huserSmall = new TH2F("huserSmall",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huserSmall->GetXaxis()->SetTimeDisplay(1);
    huserSmall->GetXaxis()->SetTimeFormat("%H:%M");
    huserSmall->GetXaxis()->SetLabelSize(0.02);
    huserSmall->GetXaxis()->LabelsOption("v");
    huserSmall->GetXaxis()->SetTickLength(0);
    huserSmall->SetYTitle("running jobs");
    huserSmall->GetYaxis()->SetTitleOffset(2);

    huserSmall->Draw();

    TLegend* luserSmall = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luserSmall,mUserSmall);
    draw(mUserSmall);
    luserSmall->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cuserBig = new TCanvas("cuserBig","user stat Big",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mUserBig);

    TH2F* huserBig = new TH2F("huserBig",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huserBig->GetXaxis()->SetTimeDisplay(1);
    huserBig->GetXaxis()->SetTimeFormat("%H:%M");
    huserBig->GetXaxis()->SetLabelSize(0.02);
    huserBig->GetXaxis()->LabelsOption("v");
    huserBig->GetXaxis()->SetTickLength(0);
    huserBig->SetYTitle("running jobs");
    huserBig->GetYaxis()->SetTitleOffset(2);

    huserBig->Draw();

    TLegend* luserBig = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luserBig,mUserBig);
    draw(mUserBig);
    luserBig->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cuserPend = new TCanvas("cuserPend","user stat pend",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mUserPend);

    TH2F* huserPend = new TH2F("huserPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huserPend->GetXaxis()->SetTimeDisplay(1);
    huserPend->GetXaxis()->SetTimeFormat("%H:%M");
    huserPend->GetXaxis()->SetLabelSize(0.02);
    huserPend->GetXaxis()->LabelsOption("v");
    huserPend->GetXaxis()->SetTickLength(0);
    huserPend->SetYTitle("pending jobs");
    huserPend->GetYaxis()->SetTitleOffset(2);

    huserPend->Draw();

    TLegend* luserPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luserPend,mUserPend);
    draw(mUserPend);
    luserPend->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cuserBigPend = new TCanvas("cuserBigPend","user stat Big pend",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mUserBigPend);

    TH2F* huserBigPend = new TH2F("huserBigPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huserBigPend->GetXaxis()->SetTimeDisplay(1);
    huserBigPend->GetXaxis()->SetTimeFormat("%H:%M");
    huserBigPend->GetXaxis()->SetLabelSize(0.02);
    huserBigPend->GetXaxis()->LabelsOption("v");
    huserBigPend->GetXaxis()->SetTickLength(0);
    huserBigPend->SetYTitle("pending jobs");
    huserBigPend->GetYaxis()->SetTitleOffset(2);

    huserBigPend->Draw();

    TLegend* luserBigPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luserBigPend,mUserBigPend);
    draw(mUserBigPend);
    luserBigPend->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cuserSmallPend = new TCanvas("cuserSmallPend","user stat Small pend",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mUserSmallPend);

    TH2F* huserSmallPend = new TH2F("huserSmallPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    huserSmallPend->GetXaxis()->SetTimeDisplay(1);
    huserSmallPend->GetXaxis()->SetTimeFormat("%H:%M");
    huserSmallPend->GetXaxis()->SetLabelSize(0.02);
    huserSmallPend->GetXaxis()->LabelsOption("v");
    huserSmallPend->GetXaxis()->SetTickLength(0);
    huserSmallPend->SetYTitle("pending jobs");
    huserSmallPend->GetYaxis()->SetTitleOffset(2);

    huserSmallPend->Draw();

    TLegend* luserSmallPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(luserSmallPend,mUserSmallPend);
    draw(mUserSmallPend);
    luserSmallPend->Draw();
    //-----------------------------------------------------------


    //-----------------------------------------------------------
    TCanvas* cgroup = new TCanvas("cgroup","group stat",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mGroup);

    TH2F* hgroup = new TH2F("hgroup",Form("group stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    hgroup->GetXaxis()->SetTimeDisplay(1);
    hgroup->GetXaxis()->SetTimeFormat("%H:%M");
    hgroup->GetXaxis()->SetLabelSize(0.02);
    hgroup->GetXaxis()->LabelsOption("v");
    hgroup->GetXaxis()->SetTickLength(0);
    hgroup->SetYTitle("running jobs");
    hgroup->GetYaxis()->SetTitleOffset(2);

    hgroup->Draw();
    TLegend* lgroup = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(lgroup,mGroup);
    draw(mGroup);
    lgroup->Draw();
    //-----------------------------------------------------------

    //-----------------------------------------------------------
    TCanvas* cgroupPend = new TCanvas("cgroupPend","group stat pend",1000,800);
    gPad->SetGridy();
    gPad->SetGridx();

    findMinMax(xmin,xmax,ymin,ymax,mGroupPend);

    TH2F* hgroupPend = new TH2F("hgroupPend",Form("group stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    hgroupPend->GetXaxis()->SetTimeDisplay(1);
    hgroupPend->GetXaxis()->SetTimeFormat("%H:%M");
    hgroupPend->GetXaxis()->SetLabelSize(0.02);
    hgroupPend->GetXaxis()->LabelsOption("v");
    hgroupPend->GetXaxis()->SetTickLength(0);
    hgroupPend->SetYTitle("running jobs");
    hgroupPend->GetYaxis()->SetTitleOffset(2);

    hgroupPend->Draw();
    TLegend* lgroupPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(lgroupPend,mGroupPend);
    draw(mGroupPend);
    lgroupPend->Draw();
    //-----------------------------------------------------------




    //-----------------------------------------------------------
    // saving pictures
    Int_t zoom=2;
    TString pictureName=gSystem->BaseName(userDat.Data());
    pictureName.ReplaceAll(".dat","");
    cuser          ->SaveAs(Form("%s/%s_run.png"       ,outdir.Data(),pictureName.Data()));
    cuserSmall     ->SaveAs(Form("%s/%s_Small_run.png" ,outdir.Data(),pictureName.Data()));
    cuserBig       ->SaveAs(Form("%s/%s_Big_run.png"   ,outdir.Data(),pictureName.Data()));
    cuserPend      ->SaveAs(Form("%s/%s_pend.png"      ,outdir.Data(),pictureName.Data()));
    cuserSmallPend ->SaveAs(Form("%s/%s_Small_pend.png",outdir.Data(),pictureName.Data()));
    cuserBigPend   ->SaveAs(Form("%s/%s_Big_pend.png"  ,outdir.Data(),pictureName.Data()));


    huser->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    huserSmall->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    huserBig->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    huserPend->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    huserSmallPend->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    huserBigPend->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);

    cuser          ->SaveAs(Form("%s/%s_run_%ih.png"       ,outdir.Data(),pictureName.Data(),zoom));
    cuserSmall     ->SaveAs(Form("%s/%s_Small_run_%ih.png" ,outdir.Data(),pictureName.Data(),zoom));
    cuserBig       ->SaveAs(Form("%s/%s_Big_run_%ih.png"   ,outdir.Data(),pictureName.Data(),zoom));
    cuserPend      ->SaveAs(Form("%s/%s_pend_%ih.png"      ,outdir.Data(),pictureName.Data(),zoom));
    cuserSmallPend ->SaveAs(Form("%s/%s_Small_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));
    cuserBigPend   ->SaveAs(Form("%s/%s_Big_pend_%ih.png"  ,outdir.Data(),pictureName.Data(),zoom));

    pictureName=gSystem->BaseName(groupDat.Data());
    pictureName.ReplaceAll(".dat","");
    cgroup    ->SaveAs(Form("%s/%s_run.png" ,outdir.Data(),pictureName.Data()));
    cgroupPend->SaveAs(Form("%s/%s_pend.png",outdir.Data(),pictureName.Data()));

    hgroup    ->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);
    hgroupPend->GetXaxis()->SetRangeUser(tnow-toffset-3600*zoom,tnow-toffset);

    cgroup    ->SaveAs(Form("%s/%s_run_%ih.png" ,outdir.Data(),pictureName.Data(),zoom));
    cgroupPend->SaveAs(Form("%s/%s_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));


    //-----------------------------------------------------------



}
