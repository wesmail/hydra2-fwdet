//Task for Tof clustering for Spline
//*-- Author : Anar Rustamov
//This task adds a flag to HMetaMatch2
//The flag is : isTofCluster.
//+100 --means the cluster was accepted
//-100 --means the cluster was not accepted
#include "hsplinetofclF2.h"
#include "hades.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hmdcdetector.h"
#include "hspecgeompar.h"
#include "tofdef.h"
#include "hmatrixcategory.h"
#include "hmetamatch2.h"
#include "hmdctrackddef.h"
#include "hmdctrackgdef.h"
#include "hmdctrkcand.h"
#include "hlocation.h"
#include "hshowerhitsim.h"
#include "htofhit.h"
#include <iostream>
using namespace std;

ClassImp(HSplineTofClF2)
   
   HSplineTofClF2::HSplineTofClF2()
{
   iterMetaMatch1 = NULL;
   iterMetaMatch2 = NULL;
}

HSplineTofClF2::HSplineTofClF2(const Text_t name[],const Text_t title[],Int_t MODE):
   HReconstructor(name,title) {
   mode=MODE;
   iterMetaMatch1 = NULL;
   iterMetaMatch2 = NULL;
}

HSplineTofClF2::~HSplineTofClF2()
{
    if (iterMetaMatch1) {delete iterMetaMatch1; iterMetaMatch1 = NULL;}
    if (iterMetaMatch2) {delete iterMetaMatch2; iterMetaMatch2 = NULL;}
}

Bool_t HSplineTofClF2::init()
{
   if (!gHades) return kFALSE;
   HRuntimeDb *rtdb=gHades->getRuntimeDb();
   HSpectrometer *spec=gHades->getSetup();
   HEvent *event=gHades->getCurrentEvent();
   if(rtdb && spec && event)
      {
         fCatMetaMatch=event->getCategory(catMetaMatch);
         if(!fCatMetaMatch) return kFALSE;
	 iterMetaMatch1=(HIterator*)fCatMetaMatch->MakeIterator();
	 if(!iterMetaMatch1) return kFALSE;
         iterMetaMatch2=(HIterator*)fCatMetaMatch->MakeIterator();
	 if(!iterMetaMatch2) return kFALSE;
         fCatTrkCand=event->getCategory(catMdcTrkCand);
	 if(!fCatTrkCand) return kFALSE;
	 
      }
   return kTRUE;
}
Bool_t HSplineTofClF2::reinit()
{
   return kTRUE;  
}

Bool_t HSplineTofClF2::finalize()
{
   return kTRUE;
}

Int_t HSplineTofClF2::execute()
{
  
  if(mode==0)
    {
      checkShowerOverlap();
      return unsetTofHitsFromCluster();
    }
  else 
    {
   iterMetaMatch1->Reset();
   step=0;
   while((pMetaMatch1=(HMetaMatch2*)(iterMetaMatch1->Next()))!=0)
      {
//          if((Int_t)(pMetaMatch1->getTofClusterSize())==2)
// 	    {
// 	    //cout<<"tof cluster size is 2"<<endl;
// 	       checkCluster(pMetaMatch1);
// 	    }
	 step++;
      }
   
    }
    return 1;
}

void HSplineTofClF2::checkCluster(HMetaMatch2 *p1)
{
   iterMetaMatch2->Reset();
   step1=0;
   while((pMetaMatch2=(HMetaMatch2*)(iterMetaMatch2->Next()))!=0)
      {
	 if(step1==step) continue;
	 if(condMeta(pMetaMatch2,p1))
	    {
	       if(checkForSeg(p1,pMetaMatch2))
		  {
		    // p1->unsetSplineAccept();
		     //p1->unsetRungeKuttaAccept();
		     
		  }
	    }
	 step1++;
      }
}

Bool_t HSplineTofClF2::condMeta(HMetaMatch2 *p1,HMetaMatch2 *p2)
{
//???    if(p1->getTofHitInd()==p2->getTofHitInd())
//???       return kTRUE;
//???    else 
      return kFALSE;
}

