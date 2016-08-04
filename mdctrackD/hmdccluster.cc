using namespace std;
#include "hmdccluster.h"
#include "TMath.h"
#include "hmdcclussim.h"
#include "hmdcgeanttrack.h"
#include "hmdclistcells.h"

//*-- AUTHOR : Pechenov Vladimir
//*-- Modified : 15/02/2007 by V. Pechenov

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcCluster
//
// Temporary cluster information for cluster finder
//
////////////////////////////////////////////////////////////////

ClassImp(HMdcCluster)
ClassImp(HMdcClustersArrs)

void HMdcCluster::init(Char_t sec,Char_t seg,Float_t x,Float_t y,Float_t sx,Float_t sy) {
  sector      = sec;
  segment     = seg;
  status      = kTRUE;
  nMergedClus = 1;
  clusMerg    = 0;
  lCells1.clear();
  lCells2.clear();
  clearBinStat();
  x0       = x;
  y0       = y;
  xSt      = sx;
  ySt      = sy;
  errX     = 0.5*xSt;
  errY     = 0.5*ySt;
  sigma1   = 0.;
  sigma2   = 0.;
  alpha    = 0.;
  fakeFlag = 0;
  realFlag = 0;
}

void HMdcCluster::clearBinStat(void) {
  nBins    = 0;
  sumWt    = 0;   
  meanX    = 0;   
  meanY    = 0;   
  meanXX   = 0;  
  meanYY   = 0;  
  meanYX   = 0;  
  meanXWt  = 0; 
  meanYWt  = 0; 
  meanXXWt = 0;
  meanYYWt = 0;
  minWt    = 1000; 
   
  clusMod1  = 0;
  clusMod2  = 0;
  flag      = 0;
  numCells1 = -1;
  numCells2 = -1;
}

void HMdcCluster::setModCluster(Int_t im, HMdcCluster* cl) {
  if(im==0)      clusMod1 = cl;
  else if(im==1) clusMod2 = cl;
  cl->flag++;
}

void HMdcCluster::addClus(HMdcCluster& clst2) {
  // seg!=0 and seg!=1 - inner and outer segments
  if(segment != 1) {
    lCells1.add(&(clst2.lCells1));
    numCells1 = -1;
  }
  if(segment != 0) {
    lCells2.add(&(clst2.lCells2));
    numCells2 = -1;
  }
  clst2.status   = kFALSE;
  clst2.clusMerg = this;

  clst2.correctContent(iXFirst,iYFirst);
  sumWt         += clst2.sumWt;
  nBins         += clst2.nBins;
  meanX         += clst2.meanX;
  meanY         += clst2.meanY;
  meanXX        += clst2.meanXX;
  meanYY        += clst2.meanYY;
  meanYX        += clst2.meanYX;
  meanXWt       += clst2.meanXWt;
  meanYWt       += clst2.meanYWt;
  meanXXWt      += clst2.meanXXWt;
  meanYYWt      += clst2.meanYYWt;
  nMergedClus   += clst2.nMergedClus;
  if(minWt > clst2.minWt) minWt = clst2.minWt;

  calcXY();
}

void HMdcCluster::correctContent(Int_t iXf,Int_t iYf) {
  Int_t shX = iXf - iXFirst;
  Int_t shY = iYf - iYFirst;
  if(shX != 0) {
    Int_t vl1 = shX*nBins;
    Int_t vl2 = shX*sumWt;
    meanX    -= vl1;
    meanXX   -= shX*(2*meanX + vl1);
    meanXWt  -= vl2;
    meanXXWt -= shX*(2*meanXWt + vl2);
  }
  if(shY != 0) {
    Int_t vl1 = shY*nBins;
    Int_t vl2 = shY*sumWt;
    meanY    -= vl1;
    meanYY   -= shY*(2*meanY + vl1);
    meanYWt  -= vl2;
    meanYYWt -= shY*(2*meanYWt + vl2);
  }
  meanYX   -= meanX*shY + meanY*shX + shX*shY*nBins;
  iXFirst = iXf;
  iYFirst = iYf;
}

