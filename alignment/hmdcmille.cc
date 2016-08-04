//*--- Author : V.Pechenov
//*--- Modified: 26.06.07 V.Pechenov
//*--- $Id: hmdcmille.cc,v 1.61 2007/07/09 12:38:36 pechenov Exp $

using namespace std;
#include "hmdcmille.h"
#include "Mille.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hmdcsizescells.h"
#include "hmdcclussim.h"
#include "hmdctrackdset.h"
#include "hmdctrackparam.h"
#include "hmdcwiredata.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hmdcgetcontainers.h"
#include "hmdclayergeompar.h"
#include "hmdclayercorrpar.h"
#include "hmdcgeompar.h"
#include "hspecgeompar.h"
#include "hmdcgeomstruct.h"
#include "hparasciifileio.h"
#include "hmdcdetector.h"
#include "hmdclistcells.h"
#include <TH1.h>
#include <TH2.h>
#include <TSystem.h>

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
// HMdcMille
//
// Interface class for Millipede alignment.
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HMdcMille)

HMdcMille::HMdcMille() : HMdc12Fit("mdc.Mille","mdc.Mille") {
  setDef();
}

HMdcMille::HMdcMille(const Char_t* milleOutFName,const Char_t* geomPFName,
    Int_t bmTmID,const Char_t* sumOfShFName) : HMdc12Fit("mdc.Mille","mdc.Mille") {
  
  setDef();
  if(bmTmID<0 || bmTmID>9) printErrorAndExit("HMdcMille","beamTimeId must be >= 0 and <=9 !!!");
  beamTimeId = bmTmID;
  
//!  if(milleOutFName) mille = new Mille(milleOutFName);
//!  else printErrorAndExit("HMdcMille",
//!      "Mille output file name is not specifaed !!!");
  milleFileName = milleOutFName;
  openNewBinaryFile();
  
  if(sumOfShFName != 0) sumOfShiftsFName = sumOfShFName;
  if(geomPFName != 0)   geomParFileName  = geomPFName;
  
  readSumOfShiftsFile();
  readPedeResFile(pedeResFName + ".res");
  if(!loadGeometryPar()) printErrorAndExit("HMdcMille","Can't load mdc geometry!!! Stop!");
  if(!makeShifts())      printErrorAndExit("HMdcMille","Can't change mdc geometry!");
   
  char buf[50];
  sprintf(buf,".st%02i",iteration);
  stepIter = buf;
}

void HMdcMille::setDef(void) {
  HMdcTrackDSet::setAnotherFit(this);
  nParMax          = 9;
  iteration        = 0;
  mille            = 0;
  shiftType        = 1;
  doCopyGeomFile   = kFALSE;
  doCopySumShFile  = kTRUE;
  doCopyResFile    = kTRUE;
  doCopyLogFile    = kFALSE;
  doCopyHisFile    = kFALSE;
  
  createPedeInFile = kFALSE; // kTRUE - create par.file
  isGeomChanged    = kFALSE;
  isGeomFileExist  = kFALSE;
  nTracks          = 0;
  nWiresTot        = 0;
  nLayersTot       = 0;
  parSteps[0]      = 0.01;     // shift along X
  parSteps[1]      = 0.01;     // shift along Y
  parSteps[2]      = 0.01;     // shift along Z
  parSteps[3]      = 0.003;    // rotation around X (in degree)
  parSteps[4]      = 0.003;    // rotation around Y (in degree)
  parSteps[5]      = 0.003;    // rotation around Z (in degree)
  parSteps[6]      = 0.004;    // cell thickness
  cellThicknFree   = kFALSE;
  nHists           = 0;
  fixFitTOffset    = kTRUE;
  useMdcShParOnly  = kFALSE;
  doLayP2Align     = kTRUE;
  nWiresMin        = 15;
  thetaCut         = -1.;
  
  pGetCont         = HMdcGetContainers::getObject();
  pMdcDet          = pGetCont->getMdcDetector();
  
  nBinaryFiles     = 0;
  milleFileSize    = 0;
  crPedeTaskFile   = kFALSE;
  
  printDebugFlag   = kFALSE;
  doHists          = kTRUE;
  
  nClustersCut     = 5;  //???
  filteringFlag    = 0;
  for(Int_t s=0;s<6;s++) {
    useSector[s] = kTRUE;
    nWiresCut[s] = 320;
  }
  
  // Clean arrais:
  for(Int_t im=0;im<24;im++)  pSCMod[im]      = NULL;
  for(Int_t il=0;il<144;il++) pSCLayer[il]    = NULL;
  for(Int_t il=0;il<144;il++) nLayerParts[il] = 0;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) {
    isMdcInAlign[s][m] = kFALSE;
    histInd[s]         = -1;
    mdcMods[s][m]      = 0;
    shiftSysMdc[s][m]  = -1;
    for(Int_t p=0;p<nParMax;p++) {   // Mdc shifts
      mShFlag[s][m][p]   = mFixFlag[s][m][p] = kFALSE;
      shiftsMdc[s][m][p] = 0.;
      sigmaMdc[s][m][p]  = 0.;
    }
    for(Int_t l=0;l<6;l++) {   // Layer shifts
      shiftSysLay[s][m][l] = -1;
      for(Int_t p=0;p<nParMax;p++) {
        lShFlag[s][m][l][p]   = kFALSE;
        lFixFlag[s][m][l][p]  = kFALSE;
        shiftsLay[s][m][l][p] = 0.;
        sigmaLay[s][m][l][p]  = 0.;
      }
    }
  }
  
  shitsInfo[0] = "shift along X axis [mm]";
  shitsInfo[1] = "shift along Y axis [mm]";
  shitsInfo[2] = "shift along Z axis [mm]";
  shitsInfo[3] = "rotation around X axis [deg]";
  shitsInfo[4] = "rotation around Y axis [deg]";
  shitsInfo[5] = "rotation around Z axis [deg]";
  shitsInfo[6] = "cell thickness";
  shitsInfo[7] = "shift along Y axis [mm]";
  shitsInfo[8] = "rotation around Z axis [deg]";
  
  // Default file names:
  pedeResFName     = "millepede";
  sumOfShiftsFName = "sumShifts.txt";
  geomParFileName  = "geoParameters.txt";
}

HMdcMille::~HMdcMille() {
  pGetCont->deleteCont();
}

Bool_t HMdcMille::init(void) {
  return HMdc12Fit::init();
}

Bool_t HMdcMille::reinit(void) {
  if( !HMdc12Fit::reinit() ) return kFALSE;
  HMdcSizesCells* pSizesCells = HMdcSizesCells::getExObject();
  if(pSizesCells == 0) return kFALSE;
  
  setShiftTransformation();
  
  for(Int_t sec=0;sec<6;sec++) {
    nMods[sec] = 0;
    HMdcSizesCellsSec& pSizesCellsSec = (*pSizesCells)[sec];
    if(&pSizesCellsSec == 0) continue;
    if(doHists) createHists(sec);
    for(module=0;module<4;module++) {
      HMdcSizesCellsMod& pSizesCellsMod = pSizesCellsSec[module];
      if(&pSizesCellsMod == 0) continue;
      nMods[sec]++;
      setIMod(sec,module);    // calculate iMod
      pSCMod[iMod] = &pSizesCellsMod;
      
      for(layer=0;layer<6;layer++) {
        HMdcSizesCellsLayer& rSizesCellsLay = pSizesCellsMod[layer];
        setILay(layer);      // calculate iLay
        if(&rSizesCellsLay == 0) continue;
        pSCLayer[iLay]    = &rSizesCellsLay;
        nLayerParts[iLay] = rSizesCellsLay.getLayerNParts();
        if     (shiftType == 0) setTrShiftInSecSys();
        else if(shiftType == 1) setTrShiftInModSys();
        else if(shiftType == 2) setTrShiftInLabSys();
      }
    }
  }

  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=-1;l<6;l++) {
    if(l>=0 && useMdcShParOnly) continue;
    for(Int_t p=0;p<nParMax;p++) {
      if(l <0 && (!mShFlag[s][m][p]    || mFixFlag[s][m][p]    || sigmaMdc[s][m][p]<0.)   ) continue;
      if(l>=0 && (!lShFlag[s][m][l][p] || lFixFlag[s][m][l][p] || sigmaLay[s][m][l][p]<0.)) continue;
      isMdcInAlign[s][m] = kTRUE;
    }
  }

  return kTRUE;
}

