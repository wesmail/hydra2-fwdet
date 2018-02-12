//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700GeoTestQa
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700geotestqa.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hevent.h"
#include "hgeantrich.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hparset.h"
#include "hspectrometer.h"
#include "richdef.h"
#include "hrich700drawhist.h"
#include "hrich700pmt.h"
#include "hrichhitsim.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TPad.h"
#include "TEllipse.h"
#include "TRandom.h"
#include "TLatex.h"

#include "hgeantkine.h"

#include "hrich700histmanager.h"
#include "hrich700digipar.h"
#include "hrich700utils.h"

#include <iostream>
#include <sstream>

using namespace std;


ClassImp(HRich700GeoTestQa)

    HRich700GeoTestQa::HRich700GeoTestQa():
fEventNum(0)
{

}

HRich700GeoTestQa::~HRich700GeoTestQa()
{

}

Bool_t HRich700GeoTestQa::init()
{


    fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (NULL == fCatKine) {
	Error("init", "Initializatin of kine category failed, returning...");
	return kFALSE;
    }

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


    fCatRichMirror = gHades->getCurrentEvent()->getCategory(catRichGeantRaw + 2);
    if (NULL == fCatRichMirror) {
	Error("init", "Initializatin of RICH mirror category failed, returning...");
	return kFALSE;
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
	Error("init", "Initializatin of RICH hit category failed, returning...");
	return kFALSE;
    }

    fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(!fDigiPar) {
	Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }

    initHist();
    return kTRUE;

}

void HRich700GeoTestQa::initHist()
{
    fHM = new HRich700HistManager();
    fHM->Create2<TH2D>("fhNofRichPhotonsVsThetaElectron", "fhNofRichPhotonsVsThetaElectron;Theta [deg];Number of photons;Yield", 30, 0, 90, 125, 0, 250);
    fHM->Create2<TH2D>("fhNofRichMirrorPhotonsVsThetaElectron",
		       "fhNofRichMirrorPhotonsVsThetaElectron;Theta [deg];Number of mirror photons;Yield", 30, 0, 90, 175, 0, 350);
    fHM->Create2<TH2D>("fhNofRichCalsVsThetaElectron", "fhNofRichCalsVsThetaElectron;Theta [deg];Number of RICH cals;Yield", 30, 0, 90, 30, -.5, 29.5);

    fHM->Create2<TH2D>("fhNofRichPhotonsVsPhiElectron", "fhNofRichPhotonsVsPhiElectron;Phi [deg];Number of photons;Yield", 60, 0, 360, 125, 0, 250);
    fHM->Create2<TH2D>("fhNofRichMirrorPhotonsVsPhiElectron",
		       "fhNofRichMirrorPhotonsVsPhiElectron;Phi [deg];Number of mirror photons;Yield", 60, 0, 360, 175, 0, 350);
    fHM->Create2<TH2D>("fhNofRichCalsVsPhiElectron", "fhNofRichCalsVsPhiElectron;Phi [deg];Number of RICH cals;Yield", 60, 0, 360, 30, -.5, 29.5);

    fHM->Create2<TH2D>("fhRingXYElectron", "fhRingsXYElectron;X [mm];Y [mm];Yield", 175,-700, 700, 175, -700, 700);
    fHM->Create2<TH2D>("fhRingRadiusVsThetaElectron", "fhRingRadiusVsThetaElectron;Theta [deg];Ring radius [mm];Yield", 30, 0, 90, 70, 0., 35.);

    fHM->Create1<TH1D>("fhRingChi2Electron", "fhRingChi2Electron;Chi2;Yield", 80, 0., 12.);

    fHM->Create2<TH2D>("fhRichPhotonsXY", "fhRichPhotonsXY;X [mm];Y [mm];Yield", 175,-700, 700, 175, -700, 700);
    fHM->Create2<TH2D>("fhRichPhotonsSecXY", "fhRichPhotonsSecXY;X [mm];Y [mm];Yield", 175,-700, 700, 175, -700, 700);
    fHM->Create1<TH1D>("fhRichPhotonsEnergy", "fhRichPhotonsEnergy;Energy [eV];Yield", 120, 0, 12);
    fHM->Create1<TH1D>("fhRichPhotonsWavelength", "fhRichPhotonsWavelength;Wavelength [nm];Yield", 100, 0., 1000);
    fHM->Create2<TH2D>("fhRichDirectsXY", "fhRichDirectsXY;X [mm];Y [mm];Yield", 175,-700, 700, 175, -700, 700);
}

Bool_t HRich700GeoTestQa::reinit()
{
    return kTRUE;
}


Int_t HRich700GeoTestQa::execute()
{
    HRichDrawHist::SetDefaultDrawStyle();
    fEventNum++;
    cout << "HRich700GeoTestQa::execute eventNum " << fEventNum << endl;

    fillMcHist();

    return 0;
}