void HMdcCluster::calcXY(void) {
  if(nBins<2) {
    x = x0 + iXFirst*xSt;
    y = y0 + iYFirst*ySt;
  } else if(minWt > 1) {
    Int_t norm = minWt-1;
    Float_t rSumWt = 1/Float_t(sumWt-nBins*norm);
    
    x    = Float_t(meanXWt-meanX*norm)*rSumWt;
    errX = (meanXXWt-meanXX*norm)*rSumWt - x*x;
    x    = (x + iXFirst)*xSt + x0;
    
    y    = Float_t(meanYWt-meanY*norm)*rSumWt;
    errY = (meanYYWt-meanYY*norm)*rSumWt - y*y;
    y    = (y + iYFirst)*ySt + y0;
  } else {
    Float_t rSumWt = 1/Float_t(sumWt);
    
    x    = meanXWt*rSumWt;
    errX = meanXXWt*rSumWt - x*x;
    x    = (x + iXFirst)*xSt + x0;
    
    y    = meanYWt*rSumWt;
    errY = meanYYWt*rSumWt - y*y;
    y    = (y + iYFirst)*ySt + y0;
  }
}

void HMdcCluster::sumClus(HMdcCluster& clst1, HMdcCluster& clst2) {
  lCells1=clst1.lCells1;
  lCells1.add(&(clst2.lCells1));
  if(segment != 0) {
    lCells2=clst1.lCells2;
    lCells2.add(&(clst2.lCells2));
  }
  clst1.flag = 1;
  clst2.flag = 1;
//  clst2.status=kFALSE; ?
//  clst2.clusMerg=this; ?
  clst2.correctContent(clst1.iXFirst,clst1.iYFirst);
  sumWt        = clst1.sumWt+clst2.sumWt;
  nBins        = clst1.nBins+clst2.nBins;
  meanX        = clst1.meanX+clst2.meanX;
  meanY        = clst1.meanY+clst2.meanY;
  meanXX       = clst1.meanXX+clst2.meanXX;
  meanYY       = clst1.meanYY+clst2.meanYY;
  meanYX       = clst1.meanYX+clst2.meanYX;
  meanXWt      = clst1.meanXWt+clst2.meanXWt;
  meanYWt      = clst1.meanYWt+clst2.meanYWt;
  meanXXWt     = clst1.meanXXWt+clst2.meanXXWt;
  meanYYWt     = clst1.meanYYWt+clst2.meanYYWt;
  minWt        = clst1.minWt <= clst2.minWt ? clst1.minWt : clst2.minWt; //???
  nMergedClus += clst2.nMergedClus;  //?????????????? 
  
  calcXY();
}

void HMdcCluster::calcClusParam(void) {
  if(nBins<2) return;
  errX = TMath::Sqrt(errX + 0.25)*xSt;  // 0.25 = 0.5*0.5 (half of bin)
  errY = TMath::Sqrt(errY + 0.25)*ySt;
  // Cluster shape:-------------------------------------------------
  //  eXX,eYY,eXY=eYX - covariance matrix componets
  //
  //    | eXX-sigma   aXY       |           | E1 |
  // A= |                       |         B=|    |
  //    | eYX         eYY-sigma |           | E2 |
  //
  // A^2=0;  ==> sigma1 & sigma2  - (sigma1 > sigma2)
  //
  //    | 0 |        | E1 |
  // AB=|   |;   ==> |    | - direction of main axis
  //    | 0 |        | E2 |
  Float_t eXX     = (meanXX*nBins - meanX*meanX)*xSt*xSt;
  Float_t eYX     = (meanYX*nBins - meanY*meanX)*xSt*ySt;
  Float_t eYY     = (meanYY*nBins - meanY*meanY)*ySt*ySt;
  Float_t v1      = (eXX+eYY)*0.5;
  Float_t eYX2    = eYX*eYX;
  Float_t v2      = TMath::Sqrt(v1*v1 - eXX*eYY + eYX2);
  Float_t sigma1D = v1 + v2;
  Float_t sigma2D = v1 - v2;
  Float_t e1      = sigma1D - eXX;
  Float_t e2      = sigma1D - eYY;
  sigma1 = TMath::Sqrt(sigma1D)/nBins;
  sigma2 = (sigma2D<10e-5) ? 0. : TMath::Sqrt(sigma2D)/nBins;
  alpha  = TMath::ATan2(TMath::Sqrt(eYX2+e1*e1),TMath::Sqrt(eYX2+e2*e2))*TMath::RadToDeg();
  if(eYX<0.) alpha=180.-alpha;
}

