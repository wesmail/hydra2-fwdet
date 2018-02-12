//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700DigiParCreator
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700digiparcreator.h"
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
#include "htool.h"
#include "hhistconverter.h"

#include "TCanvas.h"
#include "TH2D.h"
#include "TPad.h"
#include "TEllipse.h"
#include "TRandom.h"
#include "TLatex.h"
#include "TStyle.h"

#include "hgeantkine.h"

#include "hrich700histmanager.h"
#include "hrich700digipar.h"
#include "hrich700utils.h"

#include <iostream>
#include <sstream>
#include <fstream>

using namespace std;


ClassImp(HRich700DigiParCreator)

HRich700DigiParCreator::HRich700DigiParCreator():
fEventNum(0),
fOutputFilePath("richpar_output.dat"),
fOutputHistFilePath("richpar_hist.root")
{

}

HRich700DigiParCreator::~HRich700DigiParCreator()
{

}

Bool_t HRich700DigiParCreator::init()
{
    fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (NULL == fCatKine) {
    	Error("init", "Initializatin of kine category failed, returning...");
    	return kFALSE;
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
    	Error("init", "Initializatin of RICH hit category failed, returning...");
    	return kFALSE;
    }

    fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(fDigiPar == NULL) {
	    Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }

    fRingFinderPar = (HRich700RingFinderPar*) gHades->getRuntimeDb()->getContainer("Rich700RingFinderPar");
    if(fRingFinderPar == NULL) {
        Error("init", "Can not retrieve fRingFinderPar");
        return kFALSE;
    }

    initHist();
    return kTRUE;
}

void HRich700DigiParCreator::initHist()
{
    // XY [mm]
    for(Int_t i = 0; i < knPmt; i++){
        fhPhi[i] = new TH1F(Form("fhPhi[%i]", i), Form("fhPhi[%i];Phi [deg];Counter", i), 1440, 0, 360);
        fhTheta[i] = new TH1F(Form("fhTheta[%i]", i), Form("fhTheta[%i];Theta [deg];Counter", i), 360, 0, 90);
    }
    const Double_t hMinXY = -660.;
    const Double_t hMaxXY = 660;
    fhXYCounter = new TH2F("fhXYCounter", "fhXYCounter;X [mm];Y [mm];Counter", knXY, hMinXY, hMaxXY, knXY, hMinXY, hMaxXY);
    fhXYPhiMean = new TH2F("fhXYPhiMean", "fhXYPhiMean;X [mm];Y [mm];Phi_{mean} [deg]", knXY, hMinXY, hMaxXY, knXY, hMinXY, hMaxXY);
    fhXYThetaMean = new TH2F("fhXYThetaMean", "fhXYThetaMean;X [mm];Y [mm];Theta_{mean} [deg]", knXY,  hMinXY, hMaxXY, knXY, hMinXY, hMaxXY);
    fhXYPhiRms = new TH2F("fhXYPhiRms", "fhXYPhiRms;X [mm];Y [mm];Phi_{RMS} [deg]", knXY, hMinXY, hMaxXY, knXY, hMinXY, hMaxXY);
    fhXYThetaRms = new TH2F("fhXYThetaRms", "fhXYThetaRms;X [mm];Y [mm];Theta_{RMS} [deg]", knXY, hMinXY, hMaxXY, knXY, hMinXY, hMaxXY);


    // XY pmtIndX, pmtIndY
    for(Int_t i = 0; i < knXY; i++){
        for(Int_t j = 0; j < knXY; j++){
            fhPhiXY[i][j] = new TH1F(Form("fhPhiXY[%i][%i]", i, j), Form("fhPhiXY[%i][%i];Phi [deg];Counter", i, j), 1440, 0, 360);
            fhThetaXY[i][j] = new TH1F(Form("fhThetaXY[%i][%i]", i, j), Form("fhThetaXY[%i][%i];Theta [deg];Counter", i, j), 360, 0, 90);
        }
    }
    const Double_t hMinInd = -0.5;
    const Double_t hMaxInd = 23.5;
    const Int_t nofBinsInd = 24;
    fhXYIndCounter = new TH2F("fhXYIndCounter", "fhXYIndCounter;PMT X_{ind};PMT Y_{ind};Counter", nofBinsInd, hMinInd, hMaxInd, nofBinsInd, hMinInd, hMaxInd);
    fhXYIndPhiMean = new TH2F("fhXYIndPhiMean", "fhXYIndPhiMean;PMT X_{ind};PMT Y_{ind};Phi_{mean} [deg]", nofBinsInd, hMinInd, hMaxInd, nofBinsInd, hMinInd, hMaxInd);
    fhXYIndThetaMean = new TH2F("fhXYIndThetaMean", "fhXYIndThetaMean;PMT X_{ind};PMT Y_{ind};Theta_{mean} [deg]", nofBinsInd, hMinInd, hMaxInd, nofBinsInd, hMinInd, hMaxInd);
    fhXYIndPhiRms = new TH2F("fhXYIndPhiRms", "fhXYIndPhiRms;PMT X_{ind};PMT Y_{ind};Phi_{RMS} [deg]", nofBinsInd, hMinInd, hMaxInd, nofBinsInd, hMinInd, hMaxInd);
    fhXYIndThetaRms = new TH2F("fhXYIndThetaRms", "fhXYIndThetaRms;PMT X_{ind};PMT Y_{ind};Theta_{RMS} [deg]", nofBinsInd, hMinInd, hMaxInd, nofBinsInd, hMinInd, hMaxInd);

    // interpolation
    fhXYPhiInterpolation = new TH2F("fhXYPhiInterpolation", "fhXYPhiInterpolation;X [mm];Y [mm];Phi", 200, -636, 636, 200, -636, 636);
    fhXYThetaInterpolation = new TH2F("fhXYThetaInterpolation", "fhXYThetaInterpolation;X [mm];Y[mm];Theta", 200, -636, 636, 200, -636, 636);
}

