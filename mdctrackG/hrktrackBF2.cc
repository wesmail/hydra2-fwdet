//*-- Author : A.Sadovsky <sadovski@fz-rossendorf.de> (04.11.2004); Special thanks to A.Ivashkin, V.Pechenov and A.Rustamov
//*-- Last modified : 10/08/2005 by Ilse Koenig
//*-- Last modified : 22/01/2010 by A. Rustamov

#include <iostream>
#include "hrktrackBF2.h"
#include "hrktrackB.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hevent.h"
#include "hdetector.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hmdctrkcand.h"
#include "hmdcgetcontainers.h"
#include "hrungekutta.h"
#include "hmetamatch2.h"
#include "htofcluster.h"
#include "htofclustersim.h"
#include "hgeomvolume.h"
#include "hspectrometer.h"
#include "hmdcdetector.h"
#include "hmdcgeompar.h"
#include "htofgeompar.h"
#include "hshowergeometry.h"
#include "hemcgeompar.h"
#include "hspecgeompar.h"
#include "tofdef.h"
#include "hmdcseg.h"
#include "hmdchit.h"
#include "showerdef.h"
#include "emcdef.h"
#include "hmdctrackgfieldpar.h"
#include "hmatrixcategory.h"
#include "hgeomtransform.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "hmdctrackgspline.h"
#include "hgeomvector.h"
#include "hsplinetrack.h"
#include "hmagnetpar.h"
#include "hshowerhitsim.h"
#include "hemcclustersim.h"
#include "hmdcsizescells.h"
#include "hrpccluster.h"
#include "rpcdef.h"
#include "hrpcgeompar.h"
#include "hmetamatchpar.h"

using namespace std;

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////
//
// This class implements Runge Kutta method of momentum calculation
// into Hydra framework. Output is compartible with General Tracking Scheme
// User valuable output is HRKTrackB class in a ROOT tree
// Now it works with all 4 MDCs present
//
/////////////////////////////////////////////////////////////////////////////

ClassImp(HRKTrackBF2)

HRKTrackBF2::HRKTrackBF2() {
  // DUMMY constructor:
  // Initial momentum is taken as a self guess (mode=0)
  mode=0;
  clear();
}


HRKTrackBF2::HRKTrackBF2(const Text_t *name, Short_t m) : HReconstructor(name,name) {
  // Mode m = {0,1,2} Take initial momentum from: 0-self guess, 2-from SplineTrack
  mode=m;
  if(mode==1) {
    Error("HRKTrackBF2","mode=%i is not supported any more!",mode);
    mode = 2;
  }
  clear();
}

void HRKTrackBF2::clear() 
{
  fMatchPar       = NULL;
  pMagnet         = NULL;
  field           = NULL;
  fieldFactor     = -1000.;
  fSpecGeomPar    = NULL;
  fGetCont        = NULL;
  fTofGeometry    = NULL;
  fRpcGeometry    = NULL;
  fShowerGeometry = NULL;
  fEmcGeometry    = NULL;
  fCatMetaMatch   = NULL;
  fMetaMatchIter  = NULL;
  fCatMdcTrkCand  = NULL;
  fCatRpcCluster  = NULL;
  fCatMdcSeg      = NULL;
  fCatMdcHit      = NULL;
  fSplineTrack    = NULL;
  fCatKine        = NULL;
  fCatShower      = NULL;
  fCatEmc         = NULL;
  fCatTof         = NULL;
  fCatTofCluster  = NULL;
  fCatRKTrack     = NULL;
  pMetaMatch      = NULL;
  pMdcTrkCand     = NULL;
  pSplineTrack    = NULL;
  pTofCluster     = NULL;
  pShowerHit      = NULL;
  pEmcCluster     = NULL;
  pRungeKutta     = NULL;
  pMdcSeg1        = NULL;
  pMdcSeg2        = NULL;

  qualityRpc      = -1.;
  qualityShower   = -1.;
  qualityTof      = -1.;




  for(Int_t i = 0; i < 3; ++i)
    {
      pTofHit[i] = NULL;
    }

  for(Int_t m = 0;m < 4; m++)
    {
      for(Int_t s = 0;s < 6; s++)
	{
	  mdcInstalled[m][s]=kFALSE;
	}
    }
}

HRKTrackBF2::~HRKTrackBF2() {
  // destructor
  if (pRungeKutta) {
    delete pRungeKutta;
    pRungeKutta=0;
  }
  HMdcSizesCells::deleteCont();
}


