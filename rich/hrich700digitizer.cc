//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700Digitizer
//
//  This class handles the mapping between xy position and PMT
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700digitizer.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hevent.h"
#include "hgeantrich.h"
#include "hparset.h"
#include "hspectrometer.h"
#include "hrichdetector.h"
#include "richdef.h"
#include "hrichcalsim.h"
#include "hgeantkine.h"
#include "hrich700digipar.h"

#include "TRandom.h"

#include <iostream>
#include <sstream>

using namespace std;


ClassImp(HRich700Digitizer)

HRich700Digitizer* HRich700Digitizer::fDigitizer = 0;

HRich700Digitizer::HRich700Digitizer(const Text_t* name,const Text_t* title):
    HReconstructor(name,title),
fStoreOnlyConvertedPhotonTrackIds(kTRUE)
{
    fDigitizer = this;
    setDeltaElectronUse(kFALSE,kFALSE,109,20.,2.);
    setDeltaElectronMinMomCut(0.,0.,0.,0.,0.,0.);

}

HRich700Digitizer::~HRich700Digitizer()
{

}

Bool_t HRich700Digitizer::init()
{
    fCatKine = gHades->getCurrentEvent()->getCategory(catGeantKine);
    if (NULL == fCatKine) {
    	Error("init", "Initializatin of kine category failed, returning...");
    	return kFALSE;
    }

    // Initialize geant rich cherenkov photon category and set appropriate iterator
    fCatRichPhoton = gHades->getCurrentEvent()->getCategory(catRichGeantRaw);
    if (NULL == fCatRichPhoton) {
    	Error("init", "Initializatin of Cherenkov photon category failed, returning...");
    	return kFALSE;
    }

    // Initialize geant category for direct hits and set appropriate iterator
    fCatRichDirect = gHades->getCurrentEvent()->getCategory(catRichGeantRaw + 1);
    if (NULL == fCatRichDirect) {
	    Error("init", "Initialization of geant category for direct hits failed, returning...");
	    return kFALSE;
    }

    // Initialize output category catRichCalSim
    HRichDetector* pRichDet = static_cast<HRichDetector*>(gHades->getSetup()->getDetector("Rich"));
    fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCatRichCal) {
	    fCatRichCal = pRichDet->buildMatrixCat("HRichCalSim", 1);
	    if (NULL == fCatRichCal) {
	        Error("init", "Can not build output category catRichCalSim, returning...");
	           return kFALSE;
	     } else {
	         gHades->getCurrentEvent()->addCategory(catRichCal, fCatRichCal, "Rich");
	   }
    }

    fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(!fDigiPar) {
	Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }
    return kTRUE;
}


Bool_t HRich700Digitizer::reinit()
{
    return kTRUE;
}


Int_t HRich700Digitizer::execute()
{
    processEvent();

    return 0;
}

void HRich700Digitizer::processEvent()
{
    // prob selection for delta electrons
    setProbabilityForDeltaElectrons();

    Int_t nofPhotons = fCatRichPhoton->getEntries();
    for (Int_t i = 0; i < nofPhotons; i++) {
        HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));

        // work on delta electrons
        Int_t trkNum = richPhoton->getTrack();
        HGeantKine* primary = HGeantKine::getPrimary(trkNum,(HLinearCategory*)fCatKine);
        Bool_t isSkip = workOnDeltaElectrons(primary, richPhoton->getSector());
        if (isSkip) continue;

        processRichPhoton(richPhoton);
    }

    Int_t nofDirects = fCatRichDirect->getEntries();
    for (Int_t i = 0; i < nofDirects; i++) {
        HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));

        // work on delta electrons
        Int_t trkNum = richDirect->getTrack();
        HGeantKine* primary = HGeantKine::getPrimary(trkNum,(HLinearCategory*)fCatKine);
        Bool_t isSkip = workOnDeltaElectrons(primary, richDirect->getSector());
        if (isSkip) continue;

        processRichDirect(richDirect);
    }

    addNoiseHits();

    if (fStoreOnlyConvertedPhotonTrackIds == kFALSE) addAllTrackIds();

}

