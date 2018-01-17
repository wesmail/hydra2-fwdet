using namespace std;
#include "TRandom.h"
#include "TDirectory.h"
#include <time.h>
#include "htofdigitizer.h"
#include "tofdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "htofdetector.h"
#include "hgeanttof.h"
#include "hgeantkine.h"
#include "hlinearcategory.h"
#include "htofrawsim.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "htofdigipar.h"
#include "htofcalpar.h"
#include "htofwalkpar.h"
#include "htofrawsimfilter.h"
#include "hparticletool.h"

#include <iostream> 
#include <iomanip>


//*-- Author : D.Vasiliev
//*-- Modified: 03/08/2006 J. Markert
//*-- Modified: 05/06/2006 P. Tlusty
//*-- Modified: 27/10/2002 D.Zovinec
//*-- Modified: 13/02/2002 by D.Zovinec
//*-- Modified: 30/11/2000 by R.Holzmann
//*-- Modified: 16/12/99 by J.Bielcik
//*-- Modified: 9/12/99 by V.Charriere
//*-- Modified: 8/11/99 by R.Holzmann
//*-- Modified: 24/10/99 by D.Vasiliev

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////
//
//  HTofDigitizer digitizes data, puts output values into
//  raw data category. The input data are read from the HGeantTof
//  category. After calculating TOF of and Charge etc the output
//  is stored in the HTofRawSim category.
//                                                                        //
//                                                                        //
//                                      ------------                      //
//                                     | HGeantTof  |                     //
//                                      ------------                      //
//                                           |                            //
//   ----------------------           -----------------                   //
//  |     HTofUnpacker     |         |  HTofDigitizer  |                  //
//  |   (embedding mode)   |      -- |                 |                  //
//  |                      |     /   ------------------                   //
//   ----------------------     /             |                           //
//              |              /       ----------------                   //
//         -------------      /       |  HTofRawSimTmp |                  //
//        | HTofRawSim  |----         | non persistent | (embedding mode, //
//         -------------               ----------------   sim data)       //
//  sim (sim mode)      \              /                                  //
//  or real (embedding)  \            /                                   //
//                       -----------------                                //
//                      |  HTofHitFSim    |                               //
//                       -----------------                                //
//                       /       |                                        //
//                      /  ----------------                               //
//                     /  | HTofHitSimTmp  | (embedding mode              //
//                    /   | non persistent |  sim data )                  //
//                   /     ----------------                               //
//                  /     /                                               //
//                 /     /                                                //
//         -------------                                                  //
//        | HTofHitSim  | sim (sim mode) or                               //
//         -------------  sim and real data                               //
//                        (embedding)                                     //
//                                                                        //
//                                                                        //
//                                                                        //
//  In the case of TRACK EMBEDDING of simulated tracks into
//  experimental data the real data are written by the HTofUnpacker into
//  HTofRawSim category. In embedding mode the digitizer will write his
//  output to HTofRawSimTmp to merge real and sim data on hit level
//  (keep calibrations constistent).
//  The embedding mode is recognized automatically by analyzing the
//  gHades->getEmbeddingMode() flag.
//            Mode ==0 means no embedding
//                 ==1 realistic embedding (first real or sim hit makes the game)
//                 ==2 keep GEANT tracks   (as 1, but GEANT track numbers will always
//                     win against real data. besides the tracknumber the output will
//                     be the same as in 1)
//
// In HTofRawSim the track number which created the left/right hit will be
// stored. For real data the number will be equal to gHades->getEmbeddingRealTrackId().
// For the suppression of secondaries produced in the TOF itself there are
// several configuration possibilities:
// They can be switched via setStoreFirstTrack(Int_t flag):
//     flag ==  0 realistic (secondaries included)
//              1 primary particle is stored
//              2 (default) the first track number entering the tof in SAME SECTOR is stored
//              3 as 2 but condition on SAME SECTOR && MOD
//              4 as 2 but SAME SECTOR && MOD && ROD
//
// To make the influence of the above selections easily visible
// in the output HTofRawSim a debug mode can be switched with
// void   setDebug(Bool_t flag). In this case the track numbers
// affected by the criteria 1-4 will have negative track numbers
// (multiples of -200). The multiplication factor gives the number
// of steps from the start point to the stored track (chain of
// secondaries). In case 1 it is always -200 if a particle has not
// been a primary one. With void setOutputFile(TString outname)
// an internal Ntuple can be written out with HGeantTof infos
// of the original hit and the changed one.
//
// For comparison of the new digitizer (after implemantation of
// embedding) and old digitizer (stored 2 fastest tracks instead
// of left/right track) the old execute function can be
// run by using void   setUseOld(Bool_t flag).
//
// The pointer to the TofDigitizer can be retrieved via
// static HTofDigitizer* getTofDigtizer(). This function
// returns NULL if the Digitizer has not been created yet!
//
/////////////////////////////////////////////////////////////////////
HTofDigitizer*  HTofDigitizer::pTofDigi=NULL;
Float_t HTofDigitizer::timeResZero = 0.240;  // time resolution in the centre of the strip [ns].

Bool_t HTofDigitizer::initParContainer() {

    fDigiPar=(HTofDigiPar *)gHades->getRuntimeDb()->getContainer("TofDigiPar");
    if(!fDigiPar){
	Error("initParContainer()","Could not retrieve HTofDigiPar !");
	return kFALSE;
    }
    fTofCalPar=(HTofCalPar *)gHades->getRuntimeDb()->getContainer("TofCalPar");
    if(!fTofCalPar){
	Error("initParContainer()","Could not retrieve HTofCalPar !");
	return kFALSE;
    }
    fTofWalkPar=(HTofWalkPar *)gHades->getRuntimeDb()->getContainer("TofWalkPar");
    if(!fTofWalkPar){
	Error("initParContainer()","Could not retrieve HTofWalkPar !");
	return kFALSE;
    }

    return kTRUE;
}