Bool_t HRKTrackBF2::init(){

// sets pointers to all categories and the parameter containers
  if (gHades) {
    HRuntimeDb *rtdb=gHades->getRuntimeDb();
    HSpectrometer *spec=gHades->getSetup();
    HEvent *event=gHades->getCurrentEvent();
    if (!event) return kFALSE;

    field=(HMdcTrackGFieldPar*)(rtdb->getContainer("MdcTrackGFieldPar"));
    fSpecGeomPar=(HSpecGeomPar*)(rtdb->getContainer("SpecGeomPar"));
    pMagnet=(HMagnetPar*)( rtdb->getContainer("MagnetPar"));
    fGetCont=HMdcGetContainers::getObject();
    pMSizesCells = HMdcSizesCells::getObject();

    // TOF-geometry-for-metaEnergy loss calculation
    if (spec->getDetector("Tof")) { // has the user set up a TOF?
      fTofGeometry = (HTofGeomPar *)rtdb->getContainer("TofGeomPar");
    }
    // Shower-geometry-for-metaEnergy loss calculation
    if (spec->getDetector("Shower")) {
      fShowerGeometry = (HShowerGeometry *)rtdb->getContainer("ShowerGeometry");
    }
 
    // Emc-geometry
    if (spec->getDetector("Emc")) {
      fEmcGeometry = (HEmcGeomPar *)rtdb->getContainer("EmcGeomPar");
    }
    
    if (spec->getDetector("Rpc"))
    {
      fRpcGeometry   = (HRpcGeomPar*)rtdb->getContainer("RpcGeomPar");
    }

    fMatchPar=(HMetaMatchPar*)rtdb->getContainer("MetaMatchPar");
    
    // Categories
    fCatMetaMatch=event->getCategory(catMetaMatch);
    if (!fCatMetaMatch) return kFALSE;
    fMetaMatchIter=(HIterator*)fCatMetaMatch->MakeIterator();
    if (!fMetaMatchIter) return kFALSE;
    fCatMdcTrkCand=event->getCategory(catMdcTrkCand);
    if (!fCatMdcTrkCand) return kFALSE;
    fCatMdcSeg=event->getCategory(catMdcSeg);
    if (!fCatMdcSeg) return kFALSE;
    fCatMdcHit=event->getCategory(catMdcHit);
   // if (!fCatMdcHit) return kFALSE;

    fCatRpcCluster = event->getCategory(catRpcCluster);
    if(!fCatRpcCluster) Warning("init","NO catRpcCluster in input! \n");
       

    // momentum guess if any
    if (mode==2) { //- use SplineTrack as initial guess for momentum
      fSplineTrack=event->getCategory(catSplineTrack);
      if (!fSplineTrack) {
        Error("init",
              "Spline is used as initial momentum guess, but was not initialized before Runge-Kutta");
        return kFALSE;
      }
    }
    
    //- META detectors part
    fCatTof=event->getCategory(catTofHit);
    if (!fCatTof) Warning("init","No catTofHit in input!");;
    fCatTofCluster=event->getCategory(catTofCluster);
    if (!fCatTofCluster) Warning("init","No catTofCluster in input!");

    fCatShower = event->getCategory(catShowerHit);
    fCatEmc    = event->getCategory(catEmcCluster);
    if(fCatShower == NULL && fCatEmc == NULL) {
	Warning("init","NO catShowerHit and catEmcCluster in input!");
    }

    //- Here we get category HRKTrackB to use it as an output in "execute()"
    fCatRKTrack=event->getCategory(catRKTrackB);
    if (!fCatRKTrack) {
      Int_t size[2]={6,8000};
      fCatRKTrack=new HMatrixCategory("HRKTrackB",2,size,0.5);
      if (fCatRKTrack) event->addCategory(catRKTrackB,fCatRKTrack,"RKTrackB");
    }
    return kTRUE;
  } // end if (gHades)
  return kFALSE;
}


