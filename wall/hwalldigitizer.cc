using namespace std;
#include "TRandom.h"
#include <time.h>
#include "hwalldigitizer.h"
#include "walldef.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hwalldetector.h"
#include "hwalldigipar.h"
#include "hwallgeompar.h"
#include "hgeantwall.h"
#include "hwallrawsim.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hwallrawsimfilter.h"
#include <iostream> 
#include <iomanip>

//*-- Author : D.Vasiliev
//*-- Modified: 07/04/2005 F.Krizek
//*-- Modified: 27/10/2002 D.Zovinec
//*-- Modified: 13/02/2002 by D.Zovinec
//*-- Modified: 30/11/2000 by R.Holzmann
//*-- Modified: 16/12/99 by J.Bielcik
//*-- Modified: 9/12/99 by V.Charriere
//*-- Modified: 8/11/99 by R.Holzmann
//*-- Modified: 24/10/99 by D.Vasiliev
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////////////
//
//  HTofDigitizer digitizes data, puts output values into
//  raw data category
//
//  Need to implement quenching?
//
/////////////////////////////////////////////////////////////////////

void HWallDigitizer::initParContainer() {

    pWallDigiPar=(HWallDigiPar *)gHades->getRuntimeDb()->getContainer("WallDigiPar");
}

HWallDigitizer::HWallDigitizer(void) {

    fGeantCat    = 0;
    fRawCat      = 0;
    pWallDigiPar = 0;
    pWallGeomPar = 0;
    fLoc.set(1,0);
    iterGeant    = 0;
    iterWallRaw  = 0;
}

HWallDigitizer::HWallDigitizer(const Text_t *name,const Text_t *title) :
HReconstructor(name,title) {

    fGeantCat    = 0;
    fRawCat      = 0;
    pWallDigiPar = 0;
    pWallGeomPar = 0;
    fLoc.set(1,0);
    iterGeant    = 0;
    iterWallRaw  = 0;
}

HWallDigitizer::~HWallDigitizer(void) {
}

Bool_t HWallDigitizer::init(void) {

    time_t curtime;

    fGeantCat = gHades->getCurrentEvent()->getCategory(catWallGeantRaw);
    if (!fGeantCat) {
        Warning("init()","No category catWallGeantRaw found!") ;
    } else iterGeant   = (HIterator *)fGeantCat->MakeIterator("native");

    fRawCat = gHades->getCurrentEvent()->getCategory(catWallRaw);
    if (!fRawCat) {
	HWallDetector* wall=(HWallDetector*)(gHades->getSetup()->getDetector("Wall"));
	if(wall){
	    fRawCat=wall->buildMatrixCategory("HWallRawSim",0.5F);
	    if (fRawCat) gHades->getCurrentEvent()->addCategory(catWallRaw,fRawCat,"Wall");
	}
    }

    
    if(fRawCat) iterWallRaw = (HIterator *)fRawCat  ->MakeIterator("native");
    else        Warning("init()","No category catWallRaw !") ;


    if(fGeantCat&&fRawCat) initParContainer();


    time(&curtime);

    return kTRUE;
}

