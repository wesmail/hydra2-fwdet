//*--- Author: Jochen Markert

// from ROOT
#include "TF2.h"
#include "TRandom.h"

// form hydra
#include "hcategory.h"
#include "hgeantkine.h"
#include "hgeantmdc.h"
#include "hgeomtransform.h"
#include "hgeomvector.h"
#include "hlocation.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdccal2.h" //?
#include "hmdccal2sim.h" //?
#include "hmdccal2par.h"
#include "hmdccal2parsim.h"
#include "hmdcgetcontainers.h"
#include "hmdcgeomobj.h"
#include "hmagnetpar.h"
#include "hmdcdigitpar.h"
#include "hmdcseg.h"
#include "hmdcidealclasses.h"
#include "hmdcsizescells.h"
#include "hmdctaskset.h"
#include "hmdctrackgdef.h"
#include "hmdctrackgfieldpar.h"
#include "hmdctrkcand.h"
#include "hmdcidealclasses.h"
#include "hmetamatch2.h"
#include "hrootsource.h"
#include "hruntimedb.h"
#include "hmdcdetector.h"
#include "htaskset.h"
#include "htofdetector.h"
#include "hspectrometer.h"

#include "hkaldef.h"
#include "hkaldetcradle.h"
#include "hkalinput.h"
#include "hkalgeomtools.h"
#include "hkalmdchit.h"

#include <iostream>
#include <vector>
using namespace std;

ClassImp (HKalInput)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//    HKalInput is a helper class to fill a TObjArray of hits in sector
//    coordinate system to be used in the Kalman filter. Input can be provided
//    from HMdcTrkCand (real tracking) or HMdcTrkCandIdeal (ideal tracking from
//    from GEANT hit points). It will fill 4 (2x2) hits from inner/outer segment.
//    Errors for real tracking are taken from HMdcSeg, for ideal tracking 200/100 mu
//    in x/y. The errors can be scaled by a factor. The geometrical transformations
//    are performed via HMdcSizesCells, thus the container has to initialized
//    before usage.
//
//-----------------------------------------------------------------------
//    HOWTO use the HKalMdcInput class::
//    before Event loop:
//
//    // assume Spectrometer setup + Hades creation is performed ....
//
//    HKalDetCradle *pCradleHades = new HKalDetCradle();
//    HKalMdcInput *input = new HKalMdcInput(refID, pCradleHades);   // fill the input hits
//    input->setPrint(0);                       // debug print (off == 0)
//
//    TObjArray allhits;                        // array of hits to be fed into kalman
//    Int_t measDim = 2;                        // dimension of hit vector (default = 2)
//    Double_t scaleErr = 1.;                   // scaling factor for errors (default = 1.)
//
//    Inside eventloop:
//
//        //-----------------------------------------------------------------------
//        // Run Kalman filter on hits.
//        Int_t nrHits = 0;
//        input->resetIterMdcTrkCand();
//	  	  while( (nrHits = input->nextMdcTrackCand(allhits, measDim, scaleErr)) )
//        {
//	       // ..... kalman
//
//	      }
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////

//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalInput::HKalInput() : TObject() {
    // It is recommended to use the constructor
    // HKalInput(Int_t refID, HKalDetCradle *detCradle)
    // instead where the object will be initialized correctly.
    // This empty constructor sets all pointers to NULL. To use the object
    // the following still has to be done afterwards:
    // Do spectrometer setup + Hades creation ....
    // To set the pointer to detector cradle:
    // setDetectorCradle(detCradle)  // set pointer to detector cradle
    // To get pointer to categories, HMdcSizesCells, magnet and B-field
    // parameters:
    // init(refID)                   // gets categories

    pCradleHades = NULL;
    fSizesCells  = NULL;
    pField       = NULL;
    pMagnet      = NULL;
    setPrint(kFALSE);
}

HKalInput::HKalInput(HKalDetCradle *detCradle) : TObject() {
    // Assumes spectrometer setup + Hades creation have been performed ....
    // This constructor sets all needed class members.
    // refID:     runtime ID
    // detCradle: pointer to Hades detector cradle

    pCradleHades = NULL;
    fSizesCells  = NULL;
    pField       = NULL;
    pMagnet      = NULL;
    setDetectorCradle(detCradle);
    setPrint(kFALSE);
}

HKalInput::~HKalInput() {
    delete pCradleHades;
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

void HKalInput::calcSegPoints(const HMdcSeg *seg, Double_t& x1,  Double_t& y1, Double_t& z1, Double_t& x2, Double_t& y2, Double_t& z2) {
    // calculates 2 coordinate points from segment.

    Double_t teta = seg->getTheta();
    Double_t phi  = seg->getPhi();
    Double_t z    = seg->getZ();
    Double_t r    = seg->getR();
    Double_t pi2  = acos(-1.)/2.;

    Double_t X = r * cos(phi + pi2);
    Double_t Y = r * sin(phi + pi2);
    Double_t Z = z;

    x1 = X;
    y1 = Y;
    z1 = Z;
    x2 = X + cos(phi) * sin(teta);
    y2 = Y + sin(phi) * sin(teta);
    z2 = Z + cos(teta);
}


void HKalInput::clearCategories() {
    // Clear the categories (for macros in MakeCode style).

    if(kineCat)            kineCat           ->Clear();
    if(mdcTrkCandCat)      mdcTrkCandCat     ->Clear();
    if(mdcTrkCandCat)      mdcTrkCandCat     ->Clear();
    if(mdcSegCat)          mdcSegCat         ->Clear();
    if(mdcHitCat)          mdcHitCat         ->Clear();
    if(mdcTrkCandIdealCat) mdcTrkCandIdealCat->Clear();
    if(mdcSegIdealCat)     mdcSegIdealCat    ->Clear();
    if(mdcHitIdealCat)     mdcHitIdealCat    ->Clear();
}

void HKalInput::getCategories() {
    // Retries the pointer to needed input categories. Creates
    // iterators.

    geantMdcCat         = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcGeantRaw);
    kineCat             = (HCategory*)gHades->getCurrentEvent()->getCategory(catGeantKine);

    mdcTrkCandCat       = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcTrkCand);
    mdcSegCat           = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcSeg);
    mdcHitCat           = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcHit);

    mdcTrkCandIdealCat  = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcTrkCandIdeal);
    mdcSegIdealCat      = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcSegIdeal);
    mdcHitIdealCat      = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcHitIdeal);

    mdcCal1Cat          = (HCategory*)gHades->getCurrentEvent()->getCategory(catMdcCal1);

    metaMatchCat        = (HCategory*)gHades->getCurrentEvent()->getCategory(catMetaMatch);

    if(kineCat) iterKine = (HIterator *) kineCat->MakeIterator("native");
    else        iterKine = NULL;

    if(mdcTrkCandCat) iterMdcTrkCand = (HIterator *) mdcTrkCandCat->MakeIterator("native");
    else              iterMdcTrkCand = NULL;

    if(mdcTrkCandIdealCat) iterMdcTrkCandIdeal = (HIterator *) mdcTrkCandIdealCat->MakeIterator("native");
    else                   iterMdcTrkCandIdeal = NULL;

    if(metaMatchCat)  iterMetaMatch  = (HIterator *) metaMatchCat->MakeIterator("native");
    else              iterMetaMatch  = NULL;

}

