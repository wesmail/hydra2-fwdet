//*-- Author : Vladimir Pechenov 24.11.2011

#include "halignmentmeta.h"
#include "TMinuit.h"
#include "TFile.h"
#include "TString.h"
#include "TNtuple.h"
#include "hmdcsizescells.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspecgeompar.h"
#include "hgeomvolume.h"
#include <iostream>

using namespace std;

ClassImp(HAlignmentMeta)
    
HAlignmentMeta::HAlignmentMeta() {
  trackSelecCutX  = 1.5;
  trackSelecCutY  = 1.0;
  filterFlag      = kTRUE;
  nMetaModules    = 0;
  metaDetector    = 3;
  fitTofModYPos   = kFALSE;
  xShitfRpc       = 0.;
  calcCellXOffset = kFALSE;
  nCells          = 0;
  nCellsTot       = 0;
}

void HAlignmentMeta::setTofDetector(Double_t cutX,Double_t cutY) {
  metaDetector = 0;
  nMetaModules = 8;
  setCuts(cutX,cutY);
  nCells       = 8;
  nCellsTot    = nMetaModules*nCells;
}

void HAlignmentMeta::setShowerDetector(Double_t cutX,Double_t cutY) {
  metaDetector = 1;
  nMetaModules = 1;
  setCuts(cutX,cutY);
  nCells       = 32;       // number of rows
  nCellsTot    = nCells;
}

void HAlignmentMeta::setRpcDetector(Double_t cutX,Double_t cutY) {
  metaDetector = 2;
  nMetaModules = 1;
  setCuts(cutX,cutY);
  nCells       = 32;
  nCellsTot    = 6*nCells; // 6-number of columns in RPC
}

void HAlignmentMeta::setCuts(Double_t cutX,Double_t cutY) {
  trackSelecCutX = cutX;
  trackSelecCutY = cutY;
  if(trackSelecCutX > 0. && trackSelecCutY > 0.) filterFlag = kTRUE;
  else                                           filterFlag = kFALSE;
}

void HAlignmentMeta::setNtuple(TNtuple *ntp) {
  nt = ntp;
  nt -> SetBranchAddress("s",&sec);
  nt -> SetBranchAddress("x1s",&x1); // yuxari dubna 
  nt -> SetBranchAddress("y1s",&y1); // yuxari dubna
  nt -> SetBranchAddress("z1s",&z1);
  nt -> SetBranchAddress("x2s",&x2); // yuxari gsi 
  nt -> SetBranchAddress("y2s",&y2); // yuxari gsi
  nt -> SetBranchAddress("z2s",&z2);
  nt -> SetBranchAddress("metaMod",&metaModule);
  nt -> SetBranchAddress("col",&metaColumn);
  nt -> SetBranchAddress("cell",&metaCell);
  nt -> SetBranchAddress("xMeta",&xMetaLocal); // ashagi gsi
  nt -> SetBranchAddress("yMeta",&yMetaLocal);
  nt -> SetBranchAddress("zMeta",&zMetaLocal);
  nt -> SetBranchAddress("sigX",&xRMS);
  nt -> SetBranchAddress("sigY",&yRMS);
  nt -> SetBranchAddress("sigZ",&zRMS);
//nt -> SetBranchAddress("beta",&beta);
}

HAlignmentMeta::~HAlignmentMeta() {
  if(gMinuit != NULL) delete gMinuit;
}

void HAlignmentMeta::fcnMeta(Int_t &npar, Double_t *gin, Double_t &fn, Double_t *par, Int_t iflag) {
  static int count = 1;
  count++;
  HAlignmentMeta *myObject   = (HAlignmentMeta*)(gMinuit -> GetObjectFit());
  fn = myObject->getMinFunction(par);
  if(count%100 == 0)  cout<<"iter:  "<<count<<"  function  "<<fn<<" "<<par[0]<<"  "<<par[1]<<"  "
                      <<par[2]<<"  "<<par[3]<<"  "<<par[4]<<"  "<<par[5]<< endl;
}

