//*-- Author : H.Agakichiev & V.Pechenov
//*-- Modified : 30.06.2004 by V.Pechenov

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
// HMdcTrackFinder
//
// The program for candidates to tracks finding.
//
// Using:
// Put in task list task:
// HMdcTrackFinder* trackFinder;
//
//////////////////////////////////////////////////////////////////////

#include "hmdcalignerd.h"
#include "hmdcalignerparam.h"

#include "hiterator.h"
#include "hmdclookuptb.h"
#include "hmdctrackfittera.h"
#include "hmdctrackfitterb.h"
#include "hmdccal1sim.h"
#include "hcategory.h"
#include "hmdctrackdset.h"
#include "hmdcclussim.h"
#include "hmdcsizescells.h"
#include "hmdctrackfitter.h"
#include "hmdclistcells.h"
#include "hmdcminimize.h"
#include "hgeomvertexfit.h"
#include "hgeomvector.h"
#include "TH2.h"
#include "TCanvas.h"
#include "TFile.h"
#include "TNtuple.h"

Double_t clusterFunctional(TObject *obj,Int_t& nPar, Double_t* par);
Double_t alignmentFunctional(TObject *obj,Int_t& nPar, Double_t* par);
Double_t directionFunctional(TObject *obj,Int_t& nPar, Double_t* par);
Double_t beamLine(TObject *obj,Int_t& nPar, Double_t* par);

HMdcAlignerD::HMdcAlignerD(Bool_t isCOff, Int_t typeClFn) 
    : HMdcTrackFinder(isCOff,typeClFn) {
  event=0;
  param = new HMdcAlignerParam();
}
    
HMdcAlignerD::HMdcAlignerD(const Text_t *name,const Text_t *title, Bool_t isCOff, Int_t typeClFn)
    : HMdcTrackFinder(name,title,isCOff,typeClFn) {
  event=0;
  param = new HMdcAlignerParam();
}

HMdcAlignerD::HMdcAlignerD(void){
  event=0;
  param = new HMdcAlignerParam();
}

HMdcAlignerD::HMdcAlignerD(const Text_t *name,const Text_t *title) :
    HMdcTrackFinder(name,title) {
  event=0;
  param = new HMdcAlignerParam();

}

HMdcAlignerD::~HMdcAlignerD() {
  if(HMdcTrackFinder::event) {
    delete event;
    event=0;
    HMdcTrackFinder::event=0;
  }
}

Bool_t HMdcAlignerD::init(void) {
  event=new HMdcEvntListCells;
  HMdcTrackFinder::event=event;
  HMdcTrackFinder::init();
  return kTRUE;
}

Bool_t HMdcAlignerD::reinit(void) {
   
  HMdcTrackFinder::reinit();
  fLookUpTb->setQuietMode();        // switch off print
  fLookUpTb->fillTrackList(kFALSE); // don't fill list of tracks for sim.data
  param->init();
  param->print(" initial value "); cout << endl;
  return kTRUE;
}

Int_t HMdcAlignerD::execute(void) {
//   iter->Reset();
//   Int_t  nHits;
//   event->clear();
//   HMdcCal1* cal;
// //   Int_t nWires = 0;
//   while ((cal=nextCell(nHits)) != 0) {
//     Int_t s,m,l,c;
//     cal->getAddress(s,m,l,c);
//     
//     if(s != param->getAlignSec()) continue;
// //     nWires++;
// 
//     if(nHits==1) event->addTime1(s,m,l,c,cal->getTime1());
//     else if(nHits==2) event->addTime1(s,m,l,c,cal->getTime2()); //???
//     else continue; // to keep 2 dr.times in EvntListCells is not possible yet
//   }

  event->clear();
/*  Int_t  nHits =*/ event->collectWires(param->getAlignSec());
//   if (nWires > 24) storeWires.addEvent(*event);
  storeWires.addEvent(*event);
  return 0;
}

