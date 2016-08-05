using namespace std;
#include "hmdclookuptb.h"
#include "hspectrometer.h"
#include "hevent.h"
#include "heventheader.h"
#include "hmdcdetector.h"
#include "hpario.h"
#include "hmdcgetcontainers.h"
#include <iostream>
#include <iomanip>
#include "hmdcgeomstruct.h"
#include "hspecgeompar.h"
#include "hmdcgeompar.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hmdclayergeompar.h"
#include "TObjArray.h"
#include "TH2.h"
#include "hmdcclfnstack.h"
#include "hmdccluster.h"
#include "hmdcclussim.h"
#include "hmdctrackdset.h"
#include "hmdclistgroupcells.h"
#include "hmdclistcells.h"
#include "hmdcdrifttimepar.h"
#include "hruntimedb.h"
#include "hstart2geompar.h"


//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 18/06/2012 by O. Pechenova
//*-- Modified : 29/04/2010 by O. Pechenova
//*-- Modified : 21/02/2008 by V. Pechenov
//*-- Modified : 17/07/2003 by V. Pechenov
//*-- Modified : 05/02/2003 by V. Pechenov
//*-- Modified : 04/06/2002 by V.Pechenov
//*-- Modified : 09/05/2001 by V.Pechenov
//*-- Modified : 12/07/2000 by V.Pechenov
//*-- Modified : 23/05/2000 by V.Pechenov
//*-- Modified : 07/03/2000 by R. Holzmann
//*-- Modified : 02/12/99 by V.Pechenov
//*-- Modified : 26/10/99 by V.Pechenov
//*-- Modified : 20/05/99

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcLookUpTb
//
// Trackfinder for MDC1&2 and MDC3&4 if magnet off
//
////////////////////////////////////////////////////////////////

ClassImp(HMdcLookUpTbCell)
ClassImp(HMdcLookUpTbLayer)
ClassImp(HMdcLookUpTbMod)
ClassImp(HMdcLookUpTbSec)
ClassImp(HMdcLookUpTb)

HMdcLookUpTbCell::HMdcLookUpTbCell(void) {
  nLines    = 0;
  xBinMin   = 0;
  xBinMax   = 0;
  distCutFT = -1.;
  distCut1T = -1.;
  alphaMean = 80.;
}

void HMdcLookUpTbCell::init(Int_t yBinMinT, Int_t yBinMaxT) {
  Int_t newNLines = yBinMaxT-yBinMinT+1;
  if(newNLines>nLines) {
    if(xBinMin) delete [] xBinMin;
    if(xBinMax) delete [] xBinMax;
    nLines  = newNLines;
    xBinMin = new UShort_t [nLines];
    xBinMax = new UShort_t [nLines];
  }
  yBinMin = yBinMinT;
  yBinMax = yBinMaxT;
  line    = 0;
}

HMdcLookUpTbCell::~HMdcLookUpTbCell(void) {
  if(xBinMin) delete [] xBinMin;
  if(xBinMax) delete [] xBinMax;
}

Bool_t HMdcLookUpTbCell::addLine(UShort_t nc1, UShort_t nc2){
  if(nc1<=nc2 && line<nLines) {
    xBinMin[line]=nc1;
    xBinMax[line]=nc2;
    line++;
    return kTRUE;
  }
  Error("addLine","line number=%i > max.=%i  OR  nBin1=%i > nBin2=%i",
      line,nLines,nc1,nc2);
  return kFALSE;
}

//----------Layer-----------------------------

HMdcLookUpTbLayer::HMdcLookUpTbLayer(Int_t sec, Int_t mod, Int_t layer) {
  // Geting of pointers to par. cont.
  layPart2 = NULL;
  HMdcGetContainers* parCont = HMdcGetContainers::getObject();
  if( !parCont ) return;
  HMdcGeomStruct* fMdcGeomStruct = parCont->getMdcGeomStruct();
  if( !fMdcGeomStruct ) return;
  nCells = ((*fMdcGeomStruct)[sec][mod])[layer];
  pLTCellArr = new HMdcLookUpTbCell [nCells];
  HMdcSizesCells *pSizesCells = HMdcSizesCells::getExObject();
  pSCellLay  = &((*pSizesCells)[sec][mod][layer]);
}

HMdcLookUpTbLayer::~HMdcLookUpTbLayer(void) {
  if(pLTCellArr) delete [] pLTCellArr;
  pLTCellArr = NULL;
  if(layPart2 != NULL) delete layPart2;
  layPart2 = NULL;
}

Int_t HMdcLookUpTbLayer::getSize(void) {
  // return the size of pLTCellArr
  return pLTCellArr != 0  ? nCells : 0;
}

void HMdcLookUpTbLayer::setMinDistLUTb(Int_t lPart,HGeomVector& t,Double_t* carr) {
  // lPart=0 - first part of layer
  // lPart=1 - second part of layer (it can be in MDCIII and MDCIV only)
  DistCalcLuTb*  layPart = &layPart1;
  if(lPart==1) {
    if(layPart2 != NULL) delete layPart2;
    layPart2 = new DistCalcLuTb;
    layPart  = layPart2;
  }
  layPart->yt  = t(1);
  layPart->zt  = t(2);
  layPart->C1x = carr[0];
  layPart->C1y = carr[1];
  layPart->C1  = carr[2]; 
  layPart->C2x = carr[3];
  layPart->C2y = carr[4];
  layPart->C2  = carr[5];
}

//------------Module----------------------------
HMdcLookUpTbMod::HMdcLookUpTbMod(Int_t sec, Int_t mod) {
  // constructor creates an array of pointers of type HMdcLookUpTbMod
  array = new TObjArray(6);
  for (Int_t layer = 0; layer < 6; layer++) (*array)[layer] = new HMdcLookUpTbLayer(sec, mod, layer);
  nLayers=6;
}

HMdcLookUpTbMod::~HMdcLookUpTbMod(void) {
  // destructor
  if(array) {
    array->Delete();
    delete array;
  }
}

Int_t HMdcLookUpTbMod::getSize(void) {
  // returns the size of the pointer array
  if(array) return array->GetEntries();
  else return -1;
}

//----------Sector------------------------------------
Int_t              HMdcLookUpTbSec::hPlModsSize  = 0;
UChar_t*           HMdcLookUpTbSec::hPlMod[4]    = {0,0,0,0};
Int_t              HMdcLookUpTbSec::sizeBArSt    = 0;
UChar_t*           HMdcLookUpTbSec::plotBArSc    = 0;
UChar_t*           HMdcLookUpTbSec::plotBArM[4]  = {0,0,0,0};
Short_t*           HMdcLookUpTbSec::clusIndM1    = 0;
Int_t              HMdcLookUpTbSec::clIndArrSzM1 = 0;
Short_t*           HMdcLookUpTbSec::clusIndM2    = 0;
Int_t              HMdcLookUpTbSec::clIndArrSzM2 = 0;

HMdcLookUpTbSec::HMdcLookUpTbSec(Int_t sec, Int_t nSegs,Int_t inBinX, Int_t inBinY) {
  // constructor creates an array of pointers of type HMdcLookUpTbMod
  sector       = sec;
  nSegments    = nSegs;
  nModules     = 0;
  nMods        = 0;
  maxNModules  = nSegments*2;
  segment      = nSegments==1 ? 0 : -1;
  hist         = 0;
  nBinX        = (inBinX%2 == 0) ? inBinX:inBinX+1;
  nBinY        = (inBinY%2 == 0) ? inBinY:inBinY+1;
  xBinsPos     = new Double_t [nBinX];
  yBinsPos     = new Double_t [nBinY];
  size         = nBinX*nBinY;
  size         = (size/32 + ((size%32 > 0) ? 1:0))*32;
  constUncert  = 0.4;
  dDistCut     = 1.5;    // 1.0;
  dDistCutVF   = -1.0;   // 0.5;
  yDDistCorr   = new Double_t [nBinY];
  levelVertF   = 11;
  levelVertFPP = 12;
  lTargPnt     = 0;
  fakeSuppFlag = 1;
  Double_t dDistYCorr = 1.;
  useDriftTime = HMdcTrackDSet::useDriftTimeSeg1();
  HMdcTrackDSet::getDrTimeProjParSeg1(constUncert,dDistCut,dDistYCorr,dDCutCorr[0]);
  setDrTimeCutYCorr(dDistYCorr);
  fakeSuppFlag = HMdcTrackDSet::getGhostRemovingFlagSeg1();
  doVertexFn   = HMdcTrackDSet::getVertexFinderPar(dDistCutVF,levelVertF,levelVertFPP);

  Bool_t resize = size > hPlModsSize;
  if(resize) hPlModsSize=size;
  array = new TObjArray(4);
  for (Int_t mod = 0; mod < nSegs*2; mod++) {
    if( HMdcGetContainers::getObject()->isModActive(sec,mod) ) {
      HMdcLookUpTbMod* pMod = new HMdcLookUpTbMod(sec,mod);
      (*array)[mod] = pMod;
      pMod->setNLayers(HMdcTrackDSet::getNLayersInMod(sec,mod));
      nModules++;
    }
    if(hPlMod[mod] && !resize) continue;
    if(hPlMod[mod])  delete [] hPlMod[mod];
    hPlMod[mod] = new UChar_t [hPlModsSize];
    memset(hPlMod[mod],0,hPlModsSize);
  }

  if(plotBArSc==0 || sizeBArSt<size/8) {
    if(plotBArSc) delete [] plotBArSc;
    for(Int_t mod=0;mod<4;mod++) if(plotBArM[mod]) {
      delete [] plotBArM[mod];
      plotBArM[mod] = 0;
    }
    sizeBArSt=size/8;
    plotBArSc=new UChar_t [sizeBArSt];
    memset(plotBArSc,0,sizeBArSt);
    for(Int_t mod=0;mod<nSegs*2;mod++) {
      plotBArM[mod]=new UChar_t [sizeBArSt];
      memset(plotBArM[mod],0,sizeBArSt);
    }
  } else if(nSegs==2) {
    if(plotBArM[2]==0) plotBArM[2] = new UChar_t [sizeBArSt];
    if(plotBArM[3]==0) plotBArM[3] = new UChar_t [sizeBArSt];
  }
  sizeBAr       = size/8;
  typeClFinder  = 0;
  neighbBins[0] = -1;
  neighbBins[1] = +1;
  neighbBins[2] = +nBinX-1;
  neighbBins[3] = +nBinX;
  neighbBins[4] = +nBinX+1;
  neighbBins[5] = -nBinX-1;
  neighbBins[6] = -nBinX;
  neighbBins[7] = -nBinX+1;
  
  isGeant = HMdcGetContainers::isGeant();
  trackListFlag = isGeant;
  locClus.set(2,sec,0);
  pClustersArrs = HMdcClustersArrs::getObject();
  clusArrSize   = pClustersArrs->createAllArrays(10000);
  clusArr       = pClustersArrs->getArray1();
  clusArrM1     = pClustersArrs->getArray2();
  clusArrM2     = pClustersArrs->getArray3();
  layerOrder[0] = 2;
  layerOrder[1] = 3;
  layerOrder[2] = 1;
  layerOrder[3] = 4;
  layerOrder[4] = 0;
  layerOrder[5] = 5;
  if(size>clIndArrSzM1) {
    if(clusIndM1) delete [] clusIndM1;
    clusIndM1    = new Short_t [size];
    clIndArrSzM1 = size;
  }
  if(size>clIndArrSzM2) {
    if(clusIndM2) delete [] clusIndM2;
    clusIndM2    = new Short_t [size];
    clIndArrSzM2 = size;
  }
  for(Int_t mod=0;mod<4;mod++) {
    xMin[mod] = new UInt_t [nBinY];
    xMax[mod] = new UInt_t [nBinY];
    for(Int_t y=0;y<nBinY;y++) {
      xMin[mod][y] = size;
      xMax[mod][y] = 0;
    }
  }
  xMaxCl = new UShort_t [nBinY];
  xMinCl = new UShort_t [nBinY];
  
  HMdcEvntListCells* pEvLCells = HMdcEvntListCells::getExObject();
  pListCells = &((*pEvLCells)[sector]);
  
  pDriftTimeParSec = HMdcDriftTimePar::getObject()->at(sector);
  HMdcSizesCells *pSizesCells = HMdcSizesCells::getExObject();
  pSCellSec = &((*pSizesCells)[sector]);
}

HMdcLookUpTbSec::~HMdcLookUpTbSec(void) {
  // destructor
  if(array) {
    array->Delete();
    delete array;
  }
  for(Int_t mod=0; mod<4; mod++) {
    if(hPlMod[mod]) delete [] hPlMod[mod];
    hPlMod[mod]=0;
  }
  hPlModsSize=0;
  if(hist && hist->IsOnHeap()) delete hist;
  hist=0;
  if(plotBArSc) {
    delete [] plotBArSc;
    plotBArSc=0;
    sizeBArSt=0;
  }
  for(Int_t mod=0;mod<4;mod++) {
    if(plotBArM[mod]) {
      delete [] plotBArM[mod];
      plotBArM[mod]=0;
    }
  }
  delete [] xBinsPos;
  delete [] yBinsPos;
  delete [] yDDistCorr;
  for(Int_t mod=0;mod<4;mod++) {
    delete [] xMin[mod];
    delete [] xMax[mod];
  }

  HMdcClustersArrs::deleteCont();

  if(clusIndM1) {
    delete [] clusIndM1;
    clusIndM1=0;
    clIndArrSzM1=0;
  }
  if(clusIndM2) {
    delete [] clusIndM2;
    clusIndM2=0;
    clIndArrSzM2=0;
  }
  delete [] xMaxCl;
  delete [] xMinCl;
}

Bool_t HMdcLookUpTbSec::calcXYBounds(Double_t& xL,Double_t& xU,
                                     Double_t& yL,Double_t& yU) {
  HGeomVector senVol;
  HGeomVector cross;
  xL = yL =  1.0e+30;
  xU = yU = -1.0e+30;
  
  HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();
  if(fSizesCells == 0) return kFALSE;
  for(Int_t mod=0; mod<4; mod++) if( (*array)[mod] ) {
    for(Int_t layer=0; layer<6; layer++ /*layer+=5*/) {
      // Take size of first and last MDC layer only
      HMdcSizesCellsLayer* fSizesCellsLay = &((*fSizesCells)[sector][mod][layer]);
      if( !fSizesCells ) return kFALSE;
      HGeomVolume* fGeomVolLay = fSizesCellsLay->getGeomVolume();
      if(!fGeomVolLay || fGeomVolLay->getNumPoints()!=8) return kFALSE;
      HMdcLayerGeomParLay* pLayGmParLay = fSizesCellsLay->getLayerGeomParLay();
      if(pLayGmParLay == 0) return kFALSE;
      Double_t dstCathPl = pLayGmParLay->getCatDist()*0.5;
      for(Int_t point=0; point<8; point++) {
        senVol = *(fGeomVolLay->getPoint(point)); // mm!
        if(point<4) senVol.setZ(-dstCathPl);
        else        senVol.setZ( dstCathPl);
        senVol = fSizesCellsLay->getSecTrans()->transFrom(senVol);
        for(Int_t targ=0; targ<2; targ++) {
          prPlane.calcIntersection(targVc[targ],senVol-targVc[targ],cross);
          if(cross(0)<xL)      xL = cross(0);
          else if(cross(0)>xU) xU = cross(0);
          if(cross(1)<yL)      yL = cross(1);
          else if(cross(1)>yU) yU = cross(1);
        }
      }
    }
  }
  return kTRUE;
}

void HMdcLookUpTbSec::setPrPlotSize(Double_t xL,Double_t xU,
                                    Double_t yL,Double_t yU) {
  xLow  = xL;
  xUp   = xU;
  yLow  = yL;
  yUp   = yU;
  xStep = (xUp-xLow)/(Double_t)nBinX;
  yStep = (yUp-yLow)/(Double_t)nBinY;
  Double_t xHStep = xStep/2;
  Double_t yHStep = yStep/2;
  xFirstBin = xLow+xHStep;
  yFirstBin = yLow+yHStep;
  for(Int_t n=0; n<nBinX; n++) xBinsPos[n] = ((Double_t)n)*xStep+xLow+xHStep;
  for(Int_t n=0; n<nBinY; n++) yBinsPos[n] = ((Double_t)n)*yStep+yLow+yHStep;
}

void HMdcLookUpTbSec::clearPrArrs(void) {
  if(hPlMod[0]) memset(hPlMod[0],0,size);
  if(hPlMod[1]) memset(hPlMod[1],0,size);
  if(hPlMod[2]) memset(hPlMod[2],0,size);
  if(hPlMod[3]) memset(hPlMod[3],0,size);
}

