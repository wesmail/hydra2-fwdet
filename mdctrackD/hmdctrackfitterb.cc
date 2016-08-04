//*--- Author : V.Pechenov
//*--- Modified: 23.01.07 V.Pechenov
//*--- Modified: 16.06.2005 by V.Pechenov

#include "hmdctrackfitterb.h"
#include "hmdctrackfitpar.h"
#include "hmdcsizescells.h"
#include "hsymmat.h"
#include "hmdccal1sim.h"
#include "hcategory.h"
#include "hmdcclus.h"
#include <stdlib.h>
#include "TMatrixD.h"

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// 
// HMdcTrackFitterB
//
// Dubna track straight piece fitter. Version B.
// It is tuned version of HMdcTrackFitterA.
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HMdcTrackFitterB)

HMdcTrackFitterB::HMdcTrackFitterB(HMdcTrackFitInOut* fIO)
    : HMdcTrackFitterA(fIO) {
}

HMdcTrackFitterB::~HMdcTrackFitterB(void) {
}

//-----------------------------------------------------
Int_t HMdcTrackFitterB::minimize(Int_t iter) {
  if(fprint) {
    printf("\n  ********************************\n");
    printf("  *** PROGRAM OF TRACK FITTING ***\n");
    printf("  ********************************\n");
  }
//+++---initParam.setFixedTimeOffset(2.6,3.1,5.,6.);
  wires.setSizeGrad2Matr(initParam);
  finalParam.copyLine(initParam);
//+++---finalParam.setFixedTimeOffset(2.6,3.1,5.,6.);
  finalParam.setIterNumb(0);
  parMin.copyPlanes(initParam);
//+++---parMin.setFixedTimeOffset(2.6,3.1,5.,6.);
  pari.copyPlanes(initParam);
//+++---pari.setFixedTimeOffset(2.6,3.1,5.,6.);
  tmpPar.copyPlanes(initParam);
//+++---tmpPar.setFixedTimeOffset(2.6,3.1,5.,6.);

  // Target scanning:
  Bool_t doTargScan       = fitInOut->getDoTargScanFlag();
  Bool_t noWightsFor3Iter = kFALSE; //kTRUE;
  Bool_t useTukeyInScan   = kFALSE;
  
  Bool_t useNewErrorsForFirstIter = kFALSE; //kFALSE;

  if(fitInOut->getCalcInitValueFlag()==0 && doTargScan) targetScan(useTukeyInScan);


  //********* Initialisation of fit ************************
  stepFit        = initStepFit;  // init value of step
  return2to1     = 0;            // counter
  recalcTdcError = kFALSE;       // kTRUE - old version of fitter

  wires.setInitWeghts(finalParam);
  
  if(useNewErrorsForFirstIter) {
    if(recalcTdcError) wires.valueOfFunctAndErr(finalParam);
    else               wires.calcTdcErrorsTOff0AndFunct(finalParam); 
  } else {
     wires.valueOfFunctional(finalParam);
     if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam);
     else               wires.calcTdcErrorsTOff0AndFunct(finalParam);
  }
  iterAfterFilter=0;
  if(!noWightsFor3Iter && fitInOut->useTukey()) wires.filterOfHitsV2(finalParam,1); //!!!!!!!!!!!!!!???

//iterAfterFilter=-9; //!!!!!!!!!!!!!!!!!!!

  return doMinimization();
}

Int_t HMdcTrackFitterB::doMinimization(void) {
  //******************* BEGIN of FIT PROCEDURE ******************
  Int_t minimizationMethod = 1;
  for (iteration = 0; iteration < maxIteration; iteration++ ) {
    if(minimizationMethod==1) {
      minimizationMethod=firstMethod();
      if(minimizationMethod<1) break;
//+++---finalParam.unfixTimeOffset();
    } else if(minimizationMethod==2) {
//      if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam);         // ??? test it for real data
//      else               wires.calcTdcErrorsTOff0AndFunct(finalParam);    
      minimizationMethod = secondMethod();
      if(minimizationMethod<1) break;
    }
  } //end of iter loop
  exitFlag=(minimizationMethod<0) ? -minimizationMethod : 4;
  if(fprint) printResult();

  //******************* TEST of RESULT ***************************
  wires.valueOfFunctional(finalParam,2);
  if(!wires.calcNGoodWiresAndChi2(finalParam)) return 0;
  if(testChi2Cut() && exitFlag <= 3) {
    // if(wires.calculateErrors(finalParam)) { // Numerical errors calculations
    if(wires.calcErrorsAnalyt(finalParam)) {   // Analytical errors calculations
      if(fprint) printf("    !!! GOOD EXIT !!!\n");
      return 1;
    }
    exitFlag=5;
  }
  return 0;
}

