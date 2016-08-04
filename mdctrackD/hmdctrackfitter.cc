//*--- Author : G.Agakichiev
//*--- Modified: 23.01.07 V.Pechenov
//*--- Modified: 16.06.2005 by V.Pechenov
//*--- Modified: 21.07.2003 by V.Pechenov
//*--- Modified: 28.07.2002 by V.Pechenov
//*--- Modified: 07.05.2002 by V.Pechenov

using namespace std;
#include "hmdctrackfitter.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hmdctrackfitpar.h"
#include "hruntimedb.h"
#include "hmdcsizescells.h"
#include "hgeomvector.h"
#include "hmdcgetcontainers.h"
#include "hmdclistcells.h"
#include "hmdcwirefitsim.h"
#include "hmdcclusfitsim.h"
#include "hmdcclussim.h"
#include "hmdctrackdset.h"
#include "hmdcdigitpar.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// 
// HMdcTrackFitInOut
//
// Service class for for Dubna track piece fitters 
//
//
// HMdcTrackFitter
//
// Base class for Dubna track piece fitters
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcTrackFitInOut)
ClassImp(HMdcTrackFitter)

HMdcTrackFitInOut::HMdcTrackFitInOut(void) {
  fSizesCells=0;
  setDefaultFitParam();
  useRndbPar = kTRUE;
}

Bool_t HMdcTrackFitInOut::init(void) {
  // parameters:
  version=HMdcTrackDSet::getFitVersion();
  if(version==0) {
    Warning("init","track fit version 0 is not supported more, version 1 will used");
    version=1;
  }
  if(!useRndbPar) fitPar = 0;
  else            fitPar = (HMdcTrackFitPar*)gHades->getRuntimeDb()->getContainer("MdcTrackFitPar");
  fprint = HMdcTrackDSet::fPrint();
  HMdcGetContainers* fGetCont = HMdcGetContainers::getObject();

  fDriftTimePar = HMdcDriftTimePar::getObject();
  HMdcWireData::setDriftTimePar(fDriftTimePar);
  wireOffsetFlag=HMdcTrackDSet::getUseWireOffset();
  if(wireOffsetFlag) {
    fDigitPar=(HMdcDigitPar*)gHades->getRuntimeDb()->getContainer("MdcDigitPar");
    if(!fDigitPar) {
      Error("init:","Zero pointer for HMdcDigitPar recieved!");
      return kFALSE;
    }
  } else fDigitPar=0;
  
  fSizesCells=HMdcSizesCells::getObject();
  if (!fSizesCells) {
    Error("init","HMdcSizesCells is absent");
    return kFALSE;
  }
  
  // categoryes:
  geantFlag=HMdcGetContainers::isGeant();
  if(geantFlag && HMdcTrackDSet::fNTuple()) {
    fGeantKineCat = fGetCont->getCatGeantKine();
    fGeantMdcCat  = fGetCont->getCatGeantMdc();
  } else {
    fGeantKineCat = 0;
    fGeantMdcCat  = 0;
  }
  fCalCat = fGetCont->getCatMdcCal1();
  if (!fCalCat) return kFALSE;
  if(HMdcTrackDSet::fNTuple()) {
    fClusFitCat = fGetCont->getCatMdcClusFit(kTRUE);
    fWireFitCat = fGetCont->getCatMdcWireFit(kTRUE);
    if(!fClusFitCat || !fWireFitCat) return kFALSE;
  } else {
    fClusFitCat = 0;
    fWireFitCat = 0;
  }
  locClusFit.set(1,0);
  locWireFit.set(1,0);
  loc.set(4,0,0,0,0);
  return kTRUE;
}

