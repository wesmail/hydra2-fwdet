#include "hkalmetamatcher.h"

#include "hades.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hgeomvector.h"
#include "hmetamatchpar.h"
#include "hmdcsizescells.h"
#include "hphysicsconstants.h"
#include "hrpccluster.h"
#include "hrpcgeompar.h"
#include "hruntimedb.h"
#include "hshowerhit.h"
#include "hemccluster.h"
#include "hshowergeometry.h"
#include "hemcgeompar.h"
#include "hspectrometer.h"
#include "htofgeompar.h"
#include "htofhit.h"

#include "rpcdef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "tofdef.h"

#include "hkalmetamatch.h"

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
//

ClassImp (HKalMetaMatcher)

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalMetaMatcher::HKalMetaMatcher() {
    // Be sure to call the init() function after HADES has
    // been initialized.

    bIsMatched      = kFALSE;
    clearMatches();

    fCatRpcCluster  = NULL;
    fCatShower      = NULL;
    fCatEmcCluster  = NULL;
    fCatTofCluster  = NULL;
    fCatTof         = NULL;

    fMatchPar       = NULL;
    fRpcGeometry    = NULL;
    fShowerGeometry = NULL;
    fEmcGeometry    = NULL;
    fTofGeometry    = NULL;

    pKalsys         = NULL;
}

//  -----------------------------------
//  Protected Methods
//  -----------------------------------

void HKalMetaMatcher::getMetaRecoPos(Float_t &xReco, Float_t &yReco, Float_t &zReco) const {
    // Return the META hit coordinates expected by the Kalman filter in sector coordinates.

    if(!pKalsys) {
        Error("getRecoMetaPos()", "Pointer to track propagator not found. This is disaster.");
        return;
    }
    Double_t x,y,z = -1.; // Type compatibility.
    pKalsys->getMetaPos(x, y, z);
    xReco = x;
    yReco = y;
    zReco = z;
}

Float_t HKalMetaMatcher::getP() const {
    // Returns the reconstructed momentum.

    if(!pKalsys) {
        Error("getP()", "Pointer to track propagator not found. This is disaster.");
        return -1.F;
    }

    Float_t qp = pKalsys->getSite(0)->getStateParam(Kalman::kSmoothed, Kalman::kIdxQP);
    if(qp == 0.F) return 0.F;
    Float_t p  = 1.F / (qp * HPhysicsConstants::charge(pKalsys->getPid()));
    return p;
}

Float_t HKalMetaMatcher::getTrackLength() const {
    // Returns track length from the Kalman filter.

    if(!pKalsys) {
        Error("getTrackLength()", "Pointer to track propagator not found. This is disaster.");
        return -1.F;
    }

    return pKalsys->getTrackLength();
}

Bool_t HKalMetaMatcher::traceToMeta(const TVector3 &metaHit, const TVector3 &metaNorm) {
    if(!pKalsys) {
        Error("traceToMeta()", "Pointer to track propagator not found. This is disaster.");
        return kFALSE;
    }
#if metaDebug > 0
    if(metaNorm.Mag() == 0.) {
        Error("traceToMeta()", "Normal vector of meta detector has magnitude zero.");
        return kFALSE;
    }
#endif
    return pKalsys->traceToMeta(metaHit, metaNorm);
}

//  -----------------------------------
//  Public Methods
//  -----------------------------------

void HKalMetaMatcher::clearMatches() {
    for(Int_t tab = 0; tab < nMetaTabs; tab++) {
        matchesRpc   [tab].clear();
        matchesShower[tab].clear();
        matchesEmc   [tab].clear();
        matchesRpc   [tab].setSystem(0);
        matchesShower[tab].setSystem(1);
        matchesEmc   [tab].setSystem(3);
        for(Int_t clstr = 0; clstr < tofClusterSize; clstr++) {
            matchesTof[tab][clstr].clear();
            matchesTof[tab][clstr].setSystem(2);
        }
    }
}