Bool_t HRich700DigiParCreator::reinit()
{
    return kTRUE;
}


Int_t HRich700DigiParCreator::execute()
{
    HRichDrawHist::SetDefaultDrawStyle();
    fEventNum++;
    if (fEventNum % 10000 == 0) cout << "HRich700DigiParCreator::execute eventNum " << fEventNum << endl;

    processEvent();

    return 0;
}

void HRich700DigiParCreator::processEvent()
{
    Int_t nofRichHits = fCatRichHit->getEntries();
    for (Int_t i = 0; i < nofRichHits; i++) {
    	HRichHitSim* richHit = (HRichHitSim*) fCatRichHit->getObject(i);
        if (richHit == NULL) continue;
        HGeantKine* kine = (HGeantKine*)fCatKine->getObject(richHit->track1 - 1);
        // select only rings from primary electrons
    	if (!isPrimaryElectron(kine)) continue;

        Int_t pmtId = fDigiPar->getPMTId((Float_t)richHit->fRich700CircleCenterX, (Float_t)richHit->fRich700CircleCenterY);
        if (pmtId < 0) {
            //cout << "HRich700DigiParCreator::processEvent pmtId is not correct:" << pmtId << endl;
            continue;
        }

        Float_t theta = 0;
        Float_t phi = 0;
        Int_t gsec = kine->getPhiThetaDeg(theta, phi);
        HRich700PmtData* richdata = fDigiPar->getPMTData(pmtId);
        Double_t ringX = richHit->fRich700CircleCenterX;
        Double_t ringY = richHit->fRich700CircleCenterY;
        Int_t sec = fDigiPar->getSector(ringX, ringY);
        // PMT
        Bool_t isGoodXdY = ( fabs(ringX - richdata->fX) < 5. && fabs(ringY - richdata->fY) < 5. );
        if(isGoodXdY && richHit->fRich700CircleChi2 < 500. &&  sec == gsec){
            fhPhi[pmtId]->Fill(phi);
            fhTheta[pmtId]->Fill(theta);
            fhXYIndCounter->Fill(richdata->fIndX, richdata->fIndY);
        }

        // x,y [mm]
        if(richHit->fRich700CircleChi2 < 500. &&  sec == gsec){
            fhXYCounter->Fill(ringX, ringY);

            Int_t xBin = fhXYCounter->GetXaxis()->FindBin(ringX) - 1;
            Int_t yBin = fhXYCounter->GetYaxis()->FindBin(ringY) - 1;
            if (xBin < 0 || xBin >= knXY || yBin < 0 || yBin >= knXY) {
                //cout << "processEvent: xBin < 0 || xBin >= knXY || yBin < 0 || yBin >= knXY xBin:" << xBin << " yBin:" << yBin  << endl;
            } else {
                fhPhiXY[xBin][yBin]->Fill(phi);
                fhThetaXY[xBin][yBin]->Fill(theta);
            }
        }
    }
}

