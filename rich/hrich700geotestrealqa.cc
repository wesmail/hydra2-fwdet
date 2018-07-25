//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700GeoTestRealQa
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700geotestrealqa.h"
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
#include "hrichhit.h"
#include "hrichcal.h"
#include "hrich700raw.h"

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


ClassImp(HRich700GeoTestRealQa)

    HRich700GeoTestRealQa::HRich700GeoTestRealQa():
fEventNum(0)
{

}

HRich700GeoTestRealQa::~HRich700GeoTestRealQa()
{

}

Bool_t HRich700GeoTestRealQa::init()
{
    fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCatRichCal) {
       Error("init()", "Initialization of catRicCal category failed, returning...");
       return kFALSE;
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
	   Error("init", "Initializatin of RICH hit category failed, returning...");
	   return kFALSE;
    }

    fCatRichRaw = gHades->getCurrentEvent()->getCategory(catRich700Raw);
    if (NULL == fCatRichRaw) {
        Error("init", "Initializatin of RICH raw category failed, returning...");
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

void HRich700GeoTestRealQa::initHist()
{
    fHM = new HRich700HistManager();
    fHM->Create2<TH2D>("fhRichCalsColRow", "fhRichCalsColRow;Col index;Row index;Entries", 194,-0.5, 193.5, 194, -0.5, 193.5);
    fHM->Create2<TH2D>("fhRichCalsXY", "fhRichCalsXY;X [mm];Y [mm];Entries", 175,-700, 700, 175, -700, 700);
    fHM->Create1<TH1D>("fhRichCalsPmtId", "fhRichCalsPmtId;PMT ID;Entries", 600,-0.5, 599.5);
    fHM->Create2<TH2D>("fhRichCalsPmtXPmtY", "fhRichCalsPmtXPmtY;PMT X;PMT Y;Entries", 25,-0.5, 24.5, 25, -0.5, 24.5);
    fHM->Create1<TH1D>("fhRichCalsSectors", "fhRichCalsSectors;Sector;Entries", 6,-0.5, 5.5);

    fHM->Create1<TH1D>("fhRichCalsTheta", "fhRichCalsTheta;Theta [deg];Entries", 100, 0., 100.);
    fHM->Create1<TH1D>("fhRichCalsPhi", "fhRichCalsPhi;Phi [deg];Entries", 360, 0., 360.);
    fHM->Create1<TH1D>("fhRichCalsSectorsInterp", "fhRichCalsSectorsInterp;Sector;Entries", 6,-0.5, 5.5);

    for (Int_t iS = 0; iS < 6; iS++) {
        stringstream ss;
        ss << "fhRichCalsThetaForSector" << iS;
        fHM->Create1<TH1D>(ss.str().c_str() , (ss.str() + ";Theta [deg];Entries").c_str(), 100, 0., 100.);
    }

    fHM->Create2<TH2D>("fhRichRawPmtPixel", "fhRichRawPmtPixel;PMT;Pixel;Entries", 600,-0.5, 599.5, 65, -0.5, 64.5);
    fHM->Create1<TH1D>("fhRichRawMult", "fhRichRawMult;Multiplicity;Entries", 10,-0.5, 9.5);
}

Bool_t HRich700GeoTestRealQa::reinit()
{
    return kTRUE;
}


Int_t HRich700GeoTestRealQa::execute()
{
    HRichDrawHist::SetDefaultDrawStyle();
    fEventNum++;
    cout << "HRich700GeoTestRealQa::execute eventNum " << fEventNum << endl;

    fillHistCals();
    fillHistRaw();
    return 0;
}

void HRich700GeoTestRealQa::fillHistCals()
{
    Int_t nofRichCals = fCatRichCal->getEntries();
    for (Int_t iC = 0; iC < nofRichCals; iC++) {
       HRichCal* cal = static_cast<HRichCal*>(fCatRichCal->getObject(iC));
       if (NULL == cal) continue;
       Int_t col = cal->getCol();
       Int_t row = cal->getRow();
       Int_t sector = cal->getSector();
       fHM->H2("fhRichCalsColRow")->Fill(col, row);
       fHM->H1("fhRichCalsSectors")->Fill(sector);

       Int_t loc[3];
       loc[0] = cal->getSector();
       loc[1] = col;
       loc[2] = row;
       pair<Double_t, Double_t> xy = fDigiPar->getXY(loc);
       fHM->H2("fhRichCalsXY")->Fill(xy.first, xy.second);

       Int_t pmtId = fDigiPar->getPMTId(col, row);
       fHM->H1("fhRichCalsPmtId")->Fill(pmtId);

       HRich700PmtData* pmtData = fDigiPar->getPMTData(pmtId);
       if (pmtData != NULL) {
           fHM->H2("fhRichCalsPmtXPmtY")->Fill(pmtData->fIndX, pmtData->fIndY);
       }

       Float_t phiDeg, thetaDeg;
       Int_t sectorInterp = fDigiPar->getSectorPhiThetaDegPixels(col, row, phiDeg, thetaDeg);
       fHM->H1("fhRichCalsPhi")->Fill(phiDeg);
       fHM->H1("fhRichCalsTheta")->Fill(thetaDeg);
       fHM->H1("fhRichCalsSectorsInterp")->Fill(sectorInterp);

       stringstream ss;
       ss << "fhRichCalsThetaForSector" << sectorInterp;
       fHM->H1(ss.str())->Fill(thetaDeg);
    }
}

void HRich700GeoTestRealQa::fillHistRaw()
{
    Int_t nofRichRaws = fCatRichRaw->getEntries();
    for (Int_t iR = 0; iR < nofRichRaws; iR++){
       HRich700Raw* raw = static_cast<HRich700Raw*>(fCatRichRaw->getObject(iR));
       if (NULL == raw) continue;
       Int_t pmtId = raw->getPMT();
       Int_t pixel = raw->getPixel();
       Int_t mult = raw->getMultiplicity();
       fHM->H2("fhRichRawPmtPixel")->Fill(pmtId, pixel);
       fHM->H1("fhRichRawMult")->Fill(mult);
    }
}

void HRich700GeoTestRealQa::drawHist()
{
    {
	TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsColRow", "hrich_fhRichCalsColRow", 1000, 800);
	HRichDrawHist::DrawH2(fHM->H2("fhRichCalsColRow"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsXY", "hrich_fhRichCalsXY", 1000, 800);
    HRichDrawHist::DrawH2(fHM->H2("fhRichCalsXY"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsPmtId", "hrich_fhRichCalsPmtId", 1000, 800);
    HRichDrawHist::DrawH1(fHM->H1("fhRichCalsPmtId"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsPmtXPmtY", "hrich_fhRichCalsPmtXPmtY", 1000, 800);
    HRichDrawHist::DrawH2(fHM->H2("fhRichCalsPmtXPmtY"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsSector", "hrich_fhRichCalsSector", 1000, 800);
    HRichDrawHist::DrawH1(fHM->H1("fhRichCalsSectors"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsPhiThetaInterp", "hrich_fhRichCalsPhiThetaInterp", 1500, 500);
    c->Divide(3, 1);
    c->cd(1);
    HRichDrawHist::DrawH1(fHM->H1("fhRichCalsSectorsInterp"));
    c->cd(2);
    HRichDrawHist::DrawH1(fHM->H1("fhRichCalsPhi"));
    c->cd(3);
    HRichDrawHist::DrawH1(fHM->H1("fhRichCalsTheta"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichCalsThetaForSector", "hrich_fhRichCalsThetaForSector", 1500, 1000);
    c->Divide(3, 2);
    for (Int_t iS = 0; iS < 6; iS++) {
        c->cd(iS+1);
        stringstream ss;
        ss << "fhRichCalsThetaForSector" << iS;
        HRichDrawHist::DrawH1(fHM->H1(ss.str()));
        stringstream ss2;
        ss2 << "Sector " << iS;
        HRichDrawHist::DrawTextOnPad(ss2.str(), 0.25, 0.9, 0.75, 0.99);
    }
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichRawPmtPixel", "hrich_fhRichRawPmtPixel", 1600, 600);
    HRichDrawHist::DrawH2(fHM->H2("fhRichRawPmtPixel"));
    gPad->SetLeftMargin(0.07);
    gPad->SetRightMargin(0.12);
    fHM->H2("fhRichRawPmtPixel")->GetYaxis()->SetTitleOffset(0.5);
    fHM->H2("fhRichRawPmtPixel")->GetZaxis()->SetTitleOffset(0.6);
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrich_fhRichRawMult", "hrich_fhRichRawMult", 1000, 800);
    HRichDrawHist::DrawH1(fHM->H1("fhRichRawMult"));
    }

}

//---------------------------------------------------------------------------
Bool_t HRich700GeoTestRealQa::finalize()
{
    drawHist();
    fHM->SaveCanvasToImage(string(fOutputDir + "/"));
    return kTRUE;
}