void HKalMetaMatcher::init() {
    // Get categories and coordinate transformations.
    // Call this after HADES has been initialized.

    if(!gHades) {
        Error("init()", "HADES not initialized.");
        return;
    }

    HRuntimeDb *rtdb=gHades->getRuntimeDb();

    if(!rtdb) {
        Error("init()", "HADES runtime database not found.");
        return;
    }

    fMatchPar       = (HMetaMatchPar*)rtdb->getContainer("MetaMatchPar");
    
    fCatRpcCluster  = HCategoryManager::getCategory(catRpcCluster);
    fCatShower      = HCategoryManager::getCategory(catShowerHit);
    fCatEmcCluster  = HCategoryManager::getCategory(catEmcCluster);
    fCatTofCluster  = HCategoryManager::getCategory(catTofCluster);
    fCatTof         = HCategoryManager::getCategory(catTof);

    if(!fCatShower && !fCatEmcCluster) Warning("init()", "Category catShower and catEmcCluster not found.");
    if(!fCatRpcCluster) Warning("init()", "Category catRpcCluster not found.");
    if(!fCatTofCluster) Warning("init()", "Category catTofCluster not found.");
    if(!fCatTof)        Warning("init()", "Category catTof not found.");

    HSpectrometer *spec = gHades->getSetup();

    if(spec->getDetector("Rpc")) {
        fRpcGeometry   = (HRpcGeomPar*)rtdb->getContainer("RpcGeomPar");
    }

    // Shower-geometry-for-metaEnergy loss calculation
    if(spec->getDetector("Shower")) {
        fShowerGeometry = (HShowerGeometry *)rtdb->getContainer("ShowerGeometry");
    } else if (spec->getDetector("Emc")) {
    // Emc-geometry
      fEmcGeometry = (HEmcGeomPar *)rtdb->getContainer("EmcGeomPar");
    }
     
    // TOF-geometry-for-metaEnergy loss calculation
    if(spec->getDetector("Tof")) { // has the user set up a TOF?
        fTofGeometry = (HTofGeomPar *)rtdb->getContainer("TofGeomPar");
    }

}

void HKalMetaMatcher::reinit() {

    HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();  // check if is already there
    if(!fSizesCells) {
	fSizesCells = HMdcSizesCells::getObject();
	fSizesCells->initContainer();
    }

    // Geometry transformation from lab to sector coordinate system.
    for(Int_t i = 0; i < 6; i++) {
        HMdcSizesCellsSec& fSCSec = (*fSizesCells)[i];
        if(&fSCSec == 0) continue; // sector is not active
        labSecTrans[i] = *(fSCSec.getLabTrans());
    }

    for(Int_t iSec = 0; iSec < 6; iSec++) {
        if(fRpcGeometry) {
            HModGeomPar *module = fRpcGeometry->getModule(iSec,0);
            if(module) {
                // Transformation moudule->lab.
                HGeomTransform modTrans(module->getLabTransform());
                // Combine with transformation lab->sector
                // to get the transformation from module to sector.
                modTrans.transTo(labSecTrans[iSec]);

                modSecTransRpc[iSec] = modTrans;

                // Centre point.
                HGeomVector r0_mod(0.0, 0.0, 0.0);
                // Normal vector.
                HGeomVector rz_mod(0.0, 0.0, 1.0);
                HGeomVector nRpc = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
                normVecRpc[iSec].SetXYZ(nRpc.getX(), nRpc.getY(), nRpc.getZ());
            }
        }

        if (fShowerGeometry) {
            HGeomTransform modTrans(fShowerGeometry->getTransform(iSec));
            modTrans.transTo(labSecTrans[iSec]);

            modSecTransShower[iSec] = modTrans;

            HGeomVector r0_mod(0.0, 0.0, 0.);
            HGeomVector rz_mod(0.0, 0.0, 1.);
            HGeomVector nShower = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
            normVecShower[iSec].SetXYZ(nShower.getX(), nShower.getY(), nShower.getZ());
        }

        if (fEmcGeometry) {
          HModGeomPar *pmodgeom = fEmcGeometry->getModule(iSec);
          HGeomTransform modTrans(pmodgeom->getLabTransform());

          modTrans.transTo(labSecTrans[iSec]);

          modSecTransEmc[iSec] = modTrans;

          HGeomVector r0_mod(0.0, 0.0, 0.);
          HGeomVector rz_mod(0.0, 0.0, 1.);
          HGeomVector nEmc = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
          normVecEmc[iSec].SetXYZ(nEmc.getX(), nEmc.getY(), nEmc.getZ()) ;
        }
    
        if (fTofGeometry) {
            for(Int_t iTofMod = 0; iTofMod < 8; iTofMod++) {
                HModGeomPar *module = fTofGeometry->getModule(iSec,iTofMod);
                if(module) {
                    HGeomTransform modTrans(module->getLabTransform());
                    modTrans.transTo(labSecTrans[iSec]);

                    modSecTransTof[iSec][iTofMod] = modTrans;

                    HGeomVector r0_mod(0.0, 0.0, 0.0);
                    HGeomVector rz_mod(0.0, 0.0, 1.0);
                    HGeomVector nTof = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
                    normVecTof[iSec][iTofMod].SetXYZ(nTof.getX(), nTof.getY(), nTof.getZ());
                }
            }
        }
    }
}

