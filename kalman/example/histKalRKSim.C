#include "hcategorymanager.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hkalhit.h"
#include "hkaltrack.h"
#include "hloop.h"
#include "hmdccal2parsim.h"
#include "hmdcsegsim.h"
#include "hmdctrackgdef.h"
#include "hphysicsconstants.h"
#include "hrktrackB.h"
#include "hruntimedb.h"
#include "htool.h"

#include "hades.h"
#include "createHades.C"

#include "TIterator.h"
#include "TSystem.h"
#include "TMath.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TDatabasePDG.h"
#include "TFrame.h"
#include "TFile.h"
#include "TFitResult.h"
#include "TGraph.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TH3F.h"
#include "THStack.h"
#include "TLegend.h"
#include "TLegendEntry.h"
#include "TMultiGraph.h"
#include "TNtupleD.h"
#include "TObjArray.h"
#include "TPaveStats.h"
#include "TPaveText.h"
#include "TProfile.h"
#include "TROOT.h"
#include "TStyle.h"

#include <iostream>
#include <map>
#include <limits>
using namespace std;

// ------------------------------------------------------------
// This macro creates histograms that compare the Kalman filter
// and Runge-Kutta.
// ------------------------------------------------------------

HKalTrack* findKalman(HMetaMatch2 const* const pMetaMatch, HCategory* catTrack) {

    if(!pMetaMatch) return NULL;

    Int_t best = pMetaMatch->getKalmanFilterInd();
    HKalTrack *bestTrack = NULL;
    if(best > 0) bestTrack = (HKalTrack*)catTrack->getObject(best);
    Float_t bestQual = numeric_limits<Float_t>::infinity();

    Int_t idx = -1;
    Float_t qual = -1.F;
    HKalTrack *track = NULL;

    for(UChar_t n = 0; n < 3; n++) {
        idx = pMetaMatch->getKalmanFilterIndShowerHit(n);
        if(idx >= 0) {
            track = (HKalTrack*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityShower();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getKalmanFilterIndTofClst(n);
        if(idx >= 0) {
            track = (HKalTrack*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getKalmanFilterIndTofHit1(n);
        if(idx >= 0) {
            track = (HKalTrack*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getKalmanFilterIndTofHit2(n);
        if(idx >= 0) {
            track = (HKalTrack*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getKalmanFilterIndRpcClst(n);
        if(idx >= 0) {
            track = (HKalTrack*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityRpc();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }
    }

    return bestTrack;
}

HRKTrackB* findRungeKutta(HMetaMatch2 const* const pMetaMatch,
			  HCategory* catTrack) {

    if(!pMetaMatch) return NULL;

    Int_t best = pMetaMatch->getRungeKuttaInd();
    HRKTrackB *bestTrack = NULL;
    if(best > 0) bestTrack = (HRKTrackB*)catTrack->getObject(best);
    Float_t bestQual = numeric_limits<Float_t>::infinity();

    Int_t idx = -1;
    Float_t qual = -1.F;
    HRKTrackB *track = NULL;

    for(UChar_t n = 0; n < 3; n++) {
        idx = pMetaMatch->getRungeKuttaIndShowerHit(n);
        if(idx >= 0) {
            track = (HRKTrackB*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityShower();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getRungeKuttaIndTofClst(n);
        if(idx >= 0) {
            track = (HRKTrackB*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getRungeKuttaIndTofHit1(n);
        if(idx >= 0) {
            track = (HRKTrackB*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getRungeKuttaIndTofHit2(n);
        if(idx >= 0) {
            track = (HRKTrackB*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityTof();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }

        idx = pMetaMatch->getRungeKuttaIndRpcClst(n);
        if(idx >= 0) {
            track = (HRKTrackB*)catTrack->getObject(idx);
            if(track) {
                qual = track->getQualityRpc();
                if(qual > 0.F && qual < bestQual) {
                    best = idx;
                    bestQual = qual;
                    bestTrack = track;
                }
            }
        }
    }

    return bestTrack;
}

void setBinLabelsGeant(TAxis *axis) {
    TDatabasePDG *pdg = TDatabasePDG::Instance();

    if(axis->GetXmin() > pdg->ConvertPdgToGeant3(pdg->GetParticle("e+")->PdgCode()) ||
       axis->GetXmax() < pdg->ConvertPdgToGeant3(pdg->GetParticle("neutron")->PdgCode())) {
	cerr<<"-E- CbmAnaEpemPairsTask::SetBinLabelsGeant() :: "
	    <<"Axis has wrong binning."<<endl;
	return;
    }

    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("e+")     ->PdgCode())),  "e^{+}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("e-")     ->PdgCode())),  "e^{-}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("mu+")    ->PdgCode())),  "#mu^{+}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("mu-")    ->PdgCode())),  "#mu^{-}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("pi+")    ->PdgCode())),  "#pi^{+}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("pi-")    ->PdgCode())),  "#pi^{-}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("K+")     ->PdgCode())),  "K^{+}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("K-")     ->PdgCode())),  "K^{-}");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("proton") ->PdgCode())),  "p");
    axis->SetBinLabel(axis->FindBin(pdg->ConvertPdgToGeant3(pdg->GetParticle("neutron")->PdgCode())),  "n");
}
void histKalRKSim() {

    TString dataDir  = "./sim/";

    TString dataFiles = dataDir;
    dataFiles = "./sim/Au_Au_1230MeV_1000evts_1_1_dst_apr12.root";
    //dataFiles = "/u/ekrebs/workspace/hades/dstsim/12001/p/p_1230MeV_10000evts_1_dst_apr12.root";

    //Bool_t bStoreHists = kFALSE;
    //TString histFile = "hist_daf_p.root";

    Int_t pid = -1;

    // To cut 1% of all tracks:
    // protons: KF: 200, kfwire: 300, daf single: 200, DAF: 8.
    // positrons: KF: 400, kfwire: 600, DAF single: 600 (1.5% cut), DAF: 600, pseudo-DAF: 6000 (2% cut), ideal+spline: 2800
    // electrons: KF: 800, kfwire: 800 (2% cut), DAF single: 800 (2% cut),  DAF: 40
    // To cut 2% of all tracks:
    // protons: KF: 100, kfwire: 150, daf single 140, DAF: 5
    // positrons: KF: 400, kfwire: 600, daf single: 600, DAF: 5, ideal+spline: 2800
    // electrons: KF: 800, kfwire: 800, DAF single: 800,  DAF: 40
    Float_t kfChi2Cut = -4.F;

    // To cut 1% of all tracks:
    // 100 for protons, ideal: 700.
    // 150 for positrons, ideal: infinity
    // 2100 for electrons
    // To cut 2% of all tracks:
    // 60 for protons, ideal: .
    // 150 for positrons, ideal: infinity
    // for electrons
    Float_t rkChi2Cut = -7.F;

    Int_t maxEntries = -1;

    gROOT->SetStyle("Plain");

    // n = 1 the name of histogram is printed
    // e = 1 the number of entries
    // m = 1 the mean value m = 2 the mean and mean error values
    // r = 1 the root mean square (RMS) r = 2 the RMS and RMS error
    // u = 1 the number of underflows
    // o = 1 the number of overflows
    // i = 1 the integral of bins s = 1 the skewness s = 2 the skewness and the skewness error
    // k = 1 the kurtosis k = 2 the kurtosis and the kurtosis error
    // 1 -> lower case letter, 2 -> upper case letter
    gStyle->SetOptStat("emr");


    // ---------------------------------------------------------------
    // Make histograms
    // ---------------------------------------------------------------


    // Momentum resolution and chi2
    // ----------------------------

    // Number of tracks over chi2.
    TH1F *hkfChi2     = new TH1F("KF Chi2", "#Shi^{2} Distribution",
				 100, 0., 7.);
    hkfChi2->GetXaxis()->SetTitle("#chi^{2}");
    hkfChi2->GetYaxis()->SetTitle("Number of Tracks");

    // Probability distribution
    TH1F *hkfprob          = new TH1F("hkfprob"         , "Probability Distribution", 20, 0., 1.);
    hkfprob->GetXaxis()->SetTitle("Probability");
    hkfprob->GetYaxis()->SetTitle("counts");
    hkfprob->SetLineColor(kRed);

    // Reconstructed momentum resolution
    TH1F *hkfdmomGeant = new TH1F("KF Momentum", "Momentum Resolution", 1000, -20., 20.);
    hkfdmomGeant->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hkfdmomGeant->GetYaxis()->SetTitle("counts");

    TH1F *hkfFiltdmomGeant = new TH1F("KF Momentum Filtered", "Momentum Resolution", 1000, -20., 20.);
    hkfFiltdmomGeant->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hkfFiltdmomGeant->GetYaxis()->SetTitle("counts");
    hkfFiltdmomGeant->SetLineColor(kBlue);

    // Resolution of input momentum for the Kalman filter
    TH1F *hkfdmomGeantIn = new TH1F("Input Momentum", "Momentum Resolution", 1000, -20., 20.);
    hkfdmomGeantIn->SetLineColor(kRed);
    hkfdmomGeantIn->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hkfdmomGeantIn->GetYaxis()->SetTitle("counts");

    // Momentum resolution for different polar angles
    Double_t minTheta = 15.;
    Double_t maxTheta = 90.;
    Int_t nThetaSteps = 4;
    TH2F* hkfdmomThetaResolution[4];
    for(Int_t k=0; k < nThetaSteps; k++){
        hkfdmomThetaResolution[k] = new TH2F(Form("kfdmomThetaResolution[%i]",k),
                                           "#Delta p : p",
                                           10,0.,2000.,100,-10.,10.);
        hkfdmomThetaResolution[k]->GetYaxis()->SetTitle("#Delta p [%]");
        hkfdmomThetaResolution[k]->GetXaxis()->SetTitle("p [MeV/c]");
    }

    // ----------------------------

    // Efficiency
    // ----------------------------

    Style_t markerStyleEff = kFullDotMedium;

    // Number of tracks over momentum.
    TH1F *hCandTracksMomReal = new TH1F("hCandTracksMomReal", "Momentum Distribution",
				      30, 0., 3000.);
    //marker and colour
    hCandTracksMomReal->GetXaxis()->SetTitle("p [MeV/c]");
    hCandTracksMomReal->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hkfTracksMomReal = new TH1F("hkfTracksMomReal", "Momentum Distribution",
				      30, 0., 3000.);
    //marker and colour
    hkfTracksMomReal->GetXaxis()->SetTitle("p [MeV/c]");
    hkfTracksMomReal->GetYaxis()->SetTitle("Number of Tracks");
    hkfTracksMomReal->SetLineColor(kBlue);

    TH1F *hkfTracksMomReco = new TH1F("hkfTracksMomReco", "Momentum Distribution Reco.",
				      30, 0., 3000.);
    hkfTracksMomReco->GetXaxis()->SetTitle("p [MeV/c]");
    hkfTracksMomReco->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hkfTracksMomIn = new TH1F("hkfTracksMomIn", "Momentum Distribution Input.",
				    30, 0., 3000.);
    hkfTracksMomIn->GetXaxis()->SetTitle("p [MeV/c]");
    hkfTracksMomIn->GetYaxis()->SetTitle("Number of Tracks");
    hkfTracksMomIn->SetLineColor(kRed);

    // Number of tracks over polar angle
    TH1F *hkfTracksTheta    = new TH1F("hkfTracksTheta", "#Theta Distribution", 20, 0., 90.);
    hkfTracksTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfTracksTheta->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hkfTracksThetaReco = new TH1F("hkfTracksThetaReco", "#Theta Distribution Reco.", 20, 0., 90.);
    hkfTracksThetaReco->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfTracksThetaReco->GetYaxis()->SetTitle("Number of Tracks");
    hkfTracksThetaReco->SetLineColor(kRed);

    // Reconstruction efficiency over momentum
    TH1F *hkfEffMom        = new TH1F("hkfEffMom",    "Efficiency : p",
				      30, 0., 3000.);
    hkfEffMom->GetXaxis()->SetTitle("p [MeV/c]");
    hkfEffMom->GetYaxis()->SetTitle("Efficiency");
    //hEffMom->GetYaxis()->SetRangeUser(0.8, 1.02);
    hkfEffMom->SetMarkerStyle(markerStyleEff);
    hkfEffMom->Sumw2();

    // Reconstruction efficiency over polar angle
    TH1F *hkfEffTheta       = new TH1F("hkfEffTheta",   "Efficiency : #Theta", 20, 0., 90.);
    hkfEffTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfEffTheta->GetYaxis()->SetTitle("Efficiency");
    hkfEffTheta->SetMarkerStyle(markerStyleEff);
    hkfEffTheta->Sumw2();
    // ----------------------------


    // Angle dependencies
    // ----------------------------

    TH2F *hkfPhiTheta  = new TH2F("hkfphitheta", "#Theta : #Phi", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hkfPhiTheta->GetXaxis()->SetTitle("#Phi [#circ ]");
    hkfPhiTheta->GetYaxis()->SetTitle("#Theta [#circ ]");

    TH2F *hkfPhiThetaFail  = new TH2F("hkfphiithetaphail", "#Theta : #Phi for phailed tracks", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hkfPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hkfPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");


    // Residuals and pulls
    // ----------------------------

    TH1F *hkfResX  = new TH1F("hkfResX",  "Residual X", 100, -1., 1.);
    TH1F *hkfResY  = new TH1F("hkfResY",  "Residual Y", 100, -1., 1.);
    TH1F *hkfResTx = new TH1F("hkfResTx", "Residual Tx", 100, -0.05, 0.05);
    TH1F *hkfResTy = new TH1F("hkfResTy", "Residual Ty", 100, -0.05, 0.05);
    TH1F *hkfResQp = new TH1F("hkfResQp", "Residual Qp", 100, -0.0005, 0.0005);

    TH1F *hkfPullX  = new TH1F("hkfPullX",  "Pull X",  100, -5., 5.);
    TH1F *hkfPullY  = new TH1F("hkfPullY",  "Pull Y",  100, -5., 5.);
    TH1F *hkfPullTx = new TH1F("hkfPullTx", "Pull Tx", 100, -5., 5.);
    TH1F *hkfPullTy = new TH1F("hkfPullTy", "Pull Ty", 100, -5., 5.);
    TH1F *hkfPullQp = new TH1F("hkfPullQp", "Pull Qp", 100, -5., 5.);

    TH1F *hkfErrInX  = new TH1F("hkfErrInX",  "Input Error X", 100, -1., 1.);
    TH1F *hkfErrInY  = new TH1F("hkfErrInY",  "Input Error Y", 100, -1., 1.);
    TH1F *hkfErrInTx = new TH1F("hkfErrInTx", "Input Error Tx", 100, -0.05, 0.05);
    TH1F *hkfErrInTy = new TH1F("hkfErrInTy", "Input Error Ty", 100, -0.05, 0.05);
    TH1F *hkfErrInQp = new TH1F("hkfErrInQp", "Input Error QP", 100, -5.e-4, 5.e-4);


    TH2F *hkfdMomInTheta = new TH2F("hkfdMomInTheta", "", 40, -30., 30.,
                                    40, 10., 90.);
    hkfdMomInTheta->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p_{In}} [%] #right)");
    hkfdMomInTheta->GetYaxis()->SetTitle("#Theta");

    TH2F *hkfdTxInTheta = new TH2F("hkfdTxInTheta", "", 40, -0.05, 0.05,
                                    40, 10., 90.);
    hkfdTxInTheta->GetXaxis()->SetTitle("tx_{Geant} - tx_{Input}");
    hkfdTxInTheta->GetYaxis()->SetTitle("#Theta");

    TH2F *hkfdTyInTheta = new TH2F("hkfdTyInTheta", "", 40, -0.05, 0.05,
                                    40, 10., 90.);
    hkfdTyInTheta->GetXaxis()->SetTitle("ty_{Geant} - ty_{Input}");
    hkfdTyInTheta->GetYaxis()->SetTitle("#Theta");

    TH2F *hkfdMomInMom = new TH2F("hkfdMomInMom", "", 40, -40., 40.,
				  40, 0., 3000.);
    hkfdMomInMom->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p_{In}} [%] #right)");
    hkfdMomInMom->GetYaxis()->SetTitle("p_{In} [MeV/c^{2}]");

    TH2F *hkfdTxInMom = new TH2F("hkfdTxInMom", "", 40, -0.05, 0.05,
				 40, 0., 3000.);
    hkfdTxInMom->GetXaxis()->SetTitle("tx_{Geant} - tx_{Input}");
    hkfdTxInMom->GetYaxis()->SetTitle("p_{In} [MeV/c^{2}]");

    TH2F *hkfdTyInMom = new TH2F("hkfdTyInMom", "", 40, -0.05, 0.05,
				 40, 0., 3000.);
    hkfdTyInMom->GetXaxis()->SetTitle("ty_{Geant} - ty_{Input}");
    hkfdTyInMom->GetYaxis()->SetTitle("p_{In} [MeV/c]");

    TH2F *hkfMomBetaIn = new TH2F("hkfMomBetaIn", "",
                                  50, -2000., 3000., 50, 0., 1.1);
    hkfMomBetaIn->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMomBetaIn->GetYaxis()->SetTitle("#beta");

    TH2F *hkfMomBetaInLep = new TH2F("hkfMomBetaInLep", "Real Leptons",
                                     50, -2000., 3000., 50, 0., 1.1);
    hkfMomBetaInLep->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMomBetaInLep->GetYaxis()->SetTitle("#beta");

    TH2F *hkfMomBetaInK = new TH2F("hkfMomBetaInK", "Real Kaons",
                                   50, -2000., 3000., 50, 0., 1.1);
    hkfMomBetaInK->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMomBetaInK->GetYaxis()->SetTitle("#beta");

    TH2F *hkfMomBetaInPie = new TH2F("hkfMomBetaInPie", "Real Pions",
                                     50, -2000., 3000., 50, 0., 1.1);
    hkfMomBetaInPie->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMomBetaInPie->GetYaxis()->SetTitle("#beta");

    TH2F *hkfMomBetaInP = new TH2F("hkfMomBetaInP", "Real Protons",
                                   50, -2000., 3000., 50, 0., 1.1);
    hkfMomBetaInP->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMomBetaInP->GetYaxis()->SetTitle("#beta");

    TH1F *hkfRichCandLep = new TH1F("hkfRichCandLep", "e^{+}/e^{-}",
                                    10, -0.5, 9.5);
    hkfRichCandLep->GetXaxis()->SetTitle("Number of RICH Candidates");
    hkfRichCandLep->GetYaxis()->SetTitle("counts");

    TH1F *hkfRichCandNoLep = new TH1F("hkfRichCandNoLep", "No e^{+}/e^{-}",
				      10, -0.5, 9.5);
    hkfRichCandNoLep->GetXaxis()->SetTitle("Number of RICH Candidates");
    hkfRichCandNoLep->GetYaxis()->SetTitle("counts");

    TH2F *hkfPid = new TH2F("hkfPid", "", 15, 1., 16., 15, 1., 16.);
    hkfPid->GetXaxis()->SetTitle("Assigned PID");
    hkfPid->GetYaxis()->SetTitle("Real PID");
    setBinLabelsGeant(hkfPid->GetXaxis());
    setBinLabelsGeant(hkfPid->GetYaxis());

    // ----------------------------
    // Runge-Kutta histograms
    // ----------------------------

    TH1F *hrkChi2     = new TH1F("RK Chi2", "#chi^{2} Distribution",
				 100, 0., 7.);
    hrkChi2->GetXaxis()->SetTitle("#chi^{2}");
    hrkChi2->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkprob          = new TH1F("hrkprob"         , "Probability Distribution", 20, 0., 1.);
    hrkprob->GetXaxis()->SetTitle("Probability");
    hrkprob->GetYaxis()->SetTitle("counts");
    hrkprob->SetLineColor(kRed);

    TH1F *hrkdmomGeant = new TH1F("RK Momentum", "Momentum Resolution", 1000, -20., 20.);
    hrkdmomGeant->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hrkdmomGeant->GetYaxis()->SetTitle("counts");

    TH1F *hrkdmomGeantIn = new TH1F("RK Input Momentum", "Momentum Resolution", 1000, -20., 20.);
    hrkdmomGeantIn->SetLineColor(kRed);
    hrkdmomGeantIn->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hrkdmomGeantIn->GetYaxis()->SetTitle("counts");

    TH2F* hrkdmomThetaResolution[4];
    for(Int_t k=0; k < nThetaSteps; k++){
        hrkdmomThetaResolution[k] = new TH2F(Form("rkdmomThetaResolution[%i]",k),
                                           "#Delta p : p",
                                           10,0.,2000.,100,-10.,10.);
        hrkdmomThetaResolution[k]->GetYaxis()->SetTitle("#Delta p [%]");
        hrkdmomThetaResolution[k]->GetXaxis()->SetTitle("p [MeV/c]");
    }

    // ----------------------------

    // Efficiency
    // ----------------------------

    TH1F *hrkTracksMom     = new TH1F("hrkTracksMom", "Momentum Distribution",
				      30, 0., 3000.);
    //marker and colour
    hrkTracksMom->GetXaxis()->SetTitle("p [MeV/c]");
    hrkTracksMom->GetYaxis()->SetTitle("Number of Tracks");
    hrkTracksMom->SetLineColor(kRed);

    TH1F *hrkTracksMomReco = new TH1F("hrkTracksMomReco", "Momentum Distribution Reco.",
				      30, 0., 3000.);
    hrkTracksMomReco->GetXaxis()->SetTitle("p [MeV/c]");
    hrkTracksMomReco->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkTracksTheta    = new TH1F("hrkTracksTheta", "#Theta Distribution", 20, 0., 90.);
    hrkTracksTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkTracksTheta->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkTracksThetaReco = new TH1F("hrkTracksThetaReco", "#Theta Distribution Reco.", 20, 0., 90.);
    hrkTracksThetaReco->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkTracksThetaReco->GetYaxis()->SetTitle("Number of Tracks");
    hrkTracksThetaReco->SetLineColor(kRed);

    TH1F *hrkEffMom        = new TH1F("hrkEffMom",    "Efficiency : p",
				      30, 0., 3000.);
    hrkEffMom->GetXaxis()->SetTitle("p [MeV/c]");
    hrkEffMom->GetYaxis()->SetTitle("Efficiency");
    hrkEffMom->SetMarkerStyle(markerStyleEff);
    hrkEffMom->Sumw2();

    TH1F *hrkEffTheta       = new TH1F("hrkEffTheta",   "Efficiency : #Theta", 20, 0., 90.);
    hrkEffTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkEffTheta->GetYaxis()->SetTitle("Efficiency");
    hrkEffTheta->SetMarkerStyle(markerStyleEff);
    hrkEffTheta->Sumw2();
    // ----------------------------


    // Angle dependencies
    // ----------------------------

    TH2F *hrkPhiTheta  = new TH2F("hrkphitheta", "#Theta : #Phi", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hrkPhiTheta->GetXaxis()->SetTitle("#Phi [#circ ]");
    hrkPhiTheta->GetYaxis()->SetTitle("#Theta [#circ ]");

    TH2F *hrkPhiThetaFail  = new TH2F("hrkphithetafail", "#Theta : #Phi for failed tracks", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hrkPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hrkPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");

    // ----------------------------
    // End make histograms
    // ----------------------------



    HLoop* loop = new HLoop(kTRUE);  // kTRUE : create Hades  (needed to work with standard eventstructure)
    loop->addFiles(dataFiles);
    if(!loop->setInput("-*,+HKalTrack,+HKalSite,+HKalHit2d,+HMdcSegSim,+HMdcCal1Sim,+HParticleCandSim,+HMetaMatch2,+HRKTrackB,+HGeantKine,+HGeantMdc")) {
        exit(1);
    }

    TIterator* iterCand = 0;
    if (loop->getCategory("HParticleCandSim")) {
        iterCand = loop->getCategory("HParticleCandSim")->MakeIterator();
    }

    HCategory* fCatCand = HCategoryManager::getCategory(catParticleCand);
    if(!fCatCand) {
        cout<<"No catCand "<<endl;
    }

    TIterator* iterCandKine = 0;
    if (loop->getCategory("HGeantKine")) iterCandKine = loop->getCategory("HGeantKine")->MakeIterator();
    if(!iterCandKine) {
        cout<<"Could not make iterator for HGeantKine."<<endl;
        exit(1);
    }

    HCategory* fCatKine = HCategoryManager::getCategory(catGeantKine);
    if(!fCatKine) {
        cout<<"No catKine."<<endl;
        exit(1);
    }

    HCategory* fCatMdcSeg = HCategoryManager::getCategory(catMdcSeg);
    if(!fCatMdcSeg) {
        cout<<"No catMdcSeg."<<endl;
        exit(1);
    }

    HCategory* fCatMeta     = HCategoryManager::getCategory(catMetaMatch);

    HCategory* fCatKalTrack = HCategoryManager::getCategory(catKalTrack);

    HCategory* fCatKalSite  = HCategoryManager::getCategory(catKalSite);

    HCategory* fCatKalHit2d = HCategoryManager::getCategory(catKalHit2d);

    HCategory* fCatRKTrack  = HCategoryManager::getCategory(catRKTrackB);

    Int_t entries = loop->getEntries();
    if(maxEntries > 0 && entries > maxEntries) entries = maxEntries;

    Int_t nbytes  = 0;

    Int_t nTracksCand    = 0;
    Int_t nTracksMeta    = 0;
    Int_t nTracksKal     = 0;
    Int_t nTracksKalCut  = 0;
    Int_t nTracksRK      = 0;
    Int_t nTracksRKCut   = 0;

    // ---------------------------------------------------------------
    // Fill histograms.
    // ---------------------------------------------------------------

    for (Int_t i = 0; i < entries; i++) {
        nbytes = loop->nextEvent(i);                  // get next event. categories will be cleared before

        if(nbytes <= 0) {
            cout<<"Error reading next event. nbytes = "<<nbytes<<endl;
            break;
        } // last event reached

        if((i % 100) == 0) cout<<"Event number "<<i<<endl;

        iterCand->Reset();
        HParticleCandSim *cand = NULL;

        while((cand = (HParticleCandSim*)iterCand->Next()) != 0) {

	    if(cand->getGeantPID() != pid && pid >= 0) continue;

	    //if(cand->getPID() < 0) continue;
            if(!cand->isFlagBit(kIsUsed) || !cand->getBeta() > 0.) continue;

            nTracksCand ++;

            Bool_t bIsGhost = cand->isGhostTrack();
            if(bIsGhost) continue;

            HMetaMatch2 *pMetaMatch = (HMetaMatch2*)fCatMeta->getObject(cand->getMetaInd());
            if(!pMetaMatch) {
                cout<<"No meta match found."<<endl;
                continue;
            }
            nTracksMeta++;

            HKalTrack *track = findKalman(pMetaMatch, fCatKalTrack);
            if(track) {
		Int_t idxFirst = track->getIdxFirst();
		if(idxFirst < 0) {
		    cout<<"First site not found"<<endl;
		}
		HKalSite *firstSite = (HKalSite*)fCatKalSite->getObject(idxFirst);
		if(!firstSite) {
		    cout<<"Could not find site."<<endl;
		    continue;
		}

		Int_t idxLast = track->getIdxLast();
		HKalSite *lastSite = (HKalSite*)fCatKalSite->getObject(idxLast);
		if(!lastSite) {
		    cout<<"Could not find last site."<<endl;
		    continue;
		}

		Int_t idxHit = firstSite->getIdxFirst();
		HKalHit2d *hit = NULL;
		if(idxHit >= 0) {
		    hit = dynamic_cast<HKalHit2d*>(fCatKalHit2d->getObject(idxHit));
		}

		Float_t momGeantFirst = firstSite->getMomReal();
		Bool_t bHasGeant = kTRUE;
		if(hit) {
		    if(hit->getXreal() <= -10000.F ||
		       hit->getYreal() <= -10000.F) {
			bHasGeant = kFALSE;
		    }
		}
		if(momGeantFirst < 0.F ||
		   firstSite->getTxReal() <= -10.F ||
		   firstSite->getTyReal() <= -10.F) {
                    bHasGeant = kFALSE;
		}

		if(idxFirst >= 0 && bHasGeant) {

		    Float_t Theta = track->getTheta();
		    Float_t Phi   = track->getPhi();
		    Float_t chi2 = track->getChi2();
		    Float_t ndf  = track->getNdf();

		    hkfTracksMomReal->Fill(momGeantFirst);
		    hkfTracksTheta->Fill(Theta);
                    hkfChi2->Fill(chi2/ndf);

		    if(chi2 > 0.F && (kfChi2Cut < 0 || chi2 / ndf < kfChi2Cut)) {
			Float_t momSmoo       = firstSite->getMomSmoo();
			Float_t momInput      = track    ->getMomInput();

			Float_t dMomGeantSmoo = (momGeantFirst - momSmoo) / momGeantFirst * 100.F;

			hkfdmomGeant->Fill(dMomGeantSmoo);

                        Float_t momGeantLast  = lastSite->getMomReal();
                        Float_t momFilt       = lastSite->getMomFilt();
                        Float_t dMomGeantFilt = (momGeantLast - momFilt) / momGeantLast * 100.F;
                        hkfFiltdmomGeant->Fill(dMomGeantFilt);

                        Float_t dMomGeantIn   = (momGeantFirst - momInput) / momGeantFirst * 100.F;
			hkfdmomGeantIn  ->Fill(dMomGeantIn);
			hkfdMomInTheta->Fill(dMomGeantIn, Theta);
			hkfdMomInMom->Fill(dMomGeantIn, momInput);

			hkfTracksMomReco->Fill(momSmoo);
			hkfTracksThetaReco->Fill(Theta);
                        hkfTracksMomIn->Fill(momInput);

			Int_t thetabin = (Int_t) (Theta / ((maxTheta - minTheta)/(nThetaSteps - 1)));
			if(thetabin > nThetaSteps - 1) {
			    thetabin = nThetaSteps - 1;
			}
			hkfdmomThetaResolution[thetabin] ->Fill(momGeantFirst, dMomGeantSmoo);

			hkfprob->Fill(TMath::Prob(chi2, ndf));
			hkfPhiTheta->Fill(Phi, Theta);

			if(hit) {
			    Float_t dx = hit->getXreco() - hit->getXreal();
			    Float_t dy = hit->getYreco() - hit->getYreal();
			    hkfResX->Fill(dx);
			    hkfResY->Fill(dy);
			    hkfPullX->Fill(dx / sqrt(firstSite->getCxxReco()));
			    hkfPullY->Fill(dy / sqrt(firstSite->getCyyReco()));

			    Float_t dxRecoIn  = hit->getXreal() - track->getXinput();
			    Float_t dyRecoIn  = hit->getYreal() - track->getYinput();
			    hkfErrInX->Fill(dxRecoIn);
			    hkfErrInY->Fill(dyRecoIn);
			}
			Float_t dtx = firstSite->getTxReco() - firstSite->getTxReal();
			Float_t dty = firstSite->getTyReco() - firstSite->getTyReal();
			hkfResTx->Fill(dtx);
			hkfResTy->Fill(dty);
			hkfPullTx->Fill(dtx / sqrt(firstSite->getCtxReco()));
			hkfPullTy->Fill(dty / sqrt(firstSite->getCtyReco()));

			Float_t dtxRealIn = firstSite->getTxReal() - track->getTxInput();
			Float_t dtyRealIn = firstSite->getTyReal() - track->getTyInput();
			hkfErrInTx->Fill(dtxRealIn);
			hkfErrInTy->Fill(dtyRealIn);
			hkfdTxInTheta->Fill(dtxRealIn, Theta);
			hkfdTyInTheta->Fill(dtyRealIn, Theta);
			hkfdTxInMom->Fill(dtxRealIn, momInput);
			hkfdTyInMom->Fill(dtyRealIn, momInput);

			Float_t dqpRealIn = 1./momGeantFirst - 1./momInput;
                        hkfErrInQp->Fill(dqpRealIn);

			Int_t charge = HPhysicsConstants::charge(cand->getPID());
			Float_t dqp = charge/momSmoo - charge/momGeantFirst;
			hkfResQp->Fill(dqp);
			hkfPullQp->Fill(dqp / sqrt(firstSite->getCqpReco()));
		    } else {
			hkfPhiThetaFail->Fill(Phi, Theta);
			nTracksKalCut++;
                    }

                    Float_t betaInput = track->getBetaInput();
                    Float_t momInput  = track->getMomInput();
                    Int_t   charge    = (Int_t)track->getPolarity();
                    Int_t   pidReal   = cand->getGeantPID();
                    Int_t   pidAss    = track->getPid();
                    Int_t   nRichCand = pMetaMatch->getNCandForRich();

                    hkfMomBetaIn->Fill(momInput*charge, betaInput);
                    if(pidReal == HPhysicsConstants::pid("e+") ||
                       pidReal == HPhysicsConstants::pid("e-")) {
                        hkfMomBetaInLep->Fill(momInput*charge, betaInput);
                        hkfRichCandLep->Fill(nRichCand);
                    } else {
                        hkfRichCandNoLep->Fill(nRichCand);
                    }
                    if(pidReal == HPhysicsConstants::pid("K+") ||
                       pidReal == HPhysicsConstants::pid("K-")) {
                        hkfMomBetaInK->Fill(momInput*charge, betaInput);
                    }
                    if(pidReal == HPhysicsConstants::pid("pi+") ||
                       pidReal == HPhysicsConstants::pid("pi-")) {
                        hkfMomBetaInPie->Fill(momInput*charge, betaInput);
                    }
                    if(pidReal == HPhysicsConstants::pid("p")) {
                        hkfMomBetaInP->Fill(momInput*charge, betaInput);
		    }

                    hkfPid->Fill(pidAss, pidReal);

		    nTracksKal++;
		}
	    } // filled Kalman histograms






            HMdcSegSim *segInner = HCategoryManager::getObject(segInner, fCatMdcSeg, cand->getInnerSegInd());
            Int_t trackNum = segInner->getTrack(0);

            iterCandKine->Reset();
            HGeantKine *kine = NULL;
            kine = HCategoryManager::getObject(kine, fCatKine, trackNum-1);
            if(!kine) {
                cout<<"No kine object found."<<endl;
                continue;
            }

            Float_t momGeant1 = kine->getTotalMomentum();
            Float_t momGeant2 = kine->getTotalMomentum();

            HGeantMdc* geantMdc;
            kine->resetMdcIter();

            Float_t ax, ay, atof;
            Float_t sector = -1.F;
            while( (geantMdc = (HGeantMdc*)kine->nextMdcHit()) != 0) {
                if(geantMdc->getModule() == 1 && geantMdc->getLayer() == 5) {
                    geantMdc->getHit(ax, ay, atof, momGeant1);
                    sector = (Float_t)geantMdc->getSector();
                }
                if(geantMdc->getModule() == 2 && geantMdc->getLayer() == 0) {
                    geantMdc->getHit(ax, ay, atof, momGeant2);
                    sector = (Float_t)geantMdc->getSector();
                }
            }

            Float_t rkmomGeant = (momGeant2 + momGeant1) / 2.F;

	    HRKTrackB *trackRK = findRungeKutta(pMetaMatch, fCatRKTrack);
	    if(trackRK) {
               /*
		Float_t rkchi2     = cand->getChi2();
		Float_t rkndf      = 4.F * 2.F - 5.F;
		Float_t rkmom      = cand->getMomentum();
		Float_t rkTheta    = cand->getTheta();
		Float_t rkPhi      = cand->getPhi();
                */
		Float_t rkchi2     = trackRK->getChiq();
		Float_t rkndf      = 4.F * 2.F - 5.F;
		Float_t rkmom      = trackRK->getP();
		Float_t rkTheta    = trackRK->getTheta() * TMath::RadToDeg();
		Float_t rkPhi      = trackRK->getPhi() * TMath::RadToDeg();

		if(sector >= 0.F && sector <= 5.F) {
		    //rkPhi -= (60.F * sector);
		} else {
		    cout<<"No GeantMdc found in event "<<i<<endl;
		}

		Float_t rkmomRes = (rkmomGeant - rkmom) / rkmomGeant * 100.F;

		hrkTracksMom->Fill(rkmomGeant);
		hrkTracksTheta->Fill(rkTheta);
                hrkChi2->Fill(rkchi2/rkndf);

		if(rkchi2 >= 0.F && (rkChi2Cut < 0 || rkchi2 / rkndf < rkChi2Cut)) {
		    hrkTracksMomReco->Fill(rkmom);
		    hrkTracksThetaReco->Fill(rkTheta);
		    hrkprob->Fill(TMath::Prob(rkchi2, rkndf));
		    hrkdmomGeant->Fill(rkmomRes);

		    Int_t thetabin = (Int_t) (rkTheta / ((maxTheta - minTheta)/(nThetaSteps - 1)));
		    if(thetabin > nThetaSteps - 1) {
			thetabin = nThetaSteps - 1;
		    }

		    hrkdmomThetaResolution[thetabin] ->Fill(rkmomGeant, rkmomRes);
		    hrkPhiTheta->Fill(rkPhi, rkTheta);
		} else {
		    hrkPhiThetaFail->Fill(rkPhi, rkTheta);
		    nTracksRKCut++;
		}
		nTracksRK++;
	    }

        } // while cand
    } // loop over entries


    // End fill histograms.
    // ---------------------------------------------------------------

    // ---------------------------------------------------------------
    // Display histograms.
    // ---------------------------------------------------------------

    TCanvas *kfresult = new TCanvas("kfresult", "Kalman", 1000, 1000);
    kfresult->Divide(3,3);

    kfresult->cd(1);
    hkfdmomGeant->Draw();
    hkfdmomGeantIn->Draw("sames");
    hkfFiltdmomGeant->Draw("sames");

    gPad->Update();
    TPaveStats *stGeantSmoo = (TPaveStats*)hkfdmomGeant->FindObject("stats");
    stGeantSmoo->SetOptStat(1111);
    stGeantSmoo->SetY1NDC(stGeantSmoo->GetY1NDC() - .05);
    stGeantSmoo->SetLineColor(hkfdmomGeant->GetLineColor());
    stGeantSmoo->Draw("same");

    TPaveStats *stGeantFilt = (TPaveStats*)hkfFiltdmomGeant->FindObject("stats");
    stGeantFilt->SetOptStat(1111);
    stGeantFilt->SetY1NDC(stGeantFilt->GetY1NDC() - .25);
    stGeantFilt->SetY2NDC(stGeantFilt->GetY2NDC() - .2);
    stGeantFilt->SetLineColor(hkfFiltdmomGeant->GetLineColor());
    stGeantFilt->Draw("sames");

    TPaveStats *stGeantIn = (TPaveStats*)hkfdmomGeantIn->FindObject("stats");
    stGeantIn->SetOptStat(1111);
    stGeantIn->SetY1NDC(stGeantIn->GetY1NDC() - .45);
    stGeantIn->SetY2NDC(stGeantIn->GetY2NDC() - .4);
    stGeantIn->SetLineColor(hkfdmomGeantIn->GetLineColor());
    stGeantIn->Draw("sames");


    kfresult->cd(2);
    hkfTracksMomReal->Draw();
    hkfTracksMomReco->Draw("same");
    hkfTracksMomIn->Draw("same");

    TLegend *legendaryMoms = new TLegend(0.61,0.84,0.99,0.99, "","brNDC");
    legendaryMoms->AddEntry(hkfTracksMomReal, "Jeant Momentum");
    legendaryMoms->AddEntry(hkfTracksMomIn,   "Input Momentum");
    legendaryMoms->AddEntry(hkfTracksMomReco, "KF Momentum");
    legendaryMoms->Draw("same");

    kfresult->cd(3);
    hkfChi2->Draw();

    kfresult->cd(4);
    hkfprob->Draw();


    kfresult->cd(5);

    TObjArray* arrp1 = HTool::projections(hkfdmomThetaResolution[0],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* arrp2 = HTool::projections(hkfdmomThetaResolution[1],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* arrp3 = HTool::projections(hkfdmomThetaResolution[2],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* arrp4 = HTool::projections(hkfdmomThetaResolution[3],"y",0,-1,-99,"e","", 8,2,  0.7);

    TH1D* hkfrms[4];
    Int_t cols[]={ 1, 2, 4, 8};
    for(Int_t i = 0; i < nThetaSteps; i++){
        hkfrms[i] = new TH1D(Form("hkfrms[%i]",i),"",
                           hkfdmomThetaResolution[0]->GetNbinsX(),
                           hkfdmomThetaResolution[0]->GetXaxis()->GetXmin(),
                           hkfdmomThetaResolution[0]->GetXaxis()->GetXmax());
        hkfrms[i]->SetLineColor(cols[i]);
        hkfrms[i]->SetMarkerStyle(kFullSquare);
        hkfrms[i]->SetMarkerColor(cols[i]);
        hkfrms[i]->SetMarkerSize(0.65);
        hkfrms[i]->SetStats(kFALSE);
    }

    for(Int_t i=0; i < arrp1->GetEntries(); i++){
        hkfrms[0]->SetBinContent(i+1, ((TH1D*)(arrp1->At(i)))->GetRMS());
        hkfrms[1]->SetBinContent(i+1, ((TH1D*)(arrp2->At(i)))->GetRMS());
        hkfrms[2]->SetBinContent(i+1, ((TH1D*)(arrp3->At(i)))->GetRMS());
        hkfrms[3]->SetBinContent(i+1, ((TH1D*)(arrp4->At(i)))->GetRMS());

        hkfrms[0]->SetBinError(i+1, ((TH1D*)(arrp1->At(i)))->GetRMSError());
        hkfrms[1]->SetBinError(i+1, ((TH1D*)(arrp2->At(i)))->GetRMSError());
        hkfrms[2]->SetBinError(i+1, ((TH1D*)(arrp3->At(i)))->GetRMSError());
        hkfrms[3]->SetBinError(i+1, ((TH1D*)(arrp4->At(i)))->GetRMSError());
    }

    hkfrms[0]->Draw();
    hkfrms[0]->GetXaxis()->SetTitle("p [MeV/c]");
    hkfrms[0]->GetYaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hkfrms[0]->GetYaxis()->SetRangeUser(0., 10.);

    for(Int_t i=1; i < nThetaSteps; i++ ){
        hkfrms[i]->Draw("same");
    }

    gPad->SetGridx();
    gPad->SetGridy();
    gPad->Update();

    TLegend* legendMomTheta = new TLegend(0.55,0.51,0.85,0.85, "#Theta","brNDC");
    legendMomTheta->AddEntry(hkfrms[0],"15-33 [#circ]","lpf");
    legendMomTheta->AddEntry(hkfrms[1],"33-51 [#circ]","lpf");
    legendMomTheta->AddEntry(hkfrms[2],"51-69 [#circ]","lpf");
    legendMomTheta->AddEntry(hkfrms[3],"69-90 [#circ]","lpf");

    legendMomTheta->Draw("same");


    kfresult->cd(6);
    hkfPhiTheta->Draw("colz");

    kfresult->cd(7);
    hkfPhiThetaFail->Draw("colz");

    kfresult->cd(8);
    hkfEffMom->Divide(hkfTracksMomReco, hkfTracksMomReal);
    hkfEffMom->Draw("EP");

    kfresult->cd(9);
    hkfEffTheta->Divide(hkfTracksThetaReco, hkfTracksTheta);
    hkfEffTheta->Draw("EP");



    TCanvas *rkresult = new TCanvas("rkresult", "Runge-Kutta", 1000, 1000);
    rkresult->Divide(3,3);

    rkresult->cd(1);
    hrkdmomGeant->Draw();

    rkresult->cd(2);
    hrkTracksMom->Draw();
    hrkTracksMomReco->Draw("same");

    rkresult->cd(3);
    hrkChi2->Draw();

    rkresult->cd(4);
    hrkprob->Draw();

    rkresult->cd(5);

    TObjArray* rkarrp1 = HTool::projections(hrkdmomThetaResolution[0],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* rkarrp2 = HTool::projections(hrkdmomThetaResolution[1],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* rkarrp3 = HTool::projections(hrkdmomThetaResolution[2],"y",0,-1,-99,"e","", 8,2,  0.7);
    TObjArray* rkarrp4 = HTool::projections(hrkdmomThetaResolution[3],"y",0,-1,-99,"e","", 8,2,  0.7);

    TH1D* hrkrms[4];
    for(Int_t i = 0; i < nThetaSteps; i++){
        hrkrms[i] = new TH1D(Form("hrkrms[%i]",i),"",
                           hrkdmomThetaResolution[0]->GetNbinsX(),
                           hrkdmomThetaResolution[0]->GetXaxis()->GetXmin(),
                           hrkdmomThetaResolution[0]->GetXaxis()->GetXmax());
        hrkrms[i]->SetLineColor(cols[i]);
        hrkrms[i]->SetMarkerStyle(kFullSquare);
        hrkrms[i]->SetMarkerColor(cols[i]);
        hrkrms[i]->SetMarkerSize(0.65);
        hrkrms[i]->SetStats(kFALSE);
    }

    for(Int_t i=0; i < rkarrp1->GetEntries(); i++){
        hrkrms[0]->SetBinContent(i+1, ((TH1D*)(rkarrp1->At(i)))->GetRMS());
        hrkrms[1]->SetBinContent(i+1, ((TH1D*)(rkarrp2->At(i)))->GetRMS());
        hrkrms[2]->SetBinContent(i+1, ((TH1D*)(rkarrp3->At(i)))->GetRMS());
        hrkrms[3]->SetBinContent(i+1, ((TH1D*)(rkarrp4->At(i)))->GetRMS());

        hrkrms[0]->SetBinError(i+1, ((TH1D*)(rkarrp1->At(i)))->GetRMSError());
        hrkrms[1]->SetBinError(i+1, ((TH1D*)(rkarrp2->At(i)))->GetRMSError());
        hrkrms[2]->SetBinError(i+1, ((TH1D*)(rkarrp3->At(i)))->GetRMSError());
        hrkrms[3]->SetBinError(i+1, ((TH1D*)(rkarrp4->At(i)))->GetRMSError());
    }

    hrkrms[0]->Draw();
    hrkrms[0]->GetXaxis()->SetTitle("p [MeV/c]");
    hrkrms[0]->GetYaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hrkrms[0]->GetYaxis()->SetRangeUser(0., 10.);

    for(Int_t i=1; i < nThetaSteps; i++ ){
        hrkrms[i]->Draw("same");
    }

    gPad->SetGridx();
    gPad->SetGridy();
    gPad->Update();

    legendMomTheta->Draw("same");

    rkresult->cd(6);
    hrkPhiTheta->Draw("colz");

    rkresult->cd(7);
    hrkPhiThetaFail->Draw("colz");

    rkresult->cd(8);
    hrkEffMom->Divide(hrkTracksMomReco, hrkTracksMom);
    hrkEffMom->Draw("EP");

    rkresult->cd(9);
    hrkEffTheta->Divide(hrkTracksThetaReco, hrkTracksTheta);
    hrkEffTheta->Draw("EP");


    TCanvas *cSpline = new TCanvas("cSpline", "cSpline", 1000, 1000);
    cSpline->Divide(3,3);

    cSpline->cd(1);
    hkfdMomInTheta->Draw("colz");

    cSpline->cd(2);
    hkfdMomInMom->Draw("colz");

    cSpline->cd(3);
    const Float_t thetaStart = 20.F;
    const Float_t thetaEnd   = 80.F;
    const Float_t fStepSize   = 5.F;//2.*(hkfdMomInTheta->GetYaxis()->GetXmax() - hkfdMomInTheta->GetYaxis()->GetXmin()) / hkfdMomInTheta->GetYaxis()->GetNbins();
    Int_t nSteps = (thetaEnd - thetaStart) / fStepSize;

    TH1F *hRmsdMomThetaSlices = new TH1F("hRmsdMomThetaSlices", "hRmsdMomThetaSlices",
                                         nSteps, thetaStart, thetaEnd);

    for(Int_t iStep = 0; iStep < nSteps; iStep++) {
        Float_t theta1 = thetaStart + fStepSize * iStep;
        Float_t theta2 = thetaStart + fStepSize * (iStep+1);
        Int_t bin1 = hkfdMomInTheta->GetYaxis()->FindBin(theta1);
	Int_t bin2 = hkfdMomInTheta->GetYaxis()->FindBin(theta2);
	TH1D *slice = hkfdMomInTheta->ProjectionX(Form("dmom_theta_px_%i_%i", bin1, bin2), bin1, bin2);
        //slice->Fit("gaus", "R", "", -10., 10.)->Get()
        hRmsdMomThetaSlices->SetBinContent(iStep+1, slice->GetRMS());
        hRmsdMomThetaSlices->SetBinError(iStep+1, slice->GetRMSError());
    }
    hRmsdMomThetaSlices->Draw();
    cout<<"Fit Mom"<<endl;
    hRmsdMomThetaSlices->Fit("pol2");

    //TProfile *profdMomInTheta = hkfdMomInTheta->ProfileY("_pfy", 1, -1, "");
    //profdMomInTheta->Draw();

    cSpline->cd(4);
    hkfdTxInTheta->Draw("colz");

    cSpline->cd(5);
    hkfdTxInMom->Draw("colz");

    cSpline->cd(6);
    const Float_t txStart = hkfdTxInTheta->GetYaxis()->GetXmin();
    const Float_t txEnd   = hkfdTxInTheta->GetYaxis()->GetXmax();
    Int_t nBins = hkfdTxInTheta->GetYaxis()->GetNbins();
    Int_t stepSize = 4;
    nSteps = TMath::Ceil((Float_t)nBins / (Float_t)stepSize);

    TH1F *hRmsdTxThetaSlices = new TH1F("hRmsdTxThetaSlices", "hRmsdTxThetaSlices",
                                         nSteps, txStart, txEnd);

    for(Int_t iStep = 0; iStep < nSteps; iStep++) {
        Int_t binStart = iStep * stepSize;
	Int_t binEnd   = (iStep+1) * stepSize;
	TH1D *slice = hkfdTxInTheta->ProjectionX(Form("dtx_theta_px_%i_%i",
						      binStart, binEnd),
						 binStart, binEnd);
        //slice->Fit("gaus", "R", "", -10., 10.)->Get()
        hRmsdTxThetaSlices->SetBinContent(iStep+1, slice->GetRMS());
        hRmsdTxThetaSlices->SetBinError(iStep+1, slice->GetRMSError());
    }
    hRmsdTxThetaSlices->Draw();
    cout<<"Fit Tx"<<endl;
    hRmsdTxThetaSlices->Fit("pol2");


    cSpline->cd(7);
    hkfdTyInTheta->Draw("colz");

    cSpline->cd(8);
    hkfdTyInMom->Draw("colz");

    cSpline->cd(9);
    const Float_t tyStart = hkfdTyInTheta->GetYaxis()->GetXmin();
    const Float_t tyEnd   = hkfdTyInTheta->GetYaxis()->GetXmax();
    nBins = hkfdTyInTheta->GetYaxis()->GetNbins();
    stepSize = 4;
    nSteps = TMath::Ceil((Float_t)nBins / (Float_t)stepSize);

    TH1F *hRmsdTyThetaSlices = new TH1F("hRmsdTyThetaSlices", "hRmsdTyThetaSlices",
                                         nSteps, tyStart, tyEnd);

    for(Int_t iStep = 0; iStep < nSteps; iStep++) {
        Int_t binStart = iStep * stepSize;
	Int_t binEnd   = (iStep+1) * stepSize;
	TH1D *slice = hkfdTyInTheta->ProjectionX(Form("dtx_theta_px_%i_%i",
						      binStart, binEnd),
						 binStart, binEnd);
        //slice->Fit("gaus", "R", "", -10., 10.)->Get()
        hRmsdTyThetaSlices->SetBinContent(iStep+1, slice->GetRMS());
        hRmsdTyThetaSlices->SetBinError(iStep+1, slice->GetRMSError());
    }
    hRmsdTyThetaSlices->Draw();
    cout<<"Fit Ty"<<endl;
    hRmsdTyThetaSlices->Fit("pol2");

    TCanvas *cRes = new TCanvas("cRes", "cRes", 1200, 800);
    cRes->Divide(3,2);
    cRes->cd(1);
    hkfResX->Draw();
    cRes->cd(2);
    hkfResY->Draw();
    cRes->cd(3);
    hkfResTx->Draw();
    cRes->cd(4);
    hkfResTy->Draw();
    cRes->cd(5);
    hkfResQp->Draw();

    TCanvas *cPull = new TCanvas("cPull", "cPull", 1200, 800);
    cPull->Divide(3,2);
    cPull->cd(1);
    hkfPullX->Draw();
    cPull->cd(2);
    hkfPullY->Draw();
    cPull->cd(3);
    hkfPullTx->Draw();
    cPull->cd(4);
    hkfPullTy->Draw();
    cPull->cd(5);
    hkfPullQp->Draw();

    TCanvas *cErrIn = new TCanvas("cErrIn", "cErrIn", 1200, 800);
    cErrIn->Divide(3,2);
    cErrIn->cd(1);
    hkfErrInX->Draw();
    cErrIn->cd(2);
    hkfErrInY->Draw();
    cErrIn->cd(3);
    hkfErrInTx->Draw();
    cErrIn->cd(4);
    hkfErrInTy->Draw();
    cErrIn->cd(5);
    hkfErrInQp->Draw();

    TCanvas *cPid = new TCanvas("cPid", "cPid", 1000, 1000);
    cPid->Divide(3,3);
    cPid->cd(1);
    hkfMomBetaIn->Draw("colz");
    cPid->cd(2);
    hkfMomBetaInLep->Draw("colz");
    cPid->cd(3);
    hkfMomBetaInK->Draw("colz");
    cPid->cd(4);
    hkfMomBetaInPie->Draw("colz");
    cPid->cd(5);
    hkfMomBetaInP->Draw("colz");
    cPid->cd(7);
    hkfRichCandLep->Draw();
    cPid->cd(8);
    hkfRichCandNoLep->Draw();
    cPid->cd(9);
    hkfPid->Draw("colztext");

    // End display histograms.
    // ---------------------------------------------------------------

    cout<<nTracksCand<<" track candidates, "
	<<nTracksKal<<" tracks reconstructed by Kalman, "
	<<nTracksKalCut<<" tracks cut."<<endl;

    Float_t effKal = 1.F - ((Float_t)(nTracksKalCut)) / ((Float_t)(nTracksKal));
    cout<<"Efficiency = "<<effKal<<endl;

    cout<<nTracksCand<<" track candidates, "
	<<nTracksRK<<" tracks reconstructed by Runge-Kutta, "
	<<nTracksRKCut<<" tracks cut."<<endl;

    Float_t effRK = 1.F - ((Float_t)(nTracksRKCut)) / ((Float_t)(nTracksRK));
    cout<<"Efficiency = "<<effRK<<endl;

}
