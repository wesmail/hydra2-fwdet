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
#include "hrich700digipar.h"
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

    fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(!fDigiPar) {
	Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }

    fHM = new HRich700HistManager();

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
    //HRichHitSim* richHit = static_cast<HRichHitSim*>(fCatRichHit->getObject(0));
   // if (richHit != NULL) {
	isDrawCurrentEvent = kTRUE;
	fNofDrawnEvents++;
   // }
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
    TCanvas *c = fHM->CreateCanvas(ss.str().c_str(), ss.str().c_str(), 800, 900);
    c->cd();
    TH2D* pad = new TH2D("padU", ";x [cm];y [cm]", 1, -700., 700., 1, -700., 700);
    HRichDrawHist::DrawH2(pad);
    pad->GetYaxis()->SetTitleOffset(0.75);
    gPad->SetLeftMargin(0.1);
    gPad->SetRightMargin(0.05);

    drawPmts(0., 0., false);

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
	    pair<Double_t, Double_t> xy = fDigiPar->getXY(loc);

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
	    pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richPhoton->getPmtId());
	    // x and y have to be swapped for compatibility
	    Double_t photonX = richPhoton->getY();
            Double_t photonY = richPhoton->getX();
	    TEllipse* hitDr = new TEllipse(pmtXY.first + photonX, pmtXY.second + photonY, 1.5);
	    hitDr->SetFillColor(kBlue);
	    hitDr->SetLineColor(kBlue);
	    hitDr->Draw();
	}
    }

    if (fDrawRichDirects){
	Int_t nofRichDirect = fCatRichDirect->getEntries();
	for (Int_t i = 0; i < nofRichDirect; i++) {
	    HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richDirect->getPmtId());
	    // x and y have to be swapped for compatibility
	    Double_t directX = richDirect->getY();
            Double_t directY = richDirect->getX();
	    TEllipse* hitDr = new TEllipse(pmtXY.first + directX, pmtXY.second + directY, 3.0);
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
    TCanvas *c = fHM->CreateCanvas(ss.str().c_str(), ss.str().c_str(), 800, 900);
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

    drawPmts(xc, yc, true);

    if (fDrawRichCals){
	Int_t nofRichCals = fCatRichCal->getEntries();
	Double_t pixelHalfSize = fDigiPar->getPmtSensSize() /fDigiPar->getNPixelInRow() / 2.;

	for (Int_t i = 0; i < nofRichCals; i++) {
	    HRichCal* richCal = static_cast<HRichCal*>(fCatRichCal->getObject(i));

	    Int_t loc[3];
	    loc[0] = richCal->getSector();
	    loc[1] = richCal->getCol();
	    loc[2] = richCal->getRow();
	    pair<Double_t, Double_t> xy = fDigiPar->getXY(loc);

	  //  TEllipse* hitDr = new TEllipse(xy.first - xc, xy.second - yc, 2.9);
	  //  hitDr->SetFillColor(kRed);
	  //  hitDr->SetLineColor(kRed);
	  //  hitDr->Draw();

            TBox* box = new TBox(xy.first - xc - pixelHalfSize, xy.second - yc - pixelHalfSize,
				 xy.first - xc + pixelHalfSize, xy.second - yc + pixelHalfSize);
            box->SetFillColor(kRed);
	   // box->SetLineColor(kRed + 3);
	    box->SetFillStyle(3001);
	    box->SetLineWidth(2);
	    box->Draw();

            // workaround, draw box outer line
	    TBox* box2 = new TBox(xy.first - xc - pixelHalfSize, xy.second - yc - pixelHalfSize,
				 xy.first - xc + pixelHalfSize, xy.second - yc + pixelHalfSize);
	    box2->SetLineColor(kRed + 3);
	    box2->SetFillStyle(0);
	    box2->SetLineWidth(2);
	    box2->Draw();

	}
    }


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


    if (fDrawRichPhotons){
	Int_t nofRichPhotons = fCatRichPhoton->getEntries();
	for (Int_t i = 0; i < nofRichPhotons; i++) {
	    HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richPhoton->getPmtId());
	    // x and y have to be swapped for compatibility
	    Double_t photonX = richPhoton->getY();
            Double_t photonY = richPhoton->getX();
	    TEllipse* hitDr = new TEllipse( pmtXY.first + photonX - xc, pmtXY.second + photonY - yc, .6);
	    hitDr->SetFillColor(kBlue);
	    hitDr->SetLineColor(kBlue);
	    hitDr->Draw();
	}
    }

    if (fDrawRichDirects){
	Int_t nofRichDirect = fCatRichDirect->getEntries();
	for (Int_t i = 0; i < nofRichDirect; i++) {
	    HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
	    pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richDirect->getPmtId());
	    // x and y have to be swapped for compatibility
	    Double_t directX = richDirect->getY();
            Double_t directY = richDirect->getX();
	    TEllipse* hitDr = new TEllipse( pmtXY.first + directX - xc, pmtXY.second + directY - yc, .8);
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

void HRich700EventDisplay::drawPmts(Double_t offsetX, Double_t offsetY, Bool_t drawSens)
{
    Double_t pmtHalfSize = fDigiPar->getPmtSize() / 2.;
    Double_t pmtHalfSensSize = fDigiPar->getPmtSensSize() / 2.;

    map<Int_t,HRich700PmtData> pmtDataMap = fDigiPar->getPmtDataMapPmtId();

    const Int_t colors[] = { kBlue, kGreen + 1, kMagenta, kYellow+3, kRed, kCyan+2, kGray, kPink - 7};
    vector<Int_t> colorVec = RichUtils::MakeVector(colors);
    map<Int_t, Int_t> colorMap;
    for(map<Int_t,HRich700PmtData>::iterator it = pmtDataMap.begin(); it != pmtDataMap.end(); it++) {
        if (colorMap.find(it->second.fPmtType) == colorMap.end()) {
            Int_t colorInd = (colorMap.size() >= colorVec.size() )? colorVec.size() - 1: colorMap.size();
            colorMap[it->second.fPmtType] = colors[colorInd];
        }
    }

    for(map<Int_t,HRich700PmtData>::iterator it = pmtDataMap.begin(); it != pmtDataMap.end(); it++) {
        Double_t pmtX = it->second.fX;
        Double_t pmtY = it->second.fY;
        TBox* box = new TBox(pmtX - pmtHalfSize - offsetX, pmtY - pmtHalfSize - offsetY, pmtX + pmtHalfSize - offsetX, pmtY + pmtHalfSize - offsetY);
        box->SetFillStyle(drawSens?3003:3001);
        box->SetFillColor(colorMap[it->second.fPmtType]);
        box->Draw();

        // workaround, draw outer line
        TBox* box2 = new TBox(pmtX - pmtHalfSize - offsetX, pmtY - pmtHalfSize - offsetY, pmtX + pmtHalfSize - offsetX, pmtY + pmtHalfSize - offsetY);
        box2->SetLineColor(kOrange + 1);
        box2->SetFillStyle(0);
        box2->SetLineWidth(2);
        box2->Draw();


        if (drawSens) {
            TBox* boxSens = new TBox(pmtX - pmtHalfSensSize - offsetX, pmtY - pmtHalfSensSize - offsetY,
                         pmtX + pmtHalfSensSize - offsetX, pmtY + pmtHalfSensSize - offsetY);
            boxSens->SetLineColor(kGreen + 2);
            boxSens->SetFillStyle(0);
            boxSens->SetLineWidth(2);
            boxSens->Draw();
        }


    }
}

//---------------------------------------------------------------------------
Bool_t HRich700EventDisplay::finalize()
{
    fHM->SaveCanvasToImage(string(fOutputDir + "/"));
    return kTRUE;
}
