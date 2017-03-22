//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichEventDisplay
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700eventdisplay.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hevent.h"
#include "hgeantrich.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hparset.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "richdef.h"
#include "hrich700drawhist.h"
#include "hrich700utils.h"
#include "hrich700digimappar.h"
#include "hrichcalsim.h"
#include "hrichhitsim.h"
#include "hgeantkine.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TPad.h"
#include "TEllipse.h"
#include "TLatex.h"
#include "TBox.h"

#include <iostream>
#include <sstream>
#include <utility>

using namespace std;


ClassImp(HRich700EventDisplay)

    HRich700EventDisplay::HRich700EventDisplay():
fEventNum(0),
    fDrawRichPhotons(kTRUE),
    fDrawRichDirects(kTRUE),
    fDrawRichCals(kTRUE),
    fDrawRichHits(kTRUE),
    fNofEventsToDraw(10),
    fNofDrawnEvents(0)
{

}

HRich700EventDisplay::~HRich700EventDisplay()
{

}

Bool_t HRich700EventDisplay::init()
{


    // Initialize geant rich cherenkov photon category and set appropriate iterator
    fCatRichPhoton = gHades->getCurrentEvent()->getCategory(catRichGeantRaw);
    if (NULL == fCatRichPhoton) {
	Error("init", "Initializatin of Cherenkov photon category failed, returning...");
	return kFALSE;
    }

    fCatRichDirect = gHades->getCurrentEvent()->getCategory(catRichGeantRaw + 1);
    if (NULL == fCatRichDirect) {
	Error("init", "Initialization of geant category for direct hits failed, returning...");
	return kFALSE;
    }

    fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCatRichCal) {
	Error("init", "Initialization of RICH cal failed, returning...");
	return kFALSE;
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
	Error("init", "Initialization of RICH hit failed, returning...");
	return kFALSE;
    }

    fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (NULL == fCatKine) {
	Error("init", "Initializatin of kine category failed, returning...");
	return kFALSE;
    }

    fDigiMap = (HRich700DigiMapPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiMapPar");
    if(!fDigiMap) {
	Error("init", "Can not retrieve HRich700DigiMapPar");
        return kFALSE;
    }

    return kTRUE;

}
//============================================================================

//----------------------------------------------------------------------------
Bool_t HRich700EventDisplay::reinit()
{

    return kTRUE;
}
//============================================================================

//----------------------------------------------------------------------------
Int_t HRich700EventDisplay::execute()
{
    HRichDrawHist::SetDefaultDrawStyle();
    fEventNum++;
    cout << "HRich700EventDisplay::execute eventNum " << fEventNum << endl;

    Bool_t isDrawCurrentEvent = kFALSE;
    HRichHitSim* richHit = static_cast<HRichHitSim*>(fCatRichHit->getObject(0));
    if (richHit != NULL) {
	isDrawCurrentEvent = kTRUE;
	fNofDrawnEvents++;
    }
    if (fNofDrawnEvents > fNofEventsToDraw) return 0;

    if (isDrawCurrentEvent) {
	drawOneEvent();
	drawOneRing();
    }

    return 0;
}
//============================================================================

