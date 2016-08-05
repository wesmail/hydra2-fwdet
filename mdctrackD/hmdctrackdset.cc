using namespace std;
#include "TError.h"
#include "hmdctrackdset.h"
#include "hmdctrackfinder.h"
#include "hmdcclustertohit.h"
#include "hmdc12fit.h"
#include "hmdcidealtracking.h"
#include "TCutG.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "TArrayD.h"

//*-- Author : Vladimir Pechenov
//*-- Modified : 22.07.2002 by V.Pechenov
//*-- Modified : 16.02.2010 by V.Pechenov
//*-- Modified : 26.09.2012 by O.Pechenova
//*-- Modified : 11.03.2015 by O.Pechenova

// HMdcTrackFinder param.:
HMdcTrackFinder* HMdcTrackDSet::pTrackFinder   = NULL;
Bool_t   HMdcTrackDSet::trFndrIsAdded          = kFALSE;
Bool_t   HMdcTrackDSet::isCoiloff              = kFALSE;
Int_t    HMdcTrackDSet::clFinderType           = -1; // =0 - segment finder, =1 - mdc finder,
                                                     // =2 - mixed(segment+mdc) cluster finder
Int_t    HMdcTrackDSet::level4[2]              = {-10,-10};
Int_t    HMdcTrackDSet::level5[2]              = {-50,-30};
Int_t    HMdcTrackDSet::nLayersInMod[6][4]     = {{-6,-6,-6,-6},{-6,-6,-6,-6},{-6,-6,-6,-6},
                                                  {-6,-6,-6,-6},{-6,-6,-6,-6},{-6,-6,-6,-6}};
// HMdcLookUpTb param:
Bool_t   HMdcTrackDSet::findVertex             = kFALSE;
Double_t HMdcTrackDSet::dDistCutVF             = 0.5;
Int_t    HMdcTrackDSet::levelVertF             = 11;
Int_t    HMdcTrackDSet::levelVertPPlot         = 12;
Double_t HMdcTrackDSet::zStartDetertor         = 0.;
Int_t    HMdcTrackDSet::nBinXSeg1              = 320; // 1194;  // Project plot size;
Int_t    HMdcTrackDSet::nBinYSeg1              = 886; // 1100;
Bool_t   HMdcTrackDSet::useFloatLevSeg1        = kTRUE;
Bool_t   HMdcTrackDSet::useAmpCutSeg1          = kTRUE;
Char_t   HMdcTrackDSet::fakeSuppFlSeg1         = 1;
Bool_t   HMdcTrackDSet::useDrTimeSeg1          = kFALSE;
Double_t HMdcTrackDSet::constUncertSeg1        = 0.7; // ??? 0.4; // //1.3; //0.7;
Double_t HMdcTrackDSet::dDistCutSeg1           = 0.7; //1.5; // 1.0;
Double_t HMdcTrackDSet::dDistCutYcorSeg1       = 1.5;
Double_t HMdcTrackDSet::dDistCorrLaySeg1[4][6] = { {1.10,1.09,1.27,1.23,0.90,0.83},   // MDC I
                                                   {0.94,0.95,1.36,1.40,1.12,1.14},   // MDC II
                                                   {  1.,  1.,  1.,  1.,  1.,  1.},   // MDC III
                                                   {  1.,  1.,  1.,  1.,  1.,  1.} }; // MDC IV

// HMdc34ClFinder param:
Int_t    HMdcTrackDSet::nBinXSeg2              = 320; // 1194;  // Project plot size;
Int_t    HMdcTrackDSet::nBinYSeg2              = 886; // 1100;
Bool_t   HMdcTrackDSet::useFloatLevSeg2        = kTRUE;
Char_t   HMdcTrackDSet::fakeSuppFlSeg2         = 1;
Int_t    HMdcTrackDSet::wLevSeg2               = 1000;
Int_t    HMdcTrackDSet::wBinSeg2               = 100;
Int_t    HMdcTrackDSet::wLaySeg2               = 600;
Int_t    HMdcTrackDSet::dWtCutSeg2             = 1650;
Bool_t   HMdcTrackDSet::useDrTimeSeg2          = kFALSE;
Double_t HMdcTrackDSet::dDistCutSeg2           = 0.2;
Double_t HMdcTrackDSet::dDistCutYcorSeg2       = 2.3;
Double_t HMdcTrackDSet::dDistCorrLaySeg2[2][6] = { {0.5,0.2,0.1,0.1, 0.,0.3},    // MDC III
                                                   {1.9,0.7,0.8,0.9,0.6,2.0} };  // MDC IV
Bool_t   HMdcTrackDSet::useDxDyCut             = kTRUE;
TCutG    HMdcTrackDSet::cutDxDyArr[36];
Bool_t   HMdcTrackDSet::isDxDyInited           = kFALSE;
Bool_t   HMdcTrackDSet::useFittedSeg1Par       = kFALSE;

Bool_t   HMdcTrackDSet::useKickCor             = kTRUE;

// Meta match:
Bool_t   HMdcTrackDSet::doMetaMatch            = kTRUE; //kFALSE;
Bool_t   HMdcTrackDSet::doMMPlots              = kFALSE;

// HMdcClusterToHit param.:
HMdcClusterToHit* HMdcTrackDSet::pClusterToHit = NULL;
Int_t    HMdcTrackDSet::modForSeg1             = 1;
Int_t    HMdcTrackDSet::modForSeg2             = 3;

// HMdc12Fit param.:
HMdc12Fit* HMdcTrackDSet::p12Fit               = NULL;
Bool_t   HMdcTrackDSet::useAnotherFit          = kFALSE;
Int_t    HMdcTrackDSet::fitAuthor              = -1;
Int_t    HMdcTrackDSet::distTimeVer            = -1;
Int_t    HMdcTrackDSet::fitType                = -1;
Bool_t   HMdcTrackDSet::fntuple                = kFALSE;
Bool_t   HMdcTrackDSet::fprint                 = kFALSE;
Int_t    HMdcTrackDSet::useWireOffset          = 1;
Int_t    HMdcTrackDSet::mixFitCut              = 2;
Int_t    HMdcTrackDSet::mixClusCut             = 4;

// OffVertex tracking param.:
Bool_t   HMdcTrackDSet::findOffVertTrk         = kTRUE; //kFALSE;
Int_t    HMdcTrackDSet::nLayersCutOVT          = 7;
Int_t    HMdcTrackDSet::nWiresCutOVT           = 60;
Int_t    HMdcTrackDSet::nCellsCutOVT           = 7;

UChar_t  HMdcTrackDSet::calcInitValue          = 0;
Double_t HMdcTrackDSet::cInitValCutSeg1        = 1.8;  // [mm]
Double_t HMdcTrackDSet::cInitValCutSeg2        = 3.8;  // [mm]

Bool_t   HMdcTrackDSet::isSetCall              = kFALSE;

// HMdcIdealTracking:
HMdcIdealTracking* HMdcTrackDSet::pIdealTracking = NULL;

HReconstructor* HMdcTrackDSet::hMdcTrackFinder(const Char_t* name,const Char_t* title) {
  if(trFndrIsAdded) return pTrackFinder;
  if(pTrackFinder) exitSetup("hMdcTrackFinder","HMdcTrackFinder");
  isSetCall    = kTRUE;
  pTrackFinder = new HMdcTrackFinder(name,title);
  isSetCall    = kFALSE;
  return pTrackFinder;
}

Bool_t HMdcTrackDSet::isModActive(UInt_t sec,UInt_t mod) {
// -  return nLayersInMod[sec][mod] > 0 && sec<6 && mod<4;
  return nLayersInMod[sec][mod] != 0 && sec<6 && mod<4;
}

Bool_t HMdcTrackDSet::isSegActive(UInt_t sec,UInt_t seg) {
  return seg<2 && (isModActive(sec,seg*2) || isModActive(sec,seg*2+1));
}

Bool_t HMdcTrackDSet::isSecActive(UInt_t sec) {
  return isSegActive(sec,0) || isSegActive(sec,1);
}

void HMdcTrackDSet::useThisTrackFinder(HMdcTrackFinder* trFndr) {
  if(pTrackFinder) trFndrIsAdded=kTRUE;
  else Error("useThisTrackFinder(HMdcTrackFinder* trFndr)"," trFndr==0 !!!");
}

HReconstructor* HMdcTrackDSet::hMdcClusterToHit(const Char_t* name,const Char_t* title) {
  if(pClusterToHit)  exitSetup("hMdcClusterToHit","HMdcClusterToHit");
  isSetCall = kTRUE;
  pClusterToHit=new HMdcClusterToHit(name,title);
  isSetCall = kFALSE;
  return pClusterToHit;
}

HReconstructor* HMdcTrackDSet::hMdcClusterToHit(const Char_t* name,const Char_t* title,
    Int_t mSeg1, Int_t mSeg2) {
  setMdcClusterToHit(mSeg1,mSeg2);
  return hMdcClusterToHit(name,title);
}

HReconstructor* HMdcTrackDSet::hMdc12Fit(const Char_t* name,const Char_t* title) {
  if(p12Fit) {
    if(useAnotherFit) return p12Fit;
    exitSetup("hMdc12Fit","HMdc12Fit");
  }
  isSetCall = kTRUE;
  p12Fit    = new HMdc12Fit(name,title);
  isSetCall = kFALSE;
  return p12Fit;
}

void HMdcTrackDSet::setAnotherFit(HMdc12Fit* fit) {
  p12Fit        = fit;
  useAnotherFit = kTRUE;
}

HReconstructor* HMdcTrackDSet::hMdc12Fit(const Char_t* name,const Char_t* title,
    Int_t author, Int_t version, Int_t indep, Bool_t nTuple, Bool_t prnt) {
  setMdc12Fit(author,version,indep,nTuple,prnt);
  return hMdc12Fit(name,title);
}

HReconstructor* HMdcTrackDSet::hMdcIdealTracking(const Char_t* name,const Char_t* title,Bool_t fillParallel) {
  pIdealTracking    = new HMdcIdealTracking(name,title);
  if(fillParallel) pIdealTracking->fillParallelCategories();
  return pIdealTracking;
}

void HMdcTrackDSet::setMdcTrackFinder(Bool_t coilSt,Int_t type) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  if(isSetCall) return;
  setCoilStatus(coilSt);
  if(clFinderType<0) setClFinderType(type); // if clFinderType >= 0) - don't change it
}

void HMdcTrackDSet::setClFinderType(Int_t type) {
  if(isSetCall) return;
  if(type<0 || type>3) exitSetup("setClFinderType(Int_t):",1,"0,1 or 2",type);
  clFinderType = type;
}

void HMdcTrackDSet::setTrFnLevel(Int_t l4s1,Int_t l5s1,Int_t l4s2,Int_t l5s2) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  if(isSetCall) return;
  // if level4[seg] >= 0) - don't change it
  if(level4[0] < 0) setTrFnSegLevel(0,l4s1,l5s1);
  if(level4[1] < 0) setTrFnSegLevel(1,l4s2,l5s2);
}

void HMdcTrackDSet::setTrackFinderLevel(Int_t l4s1,Int_t l5s1,Int_t l4s2,Int_t l5s2) {
  setTrFnSegLevel(0,l4s1,l5s1);
  setTrFnSegLevel(1,l4s2,l5s2);
}

void HMdcTrackDSet::setTrFnSegLevel(Int_t seg, Int_t l4, Int_t l5) {
  if(isSetCall) return;
  if(seg<0||seg>1) exitSetup("setTrFnSegLevel(Int_t,Int_t,Int_t):",1,"0 or 1",seg);
  level4[seg] = l4;
  level5[seg] = l5;
}

void HMdcTrackDSet::setTrFnNLayers(const Int_t* lst) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  if(isSetCall) return;
  if(!lst) {
    Error("setTrFnNLayers(const Int_t*)"," lst-pointer = 0");
    exit(1);
  }
  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) {
    if(nLayersInMod[s][m]<0) setTrFnNLayers(s,m,lst[s*4+m]); // if nLayersInMod>=0 - don't change it
  }
}

void HMdcTrackDSet::setTrFnNLayers(Int_t s, Int_t m,Int_t nl) {
  if(isSetCall) return;
  if(s<0 || s>5) exitSetup("setTrFnNLayers(Int_t,Int_t,Int_t):",1,"0,...,5",s);
  if(m<0 || m>3) exitSetup("setTrFnNLayers(Int_t,Int_t,Int_t):",2,"0,...,3",m);
  if(nl<0||nl>6) exitSetup("setTrFnNLayers(Int_t,Int_t,Int_t):",3,"0,...,6",nl);
  nLayersInMod[s][m] = nl;
}

void HMdcTrackDSet::setMdcClusterToHit(Int_t mSeg1,Int_t mSeg2) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  if(isSetCall) return;
  if(mSeg1<-1 || mSeg1>1) exitSetup("setMdcClusterToHit(Int_t,Int_t):",1,"-1, 0 or 1",mSeg1);
  modForSeg1 = mSeg1;
  if(mSeg2!=-1 && mSeg2!=2 && mSeg2!=3) exitSetup("setMdcClusterToHit(Int_t,Int_t):",2,"-1, 2 or 3",mSeg2);
  modForSeg2 = mSeg2;
}

void HMdcTrackDSet::setMdc12Fit(Int_t author,Int_t version,Int_t indep,Bool_t nTuple,Bool_t prnt) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  if(isSetCall) return;
  if(fitAuthor<0)   setFitAuthor(author);
  if(distTimeVer<0) setFitVersion(version);
  if(fitType<0)     setFitType(indep);
  setFitOutput(nTuple,prnt);
}

void HMdcTrackDSet::setFitAuthor(Int_t author) {
  if(isSetCall) return;
  if(author<1 || author>2) exitSetup("setFitAuthor(Int_t):",1,"1 or 2",author);
  fitAuthor = author;
}

void HMdcTrackDSet::setFitVersion(Int_t ver) {
  if(isSetCall) return;
  if(ver<0||ver>1) exitSetup("setFitVersion(Int_t):",1,"0 or 1",ver);
  distTimeVer = ver;
}

void HMdcTrackDSet::setFitType(Int_t type) {
  if(isSetCall) return;
  if(type<0 || type>1) exitSetup("setFitType(Int_t):",1,"0 or 1",type);
  fitType = type;
}

void HMdcTrackDSet::setFitOutput(Bool_t nTuple, Bool_t prnt) {
  if(isSetCall) return;
  fntuple = nTuple;
  fprint  = prnt;
}

void HMdcTrackDSet::setUseWireOffset(Bool_t off) {
  // Function for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
  useWireOffset = off;
}

void HMdcTrackDSet::setMixFitCut(Int_t cut) {
  mixFitCut = cut;
}

void HMdcTrackDSet::setMixClusCut(Int_t cut) {
  mixClusCut = cut;
}

void HMdcTrackDSet::setMixCuts(Int_t cutFit,Int_t cutClus) {
  mixFitCut  = cutFit;
  mixClusCut = cutClus;
}

void HMdcTrackDSet::exitSetup(const Char_t* funNm, Int_t npar,const Char_t* use, Int_t inst) {
  Error(funNm,"\n unknown argument %i: use %s instead %i",npar,use,inst);
  exit(1);
}

void HMdcTrackDSet::getTrFnLevels(Int_t& l4s1,Int_t& l5s1,Int_t& l4s2,Int_t& l5s2) {
  l4s1 = level4[0];
  l4s2 = level4[1];
  l5s1 = level5[0];
  l5s2 = level5[1];
}

void HMdcTrackDSet::printStatus(void) {
  if(pTrackFinder   != NULL) pTrackFinder->printStatus();
  if(pClusterToHit  != NULL) pClusterToHit->printStatus();
  if(p12Fit         != NULL) p12Fit->printStatus();
  if(pIdealTracking != NULL) pIdealTracking->printStatus();
}

void HMdcTrackDSet::setTrackFinder(HMdcTrackFinder* tr) {
  if(isSetCall) return;
  if(pTrackFinder) exitSetup("setTrackFinder","HMdcTrackFinder");
  pTrackFinder = tr;
}

void HMdcTrackDSet::setClusterToHit(HMdcClusterToHit* cl) {
  if(isSetCall) return;
  if(pClusterToHit) exitSetup("setClusterToHit","HMdcClusterToHit");
  pClusterToHit = cl;
}

void HMdcTrackDSet::set12Fit(HMdc12Fit* fit) {
  if(isSetCall) return;
  if(p12Fit) exitSetup("set12Fit","HMdc12Fit");
  p12Fit = fit;
}

void HMdcTrackDSet::exitSetup(const Char_t* funNm,const Char_t* className) {
  Error(funNm,"%s object exist already.",className);
  exit(1);
}

void HMdcTrackDSet::setGhostRemovingParamSeg2(Int_t wlev,Int_t wbin,Int_t wlay,Int_t dWtCut) {
  wLevSeg2       = wlev;
  wBinSeg2       = wbin;
  wLaySeg2       = wlay;
  dWtCutSeg2     = dWtCut;
  fakeSuppFlSeg2 = 1;
}

Char_t HMdcTrackDSet::getGhostRemovingParamSeg2(Int_t &wlev,Int_t &wbin,Int_t &wlay,Int_t &dWtCut) {
  wlev   = wLevSeg2;
  wbin   = wBinSeg2;
  wlay   = wLaySeg2;
  dWtCut = dWtCutSeg2;
  return fakeSuppFlSeg2;
}

void HMdcTrackDSet::setProjectPlotSizeSeg1(Int_t xb,Int_t yb) {
  if(xb>20) nBinXSeg1 = xb;
  if(yb>20) nBinYSeg1 = yb;
}

void HMdcTrackDSet::getProjectPlotSizeSeg1(Int_t &xb,Int_t &yb) {
  xb = nBinXSeg1;
  yb = nBinYSeg1;
}

void HMdcTrackDSet::setProjectPlotSizeSeg2(Int_t xb,Int_t yb) {
  if(xb>20) nBinXSeg2 = xb;
  if(yb>20) nBinYSeg2 = yb;
}

void HMdcTrackDSet::getProjectPlotSizeSeg2(Int_t &xb,Int_t &yb) {
  xb = nBinXSeg2;
  yb = nBinYSeg2;
}

Bool_t HMdcTrackDSet::getVertexFinderPar(Double_t &d,Int_t &l,Int_t &lp) {
  d  = dDistCutVF;
  l  = levelVertF;
  lp = levelVertPPlot;
  return findVertex;
}

void HMdcTrackDSet::setVertexFinderPar(Double_t dc, Int_t lv, Int_t lp)  {
  dDistCutVF     = dc;
  levelVertF     = lv;
  levelVertPPlot = lp;
  findVertex = dc >= 0.;
}

void HMdcTrackDSet::setDrTimeProjParamSeg1(Double_t cUns, Double_t cut) {
  constUncertSeg1 = cUns;     // Constant part in LookUpTbCell::yProjUncer
  dDistCutSeg1    = cut;      // Cut:  dDist/yProjUncer < dDistCut
}


void HMdcTrackDSet::setDrTimeCutLCorrSeg1(Int_t m,Double_t* corr) {
  if(m<0 || m>3) return;
  for(Int_t lay=0;lay<6;lay++) if(corr[lay]>0.)  dDistCorrLaySeg1[m][lay] = corr[lay];
}

void HMdcTrackDSet::getDrTimeProjParSeg1(Double_t &uns,Double_t &cut,Double_t &ycor,Double_t *lcor) {
  uns  = constUncertSeg1;
  cut  = dDistCutSeg1;
  ycor = dDistCutYcorSeg1;
  for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) lcor[m*6+l] = dDistCorrLaySeg1[m][l];
}

void HMdcTrackDSet::setDrTimeCutLCorrSeg2(Int_t m,Double_t* corr) {
  if(m<2 || m>3) return;
  for(Int_t lay=0;lay<6;lay++) if(corr[lay]>0.)  dDistCorrLaySeg2[m-2][lay] = corr[lay];
}

void HMdcTrackDSet::getDrTimeProjParSeg2(Double_t &cut,Double_t &ycor,Double_t *lcor) {
  cut  = dDistCutSeg2;
  ycor = dDistCutYcorSeg2;
  for(Int_t im=0;im<2;im++) for(Int_t l=0;l<6;l++) lcor[im*6+l] = dDistCorrLaySeg2[im][l];
}

void HMdcTrackDSet::setDxDyKickCut(TCutG* cutR) {
    // cutR is pointer to array TCutG cutReg[36] !

  for(Int_t nr=0;nr<36;nr++) {
      Double_t x,y;
      for(Int_t i=0;i<cutR[nr].GetN();i++){
	  cutR[nr].GetPoint(i, x, y);
	  cutDxDyArr[nr].SetPoint(i,x,y);
      }
  }
  isDxDyInited = kTRUE;
  useDxDyCut   = kTRUE;
}

Bool_t HMdcTrackDSet::getDxDyKickCut(TCutG* cutR) {
    // cutR is pointer to array TCutG cutReg[36] !

  if(!isDxDyInited) {
    ::Error("getDxDyKickCut","Parameters is not initialized! Use ::setTrackParam(...) first!");
    exit(1);
  }
  for(Int_t nr=0;nr<36;nr++) {
      Double_t x,y;
      for(Int_t i=0;i<cutDxDyArr[nr].GetN();i++){
	  cutDxDyArr[nr].GetPoint(i, x, y);
	  cutR[nr].SetPoint(i,x,y);
      }
  }
  return useDxDyCut;
}