Bool_t HKalMetaMatcher::matchWithMeta(const HMetaMatch2 *pMetaMatch, HKalIFilt* const kalsys) {

#if metaDebug > 1
    cout<<"***** Matching with Meta. *****"<<endl;
    cout<<"System "<<pMetaMatch->getSystem()<<" with "
        <<(Int_t)pMetaMatch->getNRpcClusters()<<" Rpc clusters, "
        <<(Int_t)pMetaMatch->getNShrHits()<<" Shower hits or "
        <<(Int_t)pMetaMatch->getNEmcClusters()<<" Emc Clusters and "
        <<(Int_t)pMetaMatch->getNTofHits()<<" Tof hits"<<endl;
#endif
    clearMatches();
    pKalsys = kalsys;
    bIsMatched = kFALSE;
    if(pMetaMatch->getSystem() == -1) return kFALSE;

    if(pMetaMatch->getNRpcClusters()) {
        bIsMatched = kTRUE;
        matchWithRpc(pMetaMatch);
    }

    if(fCatShower != NULL) {
      if(pMetaMatch->getNShrHits() > 0) {
          bIsMatched = kTRUE;
          matchWithShower(pMetaMatch);
      }
    } else {
      if(pMetaMatch->getNEmcClusters() > 0) {
          bIsMatched = kTRUE;
          matchWithEmc(pMetaMatch);
      }
    }

    if(pMetaMatch->getNTofHits()) {
        bIsMatched = kTRUE;
        matchWithTof(pMetaMatch);
    }

#if metaDebug > 1
    cout<<"***** Done matching with Meta. *****"<<endl;
#endif
    return bIsMatched;
}

