
#include "hparticlepairdraw.h"


#include "TCanvas.h"
#include "TPad.h"
#include "TStyle.h"
#include "TH1F.h"

ClassImp(HParticleDraw)
ClassImp(HParticlePairDraw)

#include <iostream>

using namespace Particle;
using namespace std;

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
//
//
// HParticlePairDraw
//
// Helper class to draw pair cases
//  (HParticleDraw works for single tracks using eClosePairSelect flags
//  for candidate 2 (kxxx2) (hparticledef.h).)
//
//    Example:
//
//
//    TCanvas* c = new TCanvas("pairCase","pairCase",1500,800);
//    c->SetGridx();
//    c->SetGridy();
//    c->SetBottomMargin(0.25); // 0.15
//
//
//    Double_t offsetX  =  0.25;
//    Double_t offsetY  = -8;
//    Double_t scaleX   =  1;
//    Double_t scaleY   =  1;
//
//    HParticlePairDraw pairdraw;
//
//    vector<UInt_t> cases;
//
//
//    cases.push_back(kPairCase1|kIsLepton2|kNoSamePolarity|kNoFittedInnerMDC2|kRK2);
//    cases.push_back(kPairCase2|kIsUsed2);
//    cases.push_back(kPairCase3|kNoRK2);
//    cases.push_back(kPairCase4);
//    cases.push_back(kPairCase5);
//    cases.push_back(kPairCase6);
//    cases.push_back(kPairCase7|kNoFittedInnerMDC2);
//    cases.push_back(kPairCase8|kNoFittedOuterMDC2);
//    cases.push_back(kPairCase9);
//    cases.push_back(kPairCase10);
//    cases.push_back(kPairCase11);
//    cases.push_back(kPairCase12);
//
//    cases.push_back(kPairCase13);
//    cases.push_back(kPairCase14);
//    cases.push_back(kPairCase15);
//    cases.push_back(kPairCase16);
//    cases.push_back(kPairCase17);
//    cases.push_back(kPairCase18);
//    cases.push_back(kPairCase19);
//    cases.push_back(kPairCase20);
//
//
//    UInt_t bins = cases.size();
//
//    TH1F* h = new TH1F("h","",bins,0,bins);
//    h->SetLineColor(kRed);
//    h->SetLineWidth(3);
//    h->SetYTitle("Fraction of pairs [%]");
//    h->GetXaxis()->SetNdivisions(100+bins);
//    h->GetXaxis()->SetLabelSize(0);
//
//    h->GetYaxis()->SetRangeUser(0.,50.);
//
//    for(Int_t i=0;i<100;i++){
//	h->Fill(gRandom->Rndm()*bins);
//    }
//    h->Draw();
//
//    for(UInt_t i=0; i < bins; i++){
//	pairdraw.drawPair(offsetX+i,offsetY,scaleX,scaleY,cases[i],Form("Case%i",i+1),"RK<100,oA<5,MQA<2,RQA<2,#beta>0.9");
//    }
////////////////////////////////////////////////////////////////////////////////

HParticleDraw::HParticleDraw()
{

    flineWidth = 2;

    frefColor          = kBlack;
    fusedColor         = kGreen;
    fleptonColor       = kRed;
    fmetaColor         = kBlue;
    frichColor         = kOrange;

    ffittedStyle       = 1;
    fnoFittedStyle     = 2;
    fuseRICH           = kTRUE;
    ftextSizeScale     = 1.;
    ftextSizeInfoScale = 1.;
}

HParticleDraw::~HParticleDraw()
{

}

