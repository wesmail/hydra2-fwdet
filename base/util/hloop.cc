//*-- Author : Jochen Markert 28.03.2011

//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////
//
// HLoop is a helper class to allow for fast looping of HADES dsts.
// The categories are maped directly from the Tree and allow partical
// reading to speed up. If Hades object exists, the current event structure
// will be replaced by the one defined by HLoop. If Hades not exists, it
// can be created by HLoop constructor. The Hades eventstructure is important
// if one wants to access the data via gHades->getCurrentEvent()->getCategory(cattype)
// or implicit by Classes like HParticleTrackSorter. See the example below for
// further explanation.
//
//
//
//    HEventHeader* getEventHeader()  // retrieve the pointer to the HADES HEventHeader
//    TTree*        getTree       ()  // retrieve the pointer to the HADES tree
//    TChain*       getChain      ()  // retrieve TChain pointer
//    Long64_t      getEntries    ()  // get Number of events in the chain
//
//--------------------------------------------------------
// example:
//
// #include "hloop.h"
// #include "hcategory.h"
// #include "hzip.h"
// #include "heventheader.h"
// #include "hgeantmedia.h"
// #include "hparticlecand.h"
// #include "hparticletracksorter.h"
//
// #include "TIterator.h"
//
// #include <iostream>
// using namespace std;
// void myLoop()
// {
//
//    //---------------LOOP CONFIG----------------------------------------------------------------
//    Bool_t createHades = kTRUE;  // kTRUE = create HADES new
//    HLoop* loop = new HLoop(createHades); // create HADES (needed if one wants to use gHades)
//    loop.setTreeCacheSize(8000000);//8MB (default is 8MB, values <= 0 will disable the cache)
//    //--------------------------------------------------------------------------------------------
//    loop->readSectorFileList("selection.list",kFALSE,kTRUE);  // hldname_without_evtbuilder 1 1 1 1 1 1
//                                                              // a map of all files to active sector is filled
//                                                              // see functions HLoop::getSectors(Int_t*), Bool_t HLoop::goodSector(Int_t sector),HSectorSelector& getSectorSelector()
//                                                              // for file format description see HSectorSelector
//                                                              // parmeters: default value for files not inside the list (kTRUE all sectors on, kFALSE all sectors off)
//                                                              //            print the filelist during read
//                                                              // If no list is read, all sectors are on by default
//                                                              // reading the files list is optional
//                                                              // lines which uses the feature are marked ##SECTOR_SELECTION## in the following example
//    //--------------------------------------------------------------------------------------------
//
//    // add files : Input sources may be combined
//    loop->addFile ("myFile1.root");
//    loop->addFile ("myFile2.root");  // ....
//    loop->addFilesList("list.txt");  // add all files in ascii file list. list contains 1 root file per line (including path)
//    loop->addFiles("myFiles*.root"); // add all files matching this expression
//    loop->addMultFiles("file1.root,file2.root,file3.root");  add files as comma separated list
//    HZip::makeChain("all_files.zip",loop->getChain()); // add all root files contained in zip file (this file has to be produced by hzip)
//
//    if(!loop->setInput("-*,+HParticleCand")) // global disable "-*" has to be first in the list
//    {                                        // read only one category from file (HEventHeader is allways on)
//        exit(1);
//    }
//    loop->printCategories(); // print status from all categories in event
//    //------------------------------------------------------------------------------------------
//
//    TIterator* iterCand = 0;
//    if (loop->getCategory("HParticleCand")) iterCand = loop->getCategory("HParticleCand")->MakeIterator();
//    HEventHeader* header = loop->getEventHeader();
//    HGeantMedia* media = loop->getGeantMedia();
//    if(media){  // NULL for real data or if not in sim input
//       media->print();
//    }
//
//    //--------------------------CONFIGURATION---------------------------------------------------
//    //At begin of the program (outside the event loop)
//
//    HParticleTrackSorter sorter;
//    //sorter.setDebug();                                            // for debug
//    //sorter.setPrintLevel(3);                                      // max prints
//    //sorter.setRICHMatching(HParticleTrackSorter::kUseRKRICHWindow,4.); // select matching RICH-MDC for selectLeptons() function
//    //sorter.setUseYMatching(kTRUE);                                // use meta cell build in select functions
//    //sorter.setMetaBoundary(3.);                                   // match in 3 mm to meta cell
//    //sorter.setUseBeta(kTRUE);                                     // require beta >0 in build in select functions
//    //sorter.setBetaLeptonCut(0.9);                                 // lower beta cut for lepton select
//    //sorter.setUseFakeRejection(kTRUE);                            // reject fakes in  build in select functions (default kTRUE)
//    //sorter.setIgnoreInnerMDC();                                   // do not reject Double_t inner MDC hits
//    //sorter.setIgnoreOuterMDC();                                   // do not reject Double_t outer MDC hits
//    //sorter.setIgnoreMETA();                                       // do not reject Double_t META hits
//    //sorter.setIgnorePreviousIndex();                              // do not reject indices from previous selctions
//    sorter.init();                                                  // get catgegory pointers etc...
//    //--------------------------------------------------------------------------------------------
//    Int_t nFile = 0;
//
//    //--------------------------------------------------------------------------------------------
//    ##SECTOR_SELECTION##
//    Int_t sectors[6];  // array of active sector  (1 active, 0 off) will be retrive inside loop for each event
//    //--------------------------------------------------------------------------------------------
//
//    for (Int_t i = 0; i < 10; i++) {
//        if(loop->nextEvent(i) <= 0) break;  // get next event. categories will be cleared before
//                                            // if 0 (entry not found) or -1 (Io error) is returned stop the loop
//        //--------------------------------------------------------------------------------------------
//        ##SECTOR_SELECTION##
//        loop->getSectors(sectors);  // get active sectors for current event (somethime arrays are useful)
//        //--------------------------------------------------------------------------------------------
//
//        cout<<i<<"----------------------------------- "<<endl;
//        cout<<"sequence Nr = "<<header->getEventSeqNumber()<<endl; // retrieve full header infos
//        TString filename;
//        if(loop->isNewFile(filename)){ // new file opened from chain ?
//            cout<<"new File found "<<filename.Data()<<endl;
//            nFile++;
//	  }
//
//        sorter.cleanUp();
//        sorter.resetFlags(kTRUE,kTRUE,kTRUE,kTRUE);     // reset all flags for flags (0-28) ,reject,used,lepton
//        Int_t nCandLep     = sorter.fill(HParticleTrackSorter::selectLeptons);   // fill only good leptons
//        Int_t nCandLepBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsLepton);
//        Int_t nCandHad     = sorter.fill(HParticleTrackSorter::selectHadrons);   // fill only good hadrons (already marked good leptons will be skipped)
//        Int_t nCandHadBest = sorter.selectBest(HParticleTrackSorter::kIsBestRKRKMETA,HParticleTrackSorter::kIsHadron);
//
//        if(iterCand){
//           iterCand->Reset();
//
//           HParticleCand* cand;
//           while ( (cand = (HParticleCand*)iterCand->Next()) != 0 ){
//              // do some work  ....
//              //--------------------------------------------------------------------------------------------
//              ##SECTOR_SELECTION##
//              if(!loop->goodSector(cand->getSector())) continue;  // skipp all candidates from inactive sectors
//              //--------------------------------------------------------------------------------------------
//          }
//       }
//    }
//    sorter.finalize(); // clean up stuff
//
//   if(createHades) delete gHades; // delete Hades or call destructor of HLoop.
// }
//
//----------------------------------------------------------------
//
// A second example running a task with hloop.
// The task can use parameter containers if HSpectrometer
// and parameterio are specified. The task will be executed
// inside nextEvent() function.
//
// void hloopDst()
//{
//    Bool_t createHades=kTRUE;
//    HLoop* loop = new HLoop(createHades);  // kTRUE : create Hades  (needed to work with standard eventstructure)
//    loop.setTreeCacheSize(8000000);//8MB (default is 8MB, values <= 0 will disable the cache)
//
//    TString asciiParFile = "/misc/kempter/projects/plot_dedx/param_31oct11.txt";
//    TString rootParFile = "";
//
//    Int_t mdcMods[6][4]={
//	{0,0,0,0},
//	{0,0,0,0},
//	{1,1,1,1},
//	{0,0,0,0},
//	{1,1,1,1},
//	{1,1,1,1} };
//
//    HDst::setupSpectrometer    ("aug11_3sec",mdcMods,"rich,mdc,tof,rpc,shower,wall,tbox,start");
//    HDst::setupParameterSources("ascii,oracle",asciiParFile,rootParFile,"now");
//
//    loop->addFile("be1122719211201.hld_dst_aug11.root");
//
//    if(!loop->setInput("")) { exit(1); } // read all categories
//
//    HEventHeader* header = loop->getEventHeader();
//    HGeantMedia* media   = loop->getGeantMedia();    // NULL if not in input file ()
//    if(media){ // NULL if not in input file
//        media->print();
//    }
//
//
//    loop->printCategories();
//    loop->printChain();
//
//    HTaskSet *masterTaskSet = gHades->getTaskSet("real");
//    masterTaskSet->add(new HMdcPlotDeDx()); // some task derived from HReconstructor
//
//    HCategory* catCand = HCategoryManager::getCategory(catParticleCand);
//    if(!catCand) { exit(1); }
//
//    Int_t entries=loop->getEntries();
//    for (Int_t i=0; i < entries; i++) {
//	Int_t nbytes =  loop->nextEvent(i);            // get next event. categories will be cleared before
//	if(nbytes <= 0) { cout<<nbytes<<endl; break; } // last event reached
//
//	if(catCand){
//	    HParticleCand* cand=0;
//	    for(Int_t j=0; j<catCand->getEntries();j++){
//		cand = HCategoryManager::getObject(cand,catCand,j);
//                // do something  ....
//	    } // end loop cand
//	} //catCand
//
//    }// end event loop
//    if(gHades)gHades->finalizeTasks();
//    if(createHades) delete gHades; // delete Hades or call destructor of HLoop.
//}
//
////////////////////////////////////////////////////////////////////////////