Bool_t HKalInput::init(Int_t refID) {
    Bool_t bSuccess;
    if(!gHades) {
        Error("init()", "Hades object does not exist!");
        exit(1);
    }

    getCategories();                   // get Pointer to categories
    bSuccess  = initSizesCells(refID);             // init HMdcSizesCells
    if(!bSuccess) {
        Warning("init()", "Could not initialize SizesCells.");
    }
    bSuccess &= initBField(refID);                 // init Bfield parameters
    if(!bSuccess) {
        Warning("init()", "Could not initialize B-field.");
    }
    mdcGetContainers = HMdcGetContainers::getObject();
    return bSuccess;
}

Bool_t HKalInput::initBField(Int_t refID) {
    // Initialize HMdcTrackGFieldPar and HMagnetPar for run refID.
    // If the container already exists it will not
    // be created a second time.

    HRuntimeDb* rtdb = gHades->getRuntimeDb();
    if(!pField) {
        pField =  (HMdcTrackGFieldPar*)(rtdb->getContainer("MdcTrackGFieldPar"));
    }
    if(!pMagnet) {
        pMagnet = (HMagnetPar*)( rtdb->getContainer("MagnetPar"));
    }
    return (pField && pMagnet);
}


Bool_t HKalInput::initSizesCells(Int_t refID) {
    // Initialize HMdcSizesCells for run refID.
    // If the container already exists it will not
    // be created a second time.

    fSizesCells = HMdcSizesCells::getExObject();  // check if is already there

    if(!fSizesCells) {
        fSizesCells = HMdcSizesCells::getObject();

        return fSizesCells->initContainer();
    } else {
        return kTRUE;
    }
}

HMdcSizesCells* HKalInput::getSizesCells() {
    if(!fSizesCells) {
        Error("getSizesCells()","HMdcSizesCells is NULL!");
        exit(1);
    }
    return fSizesCells;
}

HMdcTrkCand* HKalInput::nextMdcTrackCand(TObjArray &allhits, Int_t measDim, Double_t scaleErr) {
    // Fills 4 hits in sector coordinates system to allhits array.
    // Loops over HMdcTrkCand Category for the next for hits. The
    // iterator for the category has to be reset by hand at the begin
    // of each event. Return 0 if no next candidate has been found.
    // The errors of the hits will be taken from HMdcSeg and can be scaled
    // by scaleErr.
    //
    // allhits: array with HKalMdcHit objects (return value. Existing content of array will be deleted.)
    // measDim: dimension of measurement vector
    // scaleErr: scale errX and errY by this factor

    if(fSizesCells == NULL){
        Error("nextMdcTrackCand()","HMdcSizeCells is NULL!");
        exit(1);
    }
    if(pCradleHades == NULL) {
        Error("nextMdcTrackCand()","DetectorCadle is NULL!");
        exit(1);
    }
    Int_t nrhits = 0;

    HMetaMatch2 *pMetaMatch = NULL;
    HMdcTrkCand *mdcTrkCand = NULL;

    if(!iterMetaMatch) return NULL;


    // This is the SEGment class of MDC. This data container
    // holds the information about a straight track let between
    // a pair of MDCs.

    Int_t   indexSeg    [2];   // index in catMdcSeg for inner/outer seg
    HMdcSeg *mdcSeg     [2];   // segment pointer for inner/outer seg
    Int_t   mdcSegIndHit[4];

    Double_t coordHit   [3];
    Double_t coordErrHit[3];
    Double_t coordErrLay[3];

    // Collecting track candidates.
    for(Int_t ihit = 0; ihit < 4; ihit ++) mdcSegIndHit[ihit] = -1;

    allhits.Clear();
    while ((pMetaMatch = (HMetaMatch2*)(iterMetaMatch->Next())) != 0) {
        mdcTrkCand = (HMdcTrkCand*)mdcTrkCandCat->getObject(pMetaMatch->getTrkCandInd());

        for(Int_t io = 0; io < 2; io ++) { // loop inner/outer segment
            mdcSeg[io]     = 0;

            indexSeg   [io] =  mdcTrkCand->getSegInd(io);
            if(indexSeg[io] != -1) {
                mdcSeg[io] = (HMdcSeg*) mdcSegCat->getObject(indexSeg[io]);
                for(Int_t nhits = 0; nhits < 2; nhits ++) { // loop hits in segment
                    if(mdcSeg[io]) {
                        mdcSegIndHit[io * 2 + nhits] = mdcSeg[io]->getHitInd(nhits);
#if kalDebug > 2
                        cout<<io * 2 + nhits<<" "<< mdcSegIndHit[io * 2 + nhits]<<endl;
#endif
                    } else {
                        if(bPrint) {
                            Warning("nextMdcTrackCand()", "zero segment!");
                        }
                        mdcSegIndHit[io * 2 + nhits] = -1;
                    }
                }
            }
        }
        if(!mdcSeg[0] || !mdcSeg[1]) continue;

        // If a Hit doesn't exist -1 is returned.
#if kalDebug > 2
        cout<<"filling sec "<<(Int_t)mdcSeg[0]->getSec()<<endl;
#endif
        Int_t sec,mod;
        //-------inner segment----------------------------
        HMdcHit* locHit; // temp hit pointer
        for(Int_t io = 0; io < 2; io ++) {              // loop inner/outer segment
            for(Int_t nhits = 0; nhits < 2; nhits ++) { // loop hits in segment
                locHit = (HMdcHit*)mdcHitCat->getObject(mdcSegIndHit[io * 2 + nhits]);
                if(!locHit) {
#if kalDebug > 0
                    if(bPrint) {
                        cout<<"nextMdcTrackCand(), No hit in segment "<<io<<"."<<endl;
                    }
#endif
                    continue;
                }
                locHit->getSecMod(sec, mod);
                const HKalMdcMeasLayer* hitdmeas = pCradleHades->getMdcLayerAt(sec, mod, 0);

                // Get HMdcHit coordinates and errors in local Mdc coordinate system.
                coordHit   [0] = locHit->getX();
                coordHit   [1] = locHit->getY();
		coordHit   [2] = 0;
#if kalDebug > 3
		cout<<"io = "<<io<<" hit = "<<nhits<<", x = "<<locHit->getX()<<", y = "<<locHit->getY()<<endl;
#endif
                (*fSizesCells)[sec][mod].transFrom(coordHit[0], coordHit[1], coordHit[2]);

                coordErrLay[0] = locHit->getErrX() * scaleErr;
                coordErrLay[1] = locHit->getErrY() * scaleErr;
                coordErrLay[2] = 0.03;

		if(locHit->getErrX() == 0. || locHit->getErrY() == 0.) {
#if kalDebug > 0
		    Warning("nextMdcTrackCand()", "Measurement errors are zero.");
#endif
                    coordErrLay[0] = 0.2;
                    coordErrLay[1] = 0.1;
                }
                if(measDim > 2) {
                    HGeomVector errLay(coordErrLay[0], coordErrLay[1], coordErrLay[2]);
                    HGeomVector errSec = (*fSizesCells)[sec][mod].getSecTrans()->getRotMatrix() * errLay;
                    coordErrHit[0] = TMath::Abs(errSec.getX());
                    coordErrHit[1] = TMath::Abs(errSec.getY());
                    coordErrHit[2] = TMath::Abs(errSec.getZ());
                } else {
                    coordErrHit[0] = coordErrLay[0];
                    coordErrHit[1] = coordErrLay[1];
                    coordErrHit[2] = coordErrLay[2];
                }

                allhits.Add(new HKalMdcHit(&coordHit[0], &coordErrHit[0], *hitdmeas, measDim, TVector3(&coordHit[0]), Kalman::kSegHit));
                nrhits++;
            }
	}
        return mdcTrkCand;
    } // end loop over

    return mdcTrkCand;
}