void HMdcTrackDSet::initDxDyCutAug11(void) {
  //--------------  Butterfly cut  (for kick dX dY on the project plane) ----------

  isDxDyInited = kTRUE;
  cutDxDyArr[0].Set(28);
  cutDxDyArr[0].SetPoint(0,16.3862,-25.1868);
  cutDxDyArr[0].SetPoint(1,15.4919,-63.6264);
  cutDxDyArr[0].SetPoint(2,7.66667,-92.7473);
  cutDxDyArr[0].SetPoint(3,-1.94715,-103.231);
  cutDxDyArr[0].SetPoint(4,-17.3741,-98.6845);
  cutDxDyArr[0].SetPoint(5,-22.6394,-86.2294);
  cutDxDyArr[0].SetPoint(6,-23.8095,-68.2386);
  cutDxDyArr[0].SetPoint(7,-20.2805,-39.1648);
  cutDxDyArr[0].SetPoint(8,-15.3618,-17.033);
  cutDxDyArr[0].SetPoint(9,-2.39431,-0.725275);
  cutDxDyArr[0].SetPoint(10,-1.72358,20.2418);
  cutDxDyArr[0].SetPoint(11,-9.99594,21.4066);
  cutDxDyArr[0].SetPoint(12,-20.0569,34.2198);
  cutDxDyArr[0].SetPoint(13,-40.4024,80.8132);
  cutDxDyArr[0].SetPoint(14,-72.1504,183.319);
  cutDxDyArr[0].SetPoint(15,-72.9526,209.926);
  cutDxDyArr[0].SetPoint(16,-63.9614,235.075);
  cutDxDyArr[0].SetPoint(17,-49.3455,235.736);
  cutDxDyArr[0].SetPoint(18,-24.752,179.824);
  cutDxDyArr[0].SetPoint(19,-16.2561,155.363);
  cutDxDyArr[0].SetPoint(20,-5.3808,117.204);
  cutDxDyArr[0].SetPoint(21,4.56484,74.3034);
  cutDxDyArr[0].SetPoint(22,6.61247,49.3932);
  cutDxDyArr[0].SetPoint(23,3.9798,25.8668);
  cutDxDyArr[0].SetPoint(24,-1.28553,14.7956);
  cutDxDyArr[0].SetPoint(25,-1.5,-1.89011);
  cutDxDyArr[0].SetPoint(26,12.1382,-8.87912);
  cutDxDyArr[0].SetPoint(27,16.3862,-25.1868);

  cutDxDyArr[1].Set(28);
  cutDxDyArr[1].SetPoint(0,31.2937,-40.1396);
  cutDxDyArr[1].SetPoint(1,31.7719,-111.768);
  cutDxDyArr[1].SetPoint(2,20.2513,-165.011);
  cutDxDyArr[1].SetPoint(3,2.25014,-201.013);
  cutDxDyArr[1].SetPoint(4,-20.8252,-189.907);
  cutDxDyArr[1].SetPoint(5,-30.8665,-144.326);
  cutDxDyArr[1].SetPoint(6,-29.432,-107.426);
  cutDxDyArr[1].SetPoint(7,-17.0407,-66.302);
  cutDxDyArr[1].SetPoint(8,-14,-40.1843);
  cutDxDyArr[1].SetPoint(9,-11.7403,-16.2636);
  cutDxDyArr[1].SetPoint(10,-2.39024,0.443182);
  cutDxDyArr[1].SetPoint(11,-3.77236,16.404);
  cutDxDyArr[1].SetPoint(12,-9.30081,17.855);
  cutDxDyArr[1].SetPoint(13,-19.5285,30.9138);
  cutDxDyArr[1].SetPoint(14,-37.4959,62.8354);
  cutDxDyArr[1].SetPoint(15,-55.7398,104.914);
  cutDxDyArr[1].SetPoint(16,-77.2476,220.326);
  cutDxDyArr[1].SetPoint(17,-74.3787,259.395);
  cutDxDyArr[1].SetPoint(18,-59.5558,276.76);
  cutDxDyArr[1].SetPoint(19,-32.5203,245.659);
  cutDxDyArr[1].SetPoint(20,-10.3058,198.62);
  cutDxDyArr[1].SetPoint(21,1.7561,142.639);
  cutDxDyArr[1].SetPoint(22,6.17886,80.2472);
  cutDxDyArr[1].SetPoint(23,5.95147,35.8295);
  cutDxDyArr[1].SetPoint(24,-1.00813,17.855);
  cutDxDyArr[1].SetPoint(25,-0.731708,0.443182);
  cutDxDyArr[1].SetPoint(26,15.3008,-6.81173);
  cutDxDyArr[1].SetPoint(27,31.2937,-40.1396);

  cutDxDyArr[2].Set(25);
  cutDxDyArr[2].SetPoint(0,31.8623,-47.2211);
  cutDxDyArr[2].SetPoint(1,39.7692,-104.905);
  cutDxDyArr[2].SetPoint(2,28.3773,-184.389);
  cutDxDyArr[2].SetPoint(3,0.905657,-215.118);
  cutDxDyArr[2].SetPoint(4,-21.3333,-137.39);
  cutDxDyArr[2].SetPoint(5,-23.691,-60.9238);
  cutDxDyArr[2].SetPoint(6,-21.0753,-19.4586);
  cutDxDyArr[2].SetPoint(7,-13.9105,-7.40188);
  cutDxDyArr[2].SetPoint(8,-0.80359,-1.91571);
  cutDxDyArr[2].SetPoint(9,-0.80359,10.8428);
  cutDxDyArr[2].SetPoint(10,-17.8057,34.765);
  cutDxDyArr[2].SetPoint(11,-24.7031,58.1223);
  cutDxDyArr[2].SetPoint(12,-30.6576,96.7648);
  cutDxDyArr[2].SetPoint(13,-41.02,143.212);
  cutDxDyArr[2].SetPoint(14,-45.4008,258.505);
  cutDxDyArr[2].SetPoint(15,-25.2874,281.579);
  cutDxDyArr[2].SetPoint(16,-2.93927,257.063);
  cutDxDyArr[2].SetPoint(17,10.0972,232.547);
  cutDxDyArr[2].SetPoint(18,16.8524,108.126);
  cutDxDyArr[2].SetPoint(19,11.294,26.7909);
  cutDxDyArr[2].SetPoint(20,0.177299,10.8428);
  cutDxDyArr[2].SetPoint(21,0.177299,1.27392);
  cutDxDyArr[2].SetPoint(22,19.7814,-6.84211);
  cutDxDyArr[2].SetPoint(23,26.0074,-19.4586);
  cutDxDyArr[2].SetPoint(24,31.8623,-47.2211);

  cutDxDyArr[3].Set(26);
  cutDxDyArr[3].SetPoint(0,21.8623,-68.0107);
  cutDxDyArr[3].SetPoint(1,26.332,-151.904);
  cutDxDyArr[3].SetPoint(2,22.9798,-179.36);
  cutDxDyArr[3].SetPoint(3,5.38057,-209.867);
  cutDxDyArr[3].SetPoint(4,-13.6154,-200.715);
  cutDxDyArr[3].SetPoint(5,-27.0243,-173.259);
  cutDxDyArr[3].SetPoint(6,-36.8016,-100.043);
  cutDxDyArr[3].SetPoint(7,-36.5223,-39.0293);
  cutDxDyArr[3].SetPoint(8,-24.7895,-10.048);
  cutDxDyArr[3].SetPoint(9,-1.04453,-0.896);
  cutDxDyArr[3].SetPoint(10,-1.04453,11.3067);
  cutDxDyArr[3].SetPoint(11,-10.8219,25.0347);
  cutDxDyArr[3].SetPoint(12,-17.247,70.7947);
  cutDxDyArr[3].SetPoint(13,-13.8947,169.941);
  cutDxDyArr[3].SetPoint(14,-7.46964,247.733);
  cutDxDyArr[3].SetPoint(15,19.3482,272.139);
  cutDxDyArr[3].SetPoint(16,39.7409,226.379);
  cutDxDyArr[3].SetPoint(17,42.5344,177.568);
  cutDxDyArr[3].SetPoint(18,40.2996,137.909);
  cutDxDyArr[3].SetPoint(19,26.332,70.7947);
  cutDxDyArr[3].SetPoint(20,11.5263,21.984);
  cutDxDyArr[3].SetPoint(21,0.0728744,11.3067);
  cutDxDyArr[3].SetPoint(22,-0.206478,-0.896);
  cutDxDyArr[3].SetPoint(23,17.9514,-10.048);
  cutDxDyArr[3].SetPoint(24,21.0243,-37.504);
  cutDxDyArr[3].SetPoint(25,21.8623,-68.0107);

  cutDxDyArr[4].Set(32);
  cutDxDyArr[4].SetPoint(0,42.096,-149.264);
  cutDxDyArr[4].SetPoint(1,44.868,-185.478);
  cutDxDyArr[4].SetPoint(2,31.008,-200.799);
  cutDxDyArr[4].SetPoint(3,3.288,-200.799);
  cutDxDyArr[4].SetPoint(4,-20.428,-167.371);
  cutDxDyArr[4].SetPoint(5,-34.596,-100.515);
  cutDxDyArr[4].SetPoint(6,-35.212,-67.0865);
  cutDxDyArr[4].SetPoint(7,-32.132,-32.2655);
  cutDxDyArr[4].SetPoint(8,-24.74,-15.5515);
  cutDxDyArr[4].SetPoint(9,-11.5377,-8.48539);
  cutDxDyArr[4].SetPoint(10,2.31842,-0.711981);
  cutDxDyArr[4].SetPoint(11,2.31842,15.2188);
  cutDxDyArr[4].SetPoint(12,-6.15297,20.5291);
  cutDxDyArr[4].SetPoint(13,-9.99974,39.9163);
  cutDxDyArr[4].SetPoint(14,-11.145,71.8616);
  cutDxDyArr[4].SetPoint(15,-6.45552,119.654);
  cutDxDyArr[4].SetPoint(16,3.17167,179.821);
  cutDxDyArr[4].SetPoint(17,10.8429,204.676);
  cutDxDyArr[4].SetPoint(18,22.384,240.731);
  cutDxDyArr[4].SetPoint(19,37.8939,270.364);
  cutDxDyArr[4].SetPoint(20,59.96,283.909);
  cutDxDyArr[4].SetPoint(21,76.9,233.767);
  cutDxDyArr[4].SetPoint(22,70.432,166.91);
  cutDxDyArr[4].SetPoint(23,53.492,100.054);
  cutDxDyArr[4].SetPoint(24,40.3164,78.625);
  cutDxDyArr[4].SetPoint(25,24.848,42.9477);
  cutDxDyArr[4].SetPoint(26,3.50013,12.6904);
  cutDxDyArr[4].SetPoint(27,3.8419,-3.44354);
  cutDxDyArr[4].SetPoint(28,14.684,-22.5157);
  cutDxDyArr[4].SetPoint(29,16.224,-48.9796);
  cutDxDyArr[4].SetPoint(30,19.92,-86.5862);
  cutDxDyArr[4].SetPoint(31,42.096,-149.264);

  cutDxDyArr[5].Set(27);
  cutDxDyArr[5].SetPoint(0,24.58,-66.7476);
  cutDxDyArr[5].SetPoint(1,22.6,-90.8835);
  cutDxDyArr[5].SetPoint(2,15.34,-106.243);
  cutDxDyArr[5].SetPoint(3,3.02,-99.6602);
  cutDxDyArr[5].SetPoint(4,-17.88,-41.5146);
  cutDxDyArr[5].SetPoint(5,-16.12,-17.3786);
  cutDxDyArr[5].SetPoint(6,-8.64,-5.31068);
  cutDxDyArr[5].SetPoint(7,1.7,-3.1165);
  cutDxDyArr[5].SetPoint(8,2.20189,16.3683);
  cutDxDyArr[5].SetPoint(9,-4.20361,16.3683);
  cutDxDyArr[5].SetPoint(10,-8.2,30.8932);
  cutDxDyArr[5].SetPoint(11,-9.74,68.1942);
  cutDxDyArr[5].SetPoint(12,0.16,119.757);
  cutDxDyArr[5].SetPoint(13,20.62,174.612);
  cutDxDyArr[5].SetPoint(14,32.94,206.427);
  cutDxDyArr[5].SetPoint(15,63.9335,261.41);
  cutDxDyArr[5].SetPoint(16,70.9202,255.991);
  cutDxDyArr[5].SetPoint(17,72.4728,227.09);
  cutDxDyArr[5].SetPoint(18,62.2,184.485);
  cutDxDyArr[5].SetPoint(19,43.72,107.689);
  cutDxDyArr[5].SetPoint(20,28.98,57.2233);
  cutDxDyArr[5].SetPoint(21,14.2122,29.9146);
  cutDxDyArr[5].SetPoint(22,3.00258,14.1106);
  cutDxDyArr[5].SetPoint(23,2.14,-3.1165);
  cutDxDyArr[5].SetPoint(24,12.04,-11.8932);
  cutDxDyArr[5].SetPoint(25,17.54,-32.7379);
  cutDxDyArr[5].SetPoint(26,24.58,-66.7476);

  cutDxDyArr[6].Set(27);
  cutDxDyArr[6].SetPoint(0,10.2197,-50.1176);
  cutDxDyArr[6].SetPoint(1,0.0901413,-90.5882);
  cutDxDyArr[6].SetPoint(2,-29.187,-139.353);
  cutDxDyArr[6].SetPoint(3,-73.6113,-190.078);
  cutDxDyArr[6].SetPoint(4,-79.5493,-168.157);
  cutDxDyArr[6].SetPoint(5,-78.1521,-137.804);
  cutDxDyArr[6].SetPoint(6,-25.878,-40.782);
  cutDxDyArr[6].SetPoint(7,-19.561,-18.3795);
  cutDxDyArr[6].SetPoint(8,-12.9431,-5.68481);
  cutDxDyArr[6].SetPoint(9,-2.11382,-1.20431);
  cutDxDyArr[6].SetPoint(10,-1.81301,11.4904);
  cutDxDyArr[6].SetPoint(11,-8.73171,25.6787);
  cutDxDyArr[6].SetPoint(12,-16.8537,49.5746);
  cutDxDyArr[6].SetPoint(13,-28.2846,103.341);
  cutDxDyArr[6].SetPoint(14,-39.3803,147.176);
  cutDxDyArr[6].SetPoint(15,-66.9746,228.118);
  cutDxDyArr[6].SetPoint(16,-68.3718,251.725);
  cutDxDyArr[6].SetPoint(17,-63.831,265.216);
  cutDxDyArr[6].SetPoint(18,-38.3324,228.118);
  cutDxDyArr[6].SetPoint(19,-13.8817,172.471);
  cutDxDyArr[6].SetPoint(20,3.60163,114.542);
  cutDxDyArr[6].SetPoint(21,9.87042,64.549);
  cutDxDyArr[6].SetPoint(22,6.37747,20.7059);
  cutDxDyArr[6].SetPoint(23,-0.308943,14.4774);
  cutDxDyArr[6].SetPoint(24,-0.00813029,-1.95106);
  cutDxDyArr[6].SetPoint(25,9.52113,-13.0196);
  cutDxDyArr[6].SetPoint(26,10.2197,-50.1176);

  cutDxDyArr[7].Set(29);
  cutDxDyArr[7].SetPoint(0,15.8182,-65.9598);
  cutDxDyArr[7].SetPoint(1,9.36028,-122.773);
  cutDxDyArr[7].SetPoint(2,-4.01683,-170.845);
  cutDxDyArr[7].SetPoint(3,-35.6633,-220.759);
  cutDxDyArr[7].SetPoint(4,-72.7475,-227.658);
  cutDxDyArr[7].SetPoint(5,-85.202,-212.362);
  cutDxDyArr[7].SetPoint(6,-84.7407,-186.141);
  cutDxDyArr[7].SetPoint(7,-42.7643,-92.1811);
  cutDxDyArr[7].SetPoint(8,-23.3712,-42.4205);
  cutDxDyArr[7].SetPoint(9,-16.9327,-17.8873);
  cutDxDyArr[7].SetPoint(10,-2.47465,-0.902564);
  cutDxDyArr[7].SetPoint(11,-3.70385,14.6667);
  cutDxDyArr[7].SetPoint(12,-10.6694,23.6308);
  cutDxDyArr[7].SetPoint(13,-16.4057,56.1846);
  cutDxDyArr[7].SetPoint(14,-26.2394,113.272);
  cutDxDyArr[7].SetPoint(15,-36.8925,148.656);
  cutDxDyArr[7].SetPoint(16,-49.1846,185.456);
  cutDxDyArr[7].SetPoint(17,-60.2929,229.03);
  cutDxDyArr[7].SetPoint(18,-60.2929,279.288);
  cutDxDyArr[7].SetPoint(19,-51.0673,290.213);
  cutDxDyArr[7].SetPoint(20,-11.3973,237.771);
  cutDxDyArr[7].SetPoint(21,5.72008,165.641);
  cutDxDyArr[7].SetPoint(22,10.6369,119.405);
  cutDxDyArr[7].SetPoint(23,11.0467,65.6205);
  cutDxDyArr[7].SetPoint(24,8.43772,28);
  cutDxDyArr[7].SetPoint(25,-0.0162272,7.58974);
  cutDxDyArr[7].SetPoint(26,-0.8357,-0.902564);
  cutDxDyArr[7].SetPoint(27,12.5892,-17.8873);
  cutDxDyArr[7].SetPoint(28,15.8182,-65.9598);

  cutDxDyArr[8].Set(26);
  cutDxDyArr[8].SetPoint(0,24.6815,-94.2084);
  cutDxDyArr[8].SetPoint(1,17.8855,-156.017);
  cutDxDyArr[8].SetPoint(2,7.8615,-190.011);
  cutDxDyArr[8].SetPoint(3,-3.5217,-200.828);
  cutDxDyArr[8].SetPoint(4,-15.7763,-185.18);
  cutDxDyArr[8].SetPoint(5,-20.1718,-166.833);
  cutDxDyArr[8].SetPoint(6,-24.0794,-115.841);
  cutDxDyArr[8].SetPoint(7,-24.759,-54.033);
  cutDxDyArr[8].SetPoint(8,-18.2034,-6.13178);
  cutDxDyArr[8].SetPoint(9,-1.21356,-1.22636);
  cutDxDyArr[8].SetPoint(10,-1.21356,11.0372);
  cutDxDyArr[8].SetPoint(11,-12.5263,24.7726);
  cutDxDyArr[8].SetPoint(12,-22.3804,75.7645);
  cutDxDyArr[8].SetPoint(13,-26.1182,128.302);
  cutDxDyArr[8].SetPoint(14,-26.2881,221.014);
  cutDxDyArr[8].SetPoint(15,-13.5457,251.918);
  cutDxDyArr[8].SetPoint(16,0.89566,251.918);
  cutDxDyArr[8].SetPoint(17,15.7763,199.896);
  cutDxDyArr[8].SetPoint(18,18.2034,136.126);
  cutDxDyArr[8].SetPoint(19,15.7763,57.6387);
  cutDxDyArr[8].SetPoint(20,12.5401,23.3008);
  cutDxDyArr[8].SetPoint(21,-0.40452,11.0372);
  cutDxDyArr[8].SetPoint(22,-0.40452,1.22636);
  cutDxDyArr[8].SetPoint(23,14.9673,-6.13178);
  cutDxDyArr[8].SetPoint(24,23.0577,-33.1116);
  cutDxDyArr[8].SetPoint(25,24.6815,-94.2084);

  cutDxDyArr[9].Set(26);
  cutDxDyArr[9].SetPoint(0,28.0162,-90.6667);
  cutDxDyArr[9].SetPoint(1,26.2672,-157.333);
  cutDxDyArr[9].SetPoint(2,15.5547,-172);
  cutDxDyArr[9].SetPoint(3,1.12551,-184);
  cutDxDyArr[9].SetPoint(4,-8.05668,-169.333);
  cutDxDyArr[9].SetPoint(5,-19.6437,-134.667);
  cutDxDyArr[9].SetPoint(6,-24.2348,-89.3333);
  cutDxDyArr[9].SetPoint(7,-23.5789,-60);
  cutDxDyArr[9].SetPoint(8,-16.8016,-5.33333);
  cutDxDyArr[9].SetPoint(9,-1.21356,-1.22636);
  cutDxDyArr[9].SetPoint(10,-1.21356,11.0372);
  cutDxDyArr[9].SetPoint(11,-10.113,13.4899);
  cutDxDyArr[9].SetPoint(12,-14.9673,35.5643);
  cutDxDyArr[9].SetPoint(13,-18.2058,117.174);
  cutDxDyArr[9].SetPoint(14,-14.0554,194.876);
  cutDxDyArr[9].SetPoint(15,-4.00713,233.727);
  cutDxDyArr[9].SetPoint(16,9.53621,235.346);
  cutDxDyArr[9].SetPoint(17,21.7689,206.208);
  cutDxDyArr[9].SetPoint(18,26.3562,154.407);
  cutDxDyArr[9].SetPoint(19,21.7689,88.0361);
  cutDxDyArr[9].SetPoint(20,12.5401,23.3008);
  cutDxDyArr[9].SetPoint(21,-0.40452,11.0372);
  cutDxDyArr[9].SetPoint(22,-0.40452,1.22636);
  cutDxDyArr[9].SetPoint(23,14.9673,-6.13178);
  cutDxDyArr[9].SetPoint(24,20.8016,-17.3333);
  cutDxDyArr[9].SetPoint(25,28.0162,-90.6667);

  cutDxDyArr[10].Set(30);
  cutDxDyArr[10].SetPoint(0,45.5459,-110.814);
  cutDxDyArr[10].SetPoint(1,67.7998,-150.932);
  cutDxDyArr[10].SetPoint(2,70.7025,-213.339);
  cutDxDyArr[10].SetPoint(3,58.608,-237.856);
  cutDxDyArr[10].SetPoint(4,27.264,-217);
  cutDxDyArr[10].SetPoint(5,-5.40464,-178.923);
  cutDxDyArr[10].SetPoint(6,-18.353,-135.986);
  cutDxDyArr[10].SetPoint(7,-21.0531,-46.411);
  cutDxDyArr[10].SetPoint(8,-17.8294,-19.4322);
  cutDxDyArr[10].SetPoint(9,1.80155,-0.564426);
  cutDxDyArr[10].SetPoint(10,1.80155,15.2395);
  cutDxDyArr[10].SetPoint(11,-4.60395,17.4972);
  cutDxDyArr[10].SetPoint(12,-11.0094,22.0126);
  cutDxDyArr[10].SetPoint(13,-14.2122,37.8165);
  cutDxDyArr[10].SetPoint(14,-17.4149,105.548);
  cutDxDyArr[10].SetPoint(15,-11.8101,177.794);
  cutDxDyArr[10].SetPoint(16,-1.12507,232.73);
  cutDxDyArr[10].SetPoint(17,23.0497,257.727);
  cutDxDyArr[10].SetPoint(18,43.751,261.894);
  cutDxDyArr[10].SetPoint(19,49.6014,228.563);
  cutDxDyArr[10].SetPoint(20,46.9012,191.067);
  cutDxDyArr[10].SetPoint(21,29.0973,116.525);
  cutDxDyArr[10].SetPoint(22,22.3244,83.0932);
  cutDxDyArr[10].SetPoint(23,10.2299,27.3729);
  cutDxDyArr[10].SetPoint(24,3.40292,12.9818);
  cutDxDyArr[10].SetPoint(25,3.40292,-2.82213);
  cutDxDyArr[10].SetPoint(26,14.6125,-11.8529);
  cutDxDyArr[10].SetPoint(27,22.096,-25.1429);
  cutDxDyArr[10].SetPoint(28,30.0649,-68.4661);
  cutDxDyArr[10].SetPoint(29,45.5459,-110.814);

  cutDxDyArr[11].Set(31);
  cutDxDyArr[11].SetPoint(0,51.0438,-97.6457);
  cutDxDyArr[11].SetPoint(1,67.28,-129.355);
  cutDxDyArr[11].SetPoint(2,67.3425,-150.363);
  cutDxDyArr[11].SetPoint(3,62.949,-162.512);
  cutDxDyArr[11].SetPoint(4,12.384,-114.925);
  cutDxDyArr[11].SetPoint(5,-8.348,-78.1935);
  cutDxDyArr[11].SetPoint(6,-13.7359,-53.1689);
  cutDxDyArr[11].SetPoint(7,-14.6125,-25.3992);
  cutDxDyArr[11].SetPoint(8,-10.1,-6.04301);
  cutDxDyArr[11].SetPoint(9,1.872,-3.41935);
  cutDxDyArr[11].SetPoint(10,2.20189,15.2395);
  cutDxDyArr[11].SetPoint(11,-4.20361,17.4972);
  cutDxDyArr[11].SetPoint(12,-10.9401,29.8508);
  cutDxDyArr[11].SetPoint(13,-11.3395,68.3233);
  cutDxDyArr[11].SetPoint(14,-8.348,101.527);
  cutDxDyArr[11].SetPoint(15,1.4012,141.671);
  cutDxDyArr[11].SetPoint(16,19.8138,195.89);
  cutDxDyArr[11].SetPoint(17,58.1562,264.736);
  cutDxDyArr[11].SetPoint(18,62.949,256.636);
  cutDxDyArr[11].SetPoint(19,62.949,228.288);
  cutDxDyArr[11].SetPoint(20,49.4424,177.794);
  cutDxDyArr[11].SetPoint(21,37.7868,139.194);
  cutDxDyArr[11].SetPoint(22,30.997,114.895);
  cutDxDyArr[11].SetPoint(23,21.8108,76.4228);
  cutDxDyArr[11].SetPoint(24,11.027,29.8508);
  cutDxDyArr[11].SetPoint(25,3.332,16.2581);
  cutDxDyArr[11].SetPoint(26,3.04,-2.10753);
  cutDxDyArr[11].SetPoint(27,15.0129,-11.8529);
  cutDxDyArr[11].SetPoint(28,31.8273,-50.2339);
  cutDxDyArr[11].SetPoint(29,43.92,-83.4409);
  cutDxDyArr[11].SetPoint(30,51.0438,-97.6457);

  cutDxDyArr[12].Set(32);
  cutDxDyArr[12].SetPoint(0,8.91993,-62.0728);
  cutDxDyArr[12].SetPoint(1,4.51911,-73.9704);
  cutDxDyArr[12].SetPoint(2,-17.8028,-105.787);
  cutDxDyArr[12].SetPoint(3,-38.7726,-129.123);
  cutDxDyArr[12].SetPoint(4,-80.2052,-148.355);
  cutDxDyArr[12].SetPoint(5,-84.9859,-137.537);
  cutDxDyArr[12].SetPoint(6,-81.002,-115.901);
  cutDxDyArr[12].SetPoint(7,-57.9171,-88.9421);
  cutDxDyArr[12].SetPoint(8,-49.525,-73.1366);
  cutDxDyArr[12].SetPoint(9,-39.5694,-44.9852);
  cutDxDyArr[12].SetPoint(10,-26.6873,-18.76);
  cutDxDyArr[12].SetPoint(11,-15.4278,-5.9882);
  cutDxDyArr[12].SetPoint(12,-1.24706,-1.039);
  cutDxDyArr[12].SetPoint(13,-0.563526,9.62504);
  cutDxDyArr[12].SetPoint(14,-6.63581,19.1034);
  cutDxDyArr[12].SetPoint(15,-18.0563,53.8325);
  cutDxDyArr[12].SetPoint(16,-22.5714,88.5616);
  cutDxDyArr[12].SetPoint(17,-54.1771,178.581);
  cutDxDyArr[12].SetPoint(18,-61.8793,221.852);
  cutDxDyArr[12].SetPoint(19,-57.5493,239.467);
  cutDxDyArr[12].SetPoint(20,-20.1489,211.55);
  cutDxDyArr[12].SetPoint(21,-0.688129,180.545);
  cutDxDyArr[12].SetPoint(22,12.7567,132.72);
  cutDxDyArr[12].SetPoint(23,15.5581,108.48);
  cutDxDyArr[12].SetPoint(24,17.4257,50.6769);
  cutDxDyArr[12].SetPoint(25,14.6146,26.4377);
  cutDxDyArr[12].SetPoint(26,7.42134,13.7934);
  cutDxDyArr[12].SetPoint(27,0.632665,8.10161);
  cutDxDyArr[12].SetPoint(28,0.192753,-0.656174);
  cutDxDyArr[12].SetPoint(29,10.7211,-13.0533);
  cutDxDyArr[12].SetPoint(30,12.2213,-36.4631);
  cutDxDyArr[12].SetPoint(31,8.91993,-62.0728);

  cutDxDyArr[13].Set(32);
  cutDxDyArr[13].SetPoint(0,16.9084,-85.5055);
  cutDxDyArr[13].SetPoint(1,3.22065,-121.508);
  cutDxDyArr[13].SetPoint(2,-15.2981,-162.01);
  cutDxDyArr[13].SetPoint(3,-29.791,-177.761);
  cutDxDyArr[13].SetPoint(4,-75.1549,-213.33);
  cutDxDyArr[13].SetPoint(5,-87.3883,-200.032);
  cutDxDyArr[13].SetPoint(6,-80.507,-158.66);
  cutDxDyArr[13].SetPoint(7,-48.3944,-87.7361);
  cutDxDyArr[13].SetPoint(8,-39.2193,-56.7071);
  cutDxDyArr[13].SetPoint(9,-25.839,-34.5435);
  cutDxDyArr[13].SetPoint(10,-22.7807,-18.2902);
  cutDxDyArr[13].SetPoint(11,-14.7525,-3.51451);
  cutDxDyArr[13].SetPoint(12,-1.61033,0.);
  cutDxDyArr[13].SetPoint(13,-0.805163,11.2507);
  cutDxDyArr[13].SetPoint(14,-12.0775,24.7516);
  cutDxDyArr[13].SetPoint(15,-15.2981,51.7533);
  cutDxDyArr[13].SetPoint(16,-26.5704,103.507);
  cutDxDyArr[13].SetPoint(17,-35.0141,156.063);
  cutDxDyArr[13].SetPoint(18,-34.6318,219.599);
  cutDxDyArr[13].SetPoint(19,-26.6036,249.15);
  cutDxDyArr[13].SetPoint(20,-18.5755,252.106);
  cutDxDyArr[13].SetPoint(21,3.21529,218.121);
  cutDxDyArr[13].SetPoint(22,20.8008,157.541);
  cutDxDyArr[13].SetPoint(23,25.7706,98.438);
  cutDxDyArr[13].SetPoint(24,21.1831,51.1557);
  cutDxDyArr[13].SetPoint(25,13.6878,24.7516);
  cutDxDyArr[13].SetPoint(26,7.24647,15.751);
  cutDxDyArr[13].SetPoint(27,0.,9.00058);
  cutDxDyArr[13].SetPoint(28,0.,0.);
  cutDxDyArr[13].SetPoint(29,12.8826,-9.00058);
  cutDxDyArr[13].SetPoint(30,17.3602,-46.3641);
  cutDxDyArr[13].SetPoint(31,16.9084,-85.5055);

  cutDxDyArr[14].Set(26);
  cutDxDyArr[14].SetPoint(0,27.325,-81.8395);
  cutDxDyArr[14].SetPoint(1,20.0414,-111.721);
  cutDxDyArr[14].SetPoint(2,3.61489,-143.96);
  cutDxDyArr[14].SetPoint(3,-18.1715,-150.542);
  cutDxDyArr[14].SetPoint(4,-28.5145,-98.9512);
  cutDxDyArr[14].SetPoint(5,-28.9547,-48.1542);
  cutDxDyArr[14].SetPoint(6,-22.1327,-12.7145);
  cutDxDyArr[14].SetPoint(7,-16.1909,-4.44519);
  cutDxDyArr[14].SetPoint(8,0.627792,-0.727627);
  cutDxDyArr[14].SetPoint(9,0.529744,9.06588);
  cutDxDyArr[14].SetPoint(10,-8.19657,13.6362);
  cutDxDyArr[14].SetPoint(11,-14.3736,30.6116);
  cutDxDyArr[14].SetPoint(12,-24.9833,92.0517);
  cutDxDyArr[14].SetPoint(13,-27.6486,123.056);
  cutDxDyArr[14].SetPoint(14,-23.453,171.572);
  cutDxDyArr[14].SetPoint(15,-7.32504,191.803);
  cutDxDyArr[14].SetPoint(16,9.33656,196.97);
  cutDxDyArr[14].SetPoint(17,23.8608,131.407);
  cutDxDyArr[14].SetPoint(18,21.9943,81.2677);
  cutDxDyArr[14].SetPoint(19,14.178,28.6319);
  cutDxDyArr[14].SetPoint(20,1.80437,8.41298);
  cutDxDyArr[14].SetPoint(21,2.017,-0.982102);
  cutDxDyArr[14].SetPoint(22,13.2225,-6.72763);
  cutDxDyArr[14].SetPoint(23,23.9933,-36.0077);
  cutDxDyArr[14].SetPoint(24,28.6577,-56.2276);
  cutDxDyArr[14].SetPoint(25,27.325,-81.8395);

  cutDxDyArr[15].Set(26);
  cutDxDyArr[15].SetPoint(0,29.6766,-101.508);
  cutDxDyArr[15].SetPoint(1,20.7894,-134.135);
  cutDxDyArr[15].SetPoint(2,8.8551,-144.261);
  cutDxDyArr[15].SetPoint(3,-15.7752,-127.384);
  cutDxDyArr[15].SetPoint(4,-25.932,-90.2571);
  cutDxDyArr[15].SetPoint(5,-28.7251,-52.0046);
  cutDxDyArr[15].SetPoint(6,-23.9006,-12.6271);
  cutDxDyArr[15].SetPoint(7,-12.4742,-3.62651);
  cutDxDyArr[15].SetPoint(8,-0.0321159,0.873776);
  cutDxDyArr[15].SetPoint(9,-0.286036,12.1245);
  cutDxDyArr[15].SetPoint(10,-9.17326,16.6248);
  cutDxDyArr[15].SetPoint(11,-17.2987,34.6259);
  cutDxDyArr[15].SetPoint(12,-25.1702,86.3792);
  cutDxDyArr[15].SetPoint(13,-24.6624,153.884);
  cutDxDyArr[15].SetPoint(14,-4.34876,201.137);
  cutDxDyArr[15].SetPoint(15,7.5855,203.387);
  cutDxDyArr[15].SetPoint(16,19.7737,167.384);
  cutDxDyArr[15].SetPoint(17,22.5668,120.131);
  cutDxDyArr[15].SetPoint(18,17.9962,57.1274);
  cutDxDyArr[15].SetPoint(19,11.3943,22.2501);
  cutDxDyArr[15].SetPoint(20,1.49141,9.87435);
  cutDxDyArr[15].SetPoint(21,1.49141,1.99885);
  cutDxDyArr[15].SetPoint(22,19.5198,-7.00173);
  cutDxDyArr[15].SetPoint(23,27.6452,-28.3781);
  cutDxDyArr[15].SetPoint(24,32.2158,-72.2559);
  cutDxDyArr[15].SetPoint(25,29.6766,-101.508);

  cutDxDyArr[16].Set(27);
  cutDxDyArr[16].SetPoint(0,88.3277,-162.971);
  cutDxDyArr[16].SetPoint(1,88.7721,-184.472);
  cutDxDyArr[16].SetPoint(2,81.6616,-198.807);
  cutDxDyArr[16].SetPoint(3,47.3548,-192.283);
  cutDxDyArr[16].SetPoint(4,14.4913,-164.567);
  cutDxDyArr[16].SetPoint(5,-4.61538,-130.383);
  cutDxDyArr[16].SetPoint(6,-14.1687,-103.591);
  cutDxDyArr[16].SetPoint(7,-21.0471,-65.7113);
  cutDxDyArr[16].SetPoint(8,-20.1068,-30.3784);
  cutDxDyArr[16].SetPoint(9,-14.7739,-8.87686);
  cutDxDyArr[16].SetPoint(10,-1.55831,-0.115486);
  cutDxDyArr[16].SetPoint(11,-1.17618,8.19947);
  cutDxDyArr[16].SetPoint(12,-15.6973,27.601);
  cutDxDyArr[16].SetPoint(13,-24.1042,57.1654);
  cutDxDyArr[16].SetPoint(14,-25.4396,150.593);
  cutDxDyArr[16].SetPoint(15,-11.2187,202.554);
  cutDxDyArr[16].SetPoint(16,2.55781,222.264);
  cutDxDyArr[16].SetPoint(17,16.3343,224.056);
  cutDxDyArr[16].SetPoint(18,31.6873,187.433);
  cutDxDyArr[16].SetPoint(19,30.1108,139.842);
  cutDxDyArr[16].SetPoint(20,9.90571,21.1339);
  cutDxDyArr[16].SetPoint(21,1.49876,10.9711);
  cutDxDyArr[16].SetPoint(22,0.734491,-1.03937);
  cutDxDyArr[16].SetPoint(23,16.7787,-8.87686);
  cutDxDyArr[16].SetPoint(24,36.6551,-54.6247);
  cutDxDyArr[16].SetPoint(25,60.3474,-108.21);
  cutDxDyArr[16].SetPoint(26,88.3277,-162.971);

  cutDxDyArr[17].Set(26);
  cutDxDyArr[17].SetPoint(0,74.0717,-122.266);
  cutDxDyArr[17].SetPoint(1,72.4966,-141.655);
  cutDxDyArr[17].SetPoint(2,66.1962,-148.705);
  cutDxDyArr[17].SetPoint(3,29.8908,-123.429);
  cutDxDyArr[17].SetPoint(4,5.23573,-89.1429);
  cutDxDyArr[17].SetPoint(5,-7.27047,-62.8571);
  cutDxDyArr[17].SetPoint(6,-14.1339,-37.6581);
  cutDxDyArr[17].SetPoint(7,-13.7402,-12.9809);
  cutDxDyArr[17].SetPoint(8,-1.91067,-1.14286);
  cutDxDyArr[17].SetPoint(9,-1.55335,10.2857);
  cutDxDyArr[17].SetPoint(10,-13.7022,24);
  cutDxDyArr[17].SetPoint(11,-17.6779,68.1012);
  cutDxDyArr[17].SetPoint(12,-11.9156,144);
  cutDxDyArr[17].SetPoint(13,2.37717,187.429);
  cutDxDyArr[17].SetPoint(14,38.1092,242.286);
  cutDxDyArr[17].SetPoint(15,59.502,253.18);
  cutDxDyArr[17].SetPoint(16,62.6522,228.503);
  cutDxDyArr[17].SetPoint(17,59.5484,201.143);
  cutDxDyArr[17].SetPoint(18,28.1042,113.143);
  cutDxDyArr[17].SetPoint(19,10.5955,34.2857);
  cutDxDyArr[17].SetPoint(20,0.947891,11.4286);
  cutDxDyArr[17].SetPoint(21,0.233251,-1.14286);
  cutDxDyArr[17].SetPoint(22,18.8139,-8);
  cutDxDyArr[17].SetPoint(23,27.6062,-27.0822);
  cutDxDyArr[17].SetPoint(24,69.3464,-99.3511);
  cutDxDyArr[17].SetPoint(25,74.0717,-122.266);

  cutDxDyArr[18].Set(32);
  cutDxDyArr[18].SetPoint(0,4.39437,-58.1417);
  cutDxDyArr[18].SetPoint(1,-3.91147,-82.1732);
  cutDxDyArr[18].SetPoint(2,-24.9601,-104.419);
  cutDxDyArr[18].SetPoint(3,-44.7591,-118.663);
  cutDxDyArr[18].SetPoint(4,-70.8544,-138.284);
  cutDxDyArr[18].SetPoint(5,-100.81,-137.892);
  cutDxDyArr[18].SetPoint(6,-107.684,-118.663);
  cutDxDyArr[18].SetPoint(7,-103.983,-96.2294);
  cutDxDyArr[18].SetPoint(8,-55.5563,-50.2339);
  cutDxDyArr[18].SetPoint(9,-43.4788,-34.43);
  cutDxDyArr[18].SetPoint(10,-28.1807,-11.8529);
  cutDxDyArr[18].SetPoint(11,-16.1033,-5.07983);
  cutDxDyArr[18].SetPoint(12,-1.61033,-0.564426);
  cutDxDyArr[18].SetPoint(13,-0.805163,10.7241);
  cutDxDyArr[18].SetPoint(14,-7.37223,29.9738);
  cutDxDyArr[18].SetPoint(15,-10.4869,65.4488);
  cutDxDyArr[18].SetPoint(16,-13.9477,83.7585);
  cutDxDyArr[18].SetPoint(17,-25.0221,138.688);
  cutDxDyArr[18].SetPoint(18,-42.326,185.606);
  cutDxDyArr[18].SetPoint(19,-43.7015,221.049);
  cutDxDyArr[18].SetPoint(20,-35.7697,232.266);
  cutDxDyArr[18].SetPoint(21,13.7384,173.018);
  cutDxDyArr[18].SetPoint(22,23.4286,147.843);
  cutDxDyArr[18].SetPoint(23,28.2456,86.8772);
  cutDxDyArr[18].SetPoint(24,23.2214,47.8147);
  cutDxDyArr[18].SetPoint(25,14.429,22.7031);
  cutDxDyArr[18].SetPoint(26,7.24647,15.2395);
  cutDxDyArr[18].SetPoint(27,0,8.46639);
  cutDxDyArr[18].SetPoint(28,0,-0.564426);
  cutDxDyArr[18].SetPoint(29,8.89336,-7.79003);
  cutDxDyArr[18].SetPoint(30,10.9698,-28.3885);
  cutDxDyArr[18].SetPoint(31,4.39437,-58.1417);

  cutDxDyArr[19].Set(31);
  cutDxDyArr[19].SetPoint(0,20.2173,-53.6963);
  cutDxDyArr[19].SetPoint(1,15.835,-90.1675);
  cutDxDyArr[19].SetPoint(2,-11.2555,-140.147);
  cutDxDyArr[19].SetPoint(3,-40.2582,-160.861);
  cutDxDyArr[19].SetPoint(4,-67.4286,-172.565);
  cutDxDyArr[19].SetPoint(5,-79.7112,-156.346);
  cutDxDyArr[19].SetPoint(6,-73.8028,-86.1152);
  cutDxDyArr[19].SetPoint(7,-43.4788,-47.9762);
  cutDxDyArr[19].SetPoint(8,-38.3461,-32.0838);
  cutDxDyArr[19].SetPoint(9,-25.996,-14.5236);
  cutDxDyArr[19].SetPoint(10,-16.1033,-5.07983);
  cutDxDyArr[19].SetPoint(11,-1.61033,-0.564426);
  cutDxDyArr[19].SetPoint(12,-0.805163,10.7241);
  cutDxDyArr[19].SetPoint(13,-11.2723,31.0434);
  cutDxDyArr[19].SetPoint(14,-15.2981,60.3936);
  cutDxDyArr[19].SetPoint(15,-18.5188,82.9706);
  cutDxDyArr[19].SetPoint(16,-18.5188,125.867);
  cutDxDyArr[19].SetPoint(17,-15.2981,168.763);
  cutDxDyArr[19].SetPoint(18,-1.29577,201.602);
  cutDxDyArr[19].SetPoint(19,7.46881,205.654);
  cutDxDyArr[19].SetPoint(20,30.5962,184.567);
  cutDxDyArr[19].SetPoint(21,39.7384,147.571);
  cutDxDyArr[19].SetPoint(22,37.3481,84.0838);
  cutDxDyArr[19].SetPoint(23,25.7652,53.6205);
  cutDxDyArr[19].SetPoint(24,14.4929,22.0126);
  cutDxDyArr[19].SetPoint(25,7.24647,15.2395);
  cutDxDyArr[19].SetPoint(26,0.,8.46639);
  cutDxDyArr[19].SetPoint(27,0.,-0.564426);
  cutDxDyArr[19].SetPoint(28,10.4671,-2.82213);
  cutDxDyArr[19].SetPoint(29,17.827,-21.2775);
  cutDxDyArr[19].SetPoint(30,20.2173,-53.6963);

  cutDxDyArr[20].Set(27);
  cutDxDyArr[20].SetPoint(0,26.8118,-87.662);
  cutDxDyArr[20].SetPoint(1,17.9831,-117.476);
  cutDxDyArr[20].SetPoint(2,8.12955,-132.758);
  cutDxDyArr[20].SetPoint(3,-3.93658,-145.082);
  cutDxDyArr[20].SetPoint(4,-19.7674,-149.499);
  cutDxDyArr[20].SetPoint(5,-40.4693,-110.851);
  cutDxDyArr[20].SetPoint(6,-42.9049,-56.7437);
  cutDxDyArr[20].SetPoint(7,-30.1184,-22.5127);
  cutDxDyArr[20].SetPoint(8,-17.6364,-3.74085);
  cutDxDyArr[20].SetPoint(9,0.24635,0.);
  cutDxDyArr[20].SetPoint(10,0.24635,11.2507);
  cutDxDyArr[20].SetPoint(11,-8.62225,15.751);
  cutDxDyArr[20].SetPoint(12,-16.5055,29.2519);
  cutDxDyArr[20].SetPoint(13,-20.6808,73.5549);
  cutDxDyArr[20].SetPoint(14,-17.9408,144.225);
  cutDxDyArr[20].SetPoint(15,-9.11205,157.476);
  cutDxDyArr[20].SetPoint(16,13.1121,159.685);
  cutDxDyArr[20].SetPoint(17,23.463,147.538);
  cutDxDyArr[20].SetPoint(18,30.1607,118.828);
  cutDxDyArr[20].SetPoint(19,30.1607,86.8056);
  cutDxDyArr[20].SetPoint(20,17.6786,24.969);
  cutDxDyArr[20].SetPoint(21,1.23175,9.00058);
  cutDxDyArr[20].SetPoint(22,1.23175,0.);
  cutDxDyArr[20].SetPoint(23,17.3742,-7.05352);
  cutDxDyArr[20].SetPoint(24,24.8814,-29.2519);
  cutDxDyArr[20].SetPoint(25,30.4651,-58.9521);
  cutDxDyArr[20].SetPoint(26,26.8118,-87.662);

  cutDxDyArr[21].Set(26);
  cutDxDyArr[21].SetPoint(0,37.6658,-97.6428);
  cutDxDyArr[21].SetPoint(1,28.3131,-152.726);
  cutDxDyArr[21].SetPoint(2,18.9603,-155.021);
  cutDxDyArr[21].SetPoint(3,-16.5251,-121.742);
  cutDxDyArr[21].SetPoint(4,-33.5801,-76.9865);
  cutDxDyArr[21].SetPoint(5,-32.7548,-50.5923);
  cutDxDyArr[21].SetPoint(6,-26.1529,-31.0835);
  cutDxDyArr[21].SetPoint(7,-11.5736,-1.24662);
  cutDxDyArr[21].SetPoint(8,-0.295301,1.04853);
  cutDxDyArr[21].SetPoint(9,-0.295301,11.3767);
  cutDxDyArr[21].SetPoint(10,-9.37296,15.967);
  cutDxDyArr[21].SetPoint(11,-16.25,29.7379);
  cutDxDyArr[21].SetPoint(12,-30.8292,76.7884);
  cutDxDyArr[21].SetPoint(13,-34.9892,133.307);
  cutDxDyArr[21].SetPoint(14,-26.3261,167.395);
  cutDxDyArr[21].SetPoint(15,-10.7484,175.48);
  cutDxDyArr[21].SetPoint(16,13.4587,139.905);
  cutDxDyArr[21].SetPoint(17,20.3357,85.969);
  cutDxDyArr[21].SetPoint(18,18.4102,54.9845);
  cutDxDyArr[21].SetPoint(19,11.5332,15.967);
  cutDxDyArr[21].SetPoint(20,1.63026,9.08155);
  cutDxDyArr[21].SetPoint(21,1.63026,1.04853);
  cutDxDyArr[21].SetPoint(22,17.3099,-5.83691);
  cutDxDyArr[21].SetPoint(23,31.8891,-26.4932);
  cutDxDyArr[21].SetPoint(24,38.7661,-58.6253);
  cutDxDyArr[21].SetPoint(25,37.6658,-97.6428);

  cutDxDyArr[22].Set(27);
  cutDxDyArr[22].SetPoint(0,85.6911,-135.497);
  cutDxDyArr[22].SetPoint(1,88.7288,-157.086);
  cutDxDyArr[22].SetPoint(2,80.122,-180.336);
  cutDxDyArr[22].SetPoint(3,58.0596,-179.14);
  cutDxDyArr[22].SetPoint(4,37.2705,-167.43);
  cutDxDyArr[22].SetPoint(5,6.92804,-125);
  cutDxDyArr[22].SetPoint(6,-5.01241,-109.677);
  cutDxDyArr[22].SetPoint(7,-13.8313,-84.4194);
  cutDxDyArr[22].SetPoint(8,-20.0149,-51);
  cutDxDyArr[22].SetPoint(9,-15.5654,-10.9462);
  cutDxDyArr[22].SetPoint(10,-1.84119,-0.688172);
  cutDxDyArr[22].SetPoint(11,-2.34739,9.87097);
  cutDxDyArr[22].SetPoint(12,-18.6031,17.2855);
  cutDxDyArr[22].SetPoint(13,-28.6203,37.2473);
  cutDxDyArr[22].SetPoint(14,-35.9156,74.3226);
  cutDxDyArr[22].SetPoint(15,-42.9047,140.176);
  cutDxDyArr[22].SetPoint(16,-35.3104,180.033);
  cutDxDyArr[22].SetPoint(17,-20.0149,194.871);
  cutDxDyArr[22].SetPoint(18,2.95285,184.129);
  cutDxDyArr[22].SetPoint(19,14.3052,128.551);
  cutDxDyArr[22].SetPoint(20,9.7866,18.5806);
  cutDxDyArr[22].SetPoint(21,0.62531,8.34409);
  cutDxDyArr[22].SetPoint(22,0.272953,-1.29032);
  cutDxDyArr[22].SetPoint(23,20.3806,-5.96411);
  cutDxDyArr[22].SetPoint(24,35.0628,-29.2137);
  cutDxDyArr[22].SetPoint(25,50.2134,-55.7742);
  cutDxDyArr[22].SetPoint(26,85.6911,-135.497);

  cutDxDyArr[23].Set(27);
  cutDxDyArr[23].SetPoint(0,99.4932,-97.423);
  cutDxDyArr[23].SetPoint(1,100.281,-112.313);
  cutDxDyArr[23].SetPoint(2,92.2476,-128.629);
  cutDxDyArr[23].SetPoint(3,42.6641,-122.726);
  cutDxDyArr[23].SetPoint(4,14.8828,-97.6099);
  cutDxDyArr[23].SetPoint(5,-2.97657,-79.3437);
  cutDxDyArr[23].SetPoint(6,-8.92971,-61.0775);
  cutDxDyArr[23].SetPoint(7,-15.875,-35.9615);
  cutDxDyArr[23].SetPoint(8,-13.9029,-9.52564);
  cutDxDyArr[23].SetPoint(9,-2.29851,-0.787401);
  cutDxDyArr[23].SetPoint(10,-1.98438,9.70391);
  cutDxDyArr[23].SetPoint(11,-14.8828,16.5537);
  cutDxDyArr[23].SetPoint(12,-29.969,60.631);
  cutDxDyArr[23].SetPoint(13,-31.1165,130.788);
  cutDxDyArr[23].SetPoint(14,-24.2311,179.734);
  cutDxDyArr[23].SetPoint(15,-8.16507,207.47);
  cutDxDyArr[23].SetPoint(16,17.6553,210.734);
  cutDxDyArr[23].SetPoint(17,29.7049,204.207);
  cutDxDyArr[23].SetPoint(18,33.1476,169.945);
  cutDxDyArr[23].SetPoint(19,15.934,101.42);
  cutDxDyArr[23].SetPoint(20,6.1796,23.1054);
  cutDxDyArr[23].SetPoint(21,0.0398016,8.66142);
  cutDxDyArr[23].SetPoint(22,-0.42786,-1.41732);
  cutDxDyArr[23].SetPoint(23,22.4876,-8.97638);
  cutDxDyArr[23].SetPoint(24,35.7188,-24.5452);
  cutDxDyArr[23].SetPoint(25,61.8369,-61.7352);
  cutDxDyArr[23].SetPoint(26,99.4932,-97.423);

  cutDxDyArr[24].Set(26);
  cutDxDyArr[24].SetPoint(0,46.0375,161.099);
  cutDxDyArr[24].SetPoint(1,54.2521,123.035);
  cutDxDyArr[24].SetPoint(2,49.3387,71.4672);
  cutDxDyArr[24].SetPoint(3,36.7708,40.306);
  cutDxDyArr[24].SetPoint(4,11.1079,10.6652);
  cutDxDyArr[24].SetPoint(5,0.877232,7.32813);
  cutDxDyArr[24].SetPoint(6,-0.517857,0.654038);
  cutDxDyArr[24].SetPoint(7,6.65417,-12.5714);
  cutDxDyArr[24].SetPoint(8,6.075,-44.5283);
  cutDxDyArr[24].SetPoint(9,-11.8792,-86.3181);
  cutDxDyArr[24].SetPoint(10,-64.89,-134.412);
  cutDxDyArr[24].SetPoint(11,-105.61,-164.548);
  cutDxDyArr[24].SetPoint(12,-134.81,-160.752);
  cutDxDyArr[24].SetPoint(13,-149.773,-153.061);
  cutDxDyArr[24].SetPoint(14,-149.773,-124.217);
  cutDxDyArr[24].SetPoint(15,-66.9,-33.4663);
  cutDxDyArr[24].SetPoint(16,-26.3583,-6.42588);
  cutDxDyArr[24].SetPoint(17,-2.84301,-0.680781);
  cutDxDyArr[24].SetPoint(18,-0.517857,12.6674);
  cutDxDyArr[24].SetPoint(19,-2.6125,38.8329);
  cutDxDyArr[24].SetPoint(20,-9.11694,89.2243);
  cutDxDyArr[24].SetPoint(21,-20.3395,168.063);
  cutDxDyArr[24].SetPoint(22,-15.1023,193.061);
  cutDxDyArr[24].SetPoint(23,-3.87973,210.367);
  cutDxDyArr[24].SetPoint(24,23.0545,198.829);
  cutDxDyArr[24].SetPoint(25,46.0375,161.099);

  cutDxDyArr[25].Set(25);
  cutDxDyArr[25].SetPoint(0,53.975,87.7037);
  cutDxDyArr[25].SetPoint(1,37.8,44.6667);
  cutDxDyArr[25].SetPoint(2,13.65,10);
  cutDxDyArr[25].SetPoint(3,0.7,7.33333);
  cutDxDyArr[25].SetPoint(4,0,-1.33333);
  cutDxDyArr[25].SetPoint(5,14.3186,-11.3832);
  cutDxDyArr[25].SetPoint(6,16.7263,-42.1495);
  cutDxDyArr[25].SetPoint(7,10.1052,-84.4533);
  cutDxDyArr[25].SetPoint(8,-18.1847,-126.757);
  cutDxDyArr[25].SetPoint(9,-49.1667,-155.153);
  cutDxDyArr[25].SetPoint(10,-91.6181,-190.213);
  cutDxDyArr[25].SetPoint(11,-100.647,-176.752);
  cutDxDyArr[25].SetPoint(12,-97.0354,-140.217);
  cutDxDyArr[25].SetPoint(13,-54.9014,-38.3037);
  cutDxDyArr[25].SetPoint(14,-27.1375,-8.2963);
  cutDxDyArr[25].SetPoint(15,-2.1,0);
  cutDxDyArr[25].SetPoint(16,-1.4,8.66667);
  cutDxDyArr[25].SetPoint(17,-8.75,22.6667);
  cutDxDyArr[25].SetPoint(18,-7.95216,88.6075);
  cutDxDyArr[25].SetPoint(19,1.67845,146.294);
  cutDxDyArr[25].SetPoint(20,18.532,177.061);
  cutDxDyArr[25].SetPoint(21,32.978,184.752);
  cutDxDyArr[25].SetPoint(22,43.8124,184.752);
  cutDxDyArr[25].SetPoint(23,56.1208,155.259);
  cutDxDyArr[25].SetPoint(24,53.975,87.7037);

  cutDxDyArr[26].Set(25);
  cutDxDyArr[26].SetPoint(0,-1.20609,-175.525);
  cutDxDyArr[26].SetPoint(1,-40.9017,-168.651);
  cutDxDyArr[26].SetPoint(2,-60.5907,-98.0282);
  cutDxDyArr[26].SetPoint(3,-60.5907,-70.5292);
  cutDxDyArr[26].SetPoint(4,-34.868,-19.9061);
  cutDxDyArr[26].SetPoint(5,-19.3073,-6.15661);
  cutDxDyArr[26].SetPoint(6,-3.11148,0.718131);
  cutDxDyArr[26].SetPoint(7,-1.20609,6.9679);
  cutDxDyArr[26].SetPoint(8,-20.8951,34.4464);
  cutDxDyArr[26].SetPoint(9,-31.0572,82.6507);
  cutDxDyArr[26].SetPoint(10,-29.1518,148.759);
  cutDxDyArr[26].SetPoint(11,-25.6586,166.962);
  cutDxDyArr[26].SetPoint(12,26.1045,179.462);
  cutDxDyArr[26].SetPoint(13,51.5097,153.837);
  cutDxDyArr[26].SetPoint(14,54.6853,118.214);
  cutDxDyArr[26].SetPoint(15,27.6923,36.3418);
  cutDxDyArr[26].SetPoint(16,10.5438,9.46781);
  cutDxDyArr[26].SetPoint(17,0.6993,7.59288);
  cutDxDyArr[26].SetPoint(18,0.0641709,-1.1568);
  cutDxDyArr[26].SetPoint(19,13.0843,-5.53164);
  cutDxDyArr[26].SetPoint(20,34.6787,-31.7807);
  cutDxDyArr[26].SetPoint(21,38.4895,-63.0295);
  cutDxDyArr[26].SetPoint(22,38.1719,-94.2784);
  cutDxDyArr[26].SetPoint(23,29.5977,-127.402);
  cutDxDyArr[26].SetPoint(24,-1.20609,-175.525);

  cutDxDyArr[27].Set(25);
  cutDxDyArr[27].SetPoint(0,-7.8755,-157.328);
  cutDxDyArr[27].SetPoint(1,-20.8632,-141.701);
  cutDxDyArr[27].SetPoint(2,-35.2839,-108.89);
  cutDxDyArr[27].SetPoint(3,-39.0458,-49.3436);
  cutDxDyArr[27].SetPoint(4,-32.149,-25.0389);
  cutDxDyArr[27].SetPoint(5,-24.3116,-12.8866);
  cutDxDyArr[27].SetPoint(6,-2.25014,0.);
  cutDxDyArr[27].SetPoint(7,-2.25014,5.82696);
  cutDxDyArr[27].SetPoint(8,-38.1053,40.5836);
  cutDxDyArr[27].SetPoint(9,-52.526,91.6234);
  cutDxDyArr[27].SetPoint(10,-46.2562,135.372);
  cutDxDyArr[27].SetPoint(11,-39.0458,159.676);
  cutDxDyArr[27].SetPoint(12,5.47024,175.474);
  cutDxDyArr[27].SetPoint(13,18.3235,134.157);
  cutDxDyArr[27].SetPoint(14,22.3989,81.9016);
  cutDxDyArr[27].SetPoint(15,15.8155,35.7227);
  cutDxDyArr[27].SetPoint(16,8.91867,15.0638);
  cutDxDyArr[27].SetPoint(17,0.,8.74044);
  cutDxDyArr[27].SetPoint(18,0.,0.);
  cutDxDyArr[27].SetPoint(19,28.3553,-8.02567);
  cutDxDyArr[27].SetPoint(20,55.9427,-48.1284);
  cutDxDyArr[27].SetPoint(21,56.8832,-91.8767);
  cutDxDyArr[27].SetPoint(22,43.8778,-136.934);
  cutDxDyArr[27].SetPoint(23,15.502,-178.158);
  cutDxDyArr[27].SetPoint(24,-7.8755,-157.328);

  cutDxDyArr[28].Set(24);
  cutDxDyArr[28].SetPoint(0,100.798,-169.619);
  cutDxDyArr[28].SetPoint(1,74.2692,-185.22);
  cutDxDyArr[28].SetPoint(2,40.8461,-172.152);
  cutDxDyArr[28].SetPoint(3,-24.5928,-76.9926);
  cutDxDyArr[28].SetPoint(4,-27.6224,-34.104);
  cutDxDyArr[28].SetPoint(5,-20.9573,-9.9792);
  cutDxDyArr[28].SetPoint(6,-2.1739,-1.64278);
  cutDxDyArr[28].SetPoint(7,-2.1739,8.0713);
  cutDxDyArr[28].SetPoint(8,-17.9277,14.7497);
  cutDxDyArr[28].SetPoint(9,-62.7984,88.6761);
  cutDxDyArr[28].SetPoint(10,-65.0091,138.948);
  cutDxDyArr[28].SetPoint(11,-59.1137,178.819);
  cutDxDyArr[28].SetPoint(12,-37.743,196.154);
  cutDxDyArr[28].SetPoint(13,-14.2516,183.271);
  cutDxDyArr[28].SetPoint(14,-3.67228,146.603);
  cutDxDyArr[28].SetPoint(15,5.0971,94.5617);
  cutDxDyArr[28].SetPoint(16,11.6308,41.871);
  cutDxDyArr[28].SetPoint(17,0.249768,7.46417);
  cutDxDyArr[28].SetPoint(18,-0.356148,-0.428519);
  cutDxDyArr[28].SetPoint(19,25.0923,-5.89269);
  cutDxDyArr[28].SetPoint(20,51.7526,-30.0832);
  cutDxDyArr[28].SetPoint(21,83.8662,-81.0134);
  cutDxDyArr[28].SetPoint(22,103.746,-140.149);
  cutDxDyArr[28].SetPoint(23,100.798,-169.619);

  cutDxDyArr[29].Set(26);
  cutDxDyArr[29].SetPoint(0,121.479,-160.02);
  cutDxDyArr[29].SetPoint(1,86.3769,-158.286);
  cutDxDyArr[29].SetPoint(2,38.2524,-119.453);
  cutDxDyArr[29].SetPoint(3,-5.62536,-75.7505);
  cutDxDyArr[29].SetPoint(4,-16.8761,-34.9618);
  cutDxDyArr[29].SetPoint(5,-13.2915,-15.0769);
  cutDxDyArr[29].SetPoint(6,-1.63229,-1.37965);
  cutDxDyArr[29].SetPoint(7,-4.50029,11.6539);
  cutDxDyArr[29].SetPoint(8,-17.9552,15.7419);
  cutDxDyArr[29].SetPoint(9,-47.1031,57.9752);
  cutDxDyArr[29].SetPoint(10,-54.7071,108.676);
  cutDxDyArr[29].SetPoint(11,-43.2314,184.951);
  cutDxDyArr[29].SetPoint(12,-18.2548,205.753);
  cutDxDyArr[29].SetPoint(13,4.69672,197.086);
  cutDxDyArr[29].SetPoint(14,12.1222,172.816);
  cutDxDyArr[29].SetPoint(15,10.4274,123.496);
  cutDxDyArr[29].SetPoint(16,6.72185,82.6732);
  cutDxDyArr[29].SetPoint(17,3.37522,34.9618);
  cutDxDyArr[29].SetPoint(18,-1.12507,17.4809);
  cutDxDyArr[29].SetPoint(19,1.28251,-0.238213);
  cutDxDyArr[29].SetPoint(20,27.6482,-4.00282);
  cutDxDyArr[29].SetPoint(21,81.7309,-59.5931);
  cutDxDyArr[29].SetPoint(22,117.007,-84.491);
  cutDxDyArr[29].SetPoint(23,145.106,-121.882);
  cutDxDyArr[29].SetPoint(24,145.106,-149.619);
  cutDxDyArr[29].SetPoint(25,121.479,-160.02);

  cutDxDyArr[30].Set(28);
  cutDxDyArr[30].SetPoint(0,117.007,104.885);
  cutDxDyArr[30].SetPoint(1,91.1308,72.837);
  cutDxDyArr[30].SetPoint(2,63.004,46.6157);
  cutDxDyArr[30].SetPoint(3,40.5026,32.0483);
  cutDxDyArr[30].SetPoint(4,16.2322,14.5394);
  cutDxDyArr[30].SetPoint(5,4.6728,13.0855);
  cutDxDyArr[30].SetPoint(6,-0.41334,-1.93859);
  cutDxDyArr[30].SetPoint(7,3.749,-16.9626);
  cutDxDyArr[30].SetPoint(8,3.749,-45.6015);
  cutDxDyArr[30].SetPoint(9,-6.75043,-72.837);
  cutDxDyArr[30].SetPoint(10,-60.7539,-119.453);
  cutDxDyArr[30].SetPoint(11,-118.133,-151.501);
  cutDxDyArr[30].SetPoint(12,-197.864,-167.199);
  cutDxDyArr[30].SetPoint(13,-200.263,-104.885);
  cutDxDyArr[30].SetPoint(14,-175.511,-75.7505);
  cutDxDyArr[30].SetPoint(15,-79.5844,-22.849);
  cutDxDyArr[30].SetPoint(16,-4.11235,-1.45394);
  cutDxDyArr[30].SetPoint(17,2.36092,16.478);
  cutDxDyArr[30].SetPoint(18,0.97379,30.0481);
  cutDxDyArr[30].SetPoint(19,2.25014,69.9236);
  cutDxDyArr[30].SetPoint(20,4.50029,113.626);
  cutDxDyArr[30].SetPoint(21,12.3758,136.934);
  cutDxDyArr[30].SetPoint(22,36.0023,171.895);
  cutDxDyArr[30].SetPoint(23,69.7545,195.203);
  cutDxDyArr[30].SetPoint(24,82.1303,201.03);
  cutDxDyArr[30].SetPoint(25,97.8813,206.857);
  cutDxDyArr[30].SetPoint(26,120.383,157.328);
  cutDxDyArr[30].SetPoint(27,117.007,104.885);

  cutDxDyArr[31].Set(28);
  cutDxDyArr[31].SetPoint(0,117.007,104.885);
  cutDxDyArr[31].SetPoint(1,77.3422,58.4734);
  cutDxDyArr[31].SetPoint(2,30.513,23.0579);
  cutDxDyArr[31].SetPoint(3,10.1256,11.6539);
  cutDxDyArr[31].SetPoint(4,0.,8.74044);
  cutDxDyArr[31].SetPoint(5,-1.12507,2.91348);
  cutDxDyArr[31].SetPoint(6,17.1333,-13.3148);
  cutDxDyArr[31].SetPoint(7,18.3496,-60.2165);
  cutDxDyArr[31].SetPoint(8,15.3088,-84.1459);
  cutDxDyArr[31].SetPoint(9,-49.5032,-154.415);
  cutDxDyArr[31].SetPoint(10,-119.258,-192.29);
  cutDxDyArr[31].SetPoint(11,-174.386,-154.415);
  cutDxDyArr[31].SetPoint(12,-163.494,-116.69);
  cutDxDyArr[31].SetPoint(13,-103.507,-46.6157);
  cutDxDyArr[31].SetPoint(14,-70.8795,-17.4809);
  cutDxDyArr[31].SetPoint(15,-27.0017,-2.91348);
  cutDxDyArr[31].SetPoint(16,-3.37522,2.91348);
  cutDxDyArr[31].SetPoint(17,-1.12507,14.5674);
  cutDxDyArr[31].SetPoint(18,-5.62536,26.2213);
  cutDxDyArr[31].SetPoint(19,-3.37522,75.7505);
  cutDxDyArr[31].SetPoint(20,9.00058,116.539);
  cutDxDyArr[31].SetPoint(21,18.0012,142.761);
  cutDxDyArr[31].SetPoint(22,46.128,168.982);
  cutDxDyArr[31].SetPoint(23,70.8795,180.636);
  cutDxDyArr[31].SetPoint(24,88.8807,186.463);
  cutDxDyArr[31].SetPoint(25,101.256,186.463);
  cutDxDyArr[31].SetPoint(26,121.508,142.761);
  cutDxDyArr[31].SetPoint(27,117.007,104.885);

  cutDxDyArr[32].Set(25);
  cutDxDyArr[32].SetPoint(0,-42.3705,-177.505);
  cutDxDyArr[32].SetPoint(1,-66.8234,-152.366);
  cutDxDyArr[32].SetPoint(2,-70.9758,-108.374);
  cutDxDyArr[32].SetPoint(3,-66.8234,-58.0968);
  cutDxDyArr[32].SetPoint(4,-42.4065,-15.1545);
  cutDxDyArr[32].SetPoint(5,-2.69919,1.19241);
  cutDxDyArr[32].SetPoint(6,-3,8.09756);
  cutDxDyArr[32].SetPoint(7,-15.1492,53.4554);
  cutDxDyArr[32].SetPoint(8,-19.3016,108.446);
  cutDxDyArr[32].SetPoint(9,-11.4582,155.581);
  cutDxDyArr[32].SetPoint(10,3.76721,177.577);
  cutDxDyArr[32].SetPoint(11,21.2995,190.146);
  cutDxDyArr[32].SetPoint(12,41.1387,187.004);
  cutDxDyArr[32].SetPoint(13,61.4393,150.867);
  cutDxDyArr[32].SetPoint(14,63.2848,117.873);
  cutDxDyArr[32].SetPoint(15,53.5959,72.3093);
  cutDxDyArr[32].SetPoint(16,22.5014,20.3944);
  cutDxDyArr[32].SetPoint(17,0,8.74044);
  cutDxDyArr[32].SetPoint(18,0.162601,-0.498645);
  cutDxDyArr[32].SetPoint(19,21.7609,-18.8179);
  cutDxDyArr[32].SetPoint(20,37.4477,-53.3833);
  cutDxDyArr[32].SetPoint(21,30.9884,-119.372);
  cutDxDyArr[32].SetPoint(22,10.2265,-158.651);
  cutDxDyArr[32].SetPoint(23,-11.752,-174.892);
  cutDxDyArr[32].SetPoint(24,-42.3705,-177.505);

  cutDxDyArr[33].Set(27);
  cutDxDyArr[33].SetPoint(0,-2.25014,-174.809);
  cutDxDyArr[33].SetPoint(1,-27.0017,-139.847);
  cutDxDyArr[33].SetPoint(2,-42.7527,-104.885);
  cutDxDyArr[33].SetPoint(3,-45.9335,-71.2715);
  cutDxDyArr[33].SetPoint(4,-40.9515,-25.0313);
  cutDxDyArr[33].SetPoint(5,-20.5254,-4.80119);
  cutDxDyArr[33].SetPoint(6,-2.25014,0);
  cutDxDyArr[33].SetPoint(7,-2.25014,5.82696);
  cutDxDyArr[33].SetPoint(8,-41.6277,40.7887);
  cutDxDyArr[33].SetPoint(9,-60.7539,69.9236);
  cutDxDyArr[33].SetPoint(10,-75.8253,107.909);
  cutDxDyArr[33].SetPoint(11,-77.3199,168.6);
  cutDxDyArr[33].SetPoint(12,-59.3848,200.39);
  cutDxDyArr[33].SetPoint(13,-19.1262,195.203);
  cutDxDyArr[33].SetPoint(14,14.6259,180.636);
  cutDxDyArr[33].SetPoint(15,22.2927,168.688);
  cutDxDyArr[33].SetPoint(16,27.1707,131.097);
  cutDxDyArr[33].SetPoint(17,28.6341,100.043);
  cutDxDyArr[33].SetPoint(18,18.8322,39.994);
  cutDxDyArr[33].SetPoint(19,-1.12507,8.74044);
  cutDxDyArr[33].SetPoint(20,-1.12507,0);
  cutDxDyArr[33].SetPoint(21,32.6271,-8.74044);
  cutDxDyArr[33].SetPoint(22,79.1141,-48.1514);
  cutDxDyArr[33].SetPoint(23,87.5834,-92.9466);
  cutDxDyArr[33].SetPoint(24,84.0961,-139.187);
  cutDxDyArr[33].SetPoint(25,57.6916,-176.757);
  cutDxDyArr[33].SetPoint(26,-2.25014,-174.809);

  cutDxDyArr[34].Set(25);
  cutDxDyArr[34].SetPoint(0,143.862,-184.247);
  cutDxDyArr[34].SetPoint(1,89.2487,-189.936);
  cutDxDyArr[34].SetPoint(2,35.394,-161.494);
  cutDxDyArr[34].SetPoint(3,-16.1851,-88.9687);
  cutDxDyArr[34].SetPoint(4,-20.7362,-37.7742);
  cutDxDyArr[34].SetPoint(5,-17.7021,-15.021);
  cutDxDyArr[34].SetPoint(6,0,0);
  cutDxDyArr[34].SetPoint(7,-3.37522,8.74044);
  cutDxDyArr[34].SetPoint(8,-25.8767,14.5674);
  cutDxDyArr[34].SetPoint(9,-117.007,93.2314);
  cutDxDyArr[34].SetPoint(10,-131.48,157.049);
  cutDxDyArr[34].SetPoint(11,-123.136,191.179);
  cutDxDyArr[34].SetPoint(12,-100.131,206.857);
  cutDxDyArr[34].SetPoint(13,-55.9283,185.519);
  cutDxDyArr[34].SetPoint(14,-19.9777,137.141);
  cutDxDyArr[34].SetPoint(15,-4.04884,84.5239);
  cutDxDyArr[34].SetPoint(16,2.25014,23.3079);
  cutDxDyArr[34].SetPoint(17,-2.25014,11.6539);
  cutDxDyArr[34].SetPoint(18,2.25014,0);
  cutDxDyArr[34].SetPoint(19,50.9686,-10.0749);
  cutDxDyArr[34].SetPoint(20,94.506,-37.8753);
  cutDxDyArr[34].SetPoint(21,138.552,-73.3259);
  cutDxDyArr[34].SetPoint(22,161.308,-111.722);
  cutDxDyArr[34].SetPoint(23,161.308,-162.916);
  cutDxDyArr[34].SetPoint(24,143.862,-184.247);

  cutDxDyArr[35].Set(26);
  cutDxDyArr[35].SetPoint(0,183.794,-154.258);
  cutDxDyArr[35].SetPoint(1,155.07,-162.033);
  cutDxDyArr[35].SetPoint(2,37.477,-99.8291);
  cutDxDyArr[35].SetPoint(3,11.4451,-75.2067);
  cutDxDyArr[35].SetPoint(4,-3.40807,-44.2481);
  cutDxDyArr[35].SetPoint(5,-5.8296,-21.0956);
  cutDxDyArr[35].SetPoint(6,-1.19283,-4.08269);
  cutDxDyArr[35].SetPoint(7,-5.62536,14.5674);
  cutDxDyArr[35].SetPoint(8,-38.565,26.9354);
  cutDxDyArr[35].SetPoint(9,-112.431,80.3036);
  cutDxDyArr[35].SetPoint(10,-133.077,128.253);
  cutDxDyArr[35].SetPoint(11,-127.691,169.722);
  cutDxDyArr[35].SetPoint(12,-107.534,204.641);
  cutDxDyArr[35].SetPoint(13,-67.5043,203.944);
  cutDxDyArr[35].SetPoint(14,-32.6271,174.809);
  cutDxDyArr[35].SetPoint(15,-19.1262,149.92);
  cutDxDyArr[35].SetPoint(16,-9.00058,90.6597);
  cutDxDyArr[35].SetPoint(17,-3.37522,40.7887);
  cutDxDyArr[35].SetPoint(18,-3.37522,17.4809);
  cutDxDyArr[35].SetPoint(19,1.47534,-2.34625);
  cutDxDyArr[35].SetPoint(20,61.879,-17.4809);
  cutDxDyArr[35].SetPoint(21,95.6311,-32.0483);
  cutDxDyArr[35].SetPoint(22,141.759,-52.4427);
  cutDxDyArr[35].SetPoint(23,179.985,-86.7835);
  cutDxDyArr[35].SetPoint(24,194.35,-126.439);
  cutDxDyArr[35].SetPoint(25,183.794,-154.258);
}

