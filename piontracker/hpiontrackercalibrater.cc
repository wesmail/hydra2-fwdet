//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
//  HPionTrackerCalibrater:
//
//  Calibrates all fired cells in PionTrackerRaw category and fills
//  the PionTrackerCal category
//
////////////////////////////////////////////////////////////////

#include "hpiontrackercalibrater.h"
#include "hpiontrackerdef.h"
#include "hpiontrackerraw.h"
#include "hpiontrackercal.h"
#include "hpiontrackerdetector.h"
#include "hpiontrackercalpar.h"
#include "hpiontrackerhitfpar.h"
#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hspectrometer.h"

#include <iomanip>
#include <iostream>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define PRh(x) std::cout << "++DEBUG: " << #x << " = hex |" << std::hex << x << std::dec << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

/*ClassImp(HPionTrackerCalibrater)
HPionTrackerCalibrater::HPionTrackerCalibrater(void)
{
	// default constructor
	pRawCat = NULL;
	pCalCat = NULL;
	iter   = NULL;
	pCalpar = NULL;
}

HPionTrackerCalibrater::HPionTrackerCalibrater(const Text_t *name, const Text_t *title) : HReconstructor(name, title)
{
	// constructor
	pRawCat = NULL;
	pCalCat = NULL;
	iter   = NULL;
	pCalpar = NULL;
}*/

// raw category, cal category, iterator, cal parameter, run_id
ClassImp(HPionTrackerCalibrater)
HPionTrackerCalibrater::HPionTrackerCalibrater(void) :
	pRawCat(NULL), pCalCat(NULL), iter(NULL), pCalpar(NULL), pRunpar(NULL)
{
	// default constructor
	correction.clear();
	memset(corrs, 0, 4 * sizeof(Double_t));
}

HPionTrackerCalibrater::HPionTrackerCalibrater(const Text_t *name, const Text_t *title) : HReconstructor(name, title),
	pRawCat(NULL), pCalCat(NULL), iter(NULL), pCalpar(NULL), pRunpar(NULL)
{
	// constructor
	correction.clear();
	memset(corrs, 0, 4 * sizeof(Double_t));
}

HPionTrackerCalibrater::~HPionTrackerCalibrater(void)
{
	//destructor deletes the iterator on the raw category
	if (NULL != iter)
	{
		delete iter;
		iter = NULL;
	}
}

Bool_t HPionTrackerCalibrater::init(void)
{
	// gets the calibration parameter container
        // gets the run_id (for calibration)
       	// gets the PionTrackerRaw category
        // creates the PionTrackerCal category
	// creates an iterator which loops over all fired cells in PionTrackerRaw
	HPionTrackerDetector* det = (HPionTrackerDetector*)gHades->getSetup()->getDetector("PionTracker");
	if (!det)
	{
		Error("init", "No PionTracker found.");
		return kFALSE;
	}

	pCalpar = (HPionTrackerCalPar*)gHades->getRuntimeDb()->getContainer("PionTrackerCalPar");
	if (!pCalpar) return kFALSE;

	pHitfpar = (HPionTrackerHitFPar*)gHades->getRuntimeDb()->getContainer("PionTrackerHitFPar");
	if (!pHitfpar) return kFALSE;

       	pRunpar = (HPionTrackerCalRunPar*)gHades->getRuntimeDb()->getContainer("PionTrackerCalRunPar");
	if (!pRunpar) return kFALSE;

	pRawCat = gHades->getCurrentEvent()->getCategory(catPionTrackerRaw);
	if (!pRawCat)
	{
		Error("init()", "HPionTrackerRaw category not available!");
		return kFALSE;
	}

	pCalCat = det->buildCategory(catPionTrackerCal);
	if (!pCalCat) return kFALSE;

	iter = (HIterator*)pRawCat->MakeIterator();
	loc.set(2, 0, 0);
	fActive = kTRUE;

	return kTRUE;
}