HMdcTrkCand* HKalInput::nextWireTrack(TObjArray &allhits, Int_t minLayInSeg, Int_t minLayOutSeg) {
    // Fills drift chamber wire hits with the drift time as measurement vector
    // to allhits array.
    // Loops over HMdcTrkCandIdeal Category for the next four hits. The
    // iterator for the category has to be reset by hand at the begin
    // of each event. Return 0 if no next candidate has been found.
    //
    // output:
    // allhits: array with HKalMdcHit objects (Existing content of array will be deleted!)
    // input:
    // minLayInSeg:   minimum number of layers in the inner segment needed to fill allhits
    // minLayOutSeg:  minimum number of layers in the outer segment needed

    Int_t             indexSeg[2];   // index in catMdcSeg for inner/outer seg
    Int_t             mdcSegNlay[2];

    HMdcSeg     *mdcSeg[2];
    HMdcTrkCand *mdcTrkCand = 0;
    HMetaMatch2 *pMetaMatch = NULL;

    if(fSizesCells == NULL) {
        Error("nextWireTrack()","HMdcSizeCells is NULL!");
        exit(1);
    }
    if(pCradleHades == NULL) {
        Error("nextWireTrack()","DetectorCradle is NULL!");
        exit(1);
    }

    if(!iterMetaMatch) return NULL;

    HLocation loccal;
    loccal.set(4,0,0,0,0);

    HMdcCal2ParSim *cal2sim = (HMdcCal2ParSim*)gHades->getRuntimeDb()->getContainer("MdcCal2ParSim");
    HMdcCal2Par    *cal2    = (HMdcCal2Par   *)gHades->getRuntimeDb()->getContainer("MdcCal2Par");

    allhits.Clear();


    while ((pMetaMatch = (HMetaMatch2*)(iterMetaMatch->Next())) != 0) {
        mdcTrkCand = (HMdcTrkCand*)mdcTrkCandCat->getObject(pMetaMatch->getTrkCandInd());

        for(Int_t io = 0; io < 2; io ++) { // loop inner/outer segment
            mdcSegNlay[io] = 0;
            mdcSeg[io]     = 0;

            indexSeg   [io] =  mdcTrkCand->getSegInd(io);
            if(indexSeg[io] !=-1){
                mdcSeg[io] = (HMdcSeg*) mdcSegCat->getObject(indexSeg[io]);
                if(!mdcSeg[io]){ cout<<"zero segment!"; continue;}
                mdcSegNlay[io] = mdcSeg[io]->getNLayers();
            }
        }
        if(!mdcSeg[0] || !mdcSeg[1]) continue;

        // Skip if there were not enough hits in each segment.
        if(mdcSegNlay[0] >= minLayInSeg && mdcSegNlay[1] >= minLayOutSeg) {
#if kalDebug > 2
            cout<<"filling sec "<<(Int_t)mdcSeg[0]->getSec()<<endl;
#endif

            // Used for time of flight correction of drift time.
            Double_t sumTof [4]={ 0., 0., 0., 0. };
            Double_t sumErr [4]={ 0., 0., 0., 0. };

            for(Int_t io = 0; io < 2; io ++) {              // loop inner/outer segment

                HMdcSeg *seg = mdcSeg[io];

                for(Int_t l = 0; l < 12; l++) { // segment = 2 modules = 2x6 layers
                    // input selection
                    Int_t nCell = seg->getNCells(l); // number of cells per layer
                    loccal[0]   = seg->getSec();
                    Int_t ioseg = seg->getIOSeg(); // 0=inner,1=outer

                    if(nCell > 0) {
                        TObjArray hits;
                        Int_t    nComp = 0; // Number of competing hits in a layer.

                        //nCell = 1; //?
                        for(Int_t i = 0; i < nCell; i++) {
                            if(ioseg == 0)  {
                                (l < 6) ? loccal[1] = 0 : loccal[1] = 1;
                            } else if(ioseg == 1) {
                                (l < 6) ? loccal[1] = 2 : loccal[1] = 3;
                            }
                            (l < 6) ? loccal[2] = l : loccal[2] = l - 6;

                            // input selection
                            Int_t cell = seg->getCell(l,i);  // get cell number
                            loccal[3] = cell;

                            HMdcCal1Sim *cal1 = (HMdcCal1Sim*)mdcCal1Cat->getObject(loccal);

                            if(cal1->getStatus1() < 0) continue; // Check if hit is valid.

                            Double_t x1, y1, z1, x2, y2, z2 = 0.;

			    HMdcHit* locHit1 = (HMdcHit*)mdcHitCat->getObject(mdcSeg[io]->getHitInd(0));
			    HMdcHit* locHit2 = (HMdcHit*)mdcHitCat->getObject(mdcSeg[io]->getHitInd(1));
			    if(locHit1 && locHit2) {
				x1 = locHit1->getX();
				y1 = locHit1->getY();
				z1 = 0;
				Int_t sec1, mod1;
				locHit1->getSecMod(sec1, mod1);
				(*fSizesCells)[sec1][mod1].transFrom(x1, y1, z1);

				x2 = locHit2->getX();
				y2 = locHit2->getY();
				z2 = 0;
				Int_t sec2, mod2;
				locHit2->getSecMod(sec2, mod2);
				(*fSizesCells)[sec2][mod2].transFrom(x2, y2, z2);
			    } else {
				calcSegPoints(seg, x1, y1, z1, x2, y2, z2);
			    }


                            HMdcSizesCellsLayer &fSizesCellsLayer = (*fSizesCells)[loccal[0]][loccal[1]][loccal[2]];
                            HMdcSizesCellsCell  &fSizesCellsCell  = (*fSizesCells)[loccal[0]][loccal[1]][loccal[2]][loccal[3]];

                            TVector3 layerHit;

                            Double_t alpha, mindist;
                            if(!fSizesCellsLayer.getImpact(x1, y1, z1, x2, y2, z2, loccal[3], alpha, mindist)) {
                                if(bPrint) {
                                    Warning("nextWireTrack()", Form("No impact found with cell %i. Measurement discarded.", loccal[3]));
                                }
                                continue;
                            }
                            // Speed of signal on wire in ns/mm
                            Float_t signalSpeed = ((HMdcDigitPar*)gHades->getRuntimeDb()->getContainer("MdcDigitPar"))->getSignalSpeed();

                            // Path length of signal along the wire in mm.
                            Double_t xWire      = fSizesCellsLayer.getSignPath(x1, y1, z1, x2, y2, z2, cell);
                            // Amount of time that the signal travels on the wire.
                            Double_t tWireOffset = xWire * signalSpeed;

                            // Offset from time of flight subtracted later.
                            Float_t t1  = cal1->getTime1() - tWireOffset;

                            Double_t timeSimErr = cal2sim->
                                calcTimeErr(loccal[0], loccal[1], alpha, mindist);

                            Double_t dist       = cal2->
                                calcDistance  (loccal[0], loccal[1], alpha, t1);

                            //?
                            //Double_t sigTof[4] = { 1.23, 1.25, 1.5, 1.46 }; //? ns
                            Double_t sigTof[4] = { 2.69, 2.72, 2.07, 2.09 }; //? ns apr12
                            Double_t vd[4]     = { 1./24., 1./24., 1./25., 1./25.5 }; //? mm/ns
                            Double_t errTof    = vd[loccal[1]] * sigTof[loccal[1]];

                            Double_t distErr = 0.;

                            HGeomVector &wire1 = fSizesCellsCell.getWirePnt1();
                            TVector3 wirePoint1(wire1.X(), wire1.Y(), wire1.Z());
                            HGeomVector &wire2 = fSizesCellsCell.getWirePnt2();
                            TVector3 wirePoint2(wire2.X(), wire2.Y(), wire2.Z());

                            Int_t iflag;
                            Double_t length;
                            TVector3 pcaFinal, pcaWire;
                            HKalGeomTools::Track2ToLine(pcaFinal, pcaWire, iflag,
                                                        mindist, length,
                                                        TVector3(x1,y1,z1),
                                                        TVector3(x2,y2,z2),
                                                        wirePoint1, wirePoint2);

                            // Make virtual plane.
                            // origin: PCA on wire
                            // u axis: along wire
                            // v axis: from origin to PCA on track
                            //TVector3 u = (wirePoint2 - wirePoint1).Unit();
			    //TVector3 v = (pcaFinal - pcaWire).Unit();



			    if(locHit1 && locHit2) {
				const HGeomRotation& transRotLaySysRSec =
				    fSizesCellsLayer.getRotLaySysRSec(cell).getRotMatrix();
				HGeomRotation transRotSecToRotLay = transRotLaySysRSec.inverse();
				HGeomVector transVecRotLayToSec =
				    fSizesCellsLayer.getRotLaySysRSec(cell).getTransVector();

				//Double_t Rxx = transRotSecToRotLay.getElement(0,0);
				//Double_t Rxy = transRotSecToRotLay.getElement(0,1);
				//Double_t Rxz = transRotSecToRotLay.getElement(0,2);
				Double_t Ryx = transRotSecToRotLay.getElement(1,0);
				Double_t Ryy = transRotSecToRotLay.getElement(1,1);
				Double_t Ryz = transRotSecToRotLay.getElement(1,2);
				Double_t Rzx = transRotSecToRotLay.getElement(2,0);
				Double_t Rzy = transRotSecToRotLay.getElement(2,1);
				Double_t Rzz = transRotSecToRotLay.getElement(2,2);

				Double_t tx  = transVecRotLayToSec.getX();
				Double_t ty  = transVecRotLayToSec.getY();
				Double_t tz  = transVecRotLayToSec.getZ();

				Double_t errx1 = locHit1->getErrX();
				Double_t erry1 = locHit1->getErrY();
				Double_t errz1 = 0.03;
				HGeomVector errLay1(errx1, erry1, errz1);
				HGeomVector errSec1 = (*fSizesCells)[loccal[0]][loccal[1]].getSecTrans()->getRotMatrix() * errLay1;
				errx1 = errSec1.getX();
				erry1 = errSec1.getY();
				errz1 = errSec1.getZ();

				Double_t errx2 = locHit2->getErrX();
				Double_t erry2 = locHit2->getErrY();
				Double_t errz2 = 0.03;
				HGeomVector errLay2(errx2, erry2, errz2);
				HGeomVector errSec2 = (*fSizesCells)[loccal[0]][loccal[1]].getSecTrans()->getRotMatrix() * errLay2;
				errx2 = errSec2.getX();
				erry2 = errSec2.getY();
				errz2 = errSec2.getZ();

				Double_t yWire = fSizesCellsCell.getWirePos();
                                // What could possibly go wrong?
				distErr = TMath::Sqrt(errx2*errx2*TMath::Power(-((( 2*Ryx*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))+2*Rzx*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+( Rzx*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))-Ryx*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.) +
						      erry2*erry2*TMath::Power(-((( 2*Ryy*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))+2*Rzy*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+( Rzy*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))-Ryy*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.) +
						      errz2*errz2*TMath::Power(-((( 2*Ryz*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))+2*Rzz*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+( Rzz*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))-Ryz*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.) +
						      errx1*errx1*TMath::Power(-(((-2*Ryx*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))-2*Rzx*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+(-Rzx*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))+Ryx*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))-Rzx*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+Ryx*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.) +
						      erry1*erry1*TMath::Power(-(((-2*Ryy*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))-2*Rzy*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+(-Rzy*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))+Ryy*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))-Rzy*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+Ryy*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.) +
						      errz1*errz1*TMath::Power(-(((-2*Ryz*(Ryx*(-x1+x2)+Ryy*(-y1-y2)+Ryz*(-z1-z2))-2*Rzz*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))*(-(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2))))/(2.*TMath::Power(TMath::Power((Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2)),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.),1.5)))+(-Rzz*(Ryx*(-tx+x1)+Ryy*(-ty+y1)-yWire+Ryz*(-tz+z1))+Ryz*(Rzx*(-tx+x1)+Rzy*(-ty+y1)+Rzz*(-tz+z1))-Rzz*(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2))+Ryz*(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2)))/TMath::Sqrt(TMath::Power(Ryx*(-x1+x2)+Ryy*(-y1+y2)+Ryz*(-z1+z2),2.)+TMath::Power(Rzx*(-x1+x2)+Rzy*(-y1+y2)+Rzz*(-z1+z2),2.)),2.));
			    } else {
				distErr = cal2->calcDistanceErr(loccal[0], loccal[1],
								alpha, t1)  + errTof;
			    }

			    const Int_t measDim = 1;
                            // Measurement vector is drift radius.
                            Double_t coord   [measDim];
			    coord[0] = mindist;

			    Double_t coordErr[measDim];
                            coordErr[0] = distErr;

			    Double_t yw, zw = 0.;
			    fSizesCellsLayer.getYZinWireSys(pcaFinal.x(), pcaFinal.y(), pcaFinal.z(),
							    loccal[3], yw, zw);
			    if(yw < 0) coord[0] *= -1.;


                            const HKalMdcMeasLayer *hitmeas =
                                pCradleHades->getMdcLayerAt(loccal[0],
                                                            loccal[1],
                                                            loccal[2]);

                            // Find intersection of segment points with
                            // the measurement layer.
                            TVector3 pos1(x1, y1, z1);
                            TVector3 pos2(x2, y2, z2);
                            TVector3 dir = pos2 - pos1;
                            TVector3 pointIntersect;
                            hitmeas->findIntersection(pointIntersect,
                                                      pos1, dir);

			    Double_t weight = 1. / nCell;
                            if(dist >= 0.) {
                                HKalMdcHit *newhit =
                                    new HKalMdcHit(&coord[0], &coordErr[0],
                                                   *hitmeas, measDim,
                                                   pointIntersect,
                                                   Kalman::kWireHit,
                                                   weight, loccal[3]);
                                newhit->setDriftTime(t1, timeSimErr);
                                //newhit->setImpactAngle(alpha);
                                newhit->setImpactAngle(cal1->getAngle1());
                                newhit->setTime1(cal1->getTime1());
                                newhit->setTime2(cal1->getTime2());
                                newhit->setTimeWireOffset(tWireOffset);
				newhit->setTimeTofCal1(cal1->getTof1());
                                allhits.Add(newhit);
                                nComp++;

                                // Get the time of flight correction by a
                                // weighted least squares estimation:
                                // take the mean difference of the measured
                                // time t1 and the expected drift time timeSim.
                                // These differences are calculated for each
                                // mdc module and weighted by the variances of
                                // the calculated drift time error.
                                Double_t timeSim   =
                                    cal2sim->calcTime(loccal[0], loccal[1],
                                                      alpha, mindist);
                                sumTof[loccal[1]] += (t1 - timeSim) /
                                    (timeSimErr*timeSimErr);
                                sumErr[loccal[1]] += 1. /
                                    (timeSimErr*timeSimErr); // Normalization of weights.
                            }
                        } // for loop over number cells in layer

                        // Weight needs to be corrected if not all cells in a
                        // layer have been added as hit.
                        if(nComp != nCell) {
                            // Competing hits from a measurement layer are
                            // weighted equally.
                            Double_t weight = 1. / (Double_t)nComp;
                            Int_t iLast = allhits.GetEntries() - 1;
                            for(Int_t i = iLast; i > iLast - nComp; i--) {
                                ((HKalMdcHit*)allhits.At(i))->setWeight(weight);
                            }
                        }
                    } // if nCell > 0
                } // end loop over layers
            } // end loop over inner/outer segments

            // Time of flight correction for each module.
            Double_t tTof[4];
            for(Int_t i = 0; i < 4; i++) {
                if(sumErr[i] != 0.) {
                    tTof[i] = sumTof[i] / sumErr[i];
                } else {
                    tTof[i] = 0.;
                }
            }

            // Correct drift time by Tof.
            for(Int_t i = 0; i < allhits.GetEntries(); i++) {
                HKalMdcHit *hit = (HKalMdcHit*)allhits.At(i);
                if(hit) {
                    hit->setDriftTime(TMath::Max(hit->getDriftTime() -
                                                 tTof[hit->getModule()], 0.),
                                      hit->getDriftTimeErr());
                    hit->setTimeTof(tTof[hit->getModule()]);
                } else {
                    cout<<"null pointer at index"<<i<<" of "<<allhits.GetEntries()<<endl;
                }
            }
            return mdcTrkCand;
        }
    } // loop through ideal candidates
    return mdcTrkCand;
}