HTofDigitizer::HTofDigitizer(const Text_t *name,const Text_t *title) :
HReconstructor(name,title) {

    fGeantCat=0;
    fGeantKineCat=0;
    fRawCat=0;
    fRawCatTmp=0;
    fDigiPar=0;
    fLoc.set(3,0,0,0);
    iterGeant=0;
    iterTofRaw=0;
    iterTofRawTmp=0;

    storeFirstTrack=2;
    debug          =kFALSE;
    out            =NULL;
    outFile        =NULL;
    pTofDigi       =this;
    useOld         =kFALSE;
}

HTofDigitizer::~HTofDigitizer(void) {

    if(iterGeant)    { delete iterGeant    ; iterGeant     = 0;}
    if(iterTofRaw)   { delete iterTofRaw   ; iterTofRaw    = 0;}
    if(iterTofRawTmp){ delete iterTofRawTmp; iterTofRawTmp = 0;}
}

Bool_t HTofDigitizer::init(void) {

    time_t curtime;
    if(!initParContainer()) return kFALSE;

    fGeantCat = gHades->getCurrentEvent()->getCategory(catTofGeantRaw);
    if (!fGeantCat) {
	fGeantCat = gHades->getSetup()->getDetector("Tof")->buildCategory(catTofGeantRaw);
	if (!fGeantCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catTofGeantRaw,fGeantCat,"Tof");
    }

    fGeantKineCat=(HLinearCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);
    if(!fGeantKineCat){
	Error("HTofDigitizer::init()","No catGeantKine in input!");
	return kFALSE;
    }

    fRawCat = gHades->getCurrentEvent()->getCategory(catTofRaw);
    if (!fRawCat) {
	HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
	fRawCat=tof->buildMatrixCategory("HTofRawSim",0.5F);
	if (!fRawCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catTofRaw,fRawCat,"Tof");
    }

    fRawCatTmp = gHades->getCurrentEvent()->getCategory(catTofRawTmp);
    if (!fRawCatTmp) {
	HTofDetector* tof=(HTofDetector*)(gHades->getSetup()->getDetector("Tof"));
	fRawCatTmp=tof->buildMatrixCategory("HTofRawSimTmp",0.5F);
	if (!fRawCatTmp) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catTofRawTmp,fRawCatTmp,"Tof");
	fRawCatTmp->setPersistency(kFALSE);
    }

    iterGeant = (HIterator *)fGeantCat->MakeIterator("native");
    iterTofRaw = (HIterator *)fRawCat->MakeIterator("native");
    iterTofRawTmp = (HIterator *)fRawCatTmp->MakeIterator("native");

    time(&curtime);
    return kTRUE;
}
Int_t HTofDigitizer::executeOld(void) {

    const Float_t deg2rad = 0.017453293; // deg to rad conversion.
    const Float_t quantEff = 0.24;       // PMT quantum efficiency.
    const Float_t photYield = 11100.0;   // photon yield in scintillator (phot/MeV).
    const Float_t relAmplResol = 0.08;   // sigma of Gaus distribution.
    const Float_t minEnerRelease = 1.8;  // minimum energy release (MeV/cm)

    HGeantTof* geant = 0;
    HTofRawSim* raw = 0;
    HTofRawSimFilter fRawFilter;

    Float_t hl, al, ar, vg, slopeTDCl, slopeTDCr;
    Int_t thrCFDl, thrCFDr, thrADCl, thrADCr;

    Int_t   numTrack, numTrack1 = -1, numTrack2 = -1;      //track numbers
    Float_t trackLen;
    Float_t timeL, timeR, chargeL, chargeR;
    Int_t timeCh, chargeCh;
    Float_t prevTimeL, prevTimeR, prevChargeL, prevChargeR;
    Float_t timeResol,amplResol, chargeRef;

    Float_t geaTof = 0.;
    Float_t geaTof1[6][8][8];
    Float_t geaTof2[6][8][8];
    Float_t geaEner = 0.;
    Float_t geaX = 0.;
    Float_t geaY = 0.;     // used by Tofino only
    Float_t geaMom = 0.;

    iterGeant->Reset();   // this works only in split mode=2
    // (for 0 and 1 the iterator must be recreated)

    while ((geant=(HGeantTof *)iterGeant->Next())!=0) {

	fLoc[1] = geant->getModule();
	if (fLoc[1] > 21 || fLoc[1] < 14) continue;   // this is a Tofino, skip it!
	fLoc[1] = 21 - fLoc[1];       // Tof modules in HGeant: (0->21) = (in->out)

	fLoc[0] = geant->getSector();
	fLoc[2] = geant->getCell();
	fLoc[2] = 7 - fLoc[2];        // reverse also order of rods in Tof module


	raw = (HTofRawSim*) fRawCat->getObject(fLoc);   // test if cell in use
	if(raw) {
	    raw->incNHit();  // increment counter for additional hits
	    numTrack1 = raw->getNTrack1();
	    numTrack2 = raw->getNTrack2();
	    prevTimeL = raw->getLeftTime();
	    prevTimeR = raw->getRightTime();
	    prevChargeL = raw->getLeftCharge();
	    prevChargeR = raw->getRightCharge();
	}
	else {
	    prevTimeL = prevTimeR = 100000.;
	    prevChargeL = prevChargeR = 0.;
	    raw = (HTofRawSim*) fRawCat->getNewSlot(fLoc);  // get new slot
	    if(!raw) continue;
	    raw = new(raw) HTofRawSim;
	    raw->setNHit(1);
	}

	HTofDigiParCell& cell=(*fDigiPar)[fLoc[0]][fLoc[1]][fLoc[2]];
	hl = cell.getHalfLen();
	al = cell.getAttenLen();
	ar = cell.getAngleRef();
	vg = cell.getGroupVel();
	slopeTDCl = cell.getLeftTDCSlope();
	slopeTDCr = cell.getRightTDCSlope();
	thrCFDl = cell.getLeftCFDThreshold();
	thrCFDr = cell.getRightCFDThreshold();
	thrADCl = cell.getLeftADCThreshold();
	thrADCr = cell.getRightADCThreshold();

	geant->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);

	numTrack=geant->getTrack();

	if((vg!=0.0) && (al!=0.0))  {
	    timeL = geaTof + (hl - geaX)/vg;
	    //timeResol = timeResZero*exp((hl - geaX)/(2.*al));
	    //timeL = gRandom->Gaus(timeL,timeResol);
	    timeR = geaTof + (hl + geaX)/vg;
	    //timeResol = timeResZero*exp((hl + geaX)/(2.*al));
	    //timeR = gRandom->Gaus(timeR,timeResol);

	    timeCh = (Int_t) (timeL/slopeTDCl);
	    if (timeCh < 0) timeCh = 0;
	    if (timeCh > 4095) timeCh = 4095;
	    timeL = ( Float_t ) (timeCh) + 0.5;

	    timeCh = (Int_t) (timeR/slopeTDCr);
	    if (timeCh < 0) timeCh = 0;
	    if (timeCh > 4095) timeCh = 4095;
	    timeR = ( Float_t ) (timeCh) + 0.5;

	    if(raw->getNHit()>1){
		if(geaTof < geaTof1[fLoc[0]][fLoc[1]][fLoc[2]]){
		    numTrack2=numTrack1;
		    numTrack1=numTrack;
		    geaTof2[fLoc[0]][fLoc[1]][fLoc[2]]=geaTof1[fLoc[0]][fLoc[1]][fLoc[2]];
		    geaTof1[fLoc[0]][fLoc[1]][fLoc[2]]=geaTof;
		} else {
		    if(geaTof < geaTof2[fLoc[0]][fLoc[1]][fLoc[2]]){
			numTrack2=numTrack;
			geaTof2[fLoc[0]][fLoc[1]][fLoc[2]]=geaTof;
		    }
		}
	    }

	    if(timeL > prevTimeL) timeL = prevTimeL;
	    if(timeR > prevTimeR) timeR = prevTimeR;

	    chargeL = geaEner*photYield*quantEff*0.5*(1 - cos(ar*deg2rad))*exp(-(hl-geaX)/al);
	    amplResol = chargeL*relAmplResol;
	    chargeL = gRandom->Gaus(chargeL,amplResol);
	    chargeR = geaEner*photYield*quantEff*0.5*(1 - cos(ar*deg2rad))*exp(-(hl+geaX)/al);
	    amplResol = chargeR*relAmplResol;
	    chargeR = gRandom->Gaus(chargeR,amplResol);

	    chargeRef = photYield*quantEff*0.5*(1-cos(ar*deg2rad))*minEnerRelease*exp(-hl/al);
	    if(fLoc[1] <= 3) chargeRef *= 3.;
	    if(fLoc[1]>3 && fLoc[1]<=7) chargeRef *= 2.;

	    chargeCh = (Int_t) ((chargeL/chargeRef)*256. + prevChargeL);
	    if (chargeCh < 0) chargeCh = 0;
	    if (chargeCh > MAXCHRGCH)  chargeCh = MAXCHRGCH;
	    chargeL = (Float_t)chargeCh;

	    chargeCh = (Int_t) ((chargeR/chargeRef)*256. + prevChargeR);
	    if (chargeCh < 0) chargeCh = 0;
	    if (chargeCh > MAXCHRGCH)  chargeCh = MAXCHRGCH;
	    chargeR = (Float_t)chargeCh;

	} else {

	    timeL = 4095.;
	    timeR = 4095.;
	    chargeL = MAXCHRGCH;
	    chargeR = MAXCHRGCH;
	}
	raw->setLeftTime(timeL);
	raw->setRightTime(timeR);
	raw->setLeftCharge(chargeL);
	raw->setRightCharge(chargeR);
	raw->setSector((Char_t) fLoc[0]);
	raw->setModule((Char_t) fLoc[1]);
	raw->setCell((Char_t) fLoc[2]);
	if(raw->getNHit()>1){
	    raw->setNTrack1(numTrack1);
	    raw->setNTrack2(numTrack2);
	} else {
	    raw->setNTrack1(numTrack);
	    raw->setNTrack2(-1);
	    geaTof1[fLoc[0]][fLoc[1]][fLoc[2]]=geaTof;
	    geaTof2[fLoc[0]][fLoc[1]][fLoc[2]]=100000.;
	}
    }

    // Exclusion of hits with charge less than ADC threshold.
    // Time of hits with charge less than CFD threshold is set
    // to zero. These hits are excluded later in the hitfinder.

    iterTofRaw->Reset();
    while ( (raw=(HTofRawSim *)iterTofRaw->Next())!=NULL) {
	fLoc[0] = raw->getSector();
	fLoc[1] = raw->getModule();
	fLoc[2] = raw->getCell();
	timeL=raw->getLeftTime();
	timeR=raw->getRightTime();
	chargeL=raw->getLeftCharge();
	chargeR=raw->getRightCharge();

	HTofDigiParCell& cell=(*fDigiPar)[fLoc[0]][fLoc[1]][fLoc[2]];
	hl = cell.getHalfLen();
	al = cell.getAttenLen();
	thrCFDl = cell.getLeftCFDThreshold();
	thrCFDr = cell.getRightCFDThreshold();
	thrADCl = cell.getLeftADCThreshold();
	thrADCr = cell.getRightADCThreshold();
	slopeTDCl = cell.getLeftTDCSlope();
	slopeTDCr = cell.getRightTDCSlope();

	timeResol = timeResZero*exp(hl/(2.*al))/slopeTDCl;
	raw->setLeftTime(gRandom->Gaus(timeL,timeResol));
	timeResol = timeResZero*exp(hl/(2.*al))/slopeTDCr;
	raw->setRightTime(gRandom->Gaus(timeR,timeResol));

	if(((Int_t)timeL)<0) raw->setLeftTime(0.0);
	if(((Int_t)timeR)<0) raw->setRightTime(0.0);
	// overflow suppression
	if(((Int_t)timeL)>=4095) raw->setLeftTime(0.0);
	if(((Int_t)timeR)>=4095) raw->setRightTime(0.0);


	// CFD and ADC thresholds
	if(((Int_t)chargeL)<thrCFDl){
	    raw->setLeftTime(0.0);
	    if(((Int_t)chargeL)<thrADCl){
		raw->setLeftCharge(0.0);
	    }

	}
	if(((Int_t) chargeR)<thrCFDr){
	    raw->setRightTime(0.0);
	    if(((Int_t) chargeR)<thrADCr){
		raw->setRightCharge(0.0);
	    }
	}
    }

    fRawCat->filter(fRawFilter);

    return 0;
}

