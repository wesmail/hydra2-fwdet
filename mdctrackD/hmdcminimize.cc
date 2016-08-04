using namespace std;
#include "hmdcminimize.h"
#include "hdebug.h"
#include <iostream>
#include <iomanip> 
#include <cmath>
#include "TRandom.h"

//*-- Author : V.Pechenov
//*-- Modified: 15.04.04 A.Ierusalimov

ClassImp(HMdcMinimize)

HMdcMinimize::HMdcMinimize(void) {
  fObjectFit=0;
  nPar=0;
  fFCN=0;
}


HMdcMinimize::~HMdcMinimize(void) {
  fObjectFit=0;
  fFCN=0;
}

void HMdcMinimize::setFCN(TObject *obj,Double_t (*fcn)(TObject *, Int_t &, Double_t *)) {
  fObjectFit=obj;
  fFCN=fcn;
}

//-----------------------------------------------------
Int_t HMdcMinimize::minimize(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* stepPar, Double_t* parOut) {

  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
  
//   Double_t stepFit=.50;
  Double_t stepFit=10.;
    
  Double_t functi,pari[200];
  Double_t cosgrad[200];
  //Double_t limGrad = 0.001;
  Int_t istepinc = 0;
  Int_t istepdec = 0;
  Int_t nStepCh = 0;
  Double_t  stepCh[6] = {2.0, 1.5, 1.5, 2.0, 1.2, 1.1};
  Double_t  dFdRi;
  Double_t  dfunctmax = 1.5;
  Double_t  dfunctmin = 0.7;
  Int_t iprint = 1;
  Double_t funTmp;

  funct0=(*fFCN)(fObjectFit,nPar,par0);
    
  cout<< " *** Entry mimimize  :" << endl << "   *funct0=" << funct0 
      << "  nPar=" << nPar << endl << endl;;

  for(Int_t k = 0; k < nPar; k++) parIn[k] = par0[k];

  for(Int_t iter=0; iter < maxIter; iter++) {              //iter

     istepdec = 0;
     istepinc = 0;

    cout<< endl << "  *  iter=" << iter << endl << "  * Input parIn:" << endl;
//     printf("  * %f %f %f %f %f %f %f %f %f %f %f %f \n",parIn[0],parIn[1],parIn[2],parIn[3],parIn[4],parIn[5],parIn[6],parIn[7],parIn[8],parIn[9],parIn[10],parIn[11]);
    printf("  * %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n"
	   ,parIn[0],parIn[1],parIn[2],parIn[3],parIn[4],parIn[5],
	   parIn[6],parIn[7],parIn[8],parIn[9],parIn[10],parIn[11]
	   ,parIn[12],parIn[13],parIn[14],parIn[15],parIn[16],parIn[17]
	   ,parIn[18],parIn[19],parIn[20],parIn[21],parIn[22],parIn[23]);

    functIn = (iter == 0) ? (*fFCN)(fObjectFit,nPar,parIn) : functOut;
    
    functBest = functIn;
    for(Int_t b = 0; b < nPar; b++) parBest[b] = parIn[b];
    
    cout<< "  * FunctIn=" << functIn << endl;

    Double_t agrad;
    calculationOfGradient(functIn,parIn,stepPar, nPar,agrad,cosgrad,1);    

//     if(agrad < limGrad) {
//     cout << endl << "  *normal exit after " << iter << "iteration" << endl;
//     cout<< "   * agrad=" << agrad << endl;    
   
//     return 0;
//     }


    for(Int_t k = 0; k < nPar; k++) {
      pari[k] = parIn[k] - cosgrad[k]*.01*scalePar[k];
      cout << " my3 " << cosgrad[k] << " " << scalePar[k] << " " << parIn[k] << " " << pari[k] << endl;
    }
    funTmp = (*fFCN)(fObjectFit,nPar,pari);
    
    if(funTmp < functBest) {
       functBest = funTmp;
       for(Int_t b = 0; b < nPar; b++) parBest[b] = pari[b];
    }
    
    Double_t dFdR0 = (funTmp/functIn - 1.0)/(.01);
//       pari[k] = parIn[k] - cosgrad[k]*0.001*scalePar[k];
//       Double_t dFdR0 = ((*fFCN)(fObjectFit,nPar,pari)/functIn - 1.0)/(0.001);
    if(dFdR0 == 0) {
       if(functIn <= functBest) {
	  cout << endl << "  *abnormal(same functional) exit after " << iter << "iteration" << endl;
	  cout<< "   * agrad=" << agrad << endl;       
	  return 0;
       }
       for(Int_t b = 0; b < nPar; b++)  parOut[b] = parBest[b];   
       functOut = functBest;
    
       for(Int_t k = 0; k < nPar; k++) parIn[k] = parOut[k];
    
       cout<< "   * functOut=" << functOut << endl;    
       cout<< "   * agrad=" << agrad << endl;    
       cout<< "   * Output parOut:" << endl;
       printf("  * %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n"
	      ,parOut[0],parOut[1],parOut[2],parOut[3],parOut[4],parOut[5]
	      ,parOut[6],parOut[7],parOut[8],parOut[9],parOut[10],parOut[11]
	      ,parOut[12],parOut[13],parOut[14],parOut[15],parOut[16],parOut[17]
	      ,parOut[18],parOut[19],parOut[20],parOut[21],parOut[22],parOut[23]);
    
       continue;
    }
      

    while(kTRUE) {        //while

      for(Int_t k = 0; k < nPar; k++)      
        pari[k] = parIn[k] - cosgrad[k]*stepFit*scalePar[k];
      
      funTmp = (*fFCN)(fObjectFit,nPar,pari);
    
      if(funTmp < functBest) {
	 functBest = funTmp;
	 for(Int_t b = 0; b < nPar; b++) parBest[b] = pari[b];
      }
    
      functi = funTmp/functIn;
      
      for(Int_t k = 0; k < nPar; k++)      
        pari[k] = parIn[k] - cosgrad[k]*stepFit*1.01*scalePar[k];
      
      funTmp = (*fFCN)(fObjectFit,nPar,pari);
    
      if(funTmp < functBest) {
	 functBest = funTmp;
	 for(Int_t b = 0; b < nPar; b++) parBest[b] = pari[b];
      }
      dFdRi = (funTmp/functIn-functi)/(stepFit*0.01);

      cout << " my " << dFdRi << " " << functi  << " " << istepdec << " " << istepinc << endl; 

      if(dFdRi < 0) {
        if(functi > 1.0 && istepdec < 2) {
          stepFit = stepFit/5;
          istepdec++;
          if(iprint == 1) cout << "  *functi(dFdRi<0)=" << functi <<
                  " *! step is decreased, now stepFit=" << stepFit << endl;
          continue;
        }
        else {
          if(istepinc ==2) break;
          nStepCh = 3*istepinc + istepdec;
          stepFit *= stepCh[nStepCh];
          istepinc++;
          if(iprint == 1) cout << "  *functi(dFdRi<0)=" << functi <<
                  " *!  step is increased, now stepFit=" << stepFit << endl;
          continue;
        }
      }
      else {   //dFdRi > 0
        if(functi < dfunctmin) {
        if(istepinc == 2) break;
          nStepCh = 3*istepinc + istepdec;
          stepFit *= stepCh[nStepCh];
          istepinc++;
          if(iprint == 1) cout << "  *functi(dFdRi>0)=" << functi <<
                  " *!  step is increased, now stepFit=" << stepFit << endl;
          continue;
        } else {
          if(functi > dfunctmax) {
            if(istepdec ==2) break;
            nStepCh = 3*istepdec + istepinc;
            stepFit /= stepCh[nStepCh];
            istepdec++;
            if(iprint == 1) cout << "  *functi(dFdRi>0)=" << functi <<
                    " *!  step is decreased, now stepFit=" << stepFit << endl;
            continue;
          }
        }
      }
      break;
    }

    Double_t coeffC = 1.0;
    Double_t coeffB = dFdR0;
    Double_t coeffA = (functi - coeffC - coeffB*stepFit)/(stepFit*stepFit);
    Double_t stepFit1 = -coeffB/(2*coeffA);
    if(coeffA < 0.000001 || (stepFit1/stepFit) > 1.2)
        stepFit1 = (dFdRi > 0) ? stepFit/2.0 : stepFit/1.5;

    cout << " my2 " << coeffA << " " << coeffB << " " << stepFit1 << " " << endl;

    for(Int_t k = 0; k < nPar; k++)    
      parOut[k] = parIn[k] -cosgrad[k]*stepFit1*scalePar[k];
    functOut = (*fFCN)(fObjectFit,nPar,parOut);
    
//     Double_t dec = .03;
//     while(kTRUE) {
//       if(functOut < functIn) break;
//       for(Int_t k = 0; k < nPar; k++)     
//         parOut[k] = parIn[k] -cosgrad[k]*dec*stepFit1*scalePar[k];
//       functOut = (*fFCN)(fObjectFit,nPar,parOut);
//       dec /= 10.;
//     }
    if(functOut > functIn) {
      for(Int_t k = 0; k < nPar; k++)     
        parOut[k] = parIn[k] -cosgrad[k]*0.03*stepFit1*scalePar[k];
//         parOut[k] = parIn[k] -cosgrad[k]*0.3*stepFit1*scalePar[k];
      functOut = (*fFCN)(fObjectFit,nPar,parOut);
    }

    if(functOut > functBest) {
      for(Int_t b = 0; b < nPar; b++)  parOut[b] = parBest[b];   
      functOut = functBest;
    }
    

    for(Int_t k = 0; k < nPar; k++) parIn[k] = parOut[k];
    
    cout<< "   * functOut=" << functOut << endl;    
    cout<< "   * agrad=" << agrad << endl;    
    cout<< "   * Output parOut:" << endl;
       printf("  * %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n"
	      ,parOut[0],parOut[1],parOut[2],parOut[3],parOut[4],parOut[5]
	      ,parOut[6],parOut[7],parOut[8],parOut[9],parOut[10],parOut[11]
	      ,parOut[12],parOut[13],parOut[14],parOut[15],parOut[16],parOut[17]
	      ,parOut[18],parOut[19],parOut[20],parOut[21],parOut[22],parOut[23]);
    
    if(functIn - functOut < .00001) return 1;
    
  }
    cout << "  *!!! number of iteration exceeded !!!" << endl;
    cout<< "  * Output parOut:" << endl;
       printf("  * %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f %f \n"
	      ,parOut[0],parOut[1],parOut[2],parOut[3],parOut[4],parOut[5]
	      ,parOut[6],parOut[7],parOut[8],parOut[9],parOut[10],parOut[11]
	      ,parOut[12],parOut[13],parOut[14],parOut[15],parOut[16],parOut[17]
	      ,parOut[18],parOut[19],parOut[20],parOut[21],parOut[22],parOut[23]);
	
  return 1;
}

