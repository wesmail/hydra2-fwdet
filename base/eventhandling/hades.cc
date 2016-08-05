#include <string.h>
#include <iostream>
#include <iomanip>
#include "TClass.h"
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TBranch.h"
#include "TInterpreter.h"
#include "TDatime.h"
#include "TSystem.h"
#include "TChain.h"
#include "hades.h"
#include "htree.h"
#include "hdebug.h"
#include "hruntimedb.h"
#include "htaskset.h"
#include "hreconstructor.h"
#include "hspectrometer.h"
#include "hdatasource.h"
#include "heventheader.h"
#include "hevent.h"
#include "hrecevent.h"
#include "hmessagemgr.h"
#include "hldfileoutput.h"
#include "hpario.h"
#include "hldfilesource.h"
#include "hrootsource.h"
#include "hgeantmedia.h"
#include "hsrckeeper.h"

using namespace std;

//*-- Author : Manuel Sanchez
//*-- Modified : 23/07/2004 bj J.Wuestenfeld
//*-- Modified : 09/02/2004 by R. Holzmann
//*-- Modified : 16/09/2003 by R. Holzmann
//*-- Modified : 21/02/2003 by I. Koenig
//*-- Modified : 29/10/2002 by R. Holzmann
//*-- Modified : 24/04/2002 by Joern Wuestenfeld
//*-- Modified : 07/01/2002 by Ilse Koenig
//*-- Modified : 23/11/2001 by Ilse Koenig
//*-- Modified : 10/05/2001 by R. Holzmann
//*-- Modified : 21/04/2001 by R. Holzmann
//*-- Modified : 15/02/2000 by Ilse Koenig
//*-- Modified : 03/03/2000 by Ilse Koenig
//*-- Modified : 01/02/2000 by Manuel Sanchez
//*-- Modified : 16/3/99 by R. Holzmann
//*-- Modified : 26/11/98 by Ilse Koenig
//*-- Modified : 3/11/98/ by Manuel Sanchez
//*-- Modified : 1/10/1998 by Manuel Sanchez
//*-- Modified : 27/05/98 12:01 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Comp.)
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////
//  Hades Begin_Html <a href="Hades_class.gif">(UML Diagram)</a> End_Html
//
//  Hades is the main control class for the reconstruction program.
//
//  There must be one and only one object of this class in the program. This
//  object will be accesible anywhere in the reconstruction program or the
//  external macros through the global pointer gHades.
//
//  See the initialization section in the HYDRA manual for more information..
/////////////////////////////////////////////////////


Int_t Hades::EventCounter = 0;
Int_t Hades::doEmbedding  = 0;
Int_t Hades::doEmbeddingDebug  = 0;
Bool_t Hades::forceNoSkip = kFALSE;
Int_t Hades::fembeddingRealTrackId=-500;

ClassImp(Hades)

Hades::Hades(void)
{
    //Default constructor for Hades. It sets the global pointer gHades.
    //This constructor also creates one HDebug object if none exists.
    gHades = this;
    if (!gDebuger) { gDebuger = new HDebug; }

    fCurrentEvent     = NULL;
    fDataSource       = NULL;
    fSecondDataSource = NULL;
    fHldOutput        = NULL;
    fOutputFile       = NULL;
    fSplitLevel       = 2;
    fTree             = NULL;
    fCounter          = 0;
    rtdb              = HRuntimeDb::instance();
    setup             = new HSpectrometer();

    defineTaskSets();

    quiet            = 0;
    fFirstEventLoop  = kTRUE;
    fCycleNumber     = 0;
    fOutputSizeLimit = 1900000000;
    TTree::SetMaxTreeSize(2000000000); // set >fOutputSizeLimit to avoid conflict
    // with Root's TTree::Fill() size checking
    treeBufferSize   = 8000;
    // Adding new messsage handler
    oldHdlr    = GetErrorHandler();
    msgHandler = new HMessageMgr("Hades","Hades");
    SetErrorHandler(&msgHandler->hydraErrorHandler);
    reqEvents  = 0;
    enableCloseInput = kTRUE;
    isHldSource = kFALSE;
    writeHades  = kFALSE;
    fTaskListStatus = 0;
    fgeantMedia = 0;
    fsrckeeper  = new HSrcKeeper("Hades_src","Hades_src");
    fbeamtimeID = 0; // unknown
}


Hades::~Hades(void)
{
    //Closes the reconstruction program. If there are unsaved data, it saves them
    //When using Hades within a macro don't forget to call the destructor,
    //or data won't be saved.
    closeOutput();
    if (fTree)             delete fTree;
    if (fCurrentEvent)     delete fCurrentEvent;
    if (fHldOutput)        delete fHldOutput;
    if (fDataSource)       delete fDataSource;
    if (fSecondDataSource) delete fSecondDataSource;
    if (rtdb)              delete rtdb;
    if (setup)             delete setup;
    if (fTaskList) {
	fTaskList->Delete();
	delete fTaskList;
	fTaskList = NULL;
	fTask     = NULL;
    }
    if(msgHandler) delete msgHandler;
    SetErrorHandler(oldHdlr);
    if(fgeantMedia) {
	delete fgeantMedia;
	fgeantMedia = NULL;
    }
    gHades = NULL;
}