void HMdcCluster::fillClus(HMdcClus* fClus, Int_t nLst, Bool_t fillTrList) {
  fClus->setSumWt(sumWt);
  fClus->setNBins(nBins);
  fClus->setXY(x,errX,y,errY);
  fClus->setNMergClust(nMergedClus);
  fClus->setRealLevel(minWt);
  fClus->setFakeFlag(fakeFlag);
  HMdcList12GroupCells& list=(nLst==0) ? lCells1:lCells2;
  HMdcTrackInfSim* clusSimInf = 0;
  if(fillTrList && fClus->isGeant()) clusSimInf = HMdcGeantEvent::getMdcTrackInfSim();
  if(clusSimInf) clusSimInf->fillClusTrackInf((HMdcClusSim*)fClus);
  if(clusMod1==0) {
    Int_t nDrTm=list.getNDrTimes(0,5);
    if(nDrTm>0 && fClus->getMinCl(0)>0) {
      fClus->setClFnLevelM1(fClus->getMinCl(0));
      fClus->setClusSizeM1(nBins);
      fClus->setNMergClustM1(nMergedClus);
      fClus->setShapeM1(sigma1,sigma2,alpha);
      if(clusSimInf) clusSimInf->fillClusModTrackInf((HMdcClusSim*)fClus, &list,0);
    } else fClus->clearMod1Par();
    fClus->setNDrTimesM1(nDrTm);
  } else {
    fClus->setClFnLevelM1((UChar_t)(clusMod1->minWt+0.1));
    fClus->setClusSizeM1(clusMod1->nBins);
    fClus->setNDrTimesM1(clusMod1->lCells1.getNDrTimes(0,5));
    fClus->setNMergClustM1(clusMod1->nMergedClus);
    fClus->setShapeM1(clusMod1->sigma1,clusMod1->sigma2,clusMod1->alpha);
    if(clusSimInf) clusSimInf->fillClusModTrackInf((HMdcClusSim*)fClus, &(clusMod1->lCells1),0);
  }
  if(clusMod2==0) {
    Int_t nDrTm=list.getNDrTimes(6,11);
    if(nDrTm>0 && fClus->getMinCl(1)>0) {
      fClus->setClFnLevelM2(fClus->getMinCl(1));
      fClus->setClusSizeM2(nBins);
      fClus->setNMergClustM2(nMergedClus);
      fClus->setShapeM2(sigma1,sigma2,alpha);
      if(clusSimInf) clusSimInf->fillClusModTrackInf((HMdcClusSim*)fClus,
                                                     &list,1);
    } else fClus->clearMod2Par();
    fClus->setNDrTimesM2(nDrTm);
  } else {
    fClus->setClFnLevelM2((UChar_t)(clusMod2->minWt+0.1));
    fClus->setClusSizeM2(clusMod2->nBins);
    fClus->setNDrTimesM2(clusMod2->lCells1.getNDrTimes(6,11));
    fClus->setNMergClustM2(clusMod2->nMergedClus);
    fClus->setShapeM2(clusMod2->sigma1,clusMod2->sigma2,clusMod2->alpha);
    if(clusSimInf) clusSimInf->fillClusModTrackInf((HMdcClusSim*)fClus, &(clusMod2->lCells1),1);
  }
}