void HAlignmentMeta::alignMeta(Int_t sec,TNtuple *ntp) {
  if(metaDetector<0 || metaDetector>2) {
    Error("alignMeta","Meta detector type is not setted! Stop!");
    exit(1);
  }
  alignSec = sec;
  setNtuple(ntp);

  HSpecGeomPar*         fSpecGeomPar = (HSpecGeomPar*)gHades->getRuntimeDb()->getContainer("SpecGeomPar");
  const HGeomTransform& secLabTrans  = fSpecGeomPar->getSector(alignSec)->getTransform();
  
  xShitfRpc = 0.;
  for(Int_t im=0;im<nMetaModules;im++) {
    transMetaModSecOld[im] = transMetaModLabOld[im];
    transMetaModSecOld[im].transTo(secLabTrans);  // trans.from lab. to sector coor.sys.
  }

  Int_t ierflg = 0;
  if(gMinuit == NULL) {
    if(metaDetector == 0) new TMinuit(6+8);
    else                  new TMinuit(6);
  }
  gMinuit->mncler();
  gMinuit->SetFCN(fcnMeta);
  gMinuit->SetObjectFit(this);
  Double_t arglist[6];
  
  fillArray();

  gMinuit->mnparm(0,"x",     0.,  1.,0,0,ierflg);
  gMinuit->mnparm(1,"y",     0.,  1.,0,0,ierflg);
  gMinuit->mnparm(2,"z",     0.,  1.,0,0,ierflg);
  gMinuit->mnparm(3,"alpha", 0., 0.1,0,0,ierflg);
  gMinuit->mnparm(4,"beta",  0., 0.1,0,0,ierflg);
  gMinuit->mnparm(5,"gamma", 0., 0.1,0,0,ierflg);
  if(metaDetector != 1) {         // TOF&RPC
    gMinuit->FixParameter(0);
    gMinuit->FixParameter(4);
    if(metaDetector == 0) {         // TOF
      if(fitTofModYPos) {
        for(Int_t ip=0;ip<8;ip++) {
          TString parName("Ymod");
          parName += ip;
          gMinuit->mnparm(6+ip,parName.Data(),0.,1.,0,0,ierflg);
        }
        for(Int_t ip=0;ip<8;ip++) gMinuit->FixParameter(6+ip);
        for(Int_t ip=0;ip<8;ip++) tofModYSh[ip] = 0.;
      }
    }
  }

    
//  gMinuit->SetPrintLevel(-1);
gMinuit->SetPrintLevel(1);
//!!!  gMinuit->SetErrorDef(1);
  arglist[0] = 2;
  gMinuit->mnexcm("SET STR",arglist,1,ierflg);
  arglist[1] = 0.001; //0.001;
//  arglist[0] = 5000; //20000;
  arglist[0] = 20000; //5000; //20000;
  for(Int_t i=0;i<5;i++) {
    calcMinDist();
    if(filterFlag) {
      if(metaDetector == 1) selectTracks(i==0 ? 2. : 1.);
      else                  calcXOffset(i==0 ? 2. : 1.);
    }
    setWeights();
    if(i==0) for(Int_t tr = 0; tr < nTracks; tr++) {
      tracks[tr].xMinDistInit = tracks[tr].xMinDist;
      tracks[tr].yMinDistInit = tracks[tr].yMinDist;
    }
    gMinuit->mnexcm("MINI",arglist,2,ierflg);
    if(i<2) continue;  // At least do track selection 2 times
    if(ierflg == 0) break;
  }
  
  calcMinDist();
  if(metaDetector == 0 && fitTofModYPos) { 
    printf(" ------ Fit Yshifts --------------\n");
    gMinuit->mnfree(0);
    for(Int_t ip=0;ip<6;ip++) gMinuit->FixParameter(ip);
    gMinuit->mnexcm("MINI",arglist,2,ierflg);
    
    selectTracks(1.);
    setWeights();
    
    gMinuit->mnfree(0);
    for(Int_t ip=0;ip<8;ip++) gMinuit->FixParameter(6+ip);
    gMinuit->FixParameter(0);
    gMinuit->FixParameter(4);
    gMinuit->mnexcm("MINI",arglist,2,ierflg);
    calcMinDist();
  }
 
  if(metaDetector != 1) {
    calcXOffset(1.0); //1.5);
    calcMinDist();
  }
  
  for(Int_t im=0;im<nMetaModules;im++) {
    transMetaModLabNew[im] = transMetaModSecNew[im];
    transMetaModLabNew[im].transFrom(secLabTrans);
  }
}

