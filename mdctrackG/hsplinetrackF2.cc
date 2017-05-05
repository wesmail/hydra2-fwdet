//*-- Author : Anar Rustamov (27.08.2003)
#include <iostream>
#include "hsplinetrackF2.h"
#include "hades.h"
#include "hmetamatch2.h"
#include "hcategory.h"
#include "hdetector.h"
#include "hevent.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hsplinepar.h"
#include "hiterator.h"
#include "hgeomvolume.h"
#include "hgeomcompositevolume.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hmdcgeompar.h"
#include "hspecgeompar.h" 
#include "tofdef.h"
#include "hmdcseg.h"
#include "rpcdef.h"
#include "showerdef.h"
#include "emcdef.h"
#include "hmdctrackgcorrpar.h"
#include "hmdctrackgfieldpar.h"
#include "hmatrixcategory.h"
#include "hgeomtransform.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "hmdctrkcand.h"
#include "hmdctrackgspline.h"
#include "hgeomvector.h"
#include "hlocation.h"
#include "hsplinetrack.h"
#include "hmagnetpar.h"
#include "hshowerhitsim.h"
#include "hemcclustersim.h"
#include "hmdcsizescells.h"
#include "heventheader.h"
#include "hmdcgetcontainers.h"
#include "hrpccluster.h"

using namespace std;
  
ClassImp(HSplineTrackF2)
  
  HSplineTrackF2::HSplineTrackF2()
{
  setDef();
}

HSplineTrackF2::HSplineTrackF2(const Text_t *name,const Text_t *title):
  HReconstructor(name,title)
{
  setDef();
}

void HSplineTrackF2::setDef(void) {
  C = 299792458.;
  fCatSplineTrack = NULL;
  fCatMdcSegSim   = NULL;
  fMdcGeometry    = NULL;
  fSpecGeomPar    = NULL;
  Spline          = NULL;
  fMetaMatchIter  = NULL;
  fCatMdcTrkCand  = NULL;
  fCatTof         = NULL;
  fCatTofCluster  = NULL;
  fCatRpcCluster  = NULL;
  fCatShowerHit   = NULL;
  fCatEmcCluster  = NULL;
  field           = NULL;
  corr            = NULL;
  qSpline         = -1.f;
  fScal           =  0.f;
  qIOMatching     = -1.f;
  isSplinePar     = kFALSE;
  for(Int_t i = 0; i < 3; ++i)
    {
      pTofHit[0]  = NULL;
    }
  segments[0]     = NULL;
  segments[1]     = NULL;
  pRpc            = NULL;
}

void HSplineTrackF2::makeSplinePar()
{ 
  isSplinePar = kTRUE;
}