void HMdcMille::createHists(Int_t sec) {
  if(fHistograms != 0 && nHists>0) return;
  if(fHistograms == 0) fHistograms = new TList();
  char name[100];
  char title[100];
  histInd[sec] = nHists;
  
  sprintf(name,"pThVsPhS%i",sec+1);
  sprintf(title,"Sec.%i #Theta vs #phi",sec+1);
  fHistograms->Add(new TH2F(name,title,60,60.,120.,80,10.,90.));
  nHists++;
  
  sprintf(name,"hChi2S%i",sec+1);
  sprintf(title,"Sec.%i Chi^{2}/NDF",sec+1);
  fHistograms->Add(new TH1F(name,title,200,0.,20.));
  nHists++;
  
  sprintf(name,"hNWiresS%i",sec+1);
  sprintf(title,"Sec.%i Num.wires per track",sec+1);
  Int_t nBins = clusFitAlg==4 ? 100 : 50;
  fHistograms->Add(new TH1F(name,title,nBins,0.,nBins));
  nHists++;
  
  nBins = clusFitAlg==4 ? 49 : 25;
  sprintf(name,"hNLayersS%i",sec+1);
  sprintf(title,"Sec.%i Num.layers per track",sec+1);
  fHistograms->Add(new TH1F(name,title,nBins,0.,nBins));
  nHists++;
}

void HMdcMille::setShiftTransformation(void) {
  Double_t steps[6]={0.,0.,0.,0.,0.,0.};
  for(Int_t parn=0;parn<6;parn++) {
    derNorm[parn] =  1./parSteps[parn];
    steps[parn]   =  parSteps[parn];
    HMdcSizesCells::setTransform(steps,posShifts[parn]);

    steps[parn] = -parSteps[parn];
    HMdcSizesCells::setTransform(steps,negShifts[parn]);
    steps[parn] = 0.;
  }
  // For layer thickness align. parameter:
  for(Int_t lay=0;lay<6;lay++) {
    steps[2]        =  parSteps[6]*(lay-3+0.5);
    derLThNorm[lay] =  1./steps[2];
    HMdcSizesCells::setTransform(steps,posLThSh[lay]);
    
    steps[2] = -steps[2];
    HMdcSizesCells::setTransform(steps,negLThSh[lay]);
    steps[2] = 0.;
  }
}

void HMdcMille::setTrShiftInModSys(void) {
  // mdcSys - mdc<->sec. transfomation
  // laySec - rotated_layer<->mdc transfomation
  const HGeomTransform      *mdcSys   = pSCMod[iMod]->getSecTrans();
  const HGeomTransform      *laySec   = pSCLayer[iLay]->getRotLayP1SysRMod();
  const HGeomTransform      *layP2Mod = NULL;
  if(nLayerParts[iLay] == 2) layP2Mod = pSCLayer[iLay]->getRotLayP2SysRMod();
  for(Int_t parn=0;parn<6;parn++) {
    calcShiftInModSys(mdcSys,laySec,posShifts[parn],layPosSh[iLay][parn]);
    calcShiftInModSys(mdcSys,laySec,negShifts[parn],layNegSh[iLay][parn]);
    if(layP2Mod == NULL) continue;
    calcShiftInModSys(mdcSys,layP2Mod,posShifts[parn],layPosSh[iLay][parn+20]);
    calcShiftInModSys(mdcSys,layP2Mod,negShifts[parn],layNegSh[iLay][parn+20]);
  }
  // Cell thickness:
  calcShiftInModSys(mdcSys,laySec,posLThSh[layer],layPosSh[iLay][6]);
  calcShiftInModSys(mdcSys,laySec,negLThSh[layer],layNegSh[iLay][6]);
  if(layP2Mod != NULL) {
    calcShiftInModSys(mdcSys,layP2Mod,posLThSh[layer],layPosSh[iLay][6+20]);
    calcShiftInModSys(mdcSys,layP2Mod,negLThSh[layer],layNegSh[iLay][6+20]);
  }
}

void HMdcMille::setTrShiftInSecSys(void) {
  // laySec - rotated_layer<->sec transfomation
  const HGeomTransform*      laySec   = pSCLayer[iLay]->getRotLayP1SysRSec();
  const HGeomTransform*      layP2Sec = NULL;
  if(nLayerParts[iLay] == 2) layP2Sec = pSCLayer[iLay]->getRotLayP2SysRSec();
  for(Int_t parn=0;parn<6;parn++) {
    calcShiftInSecSys(laySec,posShifts[parn],layPosSh[iLay][parn]);
    calcShiftInSecSys(laySec,negShifts[parn],layNegSh[iLay][parn]);
    if(layP2Sec == NULL) continue;  // Layer second part:
    calcShiftInSecSys(layP2Sec,posShifts[parn],layPosSh[iLay][parn+20]);
    calcShiftInSecSys(layP2Sec,negShifts[parn],layNegSh[iLay][parn+20]);
  }
  // Cell thickness:
  calcShiftInSecSys(laySec,posLThSh[layer],layPosSh[iLay][6]);
  calcShiftInSecSys(laySec,negLThSh[layer],layNegSh[iLay][6]);
  if(layP2Sec == NULL) return;
  // Layer second part:
  calcShiftInSecSys(layP2Sec,posLThSh[layer],layPosSh[iLay][6+20]);
  calcShiftInSecSys(layP2Sec,negLThSh[layer],layNegSh[iLay][6+20]);
}

void HMdcMille::setTrShiftInLabSys(void) {
  // secSys - sec.<->lab. transfomation
  // laySec - rotated_layer<->mdc transfomation
  HMdcSizesCells* pSCells = HMdcSizesCells::getObject();
  const HGeomTransform      *secSys   = (*pSCells)[iSec].getLabTrans();
  const HGeomTransform      *laySec   = pSCLayer[iLay]->getRotLayP1SysRSec();
  const HGeomTransform      *layP2Sec = NULL;
  if(nLayerParts[iLay] == 2) layP2Sec = pSCLayer[iLay]->getRotLayP2SysRSec();
  for(Int_t parn=0;parn<6;parn++) {
    calcShiftInLabSys(secSys,laySec,posShifts[parn],layPosSh[iLay][parn]);
    calcShiftInLabSys(secSys,laySec,negShifts[parn],layNegSh[iLay][parn]);
    if(layP2Sec == NULL) continue;
    calcShiftInLabSys(secSys,layP2Sec,posShifts[parn],layPosSh[iLay][parn+20]);
    calcShiftInLabSys(secSys,layP2Sec,negShifts[parn],layNegSh[iLay][parn+20]);
  }
  // Cell thickness:
  calcShiftInLabSys(secSys,laySec,posLThSh[layer],layPosSh[iLay][6]);
  calcShiftInLabSys(secSys,laySec,negLThSh[layer],layNegSh[iLay][6]);
  if(layP2Sec != NULL) {
    calcShiftInLabSys(secSys,layP2Sec,posLThSh[layer],layPosSh[iLay][6+20]);
    calcShiftInLabSys(secSys,layP2Sec,negLThSh[layer],layNegSh[iLay][6+20]);
  }
}

void HMdcMille::calcShiftInModSys(const HGeomTransform* mdcSys, const HGeomTransform* laySysMod,
    const HGeomTransform& shift, HGeomTransform& laySh) {
  // mdcSys    - mdc<->sec. transfomation
  // laySysMod - rotated_layer<->mdc transfomation
  // shift     - transfomation for shift of one global parameter
  // laySh     - new (shifted) rotated_layer<->sec transfomation
  laySh = *laySysMod;
  laySh.transFrom(shift); 
  laySh.transFrom(*mdcSys);
}

void HMdcMille::calcShiftInSecSys( const HGeomTransform* laySysSec, const HGeomTransform& shift,
    HGeomTransform& laySh) {
  // laySysSec - rotated_layer<->sec transfomation
  // shift     - transfomation for shift of one global parameter
  // laySh     - new (shifted) rotated_layer<->sec transfomation
  laySh = *laySysSec;
  laySh.transFrom(shift); // Do shift of layer<->sec. transfomation
}