void HAlignmentMeta::calcMinDist(void) {
  Double_t out[6];
  Double_t err;
  for(Int_t ip=0;ip<6;ip++) gMinuit->GetParameter(ip,out[ip],err);
  if(metaDetector==0 && fitTofModYPos) 
      for(Int_t ip=0;ip<8;ip++) gMinuit->GetParameter(6+ip,tofModYSh[ip],err);
  HGeomTransform trans;
  HMdcSizesCells::setTransform(out,trans);
  calcMinDist(trans);
}

void HAlignmentMeta::calcMinDist(Double_t *par) {
  HGeomTransform trans;
  HMdcSizesCells::setTransform(par,trans);
  if(metaDetector == 0 && fitTofModYPos) for(Int_t ip=0;ip<8;ip++) tofModYSh[ip] = par[6+ip];
  calcMinDist(trans);
}

void HAlignmentMeta::fillArray(void) {
  nTracks = 0;
  for(Int_t c=0;c<nCellsTot;c++) {
    cellStat[c]  = 0;
    cellXCorr[c] = 0.;
  }
  Int_t nentries = nt->GetEntries();
  HGeomVector trPoint1pr,trPoint2pr;
  Int_t strInd = 0;    
  for(Int_t i = 0; i < nentries; i++) {
    nt->GetEntry(i);
    if(sec != alignSec) continue;
    if(TMath::IsNaN(xMetaLocal+yMetaLocal+zMetaLocal)) {
      printf("NaN!!! Entry %i:   xMetaLocal=%f  yMetaLocal=%f  zMetaLocal=%f\n",
      i,xMetaLocal,yMetaLocal,zMetaLocal);
      continue;
    }

    TrackMdcMeta &t = tracks[nTracks];
    t.mdcPnt1Sec.setXYZ(x1, y1, z1);      // mdc lab
    t.mdcPnt2Sec.setXYZ(x2, y2, z2);      // mdc lab
    t.metaMod = Short_t(metaModule + 0.1);
    t.column  = Short_t(metaColumn+0.1);
    t.cell    = Short_t(metaCell+0.1);
    if     (metaDetector == 0) t.cellInd = t.metaMod*nCells + t.cell;
    else if(metaDetector == 1) t.cellInd = t.cell;
    else if(metaDetector == 2) t.cellInd = t.column*nCells + t.cell;
    t.xMeta    = xMetaLocal;
    t.yMeta    = yMetaLocal;
    t.zMeta    = zMetaLocal;
    t.sigmaX   = xRMS; // shower/rpc local error
    t.sigmaY   = yRMS; // shower/rpc local error
    t.sigmaZ   = zRMS; // shower/rpc local error 
    t.useIt    = kTRUE;
    t.wt       = 1.;
    HGeomVector dv1(trPoint1pr-t.mdcPnt1Sec);
    HGeomVector dv2(trPoint2pr-t.mdcPnt2Sec);
    if(dv1.length()+dv2.length()<0.01) t.startTrInd = strInd;
    else {
      t.startTrInd = nTracks;
      strInd       = nTracks;
      trPoint1pr   = t.mdcPnt1Sec;
      trPoint2pr   = t.mdcPnt2Sec;
    }
    if(nTracks==0 || yMetaLocal<yMinMetaLocal) yMinMetaLocal = yMetaLocal;
    if(nTracks==0 || yMetaLocal>yMaxMetaLocal) yMaxMetaLocal = yMetaLocal;
    cellStat[t.cellInd]++;
    nTracks++;
    if(nTracks==1000000) return;
  }
  if(metaDetector > 0) { // RPC & Shower:
    Double_t binSize = (yMaxMetaLocal-yMinMetaLocal)/8.;
    for(Int_t tr = 0; tr < nTracks; tr++) {
      TrackMdcMeta &t = tracks[tr];
      t.binScWt = Short_t((t.yMeta-yMinMetaLocal)/binSize);
      if(t.binScWt<0)      t.binScWt = 0;
      else if(t.binScWt>7) t.binScWt = 7;
    }
  } else {  // TOF:
    for(Int_t tr = 0; tr < nTracks; tr++) {
      TrackMdcMeta &t = tracks[tr];
      t.binScWt = t.metaMod;
    }
  }
  
}

