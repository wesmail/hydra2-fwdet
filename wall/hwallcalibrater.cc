//*-- AUTHOR Bjoern Spruck
//*-- created : 24.03.06
// Modified for Wall by M.Golubeva 01.11.2006

using namespace std;
#include "hwalldetector.h"
#include "hwallcalibrater.h"
#include "walldef.h"
#include "hwallraw.h"
#include "hwallcal.h"
#include "hwallcalpar.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "TMath.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
//
//  HWallCalibrater: perform global calibration on the data
//
////////////////////////////////////////////////////////////////


HWallCalibrater::HWallCalibrater(void)
{
  rawCat=NULL;
  calCat=NULL;
  iter=NULL;
  loc.set(1,-1);
  pWallCalPar=NULL;
}

HWallCalibrater::HWallCalibrater(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
  rawCat=NULL;
  calCat=NULL;
  iter=NULL;
  loc.set(1,-1);
  pWallCalPar=NULL;
}

HWallCalibrater::~HWallCalibrater(void)
{
  if (iter) delete iter;
  iter=NULL;
}

Bool_t HWallCalibrater::init(void)
{
  HWallDetector *wall;
  wall=(HWallDetector *)gHades->getSetup()->getDetector("Wall");
  
  if(!wall){
    Error("init","No Wall Det. found.");
    return kFALSE;
  }
  
  // Categories
  rawCat=gHades->getCurrentEvent()->getCategory(catWallRaw);  
  if (!rawCat) return kFALSE;
  
  calCat=wall->buildCategory(catWallCal);
  if (!calCat) return kFALSE;
  else
    gHades->getCurrentEvent()->addCategory(catWallCal,calCat,"Wall");
  
  // Parameters
  pWallCalPar=(HWallCalPar*)gHades->getRuntimeDb()->getContainer("WallCalPar");
  if (!pWallCalPar) return kFALSE;
  
  iter=(HIterator *)rawCat->MakeIterator();
  if(!iter) return kFALSE;
  loc.set(1,-1);
  
  fActive=kTRUE;
  
  return kTRUE;
}

Int_t HWallCalibrater::execute(void)
{
  HWallRaw *raw=0;
  HWallCal *cal=0;
  Int_t cell=-1;
  loc.set(1,-1);
  
  Float_t rawTime, rawAdc;
  Float_t adc, time;
  
  //Fill cal category
  iter->Reset();
  while ((raw=(HWallRaw *)iter->Next())!=0) {

    Int_t m=raw->getNHits();    // cell hit multiplicty
    if(m<=0) continue;          // No Hits -> forget it

    if(m>raw->getMaxMult()) m=raw->getMaxMult();
    
    cell = raw->getCell();
    if(cell == -1) continue;

    loc[0]=cell;
    HWallCalParCell &pPar=(*pWallCalPar)[cell];
    
    cal=(HWallCal*)calCat->getSlot(loc);
    if (cal) {
      cal=new (cal) HWallCal;
      cal->setCell(cell);
      
      for(Int_t i=0; i<m; i++){
	rawTime = raw->getTime(i+1);
	rawAdc = raw->getADC(i+1);
	if(rawAdc>0){
	  Float_t walkCorr = pPar.getTDC_WalkCorr1() + pPar.getTDC_WalkCorr2()/sqrt(rawAdc);
	  time = pPar.getTDC_Slope()*rawTime - pPar.getTDC_Offset() - walkCorr;
	  adc = pPar.getADC_Slope()*rawAdc - pPar.getADC_Offset();	  
	  cal->setTimeAdc(time,adc);
	}//if(rawAdc>=0.0 && rawTime>0)	  
      }//for(Int_t i=0; i<m; i++)
      
    }//if (cal)
    
  }//while ((raw=(HWallRaw *)iter->Next())!=0)
  
  return 0;
 }

ClassImp(HWallCalibrater)
