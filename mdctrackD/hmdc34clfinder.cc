#include "hmdc34clfinder.h"
#include "hades.h"
#include "hmatrixcategory.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hmdctrackddef.h"
#include "hmdcgeomobj.h"
#include "hmdcgetcontainers.h"
#include "hmdcclussim.h"
#include "hmdccluster.h"
#include "hmdcseg.h"
#include "TH2.h"
#include "TMath.h"
#include "hgeomtransform.h"
#include "hmdcgeomstruct.h"
#include "hspecgeompar.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hmdclayergeompar.h"
#include "hmdcsizescells.h"
#include "hmdcgeompar.h"
#include "hmdcclfnstack.h"
#include "hmdctrackdset.h"
#include "hmdcgeanttrack.h"
#include "hmdcdrifttimepar.h"
#include "hmdcclusmetamatch.h"
#include "hmdctrackparam.h"
#include "hmdckickcor.h"

#include <stdlib.h>
#include <iostream>
#include <iomanip>

using namespace std;

using namespace TMath;

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 19/05/2010 by O. Pechenova
//*-- Modified : 25/02/2010 by O. Pechenova
//*-- Modified : 07/02/2003 by V. Pechenov
//*-- Modified : 05/06/2002 by V. Pechenov
//*-- Modified : 10/05/2001 by V. Pechenov
//*-- Modified : 07/03/2001 by V. Pechenov
//*-- Modified : 12/07/2000 by V.Pechenov

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
// HMdc34ClFinder
//
// Track finder for outer segment MDC:
// Constructor:
//
////////////////////////////////////////////////////////////////

ClassImp(HMdcProjPlot)
ClassImp(HMdc34ClFinderLayer)
ClassImp(HMdc34ClFinderMod)
ClassImp(HMdc34ClFinderSec)
ClassImp(HMdc34ClFinder)

UChar_t* HMdcProjPlot::weights=0;
UChar_t* HMdcProjPlot::weights3=0;
UChar_t* HMdcProjPlot::weights4=0;
Int_t    HMdcProjPlot::wtArrSize=0;
UChar_t  HMdcProjPlot::nBitsLuTb[64] = {
    0,1,1,2,1,2,2,3,1,2,2,3,2,3,3,4,1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,
    1,2,2,3,2,3,3,4,2,3,3,4,3,4,4,5,2,3,3,4,3,4,4,5,3,4,4,5,4,5,5,6};

HMdcProjPlot::HMdcProjPlot(UChar_t mSeg, Int_t inBinX, Int_t inBinY) {
  nBinX = inBinX;
  nBinY = inBinY;
  size  = nBinX*nBinY;
  size  = (size/32 + ((size%32 > 0) ? 1:0))*32;
  xMinL = new Short_t [nBinY];
  xMaxL = new Short_t [nBinY];
  yMinL = new Short_t [nBinX];
  rootPlot   = 0;
  if(size>wtArrSize) {
    if(weights) delete [] weights;
    weights   = new UChar_t [size*2];
    weights3  = weights;
    weights4  = weights+size;
    wtArrSize = size;
  }
  xBinsPos = new Double_t [nBinX];
  yBinsPos = new Double_t [nBinY];
  memset(weights,0,wtArrSize*2);
}

HMdcProjPlot::~HMdcProjPlot() {
  if(xMinL) delete [] xMinL;
  if(xMaxL) delete [] xMaxL;
  if(yMinL) delete [] yMinL;
  xMinL = xMaxL= yMinL = 0;
  if(rootPlot) {
    delete rootPlot;
    rootPlot = 0;
  }
  if(weights) {
    delete [] weights;
    weights   = 0;
    weights3  = 0;
    weights4  = 0;
    wtArrSize = 0;
  }
  if(xBinsPos) delete [] xBinsPos;
  if(yBinsPos) delete [] yBinsPos;
  xBinsPos = 0;
  yBinsPos = 0;
}

void HMdcProjPlot::setEdges(Double_t iyMin, Double_t ixMinD, Double_t ixMaxD,
                            Double_t iyMax, Double_t ixMin,  Double_t ixMax) {
  stY     = (iyMax-iyMin)/(nBinY-2);
  stX     = (ixMax-ixMin)/(nBinX-2);
  yMin    = iyMin-stY;
  yMax    = iyMax+stY;
  xMin    = ixMin-stX;
  xMax    = ixMax+stX;
  xMinD   = ixMinD;
  xMaxD   = ixMaxD;
  Double_t aL23 = (xMinD-xMin)/(yMin-yMax);
  Double_t aL01 = (xMaxD-xMax)/(yMin-yMax);
  for(Int_t ny=0; ny<nBinY; ny++) {
    Double_t yl = (ny+1)*stY+yMin;
    xMinL[ny]  = Short_t((((yl-yMax)*aL23+xMin)-xMin)/stX);
    if(xMinL[ny] < 1) xMinL[ny]=1;
    xMaxL[ny]  = Short_t((((yl-yMax)*aL01+xMax)-xMin)/stX);
    if(xMaxL[ny] > nBinX-2) xMaxL[ny]=nBinX-2;
  }

  for(Int_t nx=0; nx<nBinX; nx++) {
    yMinL[nx]=nBinY;
    for(Int_t ny=nBinY-2; ny>=0; ny--) {
      if(nx<xMinL[ny] || nx>xMaxL[ny]) break;
      yMinL[nx] = ny==0 ? 1 : ny;
    }
  }
  xFirstBin = 0.5*stX+xMin;            // (stX/2)^2
  yFirstBin = 0.5*stY+yMin;            // (stY/2)^2
  for(Int_t n=0; n<nBinX; n++) xBinsPos[n]=(((Double_t)n)+0.5)*stX+xMin;
  for(Int_t n=0; n<nBinY; n++) yBinsPos[n]=(((Double_t)n)+0.5)*stY+yMin;
}

void HMdcProjPlot::print(void) {
  printf("----- Project plane: -----\n");
  HMdcPlane::print();
  printf(" Sizes: nBinX=%i, nBinY=%i, stepX=%g, stepY=%g\n",nBinX,nBinY,stX,stY);
  printf(" Points (x:y) n. 0=(%.1f:%.1f)  1=(%.1f:%.1f)\n",xMaxD,yMin,xMax,yMax);
  printf("                 2=(%.1f:%.1f)  3=(%.1f:%.1f)\n",xMin,yMax,xMinD,yMin);
}


TH2C* HMdcProjPlot::getPlot(Char_t* name, Char_t* title){
  if(!rootPlot) rootPlot=new TH2C(name,title,nBinX,xMin,xMax,nBinY,yMin,yMax);
  else {
    rootPlot->Reset();
    rootPlot->SetName(name);
    rootPlot->SetTitle(title);
  }
  rootPlot->SetMaximum(12.);
  rootPlot->SetMinimum(0.);
  rootPlot->Fill(0.,0.,0);
  for (Int_t nBin=0; nBin<size; nBin++) {
    UChar_t amp = nBitsLuTb[weights3[nBin]] + nBitsLuTb[weights4[nBin]];
    if(amp != 0) rootPlot->Fill(xBinsPos[nBin%nBinX],yBinsPos[nBin/nBinX],(Stat_t)amp);
  }
  return rootPlot;
}

//----------Layer-----------------------------

HMdc34ClFinderLayer::HMdc34ClFinderLayer(Int_t sec, Int_t mod, Int_t lay) {
  HMdcGetContainers* fGetCont=HMdcGetContainers::getObject();
  if( !fGetCont ) return;
  HMdcGeomStruct* pMdc=fGetCont->getMdcGeomStruct();
  if( !pMdc ) return;
  nCells        = ((*pMdc)[sec][mod])[lay];
  yBin          = NULL;
  nBinX         = 0;
  xBin1         = NULL;
  xBin2         = NULL;
  cells         = NULL;
  module        = mod;
  layer         = lay;
  nextPartFCell = 777;
  layerNextPart = NULL;
  layerPart     = 0;
  yCross        = -1000.;
}

HMdc34ClFinderLayer::~HMdc34ClFinderLayer() {
  if(yBin != NULL)  delete [] yBin;
  if(xBin1 != NULL) delete [] xBin1;
  if(xBin2 != NULL) delete [] xBin2;
  yBin  = NULL;
  xBin1 = NULL;
  xBin2 = NULL;
  nBinX = 0;
  layerNextPart = NULL;
  if(layerNextPart != NULL) delete layerNextPart;
}

HMdc34ClFinderLayer::HMdc34ClFinderLayer(HMdc34ClFinderLayer& prevPart):TObject(prevPart) {
  // copy constructor
  nCells        = prevPart.nCells;
  yBin          = NULL;
  nBinX         = 0;
  xBin1         = NULL;
  xBin2         = NULL;
  cells         = prevPart.cells;
  module        = prevPart.module;
  layer         = prevPart.layer;
  nextPartFCell = 777;
  layerNextPart = NULL;
  layerPart     = prevPart.layerPart + 1;
}

HMdc34ClFinderLayer* HMdc34ClFinderLayer::nextLayerPart(Int_t nPartFCell)  {
  if(nPartFCell<0 || nPartFCell >= nCells) return NULL;
  nextPartFCell = nPartFCell;
  layerNextPart = new HMdc34ClFinderLayer(*this);
  return layerNextPart;
}

Bool_t HMdc34ClFinderLayer::createArrayBins(Short_t nBins) {
  if(nBinX == nBins) return kTRUE;
  if(yBin) delete [] yBin;
  nBinX = nBins;
  yBin  = new Short_t [nBins];
  if(yBin==0) return kFALSE;
  return kTRUE;
}

//------------Module----------------------------
HMdc34ClFinderMod::HMdc34ClFinderMod(Int_t sec, Int_t mod) {
  // constructor creates an array of pointers of type HMdc34ClFinderLayer
  //
  array = new TObjArray(6);
  for (Int_t layer = 0; layer < 6; layer++) {
    (*array)[layer] = new HMdc34ClFinderLayer(sec, mod, layer);
  }
}

HMdc34ClFinderMod::~HMdc34ClFinderMod() {
  // destructor
  if(array) {
    array->Delete();
    delete array;
  }
}

Int_t HMdc34ClFinderMod::getNCells(void) {
  Int_t nHits = 0;
  for(Int_t lay=0; lay<6; lay++) nHits += (*this)[lay].cells->getNCells();
  return nHits;
}

//----------Sector------------------------------------

HMdc34ClFinderSec::HMdc34ClFinderSec(Int_t sec, Int_t inBinX, Int_t inBinY) {
  // constructor creates an array of pointers of type HMdc34ClFinderMod
  //
  memset(pLayPar,0,sizeof(pLayPar));
  regInd = 0;
  sector = sec;
  for(Int_t m=0; m<4; m++) lMods[m] = HMdcGetContainers::getObject()->isModActive(sec,m) ? 1:0;
  mSeg[0] = (lMods[0] ? 1:0) + (lMods[1] ? 2:0);
  mSeg[1] = (lMods[2] ? 1:0) + (lMods[3] ? 2:0);
  array = new TObjArray(4);
  typeClFinder = 0;
  for (Int_t m=2; m<4; m++) if(lMods[m]) (*array)[m] = new HMdc34ClFinderMod(sector,m);
  prPlotSeg2 = 0;
  if(mSeg[1] > 0) {
    prPlotSeg2    = new HMdcProjPlot(mSeg[1],inBinX,inBinY);
    nBinX         = prPlotSeg2->nBinX;
    nearbyBins[0] = -1;
    nearbyBins[1] = +1;
    nearbyBins[2] =   -nBinX;
    nearbyBins[3] =   +nBinX;
    nearbyBins[4] = -1-nBinX;
    nearbyBins[5] = -1+nBinX;
    nearbyBins[6] =  1-nBinX;
    nearbyBins[7] =  1+nBinX;
    nBinYM2       = prPlotSeg2->nBinY-2;
  }
  isGeant = HMdcGetContainers::isGeant();

  locClus.set(2,sector,1);
  xCMin         = new Int_t [prPlotSeg2->nBinY];
  xCMax         = new Int_t [prPlotSeg2->nBinY];
  for(Int_t y=0;y<prPlotSeg2->nBinY;y++) {
    xCMin[y] = nBinX;
    xCMax[y] = -1;
  }

  pClustersArrs = HMdcClustersArrs::getObject();
  clusArr       = pClustersArrs->getArray1();
  if(clusArr == 0) clusArr = pClustersArrs->createArray1(1000); // Array size
  clusArrSize   = pClustersArrs->getArray1Size();

  pDriftTimeParSec = HMdcDriftTimePar::getObject()->at(sector);
  dDistCut      = 3.5;
  useDriftTime  = kFALSE; // kTRUE ?
  pMetaMatch    = NULL;
  useDxDyCut    = 0; //kFALSE;
  dDistYCorr    = 0.;        //???????????????
  pKickCor      = NULL;
  
  fakeSuppFlag  = HMdcTrackDSet::getGhostRemovingParamSeg2(wLev,wBin,wLay,dWtCut);
  useDriftTime  = HMdcTrackDSet::useDriftTimeSeg2();
  HMdcTrackDSet::getDrTimeProjParSeg2(dDistCut,dDistYCorr,dDCutCorr);
  
  for(Int_t nr=0;nr<36;nr++) cutXBins[nr] = NULL;
  
  bitsSetLay[0]   = 0x01010101;
  bitsSetLay[1]   = 0x02020202;
  bitsSetLay[2]   = 0x04040404;
  bitsSetLay[3]   = 0x08080808;
  bitsSetLay[4]   = 0x10101010;
  bitsSetLay[5]   = 0x20202020;
  oneArrLay[0][0] = 0x00000000;
  oneArrLay[0][1] = 0x00000001;
  oneArrLay[0][2] = 0x00000101;
  oneArrLay[0][3] = 0x00010101;
  for(Int_t lay=1;lay<6;lay++) for(Int_t i=0;i<4;i++) oneArrLay[lay][i] = oneArrLay[0][i]<<lay;
}

