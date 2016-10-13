#include "hmdcrichcorr.h"
#include "hades.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hrichhit.h"
#include "richdef.h"
#include "hmdctrackddef.h"
#include "hmdclookuptb.h"
#include "hmdcclus.h"
#include "hevent.h"
#include "hmdcgetcontainers.h"
#include "hmdccalibrater1.h"
#include "hmdclistcells.h"
#include "hmdctrackdset.h"
#include "TH1.h"
#include "TF1.h"
#include "TMath.h"
#include "TROOT.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TStyle.h"

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 25/03/2003 by V. Pechenov

ClassImp(HMdcRichCorr)
ClassImp(HMdcDGaus)

HMdcRichCorr::HMdcRichCorr(void) : HReconstructor() {
  nRingsCut=1;
  setup();
}

HMdcRichCorr::HMdcRichCorr(const Text_t *name,const Text_t *title,Int_t nRCut) :
    HReconstructor(name,title) {
  nRingsCut=nRCut;
  setup();
}

void HMdcRichCorr::setup(void) {
  radToDeg     = 180./acos(-1.);
  clFinderType = 1; // chamber clusters

  pListCells   = 0;
  fLookUpTb    = 0;
  level4       = 10;
  level5       = 50;
  for(Int_t sec=0; sec<6; sec++)
      for(Int_t mod=0; mod<4; mod++) nLayers[sec][mod]=6;
  isInitialised=kFALSE;

  fClusCat=0;
  fCalib1=0;
  for(Int_t s=0;s<6;s++) {
    dThCuts[s][0][0]=dThCuts[s][1][0]=-5.;
    dThCuts[s][0][1]=dThCuts[s][1][1]=5.;
    dPhCuts[s][0][0]=dPhCuts[s][1][0]=-3.;
    dPhCuts[s][0][1]=dPhCuts[s][1][1]=3.;
    hDTh[s][0]=hDTh[s][1]=0;
    hDPh[s][0]=hDPh[s][1]=0;
  }
  numSigPh=numSigTh=3.;
  hDThAll[0]=hDThAll[1]=0;
  hDPhAll[0]=hDPhAll[1]=0;
  hNRings=hNMRings=hNEvents=0;
  
  setHistFile(".","fhs_","NEW");
  saveHist=saveCanv=savePSFile=kTRUE;
}

HMdcRichCorr::~HMdcRichCorr() {
  HMdcEvntListCells::deleteCont();
  HMdcLookUpTb::deleteCont();
  if(fCalib1) delete fCalib1;  
}

void HMdcRichCorr::makeCalibrater1(Int_t vers,Int_t cut,Int_t domerge) {
  fCalib1=new HMdcCalibrater1("MdcCali1","MdcCali1",vers,cut,domerge);
}

Bool_t HMdcRichCorr::init(void) {
  nEvents          = 0;
  nRingEvents      = 0;
  nMatchedEvents   = 0;
  nMatchedEvents1R = 0;
  if(fCalib1 && !fCalib1->init()) return kFALSE;
  HMdcGetContainers* fGetCont = HMdcGetContainers::getObject();
  
  if(pListCells == 0)  pListCells = HMdcEvntListCells::getObject(isMdcLCellsOwn);

  fClusCat = fGetCont->getCatMdcClus(kTRUE);
  if(!fClusCat) return kFALSE;
  iterMdcClus=(HIterator *)fClusCat->MakeIterator("native");

  fRichHitCat = gHades->getCurrentEvent()->getCategory(catRichHit);
  if(!fRichHitCat) return kFALSE;
  if(fRichHitCat) iterRichHit=(HIterator *)fRichHitCat->MakeIterator("native");

  fLookUpTb=HMdcLookUpTb::getObject();
  HMdcTrackDSet::setProjectPlotSizeSeg1(160,444);//!!!!!!!!!!!!319! ???
  fLookUpTb->setIsCoilOffFlag(kFALSE);
  return kTRUE;
}

Bool_t HMdcRichCorr::reinit(void) {
  if(fCalib1) if(!fCalib1->reinit()) return kFALSE;
  return reinitMdcFinder();
}