void HParticleDraw::drawParticle(Double_t xoff,Double_t yoff,
				 Double_t scx,Double_t scy,
				 UInt_t flag,TString nameCase,TString cuts)
{

    if(HParticleTool::isPairsFlagsBit(flag,kNoUseRICH))  fuseRICH=kFALSE;
    else                                                 fuseRICH=kTRUE;

    Double_t offsetX  = xoff;
    Double_t offsetY  = yoff;

    Double_t scaleX       = 0.2 *scx;
    Double_t scaleY       = 0.25*scy;

    Double_t dYLeg    = 10   *scaleY;
    Double_t dXLeg    =  1   *scaleX;
    Double_t dXLeg2   =  0.25*scaleX;

    Double_t yLowLegLow = offsetY;
    Double_t yLowLegUp  = yLowLegLow+dYLeg;

    Double_t yUpLegLow  = yLowLegUp;
    Double_t yUpLegUp   = yUpLegLow+dYLeg;

    Double_t yRich      = yLowLegLow-(3*scaleY);
    Double_t yMeta      = yUpLegUp  +(3*scaleY);
    Double_t yText      = yMeta     +(3*scaleY);

    Double_t richR      = 0.5*scaleX;
    Double_t richR2     = 5;

    Double_t textsize     = ftextSizeScale*0.09*scaleX;
    Double_t textsizeInfo = ftextSizeInfoScale*0.08*scaleX;
    Double_t textInfoY    = yLowLegUp -(1*scaleY);


    TLine* l ;
    TEllipse* R;
    TLatex* paircase = new TLatex();
    paircase->SetTextSize(textsize);

    paircase->DrawLatex(offsetX,yText,Form("%s",nameCase.Data()));
    if(HParticleTool::isPairsFlagsBit(flag,kNoRK2|kOuterMDC2)){ // no RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + dXLeg +dXLeg2,textInfoY,"-RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kRK2)){ //  RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + dXLeg +dXLeg2,textInfoY,"+RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSamePosPolarity)){ // pos polarity
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX -dXLeg2 ,textInfoY,"+");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSameNegPolarity)){ // neg polarity
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX -dXLeg2,textInfoY,"-");
    }

    if(cuts != ""){

	TObjArray* ar = cuts.Tokenize(",");

	for(Int_t i = 0 ; i< ar->GetEntries(); i++){
	    paircase->SetTextSize(textsizeInfo*0.8);
	    paircase->DrawLatex(offsetX -dXLeg2 ,yRich-(3*scaleY) - ( (i+1)*5*scaleY),((TObjString*)ar->At(i))->GetString().Data());
	}

	if(ar) delete ar;
    }

    //----------------------------------------------------------------------------------
    // RICH
    if(fuseRICH){
	if(HParticleTool::isPairsFlagsBit(flag,kRICH2)){ // draw RICH middle
	    R = new TEllipse(offsetX+dXLeg+0.5*dXLeg2,yRich,richR, richR2*richR);  // Rich middle
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	}
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // META
    if(HParticleTool::isPairsFlagsBit(flag,kMETA2)){ // draw Meta middle
	R = new TEllipse(offsetX+dXLeg+0.5*dXLeg2,yMeta,richR, richR2*richR);  // Meta middle
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    }
    //----------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------
    // Inner MDC
    {
	l = new TLine(offsetX+dXLeg+0.5*dXLeg2,yLowLegLow ,offsetX+dXLeg+0.5*dXLeg2,yLowLegUp);   // middle lower leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // outer MDC
    if(HParticleTool::isPairsFlagsBit(flag,kOuterMDC2)) {   // outer MDC middle
	l = new TLine(offsetX+dXLeg+0.5*dXLeg2,yUpLegLow ,offsetX+dXLeg+0.5*dXLeg2,yUpLegUp);   // middle upper leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

}

void HParticleDraw::drawParticleNDC(Double_t xoffNDC,Double_t yoffNDC,
				    Double_t heightNDC,
				    UInt_t flag,TString nameCase,TString cuts)
{


    if(HParticleTool::isPairsFlagsBit(flag,kNoUseRICH))  fuseRICH=kFALSE;
    else                                                 fuseRICH=kTRUE;


    Double_t offsetX  = gPad->GetX1() + xoffNDC*(gPad->GetX2()-gPad->GetX1());
    Double_t offsetY  = gPad->GetY1() + yoffNDC*(gPad->GetY2()-gPad->GetY1());

    Double_t width    = (heightNDC/2.5) * (gPad->GetX2()-gPad->GetX1());
    Double_t height   = heightNDC*(gPad->GetY2()-gPad->GetY1());


    // 10 units in y
    //  9 units in x
    Double_t xu = width/4.;
    Double_t yu = height/10.;

    Double_t dYLeg      = 3.*yu;
    Double_t yLowLegLow = offsetY+2*yu;
    Double_t yLowLegUp  = yLowLegLow+dYLeg;

    Double_t yUpLegLow  = yLowLegUp;
    Double_t yUpLegUp   = yUpLegLow+dYLeg;

    Double_t yRich      = offsetY+1*yu;
    Double_t yMeta      = offsetY+9*yu;
    Double_t yText      = yMeta  + (1.5*yu);

    Double_t richR      = 1*xu ;
    Double_t richR2     = 1*yu ;

    Double_t textsize     = ftextSizeScale*0.09;
    Double_t textsizeInfo = ftextSizeInfoScale*0.09;
    Double_t textInfoY    = yLowLegUp ;

    TLine* l ;
    TEllipse* R;
    TLatex* paircase = new TLatex();
    paircase->SetTextSize(textsize);

    paircase->DrawLatex(offsetX+1*xu,yText,Form("%s",nameCase.Data()));
    if(HParticleTool::isPairsFlagsBit(flag,kNoRK2|kOuterMDC2)){ // no RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 2.5*xu,textInfoY,"-RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kRK2)){ //  RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 2.5*xu,textInfoY,"+RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSamePosPolarity)){ // pos polarity
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 1.0*xu ,textInfoY,"+");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSameNegPolarity)){ // neg polarity
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 1.0*xu,textInfoY,"-");
    }

    if(cuts != ""){

	TObjArray* ar = cuts.Tokenize(",");

	for(Int_t i = 0 ; i< ar->GetEntries(); i++){
	    paircase->SetTextSize(textsizeInfo*0.8);
	    paircase->DrawLatex(offsetX,offsetY-2*yu,((TObjString*)ar->At(i))->GetString().Data());
	}

	if(ar) delete ar;
    }

    //----------------------------------------------------------------------------------
    // RICH
    if(fuseRICH){
	if(HParticleTool::isPairsFlagsBit(flag,kRICH2)){ // draw RICH middle
	    R = new TEllipse(offsetX+2.0*xu,yRich,richR, richR2);  // Rich middle
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	}
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // META
    if(HParticleTool::isPairsFlagsBit(flag,kMETA2)){ // draw Meta middle
	R = new TEllipse(offsetX+2.0*xu,yMeta,richR, richR2);  // Meta middle
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    }
    //----------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------
    // Inner MDC
    {
	l = new TLine(offsetX+2.0*xu,yLowLegLow ,offsetX+2.0*xu,yLowLegUp);   // middle lower leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // outer MDC
    if(HParticleTool::isPairsFlagsBit(flag,kOuterMDC2) ||
       HParticleTool::isPairsFlagsBit(flag,kFittedOuterMDC2)
      ) {   // outer MDC middle
	l = new TLine(offsetX+2.0*xu,yUpLegLow ,offsetX+2.0*xu,yUpLegUp);   // middle upper leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

}

HParticlePairDraw::HParticlePairDraw()
{

    flineWidth = 2;

    frefColor          = kBlack;
    fusedColor         = kGreen;
    fleptonColor       = kRed;
    fmetaColor         = kBlue;
    frichColor         = kOrange;

    ffittedStyle       = 1;
    fnoFittedStyle     = 2;
    fuseRICH           = kTRUE;
    ftextSizeScale     = 1.;
    ftextSizeInfoScale = 1.;
}

HParticlePairDraw::~HParticlePairDraw()
{

}

void HParticlePairDraw::drawPair(Double_t xoff,Double_t yoff,
				 Double_t scx,Double_t scy,
				 UInt_t flag,TString nameCase,TString cuts)
{


    if(HParticleTool::isPairsFlagsBit(flag,kNoUseRICH))  fuseRICH=kFALSE;
    else                                                 fuseRICH=kTRUE;

    Double_t offsetX  = xoff;
    Double_t offsetY  = yoff;

    Double_t scaleX       = 0.2 *scx;
    Double_t scaleY       = 0.25*scy;

    Double_t dYLeg    = 10   *scaleY;
    Double_t dXLeg    =  1   *scaleX;
    Double_t dXLeg2   =  0.25*scaleX;

    Double_t yLowLegLow = offsetY;
    Double_t yLowLegUp  = yLowLegLow+dYLeg;

    Double_t yUpLegLow  = yLowLegUp;
    Double_t yUpLegUp   = yUpLegLow+dYLeg;

    Double_t yRich      = yLowLegLow-(3*scaleY);
    Double_t yMeta      = yUpLegUp  +(3*scaleY);
    Double_t yText      = yMeta     +(3*scaleY);

    Double_t richR      = 0.25*dXLeg ;
    Double_t richR2     = 0.2 *dYLeg ;

    Double_t textsize     = ftextSizeScale*0.09*scaleX;
    Double_t textsizeInfo = ftextSizeInfoScale*0.08*scaleX;
    Double_t textInfoY    = yLowLegUp -(1*scaleY);


    TLine* l ;
    TEllipse* R;
    TLatex* paircase = new TLatex();
    paircase->SetTextSize(textsize);

    paircase->DrawLatex(offsetX,yText,Form("%s",nameCase.Data()));
    if(HParticleTool::isPairsFlagsBit(flag,kNoRK2|kOuterMDC2)){ // no RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + dXLeg +dXLeg2,textInfoY,"-RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kRK2)){ //  RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + dXLeg +dXLeg2,textInfoY,"+RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSamePosPolarity)){ // pos pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX -dXLeg2 ,textInfoY,"++");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSameNegPolarity)){ // neg pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX -dXLeg2,textInfoY,"--");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kNoSamePolarity)){ // no same pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX -dXLeg2 ,textInfoY,"+-");
    }

    if(cuts != ""){

	TObjArray* ar = cuts.Tokenize(",");

	for(Int_t i = 0 ; i< ar->GetEntries(); i++){
	    paircase->SetTextSize(textsizeInfo*0.8);
	    paircase->DrawLatex(offsetX -dXLeg2 ,yRich-(3*scaleY) - ( (i+1)*5*scaleY),((TObjString*)ar->At(i))->GetString().Data());
	}

	if(ar) delete ar;
    }
    //----------------------------------------------------------------------------------
    // RICH

    if(fuseRICH){
	if(HParticleTool::isPairsFlagsBit(flag,kSameRICH)){ // draw RICH middle
	    R = new TEllipse(offsetX+dXLeg+0.5*dXLeg2,yRich,richR, richR2);  // Rich middle
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	} else if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kNoSameInnerMDC)){ // draw RICH left right
	    R = new TEllipse(offsetX               ,yRich,richR, richR2);  // Rich left
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	    if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kNoSameInnerMDC|kRICH2)){
		R = new TEllipse(offsetX+2*dXLeg+dXLeg2,yRich,richR, richR2);  // Rich right
		R->SetLineColor(frichColor);
		R->SetLineWidth(flineWidth);
		R->Draw();
	    }
	} else if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kSameInnerMDC)){ // draw RICH left right half
	    R = new TEllipse(offsetX+0.5*dXLeg              ,yRich,richR, richR2);  // Rich left
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	    if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kSameInnerMDC|kRICH2)){
		R = new TEllipse(offsetX+2*dXLeg+dXLeg2-0.5*dXLeg,yRich,richR, richR2);  // Rich right
		R->SetLineColor(frichColor);
		R->SetLineWidth(flineWidth);
		R->Draw();
	    }
	}
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // META
    if(HParticleTool::isPairsFlagsBit(flag,kSameMETA)){ // draw Meta middle
	R = new TEllipse(offsetX+dXLeg+0.5*dXLeg2,yMeta,richR, richR2);  // Meta middle
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameMETA|kNoSameOuterMDC)){ // draw Meta left right
	R = new TEllipse(offsetX               ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
	R = new TEllipse(offsetX+2*dXLeg+dXLeg2,yMeta,richR, richR2);  // Meta right
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameMETA|kSameOuterMDC)){ // draw Meta left right half
	R = new TEllipse(offsetX+0.5*dXLeg              ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
	R = new TEllipse(offsetX+2*dXLeg+dXLeg2-0.5*dXLeg,yMeta,richR, richR2);  // Meta right
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoMETA2)){ // draw Meta left
	R = new TEllipse(offsetX               ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    }
    //----------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------
    // Inner MDC
    if(HParticleTool::isPairsFlagsBit(flag,kSameInnerMDC)) {   // Inner MDC middle
	l = new TLine(offsetX+dXLeg+0.5*dXLeg2,yLowLegLow ,offsetX+dXLeg+0.5*dXLeg2,yLowLegUp);   // middle lower leg
	l->SetLineWidth(flineWidth);
	l->Draw();

    } else if( (HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC|kNoSameRICH) &&  fuseRICH) ||
	      (HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC)             && !fuseRICH)
	     ) {   // Inner MDC left + right
	l = new TLine(offsetX                ,yLowLegLow,offsetX+dXLeg         ,yLowLegUp);   // left lower leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+2*dXLeg+dXLeg2 ,yLowLegLow,offsetX+dXLeg+dXLeg2  ,yLowLegUp);   // right lower leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(fuseRICH && HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC|kSameRICH)) {    // Inner MDC left + right para
	l = new TLine(offsetX+dXLeg        ,yLowLegLow ,offsetX+dXLeg        ,yLowLegUp);    // left lower para leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+dXLeg+dXLeg2 ,yLowLegLow ,offsetX+dXLeg+dXLeg2 ,yLowLegUp);    // right lower para leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // outer MDC
    if(HParticleTool::isPairsFlagsBit(flag,kSameOuterMDC)) {   // outer MDC middle
	l = new TLine(offsetX+dXLeg+0.5*dXLeg2,yUpLegLow ,offsetX+dXLeg+0.5*dXLeg2,yUpLegUp);   // middle upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoOuterMDC2)) {   // outer MDC left
	l = new TLine(offsetX+dXLeg         ,yUpLegLow ,offsetX               ,yUpLegUp);    // left upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kSameMETA)) {   // outer MDC left right para
	l = new TLine(offsetX+dXLeg        ,yUpLegLow ,offsetX+dXLeg        ,yUpLegUp);    // left upper para leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+dXLeg+dXLeg2 ,yUpLegLow ,offsetX+dXLeg+dXLeg2 ,yUpLegUp);    // right upper para leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kNoSameMETA) ||
	      HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kNoMETA2)
	     )
    {
	l = new TLine(offsetX+dXLeg          ,yUpLegLow ,offsetX               ,yUpLegUp);    // left upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+dXLeg+dXLeg2   ,yUpLegLow ,offsetX+2*dXLeg+dXLeg2,yUpLegUp);    // right upper leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

}