void HMdcLookUpTbSec::clearPrMod(Int_t mod) {
  UChar_t* hPlModM=hPlMod[mod];
  if(!hPlModM) return;
  UInt_t* xMaxM = xMax[mod];
  UInt_t* xMinM = xMin[mod];
  for(Int_t y=0;y<nBinY;y++) {
    if(xMaxM[y]<xMinM[y]) continue;
    memset(hPlModM+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    xMinM[y] = size;
    xMaxM[y] = 0;
  }
}

void HMdcLookUpTbSec::clearPrMod(void) {
  // Cleaning of hPlMod[0] and hPlMod[1] by xMax[0]-xMinM[0]
  // This finction is used in findClusInSeg1TAmpCut only.
  UChar_t* hPlMod1 = hPlMod[0];
  UChar_t* hPlMod2 = hPlMod[1];
  UInt_t*  xMaxM   = xMax[0];
  UInt_t*  xMinM   = xMin[0];
  for(Int_t y=0;y<nBinY;y++) {
    if(xMaxM[y]<xMinM[y]) continue;
    memset(hPlMod1+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    memset(hPlMod2+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    xMinM[y] = size;
    xMaxM[y] = 0;
  }
}

void HMdcLookUpTbSec::clearPrModInSec(void) {
  // Cleaning of hPlMod[0] and hPlMod[1] by xMax[0]-xMinM[0]
  // This finction is used in findClusInSeg1TAmpCut only.
  UChar_t* hPlMod1 = hPlMod[0];
  UChar_t* hPlMod2 = hPlMod[1];
  UChar_t* hPlMod3 = hPlMod[2];
  UChar_t* hPlMod4 = hPlMod[3];
  UInt_t*  xMaxM   = xMax[0];
  UInt_t*  xMinM   = xMin[0];
  for(Int_t y=0;y<nBinY;y++) {
    if(xMaxM[y]<xMinM[y]) continue;
    memset(hPlMod1+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    memset(hPlMod2+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    memset(hPlMod3+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    memset(hPlMod4+xMinM[y],0,xMaxM[y]-xMinM[y]+1);
    xMinM[y] = size;
    xMaxM[y] = 0;
  }
}

Int_t HMdcLookUpTbSec::getSize(void) {
  // return the size of the pointer array
  if(array) return array->GetEntries();
  else return -1;
}

void HMdcLookUpTbSec::clearwk(void) {
  noFiredCells = kFALSE;
  isSlotAv[0]  = kTRUE;
  isSlotAv[1]  = kTRUE;
}

Int_t HMdcLookUpTbSec::xBinNumInBounds(Double_t x) {
  Int_t bin=xBinNum(x);
  if(bin < 1)            bin = 1;
  else if(bin > nBinX-2) bin = nBinX-2;
  return bin;
}

Int_t HMdcLookUpTbSec::yBinNumInBounds(Double_t y) {
  Int_t bin=yBinNum(y);
  if(bin < 1)            bin = 1;
  else if(bin > nBinY-2) bin = nBinY-2;
  return bin;
}

Bool_t HMdcLookUpTbSec::setLayerVar(void) {
  pLayLCells = &((*pListCells)[module][layer]);
  if(pLayLCells->getFirstCell() < 0) return kFALSE;
  pLUTLayer  = &((*cFMod)[layer]);
  add        = 1<<layer;
  cell       = -1;
  return kTRUE;
}

Bool_t HMdcLookUpTbSec::setNextCell(void) {
  if( !pLayLCells->nextCell(cell) ) return kFALSE;
  pLUTCell   = &((*pLUTLayer)[cell]);  
  pLUTLayer->setCurrentCell(cell);
  return kTRUE;
}

void HMdcLookUpTbSec::makeSPlot(void) {
  // Making proj.plot in sector(magnetoff) or segment(magneton).
  // Number of MDCs in sector or segment must be >1 !!!
  maxBinBAr4Sc = 0;
  minBinBAr4Sc = size;

  Int_t                lmod = -1;
  if     (minAmp[3]>0) lmod = 3;
  else if(minAmp[2]>0) lmod = 2;
  else if(minAmp[1]>0) lmod = 1;
  else if(minAmp[0]>0) lmod = 0;
  if(lmod<0) return;
  Int_t                fmod = 3;
  if     (minAmp[0]>0) fmod = 0;
  else if(minAmp[1]>0) fmod = 1;
  else if(minAmp[2]>0) fmod = 2;
  if(fmod==lmod) return;

  UChar_t minAm0 = minAmp[0];
  UChar_t minAm1 = minAmp[1];
  UChar_t minAm2 = minAmp[2];
  UChar_t minAmL = minAmp[lmod];
  for(module=fmod; module<=lmod; module++) {
    cFMod = &((*this)[module]);
    if( !cFMod ) continue;
    if(minAmp[module]==0) continue;
    cHPlModM = hPlMod[module];
    if(cHPlModM == NULL) continue;
    cXMinM = xMin[module];
    cXMaxM = xMax[module];

    Int_t nFiredLay=0;
    for(Int_t indLine=0; indLine<6; indLine++) {
      layer = layerOrder[indLine];
      if( !setLayerVar() ) continue;
      nFiredLay++;
      if(maxAmp[module]-nFiredLay+1>=minAmp[module]) {
        if(module==fmod)                                 makeLayProjV1();
        else if(module<lmod || nFiredLay<minAmp[module]) makeLayProjV1b();
        else while( setNextCell() ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbL>pXMaxM[y]) nbL = pXMaxM[y];
            if(nbF<pXMinM[y]) nbF = pXMinM[y];
            if(nbF>nbL) continue;
            if(nbF<cXMinM[y]) cXMinM[y] = nbF;
            if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
            Double_t yDDCorr = yDDistCorr[y];
            Double_t yb      = yBinsPos[y];
            UChar_t *bt      = cHPlModM+nbF;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              if(useDriftTime && !pLUTLayer->drTmTest2(xBinsPos[nb%nBinX],yb,yDDCorr)) continue;
              *bt |= add;
              if( HMdcBArray::getNSet(bt) < minAmL ) continue;
              if(fmod==0&&HMdcBArray::getNSet(hPlMod[0][nb])<minAm0) continue;
              if(module>1) {
                if(HMdcBArray::getNSet(hPlMod[1][nb])<minAm1) continue;
                if(module>2&&HMdcBArray::getNSet(hPlMod[2][nb])<minAm2) continue;
              }
              HMdcBArray::set(plotBArSc,nb);
              if(nb<minBinBAr4Sc) minBinBAr4Sc=nb;
              if(nb>maxBinBAr4Sc) maxBinBAr4Sc=nb;
            }
          }
        }
      } else {
        if(module<lmod || nFiredLay<minAmp[module]) makeLayProjV2();
        else  while(  setNextCell()  ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
            if(nbF<cXMinM[y]) nbF = cXMinM[y];
            if(nbF > nbL) continue;
            Double_t yDDCorr = yDDistCorr[y];
            Double_t yb      = yBinsPos[y];
            UChar_t *bt      = cHPlModM+nbF;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              if(useDriftTime && !pLUTLayer->drTmTest2(xBinsPos[nb%nBinX],yb,yDDCorr)) continue;
              *bt |= add;
              if( HMdcBArray::getNSet(bt) < minAmL ) continue;
              if(fmod==0&&HMdcBArray::getNSet(hPlMod[0][nb])<minAm0) continue;
              if(module>1) {
                if(HMdcBArray::getNSet(hPlMod[1][nb])<minAm1) continue;
                if(module>2&&HMdcBArray::getNSet(hPlMod[2][nb])<minAm2) continue;
              }
              HMdcBArray::set(plotBArSc,nb);
              if(nb<minBinBAr4Sc) minBinBAr4Sc=nb;
              if(nb>maxBinBAr4Sc) maxBinBAr4Sc=nb;
            }
          }
        }
      }
    }
    pXMinM = cXMinM;
    pXMaxM = cXMaxM;
  }
  maxBinBAr4Sc /= 32;
  minBinBAr4Sc /= 32;
}

void HMdcLookUpTbSec::makeS1PlotAmpCut(void) {
  // Making proj.plot in segment with cut on the total amplitude in 2 MDC's
  // Number of MDCs in sector or segment must be >1 !!!
  // Level of clusterfinder must be >=7 !!!!
//printf("sector=%i ==================== makeplot::::::::::::::::::::\n",sector);
  if(minAmp[0]<=0 || minAmp[1]<=0) return;
  Int_t seg1MinAmpCut = minAmp[0]+minAmp[1];
  if(typeClFinder==2) {
    seg1MinAmpCut = TMath::Min(minAmp[0],minAmp[1]) + 2;
    if(seg1MinAmpCut < 5) seg1MinAmpCut = 5;
  }
  maxBinBAr4Sc    = 0;
  minBinBAr4Sc    = size;

  Int_t nFiredLay = 0;
  Int_t maxAmpSeg = maxAmp[0]+maxAmp[1];
  if(seg1MinAmpCut > maxAmpSeg) return;

  cXMinM = xMin[0];
  cXMaxM = xMax[0];
  for(Int_t indLine=0; indLine<6; indLine++) {
    layer = layerOrder[indLine];
    for(module=0; module<2; module++) {
      cFMod = &((*this)[module]);
      if( !cFMod ) continue;
      if(minAmp[module]==0) continue;
      cHPlModM=hPlMod[module];
      if(!cHPlModM) continue;
//      cXMinM=xMin[module];
//      cXMaxM=xMax[module];
    
      if( !setLayerVar() ) continue;
      nFiredLay++;
      if(maxAmpSeg-nFiredLay+1 >= seg1MinAmpCut) {
        if(nFiredLay<seg1MinAmpCut) makeLayProjV1();
        else while(  setNextCell()  ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;

            UChar_t* bt  = cHPlModM+nbF;
            UChar_t* bt0 = module==0 ? hPlMod[1]+nbF:hPlMod[0]+nbF;
            if(useDriftTime) {
              Double_t  yDDCorr = yDDistCorr[y];
              Double_t  yb      = yBinsPos[y];
              Double_t *xb      = xBinsPos + nbF - shift;
              for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
                if(nb<cXMinM[y]) cXMinM[y] = nb;
                if(nb>cXMaxM[y]) cXMaxM[y] = nb;
                *bt |= add;
                UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
                if( wt >= seg1MinAmpCut ) {
                  HMdcBArray::set(plotBArSc,nb);
                  if(nb<minBinBAr4Sc) minBinBAr4Sc = nb;
                  if(nb>maxBinBAr4Sc) maxBinBAr4Sc = nb;
                }
              }
            } else {
              if(nbF<cXMinM[y]) cXMinM[y] = nbF;
              if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
              if(nbF > nbL) continue;
              for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++) {
                *bt |= add;
                UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
                if( wt >= seg1MinAmpCut ) {
                  HMdcBArray::set(plotBArSc,nb);
                  if(nb<minBinBAr4Sc) minBinBAr4Sc = nb;
                  if(nb>maxBinBAr4Sc) maxBinBAr4Sc = nb;
                }
              }
            }
          }
        }
      } else {
        if(nFiredLay<seg1MinAmpCut)           makeLayProjV2();
        else while(  setNextCell()  ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
            if(nbF<cXMinM[y]) nbF = cXMinM[y];
            if(nbF > nbL) continue;
            UChar_t* bt  = cHPlModM+nbF;
            UChar_t* bt0 = module==0 ? hPlMod[1]+nbF:hPlMod[0]+nbF;
            if(useDriftTime) {
              Double_t  yDDCorr = yDDistCorr[y];
              Double_t  yb      = yBinsPos[y];
              Double_t *xb      = xBinsPos + nbF - shift;
              for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
                *bt |= add;
                UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
                if( wt >= seg1MinAmpCut ) {
                  HMdcBArray::set(plotBArSc,nb);
                  if(nb<minBinBAr4Sc) minBinBAr4Sc = nb;
                  if(nb>maxBinBAr4Sc) maxBinBAr4Sc = nb;
                }
              }
            } else {
              for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++) {
                *bt |= add;
                UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
                if( wt >= seg1MinAmpCut ) {
                  HMdcBArray::set(plotBArSc,nb);
                  if(nb<minBinBAr4Sc) minBinBAr4Sc = nb;
                  if(nb>maxBinBAr4Sc) maxBinBAr4Sc = nb;
                }
              }
            }
          }
        }
      } 
    }
  }
  maxBinBAr4Sc /= 32;
  minBinBAr4Sc /= 32;
}

void HMdcLookUpTbSec::makeModPlot(Int_t mod) {
  module = mod;
  UChar_t minAm = minAmp[module];
  if(minAm<2) return;
  cFMod = &((*this)[module]);
  if( !cFMod ) return;
  cHPlModM = hPlMod[module];
  cXMinM   = xMin[module];
  cXMaxM   = xMax[module];
  UChar_t* cPlotBAr = plotBArM[module];
  UInt_t&  minBin   = minBinBAr4M[module];
  UInt_t&  maxBin   = maxBinBAr4M[module];
  maxBin = 0;
  minBin = size;
  Int_t nFiredLay = 0;
  for(Int_t indLine=0; indLine<6; indLine++) {
    layer = layerOrder[indLine];
    if( !setLayerVar() ) continue;
    nFiredLay++;
    if(maxAmp[module]-nFiredLay+1>=minAm) {  // determination min(max)Bin[y]
      if(nFiredLay<minAm) makeLayProjV1();  // filling pr.plot
      else while( setNextCell() ) {  // ...+ amp.checking
        // filling, min, max and bits seting  
        UInt_t y    = pLUTCell->getYBinMin();
        UInt_t nLns = pLUTCell->getNLines();
        for(UInt_t ln=0; ln<nLns; ln++,y++) {
          UInt_t shift = y * nBinX;
          UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
          UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
          if(nbF<cXMinM[y]) cXMinM[y] = nbF;
          if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
          UChar_t* bt = cHPlModM+nbF;
          if(useDriftTime) {
            Double_t  yDDCorr = yDDistCorr[y];
            Double_t  yb      = yBinsPos[y];
            Double_t *xb      = xBinsPos + nbF - shift;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
              *bt |= add;
              if(HMdcBArray::getNSet(bt) >= minAm) {
                HMdcBArray::set(cPlotBAr,nb);
                if(nb<minBin) minBin = nb;
                if(nb>maxBin) maxBin = nb;
              }
            }
          } else {
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              *bt |= add;
              if(HMdcBArray::getNSet(bt) >= minAm) {
                HMdcBArray::set(cPlotBAr,nb);
                if(nb<minBin) minBin = nb;
                if(nb>maxBin) maxBin = nb;
              }
            }
          }
        }
      }
    } else {                         // filling in minBin[y]-maxBin[y] only
      if(nFiredLay<minAm) makeLayProjV2();  // filling pr.plot
      else  while( setNextCell() ) {     // ...+ amp.checking
        UInt_t y    = pLUTCell->getYBinMin();
        UInt_t nLns = pLUTCell->getNLines();
        for(UInt_t ln=0; ln<nLns; ln++,y++) {
          UInt_t shift = y * nBinX;
          UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
          UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
          if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
          if(nbF<cXMinM[y]) nbF = cXMinM[y];
          if(nbF > nbL) continue;
          UChar_t* bt = cHPlModM+nbF;
          if(useDriftTime) {
            Double_t  yDDCorr = yDDistCorr[y];
            Double_t  yb      = yBinsPos[y];
            Double_t *xb      = xBinsPos + nbF - shift;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
              *bt |= add;
              if(HMdcBArray::getNSet(bt) >= minAm) {
                HMdcBArray::set(cPlotBAr,nb);
                if(nb<minBin) minBin = nb;
                if(nb>maxBin) maxBin = nb;
              }
            }
          } else {
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              *bt |= add;
              if(HMdcBArray::getNSet(bt) >= minAm) {
                HMdcBArray::set(cPlotBAr,nb);
                if(nb<minBin) minBin = nb;
                if(nb>maxBin) maxBin = nb;
              }
            }
          }
        }
      }
    }
  }
  maxBin /= 32;
  minBin /= 32;
}

void HMdcLookUpTbSec::makeLayProjV1(void) {
  // plot filling and filled region determination
  while( setNextCell() ) {
    UInt_t y     = pLUTCell->getYBinMin();
    UInt_t shift = y * nBinX;
    UInt_t nLns  = pLUTCell->getNLines();
    for(UInt_t ln=0; ln<nLns; ln++,y++,shift+=nBinX) {
      UInt_t nbL = pLUTCell->getXBinMax(ln)+shift;
      UInt_t nbF = pLUTCell->getXBinMin(ln)+shift;
      
      UChar_t* bt = cHPlModM+nbF;
      if(useDriftTime) {
        Double_t  yDDCorr = yDDistCorr[y];
        Double_t  yb      = yBinsPos[y];
        Double_t *xb      = xBinsPos + nbF - shift;
        for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
          *bt |= add;
          if(nb<cXMinM[y]) cXMinM[y] = nb;
          if(nb>cXMaxM[y]) cXMaxM[y] = nb;
        }
      } else {
        if(nbF<cXMinM[y]) cXMinM[y] = nbF;
        if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
        UChar_t* hPlModE = cHPlModM+nbL;
        for(; bt<=hPlModE; bt++) *bt |= add;
      }
    }
  }
}

void HMdcLookUpTbSec::makeLayProjV0(void) {
  // plot filling only (for TH2C)
  while( setNextCell() ) {
    UInt_t y     = pLUTCell->getYBinMin();
    UInt_t shift = y * nBinX;
    UInt_t nLns  = pLUTCell->getNLines();
    for(UInt_t ln=0; ln<nLns; ln++,y++,shift+=nBinX) {
      UInt_t nbL = pLUTCell->getXBinMax(ln)+shift;
      UInt_t nbF = pLUTCell->getXBinMin(ln)+shift;
      
      UChar_t* bt = cHPlModM+nbF;
      if(useDriftTime) {
        Double_t  yDDCorr = yDDistCorr[y];
        Double_t  yb      = yBinsPos[y];
        Double_t *xb      = xBinsPos + nbF - shift;
        for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,xb++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) {
          *bt |= add;
        }
      } else {
        UChar_t* hPlModE = cHPlModM+nbL;
        for(; bt<=hPlModE; bt++) *bt |= add;
      }
    }
  }
}

void HMdcLookUpTbSec::makeLayProjV1b(void) {
  // plot filling and filled region determination in region determined
  // in previous mdc
  while( setNextCell() ) {
    UInt_t y    = pLUTCell->getYBinMin();
    UInt_t nLns = pLUTCell->getNLines();
    for(UInt_t ln=0; ln<nLns; ln++,y++) {
      UInt_t shift = y * nBinX;
      UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
      UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
      if(nbL>pXMaxM[y]) nbL = pXMaxM[y];
      if(nbF<pXMinM[y]) nbF = pXMinM[y];
      if(nbF>nbL) continue;
      if(nbF<cXMinM[y]) cXMinM[y] = nbF;
      if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
      UChar_t* bt = cHPlModM+nbF;
      if(useDriftTime) {
        Double_t  yDDCorr = yDDistCorr[y];
        Double_t  yb      = yBinsPos[y];
        Double_t *xb      = xBinsPos + nbF - shift;
        Double_t *xl      = xBinsPos + nbL - shift;
        for(; xb<=xl; xb++,bt++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) *bt |= add;
      } else {
        UChar_t* hPlModE = cHPlModM+nbL;
        for(; bt<=hPlModE; bt++) *bt |= add;
      }
    }
  }
}