//-----------------------------------------------------
void HMdcMinimize::calculationOfGradient(
    Double_t fun0, Double_t* par, Double_t* stepPar, Int_t nPar,
    Double_t& agrad, Double_t* cosgrad, Int_t iflag) {
    
//   Double_t grad[200];
//   Double_t grad2[200][200];
  
  for(Int_t k = 0; k < nPar; k++) scalePar[k] = 1.0;  
    
  for(Int_t k = 0; k < nPar; k++) {

    Double_t park = par[k];
    par[k] = park + stepPar[k];
    Double_t funk = (*fFCN)(fObjectFit,nPar,par);

//     if(funk < functBest) {
//        functBest = funk;
//        for(Int_t b = 0; b < nPar; b++) parBest[b] = par[b];
//     }
    
    par[k] = park - stepPar[k];
    Double_t funk1 = (*fFCN)(fObjectFit,nPar,par);

//     if(funk1 < functBest) {
//        functBest = funk1;
//        for(Int_t b = 0; b < nPar; b++) parBest[b] = par[b];
//     }
    
    grad[k] = (funk - funk1)/(2.0*stepPar[k]);
    grad2[k][k] = (funk + funk1 - 2.0*fun0)/(stepPar[k]*stepPar[k]);
    if(iflag > 0 && grad2[k][k]>0.001) {
      scalePar[k] = 1./sqrt(grad2[k][k]);
      grad[k] *=scalePar[k];
    } else scalePar[k] = 1.0; 
    par[k] = park;

    for(Int_t l = k+1; l < nPar; l++) {
      if(iflag < 2) grad2[k][l] = grad2[l][k] = 0;
      else {
      par[k] = park + stepPar[k];
      Double_t parl = par[l];
      par[l] = parl + stepPar[k];
      Double_t funkl = (*fFCN)(fObjectFit,nPar,par);

//     if(funkl < functBest) {
//        functBest = funkl;
//        for(Int_t b = 0; b < nPar; b++) parBest[b] = par[b];
//     }
    
      par[k] = park;
      Double_t funl = (*fFCN)(fObjectFit,nPar,par);

//     if(funl < functBest) {
//        functBest = funl;
//        for(Int_t b = 0; b < nPar; b++) parBest[b] = par[b];
//     }
    
      par[l] = parl;
      grad2[k][l] = grad2[l][k] = (funkl + fun0 - funk - funl)/(stepPar[k]*stepPar[k]);
      }
    }
  }
  agrad = 0.0;
  for(Int_t k = 0; k < nPar; k++) agrad += grad[k]*grad[k];
  agrad = sqrt(agrad);
  for(Int_t k = 0; k < nPar; k++) cosgrad[k] = grad[k]/agrad;

  return;
}