Bool_t HRKTrackBF2::reinit()
{ //reinit
 
#warning will be changed: Anar
  // creates The Runge Kutta objects and sets the field and MDC geometry parameters
  // calculates norm vectors on TOF  modules
 
  if (!pRungeKutta) {
    pRungeKutta = new HRungeKutta();
  }
  if (pMagnet->hasChanged() || fieldFactor ==-1000) {
    fieldFactor=pMagnet->getScalingFactor();
    pRungeKutta->setField(field->getPointer(),fieldFactor);
  }

  if (!pMSizesCells->initContainer()) {
    Error("reinit()","HMdcSizesCells is not initialzed!");
    return kFALSE;
  }
  // Geometry transformation from module to sector coord system for MDCs
  HGeomTransform gtrMod2Sec;
  for(Int_t is=0; is<6; is++) {
    HMdcSizesCellsSec& fSCSec = (*pMSizesCells)[is];
    if(&fSCSec == 0) continue; // sector is not active
    secTrans[is]=*(fSCSec.getLabTrans());
    for(Int_t im=0; im<4; im++) {
      HMdcSizesCellsMod& fSCMod=fSCSec[im];
      if (&fSCMod) {
        gtrMod2Sec = *(fSCMod.getSecTrans());
        pRungeKutta->setMdcPosition(is,im,gtrMod2Sec);
        mdcInstalled[im][is]=kTRUE;
      }
    }
  }

  //Geometry of the TOF and TOFINO/SHOWER-Hit (needed by MetaEloss calculation)
  // (normale on each module in the sector coordinate system)
  
  if ( !fMatchPar ) return kFALSE;
   
  for(Int_t is=0; is<6; is++) {
    if (fRpcGeometry) {
      HModGeomPar *module = fRpcGeometry -> getModule(is,0);
      HGeomTransform modTrans( module -> getLabTransform());
      modTrans.transTo(secTrans[is]);

      rpcSM[is] = modTrans;

      HGeomVector r0_mod(0.0, 0.0, 0.0);
      centerRpc[is] = modTrans.transFrom(r0_mod);
      HGeomVector rz_mod(0.0, 0.0, 1.0);
      normVecRpc[is] = modTrans.transFrom(rz_mod) - centerRpc[is];
            
    }
    if (fShowerGeometry) {
      HGeomTransform modTrans(fShowerGeometry->getTransform(is));
      modTrans.transTo(secTrans[is]);
      
      showerSM[is] = modTrans;
      
      HGeomVector r0_mod(0.0, 0.0, 0.);
      HGeomVector rz_mod(0.0, 0.0, 1.);
      normVecShower[is] = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
    }
    
    if (fEmcGeometry) {
      HModGeomPar *pmodgeom = fEmcGeometry->getModule(is);
      HGeomTransform modTrans(pmodgeom->getLabTransform());
      
      modTrans.transTo(secTrans[is]);
      
      emcSM[is] = modTrans;
      
      HGeomVector r0_mod(0.0, 0.0, 0.);
      HGeomVector rz_mod(0.0, 0.0, 1.);
      normVecEmc[is] = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
    }
    
    if (fTofGeometry) {
      for(Int_t im=0; im<8; im++) {
        HModGeomPar *module = fTofGeometry->getModule(is,im);
        HGeomTransform modTrans(module->getLabTransform());
        modTrans.transTo(secTrans[is]);
	
        tofSM[is][im] = modTrans;	
	
        HGeomVector r0_mod(0.0, 0.0, 0.0);
        HGeomVector rz_mod(0.0, 0.0, 1.0);
        normVecTof[is][im] = modTrans.transFrom(rz_mod) - modTrans.transFrom(r0_mod);
      }
    }
  
  setMatchingParams(is);
  }
  
  return kTRUE;
}

void HRKTrackBF2::setMatchingParams(Int_t s)
{
    if(s>5||s<0) {
	Error("setMatchingParams()","s = %i out of range!",s);
        return;
    }
  ////RPC
  sigma2MdcInRpcX[s]    = fMatchPar -> getRpcSigmaXMdc(s)*fMatchPar -> getRpcSigmaXMdc(s);
  sigma2MdcInRpcY[s]    = fMatchPar -> getRpcSigmaYMdc(s)*fMatchPar -> getRpcSigmaYMdc(s);
  sRpcX[s]              = fMatchPar -> getRpcSigmaXOffset(s); //
  sRpcY[s]              = fMatchPar -> getRpcSigmaYOffset(s);
  quality2RPCCut[s]     = fMatchPar -> getRpcQualityCut(s)*fMatchPar -> getRpcQualityCut(s);
  ////SHOWER 
  sigma2MdcInShrX[s]    = fMatchPar -> getShowerSigmaXMdc(s)*fMatchPar -> getShowerSigmaXMdc(s);
  sigma2MdcInShrY[s]    = fMatchPar -> getShowerSigmaYMdc(s)*fMatchPar -> getShowerSigmaYMdc(s);
  sShowerX[s]           = fMatchPar -> getShowerSigmaXOffset(s); //
  sShowerY[s]           = fMatchPar -> getShowerSigmaYOffset(s);
  quality2SHOWERCut[s]  = fMatchPar -> getShowerQualityCut(s)*fMatchPar -> getShowerQualityCut(s);
  ////EMC 
  sigma2MdcInEmcX[s]    = fMatchPar -> getEmcSigmaXMdc(s)*fMatchPar -> getEmcSigmaXMdc(s);
  sigma2MdcInEmcY[s]    = fMatchPar -> getEmcSigmaYMdc(s)*fMatchPar -> getEmcSigmaYMdc(s);
  sEmcX[s]              = fMatchPar -> getEmcSigmaXOffset(s); //
  sEmcY[s]              = fMatchPar -> getEmcSigmaYOffset(s);
  quality2EMCCut[s]     = fMatchPar -> getEmcQualityCut(s)*fMatchPar -> getEmcQualityCut(s);
  //TOF
  sigma2TofX[s]         = fMatchPar -> getTofSigmaX(s)*fMatchPar -> getTofSigmaX(s);
  sigma2TofY[s]         = fMatchPar -> getTofSigmaY(s)*fMatchPar -> getTofSigmaY(s);
  sTofX[s]              = fMatchPar -> getTofSigmaXOffset(s); //
  sTofY[s]              = fMatchPar -> getTofSigmaYOffset(s);
  quality2TOFCut[s]     = fMatchPar -> getTofQualityCut(s)*fMatchPar -> getTofQualityCut(s);
}