HSplineTrackF2::~HSplineTrackF2()
{
  if(fMetaMatchIter) delete fMetaMatchIter;
  HMdcSizesCells::deleteCont();
  //HMdcGetContainers::deleteCont();
}
Bool_t HSplineTrackF2::init()
{
  if (gHades)
    {
      for(Int_t i = 0; i < 6; i++)
	{
	  tRans[i] = 0;
	}
      
      HRuntimeDb *rtdb = gHades -> getRuntimeDb();
      HSpectrometer *spec = gHades -> getSetup();
      HEvent *event = gHades -> getCurrentEvent();
      if(rtdb && spec && event)
	{
	  field = (HMdcTrackGFieldPar*)(rtdb->getContainer("MdcTrackGFieldPar"));
	  corr = (HMdcTrackGCorrPar*)(rtdb->getContainer("MdcTrackGCorrPar"));
	  fSpecGeomPar = (HSpecGeomPar*)(rtdb->getContainer("SpecGeomPar"));
	  pMagnet = (HMagnetPar*)(rtdb->getContainer("MagnetPar"));
	  fGetCont = HMdcGetContainers::getObject();
	  pSizesCells = HMdcSizesCells::getObject();
	  fGetCont -> getMdcGeomPar();
	  fGetCont -> getSpecGeomPar();
	} 
      
      fCatMdcTrkCand = event -> getCategory(catMdcTrkCand);
      if(!fCatMdcTrkCand) return kFALSE;
      fCatMetaMatch = event -> getCategory(catMetaMatch);
      if (!fCatMetaMatch) return kFALSE;
      fMetaMatchIter = (HIterator*)fCatMetaMatch->MakeIterator();
      if(!fMetaMatchIter) return kFALSE;
      fCatMdcSegSim = event -> getCategory(catMdcSeg);
      fCatTof = event -> getCategory(catTofHit);
      if(!fCatTof) Warning("init", "NO catTofHit in input!");;
      fCatTofCluster = event -> getCategory(catTofCluster);
      if(!fCatTofCluster) Warning("init","No catTofClustter in input!");
      fCatRpcCluster = event -> getCategory(catRpcCluster);
      if(!fCatRpcCluster) Warning("init", "NO catRpcCluster in input!");;
      
      
      fCatShowerHit  = event -> getCategory(catShowerHit);
      fCatEmcCluster = event->getCategory(catEmcCluster);
      if(fCatShowerHit == NULL && fCatEmcCluster == NULL) {
	  Warning("init", "NO catShowerHit and fCatEmcCluster in input!");
      }
      
      fCatSplineTrack = event -> getCategory(catSplineTrack);
      if(!fCatSplineTrack)
	{
	  Int_t size[2] = {6,8000};
	  fCatSplineTrack = new HMatrixCategory("HSplineTrack",2,size,0.5);  
	  if(fCatSplineTrack)
	    { 
	    event -> addCategory(catSplineTrack,fCatSplineTrack,"SplineTrack");
	    }	
	}
      
      fCatSplinePar = event -> getCategory(catSplinePar);
      if(!fCatSplinePar && isSplinePar)
	{
	  Int_t size[2] = {6,8000};
	  fCatSplinePar = new HMatrixCategory("HSplinePar",2,size,0.5);  
	  if(fCatSplinePar)
	    {
	    event -> addCategory(catSplinePar,fCatSplinePar,"SplinePar");
	    }	
	}    
	    
	    
    }
  return kTRUE;
}
Bool_t HSplineTrackF2::reinit()
{
  Spline=corr -> getSPline();
  if(!Spline -> splineIsInitialized())
    {
      Error("reinit()","HMetaMatch2 is not initialzed! Run HMetaMatchF2 task in front of HSplineTrackF2 !!!");
      return kFALSE;
    }
  //if(!Spline -> splineKickIsInitialized())
//    {
//      Error("reinit()","HMetaMatch2 is not initialzed! Run HMetaMatchF2 task in front of HSplineTrackF2 !!!");
//      return kFALSE;
//    }
  
  HMdcTrackGCorrections *corrScan[3];
  corr -> getCorrScan(corrScan);
  evHeader = gHades -> getCurrentEvent() -> getHeader();
  for(Int_t i=0; i<6; i++)
    {
      tRans[i]=0;
    }
  Spline -> setDataPointer(field -> getPointer(),corr -> getCorr1());
  Spline -> setCorrScan(corrScan);
  if(pMagnet -> getPolarity() >= 0)
    {
      Spline -> setMagnetScaling(pMagnet -> getScalingFactor());
      fScal = pMagnet -> getScalingFactor();
    }
  else 
  {
     Spline -> setMagnetScaling(pMagnet -> getScalingFactor()*(-1.));
     fScal = pMagnet -> getScalingFactor()*(-1.);
    }
  return kTRUE;
}

Bool_t HSplineTrackF2::finalize()
{
  return kTRUE;
}

Int_t HSplineTrackF2::execute()
{
 fMetaMatchIter -> Reset();
  while((pMetaMatch = (HMetaMatch2*)(fMetaMatchIter -> Next()))!=0)
    {
      if( !doMomentum(pMetaMatch) ) continue;
         fillData(segments[0], kFALSE);
    }
  return 0;
}


Bool_t HSplineTrackF2::doMomentum(HMetaMatch2 *pMetaMatch)
{ //Impuls hesblamasini bashla()

  sector = pMetaMatch -> getSector();
  sectorloc.set(1,(Int_t)sector);
  indTrkCand = pMetaMatch -> getTrkCandInd();
  pMdcTrkCand=(HMdcTrkCand*)fCatMdcTrkCand->getObject(indTrkCand);
  if(!pMdcTrkCand) return kFALSE;
  index1 = pMdcTrkCand -> getSeg1Ind();
  index2 = pMdcTrkCand -> getSeg2Ind();
  if(index1 < 0 || index2 < 0) return kFALSE;
   segments[0] = (HMdcSeg*)fCatMdcSegSim -> getObject(index1);
   segments[1] = (HMdcSeg*)fCatMdcSegSim -> getObject(index2);
   
   if(tRans[(Int_t)sector] == 0)
     {
       tRans[(Int_t)sector] = new HGeomTransform();
       if(!fGetCont -> getLabTransSec(*(tRans[(Int_t)sector]),sector))
	 {
	   return kFALSE;
	 }
     }
      
   HVertex &vertex = gHades->getCurrentEvent() -> getHeader() -> getVertex();            
   tarDist = Spline -> calcTarDist(vertex,segments[0],tRans[(Int_t)sector]);
   
   system = pMetaMatch -> getSystem();
   if(!segments[0] || !segments[1]) {qIOMatching = -1.; return kFALSE;}
   qIOMatching=Spline->calcIOMatching(segments);
   calcMomentum(segments);  //not this: calcMomentum(segments,pMetaMatch);     
   polarity = Spline -> getPolarity() ;
   
   return kTRUE;
}