void HMdcTrackDSet::initDxDyCutApr12(void) {
  //--------------  Butterfly cut  (for kick dX dY on the project plane) ----------
  isDxDyInited = kTRUE;

  cutDxDyArr[0].Set(28);
  cutDxDyArr[0].SetPoint(0,17.6344,-18.4853);
  cutDxDyArr[0].SetPoint(1,17.6344,-28.3441);
  cutDxDyArr[0].SetPoint(2,-4.68763,-94.8912);
  cutDxDyArr[0].SetPoint(3,-26.1168,-107.215);
  cutDxDyArr[0].SetPoint(4,-30.5812,-70.2441);
  cutDxDyArr[0].SetPoint(5,-30.5812,-55.4559);
  cutDxDyArr[0].SetPoint(6,-28.7955,-38.2029);
  cutDxDyArr[0].SetPoint(7,-24.3311,-18.4853);
  cutDxDyArr[0].SetPoint(8,-18.0809,-3.69706);
  cutDxDyArr[0].SetPoint(9,-2.90187,1.23235);
  cutDxDyArr[0].SetPoint(10,-2.90187,16.0206);
  cutDxDyArr[0].SetPoint(11,-10.9378,18.4853);
  cutDxDyArr[0].SetPoint(12,-27.0097,38.2029);
  cutDxDyArr[0].SetPoint(13,-40.4029,65.3147);
  cutDxDyArr[0].SetPoint(14,-63.5246,168.972);
  cutDxDyArr[0].SetPoint(15,-65.3059,201.284);
  cutDxDyArr[0].SetPoint(16,-60.5558,240.399);
  cutDxDyArr[0].SetPoint(17,-45.7116,262.507);
  cutDxDyArr[0].SetPoint(18,-22.5453,178.691);
  cutDxDyArr[0].SetPoint(19,-1.17906,157.067);
  cutDxDyArr[0].SetPoint(20,9.59849,112.144);
  cutDxDyArr[0].SetPoint(21,14.0629,72.7088);
  cutDxDyArr[0].SetPoint(22,14.0629,45.5971);
  cutDxDyArr[0].SetPoint(23,5.13408,20.95);
  cutDxDyArr[0].SetPoint(24,-1.1161,16.0206);
  cutDxDyArr[0].SetPoint(25,-2.00899,-1.23235);
  cutDxDyArr[0].SetPoint(26,11.3843,-8.62647);
  cutDxDyArr[0].SetPoint(27,17.6344,-18.4853);

  cutDxDyArr[1].Set(28);
  cutDxDyArr[1].SetPoint(0,32.3448,-10.5835);
  cutDxDyArr[1].SetPoint(1,38.3588,-92.0874);
  cutDxDyArr[1].SetPoint(2,29.7522,-136.213);
  cutDxDyArr[1].SetPoint(3,21.6524,-163.903);
  cutDxDyArr[1].SetPoint(4,2.00899,-210.732);
  cutDxDyArr[1].SetPoint(5,-36.385,-205.803);
  cutDxDyArr[1].SetPoint(6,-42.9333,-151.67);
  cutDxDyArr[1].SetPoint(7,-40.5306,-49.5768);
  cutDxDyArr[1].SetPoint(8,-33.7402,-16.5379);
  cutDxDyArr[1].SetPoint(9,-16.4244,-0.805077);
  cutDxDyArr[1].SetPoint(10,-2.39024,0.443182);
  cutDxDyArr[1].SetPoint(11,-2.54649,8.45918);
  cutDxDyArr[1].SetPoint(12,-9.91878,10.9482);
  cutDxDyArr[1].SetPoint(13,-20.4987,29.5093);
  cutDxDyArr[1].SetPoint(14,-38.7574,48.7818);
  cutDxDyArr[1].SetPoint(15,-66.743,119.538);
  cutDxDyArr[1].SetPoint(16,-81.922,215.662);
  cutDxDyArr[1].SetPoint(17,-77.4576,296.997);
  cutDxDyArr[1].SetPoint(18,-50.6711,331.503);
  cutDxDyArr[1].SetPoint(19,-21.206,292.068);
  cutDxDyArr[1].SetPoint(20,-0.669662,210.732);
  cutDxDyArr[1].SetPoint(21,12.1981,111.008);
  cutDxDyArr[1].SetPoint(22,14.3664,68.6941);
  cutDxDyArr[1].SetPoint(23,8.29511,23.8913);
  cutDxDyArr[1].SetPoint(24,-0.595,8.95699);
  cutDxDyArr[1].SetPoint(25,-0.674269,1.87928);
  cutDxDyArr[1].SetPoint(26,15.8304,0.768203);
  cutDxDyArr[1].SetPoint(27,32.3448,-10.5835);

  cutDxDyArr[2].Set(26);
  cutDxDyArr[2].SetPoint(0,46.2279,-23.2213);
  cutDxDyArr[2].SetPoint(1,44.8778,-96.9062);
  cutDxDyArr[2].SetPoint(2,40.6025,-152.447);
  cutDxDyArr[2].SetPoint(3,5.7253,-207.48);
  cutDxDyArr[2].SetPoint(4,-3.7253,-204.144);
  cutDxDyArr[2].SetPoint(5,-26.5972,-157.736);
  cutDxDyArr[2].SetPoint(6,-37.4296,-98.5807);
  cutDxDyArr[2].SetPoint(7,-39.5865,-17.5216);
  cutDxDyArr[2].SetPoint(8,-23.9766,-3.25403);
  cutDxDyArr[2].SetPoint(9,-1.41322,-0.959578);
  cutDxDyArr[2].SetPoint(10,-1.07775,6.70669);
  cutDxDyArr[2].SetPoint(11,-24.3933,40.508);
  cutDxDyArr[2].SetPoint(12,-26.5739,62.7178);
  cutDxDyArr[2].SetPoint(13,-43.9745,112.144);
  cutDxDyArr[2].SetPoint(14,-53.2476,194.911);
  cutDxDyArr[2].SetPoint(15,-45.7602,242.773);
  cutDxDyArr[2].SetPoint(16,-27.9026,299.462);
  cutDxDyArr[2].SetPoint(17,-3.91586,324.037);
  cutDxDyArr[2].SetPoint(18,13.17,272.35);
  cutDxDyArr[2].SetPoint(19,23.4063,147.01);
  cutDxDyArr[2].SetPoint(20,15.8487,28.3441);
  cutDxDyArr[2].SetPoint(21,0.431897,7.40362);
  cutDxDyArr[2].SetPoint(22,0.549971,1.04694);
  cutDxDyArr[2].SetPoint(23,11.3507,0.735813);
  cutDxDyArr[2].SetPoint(24,39.4775,-5.79794);
  cutDxDyArr[2].SetPoint(25,46.2279,-23.2213);

  cutDxDyArr[3].Set(29);
  cutDxDyArr[3].SetPoint(0,39.2516,-28.2355);
  cutDxDyArr[3].SetPoint(1,45.9882,-65.4906);
  cutDxDyArr[3].SetPoint(2,39.2516,-169.413);
  cutDxDyArr[3].SetPoint(3,11.6312,-212.55);
  cutDxDyArr[3].SetPoint(4,-27.9158,-185.683);
  cutDxDyArr[3].SetPoint(5,-38.5569,-146.207);
  cutDxDyArr[3].SetPoint(6,-46.277,-95.3429);
  cutDxDyArr[3].SetPoint(7,-46.277,-57.385);
  cutDxDyArr[3].SetPoint(8,-40.4348,-26.8111);
  cutDxDyArr[3].SetPoint(9,-24.815,0.773844);
  cutDxDyArr[3].SetPoint(10,-4.46463,4.40457);
  cutDxDyArr[3].SetPoint(11,-4.32144,10.8812);
  cutDxDyArr[3].SetPoint(12,-11.2683,26.6525);
  cutDxDyArr[3].SetPoint(13,-23.0543,111.537);
  cutDxDyArr[3].SetPoint(14,-26.6259,178.232);
  cutDxDyArr[3].SetPoint(15,-25.5544,212.59);
  cutDxDyArr[3].SetPoint(16,-19.4828,257.053);
  cutDxDyArr[3].SetPoint(17,-13.4112,277.264);
  cutDxDyArr[3].SetPoint(18,11.5895,313.643);
  cutDxDyArr[3].SetPoint(19,35.8759,317.685);
  cutDxDyArr[3].SetPoint(20,41.2332,273.222);
  cutDxDyArr[3].SetPoint(21,44.4476,188.337);
  cutDxDyArr[3].SetPoint(22,36.2016,95.7351);
  cutDxDyArr[3].SetPoint(23,24.0304,35.9583);
  cutDxDyArr[3].SetPoint(24,14.007,19.7546);
  cutDxDyArr[3].SetPoint(25,-0.312088,4.40457);
  cutDxDyArr[3].SetPoint(26,8.09465,3.66402);
  cutDxDyArr[3].SetPoint(27,31.3407,0.369325);
  cutDxDyArr[3].SetPoint(28,39.2516,-28.2355);

  cutDxDyArr[4].Set(32);
  cutDxDyArr[4].SetPoint(0,36.7621,-90.9319);
  cutDxDyArr[4].SetPoint(1,36.9912,-136.97);
  cutDxDyArr[4].SetPoint(2,29.8889,-200.446);
  cutDxDyArr[4].SetPoint(3,7.14537,-208.431);
  cutDxDyArr[4].SetPoint(4,-12.2325,-198.486);
  cutDxDyArr[4].SetPoint(5,-34.7188,-146.735);
  cutDxDyArr[4].SetPoint(6,-44.4402,-82.7575);
  cutDxDyArr[4].SetPoint(7,-44.7995,-19.0848);
  cutDxDyArr[4].SetPoint(8,-33.3688,-7.59159);
  cutDxDyArr[4].SetPoint(9,-19.6784,-6.70745);
  cutDxDyArr[4].SetPoint(10,2.31842,-0.711981);
  cutDxDyArr[4].SetPoint(11,1.8311,9.84922);
  cutDxDyArr[4].SetPoint(12,-4.23201,12.3806);
  cutDxDyArr[4].SetPoint(13,-16.1987,52.3769);
  cutDxDyArr[4].SetPoint(14,-16.1011,71.2423);
  cutDxDyArr[4].SetPoint(15,-12.1981,129.521);
  cutDxDyArr[4].SetPoint(16,-8.72878,169.484);
  cutDxDyArr[4].SetPoint(17,-0.958113,223.171);
  cutDxDyArr[4].SetPoint(18,24.4093,286.818);
  cutDxDyArr[4].SetPoint(19,49.7767,316.652);
  cutDxDyArr[4].SetPoint(20,77.2581,320.63);
  cutDxDyArr[4].SetPoint(21,90.2941,247.039);
  cutDxDyArr[4].SetPoint(22,89.5895,211.238);
  cutDxDyArr[4].SetPoint(23,64.222,107.812);
  cutDxDyArr[4].SetPoint(24,37.7311,56.4272);
  cutDxDyArr[4].SetPoint(25,27.9982,33.1382);
  cutDxDyArr[4].SetPoint(26,3.10755,10.3555);
  cutDxDyArr[4].SetPoint(27,3.42666,3.26755);
  cutDxDyArr[4].SetPoint(28,23.8498,0.736139);
  cutDxDyArr[4].SetPoint(29,28.1578,-11.9209);
  cutDxDyArr[4].SetPoint(30,31.2636,-30.9431);
  cutDxDyArr[4].SetPoint(31,36.7621,-90.9319);

  cutDxDyArr[5].Set(27);
  cutDxDyArr[5].SetPoint(0,36.4307,-81.8268);
  cutDxDyArr[5].SetPoint(1,31.7381,-99.8167);
  cutDxDyArr[5].SetPoint(2,20.3851,-104.204);
  cutDxDyArr[5].SetPoint(3,2.06897,-95.8677);
  cutDxDyArr[5].SetPoint(4,-21.848,-37.949);
  cutDxDyArr[5].SetPoint(5,-22.4535,-26.9796);
  cutDxDyArr[5].SetPoint(6,-12.3115,-7.2136);
  cutDxDyArr[5].SetPoint(7,-0.807119,-2.24078);
  cutDxDyArr[5].SetPoint(8,-0.855618,9.75954);
  cutDxDyArr[5].SetPoint(9,-10.0771,31.3609);
  cutDxDyArr[5].SetPoint(10,-12.4827,41.221);
  cutDxDyArr[5].SetPoint(11,-17.0853,94.5455);
  cutDxDyArr[5].SetPoint(12,-2.80987,146.061);
  cutDxDyArr[5].SetPoint(13,18.7773,197.576);
  cutDxDyArr[5].SetPoint(14,44.8908,235.353);
  cutDxDyArr[5].SetPoint(15,51.5062,209.596);
  cutDxDyArr[5].SetPoint(16,56.7289,156.364);
  cutDxDyArr[5].SetPoint(17,57.4253,125.455);
  cutDxDyArr[5].SetPoint(18,55.6844,101.414);
  cutDxDyArr[5].SetPoint(19,52.0678,87.799);
  cutDxDyArr[5].SetPoint(20,28.6131,44.0368);
  cutDxDyArr[5].SetPoint(21,25.8346,39.4544);
  cutDxDyArr[5].SetPoint(22,1.16073,21.0932);
  cutDxDyArr[5].SetPoint(23,0.857987,-1.09164);
  cutDxDyArr[5].SetPoint(24,19.7797,-9.42843);
  cutDxDyArr[5].SetPoint(25,31.5868,-23.9081);
  cutDxDyArr[5].SetPoint(26,36.4307,-81.8268);

  cutDxDyArr[6].Set(27);
  cutDxDyArr[6].SetPoint(0,19.9997,-14.0444);
  cutDxDyArr[6].SetPoint(1,7.95844,-74.5282);
  cutDxDyArr[6].SetPoint(2,-6.49104,-110.441);
  cutDxDyArr[6].SetPoint(3,-16.4681,-123.671);
  cutDxDyArr[6].SetPoint(4,-37.4542,-157.694);
  cutDxDyArr[6].SetPoint(5,-73.2339,-165.254);
  cutDxDyArr[6].SetPoint(6,-71.1697,-127.452);
  cutDxDyArr[6].SetPoint(7,-52.5918,-63.1875);
  cutDxDyArr[6].SetPoint(8,-28.5093,-6.48387);
  cutDxDyArr[6].SetPoint(9,-1.33051,1.07661);
  cutDxDyArr[6].SetPoint(10,-1.33051,12.4173);
  cutDxDyArr[6].SetPoint(11,-10.2754,16.1976);
  cutDxDyArr[6].SetPoint(12,-20.5965,29.4284);
  cutDxDyArr[6].SetPoint(13,-25.0689,71.0111);
  cutDxDyArr[6].SetPoint(14,-52.5918,133.385);
  cutDxDyArr[6].SetPoint(15,-69.1054,235.452);
  cutDxDyArr[6].SetPoint(16,-64.977,280.815);
  cutDxDyArr[6].SetPoint(17,-41.9267,258.133);
  cutDxDyArr[6].SetPoint(18,-11.6516,208.99);
  cutDxDyArr[6].SetPoint(19,9.67861,125.825);
  cutDxDyArr[6].SetPoint(20,24.1281,61.5605);
  cutDxDyArr[6].SetPoint(21,22.0639,48.3296);
  cutDxDyArr[6].SetPoint(22,10.3667,16.1976);
  cutDxDyArr[6].SetPoint(23,0.389664,12.4173);
  cutDxDyArr[6].SetPoint(24,-0.298406,-0.813509);
  cutDxDyArr[6].SetPoint(25,17.2474,-4.59375);
  cutDxDyArr[6].SetPoint(26,19.9997,-14.0444);

  cutDxDyArr[7].Set(31);
  cutDxDyArr[7].SetPoint(0,34.3389,-74.6233);
  cutDxDyArr[7].SetPoint(1,26.4102,-134.871);
  cutDxDyArr[7].SetPoint(2,-6.7821,-227.085);
  cutDxDyArr[7].SetPoint(3,-27.2087,-237.956);
  cutDxDyArr[7].SetPoint(4,-52.7419,-255.35);
  cutDxDyArr[7].SetPoint(5,-72.8037,-251.002);
  cutDxDyArr[7].SetPoint(6,-92.8655,-216.214);
  cutDxDyArr[7].SetPoint(7,-96.1483,-183.6);
  cutDxDyArr[7].SetPoint(8,-72.4728,-132.961);
  cutDxDyArr[7].SetPoint(9,-42.8363,-53.5757);
  cutDxDyArr[7].SetPoint(10,-34.4541,-8.06405);
  cutDxDyArr[7].SetPoint(11,-26.059,1.11654);
  cutDxDyArr[7].SetPoint(12,-4.13852,2.8379);
  cutDxDyArr[7].SetPoint(13,-4.83811,10.8709);
  cutDxDyArr[7].SetPoint(14,-12.9961,24.4015);
  cutDxDyArr[7].SetPoint(15,-25.3302,64.0325);
  cutDxDyArr[7].SetPoint(16,-36.3277,107.75);
  cutDxDyArr[7].SetPoint(17,-48,164.281);
  cutDxDyArr[7].SetPoint(18,-60.7666,192.546);
  cutDxDyArr[7].SetPoint(19,-66.9675,266.471);
  cutDxDyArr[7].SetPoint(20,-62.5904,331.699);
  cutDxDyArr[7].SetPoint(21,-29.3972,309.956);
  cutDxDyArr[7].SetPoint(22,4.16071,246.903);
  cutDxDyArr[7].SetPoint(23,12.5502,188.198);
  cutDxDyArr[7].SetPoint(24,20.1186,123.808);
  cutDxDyArr[7].SetPoint(25,19.2717,51.0379);
  cutDxDyArr[7].SetPoint(26,8.66643,11.4831);
  cutDxDyArr[7].SetPoint(27,-1.80656,8.00198);
  cutDxDyArr[7].SetPoint(28,-2.03975,2.26411);
  cutDxDyArr[7].SetPoint(29,29.4418,-9.78541);
  cutDxDyArr[7].SetPoint(30,34.3389,-74.6233);

  cutDxDyArr[8].Set(26);
  cutDxDyArr[8].SetPoint(0,38.0666,-72.9779);
  cutDxDyArr[8].SetPoint(1,30.1957,-148.73);
  cutDxDyArr[8].SetPoint(2,20.0211,-203.897);
  cutDxDyArr[8].SetPoint(3,-7.09701,-240.397);
  cutDxDyArr[8].SetPoint(4,-29.892,-226.128);
  cutDxDyArr[8].SetPoint(5,-34.3263,-194.698);
  cutDxDyArr[8].SetPoint(6,-42.3703,-128.145);
  cutDxDyArr[8].SetPoint(7,-40.2586,-36.7487);
  cutDxDyArr[8].SetPoint(8,-36.3541,-11.5871);
  cutDxDyArr[8].SetPoint(9,-1.94538,2.05135);
  cutDxDyArr[8].SetPoint(10,-1.57777,7.6504);
  cutDxDyArr[8].SetPoint(11,-20.3258,31.8463);
  cutDxDyArr[8].SetPoint(12,-34.4627,141.57);
  cutDxDyArr[8].SetPoint(13,-35.6432,215.235);
  cutDxDyArr[8].SetPoint(14,-28.56,250.022);
  cutDxDyArr[8].SetPoint(15,-6.91676,290.947);
  cutDxDyArr[8].SetPoint(16,22.2032,254.114);
  cutDxDyArr[8].SetPoint(17,28.4995,202.958);
  cutDxDyArr[8].SetPoint(18,29.2865,141.57);
  cutDxDyArr[8].SetPoint(19,22.0947,48.2477);
  cutDxDyArr[8].SetPoint(20,10.0019,16.1737);
  cutDxDyArr[8].SetPoint(21,-0.291143,6.65057);
  cutDxDyArr[8].SetPoint(22,-0.107338,2.65124);
  cutDxDyArr[8].SetPoint(23,24.1548,-3.34774);
  cutDxDyArr[8].SetPoint(24,31.7315,-34.2786);
  cutDxDyArr[8].SetPoint(25,38.0666,-72.9779);

  cutDxDyArr[9].Set(29);
  cutDxDyArr[9].SetPoint(0,37.8583,-93.5244);
  cutDxDyArr[9].SetPoint(1,33.3063,-190.587);
  cutDxDyArr[9].SetPoint(2,27.8641,-212.957);
  cutDxDyArr[9].SetPoint(3,8.05447,-229.227);
  cutDxDyArr[9].SetPoint(4,-23.6222,-195.867);
  cutDxDyArr[9].SetPoint(5,-28.8178,-182.317);
  cutDxDyArr[9].SetPoint(6,-38.7759,-142.512);
  cutDxDyArr[9].SetPoint(7,-40.9145,-107.357);
  cutDxDyArr[9].SetPoint(8,-40.2912,-45.9635);
  cutDxDyArr[9].SetPoint(9,-30.7661,-3.54974);
  cutDxDyArr[9].SetPoint(10,-2.40712,-0.629089);
  cutDxDyArr[9].SetPoint(11,-2.40712,8.12237);
  cutDxDyArr[9].SetPoint(12,-6.73674,12.0746);
  cutDxDyArr[9].SetPoint(13,-20.375,36.3529);
  cutDxDyArr[9].SetPoint(14,-29.8233,110.402);
  cutDxDyArr[9].SetPoint(15,-29.8233,183.615);
  cutDxDyArr[9].SetPoint(16,-28.7349,240.559);
  cutDxDyArr[9].SetPoint(17,-11.3198,287.334);
  cutDxDyArr[9].SetPoint(18,9.14291,299.536);
  cutDxDyArr[9].SetPoint(19,27.6464,242.592);
  cutDxDyArr[9].SetPoint(20,31.5648,155.143);
  cutDxDyArr[9].SetPoint(21,28.1313,74.8023);
  cutDxDyArr[9].SetPoint(22,20.3233,28.7306);
  cutDxDyArr[9].SetPoint(23,8.20043,11.7923);
  cutDxDyArr[9].SetPoint(24,-0.89176,7.55776);
  cutDxDyArr[9].SetPoint(25,-0.89176,-0.350279);
  cutDxDyArr[9].SetPoint(26,24.0035,-3.7497);
  cutDxDyArr[9].SetPoint(27,33.9616,-23.0967);
  cutDxDyArr[9].SetPoint(28,37.8583,-93.5244);

  cutDxDyArr[10].Set(30);
  cutDxDyArr[10].SetPoint(0,51.3653,-102.356);
  cutDxDyArr[10].SetPoint(1,82.3058,-177.932);
  cutDxDyArr[10].SetPoint(2,82.7914,-218.832);
  cutDxDyArr[10].SetPoint(3,60.9389,-246.099);
  cutDxDyArr[10].SetPoint(4,9.73973,-232.216);
  cutDxDyArr[10].SetPoint(5,-16.3714,-175.271);
  cutDxDyArr[10].SetPoint(6,-36.8595,-74.152);
  cutDxDyArr[10].SetPoint(7,-33.3549,-29.7595);
  cutDxDyArr[10].SetPoint(8,-22.0326,-6.36349);
  cutDxDyArr[10].SetPoint(9,1.80155,-0.564426);
  cutDxDyArr[10].SetPoint(10,0.636904,5.01598);
  cutDxDyArr[10].SetPoint(11,-8.36951,11.0973);
  cutDxDyArr[10].SetPoint(12,-18.2949,34.3169);
  cutDxDyArr[10].SetPoint(13,-20.5006,61.4064);
  cutDxDyArr[10].SetPoint(14,-22.2585,148.75);
  cutDxDyArr[10].SetPoint(15,-13.8454,219.707);
  cutDxDyArr[10].SetPoint(16,20.1474,283.329);
  cutDxDyArr[10].SetPoint(17,39.0863,306.052);
  cutDxDyArr[10].SetPoint(18,59.482,296.963);
  cutDxDyArr[10].SetPoint(19,59.9677,212.891);
  cutDxDyArr[10].SetPoint(20,50.6988,187.541);
  cutDxDyArr[10].SetPoint(21,37.9492,115.585);
  cutDxDyArr[10].SetPoint(22,25.9963,59.7653);
  cutDxDyArr[10].SetPoint(23,14.4222,30.447);
  cutDxDyArr[10].SetPoint(24,2.29114,11.0973);
  cutDxDyArr[10].SetPoint(25,2.65875,-0.512492);
  cutDxDyArr[10].SetPoint(26,21.3259,-3.48444);
  cutDxDyArr[10].SetPoint(27,34.04,-20.1612);
  cutDxDyArr[10].SetPoint(28,45.8797,-79.7715);
  cutDxDyArr[10].SetPoint(29,51.3653,-102.356);

  cutDxDyArr[11].Set(31);
  cutDxDyArr[11].SetPoint(0,55.8157,-92.6051);
  cutDxDyArr[11].SetPoint(1,78.7077,-164.523);
  cutDxDyArr[11].SetPoint(2,71.8757,-190.886);
  cutDxDyArr[11].SetPoint(3,34.2493,-153.583);
  cutDxDyArr[11].SetPoint(4,5.57587,-114.837);
  cutDxDyArr[11].SetPoint(5,-16.2356,-68.468);
  cutDxDyArr[11].SetPoint(6,-22.4777,-43.0401);
  cutDxDyArr[11].SetPoint(7,-22.1173,-17.0179);
  cutDxDyArr[11].SetPoint(8,-13.6716,-3.8578);
  cutDxDyArr[11].SetPoint(9,0.549704,-1.50869);
  cutDxDyArr[11].SetPoint(10,0.374069,6.11444);
  cutDxDyArr[11].SetPoint(11,-15.96,15.6422);
  cutDxDyArr[11].SetPoint(12,-20.3509,27.8166);
  cutDxDyArr[11].SetPoint(13,-20.7022,71.2211);
  cutDxDyArr[11].SetPoint(14,-17.892,101.922);
  cutDxDyArr[11].SetPoint(15,-8.40769,142.639);
  cutDxDyArr[11].SetPoint(16,8.96372,199.284);
  cutDxDyArr[11].SetPoint(17,49.3868,276.615);
  cutDxDyArr[11].SetPoint(18,61.9122,290.675);
  cutDxDyArr[11].SetPoint(19,65.0436,243.222);
  cutDxDyArr[11].SetPoint(20,52.2335,178.193);
  cutDxDyArr[11].SetPoint(21,40.562,125.468);
  cutDxDyArr[11].SetPoint(22,33.9204,72.7689);
  cutDxDyArr[11].SetPoint(23,21.9772,36.0967);
  cutDxDyArr[11].SetPoint(24,5.9944,15.2649);
  cutDxDyArr[11].SetPoint(25,1.42788,11.4773);
  cutDxDyArr[11].SetPoint(26,2.30606,1.35054);
  cutDxDyArr[11].SetPoint(27,20.4592,0.428492);
  cutDxDyArr[11].SetPoint(28,35.0763,-13.8095);
  cutDxDyArr[11].SetPoint(29,46.0128,-77.3606);
  cutDxDyArr[11].SetPoint(30,55.8157,-92.6051);

  cutDxDyArr[12].Set(32);
  cutDxDyArr[12].SetPoint(0,25.9712,-42.918);
  cutDxDyArr[12].SetPoint(1,16.1309,-71.3408);
  cutDxDyArr[12].SetPoint(2,-0.456915,-104.582);
  cutDxDyArr[12].SetPoint(3,-36.4441,-136.803);
  cutDxDyArr[12].SetPoint(4,-73.8473,-177.676);
  cutDxDyArr[12].SetPoint(5,-91.0603,-170.009);
  cutDxDyArr[12].SetPoint(6,-97.2393,-137.424);
  cutDxDyArr[12].SetPoint(7,-74.3993,-59.8944);
  cutDxDyArr[12].SetPoint(8,-62.3099,-39.6637);
  cutDxDyArr[12].SetPoint(9,-43.1917,-23.9583);
  cutDxDyArr[12].SetPoint(10,-38.4121,-11.4472);
  cutDxDyArr[12].SetPoint(11,-20.4185,-0.141846);
  cutDxDyArr[12].SetPoint(12,-1.30036,1.59633);
  cutDxDyArr[12].SetPoint(13,-1.01921,8.74671);
  cutDxDyArr[12].SetPoint(14,-8.89141,14.3021);
  cutDxDyArr[12].SetPoint(15,-26.3468,47.4711);
  cutDxDyArr[12].SetPoint(16,-65.4615,188.422);
  cutDxDyArr[12].SetPoint(17,-76.4954,238.257);
  cutDxDyArr[12].SetPoint(18,-72.9646,282.342);
  cutDxDyArr[12].SetPoint(19,-47.8071,272.758);
  cutDxDyArr[12].SetPoint(20,-20.0015,253.591);
  cutDxDyArr[12].SetPoint(21,2.94923,196.089);
  cutDxDyArr[12].SetPoint(22,25.9348,111.826);
  cutDxDyArr[12].SetPoint(23,26.8146,62.6336);
  cutDxDyArr[12].SetPoint(24,25.9348,45.5962);
  cutDxDyArr[12].SetPoint(25,19.8065,24.3296);
  cutDxDyArr[12].SetPoint(26,11.7632,11.8301);
  cutDxDyArr[12].SetPoint(27,0.105385,5.4135);
  cutDxDyArr[12].SetPoint(28,0.105385,2.08029);
  cutDxDyArr[12].SetPoint(29,15.2875,0.797753);
  cutDxDyArr[12].SetPoint(30,28.2204,-3.72754);
  cutDxDyArr[12].SetPoint(31,25.9712,-42.918);

  cutDxDyArr[13].Set(32);
  cutDxDyArr[13].SetPoint(0,38.2408,-52.461);
  cutDxDyArr[13].SetPoint(1,31.0884,-102.673);
  cutDxDyArr[13].SetPoint(2,-15.6956,-194.783);
  cutDxDyArr[13].SetPoint(3,-53.5907,-243.908);
  cutDxDyArr[13].SetPoint(4,-86.8074,-245.955);
  cutDxDyArr[13].SetPoint(5,-107.392,-221.393);
  cutDxDyArr[13].SetPoint(6,-114.41,-196.83);
  cutDxDyArr[13].SetPoint(7,-64.4114,-83.3395);
  cutDxDyArr[13].SetPoint(8,-52.7329,-50.5755);
  cutDxDyArr[13].SetPoint(9,-46.7432,-24.7431);
  cutDxDyArr[13].SetPoint(10,-44.0301,-7.15017);
  cutDxDyArr[13].SetPoint(11,-40.2032,-3.66941);
  cutDxDyArr[13].SetPoint(12,-12.5609,3.9692);
  cutDxDyArr[13].SetPoint(13,-10.6173,8.29774);
  cutDxDyArr[13].SetPoint(14,-6.29814,9.82546);
  cutDxDyArr[13].SetPoint(15,-20.1193,28.1581);
  cutDxDyArr[13].SetPoint(16,-29.8023,52.098);
  cutDxDyArr[13].SetPoint(17,-49.848,218.689);
  cutDxDyArr[13].SetPoint(18,-41.8947,261.674);
  cutDxDyArr[13].SetPoint(19,-24.1168,273.955);
  cutDxDyArr[13].SetPoint(20,8.16425,243.252);
  cutDxDyArr[13].SetPoint(21,27.2589,207.424);
  cutDxDyArr[13].SetPoint(22,39.1435,152.61);
  cutDxDyArr[13].SetPoint(23,35.3234,95.8389);
  cutDxDyArr[13].SetPoint(24,29.9417,60.001);
  cutDxDyArr[13].SetPoint(25,22.208,28.1581);
  cutDxDyArr[13].SetPoint(26,13.7857,11.6078);
  cutDxDyArr[13].SetPoint(27,-2.41094,6.5154);
  cutDxDyArr[13].SetPoint(28,-4.35454,2.18685);
  cutDxDyArr[13].SetPoint(29,30.2823,-3.76503);
  cutDxDyArr[13].SetPoint(30,34.1043,-11.8081);
  cutDxDyArr[13].SetPoint(31,38.2408,-52.461);

  cutDxDyArr[14].Set(30);
  cutDxDyArr[14].SetPoint(0,36.1985,-134.761);
  cutDxDyArr[14].SetPoint(1,5.46555,-202.576);
  cutDxDyArr[14].SetPoint(2,-23.9687,-215.413);
  cutDxDyArr[14].SetPoint(3,-32.2707,-187.904);
  cutDxDyArr[14].SetPoint(4,-40.1546,-151.311);
  cutDxDyArr[14].SetPoint(5,-46.181,-116.301);
  cutDxDyArr[14].SetPoint(6,-48.1898,-72.3788);
  cutDxDyArr[14].SetPoint(7,-44.2903,-20.1817);
  cutDxDyArr[14].SetPoint(8,-42.0452,-10.6334);
  cutDxDyArr[14].SetPoint(9,-36.5088,0.48357);
  cutDxDyArr[14].SetPoint(10,-10.2274,1.75667);
  cutDxDyArr[14].SetPoint(11,-10.9282,6.21252);
  cutDxDyArr[14].SetPoint(12,-16.0093,8.24949);
  cutDxDyArr[14].SetPoint(13,-18.1539,12.2749);
  cutDxDyArr[14].SetPoint(14,-34.2833,61.5088);
  cutDxDyArr[14].SetPoint(15,-34.5348,171.544);
  cutDxDyArr[14].SetPoint(16,-27.2392,217.392);
  cutDxDyArr[14].SetPoint(17,-19.9435,246.735);
  cutDxDyArr[14].SetPoint(18,6.22027,254.07);
  cutDxDyArr[14].SetPoint(19,23.3274,215.558);
  cutDxDyArr[14].SetPoint(20,31.8809,134.866);
  cutDxDyArr[14].SetPoint(21,32.1733,108.043);
  cutDxDyArr[14].SetPoint(22,26.1564,44.3888);
  cutDxDyArr[14].SetPoint(23,11.849,12.0688);
  cutDxDyArr[14].SetPoint(24,5.54145,7.99487);
  cutDxDyArr[14].SetPoint(25,-4.62069,4.93942);
  cutDxDyArr[14].SetPoint(26,27.5156,4.16793);
  cutDxDyArr[14].SetPoint(27,42.3853,-19.696);
  cutDxDyArr[14].SetPoint(28,44.2173,-70.4692);
  cutDxDyArr[14].SetPoint(29,36.1985,-134.761);

  cutDxDyArr[15].Set(26);
  cutDxDyArr[15].SetPoint(0,47.3882,-158.87);
  cutDxDyArr[15].SetPoint(1,33.6354,-214.905);
  cutDxDyArr[15].SetPoint(2,19.2118,-214.905);
  cutDxDyArr[15].SetPoint(3,-18.2707,-162.571);
  cutDxDyArr[15].SetPoint(4,-45.9925,-119.542);
  cutDxDyArr[15].SetPoint(5,-51.7564,-54.4564);
  cutDxDyArr[15].SetPoint(6,-34.2222,0.0938947);
  cutDxDyArr[15].SetPoint(7,-8.01416,4.26041);
  cutDxDyArr[15].SetPoint(8,-0.582027,4.67706);
  cutDxDyArr[15].SetPoint(9,-4.49368,8.21859);
  cutDxDyArr[15].SetPoint(10,-16.2286,15.7183);
  cutDxDyArr[15].SetPoint(11,-20.192,32.387);
  cutDxDyArr[15].SetPoint(12,-38.5012,118.532);
  cutDxDyArr[15].SetPoint(13,-40.4953,184.346);
  cutDxDyArr[15].SetPoint(14,-33.1157,208.861);
  cutDxDyArr[15].SetPoint(15,-4.60394,247.385);
  cutDxDyArr[15].SetPoint(16,13.8449,254.389);
  cutDxDyArr[15].SetPoint(17,31.9583,191.35);
  cutDxDyArr[15].SetPoint(18,36.0809,100.385);
  cutDxDyArr[15].SetPoint(19,16.0425,19.8848);
  cutDxDyArr[15].SetPoint(20,9.97942,12.1768);
  cutDxDyArr[15].SetPoint(21,0.200303,6.34366);
  cutDxDyArr[15].SetPoint(22,23.6702,-3.44764);
  cutDxDyArr[15].SetPoint(23,35.7963,-11.5723);
  cutDxDyArr[15].SetPoint(24,53.0923,-78.4161);
  cutDxDyArr[15].SetPoint(25,47.3882,-158.87);

  cutDxDyArr[16].Set(28);
  cutDxDyArr[16].SetPoint(0,76.5,-122.261);
  cutDxDyArr[16].SetPoint(1,101.879,-195.441);
  cutDxDyArr[16].SetPoint(2,112.983,-232.031);
  cutDxDyArr[16].SetPoint(3,102.408,-280.817);
  cutDxDyArr[16].SetPoint(4,23.0977,-223.9);
  cutDxDyArr[16].SetPoint(5,-24.455,-119.656);
  cutDxDyArr[16].SetPoint(6,-34.5276,-86.5403);
  cutDxDyArr[16].SetPoint(7,-42.6632,-33.5556);
  cutDxDyArr[16].SetPoint(8,-40.145,-14.4222);
  cutDxDyArr[16].SetPoint(9,-25.4817,-2.5752);
  cutDxDyArr[16].SetPoint(10,-0.300227,1.99204);
  cutDxDyArr[16].SetPoint(11,-12.0838,15.6938);
  cutDxDyArr[16].SetPoint(12,-18.0564,28.3209);
  cutDxDyArr[16].SetPoint(13,-33.477,60.6882);
  cutDxDyArr[16].SetPoint(14,-38.7643,172.491);
  cutDxDyArr[16].SetPoint(15,-20.2586,237.539);
  cutDxDyArr[16].SetPoint(16,14.1092,298.522);
  cutDxDyArr[16].SetPoint(17,40.0172,284.293);
  cutDxDyArr[16].SetPoint(18,48.477,223.31);
  cutDxDyArr[16].SetPoint(19,36.453,73.1149);
  cutDxDyArr[16].SetPoint(20,24.3969,36.112);
  cutDxDyArr[16].SetPoint(21,20.0386,28.0522);
  cutDxDyArr[16].SetPoint(22,4.0581,8.70858);
  cutDxDyArr[16].SetPoint(23,0.345451,5.48464);
  cutDxDyArr[16].SetPoint(24,20.0737,4.2659);
  cutDxDyArr[16].SetPoint(25,42.0784,-7.06326);
  cutDxDyArr[16].SetPoint(26,49.4109,-18.8376);
  cutDxDyArr[16].SetPoint(27,76.5,-122.261);

  cutDxDyArr[17].Set(28);
  cutDxDyArr[17].SetPoint(0,96.2201,-113.639);
  cutDxDyArr[17].SetPoint(1,103.147,-151.924);
  cutDxDyArr[17].SetPoint(2,104.687,-180.222);
  cutDxDyArr[17].SetPoint(3,93.911,-181.887);
  cutDxDyArr[17].SetPoint(4,5.92054,-124.003);
  cutDxDyArr[17].SetPoint(5,-22.4088,-63.0776);
  cutDxDyArr[17].SetPoint(6,-22.5581,-11.8333);
  cutDxDyArr[17].SetPoint(7,-18.0787,-3.38675);
  cutDxDyArr[17].SetPoint(8,-9.13072,-1.26387);
  cutDxDyArr[17].SetPoint(9,-7.92542,9.09413);
  cutDxDyArr[17].SetPoint(10,-22.4713,30.6233);
  cutDxDyArr[17].SetPoint(11,-28.127,58.9423);
  cutDxDyArr[17].SetPoint(12,-24.3565,94.5664);
  cutDxDyArr[17].SetPoint(13,-16.0615,143.986);
  cutDxDyArr[17].SetPoint(14,-5.12703,199.525);
  cutDxDyArr[17].SetPoint(15,23.9057,251.592);
  cutDxDyArr[17].SetPoint(16,66.1352,265.477);
  cutDxDyArr[17].SetPoint(17,71.791,242.914);
  cutDxDyArr[17].SetPoint(18,57.0861,166.548);
  cutDxDyArr[17].SetPoint(19,28.0565,46.4354);
  cutDxDyArr[17].SetPoint(20,17.7564,23.742);
  cutDxDyArr[17].SetPoint(21,-3.91798,6.39089);
  cutDxDyArr[17].SetPoint(22,-4.10361,2.8282);
  cutDxDyArr[17].SetPoint(23,19.9167,1.11904);
  cutDxDyArr[17].SetPoint(24,29.6786,-7.61829);
  cutDxDyArr[17].SetPoint(25,61.5834,-43.7262);
  cutDxDyArr[17].SetPoint(26,87.7534,-87.0055);
  cutDxDyArr[17].SetPoint(27,96.2201,-113.639);

  cutDxDyArr[18].Set(32);
  cutDxDyArr[18].SetPoint(0,26.2436,-38.4888);
  cutDxDyArr[18].SetPoint(1,14.2805,-89.0958);
  cutDxDyArr[18].SetPoint(2,-34.1227,-131.701);
  cutDxDyArr[18].SetPoint(3,-77.4847,-166.943);
  cutDxDyArr[18].SetPoint(4,-107.325,-173.655);
  cutDxDyArr[18].SetPoint(5,-139.031,-187.08);
  cutDxDyArr[18].SetPoint(6,-146.491,-168.621);
  cutDxDyArr[18].SetPoint(7,-118.982,-89.7471);
  cutDxDyArr[18].SetPoint(8,-95.035,-42.2717);
  cutDxDyArr[18].SetPoint(9,-72.3495,-19.7216);
  cutDxDyArr[18].SetPoint(10,-31.6669,-7.02713);
  cutDxDyArr[18].SetPoint(11,-12.0656,0.413712);
  cutDxDyArr[18].SetPoint(12,-2.08335,1.66462);
  cutDxDyArr[18].SetPoint(13,-1.99256,7.50959);
  cutDxDyArr[18].SetPoint(14,-10.1229,16.1173);
  cutDxDyArr[18].SetPoint(15,-20.603,52.4554);
  cutDxDyArr[18].SetPoint(16,-38.3551,135.413);
  cutDxDyArr[18].SetPoint(17,-63.0307,198.897);
  cutDxDyArr[18].SetPoint(18,-66.2945,234.138);
  cutDxDyArr[18].SetPoint(19,-54.638,249.241);
  cutDxDyArr[18].SetPoint(20,-21.5337,242.529);
  cutDxDyArr[18].SetPoint(21,17.1656,205.609);
  cutDxDyArr[18].SetPoint(22,31.6196,170.368);
  cutDxDyArr[18].SetPoint(23,35.8615,104.617);
  cutDxDyArr[18].SetPoint(24,34.3006,60.2095);
  cutDxDyArr[18].SetPoint(25,22.0408,21.9427);
  cutDxDyArr[18].SetPoint(26,7.21037,8.20594);
  cutDxDyArr[18].SetPoint(27,0.289514,6.24354);
  cutDxDyArr[18].SetPoint(28,-0.105964,-0.624846);
  cutDxDyArr[18].SetPoint(29,15.7131,-7.16617);
  cutDxDyArr[18].SetPoint(30,24.5244,-23.911);
  cutDxDyArr[18].SetPoint(31,26.2436,-38.4888);

  cutDxDyArr[19].Set(31);
  cutDxDyArr[19].SetPoint(0,45.3693,-19.4559);
  cutDxDyArr[19].SetPoint(1,48.5977,-55.515);
  cutDxDyArr[19].SetPoint(2,16.9689,-128.095);
  cutDxDyArr[19].SetPoint(3,-24.4234,-206.535);
  cutDxDyArr[19].SetPoint(4,-48.8015,-219.516);
  cutDxDyArr[19].SetPoint(5,-97.0161,-254.751);
  cutDxDyArr[19].SetPoint(6,-120.853,-249.188);
  cutDxDyArr[19].SetPoint(7,-130.062,-230.643);
  cutDxDyArr[19].SetPoint(8,-88.5188,-50.337);
  cutDxDyArr[19].SetPoint(9,-58.7462,-14.0422);
  cutDxDyArr[19].SetPoint(10,-20.7245,0.276179);
  cutDxDyArr[19].SetPoint(11,3.21308,1.94272);
  cutDxDyArr[19].SetPoint(12,3.13237,4.60597);
  cutDxDyArr[19].SetPoint(13,-8.04626,11.3601);
  cutDxDyArr[19].SetPoint(14,-16.4151,24.2957);
  cutDxDyArr[19].SetPoint(15,-28.8525,61.3347);
  cutDxDyArr[19].SetPoint(16,-34.1746,142.103);
  cutDxDyArr[19].SetPoint(17,-29.299,203.3);
  cutDxDyArr[19].SetPoint(18,-16.839,240.389);
  cutDxDyArr[19].SetPoint(19,8.6226,255.225);
  cutDxDyArr[19].SetPoint(20,39.5016,231.117);
  cutDxDyArr[19].SetPoint(21,52.5033,181.047);
  cutDxDyArr[19].SetPoint(22,52.8544,125.434);
  cutDxDyArr[19].SetPoint(23,43.9154,72.4549);
  cutDxDyArr[19].SetPoint(24,29.993,24.576);
  cutDxDyArr[19].SetPoint(25,9.43143,3.82587);
  cutDxDyArr[19].SetPoint(26,3.98756,3.30022);
  cutDxDyArr[19].SetPoint(27,4.02018,1.80255);
  cutDxDyArr[19].SetPoint(28,23.4709,0.496799);
  cutDxDyArr[19].SetPoint(29,39.2295,-3.53312);
  cutDxDyArr[19].SetPoint(30,45.3693,-19.4559);

  cutDxDyArr[20].Set(29);
  cutDxDyArr[20].SetPoint(0,50.7788,-81.7561);
  cutDxDyArr[20].SetPoint(1,22.7761,-162.198);
  cutDxDyArr[20].SetPoint(2,-23.8662,-183.635);
  cutDxDyArr[20].SetPoint(3,-53.0176,-149.693);
  cutDxDyArr[20].SetPoint(4,-72.1427,-92.2868);
  cutDxDyArr[20].SetPoint(5,-74.5597,-56.5995);
  cutDxDyArr[20].SetPoint(6,-65.9276,-27.3476);
  cutDxDyArr[20].SetPoint(7,-52.1162,-11.5516);
  cutDxDyArr[20].SetPoint(8,-14.3074,4.82942);
  cutDxDyArr[20].SetPoint(9,-0.496013,5.9995);
  cutDxDyArr[20].SetPoint(10,-0.841298,6.58453);
  cutDxDyArr[20].SetPoint(11,-6.88379,10.0948);
  cutDxDyArr[20].SetPoint(12,-26.3649,46.8161);
  cutDxDyArr[20].SetPoint(13,-32.6116,64.6806);
  cutDxDyArr[20].SetPoint(14,-32.6116,168.294);
  cutDxDyArr[20].SetPoint(15,-11.7892,221.888);
  cutDxDyArr[20].SetPoint(16,2.37011,236.179);
  cutDxDyArr[20].SetPoint(17,21.1103,212.955);
  cutDxDyArr[20].SetPoint(18,32.3544,187.945);
  cutDxDyArr[20].SetPoint(19,41.9328,127.206);
  cutDxDyArr[20].SetPoint(20,31.7882,41.1017);
  cutDxDyArr[20].SetPoint(21,25.7457,24.7207);
  cutDxDyArr[20].SetPoint(22,14.3513,9.50972);
  cutDxDyArr[20].SetPoint(23,0.885128,7.16957);
  cutDxDyArr[20].SetPoint(24,0.3672,5.41446);
  cutDxDyArr[20].SetPoint(25,14.006,4.82942);
  cutDxDyArr[20].SetPoint(26,42.492,-11.5516);
  cutDxDyArr[20].SetPoint(27,54.7495,-55.7911);
  cutDxDyArr[20].SetPoint(28,50.7788,-81.7561);

  cutDxDyArr[21].Set(26);
  cutDxDyArr[21].SetPoint(0,74.3503,-54.5183);
  cutDxDyArr[21].SetPoint(1,43.9807,-169.187);
  cutDxDyArr[21].SetPoint(2,21.544,-199.382);
  cutDxDyArr[21].SetPoint(3,-46.492,-121.568);
  cutDxDyArr[21].SetPoint(4,-58.5385,-57.9864);
  cutDxDyArr[21].SetPoint(5,-44.2332,-15.213);
  cutDxDyArr[21].SetPoint(6,-36.3277,-3.65263);
  cutDxDyArr[21].SetPoint(7,-14.4932,0.971515);
  cutDxDyArr[21].SetPoint(8,-7.34057,0.971515);
  cutDxDyArr[21].SetPoint(9,-6.58766,3.28359);
  cutDxDyArr[21].SetPoint(10,-16.3755,4.43963);
  cutDxDyArr[21].SetPoint(11,-24.2811,11.3759);
  cutDxDyArr[21].SetPoint(12,-42.4175,86.6808);
  cutDxDyArr[21].SetPoint(13,-46.1011,145.483);
  cutDxDyArr[21].SetPoint(14,-21.6551,204.284);
  cutDxDyArr[21].SetPoint(15,8.4838,223.355);
  cutDxDyArr[21].SetPoint(16,36.2786,132.769);
  cutDxDyArr[21].SetPoint(17,40.4693,72.6458);
  cutDxDyArr[21].SetPoint(18,37.8341,61.0855);
  cutDxDyArr[21].SetPoint(19,14.1174,12.5319);
  cutDxDyArr[21].SetPoint(20,9.97639,10.2198);
  cutDxDyArr[21].SetPoint(21,9.59994,2.12755);
  cutDxDyArr[21].SetPoint(22,26.164,-1.34056);
  cutDxDyArr[21].SetPoint(23,46.1161,-7.12075);
  cutDxDyArr[21].SetPoint(24,64.5625,-20.9932);
  cutDxDyArr[21].SetPoint(25,74.3503,-54.5183);

  cutDxDyArr[22].Set(27);
  cutDxDyArr[22].SetPoint(0,76.0228,-79.2417);
  cutDxDyArr[22].SetPoint(1,88.7069,-157.843);
  cutDxDyArr[22].SetPoint(2,90.3427,-217.979);
  cutDxDyArr[22].SetPoint(3,51.0848,-217.979);
  cutDxDyArr[22].SetPoint(4,-3.98522,-152.206);
  cutDxDyArr[22].SetPoint(5,-36.5992,-90.7849);
  cutDxDyArr[22].SetPoint(6,-46.3766,-47.0421);
  cutDxDyArr[22].SetPoint(7,-45.2804,-32.159);
  cutDxDyArr[22].SetPoint(8,-32.4249,-6.95737);
  cutDxDyArr[22].SetPoint(9,-24.4968,0.0502592);
  cutDxDyArr[22].SetPoint(10,-1.50587,4.37054);
  cutDxDyArr[22].SetPoint(11,-1.50587,5.72062);
  cutDxDyArr[22].SetPoint(12,-20.88,16.2835);
  cutDxDyArr[22].SetPoint(13,-34.472,41.1476);
  cutDxDyArr[22].SetPoint(14,-51.4218,109.009);
  cutDxDyArr[22].SetPoint(15,-49.2408,161.628);
  cutDxDyArr[22].SetPoint(16,-34.5191,210.489);
  cutDxDyArr[22].SetPoint(17,2.55775,238.677);
  cutDxDyArr[22].SetPoint(18,19.4604,191.696);
  cutDxDyArr[22].SetPoint(19,26.6919,122.648);
  cutDxDyArr[22].SetPoint(20,21.8947,31.2019);
  cutDxDyArr[22].SetPoint(21,0.707259,6.72037);
  cutDxDyArr[22].SetPoint(22,0.262663,3.56048);
  cutDxDyArr[22].SetPoint(23,29.4435,-3.45997);
  cutDxDyArr[22].SetPoint(24,55.7702,-19.5582);
  cutDxDyArr[22].SetPoint(25,60.2547,-23.8785);
  cutDxDyArr[22].SetPoint(26,76.0228,-79.2417);

  cutDxDyArr[23].Set(28);
  cutDxDyArr[23].SetPoint(0,102.687,-83.3034);
  cutDxDyArr[23].SetPoint(1,115.529,-129.119);
  cutDxDyArr[23].SetPoint(2,110.003,-172.342);
  cutDxDyArr[23].SetPoint(3,100.962,-189.255);
  cutDxDyArr[23].SetPoint(4,44.2002,-151.67);
  cutDxDyArr[23].SetPoint(5,7.33935,-102.17);
  cutDxDyArr[23].SetPoint(6,-26.8385,-58.5565);
  cutDxDyArr[23].SetPoint(7,-28.7549,-41.3429);
  cutDxDyArr[23].SetPoint(8,-21.2594,-6.72793);
  cutDxDyArr[23].SetPoint(9,-2.29851,-0.787401);
  cutDxDyArr[23].SetPoint(10,-2.6919,6.85095);
  cutDxDyArr[23].SetPoint(11,-14.421,11.9933);
  cutDxDyArr[23].SetPoint(12,-34.1899,45.221);
  cutDxDyArr[23].SetPoint(13,-39.1841,94.5113);
  cutDxDyArr[23].SetPoint(14,-33.6586,162.164);
  cutDxDyArr[23].SetPoint(15,-16.5799,209.145);
  cutDxDyArr[23].SetPoint(16,17.0752,263.643);
  cutDxDyArr[23].SetPoint(17,48.2187,267.402);
  cutDxDyArr[23].SetPoint(18,53.7442,182.836);
  cutDxDyArr[23].SetPoint(19,31.6424,111.425);
  cutDxDyArr[23].SetPoint(20,16.1072,24.5716);
  cutDxDyArr[23].SetPoint(21,5.60552,7.89801);
  cutDxDyArr[23].SetPoint(22,-1.5965,6.5494);
  cutDxDyArr[23].SetPoint(23,-1.47438,0.0914818);
  cutDxDyArr[23].SetPoint(24,27.8301,-7.02045);
  cutDxDyArr[23].SetPoint(25,50.5839,-23.9977);
  cutDxDyArr[23].SetPoint(26,90.1094,-58.5411);
  cutDxDyArr[23].SetPoint(27,102.687,-83.3034);

  cutDxDyArr[24].Set(26);
  cutDxDyArr[24].SetPoint(0,59.439,164.273);
  cutDxDyArr[24].SetPoint(1,56.6914,84.9202);
  cutDxDyArr[24].SetPoint(2,47.5464,44.2953);
  cutDxDyArr[24].SetPoint(3,29.6968,25.093);
  cutDxDyArr[24].SetPoint(4,11.0109,10.9076);
  cutDxDyArr[24].SetPoint(5,1.63494,6.68996);
  cutDxDyArr[24].SetPoint(6,1.65111,4.53162);
  cutDxDyArr[24].SetPoint(7,15.2453,-8.50477);
  cutDxDyArr[24].SetPoint(8,20.8882,-52.3441);
  cutDxDyArr[24].SetPoint(9,16.2019,-71.2382);
  cutDxDyArr[24].SetPoint(10,-3.52376,-94.8246);
  cutDxDyArr[24].SetPoint(11,-100.445,-181.085);
  cutDxDyArr[24].SetPoint(12,-164.139,-197.056);
  cutDxDyArr[24].SetPoint(13,-166.739,-143.156);
  cutDxDyArr[24].SetPoint(14,-155.69,-113.212);
  cutDxDyArr[24].SetPoint(15,-66.5765,-15.5266);
  cutDxDyArr[24].SetPoint(16,-22.7549,4.27076);
  cutDxDyArr[24].SetPoint(17,-0.179837,5.21038);
  cutDxDyArr[24].SetPoint(18,0.578162,8.76774);
  cutDxDyArr[24].SetPoint(19,-10.4648,36.3495);
  cutDxDyArr[24].SetPoint(20,-13.375,96.605);
  cutDxDyArr[24].SetPoint(21,-30.9022,176.251);
  cutDxDyArr[24].SetPoint(22,-28.3024,240.132);
  cutDxDyArr[24].SetPoint(23,-2.30497,246.121);
  cutDxDyArr[24].SetPoint(24,31.4917,222.165);
  cutDxDyArr[24].SetPoint(25,59.439,164.273);

  cutDxDyArr[25].Set(25);
  cutDxDyArr[25].SetPoint(0,64.3103,108.517);
  cutDxDyArr[25].SetPoint(1,38.344,40.0762);
  cutDxDyArr[25].SetPoint(2,14.0535,11.6811);
  cutDxDyArr[25].SetPoint(3,4.53332,6.48656);
  cutDxDyArr[25].SetPoint(4,5.07451,5.00751);
  cutDxDyArr[25].SetPoint(5,28.1623,-3.48626);
  cutDxDyArr[25].SetPoint(6,41.8981,-33.9022);
  cutDxDyArr[25].SetPoint(7,42.1903,-56.1641);
  cutDxDyArr[25].SetPoint(8,6.31169,-126.33);
  cutDxDyArr[25].SetPoint(9,-72.8656,-230.212);
  cutDxDyArr[25].SetPoint(10,-133.721,-258.383);
  cutDxDyArr[25].SetPoint(11,-138.301,-207.323);
  cutDxDyArr[25].SetPoint(12,-88.7379,-41.8936);
  cutDxDyArr[25].SetPoint(13,-62.1431,-19.2408);
  cutDxDyArr[25].SetPoint(14,-27.84,-3.60795);
  cutDxDyArr[25].SetPoint(15,1.05033,2.09134);
  cutDxDyArr[25].SetPoint(16,1.05033,6.88622);
  cutDxDyArr[25].SetPoint(17,-8.23763,16.476);
  cutDxDyArr[25].SetPoint(18,-17.4558,62.6806);
  cutDxDyArr[25].SetPoint(19,-16.5908,118.407);
  cutDxDyArr[25].SetPoint(20,8.27476,227.571);
  cutDxDyArr[25].SetPoint(21,39.6839,257.503);
  cutDxDyArr[25].SetPoint(22,56.0429,211.725);
  cutDxDyArr[25].SetPoint(23,69.7844,157.143);
  cutDxDyArr[25].SetPoint(24,64.3103,108.517);

  cutDxDyArr[26].Set(27);
  cutDxDyArr[26].SetPoint(0,-19.2313,-189.524);
  cutDxDyArr[26].SetPoint(1,-62.5603,-147.366);
  cutDxDyArr[26].SetPoint(2,-75.2188,-73.1866);
  cutDxDyArr[26].SetPoint(3,-66.5789,-32.1904);
  cutDxDyArr[26].SetPoint(4,-56.4363,-11.367);
  cutDxDyArr[26].SetPoint(5,-45.9181,-3.37637);
  cutDxDyArr[26].SetPoint(6,-21.8579,0.784091);
  cutDxDyArr[26].SetPoint(7,-1.49302,1.19648);
  cutDxDyArr[26].SetPoint(8,-1.20609,6.9679);
  cutDxDyArr[26].SetPoint(9,-13.9302,12.0876);
  cutDxDyArr[26].SetPoint(10,-37.4295,80.2855);
  cutDxDyArr[26].SetPoint(11,-30.9301,130.875);
  cutDxDyArr[26].SetPoint(12,-13.1652,171.346);
  cutDxDyArr[26].SetPoint(13,20.1982,196.641);
  cutDxDyArr[26].SetPoint(14,40.9962,191.582);
  cutDxDyArr[26].SetPoint(15,51.9119,140.864);
  cutDxDyArr[26].SetPoint(16,54.2599,95.2571);
  cutDxDyArr[26].SetPoint(17,32.5102,44.2818);
  cutDxDyArr[26].SetPoint(18,8.85633,7.38233);
  cutDxDyArr[26].SetPoint(19,0.176227,6.55755);
  cutDxDyArr[26].SetPoint(20,0.510077,0.784091);
  cutDxDyArr[26].SetPoint(21,17.4597,0.211258);
  cutDxDyArr[26].SetPoint(22,35.6313,-14.9139);
  cutDxDyArr[26].SetPoint(23,52.0443,-49.163);
  cutDxDyArr[26].SetPoint(24,31.8971,-130.503);
  cutDxDyArr[26].SetPoint(25,0.266836,-186.151);
  cutDxDyArr[26].SetPoint(26,-19.2313,-189.524);

  cutDxDyArr[27].Set(25);
  cutDxDyArr[27].SetPoint(0,35.889,-213.7);
  cutDxDyArr[27].SetPoint(1,-7.00679,-200.309);
  cutDxDyArr[27].SetPoint(2,-54.6483,-89.7339);
  cutDxDyArr[27].SetPoint(3,-58.3435,-37.3485);
  cutDxDyArr[27].SetPoint(4,-38.4052,-11.918);
  cutDxDyArr[27].SetPoint(5,-13.8434,0.332508);
  cutDxDyArr[27].SetPoint(6,-6.52248,1.42934);
  cutDxDyArr[27].SetPoint(7,-6.52248,7.08825);
  cutDxDyArr[27].SetPoint(8,-38.4052,53.1473);
  cutDxDyArr[27].SetPoint(9,-55.5928,117.733);
  cutDxDyArr[27].SetPoint(10,-53.4043,176.32);
  cutDxDyArr[27].SetPoint(11,-23.6399,221.516);
  cutDxDyArr[27].SetPoint(12,4.37373,208.124);
  cutDxDyArr[27].SetPoint(13,24.5085,146.19);
  cutDxDyArr[27].SetPoint(14,32.3873,100.994);
  cutDxDyArr[27].SetPoint(15,24.5256,45.305);
  cutDxDyArr[27].SetPoint(16,13.8969,11.771);
  cutDxDyArr[27].SetPoint(17,0.0267727,6.10919);
  cutDxDyArr[27].SetPoint(18,0.315735,4.60223);
  cutDxDyArr[27].SetPoint(19,45.892,-6.68851);
  cutDxDyArr[27].SetPoint(20,59.8419,-27.4163);
  cutDxDyArr[27].SetPoint(21,75.3068,-63.9696);
  cutDxDyArr[27].SetPoint(22,66.5289,-148.418);
  cutDxDyArr[27].SetPoint(23,59.5255,-180.222);
  cutDxDyArr[27].SetPoint(24,35.889,-213.7);

  cutDxDyArr[28].Set(26);
  cutDxDyArr[28].SetPoint(0,110.455,-233.763);
  cutDxDyArr[28].SetPoint(1,36.533,-194.383);
  cutDxDyArr[28].SetPoint(2,-32.2233,-77.6685);
  cutDxDyArr[28].SetPoint(3,-40.78,-41.7787);
  cutDxDyArr[28].SetPoint(4,-32.4678,-12.5932);
  cutDxDyArr[28].SetPoint(5,-26.3558,-3.45766);
  cutDxDyArr[28].SetPoint(6,-10.2203,4.41784);
  cutDxDyArr[28].SetPoint(7,-3.10924,4.80834);
  cutDxDyArr[28].SetPoint(8,-4.26643,7.82354);
  cutDxDyArr[28].SetPoint(9,-36.9572,19.8843);
  cutDxDyArr[28].SetPoint(10,-54.8672,68.1503);
  cutDxDyArr[28].SetPoint(11,-63.8563,126.76);
  cutDxDyArr[28].SetPoint(12,-59.3618,169.385);
  cutDxDyArr[28].SetPoint(13,-51.0147,203.131);
  cutDxDyArr[28].SetPoint(14,-15.7004,208.459);
  cutDxDyArr[28].SetPoint(15,5.48824,178.266);
  cutDxDyArr[28].SetPoint(16,18.0096,83.9572);
  cutDxDyArr[28].SetPoint(17,17.8866,24.5718);
  cutDxDyArr[28].SetPoint(18,-1.37344,7.82354);
  cutDxDyArr[28].SetPoint(19,-0.505546,5.56214);
  cutDxDyArr[28].SetPoint(20,24.6635,4.80834);
  cutDxDyArr[28].SetPoint(21,66.6118,-31.374);
  cutDxDyArr[28].SetPoint(22,79.6303,-54.7417);
  cutDxDyArr[28].SetPoint(23,130.989,-144.262);
  cutDxDyArr[28].SetPoint(24,148.003,-203.333);
  cutDxDyArr[28].SetPoint(25,110.455,-233.763);

  cutDxDyArr[29].Set(26);
  cutDxDyArr[29].SetPoint(0,152.525,-224.381);
  cutDxDyArr[29].SetPoint(1,120.921,-205.303);
  cutDxDyArr[29].SetPoint(2,37.2755,-123.92);
  cutDxDyArr[29].SetPoint(3,-28.6322,-52.4494);
  cutDxDyArr[29].SetPoint(4,-28.6322,-24.6356);
  cutDxDyArr[29].SetPoint(5,-12.4925,-7.42022);
  cutDxDyArr[29].SetPoint(6,-1.05905,-1.29983);
  cutDxDyArr[29].SetPoint(7,-2.05326,6.26066);
  cutDxDyArr[29].SetPoint(8,-22.5207,19.0717);
  cutDxDyArr[29].SetPoint(9,-47.1031,57.9752);
  cutDxDyArr[29].SetPoint(10,-55.907,136.218);
  cutDxDyArr[29].SetPoint(11,-29.8844,212.408);
  cutDxDyArr[29].SetPoint(12,10.6935,225.856);
  cutDxDyArr[29].SetPoint(13,26.1099,214.41);
  cutDxDyArr[29].SetPoint(14,30.7921,152.99);
  cutDxDyArr[29].SetPoint(15,3.19486,87.3992);
  cutDxDyArr[29].SetPoint(16,5.88411,62.1976);
  cutDxDyArr[29].SetPoint(17,5.61519,28.8054);
  cutDxDyArr[29].SetPoint(18,0.0154779,7.15152);
  cutDxDyArr[29].SetPoint(19,1.28251,-0.238213);
  cutDxDyArr[29].SetPoint(20,29.809,-3.00271);
  cutDxDyArr[29].SetPoint(21,80.2288,-36.4137);
  cutDxDyArr[29].SetPoint(22,165.629,-113.729);
  cutDxDyArr[29].SetPoint(23,185.67,-169.055);
  cutDxDyArr[29].SetPoint(24,176.42,-228.197);
  cutDxDyArr[29].SetPoint(25,152.525,-224.381);

  cutDxDyArr[30].Set(30);
  cutDxDyArr[30].SetPoint(0,124.654,104.807);
  cutDxDyArr[30].SetPoint(1,117.626,89.3369);
  cutDxDyArr[30].SetPoint(2,74.6535,44.4223);
  cutDxDyArr[30].SetPoint(3,33.7165,18.7792);
  cutDxDyArr[30].SetPoint(4,13.7676,7.80085);
  cutDxDyArr[30].SetPoint(5,6.33546,6.82204);
  cutDxDyArr[30].SetPoint(6,5.04513,-3.02382);
  cutDxDyArr[30].SetPoint(7,16.0901,-7.53388);
  cutDxDyArr[30].SetPoint(8,20.8084,-58.564);
  cutDxDyArr[30].SetPoint(9,14.5146,-70.6923);
  cutDxDyArr[30].SetPoint(10,-46.9157,-117.769);
  cutDxDyArr[30].SetPoint(11,-144.97,-190.305);
  cutDxDyArr[30].SetPoint(12,-250.65,-206.41);
  cutDxDyArr[30].SetPoint(13,-248.877,-116.39);
  cutDxDyArr[30].SetPoint(14,-167.501,-63.8597);
  cutDxDyArr[30].SetPoint(15,-108.386,-27.1149);
  cutDxDyArr[30].SetPoint(16,-68.3405,-11.3741);
  cutDxDyArr[30].SetPoint(17,-35.9227,-2.93602);
  cutDxDyArr[30].SetPoint(18,2.21588,1.84553);
  cutDxDyArr[30].SetPoint(19,3.78067,10.0847);
  cutDxDyArr[30].SetPoint(20,-0.727122,36.2915);
  cutDxDyArr[30].SetPoint(21,-0.640694,61.2923);
  cutDxDyArr[30].SetPoint(22,7.30484,91.7142);
  cutDxDyArr[30].SetPoint(23,-3.02025,120.138);
  cutDxDyArr[30].SetPoint(24,28.5805,206.944);
  cutDxDyArr[30].SetPoint(25,79.1613,214.109);
  cutDxDyArr[30].SetPoint(26,114.856,205.719);
  cutDxDyArr[30].SetPoint(27,128.138,180.548);
  cutDxDyArr[30].SetPoint(28,136.439,148.665);
  cutDxDyArr[30].SetPoint(29,124.654,104.807);

  cutDxDyArr[31].Set(30);
  cutDxDyArr[31].SetPoint(0,96.8947,50.7454);
  cutDxDyArr[31].SetPoint(1,80.9416,41.8337);
  cutDxDyArr[31].SetPoint(2,27.3595,16.0347);
  cutDxDyArr[31].SetPoint(3,14.9922,12.159);
  cutDxDyArr[31].SetPoint(4,12.0542,10.5175);
  cutDxDyArr[31].SetPoint(5,12.0177,5.49856);
  cutDxDyArr[31].SetPoint(6,29.2697,0.458237);
  cutDxDyArr[31].SetPoint(7,56.7723,-42.8779);
  cutDxDyArr[31].SetPoint(8,43.7661,-86.8007);
  cutDxDyArr[31].SetPoint(9,-72.9175,-226.108);
  cutDxDyArr[31].SetPoint(10,-114.581,-261.437);
  cutDxDyArr[31].SetPoint(11,-218.295,-266.148);
  cutDxDyArr[31].SetPoint(12,-251.093,-240.24);
  cutDxDyArr[31].SetPoint(13,-247.547,-176.647);
  cutDxDyArr[31].SetPoint(14,-130.734,-45.5913);
  cutDxDyArr[31].SetPoint(15,-91.7155,-10.4758);
  cutDxDyArr[31].SetPoint(16,-56.4903,8.49093);
  cutDxDyArr[31].SetPoint(17,-1.4983,8.77612);
  cutDxDyArr[31].SetPoint(18,-1.32089,10.4344);
  cutDxDyArr[31].SetPoint(19,-4.9828,15.1436);
  cutDxDyArr[31].SetPoint(20,-10.7535,50.5656);
  cutDxDyArr[31].SetPoint(21,-8.98386,83.856);
  cutDxDyArr[31].SetPoint(22,11.0059,148.101);
  cutDxDyArr[31].SetPoint(23,43.5594,207.519);
  cutDxDyArr[31].SetPoint(24,68.6004,221.924);
  cutDxDyArr[31].SetPoint(25,110.336,221.924);
  cutDxDyArr[31].SetPoint(26,145.552,189.768);
  cutDxDyArr[31].SetPoint(27,149.54,148.774);
  cutDxDyArr[31].SetPoint(28,120.799,92.9896);
  cutDxDyArr[31].SetPoint(29,96.8947,50.7454);

  cutDxDyArr[32].Set(27);
  cutDxDyArr[32].SetPoint(0,-60.8093,-243.733);
  cutDxDyArr[32].SetPoint(1,-96.4282,-210.471);
  cutDxDyArr[32].SetPoint(2,-110.028,-121.77);
  cutDxDyArr[32].SetPoint(3,-104.691,-31.2698);
  cutDxDyArr[32].SetPoint(4,-32.4948,16.7881);
  cutDxDyArr[32].SetPoint(5,-11.8044,17.0284);
  cutDxDyArr[32].SetPoint(6,-16.288,20.331);
  cutDxDyArr[32].SetPoint(7,-23.9252,35.2289);
  cutDxDyArr[32].SetPoint(8,-32.7942,72.4735);
  cutDxDyArr[32].SetPoint(9,-31.0697,111.846);
  cutDxDyArr[32].SetPoint(10,-14.181,170.201);
  cutDxDyArr[32].SetPoint(11,18.1998,225.639);
  cutDxDyArr[32].SetPoint(12,36.9806,225.639);
  cutDxDyArr[32].SetPoint(13,65.4757,216.399);
  cutDxDyArr[32].SetPoint(14,85.5518,164.657);
  cutDxDyArr[32].SetPoint(15,81.6661,83.3487);
  cutDxDyArr[32].SetPoint(16,70.0766,53.1629);
  cutDxDyArr[32].SetPoint(17,35.448,17.1386);
  cutDxDyArr[32].SetPoint(18,21.2121,14.8658);
  cutDxDyArr[32].SetPoint(19,20.3317,12.9435);
  cutDxDyArr[32].SetPoint(20,36.6198,12.4629);
  cutDxDyArr[32].SetPoint(21,54.8682,-12.2895);
  cutDxDyArr[32].SetPoint(22,67.8755,-35.8353);
  cutDxDyArr[32].SetPoint(23,71.3973,-68.4458);
  cutDxDyArr[32].SetPoint(24,28.5616,-182.752);
  cutDxDyArr[32].SetPoint(25,-8.35248,-240.037);
  cutDxDyArr[32].SetPoint(26,-60.8093,-243.733);

  cutDxDyArr[33].Set(27);
  cutDxDyArr[33].SetPoint(0,74.2912,-261.061);
  cutDxDyArr[33].SetPoint(1,-17.0992,-214.271);
  cutDxDyArr[33].SetPoint(2,-80.5627,-84.3266);
  cutDxDyArr[33].SetPoint(3,-81.7622,-50.8579);
  cutDxDyArr[33].SetPoint(4,-61.7705,-6.79619);
  cutDxDyArr[33].SetPoint(5,-33.6704,16.2744);
  cutDxDyArr[33].SetPoint(6,-29.8901,17.1669);
  cutDxDyArr[33].SetPoint(7,-30.5202,22.2244);
  cutDxDyArr[33].SetPoint(8,-42.7105,28.9849);
  cutDxDyArr[33].SetPoint(9,-69.0188,63.0109);
  cutDxDyArr[33].SetPoint(10,-79.5421,89.8307);
  cutDxDyArr[33].SetPoint(11,-94.4501,139.526);
  cutDxDyArr[33].SetPoint(12,-88.7499,162.402);
  cutDxDyArr[33].SetPoint(13,-38.4237,218.071);
  cutDxDyArr[33].SetPoint(14,-15.2714,212.457);
  cutDxDyArr[33].SetPoint(15,17.7984,175.023);
  cutDxDyArr[33].SetPoint(16,30.514,145.048);
  cutDxDyArr[33].SetPoint(17,34.8987,88.2531);
  cutDxDyArr[33].SetPoint(18,25.2524,36.9799);
  cutDxDyArr[33].SetPoint(19,15.8772,27.0182);
  cutDxDyArr[33].SetPoint(20,17.3311,17.4066);
  cutDxDyArr[33].SetPoint(21,65.3765,-3.09883);
  cutDxDyArr[33].SetPoint(22,83.7688,-15.4336);
  cutDxDyArr[33].SetPoint(23,100.624,-53.2699);
  cutDxDyArr[33].SetPoint(24,107.192,-184.325);
  cutDxDyArr[33].SetPoint(25,98.662,-219.886);
  cutDxDyArr[33].SetPoint(26,74.2912,-261.061);

  cutDxDyArr[34].Set(26);
  cutDxDyArr[34].SetPoint(0,201.37,-299.744);
  cutDxDyArr[34].SetPoint(1,60.0975,-251.42);
  cutDxDyArr[34].SetPoint(2,-14.4656,-124.143);
  cutDxDyArr[34].SetPoint(3,-53.1575,-67.7714);
  cutDxDyArr[34].SetPoint(4,-56.8093,-33.5675);
  cutDxDyArr[34].SetPoint(5,-26.0068,4.77932);
  cutDxDyArr[34].SetPoint(6,-6.35695,6.22003);
  cutDxDyArr[34].SetPoint(7,-6.65986,11.1631);
  cutDxDyArr[34].SetPoint(8,-19.9267,11.6467);
  cutDxDyArr[34].SetPoint(9,-112.373,79.8787);
  cutDxDyArr[34].SetPoint(10,-142.947,169.906);
  cutDxDyArr[34].SetPoint(11,-126.307,195.91);
  cutDxDyArr[34].SetPoint(12,-78.6626,216.72);
  cutDxDyArr[34].SetPoint(13,-44.1686,205.916);
  cutDxDyArr[34].SetPoint(14,-8.10663,141.097);
  cutDxDyArr[34].SetPoint(15,8.16142,56.2286);
  cutDxDyArr[34].SetPoint(16,8.16142,44.5714);
  cutDxDyArr[34].SetPoint(17,3.94178,21.0492);
  cutDxDyArr[34].SetPoint(18,0.609841,13.0167);
  cutDxDyArr[34].SetPoint(19,0.609841,5.60215);
  cutDxDyArr[34].SetPoint(20,63.0236,-0.0464011);
  cutDxDyArr[34].SetPoint(21,96.03,-16.0084);
  cutDxDyArr[34].SetPoint(22,132.076,-42.7354);
  cutDxDyArr[34].SetPoint(23,222.335,-187.872);
  cutDxDyArr[34].SetPoint(24,235.576,-280.288);
  cutDxDyArr[34].SetPoint(25,201.37,-299.744);

  cutDxDyArr[35].Set(28);
  cutDxDyArr[35].SetPoint(0,248.051,-165.908);
  cutDxDyArr[35].SetPoint(1,202.927,-204.979);
  cutDxDyArr[35].SetPoint(2,122.079,-175.211);
  cutDxDyArr[35].SetPoint(3,21.549,-106.404);
  cutDxDyArr[35].SetPoint(4,-3.42293,-76.2318);
  cutDxDyArr[35].SetPoint(5,-13.5361,-42.3687);
  cutDxDyArr[35].SetPoint(6,-14.5258,-19.7829);
  cutDxDyArr[35].SetPoint(7,0.721968,4.26207);
  cutDxDyArr[35].SetPoint(8,1.08545,7.46591);
  cutDxDyArr[35].SetPoint(9,-11.273,7.9236);
  cutDxDyArr[35].SetPoint(10,-52.8777,28.3024);
  cutDxDyArr[35].SetPoint(11,-95.8009,64.0354);
  cutDxDyArr[35].SetPoint(12,-122.283,104.404);
  cutDxDyArr[35].SetPoint(13,-135.045,145.893);
  cutDxDyArr[35].SetPoint(14,-118.663,207.357);
  cutDxDyArr[35].SetPoint(15,-68.4215,232.588);
  cutDxDyArr[35].SetPoint(16,-20.8159,185.722);
  cutDxDyArr[35].SetPoint(17,-11.4149,155.955);
  cutDxDyArr[35].SetPoint(18,-2.31669,82.0942);
  cutDxDyArr[35].SetPoint(19,-0.0050015,24.1716);
  cutDxDyArr[35].SetPoint(20,1.08545,11.3563);
  cutDxDyArr[35].SetPoint(21,11.263,4.26207);
  cutDxDyArr[35].SetPoint(22,62.1021,-6.65409);
  cutDxDyArr[35].SetPoint(23,112.311,-28.2342);
  cutDxDyArr[35].SetPoint(24,167.452,-59.8149);
  cutDxDyArr[35].SetPoint(25,196.346,-74.7448);
  cutDxDyArr[35].SetPoint(26,248.991,-123.117);
  cutDxDyArr[35].SetPoint(27,248.051,-165.908);
}