HMdc34ClFinderSec::~HMdc34ClFinderSec() {
  // destructor
  if(array) {
    array->Delete();
    delete array;
  }
  if(prPlotSeg2) delete prPlotSeg2;
  prPlotSeg2 = 0;

  if(xCMin) delete [] xCMin;
  xCMin = 0;
  if(xCMax) delete [] xCMax;
  xCMax = 0;
  
  for(Int_t nr=0;nr<36;nr++) {
    if(cutXBins[nr] != NULL) delete [] cutXBins[nr];
    cutXBins[nr] = NULL;
  }
}

void HMdc34ClFinderSec::clear(void) {
  maxAmp[2]    = -1;
  maxAmp[3]    = -1;
  notEnoughWrs = kTRUE;
  if(isGeant) {
    HMdcGeantEvent* pGEvent = HMdcGeantEvent::getExObject();
    if(pGEvent) pGEvent->clearOSeg();
  }
}

void HMdc34ClFinderSec::setMinBin(Int_t *mBin) {
  for(Int_t i=0;i<4;i++) minAmp[i]=mBin[i];
  notEnoughWrs = testMaxAmp();
}

Bool_t HMdc34ClFinderSec::testMaxAmp(void) {
  // Calculating checking max.amplitude
  for(Int_t mod=2;mod<4;mod++) {
    maxAmp[mod]=0;
    if(minAmp[mod]<=0) continue;
    maxAmp[mod] += (*pListCells)[mod].getNLayers();
  }
  if(minAmp[2]>0 && minAmp[3]>0) {
    if(minAmp[2]+minAmp[3] > maxAmp[2]+maxAmp[3]) return kTRUE; // Sum.Amp. cut
//  For mixed clusters:
//    if(minAmp[2]>maxAmp[2] || minAmp[3]>maxAmp[3]) return kTRUE;
  } else {
    if(minAmp[2]>maxAmp[2]) return kTRUE;
    if(minAmp[3]>maxAmp[3]) return kTRUE;
  }
  return kFALSE;
}

// Int_t HMdc34ClFinderSec::findClustersSeg2(HMdcSeg* fSeg, HMdcClus* pClus,Int_t *mBin){
//   if(mBin!=0) setMinBin(mBin);
//   if(notEnoughWrs) return 0;
// 
// //   errZ0 *= 2.; //3.;
//   fkick->calcSegIntersec(fSeg,seg1,dirSeg1,segOnKick,segRegOnKick);
// 
//   indexPar = pClus->getOwnIndex();
//   indexFCh = -1;
//   indexLCh = -2;
//   Int_t nClustCh = findClustersSeg2();
//   if(nClustCh>0) pClus->setIndexChilds(indexFCh,indexLCh);
//   return nClustCh;
// }

Int_t HMdc34ClFinderSec::findClustersSeg2(HMdcTrackParam *tSeg1, HMdcClus* pClus,Int_t *mBin) {
  // Fixed "errors" for calc. region on the kick plane
  if(mBin!=0) setMinBin(mBin);
  if(notEnoughWrs) return 0;
  
  Double_t  ye   = 0.6;  // Parameter
  Double_t  xe   = 1.5;  // Parameter

  HGeomVector p2;
  tSeg1->getFisrtPoint().getVector(seg1);
  tSeg1->getSecondPoint().getVector(p2);
  dirSeg1        = tSeg1->getDir();

  Double_t  x0mE = seg1.getX() - xe;
  Double_t  x0pE = seg1.getX() + xe;
  Double_t  y0mE = seg1.getY() - ye;
  Double_t  y0pE = seg1.getY() + ye;

  Double_t  xPmE = p2.getX() - xe;
  Double_t  xPpE = p2.getX() + xe;
  Double_t  yPmE = p2.getY() - ye;
  Double_t  yPpE = p2.getY() + ye;
  
  if(pKickCor != NULL && lMods[3] != 0) { // For track curvature correction. MDCIV only!
    pKickCor->interpolateAngles(tSeg1->getThetaRad(),tSeg1->getPhiRad());
    HGeomVector crossPnt;
    HGeomVector pnt;
    HMdc34ClFinderMod &pMod = (*this)[3];
    for(Int_t l=0;l<6;l++) {
      HMdc34ClFinderLayer &pLay = pMod[l];
      pLay.pSCLay->calcIntersection(seg1,dirSeg1,crossPnt);
      pnt = pLay.pSCLay->getRotLayP1SysRSec()->transTo(crossPnt);    // can be optimized
      pLay.yCross = pnt.getY();
      if(pLay.pSCLay->getLayerNParts() > 1) {
        pnt = pLay.pSCLay->getRotLayP2SysRSec()->transTo(crossPnt);  // can be optimized
        pLay.layerNextPart->yCross = pnt.getY();
      }
    }
  }

  fkick->calcIntersection(seg1,dirSeg1,segOnKick);
  HMdcPlane *pl1 = tSeg1->getFirstPlane();
  HMdcPlane *pl2 = tSeg1->getSecondPlane();
  fkick->calcIntersection(x0mE,y0mE,pl1->getZOnPlane(x0mE,y0mE),xPpE,yPpE,pl2->getZOnPlane(xPpE,yPpE),segRegOnKick[0]);
  fkick->calcIntersection(x0pE,y0mE,pl1->getZOnPlane(x0pE,y0mE),xPmE,yPpE,pl2->getZOnPlane(xPmE,yPpE),segRegOnKick[1]);
  fkick->calcIntersection(x0mE,y0pE,pl1->getZOnPlane(x0mE,y0pE),xPpE,yPmE,pl2->getZOnPlane(xPpE,yPmE),segRegOnKick[2]);
  fkick->calcIntersection(x0pE,y0pE,pl1->getZOnPlane(x0pE,y0pE),xPmE,yPmE,pl2->getZOnPlane(xPmE,yPmE),segRegOnKick[3]);

  indexPar = pClus->getOwnIndex();
  indexFCh = -1;
  indexLCh = -2;
  Int_t nClustCh = findClustersSeg2();
  if(nClustCh>0) pClus->setIndexChilds(indexFCh,indexLCh);
  return nClustCh;
}

Int_t HMdc34ClFinderSec::findClustersSeg2(HMdcClus* pClus, Int_t *mBin){
  if(pClus->getIOSeg() != 0) return 0;
  if(pClus->getFakeFlag() != 0) return 0;
  
// if((pClus->getOwnIndex()%2)==0) return 0;
// if(isGeant && ((HMdcClusSim*)pClus)->getNNotFakeTracks()<1) return 0;
  
  if(mBin!=0) setMinBin(mBin);
  if(notEnoughWrs) return 0;
  
  pClus->getTarg(seg1);
  pClus->getPoint(dirSeg1);

  Double_t x0mE  = seg1.getX() - pClus->getErrXTarg();
  Double_t x0pE  = seg1.getX() + pClus->getErrXTarg();
  Double_t y0mE  = seg1.getY() - pClus->getErrYTarg();
  Double_t y0pE  = seg1.getY() + pClus->getErrYTarg();
  Double_t z0mE  = seg1.getZ() - pClus->getErrZTarg();
  
  Double_t xPmE  = dirSeg1.getX() - pClus->getErrX();
  Double_t xPpE  = dirSeg1.getX() + pClus->getErrX();
  Double_t yPmE  = dirSeg1.getY() - pClus->getErrY();
  Double_t yPpE  = dirSeg1.getY() + pClus->getErrY();
  
  // segOnkick[4]:
  //    [0]---[1]  ^ y
  //     |     |   |
  //     |     |   |
  //    [2]---[3]  |
  // X<-------------

  dirSeg1 -= seg1; // Must be here!
  fkick->calcIntersection(seg1,dirSeg1,segOnKick);
  fkick->calcIntersection(x0mE,y0mE,z0mE,xPpE,yPpE,pClus->getZOnPrPlane(xPpE,yPpE),segRegOnKick[0]);
  fkick->calcIntersection(x0pE,y0mE,z0mE,xPmE,yPpE,pClus->getZOnPrPlane(xPmE,yPpE),segRegOnKick[1]);
  fkick->calcIntersection(x0mE,y0pE,z0mE,xPpE,yPmE,pClus->getZOnPrPlane(xPpE,yPmE),segRegOnKick[2]);
  fkick->calcIntersection(x0pE,y0pE,z0mE,xPmE,yPmE,pClus->getZOnPrPlane(xPmE,yPmE),segRegOnKick[3]);

  indexPar =  pClus->getOwnIndex();
  indexFCh = -1;
  indexLCh = -2;
  Int_t nClustCh = findClustersSeg2();
  if(nClustCh>0) pClus->setIndexChilds(indexFCh,indexLCh);
  return nClustCh;
}

Int_t HMdc34ClFinderSec::findClustersSeg2(void) {
//printf("------------------------ Sec.%i:   ",sector+1); segOnKick.print();
  for(Int_t np=0; np<4; np++) {
    if(segRegOnKick[np].getX()<prPlotSeg2->xMin || segRegOnKick[np].getX()>prPlotSeg2->xMax) {
//       Warning("findClustersSeg2","S%i: The X=%g of the point %i out of the region (%g - %g).",
//          sector+1, segRegOnKick[np].getX(), np, prPlotSeg2->xMin, prPlotSeg2->xMax);
      return 0;
    }
//(segOnKick-segRegOnKick[np]).print();
    Double_t tmp = prPlotSeg2->D() - prPlotSeg2->A()*segRegOnKick[np].getX();  // D-A*x
    al[np]  = tmp-segRegOnKick[np].getZ();                                     // D-A*x-z
    bl[np]  = tmp*segRegOnKick[np].getY();                                     // (D-A*x)*y
    cl[np]  = segRegOnKick[np].getZ()+prPlotSeg2->B()*segRegOnKick[np].getY(); // z+B*y
    nbX[np] = prPlotSeg2->binX(segRegOnKick[np].getX());
  }
  
  // For HMdcClus errors:
  errSegOnKick.setXYZ( (Abs(segRegOnKick[0].getX()-segRegOnKick[1].getX())+
                        Abs(segRegOnKick[2].getX()-segRegOnKick[3].getX()))*0.25,
                       (Abs(segRegOnKick[0].getY()-segRegOnKick[2].getY())+
                        Abs(segRegOnKick[1].getY()-segRegOnKick[3].getY()))*0.25,
                       (Abs(segRegOnKick[0].getZ()-segRegOnKick[2].getZ())+
                        Abs(segRegOnKick[1].getZ()-segRegOnKick[3].getZ()))*0.25);
  
  if(useDxDyCut>0) {
    dXdYCutReg = HMdc34ClFinder::calcDxDyCutRegion(segOnKick);
//    prPlotSeg2->calcCrossBin(seg1,dirSeg1,xBin0,yBin0);
    prPlotSeg2->calcIntersection(seg1,dirSeg1,x0,y0); 
    xBin0 = prPlotSeg2->binX(x0);
    yBin0 = prPlotSeg2->binY(y0);
    
    yBinToDxDyInd = yBin0+yLineMin[dXdYCutReg];
    yMinDxDyCut   = yBinToDxDyInd;
    yMaxDxDyCut   = yBin0+yLineMax[dXdYCutReg];
    if(yMinDxDyCut <= 0)                   yMinDxDyCut = 1;
    if(yMaxDxDyCut >  prPlotSeg2->nBinY-2) yMaxDxDyCut = prPlotSeg2->nBinY-2;
  }

  realTypeClFinder=0;
  if(minAmp[2]>0 && minAmp[3]>0) makeSeg2PlotAmpCut();   // Segment ampl.cut only!
  else if(minAmp[2]>0)  makeModS2Plot(2);
  else if(minAmp[3]>0)  makeModS2Plot(3);
  else return 0;

  Int_t nClusters = scanPlotSeg2();
//!  clearPrSeg2();
  return nClusters;
}

