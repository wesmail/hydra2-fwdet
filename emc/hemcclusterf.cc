using namespace std;
#include "hemcclusterf.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hemcdetector.h" 
#include "hgeantemc.h"
#include "hevent.h"
#include "hcategory.h"
#include "hemccalsim.h"
#include "hemcclustersim.h"
#include "rpcdef.h"
#include "hrpcclustersim.h"
#include "hrpchitsim.h"
#include "hemcgeompar.h"
#include "hgeomtransform.h"
#include "hgeomcompositevolume.h"
#include "hgeomvolume.h"
#include "hgeomvector.h"
#include <iostream>
#include <iomanip>

//*-- Author   : V. Pechenov
//*-- Modified : 

/////////////////////////////////////////////////////////////////////
//
//  HEmcClusterF combines neighboring time correlated cells to the cluster,
//  matches cells to RPC,
//  calculates cluster mean time (by corrected times)
//  In simulation the cluster is assigned to the track which has maximal energy deposit
//
//  Algorithm
//  "0": find cell which has maximal energy deposit (t0 is a time of this cell)
//  "1": test 8 neighboring cells and add time correlated cells to the cluster
//  "2": test cells neighboring to added ones in the previous step
//  "3": test cells neighboring to added ones in the previous step
//  ...
//
//  Time Correlation
//  Assumption: first cell fired by this particle has maximal energy deposit
//  dt0 = tc - V*dc    - t0
//  dt  = (tc - V*dc) - (tp - V*dp) ,
//  where
//   V = 0.30 ns/cell 
//   cells "c" and "p" are neighboring cells
//   dc and dp  are distances to the cell which has maximal energy deposit (cell from step "0"), units are cells
//   tc and tp  are measured times in cells "c" and "p"
//  Cells accepted for the cluster must fulfill conditions
//    dtmin < dt0 < dtmax 
//     &&
//    dtmin < dt < dtmax
// 
//  Matching to RPC
//  Matching by theta, phi and time 
//  Cluster is marked as matched with RPC if at least one cell in the steps "0" and "1" has a matching with RPC
//
/////////////////////////////////////////////////////////////////////

HEmcClusterF::HEmcClusterF(void) {
  initData();
}

HEmcClusterF::HEmcClusterF(const Text_t *name, const Text_t *title) : HReconstructor(name,title) {
  initData();
}

void HEmcClusterF::initData(void) {
  fLoc.set(2,0,0);
  fEmcCalCat      = NULL;
  fClusterCat     = NULL;
  // RPC matching parameters:
  dThetaSigOffset  = 0.1;   // (thetaEmc-thetaRpc)/sigmaDTheta+dThSigOffset
  dThetaScale      = 1.61;  // dTheta = dThetaSigOffset/dThetaScale
  dTimeOffset      = 0.03;  // [ns] (time1 - timeRpcN + dTimeOffset)/sigmaDTof;
  dTimeCut         = 3.;    // Nsigma cut for matchin in time 
  dThdPhCut        = 3.6;   // Nsigma cut for matching in angles  
  // Cluster finder parameters:
  cellToCellSpeed  = 0.3;   // [ns/cell]
  distOffset       = 0.9;
  timeCutMin       = -0.6;
  timeCutMax       = +0.6;
}

HEmcClusterF::~HEmcClusterF(void) {
}