Int_t HRKTrackBF2::execute()
{ //execute
  success = kTRUE;
  fMetaMatchIter->Reset();
  
  while ((pMetaMatch=(HMetaMatch2*)(fMetaMatchIter->Next()))!=0) 
  {
    sector = pMetaMatch->getSector();
    pMdcTrkCand = (HMdcTrkCand*)fCatMdcTrkCand -> getObject(pMetaMatch->getTrkCandInd());
    if(!pMdcTrkCand) { continue; }
    pMdcSeg1 = (HMdcSeg*)fCatMdcSeg -> getObject(pMdcTrkCand->getSeg1Ind());
    pMdcSeg2 = (HMdcSeg*)fCatMdcSeg -> getObject(pMdcTrkCand->getSeg2Ind());
    if (!pMdcSeg1 || !pMdcSeg2) { continue; }
    sectorloc.set(1,sector);
    // We calculate the initial point and direction for each segment.
    // This vector is used in Runge Kutta to calculate the hit points in each MDC plane.
    Double_t u1pos[3], u1dir[3], u2pos[3], u2dir[3];
    // we have to define if a missing hit point should be artificially
    // calculate to define at least 3 points for the RK algorithm. This
    // is done to keep the efficiency high (in special if the the MDC
    // cluster finder is running in "mixed mode").
    // The additional point will be always added in the inner MDC if this
    // chamber is in the setup as the influence of the magnetic field is
    // stronger in the outer chambers. In case the inner chamber is missing
    // in the setup the outer segment will be used.
    Bool_t doFakeHit=kFALSE;
    Int_t nHitsCheck[2]={0};
    if(pMdcSeg1->getHitInd(0)!=-1) nHitsCheck[0]++;
    if(pMdcSeg1->getHitInd(1)!=-1) nHitsCheck[0]++;
    if(pMdcSeg2->getHitInd(0)!=-1) nHitsCheck[1]++;
    if(pMdcSeg2->getHitInd(1)!=-1) nHitsCheck[1]++;
    

    if(mdcInstalled[0][pMdcSeg1->getSec()] &&
       mdcInstalled[1][pMdcSeg1->getSec()] &&
       nHitsCheck[0]==1 && nHitsCheck[1]==1)
    {   // if both inner chambers are installed
	// and the sum of inner and outer MDCHits is
        // equal 2
	doFakeHit=kTRUE;
    }
    Int_t nHits=calcPosDirFromSegment(pMdcSeg1,0,&u1pos[0],&u1dir[0],doFakeHit);    // inner segment

    if(nHits==1                            &&
       mdcInstalled[2][pMdcSeg2->getSec()] &&
       mdcInstalled[3][pMdcSeg2->getSec()] &&
       nHitsCheck[0]==1 && nHitsCheck[1]==1)
    {   // We could not fix the number of hits in inner segment.
	// Now we try for the outer segment. If both outer chambers
	// are installed and the sum of inner and outer MDCHits is
        // equal 2
	doFakeHit=kTRUE;
    }
    else {
	doFakeHit=kFALSE;
    }

    nHits     +=calcPosDirFromSegment(pMdcSeg2,1,&u2pos[0],&u2dir[0],doFakeHit);  // outer segment
    if (nHits<3) {
        Warning("execute()","Less than 3 points for RK detected. This should never happen!");
	continue;
    }

    Int_t splineTrackIndex=-1;
    momentumGuess=-1000000.0; //indicates the absence of initial guess
    pRK=-1000000.0;
    qRK=0;

    if (mode==2) {
      // Initial momentum guess from SplineTrack
      splineTrackIndex = pMetaMatch->getSplineInd();
      if (splineTrackIndex>=0) { 
        pSplineTrack = (HSplineTrack*)fSplineTrack->getObject(splineTrackIndex);
        pRK = pSplineTrack -> getP();
	qRK = pSplineTrack -> getPolarity();
      } else {
        Warning("execute()","SplineTrack did not provide information for RK");
      }
    }

    if (pRK>50.) {
      momentumGuess=qRK*pRK;
    } else if (pRK!=-1000000.0) {
	momentumGuess=qRK*50.;
    }

    // Now do Runge-Kutta momentum tracking and fitting with momentum guess as start value
    if (nHits == 4) {
      success = pRungeKutta->fit4Hits(u1pos,u1dir,u2pos,u2dir,multSig,sector,momentumGuess);
    } else {
      if (momentumGuess != -1000000.0) {
        success = pRungeKutta->fit3Hits(u1pos,u1dir,u2pos,u2dir,multSig,sector,momentumGuess);
      } else {
        //Runge Kutta does not work with MDC123 in case no initial guess was specified
        Warning("execute()",
                "HRKTrackF: MDC123 mode, but user did not provide momentum guess - RK failed!");
        success = kFALSE; // this prevents from writing garbage into HRKTrackB
      }
    }
    if (!success) {      
      //fillData(pMdcSeg1,pMdcSeg2,pSplineTrack); //fill HRKTrack with the initial segment information
      fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
      pMetaMatch -> setRungeKuttaInd((Short_t)indexRK);
      continue;
    }

    chiqRK=pRungeKutta->getChi2();
    pRK=pRungeKutta->getPfit();
    //cout<<"before spline pol "<<(Int_t)pSplineTrack->getPolarity()<<" spline p "<<pSplineTrack->getP()<<" pRK "<<pRK<<" qRK "<<qRK<<endl;
    if (pRK>0) {
      qRK=+1;
    } else {
      pRK*=-1.;
      qRK=-1;
    }
    //cout<<"after  spline pol "<<(Int_t)pSplineTrack->getPolarity()<<" spline p "<<pSplineTrack->getP()<<" pRK "<<pRK<<" qRK "<<qRK<<endl;
    if (pRK>10000.) pRK=10000.0;

    // Now propogate it till vertex
    pRungeKutta->traceToVertex(pMSizesCells);
    // Now we take into account META information if any
    if ( !doMassStuff() )
      {
        //cout<<"after domass"<<endl;
        tof          = -1.0;
        trackLength  = -1.0;
        beta         = -1.0;
        mass2        = -1.0;
        metaeloss    = -1.0;
        RKxyzMETA[0] = -1.0;
        RKxyzMETA[1] = -1.0;
        RKxyzMETA[2] = -1.0;
	pRungeKutta -> traceToMETA(centerRpc[sector],normVecRpc[sector]);
	fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
        pMetaMatch -> setRungeKuttaInd((Short_t)indexRK);
      }
    
  }
  
  return 0;
}


