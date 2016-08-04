#define readBackTree_cxx
#include "readBackTree.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

void readBackTree::Loop(
			TString outdir,
			TString range1S,
			TString range2S)
{
//   In a ROOT session, you can do:
//      Root > .L readBackTree.C
//      Root > readBackTree t
//      Root > t.GetEntry(12); // Fill t data members with entry number 12
//      Root > t.Show();       // Show values of entry 12
//      Root > t.Show(16);     // Read and show values of entry 16
//      Root > t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch
   if (fChain == 0) return;


   gStyle->SetOptStat(0);
   gStyle->SetPadTopMargin(0.1);
   gStyle->SetPadBottomMargin(0.1);
   gStyle->SetPadRightMargin(0.11);
   gStyle->SetPadLeftMargin(0.15);

   Int_t zoom=2;  // hours
   Int_t jobLimit=1100;
   Bool_t doLogY=kFALSE;
   Bool_t savePics=kFALSE;



   Long64_t nentries = fChain->GetEntriesFast();

   LoadTree(0);
   fChain->GetEntry(0);
   time_t tmin=timeVal;

   LoadTree(nentries-1);
   fChain->GetEntry(nentries-1);

   time_t tmax=timeVal;

   ULong64_t range1=0;
   ULong64_t range2=0;

   TDatime damin;
   TDatime damax;

   struct tm t_min;
   struct tm t_max;

   if(range1S.CompareTo("")!=0) {

       //yyyy-mm-dd hh:mm:ss
       damin.Set(range1S.Data());
       cout<<"min Range :"<<damin.AsString()<<endl;
       range1 = damin.Convert();
       localtime_r((time_t*)(&range1),&t_min);
       cout<<"start     :"<<asctime(&t_min)<<endl;
   }


   if(range2S.CompareTo("")!=0) {

       //yyyy-mm-dd hh:mm:ss
       damax.Set(range2S.Data());
       cout<<"max Range :"<<damax.AsString()<<endl;
       range2 = damax.Convert();
       localtime_r((time_t*)(&range2),&t_max);
       cout<<"start     :"<<asctime(&t_max)<<endl;
   }



   if(range1 < tmin || range1 > tmax || range1>range2) { range1=tmin; }
   if(range2 > tmax || range2 < tmin || range1>range2) { range2=tmax; }


   localtime_r((time_t*)(&range1),&t_min);
   cout<<"final start "<<asctime(&t_min)<<endl;

   localtime_r((time_t*)(&range2),&t_max);
   cout<<"final stop "<<asctime(&t_max)<<endl;

   time_t toffset = range1;
   gStyle->SetTimeOffset(range1);



   TString stampDate;
   TString stampTime;


   time_t timeStamp=0;
   map<TString,TGraph* > mUser;
   map<TString,TGraph* > mUserProject;
   map<TString,TGraph* > mUserProjectPend;
   map<TString,TGraph* > mUserNoProject;
   map<TString,TGraph* > mUserNoProjectPend;
   map<TString,TGraph* > mGroup;
   map<TString,TGraph* > mUserBig;
   map<TString,TGraph* > mUserSmall;
   map<TString,TGraph* > mUserPend;
   map<TString,TGraph* > mUserBigPend;
   map<TString,TGraph* > mUserSmallPend;
   map<TString,TGraph* > mGroupPend;


   Long64_t nbytes = 0, nb = 0;
   for (Long64_t jentry=0; jentry<nentries;jentry++) {
      Long64_t ientry = LoadTree(jentry);
      if (ientry < 0) break;
      nb = fChain->GetEntry(jentry);   nbytes += nb;
      // if (Cut(ientry) < 0) continue;

        if(timeVal>range2 || timeVal < range1) continue;
	if(project->CompareTo("sum")!=0){
	    addUserPoint(*user,njobsRun,timeVal,toffset,mUser);
	    addUserPoint(*user,njobsPend,timeVal,toffset,mUserPend);
	    if(project->CompareTo("no_project")!=0){
		addUserPoint(*user,njobsRun,timeVal,toffset,mUserProject);
		addUserPoint(*user,njobsPend,timeVal,toffset,mUserProjectPend);
	    } else {
		addUserPoint(*user,njobsRun,timeVal,toffset,mUserNoProject);
		addUserPoint(*user,njobsPend,timeVal,toffset,mUserNoProjectPend);
	    }

	} else {
	    addUserPoint(*user,njobsRun,timeVal,toffset,mGroup);
	    addUserPoint(*user,njobsPend,timeVal,toffset,mGroupPend);
	}
   }

   divideUser(jobLimit,mUser,mUserPend,
	      mUserBig,mUserSmall,
	      mUserBigPend,mUserSmallPend);

   //TString timeFormat="%y:%m:%d";
   //TString timeFormat="#splitline{%m:%d}{%H:%M}";
   TString timeFormat="#splitline{%d:%b}{%H:%M}";

   //-----------------------------------------------------------
   TCanvas* cuser = new TCanvas("cuser","user stat",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();
   Double_t xmax = 0;
   Double_t xmin = 0;
   Double_t ymax = 0;
   Double_t ymin = 0;
   findMinMax(xmin,xmax,ymin,ymax,mUser);



   TH2F* huser = new TH2F("huser",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huser->GetXaxis()->SetTimeDisplay(1);
   huser->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huser->GetXaxis()->SetLabelSize(0.02);
   huser->GetXaxis()->LabelsOption("v");
   huser->GetXaxis()->SetTickLength(0);
   huser->SetYTitle("running jobs");
   huser->GetYaxis()->SetTitleOffset(2);
   huser->GetXaxis()->SetLabelOffset(0.02);

   huser->Draw();

   TLegend* luser = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(luser,mUser);
   draw(mUser);
   luser->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cuserproject = new TCanvas("cuserproject","user project stat",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();
   findMinMax(xmin,xmax,ymin,ymax,mUser);

   TH2F* huserproject = new TH2F("huserproject",Form("user project stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserproject->GetXaxis()->SetTimeDisplay(1);
   huserproject->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserproject->GetXaxis()->SetLabelSize(0.02);
   huserproject->GetXaxis()->LabelsOption("v");
   huserproject->GetXaxis()->SetTickLength(0);
   huserproject->SetYTitle("running jobs");
   huserproject->GetYaxis()->SetTitleOffset(2);
   huserproject->GetXaxis()->SetLabelOffset(0.02);

   huserproject->Draw();

   TLegend* luserproject = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(luserproject,mUserProject);
   draw(mUserProject);
   luserproject->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cusernoproject = new TCanvas("cusernoproject","user no project stat",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();
   findMinMax(xmin,xmax,ymin,ymax,mUser);

   TH2F* husernoproject = new TH2F("husernoproject",Form("user no project stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   husernoproject->GetXaxis()->SetTimeDisplay(1);
   husernoproject->GetXaxis()->SetTimeFormat(timeFormat.Data());
   husernoproject->GetXaxis()->SetLabelSize(0.02);
   husernoproject->GetXaxis()->LabelsOption("v");
   husernoproject->GetXaxis()->SetTickLength(0);
   husernoproject->SetYTitle("running jobs");
   husernoproject->GetYaxis()->SetTitleOffset(2);
   husernoproject->GetXaxis()->SetLabelOffset(0.02);

   husernoproject->Draw();

   TLegend* lusernoproject = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(lusernoproject,mUserNoProject);
   draw(mUserNoProject);
   lusernoproject->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cuserSmall = new TCanvas("cuserSmall","user stat Small",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserSmall);

   TH2F* huserSmall = new TH2F("huserSmall",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserSmall->GetXaxis()->SetTimeDisplay(1);
   huserSmall->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserSmall->GetXaxis()->SetLabelSize(0.02);
   huserSmall->GetXaxis()->LabelsOption("v");
   huserSmall->GetXaxis()->SetTickLength(0);
   huserSmall->SetYTitle("running jobs");
   huserSmall->GetYaxis()->SetTitleOffset(2);
   huserSmall->GetXaxis()->SetLabelOffset(0.02);

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
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserBig);

   TH2F* huserBig = new TH2F("huserBig",Form("user stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserBig->GetXaxis()->SetTimeDisplay(1);
   huserBig->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserBig->GetXaxis()->SetLabelSize(0.02);
   huserBig->GetXaxis()->LabelsOption("v");
   huserBig->GetXaxis()->SetTickLength(0);
   huserBig->SetYTitle("running jobs");
   huserBig->GetYaxis()->SetTitleOffset(2);
   huserBig->GetXaxis()->SetLabelOffset(0.02);

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
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserPend);

   TH2F* huserPend = new TH2F("huserPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserPend->GetXaxis()->SetTimeDisplay(1);
   huserPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserPend->GetXaxis()->SetLabelSize(0.02);
   huserPend->GetXaxis()->LabelsOption("v");
   huserPend->GetXaxis()->SetTickLength(0);
   huserPend->SetYTitle("pending jobs");
   huserPend->GetYaxis()->SetTitleOffset(2);
   huserPend->GetXaxis()->SetLabelOffset(0.02);

   huserPend->Draw();

   TLegend* luserPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(luserPend,mUserPend);
   draw(mUserPend);
   luserPend->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cuserprojectPend = new TCanvas("cuserprojectPend","user project stat pend",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserPend);

   TH2F* huserprojectPend = new TH2F("huserprojectPend",Form("user project stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserprojectPend->GetXaxis()->SetTimeDisplay(1);
   huserprojectPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserprojectPend->GetXaxis()->SetLabelSize(0.02);
   huserprojectPend->GetXaxis()->LabelsOption("v");
   huserprojectPend->GetXaxis()->SetTickLength(0);
   huserprojectPend->SetYTitle("pending jobs");
   huserprojectPend->GetYaxis()->SetTitleOffset(2);
   huserprojectPend->GetXaxis()->SetLabelOffset(0.02);

   huserprojectPend->Draw();

   TLegend* luserprojectPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(luserprojectPend,mUserProjectPend);
   draw(mUserProjectPend);
   luserprojectPend->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cusernoprojectPend = new TCanvas("cusernoprojectPend","user no project stat pend",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserPend);

   TH2F* husernoprojectPend = new TH2F("huserprojectPend",Form("user no project stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   husernoprojectPend->GetXaxis()->SetTimeDisplay(1);
   husernoprojectPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
   husernoprojectPend->GetXaxis()->SetLabelSize(0.02);
   husernoprojectPend->GetXaxis()->LabelsOption("v");
   husernoprojectPend->GetXaxis()->SetTickLength(0);
   husernoprojectPend->SetYTitle("pending jobs");
   husernoprojectPend->GetYaxis()->SetTitleOffset(2);
   husernoprojectPend->GetXaxis()->SetLabelOffset(0.02);

   husernoprojectPend->Draw();

   TLegend* lusernoprojectPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
   addToLegend(lusernoprojectPend,mUserNoProjectPend);
   draw(mUserNoProjectPend);
   lusernoprojectPend->Draw();
   //-----------------------------------------------------------

   //-----------------------------------------------------------
   TCanvas* cuserBigPend = new TCanvas("cuserBigPend","user stat Big pend",1000,800);
   gPad->SetGridy();
   gPad->SetGridx();
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserBigPend);

   TH2F* huserBigPend = new TH2F("huserBigPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserBigPend->GetXaxis()->SetTimeDisplay(1);
   huserBigPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserBigPend->GetXaxis()->SetLabelSize(0.02);
   huserBigPend->GetXaxis()->LabelsOption("v");
   huserBigPend->GetXaxis()->SetTickLength(0);
   huserBigPend->SetYTitle("pending jobs");
   huserBigPend->GetYaxis()->SetTitleOffset(2);
   huserBigPend->GetXaxis()->SetLabelOffset(0.02);

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
   if(doLogY) gPad->SetLogy();

   findMinMax(xmin,xmax,ymin,ymax,mUserSmallPend);

   TH2F* huserSmallPend = new TH2F("huserSmallPend",Form("user stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
   huserSmallPend->GetXaxis()->SetTimeDisplay(1);
   huserSmallPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
   huserSmallPend->GetXaxis()->SetLabelSize(0.02);
   huserSmallPend->GetXaxis()->LabelsOption("v");
   huserSmallPend->GetXaxis()->SetTickLength(0);
   huserSmallPend->SetYTitle("pending jobs");
   huserSmallPend->GetYaxis()->SetTitleOffset(2);
   huserSmallPend->GetXaxis()->SetLabelOffset(0.02);

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
    if(doLogY) gPad->SetLogy();

    findMinMax(xmin,xmax,ymin,ymax,mGroup);

    TH2F* hgroup = new TH2F("hgroup",Form("group stat run : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    hgroup->GetXaxis()->SetTimeDisplay(1);
    hgroup->GetXaxis()->SetTimeFormat(timeFormat.Data());
    hgroup->GetXaxis()->SetLabelSize(0.02);
    hgroup->GetXaxis()->LabelsOption("v");
    hgroup->GetXaxis()->SetTickLength(0);
    hgroup->SetYTitle("running jobs");
    hgroup->GetYaxis()->SetTitleOffset(2);
    hgroup->GetXaxis()->SetLabelOffset(0.02);

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
    if(doLogY) gPad->SetLogy();

    findMinMax(xmin,xmax,ymin,ymax,mGroupPend);

    TH2F* hgroupPend = new TH2F("hgroupPend",Form("group stat pend : %s %s",stampDate.Data(),stampTime.Data()),1000,xmin,xmax,10,ymin == 0 ? 0 : ymin*.9,ymax*1.1);
    hgroupPend->GetXaxis()->SetTimeDisplay(1);
    hgroupPend->GetXaxis()->SetTimeFormat(timeFormat.Data());
    hgroupPend->GetXaxis()->SetLabelSize(0.02);
    hgroupPend->GetXaxis()->LabelsOption("v");
    hgroupPend->GetXaxis()->SetTickLength(0);
    hgroupPend->SetYTitle("running jobs");
    hgroupPend->GetYaxis()->SetTitleOffset(2);
    hgroupPend->GetXaxis()->SetLabelOffset(0.02);

    hgroupPend->Draw();
    TLegend* lgroupPend = new TLegend(0.9,0.9,0.995,0.1,"","brNDC");
    addToLegend(lgroupPend,mGroupPend);
    draw(mGroupPend);
    lgroupPend->Draw();
    //-----------------------------------------------------------



   if(savePics){
       //-----------------------------------------------------------
       // saving pictures
       TString pictureName=gSystem->BaseName(userDat.Data());
       pictureName.ReplaceAll(".dat","");
       cuser          ->SaveAs(Form("%s/%s_user_run.png"       ,outdir.Data(),pictureName.Data()));
       cuserproject   ->SaveAs(Form("%s/%s_user_project_run.png",outdir.Data(),pictureName.Data()));
       cusernoproject ->SaveAs(Form("%s/%s_user_no_project_run.png",outdir.Data(),pictureName.Data()));
       cuserSmall     ->SaveAs(Form("%s/%s_user_Small_run.png" ,outdir.Data(),pictureName.Data()));
       cuserBig       ->SaveAs(Form("%s/%s_user_Big_run.png"   ,outdir.Data(),pictureName.Data()));
       cuserPend      ->SaveAs(Form("%s/%s_user_pend.png"      ,outdir.Data(),pictureName.Data()));
       cuserprojectPend->SaveAs(Form("%s/%s_user_project_pend.png",outdir.Data(),pictureName.Data()));
       cusernoprojectPend->SaveAs(Form("%s/%s_user_no_project_pend.png",outdir.Data(),pictureName.Data()));
       cuserSmallPend ->SaveAs(Form("%s/%s_user_Small_pend.png",outdir.Data(),pictureName.Data()));
       cuserBigPend   ->SaveAs(Form("%s/%s_user_Big_pend.png"  ,outdir.Data(),pictureName.Data()));


       huser->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserproject->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       husernoproject->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserSmall->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserBig->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserprojectPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       husernoprojectPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserSmallPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       huserBigPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);

       cuser          ->SaveAs(Form("%s/%s_user_run_%ih.png"       ,outdir.Data(),pictureName.Data(),zoom));
       cuserproject   ->SaveAs(Form("%s/%s_user_project_run_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       cusernoproject ->SaveAs(Form("%s/%s_user_no_project_run_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       cuserSmall     ->SaveAs(Form("%s/%s_user_Small_run_%ih.png" ,outdir.Data(),pictureName.Data(),zoom));
       cuserBig       ->SaveAs(Form("%s/%s_user_Big_run_%ih.png"   ,outdir.Data(),pictureName.Data(),zoom));
       cuserPend      ->SaveAs(Form("%s/%s_user_pend_%ih.png"      ,outdir.Data(),pictureName.Data(),zoom));
       cuserprojectPend->SaveAs(Form("%s/%s_user_project_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       cusernoprojectPend->SaveAs(Form("%s/%s_user_no_project_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       cuserSmallPend ->SaveAs(Form("%s/%s_user_Small_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       cuserBigPend   ->SaveAs(Form("%s/%s_user_Big_pend_%ih.png"  ,outdir.Data(),pictureName.Data(),zoom));


       pictureName=gSystem->BaseName(userDat.Data());
       pictureName.ReplaceAll(".dat","");
       cgroup    ->SaveAs(Form("%s/%s_group_run.png" ,outdir.Data(),pictureName.Data()));
       cgroupPend->SaveAs(Form("%s/%s_group_pend.png",outdir.Data(),pictureName.Data()));

       hgroup    ->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);
       hgroupPend->GetXaxis()->SetRangeUser(range2-toffset-3600*zoom,range2-toffset);

       cgroup    ->SaveAs(Form("%s/%s_group_run_%ih.png" ,outdir.Data(),pictureName.Data(),zoom));
       cgroupPend->SaveAs(Form("%s/%s_group_pend_%ih.png",outdir.Data(),pictureName.Data(),zoom));
       //-----------------------------------------------------------
   }


}
