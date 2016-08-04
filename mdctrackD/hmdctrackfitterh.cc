#include "hmdctrackfitterh.h"
#include "hmdctrackfitpar.h"
#include "hmdcsizescells.h"
#include "hmdccal2parsim.h"

//*-- Author : G.Agakichiev
//*-- Modified: 11.07.2003 by V.Pechenov
//*-- Modified: 7.05.2002 by V.Pechenov
//*-- Modified: 7.5.99

ClassImp(HMdcTrackFitterH)

HMdcTrackFitterH::~HMdcTrackFitterH(void) {
}

Double_t HMdcTrackFitterH::getFunctional(void) {
  setParam();
  calculationOfFunctional(4);
  return calculationOfFunctional(5);
}

void HMdcTrackFitterH::setParam(void) {
  numOfParam = 5;
  driftVelMDC1 = fitInOut->fitPar->getDriftVelocityMDC(sector,0);
  driftVelMDC2 = fitInOut->fitPar->getDriftVelocityMDC(sector,1);
  driftVelMDC3 = fitInOut->fitPar->getDriftVelocityMDC(sector,2);
  driftVelMDC4 = fitInOut->fitPar->getDriftVelocityMDC(sector,3);
  Int_t nmdc=0;
  Double_t st[6];
  Double_t wt[6];
  for(Int_t mdc=0;mdc<4;mdc++) if(nTimesInFit[mdc]) {
    nmdc++;
    if(nmdc==1) {
      fitInOut->fitPar->getSteps(sector,mdc,step);
      fitInOut->fitPar->getWeights(sector,mdc,weightPar);
      stepNorm =  fitInOut->fitPar->getStepNorm(sector,mdc);
      tukeyConst = fitInOut->fitPar->getTukeyConstant(sector,mdc);
      maxIteration = fitInOut->fitPar->getMaxIteration(sector,mdc);
    } else {
      fitInOut->fitPar->getSteps(sector,mdc,st);
      fitInOut->fitPar->getWeights(sector,mdc,wt);
      for(Int_t i=0;i<6;i++) {
        step[i] += st[i];
        weightPar[i] += wt[i];
      }
      stepNorm +=  fitInOut->fitPar->getStepNorm(sector,mdc);
      tukeyConst += fitInOut->fitPar->getTukeyConstant(sector,mdc);
      maxIteration += fitInOut->fitPar->getMaxIteration(sector,mdc);
    }
  }
  for(Int_t i=0;i<6;i++) {
    step[i] /= (Double_t)nmdc;
    weightPar[i] /= (Double_t)nmdc;
  }
  
  parameter[0] = xVertex;
  parameter[1] = yVertex;
  parameter[2] = xInitPlane;
  parameter[3] = yInitPlane;
  parameter[4] = 3;
}

Int_t HMdcTrackFitterH::execute(Double_t threshold, Int_t iter) {
  if(fprint) printf("========= Iteration %i\n",iter);
  if(iter==0 || numOfParam==0) setParam();
  Double_t stepNormW=stepNorm;
  Double_t maxDelta, delta[numOfParam];
  newFunctional=0;
//printParam();

  functional = calculationOfFunctional(1);

  for (iteration = 0; iteration < maxIteration; iteration++ ) {

    maxDelta = 0;
    for(Int_t i = 0; i < numOfParam; i++) {
      parameter[i] += stepNormW*step[i];
      delta[i] = (calculationOfFunctional(2) - functional)/(stepNormW*step[i]);
      parameter[i] -= stepNormW*step[i];
    }

    for(Int_t i = 0; i < numOfParam; i++)
      if (maxDelta < fabs(delta[i])) maxDelta = fabs(delta[i]);

    for(Int_t i = 0; i < numOfParam; i++) {
      parameter[i] -= stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;
    }
    newFunctional = calculationOfFunctional(2);

    if(fprint) printParam("out1");
    if(newFunctional < functional) functional = newFunctional;
    else {
      for(Int_t i = 0; i < numOfParam; i++)
	parameter[i] += 0.5*stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;

      newFunctional = calculationOfFunctional(2);
      if( newFunctional < functional ) functional = newFunctional;
      else {
	for(Int_t i = 0; i < numOfParam; i++)
	  parameter[i] += 0.5*stepNormW*step[i]*delta[i]*weightPar[i]/maxDelta;
	
	stepNormW /= 2;
      }
    }
    if(stepNormW >= 0.05) continue;
    if(fprint) printResult(" conversion ok ");
    fillOutput();
    return ((funMin < threshold) ? 1 : 0);
  }
  fillOutput();
  if(fprint)  printResult(" no conversion ");
  return 0;
}

