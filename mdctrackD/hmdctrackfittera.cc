//*--- Author : A.P.Jerusalimov
//*--- Modified: 23.01.07 V.Pechenov
//*--- Modified: 16.06.2005 by V.Pechenov
//*--- Modified: 10.10.2003 by A.P.Jerusalimov
//*--- Modified: 15.07.2003 by V.Pechenov
//*--- Modified: 27.02.2002
//*--- Modified: 24.03.2002 by V.Pechenov
//*--- Modified: 17.07.2002 by A.P.Jerusalimov
//*--- Modified: 08.08.2002 by A.P.Jerusalimov

#include "hmdctrackfittera.h"
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
// HMdcTrackFitterA
//
// Dubna track straight piece fitter. Version A.
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcTrackFitterA)

//-----------------------------------------------------
HMdcTrackFitterA::HMdcTrackFitterA(HMdcTrackFitInOut* fIO)
    : HMdcTrackFitter(fIO) {
  setDefaultParam();
}

void HMdcTrackFitterA::setDefaultParam(void) {
  maxIteration = 250;  // maximal numer of iterations
  // Fit parameters for 1-st minim. method:
// !!!!!!!!!!!!!!!!!!!!!! 50. !!!!!!!!!!!!!!
  initStepFit       = 5.; //10.; //50.;     // init value of stepFit
  limIter1forFilter = 3;       // max. number of iter. at the filtering
  limFunct1to2      = 10.0;    // limit for functional
  limDeltaF1to2     = 0.001;   // limit for functional
  limGrad1to2       = 1.0;     //
  // DownhillOnGradient parameters:
  stepCh[0] = 2.0;
  stepCh[1] = 1.5;
  stepCh[2] = 1.5;
  stepCh[3] = 2.0;
  stepCh[4] = 1.2;
  stepCh[5] = 1.1;
  dFunctMax = 1.5;
  dFunctMin = 0.7;

  // Fit parameters for 2-nd minim. method:
  limGrad2    = 0.5;              //
  limStep2[0] = 0.25;             // dX1: limit for diff. old-new param.
  limStep2[1] = 0.15;             // dX2
  limStep2[2] = 0.25;             // dY1
  limStep2[3] = 0.15;             // dY2
  limIter2    = 6; /*3;*/         // max. number of iter.
}

//-----------------------------------------------------
HMdcTrackFitterA::~HMdcTrackFitterA(void) {
}

//-----------------------------------------------------
Double_t HMdcTrackFitterA::getFunctional(void) {
  // not used yet ???
  return wires.valueOfFunctional(finalParam);
}

//-----------------------------------------------------
Int_t HMdcTrackFitterA::minimize(Int_t iter) {
  if(fprint) {
    printf("\n  ********************************\n");
    printf("  *** PROGRAM OF TRACK FITTING ***\n");
    printf("  ********************************\n");
  }
  wires.setSizeGrad2Matr(initParam);
  finalParam.copyLine(initParam);
  finalParam.setIterNumb(0);
  parMin.copyPlanes(initParam);
  pari.copyPlanes(initParam);
  tmpPar.copyPlanes(initParam);
  //********* Initialisation of fit ************************
  stepFit=initStepFit;      // init value of step
  return2to1 = 0;           // counter

  wires.valueOfFunctional(finalParam);
  wires.calcTdcErrorsAndFunct(finalParam);
  iterAfterFilter=0;
  if(fitInOut->useTukey()) wires.filterOfHits(finalParam,1);
  
  return doMinimization();
}

Int_t HMdcTrackFitterA::doMinimization(void) {
  //******************* BEGIN of FIT PROCEDURE ******************
  Int_t minimizationMethod = 1;
  for (iteration = 0; iteration < maxIteration; iteration++ ) {
    if(minimizationMethod==1) {
      minimizationMethod=firstMethod();
      if(minimizationMethod<1) break;
    } else if(minimizationMethod==2) {
      minimizationMethod=secondMethod();
      if(minimizationMethod<1) break;
    }
  } //end of iter loop
  exitFlag=(minimizationMethod<0) ? -minimizationMethod : 4;
  if(fprint) printResult();

  //******************* TEST of RESULT ***************************
  wires.valueOfFunctional(finalParam,2);
  if(!wires.calcNGoodWiresAndChi2(finalParam)) return 0;
  if(testChi2Cut() && exitFlag <= 3) {
//    if(wires.calculateErrors(finalParam)) { //Errors calculations
    if(wires.calcErrorsAnalyt(finalParam)) { //Errors calculations
      if(fprint) printf("    !!! GOOD EXIT !!!\n");
      return 1;
    }
    exitFlag=5;
  }
  return 0;
}