//? remove
HMdcTrkCand* HKalInput::nextWireTrackNoComp(TObjArray &allhits, Int_t minLayInSeg, Int_t minLayOutSeg) {
    // Fills drift chamber wire hits with the drift time as measurement vector
    // to allhits array.
    // If there are two competing hits in an MDC layer, only the one with the
    // smaller drift time will be used.
    // Loops over HMdcTrkCandIdeal Category for the next four hits. The
    // iterator for the category has to be reset by hand at the begin
    // of each event. Return 0 if no next candidate has been found.
    //
    // output:
    // allhits: array with HKalMdcHit objects (Existing content of array will be deleted!)
    // input:
    // minLayInSeg:   minimum number of layers in the inner segment needed to fill allhits
    // minLayOutSeg:  minimum number of layers in the outer segment needed

    Int_t             indexSeg[2];   // index in catMdcSeg for inner/outer seg
    Int_t             mdcSegNlay[2];

    HMdcSeg     *mdcSeg[2];
    HMdcTrkCand *mdcTrkCand = 0;
    HMetaMatch2 *pMetaMatch = NULL;

    if(fSizesCells == NULL) {
        Error("nextWireTrack()","HMdcSizeCells is NULL!");
        exit(1);
    }
    if(pCradleHades == NULL) {
        Error("nextWireTrack()","DetectorCradle is NULL!");
        exit(1);
    }

    if(!iterMetaMatch) return NULL;

    HLocation loccal;
    loccal.set(4,0,0,0,0);

    HMdcCal2ParSim *cal2sim = (HMdcCal2ParSim*)gHades->getRuntimeDb()->getContainer("MdcCal2ParSim");
    HMdcCal2Par    *cal2    = (HMdcCal2Par   *)gHades->getRuntimeDb()->getContainer("MdcCal2Par");

    allhits.Clear();


    while ((pMetaMatch = (HMetaMatch2*)(iterMetaMatch->Next())) != 0) {
        mdcTrkCand = (HMdcTrkCand*)mdcTrkCandCat->getObject(pMetaMatch->getTrkCandInd());

        for(Int_t io = 0; io < 2; io ++) { // loop inner/outer segment
            mdcSegNlay[io] = 0;
            mdcSeg[io]     = 0;

            indexSeg   [io] =  mdcTrkCand->getSegInd(io);
            if(indexSeg[io] !=-1){
                mdcSeg[io] = (HMdcSeg*) mdcSegCat->getObject(indexSeg[io]);
                if(!mdcSeg[io]){ cout<<"zero segment!"; continue;}
                mdcSegNlay[io] = mdcSeg[io]->getNLayers();
            }
        }
        if(!mdcSeg[0] || !mdcSeg[1]) continue;

        // Skip if there were not enough hits in each segment.
        if(mdcSegNlay[0] >= minLayInSeg && mdcSegNlay[1] >= minLayOutSeg) {
#if kalDebug > 2
            cout<<"filling sec "<<(Int_t)mdcSeg[0]->getSec()<<endl;
#endif

            // Used for time of flight correction of drift time.
            Double_t sumTof [4]={ 0., 0., 0., 0. };
            Double_t sumErr [4]={ 0., 0., 0., 0. };

            for(Int_t io = 0; io < 2; io ++) {              // loop inner/outer segment

                HMdcSeg *seg = mdcSeg[io];

                for(Int_t l = 0; l < 12; l++) { // segment = 2 modules = 2x6 layers
                    // input selection
                    Int_t nCell = seg->getNCells(l); // number of cells per layer
                    loccal[0]   = seg->getSec();
                    Int_t ioseg = seg->getIOSeg(); // 0=inner,1=outer

                    if(nCell > 0) {
                        TObjArray hits;
                        Int_t    nComp = 0; // Number of competing hits in a layer.

                        // Search for the cell with the lowest drift time.
                        Int_t bestCell = -1;
                        Double_t bestTime = -1.;
                        for(Int_t i = 0; i < nCell; i++) {
                            if(ioseg == 0)  {
                                (l < 6) ? loccal[1] = 0 : loccal[1] = 1;
                            } else if(ioseg == 1) {
                                (l < 6) ? loccal[1] = 2 : loccal[1] = 3;
                            }
                            (l < 6) ? loccal[2] = l : loccal[2] = l - 6;

                            // input selection
                            Int_t cell = seg->getCell(l,i);  // get cell number
                            loccal[3] = cell;

                            HMdcCal1Sim *cal1 = (HMdcCal1Sim*)mdcCal1Cat->getObject(loccal);

                            if(cal1->getStatus1() < 0) continue; // Check if hit is valid.

                            if(bestTime < 0.) {
                                bestTime = cal1->getTime1();
                                bestCell = 1;
                            }

                            if(cal1->getTime1() < bestTime) {
                                bestTime = cal1->getTime1();
                                bestCell = i;
                            }
                        }

                        if(bestCell > 0) {

                            Int_t cell = seg->getCell(l,bestCell);  // get cell number
                            loccal[3] = cell;

                            HMdcCal1Sim *cal1 = (HMdcCal1Sim*)mdcCal1Cat->getObject(loccal);

                            if(cal1->getStatus1() < 0) continue; // Check if hit is valid.

                            Double_t x1, y1, z1, x2, y2, z2 = 0.;
                            calcSegPoints(seg, x1, y1, z1, x2, y2, z2);

                            HMdcSizesCellsLayer &fSizesCellsLayer = (*fSizesCells)[loccal[0]][loccal[1]][loccal[2]];
                            HMdcSizesCellsCell  &fSizesCellsCell  = (*fSizesCells)[loccal[0]][loccal[1]][loccal[2]][loccal[3]];

                            TVector3 layerHit;

                            Double_t alpha, mindist;
                            if(!fSizesCellsLayer.getImpact(x1, y1, z1, x2, y2, z2, loccal[3], alpha, mindist)) {
                                if(bPrint) {
                                    Warning("nextWireTrack()", Form("No impact found with cell %i. Measurement discarded.", loccal[3]));
                                }
                                continue;
                            }
                            // Speed of signal on wire in ns/mm
                            Float_t signalSpeed = ((HMdcDigitPar*)gHades->getRuntimeDb()->getContainer("MdcDigitPar"))->getSignalSpeed();

                            // Path length of signal along the wire in mm.
                            Double_t xWire      = fSizesCellsLayer.getSignPath(x1, y1, z1, x2, y2, z2, cell);
                            // Amount of time that the signal travels on the wire.
                            Double_t tWireOffset = xWire * signalSpeed;

                            // Offset from time of flight subtracted later.
                            Float_t t1  = cal1->getTime1() - tWireOffset;

                            Double_t timeSimErr = cal2sim->calcTimeErr(loccal[0], loccal[1], alpha, mindist);

                            Double_t dist       = cal2->calcDistance  (loccal[0], loccal[1], alpha, t1);
                            Double_t distErr    = cal2->calcDistanceErr(loccal[0], loccal[1], alpha, t1);

                            const Int_t measDim = 1;
                            // Measurement vector is drift radius.
                            Double_t coord   [measDim];
                            coord[0] = dist;
                            Double_t coordErr[measDim];
                            coordErr[0] = distErr;

                            HGeomVector &wire1 = fSizesCellsCell.getWirePnt1();
                            TVector3 wirePoint1(wire1.X(), wire1.Y(), wire1.Z());
                            HGeomVector &wire2 = fSizesCellsCell.getWirePnt2();
                            TVector3 wirePoint2(wire2.X(), wire2.Y(), wire2.Z());

                            Int_t iflag;
                            Double_t length;
                            TVector3 pcaFinal, pcaWire;

                            HKalGeomTools::Track2ToLine(pcaFinal, pcaWire, iflag, mindist, length, TVector3(x1,y1,z1), TVector3(x2,y2,z2), wirePoint1, wirePoint2);

                            // Make virtual plane.
                            // origin: PCA on wire
                            // u axis: along wire
                            // v axis: from origin to PCA on track
                            TVector3 u = (wirePoint2 - wirePoint1).Unit();
                            TVector3 v = (pcaFinal - pcaWire).Unit();

                            const HKalMdcMeasLayer *hitmeas = pCradleHades->getMdcLayerAt(loccal[0], loccal[1], loccal[2]);

                            Double_t weight = 1.;// / nCell;
                            if(dist >= 0.) {
                                HKalMdcHit *newhit = new HKalMdcHit(&coord[0], &coordErr[0], *hitmeas, measDim, pcaFinal, Kalman::kWireHit, weight, loccal[3]);
                                newhit->setDriftTime(t1, timeSimErr);
                                newhit->setImpactAngle(alpha);
                                newhit->setTime1(cal1->getTime1());
                                newhit->setTime2(cal1->getTime2());
                                newhit->setTimeWireOffset(tWireOffset);
                                newhit->setTimeTofCal1(cal1->getTof1());
                                allhits.Add(newhit);
                                nComp++;

                                // Get the time of flight correction by a weighted least squares estimation:
                                // take the mean difference of the measured time t1 and the expected drift
                                // time timeSim. These differences are calculated for each mdc module and weighted
                                // by the variances of the calculated drift time error.
                                Double_t timeSim   = cal2sim->calcTime   (loccal[0], loccal[1], alpha, mindist);
                                sumTof[loccal[1]] += (t1 - timeSim) / (timeSimErr*timeSimErr);
                                sumErr[loccal[1]] += 1. / (timeSimErr*timeSimErr); // Normalization of weights.
                            }
                        } // for loop over number cells in layer

                        // Weight needs to be corrected if not all cells in a layer have been added as hit.
                        if(nComp != nCell) {
                            // Competing hits from a measurement layer are weighted equally.
                            Double_t weight = 1. / (Double_t)nComp;
                            Int_t iLast = allhits.GetEntries() - 1;
                            for(Int_t i = iLast; i > iLast - nComp; i--) {
                                ((HKalMdcHit*)allhits.At(i))->setWeight(weight);
                            }
                        }
                    } // if nCell > 0
                } // end loop over layers
            } // end loop over inner/outer segments

            // Time of flight correction for each module.
            Double_t tTof[4];
            for(Int_t i = 0; i < 4; i++) {
                if(sumErr[i] != 0.) {
                    tTof[i] = sumTof[i] / sumErr[i];
                } else {
                    tTof[i] = 0.;
                }
            }

            // Correct drift time by Tof.
            for(Int_t i = 0; i < allhits.GetEntries(); i++) {
                HKalMdcHit *hit = (HKalMdcHit*)allhits.At(i);
                if(hit) {
                    hit->setDriftTime(TMath::Max(hit->getDriftTime() - tTof[hit->getModule()], 0.), hit->getDriftTimeErr());
                    hit->setTimeTof(tTof[hit->getModule()]);
                } else {
                    cout<<"null pointer at index"<<i<<" of "<<allhits.GetEntries()<<endl;
                }
            }
            return mdcTrkCand;
        }
    } // loop through ideal candidates
    return mdcTrkCand;
}