Bool_t HSplineTrackF2::doMassStuff(HMetaMatch2 *pMetaMatch)
{ 
  Bool_t isMatched = kFALSE; 
  if( pMetaMatch -> getSystem() == -1) return kFALSE;  
  
  if( pMetaMatch -> getNRpcClusters() )
    { 
      isMatched = kTRUE; 
      doMassStuff2("rpc", pMetaMatch);
    }
  if( fCatShowerHit!=NULL &&  pMetaMatch -> getNShrHits() )
    {
      isMatched = kTRUE;
      doMassStuff2("shower", pMetaMatch);
    }
  else if( fCatEmcCluster!=NULL && pMetaMatch -> getNEmcClusters() )
    {
      isMatched = kTRUE;
      doMassStuff2("emc", pMetaMatch);
    }
  if ( pMetaMatch -> getNTofHits() )
    {
      isMatched = kTRUE;
      doMassStuff2("tof", pMetaMatch);
    }
  if(!isMatched) return kFALSE;
  return kTRUE;
}

void  HSplineTrackF2::doMassStuff2(TString opt, HMetaMatch2 *pMetaMatch )
{
  if( opt.Contains("rpc") )
    {
      for(UChar_t n = 0; n < pMetaMatch -> getNRpcClusters(); ++n )
	{
	  indRpc = pMetaMatch -> getRpcClstInd(n);
	  if( indRpc < 0 ) 
	    {
	      Warning("Spline","Rpc index is < 0, DISASTER!");
	      continue;
	    }
	  
	  pRpc = (HRpcCluster*)fCatRpcCluster -> getObject(indRpc);
	  if(!pRpc) 
	    { 
	      Warning("Spline","Pointer to Rpc is NULL, DISASTER!");
	    }

	  calcBeta(pRpc -> getTof(), pRpc -> getXSec(), pRpc -> getYSec(), pRpc -> getZSec());
	  fillData(segments[0]);    
	}
    }

  if( opt.Contains("tof") )
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
		      Warning("Spline","Pointer to Tof is NULL, DISASTER!");
		      continue;
		    }

		  pTofHit[i] -> getXYZLab(xTof, yTof, zTof);
                  pointMeta.setXYZ(xTof,yTof,zTof);
		  pointMeta = tRans[(Int_t)sector]->transTo(pointMeta);
		  calcBeta(pTofHit[i] -> getTof(), pointMeta.getX(), pointMeta.getY(), pointMeta.getZ());  
		  fillData(segments[0]);
		
	    } 
	}
    }
    
    
    
  
  if( opt.Contains("shower"))
    {
      
    
      for(UChar_t n = 0; n < pMetaMatch -> getNShrHits(); ++n )
	{
	  indShower = pMetaMatch -> getShowerHitInd(n);
	  if( indShower < 0 ) 
	    { 
	      Warning("Spline","Index of shower is < 0, DISASTER!");
	      continue;
            }
	    
 	    pShowerHit = (HShowerHit*)fCatShowerHit -> getObject(indShower);
                  
            if(!pShowerHit) 
	    { 
	      Warning("Spline","Pointer to Shower is NULL, DISASTER!");
	    }        
            
	     //pShowerHit -> getLabXYZ(&xTof,&yTof,&zTof);
	
	     fillData(segments[0], kFALSE);
	}
      	
    }
    
  if( opt.Contains("emc"))
    {
      
    
      for(UChar_t n = 0; n < pMetaMatch -> getNEmcClusters(); ++n )
	{
	  indEmc = pMetaMatch->getEmcClusterInd(n);
	  if( indEmc < 0 ) 
	    { 
	      Warning("Spline","Index of emc is < 0, DISASTER!");
	      continue;
            }
	    
 	    pEmcCluster = (HEmcCluster*)fCatEmcCluster -> getObject(indEmc);
                  
            if(!pEmcCluster) 
	    { 
	      Warning("Spline","Pointer to Emc is NULL, DISASTER!");
	    }        
            
	    pEmcCluster -> getXYZLab(xTof,yTof,zTof);
            pointMeta.setXYZ(xTof,yTof,zTof);
	    pointMeta = tRans[(Int_t)sector]->transTo(pointMeta);
            calcBeta(pEmcCluster->getTime(), pointMeta.getX(), pointMeta.getY(), pointMeta.getZ());  
            fillData(segments[0]);
	}
      	
    }
    	
}