Bool_t HMdcRichCorr::reinitMdcFinder(void) {
  if(!fLookUpTb->initContainer()) return kFALSE;
  printStatus();
  if( !isInitialised ) {
    for(Int_t sec=0; sec<6; sec++) if(&((*fLookUpTb)[sec]))
      (*fLookUpTb)[sec].setTypeClFinder(clFinderType);
    for(Int_t sec=0; sec<6; sec++) {
      HMdcLookUpTbSec* fLookUpTbSec=&((*fLookUpTb)[sec]);
      if(!fLookUpTbSec) continue;
      for(Int_t mod=0; mod<2; mod++) {
        HMdcLookUpTbMod* fLookUpTbMod=&((*fLookUpTbSec)[mod]);
        if( !fLookUpTbMod ) nLayers[sec][mod]=0;
      }
    }
    isInitialised=kTRUE;
  }
  makeHist();
  return kTRUE;
}

void HMdcRichCorr::makeHist(void) {
  Char_t buf[100];
  if(hDPhAll[0]==0) {
    hDThAll[0]=new TH1F("dThetaM1","dTheta M1",100,-20.,20.);
    hDPhAll[0]=new TH1F("dPhiM1","dPhi M1",100,-20.,20.);
    hDThAll[1]=new TH1F("dThetaM2","dTheta M2",100,-20.,20.);
    hDPhAll[1]=new TH1F("dPhiM2","dPhi M2",100,-20.,20.);
  } else {
    hDThAll[0]->Reset();
    hDPhAll[0]->Reset();
    hDThAll[1]->Reset();
    hDPhAll[1]->Reset();
  }
  for(Int_t sec=0; sec<6; sec++) for(Int_t mod=0; mod<2; mod++) {
    if(hDTh[sec][mod]==0) {
      sprintf(buf,"dThetaS%iM%i",sec+1,mod+1);
      hDTh[sec][mod]=new TH1F(buf,"",100,-20.,20.);
      sprintf(buf,"dPhiS%iM%i",sec+1,mod+1);
      hDPh[sec][mod]=new TH1F(buf,"",100,-20.,20.);
    } else {
      hDTh[sec][mod]->Reset();
      hDPh[sec][mod]->Reset();
    }
    sprintf(buf,"dTheta: S%i M%i; Cut: %.2f +/- %g*%.2f",
        sec+1,mod+1,meanDTh(sec,mod),numSigTh,sigmaDTh(sec,mod));
    hDTh[sec][mod]->SetTitle(buf);
    sprintf(buf,"dPhi: S%i M%i; cut: %.2f +/- %g*%.2f",
        sec+1,mod+1,meanDPh(sec,mod),numSigPh,sigmaDPh(sec,mod));
    hDPh[sec][mod]->SetTitle(buf);
  }
  if(hNRings==0) {
    hNRings=new TH1F("hRings","<Num. of rings/event>",250,0.,500000.);
    hNMRings=new TH1F("hMRings","<Num. of matched rings/event>",250,0.,500000.);
    hNEvents=new TH1F("nEvents","Num of events",250,0.,500000.);
#if ROOT_VERSION_CODE  > ROOT_VERSION(6,0,0)
    hNRings->SetCanExtend(TH1::kAllAxes);
    hNMRings->SetCanExtend(TH1::kAllAxes);
    hNEvents->SetCanExtend(TH1::kAllAxes);
#else
    hNRings->SetBit(TH1::kCanRebin);
    hNMRings->SetBit(TH1::kCanRebin);
    hNEvents->SetBit(TH1::kCanRebin);
#endif
  } else {
    hNRings->Reset();
    hNMRings->Reset();
    hNEvents->Reset();
    hNRings->SetBins(500,0.,500000.);
    hNMRings->SetBins(500,0.,500000.);
    hNEvents->SetBins(500,0.,500000.);
  }
}

void HMdcRichCorr::setNLayers(const Int_t *lst) {
  Int_t* nLay=nLayers[0];
  for(Int_t i=0;i<6*4;i++) nLay[i]=lst[i];
}

void HMdcRichCorr::setDThetaCuts(Float_t dTh1, Float_t dTh2, Float_t nSig) {
  // Set the same cuts for all mdc modules, (dTh1<dTh2 !)
  numSigTh=nSig;
  for(Int_t s=0;s<6;s++) {
    dThCuts[s][0][0]=dThCuts[s][1][0]=dTh1;
    dThCuts[s][0][1]=dThCuts[s][1][1]=dTh2;
  }
}