void HKalMetaMatcher::matchWithRpc(HMetaMatch2 const* const pMetaMatch) {

    for(Int_t n = 0; n < (Int_t)pMetaMatch->getNRpcClusters(); ++n) {
        Int_t ind = (Int_t)pMetaMatch->getRpcClstInd(n);
        if(ind < 0) {
            Warning("matchWithRpc()","RPC cluster index is < 0, DISASTER!"); // Anar
            continue;
        }

        if(!fCatRpcCluster) continue;

        HRpcCluster *pRpc = (HRpcCluster*)fCatRpcCluster->getObject(ind);
        if(!pRpc) {
            Warning("matchWithRpc()","Pointer to Rpc is NULL, DISASTER!");
            continue;
        }

        Int_t s = (Int_t)pMetaMatch->getSector();
        if(s < 0 || s > 5) {
#if metaDebug > 0
            Warning("matchWithRpc()", Form("Sector in HRpcHit is %i.", s));
#endif
            continue;
        }

        matchesRpc[n].setIndex(ind);
        matchesRpc[n].setTof(pRpc->getTof());
        matchesRpc[n].setMetaEloss(pRpc->getCharge());

        matchesRpc[n].setMetaHit(pRpc->getXSec(), pRpc->getYSec(), pRpc->getZSec());

#if metaDebug > 1
        cout<<"---- Matching with Rpc cluster "<<n<<" ----"<<endl;
#endif

        traceToMeta(matchesRpc[n].getMetaHit(), normVecRpc[s]);

        // Matching quality.
        // -----------------
        // Propagate track to META.
        Float_t xReco, yReco, zReco;
        getMetaRecoPos(xReco, yReco, zReco);
        matchesRpc[n].setMetaReco(xReco, yReco, zReco);
        // Transform to module coordinates to determine matching quality.
        HGeomVector localMetaReco(xReco, yReco, zReco);
        localMetaReco = modSecTransRpc[s].transTo(localMetaReco);
        matchesRpc[n].setMetaHitLocal(pRpc->getXMod(), pRpc->getYMod(), pRpc->getZMod());
        matchesRpc[n].setMetaRecoLocal(localMetaReco.getX(), localMetaReco.getY(), localMetaReco.getZ());

#if metaDebug > 2
        cout<<"Meta hit in sector coords: \n"
            <<"  x = "<<matchesRpc[n].getMetaHit().X()
            <<", y = "<<matchesRpc[n].getMetaHit().Y()
            <<", z = "<<matchesRpc[n].getMetaHit().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<pRpc->getXMod()<<", y = "<<pRpc->getYMod()
            <<endl;
        cout<<"Reconstructed meta hit in sector coords: \n"
            <<"  x = "<<xReco<<", y = "<<yReco<<", z = "<< zReco
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaReco.getX()<<", y = "<<localMetaReco.getY()
            <<"\n"<<endl;
#endif

        //? why subtract SigmaOffset?
        Float_t dX     = localMetaReco.getX() - pRpc->getXMod() - fMatchPar->getRpcSigmaXOffset(s);
        Float_t dY     = localMetaReco.getY() - pRpc->getYMod() - fMatchPar->getRpcSigmaYOffset(s);
        // HRpcHit->getXRMS() returns sigma of X.
        //? why add two sigmas
        Float_t dXsig2 =  TMath::Power(pRpc->getXRMS(), 2.F) + TMath::Power(fMatchPar->getRpcSigmaXMdc(s), 2.F);
        Float_t dYsig2 =  TMath::Power(pRpc->getYRMS(), 2.F) + TMath::Power(fMatchPar->getRpcSigmaYMdc(s), 2.F);
        matchesRpc[n].setQuality(calcQuality(dX, dY, dXsig2, dYsig2));
        // -----------------

        // beta and mass^2.
        Float_t length = getTrackLength();
        Float_t time   = matchesRpc[n].getTof();
        Float_t beta   = 1.0e6 * (length / time) / TMath::C();
        matchesRpc[n].setBeta(beta);

        Float_t beta2  = TMath::Power(beta, 2.F);
        Float_t p      = getP();
        Float_t mass2  = p * p * (1.F / beta2 - 1.F);
        matchesRpc[n].setMass2(mass2);
    }
}