void Hades::defineTaskSets()
{  // set up empty task sets for all trigger types

    // Do not remove old names ...only add new once
    // The tasklists have to be created before they
    // are configured since they might be called
    // before the first event
    fTask = NULL;

    fTaskList = new TObjArray(25);

    fTaskList->AddAt(new HTaskSet("simulation"        ,"Main task for simulation events")       , 0);

    fTaskList->AddAt(new HTaskSet("real1"             ,"Main task for real1 events")            , 1);
    fTaskList->AddAt(new HTaskSet("real2"             ,"Main task for real2 events")            , 2);
    fTaskList->AddAt(new HTaskSet("real3"             ,"Main task for real3 events")            , 3);
    fTaskList->AddAt(new HTaskSet("real4"             ,"Main task for real4 events")            , 4);
    fTaskList->AddAt(new HTaskSet("real5"             ,"Main task for real5 events")            , 5);
    fTaskList->AddAt(new HTaskSet("real6"             ,"Main task for real6 events")            , 6);
    fTaskList->AddAt(new HTaskSet("real7"             ,"Main task for real7 events")            , 7);
    fTaskList->AddAt(new HTaskSet("real8"             ,"Main task for real8 events")            , 8);

    fTaskList->AddAt(new HTaskSet("MDCcalibration"    ,"Main task for MDC calibration events")  , 9);
    fTaskList->AddAt(new HTaskSet("SHOWERcalibration" ,"Main task for SHOWERcalibration events"), 10);
    fTaskList->AddAt(new HTaskSet("SHOWERpedestals"   ,"SHOWERpedestals")                       , 11);
    fTaskList->AddAt(new HTaskSet("RICHpedestals"     ,"RICHpedestals")                         , 12);

    fTaskList->AddAt(new HTaskSet("CTSStatusConfig"   ,"CTSStatusConfig")                       , 14);

    fTaskList->AddAt(new HTaskSet("real"              ,"Main task for all real events")         , 16);
    fTaskList->AddAt(new HTaskSet("calibration"       ,"Main task for all calibration events")  , 17);

    fTaskList->AddAt(new HTaskSet("all"               ,"Main task for all events")              , 18);


    // compatibility
    fTaskList->AddAt(new HTaskSet("special1","sep03")      ,19);
    fTaskList->AddAt(new HTaskSet("offspill","sep03")      ,20);
    fTaskList->AddAt(new HTaskSet("special3","sep03")      ,21);
    fTaskList->AddAt(new HTaskSet("special5","sep03")      ,22);
    fTaskList->AddAt(new HTaskSet("beginrun","sep03")      ,23);
    fTaskList->AddAt(new HTaskSet("endrun"  ,"sep03")      ,24);




}

Int_t Hades::mapId(Int_t id)
{
    if (id >= 0 && id < MAXEVID) return evIdMap[id];
    else                         return -1;
}

Int_t Hades::setAlgorithmLayout(Text_t *fileName)
{
    //This method is used to build the reconstructor's graph. For that purpose
    //the macro in the file "fileName" is executed.
    //
    //Returns 0 if successfull.
    return gROOT->Macro(fileName);
}

Int_t Hades::setEventLayout(Text_t *fileName)
{
    //This method is intended to build the event's structure. For that purpose
    //the macro in the file "fileName" is executed.
    //
    //Returns 0 if succesfull.
    if (fTree) delete fTree;
    fTree = NULL;
    return gROOT->Macro(fileName);
}

Int_t Hades::setConfig(Text_t *fileName)
{
    // This method is used to interpret a "configuration macro". Within this
    // macro all the information to run the analisys is given. This is an
    // alternative way of initialization without using setEventLayout(),
    // setAlgorithmLayout()...
    //
    // For more information on initialization see the initialization section in
    // the HYDRA manual.
    //
    // Within this macro the user is responsible of calling the Hades::init()
    // function before the call to Hades::makeTree();

    if (gROOT->Macro(fileName)) return kTRUE;
    else                        return kFALSE;
}

Bool_t Hades::init(Bool_t externInit)
{
    // This function initializes Hades. This includes initialization of the
    // reconstructors, the data source and the detectors.

    rtdb->initContainers(fDataSource->getCurrentRunId(),
			 fDataSource->getCurrentRefId());
    if(!setup->init()) return kFALSE;


    if(!externInit){
	fDataSource->setEventAddress(&fCurrentEvent);

	if (!fDataSource->init()){
	    Error("init","Error in fDataSource->init()");
	    return kFALSE;
	}
	TString sourcetype = fDataSource->IsA()->GetName();
	if(sourcetype.CompareTo("HldFileSource") == 0)isHldSource = kTRUE;
	else                                          isHldSource = kFALSE;

	if (fSecondDataSource) {
	    fSecondDataSource->setEventAddress(&fCurrentEvent);
	    if (!fSecondDataSource->init()){
		Error("init","Error in fSecondDataSource->init()");
		return kFALSE;
	    }
	}

    }
    Int_t vers = ((fCurrentEvent->getHeader()->getVersion()));
    initTaskSetsIDs(vers);

    if (!initTasks()) return kFALSE;
    else              return kTRUE;
}

Bool_t Hades::isCalibration()
{ // is event a calibration event?
    HEventHeader *head = fCurrentEvent->getHeader();
    if (!head) {
	Error("isCalibration", "No event header found, exiting...");
	exit(1);
    }
    Int_t id = head->getId();
    Int_t version = head->getVersion() & 0xF;
    if (version == 0) {
	if (id == 2 || id == 3 || id == 4 || id == 5 || id == 6 || id == 7 || id == 8 || id == 9){
	    return kTRUE;
	}
    }
    else if (version == 1) {
	if (id == 7 || id == 9) { return kTRUE; }
    }
    else if (version == 2) {
	if (id >= 9 && id <= 12) { return kTRUE; }
    }
    return kFALSE;
}

