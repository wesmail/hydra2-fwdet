using namespace std;
#include "hmdcefficiency.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>
#include "TMath.h"
#include "TLine.h"
#include "TText.h"
#include "TStyle.h"
#include "TSystem.h"
#include "TMarker.h"
#include "TPaveLabel.h"
#include "TColor.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TSlider.h"
#include "TSystem.h"
#include "hmdcgeomstruct.h"
#include "hmdcdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hmdcgetcontainers.h"
#include "hmdccal1sim.h"
#include "hmdclookupgeom.h"
#include "hmdctimecut.h"
#include "hcategory.h"
#include "hmdclookuptb.h"
#include "hmdcrawstruct.h"
#include "hmdcsizescells.h"
#include "hmdcclussim.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hshowerhittof.h"
#include "showerdef.h"
#include "tofdef.h"

//*-- Author : V.Pechenov
//*-- Modified : 15.06.2002 by V. Pechenov
//*-- Modified : 10.05.2001 by V. Pechenov
//*-- Modified : 10.05.2000 by V. Pechenov

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////
//  HMdcEfficiency
//
// Estimation of MDC efficiency.
// Use macros mdcEfficiency.C
//
///////////////////////////////////////////////////////////////


ClassImp(HMdcEff)
ClassImp(HMdcEffMod)
ClassImp(HMdcEfficiency)
ClassImp(HMdcEfficiencySec)
ClassImp(HMdcEfficiencyMod)
ClassImp(HMdcEfficiencyLayer)

TFile* HMdcEfficiency::fRootOut=0;

    // !!! mishen', dlya magn.polya budet ...
//

void HMdcEff::calcEff(Double_t iN0,  Double_t eN0,
                      Double_t iN1,  Double_t eN1,
                      Double_t iN2m, Double_t eN2m,
		      Double_t iN2b, Double_t eN2b) {
  n0       = iN0;
  n1       = iN1;
  n2m      = iN2m;
  n2b      = iN2b;
  errN0    = eN0;
  errN1    = eN1;
  errN2m   = eN2m;
  errN2b   = eN2b;
  totNumTr = n0+n1+n2m+n2b;

  calculation();
}

Double_t HMdcEff::efficiency(Double_t n0T, Double_t n1T,
                             Double_t n2mT, Double_t n2bT) {
  Double_t eC = n2mT+0.5*n2bT;
  Double_t eB = n1T+2.*eC;
  Double_t eA = n0T+n1T+eC;
  if( eA == 0. ) return 0.;
  Double_t sq = eB*eB-4*eA*eC;
  if( sq < 0. ) sq = 0.;
  return (eB+sqrt(sq))/(2.*eA);
}

Double_t HMdcEff::fractSingle(Double_t effT, Double_t n0T, Double_t n1T,
                             Double_t n2mT, Double_t n2bT) {
  Double_t b = n1T+2.*n2mT+n2bT;
  if( b == 0.0 ) return 0.;
  Double_t tmp = effT*b-n2mT;
  if( tmp < n2mT+n2bT ) return 0.;
  return 1.-(n2mT+n2bT)/tmp;
}

void HMdcEff::calculation(void) {

  // Determination of M coefficient
  eM   = 1.;
  errM = 0.;
  if(n2m+n2b>0) {
    eM = 1.-0.5*n2b/(n2m+n2b);
    Double_t dMdN2m = +0.5*n2b/((n2m+n2b)*(n2m+n2b));
    Double_t dMdN2b = -0.5*n2m/((n2m+n2b)*(n2m+n2b));
    errM = sqrt(dMdN2b*errN2b*dMdN2b*errN2b+dMdN2m*errN2m*dMdN2m*errN2m);
  }

  feff      = efficiency(n0,n1,n2m,n2b);
  single    = fractSingle(feff,n0,n1,n2m,n2b);
  p0        = single*(1.-feff)+(1.-single)*(1.-feff)*(1.-feff)*eM;
  p1        = single*feff+(1.-single)*feff*(1.-feff)*2.*eM;
  p2        = (1.-single)*feff*feff;
  errEff    = 0.;
  errSingle = 0.;
  errP0     = 0.;
  errP1     = 0.;
  errP2     = 0.;
  Double_t dEn[4],dSn[4],dP0[4],dP1[4],dP2[4];
  for(Int_t i=0; i<4; i++) dEn[i]=dSn[i]=dP0[i]=dP1[i]=dP2[i]=0.;
  if(n0+n1+n2m+n2b > 0) {
    for(Int_t i0=-1; i0<=1; i0++) {
      for(Int_t i1=-1; i1<=1; i1++) {
        for(Int_t i2m=-1; i2m<=1; i2m++) {
          for(Int_t i2b=-1; i2b<=1; i2b++) {
            Double_t shN0 = n0+errN0*i0;
            if(shN0<0.) continue;
            Double_t shN1 = n1+errN1*i1;
            if(shN1<0.) continue;
            Double_t shN2m = n2m+errN2m*i2m;
            if(shN2m<0.) continue;
            Double_t shN2b = n2b+errN2b*i2b;
            if(shN2b<0.) continue;
            //Maximal error:
            Double_t shEff = efficiency(shN0,shN1,shN2m,shN2b);
            errEff = TMath::Max(TMath::Abs(shEff-feff),errEff);
            Double_t shSingle = fractSingle(shEff,shN0,shN1,shN2m,shN2b);
            if(shN1+shN2m+shN2b > 0.) errSingle = TMath::Max(TMath::Abs(shSingle-single),errSingle);
            Double_t shM  = (shN2m+shN2b>0) ? 1.-0.5*shN2b/(shN2m+shN2b) : 1.;
            Double_t shP0 = shSingle*(1.-shEff)+(1.-shSingle)*(1.-shEff)*(1.-shEff)*shM;
            Double_t shP1 = shSingle*shEff+(1.-shSingle)*shEff*(1.-shEff)*2.*shM;
            Double_t shP2 = (1.-shSingle)*shEff*shEff;
            errP0 = TMath::Max(TMath::Abs(shP0-p0),errP0);
            errP1 = TMath::Max(TMath::Abs(shP1-p1),errP1);
            errP2 = TMath::Max(TMath::Abs(shP2-p2),errP2);

            Int_t n=abs(i0)+abs(i1)+abs(i2m)+abs(i2b);
            if(n==1) {
              Int_t i = abs(i0)*0+abs(i1)*1+abs(i2m)*2+abs(i2b)*3;
              dEn[i]  = TMath::Max((shEff-feff)*(shEff-feff),dEn[i]);
              dSn[i]  = TMath::Max((shSingle-single)*(shSingle-single),dSn[i]);
              dP0[i]  = TMath::Max((shP0-p0)*(shP0-p0),dP0[i]);
              dP1[i]  = TMath::Max((shP1-p1)*(shP1-p1),dP1[i]);
              dP2[i]  = TMath::Max((shP2-p2)*(shP2-p2),dP2[i]);
            }
          }
        }
      }
    }
  }

  Double_t errEffN    = sqrt(dEn[0]+dEn[1]+dEn[2]+dEn[3]);
  Double_t errSingleN = sqrt(dSn[0]+dSn[1]+dSn[2]+dSn[3]);
  Double_t errP0N     = sqrt(dP0[0]+dP0[1]+dP0[2]+dP0[3]);
  Double_t errP1N     = sqrt(dP1[0]+dP1[1]+dP1[2]+dP1[3]);
  Double_t errP2N     = sqrt(dP2[0]+dP2[1]+dP2[2]+dP2[3]);
  if(errEffN<errEff)       errEff    = (errEff+errEffN)*0.5;
  if(errSingleN<errSingle) errSingle = (errSingle+errSingleN)*0.5;
  if(errP0N<errP0) errP0 = (errP0+errP0N)*0.5;
  if(errP1N<errP1) errP1 = (errP1+errP1N)*0.5;
  if(errP2N<errP2) errP2 = (errP2+errP2N)*0.5;

/*  errEff=TMath::Min(errEff, sqrt(dEn[0]+dEn[1]+dEn[2]+dEn[3]));
  errSingle=TMath::Min(errSingle,sqrt(dSn[0]+dSn[1]+dSn[2]+dSn[3]));
  errP0=TMath::Min(errP0,sqrt(dP0[0]+dP0[1]+dP0[2]+dP0[3]));
  errP1=TMath::Min(errP1,sqrt(dP1[0]+dP1[1]+dP1[2]+dP1[3]));
  errP2=TMath::Min(errP2,sqrt(dP2[0]+dP2[1]+dP2[2]+dP2[3])); */
  if(n2b > 0.) totNumTr = totNumTr/(p0+p1+p2);
}

Double_t HMdcEffMod::efficiency(Double_t n0T, Double_t n1T,
                                Double_t n2T, Double_t n2bT) {
  Double_t eA = (n1T+n2T)*(n1T+n2T-4.*n0T);
  if( eA == 0. ) return 0.;
  Double_t eB = (2.*n2T+n1T)*(n1T+n2T-5.*n0T);
  Double_t eC = n2T*(n1T+n2T-5.*n0T);
  Double_t sq = eB*eB-4*eA*eC;
  if( sq < 0. ) sq=0.;
  return (eB+sqrt(sq))/(2.*eA);
}

void HMdcEffMod::calcEff(Double_t iN0,  Double_t eN0,
			 Double_t iN1,  Double_t eN1,
 			 Double_t iN2m, Double_t eN2m,
 			 Double_t iN2b, Double_t eN2b) {
  n0     = iN0;
  n1     = iN1;
  n2m    = iN2m;
  n2b    = iN2b;
  errN0  = eN0;
  errN1  = eN1;
  errN2m = eN2m;
  errN2b = eN2b;

  totNumTr = (n0+n1+n2m)/6.;

  calculation();

  Double_t p12 = p1+p2;
  p6l = p12 *p12 *p12 *p12 *p12 *p12;
  p5l = p12 *p12 *p12 *p12 *p12 *p0   *6.;
  p4l = p12 *p12 *p12 *p12 *p0  *p0  *15.;
  p3l = p12 *p12 *p12 *p0  *p0  *p0  *20.;
  p2l = p0  *p0  *p0  *p0  *p12 *p12 *15.;
  p1l = p0  *p0  *p0  *p0  *p0  *p12  *6.;
  p0l = p0  *p0  *p0  *p0  *p0  *p0;
}

void HMdcEfficiencyLayer::init(Short_t sec, Short_t mod, Short_t lay) {
  // init HMdcEfficiencyLayer object
  sector = sec;
  module = mod;
  layer  = lay;
  is     = 0;
  // Geting of pointers to par. cont.
  HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
  if( !fGetCont ) return;
  HMdcGeomStruct* fGeomStr=fGetCont->getMdcGeomStruct();
  if ( fGeomStr ) {
    HMdcGetContainers::getFileName(fileName);
    nCells=((*fGeomStr)[sector][module])[layer];
    if( nCells > 0 ) {
      is         = 1;
      n0         = new Int_t [nCells];
      n1         = new Int_t [nCells];
      n2         = new Int_t [nCells];
      nCellTot   = new Int_t [nCells];
      nCellCutS  = new Int_t [nCells];
      nCellStat  = new Int_t [nCells];
      nCellsClus = new Int_t [nCells];
      nClustCell = new UChar_t [nCells];
      clear();
      createHists();
      for(Int_t nP=0; nP<6; nP++) listPads[nP]=1;
      nPads=6;
      Char_t buf[200];
      sprintf(buf,"%s_S%iM%iL%iEff",fileName.Data(),sector+1,module+1,layer+1);
      nameCanv=buf;
      sprintf(buf,"%s S%i M%i L%i Efficiency",fileName.Data(),sector+1,module+1,layer+1);
      titleCanv=buf;
      for(Int_t nPlXY=0; nPlXY<3; nPlXY++) {
        titlePadXY[nPlXY].SetX1(0.05);
        titlePadXY[nPlXY].SetX2(0.6);
        titlePadXY[nPlXY].SetY1(0.955);
        titlePadXY[nPlXY].SetY2(0.995);
        titlePadXY[nPlXY].SetTextAlign(22);
        titlePadXY[nPlXY].SetTextSize(0);
        titlePadXY[nPlXY].SetBorderSize(1);
        titlePadXY[nPlXY].SetFillColor(7);
      }
    }
  }
}

HMdcEfficiencyLayer::~HMdcEfficiencyLayer() {
  delete [] n0;
  delete [] n1;
  delete [] n2;
  delete [] nCellTot;
  delete [] nCellCutS;
  delete [] nCellStat;
  delete [] nClustCell;
  delete [] nCellsClus;
  TSeqCollection* fSegColl = gROOT->GetListOfCanvases();
  canvEff = (TCanvas*)fSegColl->FindObject(nameCanv);
  if( canvEff ) {
    delete canvEff;
    canvEff = NULL;
  }
  for(Int_t i=0; i<3; i++) {
    canvXY[i]=(TCanvas*)fSegColl->FindObject(nameCanvXY[i]);
    if( canvXY[i] ) {
      delete canvXY[i];
      canvXY[i] = NULL;
    }
  }
}

void HMdcEfficiencyLayer::clear(void) {
  for (Short_t i=0; i<nCells; i++) {
    n0[i]=n1[i]=n2[i]=0;
    nCellStat[i]=nCellCutS[i]=nCellTot[i]=nCellsClus[i]=0;
  }
  for (Short_t xy=0; xy<256; xy++) n0xy[xy]=n1xy[xy]=n2xy[xy]=0;
}

void HMdcEfficiencyLayer::createHists(void) {
    const Char_t* titH[14] = {
	"Pattern",
	"Total num. of hits (no cut)",
	"Num. of selected tracks crossing cell(s) vs. cell",
	"Num. of \"zero\" hits",
	"Num. of \"single\" hits",
	"Num. of \"double\" hits",
	"Cells efficiency",
	"Fraction of single hits",
	"Probability to have \"zero\" hit",
	"Probability to have \"single\" hit",
	"Probability to have \"double\" hit",
	"Total num. of hits (cut time1&time2)",
	"Hits from clusters",
        "Ratio" };

    const Char_t* nmH[14]={
	"Pattern",
	"TNhitsNoCut",
	"TNtracks",
	"N0hits",
	"N1hits",
	"N2hits",
	"Eff",
	"Sing",
	"P0",
	"P1",
	"P2",
	"TNhitsCutT1T2",
	"TNhitsClust",
        "Ratio" };

  Float_t minimum[14]   = {  0.,  0., 0., 0., 0., 0., .0,  .0, .0, .0, .0,  0.,  0., 0.};
  Float_t maximim[14]   = { -1., -1.,-1.,-1.,-1.,-1.,1.1, 1.1,1.1,1.1,1.1, -1., -1.,1.1};
  Int_t   markStyle[14] = {  -1,  -1, -1, -1, -1, -1, 21,  21, 21, 21, 21,  -1,  -1, 21};
  Float_t markSize[14]  = { 0.4, 0.4,0.4,0.4,0.4,0.4,0.4, 0.4,0.4,0.4,0.4, 0.4, 0.4,0.4};
  Int_t   markColor[14] = {  -1,  -1, -1, -1, -1, -1,  4,   4,  2,  4,  3,  -1,  -1,  3};
  Int_t   fillStyle[14] = {1001,1001,  0,  0,  0,  0,  0,   0,  0,  0,  0,1001,   0,  0};
  Char_t  name[20];
  Char_t  title[100];

  gROOT->cd();
  for(Int_t nHist=0; nHist<14; nHist++) {
    sprintf(name,"S%iM%iL%i%s",sector+1,module+1,layer+1,nmH[nHist]);
    sprintf(title,"S%i M%i L%i %s",sector+1,module+1,layer+1,titH[nHist]);
    hists[nHist]=new TH1F(name,title,nCells,0.,(Double_t)nCells);
    hists[nHist]->SetMinimum(minimum[nHist]);
    if(maximim[nHist]>0.) hists[nHist]->SetMaximum(maximim[nHist]);
    if(markStyle[nHist]>0) {
      hists[nHist]->SetMarkerStyle(markStyle[nHist]);
      hists[nHist]->SetMarkerSize(markSize[nHist]);
      hists[nHist]->SetMarkerColor(markColor[nHist]);
    }
    if(nHist>4&&nHist<11) hists[nHist]->SetOption("e1p");
    hists[nHist]->SetFillStyle(fillStyle[nHist]);
  }
  Char_t buf[200];
  sprintf(buf,"%s_S%iM%iL%inHitsXY",fileName.Data(),sector+1,module+1,layer+1);
  nameCanvXY[0]=buf;
  sprintf(buf,"%s_S%iM%iL%iEffXY",fileName.Data(),sector+1,module+1,layer+1);
  nameCanvXY[1]=buf;
  sprintf(buf,"%s_S%iM%iL%iSinXY",fileName.Data(),sector+1,module+1,layer+1);
  nameCanvXY[2]=buf;
  sprintf(buf,"%s S%i M%i L%i Num. of hits",fileName.Data(),sector+1,module+1,layer+1);
  titleCanvXY[0]=buf;
  sprintf(buf,"%s S%i M%i L%i Efficiency",fileName.Data(),sector+1,module+1,layer+1);
  titleCanvXY[1]=buf;
  sprintf(buf,"%s S%i M%i L%i Fraction of single hits vs XY",
                               fileName.Data(),sector+1,module+1,layer+1);
  titleCanvXY[2]=buf;
  setBiningXY(1);
}