HGeantKine* HKalInput::getGeantKine(const HMdcTrkCand *cand) {
    if(!cand)          { Error("getGeantKine()","HMdcTrkCand == NULL!"); return NULL; }
    if(!mdcSegCat)     { Error("getGeantKine()","mdcSegCat == NULL!"  ); return NULL; }
    HMdcSegSim* mdcSeg = (HMdcSegSim*) mdcSegCat->getObject(cand->getSegInd(0));
    Int_t ind = mdcSeg->getTrack(0);
    if(ind < 0)        { Error("getGeantKine()","ind < 0!"                 ); return NULL; }
    if(!kineCat)       { Error("getGeantKine()","kineCat == NULL!"         ); return NULL; }

    return (HGeantKine* )kineCat->getObject(ind-1);
}

HGeantKine* HKalInput::getGeantKine(const HMdcTrkCandIdeal *cand) {
    if(!cand)          {Error("getGeantKine()","HMdcTrkCandIdeal == NULL!"); return NULL;}
    if(!mdcSegIdealCat){Error("getGeantKine()","mdcSegIdealCat == NULL!"  ); return NULL;}
    HMdcSegIdeal* mdcSeg = (HMdcSegIdeal*) mdcSegIdealCat->getObject(cand->getSegInd(0));
    Int_t ind = mdcSeg->getTrack(0);
    if(ind < 0)        {Error("getGeantKine()","ind < 0!"                 ); return NULL;}
    if(!kineCat)       {Error("getGeantKine()","kineCat == NULL!"         ); return NULL;}

    return (HGeantKine* )kineCat->getObject(ind-1);
}