Bool_t HMdcTrackFitInOut::reinit(void) {
  if(!fSizesCells->initContainer())   return kFALSE;
  if(!fDriftTimePar->initContainer()) return kFALSE;
  for(Int_t sec=0; sec<6; sec++) {
    HMdcSizesCellsSec& fSCSec=(*fSizesCells)[sec];
    for(Int_t mod=0; mod<4; mod++)
        fSCModAr[sec][mod]=(fSizesCells->modStatus(sec,mod)) ? &fSCSec[mod] : 0;
  }
  if( fitPar ) {
    if( !HMdcGetContainers::isInited(fitPar) ) return kFALSE;
    else {
      if(useRtdbTofFlag) tofFlag = fitPar->getTofFlag();
      cutWeight     = fitPar->getCutWeight     ();
      if(useRtdbTScFlag) doTargScan = fitPar->getDoTargScan();

      minTimeOffset = fitPar->getMinTimeOffset ();
      maxTimeOffset = fitPar->getMaxTimeOffset ();
      minCellsNum   = fitPar->getMinCellsNum   ();
      chi2CutFlag   = fitPar->getChi2CutFlag   ();
      totalChi2Cut  = fitPar->getTotalChi2Cut  ();
      chi2PerNdfCut = fitPar->getChi2PerNdfCut ();

      if(useRtdbTFlag) useTukeyFlag = fitPar->getUseTukeyFlag();
      cnWs          = fitPar->getCnWs          ();
      cn2s          = fitPar->getCn2s          ();
      cn4s          = fitPar->getCn4s          ();
      cn2s          = cn4s*cn4s/cnWs;               //No need to keep it RTDB
      minSig2       = fitPar->getMinSig2       ();
      maxNFilterIter= fitPar->getMaxNFilterIter();
      minWeight     = fitPar->getMinWeight     ();
      maxChi2       = fitPar->getMaxChi2       ();

      minTOffsetIter= fitPar->getMinTOffsetIter();

      funCt1        = fitPar->getFunCt1        ();
      stepD1        = fitPar->getStepD1        ();
      funCt2        = fitPar->getFunCt2        ();
      stepD2        = fitPar->getStepD2        ();
      stepD3        = fitPar->getStepD3        ();
    }
  }
  if(wireOffsetFlag) {
    if( !HMdcGetContainers::isInited(fDigitPar) ) return kFALSE;
    signalSpeed=fDigitPar->getSignalSpeed();
  } else signalSpeed=0.;
  return kTRUE;
}

void HMdcTrackFitInOut::setDefaultFitParam(void) {
  // Default parameters of track fitter setting
  // If it calls by user Rtdb fit parameters will not used.
  useRndbPar     = kFALSE;
  
  fprint         = kFALSE;     
  printStartEv   = 0;
  nEventPrint    = 1000000000; 

  version        = 1;
  geantFlag      = kFALSE;
  wireOffsetFlag = 0;
  signalSpeed    = 0.005;
  cutWeight      = 0.01;
  useRtdbTofFlag = kTRUE;
  tofFlag        = 3;
  doTargScan     = kTRUE;
  calcInitValue  = HMdcTrackDSet::getCalcInitValueFlag();
  
  minTimeOffset  = -30.;       // Time offset cut
  maxTimeOffset  = 60.;        // -/-
  minCellsNum    = 5;
  
  chi2CutFlag    = kTRUE;      // kTRUE - do cut for funct., else for chi2/ndf
  totalChi2Cut   = 300.;       // default value for funct. cut
  chi2PerNdfCut  = 50.;        // default value for chi2/ndf cut
  
  // Tukey weight constants:
  useTukeyFlag   = kTRUE;
  useRtdbTFlag   = kTRUE;
  useRtdbTScFlag = kTRUE;
  cnWs           = 6.4516;    //2.54*2.54;
  cn4s           = 10.6276;   //3.26*3.26;
  cn2s           = cn4s*cn4s/cnWs; //17.5561;   //4.19*4.19;
  minSig2        = 2.5*2.5;
  tukeyScale     = 1.;
  maxNFilterIter = 4;
  minWeight      = 0.5;              // wt[time]=(wt[time]<minWeight) ? 0.:1.;
  maxChi2        = 25.; /*36.;6.0*/; // wt[time]=(chi2[time]>maxChi2) ? 0.:1.;
  
  minTOffsetIter = -50.; // if(timeOffset<minTOffsetIter) timeOffset=minTOffsetIter
  
  // Fit parameters for derivatives calc.:
  funCt1 = 500.;        // if(fun0 < funCt1)
  stepD1 = 0.0001;      //               stepD = stepD1;
  funCt2 = 10000.0;     // else if(fun0 < funCt2)
  stepD2 = 0.001;       //                stepD = stepD2;
  stepD3 = 0.01;        // else stepD = stepD3;
  
  // For alignment check:
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) exclLay[s][m][l] = kFALSE;
}

