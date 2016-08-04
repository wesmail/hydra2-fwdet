//*--- Author : V.Pechenov
//*--- Modified: 16.08.05 V.Pechenov

using namespace std;
#include <iostream>
#include <iomanip>

#include "hmdctrackparam.h"
#include "hmdcclusfit.h"
#include "hmdcgeomobj.h"
#include "hmdcwiredata.h"


//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
//
//  HMdcTrackParam
//
//    Class keep track fit parameters
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HMdcTrackParam)

HMdcTrackParam::HMdcTrackParam(void) {
  nParam         = 4;
  timeOffsetFlag = 3;
  numOfGoodWires = 0;
  isTmoffFixed   = 0;
  iterNumb       = 0;
  chi2perDF      = -1.;
  funct          = -1.;
  oldFunct       = -1.;
  nMods          = 4;
}

void HMdcTrackParam::copyNewParam(const HMdcTrackParam& tp) {
  copyParam(tp);
  copyTimeOffsets(tp);
  funct          = tp.funct;
  iterNumb       = tp.iterNumb;
  numOfGoodWires = tp.numOfGoodWires;
  chi2perDF      = tp.chi2perDF;
  sumWeight      = tp.sumWeight;
  nMods          = tp.nMods;
  for(Int_t m=0;m<nMods;m++) sWht[m] = tp.sWht[m]; //!!!!!!!!!!!!! mozhet perenesti v wires arr.
}

void HMdcTrackParam::copyParAndAdd(const HMdcTrackParam& tp, Int_t ip, Double_t add) {
  copyParam(tp);
  addToParam(ip,add);
  isTmoffFixed = tp.isTmoffFixed;
  nMods        = tp.nMods;
}

void HMdcTrackParam::copyParAndAdd(const HMdcTrackParam& tp,
    Int_t ip1, Double_t add1, Int_t ip2, Double_t add2) {
  copyParam(tp);
  addToParam(ip1,add1,ip2,add2);
  isTmoffFixed = tp.isTmoffFixed;
  nMods        = tp.nMods;
}

void HMdcTrackParam::clearFunct(void) {
  funct        = 0.;
  sumWeight    = 0.;
  if(isTmoffFixed>0) isTmoffFixed = 0;
  for(Int_t m=0;m<nMods;m++) {
    sDev[m] = 0.;
    sWht[m] = 0.;
  }
}

void HMdcTrackParam::calcTimeOffsets(Int_t tofFlag) {
  // tofFlag=1 - calcul. timeOffsets for each Mdc
  // tofFlag=2 - calcul. timeOffsets for each segment
  // else      - calcul. one timeOffset for all mdc's
  if(isTmoffFixed < 0) return;
  timeOffsetFlag = tofFlag;
  if     (timeOffsetFlag==0) for(Int_t m=0;m<nMods;m++) timeOffset[m] = 0.;
  else if(timeOffsetFlag==1) for(Int_t m=0;m<nMods;m++) timeOffset[m] = sWht[m]>0. ? -sDev[m]/sWht[m]:0.;
  else if(timeOffsetFlag==2) {
    for(Int_t m=0;m<nMods;m+=2) {
      Double_t sW     = sWht[m]+sWht[m+1];
      Double_t tos    = sW >0. ? -(sDev[m]+sDev[m+1])/sW : 0.;
      timeOffset[m]   = sWht[m]  >0. ? tos : 0.;
      timeOffset[m+1] = sWht[m+1]>0. ? tos : 0.;
    }
  } else {
    Double_t sD = 0;
    Double_t sW = 0;
    for(Int_t m=0;m<nMods;m++) sD += sDev[m];
    for(Int_t m=0;m<nMods;m++) sW += sWht[m];
    Double_t tos = -sD/sW;
    for(Int_t m=0;m<nMods;m++) timeOffset[m] = sWht[m]>0. ? tos : 0.; // ! ???
  }
  isTmoffFixed = 0;
}