Double_t HAlignmentMeta::getMinFunction(Double_t *par) {
  calcMinDist(par);
  Double_t dist = 0;
  for(Int_t tr = 0; tr < nTracks; tr++) if(tracks[tr].useIt) {
    dist += tracks[tr].minDist2*tracks[tr].wt;
//dist += TMath::Sqrt(tracks[tr].minDist2)*tracks[tr].wt;
  }
  return dist;
}

void HAlignmentMeta::calcMinDist(HGeomTransform& trans) {
  for(Int_t im=0;im<nMetaModules;im++) {
    transMetaModSecNew[im] = transMetaModSecOld[im];
    transMetaModSecNew[im].transTo(trans);
    if(metaDetector == 2 && xShitfRpc != 0.) { // Take into account Xmodule shift (for RPC only!):
      HGeomTransform shifterTrans;
      HGeomVector tr(xShitfRpc,0.,0.);
      shifterTrans.setTransVector(tr);
      shifterTrans.transFrom(transMetaModSecNew[im]);
      transMetaModSecNew[im] = shifterTrans;      
    }
    if(metaDetector == 0 && fitTofModYPos && tofModYSh[im] != 0.) {
      HGeomTransform trans;
      HGeomVector tr(0.,tofModYSh[im],0.);
      trans.setTransVector(tr) ;
      trans.transFrom(transMetaModSecNew[im]);
      transMetaModSecNew[im] = trans;
    }
  } 

  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    HGeomVector point1(transMetaModSecNew[t.metaMod].transTo(t.mdcPnt1Sec)); 
    HGeomVector point2(transMetaModSecNew[t.metaMod].transTo(t.mdcPnt2Sec));
    point1.setZ(point1.getZ() - t.zMeta);
    point2.setZ(point2.getZ() - t.zMeta);
    Double_t diffZ  = point2.getZ() - point1.getZ();
    Double_t diffX  = point2.getX() - point1.getX();
    Double_t diffY  = point2.getY() - point1.getY();
    t.xMinDist  = (point1.getX()*diffZ - point1.getZ()*diffX)/diffZ - t.xMeta;
    t.yMinDist  = (point1.getY()*diffZ - point1.getZ()*diffY)/diffZ - t.yMeta;
    t.xMinDist -= cellXCorr[t.cellInd];
    
    Double_t dYNr   = t.dYNorm();
    t.minDist2      = dYNr*dYNr;        // => chi2
    if(metaDetector == 1) {
      Double_t dXNr = t.dXNorm();
      t.minDist2   += dXNr*dXNr;
    }
  }
}