void HMdcTrackFitterB::targetScan(Bool_t useTukeyInScan) {
  if(wires.getSegment()==1) return;
  HMdcSizesCellsSec& fSCSec=(*fitInOut->getMdcSizesCells())[wires.getSector()];
  Int_t nTargets=fSCSec.getNumOfTargets();
  HMdcClus* fClst=wires.getClust1();
  if(nTargets>1) {
    for(Int_t nTg=0;nTg<nTargets;nTg++) {
      HGeomVector* targ=fSCSec.getTarget(nTg);
      parMin.setParam( targ->getX(),  targ->getY(),  targ->getZ(),
          wires.getXClst(),wires.getYClst(),wires.getZClst());
      
      if(recalcTdcError) wires.valueOfFunctAndErr(parMin);
      else               wires.valueOfFunctional(parMin);
      if(useTukeyInScan) wires.filterOfHitsV2(parMin,1);
      wires.calcNGoodWiresAndChi2(parMin);
      
      if(fprint)printf("Scan: %i target. chi2/NDF=%g\n",nTg,parMin.getChi2());
      if(nTg==0 || parMin.getChi2()<finalParam.getChi2()) {
        finalParam.copyNewParam(parMin);
        if(fClst) fClst->setTarg(targ->getX(), targ->getY(), targ->getZ());
      }
      wires.setUnitWeights();
    }
  } else {
    const HGeomVector& firstTargPoint=fSCSec.getTargetFirstPoint();
    const HGeomVector& lastTargPoint=fSCSec.getTargetLastPoint();
    Double_t z1=firstTargPoint.getZ();
    Double_t z2=lastTargPoint.getZ();
    if(z2-z1 < 5.) return;
    Double_t xPl=wires.getXClst();
    Double_t yPl=wires.getYClst();
    Double_t zPl=wires.getZClst();
    const HGeomVector& targ=fSCSec.getTargetMiddlePoint();
    Double_t xTg=targ.getX();
    Double_t yTg=targ.getY();
    
    Double_t zMin=-5000.;
    for(Double_t z=z1;z<=z2;z+=2.5) { //!!!!!!!!!!!!!!!!!!!!
      parMin.setParam(xTg,yTg,z, xPl,yPl,zPl); //!!!!!!!!!!!!!!!!!!!!

      if(recalcTdcError) wires.valueOfFunctAndErr(parMin);
      else               wires.valueOfFunctional(parMin);
      if(useTukeyInScan) wires.filterOfHitsV2(parMin,1);
      wires.calcNGoodWiresAndChi2(parMin);

      if(fprint)printf("Scan: Ztarget=%5.1f chi2/NDF=%g\n",z,parMin.getChi2());
      if(zMin<-4000. || parMin.getChi2()<finalParam.getChi2()) {
        zMin=z;
        finalParam.copyNewParam(parMin);
        if(fClst) fClst->setTarg(xTg,yTg,z);
      }
      wires.setUnitWeights();
    }
  }
  finalParam.setIterNumb(0);
}