void HRich700GeoTestQa::fillMcHist()
{
    Int_t nofRichPhotons = fCatRichPhoton->getEntries();
    for (Int_t i = 0; i < nofRichPhotons; i++) {
	HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));
	pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richPhoton->getPmtId());

    // if (richPhoton->getPmtId() > 400) {
    //     cout << "pmtId:" << richPhoton->getPmtId() << " x:" << pmtXY.first << " y:" << pmtXY.second << endl;
    // }

	if (!(pmtXY.first == 0 && pmtXY.second == 0)) {
            // x and y have to be swapped for compatibility
	    fHM->H2("fhRichPhotonsXY")->Fill(pmtXY.first + richPhoton->getY(), pmtXY.second + richPhoton->getX());
            Int_t trackId = richPhoton->getTrack();
	    HGeantKine* kine = (HGeantKine*)fCatKine->getObject(trackId - 1);
	    if (kine != NULL && !kine->isPrimary()) {
		          fHM->H2("fhRichPhotonsSecXY")->Fill(pmtXY.first + richPhoton->getY(), pmtXY.second + richPhoton->getX());
	    }
	} else {
	    //cout << "No Pmt  " <<richPhoton->getPmtId() << " " << pmtXY.first << " " <<  pmtXY.second  << endl;
	}

	fHM->H1("fhRichPhotonsEnergy")->Fill(richPhoton->getEnergy());
	Double_t energy = richPhoton->getEnergy();

	Double_t lambda = HRich700Pmt::getWavelength(energy);
	fHM->H1("fhRichPhotonsWavelength")->Fill(lambda);

	//Float_t x,y,z;
	//kineTrack->getVertex(x,y,z);
    }

    Int_t nofRichDirects = fCatRichDirect->getEntries();
    for (Int_t i = 0; i < nofRichDirects; i++) {
	HGeantRichDirect* richDirect =  static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
	pair<Double_t, Double_t> pmtXY = fDigiPar->getPmtCenter(richDirect->getPmtId());
	fHM->H2("fhRichDirectsXY")->Fill(pmtXY.first + richDirect->getX(), pmtXY.second + richDirect->getY());
    }

    Int_t nKine = fCatKine->getEntries();
    for(Int_t i=0; i< nKine; i++){
	HGeantKine* kine =  (HGeantKine*)fCatKine-> getObject(i);
	Int_t nofRichHits = kine->getNRichHits();
	if (isPrimaryElectron(kine) && nofRichHits > 0) {
	    fHM->H2("fhNofRichPhotonsVsThetaElectron")->Fill(kine->getThetaDeg(), nofRichHits);
	    fHM->H2("fhNofRichPhotonsVsPhiElectron")->Fill(kine->getPhiDeg(), nofRichHits);
	}
    }

    Int_t nofRichMirror = fCatRichMirror->getEntries();
    for (Int_t i = 0; i < nofRichMirror; i++) {
	HGeantRichMirror* richMirror = (HGeantRichMirror*) fCatRichMirror->getObject(i);
	Int_t nofPhotons = richMirror->getNumPhot();
	HGeantKine* kine =  (HGeantKine*)fCatKine-> getObject(richMirror->getTrack() - 1);
	if (isPrimaryElectron(kine)) {
	    fHM->H2("fhNofRichMirrorPhotonsVsThetaElectron")->Fill(kine->getThetaDeg(), nofPhotons);
	    fHM->H2("fhNofRichMirrorPhotonsVsPhiElectron")->Fill(kine->getPhiDeg(), nofPhotons);
	}
    }

    Int_t nofRichHits = fCatRichHit->getEntries();
    for (Int_t i = 0; i < nofRichHits; i++) {
	HRichHitSim* richHit = (HRichHitSim*) fCatRichHit->getObject(i);
	Int_t nofCals = richHit->fRich700NofRichCals;
	HGeantKine* kine =  (HGeantKine*)fCatKine->getObject(richHit->track1 - 1);
	if (isPrimaryElectron(kine)) {
	    fHM->H2("fhNofRichCalsVsThetaElectron")->Fill(kine->getThetaDeg(), nofCals);
	    fHM->H2("fhNofRichCalsVsPhiElectron")->Fill(kine->getPhiDeg(), nofCals);
	    fHM->H1("fhRingXYElectron")->Fill(richHit->fRich700CircleCenterX, richHit->fRich700CircleCenterY);
	    fHM->H1("fhRingRadiusVsThetaElectron")->Fill(kine->getThetaDeg(), richHit->fRich700CircleRadius);
	    fHM->H1("fhRingChi2Electron")->Fill(richHit->fRich700CircleChi2 / nofCals);
	}
    }
}

Bool_t HRich700GeoTestQa::isPrimaryElectron(
					    HGeantKine* kine)
{
    if (kine == NULL) return kFALSE;
    return (kine->getID() == 2 || kine->getID() == 3) && kine->isPrimary();
}