Bool_t HEmcClusterF::init(void) {

  gHades->getRuntimeDb()->getContainer("EmcGeomPar");

  HEmcDetector* emc = (HEmcDetector*)(gHades->getSetup()->getDetector("Emc"));  
  if (emc == NULL) {
    Error("HEmcClusterF::init()","No Emc Detector");
    return kFALSE;  
  }  

  HCategory* fGeantKineCat = gHades->getCurrentEvent()->getCategory(catGeantKine);
  if (fGeantKineCat) { isSimulation = kTRUE;  }
  else               { isSimulation = kFALSE; }
  
  fEmcCalCat = gHades->getCurrentEvent()->getCategory(catEmcCal);  
  if (!fEmcCalCat) {
    Error("HEmcClusterF::init()","Cal EMC input missing");
    return kFALSE;
  }
  
  fRpcCat = gHades->getCurrentEvent()->getCategory(catRpcCluster);  
  if (!fRpcCat) {
    Warning("HEmcClusterF::init()","Cluster RPC input missing");
    return kFALSE;
  }

  //Build the Cluster category
  fClusterCat = gHades->getCurrentEvent()->getCategory(catEmcCluster);  
  if (!fClusterCat) {
    if(isSimulation) { fClusterCat = emc->buildMatrixCategory("HEmcClusterSim",0.5); }
    else             { fClusterCat = emc->buildMatrixCategory("HEmcCluster",0.5); } 
    gHades->getCurrentEvent()->addCategory(catEmcCluster,fClusterCat,"Emc");
    fClusterCat->setPersistency(kTRUE);
  }

  return kTRUE;
}

Bool_t HEmcClusterF::reinit(void) {
  sigmaXYmod  = 92./TMath::Sqrt(12.);             // 92.mm - size of cell (module)
  HEmcGeomPar *emcGeomPar  = (HEmcGeomPar*)(gHades->getRuntimeDb()->getContainer("EmcGeomPar"));
  HGeomTransform labTrans[6];
  for(Int_t s=0;s<6;s++) {
    HModGeomPar* fmodgeom = emcGeomPar->getModule(s);
    labTrans[s] = fmodgeom->getLabTransform();
    HGeomCompositeVolume* fMod    = fmodgeom->getRefVolume();
    for(Int_t c=0;c<emcMaxComponents;c++) {
      HGeomVolume* fVol=fMod->getComponent(c);
      if(fVol == NULL || fVol->getNumPoints() != 8) {
        thetaEmcLab[s][c] = 0.;
        phiEmcLab[s][c]   = 0.;
        sigmaTheta[s][c]  = 0.;
        sigmaPhi[s][c]    = 0.;
        emcCellsLab[s][c] = NULL;
        continue;
      }
      emcCellsLab[s][c] = new HGeomVector;
      HGeomVector* p    = emcCellsLab[s][c];
      *p = fVol->getTransform().getTransVector();
      cellXmod[c] = p->getX();
      cellYmod[c] = p->getY();
      *p = labTrans[s].transFrom(*p);
      
      // Calc. theta and phi of module(cell)
      Double_t xy2      =  p->getX()*p->getX() + p->getY()*p->getY();
      Double_t xyz2     = xy2 + p->getZ()*p->getZ();
      thetaEmcLab[s][c] = TMath::ATan2(TMath::Sqrt(xy2),p->getZ())*TMath::RadToDeg();
      phiEmcLab[s][c]   = TMath::ATan2(p->getY(),p->getX())*TMath::RadToDeg();
      if(phiEmcLab[s][c] < 0.) phiEmcLab[s][c] += 360.;
      
      // Calc. errors of theta and phi
      sigmaTheta[s][c]    = p->getZ()/xyz2 * sigmaXYmod * TMath::RadToDeg();
      sigmaPhi[s][c]      = 1./TMath::Sqrt(xy2) * sigmaXYmod * TMath::RadToDeg();
    }
  }
  return kTRUE;
}

