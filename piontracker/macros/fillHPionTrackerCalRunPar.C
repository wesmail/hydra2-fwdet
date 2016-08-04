#include "hades.h"
#include "hruntimedb.h"
#include "hparora2io.h"
#include "hparasciifileio.h"
#include "hparrootfileio.h"
#include "hora2info.h"
#include "hruninfo.h"
#include "hpiontrackercalrunpar.h"
#include "hdst.h"

#include "TString.h"
#include "TCollection.h"
#include "TArrayI.h"

#include <iostream>
#include <fstream>
using namespace std;


void fillHPionTrackerCalRunPar()
{
    // .x fillHPionTrackerCalRunPar.C+   with hydra-dev
    //
    // Creates 1 param file for HPionTrackerCalRunPar containing
    // all runIDs of jul14 run. In the case of
    // RootFile IO the mapping of all RunIDs to the
    // container version is done. The macro contains
    // the options to read back the created files via
    // ASCII and ROOT IO for test.


    //-------------------------------------------------
    // SETUP
    Bool_t create = kTRUE;   // =kTRUE to fill container from scratch
                              // =kFALSE if init from ORA,ASCI,ROOT






    Bool_t makeDummy  = kFALSE; // dummy input will be created, kFALSE should be used with o own inpur file
    TString inputAscii="dummy_list.txt";


    TString rin ="test_piontrackerpar.root";   // root input : readback container for test
    TString ain ="test_piontrackerpar.txt";    // ascii input : readback container for test


    TString aout="test_piontrackerpar2.txt";   // ascii output
    TString rout="test_piontrackerpar2.root";  // root output

    Int_t input  =  0 ; // 0=ascii, 1=root    -1 = none
    Int_t output =  0 ; // 0=ascii, 1=root    -1 = none

    Int_t nParsPerRun =1;  // number of cal values to store for each run

    if(create) input = -1;

    if((input==0 && output == 0 && aout == ain) ||
       (input==1 && output == 1 && rout == rin)
      ) {
	cout<<"input and output files are the same!"<<endl;
	return;
    }
    //-------------------------------------------------


    //---------------------------------------------------
    // hades stuff to retrieve the runid by hld file name
    Hades* myhades      = new Hades;
    HRuntimeDb* rtdb    = gHades->getRuntimeDb();
    HDst::setupSpectrometer    ("jul14",0,"rich,mdc,tof,rpc,shower,wall,tbox,start,piontracker");  // piontrackerdetector is needed


    //---------------------------------------------------
    // get run infos via ORACLE
    HParOra2Io* ora     = new HParOra2Io;
    ora ->open();
    rtdb->setSecondInput(ora);

    HOra2Info* info = ora->getOra2Info();
    TList* l = info->getListOfRuns("jul14"); // all runs of the period, range can be set if needed
    //---------------------------------------------------







    //---------------------------------------------------
    // param input/output
    if(output == 0){
	HParAsciiFileIo* output=new HParAsciiFileIo;
	output->open(aout.Data(),"out");
	rtdb->setOutput(output);
    }

    if(output == 1){
	HParRootFileIo* output=new HParRootFileIo;
	output->open(rout.Data(),"RECREATE");
	rtdb->setOutput(output);
    }

    if(!create && input == 0){
	HParAsciiFileIo* ainp=new HParAsciiFileIo;
	ainp->open(ain.Data(),"in");
	rtdb->setFirstInput(ainp);
    }

    if(!create && input == 1){
	HParRootFileIo* rinp=new HParRootFileIo;
	rinp->open(rin.Data(),"read");
	rtdb->setFirstInput(rinp);
    }
    //---------------------------------------------------

    //---------------------------------------------------

    HPionTrackerCalRunPar* p = (HPionTrackerCalRunPar*)rtdb->getContainer("PionTrackerCalRunPar");
    HPionTrackerCalRunPars c(nParsPerRun); // helper object to fill the corrections


    // first fill all runs of beam time
    // with default values
    TArrayI aRuns(l->GetSize());
    Int_t ct = 0;
    TIter next(l);
    HRunInfo* rinfo;

    ofstream*  dummy=0;

    if(makeDummy ){
	dummy = new ofstream("dummy_list.txt");
    }
    while( (rinfo = (HRunInfo*) next() ) )
    {
	cout<<rinfo->GetName()<<" runID "<<rinfo->getRunId()<<" nevts "<<rinfo->getNumEvents()<<endl;
	c.clear(); // all params = 0

	if(create) p->addRun(rinfo->getRunId(),&c);
	aRuns.SetAt(rinfo->getRunId(),ct);
	ct++;
        TString daqname;
	info->getDaqFilename(rinfo->getRunId(),daqname);
        if(makeDummy) (*dummy)<<daqname <<" "<<1000.+ct<<endl;

    }
    if(create)cout<<"NRuns after dummy fill "<<p->getNRunIDs()<<endl;
    //----------------------------------------------------

    if(makeDummy && dummy)dummy->close();


    if(create)
    {
        cout<<"CREATE"<<endl;
	//----------------------------------------------------
	// Now fill analyzed runs
	ifstream input;
	input.open(inputAscii.Data());
	// 1 hld file (here evtbuild 1, without path) per line , format like :
	// filename  val1 .... nParsPerRun



	TString name;
	while(!input.eof())
	{
            Double_t* dat = c.data();
	    input>>name;

	    for(UInt_t i = 0; i< (UInt_t)c.getNVals(); i++){
		input>>dat[i];
	    }
	    if(name == "")          continue;
	    if(name.BeginsWith("#"))continue;

	       name.ReplaceAll(".hld","");
	       if(name.Length()==15) name.Replace(name.Length()-2,2,""); // get rid of evtbuilder number
	       rinfo = (HRunInfo *)l->FindObject(name.Data());
	       if(!rinfo){
		   cout<<name<<" not found "<<endl;
		   continue;
	       }

	       p->addRun(rinfo->getRunId(),&c,kTRUE);  // overwrite exiting run
	}
	input.close();
	cout<<"NRuns after real fill "<<p->getNRunIDs()<<endl;
	//----------------------------------------------------

	//----------------------------------------------------
	// now the container is filled
	// and we have to write it output
	// connecting all rinIDs to this version
	p->finishRunList();
	p->setStatic();

	for(Int_t i = 0; i < aRuns.GetSize(); i ++){
	    if(i == 0){ // init for on runID and set static
		rtdb->initContainers(aRuns.At(i));
		p->setChanged();
		p->setInputVersion(1,1);
	    } else {
	       rtdb->initContainers(aRuns.At(i));
	    }
	}
	//----------------------------------------------------

	rtdb->saveOutput();
    }



    if(!create ){
        rtdb->initContainers(aRuns.At(0));
    }

    //rtdb->print(); // can be very long
    //p->printParams();  // also long :-)
    //p->printRunIDList();  // also long :-)
    cout<<"NRuns in container :"<<p->getNRunIDs()<<endl;


    delete gHades;
}