Bool_t HRKTrackBF2::doMassStuff()
{

  Bool_t isMatched = kFALSE;
  if( pMetaMatch -> getSystem() == -1) return kFALSE;
  
  if( pMetaMatch -> getNRpcClusters() )
    {
      isMatched = kTRUE;
      matchWithRpc();
    }
  if( fShowerGeometry!=NULL && pMetaMatch -> getNShrHits() )
    {
      isMatched = kTRUE;
      matchWithShower();
    }
  else if(fEmcGeometry!=NULL && pMetaMatch -> getNEmcClusters() )
    {
      isMatched = kTRUE;
      matchWithEmc();
    }
  if ( pMetaMatch -> getNTofHits() )
    {
      isMatched = kTRUE;
      matchWithTof();
    }
  if(!isMatched) return kFALSE;
  return kTRUE;
}


void HRKTrackBF2::matchWithRpc()
{

  metaNormVec = normVecRpc[sector];
  transMetaSM = rpcSM[sector];
  
  for(UChar_t n = 0; n < pMetaMatch -> getNRpcClusters(); ++n )
    {
      indRpc = pMetaMatch -> getRpcClstInd(n);
      if( indRpc < 0 )
	{
	  Warning("matchWithRpc","Rpc index is < 0, DISASTER!");
	  continue;
	}
      pRpc = (HRpcCluster*)fCatRpcCluster -> getObject(indRpc);
      if(!pRpc)
	{
	  Warning("matchWithRpc","Pointer to Rpc is NULL, DISASTER!");
	  continue;
	}
      
      tof       =       pRpc -> getTof();
      metaeloss =       pRpc -> getCharge();  
      xTof      =       pRpc -> getXSec();
      yTof      =       pRpc -> getYSec();
      zTof      =       pRpc -> getZSec();
      zMod      =       0; // will be checked later
      
      pointMeta.setXYZ(xTof,yTof,zTof);
      
      calcBeta(zMod,0);
      HRKTrackB* pRK = fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
      pRK->setRpcClustInd(indRpc);

      pMetaMatch -> setRungeKuttaIndRpcClst(n, (Short_t)indexRK);
    }
}