void HMdcRichCorr::setDPhiCuts(Float_t dPh1, Float_t dPh2, Float_t nSig) {
  // set the same cuts for all mdc modules, (dPh1<dPh2 !)
  numSigPh=nSig;
  for(Int_t s=0;s<6;s++) {
    dPhCuts[s][0][0]=dPhCuts[s][1][0]=dPh1;
    dPhCuts[s][0][1]=dPhCuts[s][1][1]=dPh2;
  }
}

void HMdcRichCorr::setDThetaCuts(const Float_t* dTh, Float_t nSig) {
  // dTheta cuts seting
  numSigTh=nSig;
  Float_t* dThAr=dThCuts[0][0];
  for(Int_t i=0;i<6*2*2;i++) dThAr[i]=dTh[i];
}

void HMdcRichCorr::setDPhiCuts(const Float_t* dPh, Float_t nSig) {
  // dPhi cuts seting
  numSigPh=nSig;
  Float_t* dPhAr=dPhCuts[0][0];
  for(Int_t i=0;i<6*2*2;i++) dPhAr[i]=dPh[i];
}

void HMdcRichCorr::setDThetaCutsSig(Float_t mDTh,Float_t sDTh,Float_t nSig) {
  // Set the same cuts for all mdc modules. 
  // Cut: mDTh-nSig*sDTh < dTheta < mDTh+nSig*sDTh
  numSigTh=nSig;
  for(Int_t s=0;s<6;s++) {
    dThCuts[s][0][0]=dThCuts[s][1][0]=mDTh-nSig*sDTh;
    dThCuts[s][0][1]=dThCuts[s][1][1]=mDTh+nSig*sDTh;
  }
}

void HMdcRichCorr::setDPhiCutsSig(Float_t mDPh,Float_t sDPh,Float_t nSig) {
  // set the same cuts for all mdc modules, (dPh1<dPh2 !)
  // Cut: mDPh-nSig*sDPh < dPhi < mDPh+nSig*sDPh
  numSigPh=nSig;
  for(Int_t s=0;s<6;s++) {
    dPhCuts[s][0][0]=dPhCuts[s][1][0]=mDPh-nSig*sDPh;
    dPhCuts[s][0][1]=dPhCuts[s][1][1]=mDPh+nSig*sDPh;
  }
}

void HMdcRichCorr::setDThetaCuts(const Float_t* mDTh, const Float_t* sDTh, 
    Float_t nSig) {
  // dTheta cuts seting
  numSigTh=nSig;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<2;m++) {
    Int_t i=s*2+m;
    dThCuts[s][m][0]=mDTh[i]-nSig*sDTh[i];
    dThCuts[s][m][1]=mDTh[i]+nSig*sDTh[i];
  }
}

void HMdcRichCorr::setDPhiCuts(const Float_t* mDPh, const Float_t* sDPh, 
    Float_t nSig) {
  // dPheta cuts seting
  numSigPh=nSig;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<2;m++) {
    Int_t i=s*2+m;
    dPhCuts[s][m][0]=mDPh[i]-nSig*sDPh[i];
    dPhCuts[s][m][1]=mDPh[i]+nSig*sDPh[i];
  }
}