void HMdcLookUpTbSec::makeLayProjV2(void) {
  // plot filling in filled regions only
  while( setNextCell() ) {
    UInt_t y    = pLUTCell->getYBinMin();
    UInt_t nLns = pLUTCell->getNLines();
    for(UInt_t ln=0; ln<nLns; ln++,y++) {
      UInt_t shift = y * nBinX;
      UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
      UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
      if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
      if(nbF<cXMinM[y]) nbF = cXMinM[y];
      if(nbF > nbL) continue;
      UChar_t* bt = cHPlModM+nbF;
      if(useDriftTime) {
        Double_t  yDDCorr = yDDistCorr[y];
        Double_t  yb      = yBinsPos[y];
        Double_t *xb      = xBinsPos + nbF - shift;
        Double_t *xl      = xBinsPos + nbL - shift;
        for(; xb<=xl; xb++,bt++) if(pLUTLayer->drTmTest2(*xb,yb,yDDCorr)) *bt |= add;
      } else {
        UChar_t* hPlModE = cHPlModM+nbL;
        for(; bt<=hPlModE; bt++) *bt |= add;
      }
    }
  }
}

Int_t HMdcLookUpTbSec::findClusters(Int_t *imax) {
  if(nModules == 0) return 0;
  nClusters    = 0;
  //isCoilOff - !!!??? ---------------------------------------
  nMods        = 0;
  noFiredCells = kTRUE;
  for(Int_t mod=0; mod<4; mod++) {
    if(mod < maxNModules) {
      maxAmp[mod] = (*pListCells)[mod].getNLayers();
      minAmp[mod] = imax[mod];
      if(minAmp[mod]>0) nMods++;
      if(maxAmp[mod]>0) noFiredCells = kFALSE;
    } else {
      maxAmp[mod] = 0;
      minAmp[mod] = 0;
    }
  }
  if( noFiredCells ) return 0;
  if(typeClFinder==1) {
    // chamber clust. finding
    for(Int_t m=0; m<4; m++) if(minAmp[m]>0) findClusInMod(m);
  } else {
    // combined clust. finding
    nModSeg[0]=nModSeg[1]=0;
    for(Int_t m=0;m<4;m++) if(minAmp[m]>0) nModSeg[m/2] |= m%2+1;
    if(nModSeg[0]>0 && nModSeg[1]>0) findClusInSec();
    else {
      if(nModSeg[0]==3) {
        if(isCoilOff)                              findClusInSeg(0);
        else if(HMdcTrackDSet::useSegAmpCutSeg1()) findClusInSeg(0);
        else                                       findClusInSeg1();
      } else if(nModSeg[0]>0)                      findClusInMod(nModSeg[0]-1);
      if(nModSeg[1]==3)                            findClusInSeg(1);
      else if(nModSeg[1]>0)                        findClusInMod(nModSeg[1]+1);
    }
  }
  return nClusters;
}

void HMdcLookUpTbSec::initCluster(Int_t nBin) {
  clus = &cClusArr[*cNClusArr];
  clus->init(sector,segment,xFirstBin,yFirstBin,xStep,yStep);
          // segment=0-inner segment, segment=-1-sector track
  Int_t ny      = nBin/nBinX;
  nLMinCl       = nLMaxCl    = ny;
  xMinCl[ny]    = xMaxCl[ny] = nBin%nBinX;
  isClstrInited = kTRUE;
}

void HMdcLookUpTbSec::reinitCluster(Int_t nBin) {
  // Use it if list of wires empty yet!
  clus->clearBinStat();
  Int_t ny   = nBin/nBinX;
  nLMinCl    = nLMaxCl    = ny;
  xMinCl[ny] = xMaxCl[ny] = nBin%nBinX;
}

void HMdcLookUpTbSec::initCluster(Int_t nBin,UChar_t amp) {
  clus = &cClusArr[*cNClusArr];
  clus->init(sector,segment,xFirstBin,yFirstBin,xStep,yStep);
          // segment=0-inner segment, segment=-1-sector track
  Int_t ny   = nBin/nBinX;
  nLMinCl    = nLMaxCl    = ny;
  xMinCl[ny] = xMaxCl[ny] = nBin%nBinX;
  isClstrInited = kTRUE;
  addBinInCluster(nBin,amp);
}

void HMdcLookUpTbSec::reinitCluster(Int_t nBin,UChar_t amp) {
  // Use it if list of wires empty yet!
  clus->clearBinStat();
  Int_t ny   = nBin/nBinX;
  nLMinCl    = nLMaxCl    = ny;
  xMinCl[ny] = xMaxCl[ny] = nBin%nBinX;
  addBinInCluster(nBin,amp);
}

void HMdcLookUpTbSec::addBinInCluster(Int_t nBin,UChar_t wt) {
  Int_t nx = nBin%nBinX;
  Int_t ny = nBin/nBinX;
  clus->addBin(nx,ny,wt);
  // ny-nLMaxCl and nLMinCl-ny can't be >1 !
  if(ny<nLMinCl) {
    nLMinCl = ny;
    xMinCl[ny] = xMaxCl[ny] = nx;
  } else if(ny>nLMaxCl) {
    nLMaxCl = ny;
    xMinCl[ny] = xMaxCl[ny] = nx;
  }
  else if(nx<xMinCl[ny]) xMinCl[ny]=nx;
  else if(nx>xMaxCl[ny]) xMaxCl[ny]=nx;
}

void HMdcLookUpTbSec::initClusterT2(Int_t nBin,UChar_t amp) {
  initCluster(nBin);
  addBinInClusterT2(nBin,amp);
}

void HMdcLookUpTbSec::reinitClusterT2(Int_t nBin,UChar_t amp) {
  reinitCluster(nBin);
  addBinInClusterT2(nBin,amp);
}

void HMdcLookUpTbSec::addBinInClusterT2(Int_t nBin,UChar_t wt) {
  // The same as "addBinInCluster" plus clusters chamber clusters setting
  Int_t nx = nBin%nBinX;
  Int_t ny = nBin/nBinX;
  clus->addBin(nx,ny,wt);
  // ny-nLMaxCl and nLMinCl-ny can't be >1 !
  if(ny<nLMinCl) {
    nLMinCl = ny;
    xMinCl[ny] = xMaxCl[ny] = nx;
  } else if(ny>nLMaxCl) {
    nLMaxCl = ny;
    xMinCl[ny] = xMaxCl[ny] = nx;
  }
  else if(nx<xMinCl[ny]) xMinCl[ny]=nx;
  else if(nx>xMaxCl[ny]) xMaxCl[ny]=nx;
  
  if(clus->getMod1Clus()==0 && clusIndM1[nBin]>=0)
      clus->setMod1ClusSkipMerg(&clusArrM1[clusIndM1[nBin]]);
  if(clus->getMod2Clus()==0 && clusIndM2[nBin]>=0)
      clus->setMod2ClusSkipMerg(&clusArrM2[clusIndM2[nBin]]);
}

Int_t HMdcLookUpTbSec::getClusterSlot(Int_t seg, HMdcList12GroupCells& list) {
  locClus[1]=seg;
  Int_t index;
  fClus = (HMdcClus*)fClusCat->getNewSlot(locClus,&index);
  if(!fClus) {
    Warning("getClusterSlot","Sec.%i Segment %i No slot HMdcClus available",sector+1,seg+1);
    isSlotAv[seg] = kFALSE;
    return -1;
  }
  if(isGeant) fClus = (HMdcClus*)(new(fClus) HMdcClusSim(list));
  else        fClus = new(fClus) HMdcClus(list);
  fClus->setSecSegInd(sector,seg,index);
  return index;
}

Bool_t HMdcLookUpTbSec::fillSecCluster(void) {
  clus->calcXY();
  if(minAmp[0]>0) fillModWiresList(0,clus->getLCells1());
  if(minAmp[1]>0) fillModWiresList(1,clus->getLCells1());
  if(minAmp[2]>0) fillModWiresList(2,clus->getLCells2());
  if(minAmp[3]>0) fillModWiresList(3,clus->getLCells2());
  return increaseClusterNum();
}

Bool_t HMdcLookUpTbSec::fillSegCluster(void) {
  clus->calcXY();
  fillModWiresList(cSeg*2,  clus->getLCells1());
  fillModWiresList(cSeg*2+1,clus->getLCells1());
  return increaseClusterNum();
}

Bool_t HMdcLookUpTbSec::fillModCluster(Int_t mod) {
  clus->calcXY();
  fillModWiresList(mod,clus->getLCells1());
  return increaseClusterNum();
}

void HMdcLookUpTbSec::fillModWiresList(Int_t mod, HMdcList12GroupCells& list) {
  module = mod;
  cFMod = &((*this)[module]);
  if( cFMod == 0 ) return;
  Int_t layAdd = (module%2)*6;
  for(layer=0; layer<6; layer++) {
    if( !setLayerVar() ) continue;
    Int_t clLay = layAdd+layer;
    while( setNextCell() ) {
//?W      if(hitsDel[iLayer][i]<0) continue;
      Short_t yBinMax = pLUTCell->getYBinMax();
      if( nLMinCl > yBinMax ) continue;
      Short_t yBinMin=pLUTCell->getYBinMin();
      if( nLMaxCl < yBinMin ) break;    //        Hits are sorted !!!
      Short_t lMax = (yBinMax < nLMaxCl) ? yBinMax : nLMaxCl;
      Short_t lMin = (yBinMin > nLMinCl) ? yBinMin : nLMinCl;
      Bool_t   nextCell   = kFALSE;
      for (Short_t nl=lMin; nl<=lMax; nl++) {
        if( pLUTCell->getXBinMin(nl-yBinMin) > xMaxCl[nl] ||
            pLUTCell->getXBinMax(nl-yBinMin) < xMinCl[nl] ) continue;

        Double_t yb  = yBinsPos[nl];
        Int_t    xMi = TMath::Max(pLUTCell->getXBinMin(nl-yBinMin), xMinCl[nl]);
        Int_t    xMa = TMath::Min(pLUTCell->getXBinMax(nl-yBinMin), xMaxCl[nl]);
        if(useDriftTime) {
          Double_t yDDCorr = yDDistCorr[nl];
          for(Int_t x=xMi;x<=xMa;x++) if(pLUTLayer->drTmTest2(xBinsPos[x],yb,yDDCorr)) {
            list.setTime(clLay,cell,pLayLCells->getTime(cell));
            nextCell = kTRUE;
            break;
          }
        } else {
          for(Int_t x=xMi;x<=xMa;x++) {
            list.setTime(clLay,cell,pLayLCells->getTime(cell));
            nextCell = kTRUE;
            break;
          }
        }
        if(nextCell) break;
      }
    }
  }
}

void HMdcLookUpTbSec::findClusInSeg(Int_t seg){
  cMod1      = seg*2;
  cMod2      = cMod1+1;
  cSeg       = seg;
  if(maxAmp[cMod1]<minAmp[cMod1] || maxAmp[cMod2]<minAmp[cMod2]) return;
  if(minAmp[cMod1]+minAmp[cMod2] < 3) return;
  
  if(seg==0 && HMdcTrackDSet::useSegAmpCutSeg1()) makeS1PlotAmpCut();
  else                                            makeSPlot();
  setCurrentArraySec();
  nClsArr  = 0;
  cPlModF  = hPlMod[cMod1];
  cPlModS  = hPlMod[cMod2];
  cPlotBAr = plotBArSc;
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg1();
  
  UInt_t* plotBAr4b = (UInt_t*)cPlotBAr;
  //---Cluster finding-------------------------------------
  for(UInt_t n4=maxBinBAr4Sc; n4>=minBinBAr4Sc; n4--) {
    if(plotBAr4b[n4]==0) continue;
    UChar_t *b1    = cPlotBAr+n4*4;
    UChar_t *b2    = b1+3;
    Int_t    nBin4 = n4*32;
    Int_t    nLBin = 33;
    if(useFloatLevel) while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcClusterInSegFloatLevel(nBin4+nLBin));
    else              while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcClusterInSegFixedLevel(nBin4+nLBin));
    if(*cNClusArr >= clusArrSize) break;
  }

  if(nClsArr>0) {
    if(nClsArr>1) mergeClusInSeg();
    calcClParam();
    fillClusCat(-2,seg,0);
  }
  if(seg==0 && HMdcTrackDSet::useSegAmpCutSeg1()) clearPrMod();
  else for(Int_t mod=seg*2;mod<(seg+1)*2;mod++) if(minAmp[mod]>0) clearPrMod(mod);
}

Bool_t HMdcLookUpTbSec::calcClusterInSegFixedLevel(Int_t nBin) {
  initCluster(nBin,HMdcBArray::getNSet(cPlModF[nBin])+ HMdcBArray::getNSet(cPlModS[nBin]));
  stack->init();
  do {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if(HMdcBArray::testAndUnset(cPlotBAr,nb)) {
        stack->push(nb);
        addBinInCluster(nb,HMdcBArray::getNSet(cPlModF[nb])+
                           HMdcBArray::getNSet(cPlModS[nb]));
      }
    }
  } while ((nBin=stack->pop()) >= 0);
  if( fillSegCluster() ) return kTRUE; // Filling of cluster
  return kFALSE;
}

Bool_t HMdcLookUpTbSec::calcClusterInSegFloatLevel(Int_t nBin) {
  UChar_t amp = HMdcBArray::getNSet(cPlModF[nBin])+HMdcBArray::getNSet(cPlModS[nBin]);
  initCluster(nBin,amp);
  stacksArr->init(12);
  while (kTRUE) {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if( !HMdcBArray::testAndUnset(cPlotBAr,nb) ) continue;
      UChar_t ampN = HMdcBArray::getNSet(cPlModF[nb])+HMdcBArray::getNSet(cPlModS[nb]);
      stacksArr->push(nb,ampN);
      if(ampN  > amp) {
        // Increasing amplitude:
        if(nTs<7) stacksArr->push(nBin,amp);
        amp = ampN;
        if(isClstrInited) reinitCluster(nb,amp);
        else              initCluster(nb,amp);
        break;
      } else if(ampN==amp && isClstrInited) addBinInCluster(nb,amp);
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-------------------------
    if(isClstrInited && !fillSegCluster()) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
  }
  return kTRUE;
}
    
void HMdcLookUpTbSec::findClusInSeg1(void){
  if(typeClFinder==0) {
    if(maxAmp[0]<minAmp[0] || maxAmp[1]<minAmp[1]) return;
  } else if(maxAmp[0]<minAmp[0] && maxAmp[1]<minAmp[1]) return;
  if(minAmp[0]+minAmp[1]<3) return;
  makeModPlot(0);
  makeModPlot(1);

  minBinBAr4Sc=(minBinBAr4M[0]>=minBinBAr4M[1])?minBinBAr4M[0]:minBinBAr4M[1];
  maxBinBAr4Sc=(maxBinBAr4M[0]<=maxBinBAr4M[1])?maxBinBAr4M[0]:maxBinBAr4M[1];
  UInt_t* plotBAr4bM1=(UInt_t *)(plotBArM[0]);
  UInt_t* plotBAr4bM2=(UInt_t *)(plotBArM[1]);
  UInt_t* plotBArSc4b=(UInt_t *)plotBArSc;
  for(UInt_t n4=minBinBAr4Sc; n4<=maxBinBAr4Sc; n4++)
      plotBArSc4b[n4] = plotBAr4bM1[n4] & plotBAr4bM2[n4];

  setCurrentArrayMod1();
  scanPlotInMod(0);
  if(nClsArrM1>1) mergeClusInMod(0);
  calcClParam();

  setCurrentArrayMod2();
  scanPlotInMod(1);
  if(nClsArrM2>1) mergeClusInMod(1);
  calcClParam();

  setCurrentArraySec();
  scanPlotInSeg1(0,plotBArSc);

  if(nClsArr>1) mergeClusInSeg();
  testClusMod12toSeg();
  if(nClsArrM1>0 && nClsArrM2>0) mergeClusMod1to2();
  if(nClsArr>0) {
    calcClParam();
    fillClusCat(-2,0,0);
  }
  
  if(typeClFinder==2) {
    setCurrentArrayMod1();
    fillClusCat(0,0,2);
    setCurrentArrayMod2();
    fillClusCat(1,0,2);
  }
  
  clearPrMod(0);
  clearPrMod(1);
}

void HMdcLookUpTbSec::scanPlotInMod(Int_t mod) {
  // Scan proj.plot in one module but determination of wires list in segment(!)
  cMod              = mod;
  (*cNClusArr)      = 0;
  cSeg              = mod/2;
  cPlMod            = hPlMod[mod];
  cPlotBAr          = plotBArM[mod];
  UInt_t* plotBAr4b = (UInt_t*) cPlotBAr;
  UInt_t minBin     = minBinBAr4M[mod];
  clusInd           = mod==0 ? clusIndM1:clusIndM2;
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg1();
  for(UInt_t n4=maxBinBAr4M[mod]; n4>=minBin; n4--) {
    if(plotBAr4b[n4]==0) continue;
    UChar_t *b1=cPlotBAr+n4*4;
    UChar_t *b2=b1+3;
    Int_t nBin4=n4*32;
    Int_t nLBin=33;
    if(useFloatLevel) while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcMixedClusterFloatLevel(nBin4+nLBin));
    else              while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcMixedClusterFixedLevel(nBin4+nLBin));
    if((*cNClusArr) >= clusArrSize) break;
  }
}

Bool_t HMdcLookUpTbSec::calcMixedClusterFixedLevel(Int_t nBin) {
  initCluster(nBin,HMdcBArray::getNSet(cPlMod[nBin]));
  clusInd[nBin]=*cNClusArr;
  stack->init();
  do {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if(HMdcBArray::testAndUnset(cPlotBAr,nb)) {
        stack->push(nb);
        addBinInCluster(nb,HMdcBArray::getNSet(cPlMod[nb]));
        clusInd[nb]=*cNClusArr;
      }
    }
  } while ((nBin=stack->pop()) >= 0);
  if(fillSegCluster()) return kTRUE; // Filling of cluster
  return kFALSE;
}