void HKalMetaMatcher::matchWithShower(HMetaMatch2 const* const pMetaMatch) {

    for(Int_t n = 0; n < (Int_t)pMetaMatch->getNShrHits(); ++n) {
        Int_t ind = (Int_t)pMetaMatch->getShowerHitInd(n);

        if(ind < 0) {
            Warning("matchWithShower()","Index of shower is < 0, DISASTER!");
            continue;
        }

        if(!fCatShower) continue;

        HShowerHit *pShowerHit = (HShowerHit*)fCatShower->getObject(ind);

        if(!pShowerHit) {
            Warning("matchWithShower()","Pointer to Shower is NULL, DISASTER!");
            continue;
        }

        Int_t s = (Int_t)pMetaMatch->getSector();
        if(s < 0 || s > 5) {
#if metaDebug > 0
            Warning("matchWithShower()", Form("Sector in HShowerHit is %i.", s));
#endif
            continue;
        }

#if metaDebug > 1
        cout<<"---- Matching with Shower hit "<<n<<" ----"<<endl;
#endif

        matchesShower[n].setIndex(ind);

        // Store shower hit in sector coordinates.
        Float_t xTof, yTof, zTof;
        pShowerHit->getLabXYZ(&xTof, &yTof, &zTof);
        HGeomVector metaHit(xTof, yTof, zTof);

        metaHit = labSecTrans[s].transTo(metaHit);
        matchesShower[n].setMetaHit(metaHit.getX(), metaHit.getY(), metaHit.getZ());

        // Matching quality.
        // -----------------

        // Propagate track to META.
        traceToMeta(matchesShower[n].getMetaHit(), normVecShower[s]);

        Float_t xReco, yReco, zReco;
        getMetaRecoPos(xReco, yReco, zReco);
        matchesShower[n].setMetaReco(xReco, yReco, zReco);

        // Transform to module coordinates to determine matching quality.
        HGeomVector localMetaReco(xReco, yReco, zReco);
        localMetaReco = modSecTransShower[s].transTo(localMetaReco);
        matchesShower[n].setMetaRecoLocal(localMetaReco.getX(), localMetaReco.getY(), localMetaReco.getZ());

        Float_t xHit, yHit, zHit;
        matchesShower[n].getMetaHit(xHit, yHit, zHit);
        HGeomVector localMetaHit(xHit, yHit, zHit);
        localMetaHit  = modSecTransShower[s].transTo(localMetaHit);
        //?
        localMetaHit -= HGeomVector(0., 0., pShowerHit->getZ());
        matchesShower[n].setMetaHitLocal(localMetaHit.getX(), localMetaHit.getY(), localMetaHit.getZ());

#if metaDebug > 2
        cout<<"Meta hit in sector coords: \n"
            <<"  x = "<<matchesShower[n].getMetaHit().X()
            <<", y = "<<matchesShower[n].getMetaHit().Y()
            <<", z = "<<matchesShower[n].getMetaHit().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaHit.getX()<<", y = "<<localMetaHit.getY()
            <<endl;
        cout<<"Reconstructed meta hit in sector coords: \n"
            <<"  x = "<<matchesShower[n].getMetaReco().X()
            <<", y = "<<matchesShower[n].getMetaReco().Y()
            <<", z = "<<matchesShower[n].getMetaReco().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaReco.getX()<<", y = "<<localMetaReco.getY()
            <<"\n"<<endl;
#endif

        //? why subtract SigmaOffset?
        Float_t dX     = localMetaReco.getX() - localMetaHit.getX() - fMatchPar->getShowerSigmaXOffset(s);
        Float_t dY     = localMetaReco.getY() - localMetaHit.getY() - fMatchPar->getShowerSigmaYOffset(s);
        Float_t dXsig2 = TMath::Power(pShowerHit->getSigmaX(), 2.F);
        Float_t dYsig2 = TMath::Power(pShowerHit->getSigmaY(), 2.F);
        matchesShower[n].setQuality(calcQuality(dX, dY, dXsig2, dYsig2));
        // -----------------
    }
}

void HKalMetaMatcher::matchWithEmc(HMetaMatch2 const* const pMetaMatch) {

    if(!fCatEmcCluster) return;
    
    for(Int_t n = 0; n < (Int_t)pMetaMatch->getNEmcClusters(); ++n) {
        Int_t ind = (Int_t)pMetaMatch->getEmcClusterInd(n);

        if(ind < 0) {
            Warning("matchWithEmc()","Index of emc is < 0, DISASTER!");
            continue;
        }

        HEmcCluster *pEmcCluster = (HEmcCluster*)fCatEmcCluster->getObject(ind);

        if(!pEmcCluster) {
            Warning("matchWithEmc()","Pointer to EmcCluster is NULL, DISASTER!");
            continue;
        }

        Int_t s = (Int_t)pMetaMatch->getSector();
        if(s < 0 || s > 5) {
#if metaDebug > 0
            Warning("matchWithEmc()", Form("Sector in HEmcCluster is %i.", s));
#endif
            continue;
        }

#if metaDebug > 1
        cout<<"---- Matching with Emc hit "<<n<<" ----"<<endl;
#endif

        matchesEmc[n].setIndex(ind);
        matchesEmc[n].setTof(pEmcCluster->getTime());
        matchesEmc[n].setMetaEloss(pEmcCluster->getEnergy());

        // Store emc hit in sector coordinates.
        HGeomVector metaHit(pEmcCluster->getXLab(), pEmcCluster->getYLab(), pEmcCluster->getZLab());

        metaHit = labSecTrans[s].transTo(metaHit);
        matchesEmc[n].setMetaHit(metaHit.getX(), metaHit.getY(), metaHit.getZ());

        // Matching quality.
        // -----------------

        // Propagate track to META.
        traceToMeta(matchesEmc[n].getMetaHit(), normVecEmc[s]);

        Float_t xReco, yReco, zReco;
        getMetaRecoPos(xReco, yReco, zReco);
        matchesEmc[n].setMetaReco(xReco, yReco, zReco);

        // Transform to module coordinates to determine matching quality.
        HGeomVector localMetaReco(xReco, yReco, zReco);
        localMetaReco = modSecTransEmc[s].transTo(localMetaReco);
        matchesEmc[n].setMetaRecoLocal(localMetaReco.getX(), localMetaReco.getY(), localMetaReco.getZ());

        Float_t xHit, yHit, zHit;
        matchesEmc[n].getMetaHit(xHit, yHit, zHit);
        HGeomVector localMetaHit(xHit, yHit, zHit);
        localMetaHit  = modSecTransEmc[s].transTo(localMetaHit);
        //?
        //localMetaHit -= HGeomVector(0., 0., pEmcCluster->getZ());
        matchesEmc[n].setMetaHitLocal(localMetaHit.getX(), localMetaHit.getY(), localMetaHit.getZ());

#if metaDebug > 2
        cout<<"Meta hit in sector coords: \n"
            <<"  x = "<<matchesEmc[n].getMetaHit().X()
            <<", y = "<<matchesEmc[n].getMetaHit().Y()
            <<", z = "<<matchesEmc[n].getMetaHit().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaHit.getX()<<", y = "<<localMetaHit.getY()
            <<endl;
        cout<<"Reconstructed meta hit in sector coords: \n"
            <<"  x = "<<matchesEmc[n].getMetaReco().X()
            <<", y = "<<matchesEmc[n].getMetaReco().Y()
            <<", z = "<<matchesEmc[n].getMetaReco().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaReco.getX()<<", y = "<<localMetaReco.getY()
            <<"\n"<<endl;
#endif

        //? why subtract SigmaOffset?
        Float_t dX     = localMetaReco.getX() - localMetaHit.getX() - fMatchPar->getEmcSigmaXOffset(s);
        Float_t dY     = localMetaReco.getY() - localMetaHit.getY() - fMatchPar->getEmcSigmaYOffset(s);
        Float_t dXsig2 = TMath::Power(pEmcCluster->getSigmaXMod(), 2.F);
        Float_t dYsig2 = TMath::Power(pEmcCluster->getSigmaYMod(), 2.F);
        matchesEmc[n].setQuality(calcQuality(dX, dY, dXsig2, dYsig2));
        // -----------------
    }
}

