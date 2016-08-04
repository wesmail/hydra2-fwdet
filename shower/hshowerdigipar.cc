//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////
//
// HShowerDigiPar - parameters of digitalisation
//
/////////////////////////////////////////////////////////

#include "hshowerdigipar.h"
#include "hhistconverter.h"
#include "hparamlist.h"
#include "hpario.h"
#include "hdetpario.h"
#include "TMath.h"
#include "TRandom.h"

#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include <cmath>
using namespace std;

ClassImp(HShowerDigiPar)

//------------------------------------------------------------------------------
HShowerDigiPar::HShowerDigiPar(const Char_t* name,const Char_t* title,
                                      const Char_t* context)
                 : HParCond(name,title,context) {
  gain.Set(18);
  threshold.Set(18);
  effScaleMap.Set(18432);  // 6*3*32*32
  clear();
  pfChargeMatrix = NULL;
  phEff          = NULL;
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      ph2QvB[s][m]    = NULL;
      pArrayQvB[s][m] = NULL;
      nQvBxbins[s][m] = 0;
    }
  }
  nMatrixRange     = 0;
  chargeMatrixSize = 0;
  nGlobalEffBins   = 0;
}

//------------------------------------------------------------------------------

HShowerDigiPar::~HShowerDigiPar() {
  removeHistograms();
}

//------------------------------------------------------------------------------

void HShowerDigiPar::clear(void) {
  gain.Reset(0.f);
  threshold.Reset(0.f);
  effScaleMap.Reset(1.f);
  globalEff.Set(0);
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      chargeVsBeta[s][m].Set(0);
    }
  }
  fThickDet           = 1.0f;
  nMatrixRange        = 0;
  fBoxSize            = 1.0f;
  fChargeSlope        = 1.0f;
  fPlaneDist          = 1.0f;
  fPadThreshold       = 0.0f;
  fUpdatePadThreshold = 0.0f;
  status=kFALSE;
  resetInputVersions();
  changed=kFALSE;
}

//------------------------------------------------------------------------------

void HShowerDigiPar::removeHistograms(void) {
  // removes all histograms
  removeChargeHistograms();
  removeEfficiencyHistograms();
  removeQvBHistograms();
}

//------------------------------------------------------------------------------

void HShowerDigiPar::removeChargeHistograms(void) {
  if (pfChargeMatrix != NULL) {
    delete [] pfChargeMatrix;
    pfChargeMatrix = NULL;
  }
  chargeMatrixSize = 0;
}

//------------------------------------------------------------------------------

void HShowerDigiPar::removeEfficiencyHistograms(void) {
  if (phEff != NULL) {
    delete phEff;
    phEff = NULL;
  }
  nGlobalEffBins = 0;
}

//------------------------------------------------------------------------------

void HShowerDigiPar::removeQvBHistograms(void) {
  nGlobalEffBins = 0;
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      if (ph2QvB[s][m] != NULL) {
        delete ph2QvB[s][m];
        ph2QvB[s][m] = NULL;
      }
      if (pArrayQvB[s][m] != NULL) {
        pArrayQvB[s][m]->Delete();
        delete pArrayQvB[s][m];
        pArrayQvB[s][m] = NULL;
      }
      nQvBxbins[s][m]= 0;
    }
  }
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::init(HParIo* inp,Int_t* set) {
  Bool_t rc=kFALSE;
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HCondParIo");
  if (input) rc=input->init(this,set);
  if (rc && changed) {
    rc=recreateHistograms();
  }	
  return rc;
}

//------------------------------------------------------------------------------