//-----------------------------------------------------
Int_t HMdcMinimize::minimize2(Int_t maxIter2, Int_t numOfParam,
     Double_t* par0, Double_t* stepPar, Double_t* parOut) {
//*** 2-nd method
  Double_t pari[200];
  Double_t functIn;
  Double_t cosgrad[200];
  Double_t agrad;  
        
  for(Int_t k = 0; k < numOfParam; k++) pari[k] = par0[k];
  
  funct0=(*fFCN)(fObjectFit,numOfParam,pari);
  cout<< endl << endl << " *** Entry mimimize2  :" << endl 
      << "   *funct0=" << funct0 << "  nPar=" << numOfParam << endl;  
  cout<< "   * Input param:" << endl;
  for(Int_t l=0; l < numOfParam; l++) printf("  %f",pari[l]);
  cout<< endl;
  
  for(Int_t iter2 = 0; iter2 < maxIter2; iter2++) {   
  
    functIn = funct0;

    calculationOfGradient(functIn,pari,stepPar,numOfParam,agrad,cosgrad,2);
      
    solutionOfLinearEquationsSystem(pari,numOfParam);

    funct0=(*fFCN)(fObjectFit,numOfParam,pari);    

  }

    for(Int_t k = 0; k < nPar; k++) parOut[k] = pari[k];

    functOut = (*fFCN)(fObjectFit,numOfParam,parOut);

    cout<< "   * functOut2=" << functOut << endl;    
    cout<< "   * agrad=" << agrad << endl;    
    cout<< "   * Output parOut2:" << endl;
    for(Int_t l=0; l < nPar; l++) printf("  %f",parOut[l]);
        
  return 1;
}
//-----------------------------------------------------
void HMdcMinimize::solutionOfLinearEquationsSystem(Double_t* par,
     Int_t nmOfPar) {
  //  input:  matrix grad2[i][j] (i-string, j-column),  vector grad[i]
  // output:  new param. in vector par[i]
  Double_t a[10][11];
  Int_t ieq[10];

  
  for(Int_t i = 0; i < nmOfPar; i++) {
    for(Int_t j = 0; j < nmOfPar; j++) a[i][j] = grad2[i][j];
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
    if(iout>=0) par[iout] += a[i][nmOfPar]; //!!!
  }

  return;
}
//-----------------------------------------------------
Int_t HMdcMinimize::minpar2(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* dpar, Double_t* parOut) {
  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
  
  Double_t pari[25], parbest[25], b[25];
//   Double_t dpar[25] = {10.0, 10.0, 20.0, 10.0, 10.0, 20.0,10.0,10.0,20.0,10.0,10.0,20.0};
//   Double_t dpar[25] = {10.0, 10.0, 10.0, 10.0, 10.0, 10.0,10.0,10.0,10.0,10.0,10.0,10.0};
//   Double_t dpar[25] = {5.0, 5.0, 5.0, 5.0, 5.0, 5.0,5.0,5.0,5.0,5.0,5.0,5.0};

  for(Int_t i = 0; i < nPar; i++) pari[i] = par0[i];
  
  Double_t f0 = (*fFCN)(fObjectFit,nPar,pari);
  Double_t fbest = 1000000000.;

  for(Double_t iter = 10; iter < maxIter; iter++) {
    Double_t fip[25], fim[25];
//     for(Int_t i = 0; i < nPar; i++) pari[i] = par0[i];
  
    for(Int_t i = 0; i < nPar; i++) { 
      Double_t pard = pari[i];
      pari[i] = pard + iter;
      fip[i] = (*fFCN)(fObjectFit,nPar,pari);
      pari[i] = pard - iter;
      fim[i] = (*fFCN)(fObjectFit,nPar,pari);
      pari[i] = pard;
      b[i] = pari[i] - 
             (fip[i] - fim[i])*dpar[i]/(2.0*(fip[i] - 2.0*f0 + fim[i]));
      parOut[i] = b[i];      
    }
    
    for(Int_t i = 0; i < nPar; i++) pari[i] = parOut[i];
//     f0 = (*fFCN)(fObjectFit,nPar,pari);
    f0 = (*fFCN)(fObjectFit,nPar,parOut);
    cout << " ========================================================== " << endl;
    if(f0 < fbest) {
       fbest = f0;
       for(Int_t i = 0; i < nPar; i++) parbest[i] = parOut[i];
    }
//     else break;
       
  }
  for(Int_t i = 0; i < nPar; i++) parOut[i] = parbest[i];
  
  for(Int_t i = 0; i < nPar; i++) cout << parbest[i] << " , ";
  cout << fbest << endl;

  return 1;
}  
//-----------------------------------------------------
Int_t HMdcMinimize::minbar(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* step, Double_t* parOut) {
  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  Double_t stepNormW = 10;
  Double_t maxDelta, delta[24], parameter[24];
  Double_t newFunctional = 0;
  
  Int_t numOfParam = nParIn;
  
  for(Int_t i = 0; i < numOfParam; i++) parameter[i] = par0[i];
  
//   Double_t step[24] = {.10, .10, .20, .10, .10, .20,.10,.10,.20,.10,.10,.20};
//   Double_t step[24] = {1.0, 1.0, 2.0, 1.0, 1.0, 2.0,1.0,1.0,2.0,1.0,1.0,2.0};
  Double_t weightPar[24] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};
  
  Double_t functional = (*fFCN)(fObjectFit,nPar,parameter);

  for (Int_t iteration = 0; iteration < maxIter; iteration++ ) {

    maxDelta = 0;
    for(Int_t i = 0; i < numOfParam; i++) {
      parameter[i] += stepNormW*step[i];
      delta[i] = ((*fFCN)(fObjectFit,nPar,parameter) - functional)/(stepNormW*step[i]);
      parameter[i] -= stepNormW*step[i];
    }

    for(Int_t i = 0; i < numOfParam; i++)
      if (maxDelta < fabs(delta[i])) maxDelta = fabs(delta[i]);

    for(Int_t i = 0; i < numOfParam; i++) {
      parameter[i] -= stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;
    }
    newFunctional = (*fFCN)(fObjectFit,nPar,parameter);

    if(newFunctional < functional) functional = newFunctional;
    else {
      for(Int_t i = 0; i < numOfParam; i++)
	parameter[i] += 0.5*stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;

      newFunctional = (*fFCN)(fObjectFit,nPar,parameter);
      if( newFunctional < functional ) functional = newFunctional;
      else {
	for(Int_t i = 0; i < numOfParam; i++)
	  parameter[i] += 0.5*stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;
	
	stepNormW /= 2;
      }
    }
    if(stepNormW >= 0.05) continue;
  }
  
  for(Int_t i = 0; i < numOfParam; i++) parOut[i] = parameter[i];
  
  return 1;
}  
//-----------------------------------------------------
Int_t HMdcMinimize::scanXYZ(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* parOut) {

  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
 
  Double_t functi, pari[200];
  //Double_t funmin = 1000000000.;
  //Double_t  parmin[200];

  for(Int_t k = 0; k < nPar; k++) pari[k] = par0[k];

//   for(Int_t iter=0; iter < 3; iter++) {              //iter

     Int_t i = 0;
     Double_t stepx = 0;
//      for (Double_t stepx = -10; stepx < 10.1; stepx += 1.) {        // stepx loop
	for (Double_t stepy = -10; stepy < 10.1; stepy += 1.) {        // stepy loop
	   for (Double_t stepz = -10; stepz < 10.1; stepz += 1.) {        // stepz loop

	pari[0] = par0[0] + stepx;
	pari[1] = par0[1] + stepy;
	pari[2] = par0[2] + stepz;
	
	functi = (*fFCN)(fObjectFit,nPar,pari);
// 	if (functi < funmin) {
// 	   funmin = functi;
// 	   for(Int_t k = 0; k < nPar; k++) parmin[k] = pari[k];
// 	}
	   
	cout << "parx[" << i << "]=" << pari[0] << ";" 
	     << "pary[" << i << "]=" << pari[1] << ";" 
	     << "parz[" << i << "]=" << pari[2] << ";" 
	     << "sumFunctional[" << i << "]="
	     << setprecision(9) << functi << ";" << endl;
	i++;
	   }
	}
//      }
     
//      for(Int_t k = 0; k < nPar; k++) cout << "parmin[" << k << "]=" << parmin[k];
//      cout << "  minFunctional = " << funmin << ";" << endl;
  return 1;
}