void HMdcCluster::print(void) {
  // Debug print:
  printf("Sector %i Cluster: status=%s, flag=%i,nMergedClus=%i  x=%.1f y=%.1f\n",
         sector+1,status ? "kTRUE" : "kFALSE",flag,nMergedClus,x,y);
  HMdcEvntListCells *listCells = HMdcEvntListCells::getExObject();
  for(Int_t seg=0;seg<2;seg++) {
    HMdcList12GroupCells &lCells = seg==0 ? lCells1 : lCells2;
    if(lCells.getNCells()==0) continue;
    if(listCells == NULL) {
      printf(" - - - - - - - lCells%i:\n",seg+1);
      lCells.print();
    } else  for(Int_t lay=0; lay<12; lay++) {
      Int_t mod   = seg*2 + lay/6;
      Int_t layer = lay%6;
      printf("M.%i L.%i  %2i cells:",mod+1,layer+1,lCells.getNCells(lay));
      HMdcLayListCells &listCellsLay = (*listCells)[sector][mod][layer];
      Int_t cell=-1;
      while((cell=lCells.next(lay,cell)) >= 0) listCellsLay.printCellInfo(cell);
      printf("\n");
    }
  }
}

Int_t HMdcCluster::testForInclude(HMdcCluster& cl2) {
  // For HMdcLookUpTb !
  // Function test are wires from one cluster fully included in another cluster.
  // Index of "this" obj. in array must be < index cl2 !!!
  // ??? return kTRUE 0,1,2, or 3if "cl2" is included to "this" (STOP LOOP)
  // ??? return kFALSE cl2 is not included to "this" or "this" is included to "cl2"
  if(numCells1<0)         {
    numCells1 = lCells1.getNCells();
    nLayers1  = lCells1.getNLayers();
  }
  if(numCells2<0)         {
    numCells2 = lCells2.getNCells();
    nLayers2  = lCells2.getNLayers();
  }
  if(cl2.numCells1<0) {
    cl2.numCells1 = cl2.lCells1.getNCells();
    cl2.nLayers1  = cl2.lCells1.getNLayers();
  }
  if(cl2.numCells2<0) {
    cl2.numCells2 = cl2.lCells2.getNCells();
    cl2.nLayers2  = cl2.lCells2.getNLayers();
  }

  // dCells - Number of not identical cells (for smaller cluster)
  //                       nIdentDrTimes return num.cells!
  Int_t dCells;
  Int_t dNCells;
  Int_t dLayers;
  Int_t nIdCells;
  if       (segment == 0) {
    nIdCells = lCells1.nIdentDrTimes(&(cl2.lCells1));
    dCells   = numCells1 <= cl2.numCells1 ?    numCells1-nIdCells : 
                                           cl2.numCells1-nIdCells;
    // !!!!!!!!!! 6 ???        2 ??? It is not fine tuning!
    if(nIdCells < 6  || dCells+2 > nIdCells) return 0;
    dNCells  = numCells1 - cl2.numCells1;
    dLayers  = nLayers1  - cl2.nLayers1;
  } else if(segment == 1) {
    nIdCells = lCells2.nIdentDrTimes(&(cl2.lCells2));
    dCells  = numCells2 <= cl2.numCells2 ?    numCells2-nIdCells :
                                          cl2.numCells2-nIdCells;
    //!!!!!!!!!!!!!!  ????????? 6 !!!!!!   One need to tune next line:
    if(nIdCells < 6  || dCells+2 > nIdCells) return 0;
    dNCells = numCells2 - cl2.numCells2;
    dLayers = nLayers2  - cl2.nLayers2;
  } else {
    Int_t nCells1  =     numCells1 +     numCells2;
    Int_t nCells2  = cl2.numCells1 + cl2.numCells2;
    nIdCells = lCells1.nIdentDrTimes(&(cl2.lCells1)) +
               lCells2.nIdentDrTimes(&(cl2.lCells2));
    dCells   = nCells1 <= nCells2 ? nCells1-nIdCells : nCells2-nIdCells;
    //!!!!!!!!!!!!!!  ????????? 6 !!!!!!   One need to tune next line:
    if(nIdCells < 6  || dCells+2 > nIdCells) return 0;
    dNCells  = nCells1 - nCells2;
    dLayers  = (nLayers1+nLayers2)  - (cl2.nLayers1+cl2.nLayers2);
  }
  
  
  Int_t nDBins = nBins - cl2.nBins;
  Int_t dRLevel = (Int_t)(minWt+0.1) - (Int_t)(cl2.minWt+0.1);
  Int_t inclFlag = 0;                //  inclFlag==0 continue loop!

  // Don't change order without testing efficiency!:
  
  if(dCells==0) {  //== dCells==0 ============================:
    if     (nDBins>0 && dNCells>0)   inclFlag = 3; // delete cl2
    else if(nDBins<0 && dNCells<0)   inclFlag = 2; // replace "this"
    else if(dRLevel > 0)             inclFlag = 3; // delete cl2
    else if(dRLevel < 0)             inclFlag = 2; // replace "this"
    else if(nDBins  > 0)             inclFlag = 3; // delete cl2
    else if(nDBins  < 0)             inclFlag = 2; // replace "this"
    else if(dNCells > 0)             inclFlag = 3; // delete cl2
    else if(dNCells < 0)             inclFlag = 2; // replace "this"
    else                             inclFlag = 1; // add "this" to cl2  
  } else if(dCells==1) { //== dCells==1 ============================:
//???? 2 and 4 are parameters. ???
    if     (nDBins> 2 && dNCells> 4) inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-4) inclFlag = 2; // replace "this"
    else if(dRLevel > 0)             inclFlag = 3; // delete cl2
    else if(dRLevel < 0)             inclFlag = 2; // replace "this"
    else if(dLayers > 0)             inclFlag = 3; // delete cl2
    else if(dLayers < 0)             inclFlag = 2; // replace "this"
    else if(nDBins  > 0)             inclFlag = 3; // delete cl2
    else if(nDBins  < 0)             inclFlag = 2; // replace "this"
    else if(dNCells > 0)             inclFlag = 3; // delete cl2
    else if(dNCells < 0)             inclFlag = 2; // replace "this"
    else                             inclFlag = 1; // add "this" to cl2
  } else if(dCells==2) { //== dCells==2 ============================:
//???? 2 and 6 are parameters. ???
    if     (nDBins> 2 && dNCells> 6)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-6)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    else                              inclFlag = 1; // add "this" to cl2
  } else if(dCells==3) { //== dCells==3 ============================:
//???? 2 and 7 are parameters. ???
    if     (nDBins> 2 && dNCells> 7)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-7)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    else                              inclFlag = 1; // add "this" to cl2
  } else if(dCells==4) { //== dCells==4 ============================:
//???? 2 and 8 are parameters. ???
    if     (nDBins> 2 && dNCells> 8)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-8)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    else                              inclFlag = 1; // add "this" to cl2
  }

  if(inclFlag == 3) {                     // "cl2" cluster will not used.  Stop loop!
    cl2.clusMerg = this;
    cl2.setFakeFlagAndStatus(dCells+1);   // dCells+1 for tuning of fake suppr.cuts
  } else if(inclFlag == 2) {              // "this" cluster will not used. Continue loop!
    clusMerg = &cl2;
    setFakeFlagAndStatus(dCells+1);       // dCells+1 for tuning of fake suppr.cuts
  } else if(inclFlag == 1) {              // add "this" to "cl2" and repeat loop!
    cl2.addClus(*this);
    clusMerg = &cl2;
    setFakeFlagAndStatus(dCells+1);       // dCells+1 for tuning of fake suppr.cuts
  }
  return inclFlag;
}