void HMdcEfficiencyLayer::addCellInClust(Short_t cell) {
  if(cell>=0 && cell<nCells) nClustCell[cell]++;
}

void HMdcEfficiencyLayer::addCellTot(Short_t cell) {
  if(cell>=0 && cell<nCells) nCellTot[cell]++;
}

void HMdcEfficiencyLayer::addCellCutS(Short_t cell) {
  if(cell>=0 && cell<nCells) nCellCutS[cell]++;
}

void HMdcEfficiencyLayer::addCellClus(Short_t cell) {
  if(cell>=0 && cell<nCells) nCellsClus[cell]++;
}

void HMdcEfficiencyLayer::addCell(Short_t cell) {
  if(cell>=0 && cell<nCells) {
    nClustCell[cell]++;
    nCellStat[cell]++;
  }
}

Int_t HMdcEfficiencyLayer::getCellStat(Short_t cell) {
  if(cell>=0 && cell<nCells) return nCellStat[cell];
  return 0;
}

void HMdcEfficiencyLayer::add0Hit(Short_t cell, Int_t nB) {
  if(cell>=0 && cell<nCells) n0[cell]++;
  if(nB>=0) n0xy[nB]++;
}

void HMdcEfficiencyLayer::add1Hit(Short_t cell, Int_t nB) {
  if(cell>=0 && cell<nCells) n1[cell]++;
  if(nB>=0) n1xy[nB]++;
}

void HMdcEfficiencyLayer::add2Hit(Short_t cell1, Short_t cell2, Int_t nB) {
  if(cell1>=0 && cell1<nCells && cell2>=0 && cell2<nCells) {
   n2[(cell1>cell2) ? cell1 : cell2]++;
  }
  if(nB>=0) n2xy[nB]++;
}

void HMdcEfficiencyLayer::getEffCells(HMdcEff* efficiency,Short_t fCell, Short_t lCell) {
  // Calculating of avarage value of efficiency (eff +/- errEff)
  // and fraction of singles hits (singles +/- errSingles)
  // for cells  fCell, fCell+1, ... lCell (for lCell-fCell+1 cells).
  //
  // getEfficiency(fCell) -
  // - for cell number fCell
  //
  // getEfficiency() -
  // - for all cells in layers.


  // Finding of working wires region
  Short_t fCellN=0;
  Short_t lCellN=nCells-1;
  for(Short_t cell=0; cell<nCells; cell++) {
    if(nCellStat[cell]>0) {
      if(nCellStat[cell]>10) fCellN=cell;
      break;
    }
  }
  for(Short_t cell=lCellN; cell>=0; cell--) {
    if(nCellStat[cell]>0) {
      if(nCellStat[cell]>10) lCellN=cell;
      break;
    }
  }

  Short_t fC=(fCell<fCellN) ? fCellN : fCell;
  Short_t lC=(lCell>lCellN) ? lCellN : lCell;
  if(fCell!=-1000 && lCell == 1000) lC=fC=fCell;
  Int_t nHits,nHitsNoCut,nHitsCutS,nHitsClus,nC0,nC1,nC2b,nC2m;
  nHits=nHitsNoCut=nHitsCutS=nHitsClus=nC0=nC1=nC2b=nC2m=0;
  Double_t eN0,eN1,eN2m,eN2b;
  eN0=eN1=eN2m=eN2b=0.;
  if(fC>=fCellN && lC<=lCellN) {
    for (Int_t cell=fC; cell<=lC; cell++) {
      nHits      += nCellStat[cell];
      nHitsNoCut += nCellTot[cell];
      nHitsCutS  += nCellCutS[cell];
      nHitsClus  += nCellsClus[cell];
      nC0+=n0[cell];
      nC1+=n1[cell];
      if(cell>fC) nC2m+=n2[cell];
    }
    if(fC>fCellN) nC2b+=n2[fC];
    if(lC<lCellN) nC2b+=n2[lC+1];
    eN0=(nC0>0) ? sqrt((Double_t)nC0) : 1.;
    eN1=(nC1>0) ? sqrt((Double_t)nC1) : 1.;
    if(fC != lC) eN2m=(nC2m>0) ? sqrt((Double_t)nC2m) : 1.;
    if(fC>fCellN || lC<lCellN) eN2b=(nC2b>0) ? sqrt((Double_t)nC2b) : 1.;
  }

  efficiency->setTotNhits((Float_t)nHits);
  efficiency->setTotNhitsCutS((Float_t)nHitsCutS);
  efficiency->setTotNhitsNoCut((Float_t)nHitsNoCut);
  efficiency->setTotNhitsClus((Float_t)nHitsClus);
  efficiency->calcEff((Double_t)nC0, eN0,  (Double_t)nC1,  eN1,
 		      (Double_t)nC2m,eN2m, (Double_t)nC2b,eN2b);
}

void HMdcEfficiencyLayer::getEffXY(HMdcEff* efficiency) {
  // Calculation of efficiency of MDC layer.
  getEffXY(efficiency,0,15,0,15);
}

void HMdcEfficiencyLayer::getEffXY(HMdcEff* efficiency,
                              Short_t xBin, Short_t yBin){
  // Calculation of efficiency of MDC for bin (xBin:yBin).
  getEffXY(efficiency,xBin,xBin,yBin,yBin);
}

void HMdcEfficiencyLayer::getEffXY(HMdcEff* efficiency,
                                   Short_t fXbin, Short_t lXbin,
                                   Short_t fYbin, Short_t lYbin){
  // Calculation of efficiency of MDC ragion
  // (MDC is divided on 16x16 bins).
  // Region: fXbin <= x <= lXbin, fYbin <= y <= lYbin
  Int_t    nC0 = 0;
  Int_t    nC1 = 0;
  Int_t    nC2 = 0;
  Double_t eN0 = 0.;
  Double_t eN1 = 0.;
  Double_t eN2 = 0.;

  Int_t fXb = (fXbin < 0 ) ?  0 : fXbin;
  Int_t lXb = (lXbin > 15) ? 15 : lXbin;
  Int_t fYb = (fYbin < 0 ) ?  0 : fYbin;
  Int_t lYb = (lYbin > 15) ? 15 : lYbin;
  for(Int_t y=fYb; y<=lYb; y++) {
    Int_t nBin=y*16;
    for(Int_t x=fXb; x<=lXb; x++){
      nC0 += n0xy[nBin+x];
      nC1 += n1xy[nBin+x];
      nC2 += n2xy[nBin+x];
    }
  }
  if(nC0+nC1+nC2 > 0) {
    eN0 = nC0>0 ? sqrt((Double_t)nC0) : 1.;
    eN1 = nC1>0 ? sqrt((Double_t)nC1) : 1.;
    eN2 = nC2>0 ? sqrt((Double_t)nC2) : 1.;
  }
  efficiency->calcEff((Double_t)nC0,eN0, (Double_t)nC1,eN1, (Double_t)nC2,eN2);
}

void HMdcEfficiencyLayer::fillHists(void) {
  Float_t max0     = 1.;
  Float_t max1     = 0.;
  Float_t maxClust = 0.;
  Float_t maxClDr  = 0.;
  for (Int_t bin=0; bin<nCells; bin++) {
    getEffCells(&eff, bin, bin);
    hists[ 0]->SetBinContent(bin+1,eff.getTotNhits());
    hists[ 1]->SetBinContent(bin+1,eff.getTotNhitsNoCut());
    hists[11]->SetBinContent(bin+1,eff.getTotNhitsCutS());
    hists[12]->SetBinContent(bin+1,eff.getTotNhitsClus());
    hists[ 2]->SetBinContent(bin+1,eff.getTotNumTracks());
    hists[ 3]->SetBinContent(bin+1,eff.getN0());
    hists[ 4]->SetBinContent(bin+1,eff.getN1());
    hists[ 5]->SetBinContent(bin+1,eff.getN2());
    hists[ 6]->SetBinContent(bin+1,eff.getEfficiency());
    hists[ 6]->SetBinError(bin+1,eff.getErrEfficiency());
    hists[ 7]->SetBinContent(bin+1,eff.getFracSingle());
    hists[ 7]->SetBinError(bin+1,eff.getErrFracSingle());
    hists[ 8]->SetBinContent(bin+1,eff.getP0());
    hists[ 8]->SetBinError(bin+1,eff.getErrP0());
    hists[ 9]->SetBinContent(bin+1,eff.getP1());
    hists[ 9]->SetBinError(bin+1,eff.getErrP1());
    hists[10]->SetBinContent(bin+1,eff.getP2());
    hists[10]->SetBinError(bin+1,eff.getErrP2());
    if(eff.getTotNhits()>0. && eff.getTotNhitsClus()>0.) {
      Float_t tot      = eff.getTotNhits();
      Float_t clst     = eff.getTotNhitsClus();
      Float_t ratio    = clst/tot;
      Float_t errRatio = TMath::Sqrt(1./tot + 1./clst)*ratio;
      hists[13]->SetBinContent(bin+1,ratio);
      hists[13]->SetBinError(bin+1,errRatio);
    }
    if(eff.getTotNhitsClus()>maxClust) {
      maxClust = eff.getTotNhitsClus();
      maxClDr  = eff.getTotNhits();
      if(maxClDr>maxClust*3.) maxClDr = maxClust*3.;
    }
    if(eff.getTotNhits()>max0) {
      Float_t dn=(eff.getTotNhitsNoCut()-eff.getTotNhits())/eff.getTotNhits();
      if(dn<0.2)                   max0 = eff.getTotNhitsCutS(); // max after cut "common stop"
    }
    if(eff.getTotNumTracks()>max1) max1 = eff.getTotNumTracks();
  }
  if(maxClust>50.) max0 = maxClDr;
  if(max0>5.) {
    hists[1]->GetYaxis()->SetRangeUser(0.,max0+0.1*max0);
    hists[0]->GetYaxis()->SetRangeUser(0.,max0+0.1*max0);
    for(Int_t n=2; n<6; n++) hists[n]->GetYaxis()->SetRangeUser(0.,max1+0.1*max1);
    hists[11]->GetYaxis()->SetRangeUser(0.,max0+0.1*max0);
    hists[12]->GetYaxis()->SetRangeUser(0.,max0+0.1*max0);
  }
  getEffCells(&eff);
}

void HMdcEfficiencyLayer::setTitle(TH1F *hist,const Char_t* nm){
  Char_t title[50];
  sprintf(title,"%s",nm);
  hist->SetYTitle(title);
  hist->GetYaxis()->CenterTitle(1);
  hist->SetTitleOffset(0.6,"Y");
  hist->GetXaxis()->SetLabelSize(0.07);
  hist->GetYaxis()->SetLabelSize(0.07);
  hist->GetYaxis()->SetTitleSize(0.06);
}


void HMdcEfficiencyLayer::drawTotNhits(void) {
  hists[0]->SetFillColor(3);
  if(TMath::Abs(hists[0]->GetSumOfWeights()-hists[1]->GetSumOfWeights())>0.5) {
    Char_t title[90];
    Float_t h0      = hists[ 0]->GetSumOfWeights();
    Float_t h1      = hists[ 1]->GetSumOfWeights();
    Float_t h11     = hists[11]->GetSumOfWeights();
    Float_t prCut   = (h1-h0)/h1*100.;
    Float_t prCutS  = (h1-h11)/h1*100.;
    Float_t prCutTD = (h11-h0)/h1*100.;
    sprintf(title,"S%i M%i L%i: Nhits (Com.stop cut(%6.2f%c)+times diff. cut(%6.2f%c)=%6.2f%c)",
        sector+1,module+1,layer+1,prCutS,'%',prCutTD,'%',prCut,'%');
    setTitle(hists[1],"Nhits");
    setLayerPadParam(1);
    hists[ 1]->SetTitle(title);
    hists[ 1]->SetFillColor(4);
    hists[ 1]->Draw();
    hists[11]->SetFillColor(5);
    hists[11]->Draw("same");
    hists[ 0]->Draw("same");
    hists[12]->SetLineColor(6);
    hists[12]->Draw("same");
 } else {
    setTitle(hists[0],"Nhits");
    setLayerPadParam(0);
    hists[ 0]->Draw();
    hists[12]->SetLineColor(6);
    hists[12]->Draw("same");  
  }
}

void HMdcEfficiencyLayer::drawRatio(void) {
  Char_t title[90];
  sprintf(title,"S%i M%i L%i: Ratio Nclust.wires/Nwires",sector+1,module+1,layer+1);
  setTitle(hists[13],"Ratio");
  setLayerPadParam(13);
  hists[13]->SetTitle(title);
  hists[13]->Draw();
}

void HMdcEfficiencyLayer::drawNHits(void) {
  setTitle(hists[2],"N0,N1,N2,Ntr");
  Char_t title[80];
  sprintf(title,"S%i M%i L%i:  N0[r]=%.0f  N1[b]=%.0f N2[g]=%.0f Ntr[black]=%.0f",
      sector+1,module+1,layer+1,eff.getN0(),eff.getN1(),eff.getN2(),eff.getTotNumTracks());
  setLayerPadParam(2);
  hists[2]->SetTitle(title);
  hists[2]->Draw();
  hists[3]->SetLineColor(2);
  hists[3]->Draw("same");
  hists[4]->SetLineColor(4);
  hists[4]->Draw("same");
  hists[5]->SetLineColor(3);
  hists[5]->Draw("same");
}

void HMdcEfficiencyLayer::drawHistEff(void) {
  // Drawing of the efficiency.
  setTitle(hists[6],"E");
  Char_t title[60];
  sprintf(title,"S%i M%i L%i:  E=%.3f+/-%.4f",sector+1,module+1,layer+1,
                       eff.getEfficiency(), eff.getErrEfficiency());
  setLayerPadParam(6);
  hists[6]->SetTitle(title);
  hists[6]->Draw();
  drawLineTot(hists[6],2, eff.getEfficiency(), eff.getErrEfficiency());
}

