//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700DrawHist
//
//  This class contains drawing util functions of RICH.
//
//////////////////////////////////////////////////////////////////////////////
#include "hrich700drawhist.h"

#include "TH1.h"
#include "TH2.h"
#include "TH1D.h"
#include "TPad.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TGraph.h"
#include "TGraph2D.h"
#include "TMath.h"
#include "TGaxis.h"
#include "TLegend.h"
#include "TF1.h"
#include "hrich700utils.h"
#include "TLatex.h"

#include <string>
#include <limits>
#include <iostream>
#include <sstream>
#include <cassert>


ClassImp(HRichDrawHist)

    using std::stringstream;

/* Set default styles for histograms. */
void HRichDrawHist::SetDefaultDrawStyle()
{
    gStyle->SetOptStat("rm");
    gStyle->SetOptFit(1);
    gStyle->SetOptTitle(0);

    gStyle->SetCanvasColor(kWhite);
    gStyle->SetFrameFillColor(kWhite);
    gStyle->SetFrameBorderMode(0);
    gStyle->SetPadColor(kWhite);
    gStyle->SetStatColor(kWhite);
    gStyle->SetTitleFillColor(kWhite);
    gStyle->SetPalette(1);
}

/* Draw TH1 histogram.*/
void HRichDrawHist::DrawH1(
			   TH1* hist,
			   HistScale logx,
			   HistScale logy,
			   const string& drawOpt,
			   Int_t color,
			   Int_t lineWidth,
			   Int_t lineStyle,
			   Int_t markerSize,
			   Int_t markerStyle)
{
    // Draw 1D histogram.
    // hist Pointer to histogram.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // drawOpt Other drawing options (see ROOT documentation for details).
    // color Color.
    // lineWidth Line width.
    // lineStyle Line style (see ROOT documentation for details).
    // markerSize Marker size.
    // markerStyle Marker style (see ROOT documentation for details).

    if (hist == NULL) return;
    Double_t textSize = RichDrawingOptions::TextSize();
    hist->SetLineColor(color);
    hist->SetLineWidth(lineWidth);
    hist->SetLineStyle(lineStyle);
    hist->SetMarkerColor(color);
    hist->SetMarkerSize(markerSize);
    hist->SetMarkerStyle(markerStyle);
    if (logx == kLog) { gPad->SetLogx(); }
    if (logy == kLog) { gPad->SetLogy(); }
    hist->GetXaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetNdivisions(505, kTRUE);
    hist->GetYaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetTitleSize(textSize);
    hist->GetYaxis()->SetTitleSize(textSize);
    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.3);
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    gPad->SetTopMargin(0.12);
    gPad->SetTicks(1, 1);
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(kTRUE, kTRUE);
    hist->SetStats(kFALSE);
}

/* Draw TH2 histogram.*/
void HRichDrawHist::DrawH2(
			   TH2* hist,
			   HistScale logx,
			   HistScale logy,
			   HistScale logz,
			   const string& drawOpt)
{
    // Draw 2D histogram.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // logz Specify logarithmic or linear scale for Z axis.
    // drawOpt Other drawing options (see ROOT documentation for details).

    if (hist == NULL) return;
    Double_t textSize = RichDrawingOptions::TextSize();
    if (logx == kLog) { gPad->SetLogx(); }
    if (logy == kLog) { gPad->SetLogy(); }
    if (logz == kLog) { gPad->SetLogz(); }
    hist->GetXaxis()->SetLabelSize(textSize);
    hist->GetXaxis()->SetNdivisions(505, kTRUE);
    hist->GetYaxis()->SetLabelSize(textSize);
    hist->GetYaxis()->SetNdivisions(505, kTRUE);
    hist->GetZaxis()->SetLabelSize(textSize);
    // hist->GetZaxis()->SetNdivisions(505, kTRUE);
    hist->GetXaxis()->SetTitleSize(textSize);
    hist->GetYaxis()->SetTitleSize(textSize);
    hist->GetZaxis()->SetTitleSize(textSize);
    hist->GetXaxis()->SetTitleOffset(1.0);
    hist->GetYaxis()->SetTitleOffset(1.3);
    hist->GetZaxis()->SetTitleOffset(1.5);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.30);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    hist->Draw(drawOpt.c_str());
    gPad->SetGrid(kTRUE, kTRUE);
    hist->SetStats(kFALSE);
}