Int_t HMdcRichCorr::execute(void) {
  if(nEvents==0) {
    HMdcGetContainers::getFileNameWInd(fileName);
    HMdcGetContainers::getFileName(flNmWoExt);
    HMdcGetContainers::getRunId(runId);
    HMdcGetContainers::getEventDate(eventDate);
    HMdcGetContainers::getEventTime(eventTime);
  }
  nEvents++;
  hNEvents->Fill(nEvents-0.5);
  iterRichHit->Reset();
  for(Int_t s=0;s<6;s++) nRichHits[s]=0;
  fillListByRichHit();
  Int_t nRings=nRichHits[0]+nRichHits[1]+nRichHits[2]+nRichHits[3]+
      nRichHits[4]+nRichHits[5];
  if(nRings==0) return kSkipEvent;
  hNRings->Fill(nEvents-0.5,(Float_t)nRings);
  
  nRingEvents++;

  if(fCalib1) fCalib1->execute();
  if(isMdcLCellsOwn) {
    pListCells->clear();
    Int_t nFiredWires = pListCells->collectWires();
    if(nFiredWires<3) return kSkipEvent;
  }
  findMdcClusters();

  iterMdcClus->Reset();
  HMdcClus* pMdcClus;
  Int_t nMatchedRings=0;
  while( (pMdcClus=(HMdcClus*)iterMdcClus->Next()) !=0) {
    if(pMdcClus->getIOSeg()!=0) continue;
    Int_t sec=pMdcClus->getSec();
    Float_t mdcTheta=pMdcClus->getTheta()*radToDeg;
    Float_t mdcPhi=pMdcClus->getPhi()*radToDeg+sec*60.;
    if(mdcPhi>360.) mdcPhi-=360.;
    Int_t mod=pMdcClus->getMod();
    if(mod<0) mod=1;
    for(Int_t r=0;r<nRichHits[sec];r++) {
      Float_t dPh=richPhi[sec][r]-mdcPhi;
      Float_t dTh=richTheta[sec][r]-mdcTheta;
      Bool_t dPhCond = dPh>=dPhCuts[sec][mod][0] && dPh<=dPhCuts[sec][mod][1];
      Bool_t dThCond = dTh>=dThCuts[sec][mod][0] && dTh<=dThCuts[sec][mod][1];
      if(dPhCond) hDTh[sec][mod]->Fill(dTh);
      if(dThCond) hDPh[sec][mod]->Fill(dPh);
      if(!dPhCond || !dThCond) continue;
      nMdcHitRing[sec][r]++;
      if(nMdcHitRing[sec][r]==1) nMatchedRings++;
    }
  }

  if(nMatchedRings) hNMRings->Fill(nEvents-0.5,(Float_t)nMatchedRings);
  if(nMatchedRings<nRingsCut) return kSkipEvent;
  nMatchedEvents++;
  if(nMatchedRings==1) nMatchedEvents1R++;

  return 0;
}

void HMdcRichCorr::fillListByRichHit(void) {
  HRichHit* pRichHit;
  while( (pRichHit=(HRichHit*)iterRichHit->Next()) !=0) {
    Int_t sec=pRichHit->getSector();
    Int_t& nHits=nRichHits[sec];
    if(nHits>=400) continue;
//     if(pRichHit->getRingPatMat()<=250 &&
//        pRichHit->getRingHouTra()<=28) continue;
    richTheta[sec][nHits]=pRichHit->getTheta();
    richPhi[sec][nHits]=pRichHit->getPhi();
    nMdcHitRing[sec][nHits]=0;
    nHits++;
  }
}

void HMdcRichCorr::findMdcClusters(void) {
  // Fired wires collecting:
  (*fLookUpTb).clear();

  // Cluster finder levels determination:
  Int_t minLevel[6][4];
  for(Int_t sec=0; sec<6; sec++) {
    if(nRichHits[sec]==0) continue;
    Int_t* minlv=minLevel[sec];
    minlv[2]=minlv[3]=0;
    for(Int_t mod=0; mod<2; mod++) {
      minlv[mod]=nLayers[sec][mod];
      if( minlv[mod]>0 ) {
        Int_t occup=((*pListCells)[sec][mod].getNCells()*6)/minlv[mod];
        if( occup <= level5 ) minlv[mod]--;
        if( occup <= level4 ) minlv[mod]--;
        if( occup < 3 ) minlv[mod]=3;
      }
    }
  }

  // Track finder:
  for(Int_t sec=0; sec<6; sec++) {
    nMdcHits[sec]=0;
    if(nRichHits[sec]==0 || &(*fLookUpTb)[sec]==0 ) continue;
    nMdcHits[sec]=(*fLookUpTb)[sec].findClusters(minLevel[sec]);
  }
}