Double_t HMdcTrackParam::getSumWtNorm(Int_t m) const {
  // Return sum of weight/errTdcTime^2 for all valid wires
  if(isTmoffFixed<0 || m<0 || m>=nMods || timeOffsetFlag==0) return 0.;
  if(timeOffsetFlag == 3) {                           // One Toff per track
    Double_t sW = 0;
    for(Int_t im=0;im<nMods;im++) sW += sWht[im];
    return sW;
  }
  if(timeOffsetFlag == 2) {                         // One Toff per segment
    m &= 254;  // = (m/2)*2
    return sWht[m]+sWht[m+1];
  } 
  if(timeOffsetFlag == 1) return sWht[m];           // One Toff per module
  return 0.;
}

void HMdcTrackParam::correctMinTimeOffsets(Double_t minTos) {
  // if timeOffset[m]<minTos timeOffset[m]=minTos
  if(isTmoffFixed<0) return;
  for(Int_t m=0;m<nMods;m++) if(sWht[m]>0. && timeOffset[m]<minTos) {
    timeOffset[m] = minTos;
    isTmoffFixed |= 1<<m;
  }
}

void HMdcTrackParam::addToTOffsetErr(Int_t m, Double_t* dTdA, Double_t wtNorm) {
  for(Int_t ip=0;ip<nParam;ip++) dTdPar[ip][m] += dTdA[ip]*wtNorm;
}

void HMdcTrackParam::clearTOffsetDer(void) {
  for(Int_t ip=0;ip<nParam;ip++) for(Int_t m=0;m<nMods;m++) dTdPar[ip][m] = 0.;
}

void HMdcTrackParam::getTimeOffsetDer(Double_t* der) {
  for(Int_t k=0;k<nParam;k++) der[k] = 0.;
  if(isTmoffFixed<0 || timeOffsetFlag==0) return;
  if(timeOffsetFlag==1) {                                // One timeoffset per module:
    for(Int_t m=0;m<nMods;m++) if(sWht[m]>0. && !isMdcTimeOffsetFixed(m)) {
      for(Int_t k=0;k<nParam;k++) der[k] -= dTdPar[k][m]/sWht[m];
    }
  } else if(timeOffsetFlag==2) {                         // One timeoffset per segment:
    for(Int_t m=0;m<nMods;m+=2) {
      Double_t sW = sWht[m]+sWht[m+1];
      if(sW>0. && !isSegTimeOffsetFixed(m/2)) for(Int_t k=0;k<nParam;k++)
        der[k] -= (dTdPar[k][m]+dTdPar[k][m+1])/sW;
    }
  } else {                                               // One timeoffset per track:
    Double_t sW = 0;
    for(Int_t m=0;m<nMods;m++) sW += sWht[m];
    if(sW>0. && !isTrackTimeOffsetFixed()) for(Int_t k=0;k<nParam;k++) {
      Double_t sDT = 0.;
      for(Int_t m=0;m<nMods;m++) sDT += dTdPar[k][m];
      der[k] = -sDT/sW;
    }
  }
}

void HMdcTrackParam::addTimeOffsetDer1(TMatrixD& grad2) {
  if(isTmoffFixed<0 || timeOffsetFlag==0) return;
  if(timeOffsetFlag==1) {                       // One timeoffset per module:
    for(Int_t m=0;m<nMods;m++) if(sWht[m]>0. && !isMdcTimeOffsetFixed(m)) {
      for(Int_t k=0;k<nParam;k++) 
          grad2(k,k) -= 2.*dTdPar[k][m]*dTdPar[k][m]/sWht[m];
    }
  } else if(timeOffsetFlag==2) {                // One timeoffset per segment:
    for(Int_t m=0;m<nMods;m+=2) {
      Double_t sW  = sWht[m]+sWht[m+1];
      if(sW>0. && !isSegTimeOffsetFixed(m/2)) for(Int_t k=0;k<nParam;k++) {
        Double_t dTdP = dTdPar[k][m]+dTdPar[k][m+1];
        grad2(k,k)   -= 2.*dTdP*dTdP/sW;
      }
    }
  } else {                                      // One timeoffset per track:
    Double_t sW = 0;
    for(Int_t m=0;m<nMods;m++) sW += sWht[m];
    if(sW>0. && !isTrackTimeOffsetFixed()) for(Int_t k=0;k<nParam;k++) {
      Double_t dTdP = 0;
      for(Int_t m=0;m<nMods;m++) dTdP += dTdPar[k][m];
      grad2(k,k) -= 2.*dTdP*dTdP/sW;
    }
  }
}