void HMdcProjPlot::calcCrossBin(const HGeomVector &p,const HGeomVector &dir,Short_t& xb,Short_t& yb) const {
  // Function calculate xBinNumber and yBinNumber of bin which line p,dir crosses.
  Double_t x,y;
  calcIntersection(p,dir,x,y); 
  xb = binX(x);
  yb = binY(y);
}

Int_t HMdc34ClFinder::calcDxDyCutRegion(const HGeomVector& pnt) {
  // Return region number for dXdY kick cut.
  // pnt - cross point of inner cluster (or segment) with kick plane
  Double_t x = pnt.getX();
  Double_t y = pnt.getY();
  Double_t z = pnt.getZ();
  Double_t theta = TMath::ATan2(TMath::Sqrt(x*x+y*y),z)*TMath::RadToDeg();
  Double_t phi   = TMath::ATan2(y,x)*TMath::RadToDeg();
  Double_t c1, c2;
  Int_t c3;
  if(theta < 30.) {c1 = 18.; c2 =  6.; c3 = 0;}
  else            {c1 = 30.; c2 = 10.; c3 = 2;}
  if     (theta<19.) theta = 19.;
  else if(theta>69.) theta = 69.;
  if     (phi<61.)   phi = 61.;
  else if(phi>119.)  phi = 119.;
  return (Int_t((theta-c1)/c2)+c3)*6 + Int_t((phi-60.)/10.); 

  /*
  if     (phi <  80.)  dXdYCutReg = 0;
  else if(phi > 100.)  dXdYCutReg = 2;
  if(theta < 30.) return dXdYCutReg;
  if(theta < 50.) return dXdYCutReg + 3;
  return                 dXdYCutReg + 6;
  */
}


void HMdc34ClFinderSec::clearPrSeg2(void) {
  for(Int_t y=0;y<prPlotSeg2->nBinY;y++) {
    if(xCMax[y]<0) continue;
    memset(prPlotSeg2->weights3+xCMin[y] + y*nBinX,0, xCMax[y]-xCMin[y]+1);
    memset(prPlotSeg2->weights4+xCMin[y] + y*nBinX,0, xCMax[y]-xCMin[y]+1);
    xCMin[y] = nBinX;
    xCMax[y] = -1;
  }
}

void HMdc34ClFinderSec::makeModS2Plot(Int_t mod) {
  // Filling proj.plot for one MDC
  module = mod;
  cFMod  = &((*this)[module]);
  if(cFMod == 0) return;

  seg2MinAmpCut = minAmp[module];
  if(seg2MinAmpCut<3) seg2MinAmpCut = 3;

  if(mod==2) weightsArr = prPlotSeg2->weights3;
  else       weightsArr = prPlotSeg2->weights4;

  Int_t layList[6] = {2,3,1,4,0,5};   // order of layers at the pr.plot filling

  Int_t nFiredLay = 0;
  for(Int_t il=0; il<6; il++) {
    Int_t lay=layList[il];
    if(!calcLayerProjVar(lay)) continue; // No fired wires in this layer.
    nFiredLay++;
    if(maxAmp[module]-nFiredLay+1>=seg2MinAmpCut) makeLayProjV1();
    else                                          makeLayProjV2();
  }
}

void HMdc34ClFinderSec::calcDriftDist(void) {
  Double_t tdcTime  = cFLay->cells->getTimeValue(cell);
  if(tdcTime > 0.) {
    tdcTDist = pDriftTimeParSec->calcDistance(module,80.,tdcTime);
    if(tdcTDist < 0.) tdcTDist = 0;
  } else tdcTDist = 0;
}

Double_t HMdc34ClFinderSec::calcKickCor(void) {
  // return correction for cell number
  if(pKickCor != NULL && cFLay->module==3) {
    Double_t wp = (*(cFLay->pSCLay))[cell].getWirePos();
    Double_t corr  = pKickCor->calcCorrection(wp-cFLay->yCross,cFLay->module,cFLay->layer);
    if(corr >= -60. && corr <= 60.) return corr/cFLay->pSCLay->getPitch();
  }
  return 0.;
}

void HMdc34ClFinderSec::setArrays(Int_t lay) {
  layInd    = (module-2)*6 + lay;
  shUpArr   = sUAr[layInd];
  shDnArr   = sDAr[layInd];
  cellNum   = cNum[layInd];
  numPrRegs = &(nPRg[layInd]);
  cFLayArr  = pLayPar[layInd];
}

void HMdc34ClFinderSec::makeSeg2PlotAmpCut(void) {
  // Making proj.plot in segment with cut on the total amplitude in 2 MDC's
  // Number of MDCs in segment must be eq.2 !!!
  realTypeClFinder = 3;
  if(minAmp[2]<=0 || minAmp[3]<=0) return;
  seg2MinAmpCut = minAmp[2]+minAmp[3];
  if(typeClFinder==2) {
    seg2MinAmpCut = Min(minAmp[2],minAmp[3]) + 2;
    if(seg2MinAmpCut < 5) seg2MinAmpCut = 5;
  }
  Int_t maxAmpSeg = maxAmp[2]+maxAmp[3];
  if(maxAmpSeg<seg2MinAmpCut) return;

  Int_t layList[6] = {2,3,1,4,0,5};   // order of layers at the pr.plot filling
  Int_t nFiredLay  = 0;
  for(Int_t il=0; il<6; il++) {
    Int_t lay = layList[il];
    for(module=2; module<4; module++) {
      if(module==2) weightsArr = prPlotSeg2->weights3;
      else          weightsArr = prPlotSeg2->weights4;
      cFMod  = &((*this)[module]);
      if(cFMod == 0) continue;
      if(!calcLayerProjVar(lay)) continue; // No fired wires in this layer.
      nFiredLay++;
      if(maxAmpSeg-nFiredLay+1 >= seg2MinAmpCut) makeLayProjV1();
      else                                       makeLayProjV2();
    }
  }
}

Bool_t HMdc34ClFinderSec::calcLayerProjVar(Int_t lay) {
  // Setting of layer var. for filling proj.plot
  cFLay     = &((*cFMod)[lay]);
  cell      = cFLay->cells->getFirstCell();
  if(cell<0) return kFALSE;
  if(cell >= cFLay->nCells) {
    Warning("calcLayerProjVar","Cell %i >= num.cells(=%i)",cell,cFLay->nCells);
    cell = -1;
    return kFALSE;
  }
  setFirstLayerPart(cell);
  setArrays(lay);
  oneArr     = oneArrLay[lay];
  bitsSet    = bitsSetLay[lay];
  nYLinesL   = cFLay->nYLines-1;
  xBin1L     = cFLay->xBin1;
  xBin2L     = cFLay->xBin2;
  regInd     = 0;
  *numPrRegs = 0;
  if(useDriftTime) {
    Double_t cellNorm = 1./cFLay->nCells;
    while(kTRUE) {
      Double_t dDCutYCellCorr  = cell*cellNorm;             // Correction vs cell (~y)
      dDCutYCellCorr          *= dDCutYCellCorr*dDistYCorr; //quadratic
      calcYbinDrTm(dDCutYCellCorr);
      cell = cFLay->cells->next(cell);
      if(cell < 0) break;
      setLayerPart(cell);
    }
  } else {
    while(kTRUE) {
      shUpArr[*numPrRegs]  = Max(calcYbin(0,0,cell+1),calcYbin(0,1,cell+1));
      shDnArr[*numPrRegs]  = Min(calcYbin(1,0,cell),calcYbin(1,1,cell));
      cellNum[*numPrRegs]  = cell;
      cFLayArr[*numPrRegs] = cFLay;
      (*numPrRegs)++;
      cell = cFLay->cells->next(cell);
      if(cell < 0) break;
      setLayerPart(cell);
    }
  }
  cellNum[*numPrRegs] = -1;
  shDownMax  = -1000000;
  return kTRUE;
}

void HMdc34ClFinderSec::setFirstLayerPart(Int_t c) {
  if(c >= cFLay->nextPartFCell) {
    cFLay = cFLay->layerNextPart;                                // Layer part N2 (layerPart=1)
    if(c >= cFLay->nextPartFCell) cFLay = cFLay->layerNextPart;  // Layer part N3 (layerPart=2)
  }
  nYLinesL = cFLay->nYLines-1;
  xBin1L   = cFLay->xBin1;
  xBin2L   = cFLay->xBin2;
}

void HMdc34ClFinderSec::setLayerPart(Int_t c) {
  if(c >= cFLay->nextPartFCell) {
    cFLay = cFLay->layerNextPart;                                // Layer part N2 (layerPart=1)
    if(c >= cFLay->nextPartFCell) cFLay = cFLay->layerNextPart;  // Layer part N3 (layerPart=2)
    nYLinesL = cFLay->nYLines-1;
    xBin1L   = cFLay->xBin1;
    xBin2L   = cFLay->xBin2;
  }
}

HMdc34ClFinderLayer* HMdc34ClFinderLayer::getLayerPart(Int_t c) {
  if(c < nextPartFCell) return this;
  return layerNextPart->getLayerPart(c);
}

void HMdc34ClFinderSec::makeLayProjV1(void) {
  // plot filling and filled region determination
  while(setRegionVar()) {
    for(Int_t ny=ny1; ny<=ny2; ny++) {
      nbF = prPlotSeg2->xMinL[ny];
      nbL = prPlotSeg2->xMaxL[ny];
      
      Int_t iy1 = ny-shUp;
      Int_t iy2 = ny-shDown;
      if(cFLay->wOrType > 0) {
        if(iy1>=0        && xBin1L[iy1]>nbF) nbF = xBin1L[iy1];
        if(iy2<=nYLinesL && xBin2L[iy2]<nbL) nbL = xBin2L[iy2];
      } else {
        if(iy2<=nYLinesL && xBin1L[iy2]>nbF) nbF = xBin1L[iy2];
        if(iy1>=0        && xBin2L[iy1]<nbL) nbL = xBin2L[iy1];
      }
      if(nbL < nbF) continue;
      if(useDxDyCut==1) {
        Int_t ind = ny - yBinToDxDyInd;  // = ny - yBin0-yLineMin[dXdYCutReg]
        DxDyBinsCut& dxdycut = cutXBins[dXdYCutReg][ind];
        Int_t xCutMin = dxdycut.xBMin + xBin0;
        Int_t xCutMax = dxdycut.xBMax + xBin0;
        if(nbF<xCutMin) nbF = xCutMin;
        if(nbL>xCutMax) nbL = xCutMax;
        if(nbL < nbF) continue;
      }
      if(nbF<xCMin[ny]) xCMin[ny] = nbF;
      if(nbL>xCMax[ny]) xCMax[ny] = nbL;
      fillBins(ny);
    }
  }
}

void HMdc34ClFinderSec::makeLayProjV2(void) {
  // plot filling in filled regions only
  while(setRegionVar()) {
    for(Int_t ny=ny1; ny<=ny2; ny++) {
      nbL = xCMax[ny];
      if(nbL<0) continue;
      nbF = xCMin[ny];
      Int_t iy1 = ny-shUp;
      Int_t iy2 = ny-shDown;
      if(cFLay->wOrType > 0) {
        if(iy1>=0        && xBin1L[iy1]>nbF) nbF = xBin1L[iy1];
        if(iy2<=nYLinesL && xBin2L[iy2]<nbL) nbL = xBin2L[iy2];
      } else {
        if(iy2<=nYLinesL && xBin1L[iy2]>nbF) nbF = xBin1L[iy2];
        if(iy1>=0        && xBin2L[iy1]<nbL) nbL = xBin2L[iy1];
      }
      if(nbL < nbF) continue;
      if(useDxDyCut==1) {
        Int_t ind = ny - yBinToDxDyInd;  // = ny - yBin0-yLineMin[dXdYCutReg]
        DxDyBinsCut& dxdycut = cutXBins[dXdYCutReg][ind];
        Int_t xCutMin = dxdycut.xBMin + xBin0;
        Int_t xCutMax = dxdycut.xBMax + xBin0;
        if(nbF<xCutMin) nbF = xCutMin;
        if(nbL>xCutMax) nbL = xCutMax;
        if(nbL < nbF) continue;
      }
      fillBins(ny);
    }
  }
}