#include "hloop.h"

#include "hades.h"
#include "htree.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "heventheader.h"
#include "hgeantheader.h"
#include "hcategory.h"
#include "htool.h"
#include "htime.h"
#include "hrootsource.h"
#include "hruntimedb.h"
#include "htaskset.h"

#include "TDirectory.h"
#include "TROOT.h"
#include "TGlobal.h"
#include "TList.h"
#include "TKey.h"
#include "TObject.h"
#include "TString.h"
#include "TObjString.h"
#include "TObjArray.h"
#include "TCollection.h"
#include "TOrdCollection.h"
#include "TChainElement.h"
#include "TSystem.h"



#include <iostream>
#include <fstream>
#include <iomanip>

ClassImp(HLoop)
HLoop *gLoop=0;

HLoop::HLoop(Bool_t createHades)
{
    // if createHades == kTRUE (default=kFALSE) Hades will be created and event structure set
    //                == kFALSE Hades will not be created, but if exists
    //                   the event structure will be replaced by the one defined in HLoop
    // if Hades does not exists and is not created new, the event structure will be
    // available only via HLoop and not via gHades
    gLoop = this;
    fChain        = new TChain("T");
    fHead         = NULL;
    fTree         = NULL;
    fFileCurrent  = NULL;
    fGeantMedia   = NULL;
    fMaxEntries   = 0;
    fCurrentEntry = 0;
    fCurrentName  ="unset";
    fIsNewFile    = kTRUE;
    fIsSkipped    = kFALSE;
    fUseTaskSet   = kFALSE;
    fHasCreatedHades = kFALSE;
    fRefID        = -1;
    fFirstEvent   = kTRUE;

    fTreeCacheSize        = 8000000; //8Mb
    fTreeCacheDefaultSize = 8000000; //8Mb;
    fIsCacheSet           = kFALSE;

    fRecEvent     = new HRecEvent();
    for(Int_t s=0;s<6;s++) fsectors[s]=1;

    if(createHades){
	if(gHades == 0) {
            fHasCreatedHades = kTRUE;
	    Hades* hades = new Hades();
            hades->setEvent(fRecEvent);
            Info("HLoop()","HADES created and event structure set !" );
	} else {
	    Error("HLoop()","HADES already exists, but should be created new! Check your macro ..." );
            exit(1);
	}

    } else {
	if(gHades != 0) {
	    gHades->setEvent(fRecEvent);
            Info("HLoop()","HADES already exists, event structure replaced !" );
	} else {
            Info("HLoop()","HADES does not exists! No functions via gHades will be available!" );
	}
    }
}

