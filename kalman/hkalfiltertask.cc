#include "hkalfiltertask.h"

// from ROOT
#include "TF1.h"
#include "TMath.h"
#include "TMatrixD.h"
#include "TRandom.h"

// from hydra
#include "hades.h"
#include "hcategory.h"
#include "hmdccal2parsim.h"
#include "hcategorymanager.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hmagnetpar.h"
#include "hmdcdef.h"
#include "hmdchit.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdcsizescells.h"
#include "hmdctrackgdef.h"
#include "hmdctrkcand.h"
#include "hmdctrackgfieldpar.h"
#include "hmetamatch2.h"
#include "hphysicsconstants.h"
#include "hrpccluster.h"
#include "hrktrackB.h"
#include "hruntimedb.h"
#include "hsplinetrack.h"
#include "htofhit.h"

#include "rpcdef.h"
#include "tofdef.h"

//#include "hkaldaflrwire.h"
#include "hkaldafwire.h"
#include "hkaldafsinglewire.h"
#include "hkalfilt2d.h"
#include "hkalfiltwire.h"
#include "hkalhit.h"
#include "hkaltrack.h"
#include "hkaltrackstate.h"

#include <iostream>
using namespace std;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// This class implements the track reconstruction using the Kalman filter.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

ClassImp(HKalFilterTask)

//  -----------------------------------
//  Implementation of protected methods
//  -----------------------------------

Bool_t HKalFilterTask::createKalSystem() {
    // Create a new kalsys object depending on the class variables
    // bWire, bDaf and bCompHits.
    // An existing kalsys object will be deleted.

    if(kalsys) {
        delete kalsys;
    }

    Int_t nLayersInMdc = 1;
    if(bWire) {
        nLayersInMdc = 6;
    }

    // Get magnetic field parameters if available.
    HMdcTrackGField *fMap = NULL;
    Float_t fieldFactor   = 0.F;

    // input object created during reinit().
    if(input) {
        HMagnetPar         *pMagnet = input->getMagnetPar();
        HMdcTrackGFieldPar *pField  = input->getGFieldPar();
        fieldFactor = pMagnet->getScalingFactor();
        if(pMagnet->getPolarity() < 0) {
            fieldFactor = - fieldFactor;
        }
        fMap = pField->getPointer();
    }

    kalsys = NULL;
    Int_t nMaxHitsPerTrack = 4 * nLayersInMdc;
    if(bWire) {
        if(bDaf) {
            if(bCompHits) {
                kalsys = new HKalDafWire(nMaxHitsPerTrack, measDim, stateDim, fMap, fieldFactor);
            } else {
                kalsys = new HKalDafSingleWire(nMaxHitsPerTrack, measDim, stateDim, fMap, fieldFactor);
            }
        } else {
            kalsys = new HKalFiltWire(nMaxHitsPerTrack, measDim, stateDim, fMap, fieldFactor);
        }
    } else { // segment hits
        if(bDaf) {
            Error("createKalSystem()", "Applying DAF on segment hits is not supported.");
        } else {
            // Filter reconstructed segment hits.
	    kalsys = new HKalFilt2d(nMaxHitsPerTrack, measDim, stateDim, fMap, fieldFactor);
        }
    }

    if(!kalsys) {
        Error("createKalSystem()", "Failed to create Kalman filter object.");
        return kFALSE;
    }
    return kTRUE;
}

void HKalFilterTask::fillData(HMetaMatch2* const pMetaMatch, const TObjArray &allhitsGeantMdc,
			      HMdcSeg* const inSeg, HMdcSeg* const outSeg) {
    // Fills categories with reconstruction results.
    // pMetaMatch: Meta match candidate. Index to Kalman filter is set.
    // allhitsGeantMdc:

#if kalDebug > 1
    cout<<"**** KalFilterTask: filling data ****"<<endl;
#endif

    Int_t iCatSiteFirst, iCatSiteLast = -1;
    if(bFillSites) fillDataSites(iCatSiteFirst, iCatSiteLast, allhitsGeantMdc);

    if(metaMatcher.getIsMatched()) {
        for(Int_t sys = 0; sys < 4; sys++) {
            for(Int_t tab = 0; tab < metaMatcher.getNmetaTabs(); tab++) {
                for(Int_t clstr = 0; clstr < metaMatcher.getClstrSize(sys); clstr++) {
                    const HKalMetaMatch& kalMatch = metaMatcher.getMatch(sys, tab, clstr);
                    if(kalMatch.isFilled()) {
                        Int_t iKalTrack = -1;
                        HKalTrack *pKalTrack = fillDataTrack(iKalTrack, pMetaMatch, iCatSiteFirst, iCatSiteLast, inSeg, outSeg, kalMatch);
                        switch (kalMatch.getSystem()) {
                        case 0:
                            pMetaMatch->setKalmanFilterIndRpcClst((UChar_t)tab, (Short_t)iKalTrack);
                            pKalTrack->setRpcClustInd(kalMatch.getIndex());
                            break;
                        case 1:
                            pMetaMatch->setKalmanFilterIndShowerHit((UChar_t)tab, (Short_t)iKalTrack);
                            pKalTrack->setShowerHitInd(kalMatch.getIndex());
                            break;
                        case 2:
                            switch(clstr) {
                            case 0:
                                pMetaMatch->setKalmanFilterIndTofHit1  ((UChar_t)tab, (Short_t)iKalTrack);
                                pKalTrack->setTofHitInd(kalMatch.getIndex());
                                break;
                            case 1:
                                pMetaMatch->setKalmanFilterIndTofHit2  ((UChar_t)tab, (Short_t)iKalTrack);
                                pKalTrack->setTofHitInd(kalMatch.getIndex());
                                break;
                            case 2:
                                pMetaMatch->setKalmanFilterIndTofClst  ((UChar_t)tab, (Short_t)iKalTrack);
                                pKalTrack->setTofClustInd(kalMatch.getIndex());
                                break;
                            }
                            break;
                        case 3:
                            pMetaMatch->setKalmanFilterIndEmcCluster((UChar_t)tab, (Short_t)iKalTrack);
                            pKalTrack->setEmcClustInd(kalMatch.getIndex());
                            break;
                        }
                    }
                }
            }
        }
    } else {
        HKalMetaMatch noMatch;
        Int_t iKalTrack = -1;
        fillDataTrack(iKalTrack, pMetaMatch, iCatSiteFirst, iCatSiteLast, inSeg, outSeg, noMatch);
        pMetaMatch->setKalmanFilterInd((Short_t)iKalTrack);
    }

    if(bSim && (kalsys->getNsites() > allhitsGeantMdc.GetEntries()) && bPrintWarn) {
        Warning("fillData()", "There are %i sites in the Kalman system, but only %i Geant hits.",
                kalsys->getNsites(), allhitsGeantMdc.GetEntries());
    }
}