Bool_t HMdc34ClFinderSec::setRegionVar(void) {
  // return kFALSE if no projection region more
  // Calculate
  // shDown,shUp - Y-region of the cell (or some cells) on the proj.plot (can be outside plot!)
  // ny1,ny2 - Y region for filling on the the proj.plot
  if(regInd >= *numPrRegs) return kFALSE;
  if(shDnArr[regInd]>shDownMax) shDown = shDnArr[regInd];
  else                          shDown = shDownMax;
  shUp  = shUpArr[regInd];
  cell  = cellNum[regInd];
  cFLay = cFLayArr[regInd];
  regInd++;
  while(regInd < *numPrRegs && cell != cellNum[regInd]) {
                  // cell can be eq. cellNum[regInd] when drifttime is used only
    if(shDnArr[regInd]-shUp > 1) break;   // No overlap
    if(cFLay != cFLayArr[regInd]) break; // New part of layer
    // is overlap! Combine in one region:
    shUp = shUpArr[regInd];
    if(shDnArr[regInd] < shDown) {
      if(shDnArr[regInd]>shDownMax) shDown = shDnArr[regInd];
      else                          shDown = shDownMax;
    }
    cell = cellNum[regInd];
    regInd++;
  }
  if(cFLay != cFLayArr[regInd]) shDownMax = -1000000; // New part of layer
  else                          shDownMax = shUp+1;   // shDownMax prevent to fill one bin twice.
  // Setting of Y limits:
  shDown += cFLay->yFirst;
  shUp   += cFLay->yFirst;
  ny1     = shDown<=0 ? 1 : shDown;
  ny2     = nYLinesL+shUp>nBinYM2 ? nBinYM2 : nYLinesL+shUp;
  if(useDxDyCut==1) {
    if(ny1 < yMinDxDyCut) ny1 = yMinDxDyCut;
    if(ny2 > yMaxDxDyCut) ny2 = yMaxDxDyCut;
    // If ny1-ny2 - outside of yMinDxDyCut-yMaxDxDyCut 
    // ny2 will smaller ny1 and loops for(.=ny1;<=ny2;...) will ignored!
//  Mozhno optimizirovat' code v etom sluchae ...!!!
  }
  return kTRUE;
}

TH2C* HMdc34ClFinderSec::getPlot(Char_t* name, Char_t* title,Int_t ver) {
  for(module=2; module<4; module++) {
    cFMod = &((*this)[module]);
    if(&cFMod==0) continue;
    for(Int_t lay=0; lay<6; lay++) if(calcLayerProjVar(lay)) makeLayProjV1();
  }
  TH2C* plt = prPlotSeg2->getPlot(name,title);
  clearPrSeg2();
  return plt;
}

Int_t HMdc34ClFinderSec::calcYbin(Int_t upDo, Int_t leRi, Int_t c) {
  //upDo=0 - up rib, =1 down rib
  Int_t    np    = cFLay->nPSegOnKick[upDo][leRi];
  Double_t xp    = segRegOnKick[np].getX();
  Double_t yp    = cFLay->tgY*xp+c*cFLay->yStep+cFLay->y0[leRi];
  Double_t zp    = cFLay->tgZ*xp+c*cFLay->zStep+cFLay->z0[leRi];
  Double_t yPrPl = (al[np]*yp-bl[np]+segRegOnKick[np].getY()*zp)/(zp+prPlotSeg2->B()*yp-cl[np]);
  return Int_t((yPrPl-prPlotSeg2->yMin)/prPlotSeg2->stY) - cFLay->yBin[nbX[np]];
}

void HMdc34ClFinderSec::calcYbinDrTm(Double_t dDCutYCellCorr) {
  // Cut for tdcTDist = dDistCut + dDCutCorr[layInd] + dDCutYCellCorr
  // dDistCut constant for all layers and cells
  // dDCutCorr[lay] - correction for layers
  // dDCutYCellCorr = dDistYCorr*(cell/Number_cells)^2
  calcDriftDist();
  Double_t corr = calcKickCor();
  Double_t cellCorr = cell + corr;  // +or- ? => MINUS!
  Double_t cnstYC = cellCorr*cFLay->yStep + cFLay->y0w;   // wire position in sec.coor.sys
  Double_t cnstZC = cellCorr*cFLay->zStep + cFLay->z0w;   // (x=0!)
  Double_t B      = prPlotSeg2->B();
  Short_t &shDnR1 = shDnArr[*numPrRegs];
  Short_t &shUpR1 = shUpArr[*numPrRegs];
  Short_t &shDnR2 = shDnArr[(*numPrRegs)+1];
  Short_t &shUpR2 = shUpArr[(*numPrRegs)+1];
  Double_t dDistCutLCorr = dDistCut+dDCutCorr[layInd]+dDCutYCellCorr;
// static Double_t ddMin = 1000.;
// static Double_t ddMax = 0.;
// if(ddMin>dDistCutLCorr) {
//   ddMin=dDistCutLCorr;
//   printf("ddMin=%g\n",ddMin);
// }
// if(ddMax<dDistCutLCorr) {
//   ddMax=dDistCutLCorr;
//   printf("  ddMax=%g\n",ddMax);
// }
  Double_t distPlCut = tdcTDist + dDistCutLCorr;
  if(distPlCut > cFLay->maxDrDist) distPlCut = cFLay->maxDrDist;
  Double_t distMnCut = tdcTDist - dDistCutLCorr;
  if(distMnCut < 0.) distMnCut = 0.;
  for(Int_t i=0;i<2;i++) {  //i=0 - shift down, =1 -shift up
    Int_t np = cFLay->nPSegOnKick[1-i][0];

    Double_t distA = i==0 ? distPlCut : distMnCut;
    Double_t distB = i==0 ? distMnCut : distPlCut;
    for(Int_t j=0;j<2;j++) {
      if(j==1) np ^= 1;
      Double_t x0 = segRegOnKick[np].getX();
      Double_t y0 = segRegOnKick[np].getY();
      Double_t z0 = segRegOnKick[np].getZ();

      // Point on the wire for xp = x0.
      Double_t yp = cFLay->tgY*x0 + cnstYC;
      Double_t zp = cFLay->tgZ*x0 + cnstZC;

      // Direction of the line kick->(xp,yp,zp). (dX=0 !);
      Double_t dY = yp-y0;
      Double_t dZ = zp-z0;

      // Calculation of the perpendicular to the plane wire and line dX,dY,dZ
      Double_t x     =  dY*cFLay->zWirDir - dZ*cFLay->yWirDir;
      Double_t y     =  dZ*cFLay->xWirDir;
      Double_t z     = -dY*cFLay->xWirDir;
      Int_t    yWrLU = cFLay->yBin[nbX[np]];

      Double_t normM = 1/Sqrt(x*x+y*y+z*z);
      Double_t coefy = cFLay->tgY*x - y;
      Double_t coefZ = cFLay->tgZ*x - z;

      // Region 1:
      Double_t norm  = distA*normM;
      Double_t ypr   = yp + coefy*norm;
      Double_t zpr   = zp + coefZ*norm;
      Double_t yPrPl = (al[np]*ypr-bl[np]+y0*zpr)/(zpr+B*ypr-cl[np]);
      Int_t    yBin  = Int_t((yPrPl-prPlotSeg2->yMin)/prPlotSeg2->stY) - yWrLU;
      if(i==0) {  // Low proj. bound
        if(j==0 || shDnR1>yBin) shDnR1 = yBin;
      } else {    // Upper proj. bound
        if(j==0 || shUpR1<yBin) shUpR1 = yBin;
      }

      // Region 2:
      norm  = distB*normM;
      ypr   = yp - coefy*norm;
      zpr   = zp - coefZ*norm;
      yPrPl = (al[np]*ypr-bl[np]+y0*zpr)/(zpr+B*ypr-cl[np]);
      yBin  = Int_t((yPrPl-prPlotSeg2->yMin)/prPlotSeg2->stY) - yWrLU;
      if(i==0) {  // Low proj. bound
        if(j==0 || shDnR2>yBin) shDnR2 = yBin;
      } else {    // Upper proj. bound
        if(j==0 || shUpR2<yBin) shUpR2 = yBin;
      }
    }
  }
  cellNum[*numPrRegs]  = cell;
  cFLayArr[*numPrRegs] = cFLay;
  (*numPrRegs)++;
  if(shDnR2-shUpR1 <= 1) { // One region
    shUpR1 = shUpR2;
  } else {                 // Two regions
    cellNum[*numPrRegs]  = cell;
    cFLayArr[*numPrRegs] = cFLay;
    (*numPrRegs)++;
  }
}

Int_t HMdc34ClFinderSec::scanPlotSeg2(void) {
  nClsArr = 0;
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg2();
  for(Int_t y=0;y<prPlotSeg2->nBinY;y++) {
    if(xCMax[y]<0) continue;

    Int_t nb0 = nBinX*y;
    Int_t nb  = nb0 + xCMin[y];
    Int_t nbL = nb0 + xCMax[y];

    UChar_t *wt3 = prPlotSeg2->weights3 + nb;
    UChar_t *wt4 = prPlotSeg2->weights4 + nb;
    for(; nb<=nbL; nb++,wt3++,wt4++) {
      UChar_t amp = prPlotSeg2->nBitsLuTb[*wt3] + prPlotSeg2->nBitsLuTb[*wt4];
      if(amp > 0) {
        *wt3 = 0;
        *wt4 = 0;
        if(amp >= seg2MinAmpCut) {
          if(useFloatLevel) calcClusterSeg2FloatLevel(nb,amp);
          else              calcClusterSeg2FixedLevel(nb,amp);
        }
      }
    }
//    memset(prPlotSeg2->weights+xCMin[y] + y*nBinX,0, xCMax[y]-xCMin[y]+1);
    xCMin[y] = nBinX;
    xCMax[y] = -1;
  }

  if(nClsArr == 0) return 0; // No clusters are found!
  if     (fakeSuppFlag<0) mergeClustSeg2();
  else if(fakeSuppFlag>0) removeGhosts();
  if(pMetaMatch != NULL)  checkMetaMatch();
  return fillClusCat();
}

void HMdc34ClFinderSec::removeGhosts(void) {
   // Ghost removing by selecting best cluster(s)
  if(nClsArr < 2) return;   // no clusters or one cluster only!
  Int_t sumMax1 = 0;
  Int_t sumMax2 = 0;
  for(Int_t cl=0; cl<nClsArr; cl++) {
    HMdcCluster& clst = clusArr[cl];
    if(clst.getFakeFlag() > 0) continue;
    Int_t nLevel     = clst.getRealLevel();
    Int_t nBins      = clst.getNBins();
    Int_t nLayers    = clst.getLCells2().getNLayers();
    Int_t currentSum = wLev*nLevel+wBin*nBins+wLay*nLayers;
    
    if(currentSum==sumMax1 || currentSum<=sumMax2) continue;
    if(sumMax1 == 0) sumMax1 = currentSum;
    else if(currentSum > sumMax1) {
      sumMax2 = sumMax1;
      sumMax1 = currentSum;
    } else if(currentSum > sumMax2) sumMax2 = currentSum;
  }

  Int_t dWtCutC = sumMax1-sumMax2;
  if(dWtCutC>dWtCut) dWtCutC = dWtCut;
  for(Int_t cl=0; cl<nClsArr; cl++) {
    HMdcCluster& clst = clusArr[cl];
    if(clst.getFakeFlag() > 0) continue;
    Int_t nLevel     = clst.getRealLevel();
    Int_t nBins      = clst.getNBins();
    Int_t nLayers    = clst.getLCells2().getNLayers();
    Int_t currentSum = wLev*nLevel+wBin*nBins+wLay*nLayers;
    if(sumMax1-currentSum>dWtCutC) clst.setFakeFlag(21);
  }  
}