void HMdcTrackParam::addTimeOffsetDer2(TMatrixD& grad2) {
  if(isTmoffFixed<0 || timeOffsetFlag==0) return;
  if(timeOffsetFlag==1) {                          // One timeoffset per module:
    for(Int_t m=0;m<nMods;m++) if(sWht[m]>0. && !isMdcTimeOffsetFixed(m)) {
      for(Int_t k=0;k<nParam;k++) {
        Double_t dTdP = -2.*dTdPar[k][m]/sWht[m];
        for(Int_t l=0;l<=k;l++) grad2(k,l) += dTdP*dTdPar[l][m];
      }
    }
  } else if(timeOffsetFlag==2) {                   // One timeoffset per segment:
    for(Int_t m=0;m<nMods;m+=2) {
      Double_t sW  = sWht[m]+sWht[m+1];
      if(sW>0. && !isSegTimeOffsetFixed(m/2)) for(Int_t k=0;k<nParam;k++) {
        Double_t dTdP = -2.*(dTdPar[k][m]+dTdPar[k][m+1])/sW;
        for(Int_t l=0;l<=k;l++) grad2(k,l) += dTdP*(dTdPar[l][m]+dTdPar[l][m+1]);
      }
    }
  } else {                                         // One timeoffset per track:
    Double_t sW = 0;
    for(Int_t m=0;m<nMods;m++) sW += sWht[m];
    if(sW>0. && !isTrackTimeOffsetFixed()) for(Int_t k=0;k<nParam;k++) {
      Double_t dTdPk = 0;
      for(Int_t m=0;m<nMods;m++) dTdPk += dTdPar[k][m];
      dTdPk *= -2./sW;
      for(Int_t l=0;l<=k;l++) {
        Double_t dTdPl = 0;
        for(Int_t m=0;m<nMods;m++) dTdPl += dTdPar[l][m];
        grad2(k,l) += dTdPk*dTdPl;
      }
    }
  }
}

void HMdcTrackParam::calcTimeOffsetsErr(void) {
  if(isTmoffFixed<0 || timeOffsetFlag==0) return;
  if(timeOffsetFlag==1) for(Int_t m=0;m<nMods;m++) errTimeOffset[m] = calcTosErr(m);
  else if(timeOffsetFlag==2) {
    for(Int_t m=0;m<nMods;m+=2) {
      Double_t err = calcTosErr(m,m+1);
      errTimeOffset[m]   = sWht[m]  >0. ? err : 0.;
      errTimeOffset[m+1] = sWht[m+1]>0. ? err : 0.;
    }
  } else {
    Double_t err = calcTosErr();
    for(Int_t m=0;m<nMods;m++) errTimeOffset[m] = sWht[m]>0. ? err : 0.;
  }
}

Double_t HMdcTrackParam::calcTosErr(Int_t m) {
  if(isTmoffFixed < 0) return 0.;
  if(sWht[m]==0.) return 0.;
  Double_t sum=0.;
  for(Int_t ip=0; ip<nParam; ip++) sum += calcTosErr(sWht[m],dTdPar[ip][m],errMatr(ip,ip));
  return sqrt(sum);
}

Double_t HMdcTrackParam::calcTosErr(Int_t m1, Int_t m2) {
  if(isTmoffFixed<0) return 0.;
  if(sWht[m1]==0. && sWht[m2]==0.) return 0.;
  Double_t sum=0.;
  if(sWht[m1]>0. || sWht[m2]>0.) for(Int_t ip=0; ip<nParam; ip++) sum +=
      calcTosErr(sWht[m1]+sWht[m2],dTdPar[ip][m1]+dTdPar[ip][m2],errMatr(ip,ip));
  return sqrt(sum);
}