HLoop::~HLoop()
{
    delete fChain;
    if(fHasCreatedHades && gHades) delete gHades;
    if(fGeantMedia) delete fGeantMedia;
}

void HLoop::setTreeCacheSize(Long64_t cs )
{
    // Configure the read of the Root Tree:
    // default is 8MB to minimize the seek/read on the file system
    // values <= 0 will disable the cache (for comparison)
    // THis function has to be called before setInput().

    fTreeCacheSize = cs;
    fIsCacheSet    = kTRUE;
}


Bool_t HLoop::addFile (TString infile)
{
    // add a single root file to the chain.
    // It will be checked if the file exists.

    TObjArray* elements = fChain->GetListOfFiles();
    Int_t nfiles        = elements->GetEntries();

    if(gSystem->AccessPathName(infile.Data()) == 0) {
	cout<<setw(4)<<nfiles<<" add file : "<<infile.Data()<<endl;
	fChain->AddFile(infile.Data());
        return kTRUE;
    } else {
        Error("addFile()","File = %s not found! Will be skipped .... ",infile.Data());
        return kFALSE;
    }

}

Bool_t HLoop::addFiles(TString expression){

    // add all files matching the expression to the chain

    TObjArray* arr = HTool::glob(expression);
    Int_t n = arr->GetEntries();
    if(n == 0) {
	Warning("addFiles()","No file matching expression '%s' found !",expression.Data());
    } else {
	for(Int_t i = 0; i < n; i ++){
	    TString name = ((TObjString*)arr->At(i))->GetString();
	    addFile(name);
	}
    }
    delete arr;
    return kTRUE;

}
Bool_t HLoop::addFilesList(TString filelist)
{
    // add all files in list filelist. The list should contain
    // 1 root file per line (including path). empty lines and
    // commented lines (starting with "#") will be ignored.
    // Return kFALSE if list files does not exist or and error
    // occured while reading the file

    if(gSystem->AccessPathName(filelist.Data()) == 0 )
    {
	ifstream in;
	in.open(filelist.Data());
	TString name;
        Char_t line[10000];

	while(!in.eof()){
	    in.getline(line,10000);
	    name=line;
	    name.ReplaceAll(" ","");   // remove spaces

	    if(!in.fail()              &&
	       name.CompareTo("")  !=0 &&  // skip empty lines
	       name.BeginsWith("#")==0     // skip commented lines
	      )
	    {
		addFile(name);
	    }
	}
        in.close();

       return kTRUE;
    } else {
       Error("addFilesList()","File = %s not found! Will be skipped .... ",filelist.Data());
       return kFALSE;
    }
}