Int_t HEmcClusterF::execute(void) {
  Int_t nEmcCal = fEmcCalCat->getEntries();
  for(Int_t sec=0;sec<6;sec++) {
    memset(flagUsed,-1,emcMaxComponents);
    
    // Collect HEmcCal in one sector
    for(Int_t e=0; e<nEmcCal; e++) {
      HEmcCal* cal = (HEmcCal*)fEmcCalCat->getObject(e);
      if(sec != cal->getSector()) continue;
      // Test status:
      if(cal->getStatus1() < 0) continue;              // Energy < energyCut for sim.data
      Float_t ener = cal->getEnergy();
      if(ener <= 0.) continue; // !!!???
      
      Int_t cell       = cal->getCell();
      energy[cell]     = ener;
      pSecECells[cell] = cal;
      flagUsed[cell]   = 0;
    }
    
    // Cluster finder:
    while(kTRUE) {
      Int_t   cell  = maxEnergyCell();
      if(cell < 0) break;  // all fired cells was used already
//        if(energy[cell] < 15) break;
//        if(energy[cell] < 150.) break;
      // New cluster:
      HEmcCal *cal           = pSecECells[cell];
      Float_t  xPos          = 0.;
      Float_t  yPos          = 0.;
      Float_t  errXYPos      = 0.;
      Double_t posNorm       = 0.;
      Int_t    nMatchedCells = 0;
      HGeomVector pos;
      Float_t qualityDThDPh,qualityDTime;
      HRpcCluster* pRpcClusF = rpcMatch(cal,qualityDThDPh,qualityDTime);
      if(pRpcClusF != NULL) nMatchedCells++;

      Float_t time0       = cal->getTime1();

      listClustCell[0]    = cell;
      pClustCells[0]      = cal;
      flagUsed[cell]      = 1;
      Int_t size          = 1;
      Int_t ind           = 0;
      Float_t clustEnergy = 0.;
      Float_t clustEnErr  = 0.;
      Float_t timeSum     = time0*energy[cell];
      Float_t timeError   = TMath::Power(cal->getSigmaTime1()*energy[cell],2); 

      while(ind<size) {
//        Float_t timeN = pClustCells[ind]->getTime1();
        Int_t cind     = listClustCell[ind];
        clustEnergy   += energy[cind];
        clustEnErr    += pClustCells[ind]->getSigmaEnergy()*pClustCells[ind]->getSigmaEnergy();
        Float_t distN  = ind==0 ? 0. : calcDist(cal,pClustCells[ind]);
        Float_t tCorrN = pClustCells[ind]->getTime1() - cellToCellSpeed*(distN - distOffset);
        if(distN<1.9) {
          HGeomVector *pemc = emcCellsLab[sec][cind];
          HGeomVector vc(pemc->getX(),pemc->getY(),pemc->getZ());
          vc       *= energy[cind];
          pos      += vc;
          posNorm  += energy[cind];
          xPos     += cellXmod[cind]*energy[cind];
          yPos     += cellYmod[cind]*energy[cind];
          errXYPos += energy[cind]*energy[cind];
        }
        for(Int_t i=0;i<8;i++) {
          Int_t celli = getNearbyCell(cind,i);
          if(celli < 0) continue;
          if( flagUsed[celli] != 0 ) continue;  // skip not fired cells & cells which used already
          HEmcCal *cali =  pSecECells[celli];
          if(cali == NULL) continue;

          Float_t dist0   = calcDist(cal,cali);

          Float_t tCorrI  = cali->getTime1() - cellToCellSpeed*(dist0- distOffset);
          Float_t dT0corr = tCorrI - time0;
          if(dT0corr < timeCutMin || dT0corr > timeCutMax) continue;  // 0.6
          if(ind > 0) {
           Float_t dTcorr = tCorrI - tCorrN;
//           Float_t dTcorr = cali->getTime1()-timeN - cellToCellSpeed*distOffset;
            if(dTcorr  < timeCutMin || dTcorr  > timeCutMax) continue; 
          }       
          //======== Add to cluster ========================
          Float_t qualityDThDPhI,qualityDTimeI;
          HRpcCluster* pRpcClus = rpcMatch(cali,qualityDThDPhI,qualityDTimeI);
          if(pRpcClus != NULL && pRpcClus != pRpcClusF) {
            if(pRpcClus == pRpcClusF) nMatchedCells++;
            else {
              if(dist0 < 1.9) nMatchedCells++;
              else if(nMatchedCells == 0) continue;  // ignore???
              if(pRpcClusF == NULL) {
                pRpcClusF = pRpcClus;
                qualityDThDPh = qualityDThDPhI;
                qualityDTime  = qualityDTimeI;
              }
            }
          }

          timeSum        += tCorrI*energy[celli]; 
          timeError      += TMath::Power(cali->getSigmaTime1()*energy[celli],2);           
          flagUsed[celli] = 1;

          listClustCell[size] = celli;
          pClustCells[size]   = cali;
          size++;
        }
        ind++;
      } //cluter end


      timeSum  /= clustEnergy;
      timeError = TMath::Sqrt(timeError)/clustEnergy;
      xPos     /= posNorm;
      yPos     /= posNorm; 
      pos      /= posNorm;
      errXYPos = sigmaXYmod*TMath::Sqrt(errXYPos)/posNorm;

//--------------------------------------------------------------------------------------
      // end of cluster
      fLoc[0] = sec;
      fLoc[1] = cell;
      Int_t clustIndex;
      HEmcCluster* pCluster = (HEmcCluster*)fClusterCat->getSlot(fLoc,&clustIndex);
      if(pCluster == NULL) {
        Warning("execute","S.%i No HEmcCluster slot available",sec+1);
        return 1;
      }
      
      for(Int_t s=0;s<size;s++) pClustCells[s]->setClusterIndex(clustIndex);
      pCluster = isSimulation ? (HEmcCluster*)(new(pCluster) HEmcClusterSim) : new(pCluster) HEmcCluster;
      pCluster->setSector(sec);
      pCluster->setCellList(size,listClustCell);
      pCluster->setIndex(clustIndex);
      pCluster->setMaxEnergy(energy[cell]);
      pCluster->setEnergy(clustEnergy);
      pCluster->setSigmaEnergy(TMath::Sqrt(clustEnErr));
      pCluster->setTime(timeSum);
      pCluster->setSigmaTime(timeError);
      pCluster->setXYMod(xPos,yPos);
      pCluster->setSigmaXYMod(errXYPos);
      pCluster->setXYZLab(pos.getX(),pos.getY(),pos.getZ());
      Double_t xy    =  TMath::Sqrt(pos.getX()*pos.getX() + pos.getY()*pos.getY());
      Double_t theta = TMath::ATan2(xy,pos.getZ())*TMath::RadToDeg();
      Double_t phi   = TMath::ATan2(pos.getY(),pos.getX())*TMath::RadToDeg();
      if(phi < 0.) phi += 360.;
      pCluster->setTheta(theta);
      pCluster->setPhi(phi);
      pCluster->setCellList(size,listClustCell);
      if(pRpcClusF != NULL) {
        pCluster->setRpcIndex(pRpcClusF->getIndex());    
        pCluster->setQualDThDPh(qualityDThDPh);
        pCluster->setQualDTime(qualityDTime); 
        pCluster->setNMatchedCells(nMatchedCells);
      }
      
      if(isSimulation) { // For sim output:
        HEmcClusterSim* pClusterSim = (HEmcClusterSim*)pCluster;
        map<Int_t,Float_t>clTrackEnergy;  // For simulation only
        for(Int_t ind=0;ind<size;ind++) { 
          HEmcCalSim* cal = (HEmcCalSim*)pClustCells[ind];
          Int_t ntr = cal->getNTracks();
          if(ntr < 1) continue; // Real track iEmbedded track
          for(Int_t i=0;i<ntr;i++) {
            Int_t track           = cal->getTrack(i);
            clTrackEnergy[track] += cal->getTrackEnergy(i); //ener;
          }
        }
//?        Int_t nTracksInCluster = clTrackEnergy.size();

      // Sort tracks by enegy deposit 
      vector<pair<Int_t,Float_t> > vEn(clTrackEnergy.begin(),clTrackEnergy.end());
      if(vEn.size() > 1) sort(vEn.begin(),vEn.end(),cmpEnergy);      
      for(UInt_t i = 0; i < vEn.size(); i++) pClusterSim->setTrack(vEn[i].first,vEn[i].second);
      if(pRpcClusF != NULL) pClusterSim->setRpcTrack(((HRpcClusterSim*)pRpcClusF)->getTrack());
//?  Float_t clTrMaxEnergy = vEn[0].second;
//?  Int_t   trackMaxEng   = vEn[0].first;        
 
      }
    }
  }
  return 0;
}

