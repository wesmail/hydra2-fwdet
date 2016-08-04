#include "hades.h"
#include "hloop.h"
#include "htime.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hparticlecand.h"
#include "hparticletracksorter.h"
#include "hparticlebooker.h"
#include "hparticletool.h"
#include "hparticledef.h"
#include "hparticleevtinfo.h"
#include "henergylosscorrpar.h"
#include "hphysicsconstants.h"

#include "TString.h"
#include "TROOT.h"
#include "TSystem.h"
#include "TFile.h"
#include "TH1F.h"


#include <iostream>
#include <vector>
using namespace std;

Int_t loopDST(
	      TString infileList="/hera/hades/dst/apr12/gen8/108/root/be1210819405908.hld_dst_apr12.root",
	      TString outfile="test.root",Int_t nEvents=1000)
{
    //  infileList : comma seprated file list "file1.root,file2.root" or "something*.root"
    //  outfile    : optional (not used here) , used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    Bool_t isSimulation = kFALSE;   // sim or real data

    //-------------------------------------------------
    // create loop obejct and hades
    HLoop loop(kTRUE);
    //-------------------------------------------------

    //-------------------------------------------------
    // list of all files with working sectors
    if(!isSimulation) loop.readSectorFileList("/hera/hades/dst/apr12/gen8/sector_selection/FileListHadron.list",kFALSE,kFALSE);
    //-------------------------------------------------


    //-------------------------------------------------
    // reading input files and decalring containers
     Bool_t ret =kFALSE;
    if(infileList.Contains(",")){
	ret = loop.addMultFiles(infileList);  // file1,file2,file3
    } else{
	ret = loop.addFiles(infileList);      // myroot*.root
    }

    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfiles : "<<infileList.Data()<<endl;
	return 1;
    }

    //if(!loop.setInput("")) {   // all input categories
    if(!loop.setInput("-*,+HParticleCand,+HParticleEvtInfo")) {  // select some Categories
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    } // read all categories
    loop.printCategories();
    loop.printChain();
    //-------------------------------------------------

    //#################################################
    //#################################################
    // add your histograms here
    TFile* out = new TFile(outfile.Data(),"RECREATE");
    out->cd();


    //#################################################
    //#################################################


    //-------------------------------------------------
    //parameters
    HEnergyLossCorrPar dEdxCorr;
    dEdxCorr.setDefaultPar("apr12");
    //-------------------------------------------------


    //-------------------------------------------------
    // input data
    HCategory* candCat    = (HCategory*)HCategoryManager::getCategory(catParticleCand);
    HCategory* evtInfoCat = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    //-------------------------------------------------


    //-------------------------------------------------
    // event loop starts here
    Int_t entries = loop.getEntries();
    if(nEvents < entries && nEvents >= 0 ) entries = nEvents;
    TString filename;
    Int_t sectors [6];

    for (Int_t i = 0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
        if(i%1000 == 0) cout<<"event "<<i<<endl;

	loop.getSectors(sectors); // fill sector array

	if(loop.isNewFile(filename)){ // if the actual filename is needed
	   if(!isSimulation) filename = HTime::stripFileName(filename,kTRUE,kFALSE); // get hld file name
	}

	//-------------------------------------------------
        // summary event info object
	HParticleEvtInfo* evtInfo=0;
        evtInfo = HCategoryManager::getObject(evtInfo,evtInfoCat,0 );

	if(evtInfo&&!evtInfo->isGoodEvent(Particle::kGoodTRIGGER|          // standard event selection apr12
					  Particle::kGoodVertexClust|
					  Particle::kGoodVertexCand|
					  Particle::kGoodSTART|
					  Particle::kNoPileUpSTART|
					  Particle::kNoVETO|
					  Particle::kGoodSTARTVETO|
					  Particle::kGoodSTARTMETA
					 )) continue;
	//-------------------------------------------------


	//-------------------------------------------------
        // loop over particle candidates in event
	if(candCat){

	    Int_t size = candCat->getEntries();
	    HParticleCand* cand=0;
	    for(Int_t j = 0; j < size; j++)
	    {
		cand = HCategoryManager::getObject(cand,candCat,j);
		if(cand) {

		    if(!loop.goodSector(cand->getSector())) { continue;}  // skipp inactive sectors
		    if(!cand->isFlagBit(kIsUsed)) { continue; }           // skip particles rejected by HParticleTrackSorter
		    if(cand->getPID()<0)          { continue; }           // skip particle without asigned PID (HParticleT0Reco)


                    //-------------------------------------------------
		    // here we get only candidates which have PID
		    Float_t pcorr = cand->getCorrectedMomentumPID(cand->getPID());          // retrieve corrected mom
		    cand->setMomentum(pcorr);                                               // write it back
		    cand->calc4vectorProperties(HPhysicsConstants::mass(cand->getPID()));   // sync with lorentz vector
		    //#################################################
		    //#################################################
		    // fill your histograms here


		    //#################################################
		    //#################################################




		     //-------------------------------------------------

		}
	    } // end cand loop
	} // end cand cat
	//-------------------------------------------------

    } // end eventloop

    //#################################################
    //#################################################
    // write your histograms here
    out->cd();





    out->Save();
    out->Close();
    //#################################################
    //#################################################

    delete gHades;
    return 0;
}