Bool_t HMdcAlignerD::finalize(void) {

   if(param->getAlignSec() < 0 || param->getAlignSec() > 5) return kTRUE;
   
   fitpar = new HMdcTrackFitInOut();// obj. keep pointer to param. and categ.
   fitpar->init();
   fitpar->reinit();
   fitpar->setModuleTof();
   fitpar->setUseTukeyFlag(kFALSE);
   if(param->getScan())
      fitter = new HMdcTrackFitterB(fitpar);
   else
      fitter = new HMdcTrackFitterA(fitpar);
   
   HMdcMinimize min;
      
   fLookUpTb->setTargLenInc(15,15);   // aug04
   fLookUpTb->initContainer();
   
   Int_t nPar = param->getNMinParams();

   Double_t * alignPar = param->getMinParams();   
   Double_t * steps = param->getMinSteps();
   
//   Double_t beamPar[3] = {0, 0, 0};
//   Double_t beamSteps[2] = {1., 1.};

   Double_t alignParFit[24];   
   Double_t alignParClus[24];
   Int_t numcl;

   if(nPar == 0) { monitor(); return kTRUE;}
   
   if(param->getCluster()) {
      nPar = 12;
      min.setFCN((TObject*)this,clusterFunctional);
      min.random(500,nPar,alignPar,alignPar);
      return kTRUE;
   }

   Int_t offset = param->getOffset();;
   
   min.setFCN((TObject*)this,alignmentFunctional);
   if(offset == 3) min.setFCN((TObject*)this,directionFunctional);
   
   Double_t functOldCluster = 10000000000.;
   for(Int_t itClus = 0; itClus < 300; itClus++) {            //iteration for clusters
   
      storeWires.resetIter();
      numcl = 0;
      delete storeClusters;
      storeClusters = new HMdcStoreClusters();

  
      while(storeWires.getNextEvent(*event)) { // loop over events to find clusters

	 fClusCat->Clear();             // Category cleaning
	 calcClFndrLevel();             // Cluster finder levels determination
	 numcl += clFndrBeforField();    // Cl.finder in MDCI-II/all(without mag.field)

	 iterClus->Reset();
	 HMdcClus * fClst1;
	 HMdcClus * fClst2 = 0;
      
      
	 while((fClst1=(HMdcClus*)iterClus->Next())) {
	    if(fClst1->getIndexParent() >= 0) continue;
	 
	    Int_t first,last;
	    fClst1->getIndexRegChilds(first,last);
	    fClst2 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
	    
	    Double_t xFirstMod, yFirstMod, xLastMod, yLastMod;
	    
	    param->getModTrackParams(fClst1->getXTarg(),fClst1->getYTarg(),fClst1->getZTarg(),
						fClst1->getX(),fClst1->getY(),fClst1->getZ(),
						xFirstMod, yFirstMod, xLastMod, yLastMod);
  
	    storeClusters->addClustWires(*event,fClst1,fClst2);
//	    storeClusters->setClusterPar(xFirstMod, yFirstMod, xLastMod, yLastMod);
            storeClusters->setTrackPar(param->getAlignSec(),
                param->getFirstMod(), xFirstMod, yFirstMod, 
                param->getLastMod(), xLastMod, yLastMod);
	    storeClusters->setEndCluster();
	 }
         storeClusters->setEndEvent();
	 
      } // end loop over events to find clusters
      
//       if(offset == 3) {
// 	 min.setFCN((TObject*)this,directionFunctional);
// 	 min.minimize(50,nPar,alignPar,steps,alignPar);
// 	 param->setNewPosition(alignPar,offset);
// 	 monitor();
// 	 return kTRUE;
//       }
   
      Bool_t iniPar = kTRUE;//kFALSE;
   
      Double_t funct, functOld = 10000000000.;
      for(Int_t itFit = 0; itFit < 50; itFit++) {            //iteration for fit
      
	 Double_t sumFunctional = 0;
	 Double_t nClus = 0;
	 storeClusters->resetIter();
	 
         while(storeClusters->nextEvent()) {
	   while(storeClusters->getNextCluster(*event)) {     //loop over clusters
      
	      Double_t x1 = storeClusters->getX1();
	      Double_t y1 = storeClusters->getY1();
	      Double_t z1 = 0.;
	      Double_t x2 = storeClusters->getX2();
	      Double_t y2 = storeClusters->getY2();
	      Double_t z2 = 0.;

	      param->getSecTrackParams(x1, y1, z1, x2, y2, z2); 

	      if(!fitter->setClustAndFill(event,x1, y1, z1, x2, y2, z2)) continue;

	      fitter->setRegionOfWires();

	      if(!fitter->fitCluster(300000)) {
	         storeClusters->resetTrackPar(-999, -999, -999, -999);
	         continue;
	      }

	      if(fitter->getChi2() > 300.) continue;

  // 	 cout << " finalize " << fitter->getFinalParam()->x1() << " " << fitter->getFinalParam()->y1() << " "
  // 	      << fitter->getFinalParam()->z1() << " " << fitter->getFinalParam()->x2() << " " << fitter->getFinalParam()->y2()
  // 	      << " " << fitter->getFinalParam()->z2() << " " << fitter->getChi2() << endl;

	      sumFunctional += fitter->getChi2();
	      nClus++ ;

	      Double_t xFirstMod, yFirstMod, xLastMod, yLastMod;

	      param->getModTrackParams(fitter->getFinalParam()->x1(),fitter->getFinalParam()->y1(),fitter->getFinalParam()->z1(),
				       fitter->getFinalParam()->x2(),fitter->getFinalParam()->y2(),fitter->getFinalParam()->z2(),
				       xFirstMod, yFirstMod, xLastMod, yLastMod);

	      storeClusters->resetTrackPar(xFirstMod, yFirstMod, xLastMod, yLastMod);

	   } //end loop over clusters
         }
	 
	 param->print(" finalize functional "); cout << numcl << " " << nClus << " " << sumFunctional/nClus  << endl;
      
	 funct = sumFunctional/nClus;
      
	 if(funct >= functOld) {
	 
	    for(Int_t i=0; i<nPar; i++) alignPar[i] = alignParFit[i];
	    param->setNewPosition(alignPar,offset);

	    param->print(" finalize best functional "); cout << numcl << " " << nClus << " " << functOld << endl;
	    
	    if(iniPar) break;
	    else iniPar = kTRUE;
	 }
      
	 functOld = funct;
   
	 for(Int_t i=0; i<nPar; i++) alignParFit[i] = alignPar[i];
	 param->setNewPosition(alignPar,offset);
      
	 if(iniPar) {	    
	    if(nPar == 0) {
// 	       min.setFCN((TObject*)this,beamLine);
// 	       min.minimize(50,2,beamPar,beamSteps,beamPar);
// 	       HGeomVector * target = new HGeomVector(beamPar[0], beamPar[1], beamPar[2]);
	       HGeomVector * target = 0;
	       
	       monitor(target);
	       return kTRUE;
	    }
	    min.minimize(50,nPar,alignPar,steps,alignPar);
	    param->setNewPosition(alignPar,offset);
	 }

      }  //End fit iteration
      
      param->print(" finalize best cluster functional "); cout << numcl << " " << " " << functOld << " " << functOldCluster << endl;
      
      if(functOld >= functOldCluster) {
	 for(Int_t i=0; i<nPar; i++) alignPar[i] = alignParClus[i];
	 param->setNewPosition(alignPar,offset);
	 break;
      }
      
      functOldCluster = functOld;
      for(Int_t i=0; i<nPar; i++) alignParClus[i] = alignPar[i];
      param->setNewPosition(alignPar,offset);

   } //End clusterfinder iteration
    
//    min.setFCN((TObject*)this,beamLine);
//    min.minimize(50,2,beamPar,beamSteps,beamPar);
//    HGeomVector * target = new HGeomVector(beamPar[0], beamPar[1], beamPar[2]);
   HGeomVector * target = 0;
	       
   monitor(target);
   return kTRUE;
}