void HKalInput::getGeantMdcRawpoints(TObjArray &allhits, HGeantKine *kine, Bool_t midplane) {
    // Return TObjArray with pointer to HGeantMdc objects for the track.
    // if midplane == kTRUE  fill only points for layer == 6.
    // if midplane == KFALSE fill only points for layer < 6.

    if(!kine)         {Error("getGeantMdcRawpoints()","HGeantKine == NULL!"); return ;}

    allhits.Clear();

    HGeantMdc* geantMdc;
    kine->resetMdcIter();
    while( (geantMdc = (HGeantMdc*)kine->nextMdcHit()) != 0) {
        if( midplane && geantMdc->getLayer() != 6) continue;  // only hits for modules
        if(!midplane && geantMdc->getLayer() == 6) continue;  // only hits for layers
        allhits.Add(geantMdc);
    }
}

void HKalInput::getPosAndDir(TVector3 &pos, TVector3 &dir, HGeantMdc *geantMdc, Bool_t sectorCoord) {
    // Return pos and dir from HGeantMdc.
    // if sectorCoord == kTRUE the points will be transformed
    // to sector coordinate system.

    if(!geantMdc) { Error("getPosAndDir()","HGeantMdc == NULL!"); return ;}

    Double_t point   [3];
    Double_t pointDir[3];

    Float_t x,y,tof,p,th,ph;
    geantMdc->getHit(x,y,tof,p);

    point[0] = x;
    point[1] = y;
    point[2] = 0;

    geantMdc->getIncidence(th,ph);
    Double_t theta = th * TMath::DegToRad();    // 0 -360 deg
    Double_t phi   = ph * TMath::DegToRad();    // 0 -180 deg
    Double_t sinTh = sin(theta);
    pointDir[0]    = sinTh * cos(phi)         * 1000; // just for precision * 1000
    pointDir[1]    = sinTh * sin(phi)         * 1000;
    pointDir[2]    = sqrt(1. - sinTh * sinTh) * 1000;

    pointDir[0]   += point[0];
    pointDir[1]   += point[1];
    pointDir[2]   += point[2];

    Int_t s = geantMdc->getSector();
    Int_t m = geantMdc->getModule();
    Int_t l = geantMdc->getLayer();

    if(sectorCoord){
        HGeomTransform trans;

        if(l == 6) { // midplane
            ((*fSizesCells)[s][m]).transFrom(point   [0],point   [1],point   [2]); // transform to sector coordinate system
            ((*fSizesCells)[s][m]).transFrom(pointDir[0],pointDir[1],pointDir[2]); // transform to sector coordinate system
        } else {
            if(mdcGetContainers->getSecTransGeantLayer(trans, s, m, l)) {
                HGeomVector layPoint(point[0], point[1], point[2]);
                HGeomVector transPoint = trans.transFrom(layPoint); // transform to sector coordinate system
                point[0] = transPoint.X();
                point[1] = transPoint.Y();
                point[2] = transPoint.Z();

                layPoint.setXYZ(pointDir[0], pointDir[1], pointDir[2]);
                transPoint = trans.transFrom(layPoint); // transform to sector coordinate system
                pointDir[0] = transPoint.X();
                pointDir[1] = transPoint.Y();
                pointDir[2] = transPoint.Z();
            } else {
                Error("getPosAndDir()", "Transformation matrix not found.");
                exit(1);
            }
        }
    }

    pos.SetX(point[0]);
    pos.SetY(point[1]);
    pos.SetZ(point[2]);

    dir.SetX(pointDir[0]);
    dir.SetY(pointDir[1]);
    dir.SetZ(pointDir[2]);

    dir -= pos;

    if(sectorCoord) {
        // Project Geant point on measurement layer.
	const HKalMdcMeasLayer *measLay = pCradleHades->getMdcLayerAt(s, m, l);
#if kalDebug > 3
	cout<<"Distance Geant point to plane: "<<measLay->distanceToPlane(pos)<<endl;
#endif
        TVector3 posNoShift = pos;
        measLay->findIntersection(pos, posNoShift, dir);
    }
}