// void HMdc34ClFinderSec::removeGhosts(void) {
//    //===Suppression==
//  
//   pListCells->clearClustCounter(1);
//   
//   for(Int_t cl=0; cl<nClsArr; cl++) {
//     HMdcCluster& clst = clusArr[cl];
//     
// if(clst.getIOSeg() != 1) continue;
// 
//     //                                          < 10 suppresed in testForIncl...
//     if(clst.getFakeFlag() > 0 && clst.getFakeFlag() < 10) continue;
//     HMdcList12GroupCells& listCells = clst.getLCells2();
//     if(clst.getRealLevel()==12) pListCells->addToClusCounters(1,listCells);
//   }
// 
//   for(Int_t cl=0; cl<nClsArr; cl++) {
//     // level 12---------------------------------------------
//     HMdcCluster& clst = clusArr[cl];
//     if(clst.getRealLevel()!=12)  continue;
//     if(clst.getFakeFlag() > 0 && clst.getFakeFlag() < 10) continue;
//     Int_t nBins = clst.getNBins();
//     
//     HMdcList12GroupCells& listCells = clst.getLCells2();
//     Int_t  nUnWr  = pListCells->getNUniqueWires(1,listCells);
// 
//     Bool_t realOk = nUnWr > 10 && nBins > 3;  // RealID!
//     Bool_t fakeOk = nUnWr < 2 && nBins < 4;   // nBins == 1;   // FakeID!
//     
//     UChar_t &fakeFd = clst.fakeFlagS();
//     UChar_t &realFd = clst.realFlagS();
// 
// // Dobavit' ispolzovanie "inRealCl..."
//     if(fakeOk && fakeFd > 0) continue;
//     if(fakeOk) {                            // if fakeFd==0 and fakeOk == kTRUE
//       fakeFd = 12;                          // substr. from clusters counter
//       pListCells->subFrClusCounters(1,listCells);
//       if(realFd > 0) {      
//         realFd = 0;                         // and substr. from real cl.counter
//         pListCells->subFrRClusCounters(1,listCells);
//       }
//     } else if(fakeFd>0) {                   // if fakeFd>0 and fakeOk == kFALSE
//       fakeFd = 0;                           // add to clusters counter
//       pListCells->addToClusCounters(1,listCells);
//       if(realOk) { 
//         realFd = 1;                         // and add to real cl.counter
//         pListCells->addToRClusCounters(1,listCells);
//       }
//     } else {                                // if fakeFd==0 and fakeOk == kFALSE
//       if(realOk && realFd > 0) continue;    // No changes is status
//       if(!realOk && realFd <= 0) continue;  // No changes is status
//       if(realOk ) {                         // if realOk=kTRUE && realFd <= 0
//         pListCells->addToRClusCounters(1,listCells); // add to real
//         realFd = 1;
//       } else {
//         realFd = 0;                         // if realOk=kFALSE && realFd > 0
//         pListCells->subFrRClusCounters(1,listCells); // sub. from real
//       }
//     }
//   }
// 
//   // Level 11:
// //!        for(Int_t g=0;g<1;g++)
//   for(Int_t cl=0; cl<nClsArr; cl++) {
//     // level 11---------------------------------------------
//     HMdcCluster& clst = clusArr[cl];
//     if(clst.getRealLevel() != 11) continue;
//     UChar_t &fakeFd = clst.fakeFlagS();
// //?    if(fakeFd > 0 && clst.getFakeFlag() < 10) continue;
//     if(fakeFd>0) continue; //???
//     HMdcList12GroupCells& listCells = clst.getLCells2();
// //          if(g==0) 
//     pListCells->addToClusCounters(1,listCells); // add to cluster counter.
//     Int_t   nRlWr;
//     Int_t   nUnWr = pListCells->getNUniqueAndRWires(1,listCells,nRlWr);
//     Bool_t fakeOk = nUnWr < 5; // && nRlWr > 5;   // FakeID!
//     Bool_t realOk = nUnWr > 10;   // RealID!
//     
//     UChar_t &realFd = clst.realFlagS();
//     if(fakeOk) {                   // if fake - remove from clust.counter
//       fakeFd = 11;
//       pListCells->subFrClusCounters(1,listCells);
//     } else if(realOk) {            // if real - add real clust.counter
//       realFd = 1;
//       pListCells->addToRClusCounters(1,listCells);
//     }        
//   }
// 
//   // Level 10:
//   /*for(Int_t g=0;g<1;g++)*/
//   for(Int_t cl=0; cl<nClsArr; cl++) {
//     // level 10---------------------------------------------
//     HMdcCluster& clst = clusArr[cl];
//     if(clst.getRealLevel() != 10) continue;
//     if(clst.getFakeFlag() > 0 && clst.getFakeFlag() < 10) continue;
//     UChar_t &fakeFd = clst.fakeFlagS();
//     UChar_t &realFd = clst.realFlagS();
//     if(fakeFd>0) continue; //???
// // ?    Int_t nBins = clst.getNBins();
//     HMdcList12GroupCells& listCells = clst.getLCells2();
// //        if(g==0) 
//     pListCells->addToClusCounters(1,listCells);
//     Int_t   nRlWr;
//     Int_t   nUnWr = pListCells->getNUniqueAndRWires(1,listCells,nRlWr);
// 
// //    Bool_t fakeOk = (nUnWr==0) || (nBins<=2 && nUnWr==1) ||
// //                    (nUnWr < 4 && nRlWr > 4); // Fake 80.68%/0.82%/0.40%
//     Bool_t fakeOk = nUnWr < 7;
//     Bool_t realOk = nUnWr > 9;   // RealID!
//     
//     if(fakeOk && fakeFd==0) {
//       fakeFd = 10;
//       pListCells->subFrClusCounters(1,listCells);
//     } else if(realOk && realFd==0) {
//       realFd = 1;
//       pListCells->addToRClusCounters(1,listCells);
//     }
//   }
// }

void HMdc34ClFinderSec::checkMetaMatch(void) {
  // Meta matching
  for(Int_t cl=0; cl<nClsArr; cl++) {
    HMdcCluster& cluster = clusArr[cl];
    if(cluster.getFakeFlag()>0 || !cluster.getStatus()) continue;
    Double_t xc = cluster.getX();
    Double_t yc = cluster.getY();
    if( !pMetaMatch->hasClusMathToMeta(sector,segOnKick,xc,yc,prPlotSeg2->getZOnPlane(xc,yc)) ) {
      cluster.setFakeFlagAndStatus(9);
    }
  }  
}

Int_t HMdc34ClFinderSec::fillClusCat(void) {
  // Filling containers:
  if(nClsArr <= 0) return 0;
  Int_t nClusters      = 0;
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg2();
  if( isGeant ) {
    HMdcGeantEvent* pGEvent = HMdcGeantEvent::getExObject();
//    if(pGEvent) pGEvent->clearOSegClus();
    if(pGEvent) pGEvent->clearOClus();  // For HMdcSeg fake supr. clean best cluster ind. only
  }
  for(Int_t cl=0; cl<nClsArr; cl++) if(clusArr[cl].getStatus()) {
    if(fakeSuppFlag==1 && clusArr[cl].getFakeFlag()>0) continue;   // Fake
    HMdcCluster& cluster = clusArr[cl];
    
    if(useDxDyCut>0 && !cutDxDyArr[dXdYCutReg].IsInside(cluster.getX()-x0,cluster.getY()-y0)) continue;
    
    cluster.calcClusParam();
    HMdcClus* clus = (HMdcClus*)fClusCat->getNewSlot(locClus,&indexLCh);
    if(!clus) {
      Warning("fillClusCat","S.%i No slot HMdcClus available!",sector+1);
      break;
    }
    if(isGeant) clus = (HMdcClus*)(new(clus) HMdcClusSim(cluster.getLCells2()));
    else        clus = new(clus) HMdcClus(cluster.getLCells2());
    if(indexFCh<0) indexFCh=indexLCh;
    nClusters++;
    Int_t nLayM1 = clus->getNLayersMod(0);
    Int_t nLayM2 = clus->getNLayersMod(1);
    clus->setSecSegInd(sector,1,indexLCh);
    Int_t tpClFndr = 0;
    if( useFloatLevel ) tpClFndr |= 128;
                        tpClFndr |= 256;  // Segment amp.cut only!
    if(realTypeClFinder==2 && (nLayM1<minAmp[2] || nLayM2<minAmp[3])) {
      clus->setMod( (nLayM1>=nLayM2) ? 2 : 3);
      clus->setTypeClFinder(tpClFndr|2);
    } else {
      clus->setMod((mSeg[1]==3) ? -2 : (mSeg[1]+1) );  // mSeg[1]+1=mSeg[1]-1+2
      clus->setTypeClFinder(tpClFndr);
    }
    clus->setMinCl(minAmp[2],minAmp[3]);
    clus->setPrPlane(prPlotSeg2->A(),prPlotSeg2->B(),prPlotSeg2->D());
    clus->setTarg(segOnKick);
    clus->setErrTarg(errSegOnKick);
    clus->setIndexParent(indexPar);
    cluster.fillClus(clus,1,isGeant); // ",1," - outer segment
  }
  return nClusters;
}

void HMdc34ClFinderSec::mergeClustSeg2(void) {
  Int_t nClus      = nClsArr;
  Int_t mergeLevM3 = minAmp[2]>4 ? 4:3; //???
  Int_t mergeLevM4 = minAmp[3]>4 ? 4:3; //???
  while(nClus>1) {
    Bool_t nomerg=kTRUE;
    for(Int_t cl1=0; cl1<nClsArr-1; cl1++) {
      HMdcCluster& cls1=clusArr[cl1];
      if(!cls1.getStatus()) continue;
      HMdcList12GroupCells& cLCSeg2 = cls1.getLCells2();
      for(Int_t cl2=cl1+1; cl2<nClsArr; cl2++) {
        HMdcCluster& cls2=clusArr[cl2];
        if(!cls2.getStatus()) continue;
        HMdcList12GroupCells* cLCSeg2s = &(cls2.getLCells2());
        Float_t dY=cls1.getY()-cls2.getY();
//???optimizaciya???        if(dY>100.) break;
        if(Abs(dY) > 30.) continue;                          //  30. mm !???
        if(Abs(cls1.getX()-cls2.getX()) > 150.) continue;    // 150. mm !???
        if(realTypeClFinder==2) { // realTypeClFinder==2 when typeClFinder==2 +...!
          if(/*typeClFinder==2 &&*/ cLCSeg2.compare(cLCSeg2s,0,11)<6) continue;
        } else {
          if(minAmp[2]>0 && cLCSeg2.compare(cLCSeg2s,0, 5)<mergeLevM3) continue;
          if(minAmp[3]>0 && cLCSeg2.compare(cLCSeg2s,6,11)<mergeLevM4) continue;
        }
        cls1.addClus(cls2);
        nomerg=kFALSE;
        nClus--;
      }
    }
    if(nomerg || nClus==1) break;
  }
}

void HMdc34ClFinderSec::initCluster(Int_t nBin,UChar_t amp) {
  cluster = &(clusArr[nClsArr]);
  cluster->init(sector,1,prPlotSeg2->xFirstBin, prPlotSeg2->yFirstBin,
                         prPlotSeg2->stX,       prPlotSeg2->stY);   // 1 -segmet number (outer)
  Int_t ny = prPlotSeg2->ybin(nBin);
  nLMinCl = nLMaxCl = ny;
  xMinClLines[ny] = xMaxClLines[ny] = prPlotSeg2->xbin(nBin);
  isClstrInited = kTRUE;
  addBinInCluster(nBin,amp);
}

void HMdc34ClFinderSec::reinitCluster(Int_t nBin,UChar_t amp) {
  // Use it if list of wires empty yet!
  cluster->clearBinStat();
  addBinInCluster(nBin,amp);
  Int_t ny = prPlotSeg2->ybin(nBin);
  nLMinCl = nLMaxCl = ny;
  xMinClLines[ny] = xMaxClLines[ny] = prPlotSeg2->xbin(nBin);
}

Bool_t HMdc34ClFinderSec::calcClusterSeg2FixedLevel(Int_t nBin,UChar_t amp) {
  // claster finder with fixed level of searching
  initCluster(nBin,amp);
  stack->init();
  do {
    for(Int_t ib=0; ib<8; ib++) {
      Int_t nb = nBin+nearbyBins[ib];
      amp = prPlotSeg2->getBinAmplAndClear(nb);
      if( amp>=seg2MinAmpCut ) {
        stack->push(nb);
        addBinInCluster(nb,amp);
      }
    }
  } while((nBin=stack->pop()) >= 0);
  return fillClusterSeg2();  // Filling of the cluster
}