Double_t clusterFunctional(TObject * obj,Int_t& nPar, Double_t* par) {

   HMdcAlignerD * aligner = (HMdcAlignerD *)obj;
   HMdcAlignerParam * param = aligner->getParam();
   HMdcStoreEvents * store = aligner->getStoreWires();
   HMdcEvntListCells * event = aligner->getEvent();
//    HCategory * fClusCat = aligner->getClusCat();
   
   param->setNewPosition(par,param->getOffset());

   //========= Event loop:=========================================
   store->resetIter();
   Double_t numcl = 0;
   
   while(store->getNextEvent(*event)) {
      
//       fClusCat->Clear();             // Category cleaning
      aligner->calcClFndrLevel();             // Cluster finder levels determination
      numcl += aligner->clFndrBeforField(); // Cl.finder in MDCI-II/all(without mag.field)
   }
   
   if(numcl == 0) numcl = 1;
   
   Double_t f = 2500000./numcl;
   
   param->print(" cluster functional "); cout <<  numcl << " " << f << endl;
   
  return (f==0) ? 1000000. : f;

}

Double_t alignmentFunctional(TObject * obj,Int_t& nPar, Double_t* par) {

   HMdcAlignerD * aligner = (HMdcAlignerD *)obj;
   HMdcAlignerParam * param = aligner->getParam();
   HMdcStoreClusters & store = *(aligner->getStoreClusters());
   HMdcEvntListCells * event = aligner->getEvent();
   HMdcTrackFitter * fitter = aligner->getFitter();
   
   param->setNewPosition(par,param->getOffset());

   //========= Event loop:=========================================
   
   store.resetIter();
   Double_t sumFunctional = 0;
   Double_t nClus = 0;
   Double_t nCells = 0;
   
   while(store.nextEvent()) {
     while(store.getNextCluster(*event)) {

        Double_t x1 = store.getX1();
        Double_t y1 = store.getY1();
        Double_t z1 = 0.;
        Double_t x2 = store.getX2();
        Double_t y2 = store.getY2();
        Double_t z2 = 0.;

        if(x1 < -998) continue;

        param->getSecTrackParams(x1, y1, z1, x2, y2, z2);

        if(!fitter->setClustAndFill(event,x1, y1, z1, x2, y2, z2)) continue;

        fitter->setRegionOfWires();

        fitter->fillOutput();

  // 	 cout << " alignmentFunctional functional " << fitter->getFinalParam()->x1() << " " << fitter->getFinalParam()->y1() << " "
  // 	      << fitter->getFinalParam()->z1() << " " << fitter->getFinalParam()->x2() << " " << fitter->getFinalParam()->y2() << " "
  // 	      << fitter->getFinalParam()->z2() << " " << fitter->getChi2() << endl;

        if(fitter->getChi2() > 300.) continue;

        nCells += fitter->getWiresArr().getNumOfGoodWires();	 
        sumFunctional += fitter->getChi2();
        nClus++ ;

     }
   }
 
   if(nClus == 0) nClus = 1;
   if(nCells == 0) nCells = 1;
   
   Double_t f = sumFunctional/nClus;

   param->print(" alignment functional "); cout	<<  nClus << " " << nCells << " " << sumFunctional/nClus << " " << f << endl;
   
  return (f==0) ? 1000000. : f;
  
}