Bool_t HLoop::addMultFiles(TString commaSeparatedList)
{
    // add multiple files as comma separated file list : "file1,file2,file3"

    TObjArray* arr = commaSeparatedList.Tokenize(",");
    Int_t n = arr->GetEntries();
    if(n == 0) {
	Warning("addMultFiles()","No file in input string '%s' found !",commaSeparatedList.Data());
    } else {
	for(Int_t i = 0; i < n; i ++){
	    TString name = ((TObjString*)arr->At(i))->GetString();
	    if(!addFile(name)){
		delete arr;
		return kFALSE;
	    }
	}
    }
    delete arr;
    return kTRUE;
}


Bool_t HLoop::setInput(TString readCategories)
{
    // read in the given file structure. Categories can be
    // disabled/enabled by the string readCategories. This
    // string should contain a comma separated list. By default
    // all categories are enabled. The format should be
    // like (example 1) "-*,+HMdcCal1,+HParticleCand"
    // (switch off all categories and enable just the to given
    // ones. The global disable -* has to be the first in the
    // list. Or (example 2) "-HMdcRaw,-HRichHit" (enable all
    // categories, but not the give ones).  For simulation
    // or real data one has to use the correct corresponding
    // names (expample : HParticleCandSim / HParticleCand)


    TObjArray* fList  = fChain->GetListOfFiles();

    if(fList->GetEntries() == 0){
        Error("setInput()","No File in List!");
	return kFALSE;
    }



    TChainElement* el = (TChainElement*)fList->At(0);
    TString file = el->GetTitle();

    TFile* fIn = (TFile*)gROOT->GetListOfFiles()->FindObject(file.Data());
    if (!fIn) {
	fIn = new TFile(file.Data());
	if (!fIn) {
	    Error("setFile()","Could not open infile %s",file.Data());
	    return kFALSE;
	}
    }
    fIn->cd();

    fChain->LoadTree(0);

    fTree = (HTree*) fChain->GetTree();
    if(!fTree) {
	Error("setInput()","Could not find Tree in infile %s",file.Data());
	return kFALSE;
    }

    fGeantMedia = (HGeantMedia*) fIn->Get("GeantMedia");


    fHead = fRecEvent->getHeader();


    TBranch* brhead = fTree->GetBranch("EventHeader.");
    if(!brhead) {
	Error("setFile()","Eventheader Branch not found !");
	return kFALSE;
    } else {
	fChain->SetBranchAddress("EventHeader.",&fHead);
	fChain->SetBranchStatus("EventHeader.",1);
	fChain->SetBranchStatus("EventHeader.*",1);
    }

    fChain->SetBranchStatus("Event.",0);
    fChain->SetBranchStatus("Event.*",0);

    TList* dirkeys = gDirectory->GetListOfKeys();

    for(Int_t d = 0; d < dirkeys->GetEntries(); d ++){

	TKey* kdir = (TKey*)dirkeys->At(d);
	TString dirname = kdir->GetName();

	if(dirname.BeginsWith("dir"))
	{

	    TString partialEvent = dirname;
	    partialEvent.ReplaceAll("dir","");

	    if(fIn->cd(dirname.Data()) )
	    {
		TList* catkeys = gDirectory->GetListOfKeys();

		for(Int_t c = 0; c <catkeys->GetEntries(); c ++)
		{
		    fIn->cd(dirname.Data());
		    TKey*   kcat      = (TKey*) catkeys->At(c);
		    TString catname   = kcat->GetName();
		    TString classname = kcat->GetClassName();

		    if(classname == "HPartialEvent") {


			fPartial[catname] = (HPartialEvent*) gDirectory->Get(catname.Data());
                        if(!fPartial[catname]) {
			    Error("setInput()","NULL pointer for partial event %s retrieved!",catname.Data()) ;
			    return kFALSE;
			}

			TBranch* br = fTree->GetBranch(Form("%s.",catname.Data()));
			if(!br) {
			    Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
			    return kFALSE;
			} else {

			    fChain->SetBranchAddress(Form("%s.",catname.Data()),&fPartial[catname]);
			    fChain->SetBranchStatus (Form("%s.",catname.Data()),1);
			    fChain->SetBranchStatus (Form("%s.*",catname.Data()),1);
			}

			fRecEvent->addPartialEvent(fPartial[catname]);

			continue;
		    }

		    //------------------------------------------------------------
		    if(classname == "HLinearCategory" || classname == "HMatrixCategory")
		    {
			fEvent[catname] = (HCategory*) gDirectory->Get(catname.Data());
			if(!fEvent[catname]) {
			    Error("setInput()","NULL pointer for category %s retrieved!",catname.Data()) ;
			    return kFALSE;
			}
			fStatus  [catname] = 0;
                        fPartialN[catname] = partialEvent;

			TBranch* br = fTree->GetBranch(Form("%s.",catname.Data()));
			if(!br) {
			    Error("setInput()","Branch not found : %s !",Form("%s.",catname.Data()));
			    return kFALSE;
			} else {
			    fChain->SetBranchAddress(Form("%s.",catname.Data()),&fEvent[catname]);
			    fChain->SetBranchStatus (Form("%s.",catname.Data()),1);
			    fChain->SetBranchStatus (Form("%s.*",catname.Data()),1);
			    fStatus[catname] = 1;
			}
		    }
		    //------------------------------------------------------------

		} // end cat keys
	    } else {
		Error("setInput()","Could no enter dir %s !",dirname.Data());
		return kFALSE;
	    }
	}
    }
    fIn->cd();

    fMaxEntries = fTree->GetEntries();
    TString readCat = readCategories;
    readCat.ReplaceAll(" ","");

    TObjArray* catNames = readCat.Tokenize(",");
    Int_t n = catNames->GetEntries();

    //------------------------------------------------------------

    for(Int_t i = 0; i < n; i ++){
	TString name = ((TObjString*)catNames->At(i))->GetString();

	//------------------------------------------------------------
	if(i == 0){
	    if(name.CompareTo("-*") == 0){
		setStatus("-*", 0);
		continue;
	    }
	} else {
	    if(name.Contains("*") != 0){
		Warning("setInput()","Wild cards are only supported for '-*' in the first position of the string! Will ignore this one!");
		continue;
	    }
	}
	//------------------------------------------------------------

	//------------------------------------------------------------
	if(name.BeginsWith("+")) { // switch on
	    name.ReplaceAll("+","");
	    if(getCategory(name)) { // exists in input
		setStatus(name, 1);
	    }
	}
	//------------------------------------------------------------

	//------------------------------------------------------------
	if(name.BeginsWith("-")) { // switch off
	    name.ReplaceAll("-","");
	    if(getCategory(name)) { // exists in input
		setStatus(name, 0);
	    }
	}
	//------------------------------------------------------------

    }

    //------------------------------------------------------------
    if(n == 0) {
	// no categories defined in input.
	// by default switch all on
	map<TString,HCategory*>::iterator iter;
	for( iter = fEvent.begin(); iter != fEvent.end(); ++iter ) {
	    addCatName(iter->first,getCategory(iter->first)->getCategory());
	}
    } else {

	map<TString,Int_t>::iterator iter;
	for( iter = fStatus.begin(); iter != fStatus.end(); ++iter ) {
	    if(iter->second == 1 ) {
		addCatName(iter->first,getCategory(iter->first)->getCategory());
	    }
	}
    }
    //------------------------------------------------------------


    fIn->Close();

    printChain();
    Long64_t cs = fChain->GetCacheSize();
    if(!fIsCacheSet ){
	cs = fTreeCacheDefaultSize;
    } else {
        cs = fTreeCacheSize;
    }

    if(fTreeCacheSize > 0) Info("setInput()","Setting Tree Cache size = %lld byte.",cs);
    else                   Info("setInput()","Tree cache is disabled");

    if(cs > 0){
	fChain->SetCacheSize(cs);
	fChain->AddBranchToCache("*",kTRUE);
	fChain->StopCacheLearningPhase();
    }
    return kTRUE;
}