HKalTrack* HKalFilterTask::fillDataTrack(Int_t &iKalTrack, HMetaMatch2* const pMetaMatch,
                                         Int_t iCatSiteFirst, Int_t iCatSiteLast,
                                         HMdcSeg* const inSeg, HMdcSeg* const outSeg,
					 const HKalMetaMatch &kalMatch) const {

    Kalman::kalFilterTypes filtType = Kalman::kFiltered;
    if(kalsys->getDoSmooth()) {
        filtType = Kalman::kSmoothed;
    }

    Int_t fromSite; //, toSite;  //unused
    if(kalsys->getDirection() == kIterForward) {
        fromSite = 0;
        //toSite   = kalsys->getNsites() - 1; //unused
    } else {
        fromSite = kalsys->getNsites() - 1;
        //toSite   = 0;                      //unused
    }

    HKalTrack *pKalTrack = NULL;
    iKalTrack = -1;

    if((pKalTrack = HCategoryManager::newObject(pKalTrack, fCatKalTrack, iKalTrack)) != 0 ) {

        pKalTrack->setIdxFirst(iCatSiteFirst);
        pKalTrack->setIdxLast(iCatSiteLast);

        //-------------------------
        // Fill HKalTrack.
	//-------------------------
        pKalTrack->setBetaInput  (kalsys->getBetaInput());
        pKalTrack->setChi2       (kalsys->getChi2());
        pKalTrack->setIsWireHit  (kalsys->getHitType() == Kalman::kWireHit);
        pKalTrack->setNdf        (kalsys->getNdf());
        pKalTrack->setTrackLength(kalsys->getTrackLength());

        Int_t pid = kalsys->getPid();
	pKalTrack->setPid(pid);

	const TVectorD& inisv = kalsys->getIniStateVec();
	pKalTrack->setXinput(inisv(Kalman::kIdxX0));
	pKalTrack->setYinput(inisv(Kalman::kIdxY0));
	pKalTrack->setTxInput(inisv(Kalman::kIdxTanPhi));
	pKalTrack->setTyInput(inisv(Kalman::kIdxTanTheta));
	Float_t iniqp = TMath::Abs(inisv(Kalman::kIdxQP));
        if(iniqp != 0.F) {
            pKalTrack->setMomInput(TMath::Abs(HPhysicsConstants::charge(pid) / iniqp));
        }

        // Fill HBaseTrack.
        Float_t qp = (kalsys->getSite(fromSite)->getStateParam(filtType, Kalman::kIdxQP));
        if(qp != 0.F) {
            pKalTrack->setP(TMath::Abs(HPhysicsConstants::charge(pid) / qp),
                        (kalsys->getSite(fromSite)->getStateCovMat(filtType))(Kalman::kIdxQP, Kalman::kIdxQP));
        }
        pKalTrack->setPolarity(TMath::Sign(1.F, qp));

        //? from kalman
        pKalTrack->setR    (inSeg->getR()     * TMath::RadToDeg(), inSeg->getErrR()     * TMath::RadToDeg());
        pKalTrack->setZ    (inSeg->getZ()     * TMath::RadToDeg(), inSeg->getErrZ()     * TMath::RadToDeg());
        pKalTrack->setTheta(inSeg->getTheta() * TMath::RadToDeg(), inSeg->getErrTheta() * TMath::RadToDeg());
        pKalTrack->setPhi  (inSeg->getPhi()   * TMath::RadToDeg(), inSeg->getErrPhi()   * TMath::RadToDeg());

        pKalTrack->setSector(kalsys->getSite(0)->getSector());

	Short_t splineTrackIndex = pMetaMatch->getSplineInd();
	if(splineTrackIndex >= 0) {
	    HSplineTrack *pSplineTrack = (HSplineTrack*)fCatSplineTrack->getObject(splineTrackIndex);
	    pKalTrack->setTarDist   (pSplineTrack->getTarDist());
	    pKalTrack->setIOMatching(pSplineTrack->getIOMatching());
	}

        if(kalMatch.isFilled()) {
            // Fill information from Meta detector.
            pKalTrack->setTofDist(kalsys->getTrackLength());

            pKalTrack->setBeta     (kalMatch.getBeta());
            pKalTrack->setMass2    (kalMatch.getMass2(), 0.F); //? Error = 0 as in HRKTrackBF2
            pKalTrack->setMetaEloss(kalMatch.getMetaEloss());
            pKalTrack->setTof      (kalMatch.getTof());

            //pKalTrack->setShowerHitInd(Short_t shower);

            //pKalTrack->setMetaHitInd(Short_t shower, Short_t tof)
            switch (kalMatch.getSystem()) {
            case 0:
                pKalTrack->setQualityRpc(kalMatch.getQuality());
                break;
            case 1:
                pKalTrack->setQualityShower(kalMatch.getQuality());
                break;
            case 2:
                pKalTrack->setQualityTof(kalMatch.getQuality());
                break;
            case 3:
                pKalTrack->setQualityEmc(kalMatch.getQuality());
                break;
            }

            Float_t xMetaHit, yMetaHit, zMetaHit, xMetaReco, yMetaReco, zMetaReco;
            kalMatch.getMetaHitLocal(xMetaHit, yMetaHit, zMetaHit);
            kalMatch.getMetaRecoLocal(xMetaReco, yMetaReco, zMetaReco);
            pKalTrack->setMETAdx(xMetaHit - xMetaReco);
            pKalTrack->setMETAdy(yMetaHit - yMetaReco);
            pKalTrack->setMETAdz(zMetaHit - zMetaReco);
        }
    } // filled track
    return pKalTrack;
}