void HMdcMille::calcShiftInLabSys(const HGeomTransform* secSys, const HGeomTransform* laySysSec,
    const HGeomTransform& shift, HGeomTransform& laySh) {
  // secSys    - lab.<->sec. transfomation
  // laySysSec - rotated_layer<->sec. transfomation
  // shift     - transfomation for shift of one global parameter
  // laySh     - new (shifted) rotated_layer<->sec transfomation
  laySh = *laySysSec;
  laySh.transFrom(*secSys);
  laySh.transFrom(shift);
  laySh.transTo(*secSys);
}

Bool_t HMdcMille::finalize(void) {
  if(mille) delete mille;
  mille = 0;
  creatSumOfShiftsFile();
  if(!writeParAsciiFile()) printErrorAndExit("finalize","Can't write geometry parameters ASCII file!!!");
  if(createPedeInFile) creatPedeInParamFile();
  if(crPedeTaskFile)   writePedeTaskFile();
  return kTRUE;
}

Int_t HMdcMille::execute(void) {
  if(HMdcTrackDSet::fPrint()) {
    fitpar.setPrintFlag(gHades->getEventCounter());
    if(fitpar.getPrintFlag()) printf(
      "============================== Event %i =============================\n",
      gHades->getEventCounter());
  }
  if(clusFitAlg==3) {
    if(filteringFlag>0) {
      if( !eventFilter() )   return kSkipEvent;
      if( filteringFlag==2 ) return 0;
    }
    fitAlgorithmForMille();
  } else if(clusFitAlg==4) fitAlgForMilleCosmic();
  return 0;
}

Bool_t HMdcMille::eventFilter(void) {
  // For beam data only!
  HMdcClus* pClst;
  UInt_t nClusters[6] = {0,0,0,0,0,0};
  iterClus->Reset();
  while((pClst=(HMdcClus*)iterClus->Next())) if(pClst->getIndexParent()<0) {
    UChar_t sec = pClst->getSec();
    nClusters[sec]++;
  }
  HMdcEvntListCells* pEvLCells = HMdcEvntListCells::getExObject();
  Bool_t isGoodSectors = kFALSE;
  for(Int_t s=0;s<6;s++) {
    useSector[s] = kFALSE;
    if(nMods[s] <= 1) continue;
    if(nClusters[s]>0 && nClusters[s]<=nClustersCut) {
      if(pEvLCells != NULL) {
        Int_t nCells  = (*pEvLCells)[s].getNCells();
        Int_t nWiresC = (nWiresMin*nMods[s])/4;
        if(nCells>=nWiresC && nCells<=nWiresCut[s]) {
          useSector[s]  = kTRUE;
          isGoodSectors = kTRUE;
        }
      } else {
        useSector[s]  = kTRUE;
        isGoodSectors = kTRUE;
      }
    }
  }
  return isGoodSectors;
}

void HMdcMille::fitAlgorithmForMille(void) {
  // Sector fit
  // Magnet off + combined clusters in sector
  HMdcClus* fClst1;
  iterClus->Reset();
  while((fClst1=(HMdcClus*)iterClus->Next())) if(fClst1->getIndexParent()<0) {
    UChar_t sec = fClst1->getSec();
    if(!useSector[sec]) continue;
    //--- Angle theta cut:
//!!!    if(thetaCut>0. && fClst1->getTheta()*TMath::RadToDeg()<thetaCut) continue;
//     Double_t theta = fClst1->getTheta()*TMath::RadToDeg();
//     Double_t phi   = fClst1->getPhi()*TMath::RadToDeg();
//     Int_t    sec   = fClst1->getSec();
/*
if(sec==1) {
  if(theta<35.) {
    if(phi>=108. && theta<35.) continue;
    if(phi>=102. && theta<25.) continue;
    if(phi>=90.  && theta<20.) continue;
  }
} else if(sec==2) {
  if(theta<30.) {
    if(phi>=96. && theta<30.) continue;
    if(phi>=78. && theta<25.) continue;
  }
} else if(sec==3) {
  if(phi>=96. && theta<25.) continue;
} else if(sec==4) {
  if(phi>=108. && theta<25.) continue;
}
*/    
    fittersArr[0].reset();
    Int_t first,last;
    fClst1->getIndexRegChilds(first,last);
    HMdcClus* fClst2 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
    // Num.wires test:
    Int_t nWr = fClst1->getNDrTimes();
    if(fClst2) nWr += fClst2->getNDrTimes();
    nWiresMinTr = (nWiresMin*nMods[sec])/4;
    if(nWr < nWiresMinTr) continue;
    
    Int_t nWiresInAlign = 0;
    Int_t mod = fClst1->getIOSeg()*2;
    if( isMdcInAlign[sec][mod]   ) nWiresInAlign += fClst1->getNDrTimesMod(0);
    if( isMdcInAlign[sec][mod+1] ) nWiresInAlign += fClst1->getNDrTimesMod(1);
    mod = fClst2->getIOSeg()*2;
    if( isMdcInAlign[sec][mod]   ) nWiresInAlign += fClst2->getNDrTimesMod(0);
    if( isMdcInAlign[sec][mod+1] ) nWiresInAlign += fClst2->getNDrTimesMod(1);
    if(nWiresInAlign < 6) continue;
    
    Bool_t flag=kFALSE;
    if(fClst2==0) {
      if(fClst1->getNDrTimesMod(0)==0 || fClst1->getNDrTimesMod(1)==0) continue;
      Int_t typeClFn = fClst1->getTypeClFinder();
      if(typeClFn==0)      flag = fitSeg(fClst1);       // Two mdc per segment
      else if(typeClFn==2) flag = fitMixedClus(fClst1);
      if(!flag) continue;
      fitter=fittersArr[0].getFitter(0); // ???
      if(fitter->getSegIndex()<0) continue;
      fillTrkCandISeg();
    } else {
      flag = fitSec(fClst1,fClst2);  //??????????????
    }
    if(flag && fitter->getFitStatus()) sendToMille();
  }
}