void HRKTrackBF2::matchWithShower()
  {

    if(!fCatShower) return;

    metaNormVec = normVecShower[sector];
    transMetaSM = showerSM[sector];

    for(UChar_t n = 0; n < pMetaMatch -> getNShrHits(); ++n )
      {
	indShower = pMetaMatch -> getShowerHitInd(n);
	if( indShower < 0 )
	  {
	    Warning("matchWithShower","Index of shower is < 0, DISASTER!");
	    continue;
	  }
	
	pShowerHit = (HShowerHit*)fCatShower -> getObject(indShower);
	
	if(!pShowerHit)
	  {
	    Warning("matchWithShower","Pointer to Shower is NULL, DISASTER!");
	    continue;
	  }
	 
	tof          = -1.0;
        beta         = -1.0;
        mass2        = -1.0;
        metaeloss    = -1.0;
	pShowerHit -> getLabXYZ(&xTof,&yTof,&zTof);
	pointMeta.setXYZ(xTof,yTof,zTof);
	pointMeta = secTrans[sector].transTo(pointMeta);
        zMod         =  pShowerHit-> getZ();
	calcBeta(zMod,1, kFALSE);
	HRKTrackB* pRK = fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
        pRK->setShowerHitInd(indShower);
	pMetaMatch -> setRungeKuttaIndShowerHit(n,(Short_t)indexRK);
      }
  }

void HRKTrackBF2::matchWithEmc()
  {

    if(!fCatEmc) return;

    metaNormVec = normVecEmc[sector];
    transMetaSM = emcSM[sector];

    for(UChar_t n = 0; n < pMetaMatch -> getNEmcClusters(); ++n )
      {
	indEmc = pMetaMatch -> getEmcClusterInd(n);
	if( indEmc < 0 )
	  {
	    Warning("matchWithEmc","Index of Emc is < 0, DISASTER!");
	    continue;
	  }
	
	pEmcCluster = (HEmcCluster*)fCatEmc -> getObject(indEmc);
	
	if(!pEmcCluster)
	  {
	    Warning("matchWithEmc","Pointer to Emc is NULL, DISASTER!");
	    continue;
	  }
	 
	tof          = pEmcCluster->getTime();
        beta         = -1.0;
        mass2        = -1.0;
        metaeloss    = -1.0;
	pEmcCluster -> getXYZLab(xTof,yTof,zTof);
	pointMeta.setXYZ(xTof,yTof,zTof);
	pointMeta = secTrans[sector].transTo(pointMeta);
        zMod         =  0.; //pEmcCluster-> getZ();
	calcBeta(zMod,3, kTRUE);   //kTRUE); -time will be used !!!!!!!!!!!!!!?????????????????
	HRKTrackB* pRK = fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
        pRK->setShowerHitInd(indEmc);                 //setEmcClusterInd(indEmc);
	pMetaMatch -> setRungeKuttaIndEmcCluster(n,(Short_t)indexRK);
      }
  }

void HRKTrackBF2::calcBeta(Float_t zMod, Int_t mode, Bool_t option)
{ 
  pRungeKutta -> traceToMETA(pointMeta, metaNormVec);
  HGeomVector localMeta1(pRungeKutta -> getXtrackOnMETA(),
			 pRungeKutta -> getYtrackOnMETA(),
			 pRungeKutta -> getZtrackOnMETA());
  HGeomVector localMeta2(pointMeta);
  localMeta1 = transMetaSM.transTo(localMeta1);
  localMeta2 = transMetaSM.transTo(localMeta2);
  
  HGeomVector shiftz(0.,0.,zMod);
  localMeta2 -= shiftz;
 
  HGeomVector localMeta = localMeta2 - localMeta1;
  RKxyzMETA[0] = localMeta.getX();
  RKxyzMETA[1] = localMeta.getY();
  RKxyzMETA[2] = localMeta.getZ();
  
  qualityRpc = qualityShower = qualityTof = -1.;
  
  if(mode == 0) //rpc
  {
   dXrms2    =  pRpc -> getXRMS();
   dYrms2    =  pRpc -> getYRMS(); 
   dX        =  localMeta.getX() - sRpcX[sector];
   dY        =  localMeta.getY() - sRpcY[sector];
   qualityRpc = getQuality(dX,dY,dXrms2*dXrms2 + sigma2MdcInRpcX[sector],dYrms2*dYrms2 + sigma2MdcInRpcY[sector]);
  }
  
  else if (mode == 1) //shower
  {
   dXrms2  = pShowerHit -> getSigmaX();
   dYrms2  = pShowerHit -> getSigmaY();
   dX      = localMeta.getX() - sShowerX[sector];
   dY      = localMeta.getY() - sShowerY[sector];
   qualityShower  = getQuality(dX, dY,dXrms2*dXrms2 + sigma2MdcInShrX[sector],dYrms2*dYrms2 + sigma2MdcInShrY[sector]);
     
  }
  
  else if(mode == 2)             //tof
  {
   dX = localMeta.getX() - sTofX[sector];
   dY = localMeta.getY() - sTofY[sector];
   qualityTof =  getQuality(dX, dY, sigma2TofX[sector], sigma2TofY[sector]);
  }
    
  else if (mode == 3) //emc
  {
   dXrms2  = pEmcCluster -> getSigmaXMod();
   dYrms2  = pEmcCluster -> getSigmaYMod();
   dX      = localMeta.getX() - sEmcX[sector];
   dY      = localMeta.getY() - sEmcY[sector];
   qualityEmc = getQuality(dX, dY,dXrms2*dXrms2 + sigma2MdcInEmcX[sector],dYrms2*dYrms2 + sigma2MdcInEmcY[sector]);
     
  }

  else 
  {
   Warning("calcBeta", "This option does not exist");
  }
  
  
  trackLength = pRungeKutta->getTrackLength();
  if(option)
    {
      beta        = 1.0e6*trackLength/tof/TMath::C(); 
      mass2       = pRK*pRK*(1-beta*beta)/beta/beta;
    }
}