void HLoop::printCategories()
{
    // print all categories found in input + status

    cout<<"--------------------------------------"<<endl;
    cout<<"current event :"<<endl;
    map<TString,HCategory*>::iterator iter;
    for( iter = fEvent.begin(); iter != fEvent.end(); ++iter ) {
	cout << setw(20)<<iter->first.Data() << " = " << iter->second <<" status = "<<fStatus[iter->first]<< endl;
    }
    cout<<"--------------------------------------"<<endl;
}

void HLoop::printChain()
{
    // print all files in the chain


    TObjArray* elements = fChain->GetListOfFiles();
    cout<<"--------------------------------------"<<endl;
    cout<<" Analize Chain : .... Can take while  for N file = "<<elements->GetEntries()<<endl;
    fMaxEntries = fChain->GetEntries();
    cout<<"N total entries = "<<fMaxEntries<<endl;
    for(Int_t i = 0; i < elements->GetEntries(); ++i ) {
        TChainElement* element = (TChainElement*)elements->At(i);
	cout <<setw(4)<<i<< setw(80)<<element->GetTitle() << ", N entries = " << element->GetEntries()<< endl;
    }
    cout<<"--------------------------------------"<<endl;
}

void HLoop::printBranchStatus()
{
    // print the branch status of the chain

    cout<<"--------------------------------------"<<endl;
    cout<<"Branch status :"<<endl;

    TObjArray* branches = fChain->GetListOfBranches();
    for(Int_t i = 0; i < branches ->GetEntries(); ++i ) {
        TBranch* br = (TBranch*) branches->At(i);
	cout <<setw(4)<<i<< setw(30)<<br->GetName() << ", status = " <<(Int_t) fChain->GetBranchStatus(br->GetName())<< endl;
    }
    cout<<"--------------------------------------"<<endl;
}