Bool_t HMdcLookUpTbSec::calcMixedClusterFloatLevel(Int_t nBin) {
  UChar_t amp = HMdcBArray::getNSet(cPlMod[nBin]);
  initCluster(nBin,amp);
  clusInd[nBin]=*cNClusArr;
  stacksArr->init(12);
  stack->init();
  if(HMdcBArray::get(plotBArSc,nBin)) stack->push(nBin);
  while (kTRUE) {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if( !HMdcBArray::testAndUnset(cPlotBAr,nb) ) continue;
      UChar_t ampN = HMdcBArray::getNSet(cPlMod[nb]);
      stacksArr->push(nb,ampN);
      if(ampN  > amp) {
        // Increasing amplitude:
        clusInd[nBin] = -1;
        if(nTs<7) stacksArr->push(nBin,amp);
        amp = ampN;
        Int_t bind;
        while((bind=stack->pop()) >= 0) clusInd[bind] = -1;
        if(isClstrInited) reinitCluster(nb,amp);
        else              initCluster(nb,amp);
        clusInd[nb]=*cNClusArr;
        if(HMdcBArray::get(plotBArSc,nb)) stack->push(nb);
        break;
      } else if(ampN==amp && isClstrInited) {
        addBinInCluster(nb,amp);
        clusInd[nb]=*cNClusArr;
        if(HMdcBArray::get(plotBArSc,nb)) stack->push(nb);
      } else clusInd[nb] = -1;
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-------------------------
    if(isClstrInited && !fillSegCluster()) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
    stack->init();
  }
  return kTRUE;
}
    
void HMdcLookUpTbSec::scanPlotInSeg1(Int_t seg, UChar_t* plotBAr) {
  cMod1      = seg*2;
  cMod2      = cMod1+1;
  cSeg       = seg;
  cPlModF    = hPlMod[cMod1];
  cPlModS    = hPlMod[cMod2];
  cPlotBAr   = plotBAr;
  *cNClusArr = 0;        // number of clusters
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg1();
  
  UInt_t* plotBAr4b = (UInt_t*)cPlotBAr;
  for(UInt_t n4=maxBinBAr4Sc; n4>=minBinBAr4Sc; n4--) {
    if(plotBAr4b[n4]==0) continue;
    UChar_t *b1    = cPlotBAr+n4*4;
    UChar_t *b2    = b1+3;
    Int_t    nBin4 = n4*32;
    Int_t    nLBin = 33;
    if(useFloatLevel) while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                             calcClusterInSeg1FloatLevel(nBin4+nLBin));
    else              while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                             calcClusterInSeg1FixedLevel(nBin4+nLBin));
    if(*cNClusArr >= clusArrSize) break;
  }
  if(useFloatLevel) testSeg1ModClMatching();
}

void HMdcLookUpTbSec::testSeg1ModClMatching() {
  HMdcList12GroupCells listIdent;
  for(Int_t segCl=0; segCl<*cNClusArr; segCl++) {
    HMdcCluster& clus = clusArr[segCl];
    for(Int_t imod=0;imod<2;imod++) {
      if(clus.getModCluster(imod) != 0) continue;  // already setted
      Int_t        nClsArrM = imod == 0 ? nClsArrM1 : nClsArrM2;
      HMdcCluster* clusArrM = imod == 0 ? clusArrM1 : clusArrM2;
      Int_t nLayers         = 0;
      Int_t nWires          = 0;
      Int_t nLayersOp       = 0;
      Int_t nWiresOp        = 0;
      HMdcCluster* modClust = 0;
      for(Int_t cl=0;cl<nClsArrM;cl++) {
        HMdcCluster& pCls=clusArrM[cl];
        if(pCls.getClusMerg() != 0) continue;
        clus.getLCells1().compare(&(pCls.getLCells1()),0,11,&listIdent);
        Int_t nLayersNext   = listIdent.getNLayersMod(imod);
        Int_t nLayersNextOp = listIdent.getNLayersMod(imod^1);
        Int_t nWiresNext    = listIdent.getNDrTimesMod(imod);
        Int_t nWiresNextOp  = listIdent.getNDrTimesMod(imod^1);
        if(nLayersNext==0 || nLayersNext < nLayers) continue;
        if(nLayersNext==nLayers) {
          if(nWiresNext < nWires) continue;
          if(nWiresNext == nWires) {
            if(nLayersNextOp < nLayersOp) continue;
            if(nLayersNextOp == nLayersOp && nWiresNextOp <= nWiresOp) continue;
          }
        }
        modClust  = &pCls;
        nLayers   = nLayersNext;
        nWires    = nWiresNext;
        nLayersOp = nLayersNextOp;
        nWiresOp  = nWiresNextOp;
      }
      if(modClust) clus.setModCluster(imod,modClust);
    }
  }
}

Bool_t HMdcLookUpTbSec::calcClusterInSeg1FixedLevel(Int_t nBin) {
  initClusterT2(nBin,HMdcBArray::getNSet(cPlModF[nBin])+
                     HMdcBArray::getNSet(cPlModS[nBin]));
  stack->init();            // init stack only
  do {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if(HMdcBArray::testAndUnset(cPlotBAr,nb)) {
        stack->push(nb);
        addBinInCluster(nb,HMdcBArray::getNSet(cPlModF[nb])+
                           HMdcBArray::getNSet(cPlModS[nb]));
      }
    }
  } while ((nBin=stack->pop()) >= 0);
  if( fillSegCluster() ) return kTRUE;
  return kFALSE;
}

Bool_t HMdcLookUpTbSec::calcClusterInSeg1FloatLevel(Int_t nBin) {
  UChar_t amp = HMdcBArray::getNSet(cPlModF[nBin])+HMdcBArray::getNSet(cPlModS[nBin]);
  initClusterT2(nBin,amp);
  stacksArr->init(12);
  while (kTRUE) {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if( !HMdcBArray::testAndUnset(cPlotBAr,nb) ) continue;
      UChar_t ampN = HMdcBArray::getNSet(cPlModF[nb])+HMdcBArray::getNSet(cPlModS[nb]);
      stacksArr->push(nb,ampN);
      if(ampN  > amp) {
        // Increasing amplitude:
        if(nTs<7) stacksArr->push(nBin,amp);
        amp = ampN;
        if(isClstrInited) reinitClusterT2(nb,amp);
        else              initClusterT2(nb,amp);
        break;
      } else if(ampN==amp && isClstrInited) addBinInClusterT2(nb,amp);
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-------------------------
    if(isClstrInited && !fillSegCluster()) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
  }
  return kTRUE;
}
    
Bool_t HMdcLookUpTbSec::increaseClusterNum(void) {
  if(*cNClusArr > 0) pClustersArrs->testCluster(clusArrInd,*cNClusArr);
  (*cNClusArr)++;
  isClstrInited = kFALSE;
  if(*cNClusArr < clusArrSize) return kTRUE;
  *cNClusArr = clusArrSize;
  Warning("increaseClusterNum"," Num.of clusters in sector %i > max\n",sector);
  memset(cPlotBAr,0,sizeBAr);
  return kFALSE;
}

void HMdcLookUpTbSec::findClusInSec(void){
//!  for(Int_t m=0;m<4;m++) if(minAmp[m]>0 && maxAmp[m]<minAmp[m]) return;
  if(minAmp[0]+minAmp[1]+minAmp[2]+minAmp[3] < 3) return;
  makeSPlot();
  setCurrentArraySec();
  nClsArr  = 0;
  cPlotBAr = plotBArSc;
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg1();
  
  UInt_t* plotBAr4b = (UInt_t*) cPlotBAr;
  //---Cluster finding-------------------------------------
  for(UInt_t n4=maxBinBAr4Sc; n4>=minBinBAr4Sc; n4--) {
    if(plotBAr4b[n4]==0) continue;
    UChar_t *b1    = cPlotBAr+n4*4;
    UChar_t *b2    = b1+3;
    Int_t    nBin4 = n4*32;
    Int_t    nLBin = 33;
    if(useFloatLevel) while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcClusterInSecFloatLevel(nBin4+nLBin));
    else              while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                            calcClusterInSecFixedLevel(nBin4+nLBin));
    if(nClsArr >= clusArrSize) break;
  }
  if(nClsArr>0) {
    if(nClsArr>1) mergeClusInSec();
    calcClParam();
    fillClusCat(-nMods,-1,0);
  }
  for(Int_t mod=0;mod<4;mod++) if(minAmp[mod]>0) clearPrMod(mod);
}

Bool_t HMdcLookUpTbSec::calcClusterInSecFixedLevel(Int_t nBin) {
  initCluster(nBin,HMdcBArray::getNSet(hPlMod[0][nBin])+
                   HMdcBArray::getNSet(hPlMod[1][nBin])+
                   HMdcBArray::getNSet(hPlMod[2][nBin])+
                   HMdcBArray::getNSet(hPlMod[3][nBin]));
  stack->init();
  do {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if(HMdcBArray::testAndUnset(cPlotBAr,nb)) {
        stack->push(nb);
        addBinInCluster(nb,HMdcBArray::getNSet(hPlMod[0][nb])+
                           HMdcBArray::getNSet(hPlMod[1][nb])+
                           HMdcBArray::getNSet(hPlMod[2][nb])+
                           HMdcBArray::getNSet(hPlMod[3][nb]));
      }
    }
  } while ((nBin=stack->pop()) >= 0);
  if( fillSecCluster() ) return kTRUE; // Filling of cluster
  return kFALSE;
}

Bool_t HMdcLookUpTbSec::calcClusterInSecFloatLevel(Int_t nBin) {
  UChar_t amp = HMdcBArray::getNSet(hPlMod[0][nBin])+
                HMdcBArray::getNSet(hPlMod[1][nBin])+
                HMdcBArray::getNSet(hPlMod[2][nBin])+
                HMdcBArray::getNSet(hPlMod[3][nBin]);
  initCluster(nBin,amp);
  stacksArr->init(24);
  while (kTRUE) {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if( !HMdcBArray::testAndUnset(cPlotBAr,nb) ) continue;
      UChar_t ampN = HMdcBArray::getNSet(hPlMod[0][nb])+
                     HMdcBArray::getNSet(hPlMod[1][nb])+
                     HMdcBArray::getNSet(hPlMod[2][nb])+
                     HMdcBArray::getNSet(hPlMod[3][nb]);
      stacksArr->push(nb,ampN);
      if(ampN  > amp) {
        // Increasing amplitude:
        if(nTs<7) stacksArr->push(nBin,amp);
        amp = ampN;
        if(isClstrInited) reinitCluster(nb,amp);
        else              initCluster(nb,amp);
        break;
      } else if(ampN==amp && isClstrInited) addBinInCluster(nb,amp);
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-------------------------
    if(isClstrInited && !fillSecCluster()) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
  }
  return kTRUE;
}


void HMdcLookUpTbSec::mergeClusInMod(Int_t mod) {
  if(fakeSuppFlag>=0) return;
  Int_t nClus = *cNClusArr;
  Int_t fLay  = mod==0 || mod==2 ? 0:6;
  Int_t lLay  = fLay+5;
  while(nClus>1) {
    Bool_t nomerg = kTRUE;
    for(Int_t cl1=0; cl1<(*cNClusArr)-1; cl1++) {
      HMdcCluster& cls1 = cClusArr[cl1];
      if(!cls1.getStatus()) continue;
      HMdcList12GroupCells& listCells1 = cls1.getLCells1();
      for(Int_t cl2=cl1+1; cl2<(*cNClusArr); cl2++) {
        HMdcCluster& cls2 = cClusArr[cl2];
        if(!cls2.getStatus()) continue;
        HMdcList12GroupCells& listCells2 = cls2.getLCells1();
        Float_t dY = cls1.getY()-cls2.getY();
// ???optimizaciya???        if(dY>100.) break;  //????????? biylo zakomentirovano ???
        if(TMath::Abs(dY) > 30.f) continue;                         // 10. mm !???
        if(TMath::Abs(cls1.getX()-cls2.getX()) > 100.f) continue;   // 40. mm !???
        if(listCells1.compare(&listCells2,fLay,lLay)<4) continue;
        cls1.addClus(cls2);
        nomerg = kFALSE;
        nClus--;
      }
    }
    if(nomerg || nClus==1) break;
  }
}

void HMdcLookUpTbSec::mergeClusMod1to2(void) {
  if(fakeSuppFlag>=0) return;
  for(Int_t cl1=0; cl1<nClsArrM1; cl1++) {
    HMdcCluster& cls1=clusArrM1[cl1];
    if(!cls1.getStatus() || cls1.getFlag()>0) continue;
    HMdcList12GroupCells& listCells1 = cls1.getLCells1();
    for(Int_t cl2=0; cl2<nClsArrM2; cl2++) {
      HMdcCluster& cls2=clusArrM2[cl2];
      if(!cls2.getStatus() || cls2.getFlag()>0) continue;
      HMdcList12GroupCells& listCells2=cls2.getLCells1();
      Float_t dY = cls1.getY()-cls2.getY();
// ???optimizaciya???      if(dY>100.) break;  // ????????? biylo zakomentirovano ???
      if(TMath::Abs(dY) > 30.f) continue;                         // 10. mm !???
      if(TMath::Abs(cls1.getX()-cls2.getX()) > 100.f) continue;   // 40. mm !???
      if(listCells1.compare(&listCells2)<4) continue;
//      if(listCells1.compare(&listCells2,0, 5)<4) continue;
//      if(listCells1.compare(&listCells2,6,11)<4) continue;
      if(nClsArr >= clusArrSize) break;
      HMdcCluster& cls = clusArr[nClsArr++];
      cls.init(sector,segment,xFirstBin,yFirstBin,xStep,yStep);
      cls.sumClus(cls1,cls2);
    }
  }
}

void HMdcLookUpTbSec::testClusMod12toSeg(void) {
  // excluding clusters in MOD with <4 non identical wires to segment clusters
  for(Int_t cl=0; cl<nClsArr; cl++) {
    HMdcCluster& cls=clusArr[cl];
    if(!cls.getStatus() || cls.getFlag()>0) continue;
    HMdcList12GroupCells& lCells=cls.getLCells1();
    for(Int_t cl1=0; cl1<nClsArrM1; cl1++) {
      HMdcCluster& cls1=clusArrM1[cl1];
      if(!cls1.getStatus() || cls1.getFlag()>0) continue;
      HMdcList12GroupCells& lM1Cells1=cls1.getLCells1();
      if(TMath::Abs(cls.getY()-cls1.getY()) > 30.f) continue;         // 10. mm !???
      if(TMath::Abs(cls.getX()-cls1.getX()) > 100.f) continue;        // 40. mm !???
      if(lM1Cells1.getNCells()-lCells.nIdentDrTimes(&lM1Cells1) >= 4) continue;
      cls1.setFlag(1);
    }
    for(Int_t cl2=0; cl2<nClsArrM2; cl2++) {
      HMdcCluster& cls2=clusArrM2[cl2];
      if(!cls2.getStatus() || cls2.getFlag()>0) continue;
      HMdcList12GroupCells& lM1Cells2=cls2.getLCells1();
      if(TMath::Abs(cls.getY()-cls2.getY()) > 30.f) continue;         // 10. mm !???
      if(TMath::Abs(cls.getX()-cls2.getX()) > 100.f) continue;        // 40. mm !???
      if(lM1Cells2.getNCells()-lCells.nIdentDrTimes(&lM1Cells2) >= 4) continue;
      cls2.setFlag(1);
    }
  }
}

void HMdcLookUpTbSec::mergeClusInSeg(void) {
  if(fakeSuppFlag>=0) return;
  Int_t nClus=nClsArr;
  while(nClus>1) {
    Bool_t nomerg = kTRUE;
    for(Int_t cl1=0; cl1<nClsArr-1; cl1++) {
      HMdcCluster& cls1=clusArr[cl1];
      if(!cls1.getStatus()) continue;
      HMdcList12GroupCells& listCells1 = cls1.getLCells1();
      for(Int_t cl2=cl1+1; cl2<nClsArr; cl2++) {
        HMdcCluster& cls2=clusArr[cl2];
        if(!cls2.getStatus()) continue;

        if( cls1.isModClusNotTheSame(cls2) ) continue;

        HMdcList12GroupCells& listCells2=cls2.getLCells1();
        Float_t dY = cls1.getY()-cls2.getY();
// ???optimizaciya???        if(dY>100.) break;
        if(TMath::Abs(dY) > 30.f) continue;                         // 10. mm !???
        if(TMath::Abs(cls1.getX()-cls2.getX()) > 100.f) continue;   // 40. mm !???
        Int_t n1=listCells1.compare(&listCells2,0, 5);
        if(n1<3) continue;
        Int_t n2=listCells1.compare(&listCells2,6,11);
        if(n2<3 || n1+n2<7) continue;
//        if(listCells1.compare(&listCells2,0, 5)<4) continue;
//        if(listCells1.compare(&listCells2,6,11)<4) continue;
  if(cls1.getMod1Clus()==0) cls1.setMod1Clus(cls2.getMod1Clus());
  if(cls1.getMod2Clus()==0) cls1.setMod2Clus(cls2.getMod2Clus());  
        cls1.addClus(cls2);
        nomerg=kFALSE;
        nClus--;
      }
    }
    if(nomerg || nClus==1) break;
  }
}

void HMdcLookUpTbSec::mergeClusInSec(void) {
  if(fakeSuppFlag>=0) return;
  Int_t nClus = nClsArr;
  Int_t m1    = minAmp[0];
  Int_t m2    = minAmp[1];
  Int_t m3    = minAmp[2];
  Int_t m4    = minAmp[3];
  while(nClus>1) {
    Bool_t nomerg=kTRUE;
    for(Int_t cl1=0; cl1<nClsArr-1; cl1++) {
      HMdcCluster& cls1=clusArr[cl1];
      if(!cls1.getStatus()) continue;
      HMdcList12GroupCells& listCells1 = cls1.getLCells1();
      HMdcList12GroupCells& listCells2 = cls1.getLCells2();
      for(Int_t cl2=cl1+1; cl2<nClsArr; cl2++) {
        HMdcCluster& cls2=clusArr[cl2];
        if(!cls2.getStatus()) continue;
        Float_t dY = cls1.getY()-cls2.getY();
// ???optimizaciya???        if(dY>100.) break;
        if(TMath::Abs(dY) > 30.f) continue;                          // 10. mm !???
        if(TMath::Abs(cls1.getX()-cls2.getX()) > 100.f) continue;    // 40. mm !???
        HMdcList12GroupCells* listCells1s = &(cls2.getLCells1());
        HMdcList12GroupCells* listCells2s = &(cls2.getLCells2());
        if(m1>0 && listCells1.compare(listCells1s,0, 5)<4) continue;
        if(m2>0 && listCells1.compare(listCells1s,6,11)<4) continue;
        if(m3>0 && listCells2.compare(listCells2s,0, 5)<4) continue;
        if(m4>0 && listCells2.compare(listCells2s,6,11)<4) continue;
        cls1.addClus(cls2);
        nomerg=kFALSE;
        nClus--;
      }
    }
    if(nomerg || nClus==1) break;
  }
}