Double_t directionFunctional(TObject * obj,Int_t& nPar, Double_t* par) {

   HMdcAlignerD * aligner = (HMdcAlignerD *)obj;
   HMdcAlignerParam * param = aligner->getParam();
   HMdcStoreClusters & store = *(aligner->getStoreClusters());
   HMdcEvntListCells * event = aligner->getEvent();
   HMdcTrackFitter * fitter = aligner->getFitter();
   
   param->setNewPosition(par,param->getOffset());

   //========= Event loop:=========================================
   
   store.resetIter();
   Double_t sumFunctional = 0;
   Double_t nClus = 0;
   Double_t nCells = 0;
   
   while(store.nextEvent()) {
     while(store.getNextCluster(*event)) {

        Double_t xDir[4] = {-10000, -10000, -10000, -10000};
        Double_t yDir[4] = {-10000, -10000, -10000, -10000};

        Double_t x1 = store.getX1();
        Double_t y1 = store.getY1();
        Double_t z1 = 0.;
        Double_t x2 = store.getX2();
        Double_t y2 = store.getY2();
        Double_t z2 = 0.;

        if(x1 < -998) continue;

        param->getSecTrackParams(x1, y1, z1, x2, y2, z2);

        if(!fitter->setClustAndFill(event,x1, y1, z1, x2, y2, z2)) continue;

        for(Int_t iMod=1; iMod>-1; iMod--) {
	   fitter->setRegionOfWires(iMod);
	   if(!fitter->fitCluster(300000)) continue;
	   if(fitter->getChi2() > 300.) continue;

	   Double_t fitX1 = fitter->getFinalParam()->x1();
	   Double_t fitY1 = fitter->getFinalParam()->y1();
	   Double_t fitZ1 = fitter->getFinalParam()->z1();
	   Double_t fitX2 = fitter->getFinalParam()->x2();
	   Double_t fitY2 = fitter->getFinalParam()->y2();
	   Double_t fitZ2 = fitter->getFinalParam()->z2();
	   Double_t dx = fitX2 - fitX1;
	   Double_t dy = fitY2 - fitY1;
	   Double_t dz = fitZ2 - fitZ1;
	   Double_t r = 1./sqrt(dx*dx+dy*dy+dz*dz);
	   dx *= r;
	   dy *= r;
	   dz *= r;
	   const Double_t * tfSysRSec = param->getTfSysRSec(iMod);

	   xDir[iMod] = tfSysRSec[0]*dx + tfSysRSec[3]*dy + tfSysRSec[6]*dz;
	   yDir[iMod] = tfSysRSec[1]*dx + tfSysRSec[4]*dy + tfSysRSec[7]*dz;

	   cout << iMod << " " << fitter->getChi2() << " " << xDir[iMod] << " " << yDir[iMod] << endl; 
        }
        if(xDir[0] == -10000 || xDir[1] == -10000) continue;



  // 	 cout << " alignmentFunctional functional " << fitter->getFinalParam()->x1() << " " << fitter->getFinalParam()->y1() << " "
  // 	      << fitter->getFinalParam()->z1() << " " << fitter->getFinalParam()->x2() << " " << fitter->getFinalParam()->y2() << " "
  // 	      << fitter->getFinalParam()->z2() << " " << fitter->getChi2() << endl;

        nCells += fitter->getWiresArr().getNumOfGoodWires();	 
        sumFunctional += (xDir[0] - xDir[1])*(xDir[0] - xDir[1]) + (yDir[0] - yDir[1])*(yDir[0] - yDir[1]);
        nClus++ ;

     }
   }
 
   if(nClus == 0) nClus = 1;
   if(nCells == 0) nCells = 1;
   
   Double_t f = sumFunctional/nClus;

   param->print(" direction functional "); cout	<<  nClus << " " << nCells << " " << f << endl;
   
  return (f==0) ? 1000000. : f;
  
}

