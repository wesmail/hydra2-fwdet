//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700RecoQa
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hrich700recoqa.h"
#include "hades.h"
#include "hcategory.h"
#include "hevent.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hparset.h"
#include "hspectrometer.h"
#include "richdef.h"
#include "hrichcalsim.h"
#include "hgeantkine.h"
#include "hgeantrich.h"
#include "hrich700histmanager.h"
#include "hrichhitsim.h"
#include "hrich700ringfittercop.h"
#include "hrich700drawhist.h"
#include "hrich700utils.h"



#include "TCanvas.h"

#include <iostream>
#include <sstream>
#include <map>
#include <vector>

using namespace std;


ClassImp(HRich700RecoQa)

    HRich700RecoQa::HRich700RecoQa():
fEventNum(0),
    fMinNofRichCalsAcc(5),
    fTrueQuota(0.7)
{

}

HRich700RecoQa::~HRich700RecoQa()
{

}

Bool_t HRich700RecoQa::init()
{

    fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (NULL == fCatKine) {
	Error("init", "Initializatin of kine category failed, returning...");
	return kFALSE;
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
	Error("init", "Initialization of geant category for catRichHit failed, returning...");
	return kFALSE;
    }

    fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCatRichCal) {
	Error("init", "Initialization of geant category for catRichCal failed, returning...");
	return kFALSE;
    }

    initHist();

    return kTRUE;

}


Bool_t HRich700RecoQa::reinit()
{
    return kTRUE;
}

void HRich700RecoQa::initHist()
{
    fHM = new HRich700HistManager();
    fHM->Create1<TH1D>("fhTrueAllRatioAll"            , "fhTrueAllRatioAll;true/all;Yield"                        , 110, -0.05, 1.05);

    fHM->Create1<TH1D>("fhTrueAllRatioElectron"       , "fhTrueAllRatioElectron;true/all;Yield"                   , 110, -0.05, 1.05);
    fHM->Create2<TH2D>("fhTrueAllRatioVsThetaElectron", "fhTrueAllRatioVsThetaElectron;Theta [deg];true/all;Yield", 30 ,  0.  , 90., 55, -0.05, 1.05);
    fHM->Create1<TH1D>("fhNofTrueCalsElectron"        , "fhNofTrueCalsElectron;Nof cals;Yield"                    , 40 , -0.5 , 39.5);
    fHM->Create1<TH1D>("fhNofWrongCalsElectron"       , "fhNofWrongCalsElectron;Nof cals;Yield"                   , 40 , -0.5 , 39.5);
    fHM->Create1<TH1D>("fhNofAllCalsElectron"         , "fhNofAllCalsElectron;Nof cals;Yield"                     , 40 , -0.5 , 39.5);

    fHM->Create1<TH1D>("fhElectronAccTheta"           , "fhElectronAccTheta;Theta [deg];Yield"                    , 90 ,  0.  , 90.);
    fHM->Create1<TH1D>("fhElectronRecTheta"           , "fhElectronRecTheta;Theta [deg];Yield"                    , 90 ,  0.  , 90.);
    fHM->Create1<TH1D>("fhElectronAccPhi"             , "fhElectronAccPhi;Phi [deg];Yield"                        , 90 ,  0.  , 360.);
    fHM->Create1<TH1D>("fhElectronRecPhi"             , "fhElectronRecPhi;Phi [deg];Yield"                        , 90 ,  0.  , 360.);
    fHM->Create1<TH1D>("fhElectronAccMom"             , "fhElectronAccMom;P [MeV/c];Yield"                        , 100,  0.  , 1500.);
    fHM->Create1<TH1D>("fhElectronRecMom"             , "fhElectronRecMom;P [MeV/c];Yield"                        , 100,  0.  , 1500.);
    fHM->Create1<TH1D>("fhElectronAccNofCals"         , "fhElectronAccNofCals;Number of cals;Yield"               , 30 , -0.5 , 29.5);
    fHM->Create1<TH1D>("fhElectronRecNofCals"         , "fhElectronRecNofCals;Number of cals;Yield"               , 30 , -0.5 , 29.5);

    fHM->Create1<TH1D>("fhGhostsClones"               , "fhGhostsClones;Ghosts and clones;Number per event"       , 2  , -0.5 , 1.5);

    //pair efficiency
    fHM->Create1<TH1D>("fhPairAccTheta"               , "fhPairAccTheta;Theta [deg];Yield"                        , 90 , 0.   , 90.);
    fHM->Create1<TH1D>("fhPairRecTheta"               , "fhPairRecTheta;Theta [deg];Yield"                        , 90 , 0.   , 90.);
}