void HRich700EventDisplay::drawOneEvent()
{
    stringstream ss;
    ss << "hrich_event_display_event_"<< fNofDrawnEvents;
    TCanvas *c = createCanvas(ss.str().c_str(), ss.str().c_str(), 800, 900);
    c->cd();
    TH2D* pad = new TH2D("padU", ";x [cm];y [cm]", 1, -700., 700., 1, -700., 700);
    HRichDrawHist::DrawH2(pad);
    pad->GetYaxis()->SetTitleOffset(0.75);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.05);

    drawPmts(0., 0.);

    if (fDrawRichHits) {
	Int_t nofRichHits = fCatRichHit->getEntries();
	for (Int_t i = 0; i < nofRichHits; i++) {
	    HRichHit* richHit = static_cast<HRichHit*>(fCatRichHit->getObject(i));
	    TEllipse* circle = new TEllipse(richHit->fRich700CircleCenterX, richHit->fRich700CircleCenterY, richHit->fRich700CircleRadius);
	    circle->SetFillStyle(0);
	    circle->SetLineWidth(3);
	    circle->Draw();
	    TEllipse* cCircle = new TEllipse(richHit->fRich700CircleCenterX, richHit->fRich700CircleCenterY, 5.);
	    cCircle->SetFillColor(kBlack);
	    cCircle->Draw();
	}
    }

    if (fDrawRichCals){
	Int_t nofRichCals = fCatRichCal->getEntries();
	for (Int_t i = 0; i < nofRichCals; i++) {
	    HRichCal* richCal = static_cast<HRichCal*>(fCatRichCal->getObject(i));

	    Int_t loc[3];
	    loc[0] = richCal->getSector();
	    loc[1] = richCal->getCol();
	    loc[2] = richCal->getRow();
	    pair<Double_t, Double_t> xy = fDigiMap->getXY(loc);

	    TEllipse* hitDr = new TEllipse(xy.first, xy.second, 5.0);
	    hitDr->SetFillColor(kRed);
	    hitDr->SetLineColor(kRed);
	    hitDr->Draw();
	}
    }


    if (fDrawRichPhotons){
	Int_t nofRichPhotons = fCatRichPhoton->getEntries();
	for (Int_t i = 0; i < nofRichPhotons; i++) {
	    HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiMap->getPmtCenter(richPhoton->getPmtId());
	    TEllipse* hitDr = new TEllipse(pmtXY.first + richPhoton->getX(), pmtXY.second + richPhoton->getY(), 1.5);
	    hitDr->SetFillColor(kBlue);
	    hitDr->SetLineColor(kBlue);
	    hitDr->Draw();
	}
    }

    if (fDrawRichDirects){
	Int_t nofRichDirect = fCatRichDirect->getEntries();
	for (Int_t i = 0; i < nofRichDirect; i++) {
	    HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiMap->getPmtCenter(richDirect->getPmtId());
	    TEllipse* hitDr = new TEllipse(pmtXY.first + richDirect->getX(), pmtXY.second + richDirect->getY(), 3.0);
	    hitDr->SetFillColor(kGreen);
	    hitDr->SetLineColor(kGreen);
	    hitDr->Draw();
	}
    }

}

