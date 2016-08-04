//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Thu Jan 23 10:58:56 2014 by ROOT version 5.34/01
// from TTree T/userDat
// found on file: testTree.root
//////////////////////////////////////////////////////////

#ifndef readBackTree_h
#define readBackTree_h

#include "TROOT.h"
#include "TChain.h"
#include "TFile.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TCanvas.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TH2F.h"
#include "TDatime.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TLegend.h"
#include "TTree.h"
#include "TFile.h"
#include "TString.h"

#include <map>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

// Fixed size dimensions of array or collections stored in the TTree if any.
    static Int_t colors[]=
   {
       kRed  ,kMagenta  ,kBlue  ,kCyan  ,kGreen  ,kOrange,
       kRed-6,kMagenta-6,kBlue-6,kCyan-6,kGreen-6,kOrange-6,
       kRed-7,kMagenta-7,kBlue-7,kCyan-7,kGreen-7,kOrange-7,
       kRed-2,kMagenta-2,kBlue-2,kCyan-2,kGreen-2,kOrange-2,kBlack
   };
   static Int_t markers[]={
       20,21,22,23,24,25,26,27,28
   };

   static Int_t styles[]={
       1,2
   };

class readBackTree {
public :
   TTree          *fChain;   //!pointer to the analyzed TTree or TChain
   Int_t           fCurrent; //!current Tree number in a TChain

   // Declaration of leaf types
   TString         *user;
   TString         *group;
   TString         *project;
   ULong64_t       timeVal;
   Int_t           njobsRun;
   Int_t           njobsPend;

   // List of branches
   TBranch        *b_user;   //!
   TBranch        *b_group;   //!
   TBranch        *b_project;   //!
   TBranch        *b_timeVal;   //!
   TBranch        *b_njobsRun;   //!
   TBranch        *b_njobsPend;   //!

   readBackTree(TString filename="testTree.root");
   virtual ~readBackTree();
   virtual Int_t    Cut(Long64_t entry);
   virtual Int_t    GetEntry(Long64_t entry);
   virtual Long64_t LoadTree(Long64_t entry);
   virtual void     Init(TTree *tree);
   virtual void     Loop(TString outdir="/misc/kempter/GE_mon_log/test",TString range1="2014-01-01 00:00:00",TString range2="2020-01-01 00:00:00");
   virtual Bool_t   Notify();
   virtual void     Show(Long64_t entry = -1);


   Int_t nColors;
   Int_t nMarkers;
   Int_t nStyles;

   TString userDat;

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
	   g->SetPoint(0,timeStamp-toffset,njobs+0.1);
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










};

#endif

#ifdef readBackTree_cxx
readBackTree::readBackTree(TString filename) : fChain(0)
{
// if parameter tree is not specified (or zero), connect the file
// used to generate this class and read the Tree.
    if(gSystem->AccessPathName(filename.Data())!=0){
	cout<<"File : "<<filename.Data()<<" not found!";
	exit(1);
    }
    TFile* f = new TFile(filename.Data(),"READ");
    TTree* tree=0;
    f->GetObject("T",tree);
    if(!tree){
	cout<<"TTree T  not found in file : "<<filename.Data()<<"!";
	exit(1);
    }

   Init(tree);
   userDat  =filename;
   nColors  = sizeof(colors) /sizeof(Int_t);
   nMarkers = sizeof(markers)/sizeof(Int_t);
   nStyles  = sizeof(styles)/sizeof(Int_t);

}

readBackTree::~readBackTree()
{
   if (!fChain) return;
   delete fChain->GetCurrentFile();
}

Int_t readBackTree::GetEntry(Long64_t entry)
{
// Read contents of entry.
   if (!fChain) return 0;
   return fChain->GetEntry(entry);
}
Long64_t readBackTree::LoadTree(Long64_t entry)
{
// Set the environment to read one entry
   if (!fChain) return -5;
   Long64_t centry = fChain->LoadTree(entry);
   if (centry < 0) return centry;
   if (fChain->GetTreeNumber() != fCurrent) {
      fCurrent = fChain->GetTreeNumber();
      Notify();
   }
   return centry;
}

void readBackTree::Init(TTree *tree)
{
   // The Init() function is called when the selector needs to initialize
   // a new tree or chain. Typically here the branch addresses and branch
   // pointers of the tree will be set.
   // It is normally not necessary to make changes to the generated
   // code, but the routine can be extended by the user if needed.
   // Init() will be called many times when running on PROOF
   // (once per file to be processed).

   // Set object pointer
   user = 0;
   group = 0;
   project = 0;
   // Set branch addresses and branch pointers
   if (!tree) return;
   fChain = tree;
   fCurrent = -1;
   fChain->SetMakeClass(1);

   fChain->SetBranchAddress("user", &user, &b_user);
   fChain->SetBranchAddress("group", &group, &b_group);
   fChain->SetBranchAddress("project", &project, &b_project);
   fChain->SetBranchAddress("timeVal", &timeVal, &b_timeVal);
   fChain->SetBranchAddress("njobsRun", &njobsRun, &b_njobsRun);
   fChain->SetBranchAddress("njobsPend", &njobsPend, &b_njobsPend);
   Notify();
}

Bool_t readBackTree::Notify()
{
   // The Notify() function is called when a new file is opened. This
   // can be either for a new TTree in a TChain or when when a new TTree
   // is started when using PROOF. It is normally not necessary to make changes
   // to the generated code, but the routine can be extended by the
   // user if needed. The return value is currently not used.

   return kTRUE;
}

void readBackTree::Show(Long64_t entry)
{
// Print contents of entry.
// If entry is not specified, print current entry
   if (!fChain) return;
   fChain->Show(entry);
}
Int_t readBackTree::Cut(Long64_t entry)
{
// This function may be called from Loop.
// returns  1 if entry is accepted.
// returns -1 otherwise.
   return 1;
}
#endif // #ifdef readBackTree_cxx