void HRich700RecoQa::initRichHitMap()
{
    fHitMap.clear();
    Int_t nofRichCals = fCatRichCal->getEntries();
    for (Int_t iC = 0; iC < nofRichCals; iC++) {
	HRichCalSim* calSim = static_cast<HRichCalSim*>(fCatRichCal->getObject(iC));
	if (NULL == calSim) continue;
	Int_t nofTrackIds = calSim->getNofTrackIds();
	for (Int_t iT = 0; iT < nofTrackIds; iT++) {
	    Int_t trackId = calSim->getTrackId(iT);
	    if (trackId < 0) continue;
	    fHitMap[trackId]++;
	}
    }
}


Int_t HRich700RecoQa::execute()
{
    fEventNum++;
    initRichHitMap();
    processEvent();
    fillAccRecHist();

    return 0;
}

void HRich700RecoQa::processEvent()
{
    Int_t nofRichRings = fCatRichHit->getEntries();
    for (Int_t i = 0; i < nofRichRings; i++) {
	HRichHitSim* ring = static_cast<HRichHitSim*>(fCatRichHit->getObject(i));
	if (ring == NULL) continue;
	Int_t trueCals = ring->weigTrack1;
	Int_t trackId = ring->track1;
	Int_t nofCals = ring->fRich700NofRichCals;
	Double_t trueAllRatio = (Double_t)trueCals / (Double_t) nofCals;

	if (trackId <= 0) continue;
	fHM->H1("fhTrueAllRatioAll")->Fill(trueAllRatio);
	HGeantKine* kine = (HGeantKine*)fCatKine-> getObject(trackId - 1);
	if (isPrimaryElectron(kine)) {
	    fHM->H1("fhTrueAllRatioElectron")->Fill(trueAllRatio);
	    fHM->H2("fhTrueAllRatioVsThetaElectron")->Fill(kine->getThetaDeg(), trueAllRatio);
	    fHM->H1("fhNofTrueCalsElectron")->Fill(trueCals);
	    fHM->H1("fhNofWrongCalsElectron")->Fill(nofCals - trueCals);
	    fHM->H1("fhNofAllCalsElectron")->Fill(nofCals);
	}
    }
}