void HRichDrawHist::DrawH1(
			   const vector<TH1*>& histos,
			   const vector<string>& histLabels,
			   HistScale logx,
			   HistScale logy,
			   Bool_t drawLegend,
			   Double_t x1,
			   Double_t y1,
			   Double_t x2,
			   Double_t y2,
			   const string& drawOpt)
{
    // Draw several TH1 histograms.
    // histos Array of histograms.
    // histLabels Array of histogram labels.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // drawLegend If true than legend will be drawn.
    // x1 X coordinate of legend in current pad.
    // y1 Y coordinate of legend in current pad.
    // x2 X coordinate of legend in current pad.
    // y2 Y coordinate of legend in current pad.
    // drawOpt Draw options for TH1->Draw method.

    assert(histos.size() != 0 && histLabels.size() == histos.size());
    Double_t max = std::numeric_limits<Double_t>::min();
    UInt_t nofHistos = histos.size();
    TLegend* legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(kWhite);
    for (UInt_t iHist = 0; iHist < nofHistos; iHist++) {
	TH1* hist = histos[iHist];
	string opt = (iHist == 0) ? drawOpt : (iHist == nofHistos - 1) ? "SAME" + drawOpt : "SAME" + drawOpt;
	DrawH1(hist, logx, logy, opt, RichDrawingOptions::Color(iHist), RichDrawingOptions::LineWidth(),
	       RichDrawingOptions::LineStyle(0), RichDrawingOptions::MarkerSize(), RichDrawingOptions::MarkerStyle(iHist));
	max = std::max(max, hist->GetMaximum());
	legend->AddEntry(hist, histLabels[iHist].c_str(), "lp");
    }
    histos[0]->SetMaximum(max * 1.17);
    if (drawLegend) { legend->Draw(); }
}

void HRichDrawHist::DrawGraph(
			      TGraph* graph,
			      HistScale logx,
			      HistScale logy,
			      const string& drawOpt,
			      Int_t color,
			      Int_t lineWidth,
			      Int_t lineStyle,
			      Int_t markerSize,
			      Int_t markerStyle)
{
    // graph Pointer to TGraph.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // drawOpt Other drawing options (see ROOT documentation for details).
    // color Color.
    // lineWidth Line width.
    // lineStyle Line style (see ROOT documentation for details).
    // markerSize Marker size.
    // markerStyle Marker style (see ROOT documentation for details).

    if (graph == NULL) return;
    Double_t textSize = RichDrawingOptions::TextSize();
    graph->SetLineColor(color);
    graph->SetLineWidth(lineWidth);
    graph->SetLineStyle(lineStyle);
    graph->SetMarkerColor(color);
    graph->SetMarkerSize(markerSize);
    graph->SetMarkerStyle(markerStyle);
    if (drawOpt.find("A") != string::npos) {
	if (logx == kLog) { gPad->SetLogx(); }
	if (logy == kLog) { gPad->SetLogy(); }
	graph->GetXaxis()->SetLabelSize(textSize);
	graph->GetXaxis()->SetNdivisions(505, kTRUE);
	graph->GetYaxis()->SetLabelSize(textSize);
	graph->GetXaxis()->SetTitleSize(textSize);
	graph->GetYaxis()->SetTitleSize(textSize);
	graph->GetXaxis()->SetTitleOffset(1.0);
	graph->GetYaxis()->SetTitleOffset(1.3);
    }
    gPad->SetLeftMargin(0.17);
    gPad->SetBottomMargin(0.15);
    graph->Draw(drawOpt.c_str());
    gPad->SetGrid(kTRUE, kTRUE);
}


void HRichDrawHist::DrawGraph(
			      const vector<TGraph*>& graphs,
			      const vector<string>& graphLabels,
			      HistScale logx,
			      HistScale logy,
			      Bool_t drawLegend,
			      Double_t x1,
			      Double_t y1,
			      Double_t x2,
			      Double_t y2)
{
    // Draw upto 3 TGraphs. If graph == NULL than graph will not be drawn.
    // graphs Vector of TGraphs.
    // graphLabels Vector of graph labels.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // drawLegend If true than legend will be drawn.
    // x1 X coordinate of legend in current pad.
    // y1 Y coordinate of legend in current pad.
    // x2 X coordinate of legend in current pad.
    // y2 Y coordinate of legend in current pad.

    assert(graphs.size() != 0 && graphs.size() == graphLabels.size());

    Double_t max = std::numeric_limits<Double_t>::min();
    Double_t min = std::numeric_limits<Double_t>::max();
    TLegend* legend = new TLegend(x1, y1, x2, y2);
    legend->SetFillColor(kWhite);
    UInt_t nofGraphs = graphs.size();
    for (UInt_t iGraph = 0; iGraph < nofGraphs; iGraph++) {
	TGraph* graph = graphs[iGraph];
	string opt = (iGraph == 0) ? "ACP" : "CP";
	DrawGraph(graph, logx, logy, opt, RichDrawingOptions::Color(iGraph), RichDrawingOptions::LineWidth(),
		  RichDrawingOptions::LineStyle(iGraph), RichDrawingOptions::MarkerSize(), RichDrawingOptions::MarkerStyle(iGraph));
	max = std::max(graph->GetYaxis()->GetXmax(), max);
	min = std::min(graph->GetYaxis()->GetXmin(), min);
	legend->AddEntry(graph, graphLabels[iGraph].c_str(), "lp");
    }
    graphs[0]->SetMaximum(max);
    graphs[0]->SetMinimum(min);
    if (drawLegend) { legend->Draw(); }
}