Bool_t HSplineTofClF2::checkForSeg(HMetaMatch2 *p1, HMetaMatch2 *p2)
{
   HMdcTrkCand *tr1 ,*tr2;
   Int_t tr1Ind,tr2Ind;
   tr1Ind=p1->getTrkCandInd();
   tr2Ind=p2->getTrkCandInd();
   if(tr1Ind<0 || tr2Ind<0) return kFALSE;	
   tr1=(HMdcTrkCand*)(fCatTrkCand->getObject(tr1Ind));
   tr2=(HMdcTrkCand*)(fCatTrkCand->getObject(tr2Ind));
   if(!tr1 || !tr2) return kFALSE;
   ind1=tr1->getSeg1Ind();
   ind2=tr1->getSeg2Ind();
   ind3=tr2->getSeg1Ind();
   ind4=tr2->getSeg2Ind();
   if(ind2<0 || ind4<0) return kFALSE;
   if(tr1Ind!=tr2Ind && ind1!=ind3) return kTRUE;
   else return kFALSE;
}

Bool_t HSplineTofClF2::checkCandForSameMeta(HMetaMatch2 *pMetaMatch)
{
//   Int_t index=-1;
//???    while((index=pMetaMatch->getNextMMForSameMeta())>=0)
//???       {
//???       pMetaMatch=(HMetaMatch2*)(fCatMetaMatch->getObject(index));
//???       if(!pMetaMatch) return kFALSE;
//???       pMetaMatch->unsetSplineAccept();
//???       pMetaMatch->unsetRungeKuttaAccept();
//???       }
   
   return kTRUE;
}
Int_t  HSplineTofClF2::unsetTofHitsFromCluster()
   
{
//   HMetaMatch2 *pMetaMatch;
//   iterMetaMatch1->Reset();
   
//   while((pMetaMatch=(HMetaMatch2*)(iterMetaMatch1->Next()))!=0)
//      {
//??? 	 if(pMetaMatch->getNumMMForSameMeta()<0) continue;
//??? 	 if(pMetaMatch->getTofClusterSize()==2 && pMetaMatch->getNextMMForTofHit()>=0)
//??? 	    {
//??? 	       if(!checkNextTofHits(pMetaMatch)) continue;
//??? 	    }	
//      }
   return 1;
}
Bool_t  HSplineTofClF2::checkNextTofHits(HMetaMatch2 *pMetaMatch)
   
{
//   Int_t index=-1;
//???    while((index=pMetaMatch->getNextMMForTofHit())>=0)
//???       {
//??? 	 pMetaMatch=(HMetaMatch2*)(fCatMetaMatch->getObject(index));
//??? 	 pMetaMatch->unsetSplineAccept();
//??? 	 pMetaMatch->unsetRungeKuttaAccept();
//??? 	 if(!checkCandForSameMeta(pMetaMatch)) continue;
//???       }
   return kTRUE;
}


void HSplineTofClF2::checkShowerOverlap()
{
//   HMetaMatch2 *pMetaMatch,*pMetaMatch1;
//   Int_t index;
//   Int_t firstCandIndex;
//   iterMetaMatch1->Reset();
//   while((pMetaMatch=(HMetaMatch2*)(iterMetaMatch1->Next()))!=0)
//      {

//???         firstCandIndex=pMetaMatch->getFirstMMForSameTrCnd();
//??? 	if(firstCandIndex==-1) continue;
//??? 		       
//??? 	 if(pMetaMatch->getNumMMForSameTrCnd()<0) continue;
//??? 
//??? 	 while((index=pMetaMatch->getNextMMForSameTrCnd())>=0)
//??? 	   {
//??? 	     if(pMetaMatch->getTofHitInd()>=0)
//??? 	       {
//??? 		 index=pMetaMatch->getNextMMForSameTrCnd();
//??? 		 if(index>=0)
//??? 		   {
//??? 		     pMetaMatch=(HMetaMatch2*)(fCatMetaMatch->getObject(index));  
//??? 		     if(pMetaMatch->getShowerHitInd()>=0)
//??? 		     {
//??? 		     pMetaMatch->unsetSplineAccept();
//??? 		     pMetaMatch->unsetRungeKuttaAccept();
//??? 		     }
//??? 		     
//??? 		   }
//??? 	       }
//??? 	     else if(pMetaMatch->getShowerHitInd()>=0)
//??? 		{
//??? 		
//??? 	          pMetaMatch1=pMetaMatch;	
//??? 		  index=pMetaMatch->getNextMMForSameTrCnd();
//??? 		  if(index>=0)
//??? 		    {
//??? 		      pMetaMatch=(HMetaMatch2*)(fCatMetaMatch->getObject(index));  
//??? 		      if(pMetaMatch->getTofHitInd()>=0)
//??? 		      {
//??? 		      pMetaMatch1->unsetSplineAccept();
//??? 		      pMetaMatch1->unsetRungeKuttaAccept();
//??? 		      }
//??? 		      
//??? 		    }
//??? 		}
//??? 	      
//??? 	   }
	 
	 
//      }
}

	 