Bool_t HMdcRichCorr::finalize(void) {
  for(Int_t sec=0; sec<6; sec++) for(Int_t mod=0; mod<2; mod++) {
    if(hDThAll[mod] && hDTh[sec][mod]) hDThAll[mod]->Add(hDTh[sec][mod]);
    if(hDPhAll[mod] && hDPh[sec][mod]) hDPhAll[mod]->Add(hDPh[sec][mod]);
  }
  
  Float_t meanDThN[6][2];
  Float_t sigmaDThN[6][2];
  Float_t meanDPhN[6][2];
  Float_t sigmaDPhN[6][2];
  
  gROOT->SetBatch();
  gStyle->SetOptFit(111);
  Bool_t fitFlag=kTRUE;
  TCanvas cDThDPhPlane("cDThDPhPlane","dTheta, dPhi plane",650,900);
  cDThDPhPlane.Divide(2,2);
  TCanvas cDThMod("cDThMod","dTheta",650,900);
  cDThMod.Divide(2,6);
  TCanvas cDPhMod("cDPhMod","dPhi",650,900);
  cDPhMod.Divide(2,6);
  TCanvas cNRings("cNRings","<Num. rings>",650,900);
  cNRings.Divide(1,2);
  TLine* ln=new TLine();
  // Fit:
  calcAvMnSg();
  for(Int_t m=0; m<2; m++) {
    Float_t mnN,sgN;
    cDThDPhPlane.cd(m*2+1);
    if(hDThAll[m]) funDGaus.histFit(hDThAll[m],planeMeanDTh[m],planeSigmaDTh[m],
      mnN,sgN,kFALSE);
    cDThDPhPlane.cd(m*2+2);
    if(hDPhAll[m]) funDGaus.histFit(hDPhAll[m],planeMeanDPh[m],planeSigmaDPh[m],
      mnN,sgN,kFALSE);
  }
  cDThDPhPlane.Update();
  for(Int_t s=0; s<6; s++) for(Int_t m=0; m<2; m++) {
    cDThMod.cd(s*2+m+1);
    if(hDTh[s][m]) {
      Float_t mean=meanDTh(s,m);
      Float_t sigma=sigmaDTh(s,m);
      fitStatDTh[s][m]=funDGaus.histFit(hDTh[s][m],
          mean,sigma,meanDThN[s][m],sigmaDThN[s][m]);
      if(fitStatDTh[s][m]!=0) fitFlag=kFALSE; 
      drawCut(ln,mean,sigma,numSigTh,4,hDTh[s][m]->GetMaximum()/2.);
      drawCut(ln,meanDThN[s][m],sigmaDThN[s][m],numSigTh,
          3-fitStatDTh[s][m], hDTh[s][m]->GetMaximum()/2.);
    } else fitStatDTh[s][m]=-1;
    cDPhMod.cd(s*2+m+1);
    if(hDPh[s][m]) {
      Float_t mean=meanDPh(s,m);
      Float_t sigma=sigmaDPh(s,m);
      fitStatDPh[s][m]=funDGaus.histFit(hDPh[s][m],
          mean,sigma,meanDPhN[s][m],sigmaDPhN[s][m]);
      if(fitStatDPh[s][m]!=0) fitFlag=kFALSE;
      drawCut(ln,mean,sigma,numSigPh,4,hDPh[s][m]->GetMaximum()/2.);
      drawCut(ln,meanDPhN[s][m],sigmaDPhN[s][m],numSigPh,
          3-fitStatDPh[s][m], hDPh[s][m]->GetMaximum()/2.);
    } else fitStatDPh[s][m]=-1;
  }
  cDThMod.Update();
  cDPhMod.Update();
  for(Int_t bin=0;bin<hNEvents->GetNbinsX();bin++) {
    Stat_t norm=hNEvents->GetBinContent(bin+1);
    if(norm==0.) break; 
    Stat_t nrn=hNRings->GetBinContent(bin+1);
    Stat_t nmrn=hNMRings->GetBinContent(bin+1);
    hNRings->SetBinContent(bin+1,nrn/norm);
    hNMRings->SetBinContent(bin+1,nmrn/norm);
    hNRings->SetBinError(bin+1,sqrt(nrn)/norm);
    hNMRings->SetBinError(bin+1,sqrt(nmrn)/norm);
  }
  cNRings.cd(1);
  hNRings->Draw();
  cNRings.cd(2);
  hNMRings->Draw();
  cNRings.Update();
  
  // Writing hist. to file:
  const Char_t* flag=(fitFlag) ? "":"x_";
  TFile* file=0;
  if(saveHist || saveCanv) file=openHistFile(flag);
  if(file) {
    if(saveHist) {
      if(hDThAll[0]) hDThAll[0]->Write();
      if(hDThAll[1]) hDThAll[1]->Write();
      if(hDPhAll[0]) hDPhAll[0]->Write();
      if(hDPhAll[1]) hDPhAll[1]->Write();
      for(Int_t sec=0; sec<6; sec++) for(Int_t mod=0; mod<2; mod++) {
        if(hDTh[sec][mod]) hDTh[sec][mod]->Write();
        if(hDPh[sec][mod]) hDPh[sec][mod]->Write();
      }
      hNEvents->Write();
      hNRings->Write();
      hNMRings->Write();
    }
    if(saveCanv) {
      cDThDPhPlane.Write();
      cDThMod.Write();
      cDPhMod.Write();
      cNRings.Write();
    }
    
    file->Close();
    delete file;
  }
  
  // ps-file:
  if(savePSFile) {
    TString tPsFile=psFile+"(";
    cNRings.Print(tPsFile.Data());
    cDThDPhPlane.Print(psFile.Data());
    cDThMod.Print(psFile.Data());
    tPsFile=psFile+")";
    cDPhMod.Print(tPsFile.Data());
  }

  delete ln;
  gROOT->SetBatch(kFALSE);
    
  printf("\n\n********** Events filtering *********************************\n");
  printf("* FileName      : %s\n* RunId         : %s\n* Run start time: %s %s\n",
      fileName.Data(),runId.Data(),eventTime.Data(),eventDate.Data());
  printf("* Hist.file     : %s\n",histFile.Data());
  printf("*------------------------------------------------------------\n");
  printf("* Total number of real events processed  : %8i\n",nEvents);
  if(nEvents>0) {
    printf("* Number of events with rings            : %8i (%6.2f%c)\n",
        nRingEvents,Float_t(nRingEvents)/Float_t(nEvents)*100.,'%');
    printf("* Number of matched events               : %8i (%6.2f%c)\n",
        nMatchedEvents,Float_t(nMatchedEvents)/Float_t(nEvents)*100.,'%');
    printf("* Number of events with >=2 matched rings: %8i (%6.2f%c)\n",
        nMatchedEvents-nMatchedEvents1R,
        Float_t(nMatchedEvents-nMatchedEvents1R)/Float_t(nEvents)*100.,'%');
  }
  printf("*------------------------------------------------------------\n");
  printf("  Float_t meanDTh[6][2]={\n");
  for(Int_t s=0; s<6; s++) {
    printf("    {%5.2f, %5.2f},  // Sec.%i Mdc1 Mdc2| Shift: %+5.2f, %+5.2f\n",
        meanDThN[s][0],meanDThN[s][1],s,meanDThN[s][0]-meanDTh(s,0),
        meanDThN[s][1]-meanDTh(s,1));
  }
  printf("  };\n");
  printf("  Float_t sigmaDTh[6][2]={\n");
  for(Int_t s=0; s<6; s++) {
    printf("    {%5.2f, %5.2f},  // Sec.%i Mdc1 Mdc2 | Shift: %+5.2f, %+5.2f\n",
        sigmaDThN[s][0],sigmaDThN[s][1],s,sigmaDThN[s][0]-sigmaDTh(s,0),
        sigmaDThN[s][1]-sigmaDTh(s,1));
  }
  printf("  };\n");
  printf("  Float_t meanDPh[6][2]={\n");
  for(Int_t s=0; s<6; s++) {
    printf("    {%5.2f, %5.2f},  // Sec.%i Mdc1 Mdc2 | Shift: %+5.2f, %+5.2f\n",
        meanDPhN[s][0],meanDPhN[s][1],s,meanDPhN[s][0]-meanDPh(s,0),
        meanDPhN[s][1]-meanDPh(s,1));
  }
  printf("  };\n");
  printf("  Float_t sigmaDPh[6][2]={\n");
  for(Int_t s=0; s<6; s++) {
    printf("    {%5.2f, %5.2f},  // Sec.%i Mdc1 Mdc2 | Shift: %+5.2f, %+5.2f\n",
        sigmaDPhN[s][0],sigmaDPhN[s][1],s,sigmaDPhN[s][0]-sigmaDPh(s,0),
        sigmaDPhN[s][1]-sigmaDPh(s,1));
  }
  printf("  };\n");
  printf("*************************************************************\n\n");

  if(fCalib1) if(!fCalib1->finalize()) return kFALSE;
  return kTRUE;
}

