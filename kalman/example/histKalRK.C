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
#include "hruntimedb.h"
#include "htool.h"

#include "hades.h"
#include "createHades.C"

#include "TIterator.h"
#include "TSystem.h"
#include "TMath.h"

#include "TCanvas.h"
#include "TColor.h"
#include "TFrame.h"
#include "TFile.h"
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

void histKalRK() {

    TString dataDir  = "/lustre/hades/user/ekrebs/kalman/auau_1250_bmax4/";

    TString dataFiles = dataDir;
    dataFiles.Append("kalman.root");

    //Bool_t bStoreHists = kFALSE;
    //TString histFile = "hist_daf_p.root";

    Int_t pid = 2;

    // To cut 1% of all tracks:
    // protons: KF: 200, kfwire: 300, daf single: 200, DAF: 8.
    // positrons: KF: 400, kfwire: 600, DAF single: 600 (1.5% cut), DAF: 600, pseudo-DAF: 6000 (2% cut), ideal+spline: 2800
    // electrons: KF: 800, kfwire: 800 (2% cut), DAF single: 800 (2% cut),  DAF: 40
    // To cut 2% of all tracks:
    // protons: KF: 100, kfwire: 150, daf single 140, DAF: 5
    // positrons: KF: 400, kfwire: 600, daf single: 600, DAF: 5, ideal+spline: 2800
    // electrons: KF: 800, kfwire: 800, DAF single: 800,  DAF: 40
    Float_t kfChi2Cut = 5.F;

    // To cut 1% of all tracks:
    // 100 for protons, ideal: 700.
    // 150 for positrons, ideal: infinity
    // 2100 for electrons
    // To cut 2% of all tracks:
    // 60 for protons, ideal: .
    // 150 for positrons, ideal: infinity
    // for electrons
    Float_t rkChi2Cut = 150.F;

    Int_t   refId   = 11200;

    Int_t maxEntries = -100;

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

    // Probability distribution
    TH1F *hkfprob          = new TH1F("hkfprob"         , "Probability Distribution", 20, 0., 1.);
    hkfprob->GetXaxis()->SetTitle("Probability");
    hkfprob->GetYaxis()->SetTitle("counts");
    hkfprob->SetLineColor(kRed);

    THStack *hskfmom = new THStack("hskfmom", "#Delta p");

    // Reconstructed momentum resolution
    TH1F *hkfdmomGeant = new TH1F("KF Momentum", "Momentum Resolution", 1000, -20., 20.);
    hkfdmomGeant->GetXaxis()->SetTitle("RMS #left( #frac{#Delta p}{p} [%] #right)");
    hkfdmomGeant->GetYaxis()->SetTitle("counts");

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
    TH1F *hkfTracksMom     = new TH1F("hkfTracksMom", "Momentum Distribution", 20, 0., 2000.);
    //marker and colour
    hkfTracksMom->GetXaxis()->SetTitle("p [MeV/c]");
    hkfTracksMom->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hkfTracksMomReco = new TH1F("hkfTracksMomReco", "Momentum Distribution Reco.", 20, 0., 2000.);
    hkfTracksMomReco->GetXaxis()->SetTitle("p [MeV/c]");
    hkfTracksMomReco->GetYaxis()->SetTitle("Number of Tracks");
    hkfTracksMomReco->SetLineColor(kRed);

    // Number of tracks over polar angle
    TH1F *hkfTracksTheta    = new TH1F("hkfTracksTheta", "#Theta Distribution", 20, 0., 90.);
    hkfTracksMom->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfTracksMom->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hkfTracksThetaReco = new TH1F("hkfTracksThetaReco", "#Theta Distribution Reco.", 20, 0., 90.);
    hkfTracksThetaReco->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfTracksThetaReco->GetYaxis()->SetTitle("Number of Tracks");
    hkfTracksThetaReco->SetLineColor(kRed);

    // Reconstruction efficiency over momentum
    TH1F *hkfEffMom        = new TH1F("hkfEffMom",    "Efficiency : p",      20, 0., 2000.);
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

    TH2F *hkfPhiThetaFail  = new TH2F("hkfphithetafail", "#Theta : #Phi for failed tracks", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hkfPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hkfPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");


    // ----------------------------
    // Runge-Kutta histograms
    // ----------------------------

    TH1F *hrkprob          = new TH1F("hrkprob"         , "Probability Distribution", 20, 0., 1.);
    hrkprob->GetXaxis()->SetTitle("Probability");
    hrkprob->GetYaxis()->SetTitle("counts");
    hrkprob->SetLineColor(kRed);

    THStack *hsrkmom = new THStack("hsrkmom", "#Delta p");
    
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

    TH1F *hrkTracksMom     = new TH1F("hrkTracksMom", "Momentum Distribution", 20, 0., 2000.);
    //marker and colour
    hrkTracksMom->GetXaxis()->SetTitle("p [MeV/c]");
    hrkTracksMom->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkTracksMomReco = new TH1F("hrkTracksMomReco", "Momentum Distribution Reco.", 20, 0., 2000.);
    hrkTracksMomReco->GetXaxis()->SetTitle("p [MeV/c]");
    hrkTracksMomReco->GetYaxis()->SetTitle("Number of Tracks");
    hrkTracksMomReco->SetLineColor(kRed);

    TH1F *hrkTracksTheta    = new TH1F("hrkTracksTheta", "#Theta Distribution", 20, 0., 90.);
    hrkTracksMom->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkTracksMom->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkTracksThetaReco = new TH1F("hrkTracksThetaReco", "#Theta Distribution Reco.", 20, 0., 90.);
    hrkTracksThetaReco->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkTracksThetaReco->GetYaxis()->SetTitle("Number of Tracks");
    hrkTracksThetaReco->SetLineColor(kRed);

    TH1F *hrkEffMom        = new TH1F("hrkEffMom",    "Efficiency : p",      20, 0., 2000.);
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

    TH2F *hrkPhiThetaFail  = new TH2F("hrkphithetafail", "#Theta : #Phi for failed tracks", 60, 60.F, 120.F, 90, 0.F, 90.F);
    hrkPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hrkPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");

    // ----------------------------
    // End make histograms
    // ----------------------------



    HLoop* loop = new HLoop(kTRUE);  // kTRUE : create Hades  (needed to work with standard eventstructure)
    loop->addFiles(dataFiles);
    if(!loop->setInput("-*,+HKalTrack,+HKalSite,+HMdcSegSim,+HMdcCal1Sim,+HParticleCandSim,+HMetaMatch2,+HRKTrackB,+HGeantKine,+HGeantMdc")) {
        exit(1);
    }

    TIterator* iterCand = 0;
    if (loop->getCategory("HParticleCandSim")) {
        iterCand = loop->getCategory("HParticleCandSim")->MakeIterator();
    }

    TIterator* iterTrack = NULL;
    if (loop->getCategory("HKalTrack")) {
        iterTrack = loop->getCategory("HKalTrack")->MakeIterator();
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

    HCategory* fCatKalTrack    = HCategoryManager::getCategory(catKalTrack);

    HCategory* fCatKalSite     = HCategoryManager::getCategory(catKalSite);

    Int_t entries = loop->getEntries();
    if(maxEntries > 0 && entries > maxEntries) entries = maxEntries;

    Int_t nbytes  = 0;

    Bool_t bWire = kFALSE;

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

        Int_t nTracks = fCatCand->getEntries();
        iterCand->Reset();
        HParticleCandSim *cand = NULL;

        while((cand = (HParticleCandSim*)iterCand->Next()) != 0) {

            if(cand->getGeantPID() != pid && pid >= 0) continue;

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
            if(!track) {
                cout<<"No Kalman track found."<<endl;
	    } else {

		Int_t idxFirst = track->getIdxFirst();
		if(idxFirst < 0) {
		    cout<<"First site not found"<<endl;
		}
		HKalSite *firstSite = (HKalSite*)fCatKalSite->getObject(idxFirst);

		Float_t momGeantFirst = firstSite->getMomReal();
		if(momGeantFirst < 0.F) {
		    cout<<"Geant momentum for first site not set!"<<endl;
		}

		if(idxFirst >= 0 && momGeantFirst >= 0.F) {

		    Float_t Theta = track->getTheta();
		    Float_t Phi   = track->getPhi();
		    Float_t chi2 = track->getChi2();
		    Float_t ndf  = track->getNdf();

		    hkfTracksMom->Fill(momGeantFirst);
		    hkfTracksTheta->Fill(Theta);

		    if(chi2 > 0.F && chi2 / ndf < kfChi2Cut) {
			Float_t momSmoo       = firstSite->getMomSmoo();
			Float_t momInput      = track    ->getMomInput();

			Float_t dMomGeantSmoo = (momGeantFirst - momSmoo) / momGeantFirst * 100.F;

			hkfdmomGeant->Fill(dMomGeantSmoo);
			hkfdmomGeantIn  ->Fill((momGeantFirst - momInput) / momGeantFirst * 100.F);

			hkfTracksMomReco->Fill(momGeantFirst);
			hkfTracksThetaReco->Fill(Theta);

			Int_t thetabin = (Int_t) (Theta / ((maxTheta - minTheta)/(nThetaSteps - 1)));
			if(thetabin > nThetaSteps - 1) {
			    thetabin = nThetaSteps - 1;
			}
			hkfdmomThetaResolution[thetabin] ->Fill(momGeantFirst, dMomGeantSmoo);

			hkfprob->Fill(TMath::Prob(chi2, ndf));

		    } else {
			hkfPhiThetaFail->Fill(Phi, Theta);
			nTracksKalCut++;
		    }
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

            Float_t rkchi2     = cand->getChi2();
            Float_t rkndf      = 4.F * 2.F - 5.F;
	    Float_t rkmom      = cand->getMomentum();
            Float_t rkTheta    = cand->getTheta();
            Float_t rkPhi      = cand->getPhi();

            if(sector >= 0.F && sector <= 5.F) {
                rkPhi -= (60.F * sector);
            } else {
                cout<<"No GeantMdc found in event "<<i<<endl;
            }

            Float_t rkmomRes = (rkmomGeant - rkmom) / rkmomGeant * 100.F;

            hrkTracksMom->Fill(rkmomGeant);
            hrkTracksTheta->Fill(rkTheta);

            if(rkchi2 >= 0.F && rkchi2 / rkndf < rkChi2Cut) {
                hrkTracksMomReco->Fill(rkmomGeant);
                hrkTracksThetaReco->Fill(rkTheta);
                hrkprob->Fill(TMath::Prob(rkchi2, rkndf));
                hrkdmomGeant->Fill(rkmomRes);

                Int_t thetabin = (Int_t) (rkTheta / ((maxTheta - minTheta)/(nThetaSteps - 1)));
                if(thetabin > nThetaSteps - 1) {
                    thetabin = nThetaSteps - 1;
                }
                hrkdmomThetaResolution[thetabin] ->Fill(rkmomGeant, rkmomRes);
            } else {
                hrkPhiThetaFail->Fill(rkPhi, rkTheta);
                nTracksRKCut++;
            }
            nTracksRK++;

        } // while cand
    } // loop over entries


    // End fill histograms.
    // ---------------------------------------------------------------

    // ---------------------------------------------------------------
    // Display histograms.
    // ---------------------------------------------------------------

    TCanvas *kfresult = new TCanvas("kfresult", "Kalman", 1200, 800);
    kfresult->Divide(3,2);

    kfresult->cd(1);
    hkfdmomGeant->Draw();
    hkfdmomGeantIn->Draw("sames");

    gPad->Update();
    TPaveStats *stGeantSmoo = (TPaveStats*)hkfdmomGeant->FindObject("stats");
    stGeantSmoo->SetOptStat(1111);
    stGeantSmoo->SetY1NDC(stGeantSmoo->GetY1NDC() - .05);
    stGeantSmoo->Draw("same");

    TPaveStats *stGeantIn = (TPaveStats*)hkfdmomGeantIn->FindObject("stats");
    stGeantIn->SetOptStat(1111);
    stGeantIn->SetY1NDC(stGeantIn->GetY1NDC() - .2);
    stGeantIn->SetY2NDC(stGeantIn->GetY2NDC() - .2);
    stGeantIn->Draw("sames");


    kfresult->cd(2);
    hkfprob->Draw();


    kfresult->cd(3);

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

    TLegend* legendMomTheta = new TLegend(0.55,0.51,0.85,0.85, "#Theta","brNDC");
    TLegendEntry* legentryMomTheta = legendMomTheta->AddEntry(hkfrms[0],"15-33 [#circ]","lpf");
    legentryMomTheta = legendMomTheta->AddEntry(hkfrms[1],"33-51 [#circ]","lpf");
    legentryMomTheta = legendMomTheta->AddEntry(hkfrms[2],"51-69 [#circ]","lpf");
    legentryMomTheta = legendMomTheta->AddEntry(hkfrms[3],"69-90 [#circ]","lpf");

    legendMomTheta->Draw("same");


    kfresult->cd(4);
    hkfPhiThetaFail->Draw("colz");

    kfresult->cd(5);
    hkfEffMom->Divide(hkfTracksMomReco, hkfTracksMom);
    hkfEffMom->Draw("EP");

    kfresult->cd(6);
    hkfEffTheta->Divide(hkfTracksThetaReco, hkfTracksTheta);
    hkfEffTheta->Draw("EP");



    TCanvas *rkresult = new TCanvas("rkresult", "Runge-Kutta", 1200, 800);
    rkresult->Divide(3,2);

    rkresult->cd(1);
    hrkdmomGeant->Draw();

    rkresult->cd(2);
    hrkprob->Draw();

    rkresult->cd(3);

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

    legendMomTheta->Draw("same");

    rkresult->cd(4);
    hrkPhiThetaFail->Draw("colz");

    rkresult->cd(5);
    hrkEffMom->Divide(hrkTracksMomReco, hrkTracksMom);
    hrkEffMom->Draw("EP");

    rkresult->cd(6);
    hrkEffTheta->Divide(hrkTracksThetaReco, hrkTracksTheta);
    hrkEffTheta->Draw("EP");

    // End display histograms.
    // ---------------------------------------------------------------

    cout<<nTracksCand<<" track candidates and "<<nTracksKal<<" tracks reconstructed by Kalman."<<endl;

    Float_t effKal = 1.F - ((Float_t)(nTracksKalCut)) / ((Float_t)(nTracksKal));
    cout<<"Efficiency = "<<effKal<<endl;

    cout<<nTracksCand<<" track candidates and "<<nTracksRK<<" tracks reconstructed by Runge-Kutta."<<endl;

    Float_t effRK = 1.F - ((Float_t)(nTracksRKCut)) / ((Float_t)(nTracksRK));
    cout<<"Efficiency = "<<effRK<<endl;

}