void HMdcLookUpTbSec::calcClParam(void) {
  for(Int_t nCl=0; nCl<(*cNClusArr); nCl++)
    if(cClusArr[nCl].getStatus()) cClusArr[nCl].calcClusParam();
}

void HMdcLookUpTbSec::fillClusCat(Int_t mod, Int_t segp, Int_t tpClFndr) {
  
  if(mod<0 && segp==0   && fakeSuppFlag>0) removeGhosts(); // ????????????????
  
  Int_t seg = segp>=0 ? segp : 0;
  Int_t m1  = minAmp[seg*2];
  Int_t m2  = minAmp[seg*2+1];
  Int_t m3  = 0;
  Int_t m4  = 0;
  if(mod>=0) {
    if(mod==seg*2) m2=0;
    else if(mod==seg*2+1) m1=0;
  } else if(segp<0) {
    m3 = minAmp[2];
    m4 = minAmp[3];
  }
  if( HMdcTrackDSet::useFloatLevelSeg1() ) tpClFndr |= 128;
  if( HMdcTrackDSet::useSegAmpCutSeg1() )  tpClFndr |= 256;
  // Filling containers:
  Float_t tarX,tarY,tarZ, tarErrX,tarErrY,tarErrZ;
  if(vertexPoint>=0) {
    tarX    = targetPnts[vertexPoint].getX();
    tarY    = targetPnts[vertexPoint].getY();
    tarZ    = targetPnts[vertexPoint].getZ();
    tarErrX = 0.;
    tarErrY = 0.;
    tarErrZ = vertZErr;
  } else {
    tarX    = target[0];
    tarY    = target[1];
    tarZ    = target[2];
    tarErrX = eTarg[0];
    tarErrY = eTarg[1];
    tarErrZ = eTarg[2];
  }
  for(Int_t cl=0; cl<(*cNClusArr); cl++) {
    HMdcCluster& cls = cClusArr[cl];
    if(!cls.getStatus() || cls.getFlag()>0) continue;
    if(fakeSuppFlag==1 && cls.getFakeFlag()>0) continue;
    Int_t index1 = getClusterSlot(seg,cls.getLCells1());
    if(!fClus) return;
    nClusters++;
    fClus->setMod(mod);
    fClus->setTypeClFinder(tpClFndr); // !!!
    fClus->setMinCl(m1,m2);
    fClus->setPrPlane(prPlane.A(),prPlane.B(),prPlane.D());
    fClus->setTarget(tarX,tarErrX,tarY,tarErrY,tarZ,tarErrZ);
    cls.fillClus(fClus,0,trackListFlag);
    if(segp<0) {
      HMdcClus* fClus0 = fClus;
      Int_t index2 = getClusterSlot(1,cls.getLCells2());
      if(!fClus) return;
      fClus->setMod(mod);
      fClus->setTypeClFinder(tpClFndr);
      fClus->setMinCl(m3,m4);
      fClus->setPrPlane(prPlane.A(),prPlane.B(),prPlane.D());
      fClus->setTarget(tarX,tarErrX,tarY,tarErrY,tarZ,tarErrZ);
      cls.fillClus(fClus,1,trackListFlag);
      fClus->setIndexParent(index1);
      fClus0->setIndexChilds(index2,index2);
    }
  }
}

void HMdcLookUpTbSec::findClusInMod(Int_t mod){
  if(maxAmp[mod]<minAmp[mod]) return;
  if(minAmp[mod]<3) return;
  makeModPlot(mod);
  setCurrentArraySec();
  cMod              = mod;
  nClsArr           = 0;
  cPlMod            = hPlMod[mod];
  cPlotBAr          = plotBArM[mod];
  UInt_t* plotBAr4b = (UInt_t*) cPlotBAr;
  UInt_t minBin     = minBinBAr4M[mod];
  Bool_t useFloatLevel = HMdcTrackDSet::useFloatLevelSeg1();
  
  for(UInt_t n4=maxBinBAr4M[mod]; n4>=minBin; n4--) {
    if(plotBAr4b[n4]==0) continue;
    UChar_t *b1    = cPlotBAr+n4*4;
    UChar_t *b2    = b1+3;
    Int_t    nBin4 = n4*32;
    Int_t    nLBin = 33;
    if(useFloatLevel) while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                             calcClusterInModFloatLevel(nBin4+nLBin)); 
    else              while((nLBin=HMdcBArray::prevAndUnset(b1,b2,nLBin))>=0 &&
                             calcClusterInModFixedLevel(nBin4+nLBin));
    if(nClsArr >= clusArrSize) break;
  }
  if(nClsArr>0) {
    if(nClsArr>1) mergeClusInMod(mod);
    calcClParam();
    fillClusCat(mod,mod/2,(typeClFinder==1) ? 1:0);
  }
  clearPrMod(mod);
}

Bool_t   HMdcLookUpTbSec::calcClusterInModFixedLevel(Int_t nBin) {
  initCluster(nBin,HMdcBArray::getNSet(cPlMod[nBin]));
  stack->init();
  do {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if(HMdcBArray::testAndUnset(cPlotBAr,nb)) {
        stack->push(nb);
        addBinInCluster(nb,HMdcBArray::getNSet(cPlMod[nb]));
      }
    }
  } while ((nBin=stack->pop()) >= 0);
  if( fillModCluster(cMod) ) return kTRUE; // Filling of cluster
  return kFALSE;
}

Bool_t   HMdcLookUpTbSec::calcClusterInModFloatLevel(Int_t nBin) {
  UChar_t amp = HMdcBArray::getNSet(cPlMod[nBin]);
  initCluster(nBin,amp);
  stacksArr->init(6);
  while (kTRUE) {
    for(Int_t nTs=0; nTs<8; nTs++) {
      Int_t nb = nBin+neighbBins[nTs];
      if( !HMdcBArray::testAndUnset(cPlotBAr,nb) ) continue;
      UChar_t ampN = HMdcBArray::getNSet(cPlMod[nb]);
      stacksArr->push(nb,ampN);
      if(ampN  > amp) {
        // Increasing amplitude:
        if(nTs<7) stacksArr->push(nBin,amp);
        amp = ampN;
        if(isClstrInited) reinitCluster(nb,amp);
        else              initCluster(nb,amp);
        break;
      } else if(ampN==amp && isClstrInited) addBinInCluster(nb,amp);
    }
    UChar_t ampP = stacksArr->pop(nBin);
    if(ampP == amp) continue;
    //-End of cluster region. Filling of cluster:-------------------------
    if(isClstrInited && !fillModCluster(cMod)) return kFALSE;
    if(nBin<0) break; // No more bins in stack!
    amp = ampP;
  }
  return kTRUE;
}

Int_t HMdcLookUpTbSec::getMaxClus(Int_t m) const {
  return (m>=0&&m<4) ? minAmp[m] : minAmp[0]+minAmp[1]+minAmp[2]+minAmp[3];
}

Bool_t HMdcLookUpTbSec::calcLookUpTb(Bool_t quiet) {
  HMdcSizesCells *fSizesCells = HMdcSizesCells::getExObject();
  if(fSizesCells == 0) return kFALSE;
  Int_t nTargets = fSizesCells->getNumOfTargets();
  HMdcSizesCellsSec& fSCSec = (*fSizesCells)[sector];
  HMdcTrap cellSize,cellSPr;
  HMdcTrapPlane cellPr,cellCont[2];
  for(Int_t mod=0;mod<4;mod++) if((*array)[mod]) for(Int_t lay=0;lay<6;lay++) {
    HMdcSizesCellsLayer& fSCLayer = fSCSec[mod][lay];
    Int_t nCells=fSCLayer.getNCells();
    if(nCells<1) continue;
    HMdcLookUpTbLayer& fLUpTbLayer=(*this)[mod][lay];
    Double_t dDist    = 0.;  // It is better when dDist increase with cell num.
    Double_t dDist1T  = 0.;  // It is better when dDist increase with cell num.
    Double_t dDistF   = -1.; // For print only
    Double_t dDist1TF = -1.; // For print only
    for(Int_t cell=0; cell<nCells; cell++) {
      HMdcLookUpTbCell& fLUpTbCell=fLUpTbLayer[cell];
      if( !&fLUpTbCell ) return kFALSE;
      fLUpTbCell.clear();
      HMdcSizesCellsCell& fSizesCellsCell = fSCLayer[cell];
      if( !&fSizesCellsCell || !fSizesCellsCell.getStatus()) continue;
      if(!fSizesCells->getCellVol(sector,mod,lay,cell,cellSize)) return kFALSE;      
      for(Int_t t=0; t<2; t++) {
        HGeomVector& targ = targVc[t];
        for(Int_t p=0; p<8; p++)
            prPlane.calcIntersection(targ,cellSize[p]-targ,cellSPr[p]);
        cellSPr.setDbPointNum(cellSize.getDbPointNum());
        if(cellSPr.getXYContour(cellCont[t]) < 3) return kFALSE;
      }
      if(cellPr.twoContoursSum(cellCont[0],cellCont[1]) < 3) return kFALSE;
      fillLookUpTb(mod,lay,cellPr,fLUpTbCell);
      
      if(useDriftTime || doVertexFn) {
        HGeomVector cr0;
        Double_t alphaMean = 0.;
        for(Int_t ip=0;ip<2;ip++) {  // Ends of wire
          const HGeomVector& wr = *(fSizesCellsCell.getWirePoint(ip));
          prPlane.calcIntersection(targVc[2],wr-targVc[2],cr0); //
          for(Int_t t=0; t<2; t++) { // First and last target points
            Double_t dist,alpha;
            fSCLayer.getImpact(targVc[t],cr0,cell, alpha,dist);
            alphaMean += alpha;
            if(dist>dDist) dDist = dist;
          }
          // For vertex finder:
          for(Int_t t=0; t<nTargets ; t += nTargets==1 ? 1:nTargets-1) {
            HGeomVector tar = *(fSCSec.getTarget(t));
            prPlane.calcIntersection(tar,wr-tar,cr0); // Cross with proj.plane
            for(Int_t sign=-1;sign<=1;sign+=2) {
              HGeomVector tarP(tar.getX(),tar.getY(),tar.getZ()+sign*vertZErr);
              for(Int_t ib=0;ib<4;ib++) {
                Double_t signX = ib==0 || ib==1 ? -1. : +1.;
                Double_t signY = ib==0 || ib==3 ? -1. : +1.;
                Double_t x = cr0.getX()+signX*xStep/2.;
                Double_t y = cr0.getY()+signY*yStep/2.;
                Double_t z = prPlane.getZOnPlane(x,y);
                HGeomVector binP(x,y,z);
                Double_t dist = fSCLayer.getDist(tarP,binP,cell);
                if(dist>dDist1T) dDist1T = dist;
              }
            }
          }
        }
        alphaMean /= 4.;
        
        fLUpTbCell.setAlphaMean(alphaMean);
        fLUpTbCell.setDistCutFT(  (dDist   + constUncert)*dDistCut*dDCutCorr[mod][lay]);
        fLUpTbCell.setDistCut1T(  (dDist1T + constUncert)*dDistCut*dDCutCorr[mod][lay]);
        fLUpTbCell.setDistCut1TVF((dDist1T + constUncert)*dDistCutVF); // *dDCutCorr[mod][lay] ???

        if(dDistF<0.)   dDistF   = dDist;
        if(dDist1TF<0.) dDist1TF = dDist1T;
      }
    }
    if(useDriftTime && !quiet) {
      printf(" S.%i M.%i L.%i : Proj.Uncer.= %.2f - %.2f  Cut = %.2f |",
             sector+1,mod+1,lay+1,dDistF+constUncert,dDist+constUncert,dDistCut*dDCutCorr[mod][lay]);
      printf(" 1Tar.Uncer.= %.3f - %.3f [mm]\n",dDist1TF+constUncert,dDist1T+constUncert);
    }
  }
  return kTRUE;
}

Bool_t HMdcLookUpTbSec::fillLookUpTb(Int_t m, Int_t l,HMdcTrapPlane& cellPr,
                                       HMdcLookUpTbCell& fCell) {
  // MDC cell lookup table (fCell) filling.
  // m - module; l - layer;
  Int_t yBinMin = yBinNumInBounds(cellPr.getYMin());
  Int_t yBinMax = yBinNumInBounds(cellPr.getYMax());
  fCell.init(yBinMin,yBinMax);
  for(Int_t ny=yBinMin; ny<=yBinMax; ny++) {
    Double_t x1,x2;
    Double_t y1 = ny*yStep + yLow;
    if(!cellPr.getXminXmax(y1,y1+yStep,x1,x2)) return kFALSE;
    if(!fCell.addLine(xBinNumInBounds(x1),xBinNumInBounds(x2))) return kFALSE;
  }
  return kTRUE;
}

void HMdcLookUpTbSec::calcTarget(Double_t* targLenInc) {
  targVc[0]  = pSCellSec->getTargetFirstPoint();
  targVc[1]  = pSCellSec->getTargetLastPoint();
  targVc[0].setZ(targVc[0].getZ() - targLenInc[0]);
  targVc[1].setZ(targVc[1].getZ() + targLenInc[1]);
  targVc[2]  = targVc[0]+targVc[1];
  targVc[2] *= 0.5;
  for(Int_t i=0;i<3;i++) {
    target[i] = targVc[2](i);
    eTarg[i]  = TMath::Abs(targVc[0](i)-targVc[1](i))*0.5;
  }
  
  HGeomVector targRotLay;
  Double_t cnstMinDist[6];
  for(Int_t m=0;m<4;m++) if((*array)[m]) for(Int_t l=0;l<6;l++) {
    HMdcSizesCellsLayer& fSCellLay = (*pSCellSec)[m][l];
    
    Int_t nLayParts = fSCellLay.getLayerNParts();
    for(Int_t ip=0;ip<nLayParts;ip++) {
      targRotLay = fSCellLay.getRotLayP1SysRSec()->transTo(targVc[2]);
      const Double_t* trns = fSCellLay.getRLP1SysRSec();
      if(ip==1) {
        targRotLay = fSCellLay.getRotLayP2SysRSec()->transTo(targVc[2]);
        trns       = fSCellLay.getRLP2SysRSec();
      } 
      Double_t c1 = prPlane.D()-trns[11];
      cnstMinDist[0] = trns[1] - prPlane.A()*trns[7];                    // C1x
      cnstMinDist[1] = trns[4] - prPlane.B()*trns[7];                    // C1y
      cnstMinDist[2] = trns[7]*c1-trns[1]*trns[9]-trns[4]*trns[10];      // C1
      //!!! y0 = C1x*x2 + C1y*y2 + C1;
      cnstMinDist[3] = trns[2] - prPlane.A()*trns[8];                    // C2x
      cnstMinDist[4] = trns[5] - prPlane.B()*trns[8];                    // C2y
      cnstMinDist[5] = trns[8]*c1-trns[2]*trns[9]-trns[5]*trns[10];      // C2
      //!!! z0 = C2x*x2 + C2y*y2 + C2;

      (*this)[m][l].setMinDistLUTb(ip,targRotLay,cnstMinDist);
    }
  }
}

Bool_t HMdcLookUpTb::calcVertexFnTarg(void) {
  // For vertex finder:
  Int_t nTargets = fSizesCells->getNumOfTargets();
  if(nTargets>240) {
    Error("calcTarget","Number of targets =%i (>240) !!!",nTargets);
    nTargets = 0;
    return kFALSE;
  }
  vertZErr = fSizesCells->getMaxTargHThick();
  if(nTargets==1) { // Nado uchect' mishenyami
    const HGeomVector& targetFPoint = fSizesCells->getTargetFirstPoint(); 
    const HGeomVector& targetLPoint = fSizesCells->getTargetLastPoint();
    const HGeomVector& targetMPoint = fSizesCells->getTargetMiddlePoint();
    Double_t length = targetLPoint.getZ() - targetFPoint.getZ();
    if(length<3.) {
      numVFPoins   = 1;
      use3PointMax = kFALSE;
    } else {
      if(length<5.5)     numVFPoins = 2;
      else               numVFPoins = Int_t(length/3.+0.5);
      if(numVFPoins>240) numVFPoins = 240; // Array size limitation.
      use3PointMax = kTRUE;
      Double_t pntZStep = length/numVFPoins;
      numVFPoins += 2;
      Double_t z0 = targetFPoint.getZ() - pntZStep/2.;
      for(Int_t tp=0;tp<numVFPoins;tp++) {
        targetPnts[tp] = targetMPoint;
        targetPnts[tp].setZ(z0 + pntZStep*tp);
      }
      vertZErr = pntZStep;
    }
  } else {
    HGeomVector* targets   = fSizesCells->getTargets();
// ???Double_t maxTargHThick = fSizesCells->getMaxTargHThick();
    Double_t length        = targets[nTargets-1].getZ() - targets[0].getZ();
    Double_t pntZStep      = length/nTargets;
    if(pntZStep>10.) { // No additional points for vertex finder:
      numVFPoins   = nTargets;
      use3PointMax = kFALSE;
      for(Int_t t=0;t<nTargets;t++) targetPnts[t] = targets[t];
    } else {          // Add 2 points:
      numVFPoins   = nTargets+2;
      use3PointMax = kTRUE;
      for(Int_t t=0;t<nTargets;t++) targetPnts[t+1] = targets[t];
      targetPnts[0]            = targets[0];
      targetPnts[numVFPoins-1] = targets[nTargets-1];
      targetPnts[0].setZ           (targets[0].getZ()          - pntZStep);
      targetPnts[numVFPoins-1].setZ(targets[nTargets-1].getZ() + pntZStep);      
    }
  }

  Double_t zStart = HMdcTrackDSet::getZStartDetector();
  if( fStartGeomPar != NULL ) zStart = fStartGeomPar->getLabPos(0)->getZ();
  
  numStartDPoints = 0;
  if((HMdcGetContainers::isGeant() && gHades->getEmbeddingMode()==0) || zStart==0 || zStart >= targetPnts[0].getZ()) return kTRUE;
  if(use3PointMax) {
    Double_t dz = targetPnts[1].getZ() - targetPnts[0].getZ();
    for(Int_t i=0;i<3;i++) targetPnts[numVFPoins+1] = targetPnts[0]; // Copy x and y
    targetPnts[numVFPoins].setZ(zStart-dz);
    targetPnts[numVFPoins+1].setZ(zStart);
    targetPnts[numVFPoins+2].setZ(zStart+dz);
    numStartDPoints = 3;
  } else {
    targetPnts[numVFPoins] = targetPnts[0];
    targetPnts[numVFPoins].setZ(zStart);
    numStartDPoints = 1;
  }
  return kTRUE;
}