Bool_t Hades::isReal()
{ // is event a real event?
    // The event with the sequence number == 0 is a very special event
    // which needs special treatment (does not contain any data, it is
    // header only, should be skipped from the analysis!!!). It is only
    // in the real data and has ID == 2 and Version == 0 (between jan04
    // and may06) which points to a calibration event...

    HEventHeader *head = fCurrentEvent->getHeader();
    if (!head) {
	Error("isReal", "No event header found, exiting...");
	exit(1);
    }

    Int_t id = head->getId();
    Int_t version = head->getVersion() & 0xF;

    if (head->getEventSeqNumber() == 0) {
	if (version == 0 && id == 2) { return kTRUE; }
    } else {
	if (version == 0) {
	    if (id == 1) { return kTRUE; }
	}
	else if (version == 1) {
	    if (id == 1 || id == 2 || id == 3 || id == 4 || id == 5) { return kTRUE; }
	}
        else if (version == 2) {
	    if (id >= 1 && id <= 8 ) { return kTRUE; }
	}
    }
    return kFALSE;
}

Bool_t  Hades::initTasks()
{
    HTaskSet* task;
    TIter iter(fTaskList);

    while((task = (HTaskSet*)iter())) {
	if (!task->init()) { Error("init()","Error returned from %s!",task->GetName()); return kFALSE; }
    }
    return kTRUE;
}

Bool_t Hades::reinitTasks()
{
    HTaskSet* task;
    TIter iter(fTaskList);

    while((task = (HTaskSet*)iter())) {
	if (!task->reinit()) { Error("reinit()","Error returned from %s!",task->GetName());return kFALSE; }
    }
    return kTRUE;
}

Bool_t Hades::finalizeTasks()
{
    HTaskSet* task;
    TIter iter(fTaskList);

    while((task = (HTaskSet*)iter())) {
	if (!task->finalize()) { return kFALSE; }
    }
    return kTRUE;
}

Int_t Hades::executeTasks()
{
    HTaskSet* task;
    TIter iter(fTaskList);
    Int_t returnVal = 0;

    UInt_t evtId = fCurrentEvent->getHeader()->getId();

    getTaskSet("all")->next(returnVal);  // allways execute this task set
    if (returnVal < 0 && returnVal != -99) return returnVal;
    while((task = (HTaskSet*)iter())) {
	if(mapId(evtId) !=-1) task->next(returnVal,evtId);  // execute all task sets responsible for given evtId(s),
	if (returnVal < 0 && returnVal != -99) { break; }   // but stop loop on first error/fSkipEvent
    }

    return returnVal;
}

HTaskSet* Hades::getTask()
{  // returns default set (=real events)
    return getTaskSet("real");
}

HTask* Hades::getTask(const Char_t *name)
{   // search task by name in all sets
    HTaskSet* taskSet;
    HTask* task = NULL;
    for (Int_t i = 0; i < fTaskList->GetSize(); i ++) {
	if ( (taskSet = (HTaskSet*)fTaskList->At(i)) ) {
	    if ( (task = taskSet->getTask(name)) ) { return task; } // first occurence
	}
    }
    return NULL;   // not found in any of the sets
}

HTaskSet* Hades::getTaskSet(Int_t nEvtId)
{  // find task set by event id

    Int_t ind = mapId(nEvtId);   // old/new mapping

    if (ind >= 0 && ind < fTaskList->GetSize()) { return (HTaskSet*)fTaskList->At(ind); }
    else                                        { return NULL; }
}

HTaskSet* Hades::getTaskSet(const Char_t *evtTypeName)
{ // find task set by name

    HTaskSet* taskSet;
    for (Int_t i = 0; i < fTaskList->GetSize(); i ++) {
	if ( (taskSet = (HTaskSet*)fTaskList->At(i)) ) {
	    if (strcmp(taskSet->GetName(),evtTypeName) == 0) { return taskSet; }
	}
    }

    return NULL;
}

HEvent *&Hades::getCurrentEvent(void)
{
    //Returns a pointer to the event in reconstruction
    return fCurrentEvent;
}

void Hades::setEvent(HEvent *ev)
{
    // This function sets the event layout to that given by "ev". The user first
    // builds the event layout by creating an HEvent object and then call this
    // function; which sets the data member fCurrentEvent.
    //
    // This function is typically called from the macro invoked in
    // setEventLayout()
    if (fCurrentEvent) delete fCurrentEvent;
    fCurrentEvent = ev;
}

void Hades::setDataSource(HDataSource *dataS)
{
    // Method used to establish the data source where data are taken from. For
    // that purpose an object of a class derived from HDataSource is created and
    // the setDataSource() method is called to pass a pointer to this object
    //
    //  This function should be called typically from the macro invoked with
    //  setConfig()
    fDataSource=dataS;
}

void Hades::setSecondDataSource(HDataSource *dataS)
{
    // Set second data source (needed e.g. for event merging).
    // Must be a Root source!
    if (dataS != NULL) {
	if (dataS->InheritsFrom("HRootSource") ) {
	    fSecondDataSource=dataS;
	} else {
	    Error("setSecondDataSource()","Second Datasource does not inherit from HRootSource! Ignored!");
	    fSecondDataSource = NULL;
	}
    } else { fSecondDataSource = NULL; }
}