void HParticlePairDraw::drawPairNDC(Double_t xoffNDC,Double_t yoffNDC,
				    Double_t heightNDC,
				    UInt_t flag,TString nameCase,TString cuts)
{


    if(HParticleTool::isPairsFlagsBit(flag,kNoUseRICH))  fuseRICH=kFALSE;
    else                                                 fuseRICH=kTRUE;


    Double_t offsetX  = gPad->GetX1() + xoffNDC*(gPad->GetX2()-gPad->GetX1());
    Double_t offsetY  = gPad->GetY1() + yoffNDC*(gPad->GetY2()-gPad->GetY1());
    Double_t width    = 10./10.*heightNDC*(gPad->GetX2()-gPad->GetX1());
    Double_t height   = heightNDC*(gPad->GetY2()-gPad->GetY1());



    // 10 units in y
    //  9 units in x
    Double_t xu = width/9.;
    Double_t yu = height/10.;

    Double_t dYLeg    =  3*yu;
    Double_t dXLeg    =  2.*xu;
    Double_t dXLeg2   =  2.25*xu;

    Double_t yLowLegLow = offsetY+2*yu;
    Double_t yLowLegUp  = yLowLegLow+dYLeg;

    Double_t yUpLegLow  = yLowLegUp;
    Double_t yUpLegUp   = yUpLegLow+dYLeg;

    Double_t yRich      = offsetY+1*yu;
    Double_t yMeta      = offsetY+9*yu;
    Double_t yText      = yMeta  + (1.5*yu);

    Double_t richR      = 1*xu*10./9. ;
    Double_t richR2     = 1*yu ;

    Double_t textsize     = ftextSizeScale*0.09;
    Double_t textsizeInfo = ftextSizeInfoScale*0.09;
    Double_t textInfoY    = yLowLegUp ;


    TLine* l ;
    TEllipse* R;
    TLatex* paircase = new TLatex();
    paircase->SetTextSize(textsize);

    paircase->DrawLatex(offsetX+1*xu,yText,Form("%s",nameCase.Data()));
    if(HParticleTool::isPairsFlagsBit(flag,kNoRK2|kOuterMDC2)){ // no RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 6*xu,textInfoY,"-RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kRK2)){ //  RK
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 6*xu,textInfoY,"+RK");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSamePosPolarity)){ // pos pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 0*xu,textInfoY,"++");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kSameNegPolarity)){ // neg pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 0*xu,textInfoY,"--");
    }
    if(HParticleTool::isPairsFlagsBit(flag,kNoSamePolarity)){ // no same pol
	paircase->SetTextSize(textsizeInfo);
	paircase->DrawLatex(offsetX + 0*xu ,textInfoY,"+-");
    }

    if(cuts != ""){

	TObjArray* ar = cuts.Tokenize(",");

	for(Int_t i = 0 ; i< ar->GetEntries(); i++){
	    paircase->SetTextSize(textsizeInfo);
	    paircase->DrawLatex(offsetX ,offsetY-2*yu,((TObjString*)ar->At(i))->GetString().Data());
	}

	if(ar) delete ar;
    }
    //----------------------------------------------------------------------------------
    // RICH

    if(fuseRICH){
	if(HParticleTool::isPairsFlagsBit(flag,kSameRICH)){ // draw RICH middle
	    R = new TEllipse(offsetX+4.5*xu,yRich,richR, richR2);  // Rich middle
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	} else if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kNoSameInnerMDC)){ // draw RICH left right
	    R = new TEllipse(offsetX+2.0*xu              ,yRich,richR, richR2);  // Rich left
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	    if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kNoSameInnerMDC|kRICH2)){
		R = new TEllipse(offsetX+7.0*xu,yRich,richR, richR2);  // Rich right
		R->SetLineColor(frichColor);
		R->SetLineWidth(flineWidth);
		R->Draw();
	    }
	} else if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kSameInnerMDC)){ // draw RICH left right half
	    R = new TEllipse(offsetX+3.0*xu              ,yRich,richR, richR2);  // Rich left
	    R->SetLineColor(frichColor);
	    R->SetLineWidth(flineWidth);
	    R->Draw();
	    if(HParticleTool::isPairsFlagsBit(flag,kNoSameRICH|kSameInnerMDC|kRICH2)){
		R = new TEllipse(offsetX+6*xu,yRich,richR, richR2);  // Rich right
		R->SetLineColor(frichColor);
		R->SetLineWidth(flineWidth);
		R->Draw();
	    }
	}
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // META
    if(HParticleTool::isPairsFlagsBit(flag,kSameMETA)){ // draw Meta middle
	R = new TEllipse(offsetX+4.5*xu,yMeta,richR, richR2);  // Meta middle
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameMETA|kNoSameOuterMDC)){ // draw Meta left right
	R = new TEllipse(offsetX+2.0*xu          ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
	R = new TEllipse(offsetX+7.0*xu,yMeta,richR, richR2);  // Meta right
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameMETA|kSameOuterMDC)){ // draw Meta left right half
	R = new TEllipse(offsetX+3.0*xu              ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
	R = new TEllipse(offsetX+6.0*xu,yMeta,richR, richR2);  // Meta right
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoMETA2)){ // draw Meta left
	R = new TEllipse(offsetX + 2.0*xu        ,yMeta,richR, richR2);  // Meta left
	R->SetLineColor(fmetaColor);
	R->SetLineWidth(flineWidth);
	R->Draw();
    }
    //----------------------------------------------------------------------------------


    //----------------------------------------------------------------------------------
    // Inner MDC
    if(HParticleTool::isPairsFlagsBit(flag,kSameInnerMDC)) {   // Inner MDC middle
	l = new TLine(offsetX+4.5*xu        ,yLowLegLow ,offsetX+4.5*xu,yLowLegUp);   // middle lower leg
	l->SetLineWidth(flineWidth);
	l->Draw();

    } else if( (HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC|kNoSameRICH) &&  fuseRICH) ||
	      (HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC)             && !fuseRICH)
	     ) {   // Inner MDC left + right
	l = new TLine(offsetX+2.0*xu      ,yLowLegLow,offsetX+2.0*xu+dXLeg ,yLowLegUp);   // left lower leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+7.0*xu      ,yLowLegLow,offsetX+7.0*xu-dXLeg  ,yLowLegUp);   // right lower leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(fuseRICH && HParticleTool::isPairsFlagsBit(flag,kNoSameInnerMDC|kSameRICH)) {    // Inner MDC left + right para
	l = new TLine(offsetX+2.0*xu+dXLeg2        ,yLowLegLow ,offsetX+2.*xu+dXLeg2  ,yLowLegUp);    // left lower para leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+7.0*xu-dXLeg2        ,yLowLegLow ,offsetX+7.0*xu-dXLeg2   ,yLowLegUp);    // right lower para leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedInnerMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

    //----------------------------------------------------------------------------------
    // outer MDC
    if(HParticleTool::isPairsFlagsBit(flag,kSameOuterMDC)) {   // outer MDC middle
	l = new TLine(offsetX+4.5*xu,yUpLegLow ,offsetX+4.5*xu,yUpLegUp);   // middle upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoOuterMDC2)) {   // outer MDC left
	l = new TLine(offsetX+2.0*xu+dXLeg ,yUpLegLow ,offsetX+2.0*xu       ,yUpLegUp);    // left upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kSameMETA)) {   // outer MDC left right para
	l = new TLine(offsetX+2.0*xu+dXLeg2        ,yUpLegLow ,offsetX+2.0*xu+dXLeg2  ,yUpLegUp);    // left upper para leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+7.0*xu-dXLeg2 ,yUpLegLow ,offsetX+7.0*xu-dXLeg2,yUpLegUp);    // right upper para leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    } else if(HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kNoSameMETA) ||
	      HParticleTool::isPairsFlagsBit(flag,kNoSameOuterMDC|kNoMETA2)
	     )
    {
	l = new TLine(offsetX+2.0*xu+dXLeg ,yUpLegLow ,offsetX+2.0*xu       ,yUpLegUp);    // left upper leg
	l->SetLineWidth(flineWidth);
	l->Draw();
	l = new TLine(offsetX+7.0*xu-dXLeg ,yUpLegLow ,offsetX+7.0*xu       ,yUpLegUp);    // right upper leg
	if(HParticleTool::isPairsFlagsBit(flag,kNoFittedOuterMDC2)) l->SetLineStyle(fnoFittedStyle);
	if(HParticleTool::isPairsFlagsBit(flag,kIsUsed2))           l->SetLineColor(fusedColor);
	if(HParticleTool::isPairsFlagsBit(flag,kIsLepton2))         l->SetLineColor(fleptonColor);
	l->SetLineWidth(flineWidth);
	l->Draw();
    }
    //----------------------------------------------------------------------------------

}