HRpcCluster* HEmcClusterF::rpcMatch(HEmcCal* cal,Float_t &qualityDThDPh,Float_t &qualityDTime) {

  Int_t   sec       = cal->getSector();
  Int_t   cell      = cal->getCell();
  Float_t time1     = cal->getTime1();
  Float_t thEmc     = thetaEmcLab[sec][cell];
  Float_t phEmc     = phiEmcLab[sec][cell];
  Float_t sigmaTh   = sigmaTheta[sec][cell];
  Float_t sigmaPh   = sigmaPhi[sec][cell];
  Float_t sigmaTm1  = cal->getSigmaTime1();
  HGeomVector *pemc = emcCellsLab[sec][cell];

  qualityDThDPh     = 1000.;
  qualityDTime      = 1000.;
  HRpcCluster* out  = NULL;
  
  Int_t   nRpc = fRpcCat->getEntries();
  for(Int_t n=0;n<nRpc;n++) {
    HRpcCluster* rpc = (HRpcCluster*)fRpcCat->getObject(n);
    if(rpc->getSector() != sec) continue;

    // Propagate track to the EMC and calculate TOF (timeRpc) to the EMC module 
    Float_t xrl,yrl,zrl;
    rpc->getXYZLab(xrl,yrl,zrl);
    Float_t sigTof     = rpc->getTOFRMS();
    Float_t deltaX     = pemc->getX()-xrl;
    Float_t deltaY     = pemc->getY()-yrl;
    Float_t deltaZ     = pemc->getZ()-zrl;
    Float_t distRpcEmc = TMath::Sqrt(deltaX*deltaX+deltaY*deltaY+deltaZ*deltaZ);
    Float_t timeCorr   = distRpcEmc/TMath::Sqrt(xrl*xrl+yrl*yrl+zrl*zrl) * rpc->getTof();
    Float_t timeRpc    = rpc->getTof() + timeCorr;

    // Calculate quality of matching for angles (dThdPh)
    Float_t dThSig = ((thEmc -rpc->getTheta())/sigmaTh + dThetaSigOffset)/dThetaScale;
    Float_t dPhSig = (phEmc - rpc->getPhi())/sigmaPh;
    Float_t dThdPh = TMath::Sqrt(dThSig*dThSig + dPhSig*dPhSig);  // O-shaped cut
    
    // Calculate quality of matching for time (dTOFc)
    Float_t sigmaDTof = TMath::Sqrt(sigmaTm1*sigmaTm1 + sigTof*sigTof);
    Float_t dTOFc  = (time1 - timeRpc + dTimeOffset)/sigmaDTof;

    // Test matching
    if(TMath::Abs(dTOFc) > dTimeCut || dThdPh > dThdPhCut) continue ;

    // Select best matching by smallest value of dTOFc
    if(TMath::Abs(dTOFc) < TMath::Abs(qualityDTime)) {
      qualityDThDPh = dThdPh;
      qualityDTime  = dTOFc;
      out           = rpc;
    }

  }
  if(out != NULL) cal->setMatchedRpc();
  return out;
}