Bool_t Hades::setHldOutput(Text_t* filedir,const Text_t* fileSuffix,const Option_t* option)
{
    // Creates an HLD file output
    if (fHldOutput) delete fHldOutput;
    if (fDataSource && fDataSource->InheritsFrom("HldSource")) {
	fHldOutput = new HldFileOutput((HldSource*)fDataSource,filedir,fileSuffix,option);
	return kTRUE;
    } else {
	Error("setHldOutput","No HLD data source!");
	return kFALSE;
    }
}

void Hades::setSplitLevel(Int_t splitLevel)
{
    //Method used to control the shape of the output Root tree for the events..
    //Three split levels are supported:
    //
    //  splitLevel=0 ---> An only branch is created for the event, which is
    //                    stored as a whole (i.e. the whole HEvent is stored as
    //                    one piece).
    //
    //  splitLevel=1 ---> The partial events are stored as a whole, meanwhile the
    //                    top of the event class (tracks, header ...) is stored
    //			creating one TBranch per data member
    //
    //  splitLevel=2 ---> One branch per data member is created (with little
    //			exceptions, see Root automatic split rules). However
    //			the categories (see HCategory) still can decide how the
    //			split is done (by creating their own branches). This
    //                    is the default value set by the Hades constructor
    //
    fSplitLevel = splitLevel;
}

Int_t Hades::getSplitLevel(void)
{
    //Returns the current splitLevel (see setSplitLevel() )
    return fSplitLevel;
}

Bool_t Hades::setOutputFile(Text_t *name,
			    Option_t *opt,
			    Text_t *title,
			    Int_t comp)
{
    //Sets the output file, giving its name, compresion level...
    //For more information on the parameters see the constructor of TFile
    //
    //  This method allocates the file indicated taking care of saving the
    //current file if any. If the file does not exist or the user wants it to be
    //overwritten, then opt="RECREATE"; if the file already exists and is to be
    //updated then opt="UPDATE"...
    //
    fOutputFileName = name;
    fOutputFileName.Remove(fOutputFileName.Length() - 4 - 1);
    closeOutput();
    fOutputFile = new TFile(name,opt,title,comp);
    return kTRUE;
}

HTree *Hades::getTree(void)
{
    //Returns a pointer to the current output Root tree of events
    return fTree;
}

void Hades::activateTree(TTree *tree)
{
    // Sets the right branch address and branch status (=1) for all the branches
    // in tree which correspond to the branches which would eventually be
    // generated for the output Root tree of events if the function makeTree() is
    // called.
    //
    // This mechanism allows to only read those branches in "tree" which are
    // neccesary to fill an event as defined in setEventLayout()
#if DEBUG_LEVEL>2
    gDebuger->enterFunc("Hades::activateTree");
#endif
    Int_t splitLevel = 2;
    TBranch *brEvent = 0;

    Char_t sl = *(strchr(tree->GetTitle(),'.') + 1);
    switch (sl) {
    case '0' : splitLevel = 0;
    break;
    case '1' : splitLevel = 1;
    break;
    case '2' : splitLevel = 2;
    break;
    default : splitLevel  = fSplitLevel;
    }
    if (fCurrentEvent)
    {
	tree->SetBranchStatus("*",1);   // needed since Root 303-04 because of
	// change in TBranchElement::SetAddress()
	if (splitLevel == 0)
	{
	    tree->SetBranchAddress("Event",&fCurrentEvent);
	    tree->SetBranchStatus("Event",1);
	} else {
	    brEvent = tree->GetBranch("Event.");
	    if (brEvent) { //Post ROOT3 splitting
		brEvent->SetAddress(&fCurrentEvent);
		tree->SetBranchStatus("Event.",1);
		tree->SetBranchStatus("Event.*",1);
	    } else { //Pre ROOT3 splitting
		brEvent = tree->GetBranch("Event");
		if (brEvent) {
		    tree->SetBranchAddress("Event",&fCurrentEvent);
		    tree->SetBranchStatus("Event",1);
		    tree->SetBranchStatus("Event.*",1);
		} else {
		    Warning("activateBranch","Event branch not found in tree");
		}
	    }
	    fCurrentEvent->activateBranch(tree,splitLevel);
	}
    }
#if DEBUG_LEVEL>2
    gDebuger->leaveFunc("Hades::activateTree");
#endif
}


Bool_t Hades::makeTree(void)
{
    // Creates an output tree from the information in the event structure
    // and according to the splitLevel (see setSplitLevel() )
    TBranch *b = NULL;
    Bool_t r   = kFALSE;
    Text_t treeTitle[255];

    if (fTree) delete fTree;

    sprintf(treeTitle,"T.%i",fSplitLevel);

    if (fOutputFile) { fOutputFile->cd(); } // Make sure output file is current file

    fTree = new HTree("T",treeTitle);
    gTree = fTree;
    if (fCurrentEvent && fTree)
    {
	if (fSplitLevel==0) {
	    b = fTree->Branch("Event",
			      fCurrentEvent->ClassName(),
			      &fCurrentEvent,64000,0);
	} else {
	    b = fTree->Branch("Event.",
			      fCurrentEvent->ClassName(),
			      &fCurrentEvent,getTreeBufferSize(),99);
	    fCurrentEvent->makeBranch(b,gHades->getTree());
	}
	if (b) {
	    r = kTRUE;
	}
    }
    return r;
}
Int_t Hades::getCurrentRunId(HDataSource* source)
{
    if(source->getCurrentRefId() == -1) return source->getCurrentRunId();
    else                                return source->getCurrentRefId();
}