void HMdcLookUpTbSec::calcVertexFnTarg(Int_t nTrPnts,HGeomVector* trPnts) {
  // For vertex finder:

  lTargPnt = nTrPnts;
  const HGeomTransform* labSecTrans = pSCellSec->getLabTrans();
  
  for(Int_t n=0;n<nTrPnts;n++) targetPnts[n] = labSecTrans->transTo(trPnts[n]);
  
  HGeomVector tmp;
  for(Int_t m=0;m<4;m++) if((*array)[m]) for(Int_t l=0;l<6;l++) {
    const HGeomTransform* transRLaySec   = (*pSCellSec)[m][l].getRotLayP1SysRSec();
    const HGeomTransform* transRLayP2Sec = (*pSCellSec)[m][l].getRotLayP2SysRSec();
    HMdcLookUpTbLayer& rLUTLay = (*this)[m][l];
    Double_t* yTArr   = rLUTLay.getYTargetArr();
    Double_t* zTArr   = rLUTLay.getZTargetArr();
    Double_t* yTArrP2 = rLUTLay.getYTargetArrP2();
    Double_t* zTArrP2 = rLUTLay.getZTargetArrP2();
    indFirstTPnt = 0;
    indLastTPnt  = 0;
    for(Int_t n=0;n<nTrPnts;n++) {
      if(targetPnts[n].getZ() < targetPnts[indFirstTPnt].getZ()) indFirstTPnt = n;
      if(targetPnts[n].getZ() > targetPnts[indLastTPnt ].getZ()) indLastTPnt  = n;
      tmp = transRLaySec->transTo(targetPnts[n]);
      yTArr[n] = tmp.getY();
      zTArr[n] = tmp.getZ();
      if(yTArrP2!=NULL && zTArrP2!=NULL) {
        tmp = transRLayP2Sec->transTo(targetPnts[n]);
        yTArrP2[n] = tmp.getY();
        zTArrP2[n] = tmp.getZ();
      }
    }
    rLUTLay.setNTargPnts(nTrPnts);
  }
}

void HMdcLookUpTbSec::removeGhosts(void) {
   //===Suppression==
  pListCells->clearClustCounter(0);
  
  Int_t sizeLv12p0 = 2; Int_t realUnVsBn_Lv12p0[2] ={5,4};        // element [i+1] must be <= [i]
  Int_t sizeLv12p1 = 1; Int_t fakeUnVsBn_Lv12p1[1] ={1};          //
  Int_t sizeLv12p2 = 3; Int_t fakeUnVsRl_Lv12p2[3] ={6,7,9};      //
  Int_t sizeLv12p3 = 4; Int_t realUnVsBn_Lv12p3[4] ={5,4,4,3};    // element [i+1] must be <= [i]
  Int_t sizeLv12p4 = 3; Int_t fakeUnVsRl_Lv12p4[3] ={6,7,9};      //
  Int_t sizeLv11p1 = 1; Int_t realUnVsBn_Lv11p1[1] ={7};          // element [i+1] must be <= [i]
  Int_t sizeLv11p2 = 3; Int_t fakeUnVsRl_Lv11p2[3] ={4,6,7};      //
  Int_t sizeLv11p3 = 1; Int_t realUnVsBn_Lv11p3[1] ={7};          // element [i+1] must be <= [i]
  Int_t sizeLv11p4 = 4; Int_t fakeUnVsRl_Lv11p4[4] ={4,5,6,7};    //
  Int_t sizeLv10p1 = 1; Int_t realUnVsBn_Lv10p1[1] ={9};          // element [i+1] must be <= [i]
  Int_t sizeLv10p2 = 4; Int_t fakeUnVsRl_Lv10p2[4] ={3,4,5,6};    //
  Int_t sizeLv10p3 = 1; Int_t realUnVsBn_Lv10p3[1] ={9};          // element [i+1] must be <= [i]
  Int_t sizeLv10p4 = 5; Int_t fakeUnVsRl_Lv10p4[5] ={1,3,3,4,5};  //
  Int_t sizeLv10p5 = 1; Int_t realUnVsBn_Lv10p5[1] ={9};          // element [i+1] must be <= [i]
  Int_t sizeLv10p6 = 5; Int_t fakeUnVsRl_Lv10p6[5] ={1,2,3,4,5};  //
  Int_t sizeLv09p1 = 1; Int_t realUnVsBn_Lv09p1[1] ={6};          // element [i+1] must be <= [i]
  Int_t sizeLv09p2 = 4; Int_t fakeUnVsRl_Lv09p2[4] ={3,4,5,6};    //
  Int_t sizeLv09p3 = 1; Int_t realUnVsBn_Lv09p3[1] ={6};          // element [i+1] must be <= [i]
  Int_t sizeLv09p4 = 4; Int_t fakeUnVsRl_Lv09p4[5] ={2,3,5,6};    //
  Int_t sizeLv09p5 = 1; Int_t realUnVsBn_Lv09p5[1] ={6};          // element [i+1] must be <= [i]
  Int_t sizeLv09p6 = 5; Int_t fakeUnVsRl_Lv09p6[5] ={3,3,5,5,13}; //
// Lev8 ver1 (Lev9 is previous)
//  Int_t sizeLv08p1 = 1; Int_t realUnVsBn_Lv08p1[1] ={3};          // element [i+1] must be <= [i]
//  Int_t sizeLv08p2 = 4; Int_t fakeUnVsRl_Lv08p2[4] ={2,4,4,6};    //
//  Int_t sizeLv08p3 = 1; Int_t realUnVsBn_Lv08p3[1] ={3};          // element [i+1] must be <= [i]
//  Int_t sizeLv08p4 = 4; Int_t fakeUnVsRl_Lv08p4[4] ={1,2,4,5};    //
//  Int_t sizeLv08p5 = 1; Int_t realUnVsBn_Lv08p5[1] ={4};          // element [i+1] must be <= [i]
//  Int_t sizeLv08p6 = 6; Int_t fakeUnVsRl_Lv08p6[6] ={0,0,0,1,2,5}; //
  
  // Level 12:  ------------------------------------------------------------------------
  addToClusCounter(12); 
  markReals(12,sizeLv12p0,realUnVsBn_Lv12p0);
  markFakesNBins(12,sizeLv12p1,fakeUnVsBn_Lv12p1);
  markFakes(12,sizeLv12p2,fakeUnVsRl_Lv12p2);
  markReals(12,sizeLv12p3,realUnVsBn_Lv12p3);
  markFakes(12,sizeLv12p4,fakeUnVsRl_Lv12p4);
  
  // Level 11: ------------------------------------------------------------------------
  addToClusCounter(11);
  markReals(11,sizeLv11p1,realUnVsBn_Lv11p1);
  markFakes(11,sizeLv11p2,fakeUnVsRl_Lv11p2);
  markReals(11,sizeLv11p3,realUnVsBn_Lv11p3);
  markFakes(11,sizeLv11p4,fakeUnVsRl_Lv11p4);

  // Level 10: ------------------------------------------------------------------------
  addToClusCounter(10);
  markReals(10,sizeLv10p1,realUnVsBn_Lv10p1);
  markFakes(10,sizeLv10p2,fakeUnVsRl_Lv10p2);
  markReals(10,sizeLv10p3,realUnVsBn_Lv10p3);
  markFakes(10,sizeLv10p4,fakeUnVsRl_Lv10p4);
  markReals(10,sizeLv10p5,realUnVsBn_Lv10p5);
  markFakes(10,sizeLv10p6,fakeUnVsRl_Lv10p6);

  // Level 09: ------------------------------------------------------------------------
  addToClusCounter(9);
  markReals(9,sizeLv09p1,realUnVsBn_Lv09p1);
  markFakes(9,sizeLv09p2,fakeUnVsRl_Lv09p2);
  markReals(9,sizeLv09p3,realUnVsBn_Lv09p3);
  markFakes(9,sizeLv09p4,fakeUnVsRl_Lv09p4);
  markReals(9,sizeLv09p5,realUnVsBn_Lv09p5);
  markFakes(9,sizeLv09p6,fakeUnVsRl_Lv09p6);

  // Level 08: ------------------------------------------------------------------------
//  addToClusCounter(8);
//  markReals(8,sizeLv08p1,realUnVsBn_Lv08p1);
//  markFakes(8,sizeLv08p2,fakeUnVsRl_Lv08p2);
//  markReals(8,sizeLv08p3,realUnVsBn_Lv08p3);
//  markFakes(8,sizeLv08p4,fakeUnVsRl_Lv08p4);
//  markReals(8,sizeLv08p5,realUnVsBn_Lv08p5);
//  markFakes(8,sizeLv08p6,fakeUnVsRl_Lv08p6);
}

void HMdcLookUpTbSec::addToClusCounter(Int_t clusAmp) {
  for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(clusAmp)) {
    pListCells->addToClusCounters(0,cClusArr[cl].getLCells1());
  }
}

void HMdcLookUpTbSec::markFakesNBins(Int_t clusAmp,Int_t arrSize,Int_t *nUnWiresCut) {
  // Mark clusters as "fake" by number of "unique" wires  vs. number of bins in this cluster
  // "unique" wires (nUnWr) - num.of wires wich participate in this cluster only
  for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(clusAmp)) { // 1.Mark "ghost"&"real":
    Int_t  ind   = cClusArr[cl].getNBins()-1;
    Int_t  nUnWr = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
    Bool_t fake  = ind<arrSize && nUnWr<nUnWiresCut[ind]; //nUnWr == 0 && nBins == 1;
    cClusArr[cl].resetFakeFlagSeg1(fake,pListCells);
  }
}

void HMdcLookUpTbSec::markFakes(Int_t clusAmp,Int_t arrSize,Int_t *nRlWiresCut) {
  // Mark clusters as "fake" by number of "unique" wires  vs. number of "real" wires in this cluster
  // "unique" wires (nUnWr) - num.of wires wich participate in this cluster only
  // "real" wires (nRlWr)   - num.of wires which participate in the clusters market as "real"
  for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(clusAmp)) {
    if(clusAmp==12 && cClusArr[cl].getFakeFlag() > 0) continue;
    Int_t  nRlWr;
    Int_t  nUnWr = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
    Bool_t fake  = nUnWr<arrSize && nRlWr>nRlWiresCut[nUnWr];
    cClusArr[cl].resetFakeFlagSeg1(fake,pListCells);
  }
}

void HMdcLookUpTbSec::markReals(Int_t clusAmp,Int_t arrSize,const Int_t *nUnWiresCut) {
  // Mark clusters as "real" by number of "unique" wires  vs. number of bins in this cluster
  // "unique" wires (nUnWr) - num.of wires wich participate in this cluster only
  for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(clusAmp)) {
    Int_t  nUnWr = cClusArr[cl].getNUniqueWiresSeg1(pListCells); 
    Int_t  ind   = cClusArr[cl].getNBins();
    if(ind > arrSize) ind = arrSize;
    Bool_t real  = nUnWr > nUnWiresCut[--ind];
    cClusArr[cl].resetRealFlagSeg1(real,pListCells);
  }
}

