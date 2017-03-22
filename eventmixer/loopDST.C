#include "hades.h"
#include "hloop.h"
#include "htime.h"
#include "hcategory.h"
#include "hcategorymanager.h"
#include "hparticlecand.h"
#include "hparticlepair.h"
#include "hgeantkine.h"
#include "hparticletracksorter.h"
#include "hparticlebooker.h"
#include "hparticletool.h"
#include "hparticledef.h"
#include "hgeantdef.h"
#include "hparticleevtinfo.h"
#include "hgeantkine.h"
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


#include "heventmixer.h"
#include "eventclassifier.h"


Int_t loopDST(
	      TString infileList="/lustre/nyx/hades/dst/apr12/gen8/108/root/be1210818532703.hld_dst_apr12.root",
	      TString outfile="test.root",Int_t nEvents=20000)
{
    //  infileList : comma seprated file list "file1.root,file2.root" or "something*.root"
    //  outfile    : optional (not used here) , used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    Bool_t isSimulation = kFALSE;

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
	ret = loop.addMultFiles(infileList);      // file1,file2,file3
    } else{
	ret = loop.addFiles(infileList); // myroot*.root
    }

    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfiles : "<<infileList.Data()<<endl;
	return 1;
    }

    //if(!loop.setInput("")) {   // all input categories
    if(!loop.setInput("-*,+HParticleCand,+HParticleEvtInfo,+HGeantKine")) {
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
    TH1F* hmass = new TH1F("hmass","hmass",1000,0,2000);

    //#################################################
    //#################################################


    //-------------------------------------------------
    //parameters
    HEnergyLossCorrPar dEdxCorr;
    dEdxCorr.setDefaultPar("apr12");
    //-------------------------------------------------

    HParticleEventMixer eventmixer;
    eventmixer.setUseLeptons(kTRUE);
    eventmixer.setPIDs(9,14,18);  // pi-,p,lambda  which PIDs and MotherID are stored
    eventmixer.setBuffSize(10);   // size of buffer has to chosen according to stat
    eventmixer.setEventClassifier(eventClassifierMult);



    //-------------------------------------------------
    // input data
    HCategory* candCat    = (HCategory*)HCategoryManager::getCategory(catParticleCand);
    HCategory* evtInfoCat = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    HCategory* kineCat    = (HCategory*)HCategoryManager::getCategory(catGeantKine);
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

	if(loop.isNewFile(filename)){
	   if(!isSimulation) filename = HTime::stripFileName(filename,kTRUE,kFALSE);
	}

	//-------------------------------------------------
        // summary event info object
	HParticleEvtInfo* evtInfo=0;
        evtInfo = HCategoryManager::getObject(evtInfo,evtInfoCat,0 );

	if(evtInfo&&!evtInfo->isGoodEvent(Particle::kGoodTRIGGER|
					  Particle::kGoodVertexClust|
					  Particle::kGoodVertexCand|
					  Particle::kGoodSTART|
					  Particle::kNoPileUpSTART|
					  Particle::kNoVETO|
					  Particle::kGoodSTARTVETO|
					  Particle::kGoodSTARTMETA
					 )) continue;
	//-------------------------------------------------


	vector<HParticleCand *> vpim;
	vector<HParticleCand *> vp;
        HParticleCand* cand1;
	for(Int_t j = 0; j < candCat->getEntries(); j ++){
	    cand1 = HCategoryManager::getObject(cand1,candCat,j);
	    if (cand1->getPID()==9) {
		vpim.push_back(new HParticleCand(*cand1));
	    }
	    if (cand1->getPID() == 14) {
		vp.push_back(new HParticleCand(*cand1));
	    }
	}
	eventmixer.nextEvent();
	eventmixer.addVector(vpim,9);
	eventmixer.addVector(vp  ,14);
	vector<HParticlePair>& pairsVec = eventmixer.getMixedVector();

	for(UInt_t j = 0;j < pairsVec.size(); j++){
	    HParticlePair& pair = pairsVec[j];
            hmass->Fill(pair.M());
	}

	eventmixer.RemoveObjectsToDelete();
	//-------------------------------------------------


    } // end eventloop

    //#################################################
    //#################################################
    // write your histograms here
    out->cd();

    hmass->Write();



    out->Save();
    out->Close();
    //#################################################
    //#################################################

    delete gHades;
    return 0;
}