void HKalFilterTask::fillDataSites(Int_t &iFirstSite, Int_t &iLastSite,
                                   const TObjArray &allhitsGeantMdc) {
    // Fills categories for measurement sites and hits with reconstruction results.
    // Storing these categories consumes large amounts of disk space and is mainly
    // intended for debugging purposes.
    //
    // Returns the indices for the first and last measurement site needed by the
    // main track category.
    //
    // output:
    // iFirstSite: index of first measurement site in catagory catKalSite.
    // iLastSite:  index of last measurement site in catagory catKalSite.
    //
    // input:
    // allhitsGeantMdc: Array with Geant hits in MDCs.

    iFirstSite = -1;
    iLastSite  = -1;

    Int_t pid = kalsys->getPid();

    for(Int_t iSite = 0; iSite < getNsites(); iSite++) {

        HKalTrackSite *trackSite = kalsys->getSite(iSite);
        if(!trackSite->getIsActive()) continue;

        // Find the Geant hit corresponding to a measurement site.
        // ---------
        HGeantMdc *jean = NULL;
        if(bSim) {
            if(kalsys->getNsites() <= allhitsGeantMdc.GetEntries()) {
                Int_t j = -1;
                Bool_t bGeantFound = kFALSE;
                while(!bGeantFound && j < (allhitsGeantMdc.GetEntries() - 1)) {
                    j++;
                    jean = (HGeantMdc*)allhitsGeantMdc.At(j);
                    if(jean->getSector() == trackSite->getSector() &&
                       jean->getModule() == trackSite->getModule() &&
                       (jean->getLayer() == trackSite->getLayer() || jean->getLayer() == 6)) { // Midplane in HGeant is 6.
                        bGeantFound = kTRUE;
                    }
                }
                if(!bGeantFound) {
                    if(bPrintWarn) {
                        Warning("writeOutput()", Form("Could not find a Geant hit belonging to measurement site %i.", iSite));
                    }
                    jean = NULL;
                }
            }
        }
        // ---------

        HKalSite *site  = NULL;
        Int_t     index = -1;
        if((site = HCategoryManager::newObject(site, fCatKalSite, index)) != 0 ) {

            if(iFirstSite < 0) iFirstSite = index;
            iLastSite = index;

            //-------------------------
            // Write data about the site.
            //-------------------------

            site->setSec           (trackSite->getSector());
            site->setMod           (trackSite->getModule());
            site->setLay           (trackSite->getLayer());
            site->setChi2          (trackSite->getChi2());
            site->setEnerLoss      (trackSite->getEnergyLoss());

            Double_t momFilt = trackSite->getStateParam(Kalman::kFiltered, Kalman::kIdxQP);
            if(momFilt != 0.F) {
                momFilt = HPhysicsConstants::charge(pid) / momFilt;
            }
            site->setMomFilt       ((Float_t)momFilt);
            Double_t momSmoo = trackSite->getStateParam(Kalman::kSmoothed, Kalman::kIdxQP);
            if(momSmoo != 0.F) {
                momSmoo = HPhysicsConstants::charge(pid) / momSmoo;
            }
	    site->setMomSmoo       ((Float_t)momSmoo);

	    Kalman::kalFilterTypes recoState = (kalsys->getDoSmooth()) ? Kalman::kSmoothed : Kalman::kFiltered;
            site->setTxReco(trackSite->getStateParam(recoState, Kalman::kIdxTanPhi));
	    site->setTyReco(trackSite->getStateParam(recoState, Kalman::kIdxTanTheta));

            const TMatrixD& C = trackSite->getState(recoState).getCovMat();
	    site->setCxxReco(C[Kalman::kIdxX0][Kalman::kIdxX0]);
	    site->setCyyReco(C[Kalman::kIdxY0][Kalman::kIdxY0]);
	    site->setCtxReco(C[Kalman::kIdxTanPhi][Kalman::kIdxTanPhi]);
	    site->setCtyReco(C[Kalman::kIdxTanTheta][Kalman::kIdxTanTheta]);
	    site->setCqpReco(C[Kalman::kIdxQP][Kalman::kIdxQP]);

	    if(jean) {
                TVector3 jeanPos, jeanDir;
		input->getPosAndDir(jeanPos, jeanDir, jean, kTRUE);
                site->setTxReal(jeanDir.X() / jeanDir.Z());
                site->setTyReal(jeanDir.Y() / jeanDir.Z());
                Float_t ax, ay, atof, ptof;
                jean->getHit(ax,ay,atof,ptof);
                site->setMomReal       (ptof);
                site->setTrackNum(jean->getTrack());
            }

            site->setNcomp(trackSite->getNcompetitors());



            if(kalsys->getHitType() == Kalman::kWireHit) {
                HKalHitWire *hit = NULL;
                Int_t indexHit = -1;

                //---------------------------------------
                // Write data about the site's wire hits.
                //---------------------------------------
                for(Int_t iHit = 0; iHit < trackSite->getNcompetitors(); iHit++) {
                    if((hit = HCategoryManager::newObject(hit, fCatKalHitWire, indexHit)) != 0 ) {

                        if(iHit == 0) {
                            site->setIdxFirst(indexHit);
                        }
                        if(iHit == trackSite->getNcompetitors() - 1) {
                            site->setIdxLast(indexHit);
                        }

                        hit->setCell          (trackSite->getCell(iHit));
                        hit->setTime1         (trackSite->getHit(iHit).getTime1());
                        hit->setTime2         (trackSite->getHit(iHit).getTime2());
                        hit->setTime1Err      (trackSite->getHitDriftTimeErr(iHit));
                        hit->setTimeMeas      (trackSite->getHitDriftTime(iHit));

                        Float_t alpha = trackSite->getHitImpactAngle(iHit);
                        hit->setAlpha         (alpha);
                        Float_t mindist = trackSite->getHitVec(iHit)(0);
                        hit->setMinDist       (mindist);
                        HMdcCal2ParSim *cal2  = (HMdcCal2ParSim*)gHades->getRuntimeDb()->getContainer("MdcCal2ParSim");
                        Float_t timeReco = cal2->calcTime(trackSite->getSector(), trackSite->getModule(), alpha, mindist);
                        hit->setTimeReco      (timeReco);

                        hit->setTimeTof       (trackSite->getHitTimeTof(iHit));
                        hit->setTimeTofCal1   (trackSite->getHit(iHit).getTimeTofCal1());
                        hit->setTimeWireOffset(trackSite->getHit(iHit).getTimeWireOffset());

                        if(kalsys->getDoDaf()) {
                            Int_t nDafs = kalsys->getNdafs();
                            hit->setChi2Daf(trackSite->getHitChi2(iHit));
                            for(Int_t iDaf = 0; iDaf < nDafs; iDaf++) {
                                hit->setWeight(trackSite->getHitWeightHist(iDaf, iHit), iDaf);
                            }
                        }
                    } else {
                        Error("fillDataSites()", "Failed to create hit object");
                    }
                }
            } // filled wire hits

            //------------------------------------------
            // Write data about the site's segment hits.
            // -----------------------------------------
            if(kalsys->getHitType() == Kalman::kSegHit) {
		HKalHit2d *hit = NULL;
                Int_t indexHit = -1;
                if((hit = HCategoryManager::newObject(hit, fCatKalHit2d, indexHit)) != 0 ) {


                    site->setIdxFirst(indexHit);
                    site->setIdxLast(indexHit);

                    hit->setXmeas(trackSite->getHitVec()(0));
                    hit->setYmeas(trackSite->getHitVec()(1));

                    if(kalsys->getDoSmooth()) {
                        hit->setXreco(trackSite->getStateParam(Kalman::kSmoothed, Kalman::kIdxX0));
                        hit->setYreco(trackSite->getStateParam(Kalman::kSmoothed, Kalman::kIdxY0));
                    } else {
                        hit->setXreco(trackSite->getStateParam(Kalman::kFiltered, Kalman::kIdxX0));
                        hit->setYreco(trackSite->getStateParam(Kalman::kFiltered, Kalman::kIdxY0));
		    }
		    if(jean) {
			TVector3 jeanPos, jeanDir;
			input->getPosAndDir(jeanPos, jeanDir, jean, kTRUE);
                        hit->setXreal(jeanPos.X());
                        hit->setYreal(jeanPos.Y());
		    }
		} else {
		    Error("fillDataSites()", "Failed to create hit object");
                }
            } // filled segment hit
        } // filled one site
    } // filled all sites
}

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalFilterTask::HKalFilterTask(Int_t ref, Bool_t sim, Bool_t wire,
			       Bool_t daf, Bool_t comp)