Int_t HTofDigitizer::execute(void) {

    if(useOld) return executeOld();

    Int_t embeddingmode=gHades->getEmbeddingMode(); // flag embedding 1=realistic 2:keep geant

    HTofRawSimFilter fRawFilter;

    //-------------------------------------------------
    // loop over raw sim category and set the default values
    // for embedding mode the real data
    // have -1 as track id -> set to standard
    // track number for embedding and increment the the
    // hit counter
    if(embeddingmode>0){
	HTofRawSim* raw = 0;
	iterTofRaw->Reset();
	while ((raw=(HTofRawSim *)iterTofRaw ->Next())!=0) {
	    raw->setNHit(1); // increment counter for additional hits
	    raw->setLeftNTrack(gHades->getEmbeddingRealTrackId());
	    raw->setRightNTrack(gHades->getEmbeddingRealTrackId());
	}
    }
    //-------------------------------------------------

    //-------------------------------------------------
    // loop over geant category and fill catTofRawTmp
    // the relsults will be copied later to the final output catTofRaw
    fillArray();
    //-------------------------------------------------

    //-------------------------------------------------
    // Exclusion of hits with charge less than ADC threshold.
    // Time of hits with charge less than CFD threshold is set
    // to zero. These hits are excluded later in the hitfinder.
    doFinalCheckOnArray();
    //-------------------------------------------------

    if(embeddingmode==0)
    {
	// in embedding mode catTofRawTmp will be used
	// from HTofHitF to merge real and embedded data
        // on hit level

	//-------------------------------------------------
	// Now copy tmp object to final output
	fillOutput();
	//-------------------------------------------------

	//-------------------------------------------------
	//
	// HTofRawSimFilter is an HFilter to reduce the number of the
	// HTofRawSim data in the catTofRaw category.
	// (If HTofRawSim object has LeftCharge==0 && RightCharge==0 &&
	// LeftTime==0 && RightTime==0 it is deleted from the category.)

	fRawCat->filter(fRawFilter);
	//-------------------------------------------------
    } else {
	fRawCatTmp->filter(fRawFilter);
    }
    return 0;
}
void HTofDigitizer::fillArray()
{
    //-------------------------------------------------
    // loop over geant category and fill catTofRawTmp
    // the relsults will be copied later to the final output catTofRaw


    const Float_t deg2rad = 0.017453293; // deg to rad conversion.
    const Float_t quantEff = 0.24;       // PMT quantum efficiency.
    const Float_t photYield = 11100.0;   // photon yield in scintillator (phot/MeV).
    const Float_t relAmplResol = 0.08;   // sigma of Gaus distribution.
    const Float_t minEnerRelease = 1.8;  // minimum energy release (MeV/cm)

    HGeantTof* geant = 0;
    HTofRawSim* raw = 0;

    Float_t hl, al, ar, vg, slopeTDCl, slopeTDCr;

    Int_t   numTrack, numTrack1 = -1, numTrack2 = -1;      //track numbers
    Float_t trackLen;
    Float_t timeL, timeR, chargeL, chargeR;
    Int_t timeCh, chargeCh;
    Float_t prevTimeL, prevTimeR, prevChargeL, prevChargeR;
    Float_t amplResol, chargeRef;

    Float_t geaTof  = 0.;
    Float_t geaEner = 0.;
    Float_t geaX    = 0.;
    Float_t geaY    = 0.;     // used by Tofino only
    Float_t geaMom  = 0.;


    iterGeant->Reset();   // this works only in split mode=2
    // (for 0 and 1 the iterator must be recreated)
    while ((geant=(HGeantTof *)iterGeant->Next())!=0) {

	fLoc[1] = geant->getModule();
	if (fLoc[1] > 21 || fLoc[1] < 14) continue;   // this is a Tofino, skip it!
	fLoc[1] = 21 - fLoc[1];       // Tof modules in HGeant: (0->21) = (in->out)

	fLoc[0] = geant->getSector();
	fLoc[2] = geant->getCell();
	fLoc[2] = 7 - fLoc[2];        // reverse also order of rods in Tof module


	//-------------------------------------------------
	// get TofRawSim object
	// if the same adress object exists already sorting has
	// to be done to find the earliest hit
	raw = (HTofRawSim*) fRawCatTmp->getObject(fLoc);   // test if cell in use
	if(raw) {
	    // if cell already exists
	    raw->incNHit();  // increment counter for additional hits
	    numTrack1 = raw->getLeftNTrack();
	    numTrack2 = raw->getRightNTrack();

	    prevTimeL = raw->getLeftTime();
	    prevTimeR = raw->getRightTime();
	    prevChargeL = raw->getLeftCharge();
	    prevChargeR = raw->getRightCharge();
	}
	else {
	    prevTimeL = prevTimeR = 100000.;
	    prevChargeL = prevChargeR = 0.;
	    raw = (HTofRawSim*) fRawCatTmp->getNewSlot(fLoc);  // get new slot
	    if(!raw) {
		Error("execute()","Could not retrieve new Slot in Category catTofRawTmp!");
		continue;
	    }
	    raw = new(raw) HTofRawSim;
	    raw->setNHit(1);
	}
	//-------------------------------------------------

	//-------------------------------------------------
	// get simulation paramters for the cell
	HTofDigiParCell& cell=(*fDigiPar)[fLoc[0]][fLoc[1]][fLoc[2]];
	hl = cell.getHalfLen();
	ar = cell.getAngleRef();
	al = cell.getAttenLen();
	vg = cell.getGroupVel();
	slopeTDCl = cell.getLeftTDCSlope();
	slopeTDCr = cell.getRightTDCSlope();
	//-------------------------------------------------

	//-------------------------------------------------
	// get GEANT values of the hit
	geant->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);
	numTrack=geant->getTrack();



	//-------------------------------------------------
	// find the first track ID entering the TOF
	// depending on storeFirstTrack flag
	// if replacing of tof track number is used
	HGeantTof* pOld=geant;
	HGeantTof* pNew=geant;
	Int_t tempTrack=0;
	Int_t count    =0;
	tempTrack=findFirstHitInTof(pOld,&pNew,&count);
	if(out)   fillNtuple       (pOld, pNew, count);
	numTrack =tempTrack;
	//-------------------------------------------------

	if(raw->getNHit()==1){
	    // for a new hit left and right
	    // track number have to be set
	    numTrack2=numTrack;
	    numTrack1=numTrack;
	}
	//-------------------------------------------------

	if((vg!=0.0) && (al!=0.0))  {

	    //-------------------------------------------------
	    // calculation of left / right times
	    timeL = geaTof + (hl - geaX)/vg;
	    timeR = geaTof + (hl + geaX)/vg;

	    timeCh = (Int_t) (timeL/slopeTDCl);
	    if (timeCh < 0) timeCh = 0;
	    if (timeCh > 4095) timeCh = 4095;
	    timeL = ( Float_t ) (timeCh) + 0.5;

	    timeCh = (Int_t) (timeR/slopeTDCr);
	    if (timeCh < 0) timeCh = 0;
	    if (timeCh > 4095) timeCh = 4095;
	    timeR = ( Float_t ) (timeCh) + 0.5;
	    //-------------------------------------------------

	    //-------------------------------------------------
	    // comparing the measured left/right tofs to the previous
	    // existing times (=100000 if there was no hit before)
	    // The shorter times wins.
	    //-------------------------------------------------

	    if(raw->getNHit()>1){
		// for a Double_t hit the track numbers and measured
		// times have to be sorted.
		if(timeL < prevTimeL){
		    // sort track number by left time
		    numTrack1=numTrack;
		}

		if(timeR < prevTimeR){
		    // sort track number by right time
		    numTrack2=numTrack;
		}
	    }
	    // if times are larger than prevTimes keep
	    // the previous times
	    if(timeL > prevTimeL) {timeL = prevTimeL;}
	    if(timeR > prevTimeR) {timeR = prevTimeR;}
	    //-------------------------------------------------



	    //-------------------------------------------------
	    // calculation of charges
	    chargeL = geaEner*photYield*quantEff*0.5*(1 - cos(ar*deg2rad))*exp(-(hl-geaX)/al);
	    amplResol = chargeL*relAmplResol;
	    chargeL = gRandom->Gaus(chargeL,amplResol);
	    chargeR = geaEner*photYield*quantEff*0.5*(1 - cos(ar*deg2rad))*exp(-(hl+geaX)/al);
	    amplResol = chargeR*relAmplResol;
	    chargeR = gRandom->Gaus(chargeR,amplResol);

	    chargeRef = photYield*quantEff*0.5*(1-cos(ar*deg2rad))*minEnerRelease*exp(-hl/al);
	    if(fLoc[1] <= 3) chargeRef *= 3.;
	    if(fLoc[1]>3 && fLoc[1]<=7) chargeRef *= 2.;

	    chargeCh = (Int_t) ((chargeL/chargeRef)*256. + prevChargeL);
	    if (chargeCh < 0) chargeCh = 0;
	    if (chargeCh > MAXCHRGCH)  chargeCh = MAXCHRGCH;
	    chargeL = (Float_t)chargeCh;

	    chargeCh = (Int_t) ((chargeR/chargeRef)*256. + prevChargeR);
	    if (chargeCh < 0) chargeCh = 0;
	    if (chargeCh > MAXCHRGCH)  chargeCh = MAXCHRGCH;
	    chargeR = (Float_t)chargeCh;
	    //-------------------------------------------------

	} else {

	    //-------------------------------------------------
	    // setting default values for times and charges
	    timeL = 4095.;
	    timeR = 4095.;
	    chargeL = MAXCHRGCH;
	    chargeR = MAXCHRGCH;
	    //-------------------------------------------------
	}

	//-------------------------------------------------
	// filling the hit object
	raw->setLeftTime(timeL);
	raw->setRightTime(timeR);
	raw->setLeftCharge(chargeL);
	raw->setRightCharge(chargeR);
	raw->setSector((Char_t) fLoc[0]);
	raw->setModule((Char_t) fLoc[1]);
	raw->setCell((Char_t) fLoc[2]);
	raw->setLeftNTrack(numTrack1);
	raw->setRightNTrack(numTrack2);
	//-------------------------------------------------

    } // end loop over geant category
}
void HTofDigitizer::doFinalCheckOnArray()
{

    //-------------------------------------------------
    // Exclusion of hits with charge less than ADC threshold.
    // Time of hits with charge less than CFD threshold is set
    // to zero. These hits are excluded later in the hitfinder.

    HTofRawSim* raw = 0;

    Float_t hl, al, slopeTDCl, slopeTDCr;
    Int_t thrCFDl, thrCFDr, thrADCl, thrADCr;

    Float_t timeL, timeR, chargeL, chargeR;
    Float_t timeResol;

    Float_t subCl,subCr,depE;


    iterTofRawTmp->Reset();
    while ( (raw=(HTofRawSim *)iterTofRawTmp->Next())!=NULL) {
	fLoc[0] = raw->getSector();
	fLoc[1] = raw->getModule();
	fLoc[2] = raw->getCell();
	timeL=raw->getLeftTime();
	timeR=raw->getRightTime();


	chargeL=raw->getLeftCharge();    //???? threshold should changed too
	chargeR=raw->getRightCharge();   //???? threshold should changed too

	raw->setLeftCharge (fTofWalkPar->scaleGeantToData(chargeL));
	raw->setRightCharge(fTofWalkPar->scaleGeantToData(chargeR));

	HTofDigiParCell& cell=(*fDigiPar)[fLoc[0]][fLoc[1]][fLoc[2]];

	subCl = (raw->getLeftCharge());    //original version
	subCr = (raw->getRightCharge());   //original version
	depE = (*fTofCalPar)[fLoc[0]][fLoc[1]][fLoc[2]].getEdepK()  * (TMath::Sqrt(subCl*subCr));


        //----------------------------------------------------------
        // get particle information for beta scaling of sigmadX
        // used in HTofWalkPar
	Int_t tr1 = raw->getNTrack1();
	Int_t tr2 = raw->getNTrack2();
	HGeantKine* kine1 = 0;
	HGeantKine* kine2 = 0;
	HGeantKine* kine  = 0;

	if(tr1>0) kine1 = (HGeantKine*)fGeantKineCat ->getObject(tr1-1);
	if(tr2>0) kine2 = (HGeantKine*)fGeantKineCat ->getObject(tr2-1);
        Double_t beta = 0;
        kine = kine1;


	if      (tr1 == tr2)   { kine = kine1;}
	else if (tr2 == -1 )   { kine = kine1;}
	else if (tr2 != tr1 )  {  // this case for apr12 ~ 2%
	    if        (kine2->getParentTrack()==0 && kine1->getParentTrack()>0)  { // tr2 is primary and tr1 is not
		kine = kine2;
	    } else if (kine2->getParentTrack()==0 && kine1->getParentTrack()==0) {  //both primary
		kine = kine1;
	    } else if (kine2->getParentTrack()>0  && kine1->getParentTrack()>0)  {  //both secondary
		kine = kine1;
	    } else if (kine2->getParentTrack()>0  && kine1->getParentTrack()==0) {  // tr1 is primary and tr2 is not
		kine = kine1;
	    }
	}

	Double_t mass   = HPhysicsConstants::mass(kine->getID());
	Double_t p      = kine->getTotalMomentum();
	if(mass > 0) {
	    beta = sqrt(1. / (((mass*mass)/(p*p)) + 1.));
	}
	//----------------------------------------------------------



	Float_t sigma  = fTofWalkPar->getDxSigmaDigi(fLoc[0], fLoc[1], fLoc[2],depE,cell.getGroupVel(),beta) ;   // dx sigma adoped for different vgroup
	timeResZero = sigma / cell.getGroupVel() * 1.41421;

	hl = cell.getHalfLen();
	al = cell.getAttenLen();
	thrCFDl = cell.getLeftCFDThreshold();
	thrCFDr = cell.getRightCFDThreshold();
	thrADCl = cell.getLeftADCThreshold();
	thrADCr = cell.getRightADCThreshold();
	slopeTDCl = cell.getLeftTDCSlope();
	slopeTDCr = cell.getRightTDCSlope();

	timeResol = timeResZero*exp(hl/(2.*al))/slopeTDCl;
	raw->setLeftTime(gRandom->Gaus(timeL,timeResol));
	timeResol = timeResZero*exp(hl/(2.*al))/slopeTDCr;
	raw->setRightTime(gRandom->Gaus(timeR,timeResol));

	if(((Int_t)timeL)<0) raw->setLeftTime(0.0);
	if(((Int_t)timeR)<0) raw->setRightTime(0.0);
	// overflow suppression
	if(((Int_t)timeL)>=4095) raw->setLeftTime(0.0);
	if(((Int_t)timeR)>=4095) raw->setRightTime(0.0);


	// CFD and ADC thresholds
	if(((Int_t)chargeL)<thrCFDl){
	    raw->setLeftTime(0.0);
	    if(((Int_t)chargeL)<thrADCl){
		raw->setLeftCharge(0.0);
	    }

	}
	if(((Int_t) chargeR)<thrCFDr){
	    raw->setRightTime(0.0);
	    if(((Int_t) chargeR)<thrADCr){
		raw->setRightCharge(0.0);
	    }
	}
    }
    //-------------------------------------------------
}