void HParticlePairDraw::drawPairCases()
{
    // draws 3 canvases with the predefined lepton and
    // hadron pair cases

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


    UInt_t cases3[] = {
	Particle::kPairCase16,
	Particle::kPairCase17,
	Particle::kPairCase18,
	Particle::kPairCase19,
	Particle::kPairCase20,
	Particle::kPairCase21,
	Particle::kPairCase22,
	Particle::kPairCase23,
	Particle::kPairCase24,
	Particle::kPairCase24,
	Particle::kPairCase25
    };

    UInt_t bins1 = sizeof(cases1)/sizeof(UInt_t);
    UInt_t bins2 = sizeof(cases2)/sizeof(UInt_t);
    UInt_t bins3 = sizeof(cases3)/sizeof(UInt_t);

    TH1F* h = new TH1F("h","",bins1,0,bins1);
    h->GetXaxis()->SetLabelSize(0);
    h->GetYaxis()->SetRangeUser(0.,50.);
    h->GetYaxis()->SetAxisColor(kWhite);
    h->GetYaxis()->SetLabelColor(kWhite);
    h->GetYaxis()->SetTitleColor(kWhite);
    h->GetXaxis()->SetAxisColor(kWhite);
    h->SetLineColor(kWhite);

    TH1F* h2 = new TH1F("h2","",bins3,0,bins3);
    h2->GetXaxis()->SetLabelSize(0);
    h2->GetYaxis()->SetRangeUser(0.,50.);
    h2->GetYaxis()->SetAxisColor(kWhite);
    h2->GetYaxis()->SetLabelColor(kWhite);
    h2->GetYaxis()->SetTitleColor(kWhite);
    h2->GetXaxis()->SetAxisColor(kWhite);
    h2->SetLineColor(kWhite);


    Int_t cheight = 100;

    TCanvas* c = new TCanvas("pairCaseLepton1","pairCase Lepton",950,cheight);
    c->SetTopMargin(0.);
    c->SetBottomMargin(0.9);
    c->SetLeftMargin(0.);
    c->SetRightMargin(0.);
    c->SetFrameLineColor(kWhite);

    h->DrawCopy();

    for(UInt_t i=0; i < bins1; i++){
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases1[i],Form("Case%i",i+1),"");
    }

    TCanvas* c2 = new TCanvas("pairCaseLepton2","pairCase Lepton 2",950,cheight);
    c2->SetTopMargin(0.);
    c2->SetBottomMargin(0.9);
    c2->SetLeftMargin(0.);
    c2->SetRightMargin(0.);
    c2->SetFrameLineColor(kWhite);

    h->DrawCopy();

    for(UInt_t i=0; i < bins2; i++){
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases2[i],Form("Case%i",i+1+10),"");
    }

    TCanvas* c3 = new TCanvas("pairCaseHadron","pairCase Hadron",950,cheight);
    c3->SetTopMargin(0.);
    c3->SetBottomMargin(0.9);
    c3->SetLeftMargin(0.);
    c3->SetRightMargin(0.);
    c3->SetFrameLineColor(kWhite);

    h2->Draw();

    for(UInt_t i=0; i < bins3; i++){
	pairdraw.drawPair(offsetX+i,offsetY,scaleX,50*scaleY,cases3[i],Form("Case%i",i+1+15),"");
    }

}