void HSplineTrackF2::calcBeta(Float_t tof, Float_t x, Float_t y, Float_t z)
{
  distanceTof = Spline -> getMetaDistance(x*0.1, y*0.1, z*0.1);
  const HGeomVector& targetPos=((*pSizesCells)[sector]).getTargetMiddlePoint();
  HGeomVector POINT1 = Spline -> getPointOne();
  POINT1 *= 10.;
  Double_t dist = (POINT1 - targetPos).length();
  TOFdistance = dist + distanceTof*10.; //in mm
  beta = TOFdistance/tof/C;
  beta *= 1e6;
  mass2 = Momentum*Momentum*(1-beta*beta)/(beta*beta);
  polarity = Spline -> getPolarity();
}
 
void HSplineTrackF2::calcMomentum(HMdcSeg *segments[])
{
  if(segments[1] -> getHitInd(1) == -1)
    { 
      //cout<<"3 chamber hali"<<endl;
      Momentum = Spline -> calcMomentum123(segments,kTRUE,segments[0] -> getZ());
      numChambers = 3;
    }
  else if(segments[1] -> getHitInd(0) == -1)
    {
      Momentum = Spline -> calcMomentum123P4(segments,kTRUE,segments[0] -> getZ());
      numChambers = 3;
    }		 
  else
    {
      //cout<<"4 chamber hali"<<endl;
      Momentum = Spline -> calcMomentum(segments,kTRUE,segments[0] -> getZ());
      numChambers = 4;
    }
  Momentum /= 0.7215/fScal;
  qSpline = Spline -> getqSpline(); 
}

void HSplineTrackF2::calcMomentum(HMdcSeg *segments[], HMetaMatch2 *pMetaMatch)
{
  calcMomentum(segments);
  doMassStuff(pMetaMatch);
}

HSplineTrack*  HSplineTrackF2::fillData(HMdcSeg *segment, Bool_t condition)
{
  Int_t indexspline;
  HSplineTrack *sp=(HSplineTrack*)fCatSplineTrack -> getNewSlot(sectorloc,&indexspline);
  if(!sp)
    {
      Error("fillData","No slot available");
      return 0;
    }
  sp = (HSplineTrack*)(new(sp)HSplineTrack);
  if(sp)
    {        

      sp -> setP(Momentum,0.);
      sp -> setZ(segment->getZ(),segment->getErrZ());
      sp -> setR(segment->getR(),segment->getErrR());
      sp -> setTheta(segment->getTheta(),segment->getErrTheta());
      sp -> setPhi(segment->getPhi(),segment->getErrPhi());
      sp -> setPolarity(polarity*pMagnet -> getPolarity());
      sp -> setSector(sector);
      pMetaMatch -> setSplineInd(indexspline);
      //         sp->setTofHitInd(pMetaMatch->getTofHitInd());
      //         sp->setShowerHitInd(pMetaMatch->getShowerHitInd());
      sp -> setNumOfChambers(numChambers);
      //         if(qSpline<5.)
      sp -> setQSpline(qSpline);
      sp -> setTarDist(tarDist);
      sp -> setIOMatching(qIOMatching);
      if(condition)
	{
	  sp -> setTofDist(TOFdistance);
	  sp -> setBeta(beta);
	  sp -> setMass2(mass2,0.);
	  sp -> setTof(tof);
	}
      
      if(isSplinePar) {fillParData();}
    }
  else
    Error("FillData","No slots free");
  return sp;
}


HSplinePar *HSplineTrackF2::fillParData()
{
   Int_t indexsplinepar;
   HSplinePar *sppar=(HSplinePar*)fCatSplinePar->
      getNewSlot(sectorloc,&indexsplinepar);
   if(!sppar)
      {
         Error("fillData","No slot available");
	 return 0;
      }
   sppar=(HSplinePar*)(new(sppar)HSplinePar);
   if(sppar)
      {    
         Spline -> getXYpoint(xPoints,yPoints,zPoints);
	 sppar  -> setSplinePoints(xPoints,yPoints,zPoints);
      }
   return sppar;
}