void HRich700Digitizer::setProbabilityForDeltaElectrons()
{
    if(fUseDeltaElectrons && fProbDeltaAccepted<1) {
        fmDeltaTrackProb.clear();
        if(fCatKine){
     	   Int_t nKine = fCatKine->getEntries();
     	   for(Int_t i=0;i<nKine;i++){
     	       HGeantKine* kine =  (HGeantKine*)fCatKine->getObject(i);
     	       Float_t mom = kine->getTotalMomentum() ;
     	       Int_t generator = kine->getGeneratorInfo();
                Bool_t isBeamIonsSim = (kine->getID() == fIonID); // beam ions simulated
                Bool_t isElMomCut = (fUseDeltaMomSelection && kine->getID() == 3 && mom < fMomMaxDeltaElecCut); // any electron < momCut (shooting with kine generator)
                Bool_t isDeltaEl = (!fUseDeltaMomSelection && kine->getID() == 3 && generator ==-3 ); // delta electrons input file source id ==-3
     	       if ( kine->getParentTrack() == 0 && (isBeamIonsSim || isElMomCut || isDeltaEl) ) {
     		       fmDeltaTrackProb[kine] = gRandom->Rndm();
     	       }
     	   }
        }
    }
}

Bool_t HRich700Digitizer::workOnDeltaElectrons(HGeantKine* primary, Int_t sector)
{
    // if return is true: skipp all delta electrons and daughter hits

    if(NULL != primary) {
        Float_t mom = primary->getTotalMomentum() ;
        Int_t generator = primary->getGeneratorInfo();
        Bool_t isBeamIonsSim = (primary->getID() == fIonID); // beam ions simulated
        Bool_t isElMomCut = (fUseDeltaMomSelection && primary->getID() == 3 && mom < fMomMaxDeltaElecCut); // any electron < momCut (shooting with kine generator)
        Bool_t isDeltaEl = (!fUseDeltaMomSelection && primary->getID() == 3 && generator ==-3 ); // delta electrons input file source id ==-3

        if( isBeamIonsSim || isElMomCut || isDeltaEl) {
            if(fUseDeltaElectrons) {
            if(mom < fMomMinDeltaCut[sector]) return kTRUE;  // skipp all delta lower than cutmom

            //-------------------------------------------------------------------
            if(fProbDeltaAccepted<1) {
                Float_t prob = 2;
                fitDelta = fmDeltaTrackProb.find(primary);
                if(fitDelta != fmDeltaTrackProb.end()){
                    prob = fitDelta->second;
                } else {
                    Error("workOnDeltaElectrons()","No primary in delta map for found! Should not happen!");
                    primary->print();
                }
                if(prob < fProbDeltaAccepted ) return kTRUE;
            }
            //-------------------------------------------------------------------
            } else {
                return kTRUE; // skipp all delta electrons and daughter hits
            }
        }
    }
    return kFALSE;
}

void HRich700Digitizer::processRichPhoton(HGeantRichPhoton* photon)
{
    if(photon == NULL) return;
    //trackId of the mother particle (eg. electron, pion etc)
    Int_t trackId = photon->getTrack();
    Double_t momTotal = photon->getEnergy();

    HRich700PmtData* pmtData = fDigiPar->getPMTData(photon->getPmtId());
    if (NULL == pmtData) {
        cout << "processRichPhoton: pmtData is NULL for pmtId:" << photon->getPmtId() << endl;;
        return;
    }
    Bool_t isDetected = fPmt.isPhotonDetected(pmtData->fPmtType, fDigiPar->getCollectionEfficiency(), momTotal);

    if (isDetected) {
        Int_t loc[3];
        fDigiPar->getLocation(photon->getPmtId(), photon->getY(), photon->getX(), loc);  // x and y have to be swapped for compatibility
        addRichCal(loc[0], loc[1], loc[2], trackId);
        addCrossTalkHit(loc[0], loc[1], loc[2], trackId);
    }
}

void HRich700Digitizer::processRichDirect(HGeantRichDirect* direct)
{
    if (direct == NULL) return;
    Int_t trackId = direct->getTrack();
    Bool_t isDetected = kTRUE; // assume that all charged particles are detected
    if (isDetected) {
    	Int_t loc[3];
    	fDigiPar->getLocation(direct->getPmtId(), direct->getY(), direct->getX(), loc); // x and y have to be swapped for compatibility
    	addRichCal(loc[0], loc[1], loc[2], trackId);
    	addCrossTalkHit(loc[0], loc[1], loc[2], trackId);
    }
}