void HKalMetaMatcher::matchWithTof(HMetaMatch2 const* const pMetaMatch) {
    for(Int_t n = 0; n < (Int_t)pMetaMatch->getNTofHits(); ++n) {
        // A TOF cluster may consist of up to two hits from different
        // scintillator rods. Match the individual hits and the cluster
        // separately.
        for(Int_t i = 0; i < tofClusterSize; ++i ) {

            Int_t ind = -1;
            if(i == 0) ind = (Int_t)pMetaMatch->getTofHit1Ind(n);
            if(i == 1) ind = (Int_t)pMetaMatch->getTofHit2Ind(n);
            if(i == 2) ind = (Int_t)pMetaMatch->getTofClstInd(n);

            if(ind < 0) continue;

            HTofHit *pTofHit = NULL;

            if(i == 2) {
                if(!fCatTofCluster) continue;
                pTofHit = (HTofHit*)fCatTofCluster->getObject(ind);
            } else {
                if(!fCatTof) continue;
                pTofHit = (HTofHit*)fCatTof->getObject(ind);
            }

            if(!pTofHit) {
                Warning("matchWithTof()", "Pointer to Tof is NULL, DISASTER!");
                continue;
            }

            Int_t s      = (Int_t)pTofHit->getSector();
            if(s < 0 || s > 5) {
#if metaDebug > 0
                Warning("matchWithTof()", Form("Sector in HTofHit is %i.", s));
#endif
                continue;
            }

#if metaDebug > 1
            cout<<"---- Matching with Tof cluster "<<n<<", hit "<<i<<"----"<<endl;
#endif

            matchesTof[n][i].setIndex(ind);
            matchesTof[n][i].setTof(pTofHit->getTof());
            matchesTof[n][i].setMetaEloss(pTofHit->getEdep());

            Float_t xTof, yTof, zTof;
            pTofHit->getXYZLab(xTof, yTof, zTof);
            HGeomVector hitMeta(xTof, yTof, zTof);
            hitMeta = labSecTrans[s].transTo(hitMeta);
            matchesTof[n][i].setMetaHit(hitMeta.getX(), hitMeta.getY(), hitMeta.getZ());

            // Propagate track to META.
            Int_t tofMod = (Int_t)pTofHit->getModule();
            traceToMeta(matchesTof[n][i].getMetaHit(), normVecTof[s][tofMod]);

            Float_t xReco, yReco, zReco;
            getMetaRecoPos(xReco, yReco, zReco);
            matchesTof[n][i].setMetaReco(xReco, yReco, zReco);
            // Transform to module coordinates to determine matching quality.
            HGeomVector localMetaReco(xReco, yReco, zReco);
            localMetaReco = modSecTransTof[s][tofMod].transTo(localMetaReco);
            matchesTof[n][i].setMetaRecoLocal(localMetaReco.getX(), localMetaReco.getY(), localMetaReco.getZ());

            Float_t xHit, yHit, zHit;
            matchesTof[n][i].getMetaHit(xHit, yHit, zHit);
            HGeomVector localMetaHit(xHit, yHit, zHit);
            localMetaHit  = modSecTransTof[s][tofMod].transTo(localMetaHit);
            matchesTof[n][i].setMetaHitLocal(localMetaHit.getX(), localMetaHit.getY(), localMetaHit.getZ());

#if metaDebug > 2
        cout<<"Meta hit in sector coords: \n"
            <<"  x = "<<matchesTof[n][i].getMetaHit().X()
            <<", y = "<<matchesTof[n][i].getMetaHit().Y()
            <<", z = "<<matchesTof[n][i].getMetaHit().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaHit.getX()<<", y = "<<localMetaHit.getY()
            <<endl;
        cout<<"Reconstructed meta hit in sector coords: \n"
            <<"  x = "<<matchesTof[n][i].getMetaReco().X()
            <<", y = "<<matchesTof[n][i].getMetaReco().Y()
            <<", z = "<<matchesTof[n][i].getMetaReco().Z()
            <<"\n in module coords: \n"
            <<"  x = "<<localMetaReco.getX()<<", y = "<<localMetaReco.getY()
            <<"\n"<<endl;
#endif

            // Matching quality.
            Float_t dX     = localMetaReco.getX() - localMetaHit.getX() - fMatchPar->getTofSigmaXOffset(s);
            Float_t dY     = localMetaReco.getY() - localMetaHit.getY() - fMatchPar->getTofSigmaYOffset(s);
            Float_t dXsig2 = TMath::Power(fMatchPar->getTofSigmaX(s), 2.F);
            Float_t dYsig2 = TMath::Power(fMatchPar->getTofSigmaY(s), 2.F);
            matchesTof[n][i].setQuality(calcQuality(dX, dY, dXsig2, dYsig2));

            // beta and mass^2.
            Float_t length = getTrackLength();
            Float_t time   = matchesTof[n][i].getTof();
            Float_t beta   = 1.0e6 * (length / time) / TMath::C();
            matchesTof[n][i].setBeta(beta);

            Float_t beta2  = TMath::Power(beta, 2.F);
            Float_t p      = getP();
            Float_t mass2  = p * p * (1.F / beta2 - 1.F);
            matchesTof[n][i].setMass2(mass2);
        }
    }
}