Int_t HMdcCluster::testForInc34CFnd(HMdcCluster& cl2) {
  // For HMdc34ClFinder !
  // Function test are wires from one cluster fully included in another cluster.
  // Index of "this" obj. in array must be < index cl2 !!!
  // return kTRUE if "cl2" is included to "this" (STOP LOOP)
  // return kFALSE cl2 is not included to "this" or "this" is included to "cl2"

  if(segment != 1) return 0; //?????
  
  if(numCells2<0) {
    numCells2 = lCells2.getNCells();
    nLayers2  = lCells2.getNLayers();
  }
  if(cl2.numCells2<0) {
    cl2.numCells2 = cl2.lCells2.getNCells();
    cl2.nLayers2  = cl2.lCells2.getNLayers();
  }
    
  // dCells - Number of not identical cells (for smaller cluster)
  //                       nIdentDrTimes return num.cells!
  Int_t nIdCells = lCells2.nIdentDrTimes(&(cl2.lCells2));
  Int_t dCells  = numCells2 <= cl2.numCells2 ? numCells2-nIdCells : cl2.numCells2-nIdCells;
  //!!!!!!!!!!!!!!  ????????? 6 !!!!!!   One need to tune next line:
  if(nIdCells < 6  || dCells+2 > nIdCells) return 0;
  Int_t dNCells = numCells2 - cl2.numCells2;
  Int_t dLayers = nLayers2  - cl2.nLayers2;

  Int_t nDBins = nBins - cl2.nBins;
  Int_t dRLevel = (Int_t)(minWt+0.1) - (Int_t)(cl2.minWt+0.1);
  Int_t inclFlag = 0;

  // Don't change order without testing efficiency!:
  
  if(dCells==0) {  //== dCells==0 ============================:
    if     (nDBins>0 && dNCells>0)   inclFlag = 3; // delete cl2
    else if(nDBins<0 && dNCells<0)   inclFlag = 2; // replace "this"
    else if(dRLevel > 0)             inclFlag = 3; // delete cl2
    else if(dRLevel < 0)             inclFlag = 2; // replace "this"
    else if(nDBins  > 0)             inclFlag = 3; // delete cl2
    else if(nDBins  < 0)             inclFlag = 2; // replace "this"
    else if(dNCells > 0)             inclFlag = 3; // delete cl2
    else if(dNCells < 0)             inclFlag = 2; // replace "this"
    else                             inclFlag = 1; // add "this" to cl2
  } else if(dCells==1) { //== dCells==1 ============================:
//???? 2 and 4 are parameters. ???
    if     (nDBins> 2 && dNCells> 4) inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-4) inclFlag = 2; // replace "this"
    else if(dRLevel > 0)             inclFlag = 3; // delete cl2
    else if(dRLevel < 0)             inclFlag = 2; // replace "this"
    else if(dLayers > 0)             inclFlag = 3; // delete cl2
    else if(dLayers < 0)             inclFlag = 2; // replace "this"
    else if(nDBins  > 0)             inclFlag = 3; // delete cl2
    else if(nDBins  < 0)             inclFlag = 2; // replace "this"
    else if(dNCells > 0)             inclFlag = 3; // delete cl2
    else if(dNCells < 0)             inclFlag = 2; // replace "this"
    else                             inclFlag = 1; // add "this" to cl2
  } else if(dCells==2) { //== dCells==2 ============================:
//???? 2 and 6 are parameters. ???
    if     (nDBins> 2 && dNCells> 6)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-6)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    // -no for sim.data    else                           inclFlag = 1; // add "this" to cl2  ---------------?
  } else if(dCells==3) { //== dCells==3 ============================:
//???? 2 and 6 are parameters. ???
    if     (nDBins> 2 && dNCells> 7)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-7)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    // -no for sim.data    else                           inclFlag = 1; // add "this" to cl2  ---------------?
  } else if(dCells==4) { //== dCells==4 ============================:
//???? 2 and 6 are parameters. ???
    if     (nDBins> 2 && dNCells> 8)  inclFlag = 3; // delete cl2
    else if(nDBins<-2 && dNCells<-8)  inclFlag = 2; // replace "this"
    else if(dRLevel > 0)              inclFlag = 3; // delete cl2
    else if(dRLevel < 0)              inclFlag = 2; // replace "this"
    else if(dLayers > 0)              inclFlag = 3; // delete cl2
    else if(dLayers < 0)              inclFlag = 2; // replace "this"
    else if(nDBins  > 0)              inclFlag = 3; // delete cl2
    else if(nDBins  < 0)              inclFlag = 2; // replace "this"
    else if(dNCells > 0)              inclFlag = 3; // delete cl2
    else if(dNCells < 0)              inclFlag = 2; // replace "this"
    // -no for sim.data    else                           inclFlag = 1; // add "this" to cl2
  }
  
  if(inclFlag == 3) {                     // "cl2" cluster will not used.  Stop loop!
    cl2.clusMerg = this;
    cl2.setFakeFlagAndStatus(dCells+1);   // dCells+1 for tuning of fake suppr.cuts
  } else if(inclFlag == 2) {              // "this" cluster will not used. Continue loop!
    clusMerg = &cl2;
    setFakeFlagAndStatus(dCells+1);       // dCells+1 for tuning of fake suppr.cuts
  } else if(inclFlag == 1) {              // add "this" to "cl2" and repeat loop!
    cl2.addClus(*this);
    clusMerg = &cl2;
    setFakeFlagAndStatus(dCells+1);       // dCells+1 for tuning of fake suppr.cuts
  }
  return inclFlag;
}