Double_t HRich700DigiParCreator::calculateHistMean(TH1* hist, Double_t minVal, Double_t maxVal)
{
    if (hist == NULL) return 0.;
    Double_t width = 5.;
    Double_t mean = hist->GetMean();
    hist->GetXaxis()->SetRangeUser(mean > width? mean - width : minVal, mean < maxVal - width? mean + width : maxVal);
    mean = hist->GetMean();
    width = 5. * hist->GetRMS();
    hist->GetXaxis()->SetRangeUser(mean > width? mean - width: minVal, mean < maxVal - width? mean + width : maxVal);
    return hist->GetMean();
}

void HRich700DigiParCreator::createOutputFile()
{
    string rfStr = fRingFinderPar->getStringForParTxtFile();
    string digiStr = fDigiPar->getStringForParTxtFile();

    string digiPmtIdStr = createStringPhiThetaPmtId();
    string digiXYStr = createStringPhiThetaXY();

    ofstream fout;
    fout.open(fOutputFilePath.c_str());
    fout << rfStr << digiStr << digiPmtIdStr << digiXYStr;
    fout << "##############################################################################" <<endl;
    fout.close();

}

string HRich700DigiParCreator::createStringPhiThetaPmtId()
{
    ostringstream ss;
    ss <<"fArrayPmt:  Double_t \\" << endl;
    for(Int_t i = 0; i < knPmt; i++){
        HRich700PmtData* pd = fDigiPar->getPMTData(i);
        if(pd == NULL){
            cout<<" NO pmt for id "<< i <<endl;
            continue;
        }

        Double_t phiMean = calculateHistMean(fhPhi[i], 0., 360.);
        Double_t thetaMean = calculateHistMean(fhTheta[i], 0., 90.);
        if (fhPhi[i]->Integral() < 4){
            phiMean = 0.;
            thetaMean = 0.;
        }

        ss << pd->fPmtId << " " << pd->fIndX << " " << pd->fIndY << " " << pd->fX << " "
           << pd->fY << " " << pd->fZ << " " << pd->fPmtType << " " << thetaMean << " "
           << phiMean << " \\" << endl;

        fhXYIndPhiMean->Fill(pd->fIndX, pd->fIndY, phiMean);
        fhXYIndThetaMean->Fill(pd->fIndX, pd->fIndY, thetaMean);
        fhXYIndPhiRms->Fill(pd->fIndX, pd->fIndY, fhPhi[i]->GetRMS());
        fhXYIndThetaRms->Fill(pd->fIndX, pd->fIndY, fhTheta[i]->GetRMS());
    }
    // remove last " \" and endl symbols
    string str = ss.str().substr(0, ss.str().size() - 3);
    str = string(str + "\n");
    cout << str << endl;

    return str;
    //fclose(pFile);
}

string HRich700DigiParCreator::createStringPhiThetaXY()
{
    ostringstream ss;
    for(Int_t i = 0; i < knXY; i++){
    	for(Int_t j = 0; j < knXY; j++){
            TH1F* hth = fhThetaXY[i][j];
            TH1F* hph = fhPhiXY[i][j];

            Double_t phiMean = calculateHistMean(hph, 0., 360.);
            Double_t thetaMean = calculateHistMean(hth, 0., 90.);
            if (hph->Integral() < 4){
                phiMean = 0.;
                thetaMean = 0.;
            }

    		fhXYPhiMean->SetBinContent(i+1, j+1, phiMean);
    		fhXYThetaMean->SetBinContent(i+1, j+1, thetaMean);
    		fhXYPhiRms->SetBinContent(i+1, j+1, hph->GetRMS());
    		fhXYThetaRms->SetBinContent(i+1, j+1, hth->GetRMS());
    	}
    }

    TArrayD linDataPhi;
    TArrayD linDataTheta;
    HHistConverter::fillArray(fhXYPhiMean, linDataPhi, "fArrayPhiMean", 10, 10, kFALSE);
    HHistConverter::fillArray(fhXYThetaMean, linDataTheta, "fArrayThetaMean", 10, 10, kFALSE);

    //ofstream fout;
    //fout.open(fOutputPhiThetaXYFilePath.c_str());
    HHistConverter::writeArray(ss, "fArrayPhiMean", linDataPhi, 10);
    HHistConverter::writeArray(ss, "fArrayThetaMean", linDataTheta, 10);
    //fout.close();
    return ss.str();
}