: HReconstructor("KalmanFilterTask", "Kalman filter"),
noKalman("Kalman filter not found."), fMomErr(NULL), fTxErr(NULL),
fTyErr(NULL), materials(NULL), iniSvMethod(0), numTracks(0), numTracksErr(0),
dopid(0), fGetPid(NULL) {
    // Task to run the Kalman filter.
    //
    // Input:
    // ref:   Run ID number.
    // sim:   Simulation/Data.
    // wire:  Run Kalman filter Mdc drift chamber hits or on segment hits.
    // daf:   Apply the Deterministic Annealing Filter (DAF).

    bCompHits  = comp;
    bDaf       = daf;
    bGeantPid  = kFALSE;
    bSim       = sim;
    bWire      = wire;

    bFillSites = kTRUE;

    refId   = ref;

    if(bWire) {
        measDim = 1;
    } else {
        measDim = 2;
    }
    stateDim = 5;

    counterstep  = -1;
    bPrintWarn   = kFALSE;
    bPrintErr    = kFALSE;

    errX    = 0.2; // Segment fit resolution.
    errY    = 0.1;
    errTx   = TMath::Tan(.5 * TMath::DegToRad());
    errTy   = TMath::Tan(.5 * TMath::DegToRad());
    errMom  = 0.05; // ca. Spline resolution.

    // Created later on in init() or reinit().
    input  = NULL;
    kalsys = NULL;

    fCatKalTrack   = NULL;
    fCatKalSite    = NULL;
    fCatKalHitWire = NULL;
    fCatKalHit2d   = NULL;
    fCatMetaMatch   = NULL;
    fCatSplineTrack = NULL;

    if(!createKalSystem()) {
        Error("HKalFilterTask()", "Could not create KalSystem.");
    }
}