void HShowerDigiPar::putParams(HParamList* l) {
  // Puts all params to the parameter list of type HParamList
  // (which is used by the ASCII and Oracle io);
  if (!l) return;
  l->add("gain"                ,gain);
  l->add("threshold"           ,threshold);
  l->add("fThickDet"           ,fThickDet);
  l->add("nMatrixRange"        ,nMatrixRange);
  l->add("fBoxSize"            ,fBoxSize);
  l->add("fChargeSlope"        ,fChargeSlope);
  l->add("fPlaneDist"          ,fPlaneDist);
  l->add("fPadThreshold"       ,fPadThreshold);
  l->add("fUpdatePadThreshold" ,fUpdatePadThreshold);
  l->add("effScaleMap"         ,effScaleMap);
  l->add("globalEff"           ,globalEff);
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      l->add(Form("chargeVsBeta%i%i",s,m),chargeVsBeta[s][m]);
    }
  }
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::getParams(HParamList* l) {
  if (!l) return kFALSE;
  if( !(l->fill("gain"               ,&gain)))                return kFALSE;
  if( !(l->fill("threshold"          ,&threshold)))           return kFALSE;
  if (!(l->fill("fThickDet"          ,&fThickDet)))	      return kFALSE;
  if (!(l->fill("nMatrixRange"       ,&nMatrixRange)))	      return kFALSE;
  if (!(l->fill("fBoxSize"           ,&fBoxSize)))            return kFALSE;
  if (!(l->fill("fChargeSlope"       ,&fChargeSlope)))	      return kFALSE;
  if (!(l->fill("fPlaneDist"         ,&fPlaneDist)))          return kFALSE;
  if (!(l->fill("fPadThreshold"      ,&fPadThreshold)))       return kFALSE;
  if (!(l->fill("fUpdatePadThreshold",&fUpdatePadThreshold))) return kFALSE;
  if (!(l->fill("effScaleMap"        ,&effScaleMap)))         return kFALSE;
  if (!(l->fill("globalEff"          ,&globalEff)))	      return kFALSE;
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      if (!(l->fill(Form("chargeVsBeta%i%i",s,m),&chargeVsBeta[s][m]))) return kFALSE;
    }
  }
  return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::recreateHistograms() {
  removeHistograms();
  Bool_t rc=initChargeMatrix();
  if (rc) rc=initEffHistogram();
  if (rc) rc=initSumVersBetaHistograms();
  return rc;
}

//------------------------------------------------------------------------------

void HShowerDigiPar::setChargeMatrix(Int_t nRange, const Float_t *pMatrix) {
  if(pfChargeMatrix != NULL) delete [] pfChargeMatrix;
  nMatrixRange = nRange;
  if (nMatrixRange == 0) pfChargeMatrix = NULL;
  else {
    chargeMatrixSize = 2 * nMatrixRange + 1;
    pfChargeMatrix = new Float_t[chargeMatrixSize];
    if (pMatrix != NULL) {
      memcpy(pfChargeMatrix, pMatrix, sizeof(Float_t) * chargeMatrixSize);
    } else
      memset(pfChargeMatrix, 0, sizeof(Float_t) * chargeMatrixSize);
  }
}

//------------------------------------------------------------------------------

Float_t HShowerDigiPar::calcCharge(Float_t fCharge, Float_t fDist,
                        Float_t fXd, Float_t fYd, Float_t fXu, Float_t fYu) {
  return ((fCharge / TMath::TwoPi()) *
           (atan(fXd * fYd / (fDist * sqrt(fDist * fDist + fXd * fXd + fYd * fYd)))
            - atan(fXd * fYu / (fDist * sqrt(fDist * fDist + fXd * fXd + fYu * fYu)))
            + atan(fXu * fYu / (fDist * sqrt(fDist * fDist + fXu * fXu + fYu * fYu)))
            - atan(fXu * fYd / (fDist * sqrt(fDist * fDist + fXu * fXu + fYd * fYd)))
           ));
}

//------------------------------------------------------------------------------