void HAlignmentMeta::setWeights(void) {
  Double_t stat[8];
  for(Int_t i=0;i<8;i++) stat[i] = 0;
  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    t.wt = t.useIt ? 1.:0.;
    if(t.useIt && t.startTrInd != tr) {
      Int_t nTr = 1;
      Int_t str = t.startTrInd;
      for(Int_t p=str;p<tr;p++) if(tracks[p].useIt && t.oneLay(tracks[p])) nTr++;
      if(nTr == 1) continue;
      Double_t w = 1./Double_t(nTr);
      t.wt = w;
      for(Int_t p=str;p<tr;p++) if(tracks[p].useIt && t.oneLay(tracks[p])) tracks[p].wt = w;
    }
    stat[t.binScWt] += t.wt;
  }
  Double_t statMax = 0.;
  for(Int_t i=0;i<8;i++) if(statMax < stat[i]) statMax = stat[i];
  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    if(t.wt>0. && stat[t.binScWt]>100.) t.wt *= statMax/stat[t.binScWt];
  }
}

void HAlignmentMeta::selectTracks(Double_t nSigmasCut) {
  meanX        = 0.;
  meanY        = 0.;
  meanZ        = 0.;
  sigmX        = 0.;
  sigmY        = 0.;
  sigmZ        = 0.;
  isFirstSIter = kTRUE;
  while(selectTracksIter(nSigmasCut));
}

Bool_t HAlignmentMeta::selectTracksIter(Double_t nSigmasCut) {
  Double_t dXM        = 0.;
  Double_t dX2M       = 0.;
  Double_t dYM        = 0.;
  Double_t dY2M       = 0.;
  Int_t    nTr        = 0;
  Bool_t   doNextIter = kFALSE;
  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    Double_t dXm = t.dXNorm();
    Double_t dYm = t.dYNorm();
    if(isFirstSIter) { // First iteration.
      t.useIt    = kTRUE;
      doNextIter = kTRUE;
    } else if(TMath::Abs(dXm-meanX) <= sigmX*nSigmasCut && 
              TMath::Abs(dYm-meanY) <= sigmY*nSigmasCut)  {
      if(!t.useIt) doNextIter = kTRUE;
      t.useIt = kTRUE;
    } else {
      if(t.useIt) doNextIter = kTRUE;
      t.useIt = kFALSE;
      continue;
    }
    dXM  += dXm;
    dX2M += dXm*dXm;
    dYM  += dYm;
    dY2M += dYm*dYm;
    nTr++;
  }
  meanX  = dXM/nTr;
  meanY  = dYM/nTr;
  sigmX  = TMath::Max(TMath::Sqrt(dX2M/nTr - meanX*meanX),trackSelecCutX);
  sigmY  = TMath::Max(TMath::Sqrt(dY2M/nTr - meanY*meanY),trackSelecCutY);
  if(doNextIter) printf(" * From %i tracks %i selected. <X>=%f sX=%f <Y>=%f sY=%f\n",
                        nTracks,nTr,meanX,sigmX,meanY,sigmY);
  isFirstSIter = kFALSE;
  return doNextIter;
}

void HAlignmentMeta::checkAlignment(void) {
  TString fileName("align");
  if(metaDetector == 0)      fileName += "Tof_S";
  else if(metaDetector == 1) fileName += "Shower_S";
  else if(metaDetector == 2) fileName += "Rpc_S";
  fileName += alignSec;
  fileName += ".root";
  
  TFile   *f    = new TFile(fileName.Data(),"recreate");
  TNtuple *chnt = new TNtuple("chnt","chnt",
      "alignSec:wt:xMeta:yMeta:zMeta:dXold:dYold:dX:dY:sigX:sigY:chi2:mod:col:cell");
  Float_t data[15];
  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    data[ 0] = sec;
    data[ 1] = t.wt;
    data[ 2] = t.xMeta + cellXCorr[t.cellInd];
    data[ 3] = t.yMeta;
    data[ 4] = t.zMeta;
    data[ 5] = t.xMinDistInit;
    data[ 6] = t.yMinDistInit;
    data[ 7] = t.xMinDist;
    data[ 8] = t.yMinDist;
    data[ 9] = t.sigmaX;
    data[10] = t.sigmaY;
    data[11] = t.minDist2;
    data[12] = t.metaMod;
    data[13] = t.column;
    data[14] = t.cell;
    chnt->Fill(data);
  }
  f->cd();
  chnt->Write();
  f->Close();
  delete f;
}
        