HKalFilterTask::~HKalFilterTask(){
    delete fMomErr;
    delete materials;
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Int_t HKalFilterTask::getIniPid(HMdcTrkCand const* const cand) const {
    // Get particle hypothesis.

    Int_t pid = HPhysicsConstants::pid("p");

    if(bGeantPid && bSim) {
	HGeantKine *kine = input->getGeantKine(cand);
	if(!kine) return -1;

	Int_t track = -1;
	kine->getParticle(track, pid);
	return pid;
    } else {
	HMetaMatch2 *pMetaMatch =
	    (HMetaMatch2*)fCatMetaMatch->getObject(cand->getMetaMatchInd());

	if(!pMetaMatch) return pid;

	Short_t splineTrackIndex = pMetaMatch->getSplineInd();
	HSplineTrack *pSplineTrack =
	    (HSplineTrack*)fCatSplineTrack->getObject(splineTrackIndex);

	Int_t charge = (Int_t)pSplineTrack->getPolarity();

	Double_t mom = TMath::Max(pSplineTrack->getP(), 100.F);

	UChar_t n = 0;
	Int_t idTof = -1;
	Int_t idRpc = -1;
	while(n < 3 && idTof < 0) {
	    idTof = pMetaMatch->getTofHit1Ind(n);
	    n++;
	}
	n = 0;
	while(n < 3 && idTof < 0) {
	    idTof = pMetaMatch->getTofHit2Ind(n);
	    n++;
	    }
	n = 0;
	while(n < 3 && idRpc < 0) {
	    idRpc = pMetaMatch->getRpcClstInd(n);
	    n++;
	}

	Float_t dist = 0.;
	Float_t tof  = -1.;

	if(idTof >= 0) {
	    HTofHit *tofHit = (HTofHit*)fCatTofHit->getObject(idTof);
	    tofHit->getDistance(dist);
	    dist = dist / 1.e3;
	    tof  = tofHit->getTof() / 1.e9;
	}

	if(idRpc >= 0) {
	    HRpcCluster *rpcHit = (HRpcCluster*)fCatRpcClst->getObject(idRpc);
	    Float_t x, y, z = 0.;
	    rpcHit->getXYZLab(x, y, z);
	    dist = TMath::Sqrt(x*x + y*y + z*z) / 1.e3;
	    tof  = rpcHit->getTof() / 1.e9;
	}

	if(idTof >= 0 || idRpc >= 0) {
	    Double_t beta = (dist / tof) / TMath::C();
	    kalsys->setBetaInput(beta);

	    if(mom < 500. && beta > 0.95) {
		pid = (charge > 0) ? HPhysicsConstants::pid("e+") :
		    HPhysicsConstants::pid("e-");
	    } else {
		Double_t betaPi = mom /
		    (TMath::Sqrt(mom*mom + HPhysicsConstants::mass(HPhysicsConstants::pid("pi+"))));

		Double_t betaP = mom /
		    (TMath::Sqrt(mom*mom + HPhysicsConstants::mass(HPhysicsConstants::pid("p"))));

		if(charge > 0 &&
		   (TMath::Abs(beta - betaP) < TMath::Abs(beta - betaPi))) {
		    pid = HPhysicsConstants::pid("p");
		} else {
		    pid = (charge > 0) ? HPhysicsConstants::pid("pi+") :
			HPhysicsConstants::pid("pi-");
		}
	    }
	} else {
	    pid = (charge > 0) ? HPhysicsConstants::pid("p") :
		HPhysicsConstants::pid("pi-");
	}
    }
    return pid;
}

Bool_t HKalFilterTask::getIniSv(TVectorD &inisv, TMatrixD &iniC,
                                Int_t pid, const TObjArray &allhits,
                                HMetaMatch2 const* const pMetaMatch,
                                HMdcSeg const* const inSeg,
                                HMdcSeg const* const outSeg) const {
    // Calculates initial state vector and covariance matrix for
    // the Kalman filter.
    // Position is derived from first measurement,
    // direction from MDC segments and momentum is taken from Spline fit.
    //
    // output:
    // inisv: Initial state vector.
    // iniC:  Initial covariance matrix.
    // input:
    // pid:        GEANT particle ID.
    // allhits:    Array with MDC hits. Must be of class HKalMdcHit.
    // pMetaMatch: Pointer to Meta match.
    // inSeg:      Pointer to inner MDC segment.
    // outSeg:     Pointer to outer MDC segment.

    if(!pMetaMatch || !inSeg || !outSeg) return kFALSE;

    if(inisv.GetNrows() != stateDim) {
        inisv.ResizeTo(stateDim);
    }
    if(iniC.GetNrows() != stateDim || iniC.GetNcols() != stateDim) {
        iniC.ResizeTo(stateDim, stateDim);
    }


    //---------------------------------------------------------------------
    // Calculate initial values for Kalman filter.
    //---------------------------------------------------------------------

    // Calculate starting position and direction of Geant hit.
    TVector3 inipos, inidir;

    // Derive starting position and direction from segments.
    //? check if first measurement is in mod 0/3
    Int_t iStartHit = 0;
    if(kalsys->getDirection() == kIterBackward) {
        iStartHit = allhits.GetEntries() - 1;
    }

    ((HKalMdcHit*)allhits.At(iStartHit))->getHitVec3(inipos); // Impact with drift cell calculated in input class.

    if(kalsys->getDirection() == kIterForward) {
        inidir.SetPtThetaPhi(1., inSeg->getTheta(), inSeg->getPhi());
    } else {
        inidir.SetPtThetaPhi(1., outSeg->getTheta(), outSeg->getPhi());
    }

    // Get starting momentum from spline.
    Double_t iniTrackMom = 0.;
    Short_t splineTrackIndex = pMetaMatch->getSplineInd();
    if(splineTrackIndex >= 0) {
        HSplineTrack *pSplineTrack = (HSplineTrack*)fCatSplineTrack->getObject(splineTrackIndex);
        // Use momentum from Spline task as input for KF.
        iniTrackMom = TMath::Max(pSplineTrack->getP(), 100.F);
    } else {
        if(bPrintWarn) {
            Warning("execute()", "SplineTrack did not provide information for Kalman filter.");
        }
        return kFALSE;
    }

    if(iniTrackMom == 0.) return kFALSE;

    // Create a starting state vector for the Kalman Filter.
    Int_t particleCharge = HPhysicsConstants::charge(pid);

    HKalTrackState::calcStateVec(inisv, particleCharge/iniTrackMom, inipos, inidir);

    // Initialize covariance.
    iniC(kIdxX0, kIdxX0)             = TMath::Power(errX, 2);
    iniC(kIdxY0, kIdxY0)             = TMath::Power(errY, 2);

    Double_t dTx = errTx;
    Double_t dTy = errTy;

    // Direction error from segment hits.
    if(!fTxErr || !fTyErr) {
	const HMdcSeg *mdcSeg = (kalsys->getDirection() == kIterForward) ? inSeg : outSeg;
        HCategory* mdcHitCat = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcHit);
	HMdcHit* locHit1 = (HMdcHit*)mdcHitCat->getObject(mdcSeg->getHitInd(0));
	HMdcHit* locHit2 = (HMdcHit*)mdcHitCat->getObject(mdcSeg->getHitInd(1));
	if(locHit1 && locHit2) {
	    HMdcSizesCells* fSizesCells = HMdcSizesCells::getExObject();

	    Double_t x1 = locHit1->getX();
	    Double_t y1 = locHit1->getY();
	    Double_t z1 = 0;
	    Int_t sec1, mod1;
	    locHit1->getSecMod(sec1, mod1);
	    (*fSizesCells)[sec1][mod1].transFrom(x1, y1, z1);

	    Double_t x2 = locHit2->getX();
	    Double_t y2 = locHit2->getY();
	    Double_t z2 = 0;
	    Int_t sec2, mod2;
	    locHit2->getSecMod(sec2, mod2);
	    (*fSizesCells)[sec2][mod2].transFrom(x2, y2, z2);

	    Double_t errx1 = locHit1->getErrX();
	    Double_t erry1 = locHit1->getErrY();
	    Double_t errz1 = 0.03;
	    HGeomVector errLay1(errx1, erry1, errz1);
	    HGeomVector errSec1 = (*fSizesCells)[sec1][mod1].getSecTrans()->getRotMatrix() * errLay1;
	    errx1 = errSec1.getX();
	    erry1 = errSec1.getY();
	    errz1 = errSec1.getZ();

	    Double_t errx2 = locHit2->getErrX();
	    Double_t erry2 = locHit2->getErrY();
	    Double_t errz2 = 0.03;
	    HGeomVector errLay2(errx2, erry2, errz2);
	    HGeomVector errSec2 = (*fSizesCells)[sec2][mod2].getSecTrans()->getRotMatrix() * errLay2;
	    errx2 = errSec2.getX();
	    erry2 = errSec2.getY();
	    errz2 = errSec2.getZ();

	    Double_t x21 = x2 - x1;
	    Double_t y21 = y2 - y1;
	    Double_t z21 = z2 - z1;

	    dTx = TMath::Sqrt(TMath::Power(errz1*x21/(z21*z21), 2.)+
			      TMath::Power(errz2*x21/(z21*z21), 2.)+
			      TMath::Power(errx1/(z21), 2.) +
                              TMath::Power(errx2/(z21), 2.));
	    dTy = TMath::Sqrt(TMath::Power(errz1*y21/(z21*z21), 2.)+
			      TMath::Power(errz2*y21/(z21*z21), 2.)+
			      TMath::Power(errx1/(z21), 2.) +
			      TMath::Power(errx2/(z21), 2.));
	}
    }

    // Custom function to calculate errors.
    if(fTxErr) {
	dTx = fTxErr->Eval(inSeg->getTheta() * TMath::RadToDeg());
    }
    if(fTyErr) {
	dTy = fTyErr->Eval(inSeg->getTheta() * TMath::RadToDeg());
    }

    iniC(kIdxTanPhi,   kIdxTanPhi)   = TMath::Power(dTx, 2);
    iniC(kIdxTanTheta, kIdxTanTheta) = TMath::Power(dTy, 2);

    // The variance of the estimated momentum is
    // <(mom_real - mom_input)^2> = (mom_real - (1+errMom)*mom_real)^2 = (errMom * mom_real)^2.
    // The variance of the estimation error of the state parameter q/p
    // then is <(1/mom_geant - 1/mom_input)^2>.

    Double_t dp = errMom;
    if(fMomErr) {
	dp = fMomErr->Eval(inSeg->getTheta() * TMath::RadToDeg());
    }
    iniC(kIdxQP, kIdxQP) = TMath::Power(dp/(iniTrackMom * (dp + 1.)), 2);

    if(iniC.GetNrows() == 6) {
        iniC(kIdxZ0, kIdxZ0) = TMath::Power(errY, 2);
    }

    return kTRUE;
}