Double_t beamLine(TObject * obj,Int_t& nPar, Double_t* par) {

   HMdcAlignerD * aligner = (HMdcAlignerD *)obj;
   HMdcAlignerParam * param = aligner->getParam();
   HMdcStoreClusters & store = *(aligner->getStoreClusters());
   HMdcEvntListCells * event = aligner->getEvent();
   HMdcTrackFitter * fitter = aligner->getFitter();
   
   //========= Event loop:=========================================
   
   store.resetIter();
//   Double_t sumFunctional = 0;
   Double_t nClus = 0;
//   Double_t nCells = 0;
   Double_t sumr = 0;
   Double_t sumr2 = 0;
   
   while(store.nextEvent()) {
     while(store.getNextCluster(*event)) {

        Double_t x1 = store.getX1();
        Double_t y1 = store.getY1();
        Double_t z1 = 0.;
        Double_t x2 = store.getX2();
        Double_t y2 = store.getY2();
        Double_t z2 = 0.;

        if(x1 < -998) continue;

        param->getSecTrackParams(x1, y1, z1, x2, y2, z2);

        if(!fitter->setClustAndFill(event,x1, y1, z1, x2, y2, z2)) continue;

        fitter->setRegionOfWires();

        fitter->fillOutput();

        if(fitter->getChi2() > 300.) continue;

        Double_t z, r;

        HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();   
        fSizesCells->calcRZToLineXY(fitter->getFinalParam()->x1(),fitter->getFinalParam()->y1(),fitter->getFinalParam()->z1(),
				      fitter->getFinalParam()->x2(),fitter->getFinalParam()->y2(),fitter->getFinalParam()->z2(),
				      par[0], par[1], z, r);
        sumr += r;
        sumr2 += r*r;
        nClus++ ;

     }
   }
 
   if(nClus == 0) nClus = 1;
   
   Double_t sigma = sumr2/nClus - (sumr/nClus)*(sumr/nClus);
//    Double_t f = fabs(sumr/nClus);
   Double_t f = sumr2/nClus;
   
   cout << " beamLine " << par[0] << " " << par[1] << " " << nClus << " " << sigma << " " << f << endl;
   
  return (f==0) ? 1000000. : f;
  
}

// void HMdcAlignerD::monitor(HGeomVector * target) {   


//    HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
   
//    HMdcSizesCellsSec& fSCSec = (*fSizesCells)[param->getAlignSec()];
//    (fSCSec.getTargetMiddlePoint()).print();
//    param->setNewPosition(0,param->getOffset());
   
   
//    TFile * file = new TFile(param->getRootFile(),"recreate");
//    TNtuple * inner = new TNtuple("inner","deviations","dev0:dev1:dev2:dev3:dev4:dev5:dev6:dev7:dev8:dev9:dev10:dev11:ch2:nw:z");
//    TNtuple * outer = new TNtuple("outer","deviations","dev12:dev13:dev14:dev15:dev16:dev17:dev18:dev19:dev20:dev21:dev22:dev23:ch2:nw:z");
   
//    //========= Event loop:=========================================

//    storeWires.resetIter();
//    Double_t numcl = 0;
//    delete storeClusters;
//    storeClusters = new HMdcStoreClusters();
   
//    while(storeWires.getNextEvent(*event)) { // loop over events to find clusters

//       fClusCat->Clear();             // Category cleaning
//       calcClFndrLevel();             // Cluster finder levels determination
//       numcl += clFndrBeforField();    // Cl.finder in MDCI-II/all(without mag.field)
      