void HMdcTrackFitterH::fillOutput(void) {
  funMin = calculationOfFunctional(3);
  numOfGoodWires=0;
  for(Int_t time = firstTime; time < lastTime; time++)
      if(hitStatus[time] == 1 && weight[time]>0.01) numOfGoodWires++;
  chi2perDF = funMin/(numOfGoodWires - numOfParam);
  xOutVertex = parameter[0];
  yOutVertex = parameter[1];
  zOutVertex = zVertex;
  xOutPlane = parameter[2];
  yOutPlane = parameter[3];
  zOutPlane = zInitPlane;
//   zOutPlane = parD-parA*xOutPlane-parB*yOutPlane; //!!! zInitPlane;
  arriveTime[0]=arriveTime[1]=arriveTime[2]=arriveTime[3] = parameter[4]; //!!!
}

Double_t HMdcTrackFitterH::calculationOfFunctional(Int_t iflag) {

  Double_t xV, yV, zV;
  Double_t xPlane, yPlane, zPlane;
  Double_t pV;

//  fixed zPlane
  xV = parameter[0];
  yV = parameter[1];
  zV = zVertex;

  xPlane = parameter[2];
  yPlane = parameter[3];
  zPlane = zInitPlane;
//   zPlane = parD-parA*xPlane-parB*yPlane; //!!! zInitPlane;

  pV = parameter[4];

  return deviation(xV, yV, zV, xPlane, yPlane, zPlane, pV, iflag);
}