void HMdcCluster::setMod1ClusSkipMerg(HMdcCluster* clusM1) {
  clusMod1 = clusM1;
  // Skip merged clusters:
  while(clusMod1->clusMerg != 0) clusMod1 = clusMod1->clusMerg;
  clusMod1->flag++;
}

void HMdcCluster::setMod2ClusSkipMerg(HMdcCluster* clusM2) {
  clusMod2 = clusM2;
  // Skip merged clusters:
  while(clusMod2->clusMerg != 0) clusMod2 = clusMod2->clusMerg;
  clusMod2->flag++;
}

Bool_t HMdcCluster::isModClusNotTheSame(HMdcCluster& cls2) {
  if(clusMod1!=0 && cls2.clusMod1!=0 && clusMod1!=cls2.clusMod1) return kTRUE;
  if(clusMod2!=0 && cls2.clusMod2!=0 && clusMod2!=cls2.clusMod2) return kTRUE;
  return kFALSE;
}

Bool_t HMdcCluster::testNLayersSeg1(void) {
  return minWt <= lCells1.getNLayers() + 1;
}

Bool_t HMdcCluster::testNLayersSeg2(void) {
  return minWt <= lCells2.getNLayers() + 1;
}

Int_t HMdcCluster::getNUniqueWiresSeg1(HMdcSecListCells* pListCells) {
  if(fakeFlag==0) return pListCells->getNUniqueWires(0,lCells1);
  else            return pListCells->getNUniqueWires2(0,lCells1);
}