void HMdcTrackDSet::printMdc34ClFinderPar(void) {
  printf("  Project plot size of outer segment: %ix%i\n",nBinXSeg2,nBinYSeg2);
  printf("  Drift time is %sused at the filling of project plot.\n",useDriftTimeSeg2 ? "":"NOT ");
  printf("  Cut for the kick angle is %sused.\n",useDxDyCut ? "":"NOT ");
  printf("  Meta matching is %s.\n",doMetaMatch ? "ON" : "OFF");
  printf("  %s level of cluster finder is used.\n",useFloatLevSeg2 ? "Float":"Fixed");
  if      (fakeSuppFlSeg2<0)  printf("  Cluster mergering ON. Fake suppression OFF.\n");
  else  if(fakeSuppFlSeg2==0) printf("  Fake suppression OFF. Cluster mergering OFF.\n");
  else  if(fakeSuppFlSeg2==1) printf("  Fake suppression ON. Cluster mergering OFF.\n");
  else                        printf("  Fake suppression ON but ghosts will not removed.\n");
}

void HMdcTrackDSet::setTrackParam(TString beamTime) {
    beamTime.ToLower();

  if( beamTime.Index("may14") == 0 ) beamTime="jul14";
  if( beamTime.Index("aug14") == 0 ) beamTime="jul14";

  if     ( beamTime.CompareTo("def")   == 0 ) HMdcTrackDSet::setDefParAu15Au();
  else if( beamTime.CompareTo("aug11") == 0 ) HMdcTrackDSet::setTrackParamAug11();
  else if( beamTime.CompareTo("apr12") == 0 ) HMdcTrackDSet::setTrackParamApr12();
  else if( beamTime.Index("jul14")     == 0 ) HMdcTrackDSet::setTrackParamJul14();
  else {
    ::Error("","Beam time = %s not supported !",beamTime.Data());
    exit(1);
  }
}