Bool_t HMdc34ClFinderSec::calcClusterSeg2FloatLevel(Int_t nBin,UChar_t amp) {
  // claster finder with floating level of searching
  initCluster(nBin,amp);
  stacksArr->init(12);
  while (kTRUE) {
    for(Int_t ib=0; ib<8; ib++) {
      Int_t nb = nBin+nearbyBins[ib];
      UChar_t ampN = prPlotSeg2->getBinAmplAndClear(nb);
      if( ampN<seg2MinAmpCut ) continue;
      stacksArr->push(nb,ampN);
      if(ampN > amp) {    // Increasing amplitude:
        if(ib<7) stacksArr->push(nBin,amp);
        amp = ampN;
        if(isClstrInited) reinitCluster(nb,amp);
        else              initCluster(nb,amp);
        break;
      }
      if(ampN==amp && isClstrInited) addBinInCluster(nb,amp);
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-----------------------------
    if(isClstrInited && !fillClusterSeg2()) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
  }
  return kTRUE;
}

Bool_t HMdc34ClFinderSec::increaseClusterNum(void) {
  isClstrInited = kFALSE;
  if(nClsArr > 0 && pClustersArrs->testMdc34Cluster(0,nClsArr)) { // --- Fake suppr. step1;
    if(fakeSuppFlag==1) return kTRUE;   // Cluster "cluster" is fake, don't increace counter
  }
  nClsArr++;
  if(nClsArr < clusArrSize) return kTRUE;
  nClsArr = clusArrSize;
  Warning("increaseClusterNum"," Num.of clusters in sector %i > max=%i\n",sector,clusArrSize);
  clearPrSeg2();
  return kFALSE;
}

void HMdc34ClFinderSec::addBinInCluster(Int_t nBin,UChar_t wt) {
  Int_t nx = prPlotSeg2->xbin(nBin);
  Int_t ny = prPlotSeg2->ybin(nBin);
  cluster->addBin(nx, ny, wt);
  // At the scaning of the cluster y step(in bins) can't be > 1
  if(ny<nLMinCl) {
    nLMinCl = ny;
    xMinClLines[ny] = xMaxClLines[ny] = nx;
  } else if(ny>nLMaxCl) {
    nLMaxCl = ny;
    xMinClLines[ny] = xMaxClLines[ny] = nx;
  }
  else if(nx < xMinClLines[ny]) xMinClLines[ny] = nx;
  else if(nx > xMaxClLines[ny]) xMaxClLines[ny] = nx;
}


Bool_t HMdc34ClFinderSec::fillClusterSeg2(void) {
  // Filling of cluster:
  cluster->calcXY();
  Int_t xMinCl = MinElement(nLMaxCl-nLMinCl+1,xMinClLines+nLMinCl);
  Int_t xMaxCl = MaxElement(nLMaxCl-nLMinCl+1,xMaxClLines+nLMinCl);

  for(module=2; module<4; module++) {
    if(!lMods[module]) continue;
    cFMod = &((*this)[module]);
    Int_t startLay = (module-2)*6;
    for(Int_t lay=0; lay<6; lay++) {
      Int_t layerSeg = startLay+lay;
      cFLay    = &((*cFMod)[lay]);
      HMdcLayListCells* cells = cFLay->cells;

      setArrays(lay);
      for(Int_t rInd=0;rInd<*numPrRegs;rInd++) {
        Int_t cell    = cellNum[rInd];
        Int_t shiftUp = shUpArr[rInd];
        Int_t shiftDn = shDnArr[rInd];
        Int_t cutUp   = nLMinCl - shiftUp;
        Int_t cutDn   = nLMaxCl - shiftDn;
        cFLay         = cFLayArr[rInd];
        if(cFLay->yBin[xMinCl] < cutUp && cFLay->yBin[xMaxCl] < cutUp) continue;
        if(cFLay->yBin[xMinCl] > cutDn && cFLay->yBin[xMaxCl] > cutDn) continue;
                                               // ???????????????     break;
        for(Int_t ny=nLMinCl; ny<=nLMaxCl; ny++) {
          Int_t tYmin = cFLay->yBin[xMinClLines[ny]]-ny;
          Int_t tYmax = cFLay->yBin[xMaxClLines[ny]]-ny;
          Int_t dY1b  = tYmin+shiftUp;
          Int_t dY2b  = tYmax+shiftUp;
    //if (dY1b==0 || dY2b==0) - optimizaciya, proverit' na time !!!!???
          if(dY1b<0 && dY2b<0) break;
          if(dY1b) dY1b = dY1b>0 ? 1 : -1;
          if(dY2b) dY2b = dY2b>0 ? 1 : -1;
          // if dY1b*dY2b<=0 the line cross cell projection
          if(dY1b*dY2b>0) {
            Int_t dY1a = tYmin+shiftDn;
            Int_t dY2a = tYmax+shiftDn;
            if(dY1a) dY1a = dY1a>0 ? 1 : -1;
            if(dY2a) dY2a = dY2a>0 ? 1 : -1;
            // if dY1a*dY2a<=0 the line cross the cell projection
            // dY1a*dY1b<0 The line is in the cell projection
            if(dY1a*dY2a>0 && dY1a*dY1b > 0) continue;
// ????? proverka binov???
          }
          Int_t nDeleted = cluster->getLCells2().setTime(layerSeg,cell,cells->getTime(cell));
	  if(nDeleted && !HMdc34ClFinder::getQuietMode()) Warning(
            "fillClusterSeg2","Too big cluster, %i cells was removed",nDeleted);
          break;
        }
      }
    }
  }
  return increaseClusterNum();
}

//---------------------------------------------------------

HMdc34ClFinder* HMdc34ClFinder::fMdc34ClFinder = 0;
Bool_t          HMdc34ClFinder::quietMode      = kTRUE;

HMdc34ClFinder::HMdc34ClFinder(const Char_t* name,const Char_t* title,const Char_t* context)
              : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HMdc34ClFinderSec
  strcpy(detName,"Mdc");

  array         = new TObjArray(6);
  fGetCont      = HMdcGetContainers::getObject();
  if( !fGetCont ) return;
  fSpecGeomPar  = fGetCont->getSpecGeomPar();
  fSizesCells   = HMdcSizesCells::getObject();
  fMdcGeomPar   = fGetCont->getMdcGeomPar();
  fMdcClusCat   = fGetCont->getCatMdcClus(kTRUE);
  xMinClLines   = 0;
  xMaxClLines   = 0;
  stack         = 0;
  stacksArr     = 0 ;
  pMetaMatch    = NULL;
  useDxDyCut    = kFALSE;
  HMdcDriftTimePar::getObject();
  pKickCor      = NULL;
}

Bool_t HMdc34ClFinder::initContainer(HMdcEvntListCells& event) {
  if( !fSizesCells->initContainer() )              return kFALSE;
  if( !HMdcGetContainers::isInited(fSpecGeomPar) ) return kFALSE;
  if( !HMdcGetContainers::isInited(fMdcGeomPar) )  return kFALSE;
  if( !HMdcDriftTimePar::getObject()->initContainer() ) return kFALSE;
  if( !status && (fSizesCells->hasChanged() || fSpecGeomPar->hasChanged() || fMdcGeomPar->hasChanged())) {
    changed = kTRUE;
    if(!fMdcClusCat) return kFALSE;
    useKickCor = HMdcTrackDSet::getUseKickCorFlag(); //  kFALSE;
    if(useKickCor && pKickCor == NULL) pKickCor = new HMdcKickCor();
    Int_t nBinX,nBinY;
    HMdcTrackDSet::getProjectPlotSizeSeg2(nBinX,nBinY);
    if(!quietMode) printf("Project plot size of outer segment: %ix%i\n",nBinX,nBinY);
    for (Int_t sec = 0; sec < 6; sec++) if( fGetCont->isSegActive(sec,1) ) {
      if( (*array)[sec] == 0) {
        // HMdc34ClFinderSec object creating:
        HMdc34ClFinderSec* secF = new HMdc34ClFinderSec(sec,nBinX,nBinY); //????0,0???
        (*array)[sec] = secF;
        secF->setClusCut(fMdcClusCat);
        secF->setKickPlane(&kickPlane);
        secF->setCellsList(event[sec]);
        if(xMinClLines == 0) xMinClLines = new Short_t [nBinY];
        if(xMaxClLines == 0) xMaxClLines = new Short_t [nBinY];
        secF->setXMinClLines(xMinClLines);
        secF->setXMaxClLines(xMaxClLines);
        if(stacksArr==0) {
          stacksArr = new HMdcClFnStacksArr();
          stack     = stacksArr->getOneStack();
        }
        secF->setClFnStArr(stacksArr);
        secF->setClFnStack(stack);
        secF->doMetaMatch(pMetaMatch);
        secF->setKickCorr(pKickCor);
      }
      // initialization of container ---
      if(!calcTarget(sec))        return kFALSE;
      if(!calcProjPlaneSeg2(sec)) return kFALSE;
      if(!calcSizePlotSeg2(sec))  return kFALSE;
      if(!calcWiresProj(sec))     return kFALSE;

      HMdc34ClFinderSec& p34CFSec = (*this)[sec];

      useDxDyCut = HMdcTrackDSet::getDxDyKickCut(cutDxDyArr);
      if(useDxDyCut && !p34CFSec.setDxDyCut(cutDxDyArr)) return kFALSE;

#if DEBUG_LEVEL>2
      (*this)[sec].prPlotSeg2->print();
#endif
      // --------------------------------
    }
    if(versions[1]<0 || versions[2]<0) versions[1]=versions[2]=0;
    else versions[2]++;
  } else changed=kFALSE;
  return kTRUE;
}

HMdc34ClFinder::~HMdc34ClFinder() {
  // destructor
  if(array) {
    array->Delete();
    delete array;
  }
  fMdc34ClFinder = NULL;
  if(xMinClLines) delete [] xMinClLines;
  if(xMaxClLines) delete [] xMaxClLines;
  xMinClLines = NULL;
  xMaxClLines = NULL;
  if(stacksArr) delete stacksArr;
  stacksArr   = NULL;
  HMdcDriftTimePar::deleteCont();
  if(pKickCor != NULL) delete pKickCor;
}

HMdc34ClFinder* HMdc34ClFinder::getObject(void) {
  if(!fMdc34ClFinder) fMdc34ClFinder=new HMdc34ClFinder();
  return fMdc34ClFinder;
}

HMdc34ClFinder* HMdc34ClFinder::getExObject(void) {
  return fMdc34ClFinder;
}

void HMdc34ClFinder::deleteCont(void) {
  if(fMdc34ClFinder) delete fMdc34ClFinder;
}

void HMdc34ClFinder::clear(void) { // *!*
  // clears the container
  for(Int_t s=0;s<6;s++) if((*array)[s]) (*this)[s].clear();
}

Bool_t HMdc34ClFinder::calcTarget(Int_t sec){
  //Geting size of target
  if(!fSizesCells->hasChanged() && !fSpecGeomPar->hasChanged()) return kTRUE;
  Int_t nT = fSpecGeomPar->getNumTargets()-1;
  if( nT < 0 ) {
    Error("calcTarget","Number of targets = %i!",nT+1);
    return kFALSE;
  }
  HMdc34ClFinderSec& fsec=(*this)[sec];
  HGeomVector* target = fsec.getTargetArr();
  target[0] = (fSpecGeomPar->getTarget(0)->getTransform()).getTransVector();
  target[0].setZ(target[0].getZ() + fSpecGeomPar->getTarget(0)->getPoint(0)->getZ());
  target[1] = (fSpecGeomPar->getTarget(nT)->getTransform()).getTransVector();
  target[1].setZ(target[1].getZ() + fSpecGeomPar->getTarget(nT)->getPoint(2)->getZ());
  const HGeomTransform* trans=(*fSizesCells)[sec].getLabTrans();
  if(trans == NULL) return kFALSE;
  target[0] = trans->transTo(target[0]);
  target[1] = trans->transTo(target[1]);
  return kTRUE;
}