Int_t Hades::eventLoop(Int_t nEvents, Int_t firstEvent, Double_t timeQuota)
{
    //  Executes the event loop;
    //
    //  For each new event:
    //  First, the current event is cleared.
    //  Second, a new event is read from the data source (see HDataSource)
    //  Third, the reconstruction of this event is launched (see HReconstructor)
    //  Fourth, if a tree was created (see makeTree() ) then it is filled.
    //
    //  This function returns the number of events processed. A negative value
    //  corresponds to an error
    //
    //  timeQuota - time limit for tasks in minutes.
    //  timeQuota <= 0. - no limit
    //  Event loop end condition: evN==nEvents or runing tasks time > timeQuota
#if DEBUG_LEVEL > 2
    gDebuger->enterFunc("Hades::eventLoop");
#endif
    //----------------------------------
    // local event counters
    Int_t evN       = 0;
    Int_t lastEvent = 0;
    //----------------------------------

    //----------------------------------
    // remember error codes from datasource
    // and tasklists (needed to keep sync for second datasource)
    Int_t err                    = 0;
    Int_t errAll                 = 0;
    EDsState dsCode              = kDsOk;
    EDsState dsCode2             = kDsOk;
    //----------------------------------

    //----------------------------------
    // remember the last active run
    // to decide if reinit is needed
    Int_t oldRunId            = -1;
    //----------------------------------

    //----------------------------------
    // remember the init state of rtdb
    Bool_t initOk             = kTRUE;
    //----------------------------------


    UInt_t evtId;
    TDatime dt;
    HTaskSet *taskSet;

    reqEvents = nEvents;
    if (!(fCurrentEvent && fDataSource)) {
	Error("eventLoop","No current event or no data source set!");
	return 0;
    }

    if (fCurrentEvent->InheritsFrom("HRecEvent")) {
	// Ensure we have a monolithic streamer
	( (HRecEvent *)fCurrentEvent)->setExpandedStreamer(kFALSE);
    }
    fCurrentEvent->Clear();

    //----------------------------------
    // For the first event we have to init parameters
    // and reinit the tasks lists
    if (fFirstEventLoop) {

	if (!rtdb->initContainers(fDataSource->getCurrentRunId(),
				  fDataSource->getCurrentRefId(),
				  fDataSource->getCurrentFileName())) {

	    oldRunId = getCurrentRunId(fDataSource);
	    return 0;
	}

	if(fDataSource->InheritsFrom("HRootSource")){
	    HRootSource* rsrc = (HRootSource*) fDataSource;
            TFile* frsrc = rsrc->getChain()->GetCurrentFile();
	    if(frsrc){
		fgeantMedia = (HGeantMedia*) frsrc ->Get("GeantMedia");
                if(fgeantMedia) fgeantMedia->SetName("GeantMedia");
		if(!fgeantMedia) Warning("eventLoop()","Could not retrieve HGeantMedia from first datasource!");
	    } else Warning("eventLoop()","Could not retrieve file pointer from first datasource!");
	}

	if(!(reinitTasks())) {
	    gDebuger->message("An error occurred in the task list re-init");
	    return evN;
	}
	if(enableCloseInput) rtdb->disconnectInputs();
	fFirstEventLoop = kFALSE;
    }
    //----------------------------------


    //----------------------------------
    // if start event is not 0 we have to
    // skip the events before start event
    dsCode = fDataSource->skipEvents(firstEvent);
    if (dsCode == kDsOk) {
	evN = firstEvent;
    } else {
	evN = firstEvent;
	Warning("eventLoop","Condition %i while skipping events",dsCode);
    }

    if(nEvents != kMaxInt) {
	lastEvent = evN + nEvents;
    } else {
	lastEvent = nEvents;
    }
    //----------------------------------

    //----------------------------------
    // prevent the eventloop from
    // endless waiting for online monitoring
    Bool_t timerFlag = kTRUE;
    fTimer.Reset();
    timeQuota *= 60.;   // minutes -> seconds
    //----------------------------------

    //----------------------------------
    // we need to call the reinit() of the
    // unpackers
    if (!fDataSource->reinit()) {
	Error("eventLoop","An error occurred in fDataSource->reinit()");
    }
    if (fSecondDataSource) {
	if (!fSecondDataSource->reinit()) {
	    Error("eventLoop","An error occurred in fSecondDataSource->reinit()");
	}
	if(!fgeantMedia && fSecondDataSource->InheritsFrom("HRootSource")){
	    HRootSource* rsrc = (HRootSource*) fSecondDataSource;
            TFile* frsrc = rsrc->getChain()->GetCurrentFile();
	    if(frsrc){
		fgeantMedia = (HGeantMedia*) frsrc ->Get("GeantMedia");
                if(fgeantMedia) fgeantMedia->SetName("GeantMedia");
		if(!fgeantMedia) Warning("eventLoop()","Could not retrieve HGeantMedia from second datasource!");
	    } else Warning("eventLoop()","Could not retrieve file pointer from second datasource!");
	}
    }
    //----------------------------------


    //----------------------------------
    if (fHldOutput) {
	TString cf = fDataSource->getCurrentFileName();
	Int_t l = cf.Last('/');
	TString fn = cf(l + 1,cf.Length() - l);
	fHldOutput->open(fn);
    }
    //----------------------------------


    oldRunId = getCurrentRunId(fDataSource);

    while ((evN < lastEvent) && (dsCode != kDsEndData) && (dsCode != kDsError)
	   && timerFlag)
    {
	//----------------------------------
	// here comes now the real event loop
	while ((evN < lastEvent) && timerFlag)
	{

	    EventCounter ++;
	    fTimer.Start(kFALSE);

	    fCurrentEvent->Clear();

	    //-------------------------------------------
            // save output if file is too big and open a new one
	    if (fOutputFile) {
		if (fOutputFile->GetBytesWritten() > fOutputSizeLimit) {
		    recreateOutput();
		}
	    }
	    //-------------------------------------------


	    //-------------------------------------------
	    // If second data source exists it must be first read.
            // Reading from first data source will overwrite header later
	    if (fSecondDataSource) {
		dsCode2 = fSecondDataSource->getNextEvent();
		if(evN == 0 && isHldSource) { fSecondDataSource->setCursorToPreviousEvent(); }
		if (dsCode2 == kDsError  ) { break; }
		if (dsCode2 == kDsEndData) { dsCode = kDsEndData; break; } // secondinput finished, finish out loop too
	    }
	    //-------------------------------------------



	    //-------------------------------------------
            // now read the new event
	    dsCode = fDataSource->getNextEvent();
 	    //-------------------------------------------

 	    //-------------------------------------------------------
	    // if we reached the end of all data files or recieved an
	    // read error we have to exit the event loop
	    if (dsCode == kDsError || dsCode == kDsEndData) { break; }
	    //-------------------------------------------------------


	    //-------------------------------------------
	    // check if reinit is needed (new run)
	    if (dsCode == kDsEndFile || (oldRunId != getCurrentRunId(fDataSource)))
	    {
		if(enableCloseInput) { initOk = rtdb->reconnectInputs(); }
		if (initOk) {
		    initOk = rtdb->initContainers(fDataSource->getCurrentRunId(),
						  fDataSource->getCurrentRefId(),
						  fDataSource->getCurrentFileName());
		}
		if (initOk) {
		    if(!(reinitTasks())) {
			gDebuger->message("An error occurred in the task list re-init");
			return evN;
		    }
		}
		if(enableCloseInput) { rtdb->disconnectInputs(); }

		if (initOk)
		{
		    if (!fDataSource->reinit()) {
			Error("eventLoop","An error occurred in fDataSource->reinit()");
		    }

		    if (fSecondDataSource) {
			if (!fSecondDataSource->reinit()) {
			    Error("eventLoop","An error occurred in fSecondDataSource->reinit()");
			}
		    }

		    if (fHldOutput) {
			TString cf = fDataSource->getCurrentFileName();
			Int_t l = cf.Last('/');
			TString fn = cf(l + 1,cf.Length() - l);
			fHldOutput->open(fn);
			fHldOutput->setHldSource((HldSource*)fDataSource);
		    }
		}
		oldRunId = getCurrentRunId(fDataSource);
	    }
	    //-------------------------------------------

	    //-------------------------------------------------------
	    // For event embbedding we have to take care
	    // about skipped events
	    if (dsCode == kDsSkip)
	    {
		// if the event is skipped the second data source has to
		// be rewind by 1 event
		if(fSecondDataSource)
		{
		    fSecondDataSource->setCursorToPreviousEvent();
		}
		// set all error flags before going to next
		// event
		continue;
	    }
	    //-------------------------------------------------------

	    err    = 99;   // init error
	    errAll = 99;   // init error


	    //-------------------------------------------------------
            // init the tasksset ids and print the definitions
	    UInt_t firstev = fCurrentEvent->getHeader()->getId();

	    if((firstev != 2 && EventCounter == 1) || (EventCounter == 2) )
	    {   // do it only once and
		// if ev is not a begin run ev
		// (kStartEvent=0xd seems to be wrong)
		Int_t vers = ((fCurrentEvent->getHeader()->getVersion()));
		initTaskSetsIDs(vers);
		printDefinedTaskSets();
	    }
	    //-------------------------------------------------------


	    getTaskSet("all")->next(errAll);  // allways execute this task set
            fTaskListStatus = errAll;
	    //-------------------------------------------------------
	    // if no task was returning a skip we have to analyze
            // the data
	    if (errAll != kSkipEvent)
	    {
		evtId = fCurrentEvent->getHeader()->getId();
		if (mapId(evtId) != -1)
		{
		    for (Int_t i = 0; i < fTaskList->GetSize(); i ++) {
			if ( (taskSet = (HTaskSet*)fTaskList->At(i)) ) {
			    taskSet->next(err,evtId); // execute all task sets responsible for given evtId(s),
                            fTaskListStatus = errAll;
			    if (err < 0 && err != -99) { break; }      // but stop loop on first error/fSkipEvent
			}
		    }
		    if(err != 0 )
		    {   // some error code was returned
			if (err != kSkipEvent) {
			    if(!quiet && err != 99) Warning("eventLoop",
							    "Exiting task set with errCode %i at event %i (Id %i)\n",err,evN,evtId);
			    if (fTree)      { fTree->Fill(); }
			    if (fHldOutput) { fHldOutput->writeEvent(); }
			} else {
			    //-------------------------------------------------------
			    // For event embbedding we have to take care
			    // about skipped events
			    if(fSecondDataSource)
			    {
				// if the event is skipped the second data source has to
				// be rewind by 1 event
				fSecondDataSource->setCursorToPreviousEvent();
			    }
			    //-------------------------------------------------------
			}

		    } else { // everything was fine
			if (fTree)      { fTree->Fill(); }
			if (fHldOutput) { fHldOutput->writeEvent(); }
		    }
		} else {
		    Warning("eventLoop","No tasklist defined for event id 0x%x.\n", evtId);
		    if (fHldOutput) { fHldOutput->writeEvent(); }
		}
	    }
	    else {

		//-------------------------------------------------------
		// For event embbedding we have to take care
		// about skipped events
		if(fSecondDataSource)
		{
		    // if the event is skipped the second data source has to
		    // be rewind by 1 event
		    fSecondDataSource->setCursorToPreviousEvent();
		}
		//-------------------------------------------------------
	    }
	    // remember for the next event if the prevoius
	    // event has been skipped

	    evN ++;
	    if(fCounter && (evN % fCounter == 0) && !quiet) {
		dt.Set();
		msgHandler->info(10,HMessageMgr::DET_ALL,GetName()," %s  Events processed: %i current Event: %i SequenceNumber: %i",
				 dt.AsSQLString() + 11, evN - firstEvent,evN,fCurrentEvent->getHeader()->getEventSeqNumber());

	    }
	    timerFlag = fTimer.RealTime() < timeQuota;
	}
    }
    if(dsCode == kDsError) { gDebuger->message("An error occurred in the event loop"); }
#if DEBUG_LEVEL > 2
    gDebuger->leaveFunc("Hades::eventLoop");
#endif
    fDataSource->finalize();
    if (!finalizeTasks()) Error("eventLoop","Unable to finalize");
    if(!timerFlag) msgHandler->info(10,HMessageMgr::DET_ALL,GetName(),"\n%.2f minutes time quota has expired.\n",
				    timeQuota / 60.);
    if(quiet != 2) {
	msgHandler->infoB(10,HMessageMgr::DET_ALL,"Total number of events processed: %i",evN - firstEvent);
	if (fHldOutput) {
	    msgHandler->infoB(10,HMessageMgr::DET_ALL,"\n\nWritten to HLD Output: Total number of events:    %i",
			      fHldOutput->getNumTotalEvt());
	    msgHandler->infoB(10,HMessageMgr::DET_ALL,"                       Number of filtered events: %i",
			      fHldOutput->getNumFilteredEvt());
	}
    }
    return evN - firstEvent;
}