void HRichDrawHist::DrawGraph2D(
				TGraph2D* graph,
				HistScale logx,
				HistScale logy,
				HistScale logz,
				const string& drawOpt)
{
    // Draw 2D graph.
    // graph Pointer to TGraph2D.
    // logx Specify logarithmic or linear scale for X axis.
    // logy Specify logarithmic or linear scale for Y axis.
    // logz Specify logarithmic or linear scale for Z axis.
    // drawOpt Other drawing options (see ROOT documentation for details).

    Double_t textSize = RichDrawingOptions::TextSize();
    if (logx == kLog) { gPad->SetLogx(); }
    if (logy == kLog) { gPad->SetLogy(); }
    if (logz == kLog) { gPad->SetLogz(); }
    graph->GetXaxis()->SetLabelSize(textSize);
    graph->GetXaxis()->SetNdivisions(505, kTRUE);
    graph->GetYaxis()->SetLabelSize(textSize);
    graph->GetYaxis()->SetNdivisions(505, kTRUE);
    graph->GetZaxis()->SetLabelSize(textSize);
    //   graph->GetZaxis()->SetNdivisions(505, kTRUE);
    graph->GetXaxis()->SetTitleSize(textSize);
    graph->GetYaxis()->SetTitleSize(textSize);
    graph->GetZaxis()->SetTitleSize(textSize);
    graph->GetXaxis()->SetTitleOffset(1.0);
    graph->GetYaxis()->SetTitleOffset(1.3);
    graph->GetZaxis()->SetTitleOffset(1.5);
    gPad->SetLeftMargin(0.17);
    gPad->SetRightMargin(0.30);
    gPad->SetBottomMargin(0.15);
    gPad->SetTicks(1, 1);
    graph->Draw(drawOpt.c_str());
    gPad->SetGrid(kTRUE, kTRUE);
}

void HRichDrawHist::DrawTextOnPad(
				  const string& text,
				  Double_t x1,
				  Double_t y1,
				  Double_t x2,
				  Double_t y2)
{
    // Draw text on a current pad. Latex expression can be used.
    // text Your text. Latex can be also used.
    // x1 X1 coordinate of legend in current pad.
    // y1 Y1 coordinate of legend in current pad.
    // x2 X2 coordinate of legend in current pad.
    // y2 Y2 coordinate of legend in current pad.

    TLegend* leg = new TLegend(x1, y1, x2, y2);
    leg->AddEntry(new TH2D(), text.c_str(), "");
    leg->SetFillColor(kWhite);
    leg->SetFillStyle(0);
    leg->SetBorderSize(0);
    leg->Draw();
}

void HRichDrawHist::DrawH1andFitGauss(
				      TH1* hist,
				      Bool_t drawResults,
				      Bool_t doScale,
				      Double_t userRangeMin,
				      Double_t userRangeMax)
{
    // Draw TH1 and fit with gauss. Draw fit results on top of the histogramm.
    // hist Histogram to be drawn and fitted.
    // drawResults If true - mean/sigma will be drawn.
    // doScale If true - histogramm is scaled to Integral 1.
    // userRangeMin SetRangeUser minimum.
    // userRangeMax SetRangeUser maximum.


    if (hist == NULL) return;

    hist->GetYaxis()->SetTitle("Yield");
    DrawH1(hist);
    if (doScale) hist->Scale(1./ hist->Integral());
    if (!(userRangeMin == 0. && userRangeMax == 0.)) hist->GetXaxis()->SetRangeUser(userRangeMin, userRangeMax);
    hist->Fit("gaus", "Q");
    TF1* func = hist->GetFunction("gaus");
    if (func == NULL) return;
    func->SetLineColor(kBlack);

    if (drawResults) {
	Double_t m = func->GetParameter(1);
	Double_t s = func->GetParameter(2);
	string txt1 = RichUtils::NumberToString<Double_t>(m, 2) + " / " + RichUtils::NumberToString<Double_t>(s, 2);
	TLatex text;
	text.SetTextAlign(21);
	text.SetTextSize(0.06);
	text.DrawTextNDC(0.5, 0.92, txt1.c_str());
    }
}