void HMdcRichCorr::calcAvMnSg(void) {
  for(Int_t m=0;m<2;m++) {
    planeMeanDTh[m]=0;
    planeSigmaDTh[m]=0;
    planeMeanDPh[m]=0;
    planeSigmaDPh[m]=0;
    Float_t nm=0.;
    for(Int_t s=0;s<2;s++) {
      if(dThCuts[s][m][1]-dThCuts[s][m][0]<=0. || 
          dPhCuts[s][m][1]-dPhCuts[s][m][0]<=0.) continue;
      planeMeanDTh[m]+=meanDTh(s,m);
      planeSigmaDTh[m]+=sigmaDTh(s,m);
      planeMeanDPh[m]+=meanDPh(s,m);
      planeSigmaDPh[m]+=sigmaDPh(s,m);
      nm++;
    }
    if(nm>0.) {
      planeMeanDTh[m]/=nm;
      planeSigmaDTh[m]/=nm;
      planeMeanDPh[m]/=nm;
      planeSigmaDPh[m]/=nm;
    }
  }
}

TFile* HMdcRichCorr::openHistFile(const Char_t* flag) {
  histFile=histFileDir+flag+histFileSuffix+flNmWoExt+".root";
  TFile* file=new TFile(histFile.Data(),histFileOption.Data());
  if(file==0 || file->IsOpen()) {
    psFile=histFileDir+flag+histFileSuffix+flNmWoExt+".ps";
    return file;
  }
  printf("File %s already exists!\n",histFile.Data());
  const Char_t* ind="123456789";
  for(Int_t i=0;i<9;i++){
    delete file;
    histFile=histFileDir+flag+histFileSuffix+flNmWoExt+"_"+ind[i]+".root";
    file=new TFile(histFile.Data(),histFileOption.Data());
    if(file==0 || file->IsOpen()) {
      psFile=histFileDir+flag+histFileSuffix+flNmWoExt+"_"+ind[i]+".ps";
      return file;
    }
  }
  Error("openHistFile","Can not open histograms file!");
  delete file;
  return 0;
}