void HLoop::clearCategories()
{
    //  clear all active catgegories of the event

    map<TString,HCategory*>::iterator iter;
    for( iter = fEvent.begin(); iter != fEvent.end(); ++iter ) {
	if(fStatus[iter->first]) iter->second->Clear();
    }
}

HCategory* HLoop::getCategory(TString catname,Bool_t silent)
{
    // return the pointer to given category. NULL if not found

    TString simname  = catname;
    TString realname = catname;

    if(realname.EndsWith("Sim")==1) realname.Replace(realname.Length(),3,"");
    if(simname .EndsWith("Sim")==0) simname+="Sim";


    map<TString,HCategory*>::iterator iter = fEvent.find(realname);
    if( iter == fEvent.end() ){

	iter = fEvent.find(simname);
	if(iter == fEvent.end()){

	    if(!silent)Error("getCategory()","Category \"%s\" not found!",catname.Data());
	    return 0;

	} else  return iter->second;

    } else return iter->second;
}

Bool_t HLoop::getCategoryStatus(TString catname,Bool_t silent)
{
    // return the status of a given category. kFALSE if not found

    map<TString,Int_t>::iterator iter = fStatus.find(catname);
    if( iter == fStatus.end() ){
	if(!silent)Error("getCategory()","Category \"%s\" not found!",catname.Data());
	return kFALSE;
    } else {
	return (Bool_t)fStatus[catname];
    }
}

HPartialEvent* HLoop::getPartialEvent(TString catname,Bool_t silent)
{
    // return the pointer to given partialevent. NULL if not found

    map<TString,HPartialEvent*>::iterator iter = fPartial.find(catname);
    if( iter == fPartial.end() ){
	if(!silent)Error("getPartialEvent()","PartialEvent \"%s\" not found!",catname.Data());
	return 0;
    }
    else return iter->second;
}

HGeantHeader* HLoop::getGeantHeader(Bool_t silent)
{
    // return the pointer to HGeantHader (subheader of PartialEvent Simul).
    // NULL if not found
    HPartialEvent* pEvt = getPartialEvent("Simul",silent);
    HGeantHeader* header = NULL;
    if(pEvt) header = (HGeantHeader*) pEvt->getSubHeader();
    return header;
}

