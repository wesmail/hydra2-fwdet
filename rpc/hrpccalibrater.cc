//*-- Author   : Pablo Cabanelas
//*-- Created  : 24/08/07

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
//
//  HRpcCalibrater: perform global correction on the data
//
// Calibrates RpcRaw data and writes to RpcCal.
//
////////////////////////////////////////////////////////////////

using namespace std;
#include "hrpcdetector.h"
#include "hrpccalibrater.h"
#include "rpcdef.h"
#include "hrpcraw.h"
#include "hrpccal.h"
#include "hrpccalpar.h"
#include "hrpcchargeoffsetpar.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include <iostream>
#include <iomanip>
#include <cstdlib>


HRpcCalibrater::HRpcCalibrater(void)
{
  pRawCat=NULL;
  pCalCat=NULL;
  iter=NULL;
  pCalPar=NULL;
  pChOffPar=NULL;
}

HRpcCalibrater::HRpcCalibrater(const Text_t *name,const Text_t *title) :
  HReconstructor(name,title)
{
  pRawCat=NULL;
  pCalCat=NULL;
  iter=NULL;
  pCalPar=NULL;
  pChOffPar=NULL;
}

HRpcCalibrater::~HRpcCalibrater(void)
{
  if (iter) delete iter;
  iter=NULL;
}

Bool_t HRpcCalibrater::init(void)
{
  HRpcDetector *rpc;
  rpc=(HRpcDetector *)gHades->getSetup()->getDetector("Rpc");

  if(!rpc){
    Error("init","No Rpc Detector found");
    return kFALSE;
  }

  // Categories
  pRawCat=gHades->getCurrentEvent()->getCategory(catRpcRaw);
  if (!pRawCat) {
    Error("init","No RpcRaw Category");
    return kFALSE;
  }
  pCalCat=gHades->getCurrentEvent()->getCategory(catRpcCal);
  if (!pCalCat) {
    pCalCat=gHades->getSetup()->getDetector("Rpc")->buildCategory(catRpcCal);
    if (!pCalCat) return kFALSE;
    else gHades->getCurrentEvent()->addCategory(catRpcCal,pCalCat,"Rpc");
  }

  // Parameters
  pCalPar=(HRpcCalPar*)(gHades->getRuntimeDb()->getContainer("RpcCalPar"));
  if (!pCalPar){
    Error("init","No RpcCalPar Parameters");
    return kFALSE;
  }
  pChOffPar = (HRpcChargeOffsetPar*)(gHades->getRuntimeDb()->getContainer("RpcChargeOffsetPar"));
  if (!pChOffPar){
    Error("init","No RpcChargeOffsetPar Parameters");
    return kFALSE;
  }

  iter=(HIterator *)pRawCat->MakeIterator();
  loc.set(3,0,0,0);
  fActive=kTRUE;

  return kTRUE;
}

Int_t HRpcCalibrater::execute(void)
{

    HRpcCal *pCal=0;
    HRpcRaw *pRaw=0;


    // Do the calibration here and fill RpcCal category
    iter->Reset();
    while ( (pRaw=(HRpcRaw *)iter->Next()) != NULL) {

	loc[0] = pRaw->getSector();
	loc[1] = pRaw->getColumn();
	loc[2] = pRaw->getCell();

	if(loc[2]>31) continue; //FIXME: 31 (number of cells, must be hardcoded)
	//Note: In raw, also spare TRB channels are stored, but here
	//only real cells are used

	// Calibrating and sending to hitlevel only good hits
	if(pRaw->getLeftTime()>0 && pRaw->getRightTime()>0 &&
	   pRaw->getRightTot()>0 && pRaw->getLeftTot()>0) {

	    // Decide if it is embedding mode or not! if it is embedding mode the HRpcCalSim must be used insted!

	    pCal=(HRpcCal*)pCalCat->getSlot(loc);
	    if (pCal) {

		pCal=new (pCal) HRpcCal;
		if ( !pCal ) return EXIT_FAILURE;

		// Filling address (sector, column and cell) and logBit to RpcCal category
		pCal->setAddress(pRaw->getAddress());
		pCal->setLogBit(pRaw->getRightLogBit()); //FIXME: Here this is just a convention

		// Getting calibration parameters from RpcCalPar container
		Float_t rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,rightChargeTimeOff,leftChargeTimeOff,tdc2time,tot2charge,tzero=0.0;

		HRpcCalParCol* pCalParCol=pCalPar->getCol(loc[0],loc[1]);
		if(pCalParCol) {
		    HRpcCalParCell* pCalParCell=pCalParCol->getCell(loc[2]);
		    if(pCalParCell) {
			pCalParCell->getAddress(rightTimeOffset,leftTimeOffset,rightTotOffset,leftTotOffset,tdc2time,
						tot2charge,tzero);

			leftChargeTimeOff  = pChOffPar->getPar(loc[0], pRaw->getLeftTRBNum(), pRaw->getLeftMBONum(), pRaw->getLeftDBONum() );
			rightChargeTimeOff = pChOffPar->getPar(loc[0], pRaw->getRightTRBNum(), pRaw->getRightMBONum(), pRaw->getRightDBONum() );
			// Adding offsets and correcting units
			Float_t rightTime,leftTime;
			Float_t rightTot,leftTot,rightTot2,leftTot2;

			//FIXME: temporary.
			rightTime = (pRaw->getLeftTime()*tdc2time - pRaw->getRightTime()*tdc2time)/2 + rightTimeOffset;
			leftTime  = (pRaw->getLeftTime()*tdc2time + pRaw->getRightTime()*tdc2time)/2 + leftTimeOffset;

			rightTot  = (pRaw->getRightTot()*tdc2time-rightTotOffset-rightChargeTimeOff);
			leftTot   = (pRaw->getLeftTot()*tdc2time-leftTotOffset-leftChargeTimeOff);

			rightTot2 = (pRaw->getRightTotLast()*tdc2time-rightTotOffset-rightChargeTimeOff);
			leftTot2  = (pRaw->getLeftTotLast()*tdc2time-leftTotOffset-leftChargeTimeOff);

			// Setting values to the RpcCal category
			pCal->setRightTime(rightTime);
			pCal->setLeftTime(leftTime);

			pCal->setRightCharge(rightTot);
			pCal->setRightCharge2(rightTot2);
			pCal->setLeftCharge(leftTot);
			pCal->setLeftCharge2(leftTot2);
			//By default it stores the TotLast as the second charge: Charge2

		    } else { Warning("execute","Can't get CalParCell = %i!",loc[2]); }
		} else { Warning("execute","Can't get CalParCol = %i %i!",loc[0],loc[1]); }

	    } else {
		Error ("execute()", "Can't get slot in calibrater: sec %i, col %i, cell %i",loc[0],loc[1],loc[2]);
		return -1;
	    }
	}
    }
    return EXIT_SUCCESS;
}

ClassImp(HRpcCalibrater)