void HMdcEfficiencyLayer::drawHistSing(void) {
  // Drawing of the fraction of single hits.
  setTitle(hists[7],"S");
  Char_t title[60];
  sprintf(title,"S%i M%i L%i:  S=%.3f+/-%.4f",sector+1,module+1,layer+1,
                       eff.getFracSingle(), eff.getErrFracSingle());
  setLayerPadParam(7);
  hists[7]->SetTitle(title);
  hists[7]->Draw();
  drawLineTot(hists[7], 2, (Float_t)eff.getFracSingle(),(Float_t)eff.getErrFracSingle());
}

void HMdcEfficiencyLayer::drawProbabilitis(void) {
  // Drawing of the probabilities.
  setTitle(hists[8],"P0,P1,P2");
  Char_t title[80];
  sprintf(title,
      "S%i M%i L%i:  P0=%.3f+/-%.4f(r) P1=%.3f+/-%.4f(b) P2=%.3f+/-%.4f(g)",
      sector+1,module+1,layer+1,eff.getP0(),eff.getErrP0(),
      eff.getP1(),eff.getErrP1(),eff.getP2(),eff.getErrP2());
  setLayerPadParam(8);
  hists[8]->SetTitle(title);
  hists[8]->Draw();
  drawLineTot(hists[8], 2, (Float_t)eff.getP0(), (Float_t)eff.getErrP0());
  hists[9]->Draw("same");
  drawLineTot(hists[9], 4, (Float_t)eff.getP1(), (Float_t)eff.getErrP1());
  hists[10]->Draw("same");
  drawLineTot(hists[10], 3, (Float_t)eff.getP2(), (Float_t)eff.getErrP2());
}

void HMdcEfficiencyLayer::setLayerPadParam(Int_t hs) {
  if((hs>=6 && hs<=10) || hs==13) {
    gPad->SetGridx();
    gPad->SetGridy();
    hists[hs]->GetYaxis()->SetTitleOffset(0.27);
    gPad->SetLeftMargin(0.054);
  } else {
    gPad->SetLeftMargin(0.08);
    hists[hs]->GetYaxis()->SetTitleOffset(0.45);
  }
  gPad->SetRightMargin(0.029);
  hists[hs]->GetXaxis()->SetLabelSize(0.09);
  hists[hs]->GetYaxis()->CenterTitle(true);
  hists[hs]->GetYaxis()->SetLabelSize(0.09);
  hists[hs]->GetYaxis()->SetTitleSize(0.09);
}

void HMdcEfficiencyLayer::drawLineTot(TH1F* hst, Int_t color, Float_t effL,
                                                              Float_t errEff){
  Float_t xf = hst->GetXaxis()->GetBinLowEdge(hst->GetXaxis()->GetFirst());
  Float_t xl = hst->GetXaxis()->GetBinUpEdge(hst->GetXaxis()->GetLast());
  TLine lnEffLay;
  lnEffLay.SetLineStyle(1);
  lnEffLay.SetLineColor(color);
  lnEffLay.DrawLine(xf,effL,xl,effL);
  lnEffLay.SetLineStyle(2);
  lnEffLay.DrawLine(xf, effL+errEff, xl, effL+errEff);
  lnEffLay.DrawLine(xf, effL-errEff, xl, effL-errEff);
}

void HMdcEfficiencyLayer::setListPads(Int_t* listPad) {
  nPads = 0;
  for(Int_t pad=0; pad<6; pad++) {
    if(listPad[pad]) {
      nPads++;
      listPads[pad] = 1;
    }
    else listPads[pad] = 0;
  }

  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  TCanvas* canv=(TCanvas*)fSegColl->FindObject(nameCanv);
  if(canv) {
    if(nPads>0) {
      Int_t nPd=0;
      if(nPads != (nPd=canv->GetListOfPrimitives()->GetSize())) {
       canv->Clear();
       if(nPd<3||nPads<3) canv->SetWindowSize(700,(nPads>3) ? 900 : nPads*300);
       setPads();
      }
      drawCanvas();
    }
    else canv->Clear();
  }
  for(Int_t nPl=0; nPl<3; nPl++) {
    if( listPad[nPl+1] == 1 ) continue;
    TCanvas* canv=(TCanvas*)fSegColl->FindObject(nameCanvXY[nPl]);
    if( canv ) delete canv;
    for(Int_t i=0; i<256; i++) indDrawPv[nPl][i]=1;
    indDrawTitXY[nPl]=1;
  }
}

void HMdcEfficiencyLayer::creatCanvas(void) {
  if(gROOT->GetListOfCanvases()->FindObject(nameCanv)) return;
  if(nPads==0) return;
  canvEff=new TCanvas(nameCanv,titleCanv,1,1,700,(nPads>3) ? 900 : nPads*300);
  setPads();
}

void HMdcEfficiencyLayer::setPads(void) {
  canvEff->Divide(1,nPads,0.0001,0.0001);
  for(Int_t nPad=0; nPad<nPads; nPad++) {
    canvEff->cd(nPad+1);
    TPad* pad = (TPad*)gPad;
    pad->SetRightMargin(0.03);
    pad->SetLeftMargin(0.08);
    pad->SetTopMargin(0.1);
    pad->SetBottomMargin(0.1);
  }
}

void HMdcEfficiencyLayer::updateCanv(void) {
  canvEff=(TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanv);
  if(!canvEff) return;
  drawCanvas();
}

void HMdcEfficiencyLayer::drawCanvas(void) {
  creatCanvas();
  Int_t nPad=1;
  for(Short_t pad=0; pad<6; pad++) {
    if(!listPads[pad]) continue;
    canvEff->cd(nPad++);
    if(pad==0)      drawTotNhits();
    else if(pad==1) drawNHits();
    else if(pad==2) drawHistEff();
    else if(pad==3) drawHistSing();
    else if(pad==4) drawProbabilitis();
    else if(pad==5) drawRatio();
  }
  canvEff->cd();
  canvEff->Modified();
  canvEff->Update();
}

void HMdcEfficiencyLayer::delCanvas(void){
  canvEff=(TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanv);
  if(canvEff) {
    delete canvEff;
    canvEff=0;
  }
}

void HMdcEfficiencyLayer::setPavesXY(void) {
  Int_t   nPavesSh = 16/nBinsSumXY;
  Float_t bordX    = 0.05;
  Float_t bordY    = 0.05;
  Float_t dX       = (1.-bordX*2)/nPavesSh;
  Float_t dY       = (1.-bordY*2)/nPavesSh;
  for(Int_t yp=0; yp<16; yp++) {
    Float_t y1 = bordY+yp*dY;
    Float_t y2 = bordY+(yp+1)*dY;
    for(Int_t xp=0; xp<16; xp++) {
      for(Int_t i=0; i<3; i++) {
        pvTxEffXY[i][xp+yp*16].SetX1(bordX+xp*dX);
        pvTxEffXY[i][xp+yp*16].SetX2(bordX+(xp+1)*dX);
        pvTxEffXY[i][xp+yp*16].SetY1(y1);
        pvTxEffXY[i][xp+yp*16].SetY2(y2);
        pvTxEffXY[i][xp+yp*16].SetX1NDC(bordX+xp*dX);
        pvTxEffXY[i][xp+yp*16].SetX2NDC(bordX+(xp+1)*dX);
        pvTxEffXY[i][xp+yp*16].SetY1NDC(y1);
        pvTxEffXY[i][xp+yp*16].SetY2NDC(y2);
        pvTxEffXY[i][xp+yp*16].SetTextAlign(22);
        pvTxEffXY[i][xp+yp*16].SetTextSize(0);
        pvTxEffXY[i][xp+yp*16].SetBorderSize(1);
      }
    }
  }
}

void HMdcEfficiencyLayer::setBiningXY(Int_t nBinsSum) {
  Int_t nbnSm=8;
  if( nBinsSum <= 1 ) nbnSm=1;
  else if( nBinsSum < 4 ) nbnSm=2;
  else if( nBinsSum < 8 ) nbnSm=4;
  if(nbnSm == nBinsSumXY) return;
  nBinsSumXY=nbnSm;
  for(Int_t nPl=0; nPl<3; nPl++) {
    canvXY[nPl]=(TCanvas*)gROOT->GetListOfCanvases()->
        FindObject(nameCanvXY[nPl]);
    if( canvXY[nPl] ) {
      canvXY[nPl]->Clear();
      for(Int_t i=0; i<256; i++) {
        pvTxEffXY[nPl][i].Clear();
        indDrawPv[nPl][i]=1;
      }
      indDrawTitXY[nPl]=1;
    }
  }
  setPavesXY();
  drawCanvXY(1);
}

void HMdcEfficiencyLayer::drawCanvXYlist(void) {
  drawCanvXY(0);
}

void HMdcEfficiencyLayer::updateCanvXYlist(void) {
  drawCanvXY(1);
}

void HMdcEfficiencyLayer::delCanvXYlist(void) {
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  for(Int_t i=0; i<3; i++) {
    if( !listPads[i+1] ) continue;
    canvXY[i]=(TCanvas*)fSegColl->FindObject(nameCanvXY[i]);
    if(canvXY[i]) {
      delete canvXY[i];
      canvXY[i]=0;
    }
  }
}

void HMdcEfficiencyLayer::fillPaveXYnHits(Int_t nPv) {
  Float_t edgeCol[10]={.2,.4,.5,.7,.79,.85,.89,.92,.94,.951};
  Char_t buf[20];
  Int_t color=0;
  sprintf(buf,"N0:%.0f",effXY.getN0());
  pvTxEffXY[0][nPv].InsertText(buf);
  sprintf(buf,"N1:%.0f",effXY.getN1());
  pvTxEffXY[0][nPv].InsertText(buf);
  sprintf(buf,"N2:%.0f",effXY.getN2());
  pvTxEffXY[0][nPv].InsertText(buf);
  Float_t effL=effXY.getEfficiency();
  if(effL >= 0.95) {
    color=Int_t((effL-0.95)/0.005)+10;
    if(color > 19) color=19;
  }
  else {
    color=0;
    while(effL > edgeCol[color]) color++;
  }
  pvTxEffXY[0][nPv].SetFillColor(color+31);
}

void HMdcEfficiencyLayer::fillPaveXYeff(Int_t nPv) {
  Float_t edgeCol[10]={.2,.4,.5,.7,.79,.85,.89,.92,.94,.951};
  Char_t buf[20];
  Int_t color=0;
  Float_t effL=effXY.getEfficiency();
  sprintf(buf,"%5.3f",effL);
  pvTxEffXY[1][nPv].InsertText(buf);
  sprintf(buf,"+-%5.3f",effXY.getErrEfficiency());
  pvTxEffXY[1][nPv].InsertText(buf);
  if(effL >= 0.95) {
    color=Int_t((effL-0.95)/0.005)+10;
    if(color > 19) color=19;
  }
  else {
    color=0;
    while(effL > edgeCol[color]) color++;
  }
  pvTxEffXY[1][nPv].SetFillColor(color+31);
}

void HMdcEfficiencyLayer::fillPaveXYsing(Int_t nPv) {
  Char_t buf[20];
  Float_t frS=effXY.getFracSingle();
  sprintf(buf,"%5.3f",frS);
  pvTxEffXY[2][nPv].InsertText(buf);
  sprintf(buf,"+-%5.3f",effXY.getErrFracSingle());
  pvTxEffXY[2][nPv].InsertText(buf);
  Int_t color=Int_t(frS*20.)+1;
  if(color > 20) color=20;
  pvTxEffXY[2][nPv].SetFillColor(color+30);
}


void HMdcEfficiencyLayer::drawCanvXY(Int_t indUpDr) {
  // indUpDr=0 - draw canv
  // indUpDr=1 - update
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  for(Int_t nPl=0; nPl<3; nPl++) {
    canvXY[nPl]=(TCanvas*)fSegColl->FindObject(nameCanvXY[nPl]);
    if( indUpDr == 0 && canvXY[nPl] == 0 && listPads[nPl+1] ) {
      canvXY[nPl]=new TCanvas(nameCanvXY[nPl],titleCanvXY[nPl],1,1,600,600);
      for(Int_t i=0; i<256; i++) indDrawPv[nPl][i]=1;
      indDrawTitXY[nPl]=1;
    }
  }
  setPavesXY();
  for(Short_t xp=0; xp<16; xp+=nBinsSumXY) {
    for(Short_t yp=0; yp<16; yp+=nBinsSumXY) {
      getEffXY(&effXY,xp,xp+nBinsSumXY-1,yp,yp+nBinsSumXY-1);
      if(effXY.getTotNumTracks() > 0.) {
        Int_t nPv=xp/nBinsSumXY+(yp/nBinsSumXY)*16;
        for(Int_t nPl=0; nPl<3; nPl++) {
          if( canvXY[nPl] == 0 ) continue;
          canvXY[nPl]->cd();
          pvTxEffXY[nPl][nPv].Clear();
          if( nPl == 0) fillPaveXYnHits(nPv);
          else if( nPl == 1) fillPaveXYeff(nPv);
          else fillPaveXYsing(nPv);
          if( indDrawPv[nPl][nPv] ) {
            pvTxEffXY[nPl][nPv].Draw();
            indDrawPv[nPl][nPv]=0;
          }
        }
      }
    }
  }
  getEffXY(&effXY);
  for(Int_t nPl=0; nPl<3; nPl++) {
    if( canvXY[nPl] == 0 ) continue;
    canvXY[nPl]->cd();
    Char_t buf[80];
    if( nPl == 0 ) sprintf(buf,"S%i M%i L%i:  N0=%.0f  N1=%.0f N2=%.0f",
      sector+1,module+1,layer+1,
      effXY.getN0(),effXY.getN1(),effXY.getN2());
    else if( nPl == 1 ) sprintf(buf,"S%i M%i L%i:  E=%.3f+/-%.4f",sector+1,
      module+1,layer+1,effXY.getEfficiency(), effXY.getErrEfficiency());
    else sprintf(buf,"S%i M%i L%i:  S=%.3f+/-%.4f",sector+1,module+1,layer+1,
                       effXY.getFracSingle(), effXY.getErrFracSingle());
    titlePadXY[nPl].Clear();
    titlePadXY[nPl].InsertText(buf);
    if( indDrawTitXY[nPl] ) {
      titlePadXY[nPl].Draw();
      indDrawTitXY[nPl]=0;
    }
    canvXY[nPl]->Modified();
    canvXY[nPl]->Update();
  }
}

//------------------------------