void HTofDigitizer::fillOutput()
{
    //-------------------------------------------------
    // Now copy tmp object to final output
    Int_t embeddingmode=gHades->getEmbeddingMode(); // flag embedding 1=realistic 2:keep geant

    Int_t   numTrack1 = -1, numTrack2 = -1;            //track numbers
    Float_t timeL, timeR, chargeL, chargeR;
    Int_t   numTrack1Out = -1, numTrack2Out = -1;      //track numbers
    Float_t timeLOut, timeROut, chargeLOut, chargeROut;

    HTofRawSim* raw = 0;
    HTofRawSim * rawOut;
    iterTofRawTmp->Reset();
    while ( (raw=(HTofRawSim *)iterTofRawTmp->Next())!=NULL) {
	fLoc[0] = raw->getSector();
	fLoc[1] = raw->getModule();
	fLoc[2] = raw->getCell();
	timeL     = raw->getLeftTime();
	timeR     = raw->getRightTime();
	chargeL   = raw->getLeftCharge();
	chargeR   = raw->getRightCharge();
	numTrack1 = raw->getLeftNTrack();
	numTrack2 = raw->getRightNTrack();

	rawOut = (HTofRawSim*) fRawCat->getObject(fLoc);   // test if cell in use

	if(rawOut)
	{
	    if(embeddingmode>0)
	    {
		// if cell already exists
		raw->incNHit();  // increment counter for additional hits
		numTrack1Out = rawOut->getLeftNTrack();
		numTrack2Out = rawOut->getRightNTrack();
		timeLOut     = rawOut->getLeftTime();
		timeROut     = rawOut->getRightTime();
		chargeLOut   = rawOut->getLeftCharge();
		chargeROut   = rawOut->getRightCharge();


		//-------------------------------------------------
		// in case of Double_t hits real hits should be
		// taken into account. In embeddingmode=2 real hits
		// should not overwrite sim track numbers.

		//-------------------------------------------------
		// for embeddingmode=2 the GEANT hits should be kept anyway
		// if times are larger than prevTimes keep
		// the previous times

		if(timeL < timeLOut) {
		    numTrack1Out = numTrack1;
		    timeLOut     = timeL;
		}

		if(embeddingmode==2) numTrack1Out = numTrack1; // keep Geant

		if(timeR < timeROut) {
		    numTrack2Out = numTrack2;
		    timeROut     = timeR;

		}
		if(embeddingmode==2) numTrack2Out = numTrack2; // keep Geant
		//-------------------------------------------------

		rawOut->setLeftNTrack(numTrack1Out);
		rawOut->setRightNTrack(numTrack2Out);
		rawOut->setLeftTime(timeLOut);
		rawOut->setRightTime(timeROut);
		rawOut->setLeftCharge (chargeLOut+chargeL);
		rawOut->setRightCharge(chargeROut+chargeR);
	    }
	    else
	    {
		Error("fillOutput()","Slot was used before. This case should never happen in sim mode! Will overwrite object!");
		rawOut = new(rawOut) HTofRawSim(*raw);
	    }

	}
	else
	{
	    rawOut = (HTofRawSim*) fRawCat->getNewSlot(fLoc);  // get new slot
	    if(!raw)
	    {
		Error("execute()","Could not retrieve new Slot in Category catTofRaw!");
		continue;
	    }
	    rawOut = new(rawOut) HTofRawSim(*raw);
	}
	//-------------------------------------------------
    }
    //-------------------------------------------------
}

