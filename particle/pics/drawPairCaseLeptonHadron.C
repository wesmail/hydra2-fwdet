
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


void drawPairCaseLeptonHadron(){

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
	Particle::kPairCase26,
	Particle::kPairCase27,
	Particle::kPairCase28,
	Particle::kPairCase29,
	Particle::kPairCase30,
	Particle::kPairCase31,
	Particle::kPairCase32
    };



    Int_t cheight = 100;

    TCanvas* c = new TCanvas("pairCaseMixed","pairCase Mixed",665,cheight);

    c->SetTopMargin(0.);
    c->SetBottomMargin(0.9);
    c->SetLeftMargin(0.);
    c->SetRightMargin(0.);

    UInt_t bins1 = sizeof(cases1)/sizeof(UInt_t);

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
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases1[i],Form("Case%i",i+1+25),cuts);
    }

    c->SaveAs("lepton_hadron_pair_cases.png");


}