Float_t HRKTrackBF2::getQuality(Float_t dx, Float_t dy, Float_t s2x, Float_t s2y)
{

  return sqrt(dx*dx/s2x + dy*dy/s2y);
}

void HRKTrackBF2::matchWithTof()
  {
    for(UChar_t n = 0; n < pMetaMatch -> getNTofHits(); ++n )
      {
	indTof[0]  = pMetaMatch -> getTofHit1Ind(n);
	indTof[1]  = pMetaMatch -> getTofHit2Ind(n);
	indTof[2]  = pMetaMatch -> getTofClstInd(n);
	
	
	for(Int_t i = 0; i < 3; ++i )
	  {
	    if( indTof[i] < 0 ) continue;
	      
		if (i == 2)
		  pTofHit[i] = (HTofHit*)fCatTofCluster->getObject(indTof[i]);
		else
		  pTofHit[i] = (HTofHit*)fCatTof->getObject(indTof[i]);
		if( !pTofHit[i] )
		  {
		    Warning("matchWithTof","Pointer to Tof is NULL, DISASTER!");
		    continue;
		  }
		
		metaNormVec = normVecTof[sector][(Int_t)pTofHit[i] -> getModule()];
		transMetaSM = tofSM[sector][(Int_t)pTofHit[i] -> getModule()];
		
		tof = pTofHit[i] -> getTof();
		metaeloss = pTofHit[i] -> getEdep();
		pTofHit[i] -> getXYZLab(xTof, yTof, zTof);
		pointMeta.setXYZ(xTof,yTof,zTof);
		pointMeta = secTrans[sector].transTo(pointMeta);
		zMod = 0;
		calcBeta(zMod,2);
		HRKTrackB* pRK = fillData(pMdcSeg1,pMdcSeg2,pSplineTrack,indexRK);
	        if( i == 0)      { pRK->setTofHitInd  (indTof[0]); pMetaMatch -> setRungeKuttaIndTofHit1(n, (Short_t)indexRK); }
		else if( i == 1) { pRK->setTofHitInd  (indTof[1]); pMetaMatch -> setRungeKuttaIndTofHit2(n, (Short_t)indexRK); }
		else             { pRK->setTofClustInd(indTof[2]); pMetaMatch -> setRungeKuttaIndTofClst(n, (Short_t)indexRK); }
	  }
      }
    
    
  }


Int_t HRKTrackBF2::calcPosDirFromSegment(HMdcSeg* pSeg,Int_t ioseg, Double_t* pos, Double_t* dir, Bool_t flag) {
  // calculates the initial point and direction from a MDC track segment
  // If a hit index in the outer segment is -1, the error is set to -1.
  // This hit will then be discarded in Runge Kutta. For not fitted outer
  // segments, the hits get larger errors. 
  Int_t index[2]={pSeg->getHitInd(0),pSeg->getHitInd(1)};
  Int_t nHits=2;
  if (ioseg==0) {                         // inner segment
    for (Int_t i=0;i<2;i++) {
      if (index[i]==-1 && flag==kFALSE) { // missing hit
        multSig[2*i] = multSig[2*i+1] = -1.;
        nHits--;
      } else multSig[2*i] = multSig[2*i+1] = 1.;
    }
  } else {                                // outer segment
    for (Int_t i=0;i<2;i++) {
      if (index[i]==-1) {                 // missing hit
	  if (flag==kFALSE) {             // normal mode
	      multSig[4+2*i] = multSig[4+2*i+1] = -1.;
	      nHits--;
	  } else {                        // create fake hit
              if(pSeg->getChi2()>=0) multSig[4+2*i] = multSig[4+2*i+1] = 1.;
              else                   multSig[4+2*i] = multSig[4+2*i+1] = 5.;
	  }

      } else if (pSeg->getChi2()>=0) {    // fitted single hit
	  multSig[4+2*i] = multSig[4+2*i+1] = 1.;
      } else {                            // not fitted single hit
	  multSig[4+2*i] = multSig[4+2*i+1] = 5.;
      }
    }
  }
  if (nHits>0) {
    Double_t hpi=TMath::Pi()/2.;
    pos[0] = pSeg->getR()*cos(pSeg->getPhi()+hpi); // 1st-tracklet's initial point
    pos[1] = pSeg->getR()*sin(pSeg->getPhi()+hpi);
    pos[2] = pSeg->getZ();
    dir[0] = cos(pSeg->getPhi())*sin(pSeg->getTheta()); // 1st-tracklet's direction
    dir[1] = sin(pSeg->getPhi())*sin(pSeg->getTheta());
    dir[2] = cos(pSeg->getTheta());
  }
  return nHits;
}