HMdcEfficiencyMod::HMdcEfficiencyMod(Short_t sec, Short_t mod) {
  sector=sec;
  module=mod;

  // Geting of pointers to par. cont.
  HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
  if( !fGetCont ) return;
  HMdcLookUpTb*   fLookUpTb   = HMdcLookUpTb::getExObject();
  HMdcLookupGeom* flookupGeom = fGetCont->getMdcLookupGeom();
  HMdcRawStruct*  frawStruct  = fGetCont->getMdcRawStruct();
//!!!  if( !fLookUpTb ) return;

  // Geting of proj. plane sizes
  if( fLookUpTb && &((*fLookUpTb)[sector]) ) {
    HMdcLookUpTbSec& flookutsec = (*fLookUpTb)[sec];
    xLow = flookutsec.getXlow();
    xUp  = flookutsec.getXup();
    yLow = flookutsec.getYlow();
    yUp  = flookutsec.getYup();
    dX   = (xUp-xLow)/16;
    dY   = (yUp-yLow)/16;
    // creating of arrays
    clear();
  } else {
    xLow=xUp=yLow=yUp=0.;
    dX=dY=-1.;
  }

  // geting of pointers to HMdcLookupGeom[sec][mod] and HMdcLookupRaw[sec][mod]
  flookupGMod = (flookupGeom) ? &(*flookupGeom)[sector][module] :0;
  frawModStru = (frawStruct) ? &(*frawStruct)[sector][module] : 0;

  hsEffMb       = NULL;
  hsPattMbNoCut = NULL;
  hsPattMbCutS  = NULL;
  hsPattMbAfCut = NULL;
  hsPattMbClust = NULL;
  nChannels     = 0;
  for(Int_t i=0; i<9; i++) {
    hsTimes[i]   = NULL;
    drHsTimes[i] = NULL;
  }
  plToTvsT1      = NULL;
  drPlToTvsT1    = NULL;
  tCutBox        = NULL;
  tCutBox2       = NULL;
  minTimeBin[0] = minTimeBin[1] = minTimeBin[2] =  1000000.;
  maxTimeBin[0] = maxTimeBin[1] = maxTimeBin[2] = -100000.0;
  HMdcGetContainers::getFileName(fileName);
  Char_t name[50];
  Char_t title[100];
  if(flookupGMod && frawStruct) {
    Int_t nMoth=frawModStru->getNMotherboards();
    for(Int_t nMb=0; nMb<nMoth; nMb++) {
      Int_t nChMoth = (*frawModStru)[nMb].getNTdcs();
      if( nChMoth > 0 ) nChannels += nChMoth;
    }
    gROOT->cd();
    sprintf(name,"S%iM%iEffMb",sector+1,module+1);
    hsEffMb = new TH1F(name,"",nChannels,0.,nChannels/8);
    hsEffMb->SetOption("e1p");
    hsEffMb->SetMarkerStyle(21);
    hsEffMb->SetMarkerSize(0.3);
    hsEffMb->SetMarkerColor(4);
    hsEffMb->SetMaximum(1.1);
    hsEffMb->SetMinimum(0.0);
    hsEffMb->SetYTitle("Efficiency");
    hsEffMb->SetXTitle("MBo,TDC");
    hsEffMb->SetNdivisions(0);
    sprintf(name,"S%iM%iEffMbS4",sector+1,module+1);
    hsEffMb4=new TH1F(name,"",nChannels/4,0.,nChannels/8);
    hsEffMb4->SetOption("e1p");
    hsEffMb4->SetMarkerStyle(21);
    hsEffMb4->SetMarkerSize(0.3);
    hsEffMb4->SetMarkerColor(4);
    hsEffMb4->SetMaximum(1.1);
    hsEffMb4->SetMinimum(0.0);
    hsEffMb4->SetYTitle("Efficiency");
    hsEffMb4->SetXTitle("MBo,TDC");
    hsEffMb4->SetNdivisions(0);
    
    sprintf(name,"S%iM%iPattMbNoCut",sector+1,module+1);
    sprintf(title,"S%i M%i Nhits;MBo,TDC",sector+1,module+1);
    hsPattMbNoCut = new TH1F(name,title,nChannels,0.,nChannels/8);
    hsPattMbNoCut->SetNdivisions(0);
    sprintf(name,"S%iM%iPattMbCutS",sector+1,module+1);
    hsPattMbCutS  = new TH1F(name,";MBo,TDC",nChannels,0.,nChannels/8);
    sprintf(name,"S%iM%iPattMbAfCut",sector+1,module+1);
    sprintf(title,"S%i M%i Nhits;MBo,TDC",sector+1,module+1);
    hsPattMbAfCut = new TH1F(name,title,nChannels,0.,nChannels/8);
    hsPattMbAfCut->SetNdivisions(0);
    sprintf(name,"S%iM%iPattMbClust",sector+1,module+1);
    hsPattMbClust = new TH1F(name,";MBo,TDC",nChannels,0.,nChannels/8);
  }
  const Char_t* titT[9]={
      "t1",
      "t2",
      "t2-t1",
      "t1 (after t.cuts)",
      "t2 (after t.cuts)",
      "t2-t1 (after t.cuts)",
      "t1 (clust.)",
      "t2 (clust.)",
      "t2-t1 (clust.)" };
  const Char_t* nmT[9] ={
      "Time1a",
      "Time2a",
      "Time2m1a",
      "Time1c",
      "Time2c",
      "Time2m1c",
      "Time1sc",
      "Time2sc",
      "Time2m1sc" };

  biningTimes=1;
  Int_t colors[3]={4,3,6};
  for(Int_t i=0; i<9; i++) {
    Axis_t x1   = -1000.;
    Axis_t x2   = 3200.;
    Int_t  nBHs = 840;
    if( (i+1)%3 == 0 ) {
      x1   = -100.;
      x2   = 900.;
      nBHs = 500;
    }
    sprintf(name,"S%iM%i%s",sector+1,module+1,nmT[i]);
    sprintf(title,"S%i M%i:  %s;%s",sector+1,module+1,titT[i],titT[i]);
    hsTimes[i] = new TH1F(name,title,nBHs,x1,x2);
    hsTimes[i]->SetFillColor(colors[i/3]);
    hsTimes[i]->SetLineColor(colors[i/3]);
  }
  sprintf(name,"S%iM%iToTvsT1",sector+1,module+1);
  sprintf(title,"S%i M%i:  ToT vs t1;t1;t2-t1",sector+1,module+1);
  plToTvsT1 = new TH2F(name,title,840/2,-1000.,3200.,500/2,-100.,900.);

  // init of HMdcEfficiencyLayer array
  for(Short_t layer = 0; layer < 6; layer++) mdcEffLay[layer].init(sector,module,layer);

  setNameCanv();
}

void HMdcEfficiencyMod::setNameCanv(void){
  for(Int_t nCanv=0; nCanv<6; nCanv++) canvases[nCanv]=0;
  Char_t buf[200];
  sprintf(buf,"%s_S%iM%i_Pattern",fileName.Data(),sector+1,module+1);
  nameCanv[0]=buf;
  sprintf(buf,"%s_S%iM%i_NtrNhits",fileName.Data(),sector+1,module+1);
  nameCanv[1]=buf;
  sprintf(buf,"%s_S%iM%i_Eff",fileName.Data(),sector+1,module+1);
  nameCanv[2]=buf;
  sprintf(buf,"%s_S%iM%i_SingC",fileName.Data(),sector+1,module+1);
  nameCanv[3]=buf;
  sprintf(buf,"%s_S%iM%i_Prob",fileName.Data(),sector+1,module+1);
  nameCanv[4]=buf;
  sprintf(buf,"%s_S%iM%i_Ratio",fileName.Data(),sector+1,module+1);
  nameCanv[5]=buf;
  sprintf(buf,"%s S%i M%i Pattern",fileName.Data(),sector+1,module+1);
  titleCanv[0]=buf;
  sprintf(buf,"%s S%i M%i Num. of tracks and zero, single and double hits",
      fileName.Data(),sector+1,module+1);
  titleCanv[1]=buf;
  sprintf(buf,"%s S%i M%i Efficiency",fileName.Data(),sector+1,module+1);
  titleCanv[2]=buf;
  sprintf(buf,"%s S%i M%i Single hits fraction",fileName.Data(),sector+1,module+1);
  titleCanv[3]=buf;
  sprintf(buf,"%s S%i M%i Probability to have zero, single and double hit",
      fileName.Data(),sector+1,module+1);
  titleCanv[4]=buf;
  sprintf(buf,"%s S%i M%i Ratio Nclst.wires/Nwires",fileName.Data(),sector+1,module+1);
  titleCanv[5]=buf;
  nPads=0;
  for(Int_t nLay=0; nLay<6; nLay++) listLayers[nLay]=0;
  canvasMb = 0;
  sprintf(buf,"%s_S%iM%i_EffMb",fileName.Data(),sector+1,module+1);
  nameCanvMb=buf;
  sprintf(buf,"%s S%i M%i Cells efficiency",fileName.Data(),sector+1,module+1);
  titleCanvMb=buf;
  sprintf(nameSliderMb,"Sl_S%iM%iEffMb",sector+1,module+1);
  sprintf(buf,"%s_S%iM%i_Times",fileName.Data(),sector+1,module+1);
  nameCanvTimes=buf;
  sprintf(buf,"%s S%i M%i time1, time2, time2-time1 ",fileName.Data(),sector+1,module+1);
  titleCanvTimes=buf;
}

HMdcEfficiencyMod::~HMdcEfficiencyMod() {
  // destructor
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  canvasMb = (TCanvas*)fSegColl->FindObject(nameCanvMb);
  if( canvasMb ) delete canvasMb;
  TCanvas* canv = (TCanvas*)fSegColl->FindObject(nameCanvTimes);
  if( canv ) delete canv;
  for(Int_t i=0; i<6; i++) {
    canvases[i] = (TCanvas*)fSegColl->FindObject(nameCanv[i]);
    if( canvases[i] ) delete canvases[i];
  }
  if(tCutBox  != NULL) delete tCutBox;
  if(tCutBox2 != NULL) delete tCutBox2;
  tCutBox  = NULL;
  tCutBox2 = NULL;
}

void HMdcEfficiencyMod::clear(void) {
  for (Short_t xy=0; xy<256; xy++) n0xy[xy]=n1xy[xy]=n2xy[xy]=0;
}

Int_t HMdcEfficiencyMod::getBinNum(Float_t x, Float_t y) {
  if(dX<=0. || dY<=0.) return -1000000;
  Int_t nX=(Int_t)((x-xLow+dX)/dX)-1;
  Int_t nY=(Int_t)((y-yLow+dY)/dY)-1;
  if(nX<0 || nX>=16 || nY<0 || nY>=16) return -1;
  return nY*16+nX;
}

void HMdcEfficiencyMod::getEffXY(HMdcEffMod* efficiency) {
  // Calculation of efficiency of MDC layer.
  getEffXY(efficiency,0,15,0,15);
}

void HMdcEfficiencyMod::getEffXY(HMdcEffMod* efficiency,
                              Short_t xBin, Short_t yBin){
  // Calculation of efficiency of MDC for bin (xBin:yBin).
  getEffXY(efficiency,xBin,xBin,yBin,yBin);
}
void HMdcEfficiencyMod::getEffXY(HMdcEffMod* efficiency,
                                   Short_t fXbin, Short_t lXbin,
                                   Short_t fYbin, Short_t lYbin){
  // Calculation of efficiency of MDC ragion
  // (MDC is divided on 16x16 bins).
  // Region: fXbin <= x <= lXbin, fYbin <= y <= lYbin
  Int_t nC0,nC1,nC2;
  nC0=nC1=nC2=0;
  Double_t eN0,eN1,eN2;
  eN0=eN1=eN2=0.;

  Int_t fXb=(fXbin < 0 ) ?  0 : fXbin;
  Int_t lXb=(lXbin > 15) ? 15 : lXbin;
  Int_t fYb=(fYbin < 0 ) ?  0 : fYbin;
  Int_t lYb=(lYbin > 15) ? 15 : lYbin;
  for(Int_t y=fYb; y<=lYb; y++) {
    Int_t nBin=y*16;
    for(Int_t x=fXb; x<=lXb; x++){
      nC0+=n0xy[nBin+x];
      nC1+=n1xy[nBin+x];
      nC2+=n2xy[nBin+x];
    }
  }
  if(nC0+nC1+nC2 > 0) {
    eN0=(nC0>0) ? sqrt((Double_t)nC0) : 1.;
    eN1=(nC1>0) ? sqrt((Double_t)nC1) : 1.;
    eN2=(nC2>0) ? sqrt((Double_t)nC2) : 1.;
  }
  efficiency->calcEff((Double_t)nC0,eN0, (Double_t)nC1,eN1, (Double_t)nC2,eN2);
}

void HMdcEfficiencyMod::setListLayers(Int_t* listLay) {
  nPads=0;
  for(Int_t lay=0; lay<6; lay++) {
    listLayers[lay]=(listLay[lay]) ? 1:0;
    if(listLayers[lay]) nPads++;
  }
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  for(Int_t nCanv=0; nCanv<6; nCanv++) {
    TCanvas* canv=(TCanvas*)fSegColl->FindObject(nameCanv[nCanv]);
    if(!canv) continue;
    if(nPads>0) {
      Int_t nPd=0;
      if(nPads != (nPd=canv->GetListOfPrimitives()->GetSize())) {
        canv->Clear();
        if(nPd<3 || nPads<3) canv->SetWindowSize(700,(nPads>3) ? 900:nPads*300);
        setPads(canv);
      }
      if(nCanv==0) drawCanvTotNhits();
      else if(nCanv==1) drawCanvNHits();
      else if(nCanv==2) drawCanvHistEff();
      else if(nCanv==3) drawCanvHistSing();
      else if(nCanv==4) drawCanvProbabilitis();
      else if(nCanv==5) drawCanvRatio();
    } else delete canv;
  }
}

void HMdcEfficiencyMod::creatCanvas(Int_t nCanv) {
  if(gROOT->GetListOfCanvases()->FindObject(nameCanv[nCanv])) return;
  if(nPads==0) return;
  canvases[nCanv]=new TCanvas(nameCanv[nCanv],titleCanv[nCanv],
 			    1,1,700,(nPads>3) ? 900 : nPads*300);
  setPads(canvases[nCanv]);
}

void HMdcEfficiencyMod::setPads(TCanvas* canv) {
  canv->Divide(1,nPads,0.0001,0.0001);
  for(Int_t nPad=0; nPad<nPads; nPad++) {
    canv->cd(nPad+1);
    canv->SetLeftMargin(0.05);
    canv->SetRightMargin(0.02);
    canv->SetBottomMargin(0.1);
    canv->SetTopMargin(0.15);
  }
}

void HMdcEfficiencyMod::updateCanv(void) {
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  for(Int_t nCanv=0; nCanv<6; nCanv++){
    TCanvas* canv=(TCanvas*)fSegColl->FindObject(nameCanv[nCanv]);
    if(!canv) continue;
    drawHists(nCanv);
  }
  if(fSegColl->FindObject(nameCanvMb)) drawCanvEffMb();
  if(fSegColl->FindObject(nameCanvTimes)) drawCanvTimes();
}

void HMdcEfficiencyMod::drawHists(Int_t nCanv) {
  Int_t nPad=1;
  for(Short_t lay=0; lay<6; lay++) {
    if(!listLayers[lay]) continue;
    canvases[nCanv]->cd(nPad++);
    if(nCanv==0)      (*this)[lay].drawTotNhits();
    else if(nCanv==1) (*this)[lay].drawNHits();
    else if(nCanv==2) (*this)[lay].drawHistEff();
    else if(nCanv==3) (*this)[lay].drawHistSing();
    else if(nCanv==4) (*this)[lay].drawProbabilitis();
    else if(nCanv==5) (*this)[lay].drawRatio();
  }
  canvases[nCanv]->Modified();
  canvases[nCanv]->Update();
}

void HMdcEfficiencyMod::drawCanvTotNhits(void) {
  creatCanvas(0);
  drawHists(0);
}