void HMdcRichCorr::setHistFile(const Char_t* dir,const Char_t* suf, const Char_t* option)
{
    TString tmp = dir;
    if(tmp.Length()>0) {
	histFileDir=dir;
	if(histFileDir.Length()>0 && histFileDir(histFileDir.Length()-1)!='/')
	    histFileDir+='/';
    }
    tmp = suf;

  if(tmp.Length()>0) histFileSuffix=suf;
  tmp=option;
  if(tmp.Length()>0) histFileOption=option;
}

void HMdcRichCorr::printStatus(void) const{
  // prints the parameters to the screen
  const Char_t* hType ="0 = comb.chamb.clust., 1 = single chamb.clust., 2 = 0+1";
  const Char_t* hLevS1="max. occupancy of MDC for cl.find. level 4 & 5 layers";
  const Int_t* l=nLayers[0];
  printf("\n\n-----------------------------------------------------------------\n");
   printf("HMdcRichCorr setup:\n");
  printf("Using rings param. from HRichHit container for matching to MDC.\n");
  printf("Filtering of events with >=%i matched to mdc rings.\n",nRingsCut);
  
  printf("Matching cuts:\n");
  printf("  abs(dTheta-meanDTheta)<%3.1f*sigmaDTheta  (dTheta=thetaRich-thetaMdc)\n",
      numSigTh);
  printf("  abs(dPhi  -meanDPhi  )<%3.1f*sigmaDPhi    (dPhi  =phiRich  -phiMdc  )\n",
      numSigPh);
  printf("               dTheta:  mean  sigma    dPhi:  mean  sigma\n");
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<2;m++) {
    printf("  %i sec. %i mdc:       %6.2f %6.2f         %6.2f %6.2f\n",s+1,m+1,
        (dThCuts[s][m][0]+dThCuts[s][m][1])/2.,
        (dThCuts[s][m][1]-dThCuts[s][m][0])/(2.*numSigTh),
        (dPhCuts[s][m][0]+dPhCuts[s][m][1])/2.,
        (dPhCuts[s][m][1]-dPhCuts[s][m][0])/(2.*numSigPh));
  }
  printf("-----------------------------------------------------------------\n");
  printf("HMdcTrackFinderSetup:\n");
  printf("ClustFinderType = %i :  %s\n",clFinderType,hType);
  printf("LevelsSegment1  = %i,%i  : %s\n",level4,level5,hLevS1);
  printf("NumLayersPerMod = {{%i,%i,,},\n",l[ 0],l[ 1]);
  printf("                   {%i,%i,,},\n",l[ 4],l[ 5]);
  printf("                   {%i,%i,,},\n",l[ 8],l[ 9]);
  printf("                   {%i,%i,,},\n",l[12],l[13]);
  printf("                   {%i,%i,,},\n",l[16],l[17]);
  printf("                   {%i,%i,,}}\n",l[20],l[21]);
  printf("-----------------------------------------------------------------\n");
}