Int_t HKalMetaMatcher::getClstrSize(Int_t sys) const {

    if(sys == 0 || sys == 1 || sys == 3) return 1; // Rpc or Shower or Emc
    if(sys == 2) return tofClusterSize; // Tof
    return 0; // Unknown.
}

const HKalMetaMatch& HKalMetaMatcher::getMatch(Int_t sys, Int_t tab, Int_t clstr) const {

    if(tab > (nMetaTabs - 1) || tab < 0) {
        Error("getMatch()", Form("Invalid tab parameter: %i. Must be between 0 and %i.", tab, nMetaTabs));
        return matchesRpc[0];
    }

    switch (sys) {
    case 0:
        return matchesRpc[tab];
        break;
    case 1:
        return matchesShower[tab];
        break;
    case 2:
        if(clstr >= 0 && clstr < tofClusterSize) {
            return matchesTof[tab][clstr];
        } else {
            Error("getMatch()", Form("Invalid Tof cluster: %i", clstr));
            return matchesRpc[0];
        }
        break;
    case 3:
        return matchesEmc[tab];
        break;
    default:
        Error("getMatch()", Form("Invalid Meta detector: %i", sys));
        return matchesRpc[0];
        break;
    }
    Error("getMatch()", Form("Invalid Meta detector: %i", sys));
    return matchesRpc[0];
}