Double_t HMdcTrackFitterH::deviation(
    Double_t xVertex, Double_t yVertex, Double_t zVertex,
    Double_t xPlane, Double_t yPlane, Double_t zPlane,
    Double_t arriveTime, Int_t iflag) {
  Bool_t flDstTime=(iflag==3 || (fprint && iflag==1)) ? kTRUE:kFALSE;
  Double_t sumDeviation, sumDeviation0;
  Double_t distance;
  Double_t driftTime;
  Double_t sumWeight;
  static Double_t width;

  sumDeviation = 0;
  sumDeviation0 = 0;
  sumWeight = 0;

//  Double_t ap, bp, cp;            // version = 0
//  Double_t dp={0},ep={0},fp={0};  // version = 0
  Double_t alphaDrDist;  // version = 1
  Bool_t isInCell=kTRUE;      // version = 1
//   if(version == 0) {
//     dp = xPlane - xVertex;
//     ep = yPlane - yVertex;
//     fp = zPlane - zVertex;
//   }

  for(Int_t time = firstTime; time < lastTime; time++) {
    if(hitStatus[time] == 0 || (hitStatus[time]<0 && !flDstTime) ) continue;

    if(version == 0) {
      //     ap = xRight[time] - xLeft[time];
      //     bp = yRight[time] - yLeft[time];
      //     cp = zRight[time] - zLeft[time];

      // distance = 
      //    fabs(ep*cp*xLeft[time] - ep*cp*xVertex - fp*bp*xLeft[time] + fp*bp*xVertex -
      //         dp*cp*yLeft[time] + dp*cp*yVertex + dp*bp*zLeft[time] - dp*bp*zVertex +
      //         ap*fp*yLeft[time] - ap*fp*yVertex - ap*ep*zLeft[time] + ap*ep*zVertex)/
      //         sqrt(ep*ep*cp*cp - 2*ep*cp*fp*bp + fp*fp*bp*bp+fp*fp*ap*ap -
      //           2*fp*ap*dp*cp+dp*dp*cp*cp + dp*dp*bp*bp - 2*dp*bp*ep*ap+ep*ep*ap*ap);
      distance=fSizeCellsLayer[time]->getDist(xVertex, yVertex, zVertex, 
          xPlane, yPlane, zPlane,lCell[time]);

      distWire[time]=distance;
      // printf("old dist=%g ",distance);
      if(lModule[time] == 0) {
        driftTime = ( distance / driftVelMDC1 )*1000.;
      } else if(lModule[time] == 1) {
        driftTime = ( distance / driftVelMDC2 )*1000.;
        //       driftTime = 5.85 + 4.30*distance + 18.88*distance*distance
        // 	- 8.54*distance*distance*distance
        // 	+ 1.48*distance*distance*distance*distance;
        //driftTime = ( distance / driftVelMDC2 )*1000.;
        //driftTime = driftTime*.9;    // for Dubna chambers B != 0
      } else if(lModule[time] == 2) {
        driftTime = ( distance / driftVelMDC3 )*1000.;
        //       driftTime =
        // 	- 0.03542
        // 	+ 78.7210 * distance
        // 	- 287.847 * pow(distance,2)
        // 	+ 512.043 * pow(distance,3)
        // 	- 454.673 * pow(distance,4)
        // 	+ 228.805 * pow(distance,5)
        // 	- 68.1692 * pow(distance,6)
        // 	+ 11.9125 * pow(distance,7)
        // 	- 1.12813 * pow(distance,8)
        // 	+ 0.04468 * pow(distance,9);

        //    1  p0    -3.54201e-02   1.48897e+00   6.61675e-04   1.67073e-06
        //    2  p1     7.87210e+01   1.25067e+00   2.56053e-04   2.88928e-06
        //    3  p2    -2.87847e+02   5.06960e-01   1.37256e-04   1.12583e-05
        //    4  p3     5.12043e+02   1.25888e-01   2.44161e-04   4.00551e-05
        //    5  p4    -4.54673e+02   2.81771e-02   2.16805e-04   9.80737e-05
        //    6  p5     2.28805e+02   5.78644e-03   1.09103e-04  -2.33797e-05
        //    7  p6    -6.81692e+01   1.12835e-03   3.25056e-05  -2.61630e-03
        //    8  p7     1.19125e+01   2.10236e-04   5.68031e-06  -2.54293e-02
        //    9  p8    -1.12813e+00   3.64331e-05   5.37935e-07  -1.89697e-01
        //   10  p9     4.46826e-02   5.50300e-06   2.13063e-08  -1.27341e+00
      } else if(lModule[time] == 3) {
        driftTime = ( distance / driftVelMDC4 )*1000.;
      } else continue;
      //printf("old driftTime=%g TDCTime=%g\n",driftTime,tdcTime[time]);
    } else {
      //new--------------------
  //     isInCell=fSizeCellsLayer[time]->getImpact(
  //         xVertex, yVertex, zVertex, xPlane, yPlane, zPlane,
  //         lCell[time], alphaDrDist, distance);
      distance=fSizeCellsLayer[time]->getImpact(xVertex, yVertex, zVertex, 
          xPlane, yPlane, zPlane,lCell[time], alphaDrDist);
      distWire[time]=distance;
      driftTime=fCal2ParSim->
          calcTime(sector,lModule[time],alphaDrDist,distance);

  //     ap = xRight[time] - xLeft[time];
  //     bp = yRight[time] - yLeft[time];
  //     cp = zRight[time] - zLeft[time];

  //     distanceMy = (ep*cp*xLeft[time] - ep*cp*xVertex - fp*bp*xLeft[time] + fp*bp*xVertex -
  //                dp*cp*yLeft[time] + dp*cp*yVertex + dp*bp*zLeft[time] - dp*bp*zVertex +
  //                ap*fp*yLeft[time] - ap*fp*yVertex - ap*ep*zLeft[time] + ap*ep*zVertex)/
  //                sqrt(ep*ep*cp*cp - 2*ep*cp*fp*bp + fp*fp*bp*bp+fp*fp*ap*ap -
  //                  2*fp*ap*dp*cp+dp*dp*cp*cp + dp*dp*bp*bp - 2*dp*bp*ep*ap+ep*ep*ap*ap);
  //     distanceMy = fabs(distanceMy);
  //     driftTimeMy = 5.85 + 4.30*distanceMy + 18.88*distanceMy*distanceMy
  //                   - 8.54*distanceMy*distanceMy*distanceMy
  //                   + 1.48*distanceMy*distanceMy*distanceMy*distanceMy;

  //       driftTimeJM = 5.85 + 4.30*distance + 18.88*distance*distance
  // 	- 8.54*distance*distance*distance
  // 	+ 1.48*distance*distance*distance*distance;
  //       driftTimeMyJM = fCal2ParSim->calcTime(sector,lModule[time],alphaDrDist,distanceMy);
  //       cout << xVertex << " " << yVertex << " " << zVertex << " "
  // 	   << xPlane << " " << yPlane << " " << zPlane << " "
  // 	   << distance << " " << distanceMy << " " << driftTime << " "
  // 	   << driftTimeMy << " " << driftTimeJM << " " << driftTimeMyJM << endl;


      //printf("new dist=%g \n",distance);
      //Printf("d=%f->%f a=%f %i",fabs(distance),driftDist,alphaDrDist,isIn);
      //printf("new driftTime=%g ",driftTime);
      //-----------------------
    }

    drTime[time] = driftTime;
    if (iflag == 5) {
      driftTime = ( distance / driftVelMDC4 )*1000.;
      drTime[time] = driftTime;
//       if(segment == 0 && drTime[time] > 200) drTime[time] = 200;
//       if(segment == 1 && drTime[time] > 1000) drTime[time] = 1000;
    }

    dev[time] = drTime[time] + arriveTime - tdcTime[time];

//      if (iflag == 1) weight = qualityWeight[time];
    if (iflag == 1) {
      weight[time] = 1;
      qualityWeight[time] = 1;
      if(version==0) {
        if( (segment == 0 && drTime[time] > 200) ||
            (segment == 1 && drTime[time] > 1000) ) {
          qualityWeight[time] = 0;
          hitStatus[time]=-1;
        }
      } else {
        if(!isInCell) qualityWeight[time] = 0;
      }
    } else if (iflag == 4) {
      weight[time] = 1;
      qualityWeight[time] = 1;
    } else {
      weight[time]=(fabs(dev[time]) < width) ?
          pow( (1 - pow((dev[time]/width),2)), 2) : 0;
    }

    weight[time] *= qualityWeight[time];

    if(hitStatus[time]==1) {
      sumWeight += weight[time];
      sumDeviation += weight[time]*dev[time]*dev[time] ;
    }

    if(fprint) {
      if((iflag == 1 || iflag == 3)) {
        printf(" %s %3i)%c tof=%6.2f",(iflag==1) ? "->":"<-" ,time,
            (hitStatus[time]==1) ? '+':'-',arriveTime);
        printf(" %5.2fmm %s",distWire[time], "???"); //(inCell[time]) ? "   ":"OUT");
        printf(" drTm=%6.2f fTm=%6.2f",
            drTime[time]-arriveTime,drTime[time]+arriveTime);
        printf(" TDC=%6.2f dev=%7.2f WT=%g\n",
               tdcTime[time],dev[time],weight[time]);
      }
    }

  }

  width = tukeyConst*sqrt(sumDeviation/sumWeight);
  if(fprint && iflag == 3) printf("========================= width=%f\n",width);

  return sumDeviation;
}

Bool_t HMdcTrackFitterH::finalize() {
  return kTRUE;
}

void HMdcTrackFitterH::printResult(Char_t* status) const{
  printf("%s\n",status);
  printParam("final");
}

void HMdcTrackFitterH::printParam(Char_t* title) const {
  printf("%s %3i Par.=%-6g %-6g %-6g %-6g %-6g  fun.=%-6g->%-6g\n",
      title,iteration,parameter[0],parameter[1],parameter[2],parameter[3],
      parameter[4],functional,newFunctional);
}