Bool_t HLoop::setStatus(TString catname, Int_t stat )
{
    // set the status of a given category. return kFALSE
    // if something went wrong. if catname="-*" or "+*"
    // all categories of the event are disabled /enabled.

    if(catname.CompareTo("-*") == 0 || catname.CompareTo("+*") == 0 ){
	map<TString,HCategory*>::iterator iter;
	for( iter = fEvent.begin(); iter != fEvent.end(); ++iter ) {
	    if(catname.CompareTo("-*") == 0)  {
		fChain->SetBranchStatus(Form("%s.",iter->first.Data()),0);
		fChain->SetBranchStatus(Form("%s.*",iter->first.Data()),0);
		fStatus[iter->first] = 0;
	    }
	    if(catname.CompareTo("+*") == 0)  {
		fChain->SetBranchStatus(Form("%s.",iter->first.Data()),1);
		fChain->SetBranchStatus(Form("%s.*",iter->first.Data()),1);
		fStatus[iter->first] = 1;
	    }
	}
	return kTRUE;
    }
    if(!getCategory(catname)) {
	return kFALSE;
    }

    TString simname  = catname;
    TString realname = catname;

    if(realname.EndsWith("Sim")==1) realname.Replace(realname.Length(),3,"");
    if(simname .EndsWith("Sim")==0) simname+="Sim";

    map<TString,Int_t>::iterator iter = fStatus.find(realname);

    if( iter == fStatus.end() ){

	iter = fStatus.find(simname);

	if( iter == fStatus.end() ) {
	    Error("getCategory()","Category \"%s\" not found!",catname.Data());
	    return kFALSE;
	} else {

	    fChain->SetBranchStatus(Form("%s.",simname.Data()),stat);
	    fChain->SetBranchStatus(Form("%s.*",simname.Data()),stat);
	    iter->second = stat;
	    return kTRUE;
	}
    }
    else {
	fChain->SetBranchStatus(Form("%s.",realname.Data()),stat);
	fChain->SetBranchStatus(Form("%s.*",realname.Data()),stat);
	iter->second = stat;
	return kTRUE;
    }
    return kFALSE;
}