void HRich700RecoQa::fillAccRecHist()
{
    Int_t nofAccEl = 0;
    Double_t thetaAcc[2];
    Int_t nofKine = fCatKine->getEntries();
    for (Int_t i = 0; i < nofKine; i++) {
	HGeantKine* kine = (HGeantKine*)fCatKine-> getObject(i);
	Double_t theta = kine->getThetaDeg();
	Double_t phi = kine->getPhiDeg();
	Double_t mom = kine->getTotalMomentum();
	Int_t nofCals = fHitMap[kine->getTrack()];
	if (isPrimaryElectron(kine) && isRichAcc(kine->getTrack())) {
	    fHM->H1("fhElectronAccTheta")->Fill(theta);
	    fHM->H1("fhElectronAccPhi")->Fill(phi);
	    fHM->H1("fhElectronAccMom")->Fill(mom);
	    fHM->H1("fhElectronAccNofCals")->Fill(nofCals);
	    if (nofAccEl <= 1) thetaAcc[nofAccEl] = theta;
	    nofAccEl++;
	}
    }
    if (nofAccEl == 2){
	fHM->H1("fhPairAccTheta")->Fill(0.5 * (thetaAcc[0] + thetaAcc[1]));
    }

    Int_t nofRecEl = 0;
    Double_t thetaRec[2];
    Int_t nofRichRings = fCatRichHit->getEntries();
    map<Int_t, Int_t> clonesMap;
    for (Int_t i = 0; i < nofRichRings; i++) {
	HRichHitSim* ring = static_cast<HRichHitSim*>(fCatRichHit->getObject(i));
	if (ring == NULL) continue;
	Int_t trackId = ring->track1;
	Double_t trueAllRatio = (Double_t)ring->weigTrack1 / (Double_t) ring->fRich700NofRichCals;
	Bool_t isTrue = (trueAllRatio >= fTrueQuota);
	if (trackId <= 0){
	    fHM->H1("fhGhostsClones")->Fill(0.);
	    continue;
	}
	HGeantKine* kine = (HGeantKine*)fCatKine-> getObject(trackId - 1);
	if (kine == NULL) continue;
	Double_t theta = kine->getThetaDeg();
	Double_t phi = kine->getPhiDeg();
	Double_t mom = kine->getTotalMomentum();
	Int_t nofCals = fHitMap[trackId];
	Bool_t isClone = (clonesMap[trackId] > 0);
	if (isPrimaryElectron(kine) && isRichAcc(trackId) && isTrue && !isClone) {
	    fHM->H1("fhElectronRecTheta")->Fill(theta);
	    fHM->H1("fhElectronRecPhi")->Fill(phi);
	    fHM->H1("fhElectronRecMom")->Fill(mom);
	    fHM->H1("fhElectronRecNofCals")->Fill(nofCals);
	    if (nofRecEl <= 1) thetaRec[nofRecEl] = theta;
	    nofRecEl++;
	    clonesMap[trackId]++;
	}

	if (!isTrue) {
	    fHM->H1("fhGhostsClones")->Fill(0.);
	}
	if (isClone) {
	    fHM->H1("fhGhostsClones")->Fill(1.);
	}
    }
    if (nofRecEl == 2){
	fHM->H1("fhPairRecTheta")->Fill(0.5*(thetaRec[0] + thetaRec[1]));
    }
}

Bool_t HRich700RecoQa::isRichAcc(
				 Int_t trackId)
{
    if (fHitMap[trackId] >= fMinNofRichCalsAcc) return kTRUE;
    return kFALSE;
}

Bool_t HRich700RecoQa::isPrimaryElectron(
					 HGeantKine* kine)
{
    if (kine == NULL) return kFALSE;
    return ((kine->getID() == 2 || kine->getID() == 3) && kine->isPrimary());
}