Bool_t HKalFilterTask::getIniSvGeant(TVectorD &inisv, TMatrixD &iniC,
                                     const TObjArray &allhitsGeantMdc,
				     Int_t pid, Bool_t bSmear) const {
    // Calculate starting position and direction of Geant hit.
    TVector3 inipos, inidir;

    Int_t iStartHitGeant = 0;
    if(kalsys->getDirection() == kIterBackward) {
        iStartHitGeant = allhitsGeantMdc.GetEntries() - 1;
    }

    if(allhitsGeantMdc.GetEntries() == 0) {
        if(bPrintErr) {
            Error("getIniSvGeant()", "No Geant points found.");
        }
        return kFALSE;
    }

    HGeantMdc *firstGeant = (HGeantMdc*)allhitsGeantMdc.At(iStartHitGeant);
    if(firstGeant == NULL) {
        if(bPrintErr) {
            Error("getIniSvGeant()", "Could not retrieve Geant point %i", iStartHitGeant);
        }
        return kFALSE;
    }

    input->getPosAndDir(inipos, inidir, firstGeant, kTRUE);

    if(bSmear) {
	inipos.SetX(inipos.X() + gRandom->Gaus(0., errX));
	inipos.SetY(inipos.Y() + gRandom->Gaus(0., errY));
    }

    // Use smeared Geant momentum as input for KF.
    Float_t ax, ay, atof, momGeant;
    firstGeant->getHit(ax, ay, atof, momGeant);
    Double_t iniTrackMom = (bSmear) ? momGeant * (1. + gRandom->Gaus() * errMom) : momGeant;

    Int_t particleCharge = HPhysicsConstants::charge(pid);
    HKalTrackState::calcStateVec(inisv, particleCharge/iniTrackMom, inipos, inidir);

    if(bSmear) {
	inisv[kIdxTanPhi]   = inisv[kIdxTanPhi]   + gRandom->Gaus(0., errTx);
	inisv[kIdxTanTheta] = inisv[kIdxTanTheta] + gRandom->Gaus(0., errTy);
    }

    // Initialize covariance.
    iniC(kIdxX0, kIdxX0)             = TMath::Power(errX, 2);
    iniC(kIdxY0, kIdxY0)             = TMath::Power(errY, 2);
    iniC(kIdxTanPhi,   kIdxTanPhi)   = TMath::Power(errTx, 2);
    iniC(kIdxTanTheta, kIdxTanTheta) = TMath::Power(errTy, 2);
    // The variance of the estimated momentum is
    // <(mom_real - mom_input)^2> = (mom_real - (1+errMom)*mom_real)^2 = (errMom * mom_real)^2.
    // The variance of the estimation error of the state parameter q/p
    // then is <(1/mom_geant - 1/mom_input)^2>.
    iniC(kIdxQP, kIdxQP)             = TMath::Power(1./iniTrackMom * errMom / (errMom + 1.), 2);

    if(iniC.GetNrows() == 6) {
        iniC(kIdxZ0, kIdxZ0) = TMath::Power(errY, 2);
    }

    return kTRUE;
}

Bool_t HKalFilterTask::getIniSvRungeKutta(TVectorD &inisv, TMatrixD &iniC, Int_t pid,
                                const TObjArray& allhits,
                                HMetaMatch2 const* const pMetaMatch) const {
    // Retrieve initial state from Runge-Kutta fit.

    Int_t idx = -1;
    // Find any Runge-Kutta object from MetaMatch.
    for(UChar_t n = 0; n < 3; n++) {
        idx = pMetaMatch->getRungeKuttaIndRpcClst(n);
        if(idx >= 0) break;
        idx = pMetaMatch->getRungeKuttaIndTofHit1(n);
        if(idx >= 0) break;
        idx = pMetaMatch->getRungeKuttaIndTofHit2(n);
        if(idx >= 0) break;
        idx = pMetaMatch->getRungeKuttaIndTofClst(n);
        if(idx >= 0) break;
        idx = pMetaMatch->getRungeKuttaIndShowerHit(n);
        if(idx >= 0) break;
        idx = pMetaMatch->getRungeKuttaIndEmcCluster(n);
        if(idx >= 0) break;
    }
    if(idx < 0) idx = pMetaMatch->getRungeKuttaInd();
    if(idx < 0) return kFALSE;

    HRKTrackB *prk = (HRKTrackB*)(HCategoryManager::getCategory(catRKTrackB)->getObject(idx));
    Double_t hpi   = TMath::Pi()/2.;
    Double_t r     = prk->getRSeg1RK();
    Double_t phi   = prk->getPhiSeg1RK();
    Double_t theta = prk->getThetaSeg1RK();

    TVector3 posSeg, dirSeg;
    posSeg.SetXYZ(r * TMath::Cos(phi + hpi),
                  r * TMath::Sin(phi + hpi),
                  prk->getZSeg1RK());
    dirSeg.SetXYZ(TMath::Cos(phi) * TMath::Sin(theta),
                  TMath::Sin(phi) * TMath::Sin(theta),
                  TMath::Cos(theta));
    Double_t particleCharge = (Double_t)HPhysicsConstants::charge(pid);
    Double_t iniTrackMom = prk->getP();
    Double_t QPSeg = particleCharge / iniTrackMom;

    TVectorD svSeg(getStateDim());
    HKalTrackState::calcStateVec(svSeg, QPSeg, posSeg, dirSeg);

    HKalMdcHit *hit = (HKalMdcHit*)allhits.At(0);

    HKalMdcMeasLayer planeSeg(posSeg, hit->getMeasLayer().getNormal(),
                              hit->getMeasLayer().getMaterialPtr(kFALSE),
                              hit->getMeasLayer().getMaterialPtr(kFALSE),
                              -1, hit->getSector(), 0, 1, 0.);

    Bool_t ms = kalsys->getDoMultScat();
    Bool_t el = kalsys->getDoEnerLoss();
    kalsys->setDoMultScat(kFALSE);
    kalsys->setDoEnerLoss(kFALSE);

    // Propagate track to vertex.
    if(!kalsys->propagateToPlane(inisv, svSeg,
                                 planeSeg, hit->getMeasLayer(),
                                 pid, kIterForward)) {
        kalsys->setDoMultScat(ms);
        kalsys->setDoEnerLoss(el);
        return kFALSE;
    }
    kalsys->setDoMultScat(ms);
    kalsys->setDoEnerLoss(el);

    iniC(kIdxX0, kIdxX0)             = TMath::Power(errX, 2);
    iniC(kIdxY0, kIdxY0)             = TMath::Power(errY, 2);
    iniC(kIdxTanPhi,   kIdxTanPhi)   = TMath::Power(errTx, 2);
    iniC(kIdxTanTheta, kIdxTanTheta) = TMath::Power(errTy, 2);
    // The variance of the estimated momentum is
    // <(mom_real - mom_input)^2> = (mom_real - (1+errMom)*mom_real)^2 = (errMom * mom_real)^2.
    // The variance of the estimation error of the state parameter q/p
    // then is <(1/mom_geant - 1/mom_input)^2>.
    iniC(kIdxQP, kIdxQP)             = TMath::Power(1./iniTrackMom * errMom / (errMom + 1.), 2);

    return kTRUE;
}