void HMdcMille::fitAlgForMilleCosmic(void) {
  // Two sector fit
  // Magnet off + combined clusters in sector
fitpar.setChi2PerNdfCut(100.);
  HMdcClus* fClst = NULL;
  iterClus->Reset();
  HMdcClus *fClstArr[6][2];     // [sector][0,1 -IOseg]
  Int_t nSegs[6];
  Int_t nMods[6];
  Int_t secList[4];
  Int_t nLaySec[6];
  Int_t &s1 = secList[0];
  Int_t &s2 = secList[1]; 
  Int_t &s3 = secList[2];
  Int_t &s4 = secList[3];
  // In cosmic one track per event only! First cluster finding:
  while((fClst=(HMdcClus*)iterClus->Next())) if(fClst->getIndexParent()<0) {
    Int_t nSectors  = 0;
    Int_t nSegments = 1;
    Int_t nModules  = 0;
    Int_t nWires    = 0;
    for(Int_t s=0;s<6;s++) {
      fClstArr[s][0] = NULL;
      fClstArr[s][1] = NULL;
      nSegs[s]       = 0;
      nMods[s]       = 0;
      nLaySec[s]     = 0;
    }
    Int_t nWiresInAlign = 0;
    while(kTRUE) {
      Int_t sec = fClst->getSec();
      Int_t seg = fClst->getIOSeg();
      if(fClstArr[sec][seg] != 0) {
        Error("fitAlgForMilleCosmic","Second time HMdcClus for sec.%i seg.%i",sec,seg);
        return;
      }
      Int_t mod = seg*2;
      if( isMdcInAlign[sec][mod]   ) nWiresInAlign += fClst->getNDrTimesMod(0);
      if( isMdcInAlign[sec][mod+1] ) nWiresInAlign += fClst->getNDrTimesMod(1);
      
      fClstArr[sec][seg] = fClst;
      if(nSegs[sec] == 0) {
        secList[nSectors] = sec;
        nSectors++;
      }
      nLaySec[sec] += fClst->getNLayers();
      nSegs[sec]++;
      nSegments++;
      Int_t nm = fClst->getActiveModule()<0;
      nMods[sec] += nm;
      nModules   += nm;
      nWires     += fClst->getNDrTimes();
      Int_t first,last;
      fClst->getIndexRegChilds(first,last);
      if(first < 0 || nSectors == 4) break;
      fClst = (HMdcClus*)fClusCat->getObject(first);
    }
    
    if(nWiresInAlign < 6) continue;
    
    for(Int_t s=0;s<6;s++) if(fClstArr[s][0]==NULL && fClstArr[s][1]!=NULL) {
      fClstArr[s][0] = fClstArr[s][1];
      fClstArr[s][1] = NULL;
    }
    if(nModules==1) continue;
    if(nModules>=4) nWiresMinTr = nWiresMin;
    else            nWiresMinTr = (nWiresMin*nModules)/4;

    if(nWires < nWiresMinTr) continue;  // Num.wires test
    
    
    fittersArr[0].reset();
    Bool_t flag=kFALSE;
    if(nSegments == 1) {
      if(nModules < 2) continue;
      Int_t typeClFn = fClst->getTypeClFinder();
      if(typeClFn==0)          flag = fitSeg(fClst);
      else if(typeClFn==2)     flag = fitMixedClus(fClst);
    } else {
      if     (nSectors == 1) flag = fitSec(fClstArr[s1][0],fClstArr[s1][1]);
      else if(nSectors == 2) flag = fit2Sectors(fClstArr[s1][0],fClstArr[s1][1],
                                                fClstArr[s2][0],fClstArr[s2][1]);
//       else if(nSectors > 2) { //???????????
// Int_t sec1 = 0;
// for(Int_t s=0;s<6;s++) if(s!=sec1 && nLaySec[s]>nLaySec[sec1]) sec1 = s;
// Int_t sec2 = sec1>0 ? 0 : 1;
// for(Int_t s=0;s<6;s++) if(s!=sec1 && s!=sec2 && nLaySec[s]>nLaySec[sec2]) sec2 = s;
// flag = fit2Sectors(fClstArr[sec1][0],fClstArr[sec1][1],fClstArr[sec2][0],fClstArr[sec2][1]);
//       } 
      else if(nSectors == 3) flag = fitNSectors(fClstArr[s1][0],fClstArr[s1][1],
                                                fClstArr[s2][0],fClstArr[s2][1],
                                                fClstArr[s3][0],fClstArr[s3][1],NULL,NULL);
      else if(nSectors == 4) flag = fitNSectors(fClstArr[s1][0],fClstArr[s1][1],
                                                fClstArr[s2][0],fClstArr[s2][1],
                                                fClstArr[s3][0],fClstArr[s3][1],
                                                fClstArr[s4][0],fClstArr[s4][1]);
      
      
      
    }
    
    if(flag && fitter->getFitStatus()) sendToMille(); 
    
//     Int_t first,last;
//     Int_t sec  = fClst1->getSec();
//     fClst1->getIndexRegChilds(first,last);
//     HMdcClus* fClst2 = (first<0) ? NULL : (HMdcClus*)fClusCat->getObject(first);
//     HMdcClus* fClst3 = NULL;
//     HMdcClus* fClst4 = NULL;
//     if(fClst2) {
//       if(sec == fClst2->getSec()) {
//         fClst2->getIndexRegChilds(first,last);
//         fClst3 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
//       } else {
//         fClst3 = fClst2;
//         fClst2 = NULL;
//       }
//       if(fClst3) {
//         fClst3->getIndexRegChilds(first,last);
//         fClst4 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
//       }
//     }
//     Int_t sec2 = -1;
//     if(fClst3)      sec2 = fClst3->getSec();
//     else if(fClst4) sec2 = fClst4->getSec();
// Int_t nModules = 0;
// nModules += fClst1->getActiveModule()<0 ? 2:1;
// if(fClst2) nModules += fClst2->getActiveModule()<0 ? 2:1;
// if(fClst3) nModules += fClst3->getActiveModule()<0 ? 2:1;
// if(fClst4) nModules += fClst4->getActiveModule()<0 ? 2:1;
// if(nModules==1) continue;
// if(nModules>=4) nWiresMinTr = nWiresMin;
// else            nWiresMinTr = (nWiresMin*nModules)/4;
// 
//     // Num.wires test:
//     Int_t nWr = fClst1->getNDrTimes();
//     if(fClst2) nWr += fClst2->getNDrTimes();
//     if(fClst3) nWr += fClst3->getNDrTimes();
//     if(fClst4) nWr += fClst4->getNDrTimes();
//     if(nWr < nWiresMinTr) continue;
// 
//     Bool_t flag=kFALSE;
//     if(fClst2==0 && fClst3==0 && fClst4==0) {
//       if(fClst1->getNDrTimesMod(0)==0 || fClst1->getNDrTimesMod(1)==0) continue;
//       Int_t typeClFn = fClst1->getTypeClFinder();
//       if(typeClFn==0)          flag = fitSeg(fClst1);
//       else if(typeClFn==2)     flag = fitMixedClus(fClst1);
//     } else {
//       if(fClst3==0&&fClst4==0) flag = fitSec(fClst1,fClst2);
//       else                     flag = fit2Sectors(fClst1,fClst2,fClst3,fClst4);
//     }
//     if(flag && fitter->getFitStatus()) sendToMille(); 
  }
}

void HMdcMille::sendToMille(void) {
  if(mille == 0) return;
  
  finalParam = fitter->getFinalParam();
  HMdcWiresArr& wiresArr = fitter->getWiresArr();
  Int_t nWiresData = wiresArr.getNDriftTimes();
  
  // test chi2 and ...:
  //  if(fitter->getMaxChi2()>9) return;
  
  // TEST TEST ???????????????!!!!!!!!!!!!!!!!!!!!!!!!!!!
  HMdcList24GroupCells& listCells = wiresArr.getOutputListCells();
  for(Int_t mod=0;mod<4;mod++) {
    Int_t nLayersMod = listCells.getNLayersMod(mod);
    if(nLayersMod == 1) return;
//???    if(mod==1 && nLayersMod < 4) return;
  }
  if(wiresArr.getSector2()>=0) {
    HMdcList24GroupCells &listCells2 = wiresArr.getOutputListCells2();
    for(Int_t mod=0;mod<4;mod++) if(listCells2.getNLayersMod(mod) == 1) return;
    if(wiresArr.getSector3()>=0) {
      HMdcList24GroupCells &listCells3 = wiresArr.getOutputListCells3();
      for(Int_t mod=0;mod<4;mod++) if(listCells3.getNLayersMod(mod) == 1) return;
      if(wiresArr.getSector4()>=0) {
       HMdcList24GroupCells &listCells4 = wiresArr.getOutputListCells4();
        for(Int_t mod=0;mod<4;mod++) if(listCells4.getNLayersMod(mod) == 1) return;
      }
    }
  }
  
  for(Int_t wInd=0;wInd<nWiresData;wInd++) {
    wireData = wiresArr.getWireData(wInd);
    if(wireData->getHitStatus() != 1) continue;
    if(!wireData->isInCell()) return;
  }
  

  p1.setXYZ(finalParam->x1(),finalParam->y1(),finalParam->z1());
  p2.setXYZ(finalParam->x2(),finalParam->y2(),finalParam->z2());
  
  Int_t nWires  = finalParam->getNumOfGoodWires();
  Int_t nLayers = wiresArr.getOutputNLayers();
  if(nWires < nWiresMinTr) return;
  
  // Filling histograms:
  if(doHists) {
    Int_t hstInd = histInd[wiresArr.getSector()];
    if(hstInd>=0) {
      ((TH2F*)fHistograms->At(hstInd))->Fill(finalParam->getPhiDeg(),finalParam->getThetaDeg());
      ((TH1F*)fHistograms->At(hstInd+1))->Fill(finalParam->getChi2());
      ((TH1F*)fHistograms->At(hstInd+2))->Fill(nWires+0.1);
      ((TH1F*)fHistograms->At(hstInd+3))->Fill(nLayers+0.1);
    }
  }
  
  // Derivatives calculation:
  if(fixFitTOffset) {
    for(Int_t wInd=0;wInd<nWiresData;wInd++) {
      wireData = wiresArr.getWireData(wInd);
      if(!wireData->getAnalytDeriv(locDer,0)) continue;
      int nGlobDer = getGlobalDer();
      mille->mille(4,locDer,nGlobDer,globDer,globLabel,-wireData->getDev(),wireData->getErrTdcTime());
    } // nWires
  } else {
    
    for(Int_t mi=0;mi<24;mi++) for(Int_t p=0;p<nParMax;p++) tofDerCorr[mi][p] = 0.;
    
    for(Int_t wInd=0;wInd<nWiresData;wInd++) {
      wireData = wiresArr.getWireData(wInd);
      if(wireData->getHitStatus() != 1) continue;
      setIModILay(); // calculate iMod and iLay + ...
      for(Int_t p=0;p<6;p++)         tofDerCorr[iMod][p] += calcDerTofCorr(p);
      if(mShFlag[sector][module][6]) tofDerCorr[iMod][6] += calcDerTofCorr(6);
    }
    
    for(Int_t wInd=0;wInd<nWiresData;wInd++) {
      wireData = wiresArr.getWireData(wInd);
      if(!wireData->getAnalytDeriv(locDer,finalParam)) continue;
      int nGlobDer = getGlobalDerWTof();
      mille->mille(4,locDer,nGlobDer,globDer,globLabel,
                   -wireData->getDev(),wireData->getErrTdcTime());
// if(module==0) printf("%iw.Glob: Der %g %g %g %g %g %g      %g %g\n",
// wInd,globDer[0],globDer[1],globDer[2],globDer[3],globDer[4],globDer[5],
// globDer[6],globDer[7]);
    } // nWires
  }
  nWiresTot  += nWires;
  nLayersTot += nLayers;
  nTracks++;
  
  milleFileSize += mille->end();
  if(milleFileSize > 1800000000) openNewBinaryFile();
}

