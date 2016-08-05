#include "hades.h"
#include "hrootsource.h"
#include "hevent.h"
#include "hcategory.h"
#include "hiterator.h"
#include "hreconstructor.h"
#include "hrecevent.h"
#include "htaskset.h"


#include "TString.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TTree.h"

#include <iostream>
using namespace std;



class HChangeSeqNumber : public HReconstructor
{
   Int_t ctEvt;  //!
public:

    HChangeSeqNumber(const Text_t *name = "",const Text_t *title ="") {  ctEvt=-1; }
    virtual ~HChangeSeqNumber()                                       {;}
    Bool_t init()                                                     { return kTRUE; }
    Bool_t finalize()                                                 { return kTRUE; }

    Int_t execute()
    {
        UInt_t seqNr = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber() ;
	if(ctEvt<0){
	    ctEvt = seqNr;
	    cout<<"first SeqNumber = "<<seqNr<<endl;
	}

        gHades->getCurrentEvent()->getHeader()->setEventSeqNumber(ctEvt);

	ctEvt++;

	return 0;
    }

    //ClassDef(HChangeSeqNumber,0)
};



Int_t makeLongFiles(Int_t nEvents   = 3000,
		   TString inFile  = "/hera/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/Au_Au_1230MeV_1000evts_1_1.root,/hera/hades/dstsim/apr12/hgeant/bmax10/new/deltaelectron/Au_Au_1230MeV_1000evts_2_1.root",
		   TString outFile =  "adddelta_12.root" )
{
    Hades* myHades = new Hades;
    gHades->makeCounter(1000);
    gHades->setOutputSizeLimit(100000000000);// 100Gb

    TTree::SetMaxTreeSize(200000000000); // set >fOutputSizeLimit to avoid conflict

    HRootSource* source = new HRootSource();

    TObjArray* arfiles = inFile.Tokenize(",");
    Int_t ctfile=0;
    if(arfiles){
	for(Int_t i=0;i<arfiles->GetEntries();i++){
	    TString file = ((TObjString*)(arfiles->At(i)))->GetString();
	    cout<<i<<" file : "<<file.Data()<<endl;
	    source->addFile((Text_t*)file.Data());
            ctfile++;
	}
	arfiles->Delete();
        delete arfiles;
    }
    //nEvents=ctfile*1000;
    cout<<"run "<<nEvents<<" events from n = "<<ctfile<< " files " <<endl;
    myHades->setDataSource(source);

    HTaskSet *masterTaskSet = gHades->getTaskSet("all");
    masterTaskSet->add(new HChangeSeqNumber("seq_change","seq_change"));


    if(!myHades->init()){
	cout<<"Hades Init() failed!"<<endl;
	exit(1);
    }

    gHades->setOutputFile((Text_t*)outFile.Data(),"RECREATE","Test",2);
    gHades->makeTree();

    gHades->eventLoop(nEvents);

    delete myHades;
    return 0;
}