Int_t HMdcTrackFitterB::firstMethod(void) {
  //*** 1-st method
  // function return minimizationMethod flag
  Double_t funct1beforeFilter=finalParam.functional();
  //  wires.calcDerivatives(finalParam,1);
  wires.calcAnalyticDerivatives1(finalParam);
  for(; iteration<maxIteration; iteration++) {
    downhillOnGradient(finalParam);
    if(fprint) finalParam.printParam("out1");

    iterAfterFilter++;
    //---------*** Test for method of minimization ***---------
    if(return2to1 > 0 && iterAfterFilter < 2) {
      //      wires.calcDerivatives(finalParam,1);
      wires.calcAnalyticDerivatives1(finalParam);
      continue;
    }

    if((iterAfterFilter>=2 && ((finalParam>funct1beforeFilter && iteration>2) ||
        finalParam.isFunctRelChangLess(limDeltaF1to2))) ||
        finalParam<limFunct1to2) {
// nuzhna li fil'traciya ??? dlya mdc12 i mdc3 luchshe,dlya mdc34 huzhe!!!
//if(fitInOut->useTukey() && wires.filterOfHitsV2(finalParam)) iterAfterFilter=0;
      if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam,1);
      return 2; // Go to mini. method 2
    }

    if(iterAfterFilter == limIter1forFilter) { // ??? == Pochemu???
      funct1beforeFilter = finalParam.functional();
      if(fitInOut->useTukey() && wires.filterOfHitsV2(finalParam)) iterAfterFilter=0;
      if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam);
    }

    //    wires.calcDerivatives(finalParam,1);
    wires.calcAnalyticDerivatives1(finalParam);
    // Scaled gradiend calculation // ??? Pochemu proveryaetsya sc.gr.???
    if(calcScaledAGrad(finalParam)<limGrad1to2) {
// nuzhna li fil'traciya ?
      if(fitInOut->useTukey() && wires.filterOfHitsV2(finalParam)) iterAfterFilter=0;
      if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam,1);
      return 2; // mini. method
    }
  }
  return -4;
}

Int_t HMdcTrackFitterB::secondMethod(void) {
  //*** 2-nd method
  Int_t iteration2=0;
  Int_t nTestTukey=0;
  Int_t nFinalNeg=0;
  Bool_t final = kFALSE;
  parMin.copyAllParam(finalParam);
  for(; iteration<maxIteration; iteration++) {
    //    wires.calcDerivatives(parMin,2);
    wires.calcAnalyticDerivatives2(parMin);
    parMin.saveFunct();
    pari.copyParam(parMin);
    solutionOfLinearEquationsSystem(parMin);
    if(fprint) parMin.printParam((final) ? "outF":"out2");
    Bool_t parMinEqFinalParam = (parMin > finalParam) ? kFALSE:kTRUE;
    if(parMinEqFinalParam) {
      if(recalcTdcError) wires.valueOfFunctAndErr(parMin);
      else               wires.valueOfFunctional(parMin);
      finalParam.copyAllParam(parMin);
      if(!final && fitInOut->useTukey() && iterAfterFilter>=3) {
        wires.filterOfHitsV2(parMin);
        finalParam.copyAllParam(parMin);
        iterAfterFilter=0;
        continue;
      }
    }
    iteration2++;

    //***  Test for exit from minimization
    if(!final) {
      if(!pari.compare(parMin,limStep2,1.5) ||
          (return2to1>0 && iteration2>=limIter2)) {
        // For filtering take the best parameters:
        if(!parMinEqFinalParam) wires.valueOfFunctional(finalParam);
        if(fitInOut->useTukey()) wires.filterOfHitsV2(finalParam);
        if(recalcTdcError) wires.valueOfFunctAndErr(finalParam);
        else               wires.valueOfFunctional(finalParam);
        wires.setWeightsTo1or0(finalParam);  // for finalParam!
        iterAfterFilter = -1000000;
        if(finalParam > 100000.0) return -5; // Too large functional !
        parMin.copyAllParam(finalParam);
        final = kTRUE;
        iteration2 = 0;
      } else if(iteration2 >= limIter2) {
        //  For filtering take the best parameters:
        if(!parMinEqFinalParam) wires.valueOfFunctional(finalParam);
        if(fitInOut->useTukey() && wires.filterOfHitsV2(finalParam)) iterAfterFilter=0;
        if(recalcTdcError) wires.calcTdcErrorsAndFunct(finalParam);
        else               wires.calcTdcErrorsTOff0AndFunct(finalParam);
        iteration2 = 0;
        return2to1++;
        return 1;   // go back to the first method
      }
    } else {
      if(parMinEqFinalParam) {
        if(recalcTdcError) wires.valueOfFunctAndErr(finalParam);
        else               wires.valueOfFunctional(finalParam);
        if(iteration2>=2 && nTestTukey++<2 && wires.testTukeyWeights(finalParam))
            iteration2=0;
        finalParam.saveFunct();
        parMin.copyAllParam(finalParam);
        //      wires.calcDerivatives(finalParam,2);
        //      if(wires.getAGrad() < limGrad2) return -1;          // return exit flag 1
        if(wires.calcAGradAnalyt(finalParam) < limGrad2) return -1; // return exit flag 1
        if(iteration2 < 2) continue;
        if(!pari.compare(finalParam,limStep2)) return -2;           // return exit flag 2
        if(iteration2 >= limIter2) return -3;                       // return exit flag 3
        nFinalNeg=0;
      } else {
        nFinalNeg++;
        if(nFinalNeg<3) iteration2--;    // Try three times
        else if(nFinalNeg<6) {           // Try three times
          if(recalcTdcError) wires.valueOfFunctAndErr(finalParam);
          else               wires.valueOfFunctional(finalParam);
          if(wires.testTukeyWeights(finalParam)) iteration2=0;
          finalParam.saveFunct();
          if(wires.calcAGradAnalyt(finalParam) < limGrad2) return -1; // return exit flag 1
          if(nFinalNeg>3 && !finalParam.compare(parMin,limStep2)) return -2;         // return exit flag 2
          parMin.copyAllParam(finalParam);
          iteration2--;
        } else {  
          if(wires.calcAGradAnalyt(finalParam) < limGrad2) return -1; // return exit flag 1
          if(iteration2 < 2) continue;
          if(!finalParam.compare(parMin,limStep2)) return -2;         // return exit flag 2
          if(iteration2 >= limIter2) return -3;                       // return exit flag 3
        }
      }
    }
  }
  return -4;
}

