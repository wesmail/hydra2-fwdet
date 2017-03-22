#ifndef RICHDRAWHIST_H_
#define RICHDRAWHIST_H_

#include "TH1.h"
#include "TH1D.h"
#include "TH2.h"
#include "TPad.h"
#include "TLegend.h"
#include "TGraph.h"
#include "TGraph2D.h"

#include <string>
#include <vector>
using std::string;
using std::vector;

class RichDrawingOptions
{
public:

    static Int_t Color(Int_t colorIndex) {
	static const Int_t nofColors = 6;
	static Int_t colors[nofColors] = {kRed, kBlue + 1, kGreen + 3, kMagenta + 4, kYellow + 2, kViolet};
	return (colorIndex < nofColors) ? colors[colorIndex] : colorIndex;
    }

    static Int_t LineWidth() {
	return 2;
    }

    static Int_t LineStyle(Int_t lineStyleIndex) {
	return lineStyleIndex + 1;
    }

    static Int_t MarkerSize() {
	return 1;
    }

    static Int_t MarkerStyle(Int_t markerIndex) {
	static const Int_t nofMarkers = 8;
	static Int_t markers[nofMarkers] = {kOpenCircle, kOpenSquare, kOpenTriangleUp, kOpenDiamond,
	kFullCircle, kFullSquare, kFullTriangleUp};
	return (markerIndex < nofMarkers) ? markers[markerIndex] : markerIndex;
    }

    static Double_t TextSize() {
	return 0.06;
    }
};

//Define linear or logarithmic scale for drawing.
enum HistScale {
    kLog = 0, //Linear scale.
    kLinear = 1 //
};


class HRichDrawHist : public TObject
{

public:
    static void SetDefaultDrawStyle();

    static void DrawH1(
		       TH1* hist,
		       HistScale logx = kLinear,
		       HistScale logy = kLinear,
		       const string& drawOpt = "",
		       Int_t color = RichDrawingOptions::Color(0),
		       Int_t lineWidth = RichDrawingOptions::LineWidth(),
		       Int_t lineStyle = RichDrawingOptions::LineStyle(0),
		       Int_t markerSize = RichDrawingOptions::MarkerSize(),
		       Int_t markerStyle = RichDrawingOptions::MarkerStyle(0));

    static void DrawH2(
		       TH2* hist,
		       HistScale logx = kLinear,
		       HistScale logy = kLinear,
		       HistScale logz = kLinear,
		       const string& drawOpt = "COLZ");


    static void DrawH1(
		       const vector<TH1*>& histos,
		       const vector<string>& histLabels,
		       HistScale logx = kLinear,
		       HistScale logy = kLinear,
		       Bool_t drawLegend = kTRUE,
		       Double_t x1 = 0.6,
		       Double_t y1 = 0.99,
		       Double_t x2 = 0.99,
		       Double_t y2 = 0.83,
		       const string& drawOpt = "");

    static void DrawGraph(
			  TGraph* graph,
			  HistScale logx = kLinear,
			  HistScale logy = kLinear,
			  const string& drawOpt = "AC",
			  Int_t color = RichDrawingOptions::Color(0),
			  Int_t lineWidth = RichDrawingOptions::LineWidth(),
			  Int_t lineStyle = RichDrawingOptions::LineStyle(0),
			  Int_t markerSize = RichDrawingOptions::MarkerSize(),
			  Int_t markerStyle = RichDrawingOptions::MarkerStyle(0));

    static void DrawGraph(
			  const vector<TGraph*>& graphs,
			  const vector<string>& graphLabels,
			  HistScale logx = kLinear,
			  HistScale logy = kLinear,
			  Bool_t drawLegend = kTRUE,
			  Double_t x1 = 0.50,
			  Double_t y1 = 0.78,
			  Double_t x2 = 0.99,
			  Double_t y2 = 0.99);

    static void DrawGraph2D(
			    TGraph2D* graph,
			    HistScale logx = kLinear,
			    HistScale logy = kLinear,
			    HistScale logz = kLinear,
			    const string& drawOpt = "colz");

    static void DrawTextOnPad(
			      const string& text,
			      Double_t x1,
			      Double_t y1,
			      Double_t x2,
			      Double_t y2);

    static void DrawH1andFitGauss(
				  TH1* hist,
				  Bool_t drawResults = kTRUE,
				  Bool_t doScale = kTRUE,
				  Double_t userRangeMin = 0.,
				  Double_t userRangeMax = 0.);


    ClassDef(HRichDrawHist,0)

};
#endif
