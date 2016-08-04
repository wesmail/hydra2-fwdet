
#include "hparticledef.h"
#include "hparticlepairdraw.h"


#include "TH1F.h"
#include "TCanvas.h"
#include "TEllipse.h"
#include "TLine.h"
#include "TStyle.h"
#include "TLatex.h"
#include "TObjArray.h"
#include "TObjString.h"


#include "TRandom.h"

#include <vector>
#include <iostream>
using namespace std;

using namespace Particle;


void drawPairCaseLepton(){

    gStyle->SetOptStat(0);

    Double_t offsetX  =  0.25;
    Double_t offsetY  = -360;
    Double_t scaleX   =  1;
    Double_t scaleY   =  1;


    Float_t ftextSizeScale = 10;

    HParticlePairDraw pairdraw;
    pairdraw.ftextSizeScale = ftextSizeScale;
    HParticleDraw particledraw;

    UInt_t cases1[] = {
	Particle::kPairCase1,
	Particle::kPairCase2,
	Particle::kPairCase3,
	Particle::kPairCase4,
	Particle::kPairCase5,
	Particle::kPairCase6,
	Particle::kPairCase7,
	Particle::kPairCase8,
	Particle::kPairCase9,
	Particle::kPairCase10
    };


    UInt_t cases2[] = {
	Particle::kPairCase11,
	Particle::kPairCase12,
	Particle::kPairCase13,
	Particle::kPairCase14,
	Particle::kPairCase15
    };



    Int_t cheight = 100;

    TCanvas* c = new TCanvas("pairCase","pairCase",950,cheight);

    c->SetTopMargin(0.);
    c->SetBottomMargin(0.9);
    c->SetLeftMargin(0.);
    c->SetRightMargin(0.);

    UInt_t bins1 = sizeof(cases1)/sizeof(UInt_t);
    UInt_t bins2 = sizeof(cases2)/sizeof(UInt_t);

    TH1F* h = new TH1F("h","",bins1,0,bins1);



    h->GetXaxis()->SetLabelSize(0);
    h->GetYaxis()->SetRangeUser(0.,50.);
    h->GetYaxis()->SetAxisColor(kWhite);
    h->GetYaxis()->SetLabelColor(kWhite);
    h->GetYaxis()->SetTitleColor(kWhite);
    h->GetXaxis()->SetAxisColor(kWhite);
    h->SetLineColor(kWhite);
    c->SetFrameLineColor(kWhite);

    h->Draw();

    for(UInt_t i=0; i < bins1; i++){

	TString cuts = "";
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases1[i],Form("Case%i",i+1),cuts);
    }

    c->SaveAs("lepton_pair_cases_1.png");

    TCanvas* c2 = new TCanvas("pairCase2","pairCase2",950,cheight);
    c2->SetTopMargin(0.);
    c2->SetBottomMargin(0.9);
    c2->SetLeftMargin(0.);
    c2->SetRightMargin(0.);
    c2->SetFrameLineColor(kWhite);

    h->Draw();

    for(UInt_t i=0; i < bins2; i++){

	TString cuts = "";
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases2[i],Form("Case%i",i+1+10),cuts);
    }

    c2->SaveAs("lepton_pair_cases_2.png");


}