void HMdcMille::openNewBinaryFile(void) {
  // New binary file:
  if(mille) delete mille;
  TString newFile(milleFileName);
  newFile += nBinaryFiles;
  mille = new Mille(newFile);
  milleFileSize = 0;
  nBinaryFiles++;
}

void HMdcMille::setIModILay(void) {
  sector = wireData->getSector();
  module = wireData->getModule();
  layer  = wireData->getLayer();
  setIModILay(sector,module,layer); // calculate iMod and iLay
  layPosShCurr = layPosSh[iLay];
  layNegShCurr = layNegSh[iLay];
  layerPart = pSCLayer[iLay]->getLayerPart(wireData->getCell());
  if(layerPart == 1) {
    layPosShCurr += 20;
    layNegShCurr += 20;
  }
}

Int_t HMdcMille::getGlobalDer(void) {
  setIModILay();
  Int_t nParam = 0;
//!!!  Int_t nMdcPar = mShFlag[sector][module][6] ? 7 : 6;
  Int_t nMdcPar = cellThicknFree ? 7 : 6;
  for(Int_t p=0;p<nMdcPar;p++) {
    globLabel[p] = packLabel(sector,module,-1,p); //!!!
    globDer[p]   = calcGlobDer(p);
    nParam++;
  }
  
//  Double_t cs[6]={0.766044,0.939693,1.,1.,0.939693,0.766044};
  if(shiftType == 1 && !useMdcShParOnly) {
    if(layerPart == 0 || !doLayP2Align) {  // Layer part 1
      globLabel[nParam] = packLabel(sector,module,layer,1); // in mdc sys
      globDer[nParam]   = globDer[1];                       // go to central wire number
      // ?      globDer[nParam]   = globDer[1]*cs[layer];
      nParam++;
      // ?     globLabel[nParam] = packLabel(sector,module,layer,2);
      // ?     globDer[nParam]   = globDer[2];
      // ?     nParam++;
      globLabel[nParam] = packLabel(sector,module,layer,5); // in mdc sys
      globDer[nParam]   = globDer[5];                       // go to the wire orientation
      nParam++;
    } else if(layerPart == 1) {  // Layer part 2 (it can be in mdc3 & 4 only)
      globLabel[nParam] = packLabel(sector,module,layer,7); // in mdc sys
      globDer[nParam]   = globDer[1];                       // go to central wire number
      nParam++;
      globLabel[nParam] = packLabel(sector,module,layer,8); // in mdc sys
      globDer[nParam]   = globDer[5];                       // go to the wire orientation
      nParam++;
    }
  }

//printf("0Glob: Der %.3g %.3g %.3g %.3g %.3g %.3g %.3g %.3g\n",
//globDer[0],globDer[1],globDer[2],globDer[3],globDer[4],globDer[5],globDer[6],globDer[7]);
  return nParam;
}

Int_t HMdcMille::getGlobalDerWTof(void) {
  setIModILay();
//!!!  Int_t nMdcPar = mShFlag[sector][module][6] ? 7 : 6;
  Int_t nMdcPar = cellThicknFree ? 7 : 6;
  Int_t nParam  = nMdcPar;
  for(Int_t p=0;p<nMdcPar;p++) {
    globLabel[p] = packLabel(sector,module,-1,p);
    Double_t dTm;
    Double_t corr = calcDerTofCorr(p,&dTm);
    globDer[p] = (dTm + tofDerCorr[iMod][p])*derNorm[p];
    if(useMdcShParOnly || shiftType!=1 || (p!=1 && p!=5) ) continue;
    // For HMdcLayerGeomPar:

    Int_t pInd = p==1 ? nMdcPar : nMdcPar+1;
    Int_t po   = p;
    if(layerPart==1 && doLayP2Align) {
      po    = p==1 ? 7 : 8;
      pInd += 2;
    }
    globLabel[pInd] = packLabel(sector,module,layer,po);
    globDer[pInd]   = (dTm + corr)*derNorm[p];
    nParam++;   
  }
  return nParam;
}

Double_t HMdcMille::calcDerTofCorr(Int_t p,Double_t* dDrTm) {
  Double_t   dDrT      = calcDDriftTime(p);
  if(dDrTm) *dDrTm     = dDrT;
  Double_t   wtNorm    = wireData->getWtNorm();
  Double_t   sumWtNorm = finalParam->getSumWtNorm(wireData->getModIndex());
  if(sumWtNorm > 0.) return -dDrT*wtNorm/sumWtNorm;
  return 0.;
}

int HMdcMille::packLabel(Int_t s,Int_t m,Int_t l,Int_t parn) {
  // if lay<0 - it is shift of mdc, otherwise shift of layer
  if(l<0) l = -1;
  if(createPedeInFile || !isGeomChanged) {
    if(l<0) mShFlag[s][m][parn]    = kTRUE;
    else    lShFlag[s][m][l][parn] = kTRUE;
  }
  int label = (((shiftType*10 + s+1)*10 + m+1)*10 + l+1)*10 + parn+1;
  if(beamTimeId>0 && l<0 && parn!=6) label += beamTimeId*100000;
  return label;
}

Bool_t HMdcMille::unpackLabel(Int_t label,Int_t& btId,Int_t& sys,
    Int_t& sec,Int_t& mod,Int_t& lay,Int_t& parn) {
  // shSys = 0 shift in respect sector system
  // shSys = 1 shift in respect mdc system
  // if lay==-1 - it is shift of mdc, otherwise shift of layer
  // parn = 0 - shift along X
  // parn = 1 - shift along Y
  // parn = 2 - shift along Z
  // parn = 3 - rotation around X
  // parn = 4 - rotation around Y
  // parn = 5 - rotation around Z
  // parn = 6 - cell thickness
  // parn = 7 - shift along Y for second layer part
  // parn = 8 - rotation around Z for second layer part
  btId = label/100000;              // beam time Id
  sys  = (label%100000)/10000;
  sec  = (label%10000)/1000 - 1;
  mod  = (label%1000)/100   - 1;
  lay  = (label%100)/10     - 1;
  parn = (label%10)         - 1;
  if(sec<0||sec>5 || mod<0||mod>3 || lay<-1||lay>5 || parn<0) return kFALSE;
  return kTRUE;
}