Int_t HMdcCluster::getNUniqueAndRWiresSeg1(HMdcSecListCells* pListCells,Int_t& nRWires) {
  if(fakeFlag==0) return pListCells->getNUniqueAndRWires(0,lCells1,nRWires);
  else            return pListCells->getNUniqueAndRWires2(0,lCells1,nRWires);
}

void HMdcCluster::resetFakeFlagSeg1(Bool_t fake,HMdcSecListCells* pListCells) {
  if(fake && fakeFlag == 0) {
    fakeFlag = 11;
    pListCells->subFrClusCounters(0,lCells1);
    if(realFlag > 0) {
      realFlag = 0;
      pListCells->subFrRClusCounters(0,lCells1);
    }
  } else if(!fake && fakeFlag > 0) {
    fakeFlag = 0;
    pListCells->addToClusCounters(0,lCells1);
  }
}

void HMdcCluster::resetRealFlagSeg1(Bool_t real,HMdcSecListCells* pListCells) {
  if(real && realFlag == 0) {
    if(fakeFlag > 0) {
      fakeFlag = 0;
      pListCells->addToClusCounters(0,lCells1);
    }
    realFlag = 1;
    pListCells->addToRClusCounters(0,lCells1);
  }
  else if(!real && realFlag > 0) {
    realFlag = 0;
    pListCells->subFrRClusCounters(0,lCells1);
  }
}