Bool_t HPionTrackerCalibrater::reinit(void)
{
	// gets the calibration parameter container
//	if(doCorrection)
	{
		memset(corrs,0, 4*sizeof(Double_t));

		if (!pRunpar) return kFALSE;

		Int_t runID = gHades->getRuntimeDb()->getCurrentRun()->getRunId();
		correction.clear();
		pRunpar->getRun(runID, &correction);
		for (Int_t i=0 ; i < correction.getNVals() ; ++i)
		{
			corrs[i] = correction.getVal(i);
		}
	}
	return kTRUE;
}

Int_t HPionTrackerCalibrater::execute(void)
{ 
	// calibrates all fired cells [strip,module]
	HPionTrackerRaw *pRaw = NULL;
	HPionTrackerCal *pCal = NULL;

	Int_t mod	= 0;
	Int_t strip	= 0;
	Int_t nhits	= 0;

	Float_t rawTime		= 0.F;
	Float_t rawCharge	= 0.F;
	Float_t calTime		= 0.F;
	Float_t calCharge	= 0.F;
	Char_t pileup		= 0;
	Char_t overflow		= 0;

	Float_t parCellData[4] = {1.F, 0.F, 1.F, 0.F}; //cal array: cal paramters

	//	cout << "New event\n";
	//Fill cal category
	iter->Reset();
	while ((pRaw = (HPionTrackerRaw *)iter->Next()) != 0) //interation over fired cells
	{
	  //cout << "*****************\n";
		pRaw->getAddress(mod, strip);
		loc[0] = mod;
		loc[1] = strip;
		//cout << "Mod: " << mod << endl;
		if (mod >= 0)
		{
			pCal = (HPionTrackerCal*)pCalCat->getObject(loc);
			if (!pCal) // should be no object under location loc yet
			{
				pCal = (HPionTrackerCal *)pCalCat->getSlot(loc);
				if (pCal) // there should be space reserved for the slot
				{
					pCal = new(pCal) HPionTrackerCal;
					pCal->setAddress(loc[0], loc[1]);
				}
				else
				{
					Error("execute()", "Can't get slot mod=%i, strip=%i", mod, strip);
					return -1;
				}
			}
			else
			{
				Error("execute()", "Slot already exists for mod=%i, strip=%i", mod, strip);
				return -1;
				
			}
	    
			// get the calibration parameters for module, strip
			// from parameters
			HPionTrackerCalParCell &pPar = (*pCalpar)[mod][strip];
			pPar.getData(parCellData);

			// get hitf parameter for adc offset due to temperature drift if pt file has some timeoffset
			charge_offset[mod] = pHitfpar->getClusterThresh(mod);
			// get hitf parameter to switch of the usage of the ADC value
			calib_charge[mod] =  pHitfpar->getClusterDistX(mod);
			

			// loop over number of hits, still in while, mod & strip already selected
			nhits = pRaw->getMultiplicity();
			for (Int_t i = 0; i < nhits; ++i) 
			{
				// get data from raw category
				pRaw->getTimeAndCharge(i, rawTime, rawCharge);
				pRaw->getErrors(i, pileup, overflow);

				// correct the time and width
				calTime   = parCellData[0] * rawTime + parCellData[1];

				//hack for invalid adc data used by Aug14 data
				// this version no adc data used for mod0 and mod1 in Aug14
				//	if (rawCharge == 0 or corrs[mod] == 1659 or corrs[mod] == 1491) 
				if (calib_charge[mod] == 0)
				  {
				    calCharge = HPionTrackerCal::InvalidAdc();}
				else
				  {
				    calCharge = -parCellData[2] * rawCharge + parCellData[3] - corrs[mod] - charge_offset[mod];

				  }
				//calCharge = -parCellData[2] * rawCharge + parCellData[3];
// 				calCharge = (parCellData[3] - rawCharge) * parCellData[2];

				// set calibrated data
				/*cout << "calTime:" << calTime <<endl;
				cout << "calCharge: " << calCharge << endl;
				cout << corrs[mod] << endl;
				cout << strip << endl;
				cout << nhits << endl;*/
				pCal->setData(calTime, calCharge, pileup, overflow);
			}
		}
	}
	//cout << "End event\n";
	return 0;
}
