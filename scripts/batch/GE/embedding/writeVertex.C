
#include "hades.h"
#include "hrecevent.h"
#include "hcategory.h"
#include "hcategorymanager.h"

#include "htaskset.h"
#include "hloop.h"
#include "htool.h"
#include "htime.h"
#include "hmdcvertexwriter.h"

#include "hstart2hit.h"
#include "hparticleevtinfo.h"

#include "hparticledef.h"
#include "hstartdef.h"

#include <fstream>

using namespace std;



/*
Bool_t eventSelection(TObjArray*)
{
    // example
    HRecEvent* ev= (HRecEvent*)gHades->getCurrentEvent();

    Bool_t  verZ      = kFALSE;
    Bool_t  trigPT3   = kFALSE;
    Bool_t  start     = kFALSE;
    Bool_t  startFlag = kFALSE;
    Bool_t  noMonster = kFALSE;
    Bool_t  goodTiming= kFALSE;

    if(ev)
    {
	HEventHeader* header = ev->getHeader();

	if(header){
	    if(header->getEventSeqNumber()<4) return kTRUE;
	    HVertex&  vertex =  header->getVertexReco();
	    Float_t vertexZ  = vertex.getZ();

	    if(vertexZ > -60 && vertexZ <0) verZ = kTRUE;

	    // trigger condition
	    trigPT3 = header->isTBit(13);
	}

	// start detector condition
	HStart2Hit* starthit = 0;
	starthit =  HCategoryManager::getObject(starthit,catStart2Hit,0,kTRUE);
	if(starthit) start = kTRUE;    //object not available (3%)
	if(starthit){
	    if(starthit->getCorrFlag() > -1) startFlag = kTRUE;
	}

	HParticleEvtInfo* info=0;
	info = HCategoryManager::getObject(info,catParticleEvtInfo,0,kTRUE);
	if(info){
	    Int_t nRecoCand = info->getSumSelectedParticleCandMult();
	    if(nRecoCand>0) {
		if(info->getSumMdcSegFittedMult(1)/Float_t(nRecoCand)<=2.5) noMonster = kTRUE;
	    }
            if( ((info->getSumTofMult()+info->getSumRpcMult())-(info->getSumTofMultCut()+info->getSumRpcMultCut()))<30) goodTiming=kTRUE;
	}



    }
    // final selection

    if(verZ && startFlag && trigPT3 && noMonster && goodTiming)
    {
	return kTRUE;
    }

    return kFALSE;
}
*/
Bool_t eventSelection(TObjArray*)
{

    //-------------------------------------------------
    // summary event info object
    HCategory* evtInfoCat = (HCategory*)HCategoryManager::getCategory(catParticleEvtInfo);
    if(evtInfoCat){
	HParticleEvtInfo* evtInfo=0;
	evtInfo = HCategoryManager::getObject(evtInfo,evtInfoCat,0 );

	if(evtInfo&&evtInfo->isGoodEvent(Particle::kGoodTRIGGER|          // standard event selection apr12
					 Particle::kGoodVertexClust|
					 Particle::kGoodVertexCand|
					 Particle::kGoodSTART|
					 Particle::kNoPileUpSTART|
					 Particle::kNoVETO|
					 Particle::kGoodSTARTVETO|
					 Particle::kGoodSTARTMETA
					 )) return kTRUE;
	//-------------------------------------------------
    }
    return kFALSE;
}


Int_t writeVertex(TString infile,TString outfile,Int_t nEvents)
{
    //  infileList : comma seprated file list "file1.root,file2.root"
    //  outfile    : optional (not used here) , used to store hists in root file
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    HLoop loop(kTRUE);

    Bool_t ret = loop.addMultFiles(infile);
    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfile : "<<infile.Data()<<endl;
	return 1;
    }

    if(!loop.setInput("-*,+HStart2Hit,+HParticleEvtInfo")) {
    //if(!loop.setInput("")) {
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    } // read all categories
    loop.printCategories();
    loop.printChain();


    Int_t entries = loop.getEntries();
    if(nEvents < entries && nEvents >= 0 ) entries = nEvents;


    HTaskSet *masterTaskSet = gHades->getTaskSet("all");

    HMdcVertexWriter* vertexwriter = new HMdcVertexWriter("HMdcVertexWriter","HMdcVertexWriter");
    vertexwriter->setUserEventSelection(eventSelection);
    //vertexwriter->setVertexType(5); // HGeant (switch geant info on in input and remove eventselection)
    //vertexwriter->setVertexType(3); // particleCandVertex
    vertexwriter->setVertexType(4,0.138795,0.665113); // z from ClusterVertex:  apr12 day108 Mean X = 0.138795,Mean Y = 0.665113

    vertexwriter->setOutputFile(outfile);

    masterTaskSet->add(vertexwriter);

    for (Int_t i=0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached

	if(i%1000 == 0) cout<<"event "<<i<<endl;


    }

    //------------------------------------------------------------

    if(gHades)gHades->finalizeTasks();
    delete gHades;
    return 0;

}