Int_t HMdcMinimize::random(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* parOut) {

  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
 
  Double_t functi, pari[200], par[200], alpha[200], alphamin[200], funmin = 1000000000., funstart, parmin[200];
  Double_t sigma = 40.;

  funstart = (*fFCN)(fObjectFit,nPar,par0);
//   funmin = funstart;
  
  for(Int_t k = 0; k < nPar; k++) pari[k] = par0[k];

  for(Int_t iter=0; iter < maxIter; iter++) {              //iter
     if(funmin >= funstart && iter) {
	sigma /= 2. ;
	if(sigma < 1.) {
	   for(Int_t k = 0; k < nPar; k++) parOut[k] = pari[k];
	   for(Int_t k = 0; k < nPar; k++) cout << "parOut[" << k << "] = " << parOut[k] << " ";
	   cout << "  minFunctional = " << funstart << ";" << endl;
	   break;
	}
     }
     else {
	funstart = funmin;
	for(Int_t k = 0; k < nPar; k++) pari[k] = parmin[k];
     }

     funmin = 1000000000.;
     for( Int_t itry = 0 ; itry < 10; itry++ ) {              //try
	
	Double_t sumAlpha2 = 0;
	for(Int_t k = 0; k < nPar; k++) {
	   alpha[k] = gRandom->Uniform(-1.,1.);
	   sumAlpha2 += alpha[k]*alpha[k];
	}
	
	for(Int_t k = 0; k < nPar; k++) {
	   alpha[k] /= sqrt(sumAlpha2);
	   par[k] = pari[k] + alpha[k]*sigma;
	}
	
	functi = (*fFCN)(fObjectFit,nPar,par);
     
	if (functi < funmin) {
	   funmin = functi;
	   for(Int_t k = 0; k < nPar; k++) alphamin[k] = alpha[k];
	   for(Int_t k = 0; k < nPar; k++) parmin[k] = par[k];
	}
     }

     for(Int_t k = 0; k < nPar; k++) par[k] = pari[k] + alphamin[k]*sigma/2.;
     functi = (*fFCN)(fObjectFit,nPar,par);
     if (functi < funmin) {
	funmin = functi;
	for(Int_t k = 0; k < nPar; k++) parmin[k] = par[k];
     }
     
     for(Int_t k = 0; k < nPar; k++) par[k] = pari[k] + alphamin[k]*sigma*2.;
     functi = (*fFCN)(fObjectFit,nPar,par);
     if (functi < funmin) {
	funmin = functi;
	for(Int_t k = 0; k < nPar; k++) parmin[k] = par[k];
     }
     
     for(Int_t k = 0; k < nPar; k++) cout << "parmin[" << k << "] = " << parmin[k] << "; " ;
     cout << "  minFunctional = " << funmin << ";" << "  globalMinFunctional = " << funstart << ";" <<  "  sigma = " << sigma << endl;
     
  }
	   
// 	cout << "parx[" << i << "]=" << pari[0] << ";" 
// 	     << "pary[" << i << "]=" << pari[1] << ";" 
// 	     << "parz[" << i << "]=" << pari[2] << ";" 
// 	     << "sumFunctional[" << i << "]="
// 	     << setprecision(9) << functi << ";" << endl;
    
  for(Int_t k = 0; k < nPar; k++) cout << "parmin[" << k << "]=" << parmin[k];
  cout << "  minFunctional = " << funstart << ";" << endl;
  return 1;
}