// void HMdcLookUpTbSec::removeGhosts(void) {
//    //===Suppression==
//  
//   pListCells->clearClustCounter(0);
//   
//   // Level 12:  ------------------------------------------------------------------------
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(12)) {
//     pListCells->addToClusCounters(0,cClusArr[cl].getLCells1());
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(12)) { // 1.Mark "ghost"&"real":
//     Int_t  nBins  = cClusArr[cl].getNBins();
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Bool_t realOk = (nUnWr > 5  && nBins > 0) || 
//                     (nUnWr > 4  && nBins > 1);    // RealID!
//     Bool_t fakeOk =  nUnWr == 0 && nBins == 1;    // FakeID!
//     if(fakeOk)      cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//     else if(realOk) cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(12)) { // 2.Mark "ghost":
//     if(cClusArr[cl].getFakeFlag() > 0) continue;
//     Int_t   nRlWr;
//     Int_t   nUnWr  = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t  fakeOk = (nUnWr == 0 && nRlWr > 6) || 
//                      (nUnWr == 1 && nRlWr > 7) || 
//                      (nUnWr == 2 && nRlWr > 9);  //  FakeID!
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(12)) { // 2.Mark "real":
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Int_t  nBins  = cClusArr[cl].getNBins();
//     Bool_t realOk = (nUnWr  > 5 && nBins > 0) ||
//                     (nUnWr  > 4 && nBins > 1) || 
//                     (nUnWr  > 3 && nBins > 3);   // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(12)) { // 3.Mark "ghost":
//     if(cClusArr[cl].getFakeFlag() > 0) continue; // Pochti ne vliyaet ????
//     Int_t  nRlWr;
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr==0 && nRlWr>6) ||
//                     (nUnWr==1 && nRlWr>7) ||
//                     (nUnWr==2 && nRlWr>9);  //  FakeID!
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
// 
//   // Level 11: ------------------------------------------------------------------------
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(11)) {
//     pListCells->addToClusCounters(0,cClusArr[cl].getLCells1());
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(11)) { // 1.Mark "real":
//     Int_t  nUnWr = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Bool_t realOk = nUnWr > 7;                   // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(11)) { // 1.Mark "ghost":
//     Int_t  nRlWr;
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr == 0 && nRlWr > 4) || 
//                     (nUnWr == 1 && nRlWr > 6) || 
//                     (nUnWr == 2 && nRlWr > 7);   //   FakeID!
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(11)) { // 2.Mark "real":
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Bool_t realOk = nUnWr > 7;   // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(11)) {// 2.Mark "ghost":
//     Int_t  nRlWr;
//     Int_t  nUnWr = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr==0 && nRlWr>4) || 
//                     (nUnWr==1 && nRlWr>5) ||
//                     (nUnWr==2 && nRlWr>6) ||
//                     (nUnWr==3 && nRlWr>7); //   FakeID!
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
// 
//   // Level 10: ------------------------------------------------------------------------
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) {
//     pListCells->addToClusCounters(0,cClusArr[cl].getLCells1());
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 1.Mark "real":
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Bool_t realOk = nUnWr > 9;                // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 1.Mark "ghost":
//     Int_t  nRlWr;
//     Int_t  nUnWr  = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr == 0 && nRlWr > 3) || 
//                     (nUnWr == 1 && nRlWr > 4) || 
//                     (nUnWr == 2 && nRlWr > 5) || 
//                     (nUnWr == 3 && nRlWr > 6); // Fake 
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 2.Mark "real":
//     Int_t   nUnWr = cClusArr[cl].getNUniqueWiresSeg1(pListCells);   
//     Bool_t realOk = nUnWr > 9;               // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 2.Mark "ghost":
//     Int_t  nRlWr;
//     Int_t  nUnWr = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr == 0 && nRlWr > 1) ||
//                     (nUnWr == 1 && nRlWr > 3) ||
//                     (nUnWr == 2 && nRlWr > 3) ||
//                     (nUnWr == 3 && nRlWr > 4) ||
//                     (nUnWr == 4 && nRlWr > 5); // Fake 80.68%/0.82%/0.40%
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 3.Mark "real":
//     Int_t   nUnWr = cClusArr[cl].getNUniqueWiresSeg1(pListCells);
//     Bool_t realOk = nUnWr > 9;   // RealID!
//     cClusArr[cl].resetRealFlagSeg1(realOk,pListCells);
//   }
//   for(Int_t cl=0; cl<(*cNClusArr); cl++) if(cClusArr[cl].isClusterAmpEq(10)) { // 3.Mark "ghost":
//     Int_t  nRlWr;
//     Int_t  nUnWr = cClusArr[cl].getNUniqueAndRWiresSeg1(pListCells,nRlWr);
//     Bool_t fakeOk = (nUnWr == 0 && nRlWr > 1) ||
//                     (nUnWr == 1 && nRlWr > 2) || 
//                     (nUnWr == 2 && nRlWr > 3) ||
//                     (nUnWr == 3 && nRlWr > 4) || 
//                     (nUnWr == 4 && nRlWr > 5); // Fake 80.68%/0.82%/0.40%
//     cClusArr[cl].resetFakeFlagSeg1(fakeOk,pListCells);
//   }
// }

//---------------------------------------------------------
HMdcLookUpTb* HMdcLookUpTb::fMdcLookUpTb  = NULL;

HMdcLookUpTb::HMdcLookUpTb(const Char_t* name,const Char_t* title,const Char_t* context)
            : HParSet(name,title,context) {
  // constructor creates an array of pointers of type HMdcLookUpTbSec
  fMdcLookUpTb = this;
  strcpy(detName,"Mdc");
  fGetCont=HMdcGetContainers::getObject();
  if( fGetCont==0 || gHades==0) return;
  array         = new TObjArray(6);
  fMdcGeomPar   = fGetCont->getMdcGeomPar();
  fSpecGeomPar  = fGetCont->getSpecGeomPar();
  fLayerGeomPar = fGetCont->getMdcLayerGeomPar();
  fSizesCells   = HMdcSizesCells::getObject();
  fMdcClusCat   = fGetCont->getCatMdcClus(kTRUE);
  targLenInc[0] = 0;
  targLenInc[1] = 0.;
  quietMode     = kFALSE;
  stack         = NULL;
  stacksArr     = NULL;
  isCoilOff     = kFALSE;
  numVFPoins    = 0;
  
  for(Int_t m=0;m<4;m++) for(Int_t l=0;l<6;l++) dDCutCorr[m][l] = 1.;
  memset(vertexStat,0,sizeof(vertexStat));
  
  if(HMdcTrackDSet::findClustVertex() && HMdcTrackDSet::getZStartDetector() == 0.) {
    fStartGeomPar = (HStart2GeomPar*)gHades->getRuntimeDb()->getContainer("Start2GeomPar");
  } else fStartGeomPar = NULL;
  
  HMdcDriftTimePar::getObject();
}

Bool_t HMdcLookUpTb::initContainer(void) {
  // It is called from "reinit" of reconstractor!
  if( !HMdcEvntListCells::getExObject() ) return kFALSE;
  if( !HMdcGetContainers::isInited(fMdcGeomPar)   || !HMdcGetContainers::isInited(fSpecGeomPar) ||
      !HMdcGetContainers::isInited(fLayerGeomPar) || !fSizesCells->initContainer()) return kFALSE;
  if( !HMdcDriftTimePar::getObject()->initContainer() ) return kFALSE;

  if( !status && (fSizesCells->hasChanged() || fSpecGeomPar->hasChanged() ||
                  fMdcGeomPar->hasChanged() || fLayerGeomPar->hasChanged()) ) {
    changed = kTRUE;
    if(!fMdcClusCat) return kFALSE;
    if(!calcVertexFnTarg()) return kFALSE;
    Int_t nBinX,nBinY;
    HMdcTrackDSet::getProjectPlotSizeSeg1(nBinX,nBinY);
    if(!quietMode) printf("Project plot size of inner segment: %i x %i bins\n",nBinX,nBinY);
    
    for (Int_t sec = 0; sec < 6; sec++) if( fGetCont->isSecActive(sec) ) {
      if( !(*array)[sec] ) {
        Int_t nSegs = fGetCont->isSegActive(sec,0) ? 1 : 0;
        if(isCoilOff && fGetCont->isSegActive(sec,1)) nSegs = 2;
//          nSegs = ( fGetCont->isSegActive(sec,1) ) ? 2:1;
        if(nSegs==0) continue;
        
        HMdcLookUpTbSec* secF = new HMdcLookUpTbSec(sec,nSegs,nBinX,nBinY);
        (*array)[sec] = secF;
        secF->setTypeClFinder(typeClFinder);
        secF->setClusCat(fMdcClusCat);
        if(stacksArr == NULL) {
          stacksArr = new HMdcClFnStacksArr();
          stack     = stacksArr->getOneStack();
        }
        secF->setStack(stack);
        secF->setStacksArr(stacksArr);
        secF->setVertexStat(vertexStat);
        secF->setVertexZErr(vertZErr);
      }
      (*this)[sec].setCoilFlag(isCoilOff);
      // initialization of container ---
      if(!calcPrPlane(sec))  return kFALSE;
      if(!calcTarget(sec))   return kFALSE;
      if(!calcPlotSize(sec)) return kFALSE;
      if(!(*this)[sec].calcLookUpTb(quietMode)) return kFALSE;
      (*this)[sec].calcVertexFnTarg(numVFPoins+numStartDPoints,targetPnts); // Add If ver... !!!
      // --------------------------------
    }
    
    if(versions[1]<0 || versions[2]<0) versions[1]=versions[2]=0;
    else versions[2]++;
  } else changed=kFALSE;
  return kTRUE;
}

HMdcLookUpTb* HMdcLookUpTb::getObject(void) {
  if(fMdcLookUpTb == NULL) fMdcLookUpTb = new HMdcLookUpTb();
  return fMdcLookUpTb;
}

void HMdcLookUpTb::deleteCont(void) {
  if(fMdcLookUpTb == NULL) return;
  delete fMdcLookUpTb;
  fMdcLookUpTb = NULL;
}

HMdcLookUpTb::~HMdcLookUpTb(void) {
  // destructor
  HMdcGetContainers::deleteCont();
  HMdcSizesCells::deleteCont();
  if(array) {
    array->Delete();
    delete array;
  }
  if(stacksArr) delete stacksArr;
  stacksArr = NULL;
  stack     = NULL;
  HMdcDriftTimePar::deleteCont();
}

Int_t HMdcLookUpTb::getSize(void) {
  // return the size of the pointer array
  return array->GetEntries();
}

void HMdcLookUpTb::clear(void) {
  for(Int_t s=0;s<6;s++) if((*array)[s]) (*this)[s].clearwk();
}

TH2C* HMdcLookUpTbSec::fillTH2C(const Char_t* name,const Char_t* title,Int_t type,Int_t bining) {
  // Proj.plot filling without cluster finding.
  for(module=0; module<4; module++) {
    cFMod = &((*this)[module]);
    if(cFMod == NULL) continue;
    cHPlModM = hPlMod[module];
    if(cHPlModM == NULL) continue;
    cXMinM = xMin[module];
    cXMaxM = xMax[module];
    for(layer=0;layer<6;layer++) {
//if(module!=0 || (layer!=0 && layer!=2)) continue;
      if( !setLayerVar() ) continue;
      makeLayProjV0();
    }
  }

  if(!hist) {
    plBining = bining<=0 ? 1 : bining;
    if(nBinX<100) plBining=1;
    hist = new TH2C(name,title,nBinX/plBining,xLow,xUp,nBinY/plBining,yLow,yUp);
  } else {
    hist->Reset();
    hist->SetName(name);
    hist->SetTitle(title);
  }
  hist->SetMinimum(0.);
  hist->Fill(0.,0.,0);
  Float_t          maximum = 6.;
  if(type==2)      maximum = 4.5;
  else if(type==0) maximum = (hPlMod[2]) ? 24.:12.;
  else if(type==1) maximum = (hPlMod[2]) ? 60.:18.;
  hist->SetMaximum(maximum);
  if(noFiredCells) return hist;
  for (Int_t nx=0; nx<nBinX; nx+=plBining) {
    for (Int_t ny=0; ny<nBinY; ny+=plBining) {
      Int_t nBin  = ny*nBinX+nx;
      Int_t nBin2 = nBin+nBinX;
      UChar_t binM1=hPlMod[0][nBin];
      UChar_t binM2=hPlMod[1][nBin];
      if(plBining==2) {
        binM1 |= hPlMod[0][nBin+1] | hPlMod[0][nBin2] | hPlMod[0][nBin2+1];
        binM2 |= hPlMod[1][nBin+1] | hPlMod[1][nBin2] | hPlMod[1][nBin2+1];
      }
      UChar_t binM3=0;
      UChar_t binM4=0;
      if(hPlMod[2]) {
        binM3=hPlMod[2][nBin];
        binM4=hPlMod[3][nBin];
        if(plBining==2) {
          binM3|=hPlMod[2][nBin+1] | hPlMod[2][nBin2] | hPlMod[2][nBin2+1];
          binM4|=hPlMod[3][nBin+1] | hPlMod[3][nBin2] | hPlMod[3][nBin2+1];
        }
      }
      if( binM1==0 && binM2==0 && binM3==0 && binM4==0) continue;
      Int_t color=0;
      if(type==1) {
        if(binM4)      color  = HMdcBArray::getNSet(binM4)+18;
        else if(binM3) color += HMdcBArray::getNSet(binM3)+12;
        else if(binM2) color += HMdcBArray::getNSet(binM2)+6;
        else if(binM1) color += HMdcBArray::getNSet(binM1);

// Int_t m1=HMdcBArray::getNSet(binM1);
// Int_t m2=HMdcBArray::getNSet(binM2);
// color=(m1>m2) ? m2:m1; //m1:m2;
	
      } else if(type==2) {
        if(binM4)      color = 4;
        else if(binM3) color = 3;
        else if(binM2) color = 2;
        else if(binM1) color = 1;
      } else {
        color = HMdcBArray::getNSet(binM1)+HMdcBArray::getNSet(binM2)+
                HMdcBArray::getNSet(binM3)+HMdcBArray::getNSet(binM4);
      }
      hist->Fill(xBinsPos[nx],yBinsPos[ny], color);
    }
  }
  clearPrArrs();
  return hist;
}

Bool_t HMdcLookUpTb::calcPrPlane(Int_t sec) {
  //  Calc. of projection plane:
  //  It's a plane between first and last MDC in sector
  //  (without magnetic field) or in inner segment (with magnetic fild). 
  //  par=0.0 -> plane=layer 6 of first MDC
  //  par=1.0 -> plane=layer 1 of last MDC
  //  If there is one MDC only - middle plane of MDC will used.
  Int_t firstMod = -1;
  Int_t lastMod  = 0;
  Int_t nMaxMods = isCoilOff ? 4 : 2;
  Int_t nmod     = 0;
  for(Int_t m=0; m<nMaxMods; m++) if( fGetCont->isModActive(sec,m) ) {
    nmod++;
    if(firstMod<0) firstMod=m;
    lastMod=m;
  }
  if(nmod==0) return kFALSE;
  if(nmod>1) {
    //  Double_t par = 0.0;   // <<<===================== use MDC1
    //  Double_t par = 1.0;   // <<<===================== use MDC2
    Double_t par = 0.553; //0.551232;
    HMdcPlane m1((*fSizesCells)[sec][firstMod][5]);
    HMdcPlane m2((*fSizesCells)[sec][ lastMod][0]);
    Double_t dNew=m1.D()*(1.-par)+m2.D()*par;
    Double_t bNew=dNew/(m1.D()*(1.-par)/m1.B()+m2.D()*par/m2.B());
    ((*this)[sec]).getPrPlane().setPlanePar( 0., bNew, 1., dNew);
    if(!quietMode) printf(
       "\n===> Sec.%i: Using plane between MDC%i and MDC%i (p=%f) as projection plane\n",
        sec+1,firstMod+1,lastMod+1,par);
  } else {
    ((*this)[sec]).getPrPlane().setPlanePar((*fSizesCells)[sec][firstMod]);
    if(!quietMode) printf(
        "\n===> Sec.%i: Using middle plane of MDC%i as projection plane\n",
        sec+1,firstMod+1);
  }
  return kTRUE;
}

Bool_t HMdcLookUpTb::calcTarget(Int_t sec){
  // Calculation of target parameters
  HMdcSizesCellsSec& fSCSec = (*fSizesCells)[sec];
  HMdcLookUpTbSec& fsec=(*this)[sec];
  if(&fSCSec==0 || &fsec==0) return kFALSE;
  fsec.calcTarget(targLenInc);
  return kTRUE;
}

Bool_t HMdcLookUpTb::calcPlotSize(Int_t sec) {
  HMdcLookUpTbSec& fsec=(*this)[sec];
  Double_t xLow,xUp,yLow,yUp;
  if(!fsec.calcXYBounds(xLow,xUp,yLow,yUp)) return kFALSE;

  Double_t del = (xUp-xLow)*0.02;
  xLow -= del;
  xUp  += del;
  del   = (yUp-yLow)*0.01;
  yLow -= del;
  yUp  += del;
  Double_t xStep = (xUp-xLow)/(Double_t)(fsec.getNBinX()-3);
  Double_t yStep = (yUp-yLow)/(Double_t)(fsec.getNBinY()-3);
  yLow -= yStep*1.5;            //must be bin=0 at limits of plot
  yUp  += yStep*1.5;
  xLow -= xStep*1.5;
  xUp  += xStep*1.5;
  
  fsec.setPrPlotSize(xLow,xUp,yLow,yUp);
  return kTRUE;
}

void HMdcLookUpTb::fillTrackList(Bool_t fl) {
  for(Int_t s=0;s<6;s++) if((*array)[s]) (*this)[s].fillTrackList(fl);
}

void HMdcLookUpTb::setTargLenInc(Double_t lf,Double_t rt) {
  targLenInc[0] = lf;
  targLenInc[1] = rt;
}

void HMdcLookUpTbSec::setDrTimeCutYCorr(Double_t corr) {
  if(corr<=0.) for(Int_t yb=0;yb<nBinY;yb++) yDDistCorr[yb] = 0.;
  else         for(Int_t yb=0;yb<nBinY;yb++) {
    Double_t yf = Double_t(yb)/Double_t(nBinY-1);
    yDDistCorr[yb] = corr*yf*yf;
  }
}

void HMdcLookUpTb::calcTdcDrDist(void) {
  for(Int_t sec=0;sec<6;sec++) if((*array)[sec]) (*this)[sec].calcTdcDrDist();
}

void HMdcLookUpTbSec::calcTdcDrDist(void) {
  if(!useDriftTime && !doVertexFn) return;
  for(Int_t mod=0;mod<4;mod++) if((*array)[mod]) {
    HMdcModListCells &rModLCells = (*pListCells)[mod];
    HMdcLookUpTbMod  &rLUTMod    = (*this)[mod];
    for(Int_t lay=0;lay<6;lay++) {
      HMdcLayListCells  &rLayLCells = rModLCells[lay];
      HMdcLookUpTbLayer &rLUTLayer  = rLUTMod[lay];
      Int_t cell = -1;
      while( rLayLCells.nextCell(cell) ) {
        HMdcLookUpTbCell& rLUTCell   = rLUTLayer[cell];
        Double_t alpha   = rLUTCell.getAlphaMean();
        Double_t tdcTime = rLayLCells.getTimeValue(cell);
        Float_t  tdcTDist;
        if(tdcTime>0.) tdcTDist = pDriftTimeParSec->calcDistance(mod,alpha,tdcTime);
        else           tdcTDist = 0.;
        if(doVertexFn) rLUTCell.setTdcTDistAndCuts1T(tdcTDist);
        else           rLUTCell.setTdcTDistAndCutsFT(tdcTDist);
      }
    }
  }
}

void HMdcLookUpTbCell::setTdcTDistAndCuts1T(Double_t d) {
  dDistMinCut    = d>distCut1T ? d-distCut1T : 0.;
  dDistMaxCut    =               d+distCut1T;
  dDistMinCut2VF = d>distCutVF ? (d-distCutVF)*(d-distCutVF) : 0.;
  dDistMaxCut2VF =               (d+distCutVF)*(d+distCutVF);
}

void HMdcLookUpTbCell::setTdcTDistAndCutsFT(Double_t d) {
  dDistMinCut   = d>distCutFT ? d-distCutFT : 0.;
  dDistMaxCut   =               d+distCutFT;
}

Int_t HMdcLookUpTb::findVertex(void) {
  Int_t  vertTar    = 0;
  Int_t  vertPnt    = 0;
  Int_t  maxNumBins = 0;
         vertTar    = -10;
         vertPnt    = -10;
  if(HMdcTrackDSet::findClustVertex()) {
    for(Int_t nt=0;nt<numVFPoins+numStartDPoints;nt++) vertexStat[nt] = 0;
    for(Int_t sec=0;sec<6;sec++) if(array->At(sec) != NULL) (*this)[sec].findVertex();

    if(use3PointMax) {    //  3 target points for the maximum
      for(Int_t nt=0;nt<numVFPoins-2;nt++) {
        Int_t maxSum3n = vertexStat[nt]+vertexStat[nt+1]+vertexStat[nt+2];
        if(maxSum3n > maxNumBins) {
          maxNumBins = maxSum3n;
          vertTar    = nt;
        }
      }
      vertPnt = vertTar+1;
      if(numStartDPoints>0) {
        Int_t maxSum3n = vertexStat[numVFPoins]+vertexStat[numVFPoins+1]+vertexStat[numVFPoins+2];
        if(maxSum3n > maxNumBins) {
          maxNumBins = maxSum3n;
          vertTar    = -1;               // Start detector
          vertPnt    = numVFPoins+1;
        }
      }
    } else {    // One target point for the maximum
      for(Int_t nt=0;nt<numVFPoins+numStartDPoints;nt++) if(maxNumBins < vertexStat[nt]) {
        vertPnt    = nt;
        maxNumBins = vertexStat[nt];
      }
      vertTar = vertPnt < numVFPoins ? vertPnt : -1;
    }
  }
  
  HEventHeader *evHeader   = gHades->getCurrentEvent()->getHeader();
  HVertex      &vertexClus = evHeader->getVertexCluster();
  vertexClus.setIterations(vertTar);                // Target segment
  vertexClus.setSumOfWeights(Float_t(maxNumBins));
  if(vertPnt >= 0) {
    vertexClus.setPos(targetPnts[vertPnt]);
    vertexClus.setChi2(0.f);
  } else {
    HGeomVector midPoint(fSizesCells->getTargetMiddlePoint());
    vertexClus.setPos(midPoint);
    vertexClus.setChi2(-1.f);
  }
  for(Int_t sec=0;sec<6;sec++) if((*array)[sec] != NULL) (*this)[sec].setVertexPoint(vertPnt);
  
// !!!!!!!!!!!!!!!!!!!
//if(!noMaximum) return -100; //!!!!!!!!!!!!!!!!!!!!!!!!!

  return vertTar;
}

void HMdcLookUpTbSec::findVertex(void) {
  
  if(nModules  == 0) return;
  if(nSegments == 2) return findSecVertex();
  if(nModules  != 2) return; //??????????????????????????
  Int_t minAmpCut = levelVertFPP;
  Int_t maxAmpSeg = (*pListCells)[0].getNLayers() + (*pListCells)[1].getNLayers();
  if(maxAmpSeg < minAmpCut) return;
  Int_t nFiredLay = 0;

  cXMinM = xMin[0];
  cXMaxM = xMax[0];
  for(Int_t indLine=0; indLine<6; indLine++) {
    layer = layerOrder[indLine];
    for(module=0; module<2; module++) {
      cFMod = &((*this)[module]);
      if( !cFMod ) continue;
      cHPlModM = hPlMod[module];
      if(!cHPlModM) continue;
//      cXMinM=xMin[module];
//      cXMaxM=xMax[module];
    
      if( !setLayerVar() ) continue;
      nFiredLay++;
      if(maxAmpSeg-nFiredLay+1 >= minAmpCut) {
        if(nFiredLay<minAmpCut) {
          // makeLayProjV1():
          while( setNextCell() ) {
            UInt_t y     = pLUTCell->getYBinMin();
            UInt_t shift = y * nBinX;
            UInt_t nLns  = pLUTCell->getNLines();
            for(UInt_t ln=0; ln<nLns; ln++,y++,shift+=nBinX) {
              UInt_t nbL = pLUTCell->getXBinMax(ln)+shift;
              UInt_t nbF = pLUTCell->getXBinMin(ln)+shift;   
              if(nbF<cXMinM[y]) cXMinM[y] = nbF;
              if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
              UChar_t* hPlModE = cHPlModM+nbL;
              for(UChar_t* bt = cHPlModM+nbF; bt<=hPlModE; bt++) *bt |= add;
            }
          }
        } else while(  setNextCell()  ) {
          // It should never happends...?
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;

            UChar_t* bt  = cHPlModM+nbF;
            UChar_t* bt0 = module==0 ? hPlMod[1]+nbF:hPlMod[0]+nbF;

            if(nbF<cXMinM[y]) cXMinM[y] = nbF;
            if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++) {
              *bt |= add;
              UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
              if( wt >= minAmpCut ) {
                testBinForVertexF(nb%nBinX,y);
                *bt  = 0; // Test this bin once only
                *bt0 = 0;
              }
            }
          }
        }
      } else {
        if(nFiredLay<minAmpCut) {
          // makeLayProjV2() :
          while( setNextCell() ) {
            UInt_t y    = pLUTCell->getYBinMin();
            UInt_t nLns = pLUTCell->getNLines();
            for(UInt_t ln=0; ln<nLns; ln++,y++) {
              UInt_t shift = y * nBinX;
              UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
              UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
              if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
              if(nbF<cXMinM[y]) nbF = cXMinM[y];
              if(nbF > nbL) continue;
              UChar_t* hPlModE = cHPlModM+nbL;
              for(UChar_t* bt = cHPlModM+nbF; bt<=hPlModE; bt++) *bt |= add;
            }
          }  
        } else while(  setNextCell()  ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
            if(nbF<cXMinM[y]) nbF = cXMinM[y];
            if(nbF > nbL) continue;
            UChar_t* bt  = cHPlModM+nbF;
            UChar_t* bt0 = module==0 ? hPlMod[1]+nbF:hPlMod[0]+nbF;

            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++,bt0++) {
              *bt |= add;
              UChar_t wt = HMdcBArray::getNSet(bt) + HMdcBArray::getNSet(bt0);
              if( wt >= minAmpCut ) {
                testBinForVertexF(nb%nBinX,y);
                *bt  = 0; // Test this bin once only
                *bt0 = 0;
              }
            }
          }
        }
      } 
    }
  }
  clearPrMod();
}