Bool_t HTofDigitizer::finalize()
{
    TDirectory* saveDir=gDirectory;
    if(out)
    {
	outFile->cd();
	out->Write();
	outFile->Save();
	outFile->Close();
    }
    saveDir->cd();
    return kTRUE;
}
Int_t HTofDigitizer::findFirstHitInTof(HGeantTof* poldTof,HGeantTof** pnewTof,Int_t* count)
{
    //-------------------------------------------------
    // find the first track ID entering the TOF
    // Used to suppress the secondaries created in the
    // TOF itself.
    //        0 (default) = realistic (secondaries included)
    //        1 primary particle is stored
    //        2 the first track number entering the tof in SAME SECTOR is stored
    //        3 as 2 but condition on SAME SECTOR && MOD
    //        4 as 2 but SAME SECTOR && MOD && ROD

    Int_t numTrack=poldTof->getTrack();

    if(numTrack<=0) return numTrack; // nothing to do for real data
    HGeantKine* kine=0;
    *count=0;

    //--------------------------------------------------------
    // return the track number for
    // the selected option storeFirstTrack

    //--------------------------------------
    // case 0
    if(storeFirstTrack==0) return numTrack;
    //--------------------------------------
    // case 1
    if(storeFirstTrack==1)
    {   // return track number of primary particle
	// of the given track
	kine=(HGeantKine*)fGeantKineCat->getObject(numTrack-1);
	Int_t parent=kine->getParentTrack();


	kine=HGeantKine::getPrimary(numTrack,fGeantKineCat);
	if(parent>0)(*count)--; // mark only those which change
	if(debug)
	{
	    // make the changed track numbers easily
	    // visible in output. ONLY DEBUGGING!!!!
	    return (*count)*200;
	}
	else return kine->getTrack();
    }

    //--------------------------------------
    // case 2 and 3
    kine=(HGeantKine*)fGeantKineCat->getObject(numTrack-1);
    if(kine->getParentTrack()==0){return numTrack;} // nothing to do

    Int_t s,m,c;
    s= poldTof->getSector();
    m= 21-poldTof->getModule();
    c= 7 -poldTof->getCell();

    Int_t first=0;
    Int_t tempTrack=numTrack;
    while((kine=kine->getParent(tempTrack,fGeantKineCat))!=0)
    {
	first=kine->getFirstTofHit();
	if(first!=-1)
	{
	    // track is still in TOF

	    // now we have to check if it is in TOF or TOFINO
	    HGeantTof* gtof=(HGeantTof*)fGeantCat->getObject(first);

	    Int_t s1,m1,c1;
	    s1=m1=c1=0;

	    m1 = 21-gtof->getModule();
	    if(m1>=0)
	    {
		// inside TOF
		s1= gtof->getSector();
		c1= 7-gtof->getCell();

		if(storeFirstTrack==2&&
		   s==s1)
		{   // case 2 :: check only sector
		    tempTrack  =kine->getTrack();
		    (*pnewTof)=gtof;
		    (*count)--;
		}
		if(storeFirstTrack==3&&
		   s==s1&&m==m1)
		{   // case 3 :: check only sector,module
		    tempTrack  =kine->getTrack();
		    (*pnewTof)=gtof;
		    (*count)--;
		}
		else if(storeFirstTrack==4&&
			s==s1&&m==m1&&c==c1)
		{   // case 4 :: check for same rod
		    tempTrack  =kine->getTrack();
		    (*pnewTof)=gtof;
		    (*count)--;
		}
		else {
		    // track has no TOF hit any longer
		    // which fulfills the condition
		    break;
		}

	    } else {
		// track is in TOFINO
		break;
	    }
	}
	else {
	    // track has no TOF hit any longer,
	    // so the previous object was the one we
	    // searched  for
	    break;
	}
    }
    if(debug&&(*count)<0)
    {   // make the changed track numbers easily
	// visible in output. ONLY DEBUGGING!!!!
	return (*count)*200;
    }
    else return tempTrack;
}
void HTofDigitizer::setOutputFile(TString outname)
{
    // set ouput file for internal Ntuple. Create
    // Ntuple inside output.

    TString outName;
    if(outname.CompareTo("")==0){
	outName ="tofdigiNtuple.root";
	Info("HTofDigitizer::setOutputFile()","No name speciefied use Output file : %s ",outName.Data());
    }
    else
    {
	outName=outname;
	Info("HTofDigitizer::setOutputFile()","Output file : %s ",outName.Data());
    }
    TDirectory* saveDir=gDirectory;
    outFile=new TFile(outName.Data(),"RECREATE");

    if(outFile)
    {
	outFile->cd();

	out=new TNtuple("ntuple","ntuple","s:m:c:" // old cell
			"s1:m1:c1:"                // new cell
			"gE:gE2:"                  // geant energy
			"gX:gX1:gY:gY1:"           // geant xy old/new
			"gTof:gTof1:"              // geant tof
			"gMom:gMom1:"              // geant momentum
			"gLen:gLen1:"              // geant track length
			"trackOld:trackNew:"       // geant track number
			"parentOld:parentNew:"     // geant parent track
			"count");                  // number of iterations

	if(!out)
	{
	    Error("HTofDigitizer::setOutputFile()","Could not create Ntuple!");
	    exit(1);
	}
    } else {

	Error("HTofDigitizer::setOutputFile()","Could not create Output File!");
	exit(1);

    }
    saveDir->cd();
}
void HTofDigitizer::fillNtuple(HGeantTof* pold,HGeantTof* pnew,Int_t count)
{
    // old GEANT TOF hit
    HGeantKine* kine=0;
    Int_t parentOld,parentNew;
    Int_t trackOld,trackNew;

    Float_t geaEner,geaX,geaY,geaTof,geaMom,trackLen ;
    pold->getHit(geaEner,geaX,geaY,geaTof,geaMom,trackLen);

    kine=(HGeantKine*)fGeantKineCat->getObject(pold->getTrack()-1);
    parentOld=kine->getParentTrack();
    trackOld =kine->getTrack();



    // new GEANT TOF hit
    Float_t geaEner1,geaX1,geaY1,geaTof1,geaMom1,trackLen1 ;
    pnew->getHit(geaEner1,geaX1,geaY1,geaTof1,geaMom1,trackLen1);

    kine=(HGeantKine*)fGeantKineCat->getObject(pnew->getTrack()-1);
    parentNew=kine->getParentTrack();
    trackNew =kine->getTrack();

    Float_t dat[23]={0};



    dat[0] = pold->getSector();
    dat[1] = 21-pold->getModule();
    dat[2] = 7-pold->getCell();

    dat[3] = pnew->getSector();
    dat[4] = 21-pnew->getModule();
    dat[5] = 7-pnew->getCell();

    dat[6] = geaEner;
    dat[7] = geaEner1;
    dat[8] = geaX;
    dat[9] = geaX1;
    dat[10]= geaY;
    dat[11]= geaY1;
    dat[12]= geaTof;
    dat[13]= geaTof1;
    dat[14]= geaMom;
    dat[15]= geaMom1;
    dat[16]= trackLen;
    dat[17]= trackLen1;
    dat[18]= trackOld;
    dat[19]= trackNew;
    dat[20]= parentOld;
    dat[21]= parentNew;

    dat[22]= count;


    out->Fill(dat);

}

ClassImp(HTofDigitizer)