void HRich700RecoQa::drawHist()
{
    HRichDrawHist::SetDefaultDrawStyle();
    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhTrueAllRatioAll", "hrich_fhTrueAllRatioAll", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhTrueAllRatioAll");
	HRichDrawHist::DrawH1(fHM->H1("fhTrueAllRatioAll"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhTrueAllRatioElectron", "hrich_fhTrueAllRatioElectron", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhTrueAllRatioElectron");
	HRichDrawHist::DrawH1(fHM->H1("fhTrueAllRatioElectron"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhTrueAllRatioVsThetaElectron", "hrich_fhTrueAllRatioVsThetaElectron", 1000, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhTrueAllRatioVsThetaElectron");
	HRichDrawHist::DrawH2(fHM->H2("fhTrueAllRatioVsThetaElectron"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhNofCalsElectron", "hrich_fhNofCalsElectron", 800, 800);
	c->cd();
	fHM->NormalizeToIntegral("fhNofTrueCalsElectron");
	fHM->NormalizeToIntegral("fhNofWrongCalsElectron");
	fHM->NormalizeToIntegral("fhNofAllCalsElectron");
	vector<TH1*> hist;
	hist.push_back(fHM->H1("fhNofTrueCalsElectron"));
	hist.push_back(fHM->H1("fhNofWrongCalsElectron"));
	hist.push_back(fHM->H1("fhNofAllCalsElectron"));
	vector<string> labels;
	stringstream ss;
	ss.precision(2);
	ss << "True (" << fixed << fHM->H1("fhNofTrueCalsElectron")->GetMean() << ")";
	labels.push_back(ss.str());
	ss.str("");
	ss << "Wrong (" << fixed << fHM->H1("fhNofWrongCalsElectron")->GetMean() << ")";
	labels.push_back(ss.str());
	ss.str("");
	ss << "All (" << fixed << fHM->H1("fhNofAllCalsElectron")->GetMean() << ")";
	labels.push_back(ss.str());
	HRichDrawHist::DrawH1(hist, labels);
	gPad->SetLogy(kTRUE);
    }

    drawAccRecEff("hrich_fhElectronAccRecTheta", "hrich_fhElectronEffTheta", "fhElectronAccTheta", "fhElectronRecTheta");
    drawAccRecEff("hrich_fhElectronAccRecPhi", "hrich_fhElectronEffPhi", "fhElectronAccPhi", "fhElectronRecPhi");
    drawAccRecEff("hrich_fhElectronAccRecMom", "hrich_fhElectronEffMom", "fhElectronAccMom", "fhElectronRecMom");
    drawAccRecEff("hrich_fhElectronAccRecNofCals", "hrich_fhElectronEffNofCals", "fhElectronAccNofCals", "fhElectronRecNofCals");

    drawAccRecEff("hrich_fhPairAccRecTheta", "hrich_fhPairEffTheta", "fhPairAccTheta", "fhPairRecTheta");

    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhGhostsClones", "hrich_fhGhostsClones", 800, 800);
	c->cd();
	fHM->Scale("fhGhostsClones", 1./ (Double_t)fEventNum);
	HRichDrawHist::DrawH1(fHM->H1("fhGhostsClones"));
    }

}

void HRich700RecoQa::drawAccRecEff(
				   const string& canvasNameAccRec,
				   const string& canvasNameEff,
				   const string& histNameAcc,
				   const string& histNameRec)
{
    {
	TCanvas* c = fHM->CreateCanvas(canvasNameAccRec.c_str(), canvasNameAccRec.c_str(), 800, 800);
	c->cd();
	vector<TH1*> hist;
	hist.push_back(fHM->H1(histNameAcc));
	hist.push_back(fHM->H1(histNameRec));
	vector<string> labels;
	stringstream ss;
	ss.precision(3);
	ss << "Acc (" << fixed << fHM->H1(histNameAcc)->GetEntries() / (Double_t)fEventNum << ")";
	labels.push_back(ss.str());
	ss.str("");
	ss << "Rec (" << fixed << fHM->H1(histNameRec)->GetEntries() / (Double_t)fEventNum << ")";
	labels.push_back(ss.str());
	HRichDrawHist::DrawH1(hist, labels);
    }

    {
	TH1D* pxEff = RichUtils::DivideH1((TH1D*)fHM->H1(histNameRec)->Clone(), (TH1D*)fHM->H1(histNameAcc)->Clone(), "", 100., "Efficiency [%]");
	pxEff->SetMinimum(0.);
	TCanvas* c = fHM->CreateCanvas(canvasNameEff.c_str(), canvasNameEff.c_str(), 800, 800);
	c->cd();
	vector<TH1*> hist;
	hist.push_back(pxEff);
	vector<string> labels;
	stringstream ss;
	ss.precision(1);
	ss << "Electrons (" << fixed << 100.*fHM->H1(histNameRec)->GetEntries() / fHM->H1(histNameAcc)->GetEntries() << "%)";
	labels.push_back(ss.str());
	HRichDrawHist::DrawH1(hist, labels);
    }
}

Bool_t HRich700RecoQa::finalize()
{
    drawHist();
    fHM->SaveCanvasToImage(string(fOutputDir + "/"));
    return kTRUE;
}