void HMdcLookUpTbSec::findSecVertex(void) {
  
  if(nModules<3) return;
  Int_t minAmpCut = 6*nModules - (12-levelVertFPP);
  Int_t maxAmpSeg = (*pListCells)[0].getNLayers() + (*pListCells)[1].getNLayers() +
                    (*pListCells)[2].getNLayers() + (*pListCells)[3].getNLayers();
  if(maxAmpSeg < minAmpCut) return;

  Int_t nFiredLay = 0;

  cXMinM = xMin[0];
  cXMaxM = xMax[0];
  for(Int_t indLine=0; indLine<6; indLine++) {
    layer = layerOrder[indLine];
    for(module=0; module<4; module++) if((*array)[module]) {
      cFMod = &((*this)[module]);
      if( !cFMod ) continue;
      cHPlModM = hPlMod[module];
      if(!cHPlModM) continue;
//      cXMinM=xMin[module];
//      cXMaxM=xMax[module];
    
      if( !setLayerVar() ) continue;
      nFiredLay++;
      if(maxAmpSeg-nFiredLay+1 >= minAmpCut) {
        if(nFiredLay<minAmpCut) {
          // makeLayProjV1():
          while( setNextCell() ) {
            UInt_t y     = pLUTCell->getYBinMin();
            UInt_t shift = y * nBinX;
            UInt_t nLns  = pLUTCell->getNLines();
            for(UInt_t ln=0; ln<nLns; ln++,y++,shift+=nBinX) {
              UInt_t nbL = pLUTCell->getXBinMax(ln)+shift;
              UInt_t nbF = pLUTCell->getXBinMin(ln)+shift;   
              if(nbF<cXMinM[y]) cXMinM[y] = nbF;
              if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;
              UChar_t* hPlModE = cHPlModM+nbL;
              for(UChar_t* bt = cHPlModM+nbF; bt<=hPlModE; bt++) *bt |= add;
            }
          }
        } else while(  setNextCell()  ) {
          // It should never happends...?
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbF<cXMinM[y]) cXMinM[y] = nbF;
            if(nbL>cXMaxM[y]) cXMaxM[y] = nbL;

            UChar_t* bt  = cHPlModM+nbF;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              *bt |= add;
              UChar_t wt = HMdcBArray::getNSet(hPlMod[0][nb]) + HMdcBArray::getNSet(hPlMod[1][nb]) +
                           HMdcBArray::getNSet(hPlMod[2][nb]) + HMdcBArray::getNSet(hPlMod[3][nb]);
              if( wt >= minAmpCut ) {
                testBinForVertex(nb%nBinX,y);
                for(Int_t i=0;i<4;i++) hPlMod[i][nb] = 0; // Test this bin once only
              }
            }
          }
        }
      } else {
        if(nFiredLay<minAmpCut) {
          // makeLayProjV2() :
          while( setNextCell() ) {
            UInt_t y    = pLUTCell->getYBinMin();
            UInt_t nLns = pLUTCell->getNLines();
            for(UInt_t ln=0; ln<nLns; ln++,y++) {
              UInt_t shift = y * nBinX;
              UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
              UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
              if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
              if(nbF<cXMinM[y]) nbF = cXMinM[y];
              if(nbF > nbL) continue;
              UChar_t* hPlModE = cHPlModM+nbL;
              for(UChar_t* bt = cHPlModM+nbF; bt<=hPlModE; bt++) *bt |= add;
            }
          }  
        } else while(  setNextCell()  ) {
          UInt_t y    = pLUTCell->getYBinMin();
          UInt_t nLns = pLUTCell->getNLines();
          for(UInt_t ln=0; ln<nLns; ln++,y++) {
            UInt_t shift = y * nBinX;
            UInt_t nbL   = pLUTCell->getXBinMax(ln)+shift;
            UInt_t nbF   = pLUTCell->getXBinMin(ln)+shift;
            if(nbL>cXMaxM[y]) nbL = cXMaxM[y];
            if(nbF<cXMinM[y]) nbF = cXMinM[y];
            if(nbF > nbL) continue;
            
            UChar_t* bt  = cHPlModM+nbF;
            for(UInt_t nb=nbF; nb<=nbL; nb++,bt++) {
              *bt |= add;
              UChar_t wt = HMdcBArray::getNSet(hPlMod[0][nb]) + HMdcBArray::getNSet(hPlMod[1][nb]) +
                           HMdcBArray::getNSet(hPlMod[2][nb]) + HMdcBArray::getNSet(hPlMod[3][nb]);
              if( wt >= minAmpCut ) {
                testBinForVertex(nb%nBinX,y);
                for(Int_t i=0;i<4;i++) hPlMod[i][nb] = 0; // Test this bin once only
              }
            }
          }
        }
      } 
    }
  }
  clearPrModInSec();
}

void HMdcLookUpTbSec::testBinForVertexF(Int_t bx, Int_t by) {
  // Test inner MDC only
  if(lTargPnt == 0) return;
 
  Int_t maxAmpV = nMods*6;
  Int_t ampCut1 = 12 - levelVertF;
  Int_t ampCut2 = ampCut1+2;       // For shape of spike

  Double_t xb = xBinsPos[bx];
  Double_t yb = yBinsPos[by];

  Double_t yOffArr[10];
  Double_t dDmin2[10];
  Double_t dDmax2[10];

  Int_t nFrLayers[lTargPnt];    // Number of fired layers for each target
  for(Int_t nt=0;nt<lTargPnt;nt++) nFrLayers[nt] = 0;
         
  Int_t iLay    = 0;
  Int_t maxNFrLayers = 0;
  for(Int_t mod=0;mod<2;mod++) if((*array)[mod]) {
    HMdcSizesCellsMod &rSCMod     = (*pSCellSec)[mod];
    HMdcModListCells  &rModLCells = (*pListCells)[mod];
    HMdcLookUpTbMod   &rLUTMod    = (*this)[mod];
    for(Int_t lay=0;lay<6;lay++,iLay++) {
      Int_t layCut = iLay-ampCut2;
      HMdcSizesCellsLayer &rSCLay     = rSCMod[lay];
      HMdcLayListCells    &rLayLCells = rModLCells[lay];
      HMdcLookUpTbLayer   &rLUTLayer  = rLUTMod[lay];
      
      Double_t* yTr   = rLUTLayer.getYTargetArr();
      Double_t* zTr   = rLUTLayer.getZTargetArr();
    
      Double_t  y,z;
      rLUTLayer.transToRotLay(xb,yb,y,z);
      
      Double_t  yw1   = (y*zTr[indFirstTPnt] - yTr[indFirstTPnt]*z)/(zTr[indFirstTPnt] - z);
      Double_t  yw2   = (y*zTr[indLastTPnt ] - yTr[indLastTPnt ]*z)/(zTr[indLastTPnt ] - z);
      Int_t     cell1 = rSCLay.calcInnerSegCell(yw1);
      Int_t     cell2 = rSCLay.calcInnerSegCell(yw2);
      if(cell1 > cell2) {
        Int_t c = cell2;
        cell2   = cell1;
        cell1   = c;
      }
      cell1--; //cell1 -= 2; ????
      cell2++; //cell2 += 2; ????

      Int_t nCells = 0;
      for(Int_t cell=cell1;cell<=cell2;cell++) if(rLayLCells.getTime(cell) != 0) {
        HMdcLookUpTbCell& rLUTCell = rLUTLayer[cell];
        dDmin2[nCells]  = rLUTCell.getDDistMinCut2VF();
        dDmax2[nCells]  = rLUTCell.getDDistMaxCut2VF();
        yOffArr[nCells] = rSCLay[cell].getWirePos();
        nCells++;
        if(nCells==10) break;
      }

      // Scan: ========================================
      for(Int_t nt=0;nt<lTargPnt;nt++) if(layCut <= nFrLayers[nt]) { //== if(iLay-nFrLayers[nt]<=ampCut2)
        Int_t c = 0;
          Double_t dyDdz = (y - yTr[nt])/(z - zTr[nt]);
          Double_t c1    = y-dyDdz*z;
          Double_t c2    = 1.+dyDdz*dyDdz;
          for(;c<nCells;c++) {
            Double_t dDist2 = c1-yOffArr[c];
            dDist2 *= dDist2;
            if(dDist2 > dDmin2[c]*c2 && dDist2 < dDmax2[c]*c2) {
              nFrLayers[nt]++;
              if(maxNFrLayers < nFrLayers[nt]) maxNFrLayers = nFrLayers[nt];
              break;
            }
          }
      }
      if(iLay-maxNFrLayers >= ampCut1) return;  // Not enough amplitude!
    }
  }
 
  Int_t cut = maxAmpV - ampCut2 -1;
  for(Int_t nt=0;nt<lTargPnt;nt++) if(nFrLayers[nt]>cut) vertexStat[nt] += nFrLayers[nt]-cut;
}

void HMdcLookUpTbSec::testBinForVertex(Int_t bx, Int_t by) {
  // For field off data
  if(lTargPnt == 0) return;

  Int_t maxAmpV = nMods*6;
  Int_t ampCut1 = 12 - levelVertF;
  Int_t ampCut2 = ampCut1+2;       // For shape of spike

  Double_t xb = xBinsPos[bx];
  Double_t yb = yBinsPos[by];

  Double_t yOffArr[20];
  Double_t dDmin2[20];
  Double_t dDmax2[20];

  Int_t nFrLayers[lTargPnt];    // Number of fired layers for each target
  for(Int_t nt=0;nt<lTargPnt;nt++) nFrLayers[nt] = 0;
         
  Int_t iLay    = 0;
  Int_t maxNFrLayers = 0;
  for(Int_t mod=0;mod<4;mod++) if((*array)[mod]) {
    HMdcSizesCellsMod &rSCMod     = (*pSCellSec)[mod];
    HMdcModListCells  &rModLCells = (*pListCells)[mod];
    HMdcLookUpTbMod   &rLUTMod    = (*this)[mod];
    for(Int_t lay=0;lay<6;lay++,iLay++) {
      Int_t layCut = iLay-ampCut2;
      HMdcSizesCellsLayer &rSCLay     = rSCMod[lay];
      HMdcLayListCells    &rLayLCells = rModLCells[lay];
      HMdcLookUpTbLayer   &rLUTLayer  = rLUTMod[lay];
      
      Double_t* yTr   = rLUTLayer.getYTargetArr();
      Double_t* zTr   = rLUTLayer.getZTargetArr();
      Double_t* yTrP2 = rLUTLayer.getYTargetArrP2();
      Double_t* zTrP2 = rLUTLayer.getZTargetArrP2();
    
      Double_t  y,z;
      rLUTLayer.transToRotLay(xb,yb,y,z);
      
      Double_t  yw1   = (y*zTr[indFirstTPnt] - yTr[indFirstTPnt]*z)/(zTr[indFirstTPnt] - z);
      Double_t  yw2   = (y*zTr[indLastTPnt ] - yTr[indLastTPnt ]*z)/(zTr[indLastTPnt ] - z);
      Int_t     cell1 = rSCLay.calcInnerSegCell(yw1);
      Int_t     cell2 = rSCLay.calcInnerSegCell(yw2);
      if(cell1 > cell2) {
        Int_t c = cell2;
        cell2   = cell1;
        cell1   = c;
      }
      cell1--; //cell1 -= 2; ????
      cell2++; //cell2 += 2; ????

      Int_t nCells = 0;
      Int_t endP1  = 0;
      for(Int_t cell=cell1;cell<=cell2;cell++) if(rLayLCells.getTime(cell) != 0) {
        HMdcLookUpTbCell& rLUTCell = rLUTLayer[cell];
        dDmin2[nCells]  = rLUTCell.getDDistMinCut2VF();
        dDmax2[nCells]  = rLUTCell.getDDistMaxCut2VF();
        yOffArr[nCells] = rSCLay[cell].getWirePos();
        if(mod < 2 || rSCLay.getLayerPart(cell)==0) endP1++;
        nCells++;
        if(nCells==20) break;
      }

      // Scan: ========================================
      for(Int_t nt=0;nt<lTargPnt;nt++) if(layCut <= nFrLayers[nt]) { //== if(iLay-nFrLayers[nt]<=ampCut2)
        Int_t c = 0;
        if(endP1>0) {
          Double_t dyDdz = (y - yTr[nt])/(z - zTr[nt]);
          Double_t c1    = y-dyDdz*z;
          Double_t c2    = 1.+dyDdz*dyDdz;
          for(;c<endP1;c++) {
            Double_t dDist2 = c1-yOffArr[c];
            dDist2 *= dDist2;
            if(dDist2 > dDmin2[c]*c2 && dDist2 < dDmax2[c]*c2) {
              nFrLayers[nt]++;
              if(maxNFrLayers < nFrLayers[nt]) maxNFrLayers = nFrLayers[nt];
              break;
            }
          }
        }
        if(endP1<nCells && c==endP1) { //Layer second part:
          Double_t dyDdz = (y - yTrP2[nt])/(z - zTrP2[nt]);
          Double_t c1    = y-dyDdz*z;
          Double_t c2    = 1.+dyDdz*dyDdz;
          for(;c<nCells;c++) {
            Double_t dDist2 = c1-yOffArr[c];
            dDist2 *= dDist2;
            if(dDist2 > dDmin2[c]*c2 && dDist2 < dDmax2[c]*c2) {
              nFrLayers[nt]++;
              if(maxNFrLayers < nFrLayers[nt]) maxNFrLayers = nFrLayers[nt];
              break;
            }
          }
        }
      }
      if(iLay-maxNFrLayers >= ampCut1) return;  // Not enough amplitude!
    }
  }
 
  Int_t cut = maxAmpV - ampCut2 -1;
  for(Int_t nt=0;nt<lTargPnt;nt++) if(nFrLayers[nt]>cut) vertexStat[nt] += nFrLayers[nt]-cut;
}

void HMdcLookUpTbSec::setVertexPoint(Int_t vp) {
  vertexPoint = vp;
  for(Int_t mod=0; mod<4; mod++) if( (*array)[mod] ) {
    HMdcLookUpTbMod& pLUTbMod = (*this)[mod];
    for(Int_t lay=0; lay<6; lay++) pLUTbMod[lay].setVertexPoint(vp);
  }
}

void HMdcLookUpTbLayer::setVertexPoint(Int_t vp) {
  if(vp>=0) {
    layPart1.yVertex = layPart1.yTp[vp];
    layPart1.zVertex = layPart1.zTp[vp];
  } else {
    layPart1.yVertex = layPart1.yt;
    layPart1.zVertex = layPart1.zt;
  }
  if(layPart2==NULL) return;
  if(vp>=0) {
    layPart2->yVertex = layPart2->yTp[vp];
    layPart2->zVertex = layPart2->zTp[vp];
  } else {
    layPart2->yVertex = layPart2->yt;
    layPart2->zVertex = layPart2->zt;
  }
}

void HMdcLookUpTbLayer::setCurrentCell(Int_t cell) {
  Int_t layPart = pSCellLay->getLayerPart(cell);
  if(layPart == 1) currLayPart =  layPart2;
  else             currLayPart = &layPart1;
  yWirePos = (*pSCellLay)[cell].getWirePos();
  dDmin    = pLTCellArr[cell].getDDistMinCut();
  dDmax    = pLTCellArr[cell].getDDistMaxCut();
}