void HMdcTrackFitInOut::setNEventsPrint(Int_t start,Int_t nev) {
  printStartEv = start;
  nEventPrint  = nev;
}

void HMdcTrackFitInOut::setTukeyConstants(Double_t cw,Double_t c2,Double_t c4) {
  // Setting of tukey weights constants:
  // if     (chi2<cnWs*sig2) weight=(1-(chi2/(cn4s*sig2))^2)^2
  // else if(chi2<cn2s*sig2) weight=(1- chi2/(cn2s*sig2)   )^2
  // else weight=0.                                        
  cnWs = cw*cw;
  cn4s = c4*c4;
  cn2s = cn4s*cn4s/cnWs; //  cn2s = c2*c2;
}

void HMdcTrackFitInOut::getTukeyConstants(Double_t& cw,Double_t& c2,
    Double_t& c4) const {
  cw = sqrt(cnWs);
  c2 = sqrt(cn2s);
  c4 = sqrt(cn4s);
}
   
HMdcWireFit* HMdcTrackFitInOut::getNewWireFitSlot(Int_t* indWireFit) {
  TObject* fWireFit=fWireFitCat->getNewSlot(locWireFit,indWireFit);
  if(!fWireFit) {
    Warning("getNewWireFitSlot","No slot HMdcWireFit available");
    return 0;
  }
  if(geantFlag) return new(fWireFit) HMdcWireFitSim;
  else return new(fWireFit) HMdcWireFit;
}

HMdcClusFit* HMdcTrackFitInOut::getNewClusFitSlot(Int_t* indClusFit) {
  TObject* fClusFit=fClusFitCat->getNewSlot(locClusFit,indClusFit);
  if(!fClusFit) {
    Warning("getNewClusFitSlot","No slot HMdcClusFit available");
    return 0;
  }
  if(geantFlag) return new(fClusFit) HMdcClusFitSim;
  else return new(fClusFit) HMdcClusFit;
}

Double_t HMdcTrackFitInOut::getStepDer(Double_t funct) const {
  return (funct<funCt1) ? stepD1:((funct<funCt2) ? stepD2:stepD3);
}

void HMdcTrackFitInOut::setPrintFlag(Int_t currentEvent) {
  fprint = currentEvent>=printStartEv && currentEvent<printStartEv+nEventPrint;
}

void HMdcTrackFitInOut::setChi2PerNdfCut(Double_t cut) {
  chi2CutFlag   = kFALSE;
  chi2PerNdfCut = cut;
}

void HMdcTrackFitInOut::setTotalChi2Cut(Double_t cut) {
  chi2CutFlag  = kTRUE;
  totalChi2Cut = cut;
}

void HMdcTrackFitInOut::excludeLayer(UInt_t s,UInt_t m,UInt_t l) {
  if(s<6 && m<4 && l<6) exclLay[s][m][l] = kTRUE;
}

void HMdcTrackFitInOut::excludeModule(UInt_t s,UInt_t m) {
  if(s<6 && m<4) for(UInt_t l=0;l<6;l++) exclLay[s][m][l] = kTRUE;
}

Bool_t HMdcTrackFitInOut::isLayerExcluded(Int_t s,Int_t m,Int_t l) const {
  return s<6 && m<4 && l<6 ? exclLay[s][m][l] : kFALSE;
}

//============================================================================

HMdcTrackFitter::HMdcTrackFitter(HMdcTrackFitInOut* fIO) {
  fitInOut=fIO;
  init();
}

void HMdcTrackFitter::init(void) {
  fprint      = fitInOut->getPrintFlag();
  tofFlag     = fitInOut->getTofFlag();
  wires.setPrintFlag(fprint);
  wires.setTrackFitInOut(fitInOut);
}

void HMdcTrackFitter::setPrintFlag(Bool_t prnt)        {
  fprint=prnt;
  wires.setPrintFlag(fprint);
}

Bool_t HMdcTrackFitter::fillListHits(HMdcClus* cl1,HMdcClus* cl2) {
  segIndex=-1;
  indClusFit=-1; //??? mozhet v drugoe mesto?
  if(!wires.fillListHits(cl1,cl2)) return kFALSE;
  setPlanes();
  return kTRUE;
}