Int_t HLoop::nextEvent(Int_t iev)
{
    // get next entry form tree. clears all active categories
    // before. Returns number of read bytes
    // (0 = entry not found , -1 = IO error)
    // prints the current file name if a new file
    // is entered in the chain

    if(!fHead) {
	Warning("nextEvent()","File not initialized ! Call setInput(....) before!");
	exit(1);
    }

    fCurrentEntry = iev;

    clearCategories();

    //--------------------------------------------------------
    // read next event from Chain
    Int_t nbytes = fChain->GetEntry(fCurrentEntry);  // 0 = entry not found , -1 = IO error
    if(nbytes == 0) {
	Warning("nextEvent()","Entry %i not found!",(Int_t)fCurrentEntry);
    } else if(nbytes == -1) {
	Warning("nextEvent()","Entry %i read with Io error!",(Int_t)fCurrentEntry);
    }

    //--------------------------------------------------------

    //--------------------------------------------------------
    // check for new file
    TString name = fChain->GetFile()->GetName();
    if(name != fCurrentName) {
	fIsNewFile = kTRUE;
        Int_t entries = 0;
	TObjArray* elements = fChain->GetListOfFiles();
	for(Int_t i = 0; i < elements->GetEntries(); ++i ) {
	    TChainElement* element = (TChainElement*)elements->At(i);
	    if(name.CompareTo(element->GetTitle() ) == 0) {
		entries = element->GetEntries();
                break;
	    }
	}
	fCurrentName = fChain->GetFile()->GetName() ;
	fFileCurrent = fChain->GetFile() ;
	fTree        = fChain->GetTree() ;

	//--------------------------------------------------------
	// sync the sector array from filelist
	// 1. do not try if there has been no filelist read
	// 2. do silent error
	// 3. local fsectors will hold the actual vals
	// 4. fsectors will be filled with default 1 if filename does not match hldname format (2chars+13digits)
        // 5. for correct filenames which are not inside the list fsectors will be filled by the default val set in the readSectorFileList()

	for (Int_t s=0;s<6;s++){ fsectors[s]=1;} // defaults is on
	if(fSectorSelector.getNFiles() > 0){ // no sector file list has been read
	    TString hldname = HTime::stripFileName(fCurrentName,kTRUE,kTRUE); // remove Evtbuild from hldname, do it silent if there is an error
	    if(hldname != "" ) { // filename was compatible
		fSectorSelector.getSectors(fsectors,hldname); // result depends on defaultVal set
	    }
	}
	//--------------------------------------------------------

	cout<<"---------------------------------------------------------"<<endl;
	cout<<name.Data() <<",  sectors : "<<fsectors[0]<<" "<<fsectors[1]<<" "<<fsectors[2]<<" "<<fsectors[3]<<" "<<fsectors[4]<<" "<<fsectors[5]<<" "<<endl;
	cout<<"at entry "<<setw(8)<<fCurrentEntry<<" with entries = "<<entries<<endl;
        cout<<"---------------------------------------------------------"<<endl;

    } else {
       fIsNewFile = kFALSE;
    }
    //--------------------------------------------------------

    if(fFirstEvent){
	if(gHades){
	    //--------------------------------------------------------
	    const TObjArray* l = gHades->getTaskList();

	    HTaskSet* task;
	    TIter iter(l);
	    Int_t ct = 0 ;
	    while((task = (HTaskSet*)iter())) {
		TOrdCollection* list = task->getSetOfTask();
		ct += list->GetEntries();
	    }

	    if(ct > 0) fUseTaskSet = kTRUE;

	    if(fUseTaskSet && gHades->getSetup() == 0 ) {
		Error("","TaskSet used, but Spectrometer not set!"); exit(1);
		fUseTaskSet = kFALSE;
	    }
	    //--------------------------------------------------------

	    if(fUseTaskSet){

		HRootSource* source = new HRootSource();
		source->addFile((Text_t*)fCurrentName.Data());
		source->setCurrentRunId(fHead->getEventRunNumber());
		source->setGlobalRefId(fRefID);
		gHades->setDataSource(source);

		if(!gHades->init(kTRUE)){ Error("","HADES init failed!"); exit(1);}

		HRuntimeDb* rtdb = gHades->getRuntimeDb();

		if (rtdb){
		    if(!rtdb->initContainers(fHead->getEventRunNumber(),
					     fRefID,
					     fCurrentName.Data())
		      ){
			Error("","Rtdb init failed!"); exit(1);
		    }
		}
		gHades->reinitTasks();
		gHades->printDefinedTaskSets ();
                rtdb->disconnectInputs();
	    }
	}
	fFirstEvent = kFALSE;
    }

    //--------------------------------------------------------
    // reinit if new file is opened
    if(fUseTaskSet && !fFirstEvent && gHades && fIsNewFile){
	HRootSource* source = (HRootSource*)gHades->getDataSource();
	if(source){
	    HRuntimeDb* rtdb = gHades->getRuntimeDb();
	    rtdb->reconnectInputs();
	    source->setCurrentRunId(fHead->getEventRunNumber());
	    source->setGlobalRefId(fRefID);

	    if(!rtdb->initContainers(fHead->getEventRunNumber(),
				     fRefID,
				     fCurrentName.Data())
	      ){
		Error("","Rtdb init failed!"); exit(1);
	    }

	    gHades->reinitTasks();
	    rtdb->disconnectInputs();
	}
    }
    //--------------------------------------------------------

    //--------------------------------------------------------
    // run tasks if they are defined
    if(fUseTaskSet && gHades){
	Int_t retVal = gHades->executeTasks();
	if(retVal == kSkipEvent){
	    fIsSkipped = kTRUE;
	    fRecEvent->Clear();
	} else {
            fIsSkipped = kFALSE;
	}
    }
    //--------------------------------------------------------


    return nbytes;
}

Bool_t HLoop::addCatName(TString catname,Short_t catNum)
{
    // add agiven category number to the HADES event structure.
    // this function has to be called after setInput() !!!!

    if(fHead == 0) {
        Error("addCatName()","You have to call setInput() before calling this function !");
	exit(1);
    }

    map<TString,Short_t>::iterator iter = fNameToCat.find(catname);
    if( iter == fNameToCat.end() ){
	if(getCategory(catname,kTRUE)){
            Bool_t ret =  fRecEvent->addCategory((Cat_t)catNum,getCategory(catname),fPartialN[catname].Data());
            fNameToCat[catname] = catNum;
	    return ret;
       } else {
	   Error("addCatName()","Category \"%s\" does not exists in input! Will skip ...",catname.Data());
           return kFALSE;
       }
    } else {
	Error("addCatName()","Category \"%s\" already exists with number = %i!",catname.Data(),fNameToCat[catname]);
        return kFALSE;
    }
    return kTRUE;
}

Bool_t HLoop::isNewFile(TString& name)
{
    // this function can be called after HLoop::nextEvent(Int_t iev)
    // If a new file was opened kTRUE will be returned. The
    // current file name will be returned by name
    name = fCurrentName;
    return fIsNewFile;
}

TObject* HLoop::getFromInputFile(TString name)
{
    // returns an object from root input file
    // return 0 if not existsing, file not opened
    // or anything else strange.
    if(name=="") return 0;

    if(fFileCurrent){
	TDirectory* dold =  gDirectory;

        fFileCurrent->cd();

        TObject* obj = fFileCurrent->Get(name.Data());

	dold->cd();

        return obj;

    } else return 0;
}