HRKTrackB* HRKTrackBF2::fillData(HMdcSeg* segment1,HMdcSeg* segment2,HSplineTrack* spline, Int_t &indexRK) 
{
  // fills the Runge Kutta track object
  TObject* slot=0;
  HRKTrackB* rkt=0;
  //Int_t indexRK;
  slot=fCatRKTrack->getNewSlot(sectorloc,&indexRK);
  if (!slot) {
    Error("fillData","No slot available");
  } else {
      //----------------------------------------------------------------------
     // #warning locally changed isRungeKuttaAccepted. Has to be removed when meaning of isSplineAccepted is changed
      // work arround : function setRungeKuttaInd() sets
      // isAccepted=kTRUE no matter what it was set by TofClusterF.
      // So, this depends on the order of tasks
      // metamatch -> splinetask -> tofcluster -> RK
      //Bool_t isAccepted=kTRUE;
      //if(pMetaMatch->getSplineInd()>-1){
	  // spline object already exists isSplineAccepted()
	  // should be always true if there is no overlap in
	  // TOF/TOFINO and the TofClustF runs in front of RK
	//  isAccepted=pMetaMatch->isSplineAccepted();
      //}
    //  pMetaMatch->setRungeKuttaInd(indexRK); // add index in METAMATCH
      // work arround : set is accepted to old flag
      // Should work, no matter if TofClusterF runs berfore of not
      //if(isAccepted==kFALSE) pMetaMatch->unsetRungeKuttaAccept();
      //----------------------------------------------------------------------
    rkt=new(slot) HRKTrackB;
    rkt->setSector(sector);
////    rkt->setShowerHitInd(pMetaMatch->getShowerHitInd());
////    rkt->setTofHitInd(pMetaMatch->getTofHitInd());
    rkt->setZ(    segment1->getZ()     ,segment1->getErrZ()     );
    rkt->setR(    segment1->getR()     ,segment1->getErrR()     );
    rkt->setTheta(segment1->getTheta() ,segment1->getErrTheta() );
    rkt->setPhi(  segment1->getPhi()   ,segment1->getErrPhi()   );
    rkt->setP(pRK,0.);
    rkt->setPolarity(qRK);
    rkt->setTof(tof);
    rkt->setMetaEloss(metaeloss);
    rkt->setTarDist(spline->getTarDist());
    rkt->setIOMatching(spline->getIOMatching());

    if (success) {
      rkt->setChiq(chiqRK);
      rkt->setZSeg1RK(pRungeKutta->getZSeg1());
      rkt->setRSeg1RK(pRungeKutta->getRSeg1());
      rkt->setThetaSeg1RK(pRungeKutta->getThetaSeg1());
      rkt->setPhiSeg1RK(pRungeKutta->getPhiSeg1());
      rkt->setZSeg2RK(pRungeKutta->getZSeg2());
      rkt->setRSeg2RK(pRungeKutta->getRSeg2());
      rkt->setThetaSeg2RK(pRungeKutta->getThetaSeg2());
      rkt->setPhiSeg2RK(pRungeKutta->getPhiSeg2());
      rkt->setTofDist(trackLength);
      rkt->setBeta(beta);
      rkt->setMass2(mass2, 0.);
      rkt->setMETAdx(RKxyzMETA[0]);
      rkt->setMETAdy(RKxyzMETA[1]);
      rkt->setMETAdz(RKxyzMETA[2]);
      rkt->setQualityRpc(qualityRpc);
      if(fCatShower != NULL) rkt->setQualityShower(qualityShower);
      else                   rkt->setQualityShower(qualityEmc);
      rkt->setQualityTof(qualityTof);
    } else {
      //something failed, or momentum reconstruction was not even called
      rkt->setZSeg1RK(segment1->getZ());
      rkt->setRSeg1RK(segment1->getR());
      rkt->setThetaSeg1RK(segment1->getTheta());
      rkt->setPhiSeg1RK(segment1->getPhi());
      rkt->setZSeg2RK(segment2->getZ());
      rkt->setRSeg2RK(segment2->getR());
      rkt->setThetaSeg2RK(segment2->getTheta());
      rkt->setPhiSeg2RK(segment2->getPhi());
      rkt->setTofDist(spline->getTofDist());
      rkt->setBeta(   spline->getBeta());
      rkt->setMass2(  spline->getMass2(), 0.);
    }
  }
  return rkt;
}