Bool_t HMdcTrackFitter::fillListHits(HMdcClus* cl1,HMdcClus* cl2,HMdcClus* cl3,HMdcClus* cl4,
                                     HMdcClus* cl5,HMdcClus* cl6,HMdcClus* cl7,HMdcClus* cl8) {
  // Cosmic two sectors data
  segIndex=-1;
  indClusFit=-1; //??? mozhet v drugoe mesto?
  if( !wires.fillListHits(cl1,cl2,cl3,cl4,cl5,cl6,cl7,cl8) ) return kFALSE;
  setPlanes();
  return kTRUE;
}

Bool_t HMdcTrackFitter::fillListHits(HMdcEvntListCells* store,HMdcClus* clus1,HMdcClus* clus2) {
  segIndex=-1;
  indClusFit=-1; //??? mozhet v drugoe mesto?
  if(!wires.fillListHits(store,clus1,clus2)) return kFALSE;
  setPlanes();  
  return kTRUE;
}

Bool_t HMdcTrackFitter::fillListHits(HMdcEvntListCells* store) {
  segIndex=-1;
  indClusFit=-1; //??? mozhet v drugoe mesto?
  if(!wires.fillListHits(store)) return kFALSE;
  setPlanes();  
  return kTRUE;
}

void HMdcTrackFitter::setPlanes(void) {
  Int_t sec = wires.getSector();
  HMdcSizesCellsMod** fSCModAr = fitInOut->getSCellsModArr(sec);
//  Int_t nClTimes=wires.getNDriftTimes();
  Int_t nClTimes = wires.getNDrTmFirstSec();
  initParam.setFirstPlane(&((*(fSCModAr[wires[0].getModule()]))[0]));
  initParam.setSecondPlane(&((*(fSCModAr[wires[nClTimes-1].getModule()]))[5]));
  initParam.setCoorSys(sec);
}

Bool_t HMdcTrackFitter::fillClusFitCont(void) {
  HMdcClusFit* fClusFit=fitInOut->getNewClusFitSlot(&indClusFit);
  if(!fClusFit) return kFALSE;
  fClusFit->setFitStatus(fitStatus);
  finalParam.fillClusFit(fClusFit);
  fClusFit->setExitFlag(exitFlag);
  wires.calcDistanceSign(finalParam);
  wires.fillClusFitSim(fClusFit,finalParam);
  wires.fillClusFitAndWireFit(fClusFit);       // must be after fillClusFitSim
  return kTRUE;
}

Bool_t HMdcTrackFitter::fitCluster(Int_t fittingMod) {
  if(wires.getNCellsInInput(fittingMod) < 5) return kFALSE;
  wires.setHitStatM1toP1();         // if(hitStatus==-1) hitStatus=1  ???
  wires.subtractWireOffset(initParam);
  
  // CalcInitValueFlag = 1:
  if(HMdcTrackDSet::getCalcInitValueFlag()==1) wires.calcInitialValue(initParam);
  
  wires.fillLookupTableForDer(initParam);
  fitStatus  = fit(fittingMod);
  
  // CalcInitValueFlag = 2:
  if(HMdcTrackDSet::getCalcInitValueFlag()==2 && wires.getNWiresInFit()<30) {
    if(!fitStatus) refitCluster(fittingMod);              // Fit is not accepted
    else {                                                // Fit is accepted
      Int_t nInputLayers  = wires.getInputListCells().getNLayers();
      Int_t nFittedLayers = wires.getOutputListCells().getNLayers();
      if(nInputLayers-nFittedLayers >= 3) {
        refitCluster(fittingMod);
        nFittedLayers = wires.getOutputListCells().getNLayers();
      }
    }
  }
  
  if(fitInOut->getClusFitCat() && HMdcTrackDSet::fNTuple()) fillClusFitCont();
  return fitStatus;
}

void HMdcTrackFitter::refitCluster(Int_t fittingMod) {
  if(wires.calcInitialValue(initParam)) {
    wires.reinitWtSt();
    fitStatus = fit(fittingMod);
  }
}