Double_t HMdcMille::calcGlobDer(Int_t parNum) {
  Double_t dDrT = calcDDriftTime(parNum);
  if(parNum==6) return dDrT*derLThNorm[layer];
  else          return dDrT*derNorm[parNum];    // derNorm=1/parSteps
}

Double_t HMdcMille::calcDDriftTime(Int_t p) {
  Int_t    dSign1  = {0};
  Int_t    dSign2  = {0};
  Double_t drTime1 = calcDriftTime(layPosShCurr[p],dSign1);
  Double_t drTime2 = calcDriftTime(layNegShCurr[p],dSign2);
  if(dSign1 == dSign2) return (drTime1-drTime2)*0.5; 
  Int_t dSignT = wireData->getDistanceSign();
  if(dSignT==dSign1)   return drTime1 - wireData->getDrTime();
                       return wireData->getDrTime() - drTime2;   // dSignT==dSign2
}

Double_t HMdcMille::calcDriftTime(const HGeomTransform& laySys,Int_t& distSign) {
  HGeomVector p1l,p2l;
  Int_t parSec = finalParam->getSec();
  if(parSec==sector || parSec<0) {
    p1l = p1;
    p2l = p2;
  } else {
    HMdcSizesCells* pSCells = HMdcSizesCells::getObject();
    pSCells->transLineToOtherSec(*finalParam,sector,p1l,p2l);
  }
  p1l = laySys.transTo(p1l);
  p2l = laySys.transTo(p2l);
  return wireData->calcDriftTimeForAlign(p1l,p2l,distSign);
}

void HMdcMille::setPedeInFileName(const char* fname) {
  pedeInParFName = fname;
  if(pedeInParFName.Length() > 0) createPedeInFile = kTRUE;
  else Error("setPedeInFileName","File name is not specified!!!");
}

void HMdcMille::creatPedeInParamFile(void) {
  FILE* file = fopen(pedeInParFName.Data(),"r");
  if(file != NULL) {
    printf("******CreatePedeInParamFile******\n");
    printf("* File %s exist!\n",pedeInParFName.Data());
    printf("* If you want to change this file\n");
    printf("* do it by hand or delete file.\n");
    printf("*********************************\n");
    fclose(file);
    return;
  }
  file = fopen(pedeInParFName.Data(),"w");
  if(file == NULL) Error("creatPedeInParamFile","Can't open file %s",pedeInParFName.Data());
  else {
    TString sys;
    if(shiftType == 0) sys = "All shifts done in sector sys.";
    if(shiftType == 1) sys = "All shifts done in mdc sys.";
    if(shiftType == 2) sys = "All shifts done in lab. sys.";
    fprintf(file," Parameter   !  %s\n",sys.Data());
    for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=-1;l<6;l++) {
      if(l>=0 && useMdcShParOnly) continue;
      for(Int_t p=0;p<nParMax;p++) {
        if(l<0 && !mShFlag[s][m][p]) continue;
        if(l>=0 && !lShFlag[s][m][l][p]) continue;
        if( (l<0 && mFixFlag[s][m][p]) || (l>=0 && lFixFlag[s][m][l][p]) )
             fprintf(file,"%11i     0.0   -1.0000   !",packLabel(s,m,l,p));
        else fprintf(file,"%11i     0.0    0.0000   !",packLabel(s,m,l,p));

        if(l<0) fprintf(file," %is. %im.: mdc",s+1,m+1);
        else  {
          fprintf(file," %is. %im. %il.: layer",s+1,m+1,l+1);
          if(p==7 || p==8) fprintf(file," part II");
        }
        fprintf(file," %s\n",shitsInfo[p].Data());        // Print description
      }
    }
    fclose(file);
  }
}

Bool_t HMdcMille::loadGeometryPar(void) {
  HRuntimeDb* rtdb    = gHades->getRuntimeDb();
  if(rtdb == 0) return kFALSE;
  HParAsciiFileIo* inputFile = new HParAsciiFileIo;
  if(!inputFile->open(const_cast<char*>(geomParFileName.Data()),"in")) return kTRUE; // No file!
  HMdcGeomStruct*  pMdcGeomStruct = (HMdcGeomStruct*)rtdb->getContainer("MdcGeomStruct");
  if( !((HParSet*)pMdcGeomStruct)->init(inputFile) ) return kFALSE;
  pMdcGeomStruct->setStatic();
  pMdcGeomStruct->setInputVersion(1,1);
      
  HSpecGeomPar*     pSpecGeomPar = (HSpecGeomPar*)rtdb->getContainer("SpecGeomPar");
  HMdcGeomPar*      pMdcGeomPar  = (HMdcGeomPar*)rtdb->getContainer("MdcGeomPar");  
  HMdcLayerGeomPar* pMdcLGeomPar = (HMdcLayerGeomPar*)rtdb->getContainer("MdcLayerGeomPar");
  HMdcLayerCorrPar* pMdcLCorrPar = (HMdcLayerCorrPar*)rtdb->getContainer("MdcLayerCorrPar");
  
  if( !((HParSet*)pSpecGeomPar)->init(inputFile) ||
      !((HParSet*)pMdcGeomPar )->init(inputFile) ||
      !((HParSet*)pMdcLGeomPar)->init(inputFile) ||
      !((HParSet*)pMdcLCorrPar)->init(inputFile)) return kFALSE;
    
  pSpecGeomPar->setInputVersion(1,1);
  pMdcGeomPar ->setInputVersion(1,1);
  pMdcLGeomPar->setInputVersion(1,1);
  pMdcLCorrPar->setInputVersion(1,1);
  
  pSpecGeomPar->setStatic();
  pMdcGeomPar ->setStatic();
  pMdcLGeomPar->setStatic();
  pMdcLCorrPar->setStatic();
  
  inputFile->close();
  delete inputFile;
  isGeomFileExist = kTRUE;
  return kTRUE;
}

void HMdcMille::readPedeResFile(const char* fileName) {
  if(fileName == 0) printErrorAndExit("readPedeResFile","File name of Pede results not specified!");
  // Open file:
  FILE* file = fopen(fileName,"r");
  if(file == NULL) {
    if(iteration==0) {
      for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t p=0;p<9;p++) mShFlag[s][m][p] = kTRUE;
      return;
    }
    printErrorAndExit("readPedeResFile","No pede result file!");
  }

  // Read file:
  TString buffer;
  if( !buffer.Gets(file) ) printErrorAndExit("readPedeResFile","Wrong format of file %s",fileName);
  Ssiz_t pos = buffer.Index("Parameter");
  if(pos<0 || pos>10)      printErrorAndExit("readPedeResFile","Wrong format of file %s",fileName);
  while(buffer.Gets(file) && buffer.Length()>16 && addShifts(buffer));

  fclose(file);
}

