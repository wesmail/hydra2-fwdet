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

void histKalRKData() {

    TString dataDir  = "./data/";

    TString dataFiles = dataDir;
    dataFiles.Append("be1210819405908.hld_dst_apr12.root");

    //Bool_t bStoreHists = kFALSE;
    //TString histFile = "hist_daf_p.root";

    const Int_t pid = -1;

    const Float_t kfChi2Cut = -1.F;

    const Float_t rkChi2Cut = -1.F;

    const Int_t maxEntries = -100;

    const Color_t kColorRK = kViolet;
    const Color_t kColorKF = kMagenta;

    const Int_t kLineWidth = 2;

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


    // Number of tracks over momentum.
    TH1F *hkfMom     = new TH1F("KF Mom", "Momentum Distribution",
				100, 0., 5000.);
    hkfMom->GetXaxis()->SetTitle("p [MeV/c]");
    hkfMom->GetYaxis()->SetTitle("Number of Tracks");

    // Number of tracks over chi2.
    TH1F *hkfChi2     = new TH1F("KF Chi2", "#Shi^{2} Distribution",
				 100, 0., 20.);
    hkfChi2->GetXaxis()->SetTitle("#chi^{2}");
    hkfChi2->GetYaxis()->SetTitle("Number of Tracks");

    // Probability distribution
    TH1F *hkfProb          = new TH1F("KF Prob", "Probability Distribution",
				      20, 0., 1.);
    hkfProb->GetXaxis()->SetTitle("Probability");
    hkfProb->GetYaxis()->SetTitle("counts");

    // Number of tracks over polar angle
    TH1F *hkfTheta    = new TH1F("KF Theta", "#Theta Distribution",
				 20, 0., 90.);
    hkfTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hkfTheta->GetYaxis()->SetTitle("Number of Tracks");

    TH2F *hkfPhiTheta  = new TH2F("hkfphitheta", "#Theta : #Phi",
				  60, 60.F, 120.F, 90, 0.F, 90.F);
    hkfPhiTheta->GetXaxis()->SetTitle("#Phi [#circ ]");
    hkfPhiTheta->GetYaxis()->SetTitle("#Theta [#circ ]");

    TH2F *hkfPhiThetaFail  = new TH2F("hkfphithetafail",
				      "#Theta : #Phi for failed tracks",
				      60, 60.F, 120.F, 90, 0.F, 90.F);
    hkfPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hkfPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");


    // ----------------------------
    // Runge-Kutta histograms
    // ----------------------------


    TH1F *hrkMom     = new TH1F("RK Mom", "Momentum Distribution",
				100, 0., 5000.);
    hrkMom->GetXaxis()->SetTitle("p [MeV/c]");
    hrkMom->GetYaxis()->SetTitle("Number of Tracks");

    TH1F *hrkChi2     = new TH1F("RK Chi2", "#chi^{2} Distribution",
				 100, 0., 20.);
    hrkChi2->GetXaxis()->SetTitle("#chi^{2}");
    hrkChi2->GetYaxis()->SetTitle("Number of Tracks");

    // Probability distribution
    TH1F *hrkProb          = new TH1F("RK Prob", "Probability Distribution",
				      20, 0., 1.);
    hrkProb->GetXaxis()->SetTitle("Probability");
    hrkProb->GetYaxis()->SetTitle("counts");

    TH1F *hrkTheta    = new TH1F("RK heta", "#Theta Distribution",
				 20, 0., 90.);
    hrkTheta->GetXaxis()->SetTitle("#Theta [#circ]");
    hrkTheta->GetYaxis()->SetTitle("Number of Tracks");

    TH2F *hrkPhiTheta  = new TH2F("hrkphitheta", "#Theta : #Phi",
				  60, 60.F, 120.F, 90, 0.F, 90.F);
    hrkPhiTheta->GetXaxis()->SetTitle("#Phi [#circ ]");
    hrkPhiTheta->GetYaxis()->SetTitle("#Theta [#circ ]");

    TH2F *hrkPhiThetaFail  = new TH2F("hrkphithetafail",
				      "#Theta : #Phi for failed tracks",
				      60, 60.F, 120.F, 90, 0.F, 90.F);
    hrkPhiThetaFail->GetXaxis()->SetTitle("#Phi [#circ ]");
    hrkPhiThetaFail->GetYaxis()->SetTitle("#Theta [#circ ]");

    // ----------------------------
    // End make histograms
    // ----------------------------



    HLoop* loop = new HLoop(kTRUE);  // kTRUE : create Hades  (needed to work with standard eventstructure)
    loop->addFiles(dataFiles);
    if(!loop->setInput("-*,+HKalTrack,+HParticleCand,+HMetaMatch2,+HRKTrackB")) {
        exit(1);
    }

    TIterator* iterCand = 0;
    if (loop->getCategory("HParticleCand")) {
        iterCand = loop->getCategory("HParticleCand")->MakeIterator();
    }

    TIterator* iterTrack = NULL;
    if (loop->getCategory("HKalTrack")) {
        iterTrack = loop->getCategory("HKalTrack")->MakeIterator();
    }

    HCategory* fCatCand = HCategoryManager::getCategory(catParticleCand);
    if(!fCatCand) {
        cout<<"No catCand "<<endl;
    }

    HCategory* fCatMeta     = HCategoryManager::getCategory(catMetaMatch);

    HCategory* fCatKalTrack    = HCategoryManager::getCategory(catKalTrack);

    HCategory* fCatRKTrack    = HCategoryManager::getCategory(catRKTrackB);

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
        HParticleCand *cand = NULL;

        while((cand = (HParticleCand*)iterCand->Next()) != 0) {

	    if(cand->getPID() != pid && pid >= 0) continue;

            if(cand->getPID() < 0) continue;

            nTracksCand ++;

            HMetaMatch2 *pMetaMatch = (HMetaMatch2*)fCatMeta->getObject(cand->getMetaInd());
            if(!pMetaMatch) {
                cout<<"No meta match found."<<endl;
                continue;
            }
            nTracksMeta++;

            HKalTrack *track = findKalman(pMetaMatch, fCatKalTrack);
            if(track) {
		Float_t Theta = track->getTheta();
		Float_t Phi   = track->getPhi();
		Float_t chi2 = track->getChi2();
		Float_t ndf  = track->getNdf();
		Float_t mom = track->getP();

		hkfMom->Fill(mom);
		hkfChi2->Fill(chi2/ndf);
		hkfTheta->Fill(Theta);

		if(chi2 >= 0.F && (kfChi2Cut < 0 || chi2 / ndf < kfChi2Cut)) {
		    hkfPhiTheta->Fill(Phi, Theta);
		    hkfProb->Fill(TMath::Prob(chi2, ndf));
		} else {
		    hkfPhiThetaFail->Fill(Phi, Theta);
		    nTracksKalCut++;
		}
		nTracksKal++;
	    } // filled Kalman histograms

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

		hrkMom->Fill(rkmom);
		hrkChi2->Fill(rkchi2/rkndf);
		hrkTheta->Fill(rkTheta);

		if(rkchi2 >= 0.F && (rkChi2Cut < 0 || rkchi2 / rkndf < rkChi2Cut)) {
		    hrkPhiTheta->Fill(rkPhi, rkTheta);
		    hrkProb->Fill(TMath::Prob(rkchi2, rkndf));
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

    TCanvas *result = new TCanvas("result", "Results", 1800, 900);
    result->Divide(4,2);

    result->cd(1);
    hkfMom->SetLineColor(kColorKF);
    hkfMom->SetLineWidth(kLineWidth);

    hrkMom->SetLineColor(kColorRK);
    hrkMom->SetLineWidth(kLineWidth);

    if(hkfMom->GetMaximum() > hrkMom->GetMaximum()) {
	hkfMom->Draw();
	hrkMom->Draw("sames");
    } else {
	hrkMom->Draw();
	hkfMom->Draw("sames");
    }

    gPad->Update();
    TPaveStats *stkfMom = (TPaveStats*)hkfMom->FindObject("stats");
    stkfMom->SetOptStat(11);
    stkfMom->SetY1NDC(stkfMom->GetY1NDC() - .05);
    stkfMom->SetY2NDC(stkfMom->GetY2NDC() - .05);
    stkfMom->SetLineColor(kColorKF);
    stkfMom->SetTextColor(kColorKF);
    stkfMom->Draw("same");

    TPaveStats *strkMom = (TPaveStats*)hrkMom->FindObject("stats");
    strkMom->SetOptStat(11);
    strkMom->SetY1NDC(strkMom->GetY1NDC() - .2);
    strkMom->SetY2NDC(strkMom->GetY2NDC() - .2);
    strkMom->SetLineColor(kColorRK);
    strkMom->SetTextColor(kColorRK);
    strkMom->Draw("sames");

    result->cd(2);
    hkfChi2->SetLineColor(kColorKF);
    hkfChi2->SetLineWidth(kLineWidth);

    hrkChi2->SetLineColor(kColorRK);
    hrkChi2->SetLineWidth(kLineWidth);

    if(hkfChi2->GetMaximum() > hrkChi2->GetMaximum()) {
	hkfChi2->Draw();
	hrkChi2->Draw("sames");
    } else {
	hrkChi2->Draw();
	hkfChi2->Draw("sames");
    }

    gPad->Update();
    TPaveStats *stkfChi2 = (TPaveStats*)hkfChi2->FindObject("stats");
    stkfChi2->SetOptStat(111);
    stkfChi2->SetY1NDC(stkfChi2->GetY1NDC() - .05);
    stkfChi2->SetY2NDC(stkfChi2->GetY2NDC() - .05);
    stkfChi2->SetLineColor(kColorKF);
    stkfChi2->SetTextColor(kColorKF);
    stkfChi2->Draw("same");

    TPaveStats *strkChi2 = (TPaveStats*)hrkChi2->FindObject("stats");
    strkChi2->SetOptStat(111);
    strkChi2->SetY1NDC(strkChi2->GetY1NDC() - .2);
    strkChi2->SetY2NDC(strkChi2->GetY2NDC() - .2);
    strkChi2->SetLineColor(kColorRK);
    strkChi2->SetTextColor(kColorRK);
    strkChi2->Draw("sames");

    result->cd(3);
    hkfProb->SetLineColor(kColorKF);
    hkfProb->SetLineWidth(kLineWidth);

    hrkProb->SetLineColor(kColorRK);
    hrkProb->SetLineWidth(kLineWidth);

    if(hkfProb->GetMaximum() > hrkProb->GetMaximum()) {
	hkfProb->Draw();
	hrkProb->Draw("sames");
    } else {
	hrkProb->Draw();
	hkfProb->Draw("sames");
    }


    result->cd(4);
    hkfTheta->SetLineColor(kColorKF);
    hkfTheta->SetLineWidth(kLineWidth);
    hkfTheta->Draw();

    hrkTheta->SetLineColor(kColorRK);
    hrkTheta->SetLineWidth(kLineWidth);
    hrkTheta->Draw("sames");

    result->cd(5);
    hkfPhiTheta->Draw("colz");

    result->cd(6);
    hkfPhiThetaFail->Draw("colz");

    result->cd(7);
    hrkPhiTheta->Draw("colz");

    result->cd(8);
    hrkPhiThetaFail->Draw("colz");


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