Bool_t HMdc34ClFinder::calcProjPlaneSeg2(Int_t sec){
  // Calculation of project plane:
  //
  //  a plane between MDC3 and MDC4 - -1.<par<1.
  //  plane 1,2,3,4,5 or 6 MDC3 - par=-6,-5,-4,-3,-2 or -1
  //  plane 1,2,3,4,5 or 6 MDC2 - par=+1,+2,+3,+4,+5 or +6
  //  midplane MDC1 - proj.plane=-7
  //  midplane MDC2 - proj.plane=+7
  //  Automatical selection: par>=10. (for two MDC
  //  in sector = 0.15, for one MDC - midplane of this MDC)
  //  Double_t par=-7.; //In a future will be geted from Par.Container
  if(!fSizesCells->hasChanged()) return kTRUE;
  HMdc34ClFinderSec& fsec=(*this)[sec];
  HMdcSizesCellsSec& fSCellsSec=(*fSizesCells)[sec];
  if( !&fSCellsSec ) return kFALSE;
  Double_t par = 10.; //In a future will be geted from Par.Container

  UChar_t* mSeg = fsec.getMSeg();
  Int_t    nL   = (Int_t)par;
  //                  (mdc3 don't exist)      (mdc4 don't exist)      (one mdc exist only)
  if(nL<-7 || nL>7 || (nL<0 && mSeg[1]==2) || (nL>0 && mSeg[1]==1) || (nL==0 && mSeg[1]!=3)) {
    nL  = 0;
    par = 0.1;
  }
  const Char_t *text="as projection plane";
  HGeomTransform prPl;
  if( (nL==0 && mSeg[1]<3) || nL==-7 || nL==7 ) {
    // the project plane - the middle plane of MDC3 or MDC4
    Int_t mod = nL==0 ? mSeg[1]+1 : (nL+7)/14+2;

    prPl=*(fSCellsSec[mod].getSecTrans());
    if(!quietMode) printf("\n===> Sec.%i Seg.2: Using middle plane of MDC%i %s\n",sec+1,mod+1,text);
  } else if( nL==0 ) {
    // the project plane - between MDC3 & MDC4
    const HGeomTransform* trLayer6 = fSCellsSec[2][5].getSecTrans();
    const HGeomTransform* trLayer1 = fSCellsSec[3][0].getSecTrans();
    // MDC3 & MDC4 are ~parallel:
    Double_t distToL6 = trLayer6->getTransVector().length();
    Double_t distToL1 = trLayer1->getTransVector().length();
    Double_t newDist  = distToL6+(distToL1-distToL6)*(1.+par)/2.;
    Double_t mult     = 1.;
    if(par<=0) {    // proj.plane will parallel to MDC3
      prPl.setTransform(*trLayer6);
      mult = newDist/distToL6;
    } else {        // proj.plane will parallel to MDC4
      prPl.setTransform(*trLayer1);
      mult = newDist/distToL1;
    }
    HGeomVector prTr(prPl.getTransVector());
    prTr *= mult;
    prPl.setTransVector(prTr);
    if(!quietMode) printf("\n===> Sec.%i Seg.2: Using plane between MDC 3 & 4 (p=%g) %s\n",
                          sec+1,par,text);
  } else {
    // the project plane - one of the layers
    Int_t mod = nL<0 ? 2:3;
    if(nL<0) nL += 7;
    prPl = *(fSCellsSec[mod][nL-1].getSecTrans());
    if(!quietMode) printf("\n===> Sec.%i Seg.2: Using MDC%i, layer %i %s\n",sec+1,mod+1,nL,text);
  }
  fsec.getPlotSeg2()->setPlanePar(prPl);
  return kTRUE;
}

Bool_t HMdc34ClFinder::calcSizePlotSeg2(Int_t sec){
  // Calculation of plot's size:
  if( !fMdcGeomPar->hasChanged() && !fSizesCells->hasChanged()) return kTRUE;
  HMdc34ClFinderSec& fsec=(*this)[sec];
  UChar_t* mSeg = fsec.getMSeg();
  if(mSeg[0] == 0) return kFALSE;
  HMdcSizesCellsSec& fSCellsSec=(*fSizesCells)[sec];
  if( !&fSCellsSec ) return kFALSE;
  HGeomVector* target = fsec.getTargetArr();
  HGeomVector vect[2][4];
  HGeomVector pKick[4];
  const HGeomTransform* trans=0;
  HGeomVector newP[4][2]; //[4]-num.lines, [2]-the firt&last points of the line
  HMdcPlane plane;
  for(Int_t mod=0; mod<2; mod++) {
    if(fsec.mdcFlag(mod) == 0) continue;
    HGeomCompositeVolume *fVolMdc=fGetCont->getGeomCompositeVolume(mod);
    if(!fVolMdc) return kFALSE;
    HGeomVolume* fVolLayer=fVolMdc->getComponent(5); // layer 5 only
    if(!fVolLayer) return kFALSE;
    trans=fSCellsSec[mod].getSecTrans();
    plane.setPlanePar(*trans);
    for(Int_t point=0; point<4; point++) {
      HGeomVector *fpoint = fVolLayer->getPoint(point);
      if(!fpoint) return kFALSE;
      Int_t indx = mSeg[0]==3 ? mod : 0; // for one  MDC2 in seg. only
      vect[indx][point] = *fpoint;
      vect[indx][point].setZ(0.);
      vect[indx][point] = trans->transFrom(vect[indx][point]);
      if(mod==1 && mSeg[0]==3) {
        Int_t nTag=(point==0 || point==3) ? 0:1;
        vect[0][point] -= target[nTag];
        plane.calcIntersection(target[nTag],vect[0][point],vect[0][point]);
      }
    }
  }
  if(mSeg[0]==3) {
    for(Int_t mod=0; mod<2; mod++) {
      for(Int_t point=0; point<4; point++)
                       vect[mod][point]=trans->transTo(vect[mod][point]);
    }
    newP[0][0] = vect[0][0](0)>vect[1][0](0) ? vect[0][0] : vect[1][0];
    newP[0][1] = vect[0][1](0)>vect[1][1](0) ? vect[0][1] : vect[1][1];
    newP[1][0] = vect[0][1](1)>vect[1][1](1) ? vect[0][1] : vect[1][1];
    newP[1][1] = vect[0][2](1)>vect[1][2](1) ? vect[0][2] : vect[1][2];
    newP[2][0] = vect[0][2](0)<vect[1][2](0) ? vect[0][2] : vect[1][2];
    newP[2][1] = vect[0][3](0)<vect[1][3](0) ? vect[0][3] : vect[1][3];
    newP[3][0] = vect[0][3](1)<vect[1][3](1) ? vect[0][3] : vect[1][3];
    newP[3][1] = vect[0][0](1)<vect[1][0](1) ? vect[0][0] : vect[1][0];
    calcCrossLines(newP[0][0],newP[0][1],newP[3][0],newP[3][1],vect[0][0]);
    calcCrossLines(newP[0][0],newP[0][1],newP[1][0],newP[1][1],vect[0][1]);
    calcCrossLines(newP[1][0],newP[1][1],newP[2][0],newP[2][1],vect[0][2]);
    calcCrossLines(newP[2][0],newP[2][1],newP[3][0],newP[3][1],vect[0][3]);
    for(Int_t point=0; point<4; point++)
                      vect[0][point] = trans->transFrom(vect[0][point]);
  }
  HGeomVector dir;
  for(Int_t point=0; point<4; point++) {
    if(point==0 || point==3) dir = vect[0][point]-target[0];
    else                     dir = vect[0][point]-target[1]; //???
    kickPlane.calcIntersection(vect[0][point],dir,pKick[point]);
  }
  // pKick[0-3] pointers on the kick-plane
  Double_t yMin =  1.e+10;
  Double_t yMax = -1.e+10;
  for(Int_t mod=2; mod<4; mod++) {
    if(fsec.mdcFlag(mod) == 0) continue;
    HGeomCompositeVolume *fVolMdc=fGetCont->getGeomCompositeVolume(mod);
    Int_t indx = mSeg[1]==3 ? mod-2 : 0; // for one  MDC2 in seg. only
    trans=fSCellsSec[mod].getSecTrans();
    for(Int_t point=0; point<4; point++) {
      vect[indx][point]  = *(fVolMdc->getPoint(point));
      vect[indx][point].setZ(0.);  //midplane of MDC
      vect[indx][point]  = trans->transFrom(vect[indx][point]);
      vect[indx][point] -= pKick[point];
      fsec.getPlotSeg2()->calcIntersection(pKick[point],vect[indx][point],vect[indx][point]);
      if(vect[indx][point](1)<yMin) yMin = vect[indx][point](1);
      if(vect[indx][point](1)>yMax) yMax = vect[indx][point](1);
    }
  }
  Double_t xMaxD=xLine(vect[0][0],vect[0][1],yMin);
  Double_t xMinD=xLine(vect[0][2],vect[0][3],yMin);
  Double_t xMax=xLine(vect[0][0],vect[0][1],yMax);
  Double_t xMin=xLine(vect[0][2],vect[0][3],yMax);
  if(mSeg[1]==3) {
    Double_t xnew=xLine(vect[1][0],vect[1][1],yMin);
    if(xnew>xMaxD) xMaxD=xnew;
    xnew=xLine(vect[1][2],vect[1][3],yMin);
    if(xnew<xMinD) xMinD=xnew;
    xnew=xLine(vect[1][0],vect[1][1],yMax);
    if(xnew>xMax) xMax=xnew;
    xnew=xLine(vect[1][2],vect[1][3],yMax);
    if(xnew<xMin) xMin=xnew;
  }
  fsec.getPlotSeg2()->setEdges(yMin, xMinD, xMaxD, yMax, xMin, xMax);
  return kTRUE;
}

void HMdc34ClFinder::calcCrossLines(HGeomVector& p1l1, HGeomVector& p2l1,
            HGeomVector& p1l2, HGeomVector& p2l2, HGeomVector& cross) {
  // Calculating a cross of 2 lines (p1l1-p2l1, p1l2-p2l2) on the one (!)
  // plane. Z seted to 0.
  Double_t a1 = (p1l1(1)-p2l1(1))/(p1l1(0)-p2l1(0));
  Double_t b1 = p1l1(1)-a1*p1l1(0);
  Double_t a2 = (p1l2(1)-p2l2(1))/(p1l2(0)-p2l2(0));
  Double_t b2 = p1l2(1)-a2*p1l2(0);
  Double_t x  = (b2-b1)/(a1-a2);
  Double_t y  = a2*x+b2;
  cross.setXYZ(x,y,0.);
}

Double_t HMdc34ClFinder::xLine(HGeomVector& p1,HGeomVector& p2,Double_t yi) {
  return (yi-p2(1))/(p1(1)-p2(1))*(p1(0)-p2(0))+p2(0);
}

Bool_t HMdc34ClFinder::calcWiresProj(Int_t sec) {
  if(!fMdcGeomPar->hasChanged() && !fSizesCells->hasChanged()) return kTRUE;
  HMdc34ClFinderSec& fsec=(*this)[sec];
  UChar_t* mSeg = fsec.getMSeg();
  if(mSeg[0]==0) return kFALSE;
  HMdcSizesCellsSec& fSCellsSec=(*fSizesCells)[sec];
  if( !&fSCellsSec ) return kFALSE;
  HMdcProjPlot* prPlotSeg2 = fsec.getPlotSeg2();
  Int_t mod = mSeg[0]>1 ? 1:0;
  const HGeomTransform* trans=fSCellsSec[mod].getSecTrans();
  if(!trans) return kFALSE;
  HGeomVector *target = fsec.getTargetArr();
  HGeomVector  tag    = target[0]+target[1];
  tag /= 2.;
  HGeomVector  dir    = trans->getTransVector()-tag;
  dir /= dir.length();
  HGeomVector pKick;
  kickPlane.calcIntersection(tag,dir,pKick);
  for(Int_t mod=2; mod<4; mod++) {
    if(fsec.mdcFlag(mod) == 0) continue;
    HMdcSizesCellsMod& fSCellsMod=fSCellsSec[mod];
    if(!&fSCellsMod) return kFALSE;
    HMdc34ClFinderMod& fMod=fsec[mod];
    for(Int_t lay=0; lay<6; lay++) {
      HMdcSizesCellsLayer& fSCellsLay=fSCellsMod[lay];
      HMdc34ClFinderLayer& fLayer=fMod[lay];
      fLayer.calcWiresProj(fSCellsLay,pKick,prPlotSeg2,0);
      if(fSCellsLay.getLayerNParts()>1) {               // Next layer part:
        fLayer.nextPartFCell = fSCellsLay.getFirstCellPart2();
        fLayer.layerNextPart = new HMdc34ClFinderLayer(fLayer);
        fLayer.layerNextPart->calcWiresProj(fSCellsLay,pKick,prPlotSeg2,fLayer.nextPartFCell);
      }
    }
  }
  return kTRUE;
}