Bool_t HMdcMille::makeShifts(void) {
  if( !isGeomChanged ) return kTRUE;
  if( !isGeomFileExist ) return kFALSE;
  HMdcLayerGeomPar* pMdcLGeomPar = pGetCont->getMdcLayerGeomPar();
  HMdcLayerCorrPar* pMdcLCorrPar = pGetCont->getMdcLayerCorrPar();
  Bool_t reCalcLayerTransf = kFALSE;
  HGeomTransform shift;
  HGeomTransform trans;
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) {
    if(pMdcDet->getModule(s,m) == 0) continue;
    HMdcLayerGeomParMod& fLayerParMod  = (*pMdcLGeomPar)[s][m];
    if(!useMdcShParOnly) for(Int_t l=0;l<6;l++) {
      if(shiftSysLay[s][m][l] != 1 ) continue; //!!!!!!!!!!!
      // Tol'ko dlya Y seychas!!!!!!!!!!!!!!!!!!!!!!!!
//V kakoy posledovatel'nosti????
      HMdcLayerGeomParLay& fLayerParLay = fLayerParMod[l];
      Double_t pitch     = fLayerParLay.getPitch();
      Double_t centWrOld = fLayerParLay.getCentralWireNr();
      Double_t wireOrOld = fLayerParLay.getWireOrient();
      Double_t wireOrNew = wireOrOld + shiftsLay[s][m][l][5];             // Layer rotation
      Double_t wireOrR   = wireOrNew*TMath::DegToRad();
      Double_t layShift  = shiftsLay[s][m][l][1]*TMath::Cos(wireOrR) -
                           shiftsLay[s][m][l][0]*TMath::Sin(wireOrR);     // Layer shift
      Double_t centWrNew = centWrOld - layShift/pitch;
      fLayerParLay.setWireOrient(wireOrNew);
      fLayerParLay.setCentralWireNr(centWrNew);

      //  ------- Layer second part: ------- 
      if(m<2 || !doLayP2Align) continue;
      Int_t   firstWrPII;
      Float_t layShiftPIIold, wrOrCorrPIIold;
      if(!pMdcLCorrPar->getLayerCorrPar(s,m,l, firstWrPII,layShiftPIIold,wrOrCorrPIIold)) continue;
      if(firstWrPII>300) pMdcLCorrPar->addLayerShift(s,m,l,firstWrPII,0.,0.);
      else {
//         Double_t wireOrPIIold   = wireOrOld - wrOrCorrPIIold;  
//         Double_t wireOrPIInew   = wireOrPIIold + shiftsLay[s][m][l][8];       // Layer PII rotation
//         
//         Double_t yShiftPIIold   = layShiftPIIold/TMath::Cos(wireOrPIIold);
//         Double_t yShiftPIInew   = yShiftPIIold  + shiftsLay[s][m][l][7];      // Layer PII y-shift
//         
//         Float_t  wrOrCorrPIInew = wireOrNew - wireOrPIInew;
//         Float_t  layShiftPIInew = yShiftPIInew*TMath::Cos(wireOrPIInew*TMath::DegToRad()) - layShift;
        Float_t  wrOrCorrPIInew = wrOrCorrPIIold + shiftsLay[s][m][l][5] - shiftsLay[s][m][l][8];
        Double_t cosWrOrPIInew  = TMath::Cos((wireOrNew - wrOrCorrPIInew)*TMath::DegToRad());
        Float_t  layShiftPIInew = layShiftPIIold + shiftsLay[s][m][l][7]*cosWrOrPIInew - layShift;
  
        pMdcLCorrPar->addLayerShift(s,m,l,firstWrPII,layShiftPIInew,wrOrCorrPIInew);
      }
    }
    if(shiftSysMdc[s][m]<0) continue;
    // Cell thickness:
    if(shiftsMdc[s][m][6] != 0.) {
      for(Int_t l=0;l<6;l++) {
        HMdcLayerGeomParLay& fLayerParLay  = fLayerParMod[l];
        Float_t catDist = fLayerParLay.getCatDist();
        fLayerParLay.setCatDist(catDist + shiftsMdc[s][m][6]);
      }
      reCalcLayerTransf = kTRUE;
    }
    
    // Shift MDC:
    HMdcSizesCells::setTransform(shiftsMdc[s][m],shift); //first 6 par. are used
    HModGeomPar* fMod = pGetCont->getModGeomPar(s,m);
    if(shiftSysMdc[s][m] == 0) {            // shifts in sector coor.system
      if( !pGetCont->getSecTransMod(trans,s,m) ) return kFALSE;
      trans.transFrom(shift);
      trans.transFrom(pGetCont->getLabTransSec(s));
      fMod->getLabTransform() = trans;
    } else if(shiftSysMdc[s][m] == 1) {     // shifts in mdc coor.system
      if(!pGetCont->getLabTransMod(trans,s,m)) return kFALSE;
      shift.transFrom(trans);
      fMod->getLabTransform() = shift;
    } else if(shiftSysMdc[s][m] == 2) {     // shifts in lab.coor.system
      if( !pGetCont->getLabTransMod(trans,s,m) ) return kFALSE;
      trans.transFrom(shift);
      fMod->getLabTransform() = trans;
    }
  }
  if(reCalcLayerTransf) pMdcLGeomPar->calcLayerTransformations();
  if(isGeomChanged) printf("\nMDC geometry has changed !!!\n\n");
  return kTRUE;
}

Bool_t HMdcMille::writeParAsciiFile(void) {
  TString outGeomFileName(geomParFileName + ".last");
  HParAsciiFileIo* outputFile = new HParAsciiFileIo;
  if( !outputFile->open(const_cast<char*>(outGeomFileName.Data()),"out") )
    return kFALSE;
  HRuntimeDb* rtdb    = gHades->getRuntimeDb();
  if(rtdb == 0) return kFALSE;
  if(rtdb->getContainer("MdcGeomStruct")->write(outputFile)<0)   return kFALSE; 
  if(rtdb->getContainer("SpecGeomPar")->write(outputFile)<0)     return kFALSE;
  if(rtdb->getContainer("MdcGeomPar")->write(outputFile)<0)      return kFALSE;
  if(rtdb->getContainer("MdcLayerGeomPar")->write(outputFile)<0) return kFALSE;
  if(rtdb->getContainer("MdcLayerCorrPar")->write(outputFile)<0) return kFALSE;
//  if(rtdb->getContainer("MdcCalParRaw")->write(outputFile)<0)    return kFALSE;  
  outputFile->close();
  delete outputFile;
  if(doCopyGeomFile) copyFile("cp",outGeomFileName);
  if(!isGeomFileExist && !isGeomChanged) {
    TString cpFile("cp "+outGeomFileName+" "+geomParFileName);
    gSystem->Exec(cpFile);
  }
  return kTRUE;
}

void HMdcMille::fixFullMod(Int_t s,Int_t m) {
  for(Int_t p=0;p<7;p++) fixModPar(s,m,p);
}

void HMdcMille::fixModPar(Int_t s,Int_t m,Int_t p) {
  if(s>=0&&s<6 && m>=0&&m<4 && p>=0&&p<7) {
    mFixFlag[s][m][p] = kTRUE;
    if(sigmaMdc[s][m][p] == 0.) sigmaMdc[s][m][p] = -1.;
  }
}

void HMdcMille::fixFullLay(Int_t s,Int_t m,Int_t l) {
  for(Int_t p=0;p<nParMax;p++) fixLayPar(s,m,l,p);
}

void HMdcMille::fixLayPar(Int_t s,Int_t m,Int_t l,Int_t p) {
  if(s<0 || s>5 || m<0 || m>3 || l<0 || l>5 || p<0 || p>8) return;
  lFixFlag[s][m][l][p] = kTRUE;
  if(sigmaLay[s][m][l][p] == 0.) sigmaLay[s][m][l][p] = -1.;
}

void HMdcMille::fixPar(Int_t s,Int_t m,Int_t l,Int_t p) {
  if(l<0) {
    if(p<0) fixFullMod(s,m);
    else    fixModPar(s,m,p);
  } else {
    if(p<0) fixFullLay(s,m,l);
    else    fixLayPar(s,m,l,p);
  }
}

void HMdcMille::readSumOfShiftsFile(void) {
  // Open file:
  FILE* file = fopen(sumOfShiftsFName.Data(),"r");
  if(file == NULL) return;

  // Read file:
  TString buffer;
  if( !buffer.Gets(file) ) printErrorAndExit("readSumOfShiftsFile",
      "Wrong format of file %s",sumOfShiftsFName.Data()); // Stop!
  Ssiz_t pos = buffer.Index("SumOfShifts:");
  if(pos<0 || pos>3) printErrorAndExit("readSumOfShiftsFile",
      "Wrong format of file %s",sumOfShiftsFName.Data()); // Stop!
  
  Int_t sys;
  Int_t n = sscanf(buffer.Data(),"%*s %i %*s %*s %i",&iteration,&sys);              
  if(n!=2 || iteration<0) printErrorAndExit("readSumOfShiftsFile",
      "Wrong format of file %s",sumOfShiftsFName.Data()); // Stop!
  iteration++;
  if( !buffer.Gets(file) ) printErrorAndExit("readSumOfShiftsFile",
      "Wrong format of file %s",sumOfShiftsFName.Data()); // Stop!
  pos = buffer.Index("Statistics:");
  if(pos<0 || pos>3) printErrorAndExit("readSumOfShiftsFile",
      "Wrong format of file %s",sumOfShiftsFName.Data()); // Stop!
  
  while(buffer.Gets(file) && buffer.Length()>16 && addShifts(buffer));
  
  fclose(file);
}

