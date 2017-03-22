
#include "hades.h"
#include "hrecevent.h"
#include "hcategory.h"
#include "hcategorymanager.h"

#include "hloop.h"

#include "hgeantdef.h"
#include "hgeantkine.h"


#include "TFile.h"
#include "TNtuple.h"
#include "TSystem.h"

#include <iostream>
using namespace std;




Int_t writeVertex(TString infile,TString outdir,Int_t nEvents)
{
    //  This program extracts the event vertex from the first primary
    //  kine particle to an TNtuple "vertex", ("vX:vY:vZ:seqNumber")
    //  which can be used to produce pluto a particles at the extract
    //  vertex allowing later an overlay of several hgeant root files
    //
    //  infile     : single HGeant root file including path
    //  outdir     : outfile (like infile_vertex.root) will be stored in this dir
    //  nEvents    : number of events to processed. if  nEvents < entries or < 0 the chain will be processed

    //--------------------------------------------------------------
    // setting up the input files
    HLoop loop(kTRUE);

    Bool_t ret = loop.addFile(infile);
    if(ret == 0) {
	cout<<"READBACK: ERROR : cannot find inputfile : "<<infile.Data()<<endl;
	return 1;
    }

    if(!loop.setInput("-*,+HGeantKine")) {
	cout<<"READBACK: ERROR : cannot read input !"<<endl;
	exit(1);
    } // read all categories
    loop.printCategories();
    loop.printChain();


    Int_t entries = loop.getEntries();
    if(nEvents < entries && nEvents >= 0 ) entries = nEvents;

    HCategory* kineCat    = (HCategory*)HCategoryManager::getCategory(catGeantKine);
    if(!kineCat){
	cout<<"ERROR: Can not retrieve catGeantKine!"<<endl;
        return 1;
    }
    //--------------------------------------------------------------


    //--------------------------------------------------------------
    // create outputfile
    TString file = gSystem->BaseName(infile.Data());
    file.ReplaceAll(".root","");
    file = Form("%s/%s_vertex.root",outdir.Data(),file.Data());
    file.ReplaceAll("//","/");

    TFile* out = new TFile(file.Data(),"RECREATE");
    if(!out){
	cout<<"ERROR: Can not create outputfile = "<<file<<endl;
        return 1;
    }
    TNtuple* vertex = new TNtuple("vertex","vertex","vX:vY:vZ:seqNumber");
    //--------------------------------------------------------------

    //--------------------------------------------------------------
    // retrive the vertex form first primary kine particle
    HGeantKine* kine;
    Float_t vx,vy,vz;

    for (Int_t i=0; i < entries; i++) {
	Int_t nbytes =  loop.nextEvent(i);             // get next event. categories will be cleared before
	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached

	if(i%1000 == 0) cout<<"event "<<i<<endl;

	HEvent* event = gHades->getCurrentEvent();
        if(!event) continue;
	HEventHeader* header = 0;
	if(event){
	    header = event ->getHeader();
            if(!header) continue;
	}
        Int_t n = kineCat->getEntries();
	for(Int_t j = 0; j < n; j ++){
	    kine = HCategoryManager::getObject(kine,kineCat,j);
	    if(kine) {
		if(kine->isPrimary())
		{
                    kine->getVertex(vx,vy,vz);
		    vertex->Fill(vx,vy,vz,header->getEventSeqNumber());
                    break;
		}
	    }
	}
    }
    //------------------------------------------------------------


    //--------------------------------------------------------------
    // store output
    out->cd();
    vertex->Write();
    out->Save();
    out->Close();
    //--------------------------------------------------------------

    delete gHades;
    return 0;

}