Bool_t HKalFilterTask::doPid(Int_t pid) {
    // Returns true if particles with Geant ID pid are going to be filtered.

    if(!bSim) return kTRUE;
    if(pid < 0) return kFALSE;
    Int_t n = dopid.GetSize();
    if(n == 0) return kTRUE;
    for(Int_t i = 0; i < n; i++) {
        if(dopid.At(i) == pid) return kTRUE;
    }

    return kFALSE;
}

Int_t HKalFilterTask::execute(void) {

    TObjArray allhits; // Array that stores the hits.
    TObjArray allhitsGeantMdc;

    Int_t    numTracksErrEvent = 0;
    Int_t    numTracksEvent    = 0;

    UInt_t evtNbr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

    //----------print counter----------------
    if (counterstep > 0 && evtNbr % counterstep == 0) {
        cout<<"**************** Event "<<evtNbr<<" ********************"<<endl;
    }

    HMdcTrkCand* cand = 0;

    //input->resetIterMdcTrkCand();
    if(input) {
        input->resetIterMetaMatch();
    } else {
        Error("execute()", "HKalInput not found. Unable to execute Kalman filter task.");
        return 0;
    }

    while((cand = nextTrack(allhits))) {
#if kalDebug > 1
        cout<<"\n\n\n"<<endl;
        cout<<"*****************************************************"<<endl;
        cout<<"**************** New track candidate ****************"<<endl;
        cout<<"*****************************************************"<<endl;
#endif
        if(allhits.GetEntries() < 3) continue;

	HGeantKine *kine = (bSim ? input->getGeantKine(cand) : NULL);
        if(bSim && !kine) {
            Error("execute()", "No HGeantKine object found.");
            continue;
        }

        HMetaMatch2 *pMetaMatch = (HMetaMatch2*)fCatMetaMatch->getObject(cand->getMetaMatchInd());
        if(!pMetaMatch) {
            if(bPrintWarn) {
                Error("execute()", Form("No META match in track %i of event %i.", numTracksEvent, evtNbr));
            }
            continue;
        }

        HMdcSeg* segs[2];
        input->getMdcSegs(cand, segs);
        if(segs[0] == 0 || segs[1] == 0) {
#if kalDebug > 2
            if(segs[0] == 0) {
                if(segs[1] == 0) {
                    cout<<"Skip track: inner and outer segments are empty."<<endl;
                } else {
                    cout<<"Skip track: inner segment is empty."<<endl;
                }
            } else {
                cout<<"Skip track: outer segment is empty."<<endl;
            }
#endif
            continue;
        }

        Int_t pid = (fGetPid) ? (*fGetPid)(cand) : getIniPid(cand);

        if(!doPid(pid)) {
#if kalDebug > 2
            cout<<"Skip track with PID "<<pid<<endl;
#endif
            continue;
        }

	if(bSim) {
	    input->getGeantMdcRawpoints(allhitsGeantMdc, kine, !bWire);
	}

#if kalDebug > 3
        cout<<endl;
	cout<<"### Hits in track candidate ####"<<endl;
	for(Int_t iEntry = 0; iEntry < allhits.GetEntries(); iEntry++) {
	    HKalMdcHit *hit = dynamic_cast<HKalMdcHit*>(allhits.At(iEntry));
	    if(hit) {
		cout<<"## Hit number: "<<iEntry<<" ##"<<endl;
                hit->print("Hit");
	    }
	}
	if(bSim) {
	    cout<<"### Geant hits belonging to track candidate ####"<<endl;
	    for(Int_t iEntry = 0; iEntry < allhitsGeantMdc.GetEntries(); iEntry++) {
		HGeantMdc *hit = dynamic_cast<HGeantMdc*>(allhitsGeantMdc.At(iEntry));
		if(hit) {
		    cout<<"## Geant hit number: "<<iEntry<<" ##"<<endl;
		    cout<<"x = "<<hit->getX()<<", y = "<<hit->getY()<<endl;
                    TVector3 iniPosGeantSec, iniDirGeantSec;
		    input->getPosAndDir(iniPosGeantSec, iniDirGeantSec,
					(HGeantMdc*)allhitsGeantMdc.At(iEntry),
					kTRUE);
		    cout<<"Position in sector coordinates: "<<endl;
		    cout<<"x = "<<iniPosGeantSec.X()
			<<", y = "<<iniPosGeantSec.Y()
                        <<", z = "<<iniPosGeantSec.Z()<<endl<<endl;
		}
	    }
	}
#endif

        // Create a starting state vector for the Kalman Filter.
        TVectorD inisv(stateDim);
        // Initialize covariance.
        TMatrixD iniC(stateDim, stateDim);

	switch (iniSvMethod) {
	case 1:
	    if(!getIniSvGeant(inisv, iniC, allhitsGeantMdc, pid, kFALSE)) {
		Error("execute()", "Could not calculate initial state vector.");
		continue;
	    }
	    break;
	case 2:
	    if(!getIniSvGeant(inisv, iniC, allhitsGeantMdc, pid, kTRUE)) {
		Error("execute()", "Could not calculate initial state vector.");
		continue;
	    }
	    //inisv[kIdxQP] = HPhysicsConstants::charge(pid) / TMath::Max(((HSplineTrack*)fCatSplineTrack->getObject(pMetaMatch->getSplineInd()))->getP(), 100.F);
	    break;
	case 3:
	    if(!getIniSvRungeKutta(inisv, iniC, pid, allhits, pMetaMatch)) {
		Error("execute()", "Could not calculate initial state vector.");
		continue;
	    }
	    break;
	default:
	    if(!getIniSv(inisv, iniC, pid, allhits, pMetaMatch, segs[0], segs[1])) {
		Error("execute()", "Could not calculate initial state vector.");
		continue;
	    }
	    //TVectorD inisvMomSmear(stateDim);
	    //TMatrixD iniCMomSmear(stateDim, stateDim);
	    //getIniSvGeant(inisvMomSmear, iniCMomSmear, allhitsGeantMdc, pid, kTRUE);
            //inisv[kIdxQP] = inisvMomSmear[kIdxQP];// * 1./(0.994);
	    break;
	}
#if kalDebug > 1
        cout<<"Running Kalman filter on track number "<<numTracksEvent<<" of event "<<evtNbr<<endl;
#endif

        // Run Kalman filter.
        Bool_t bAccepted = kalsys->fitTrack(allhits, inisv, iniC, pid);

        // Propagate track to vertex.
        kalsys->traceToVertex();

        // META matching.
        if(!metaMatcher.matchWithMeta(pMetaMatch, kalsys)) {
            //?
        }

	if(!bAccepted) {
            numTracksErr++;
            numTracksErrEvent++;
            if(bPrintWarn) {
                Warning("Event loop", Form("Errors in track number %i of event %i", numTracksEvent, evtNbr));
            }
        }

        fillData(pMetaMatch, allhitsGeantMdc, segs[0], segs[1]);

        numTracks++;
	numTracksEvent++;
    } // while loop over candidates

    return 0;
}