void HMdcEfficiencyMod::saveCanvases(UInt_t saveAs) {
  Int_t nPadsTmp=nPads;
  nPads=6;
  TString nmCanv;
  for(Int_t nCanv=0; nCanv<6; nCanv++) {
    nmCanv = nameCanv[nCanv];
    nmCanv.Remove(0,fileName.Length()+1);
    TCanvas* canv = new TCanvas(nmCanv,titleCanv[nCanv],1,1,700,900);
    setPads(canv);
    for(Int_t lay=0; lay<6; lay++) {
      canv->cd(lay+1);
      if(nCanv==0)      (*this)[lay].drawTotNhits();
      else if(nCanv==1) (*this)[lay].drawNHits();
      else if(nCanv==2) (*this)[lay].drawHistEff();
      else if(nCanv==3) (*this)[lay].drawHistSing();
      else if(nCanv==4) (*this)[lay].drawProbabilitis();
      else if(nCanv==5) (*this)[lay].drawRatio();
    }
    if(saveAs&&HMdcEfficiency::getFilePointer()) {
      HMdcEfficiency::getFilePointer()->cd();
      canv->Write();
    }
    delete canv;
  }
  nPads=nPadsTmp;
}

void HMdcEfficiencyMod::drawCanvNHits(void) {
  creatCanvas(1);
  drawHists(1);
}

void HMdcEfficiencyMod::drawCanvHistEff(void) {
  creatCanvas(2);
  drawHists(2);
}

void HMdcEfficiencyMod::drawCanvHistSing(void) {
  creatCanvas(3);
  drawHists(3);
}

void HMdcEfficiencyMod::drawCanvProbabilitis(void) {
  creatCanvas(4);
  drawHists(4);
}

void HMdcEfficiencyMod::drawCanvRatio(void) {
  creatCanvas(5);
  drawHists(5);
}

void HMdcEfficiencyMod::delCanv(Int_t nCanv){
  if(gROOT->GetListOfCanvases()->FindObject(nameCanv[nCanv])) delete canvases[nCanv];
  canvases[nCanv]=0;
}

void HMdcEfficiencyMod::fillHistMb(void) {
  if( !flookupGMod || !frawModStru) return;
  Int_t bin=0;
  Int_t bin4=0;
  Int_t nMoth=frawModStru->getNMotherboards();
  for(Int_t nMb=0; nMb<nMoth; nMb++) {
    Int_t nChan = (*frawModStru)[nMb].getNTdcs();
    if( nChan <= 0 ) continue;
    HMdcLookupMoth* fMb=&(*flookupGMod)[nMb];
    for(Int_t nCh4=0; nCh4<nChan/4; nCh4++) {
      Int_t nLay4,nCell1,nCell2;
      nLay4=nCell1=nCell2=-1;
      bin4++;
      for(Int_t i4=0; i4<4; i4++) {
        Int_t nCh=nCh4*4+i4;
        bin++;
        if( !fMb ) continue;
        HMdcLookupChan* fchan=&(*fMb)[nCh];
        if( !fchan ) continue;
        Int_t layer = fchan->getNLayer();
        Int_t cell  = fchan->getNCell();
        if(cell<0 || layer<0) continue;
        HMdcEfficiencyLayer& flay=(*this)[layer];
        flay.getEffCells(&eff, cell);
        hsEffMb->SetBinContent(bin,eff.getEfficiency());
        hsEffMb->SetBinError(bin,eff.getErrEfficiency());
        if(nLay4==-1) {
          nCell1 = nCell2 = cell;
          nLay4 = layer;
        } else {
          if(nLay4 != layer) Error("fillHistMb","nLay4 != layer");
          nCell2 = cell;
        }

        hsPattMbNoCut->SetBinContent(bin,eff.getTotNhitsNoCut());
        hsPattMbCutS->SetBinContent(bin,eff.getTotNhitsCutS());
        hsPattMbAfCut->SetBinContent(bin,eff.getTotNhits());
        hsPattMbClust->SetBinContent(bin,eff.getTotNhitsClus());
      }
      if(nCell1>nCell2) {
        Int_t tmp = nCell1;
        nCell1    = nCell2;
        nCell2    = tmp;
      }
      if(nLay4>=0 && nCell1>=0 && nCell2-nCell1<4) {
        HMdcEfficiencyLayer& flay=(*this)[nLay4];
        flay.getEffCells(&eff, nCell1, nCell2);
        hsEffMb4->SetBinContent(bin4,eff.getEfficiency());
        hsEffMb4->SetBinError(bin4,eff.getErrEfficiency());
      }
    }
  }
  getEffXY(&effMod);
  Char_t title[70];
  sprintf(title,"Sec.%i Mod.%i :     E=%.4f +/- %.5f",sector+1,module+1,
             effMod.getEfficiency(),effMod.getErrEfficiency());
  hsEffMb->SetTitle(title);
}

void HMdcEfficiencyMod::drawCanvEffMb(void) {
  if( !flookupGMod || !frawModStru) return;
  canvasMb = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanvMb);
  if( !canvasMb ) {
    canvasMb = new TCanvas(nameCanvMb,titleCanvMb,1,1,1000,900);
    creatCanvEffMb(canvasMb);
  }
  fillCanvEffMb(canvasMb);
  if(!(TSlider*)gROOT->FindObject(nameSliderMb)) {
    canvasMb->cd();
    Float_t x1 = 0.05f;
    Float_t x2 = 0.98f;
    mbSlider=new TSlider(nameSliderMb,"", x1,0.49*0.66,x2, 0.53*0.66);
    mbSlider->SetObject(this);

    Int_t nChan=0;
    Int_t nMoth=frawModStru->getNMotherboards();
    for(Int_t nMb=0; nMb<nMoth; nMb++) {
      Int_t nChMoth = (*frawModStru)[nMb].getNTdcs();
      if( nChMoth <= 0 ) continue;
      Float_t x1p = Float_t(nChan)/Float_t(nChannels)*(x2-x1)+x1;
      nChan += nChMoth;
      Float_t x2p = Float_t(nChan)/Float_t(nChannels)*(x2-x1)+x1;
      Char_t nameMb[100];
      sprintf(nameMb,"%i : %s",nMb+1,(*frawModStru)[nMb].GetName());
      TPaveLabel* pave = new TPaveLabel(x1p,0.45*0.66,x2p,0.48*0.66,nameMb);
      pave->SetBorderSize(1);
      pave->Draw();
    }
  }
  mbSlider->SetRange( Double_t(hsEffMb->GetXaxis()->GetFirst())/Double_t(nChannels),
                      Double_t(hsEffMb->GetXaxis()->GetLast())/Double_t(nChannels));
  canvasMb->cd(0);
  canvasMb->Modified();
  canvasMb->Update();
}

void HMdcEfficiencyMod::saveCanvEffMb(UInt_t saveAs) {
  if( !flookupGMod || !frawModStru) return;
  TString nmCanv=nameCanvMb;
  nmCanv.Remove(0,fileName.Length()+1);
  TCanvas* canv = new TCanvas(nmCanv,titleCanvMb,1,1,1000,600); 
  canv->cd();
  Float_t x1=0.05;
  Float_t x2=0.98;
  Int_t nChan=0;
  Int_t nMoth=frawModStru->getNMotherboards();
  for(Int_t nMb=0; nMb<nMoth; nMb++) {
    Int_t nChMoth = (*frawModStru)[nMb].getNTdcs();
    if( nChMoth <= 0 ) continue;
    Float_t x1p = Float_t(nChan)/Float_t(nChannels)*(x2-x1)+x1;
    nChan += nChMoth;
    Float_t x2p = Float_t(nChan)/Float_t(nChannels)*(x2-x1)+x1;
    Char_t nameMb[100];
    sprintf(nameMb,"%i : %s",nMb+1,(*frawModStru)[nMb].GetName());
    TPaveLabel* pave = new TPaveLabel(x1p,0.45,x2p,0.48,nameMb);
    pave->SetBorderSize(1);
    pave->Draw();
  }
  canv->cd();
  creatCanvEffMb(canv);
  fillCanvEffMb(canv);
  canv->Modified();
  canv->Update();
  if(saveAs==1&&HMdcEfficiency::getFilePointer()) {
      HMdcEfficiency::getFilePointer()->cd();
      canv->Write();
  }
  delete canv;
}

void HMdcEfficiencyMod::creatCanvEffMb(TCanvas* canv) {
  canv->Divide(1,3);
  canv->cd(1);
  canv->SetBorderMode(0);
  canv->SetFillColor(10);
  TPad *pad1 = (TPad*)gPad;
  pad1->SetPad(0.0,0.66,1.,1.);
  pad1->SetRightMargin(0.02);
  pad1->SetLeftMargin(0.05);
  pad1->SetTopMargin(0.1);
  pad1->SetFillColor(10);
  
  canv->cd(2);
  canv->SetBorderMode(0);
  canv->SetFillColor(10);
  TPad *upPad = (TPad*)gPad;
  upPad->SetPad(0.0,0.53*0.66,1.,1.*0.66);
  upPad->SetRightMargin(0.02);
  upPad->SetLeftMargin(0.05);
  upPad->SetTopMargin(0.1);
  upPad->SetFillColor(10);
  canv->cd(3);
  TPad *lowPad = (TPad*)gPad;
  lowPad->SetPad(0.0,0.0,1.,0.44*0.66);
  lowPad->SetRightMargin(0.02);
  lowPad->SetLeftMargin(0.05);
  lowPad->SetTopMargin(0.02);
  lowPad->SetFillColor(10);
}

void HMdcEfficiencyMod::fillCanvEffMb(TCanvas* canv) {
  canv->cd();
  canv->cd(1);
  fillCanvPattMb(canv,1);
  canv->cd(2);
  gPad->SetFrameFillColor(10);
  hsEffMb->GetXaxis()->SetTitleSize(0.06);
  hsEffMb->SetTitleOffset(0.45,"Y");;
  hsEffMb->SetTitleOffset(0.8,"X");
  hsEffMb->GetYaxis()->SetTitleSize(0.05);
  hsEffMb->GetXaxis()->SetTitleSize(0.05);
  hsEffMb->GetYaxis()->CenterTitle(1);
  hsEffMb->GetXaxis()->SetLabelSize(0.05);
  hsEffMb->GetYaxis()->SetLabelSize(0.05);
  hsEffMb->Draw();
  drawLineEffMb(hsEffMb);
  drawLineGrMb(hsEffMb);
  canv->cd(3);
  gPad->SetFrameFillColor(10);
  hsEffMb4->SetTitleOffset(0.45,"Y");
  hsEffMb4->SetTitleOffset(0.8,"X");
  hsEffMb4->GetYaxis()->SetTitleSize(0.05);
  hsEffMb4->GetXaxis()->SetTitleSize(0.05);
  hsEffMb4->GetYaxis()->CenterTitle(1);
  hsEffMb4->GetXaxis()->SetLabelSize(0.05);
  hsEffMb4->GetYaxis()->SetLabelSize(0.05);
  hsEffMb4->Draw();
  drawLineEffMb(hsEffMb4);
  drawLineGrMb(hsEffMb4);
}

void HMdcEfficiencyMod::fillCanvPattMb(TCanvas* canv,Int_t pad) {
  canv->cd();
  canv->cd(pad);
  gPad->SetFrameFillColor(10);
  hsPattMbAfCut->SetFillColor(3);
  hsPattMbClust->SetLineColor(6);
  if(TMath::Abs(hsPattMbAfCut->GetSumOfWeights()-hsPattMbNoCut->GetSumOfWeights())>0.5) {
    Char_t title[90];
    Float_t h0      = hsPattMbAfCut->GetSumOfWeights();
    Float_t h1      = hsPattMbNoCut->GetSumOfWeights();
    Float_t h11     = hsPattMbCutS->GetSumOfWeights();
    Float_t prCut   = (h1-h0)/h1*100.;
    Float_t prCutS  = (h1-h11)/h1*100.;
    Float_t prCutTD = (h11-h0)/h1*100.;
    sprintf(title,"S%i M%i: Nhits (Com.stop cut(%6.2f%c)+times diff. cut(%6.2f%c)=%6.2f%c)",
        sector+1,module+1,prCutS,'%',prCutTD,'%',prCut,'%');
    HMdcEfficiencyLayer::setTitle(hsPattMbNoCut,"Nhits");
    hsPattMbNoCut->SetTitle(title);
    hsPattMbNoCut->SetFillColor(4);
    hsPattMbNoCut->Draw();
    hsPattMbCutS->SetFillColor(5);
    hsPattMbCutS->Draw("same");
    hsPattMbAfCut->Draw("same");
    hsPattMbClust->Draw("same");
    //???
    drawLineGrMb(hsPattMbNoCut);
 } else {
    HMdcEfficiencyLayer::setTitle(hsPattMbAfCut,"Nhits");
    hsPattMbAfCut->SetMaximum(hsPattMbNoCut->GetMaximum());
    hsPattMbAfCut->Draw();
    hsPattMbClust->Draw("same");
    //???
    drawLineGrMb(hsPattMbNoCut);
  }
//   hsPattMbNoCut->GetXaxis()->SetTitleSize(0.06);
//   hsPattMbNoCut->SetTitleOffset(0.45,"Y");;
//   hsPattMbNoCut->SetTitleOffset(0.8,"X");
//   hsPattMbNoCut->GetYaxis()->SetTitleSize(0.05);
//   hsPattMbNoCut->GetXaxis()->SetTitleSize(0.05);
//   hsPattMbNoCut->GetYaxis()->CenterTitle(1);
//   hsPattMbNoCut->GetXaxis()->SetLabelSize(0.05);
//   hsPattMbNoCut->GetYaxis()->SetLabelSize(0.05);
//   hsPattMbNoCut->Draw();
//   drawLineGrMb(hsPattMbNoCut);
}

void HMdcEfficiencyMod::drawLineGrMb(TH1F* hst) {
  Float_t ymin=hst->GetMinimum();
  Float_t ymax=hst->GetMaximum();
  Float_t xf=hst->GetXaxis()->GetBinLowEdge(hst->GetXaxis()->GetFirst());
  Float_t xl=hst->GetXaxis()->GetBinUpEdge(hst->GetXaxis()->GetLast());
  TLine lineMb;
  lineMb.SetLineColor(3);
  Int_t nChan=0;
  Int_t nMoth=frawModStru->getNMotherboards();
  TText lbMb;
  for(Int_t nMb=0; nMb<nMoth; nMb++) {
    Int_t nChMoth = (*frawModStru)[nMb].getNTdcs();
    if( nChMoth <= 0 ) continue;
    lineMb.SetLineStyle(2);
    lineMb.SetLineColor(3);
    for(Int_t nTDC=0; nTDC<nChMoth/8; nTDC++) {
      lbMb.SetTextColor(3);
      Float_t x=Float_t(nChan/8+nTDC);
      if(x>xf && x<xl && nTDC>0) lineMb.DrawLine(x, ymin, x, ymax);
      if(x>=xf && x<xl) {
        Char_t labl[3];
        if( xl-xf <= 56.) {
          sprintf(labl,"%i",nTDC+1);
          lbMb.SetTextAlign(23);
          lbMb.DrawText(x+0.5,ymin-(ymax-ymin)*0.005,labl);
        }
        else if( nTDC==0 || nTDC==nChMoth/8-1 ) {
          sprintf(labl,"%i",nTDC+1);
          Int_t indFL=(nTDC+1)/(nChMoth/8);
          lbMb.SetTextAlign((indFL*2+1)*10+3);
          lbMb.DrawText(x+indFL+(1-indFL*2)*0.3,ymin-(ymax-ymin)*0.005,labl);
        }

      }
    }
    Float_t x=Float_t(nChan/8);
    if(x>=xf && x<=xl) {
      Char_t labl[3];
      sprintf(labl,"%i",nMb+1);
      lbMb.SetTextColor(1);
      lbMb.SetTextAlign(23);
      lbMb.DrawText(x,ymin-(ymax-ymin)*0.05,labl);
    }
    nChan+=nChMoth;
    lineMb.SetLineColor(6);
    lineMb.SetLineStyle(1);
    x=Float_t(nChan/8);
    if(x>xf && x<xl) lineMb.DrawLine(x, ymin-(ymax-ymin)*0.05, x, ymax);
  }
}

