
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


void drawPairCaseHadron(){

    gStyle->SetOptStat(0);

    Double_t offsetX  =  0.25;
    Double_t offsetY  = -360;
    Double_t scaleX   =  1;
    Double_t scaleY   =  1;


    Float_t ftextSizeScale = 10;

    HParticlePairDraw pairdraw;
    pairdraw.ftextSizeScale = ftextSizeScale;
    HParticleDraw particledraw;

    UInt_t cases[] = {
	Particle::kPairCase16,
	Particle::kPairCase17,
	Particle::kPairCase18,
	Particle::kPairCase19,
	Particle::kPairCase20,
	Particle::kPairCase21,
	Particle::kPairCase22,
	Particle::kPairCase23,
	Particle::kPairCase24,
	Particle::kPairCase25
    };



    Int_t cheight = 100;

    TCanvas* c = new TCanvas("pairCase","pairCase",950,cheight);

    c->SetTopMargin(0.);
    c->SetBottomMargin(0.9);
    c->SetLeftMargin(0.);
    c->SetRightMargin(0.);

    UInt_t bins = sizeof(cases)/sizeof(UInt_t);
    TH1F* h = new TH1F("h","",bins,0,bins);

    h->GetXaxis()->SetLabelSize(0);

    h->GetYaxis()->SetRangeUser(0.,50.);

    h->GetYaxis()->SetAxisColor(kWhite);
    h->GetYaxis()->SetLabelColor(kWhite);
    h->GetYaxis()->SetTitleColor(kWhite);
    h->GetXaxis()->SetAxisColor(kWhite);
    h->SetLineColor(kWhite);
    c->SetFrameLineColor(kWhite);

    h->Draw();

    for(UInt_t i=0; i < bins; i++){

	TString cuts = "";
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases[i],Form("Case%i",i+1+15),cuts);
    }

    c->SaveAs("hadron_pair_cases.png");
}