Bool_t  HShowerDigiPar::initChargeMatrix(void) {
  Float_t cornerX = -0.5 * fBoxSize;
  Float_t cornerY = cornerX;
  setChargeMatrix(nMatrixRange);
  if (pfChargeMatrix==NULL) {
    Error("initChargeMatrix()", "No charge matrix");
    return kFALSE;
  }
  for(Int_t i = 0; i <= nMatrixRange; i++) {
    pfChargeMatrix[nMatrixRange - i] = calcCharge(1., fPlaneDist,
                                                  cornerX + i * fBoxSize,
                                                  cornerY,
                                                  cornerX + (i + 1) * fBoxSize,
                                                  cornerY + fBoxSize);
    Float_t fM = (Float_t)(1.0 - fChargeSlope * i / nMatrixRange);
    if(fM < 0.0f) fM = 0.0f;
    //printf("%d: %f\n", i, fM);
    pfChargeMatrix[nMatrixRange - i] *= fM;
    pfChargeMatrix[nMatrixRange + i] = pfChargeMatrix[nMatrixRange - i];
  }
  for(Int_t i = 1; i < chargeMatrixSize; i++) {
    pfChargeMatrix[i] += pfChargeMatrix[i - 1];
  }
  return kTRUE;
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::initEffHistogram(void) {
  phEff = (TH1D*)HHistConverter::createHist("phEff",globalEff);
  if(phEff == NULL) {
    Error("initEffHistogram()", "No efficiency histogram");
    return kFALSE;
  }
  nGlobalEffBins = phEff->GetNbinsX();
  return kTRUE;
}

//------------------------------------------------------------------------------

Float_t HShowerDigiPar::getEfficiency(Int_t sec, Int_t mod, Int_t row, Int_t col,
                                      Float_t fBeta) {
  Int_t iBin = phEff->GetXaxis()->FindFixBin((Double_t)fBeta);
  //  if(iBin >= nGlobalEffBins) iBin = nGlobalEffBins - 1;
  return (Float_t)(phEff->GetBinContent(iBin) * effScaleMap[padIndex(sec,mod,row,col)]);
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::checkEfficiency(Int_t sec, Int_t mod, Int_t row,Int_t col,
                                       Float_t fBeta) {
  Float_t f = getEfficiency(sec,mod,row,col,fBeta);
  return ((f >= 1.0f) || ((f > 0.0f) && (f > gRandom->Rndm())));
}

//------------------------------------------------------------------------------

Bool_t HShowerDigiPar::initSumVersBetaHistograms(void) {
  for(Int_t s = 0; s < 6; s ++) {
    for(Int_t m = 0; m < 3; m ++) {
      TH2D *ph2 = (TH2D*)HHistConverter::createHist(Form("ph2QvB%i%i",s,m),
                                                    chargeVsBeta[s][m]);
      if(ph2 == NULL) {
        Error("%s", "No QvB histogram for sector %i, module %i",s,m);
        return kFALSE;
      }
      Int_t nBins = ph2->GetNbinsX();
      TObjArray *pArr = new TObjArray(nBins);
      pArr->SetOwner();
      for(Int_t iBeta = 0; iBeta < nBins; iBeta++) {
        TH1D *ph1 = ph2->ProjectionY(Form("sumVersBeta%i%i_%i",s,m,iBeta),
                                     iBeta, iBeta+1);
        ph1->SetDirectory(NULL);
        ph1->SetTitle(Form("sum_%03.0f_%03.0f",
                      ph2->GetXaxis()->GetBinLowEdge(iBeta),
                      ph2->GetXaxis()->GetBinUpEdge(iBeta)));
        pArr->AddAt(ph1, iBeta);
      }
      ph2QvB[s][m]    = ph2;
      nQvBxbins[s][m] = nBins;
      pArrayQvB[s][m] = pArr;
    }
  }
  return kTRUE;
}

//------------------------------------------------------------------------------

Float_t HShowerDigiPar::getCharge(Int_t sec,Int_t mod, Float_t fBeta) {
  TAxis* axis = ph2QvB[sec][mod]->GetXaxis();
  Int_t iBin = axis->FindFixBin((Double_t)fBeta);
  if(iBin > nQvBxbins[sec][mod]) iBin = nQvBxbins[sec][mod];
  if(iBin<0) iBin = 1;
  Double_t width = axis->GetBinWidth(iBin);
  TH1D *ph1 = (TH1D*)(pArrayQvB[sec][mod]->At(--iBin));
  if(ph1 == NULL) return 0.0f;
  Double_t fQ = ph1->GetRandom() + (2 * gRandom->Rndm() - 1) * width;
  return ((fQ < 0.0) ? 0.0f : (Float_t)fQ);
}

//------------------------------------------------------------------------------

void HShowerDigiPar::setEffScaleMap(TArrayF& arr) {
  Int_t n=arr.GetSize();
  if (n==18432) {
    effScaleMap.Set(n,arr.GetArray());
  } else {
    Error("setEffScaleMap(TArrayF&)","Array size %i differs from 6*3*32*32=18432",n);
  }
}  

//------------------------------------------------------------------------------

void HShowerDigiPar::setGlobalEff(TArrayD& arr) {
  removeEfficiencyHistograms();
  globalEff.Set(arr.GetSize(),arr.GetArray());  
}

//------------------------------------------------------------------------------

void HShowerDigiPar::setChargeVsBeta(Int_t sec, Int_t mod, TArrayD& arr) {
  removeQvBHistograms();
  chargeVsBeta[sec][mod].Set(arr.GetSize(),arr.GetArray());
}