void HMdcEfficiencyMod::drawLineEffMb(TH1F* hst) {
  Float_t xf=hst->GetXaxis()->GetBinLowEdge(hst->GetXaxis()->GetFirst());
  Float_t xl=hst->GetXaxis()->GetBinUpEdge(hst->GetXaxis()->GetLast());
  TLine lnEffMod;
  lnEffMod.SetLineStyle(1);
  lnEffMod.SetLineColor(2);
  lnEffMod.DrawLine(xf,effMod.getEfficiency(),xl,effMod.getEfficiency());
  lnEffMod.SetLineStyle(2);
  lnEffMod.DrawLine(xf,effMod.getEfficiency()+effMod.getErrEfficiency(),
                    xl,effMod.getEfficiency()+effMod.getErrEfficiency());
  lnEffMod.DrawLine(xf,effMod.getEfficiency()-effMod.getErrEfficiency(),
                    xl,effMod.getEfficiency()-effMod.getErrEfficiency());
}

void HMdcEfficiencyMod::delCanvMb(void){
  canvasMb=(TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanvMb);
  if( canvasMb ) {
    delete canvasMb;
    canvasMb=0;
  }
}

void HMdcEfficiencyMod::fillTimes(HMdcCal1* cal, Bool_t isGnt, Int_t nHist) {
  if( hsTimes[nHist] == NULL ) return;
  if(cal == NULL) return;
  // nHits==0 tested befor call to this function.
  Int_t nHits = abs(cal->getNHits());
  if(nHits == 0 || nHits>2 ) return;
  Bool_t tm1 = kTRUE;
  Bool_t tm2 = nHits==2;
  if(isGnt) {
    HMdcCal1Sim* calS=(HMdcCal1Sim*)cal;
    if(calS->getStatus1()<0)             tm1=kFALSE;
    if(nHits==2 && calS->getStatus2()<0) tm2=kFALSE;
  }
  if( tm1 ) {
    Float_t time = cal->getTime1();
    if(hsTimes[nHist]) hsTimes[nHist]->Fill(time);
    if( nHist == 0 /* && time > -998.*/ ) {
      if( time < minTimeBin[0] ) minTimeBin[0] = time;
      if( time > maxTimeBin[0] ) maxTimeBin[0] = time;
    }
  }
  if( tm2 ) {
    Float_t time = cal->getTime2();
    if(hsTimes[nHist+1]) hsTimes[nHist+1]->Fill(time);
    if( nHist == 0 /* && time > -998.*/ ) {
      if( time < minTimeBin[1] ) minTimeBin[1] = time;
      if( time > maxTimeBin[1] ) maxTimeBin[1] = time;
    }
  }
  if(tm1 && tm2) {
    Float_t tot = cal->getTime2()-cal->getTime1();
    if(hsTimes[nHist+2]) hsTimes[nHist+2]->Fill(tot);
    if(plToTvsT1)        plToTvsT1->Fill(cal->getTime1(),tot);
    if( nHist == 0 /* && tot > -998.*/ ) {
      if( tot < minTimeBin[2] ) minTimeBin[2] = tot;
      if( tot > maxTimeBin[2] ) maxTimeBin[2] = tot;
    }
  }
}

void HMdcEfficiencyMod::delCanvTimes(void) {
  TCanvas* canv=(TCanvas*)gROOT->GetListOfCanvases()->
      FindObject(nameCanvTimes);
  if(canv) delete canv;
}

void HMdcEfficiencyMod::drawCanvTimes(Int_t bining, UInt_t saveAs) {
  if(bining>0 && bining <=100) biningTimes=bining;
  TCanvas* canv=0;
  if(saveAs==0) {
    canv = (TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanvTimes);
    if( canv == NULL ) {
      canv = new TCanvas(nameCanvTimes,titleCanvTimes,50, 200,900, 700);
      canv->Divide(2,2,0.0001,0.0001);
      canv->SetBorderMode(0);
      canv->SetFillColor(10);
    }
  } else {
    TString nmCanv;
    nmCanv = nameCanvTimes;
    nmCanv.Remove(0,fileName.Length()+1);
    canv = new TCanvas(nmCanv,titleCanvTimes,50, 200,900, 700);
    canv->Divide(2,2,0.0001,0.0001);
    canv->SetBorderMode(0);
    canv->SetFillColor(10);
  }
  gROOT->cd();
  Char_t buf[80];
  for(Int_t i=0; i<9; i++) {
    if(drHsTimes[i]) delete (drHsTimes[i]);
    drHsTimes[i] = new TH1F(*hsTimes[i]);
    sprintf(buf,"%sR",hsTimes[i]->GetName());
    drHsTimes[i]->SetName(buf);
    if( biningTimes > 1 ) drHsTimes[i]->Rebin(biningTimes);
  }
  for(Int_t i=0; i<9; i++)  {
    drHsTimes[i]->GetXaxis()->SetRangeUser(minTimeBin[i%3]-10.0,maxTimeBin[i%3]+10.0);
  }
  if(drPlToTvsT1) delete (drPlToTvsT1);
  drPlToTvsT1 = new TH2F(*plToTvsT1);
  sprintf(buf,"%sR",plToTvsT1->GetName());
  drPlToTvsT1->SetName(buf);
  if( biningTimes > 1 ) drPlToTvsT1->Rebin2D(biningTimes,biningTimes);
  drPlToTvsT1->GetXaxis()->SetRangeUser(minTimeBin[0]-10.0,maxTimeBin[0]+10.0);
  drPlToTvsT1->GetYaxis()->SetRangeUser(minTimeBin[2]-10.0,maxTimeBin[2]+10.0);
  for(Int_t i=0; i<3; i++) {
    canv->cd(i<2 ? i+1 : i+2);
    gPad->SetGrid();
    drHsTimes[i]->Draw();
    drHsTimes[i+3]->Draw("same");
    drHsTimes[i+6]->Draw("same");
  }
  canv->cd(3);
  gPad->SetLogz();
  gPad->SetGrid();
  drPlToTvsT1->Draw("colz");
  
  if(tCutBox == NULL) {
    tCutBox = new TBox();
    tCutBox->SetFillStyle(0);
    if(tCutBox2 == NULL) {
      tCutBox2 = new TBox();
      tCutBox2->SetFillStyle(0);
    }
  }
  HMdcTimeCut* fcut = HMdcTimeCut::getExObject();
  if(fcut != NULL) {
    tCutBox->SetX1(fcut->getCutT1L(sector,module));
    tCutBox->SetX2(fcut->getCutT1R(sector,module));
    tCutBox->SetY1(fcut->getCutDTime21L(sector,module));
    tCutBox->SetY2(fcut->getCutDTime21R(sector,module));
    tCutBox->Draw();
    if(fcut->getCutBumpT1L(sector,module)<fcut->getCutBumpT1R(sector,module)) {
      tCutBox2->SetX1(fcut->getCutBumpT1L(sector,module));
      tCutBox2->SetX2(fcut->getCutBumpT1R(sector,module));
      tCutBox2->SetY1(fcut->getCutBumpTotL(sector,module)); 
      tCutBox2->SetY2(fcut->getCutBumpTotR(sector,module));
      tCutBox2->Draw();
    }
  }
  canv->Modified();
  canv->Update();
  
  if(saveAs&&HMdcEfficiency::getFilePointer()) {
      HMdcEfficiency::getFilePointer()->cd();
      canv->Write();
      delete canv;
  }
}

void HMdcEfficiencyMod::ExecuteEvent(Int_t event, Int_t px, Int_t py){
  if(event != 11) return;
  mbSlider=(TSlider*)gROOT->FindObject(nameSliderMb);
  if(!mbSlider) return;
  Int_t nTDC=hsEffMb->GetNbinsX()/32;
  Float_t min=mbSlider->GetMinimum();
  if(min<0.) min=0.;
  Float_t max=mbSlider->GetMaximum();
  Int_t nBmin=Int_t(nTDC*min+0.1);
  if(nBmin==nTDC) nBmin--;
  Int_t nBmax=Int_t(nTDC*max+0.1);
  if(nBmax==nBmin) nBmax++;
  canvasMb->cd(2);
  hsEffMb->SetAxisRange(nBmin*4,nBmax*4-0.0001);
  hsEffMb->Draw();
  drawLineEffMb(hsEffMb);
  drawLineGrMb(hsEffMb);
  mbSlider->SetRange(Float_t(nBmin)/nTDC,Float_t(nBmax)/nTDC);
  canvasMb->Update();
}

//--Sector--------------------------

HMdcEfficiencySec::HMdcEfficiencySec(Short_t sec) {
  // constructor creates an array of pointers of type
  // HMdcEfficiencyMod
  sector=sec;
  array = new TObjArray(4);
  HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
  HMdcGetContainers::getFileName(fileName);
  for (Short_t mod = 0; mod < 4; mod++) if( fGetCont->isModActive(sector,mod) ) {
    (*array)[mod] = new HMdcEfficiencyMod(sector,mod);
  }
  Char_t buf[200];
  sprintf(buf,"%s_S%i_ProjPl",fileName.Data(),sector+1);
  nameCanvProj=buf;
  sprintf(buf,"%s S%i Cells projections",fileName.Data(),sector+1);
  titleCanvProj=buf;
  sprintf(buf,"%s_S%i_HitsPl",fileName.Data(),sector+1);
  nameCanvHits=buf;
  sprintf(buf,"%s S%i MDC hits",fileName.Data(),sector+1);
  titleCanvHits=buf;
  plotHits[0]=plotHits[1]=0;
}

HMdcEfficiencySec::~HMdcEfficiencySec() {
  // destructor
  TSeqCollection* fSegColl=gROOT->GetListOfCanvases();
  canvPlotProj=(TCanvas*)fSegColl->FindObject(nameCanvProj);
  if( canvPlotProj ) {
    delete canvPlotProj;
    canvPlotProj=0;
  }
  canvPlotsHits=(TCanvas*)fSegColl->FindObject(nameCanvHits);
  if( canvPlotsHits ) {
    delete canvPlotsHits;
    canvPlotsHits=0;
  }
  if( array ) {
    array->Delete();
    delete array;
  }
}

void HMdcEfficiencySec::creatPlots(HMdcLookUpTbSec* flookupTbSec) {
  if(flookupTbSec==0) return;
  Int_t nBinX=flookupTbSec->getNBinX();
  Int_t nBinY=flookupTbSec->getNBinY();
  Float_t xLow=flookupTbSec->getXlow();
  Float_t xUp=flookupTbSec->getXup();
  Float_t yLow=flookupTbSec->getYlow();
  Float_t yUp=flookupTbSec->getYup();
  Char_t buf[100];
  sprintf(buf,"S%iAllClust.",sector+1);
  plotHits[0]=new TH2F(buf,"",nBinX,xLow,xUp,nBinY,yLow,yUp);
  sprintf(buf,"S%iSelClust.",sector+1);
  plotHits[1]=new TH2F(buf,"",nBinX,xLow,xUp,nBinY,yLow,yUp);
  sprintf(buf,"S%i All clusters",sector+1);
  plotHits[0]->SetTitle(buf);
  sprintf(buf,"S%i Selected clusters",sector+1);
  plotHits[1]->SetTitle(buf);
  plotHits[0]->SetXTitle("x [mm]");
  plotHits[0]->SetYTitle("y [mm]      ");
  plotHits[1]->SetXTitle("x [mm]");
  plotHits[1]->SetYTitle("y [mm]      ");

}

void HMdcEfficiencySec::drawCanvProj(void) {
  if(gROOT->GetListOfCanvases()->FindObject(nameCanvProj)) return;
  canvPlotProj=new TCanvas(nameCanvProj,titleCanvProj,300,1,500,600);
  canvPlotProj->SetFillColor(10);
}

void HMdcEfficiencySec::delCanvProj(void){
  canvPlotProj=(TCanvas*)gROOT->GetListOfCanvases()->
      FindObject(nameCanvProj);
  if(canvPlotProj) {
    delete canvPlotProj;
    canvPlotProj=0;
  }
}

void HMdcEfficiencySec::drawCanvHits(void) {
  if(gROOT->GetListOfCanvases()->FindObject(nameCanvHits)) return;
  canvPlotsHits=new TCanvas(nameCanvHits,titleCanvHits,100,300,800,400);
  canvPlotsHits->Divide(2,1,0.0001,0.0001);
  canvPlotsHits->SetFillColor(10);
}

void HMdcEfficiencySec::saveCanvHits(UInt_t saveAs) {
  if(plotHits[0]==0 || plotHits[1]==0) return;
  TString nmCanv=nameCanvHits;
  nmCanv.Remove(0,fileName.Length()+1);
  TCanvas* canv=new TCanvas(nmCanv,titleCanvHits,100,300,800,400);
  canv->Divide(2,1,0.0001,0.0001);
  canv->SetFillColor(10);
  canv->cd(1);
  plotHits[0]->Draw();
  canv->cd(2);
  plotHits[1]->Draw();
  canv->Modified();
  canv->Update();
  if(saveAs==1&&HMdcEfficiency::getFilePointer()) {
      HMdcEfficiency::getFilePointer()->cd();
      canv->Write();
  }
  delete canv;
}
void HMdcEfficiencySec::delCanvHits(void){
  canvPlotsHits=(TCanvas*)gROOT->GetListOfCanvases()->
      FindObject(nameCanvHits);
  if(canvPlotsHits) {
    delete canvPlotsHits;
    canvPlotsHits=0;
  }
}

//------------------------------------------------

HMdcEfficiency::HMdcEfficiency() {
  initSecArray();
}

HMdcEfficiency::HMdcEfficiency(const Text_t *name,const Text_t *title)
                            : HReconstructor(name,title) {
  initSecArray();
}

void HMdcEfficiency::initSecArray(void) {
  fCalCat       = 0;
  fLookUpTb     = 0;
  iter          = 0;
  eventId       = 0;
  isCreated     = kFALSE;
  array         = new TObjArray(6);
  firstEvent    = kTRUE;
  canvasSummary = 0;
  histSummary   = 0;
  fRootOut      = 0;
  drawGoodClus  = kTRUE;
  gStyle->SetOptStat(0);
}

HMdcEfficiency::~HMdcEfficiency() {
  // destructor
  if( array ) {
    array->Delete();
    delete array;
  }
  canvEventProj=(TCanvas*)gROOT->GetListOfCanvases()->
      FindObject(nameCanvEventCurr);
  if( canvEventProj ) {
    delete canvEventProj;
    canvEventProj=0;
  }
  if(iter) {
    delete iter;
    iter = 0;
  }
  if(iterClus) {
    delete iterClus;
    iterClus = 0;
  }
}

Bool_t HMdcEfficiency::init(void) {
  fGetCont     = HMdcGetContainers::getObject();
  if( !fGetCont ) return kFALSE;
  fLookUpTb    = HMdcLookUpTb::getExObject();
  fSizesCells  = HMdcSizesCells::getObject();
  fcut         = HMdcTimeCut::getExObject();
  if(fcut && fcut->getIsUsed()) fcut=0;
  if(!fSizesCells) return kFALSE;

  fClusCat     = fGetCont->getCatMdcClus();
  if( !fClusCat ) return kFALSE;

  fCalCat      = fGetCont->getCatMdcCal1();
  isGeant      = HMdcGetContainers::isGeant();
  iter         = 0;
  if(fCalCat) iter=(HIterator *)fCalCat->MakeIterator("native");

  gROOT->cd();
  TCanvas c; // stupid workarround to fix the colors
  for(Int_t i=31; i<51; i++) {
    TColor* cl = gROOT->GetColor(i);
    if(cl){
	Float_t gr = (i-30)/20.;
	cl->SetRGB(1.-gr,gr,0.);
    } else {
	Error("init(void)","NULL pointer for color %i received!",i);
    }
  }
//   Int_t pal[12] = {17,16,15,14,13,12, 30,7,5,4,6,3};
//   gStyle->SetPalette(12,pal);
//   gStyle->SetPalette(1,NULL);

  loc.set(4,0,0,0,0);
  iterClus=(HIterator *)fClusCat->MakeIterator();
  
  nameCanvEvent     = "ProjPlEvent";
  nameCanvEventCurr = nameCanvEvent;
  titleCanvEvent    = "Cells projections";

  return kTRUE;
}