void HRich700DigiParCreator::fillInterpolatedHist()
{
    // plot interoiolated hists from PMT x,y theta,phi (low RES!)
    for(Int_t i = 0; i < 200; i++){
    	for(Int_t j = 0; j < 200; j++){
    	    Float_t x = fhXYCounter->GetXaxis()->GetBinCenter(i+1);
    	    Float_t y = fhXYCounter->GetYaxis()->GetBinCenter(j+1);
    	    Float_t phi,theta;
    	    if(fDigiPar->getInterpolatedThetaPhiPMT(x, y, theta, phi)){
                fhXYPhiInterpolation->SetBinContent(i+1, j+1, phi) ;
                fhXYThetaInterpolation->SetBinContent(i+1, j+1, theta) ;
    	    }
    	}
    }
}

void HRich700DigiParCreator::writeHist()
{
    TFile* out = new TFile(fOutputHistFilePath.c_str(), "RECREATE");
    out->cd();

    fhXYIndCounter->SetOption("colz");
    fhXYIndPhiMean->SetOption("colz");
    fhXYIndThetaMean->SetOption("colz");
    fhXYIndPhiRms->SetOption("colz");
    fhXYIndThetaRms->SetOption("colz");

    fhXYPhiInterpolation->SetOption("colz");
    fhXYThetaInterpolation->SetOption("colz");

    fhXYCounter->SetOption("colz");
    fhXYPhiMean->SetOption("colz");
    fhXYThetaMean->SetOption("colz");
    fhXYPhiRms->SetOption("colz");
    fhXYThetaRms->SetOption("colz");

    fhXYIndCounter->Write();
    fhXYIndPhiMean->Write();
    fhXYIndThetaMean->Write();
    fhXYIndPhiRms->Write();
    fhXYIndThetaRms->Write();

    fhXYPhiInterpolation->Write();
    fhXYThetaInterpolation->Write();

    fhXYCounter->Write();
    fhXYPhiMean->Write();
    fhXYThetaMean->Write();
    fhXYPhiRms->Write();
    fhXYThetaRms->Write();

    HTool::changeToDir("phi");
    for(Int_t i = 0; i < knPmt; i++){
        fhPhi[i]->Write();
    }

    out->cd();
    HTool::changeToDir("theta");
    for(Int_t i = 0; i < knPmt;i++){
        fhTheta[i]->Write();
    }

    out->cd();
    HTool::changeToDir("thetaxy");

    for(Int_t i = 0; i < knXY; i++){
        for(Int_t j=0; j < knXY; j++){
            fhThetaXY[i][j]->Write();
        }
    }
    out->cd();

    HTool::changeToDir("phixy");
    for(Int_t i = 0; i < knXY; i++){
        for(Int_t j = 0; j < knXY; j++){
            fhPhiXY[i][j]->Write();
        }
    }
    out->Save();
    out->Write();
}

Bool_t HRich700DigiParCreator::isPrimaryElectron(
					    HGeantKine* kine)
{
    if (kine == NULL) return kFALSE;
    return (kine->getID() == 2 || kine->getID() == 3) && kine->isPrimary();
}


Bool_t HRich700DigiParCreator::finalize()
{
    createOutputFile();
    fillInterpolatedHist();

    writeHist();

    cout << "Output files:" << endl;
    cout << "Par file:" << fOutputFilePath << endl;
    cout << "Histograms:" << fOutputHistFilePath << endl;

    return kTRUE;
}