void HRich700GeoTestQa::drawHist()
{
    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichPhotonsXY", "hrich_fhRichPhotonsXY", 1000, 800);
	c->cd();
	fHM->Scale("fhRichPhotonsXY", 1./(Double_t)fEventNum);
	HRichDrawHist::DrawH2(fHM->H2("fhRichPhotonsXY"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichPhotonsSecXY", "hrich_fhRichPhotonsSecXY", 1000, 800);
	c->cd();
	fHM->Scale("fhRichPhotonsSecXY", 1./(Double_t)fEventNum);
	HRichDrawHist::DrawH2(fHM->H2("fhRichPhotonsSecXY"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichPhotonsEnergy", "hrich_fhRichPhotonsEnergy", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhRichPhotonsEnergy");
	HRichDrawHist::DrawH1(fHM->H1("fhRichPhotonsEnergy"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichPhotonsWavelength", "hrich_fhRichPhotonsWavelength", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhRichPhotonsWavelength");
	HRichDrawHist::DrawH1(fHM->H1("fhRichPhotonsWavelength"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichDirectsXY", "hrich_fhRichDirectsXY", 1000, 800);
	c->cd();
	fHM->Scale("fhRichDirectsXY", 1./(Double_t)fEventNum);
	HRichDrawHist::DrawH2(fHM->H2("fhRichDirectsXY"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRingXYElectron", "hrich_fhRingXYElectron", 800, 800);
	c->cd();
	fHM->Scale("fhRingXYElectron", 1./(Double_t)fEventNum);
	HRichDrawHist::DrawH2(fHM->H2("fhRingXYElectron"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRingChi2Electron", "hrich_fhRingChi2Electron", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhRingChi2Electron");
	HRichDrawHist::DrawH1(fHM->H1("fhRingChi2Electron"));
    }

    drawH2MeanRms(fHM->H2("fhRingRadiusVsThetaElectron"), "hrich_fhRingRadiusVsThetaElectron");

    drawH2MeanRms(fHM->H2("fhNofRichPhotonsVsThetaElectron"), "hrich_fhNofRichPhotonsVsThetaElectron");
    drawH2MeanRms(fHM->H2("fhNofRichMirrorPhotonsVsThetaElectron"), "hrich_fhNofRichMirrorPhotonsVsThetaElectron");
    drawH2MeanRms(fHM->H2("fhNofRichCalsVsThetaElectron"), "hrich_fhNofRichCalsVsThetaElectron");

    drawH2MeanRms(fHM->H2("fhNofRichPhotonsVsPhiElectron"), "hrich_fhNofRichPhotonsVsPhiElectron");
    drawH2MeanRms(fHM->H2("fhNofRichMirrorPhotonsVsPhiElectron"), "hrich_fhNofRichMirrorPhotonsVsPhiElectron");
    drawH2MeanRms(fHM->H2("fhNofRichCalsVsPhiElectron"), "hrich_fhNofRichCalsVsPhiElectron");
}

void HRich700GeoTestQa::createH2MeanRms(
					TH2D* hist,
					TH1D** meanHist,
					TH1D** rmsHist)
{
    *meanHist = (TH1D*)hist->ProjectionX( (string(hist->GetName()) + "_mean").c_str() )->Clone();
    (*meanHist)->GetYaxis()->SetTitle( ("Mean and RMS. " + string(hist->GetYaxis()->GetTitle()) ).c_str());
    *rmsHist = (TH1D*)hist->ProjectionX((string(hist->GetName()) + "_rms").c_str() )->Clone();
    (*rmsHist)->GetYaxis()->SetTitle( ("RMS. "+ string(hist->GetYaxis()->GetTitle()) ).c_str());
    for (Int_t i = 1; i <= hist->GetXaxis()->GetNbins(); i++){
	stringstream ss;
	ss << string(hist->GetName()) << "_py" << i;
	TH1D* pr = hist->ProjectionY(ss.str().c_str(), i, i);
	if (*meanHist == NULL || pr == NULL) continue;
	(*meanHist)->SetBinContent(i, pr->GetMean());
	(*meanHist)->SetBinError(i, pr->GetRMS());
	(*rmsHist)->SetBinContent(i, pr->GetRMS());    }
}

void HRich700GeoTestQa::drawH2MeanRms(
				      TH2* hist,
				      const string& canvasName)
{
    TH1D* mean, *rms;
    createH2MeanRms((TH2D*)hist, &mean, &rms);
    TCanvas *c = fHM->CreateCanvas(canvasName.c_str(), canvasName.c_str(), 1200, 600);
    c->Divide(2, 1);
    c->cd(1);
    hist->Scale(1./hist->Integral());
    HRichDrawHist::DrawH2(hist);
    HRichDrawHist::DrawH1(mean, kLinear, kLinear, "same", kBlack, 4.);
    c->cd(2);
    TH1D* py = (TH1D*)hist->ProjectionY( (string(hist->GetName())+ "_py" ).c_str() )->Clone();
    HRichDrawHist::DrawH1andFitGauss(py);
    py->Scale(1./py->Integral());
    py->GetYaxis()->SetTitle("Yield");
}


//---------------------------------------------------------------------------
Bool_t HRich700GeoTestQa::finalize()
{
    drawHist();
    fHM->SaveCanvasToImage(string(fOutputDir + "/"));
    return kTRUE;
}