Bool_t HMdcEfficiency::reinit(void) {
  if( isCreated ) return kTRUE;
  if( fLookUpTb && !HMdcGetContainers::isInited(fLookUpTb) ) return kFALSE;
  if(!fSizesCells->initContainer()) return kFALSE;
  for (Short_t sec = 0; sec < 6; sec++) if( fGetCont->isSecActive(sec) ) {
    HMdcEfficiencySec* fsec = (HMdcEfficiencySec*)(*array)[sec];
    if( !fsec ) {
      fsec = new HMdcEfficiencySec(sec);
      (*array)[sec] = fsec;
      if(fLookUpTb) fsec->creatPlots(&(*fLookUpTb)[sec]);
    }
    if(fLookUpTb) {
      HMdcLookUpTbSec& fLUTbSec = (*fLookUpTb)[sec];
      if(!&fLUTbSec) return kFALSE;
    }
  }
  evHeader=gHades->getCurrentEvent()->getHeader();
  if(!evHeader) {
    Error("reinit","Can't get pointer to event header.");
    return kFALSE;
  }
  isCreated=kTRUE;
  return kTRUE;
}

Int_t HMdcEfficiency::execute(void) {
  if(firstEvent) extractFileInfo();
  eventId=evHeader->getEventSeqNumber();
  
  //--------Num. clusters have cell ...------------------------
  for(Short_t sec=0; sec<6; sec++) {
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!&fsec) continue;
    for(Short_t mod=0; mod<4; mod++) {
      HMdcEfficiencyMod& fmod=fsec[mod];
      if(!&fmod) continue;
      for(Short_t lay=0; lay<6; lay++) {
        HMdcEfficiencyLayer& flay=fmod[lay];
        memset(flay.getNClCell(),0,flay.getNumCells());
      }
    }
  }
  Int_t cellF[6];
  Int_t cellL[6];
  if(fCalCat) {
    iter->Reset();
    // Geant data ===========
    if(isGeant) {
      HMdcCal1Sim* cal=0;
      while ((cal=(HMdcCal1Sim *)iter->Next())!=0) {
        Int_t nHits=abs(cal->getNHits());
        if(nHits==0 || cal->getNHits()>2) continue;
        if(nHits==1 && cal->getStatus1()<0) continue;
        if(nHits==2 && cal->getStatus1()<0 && cal->getStatus2()<0) continue;
        Int_t s,m,l,c;
        cal->getAddress(s,m,l,c);
        if(&(*this)[s][m][l]) {
          (*this)[s][m][l].addCell(c);
          (*this)[s][m][l].addCellTot(c);
        }
        if( &(*this)[s][m] ) (*this)[s][m].fillTimesAll(cal,isGeant);
      }
    } else { // Real data ===========
      HMdcCal1* cal=0;
      while ((cal=(HMdcCal1 *)iter->Next())!=0) {
        Int_t s,m,l,c;
        if(cal->getNHits()==0 || cal->getNHits()>2) continue;
        cal->getAddress(s,m,l,c);
        HMdcEfficiencySec &fsec=(*this)[s];
        if( !&fsec ) continue;
        HMdcEfficiencyMod &fmod=fsec[m];
        if( !&fmod ) continue;
        HMdcEfficiencyLayer &flay=fmod[l];
        if(c<0 || c >flay.getNumCells()) continue;
        flay.addCellTot(c);
        fmod.fillTimesAll(cal,isGeant);
        if(fcut != NULL) {
          if(!fcut->cutComStop(cal)) continue;
          flay.addCellCutS(c);
          if(!fcut->cut(cal)) continue;	 // Cut drift time
        }
        fmod.fillTimesAfCut(cal,isGeant);
        flay.addCell(c);
      }
    }
  }
  //-----------------------------------------------------------
  iterClus->Reset();
  HMdcClus* fClus;
  while((fClus=(HMdcClus*)iterClus->Next())) {
    Int_t sec=fClus->getSec();
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!(&fsec)) continue;
    //===Selecting of clusters (tracks)===========================
    Int_t seg=fClus->getIOSeg();
    loc[0]=sec;
    for(Int_t m=0; m<2; m++) {
      Int_t nLayers=fClus->getNLayersMod(m);
      if(nLayers<=0) continue;
      loc[1]=m+seg*2;
      HMdcEfficiencyMod& fmod=fsec[loc[1]];
      if(!(&fmod)) continue;
      for(Int_t lay=0; lay<6; lay++) {
        Int_t clLay=lay+m*6;
        loc[2]=lay;
        loc[3]=-1;
        while((loc[3]=fClus->next(clLay,loc[3])) > -1) {
 	  Short_t cell=loc[3];
          if(fCalCat==0) {
            fmod[lay].addCell(cell);
            fmod[lay].addCellTot(cell);
          }
 	  fmod[lay].addCellInClust(cell);       // fired wires num.
          if(fmod[lay].getNumClCell(cell) == 2) { //==1 after Cal1 loop filling!
            fmod[lay].addCellClus(cell);
            HMdcCal1* cal=0;
            if(fCalCat) cal=(HMdcCal1*)fCalCat->getObject(loc);
            fmod.fillTimesCls(cal,isGeant);     //tim1,time2,time2-time1 hist.
          }
 	}
      }
    }
  }
  iterClus->Reset();
  while((fClus=(HMdcClus*)iterClus->Next())) {
    if(fClus->getStatus()==0) continue;   // For clusters filtering
    Int_t sec=fClus->getSec();
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!(&fsec)) continue;
    Int_t seg=fClus->getIOSeg();
    loc[0]=sec;
    Bool_t hitPloted=kFALSE;
    Float_t xCl=fClus->getX();
    Float_t yCl=fClus->getY();
    if(fLookUpTb) fsec.getPlotAllHits()->Fill(xCl,yCl);
    for(Int_t m=0; m<2; m++) {
      Bool_t qual=kFALSE;
      if(fClus->getMinCl(m) != 5) continue;
      Int_t nLayers=fClus->getNLayersMod(m);
      if(nLayers<5) continue;
      Int_t mod=m+seg*2;
      loc[1]=mod;
      HMdcEfficiencyMod& fmod=fsec[mod];
      if(!(&fmod)) continue;
      for(Int_t lay=0; lay<6; lay++) {
        qual=kFALSE;
 	HMdcEfficiencyLayer& flay=fmod[lay];
        Int_t clLay=lay+m*6;
        Int_t nCells=fClus->getNCells(clLay);
 	//----- Num. fired cells in plane >2
 	if(nCells>2) break;
 	//----- Num. fired cells in plane >0 & <=2
 	if(nCells>0){
          cellF[lay] = fClus->getFirstCell(clLay);
          cellL[lay] = fClus->getLastCell(clLay);
 	  // For dubl.hits |n.cell1-n.cell2| must be = 1
          if(cellL[lay]-cellF[lay]>1) break;
          //selecting of fired wires in one cluster only
          if(flay.getNumClCell(cellF[lay])>2 ||
             flay.getNumClCell(cellL[lay])>2)   break;
 	  // Nearests cells can not be fired
 	  if(cellF[lay]>0 && flay.getNumClCell(cellF[lay]-1)>0) break;
 	  if(cellL[lay]+1<flay.getNumCells() && flay.getNumClCell(cellL[lay]+1) >0) break;
          qual = kTRUE;
 	} else {
 	//----- N. fired cells in plane = 0
          HMdcSizesCellsLayer& fSCLayer = (*fSizesCells)[sec][mod][lay];
          Double_t xcoor,ycoor,zcoor;
          fSCLayer.calcIntersection(
              fClus->getXTarg(),fClus->getYTarg(),fClus->getZTarg(),
              xCl,yCl,fClus->getZ(), xcoor,ycoor,zcoor);
          fSCLayer.transTo(xcoor,ycoor,zcoor);
          cellF[lay]=fSCLayer.calcCellNum(xcoor,ycoor);
 	  if( cellF[lay] < 0 ) break;
 	  if(cellF[lay]>0 && flay.getNumClCell(cellF[lay]-1) >0) break;
 	  if(flay.getNumClCell(cellF[lay]) >0) break;
 	  if(cellF[lay]+1<flay.getNumCells() && flay.getNumClCell(cellF[lay]+1) >0) break;
          cellL[lay] = -1;
          qual       = kTRUE;
 	}
      }
      // Filling ========================================
      if(!qual) {
        fClus->setStatus(2); // Don't draw this cluster(???)
        continue;
      }
      Int_t bNum = fmod.getBinNum(xCl,yCl);
      if( bNum<0 && bNum!=-1000000) continue;
      //==================================================
      for(Int_t lay=0; lay<6; lay++) {
 	HMdcEfficiencyLayer& flay = fmod[lay];
        if(cellL[lay]<0) {
 	  fmod.add0Hit(bNum);
          flay.add0Hit(cellF[lay],bNum);
 	} else if(cellL[lay]-cellF[lay] == 0) {
 	  fmod.add1Hit(bNum);
 	  if(nLayers==6) flay.add1Hit(cellF[lay],bNum);
 	} else {
 	  fmod.add2Hit(bNum);
 	  if(nLayers==6) flay.add2Hit(cellF[lay],cellL[lay],bNum);
 	}
      }
      //==================================================
      if(!hitPloted && fLookUpTb) {
        hitPloted=kTRUE;
        fsec.getPlotSelHits()->Fill(xCl,yCl);
      }
    }
  }
  
   
  gSystem->ProcessEvents();
  
 
  return 0;
}

void HMdcEfficiency::drawProjPlotsSec(Int_t sec, Int_t type) {
  if(fLookUpTb==0 || ! &((*fLookUpTb)[sec]) ) return;
  if( ! &((*this)[sec]) ) return;
  TCanvas* canv=(*this)[sec].getCanvasProj();
  if( !canv ) return;
  canv->cd();
  canv->cd(1);
  drawProjPlots(sec, type);
  canv->Modified();
  canv->Update();
}

void HMdcEfficiency::drawPrPlEvent(Int_t type) {
  if(fLookUpTb==0) return;
  canvEventProj=(TCanvas*)gROOT->GetListOfCanvases()->FindObject(nameCanvEventCurr);
  if(!canvEventProj) return;
  canvEventProj->cd();
  Char_t buf[200];
  sprintf(buf,"%s%i",nameCanvEvent.Data(),eventId);
  nameCanvEventCurr = buf;
  Int_t tBit          = evHeader->getTBit() & 255;
  UInt_t downScalFlag = evHeader->getDownscalingFlag() & 1;
  sprintf(buf,"%s TBit=%i%i%i%i%i%i%i%i DownscalingFlag=%i",titleCanvEvent.Data(),
      (tBit>>7)&1,(tBit>>6)&1,(tBit>>5)&1,(tBit>>4)&1,
      (tBit>>3)&1,(tBit>>2)&1,(tBit>>1)&1,tBit&1,downScalFlag);
  canvEventProj->SetName(nameCanvEventCurr.Data());
  canvEventProj->SetTitle(buf);

  for(Int_t sec=0; sec<6; sec++) {
    if(type<0) type=typePrPlot[sec];
    else typePrPlot[sec]=type;
    if( ! &((*fLookUpTb)[sec]) ) continue;
    canvEventProj->cd(sec+1);
    drawProjPlots(sec, type);
  }
  canvEventProj->Update();
}

void HMdcEfficiency::drawCanvEvent(void) {
//  if(gROOT->GetListOfCanvases()->FindObject(nameCanvEventCurr)) return;
  canvEventProj=(TCanvas*)(gROOT->GetListOfCanvases()->
                           FindObject(nameCanvEventCurr.Data()));
  Char_t buf[200];
  sprintf(buf,"%s%i",nameCanvEvent.Data(),eventId);
  nameCanvEventCurr=buf;
  
  if(canvEventProj) {
    canvEventProj->SetName(nameCanvEventCurr.Data());
  } else {
    canvEventProj=new TCanvas(nameCanvEventCurr.Data(),titleCanvEvent.Data(),20,1,800,600);
    canvEventProj->SetFillColor(10);
    canvEventProj->Divide(3,2,0.0001,0.0001);
  }
}

void HMdcEfficiency::delCanvEvent(void){
  canvEventProj=(TCanvas*)gROOT->GetListOfCanvases()->
      FindObject(nameCanvEventCurr);
  if(canvEventProj) {
    delete canvEventProj;
    canvEventProj=0;
  }

}

void HMdcEfficiency::drawProjPlots(Int_t sec, Int_t type) {
  if(fLookUpTb==0) return;
  HMdcLookUpTbSec& lookutsec = (*fLookUpTb)[sec];
  Char_t name[15],title[20];
  sprintf(name,"S%iPlProj",sec+1);
  sprintf(title,"Event %i Sector %i",eventId,sec+1);
  typePrPlot[sec]=type;
  Int_t bining = 0; //???
  TH2C* plt = lookutsec.fillTH2C(name,title,typePrPlot[sec],bining);
  plt->SetXTitle("x [mm]");
  plt->SetYTitle("y [mm]      ");
  plt->SetMinimum(0.0);
  Int_t modMax=3;
  for(; modMax>=0; modMax--) if(lookutsec.getMaxClus(modMax)) break;
  Float_t mxBn=0;
  if(typePrPlot[sec]==1) for(Int_t mod=0; mod<=modMax; mod++) mxBn+=6*(mod+1);
  else if(typePrPlot[sec]==2) mxBn=4.01;
  else for(Int_t mod=0; mod<4; mod++) if(lookutsec.getMaxClus(mod)) mxBn+=6.0;
  plt->SetMaximum(mxBn);
  plt->SetLabelSize(0.03,"X");
  plt->SetLabelOffset(0.001,"X");
  plt->GetXaxis()->SetTitleSize(0.04);
  plt->SetLabelSize(0.03,"Y");
  plt->SetLabelOffset(0.01,"Y");
  plt->SetTitleOffset(1.2,"Y");
  plt->GetYaxis()->SetTitleSize(0.04);
  plt->Draw("COLZ");
  TMarker marker;
  marker.SetMarkerSize(1.4);
  marker.SetMarkerColor(4);
  Int_t mark[4]={24,28,26,25};
  iterClus->Reset();
  HMdcClus* fClus;
  while((fClus=(HMdcClus*)iterClus->Next())) {
    if(fClus->getSec() != sec) continue;
    if(drawGoodClus && fClus->getStatus() != 1)continue;
    Int_t mod=fClus->getMod();
    if(mod<0) {
      if(fClus->getIOSeg()==0) marker.SetMarkerStyle(24);
      else marker.SetMarkerStyle(30);
    } else marker.SetMarkerStyle(mark[mod]);
    marker.DrawMarker(fClus->getX(),fClus->getY());
  }
  drawTofShowerHits(sec);
}