Bool_t HKalFilterTask::init(void) {
    if(!gHades) {
        Error("init()", "HADES not found.");
        return kFALSE;
    }

    HRuntimeDb* rtdb = gHades->getRuntimeDb();

    if(!rtdb) {
        Error("init()", "Runtime database not found.");
        return kFALSE;
    }

    rtdb->getContainer("MdcTrackGFieldPar");
    rtdb->getContainer("MagnetPar");

    rtdb->getContainer("MdcRawStruct");
    rtdb->getContainer("MdcGeomStruct");
    rtdb->getContainer("MdcLookupRaw");
    rtdb->getContainer("MdcLookupGeom");
    rtdb->getContainer("MdcLayerCorrPar");
    rtdb->getContainer("MdcLayerGeomPar");
    rtdb->getContainer("MdcGeomPar");
    rtdb->getContainer("SpecGeomPar");

    rtdb->getContainer("RichGeometryParameters");
    rtdb->getContainer("RpcGeomPar");
    rtdb->getContainer("ShowerGeometry");
    rtdb->getContainer("TofGeomPar");

    rtdb->getContainer("MdcDigitPar");
    rtdb->getContainer("MdcCal2Par");
    rtdb->getContainer("MdcCal2ParSim");

    fCatKalTrack    = HCategoryManager::addCategory(catKalTrack,   "HKalTrack",   5000, "Kalman filter");
    fCatKalSite     = HCategoryManager::addCategory(catKalSite,    "HKalSite" ,   5000, "Measurement site");
    fCatKalHitWire  = HCategoryManager::addCategory(catKalHitWire, "HKalHitWire", 5000, "Wire Hit");
    fCatKalHit2d    = HCategoryManager::addCategory(catKalHit2d,   "HKalHit2d",   5000, "Segment Hit");

    fCatMetaMatch   = HCategoryManager::getCategory(catMetaMatch);
    fCatSplineTrack = HCategoryManager::getCategory(catSplineTrack);
    fCatTofHit  = HCategoryManager::getCategory(catTofHit);
    fCatRpcClst = HCategoryManager::getCategory(catRpcCluster);


    metaMatcher.init();

    return kTRUE;
    //return createKalSystem();
}

HMdcTrkCand* HKalFilterTask::nextTrack(TObjArray& allhits) {
    // Fill the array allhits with the next track candidate.

    if(bWire) {
	return input->nextWireTrack(allhits);
    }
    return input->nextMdcTrackCand(allhits);
}

Bool_t HKalFilterTask::reinit(void) {
    HMdcSizesCells* fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    fSizesCells->initContainer();

    if(!fSizesCells) {
        fSizesCells = HMdcSizesCells::getObject();

        fSizesCells->initContainer();
    }

    Int_t nLayersInMdc = 1;
    if(bWire) {
        nLayersInMdc = 6;
    }

    Bool_t bInit = kTRUE;
    if(!input) {
	HKalDetCradle* pCradleHades = new HKalDetCradle(nLayersInMdc, materials);
	input = new HKalInput(pCradleHades);
        bInit = input->init(refId);
        input ->setPrint(bPrintWarn);

        HMagnetPar         *pMagnet = input->getMagnetPar();
        HMdcTrackGFieldPar *pField  = input->getGFieldPar();
        // Set B-Field for track.
        Float_t fieldFactor = pMagnet->getScalingFactor();
        if(pMagnet->getPolarity() < 0) {
            fieldFactor = - fieldFactor;
        }
        bInit &= pMagnet && pField;

        if(kalsys) {
            kalsys->setFieldMap(pField->getPointer(), fieldFactor);
        } else {
            bInit &= createKalSystem();
	}
    }

    metaMatcher.reinit();

    return (bInit);
}

Bool_t HKalFilterTask::finalize(void) {
    delete kalsys;
    delete input;

    Info("finalize", Form("%i tracks reconstructed, %i tracks with errors.",
                          numTracks, numTracksErr));
    return kTRUE;
}


void HKalFilterTask::setErrors(Double_t dx, Double_t dy, Double_t dtx, Double_t dty, Double_t dmom) {
    // Error estimates used to fill the initial covariance matrix.
    // dx, dy:   Position errors in sector coordinates and mm.
    // dtx, dty: tx = tan(px/pz), ty = tan(py/pz) in radians.
    // dmom:     Momentum uncertainty as a fraction. For example 0.1 means initial estimate is
    //           within 10% of the true momentum.

    errX = dx;
    errY = dy;
    errTx = dtx;
    errTy = dty;
    errMom = dmom;
}

void HKalFilterTask::setDoPid(Int_t pid[], Int_t n) {
    // Only reconstruct particles with certain pid.
    // pid: Array containing the Geant particle ids.
    // n:   Size of array pid. Pass n=0 to reconstruct all particles.

    dopid.Set(n);
    for(Int_t i = 0; i < n; i++) {
        dopid.SetAt(pid[i], i);
    }
}

Bool_t HKalFilterTask::setFilterPars(Bool_t wire, Bool_t daf, Bool_t comp) {
    // Modifies the type of the Kalman filter that will be applied:
    // wire:  Run Kalman filter Mdc drift chamber hits or on segment hits.
    // daf:   Apply the Deterministic Annealing Filter (DAF).
    // comp:  Consider competing hits for the DAF.
    //
    // It is recommended to specify the desired values for these parameters
    // already in the contructor.
    // Returns kFALSE if the parameter combination is not valid.
    // An existing Kalman filter object will be deleted along with its
    // settings. 

    bWire     = wire;
    bDaf      = daf;
    bCompHits = comp;
    if(bWire) {
        measDim = 1;
    } else {
        measDim = 2;
    }
    stateDim = 5;
    return createKalSystem();
}

void HKalFilterTask::setVerbose(Int_t v) {
    // Set verbose level:
    // 1: print warnings and error messages
    // 2: print error messages only
    // all else: print nothing.

    (v >= 1 ? bPrintErr  = kTRUE : bPrintErr  = kFALSE);
    (v >= 2 ? bPrintWarn = kTRUE : bPrintWarn = kFALSE);
    if(input) {
        input ->setPrint(bPrintWarn);
    }
    if(kalsys) {
        kalsys->setPrintWarnings(bPrintWarn);
        kalsys->setPrintErrors  (bPrintErr);
    }
}