Int_t HMdcMinimize::cog(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* parOut) {

  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
 
  Double_t functi, pari[200], par[200], alpha[200],  funmin = 1000000000., parsum[200];
  Double_t sigma = 32.;//8.;//4.;

  for(Int_t k = 0; k < nPar; k++) pari[k] = par0[k];

  Float_t itry = 0.;
  Float_t alltry = 0.;
     
//   for(Int_t iter=0; iter < maxIter; iter++) {              //iter
     
//      if(funmin >= funstart) {
// 	sigma /= 2.;
// 	if(sigma < 1.) {
// 	   for(Int_t k = 0; k < nPar; k++) parOut[k] = pari[k];
// 	   for(Int_t k = 0; k < nPar; k++) cout << "parOut[" << k << "] = " << parOut[k] << " ";
// 	   cout << "  minFunctional = " << funstart << ";" << endl;
// 	   break;
// 	}
//      }
//      else {
// 	funstart = funmin;
// 	for(Int_t k = 0; k < nPar; k++) pari[k] = parmin[k];
//      }

//      cout << itry << " " << alltry << endl;
//      if(itry/alltry > .5) sigma *= 2;
  funmin = 200.;//140.;//139.;
     itry = 0.;
     alltry = 0.;
     for(Int_t k = 0; k < nPar; k++) parsum[k] = 0.;

     while(itry < maxIter) {              //try
	
	Double_t sumAlpha2 = 0;
	for(Int_t k = 0; k < nPar; k++) {
	   alpha[k] = gRandom->Uniform(-1.,1.);
	   sumAlpha2 += alpha[k]*alpha[k];
	}
	
	for(Int_t k = 0; k < nPar; k++) {
	   alpha[k] /= sqrt(sumAlpha2);
	   par[k] = pari[k] + alpha[k]*sigma;
	}
	
	functi = (*fFCN)(fObjectFit,nPar,par);
     
	if (functi < funmin) {
    
	   for(Int_t k = 0; k < nPar; k++) parsum[k] += par[k];
	   itry++;
	   cout << " itry = " << itry << endl;  
	}
	
	alltry++;
     }

     for(Int_t k = 0; k < nPar; k++) pari[k] = parsum[k]/maxIter;
     functi = (*fFCN)(fObjectFit,nPar,pari);
     
     for(Int_t k = 0; k < nPar; k++) cout << "parmin[" << k << "] = " << pari[k] << "; " ;
     cout << "  minFunctional = " << funmin << ";" << "  globalMinFunctional = " << functi << ";" <<  "  sigma = " << sigma <<  "  Ntry = " << alltry << endl;
     
//   }
  
  for(Int_t k = 0; k < nPar; k++) parOut[k] = pari[k];
  
  return 1;
}

Int_t HMdcMinimize::scan(Int_t maxIter, Int_t nParIn,
     Double_t* par0, Double_t* parOut) {

  if(fFCN==0) return -1;
  if(nParIn<=0) return -1;
  
  nPar=nParIn;
  
  Double_t functi,pari[200];

  for(Int_t k = 0; k < nPar; k++) pari[k] = par0[k];

  for(Int_t iter=0; iter < 3; iter++) {              //iter

     Int_t i = 0;
     for (Double_t step = -20; step < 20.1; step += 1.) {        // step loop

	pari[iter+maxIter] += step;      	
	functi = (*fFCN)(fObjectFit,nPar,pari);
	cout << "par[" << i << "]=" << pari[iter+maxIter]
	     << " ; sumFunctional[" << i << "]="
	     << functi << ";" << endl;
	pari[iter+maxIter] -= step;      	
	i++;
     }
  }
  return 1;
}
  