void HMdcEfficiency::drawTofShowerHits(Int_t sec) {
  HEvent*    event      = gHades->getCurrentEvent();
  HCategory* fCatShower = event->getCategory(catShowerHitTof);
  if(fCatShower==0) {
    fCatShower = event->getCategory(catShowerHitTofTrack);
    if(fCatShower==0) return;
  }
  HCategory* fCatTofCluster = event->getCategory(catTofCluster);
  HCategory* fCatTofHit     = event->getCategory(catTofHit);
  if(fCatTofCluster==0 && fCatTofHit==0) return;
  
  HMdcSizesCellsSec& fSCSec = (*fSizesCells)[sec];
  const HGeomTransform* secLabTrans = fSCSec.getLabTrans();
  HMdcLookUpTbSec& lookutsec = (*fLookUpTb)[sec];
  const HMdcPlane& projPlane = lookutsec.getPrPlane();
  HGeomVector target = lookutsec.getTargetF()+lookutsec.getTargetL();
  target *= 0.5;
  
  TMarker marker;
  marker.SetMarkerSize(1.6);
  marker.SetMarkerColor(3);
  marker.SetMarkerStyle(29);
  
  HGeomVector dir;
  HGeomVector cross;
  
  HIterator* iterShower = (HIterator*)fCatShower->MakeIterator();
  iterShower->Reset();
  HShowerHitTof *pShowerHitTof;
  while((pShowerHitTof=(HShowerHitTof*)(iterShower->Next()))!=0) {
    if(sec != pShowerHitTof->getSector()) continue;
    
    Float_t xSh,ySh,zSh;
    pShowerHitTof->getLabXYZ(&xSh, &ySh, &zSh);
    dir.setXYZ(xSh,ySh,zSh);
    dir  = secLabTrans->transTo(dir);
    dir -= target;
    projPlane.calcIntersection(target,dir,cross);
    marker.DrawMarker(cross(0),cross(1));
  }
  delete iterShower;

  if(fCatTofHit) {
    HIterator*  iterTofHit= (HIterator*)fCatTofHit->MakeIterator();
    iterTofHit->Reset();
    HTofHit* pTofHit;
    while((pTofHit=(HTofHit*)(iterTofHit->Next()))!=0 ) {
      if(sec != pTofHit->getSector()) continue;
      Float_t xTf,yTf,zTf;
      pTofHit->getXYZLab(xTf,yTf,zTf);
      dir.setXYZ(xTf,yTf,zTf);
      dir  = secLabTrans->transTo(dir);
      dir -= target;
      projPlane.calcIntersection(target,dir,cross);
      marker.DrawMarker(cross(0),cross(1));
    }
    delete iterTofHit;
  } else {
    HIterator*  iterTofCluster= (HIterator*)fCatTofCluster->MakeIterator();
    iterTofCluster->Reset();
    HTofCluster* pTofCluster;
    while((pTofCluster=(HTofCluster*)(iterTofCluster->Next()))!=0 ) {
      if(pTofCluster->getClusterSize()>2) continue;
      if(sec != pTofCluster->getSector()) continue;
      Float_t xTf,yTf,zTf;
      pTofCluster->getXYZLab(xTf,yTf,zTf);
      dir.setXYZ(xTf,yTf,zTf);
      dir  = secLabTrans->transTo(dir);
      dir -= target;
      projPlane.calcIntersection(target,dir,cross);
      marker.DrawMarker(cross(0),cross(1));
    }
    delete iterTofCluster;
  }
/*
  marker.SetMarkerSize(2.3);
  marker.SetMarkerColor(38);
  marker.SetMarkerStyle(4);
  HCategory* fCatRichIPU=event->getCategory(catHardRichHit);
  if(fCatRichIPU==0) return;
  HIterator* iterRichIPU=(HIterator*)fCatRichIPU->MakeIterator();
  HRichHitIPU* pRichHitIPU;
  iterRichIPU->Reset();
  while((pRichHitIPU=(HRichHitIPU*)(iterRichIPU->Next()))!=0) {
    if(sec != pRichHitIPU->getSector()) continue;
    Double_t theta = pRichHitIPU->getTheta()*TMath::DegToRad();
    Double_t phi   = pRichHitIPU->getPhi();
    if(sec<5) phi -= sec*60.; 
    else phi += 60.;
    phi *= TMath::DegToRad();    
    dir.setZ(TMath::Cos(theta));
    Double_t dXY=TMath::Sqrt(1.-dir(2)*dir(2));
    dir.setX(TMath::Cos(phi)*dXY);
    dir.setY(TMath::Sin(phi)*dXY);
    projPlane.calcIntersection(target,dir,cross);
    marker.DrawMarker(cross(0),cross(1));
  }
  delete iterRichIPU;
*/
}


void HMdcEfficiency::drawHitsPlots(Int_t sec) {
  if(fLookUpTb==0) return;
  if( ! &((*fLookUpTb)[sec]) ) return;
  if( ! &((*this)[sec]) ) return;
  TCanvas* canv=(*this)[sec].getCanvasHits();
  if(canv) {
    canv->cd(1);
    (*this)[sec].getPlotAllHits()->Draw();
    canv->cd(2);
    (*this)[sec].getPlotSelHits()->Draw();
    canv->Modified();
    canv->Update();
  }
}


void HMdcEfficiency::printClusters(Int_t sec) {
  iterClus->Reset();
  HMdcClus* fClus;
  while((fClus=(HMdcClus*)iterClus->Next())) if(fClus->getSec()==sec) {
      if(!drawGoodClus || fClus->getStatus()==1) fClus->print();
  }
}

void HMdcEfficiency::calcEfficiency(Bool_t flag) {
  gStyle->SetTitleX(0.25);
  gStyle->SetTitleW(0.5);
  gStyle->SetTitleH(0.09);
  for(Int_t sec=0; sec<6; sec++){
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!&fsec) continue;
    if(flag) {
      for(Int_t mod=0; mod<4; mod++){
        HMdcEfficiencyMod& fmod=fsec[mod];
        if(!&fmod) continue;
        for(Int_t lay=0; lay<6; lay++) {
          fmod[lay].fillHists();
          fmod[lay].updateCanv();
          fmod[lay].updateCanvXYlist();
        }
        fmod.fillHistMb();
        fmod.updateCanv();
      }
    }
    drawProjPlotsSec(sec,typePrPlot[sec]);
    drawHitsPlots(sec);
  }
  drawPrPlEvent();
  canvasSummary = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("EffSumm");
  if(canvasSummary) drawSummary(kTRUE);
}

Bool_t HMdcEfficiency::finalize(void) {
  // writeHists();
  return kTRUE;
}

void HMdcEfficiency::extractFileInfo(void) {
  firstEvent=kFALSE;
  HMdcGetContainers::getFileName(fileName);
  TString runId;
  HMdcGetContainers::getRunId(runId);
  TString date;
  HMdcGetContainers::getEventDate(date);
  TString time;
  HMdcGetContainers::getEventTime(time);
  Char_t buf[200];
  sprintf(buf,"%s   runId:%s  %s  %s",
      fileName.Data(),runId.Data(),date.Data(),time.Data());
  fileId=buf;
}

void HMdcEfficiency::saveHist(const TString& outputDir) {
  fileName=outputDir;
  fileName.Remove(TString::kTrailing, '/');
  fileName+="/MDC";
  saveHist();
}

void HMdcEfficiency::saveHist(void) {
  calcEfficiency();
  Char_t buf[200];
  sprintf(buf,"%s_MdcEff.root",fileName.Data());
  fRootOut = new TFile(buf,"RECREATE");
  
  gROOT->SetBatch();
  for(Int_t sec=0; sec<6; sec++){
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!&fsec) continue;
    fsec.saveCanvHits(1);
    for(Int_t mod=0; mod<4; mod++){
      HMdcEfficiencyMod& fmod=fsec[mod];
      if(!&fmod) continue;
      fmod.saveCanvases(1);
      fmod.saveCanvEffMb(1);
      if(fCalCat) fmod.drawCanvTimes(-1,1);
      
//      fmod.fillHistMb();
//      fmod.updateCanv();
    }
  }
  saveAs((Char_t*)fileName.Data(),"Pattern");
  saveAs((Char_t*)fileName.Data(),"Ratio");
  
  saveAs((Char_t*)fileName.Data(),"Eff");
  if(fCalCat) saveAs((Char_t*)fileName.Data(),"Times");
  saveAs((Char_t*)fileName.Data(),"EffMb");
  
  drawSummary(kTRUE);
  fRootOut->cd();
  Char_t gifFile[500];
  
  sprintf(gifFile,"%s_%s.gif",fileName.Data(),"Summary");
//  canvasSummary->SaveAs(gifFile);
  canvasSummary->Print(gifFile);
  
//   sprintf(gifFile,"%s_Summary.eps",fileName.Data());
//   canvasSummary->SaveAs(gifFile);
//   Char_t command[500];
//   sprintf(command,"convert %s %s_Summary.gif",gifFile,fileName.Data());
//   gSystem->Exec(command);
//   sprintf(command,"rm %s",gifFile);
//   gSystem->Exec(command);

  canvasSummary->Write();
  
  gROOT->SetBatch(kFALSE);
  fRootOut->Close();
  delete fRootOut;
  fRootOut = 0;
}

void HMdcEfficiency::saveAs(const Char_t* fileName,const Char_t* type) {
  Char_t psFileOp[500];
  sprintf(psFileOp,"%s_%s.pdf(",fileName,type);
  Char_t psFile[500];
  sprintf(psFile,"%s_%s.pdf",fileName,type);
  Char_t psFileCl[500];
  sprintf(psFileCl,"%s_%s.pdf)",fileName,type);
  
  Char_t buf[100];
  TCanvas* cn=0;
  Int_t nCn=0;
  for(Int_t m=0;m<4;m++) {
    for(Int_t s=0;s<6;s++) {
      sprintf(buf,"S%iM%i_%s",s+1,m+1,type);
      TCanvas* cnNext=(TCanvas*)gROOT->FindObject(buf);
      if(cnNext) {
        if(nCn==1)     cn->Print(psFileOp);
        else if(nCn>1) cn->Print(psFile);
        nCn++;
        cn=cnNext;
      }
    }
  }
  if(nCn==1)     cn->Print(psFile);
  else if(nCn>1) cn->Print(psFileCl);
//  if(nCn>0) makePDF(psFile);
  return;
}

void HMdcEfficiency::savePDF(const Char_t* fileName,const Char_t* type) {
  // type = "Pattern","Eff","EffMb","NtrNhits","SingC","Prob","Times"
  gROOT->SetBatch();
  TString rootFile(fileName);
  rootFile += ".root";
  TFile* file=TFile::Open(rootFile.Data(),"READ");
  if(file) {
    saveAs(fileName,type);
    file->Close();
    delete file;
  } else printf("\nFile %s.root don't exist!\n\n",fileName);
  gROOT->SetBatch(kFALSE);
  return;
}

void HMdcEfficiency::makePDF(const Char_t* psFile) {
  // Execute ps2pdf GhostScript command on closed PS file, delete PS file 
  Char_t command[500];
  sprintf(command,"ps2pdf %s",psFile);
  gSystem->Exec(command);
  sprintf(command,"rm %s",psFile);
  gSystem->Exec(command);
}

void HMdcEfficiency::drawSummary(Bool_t flag) {
  canvasSummary = (TCanvas*)gROOT->GetListOfCanvases()->FindObject("EffSumm");
  if(!flag) {
    if(canvasSummary) {
      delete canvasSummary;
      canvasSummary = 0;
    }
    return;
  }
  gROOT->cd();
  if(histSummary==0) {
    histSummary = new TH1F("EffSumm",
      "Mdc Efficiency Summary; Chamber;<Layer Efficiency>",144,0,24);
    const Char_t* xLab[6][4]={{"I1","II1","III1","IV1"},
                              {"I2","II2","III2","IV2"},
                              {"I3","II3","III3","IV3"},
                              {"I4","II4","III4","IV4"},
                              {"I5","II5","III5","IV5"},
                              {"I6","II6","III6","IV6"}};
    TAxis* ax = histSummary->GetXaxis();
    ax->SetNdivisions(524);
    for(Int_t m=0;m<4;m++) for(Int_t s=0;s<6;s++) ax->SetBinLabel(s*6+m*36+3,xLab[s][m]);
  }
  
  if(canvasSummary==0) {
    canvasSummary = new TCanvas("EffSumm","Mdc Efficiency Summary", 840,630);
    canvasSummary->cd();
    canvasSummary->SetBorderSize(2);
    canvasSummary->SetFillColor(0);
    canvasSummary->SetFrameFillColor(0);
    canvasSummary->SetFrameBorderMode(0);
    canvasSummary->SetGridx();
    canvasSummary->SetGridy();
  }
  Double_t effMod[6][4];
  Int_t    nWires[6][4];
  for(Int_t mod=0;mod<4;mod++) for(Int_t sec=0;sec<6;sec++) {
    effMod[sec][mod]=0.;
    nWires[sec][mod]=0;
  }
  for(Int_t mod=0;mod<4;mod++) for(Int_t sec=0;sec<6;sec++) {
    HMdcEfficiencySec& fsec=(*this)[sec];
    if(!&fsec) continue;
    HMdcEfficiencyMod& fmod=fsec[mod];
    if(!&fmod) continue;
    for(Int_t lay=0;lay<6;lay++) {
      HMdcEfficiencyLayer& flay=fmod[lay];
      if(!&flay) continue;
      TH1F* hsEff=flay.getHist(6);
      TH1F* hsTrk=flay.getHist(2);
      TH1F* hsPat=flay.getHist(0);
      if(hsEff==0 || hsTrk==0) continue;
      Double_t effSum  = 0.;
      Double_t effSum2 = 0.;
      Int_t    sen     = 0;
      Int_t nbins = flay.getNumCells();
      for(Int_t b=5;b<=nbins-10;b++) {
        if(hsTrk->GetBinContent(b) < 50.) continue;
        if(hsPat->GetBinContent(b) < 50.) continue;
        Double_t err = hsEff->GetBinError(b);
        if(err==0.) continue;
if(err>0.2) continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        Double_t vl = hsEff->GetBinContent(b);
//        if(vl<0.2) continue;
if(vl<0.8) continue; //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
        effSum  += vl;
        effSum2 += vl*vl;
        sen++;
        effMod[sec][mod] += vl;
        nWires[sec][mod]++;
      }
      if(sen>0) {
        Int_t bin = sec*6+mod*36+lay+1;
        Double_t effL = effSum/sen;
        histSummary->SetBinContent(bin,effL);
        Double_t err  = TMath::Sqrt( (effSum2/sen-effL*effL)/sen );
        histSummary->SetBinError(bin,err);
      }      
    }
  }
  canvasSummary->cd();
  histSummary->SetMinimum(0.5);
  histSummary->SetMaximum(1.02);
  histSummary->GetYaxis()->SetNdivisions(511);
  histSummary->Draw();
  Int_t color[4]={2,8,4,6};
  for(Int_t mod=0;mod<4;mod++) for(Int_t sec=0;sec<6;sec++) {
    if(nWires[sec][mod]==0) continue;
    effMod[sec][mod] /= nWires[sec][mod];
    TLine* ln= new TLine();
    ln->SetLineColor(color[mod]);
    ln->SetLineWidth(2);
    ln->DrawLine(sec+mod*6,effMod[sec][mod],sec+mod*6+1,effMod[sec][mod]);
  }
  canvasSummary->Modified();
  canvasSummary->Update();
  TPaveText* pTitle = (TPaveText*)(canvasSummary->FindObject("title"));
  if(pTitle != NULL) pTitle->SetFillColor(0);
  canvasSummary->Modified();
  canvasSummary->Update();
}