void HKalInput::getMdcSegs(const HMdcTrkCand *cand, HMdcSeg **segs) {
    if(!cand)      {Error("getGeantKine()","HMdcTrkCand == NULL!"); return ;}
    if(!mdcSegCat) {Error("getGeantKine()","mdcSegCat == NULL!"  ); return ;}

    segs[0]     = (HMdcSeg*) mdcSegCat->getObject(cand->getSegInd(0));
    if(cand->getSegInd(1)!=-1){
        segs[1] = (HMdcSeg*) mdcSegCat->getObject(cand->getSegInd(1));
    } else {
        segs[1] = NULL;
    }
}

void HKalInput::getMdcSegs(const HMdcTrkCandIdeal *cand, HMdcSegIdeal **segs) {
    if(!cand)          {Error("getGeantKine()","HMdcTrkCand == NULL!");      return ;}
    if(!mdcSegIdealCat){Error("getGeantKine()","mdcSegIdealCat == NULL!"  ); return ;}

    segs[0]     = (HMdcSegIdeal*) mdcSegIdealCat->getObject(cand->getSegInd(0));
    if(cand->getSegInd(1)!=-1){
        segs[1] = (HMdcSegIdeal*) mdcSegIdealCat->getObject(cand->getSegInd(1));
    } else {
        segs[1] = NULL;
    }
}