Bool_t HMdcTrackFitter::fit(Int_t fittingMod) {
  Int_t iter = 0;
  while(kTRUE) {
    Int_t    exit   = minimize(iter++);
    Double_t delW   = wires.testFitResult();
    Int_t    nCells = wires.getNCellsInOutput(fittingMod);
    if( delW<0.5 || nCells<6 ) {
      if(exit == 0) return kFALSE;
      if(delW > 0.)
        if(finalParam.calcChi2PerDF(wires.getNumOfGoodWires())<0. || !testChi2Cut()) return kFALSE;
      if(finalParam.testParameters(fitInOut->getMinTimeOffset(),fitInOut->getMaxTimeOffset()) && 
         nCells >= fitInOut->getMinCellsNum()) return kTRUE;
      break;
    }
    if(fprint) printf("TestFit: num.of deleted cells=%.1f, refit this!\n",delW);
  }
  return kFALSE;
}

Bool_t HMdcTrackFitter::testChi2Cut(void) {
  if(fitInOut->getChi2CutFlag()) {
         if(finalParam.functional()<fitInOut->getTotalChi2Cut() ) return kTRUE;
  } else if(finalParam.getChi2()   <fitInOut->getChi2PerNdfCut()) return kTRUE;
  return kFALSE;
}

void HMdcTrackFitter::setRegionOfWires(Int_t mod) {
  wires.setRegionOfWires(mod);
}

Bool_t HMdcTrackFitter::setClustAndFill(HMdcClus* cl1, HMdcClus* cl2) {
  if(fprint != fitInOut->getPrintFlag()) setPrintFlag(fitInOut->getPrintFlag());
  if(fprint) {
    cl1->print();
    if(cl2) cl2->print();
  }
  if(!fillListHits(cl1,cl2)) return kFALSE;
  initParam.setParam(cl1->getXTarg(),cl1->getYTarg(),cl1->getZTarg(),
                     cl1->getX(),    cl1->getY(),    cl1->getZ());
  return kTRUE;
}

Bool_t HMdcTrackFitter::setClustAndFill(HMdcClus* cl1,HMdcClus* cl2,HMdcClus* cl3,HMdcClus* cl4,
                                        HMdcClus* cl5,HMdcClus* cl6,HMdcClus* cl7,HMdcClus* cl8) {
  // Cosmic two sectors data
  if(fprint != fitInOut->getPrintFlag()) setPrintFlag(fitInOut->getPrintFlag());
  if(fprint) {
    cl1->print();
    if(cl2 != NULL) cl2->print();
    if(cl3 != NULL) cl3->print();
    if(cl4 != NULL) cl4->print();
    if(cl5 != NULL) cl5->print();
    if(cl6 != NULL) cl6->print();
    if(cl7 != NULL) cl7->print();
    if(cl8 != NULL) cl8->print();
  }
  Int_t nSecs = 2;
  if(cl5 != NULL) nSecs = 3;
  if(cl7 != NULL) nSecs = 4;
  initParam.setNMods(nSecs); //setTwoSecData();
  finalParam.setNMods(nSecs); //setTwoSecData();
  if(!fillListHits(cl1,cl2,cl3,cl4,cl5,cl6,cl7,cl8)) return kFALSE;
  initParam.setParam(cl1->getXTarg(),cl1->getYTarg(),cl1->getZTarg(),
                     cl1->getX(),    cl1->getY(),    cl1->getZ());
  return kTRUE;
}

Bool_t HMdcTrackFitter::setClustAndFill(HMdcEvntListCells* store,
					Double_t x1, Double_t y1, Double_t z1,
					Double_t x2, Double_t y2, Double_t z2) {
  if(fprint != fitInOut->getPrintFlag()) setPrintFlag(fitInOut->getPrintFlag());
  if(fprint) printf("x1=%f  y1=%f z1=%f   x2=%f y2=%f z2=%f \n",x1,y1,z1,x2,y2,z2);
  if(!fillListHits(store)) return kFALSE;
  initParam.setParam(x1, y1, z1, x2, y2, z2);
  wires.setXYZClust(x2, y2, z2);
  return kTRUE;
}

void HMdcTrackFitter::fillOutput() {
  finalParam.copyLine(initParam);
  wires.calcNGoodWiresAndChi2(finalParam);
  wires.valueOfFunctional(finalParam,2);
  wires.calculateErrors(finalParam); //Errors calculations
}