void HRich700DigiParCreator::drawHist()
{
    {
	TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYCounter", "hrichdigipar_fhXYCounter", 1000, 800);
	HRichDrawHist::DrawH2(fHM->H2("fhXYCounter"));
    }

    {
	TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYMean", "hrichdigipar_fhXYMean", 1500, 700);
    c->Divide(2,1);
    c->cd(1);
	HRichDrawHist::DrawH2(fHM->H2("fhXYPhiMean"));
    c->cd(2);
    HRichDrawHist::DrawH2(fHM->H2("fhXYThetaMean"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYRms", "hrichdigipar_fhXYRms", 1500, 700);
    c->Divide(2,1);
    c->cd(1);
    fHM->H2("fhXYPhiRms")->GetZaxis()->SetRangeUser(0., 2.);
    HRichDrawHist::DrawH2(fHM->H2("fhXYPhiRms"));
    c->cd(2);
    fHM->H2("fhXYThetaRms")->GetZaxis()->SetRangeUser(0., 2.);
    HRichDrawHist::DrawH2(fHM->H2("fhXYThetaRms"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYIndCounter", "hrichdigipar_fhXYIndCounter", 1000, 800);
    HRichDrawHist::DrawH2(fHM->H2("fhXYIndCounter"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYIndMean", "hrichdigipar_fhXYIndMean", 1500, 700);
    c->Divide(2,1);
    c->cd(1);
    HRichDrawHist::DrawH2(fHM->H2("fhXYIndPhiMean"));
    c->cd(2);
    HRichDrawHist::DrawH2(fHM->H2("fhXYIndThetaMean"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYIndRms", "hrichdigipar_fhXYIndRms", 1500, 700);
    c->Divide(2,1);
    c->cd(1);
    HRichDrawHist::DrawH2(fHM->H2("fhXYIndPhiRms"));
    c->cd(2);
    HRichDrawHist::DrawH2(fHM->H2("fhXYIndThetaRms"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhXYInterpolation", "hrichdigipar_fhXYInterpolation", 1500, 700);
    c->Divide(2,1);
    c->cd(1);
    HRichDrawHist::DrawH2(fHM->H2("fhXYPhiInterpolation"));
    c->cd(2);
    HRichDrawHist::DrawH2(fHM->H2("fhXYThetaInterpolation"));
    }


    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhPhi_fhTheta", "hrichdigipar_fhPhi_fhTheta", 1500, 700);
    c->Divide(3,2);
    c->cd(1);
    HRichDrawHist::DrawH1(fHM->H1("fhPhi[183]"));
    c->cd(2);
    HRichDrawHist::DrawH1(fHM->H1("fhPhi[125]"));
    c->cd(3);
    HRichDrawHist::DrawH1(fHM->H1("fhPhi[350]"));

    c->cd(4);
    HRichDrawHist::DrawH1(fHM->H1("fhTheta[200]"));
    c->cd(5);
    HRichDrawHist::DrawH1(fHM->H1("fhTheta[400]"));
    c->cd(6);
    HRichDrawHist::DrawH1(fHM->H1("fhTheta[500]"));
    }

    {
    TCanvas* c = fHM->CreateCanvas("hrichdigipar_fhPhiXY_fhThetaXY", "hrichdigipar_fhPhiXY_fhThetaXY", 1500, 700);
    c->Divide(3,2);
    c->cd(1);
    HRichDrawHist::DrawH1(fHM->H1("fhPhiXY[183][100]"));
    c->cd(2);
    HRichDrawHist::DrawH1(fHM->H1("fhPhiXY[125][200]"));
    c->cd(3);
    HRichDrawHist::DrawH1(fHM->H1("fhPhiXY[50][50]"));

    c->cd(4);
    HRichDrawHist::DrawH1(fHM->H1("fhThetaXY[200][77]"));
    c->cd(5);
    HRichDrawHist::DrawH1(fHM->H1("fhThetaXY[156][99]"));
    c->cd(6);
    HRichDrawHist::DrawH1(fHM->H1("fhThetaXY[69][70]"));
    }

}

void HRich700DigiParCreator::drawFromFile( const string& fileName, const string& outputDir)
{
    HRichDrawHist::SetDefaultDrawStyle();
    gStyle->SetOptTitle(1);
    //if (fHM != NULL) delete fHM;

    fHM = new HRich700HistManager();
    TFile* file = new TFile(fileName.c_str());
    fHM->ReadFromFile(file);

    drawHist();

    fHM->SaveCanvasToImage(string(outputDir + "/"));
}