Bool_t Hades::IsFolder(void) const
{
    // Returns true. This tells the Root browser to show Hades as a folder
    // holding other objects
    return kTRUE;
}

void Hades::Browse(TBrowser *b)
{
    // Used to browse the reconstructor's tree, a particular event or the
    // reconstructor's histograms...
    //
    // This function is called by Root when browsing gHades with the Root browser
    //
#if DEBUG_LEVEL>2
    gDebuger->enterFunc("Hades::Browse");
#endif
    if (fTree) { b->Add(fTree,"Tree"); }
    //b->Add(fHistBrowser);
#if DEBUG_LEVEL>2
    gDebuger->message("Adding Task list");
#endif
    if (fTaskList) { b->Add(fTaskList,"Tasks"); }
#if DEBUG_LEVEL>2
    gDebuger->message("Adding current event");
#endif
    if (fCurrentEvent) { b->Add(fCurrentEvent,"Event"); }
#if DEBUG_LEVEL>2
    gDebuger->leaveFunc("Hades::Browse");
#endif
}
void   Hades::addObjectToOutput(TObject* obj)
{
    fObjectsAddedToOutput.AddLast(obj);
}

void Hades::closeOutput(void)
{
    if (fOutputFile)
    {
	fOutputFile->cd();
	this ->Write();
	if(fgeantMedia) fgeantMedia->Write();
	fsrckeeper->Write();
        if(fObjectsAddedToOutput.GetEntries()>0) fObjectsAddedToOutput.Write();
	fOutputFile->Write();
	delete fOutputFile;
	fOutputFile = NULL;
	fTree       = 0;
    }
}