//---------------------------------------------------------------------
Int_t HMdcTrackFitterA::firstMethod(void) {
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
// nuzhna li fil'traciya
      wires.calcTdcErrorsAndFunct(finalParam,1);
      return 2; // mini. method
    }

    if(iterAfterFilter == limIter1forFilter) { // ??? == Pochemu???
      funct1beforeFilter = finalParam.functional();
      if(fitInOut->useTukey() && wires.filterOfHits(finalParam))
          iterAfterFilter=0;
      wires.calcTdcErrorsAndFunct(finalParam);
    }

//    wires.calcDerivatives(finalParam,1);
    wires.calcAnalyticDerivatives1(finalParam);
    // Scaled gradiend calculation // ??? Pochemu proveryaetsya sc.gr.???
    if(calcScaledAGrad(finalParam)<limGrad1to2) {
// nuzhna li fil'traciya
      wires.calcTdcErrorsAndFunct(finalParam,1);
      return 2; // mini. method
    }
  }
  return -4;
}

Int_t HMdcTrackFitterA::secondMethod(void) {
  //*** 2-nd method
  Int_t iteration2=0;
  Bool_t final = kFALSE;
  parMin.copyAllParam(finalParam);
  for(; iteration<maxIteration; iteration++) {
//    wires.calcDerivatives(parMin,2);
    wires.calcAnalyticDerivatives2(parMin);
    parMin.saveFunct();
    pari.copyParam(parMin);
    solutionOfLinearEquationsSystem(parMin);
    if(fprint) parMin.printParam((final) ? "outF":"out2");
//??? esli vesa pereschitaniy to kak sravnivat' s finalParam ???
    Bool_t parMinEqFinalParam = parMin < finalParam;
    if(parMinEqFinalParam) finalParam.copyAllParam(parMin);
    iteration2++;

    //***  Test for exit from minimization
    if(!final) {
      if(/*agrad < 1.5*limGrad2 ||*/ !pari.compare(parMin,limStep2,1.5) ||
            (return2to1>0 && iteration2>=limIter2)) {
// !??? /*agrad < 1.5*limGrad2 ||*/ Proverit'. I dlya agrad nado schita' proizv.!!!
        // For filtering take the best parameters:
        if(!parMinEqFinalParam) wires.valueOfFunctional(finalParam);
        if(fitInOut->useTukey() && wires.filterOfHits(finalParam))
            iterAfterFilter=0;
// !??? Pochemu net calcTdcErrorsAndFunct ???
        wires.setWeightsTo1or0(finalParam);  // for finalParam!
        iterAfterFilter = -1000000;
        if(finalParam > 100000.0) return -5; // Too large functional !
        parMin.copyAllParam(finalParam);
        final = kTRUE;
        iteration2 = 0;
      } else if(iteration2 >= limIter2) {
        //  For filtering take the best parameters:
        if(!parMinEqFinalParam) wires.valueOfFunctional(finalParam);
        if(fitInOut->useTukey() && wires.filterOfHits(finalParam))
            iterAfterFilter=0;
        wires.calcTdcErrorsAndFunct(finalParam);
        iteration2 = 0;
        return2to1++;
        return 1;   // go back to the first method
      }
    } else {
//      wires.calcDerivatives(parMin,2);
//      if(wires.getAGrad() < limGrad2) return -1;    // return exit flag 1
      if(wires.calcAGradAnalyt(parMin) < limGrad2) return -1;  // return exit flag 1
      if(iteration2 < 2) continue;
      if(!pari.compare(parMin,limStep2)) return -2; // return exit flag 2
      if(iteration2 >= limIter2) return -3;         // return exit flag 3
    }
  }
  return -4;
}

void HMdcTrackFitterA::solutionOfLinearEquationsSystem(HMdcTrackParam& par) {
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

  for(Int_t i = 0; i < nmOfPar; i++) {
    Int_t iout = ieq[i];
    if(iout>=0) par.addToParam(iout,a[i][nmOfPar]); //!!!
  }

  wires.valueOfFunctional(par);
  par.incIterNumb();
}