Bool_t HMdcMille::addShifts(TString& buffer) {
  TObjArray* arr = buffer.Tokenize(" ");
  Bool_t exitFlag = kFALSE;
  if(arr->GetEntries()>=3) {
    Int_t    label = ((TObjString*)(arr->At(0)))->GetString().Atoi();
    Double_t shift = ((TObjString*)(arr->At(1)))->GetString().Atof();
    Double_t coef  = ((TObjString*)(arr->At(2)))->GetString().Atof();
    Int_t btId,sys,sec,mod,lay,parn;
    if(!unpackLabel(label,btId,sys,sec,mod,lay,parn))
        printErrorAndExit("addShifts","Wrong format of input file!!! Stop!");
    exitFlag = kTRUE;
    if((btId == 0 || btId == beamTimeId) && pMdcDet->getModule(sec,mod)) {
      mdcMods[sec][mod] = 1;
      if(lay < 0) {
        shiftsMdc[sec][mod][parn]             += shift;
        if(coef >= 0.)  shiftSysMdc[sec][mod]  = sys; // Otherwise = -1
        if(shift != 0.) isGeomChanged          = kTRUE;
        mShFlag[sec][mod][parn]                = kTRUE;
        sigmaMdc[sec][mod][parn]               = coef;
      } else {
        shiftsLay[sec][mod][lay][parn]             += shift;
        if(coef >= 0.)  shiftSysLay[sec][mod][lay]  = sys;
        if(shift != 0.) isGeomChanged               = kTRUE;
        lShFlag[sec][mod][lay][parn]                = kTRUE;
        sigmaLay[sec][mod][lay][parn]               = coef;
      }
    }
  }
  arr->Delete();
  delete arr;
  return exitFlag;
}

void HMdcMille::printErrorAndExit(const char* func,const char* form,const char* str) {
#warning the following warning because of string literal can be ignored!
  if(str == NULL) Error(func,form);
  else            Error(func,form,str);
  exit(1);
}

void HMdcMille::creatSumOfShiftsFile(void) {
  FILE* file = fopen(sumOfShiftsFName.Data(),"w");
  if(file == NULL) printErrorAndExit("creatSumOfShiftsFile",
                                     "Can't open file %s",sumOfShiftsFName.Data());
  TString sys;
  if(shiftType == 0) sys = "all shifts done in sector sys.";
  if(shiftType == 1) sys = "all shifts done in mdc sys.";
  if(shiftType == 2) sys = "all shifts done in lab. sys.";
  Double_t mNWr = Double_t(nWiresTot)/nTracks;
  Double_t mNLy = Double_t(nLayersTot)/nTracks;
  fprintf(file,"SumOfShifts: %i iteration.  System %i - %s\n",iteration,shiftType,sys.Data());
  fprintf(file,"Statistics: %i tracks; %8.4f <wires/tr.>; %8.4f <layers/tr.>\n",nTracks,mNWr,mNLy);
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=-1;l<6;l++) {
    if(l>=0 && useMdcShParOnly) continue;
    for(Int_t p=0;p<nParMax;p++) {
      if(l<0  && !mShFlag[s][m][p])    continue;
      if(l>=0 && !lShFlag[s][m][l][p]) continue;

      fprintf(file,"%11i",packLabel(s,m,l,p));             // Print label
      if(l < 0) fprintf(file,"  %18.10e  %9.4e  ! %is. %im.: mdc",
                             shiftsMdc[s][m][p],sigmaMdc[s][m][p],s+1,m+1);
      else {
        fprintf(file,"  %18.10e  %9.4e  ! %is. %im. %il.: layer",
                     shiftsLay[s][m][l][p],sigmaLay[s][m][l][p],s+1,m+1,l+1);
        if(p==7 || p==8) fprintf(file," part II");
      }
      fprintf(file," %s\n",shitsInfo[p].Data());            // Print description
    }
  }
  fclose(file);
  if(doCopySumShFile) copyFile("cp",sumOfShiftsFName);
  if(doCopyResFile)   copyFile("cp",pedeResFName,".res");
  if(doCopyHisFile)   copyFile("mv",pedeResFName,".his");
  if(doCopyLogFile)   copyFile("mv",pedeResFName,".log");
}

void HMdcMille::copyFile(const char* op,TString& file,const char* ext) {
  TString cpFile(op);
  if(ext) cpFile += " "+file+ext+" "+file+ext+stepIter;
  else    cpFile += " "+file+" "+file+stepIter;
  gSystem->Exec(cpFile);
}

void HMdcMille::creatPedeTaskFile(const char* fileName) {
  pedeTaskFileName = fileName;
  if(pedeTaskFileName.Length()<=0) printErrorAndExit("creatPedeTaskFile",
      "File name of pede task list is not specified!");
  crPedeTaskFile = kTRUE;
  // Set default method:
  method         = "inversion       ";
  numOfIter      = 3;
  accuracy       = 0.001;
  mthDescr       = "Gauss matrix inversion";
  bandwidth      = -1;
}

void HMdcMille::useInversionMethod(Int_t nit,Float_t acc) {
  method    = "inversion       ";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "Gauss matrix inversion";
  bandwidth = -1;
}

void HMdcMille::useBandCholeskyMethod(Int_t nit,Float_t acc,Int_t bwidth) {
  method    = "bandcholesky   ";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "Cholesky";
  bandwidth = bwidth;
}

void HMdcMille::useCholeskyMethod(Int_t nit,Float_t acc,Int_t bwidth) {
  method    = "cholesky       ";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "Cholesky";
  bandwidth = bwidth;
}

void HMdcMille::useSparseGMRESMethod(Int_t nit,Float_t acc) {
  method    = "sparseGMRES    ";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "minimal residual";
  bandwidth = -1;
}

void HMdcMille::useFullGMRESMethod(Int_t nit,Float_t acc) {
  method    = "fullGMRES      ";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "minimal residual";
  bandwidth = -1;
}

void HMdcMille::useDiagonalizationMethod(Int_t nit,Float_t acc) {
  method    = "diagonalization";
  numOfIter = nit;
  accuracy  = acc;
  mthDescr  = "diagonalization";
  bandwidth = -1;
}

void HMdcMille::setParConstrainFile(const char* file) {
  parConstrainFile = file;
}

void HMdcMille::writePedeTaskFile(void) {
  if(!crPedeTaskFile) return;
  if(pedeInParFName.Length() <= 0) printErrorAndExit("writePedeTaskFile",
    "File name of parameters list not specified!");
  FILE* file = fopen(pedeTaskFileName.Data(),"w");
  if(file == NULL) printErrorAndExit("writePedeTaskFile","Can't open file %s",pedeTaskFileName.Data());
  fprintf(file,"Cfiles   !\n");
  
  if(parConstrainFile.Length() > 0) 
       fprintf(file,"%s   ! constraints text file\n",parConstrainFile.Data());
  else fprintf(file,"!parConstrain.txt           ! constraints text file\n");
  
  fprintf(file,"%s   !  parameter text file\n",pedeInParFName.Data());
  
  for(Int_t nf=0;nf<nBinaryFiles;nf++) fprintf(file,
      "%s%i    ! binary data file\n",milleFileName.Data(),nf);
  
  if(bandwidth > 0) fprintf(file,"bandwidth %i ! width of band matrix\n",bandwidth);
  fprintf(file,"method %s %i %g  ! %s\n",method.Data(),numOfIter,accuracy,mthDescr.Data());
  fclose(file);
}

void HMdcMille::setMaxNumWiresCut(Int_t* wc) {
  for(Int_t s=0;s<6;s++) nWiresCut[s] = wc[s];
}

void HMdcMille::setMaxNumWiresCut(Int_t cs1,Int_t cs2,Int_t cs3,Int_t cs4,Int_t cs5,Int_t cs6) {
  nWiresCut[0] = cs1;
  nWiresCut[1] = cs2;
  nWiresCut[2] = cs3;
  nWiresCut[3] = cs4;
  nWiresCut[4] = cs5;
  nWiresCut[5] = cs6;
}

void HMdcMille::setMdcShiftParOnly(Bool_t fl) {
  useMdcShParOnly = fl;
  if(useMdcShParOnly) {
    for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t p=6;p<9;p++) mShFlag[s][m][p] = kFALSE;
  }
}