void Hades::recreateOutput(void)
{
    fCycleNumber ++;
    Bool_t createTree = (fTree != 0);
    Char_t name[255];
    TString opt(fOutputFile->GetOption());
    TString title(fOutputFile->GetTitle());
    Int_t comp = fOutputFile->GetCompressionLevel();

    closeOutput();
    sprintf(name,"%s_%i.root",fOutputFileName.Data(),fCycleNumber);

    fOutputFile = new TFile(name,"RECREATE",title.Data(),comp);
    if (createTree) { makeTree(); }

}

void Hades::Streamer(TBuffer &R__b)
{
    if (R__b.IsReading())
    {
	Version_t R__v = R__b.ReadVersion(); if (R__v) { }
	TObject::Streamer(R__b);
	setup->Streamer(R__b);
	rtdb->Streamer(R__b);
	R__b >> fSplitLevel;
	R__b >> fTaskList;
	fDataSource = NULL;
	fOutputFile = NULL;
	fCurrentEvent = (HEvent *)gDirectory->Get("Event");
	fTree = (HTree *)gDirectory->Get("T");
    } else {
	R__b.WriteVersion(Hades::IsA());
	TObject::Streamer(R__b);
        setup->Streamer(R__b);

	if(writeHades)rtdb->Streamer(R__b);
	R__b << fSplitLevel;
	if(writeHades)R__b << fTaskList;
	if (fCurrentEvent)
	{
	    if (fCurrentEvent->InheritsFrom("HRecEvent")) { //!!!!
		Bool_t expand = ((HRecEvent *)fCurrentEvent)->hasExpandedStreamer();
		( (HRecEvent *)fCurrentEvent)->setExpandedStreamer(kTRUE);
		fCurrentEvent->Clear();
		fCurrentEvent->Write("Event");
		( (HRecEvent *)fCurrentEvent)->setExpandedStreamer(expand);
	    } else {
		fCurrentEvent->Clear();
		fCurrentEvent->Write("Event");
	    }
	}
    }
}

