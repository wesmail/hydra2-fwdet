

// from ROOT
#include "TObjString.h"
#include "TString.h"
#include "TSystem.h"
#include "TTree.h"
#include "TVector3.h"

// from hydra

#include "showerdef.h"
#include "hstartdef.h"
#include "tofdef.h"
#include "walldef.h"
#include "rpcdef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"
#include "hmdctrackgfieldpar.h"
#include "hparticledef.h"
#include "hphysicsconstants.h"

#include "hkaldef.h"
#include "createHades.C"

#include <iostream>
using namespace std;

void exKal() {

    // Number of events to analyze.
    const Int_t nEvents          = 100;

    const Int_t refID            = 11200;

    // Magnetic field factor.
    Double_t    scaleFactor      = 0.9235;

    // Print a message after each counterstep events.
    Int_t       counterstep      = 100;

    // Parameter source.
    TString     paramSource      = "ASCII, ORACLE";

    // Oracle date.
    TString     oraDate          = "31-DEC-2011";

    // Input directory and files.
    TString     inputDir         = "/hera/hades/user/kempter/kalman/new/geant/bmax4/";

    // each file contains 10000 events
    const Int_t nEventsPerFile   = 10000;
    const Int_t nFiles           =  TMath::Max(1, 2 * nEvents / nEventsPerFile);
    TObjArray   inputFiles(nFiles);
    // index of input file array
    Int_t       iArr  = 0;
    // file name index
    Int_t       iFile = 1;

    // Loop through files. Stop when array is full or all files in the directory are added.
    while((iArr < nFiles) && (iFile <= 10000)) {
        // Input files.
        TObjString *file1 = new TObjString(Form("Au_Au_1250MeV_10000evts_ep_%i_1.root", iFile));
	TObjString *file2 = new TObjString(Form("Au_Au_1250MeV_10000evts_ep_%i_2.root", iFile));
        // Check if files are really there.
        if(gSystem->AccessPathName(gSystem->ConcatFileName(inputDir.Data(), file1->GetString().Data())) ||
          gSystem->AccessPathName(gSystem->ConcatFileName(inputDir.Data(), file2->GetString().Data()))) {
            cout<<"could not find file "<<iFile<<endl;
            delete file1;
            delete file2;
        } else {
            inputFiles.Add(file1);
            inputFiles.Add(file2);
	    iArr = iArr + 2;
	}
        iFile++;
    }

    // Output directory and filename.
    TString     outDir           = "/lustre/hades/user/ekrebs/kalman/auau_1250_bmax4";

    TString     outFile          = "kalman.root";

    // For AuAu tracking: HMdcTrackDSet::setDefParAu15Au()
    Bool_t      bUseAuAuPars     = kTRUE;

    Bool_t      bIdealTracking   = kFALSE;


    // Options for the Kalman filter and DAF.
    // --------------------------------------

    // Position error in x and y in mm
    // default 0.2mm in x and 0.1 in y direction.
    Double_t    errX             = 0.2;

    Double_t    errY             = 0.1;

    // Momentum estimation error as fraction. For example, 0.05 means
    // that the estimation is within 5% of the true value.
    Double_t    errMom           = 0.05;

    // kIterForward:  Propagate track from MDCI to MDCIV (default).
    // kIterBackward: Propagate track backwards from MDCIV to MDCI.
    Bool_t      dir              = kIterForward;

    // Number of Kalman filter iterations. (default: 1)
    // Raise this number if you wish to wait longer.
    Int_t       nIter            = 1;

    // kVarRot: Rotate coordinate system in the direction of initial
    // track estimation (default).
    // kNoRot:  Do not perform the above transformation.
    Kalman::kalRotateOptions rot = kVarRot;

    // kTRUE: work with wire hits.
    // kFALSE: work with segment hits (default).
    Bool_t      bFitWireHits     = kTRUE;

    // Turn on the annealing filter. Only works with wire hits.
    Bool_t      bDoDaf           = kTRUE;

    // Allow for competing wire hits in an MDC layer.
    Bool_t      bCompHits        = kTRUE;

    // Turn on/off energy loss and multiple scattering.
    Bool_t      bDoEnerLoss      = kTRUE;
    Bool_t      bDoMultScat      = kTRUE;

    // Reconstruct only particles with certain Geant pid's.
    // Leave this array empty to reconstruct all particles.
    const Int_t nPids            = 0;
    Int_t       pid[nPids];// = {14};
    filtMethod  filtType         = Kalman::kKalConv;

    // Parameters for the annealing filter.
    // Number of DAF iterations.
    const Int_t nDafs            = 5;
    // Annealing factors (temperatures).
    // Default is 81,9,4,1,1.
    Double_t    dafT[nDafs]      = { 81., 9., 4., 1., 1. };
    // Cut-off parameter (default is 4).
    Double_t    dafChi2Cut       = 4.;

    // Verbose level.
    // 0: no messages, 1: print error messages, >1: print errors and warnings.
    Int_t       verb             = 0;


    //----------------------------------------------------------
    // CREATE HADES
    if(!createHades(inputDir, inputFiles, refID, paramSource, oraDate,
                    errX, errY, bUseAuAuPars, bIdealTracking,
                    bFitWireHits, bDoDaf, bCompHits)) {
	Error("", "Failed to create Hades!.");
        exit(1);
    }
    //----------------------------------------------------------------------------//

    //----------------------------------------------------------
    // Persistent categories

    Cat_t notPersistentCat[] = {
	catRich, catMdc, catShower, catTof, catTracks,
	catRichRaw, catRichDirClus, catRichHitHdr, catRichHit, catRichCal,
	catMdcRaw, //catMdcCal1, catMdcCal2, catMdcRawEventHeader, catMdcTrkCand, catMdcHit, catMdcSeg,
	catShowerCal, catShowerPID, catShowerHitHdr, catShowerRaw, catShowerHit,
	catTofRaw, catTofHit, catTofCluster,
	catRpcRaw, catRpcHit, catRpcCluster,
	//catRKTrackB, catSplineTrack, catMetaMatch,
	//catParticleCandidate, catParticleEvtInfo,
	catWallRaw, catWallOneHit, catWallCal,
	catStart2Raw, catStart2Cal, catStart2Hit,
	 catTBoxChan
    };

    for(UInt_t i = 0; i < sizeof(notPersistentCat) / sizeof(Cat_t); i++){
        HCategory *cat = ((HCategory *)gHades->getCurrentEvent()->getCategory(notPersistentCat[i]));
        if(cat) cat->setPersistency(kFALSE);
    }
    //----------------------------------------------------------------------------//

    //----------------------------------------------------------
    // Set up Kalman filter.

    HKalFilterTask* kalFilter = (HKalFilterTask*) gHades->getTask("KalmanFilterTask");
    if(!kalFilter) {
        cout<<"HKalFilterTask not found."<<endl;
        exit(1);
    }
    // Change default parameters. This must be done after
    kalFilter->setCounterStep(counterstep);
    kalFilter->setDafPars(dafChi2Cut, &dafT[0], nDafs);
    kalFilter->setDirection(dir);
    kalFilter->setDoPid(&pid[0], nPids);
    kalFilter->setDoEnerLoss(bDoEnerLoss);
    kalFilter->setDoMultScat(bDoMultScat);
    kalFilter->setErrors(errX, errY, TMath::Tan(0.5*TMath::DegToRad()), TMath::Tan(0.5*TMath::DegToRad()), errMom);
    kalFilter->setFillSites(kTRUE);
    kalFilter->setFilterMethod(filtType);
    kalFilter->setNumIterations(nIter);
    kalFilter->setRotation(rot);
    kalFilter->setVerbose(verb);

    //----------------------------------------------------------------------------//

    HRuntimeDb *rtdb = gHades->getRuntimeDb();
    HMdcTrackGFieldPar *pField  = (HMdcTrackGFieldPar*)(rtdb->getContainer("MdcTrackGFieldPar"));
    kalFilter->setFieldMap(pField->getPointer(), scaleFactor);

    if(gSystem->AccessPathName(outDir.Data())) {
        Error("createHades()", "Could not access output directory.");
        exit(1);
    }


    gHades->setOutputFile((Text_t*)gSystem->ConcatFileName(outDir.Data(), outFile.Data()), "RECREATE", "Test", 2);
    gHades->makeTree();

    gHades->eventLoop(nEvents);

    delete gHades;
}