//       iterClus->Reset();
//       HMdcClus * fClst1;
//       HMdcClus * fClst2 = 0;
      
      
//       while((fClst1=(HMdcClus*)iterClus->Next())) {
// 	 if(fClst1->getIndexParent() >= 0) continue;
	 
// 	 Int_t first,last;
// 	 fClst1->getIndexRegChilds(first,last);
// 	 fClst2 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
	 
// 	 Double_t xFirstMod, yFirstMod, xLastMod, yLastMod;
	    
// 	 param->getModTrackParams(fClst1->getXTarg(),fClst1->getYTarg(),fClst1->getZTarg(),
// 				  fClst1->getX(),fClst1->getY(),fClst1->getZ(),
// 				  xFirstMod, yFirstMod, xLastMod, yLastMod);
  
// 	 storeClusters->addClustWires(*event,fClst1,fClst2);
// 	 storeClusters->setClusterPar(xFirstMod, yFirstMod, xLastMod, yLastMod);
// 	 storeClusters->setEndCluster();
//       }
	 
//    } // end loop over events to find clusters
   
      
//    storeClusters->resetIter();
//    Double_t sumFunctional = 0;
//    Double_t nClus = 0;
//    Double_t nCells = 0;   

//    while(storeClusters->getNextCluster(*event)) {     // loop over clusters 
      
//       Int_t segment = 1000;
//       fitter->setAddress(param->getAlignSec(),segment);
//       if(!fitter->fillListHits(event)) continue;

//       Double_t x1 = storeClusters->getX1();
//       Double_t y1 = storeClusters->getY1();
//       Double_t z1 = 0.;
//       Double_t x2 = storeClusters->getX2();
//       Double_t y2 = storeClusters->getY2();
//       Double_t z2 = 0.;
	 
//      param->getSecTrackParams(x1, y1, z1, x2, y2, z2);
	 
//       fitter->setTrackPar(x1, y1, z1, x2, y2, z2);
	 
//       fitter->setFittingTimesList();
	 
//       if(!fitter->fitCluster(300000)) continue;
	 
//       if(fitter->getChi2() > 300.) continue;
	 
//       Double_t z, r, dz;
//       Double_t zl, rl;
//       Double_t xv, yv, zv, xl, yl;
//       Double_t nC = fitter->getNumberOfGoodWires();	 
//       Double_t sF= fitter->getChi2();
      
//       fSCSec.calcRZToTargLine(fitter->getFinalParam()->x1(),fitter->getFinalParam()->y1(),
// 			      fitter->getFinalParam()->z1(),fitter->getFinalParam()->x2(),
// 			      fitter->getFinalParam()->y2(),fitter->getFinalParam()->z2(),
// 			      z, r);
//       Float_t dev[24] = {-1000,-1000,-1000,-1000,-1000,-1000,
// 			 -1000,-1000,-1000,-1000,-1000,-1000,
// 			 -1000,-1000,-1000,-1000,-1000,-1000,
// 			 -1000,-1000,-1000,-1000,-1000,-1000,};

//       for( Int_t nT = 0; nT < fitter->getLastTime(); nT++)
// 	 dev[fitter->getLayer(nT)] = fitter->getDeviation(nT);
// //       for( Int_t nT = 0; nT < fitter->getLastTime(); nT++) {
// // 	 if(fitter->getLayer(nT) !=  || fitter->getCell(nT) > 23) continue;
// // 	 dev[fitter->getCell(nT)] = fitter->getDeviation(nT);
// //       }
	 
//       inner->Fill(dev[0],dev[1],dev[2],dev[3],dev[4],dev[5],dev[6],dev[7],dev[8],
// 	       dev[9],dev[10],dev[11],float(sF),float(nC),float(z));
//       outer->Fill(dev[12],dev[13],dev[14],dev[15],dev[16],dev[17],dev[18],dev[19],dev[20],
// 	       dev[21],dev[22],dev[23],float(sF),float(nC),float(z));
	 
//       nCells += fitter->getNumberOfGoodWires();	 
//       sumFunctional += fitter->getChi2();
//       nClus++ ;
               
      
//    }

   
//    if(numcl == 0) numcl = 1;
//    if(nClus == 0) nClus = 1;
//    if(nCells == 0) nCells = 1;
   
//    Double_t f = sumFunctional/nClus;
   
    
//    param->print(" monitor "); cout <<  numcl << " " <<  nClus << " " << nCells << " " << sumFunctional/nClus << " " << " " << f << endl;	 
	 