void Hades::printDefinedTaskSets()
{
    HTaskSet* task;
    TIter iter(fTaskList);
    Int_t size = 0;
    Int_t* ids;

    Int_t version = ((fCurrentEvent->getHeader()->getVersion()) & 0xF);
    cout<<"#################### HADES TASKSETS ############################"<<endl;
    if(version == 0) {cout<<"#  Version Nov02"<<endl; }
    if(version == 1) {cout<<"#  Version Sep03"<<endl; }
    if(version == 2) {cout<<"#  Version June 2010"<<endl; }

    while((task = (HTaskSet*)iter()))
    {
	ids = task->getIds(size);
	cout<<"#  "<<setw(15)<<task->GetName()<<" for Ids : "<<flush;
	for(Int_t i = 0; i < size; i ++)
	{
	    if(ids[0] == -1){cout<<"not defined"<<flush;break;}
	    if(ids[i] != -1){cout<<ids[i]<<" "<<flush;}
	}
	cout<<""<<endl;
    }
    cout<<"################################################################"<<endl;
}
void  Hades::initTaskSetsIDs(Int_t version)
{
    // Mapping of event ids into HTaskSet numbers for DAQ type old (version=0),
    // i.e prior to Sep03, and new (version=1). Since June 2010 version = 2
    //
    // This table will go in due time into an rtdb container.  (R.H.)
    //
    Int_t evIdTable[3][MAXEVID] = { { 0,16,17,17,17,17,17,17, /* Nov02  */
                                     17,17,-1,-1,-1,13,14,-1},
				    { 0, 1, 2, 3, 4, 5, 6, 7, /* Sep03  */
				      8, 9,10,-1,-1,13,14,-1},
				    { 0, 1, 2, 3, 4, 5, 6, 7, /* June2010  */
				      8, 9,10,11,12,-1,14,-1}
    };

    for (Int_t i = 0; i < MAXEVID; i ++) { evIdMap[i] = evIdTable[version][i]; }

    if (version == 0) { // Nov02
	getTaskSet("simulation")    ->setIds(0);
	getTaskSet("real")          ->setIds(1);
	getTaskSet("calibration")   ->setIds(2,3,4,5,6,7,8,9);
	getTaskSet("beginrun")      ->setIds(13);
	getTaskSet("endrun")        ->setIds(14);
    }
    else if (version == 1) { // Sep03
	getTaskSet("simulation")    ->setIds(0);
	getTaskSet("real")          ->setIds(1,2,3,4,5);
	getTaskSet("calibration")   ->setIds(7,9);
	getTaskSet("real1")         ->setIds(1);
	getTaskSet("real2")         ->setIds(2);
	getTaskSet("real3")         ->setIds(3);
	getTaskSet("real4")         ->setIds(4);
	getTaskSet("real5")         ->setIds(5);
	getTaskSet("special1")      ->setIds(6);
	getTaskSet("offspill")      ->setIds(7);
	getTaskSet("special3")      ->setIds(8);
	getTaskSet("MDCcalibration")->setIds(9);
	getTaskSet("special5")      ->setIds(10);
	getTaskSet("beginrun")      ->setIds(13);
	getTaskSet("endrun")        ->setIds(14);
    }
    else if (version == 2) { // June 2010
	getTaskSet("simulation")    ->setIds(0);
	getTaskSet("real")          ->setIds(1,2,3,4,5,6,7,8);
	getTaskSet("calibration")   ->setIds(9,10,11,12);
	getTaskSet("real1")         ->setIds(1);
	getTaskSet("real2")         ->setIds(2);
	getTaskSet("real3")         ->setIds(3);
	getTaskSet("real4")         ->setIds(4);
	getTaskSet("real5")         ->setIds(5);
	getTaskSet("real6")         ->setIds(6);
	getTaskSet("real7")         ->setIds(7);
	getTaskSet("real8")         ->setIds(8);
	getTaskSet("MDCcalibration")   ->setIds(9);
	getTaskSet("SHOWERcalibration")->setIds(10);
	getTaskSet("SHOWERpedestals")  ->setIds(11);
	getTaskSet("RICHpedestals")    ->setIds(12);
        getTaskSet("CTSStatusConfig")  ->setIds(14);

	getTaskSet("beginrun") -> setIds(-1,-1,-1,-1,-1,-1,-1,-1) ;
	getTaskSet("endrun")   -> setIds(-1,-1,-1,-1,-1,-1,-1,-1) ;
    }
    else
    {
	Error("initTaskSetsIDs()","Unknown version %i ... exiting!",version);
	exit(EXIT_FAILURE);
    }

}

Hades *gHades;