Int_t HEmcClusterF::maxEnergyCell(void) const {
  // Find cell with max.energy deposit
  Int_t   cellMax   = -1;
  for(Int_t cell=0; cell<emcMaxComponents; cell++) {
    if(flagUsed[cell] == 0) {
      if(cellMax<0 || energy[cell] > energy[cellMax]) cellMax = cell;
    }
  }
  return cellMax;
}

Float_t HEmcClusterF::calcDist(HEmcCal *cal1,HEmcCal *cal2) const {
  Float_t dCol = cal1->getColumn() - cal2->getColumn();
  Float_t dRow = cal1->getRow()    - cal2->getRow();
  return TMath::Sqrt(dCol*dCol+dRow*dRow);
}

Int_t HEmcClusterF::getNearbyCell(Int_t cell,Int_t i) const {
  Char_t dColumn[8] = {-1,+1,  0, 0, -1,-1, +1,+1};
  Char_t dRow[8]    = { 0, 0, -1,+1, -1,+1, -1,+1};
  Int_t row    = cell/emcMaxColumns + dRow[i];
  if(row<0    || row>=emcMaxRows)       return -1;
  Int_t column = cell%emcMaxColumns + dColumn[i];
  if(column<0 || column>=emcMaxColumns) return -1;
  return row*emcMaxColumns + column;
}

ClassImp(HEmcClusterF)