Int_t HWallDigitizer::execute(void) {

    if(!iterGeant || !iterWallRaw) return 0;

    const Float_t quantEff = 0.24;       // PMT quantum efficiency.
    const Float_t photYield = 11100.0;   // photon yield in scintillator (phot/MeV).
    const Float_t timeResWall[3] = {0.42, 0.52, 0.67};  //values based on apr07 analysis
    //{0.35, 0.42, 0.60};  // time resolution in the centres of the
    //                        small(40x40mm), middle (80x80mm) and
    //                        large (160x160mm) cells [ns].
    const Float_t relAmplResol   = 0.08; // sigma of Gaus distribution.
    const Float_t minEnerRelease = 1.8;  // minimum energy release (MeV/cm)

    HGeantWall* geant = 0;
    HWallRawSim* raw  = 0;
    HWallRawSimFilter fRawFilter;

    TString cellName,str;
    Float_t slopeTDC;
    Int_t thrCFD, thrADC;
    Int_t cell = -1;
    fLoc.set(1,-1);

    Int_t   numTrack, numTrack1 = -1, numTrack2 = -1;      //track numbers
    Float_t trackLen;
    Float_t time, charge;
    Int_t timeCh, chargeCh;
    Float_t prevTime, prevCharge;
    Float_t timeResol, amplResol, chargeRef;


    Float_t geaTof  = 0.;
    Float_t geaTof1[383];
    Float_t geaTof2[383];
    Float_t geaEner = 0.;
    Float_t geaX    = 0.;
    Float_t geaY    = 0.;     // used by Tofino only
    Float_t geaMom  = 0.;

    iterGeant->Reset();   // this works only in split mode=2
    // (for 0 and 1 the iterator must be recreated)

    while ((geant = (HGeantWall *)iterGeant->Next()) != 0) {

	fLoc[0] = geant->getCell();

	raw = (HWallRawSim*) fRawCat->getObject(fLoc);   // test if cell in use
	if(raw) {
	    raw->incNHit();  // increment counter for additional hits
	    numTrack1  = raw->getNTrack1();
	    numTrack2  = raw->getNTrack2();
	    prevTime   = raw->getTime();
	    prevCharge = raw->getCharge();
	}
	else {
	    prevTime   = 100000.;
	    prevCharge = 0.;
	    raw = (HWallRawSim*) fRawCat->getNewSlot(fLoc);  // get new slot
	    if(!raw) continue;
	    raw = new(raw) HWallRawSim;
	    raw->setNHit(1);
	}

	cell = fLoc[0];
	slopeTDC = pWallDigiPar->getTDC_Slope(cell);

	geant->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);

	numTrack = geant->getTrack();
	cellName = pWallGeomPar->getCellName(fLoc[0]);

	str=cellName.Replace(0,3,"",0);
	str.Replace(1,3,"",0);

	time = geaTof;

	timeResol = timeResWall[atoi(str)-1];
	time      = gRandom->Gaus(time,timeResol);


	timeCh = (Int_t) (time/slopeTDC);
	if (timeCh < 0)    timeCh = 0;
	if (timeCh > 4095) timeCh = 4095;
	time = ( Float_t ) (timeCh);

	if(raw->getNHit() > 1){
	    if(geaTof < geaTof1[fLoc[0]]){
		numTrack2 = numTrack1;
		numTrack1 = numTrack;
		geaTof2[fLoc[0]] = geaTof1[fLoc[0]];
		geaTof1[fLoc[0]] = geaTof;
	    } else {
		if(geaTof < geaTof2[fLoc[0]]){
		    numTrack2 = numTrack;
		    geaTof2[fLoc[0]] = geaTof;
		}
	    }
	}

	if(time > prevTime) time = prevTime;

	//charge = geaEner*photYield*quantEff*0.5*(1 - cos(ar*deg2rad))*exp(-(hl-geaX)/al);
	charge    = geaEner*photYield*quantEff;
	amplResol = charge*relAmplResol;
	charge    = gRandom->Gaus(charge,amplResol);

	chargeRef = photYield*quantEff*minEnerRelease;

	chargeCh = (Int_t) ((charge/chargeRef)*256. + prevCharge);
	if (chargeCh < 0) chargeCh = 0;
	if (chargeCh > 4095)  chargeCh = 4095;
	charge = (Float_t)chargeCh;


	raw->setTime(time);
	raw->setCharge(charge);
	raw->setCell((Int_t) fLoc[0]);
	if(raw->getNHit() > 1){
	    raw->setNTrack1(numTrack1);
	    raw->setNTrack2(numTrack2);
	} else {
	    raw->setNTrack1(numTrack);
	    raw->setNTrack2(-1);
	    geaTof1[fLoc[0]] = geaTof;
	    geaTof2[fLoc[0]] = 100000.;
	}
    }

    // Exclusion of hits with charge less than ADC threshold.
    // Time of hits with charge less than CFD threshold is set
    // to zero. These hits are excluded later in the hitfinder.

    iterWallRaw->Reset();
    while ( (raw = (HWallRawSim *)iterWallRaw->Next()) != NULL) {
	fLoc[0] = raw->getCell();
	time    = raw->getTime();
	charge  = raw->getCharge();


	// overflow suppression
	if(((Int_t)time) >= 4095) raw->setTime(0.0);

	cell=fLoc[0];

	thrCFD = (Int_t)pWallDigiPar->getCFD_Threshold(cell);
	thrADC = (Int_t)pWallDigiPar->getADC_Threshold(cell);
	// CFD and ADC thresholds
	if(((Int_t)charge) < thrCFD){
	    raw->setTime(0.0);
	    if(((Int_t)charge) < thrADC){
		raw->setCharge(0.0);
	    }
	}

    }

    fRawCat->filter(fRawFilter);

    return 0;
}

ClassImp(HWallDigitizer)