void HAlignmentMeta::calcXOffset(Double_t nSigmasCut) {
  Int_t nCellsC = 0;
  xShitfRpc     = 0.;
  for(Short_t c=0;c<nCellsTot;c++) {
    meanX        = 0.;
    meanY        = 0.;
    meanZ        = 0.;
    sigmX        = 0.;
    sigmY        = 0.;
    sigmZ        = 0.;
    isFirstSIter = kTRUE;
    if(cellStat[c] > 100) {
      while(calcXOffset(nSigmasCut,c));
      nCellsC++;
      if(metaDetector==2 && calcCellXOffset) {  // Calculate Xmodule shift (for RPC only):
        xShitfRpc += cellXCorr[c];
        nCellsC++;
      }
    }
  }
  if(metaDetector==2 && calcCellXOffset ) {     // Calculate Xmodule shift (for RPC only):
    if(nCellsC>0) xShitfRpc /= nCellsC;
    printf("xShitf =%f nCells=%i !!!!!!!!!!!\n",xShitfRpc,nCellsC);
    for(Short_t c=0;c<nCellsTot;c++) if(cellStat[c] > 100) cellXCorr[c] -= xShitfRpc;
  }
}

Bool_t HAlignmentMeta::calcXOffset(Double_t nSigmasCut,Short_t cellInd) {
  Double_t dXM        = 0.;
  Double_t dX2M       = 0.;
  Double_t dYM        = 0.;
  Double_t dY2M       = 0.;
  Double_t xShift     = 0.;
  Int_t    nTr        = 0;
  Bool_t   doNextIter = kFALSE;
  Int_t    nTot       = 0;
  for(Int_t tr = 0; tr < nTracks; tr++) {
    TrackMdcMeta &t = tracks[tr];
    if(cellInd != t.cellInd) continue;
    nTot++;
    Double_t dXm = t.dXNorm();
    Double_t dYm = t.dYNorm();
    if(isFirstSIter) { // First iteration.
      t.useIt    = kTRUE;
      doNextIter = kTRUE;
    } else if(TMath::Abs(dXm-meanX) <= sigmX*nSigmasCut && 
              TMath::Abs(dYm-meanY) <= sigmY*nSigmasCut)  {
      if(!t.useIt) doNextIter = kTRUE;
      t.useIt = kTRUE;
    } else {
      if(t.useIt) doNextIter = kTRUE;
      t.useIt = kFALSE;
      continue;
    }
    dXM    += dXm;
    dX2M   += dXm*dXm;
    dYM    += dYm;
    dY2M   += dYm*dYm;
    xShift += t.xMinDist;
    nTr++;
  }
  meanX  = dXM/nTr;
  meanY  = dYM/nTr;
  sigmX  = TMath::Max(TMath::Sqrt(dX2M/nTr - meanX*meanX),trackSelecCutX);
  sigmY  = TMath::Max(TMath::Sqrt(dY2M/nTr - meanY*meanY),trackSelecCutY);
  isFirstSIter = kFALSE;
  if(!doNextIter) {
    cellXCorr[cellInd] += xShift/nTr;                           // !!!!!!!!!! += vmesto =
    printf(" * %icolumn %2icell: From %i tracks %i selected. xOffset=%f\n",
                        cellInd/nCells,cellInd%nCells,nTot,nTr,cellXCorr[cellInd]);
  }
  return doNextIter;
}