void HRich700Digitizer::addRichCal(Int_t sector, Int_t col, Int_t row, Int_t trackId)
{
    HLocation loc;
    loc.set(3, sector, col, row);
    if (col == -1 || row == -1) return;

    HRichCalSim* calSim = static_cast<HRichCalSim*>(fCatRichCal->getObject(loc));
    if (NULL == calSim) {
    	calSim = static_cast<HRichCalSim*>(fCatRichCal->getSlot(loc));
    	if (NULL != calSim) {
    	    calSim = new(calSim) HRichCalSim;
    	    //calSim->setEventNr(gHades->getCurrentEvent()->getHeader()->getEventSeqNumber());
    	    calSim->setSector(sector);
    	    calSim->setCol(col);
    	    calSim->setRow(row);
    	    calSim->addTrackId(trackId);
    	} else {
    	}
    } else { // pixel was already fired
    	// cout << "pixel was already fired " << calSim->getNofTrackIds()<< endl;
    	if(!calSim->checkTrackId(trackId)){
    	    calSim->addTrackId(trackId);
    	}
    }
}

void HRich700Digitizer::addAllTrackIds()
{
    Int_t nofPhotons = fCatRichPhoton->getEntries();
    for (Int_t i = 0; i < nofPhotons; i++) {
    	HGeantRichPhoton* richPhoton = static_cast<HGeantRichPhoton*>(fCatRichPhoton->getObject(i));
    	if (richPhoton == NULL) continue;
    	Int_t loc[3];
    	fDigiPar->getLocation(richPhoton->getPmtId(), richPhoton->getY(), richPhoton->getX(), loc);  // x and y have to be swapped for compatibility
        addTrackId(loc[0], loc[1], loc[2], richPhoton->getTrack());
    }

    Int_t nofDirects = fCatRichDirect->getEntries();
    for (Int_t i = 0; i < nofDirects; i++) {
    	HGeantRichDirect* richDirect = static_cast<HGeantRichDirect*>(fCatRichDirect->getObject(i));
    	if (richDirect == NULL) continue;
    	Int_t loc[3];
    	fDigiPar->getLocation(richDirect->getPmtId(), richDirect->getY(), richDirect->getX(), loc);  // x and y have to be swapped for compatibility
    	addTrackId(loc[0], loc[1], loc[2], richDirect->getTrack());
    }
}

void HRich700Digitizer::addTrackId(Int_t sector, Int_t col, Int_t row, Int_t trackId)
{
    if (sector == -1 && col == -1 && row == -1) return;
    
    HLocation loc;
    loc.set(3, sector, col, row);

    HRichCalSim* calSim = static_cast<HRichCalSim*>(fCatRichCal->getObject(loc));
    if (NULL != calSim) {
    	if(!calSim->checkTrackId(trackId)){
    	    calSim->addTrackId(trackId);
    	}
    }
}

void HRich700Digitizer::addCrossTalkHit(Int_t sector, Int_t col, Int_t row, Int_t trackId)
{
    if (fDigiPar->getCrossTalkProbability() <=0. ) return;
    if (col == -1 || row == -1) return;

    Bool_t crosstalkHitDetected = kFALSE;
    vector<pair<Int_t, Int_t> > directPixels = fDigiPar->getDirectNeighbourPixels(col, row);
    vector<pair<Int_t, Int_t> > diagonalPixels = fDigiPar->getDiagonalNeighbourPixels(col, row);

    for (UInt_t i = 0; i < directPixels.size(); i++) {
    	if (gRandom->Rndm() < fDigiPar->getCrossTalkProbability() && !crosstalkHitDetected) {
    	    addRichCal(sector, directPixels[i].first, directPixels[i].second, trackId);
    	    crosstalkHitDetected = kTRUE;
    	    break;
    	}
    }

    if (!crosstalkHitDetected) {
    	for (UInt_t i = 0; i < diagonalPixels.size(); i++) {
    	    if (gRandom->Rndm() < fDigiPar->getCrossTalkProbability() / 4. && !crosstalkHitDetected) {
        		addRichCal(sector, diagonalPixels[i].first, diagonalPixels[i].second, trackId);
        		crosstalkHitDetected = kTRUE;
        		break;
    	    }
    	}
    }
}


void HRich700Digitizer::addNoiseHits()
{
    if (fDigiPar->getNofNoiseHits() <= 0) return;
    Int_t sector = 0;
    vector<pair<Int_t, Int_t> > noisePixels = fDigiPar->getNoisePixels(fDigiPar->getNofNoiseHits());
    for (UInt_t i = 0; i < noisePixels.size(); i++) {
        sector = fDigiPar->getSectorPixels(noisePixels[i].first,noisePixels[i].second);
	    addRichCal(sector, noisePixels[i].first, noisePixels[i].second, -1);
    }
}

Bool_t HRich700Digitizer::finalize()
{
    return kTRUE;
}