//===HMdcClustersArrs===========================================================

HMdcClustersArrs* HMdcClustersArrs::pMdcClustersArrs = 0;

HMdcClustersArrs* HMdcClustersArrs::getObject(void) {
  if(!pMdcClustersArrs) pMdcClustersArrs = new HMdcClustersArrs();
  return pMdcClustersArrs;
}

void HMdcClustersArrs::deleteCont(void) {
  if(pMdcClustersArrs != 0) delete pMdcClustersArrs;
}

HMdcClustersArrs::HMdcClustersArrs(void) {
  for(Int_t i=0;i<3;i++) {
    clusArr[i] = 0;
    arrSize[i] = 0;
  }
  dXWind1 =  40.; // ??? 40. ???
  dYWind1 =  10.;
  dXWind2 = 120.; // ??? 40. ???
  dYWind2 =  40.;
}

HMdcClustersArrs::~HMdcClustersArrs(void) {
  if(this == pMdcClustersArrs) pMdcClustersArrs = 0;
  for(Int_t i=0;i<3;i++) if(clusArr[i]) {
    delete [] clusArr[i];
    clusArr[i] = 0;
  }
}

Int_t HMdcClustersArrs::createAllArrays(Int_t size) {
  if(createArray(0,size) == 0) return 0;
  size = arrSize[0];
  if(createArray(1,size) == 0) return 0;
  size = arrSize[1];
  if(createArray(2,size) == 0) return 0;
  return size;
}

HMdcCluster* HMdcClustersArrs::createArray(Int_t ind,Int_t size) {
  // return pointer to the array of HMdcCluster objects
  // if array is not created it will be created
  if(clusArr[ind] != 0) size = arrSize[ind];
  else if(size>0) {
    clusArr[ind] = new HMdcCluster [size];
    arrSize[ind] = size;
  }
  return clusArr[ind];
}

HMdcCluster* HMdcClustersArrs::getCluster(Int_t indarr,Int_t  indClus) {
  if(clusArr[indarr]==0 || indClus>=arrSize[indarr] || indClus<0) return 0;
  return &(clusArr[indarr][indClus]);
}

void HMdcClustersArrs::testCluster(Int_t indArr,Int_t indClus) {
  HMdcCluster* clArr = clusArr[indArr];
  for(Int_t iCl=0;iCl<indClus;iCl++) if(clArr[iCl].getStatus()) {
    // Optimization:
    if(TMath::Abs(clArr[iCl].getY()-clArr[indClus].getY()) > dYWind1) continue;
    if(TMath::Abs(clArr[iCl].getX()-clArr[indClus].getX()) > dXWind1) continue;
    Int_t inclFlag = clArr[iCl].testForInclude(clArr[indClus]);
    if( inclFlag == 0 || inclFlag == 2 ) continue;
    if( inclFlag == 1 ) testCluster(indArr,indClus);
    return;
  }
}

Bool_t HMdcClustersArrs::testMdc34Cluster(Int_t indArr,Int_t indClus,Int_t firstClst) {
  // return kTRUE if cluster "indClus" is removed
  HMdcCluster* clArr = clusArr[indArr];
  for(Int_t iCl=firstClst;iCl<indClus;iCl++) if(clArr[iCl].getStatus()) {
    // Optimization:
    if(TMath::Abs(clArr[iCl].getY()-clArr[indClus].getY()) > dYWind2) continue;
    if(TMath::Abs(clArr[iCl].getX()-clArr[indClus].getX()) > dXWind2) continue;
    Int_t inclFlag = clArr[iCl].testForInc34CFnd(clArr[indClus]);
    if( inclFlag == 0 || inclFlag == 2 ) continue;
    if( inclFlag == 1 ) return testMdc34Cluster(indArr,indClus,firstClst);
    return kTRUE;    // inclFlag == 3 - remove cluster "indcClus"
  }
  return kFALSE;
}