void HMdcRichCorr::drawCut(TLine* ln,Float_t mean,Float_t sigma,Float_t nSig,
    Int_t color, Double_t y) {
  ln->SetLineColor(color);
  ln->DrawLine(mean-sigma*nSig,0.,mean-sigma*nSig,y);
  ln->DrawLine(mean+sigma*nSig,0.,mean+sigma*nSig,y);
}

HMdcDGaus::HMdcDGaus(void) : TF1("dgaus",dgaus,-10.,10.,(Int_t)6) {
  condMean=0.3;
  condSig1=0.3;
  condSig2=0.5;
}

HMdcDGaus::HMdcDGaus(const Char_t* name, Double_t xmin, Double_t xmax) : 
    TF1(name,dgaus,xmin,xmax,(Int_t)6) {
  condMean=0.3;
  condSig1=0.3;
  condSig2=0.5;
}

Int_t HMdcDGaus::histFit(TH1F* hst,Float_t oMean,Float_t oSig,
    Float_t& nMean,Float_t& nSig, Bool_t doTest) {
  if(hst==0) return kFALSE;
  Double_t mean=oMean;
  Double_t hMax=hst->GetMaximum();
  SetParameters(hMax*0.9,oMean,oSig,hMax*0.1,0.,20.);
  //  SetParLimits(0,0.,1.0e+8);
  SetParLimits(1,-oSig*7,oSig*7);
  SetParLimits(2,oSig/2.,oSig*2);  // limit: 0.5*sig - 2*sig
  //  SetParLimits(3,0.,1.0e+8);
  //  SetParLimits(4,-1.0e+3,1.0e+3);
  //  SetParLimits(5,5.,1.0e+3);    
  Int_t fitStatus=hst->Fit(this,"WQ","");
  if(fitStatus!=0) fitStatus=2;
  nMean=GetParameter(1);
  for(Int_t i=0;i<5 && fitStatus!=0;i++) {
    if(fabs(nMean-mean)>oSig) {  // shift of mean <= one sigma
      if(nMean<mean) mean-=oSig;
      else mean+=oSig;
    } else mean=nMean;
    SetParameters(hMax*0.9,mean,oSig,hMax*0.1,0.,15.);
    SetParLimits(2,oSig/2.,oSig*3);  // limit: 0.5*sig - 3*sig
    fitStatus=hst->Fit(this,"WQ","");
    nMean=GetParameter(1);
  }
  nSig=GetParameter(2);
  if(doTest && fitStatus==0 && (nSig-oSig>oSig*condSig1 || 
      oSig-nSig>oSig*condSig2 || fabs(nMean-oMean)>oSig*condMean)) fitStatus=1;
  TF1* fngh=hst->GetFunction("dgaus");
  fngh->SetLineWidth((Width_t)2.);
  fngh->SetLineColor(3-fitStatus);
  
  return fitStatus;
}

Double_t HMdcDGaus::dgaus(Double_t *x, Double_t *par) {
  // The signal function: a gaussian
  return par[0]*TMath::Gaus(x[0],par[1],par[2])+
         par[3]*TMath::Gaus(x[0],par[4],par[5]);
}