//-----------------------------------------------------
void HMdcTrackFitterA::downhillOnGradient(HMdcTrackParam& par) {
  Int_t istepinc = 0;
  Int_t istepdec = 0;
  Int_t iprint = 0;

  //--- Scaling:
  Int_t numOfParam=par.getNumParam();
  Double_t scalePar[numOfParam];
  Double_t cosgrad[numOfParam];
  Double_t agrad=wires.getAGrad();
  Double_t* grad=wires.getGrad();
  TMatrixD& grad2=wires.getGrad2Matr();
  if(iteration>0) {
    agrad = 0.0;
    for(Int_t k=0; k<numOfParam; k++) {
      scalePar[k] = (grad2(k,k)>0.001) ? 1./sqrt(grad2(k,k)) : 1.;
      agrad += grad[k]*grad[k]*scalePar[k]*scalePar[k];
    }
    agrad = sqrt(agrad);
    if(agrad < 1.e-150) for(Int_t k=0;k<numOfParam;k++) cosgrad[k]=0.;
    else for(Int_t k=0;k<numOfParam;k++) cosgrad[k]=grad[k]*scalePar[k]/agrad;
  } else {
    for(Int_t k=0;k<numOfParam;k++) scalePar[k] = 1.0;
    if(agrad < 1.e-150) for(Int_t k=0;k<numOfParam;k++) cosgrad[k]=0.;
    else for(Int_t k=0;k<numOfParam;k++) cosgrad[k]=grad[k]/agrad;
  }

  Double_t functIn=par.functional();
  par.saveFunct();
  tmpPar.setIterNumb(par.getIterNumb());

  tmpPar.setScParam(par,cosgrad,0.001,scalePar);
  Double_t dFdR0 = (wires.valueOfFunctional(tmpPar)/functIn - 1.0)/(0.001);

  Double_t functi;
  Double_t  dFdRi;
  while(kTRUE) {
    tmpPar.setScParam(par,cosgrad,stepFit,scalePar);
    functi = wires.valueOfFunctional(tmpPar)/functIn;
    tmpPar.setScParam(par,cosgrad,stepFit*1.001,scalePar);
    dFdRi = (wires.valueOfFunctional(tmpPar)/functIn-functi)/(stepFit*0.001);
    if(dFdRi < 0) {
      if(functi > 1.0 && istepdec < 2) {
        stepFit = stepFit/5;
        istepdec++;
        if(fprint && iprint == 1) printStep("de",functi,stepFit);
      } else {
        if(istepinc ==2) break;
        stepFit *= stepCh[istepdec + 3*istepinc++];
        if(fprint && iprint == 1) printStep("in",functi,stepFit);
      }
    } else {   //dFdRi >= 0
      if(functi < dFunctMin) {
        if(istepinc ==2) break;
        stepFit *= stepCh[istepdec + 3*istepinc++];
        if(fprint && iprint == 1) printStep("in",functi,stepFit);
      } else if(functi > dFunctMax) {
        if(istepdec ==2) break;
        stepFit /= stepCh[istepinc + 3*istepdec++];
        if(fprint && iprint == 1) printStep("de",functi,stepFit);
      } else break;
    }
  }

  Double_t coeffC = 1.0;
  Double_t coeffB = dFdR0;
  Double_t coeffA = (functi - coeffC - coeffB*stepFit)/(stepFit*stepFit);
  Double_t stepFit1 = -coeffB/(2*coeffA);
  if(coeffA < 0.000001 || (stepFit1/stepFit) > 1.2)
      stepFit1 = (dFdRi > 0) ? stepFit/2.0 : stepFit/1.5;

  tmpPar.setScParam(par,cosgrad,stepFit1,scalePar);
  wires.valueOfFunctional(tmpPar);
  if(tmpPar < par) par.copyNewParam(tmpPar);
  else {
    par.setScParam(cosgrad,0.3*stepFit1,scalePar);  // 0.3 ????
    wires.valueOfFunctional(par);
  }
  par.incIterNumb();
}

Double_t HMdcTrackFitterA::calcScaledAGrad(HMdcTrackParam& par) {
  Double_t agradSc = 0.0;
  Int_t numOfParam=par.getNumParam();
  Double_t* grad=wires.getGrad();
  TMatrixD& grad2=wires.getGrad2Matr();
  for(Int_t k=0; k<numOfParam; k++) agradSc +=(grad2(k,k)>0.001) ?
    grad[k]*grad[k]/grad2(k,k) : grad[k]*grad[k];
  return sqrt(agradSc);
}

//-----------------------------------------------------
void HMdcTrackFitterA::printResult(void) {
  if(exitFlag==1) printResult(" conversion ok, exit=1 ",finalParam);
  else if(exitFlag==2) printResult(" conversion ok, exit=2 ",finalParam);
  else if(exitFlag==3) printResult(" no conversion, exit=3 ",finalParam);
  else if(exitFlag==4) printResult(" no conversion, exit=4 ",finalParam);
  else printResult(" no conversion, Too large functional!",finalParam);
}

void HMdcTrackFitterA::printResult(const Char_t* status,HMdcTrackParam& par) {
  printf("%s\n",status);
  par.printParam("final");
  printf("**************************************************************\n\n");
}

void HMdcTrackFitterA::printStep(const Char_t* cond, Double_t fun, Double_t step) {
  printf("  *functi(dFdRi>0)=%g  *!  step is %screased, now stepFit=%g\n",
      fun,cond,step);
}