void HMdcTrackFitterB::solutionOfLinearEquationsSystem(HMdcTrackParam& par) {
  // input :  matrix grad2[i][j] (i-string, j-column),  vector grad[i]
  // output:  new param. in vector par[i]
  Double_t a[10][11];
  Int_t ieq[10];

  TMatrixD& grad2m=wires.getGrad2Matr();
  Double_t* grad=wires.getGrad();
  Int_t nmOfPar=par.getNumParam();
  for(Int_t i = 0; i < nmOfPar; i++) {
    for(Int_t j = 0; j < nmOfPar; j++) a[i][j] = grad2m(i,j);
    a[i][nmOfPar] = -grad[i];
    ieq[i] = -1;
  }
  Int_t iMax = 0;
  Int_t jMax = 0;
  for(Int_t l = 0; l < nmOfPar; l++) {
    Double_t maxA = 0.0 ;
    for(Int_t i = 0; i < nmOfPar; i++) {
      if(ieq[i] != -1) continue;
      for(Int_t j = 0; j < nmOfPar; j++) {
	if(fabs(a[i][j]) <= maxA) continue;
	maxA = fabs(a[i][j]);
	iMax = i;
	jMax = j;
      }
    }
    ieq[iMax] = jMax;
    Double_t corr = a[iMax][jMax];
    for(Int_t j = 0; j <= nmOfPar; j++) a[iMax][j] /= corr;
    for(Int_t i = 0; i < nmOfPar; i++) {
      if(i == iMax) continue;
      corr = a[i][jMax];
      for(Int_t j = 0; j <= nmOfPar; j++) a[i][j] -= a[iMax][j]*corr;
    }
  }
//printf("slI: "); par.printParam();
//Bool_t jump=kFALSE;
  for(Int_t i = 0; i < nmOfPar; i++) {
    Int_t iout = ieq[i];
//if(iout>=0 && fabs(a[i][nmOfPar])>100.) {
//  printf("jump for par.%i on %f\n",iout,a[i][nmOfPar]);
//  jump=kTRUE;
//}
    if(iout>=0) par.addToParam(iout,a[i][nmOfPar]); //!!!
  }
//if(jump) grad2m.Print();
//printf("slO: "); par.printParam();

  wires.valueOfFunctional(par);
  par.incIterNumb();
}