Bool_t HMdc34ClFinderLayer::calcWiresProj(HMdcSizesCellsLayer& fSCellsLay,HGeomVector& pKick,
                                          HMdcProjPlot* prPlotSeg2,Int_t firstCell) {
  pSCLay = &fSCellsLay;
  HGeomVector rib[4];
  HGeomVector point[2];
  HGeomVector wire[2];
  const HGeomTransform* secsys = fSCellsLay.getSecTrans();
  Double_t catDist = fSCellsLay.getHalfCatDist();
  Double_t pitch   = fSCellsLay.getPitch();
  Double_t cosWiOr = fSCellsLay.getCosWireOr(firstCell);
  Double_t wOfSet  = fSCellsLay.getWireOffset(firstCell);
  Double_t aWire   = fSCellsLay.getTanWireOr(firstCell);   // y=a*x+ln[1]

  // calc. of the wire's projection
  Double_t bWire  = (100.*pitch-wOfSet)/cosWiOr;  // 100.-cell n.100
  wire[0].setXYZ( 500., aWire*500.+bWire,0.);
  wire[1].setXYZ(-500.,-aWire*500.+bWire,0.);
  for(Int_t np=0; np<2; np++) {
    wire[np]  = secsys->transFrom(wire[np]);
    wire[np] -= pKick;
    prPlotSeg2->calcIntersection(pKick,wire[np],point[np]);
  }
#if DEBUG_LEVEL>2
  printf("M%i L%i C%i Size of wire on the proj.plane (poin1&point2):\n",
    module+1,layer+1,100+1);
  point[0].print();
  point[1].print();
#endif
  Double_t al      = (point[0](1)-point[1](1))/(point[0](0)-point[1](0));
  Double_t x       = (prPlotSeg2->nBinX-1) * prPlotSeg2->stX + prPlotSeg2->xMin;
  Double_t yLeft   = al*(x - point[1](0))+point[1](1); // (x-x2)*a+y2
  Double_t yRight  = al*(prPlotSeg2->xMin - point[1](0))+point[1](1);
  Short_t  nYLeft  = prPlotSeg2->binY(yLeft);
  Short_t  nYRight = prPlotSeg2->binY(yRight);
  Short_t  yShift  = Min(nYLeft,nYRight);
  if(Abs(nYLeft-nYRight) >= prPlotSeg2->nBinY) {
    Error("calcWiresProj","M%i L%i The region of Y bins of proj. wire >= nBinY(%i).",
    module+1,layer+1,prPlotSeg2->nBinY);
    return kFALSE;
  }
  if( !createArrayBins(prPlotSeg2->nBinX) ) return kFALSE;
  for(Int_t nx=0; nx<nBinX; nx++) {
    Double_t x  = nx * prPlotSeg2->stX + prPlotSeg2->xMin;
    Double_t y  = al*(x-point[1](0))+point[1](1);
    Short_t  nY = prPlotSeg2->binY(y);
    yBin[nx] = nY-yShift;
  }

  Int_t nLines     = yBin[nBinX - 1]-yBin[0];
  nYLines   = Abs(nLines)+1;
  wOrType   = nLines>=0 ? 1 : -1;
  yFirst    = nLines>=0 ? yBin[0]:yBin[nBinX-1];
  maxDrDist = Sqrt(pitch*pitch/4.+catDist*catDist);
  if(xBin1) {
    delete [] xBin1;
    delete [] xBin2;
  }
  xBin1 = new Short_t [nYLines];
  xBin2 = new Short_t [nYLines];
  for(Int_t yb=0;yb<nYLines;yb++) {
    Int_t yl=yb+yFirst;
    xBin1[yb] = -1;
    xBin2[yb] = -1;
    for(Int_t xb=0;xb<nBinX;xb++) {
      Int_t dy=(wOrType>=0) ? yBin[xb]-yl:yl-yBin[xb];
      if(dy<0) continue;
      if(dy>0 && xBin2[yb]>=0) break;
      if(xBin1[yb]<0) xBin1[yb]=xb;
      xBin2[yb] = dy==0 ? xb : xb-1;
    }
  }

  // calc. of the position of the  cells' ribs:
  // Z(x)=A1*x+Y0[nCell]; Z(x)=A2*x+Z0[nCell]; Y0&Z0 - at x=0;
  // Y0[nCell]=Ystep*nCell + Yshift; Z0[nCell]=Zstep*nCell + Zshift(=0!);
  //--- cells 0 & 100 ---
  for(Int_t cell=0; cell<101; cell+=100) {
    Double_t bCell= (cell*pitch-wOfSet-pitch/2.)/cosWiOr;
    // rib[0]-rib[1] - the down and left rib of cell
    // rib[2]-rib[3] - the down and right rib of cell
    for(Int_t n=0; n<4; n++) {
      Double_t x = ((n&1) == 0) ? 500.: -500.;
      Double_t z = (n < 2) ? -catDist : catDist;
      rib[n].setXYZ(x, aWire*x+bCell, z);
      rib[n]=secsys->transFrom(rib[n]);
    }
    Double_t dX = rib[0](0)-rib[1](0);
    rib[1].setXYZ(0.,(rib[0](0)*rib[1](1)-rib[1](0)*rib[0](1))/dX,
                     (rib[0](0)*rib[1](2)-rib[1](0)*rib[0](2))/dX);
    dX = rib[2](0)-rib[3](0);
    rib[3].setXYZ(0.,(rib[2](0)*rib[3](1)-rib[3](0)*rib[2](1))/dX,
                     (rib[2](0)*rib[3](2)-rib[3](0)*rib[2](2))/dX);
    if(cell==0) {
      y0[0] = rib[1](1);
      z0[0] = rib[1](2);
      y0[1] = rib[3](1);
      z0[1] = rib[3](2);
      tgY   = (rib[0](1)-rib[1](1))/(rib[0](0)-rib[1](0));
      tgZ   = (rib[0](2)-rib[1](2))/(rib[0](0)-rib[1](0));
    } else {
      yStep = (rib[1](1)-y0[0])/100.;
      zStep = (rib[1](2)-z0[0])/100.;
      //Calc. the point(index) number in HMdc34ClFinderSec::segRegOnKick[4]:
      if(rib[1](2)+prPlotSeg2->B()*rib[1](1)-prPlotSeg2->D()<0) {
        nPSegOnKick[0][0]=(tgY>0) ? 2 : 3;
        nPSegOnKick[1][0]=(tgY>0) ? 1 : 0;
      } else {
        nPSegOnKick[0][0]=(tgY>0) ? 1 : 0;
        nPSegOnKick[1][0]=(tgY>0) ? 2 : 3;
      }
      if(rib[3](2)+prPlotSeg2->B()*rib[3](1)-prPlotSeg2->D()<0) {
        nPSegOnKick[0][1]=(tgY>0) ? 2 : 3;
        nPSegOnKick[1][1]=(tgY>0) ? 1 : 0;
      } else {
        nPSegOnKick[0][1]=(tgY>0) ? 1 : 0;
        nPSegOnKick[1][1]=(tgY>0) ? 2 : 3;
      }
    }
  }
  // For drift time using:
  xWirDir = wire[0].getX() - wire[1].getX();
  yWirDir = wire[0].getY() - wire[1].getY();
  zWirDir = wire[0].getZ() - wire[1].getZ();
  y0w     = (y0[0]+y0[1]+yStep)/2.;
  z0w     = (z0[0]+z0[1]+zStep)/2.;
  
  return kTRUE;
}

void HMdc34ClFinder::setCellsList(HMdcEvntListCells& event) {
  for(Int_t s=0;s<6;s++) if((*array)[s]) (*this)[s].setCellsList(event[s]);
}

void HMdc34ClFinderSec::setCellsList(HMdcSecListCells& event) {
  pListCells   = &event;
  notEnoughWrs = kTRUE;
  for(Int_t m=2;m<4;m++) {
    if((*array)[m]) (*this)[m].setCellsList(event[m]);
    maxAmp[m] = -1;
  }
}

void HMdc34ClFinderMod::setCellsList(HMdcModListCells& event) {
  for(Int_t l=0;l<6;l++) if((*array)[l]) (*this)[l].setCellsList(event[l]);
}

Bool_t HMdc34ClFinderSec::setDxDyCut(TCutG* cutR) {
  useDxDyCut = 0;
  for(Int_t nr=0;nr<36;nr++) {
    if( !calcXBinsCut(nr,cutR[nr]) ) return kFALSE;
    Double_t x,y;
    for(Int_t i=0;i<cutR[nr].GetN();i++){
	cutR[nr].GetPoint(i, x, y);
	cutDxDyArr[nr].SetPoint(i,x,y);
    }
  }
  useDxDyCut = 1;
//useDxDyCut = 2; //???????????????????
  return kTRUE;
}

Int_t HMdc34ClFinderSec::calcCrosses(TCutG &cutDxDy,Double_t yb,DxDyBinsCut& cutB)  {
  Int_t nPnt = cutDxDy.GetN();
  Int_t nCr  = 0;
  for(Int_t i1=0;i1<nPnt;i1++) {
    Int_t i2 = i1+1<nPnt ? i1+1 : 0;
    Double_t x1,y1,x2,y2;
    cutDxDy.GetPoint(i1,x1,y1);
    cutDxDy.GetPoint(i2,x2,y2);
    if(yb<=y1 && yb<=y2) continue;
    if(yb> y1 && yb> y2) continue;
    if(y1==y2)           continue;
    Double_t xc = (yb-y2)*(x1-x2)/(y1-y2)+x2; // Cross point
    if(xc>=0.) xc += prPlotSeg2->stX/2.;
    else       xc -= prPlotSeg2->stX/2.;
    Short_t xBin = Short_t(xc/prPlotSeg2->stX);
    if(nCr == 0) {
      cutB.xBMin = xBin;
      cutB.xBMax = xBin;
    } else {
      if     (cutB.xBMin > xBin) cutB.xBMin = xBin;
      else if(cutB.xBMax < xBin) cutB.xBMax = xBin;
    }
    nCr++;
  }
  return nCr;
}


Bool_t HMdc34ClFinderSec::calcXBinsCut(Int_t nReg, TCutG& cutDxDy) {
  Short_t& nLnCut = nYLinesInCut[nReg];
  Short_t& yLMin  = yLineMin[nReg];
  Short_t& yLMax  = yLineMax[nReg];
  Double_t stepY  = prPlotSeg2->stY;
  
  Double_t xMin,yMin,xMax,yMax;
  cutDxDy.ComputeRange(xMin,yMin,xMax,yMax);
  Short_t yBinMin   = Short_t((yMin-stepY)/stepY);
  Short_t yBinMax   = Short_t((yMax+stepY)/stepY);
  if(cutXBins[nReg] != NULL) delete [] cutXBins[nReg];
  cutXBins[nReg]  = new DxDyBinsCut [yBinMax-yBinMin+1];
  DxDyBinsCut* cutBins = cutXBins[nReg];
  nLnCut = 0;
  yLMin  = yBinMax;
  yLMax  = yBinMin;
  for(Short_t yBin=yBinMin;yBin<=yBinMax;yBin++) {
    Int_t nCross = calcCrosses(cutDxDy,yBin*stepY,cutBins[nLnCut]);
    if(nCross==0) continue;
    if(nLnCut==0) yLMin = yBin;
    if(yBin-yLMin!=nLnCut || nCross>2) {  // Test for valid TCutG papameters
      Error("calcXBinsCut","Not valid TCutG dXdY cut parameters (more 1 region along X or Y)!");
      delete [] cutBins;
      return kFALSE;
    }
    yLMax = yBin;
    nLnCut++;
  }
  return kTRUE;
}

void HMdc34ClFinder::printClFinderParam(void) {
  HMdcTrackDSet::printMdc34ClFinderPar();
//   Bool_t useDriftTime = HMdcTrackDSet::useDriftTimeSeg2();
//   printf("  Drift time is %sused at the filling of project plot.\n",useDriftTime ? "":"NOT ");
//   printf("  Cut for the kick angle is %sused.\n",useDxDyCut ? "":"NOT ");
//   printf("  Meta matching is %s.\n",pMetaMatch==NULL ? "OFF":"ON");
// //  printf("  %s amplitude cut is used.\n",useSegAmpCut ? "Segment" : "Module");
//   Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg2();
//   printf("  %s level of cluster finder is used.\n",useFloatLevel ? "Float":"Fixed");
//   Char_t fakeSuppFlag = HMdcTrackDSet::getGhostRemovingFlagSeg2();
//   if      (fakeSuppFlag<0)  printf("  Cluster mergering ON. Fake suppression OFF.\n");
//   else  if(fakeSuppFlag==0) printf("  Fake suppression OFF. Cluster mergering OFF.\n");
//   else  if(fakeSuppFlag==1) printf("  Fake suppression ON. Cluster mergering OFF.\n");
//   else                      printf("  Fake suppression ON but ghosts will not removed.\n");
}