//    Double_t targetPar[6] = {0,0,0,0,0,0};
//    HGeomTransform targetTrans;
   
//    param->printTransforms(targetPar);
   
//    inner->Write();
//    outer->Write();
//    file->Close();
	 
// }
   
void HMdcAlignerD::monitor(HGeomVector * target) {   


   HMdcSizesCells* fSizesCells   =  HMdcSizesCells::getObject();
//    if(target) fSizesCells->fillTargetPos(target);
   
   HMdcSizesCellsSec& fSCSec = (*fSizesCells)[param->getAlignSec()];
   (fSCSec.getTargetMiddlePoint()).print();
//    target->print();
   param->setNewPosition(0,param->getOffset());
   
   
   TFile * file = new TFile(param->getRootFile(),"recreate");
   
   TNtuple * inner = new TNtuple("inner","deviations","dev0:dev1:dev2:dev3:dev4:dev5:dev6:dev7:dev8:dev9:dev10:dev11:ch2:nw:z");
   TNtuple * outer = new TNtuple("outer","deviations","dev12:dev13:dev14:dev15:dev16:dev17:dev18:dev19:dev20:dev21:dev22:dev23:ch2:nw:z");
   TNtuple * zr = new TNtuple("ZR","traks","z:r:x:y:chi2:nw");
   
//    if(param->getOffset()) {
//       TNtuple * inner = new TNtuple("inner","deviations","dev0:dev1:dev2:dev3:dev4:dev5:dev6:dev7:dev8:dev9:dev10:dev11:ch2:nw:z");
//       TNtuple * outer = new TNtuple("outer","deviations","dev12:dev13:dev14:dev15:dev16:dev17:dev18:dev19:dev20:dev21:dev22:dev23:ch2:nw:z");
//    }
//    else {
//       TNtuple * zr = new TNtuple("ZR","traks","z:r:chi2:nw");
//    }
   
   //========= Event loop:=========================================

   storeWires.resetIter();
   Double_t numcl = 0;
   delete storeClusters;
   storeClusters = new HMdcStoreClusters();
   
   while(storeWires.getNextEvent(*event)) { // loop over events to find clusters

      fClusCat->Clear();             // Category cleaning
      calcClFndrLevel();             // Cluster finder levels determination
      numcl += clFndrBeforField();    // Cl.finder in MDCI-II/all(without mag.field)
      
      iterClus->Reset();
      HMdcClus * fClst1;
      HMdcClus * fClst2 = 0;
      
      
      while((fClst1=(HMdcClus*)iterClus->Next())) {
	 if(fClst1->getIndexParent() >= 0) continue;
	 
	 Int_t first,last;
	 fClst1->getIndexRegChilds(first,last);
	 fClst2 = (first<0) ? 0:(HMdcClus*)fClusCat->getObject(first);
	 
	 Double_t xFirstMod, yFirstMod, xLastMod, yLastMod;
	    
	 param->getModTrackParams(fClst1->getXTarg(),fClst1->getYTarg(),fClst1->getZTarg(),
				  fClst1->getX(),fClst1->getY(),fClst1->getZ(),
				  xFirstMod, yFirstMod, xLastMod, yLastMod);

	 storeClusters->addClustWires(*event,fClst1,fClst2);
//	 storeClusters->setClusterPar(xFirstMod, yFirstMod, xLastMod, yLastMod);
         storeClusters->setTrackPar(param->getAlignSec(),
                param->getFirstMod(), xFirstMod, yFirstMod, 
                param->getLastMod(), xLastMod, yLastMod);
	 storeClusters->setEndCluster();
      }
      storeClusters->setEndEvent();
	 
   } // end loop over events to find clusters
   
   storeClusters->resetIter();
   Double_t sumFunctional = 0;
   Double_t nClus = 0;
   Double_t nCells = 0;   
   HMdcEvntListCells cluster;

   while(storeClusters->nextEvent()) {
     while(storeClusters->getNextCluster(cluster)) {     // loop over clusters

        Double_t x1 = storeClusters->getX1();
        Double_t y1 = storeClusters->getY1();
        Double_t z1 = 0.;
        Double_t x2 = storeClusters->getX2();
        Double_t y2 = storeClusters->getY2();
        Double_t z2 = 0.;

        param->getSecTrackParams(x1, y1, z1, x2, y2, z2);

        if(!fitter->setClustAndFill(&cluster,x1, y1, z1, x2, y2, z2)) continue;

        fitter->setRegionOfWires();

  //       fitter->switchOffWires(param->getAlignSec(), 3, 2);

        if(!fitter->fitCluster(300000)) continue;
  // 	 fitter->fitCluster(300000);

        if(fitter->getChi2() > 300.) continue;

        Double_t z, r;
  //      Double_t zl, rl, dz;
  //      Double_t xv, yv, zv, xl, yl;
        Double_t nC = fitter->getWiresArr().getNumOfGoodWires();	 
        Double_t sF= fitter->getChi2();

        fSCSec.calcRZToTargLine(fitter->getFinalParam()->x1(),fitter->getFinalParam()->y1(),
			        fitter->getFinalParam()->z1(),fitter->getFinalParam()->x2(),
			        fitter->getFinalParam()->y2(),fitter->getFinalParam()->z2(),
			        z, r);

        Float_t dev[24] = {-1000,-1000,-1000,-1000,-1000,-1000,
			   -1000,-1000,-1000,-1000,-1000,-1000,
			   -1000,-1000,-1000,-1000,-1000,-1000,
			   -1000,-1000,-1000,-1000,-1000,-1000,};

        if(param->getOffset()) {
	   for( Int_t nT = 0; nT < fitter->getWiresArr().getLastTimeInd(); nT++)
	      dev[((fitter->getWiresArr())[nT]).getLayer()] = ((fitter->getWiresArr())[nT]).getDev();
  //       for( Int_t nT = 0; nT < fitter->getLastTime(); nT++) {
  // 	 if(fitter->getLayer(nT) !=  || fitter->getCell(nT) > 23) continue;
  // 	 dev[fitter->getCell(nT)] = fitter->getDeviation(nT);
  //       }

	   inner->Fill(dev[0],dev[1],dev[2],dev[3],dev[4],dev[5],dev[6],dev[7],dev[8],
		       dev[9],dev[10],dev[11],float(sF),float(nC),float(z));
	   outer->Fill(dev[12],dev[13],dev[14],dev[15],dev[16],dev[17],dev[18],dev[19],dev[20],
		       dev[21],dev[22],dev[23],float(sF),float(nC),float(z));
        }
        else {
	   zr->Fill(float(z), float(r), float(x2), float(y2), float(sF), float(nC));
        }

        nCells += fitter->getWiresArr().getNumOfGoodWires();	 
        sumFunctional += fitter->getChi2();
        nClus++ ;


     }
   }

   
   if(numcl == 0) numcl = 1;
   if(nClus == 0) nClus = 1;
   if(nCells == 0) nCells = 1;
   
   Double_t f = sumFunctional/nClus;
   
    
   param->print(" monitor "); cout <<  numcl << " " <<  nClus << " " << nCells << " " << sumFunctional/nClus << " " << " " << f << endl;	 
	 
   Double_t targetPar[6] = {0,0,0,0,0,0};
   HGeomTransform targetTrans;
   
   param->printTransforms(targetPar);
   
   if(param->getOffset()) {
      inner->Write();
      outer->Write();
   }
   else {
      zr->Write();
   }
   
   file->Close();
	 
}
   