Double_t HMdcTrackParam::calcTosErr(void) {
  if(isTmoffFixed<0) return 0.;
  Double_t sW = 0;
  for(Int_t m=0;m<nMods;m++) sW += sWht[m];
  Double_t sum=0.;
  for(Int_t ip=0; ip<nParam; ip++) {
    Double_t sTd = 0;
    for(Int_t m=0;m<nMods;m++) sTd += dTdPar[ip][m];
    sum += calcTosErr(sW,sTd,errMatr(ip,ip));
  }
  return sqrt(sum);
}

void HMdcTrackParam::fillErrorsMatr(TMatrixD& matrH) {
  for(Int_t i=0; i<nParam; i++) for(Int_t j=i; j<nParam; j++)
      errMatr.setElement(i,j,matrH(i,j));
}

Double_t HMdcTrackParam::calcChi2PerDF(Int_t nGWires) {
  if(nGWires>0) numOfGoodWires=nGWires;
  chi2perDF = (numOfGoodWires>nParam) ? funct/(numOfGoodWires - nParam) : -1.;
  return chi2perDF;
}

void HMdcTrackParam::printParam(const Char_t* title) const {
  if(title) printf("%s ",title);
  Char_t zf=(funct<=oldFunct) ? '+':'-';
  printf("%3i%c fun.=%5.4g=>%5.4g Par.=%6.2f %6.2f %6.2f %6.2f TOF=",
      iterNumb,zf,oldFunct,funct,point1.X(),point1.Y(),point2.X(),point2.Y());
  for(Int_t m=0;m<nMods;m++) printf("%5.1f ",timeOffset[m]);
  printf("\n");
}

void HMdcTrackParam::printFunctChange(const Char_t* title) const {
  if(title) printf("%s ",title);
  printf("fun.=%-6g->%-6g\n",oldFunct,funct);
}

void HMdcTrackParam::printErrors(void) {
  errMatr.print();
  printf("  timeoffsets:");
  for(Int_t m=0;m<nMods;m++) if(errTimeOffset[m]>0)
      printf(" mod%i=%g+/-%g",m+1,timeOffset[m],errTimeOffset[m]);
  printf("\n");
}

void HMdcTrackParam::fillClusFit(HMdcClusFit* fClusFit) {
  fClusFit->setNParam(nParam);
  fClusFit->setFunMin(funct);
  fClusFit->setTimeOff(timeOffset);
  fClusFit->setX1(point1.X());
  fClusFit->setY1(point1.Y());
  fClusFit->setZ1(point1.Z());
  fClusFit->setX2(point2.X());
  fClusFit->setY2(point2.Y());
  fClusFit->setZ2(point2.Z());
  fClusFit->setNumIter(iterNumb);
  fClusFit->setNumOfWires(numOfGoodWires);
  fClusFit->setErrors(errMatr.getErr(0),errMatr.getErr(1),errMatr.getErr(2),errMatr.getErr(3));
}

Bool_t HMdcTrackParam::testParameters(Double_t tosMin, Double_t tosMax) {
  if(isTmoffFixed<0) return kTRUE;
  for(Int_t m=0;m<nMods;m++) if(timeOffset[m]<tosMin||timeOffset[m]>tosMax) return kFALSE;
  return kTRUE;
}

void HMdcTrackParam::setFixedTimeOffset(Double_t o1,Double_t o2,Double_t o3,Double_t o4) {
  timeOffset[0] = o1;
  timeOffset[1] = o2;
  timeOffset[2] = o3;
  timeOffset[3] = o4;
  isTmoffFixed  = -1;
  for(Int_t m=4;m<16;m++) timeOffset[m] = 0;
}

void HMdcTrackParam::setTimeOffsets(Double_t* tos,Int_t size) {
  if(size<0) size = 0;
  if(size>0 && size<16) for(Int_t m=0;m<size;m++) timeOffset[m] = tos[m];
  for(Int_t m=size;m<16;m++) timeOffset[m] = 0;
}

void HMdcTrackParam::copyPlanes(HMdcTrackParam& p) {
  ((HMdcLineParam*)this)->copyPlanes(p);
  nMods = p.nMods;
}
void HMdcTrackParam::cleanTO(void) {
  for(Int_t m=0;m<nMods;m++) timeOffset[m] = 0.;
}