void HRich700EventDisplay::drawOneRing()
{

    stringstream ss;
    ss << "hrich_event_display_ring_"<< fNofDrawnEvents;
    TCanvas *c = createCanvas(ss.str().c_str(), ss.str().c_str(), 800, 900);
    c->cd();
    TH2D* pad = new TH2D("padU", ";x [cm];y [cm]", 1, -50., 50., 1, -50., 50);
    HRichDrawHist::DrawH2(pad);
    pad->GetYaxis()->SetTitleOffset(0.75);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.05);

    HRichHit* ring = static_cast<HRichHit*>(fCatRichHit->getObject(0));
    if (ring == NULL) return;
    Double_t xc = ring->fRich700CircleCenterX;
    Double_t yc = ring->fRich700CircleCenterY;

    drawPmts(xc, yc);


    TEllipse* circle = new TEllipse(ring->fRich700CircleCenterX - xc, ring->fRich700CircleCenterY - yc, ring->fRich700CircleRadius);
    circle->SetFillStyle(0);
    circle->SetLineWidth(3);
    circle->Draw();
    TEllipse* cCircle = new TEllipse(ring->fRich700CircleCenterX - xc, ring->fRich700CircleCenterY - yc, 1.);
    cCircle->SetFillColor(kBlack);
    cCircle->Draw();

    Bool_t drawAllRings = kTRUE;
    if (drawAllRings) {
	Int_t nofRings = fCatRichHit->getEntries();
	for (Int_t i = 1; i < nofRings; i++) {
	    HRichHit* ring1 = static_cast<HRichHit*>(fCatRichHit->getObject(i));
	    if (ring == NULL) return;
	    TEllipse* circle = new TEllipse(ring1->fRich700CircleCenterX - xc, ring1->fRich700CircleCenterY - yc, ring1->fRich700CircleRadius);
	    circle->SetFillStyle(0);
	    circle->SetLineWidth(3);
	    circle->Draw();
	    TEllipse* cCircle = new TEllipse(ring1->fRich700CircleCenterX - xc, ring1->fRich700CircleCenterY - yc, 1.);
	    cCircle->SetFillColor(kBlack);
	    cCircle->Draw();
	}
    }


    if (fDrawRichCals){
	Int_t nofRichCals = fCatRichCal->getEntries();
	for (Int_t i = 0; i < nofRichCals; i++) {
	    HRichCal* richCal = static_cast<HRichCal*>(fCatRichCal->getObject(i));

	    Int_t loc[3];
	    loc[0] = richCal->getSector();
	    loc[1] = richCal->getCol();
	    loc[2] = richCal->getRow();
	    pair<Double_t, Double_t> xy = fDigiMap->getXY(loc);

	    TEllipse* hitDr = new TEllipse(xy.first - xc, xy.second - yc, 3.0);
	    hitDr->SetFillColor(kRed);
	    hitDr->SetLineColor(kRed);
	    hitDr->Draw();
	}
    }


    if (fDrawRichPhotons){
	Int_t nofRichPhotons = fCatRichPhoton->getEntries();
	for (Int_t i = 0; i < nofRichPhotons; i++) {
	    HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiMap->getPmtCenter(richPhoton->getPmtId());
	    TEllipse* hitDr = new TEllipse(pmtXY.first + richPhoton->getX() - xc, pmtXY.second + richPhoton->getY() - yc, .6);
	    hitDr->SetFillColor(kBlue);
	    hitDr->SetLineColor(kBlue);
	    hitDr->Draw();
	}
    }

    if (fDrawRichDirects){
	Int_t nofRichDirect = fCatRichDirect->getEntries();
	for (Int_t i = 0; i < nofRichDirect; i++) {
	    HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiMap->getPmtCenter(richDirect->getPmtId());
	    TEllipse* hitDr = new TEllipse(pmtXY.first + richDirect->getX() - xc, pmtXY.second + richDirect->getY() - yc, .8);
	    hitDr->SetFillColor(kGreen);
	    hitDr->SetLineColor(kGreen);
	    hitDr->Draw();
	}
    }

    TString ssCircleParam;
    ssCircleParam.Form("(x, y, R, n)=(%.1f, %.1f, %.2f, %i)", ring->fRich700CircleCenterX, ring->fRich700CircleCenterY,
		       ring->fRich700CircleRadius, ring->fRich700NofRichCals);
    TLatex* latexCircle = new TLatex(-49, 40., ssCircleParam.Data());
    latexCircle->SetTextSize(0.05);
    latexCircle->Draw();


}

void HRich700EventDisplay::drawPmts(Double_t offsetX, Double_t offsetY)
{
    Double_t pmtHalfSize = fDigiMap->getPmtSize() / 2.;
    vector<pair<Double_t, Double_t> > pmtXY = fDigiMap->getPmtCenters();

    for (UInt_t i = 0; i < pmtXY.size(); i++) {
	Double_t pmtX = pmtXY[i].first;
	Double_t pmtY = pmtXY[i].second;
	TBox* box = new TBox(pmtX - pmtHalfSize - offsetX, pmtY - pmtHalfSize - offsetY, pmtX + pmtHalfSize - offsetX, pmtY + pmtHalfSize - offsetY);
	box->SetLineColor(kOrange);
	box->SetFillStyle(0);
	box->SetLineWidth(2);
	box->Draw();
    }
}

TCanvas* HRich700EventDisplay::createCanvas(
					    const string& name,
					    const string& title,
					    Int_t width,
					    Int_t height)
{
    TCanvas* c = new TCanvas(name.c_str(), title.c_str(), width, height);
    fCanvas.push_back(c);
    return c;
}

void HRich700EventDisplay::saveCanvasToImage()
{
    for (UInt_t i = 0; i < fCanvas.size(); i++)
    {
	RichUtils::SaveCanvasAsImage(fCanvas[i], string(fOutputDir + "/"), "png");
    }
}

//---------------------------------------------------------------------------
Bool_t HRich700EventDisplay::finalize()
{
    saveCanvasToImage();
    return kTRUE;
}