void HMdcAlignerD::printCluster(HMdcClus* fClst) {
  Int_t sec=fClst->getSec();
  Int_t seg=fClst->getIOSeg();
  Int_t l,c;
  l=c=-1;
  Int_t lOld=l;
  Int_t lay=-1;
  Int_t mod=-1;
  printf("=Cluster:=== Sec. %i ===============\n",sec);
  while(fClst->getNextCell(l,c)) {
    if(l != lOld) {
      lay=l%6;
      mod=l%6 + seg*2;
      printf("%i mod. %i layer:",mod,lay);
      if(lOld>=0) printf("\n");
      lOld=l;
    }
    printf(" %i/%.1f",c,event->getTimeValue(sec,mod,lay,c));
  }
  printf("\n");
  Int_t first,last;
  fClst->getIndexRegChilds(first,last);
  if(first<0) return;
  HMdcClus* fClst2 = (HMdcClus*)fClusCat->getObject(first);
  if(fClst2==0) return;
  l=c=-1;
  lOld=l;
  while(fClst2->getNextCell(l,c)) {
    if(l != lOld) {
      lay=l%6;
      mod=l%6 + seg*2;
      printf("%i mod. %i layer:",mod,lay);
      if(lOld>=0) printf("\n");
      lOld=l;
    }
    printf(" %i/%.1f",c,event->getTimeValue(sec,mod,lay,c));
  }
  printf("\n");
}

ClassImp(HMdcAlignerD)