void HMdcTrackDSet::setDefParam(void) {
  // =================== Inner segment track finder parameters: ===================
  // Cluster finder:
  Int_t    nBinXS1         = 398;
  Int_t    nBinYS1         = 1106;
  Bool_t   useFloatLevelS1 = kTRUE;
  Bool_t   useAmpCutS1     = kTRUE;
  Char_t   fakeRemS1       = 1;
  Bool_t   useDriftTimeS1  = kTRUE;

  for(Int_t s=0;s<6;s++) for(Int_t m=0;m<4;m++) HMdcTrackDSet::setTrFnNLayers(s,m,6);
  HMdcTrackDSet::setTrackFinderLevel(10,5000,10,5000);
  HMdcTrackDSet::setClFinderType(0);

  // vertex finder parameters:
  Bool_t   fndClVertex         = kTRUE;
  Double_t distCutVF           = 0.5;   // Cut:  dDist/yProjUncer < dDistCut
  Int_t    levelVF             = 11;    // Level is used in HMdcLookUpTbSec::testBinForVertexF(...)
  Int_t    levelVFP            = 12;    // Level is used for filling of project plot

  // For cut of drift time distance in cluster finder:
  Double_t constUncertS1       = 0.7; // ??? 0.4; // //1.3; //0.7;
  Double_t dDistCutS1          = 0.7; //1.5; // 1.0;
  Double_t dDistCutYcorS1      = 1.5;
  Double_t dDistCorrMod1Lay[6] = {1.10,1.09,1.27,1.23,0.90,0.83};   // MDC I
  Double_t dDistCorrMod2Lay[6] = {0.94,0.95,1.36,1.40,1.12,1.14};   // MDC II

  // =================== Outer segment track finder parameters: ===================
  // Cluster finder:
  Int_t    nBinXS2       = 1194;  //398;
  Int_t    nBinYS2       = 1100;
  Bool_t   useFloatLevelS2 = kTRUE;
  Char_t   fakeRemS2       = 1;
  Bool_t   useDriftTimeS2  = kTRUE;
  Bool_t   useDxDyCuts     = kTRUE;

  // For cut of drift time distance in cluster finder:
  Double_t dDistCutS2          = 0.2; //3.5; //-3.0; //???
  Double_t dDistCutYcorS2      = 2.3;
  Double_t dDistCorrMod3Lay[6] = {0.5,0.2,0.1,0.1,0.,0.3};  // MDC III
  Double_t dDistCorrMod4Lay[6] = {1.9,0.7,0.8,0.9,0.6,2.0};  // MDC IV

  // Meta match:
  Bool_t   doMMatch            = kTRUE; //kFALSE;
  Bool_t   doMMPl              = kFALSE;

  // ===================  OffVertex track finder: =================================
  Bool_t   findOVT             = kTRUE; //kFALSE;
  Int_t    nLayCutOVT          = 7;
  Int_t    nWirCutOVT          = 60;
  Int_t    nCelCutOVT          = 7;

  //================================================================================
  // Inner segment finder:
  HMdcTrackDSet::setProjectPlotSizeSeg1(nBinXS1,nBinYS1);
  HMdcTrackDSet::setFloatLevelSeg1(useFloatLevelS1);
  HMdcTrackDSet::setSegAmpCutSeg1(useAmpCutS1);
  HMdcTrackDSet::setRemoveGhostsSeg1(fakeRemS1);
  HMdcTrackDSet::setUseDriftTimeSeg1(useDriftTimeS1);
  HMdcTrackDSet::setVertexFinderFlag(fndClVertex);
  HMdcTrackDSet::setVertexFinderPar(distCutVF,levelVF,levelVFP);
  HMdcTrackDSet::setDrTimeProjParamSeg1(constUncertS1,dDistCutS1);
  HMdcTrackDSet::setDrTimeCutYCorrSeg1(dDistCutYcorS1);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(0,dDistCorrMod1Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(1,dDistCorrMod2Lay);

  //--------------------------------------------------------------------------------
  // Outer segment finder:
  HMdcTrackDSet::setProjectPlotSizeSeg2(nBinXS2,nBinYS2);
  HMdcTrackDSet::setFloatLevelSeg2(useFloatLevelS2);
  HMdcTrackDSet::setRemoveGhostsSeg2(fakeRemS2);
  HMdcTrackDSet::setUseDriftTimeSeg2(useDriftTimeS2);
  HMdcTrackDSet::setDrTimeProjParamSeg2(dDistCutS2);
  HMdcTrackDSet::setDrTimeCutYCorrSeg2(dDistCutYcorS2);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(2,dDistCorrMod3Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(3,dDistCorrMod4Lay);
  HMdcTrackDSet::setDxDyCut(useDxDyCuts);
  HMdcTrackDSet::setMetaMatchFlag(doMMatch,doMMPl);
  
  //--------------------------------------------------------------------------------
  // OffVertex track finder:
  HMdcTrackDSet::setFindOffVertTrkFlag(findOVT);
  HMdcTrackDSet::setNLayersCutOVT(nLayCutOVT);
  HMdcTrackDSet::setNWiresCutOVT(nWirCutOVT);  
  HMdcTrackDSet::setNCellsCutOVT(nCelCutOVT); 
}

void HMdcTrackDSet::setDefParAu15Au(void) {
  HMdcTrackDSet::setDefParam();
  HMdcTrackDSet::initDxDyCutAug11();
}

void HMdcTrackDSet::setTrackParamAug11(void) {
  HMdcTrackDSet::setDefParam();
  HMdcTrackDSet::initDxDyCutAug11();
  // =================== Inner segment track finder parameters: ===================
  // Cluster finder:

  // Set clusterfinder level =9 for outer segment:
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,1,5);   // MDCII
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,2,5);   // MDCIII

  // For cut of drift time distance in cluster finder:
  Double_t constUncertS1       = 0.9; // ??? 0.4; // //1.3; //0.7;
  Double_t dDistCutS1          = 0.9; //1.5; // 1.0;
  Double_t dDistCutYcorS1      = 1.5;
  Double_t dDistCorrMod1Lay[6] = {1.23,1.09,1.33,1.28,0.88,0.85};   // MDC I
  Double_t dDistCorrMod2Lay[6] = {0.96,0.92,1.40,1.40,1.08,1.10};   // MDC II

  // =================== Outer segment track finder parameters: ===================
  // Cluster finder:
  Int_t    wLev            = 1000;
  Int_t    wBin            = 67;   //For level 9!  // For level 10: 100;
  Int_t    wLay            = 467;  //For level 9!  // For level 10: 600;
  Int_t    dWtCut          = 1700; //For level 9!  // For level 10: 1650;

  // For cut of drift time distance in cluster finder:
  Double_t dDistCutS2          = 0.5; //3.5; //-3.0; //???
  Double_t dDistCutYcorS2      = 2.8;
  Double_t dDistCorrMod3Lay[6] = {0.5,0.25,0.55,0.55,0.05,0.2};  // MDC III
  Double_t dDistCorrMod4Lay[6] = {1.85,0.65,1.45,1.50,0.72,2.0};  // MDC IV

  //================================================================================
  // Inner segment finder:
  HMdcTrackDSet::setDrTimeProjParamSeg1(constUncertS1,dDistCutS1);
  HMdcTrackDSet::setDrTimeCutYCorrSeg1(dDistCutYcorS1);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(0,dDistCorrMod1Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(1,dDistCorrMod2Lay);

  //--------------------------------------------------------------------------------
  // Outer segment finder:
  HMdcTrackDSet::setDrTimeProjParamSeg2(dDistCutS2);
  HMdcTrackDSet::setDrTimeCutYCorrSeg2(dDistCutYcorS2);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(2,dDistCorrMod3Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(3,dDistCorrMod4Lay);
  HMdcTrackDSet::setGhostRemovingParamSeg2(wLev,wBin,wLay,dWtCut);
}

void HMdcTrackDSet::setTrackParamApr12(void) {
  HMdcTrackDSet::setDefParam();
  HMdcTrackDSet::initDxDyCutApr12();
  // =================== Inner segment track finder parameters: ===================
  // Cluster finder:

  // Set clusterfinder level =9 for outer segment:
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,1,5);  // MDCII   5 -> level9,  6 -> level10
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,2,5);  // MDCII

  Double_t distCutVF           = 0.55;  // Cut:  dDist/yProjUncer < dDistCut
  Int_t    levelVF             = 10;    // Level is used in HMdcLookUpTbSec::testBinForVertexF(...)
  Int_t    levelVFP            = 10;    // Level is used for filling of project plot

  Double_t zStartDetector      = -81.0; // Position of the start detector [mm]

  // For cut of drift time distance in cluster finder:
  Double_t constUncertS1       = 0.9; // ??? 0.4; // //1.3; //0.7;
  Double_t dDistCutS1          = 0.9; //1.5; // 1.0;
  Double_t dDistCutYcorS1      = 1.5;
  Double_t dDistCorrMod1Lay[6] = {1.23,1.09,1.33,1.28,0.88,0.85};   // MDC I
  Double_t dDistCorrMod2Lay[6] = {0.96,0.92,1.40,1.40,1.08,1.10};   // MDC II

  // =================== Outer segment track finder parameters: ===================
  Bool_t   useFittedSeg1       = kTRUE;
  // Cluster finder:
  Int_t    wLev                = 1000;
  Int_t    wBin                = 67;   //For level 9!  // For level 10: 100;
  Int_t    wLay                = 467;  //For level 9!  // For level 10: 600;
  Int_t    dWtCut              = 1700; //For level 9!  // For level 10: 1650;

  // For cut of drift time distance in cluster finder:
  Double_t dDistCutS2          = 0.5; //3.5; //-3.0; //???
  Double_t dDistCutYcorS2      = 2.8;
  Double_t dDistCorrMod3Lay[6] = {0.5,0.25,0.55,0.55,0.05,0.2};  // MDC III
  Double_t dDistCorrMod4Lay[6] = {1.85,0.65,1.45,1.50,0.72,2.0};  // MDC IV

  // Segment fitter:
  UChar_t  calcInitParam       = 1;     // calculate initial track param. for all segments
  Double_t cInitParCutSeg1     = 1.8;   // Cut for the HMdcWireArr::calcInitialValue
  Double_t cInitParCutSeg2     = 3.8;   // Cut for the HMdcWireArr::calcInitialValue

  //================================================================================
  // Inner segment finder:
  HMdcTrackDSet::setVertexFinderPar(distCutVF,levelVF,levelVFP);
  HMdcTrackDSet::setDrTimeProjParamSeg1(constUncertS1,dDistCutS1);
  HMdcTrackDSet::setDrTimeCutYCorrSeg1(dDistCutYcorS1);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(0,dDistCorrMod1Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(1,dDistCorrMod2Lay);
  HMdcTrackDSet::setZStarDetector(zStartDetector);

  //--------------------------------------------------------------------------------
  // Outer segment finder:
  HMdcTrackDSet::setUseFittedSeg1ParFlag(useFittedSeg1);
  HMdcTrackDSet::setDrTimeProjParamSeg2(dDistCutS2);
  HMdcTrackDSet::setDrTimeCutYCorrSeg2(dDistCutYcorS2);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(2,dDistCorrMod3Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(3,dDistCorrMod4Lay);
  HMdcTrackDSet::setGhostRemovingParamSeg2(wLev,wBin,wLay,dWtCut);

  //--------------------------------------------------------------------------------
  // Segment fitter:
  HMdcTrackDSet::setCalcInitialValue(calcInitParam);
  HMdcTrackDSet::setCalcInitialValueSeg1(cInitParCutSeg1);
  HMdcTrackDSet::setCalcInitialValueSeg2(cInitParCutSeg2);
}


void HMdcTrackDSet::setTrackParamJul14(void) {

  HMdcTrackDSet::setDefParam();
//  HMdcTrackDSet::setClFinderType(2); // 2 - mixed cluster finder
  HMdcTrackDSet::setClFinderType(0); // 0 - usual cluster finder
  HMdcTrackDSet::initDxDyCutApr12();
  HMdcTrackDSet::setDxDyCut(kFALSE);
  
  HMdcTrackDSet::setFindOffVertTrkFlag(kTRUE);
  HMdcTrackDSet::setNLayersCutOVT(7);
  HMdcTrackDSet::setNWiresCutOVT(60);  
  HMdcTrackDSet::setNCellsCutOVT(7);  

  Bool_t   useDriftTimeS1  = kFALSE; //^^^kTRUE
  Bool_t   useDriftTimeS2  = kFALSE; //^^^kTRUE

  // =================== Inner segment track finder parameters: ===================
  // Cluster finder:

  // Set clusterfinder level =7 for inner and outer segment:
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,0,4); //5
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,1,5); //5
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,2,4); //5
  for(Int_t s=0;s<6;s++) HMdcTrackDSet::setTrFnNLayers(s,3,5); //5

  Double_t distCutVF           = 0.55;  // ^^^Cut:  dDist/yProjUncer < dDistCut
  Int_t    levelVF             = 8;  //10;    // Level is used in HMdcLookUpTbSec::testBinForVertexF(...)
  Int_t    levelVFP            = 8;  //10;    // Level is used for filling of project plot

  Double_t zStartDetector      = 0;  // Position of the start detector [mm]
                                     // == 0 - take it from HStart2GeomPar

  // For cut of drift time distance in cluster finder:
  Double_t constUncertS1       = 0.9;  //1.5; // ^^^ 0.4; // //1.3; //0.7;
  Double_t dDistCutS1          = 0.9; //1.5; //1.5; // 1.0;
  Double_t dDistCutYcorS1      = 1.5;
  Double_t dDistCorrMod1Lay[6] = {1.23,1.09,1.33,1.28,0.88,0.85};   // MDC I
  Double_t dDistCorrMod2Lay[6] = {0.96,0.92,1.40,1.40,1.08,1.10};   // MDC II

  // =================== Outer segment track finder parameters: ===================
  Bool_t   useFittedSeg1       = kTRUE;
  // Cluster finder:
  Int_t    wLev                = 1000;
  Int_t    wBin                = 67;   //For level 9!  // For level 10: 100;
  Int_t    wLay                = 467;  //For level 9!  // For level 10: 600;
  Int_t    dWtCut              = 1700; //For level 9!  // For level 10: 1650;
  
  // Meta match:
  Bool_t   doMMatch            = kFALSE;
  Bool_t   doMMPl              = kFALSE;

  // For cut of drift time distance in cluster finder:
  Double_t dDistCutS2          = 0.5;  //1.0^^^0.5; //3.5; //-3.0; //???
  Double_t dDistCutYcorS2      = 2.8;
  Double_t dDistCorrMod3Lay[6] = {0.5,0.25,0.55,0.55,0.05,0.2};  // MDC III
  Double_t dDistCorrMod4Lay[6] = {1.85,0.65,1.45,1.50,0.72,2.0};  // MDC IV

  // Segment fitter:
  UChar_t  calcInitParam       = 1;     // calculate initial track param. for all segments
  Double_t cInitParCutSeg1     = 1.8;   // Cut for the HMdcWireArr::calcInitialValue
  Double_t cInitParCutSeg2     = 3.8;   // Cut for the HMdcWireArr::calcInitialValue

  //================================================================================
  // Inner segment finder:
  HMdcTrackDSet::setUseDriftTimeSeg1(useDriftTimeS1); //^^^
  HMdcTrackDSet::setVertexFinderPar(distCutVF,levelVF,levelVFP);
  HMdcTrackDSet::setDrTimeProjParamSeg1(constUncertS1,dDistCutS1);
  HMdcTrackDSet::setDrTimeCutYCorrSeg1(dDistCutYcorS1);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(0,dDistCorrMod1Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg1(1,dDistCorrMod2Lay);
  HMdcTrackDSet::setZStarDetector(zStartDetector);

  //--------------------------------------------------------------------------------
  // Outer segment finder:
  HMdcTrackDSet::setUseDriftTimeSeg2(useDriftTimeS2); //^^^
  HMdcTrackDSet::setUseFittedSeg1ParFlag(useFittedSeg1);
  HMdcTrackDSet::setDrTimeProjParamSeg2(dDistCutS2);
  HMdcTrackDSet::setDrTimeCutYCorrSeg2(dDistCutYcorS2);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(2,dDistCorrMod3Lay);
  HMdcTrackDSet::setDrTimeCutLCorrSeg2(3,dDistCorrMod4Lay);
  HMdcTrackDSet::setGhostRemovingParamSeg2(wLev,wBin,wLay,dWtCut);
  HMdcTrackDSet::setMetaMatchFlag(doMMatch,doMMPl);
  //--------------------------------------------------------------------------------
  // Segment fitter:
  HMdcTrackDSet::setCalcInitialValue(calcInitParam);
  HMdcTrackDSet::setCalcInitialValueSeg1(cInitParCutSeg1);
  HMdcTrackDSet::setCalcInitialValueSeg2(cInitParCutSeg2);
}

ClassImp(HMdcTrackDSet)
