//*-- AUTHOR : Jochen Markert

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////
//   HSlowPar
//
//   Container to keep slowcontrol summary informations for a bunch of
//   channels (mean, rm, min, max values) for a list of runs.
//
//   //##################################################
//
//   HOWTO FILL the container in a macro:
//
//   HSlowPar p("HSlowPar_test","HSlowPar_test");
//
//   HSlowChan c;
//   c.mean = 1750.;
//   c.rms  = 5;
//   c.min  = 1740.;
//   c.max  = 1760.;
//   c.print()
//
//   p.setChannel(runID,"HV_MDC_example",&c,kFALSE); // set channel in macro to write to ROOT/ASCII
//   ..... more channels and runIDs
//   p.makeChannelList(kTRUE); // kTRUE = sort, call this function after all channels have been set
//                             // needed to create the list of channels
//
//   //--------------------------------------------------
//   For the init from ORACLE the list of channels has to be
//   set before and the Partition specified (all runs for the day
//   of the runID used for init will be added automatically):
//
//   HSlowPar p("HSlowPar_test","HSlowPar_test");
//   p->addToChannelList(""HV_MDC_example""); // add a channel name
//   p->setPartition("online);  // during beam time
//                              // (default is offline) after slow control
//                              // summaries are moved to final tables
//   //--------------------------------------------------
//
//   //##################################################
//
//   HOWTO USE the container:
//
//   HSlowPar* p = (HSlowPar*)gHades->getRuntimeDb()->getConatiner("SlowPar");
//   Double_t values[4];
//   p->getChannel(runid,"HV_MDC_example",&c);        // get Channel with name for a given runID
//   p->getChannel(runid,"HV_MDC_example",values);    // get Channel with name for a given runID
//
//   p->printParam();                         // print container content
//   p->printChannel(runID,"HV_MDC_example"); // print a single channel
//   Bool_t ok = p->findRunId(runID);         // is this runID inside ?
//   Int_t first,last;
//   p->getRangeRunID(first,last);            // get first and last runID inside the container
//   Int_t n = p->getNRunIDs();               // get the number runIDs inside the container
//   TList* list = p->getChannelList();       // get the list of channel names
//
//////////////////////////////////////////////////////////////

#include "hslowpar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hpario.h"
#include "hdetpario.h"

#include <algorithm>
#include <iostream>
#include <iomanip>

HSlowPar::HSlowPar(const Char_t* name,const Char_t* title,
		   const Char_t* context) : HParSet(name,title,context)
{

    fCurrentRunID = -2;
    fMIter        = fMRunID.end();
    fPartition    = "offline";
}

void HSlowPar::clear(void) {
    fCurrentRunID = -2;
    fMIter        = fMRunID.end();
}

void HSlowPar::reset(Int_t level) {

    // empty all maps, vectors, lists
    // of runIds and channels
    // level  : 0 reset map
    //          1 reset map + runID
    //          2 reset map + runID = channelList


    map<Int_t,map<TString,HSlowChan*>* >::iterator iter;
    for( iter = fMRunID.begin(); iter != fMRunID.end(); ++iter ) {
	map<TString,HSlowChan*> *m = iter->second;

	map<TString,HSlowChan*>::iterator iter2;
	for( iter2 = m->begin(); iter2 != m->end(); ++iter2 ) {
            HSlowChan* c = iter2->second;
	    delete c;
	}
        m->clear();
        delete m;

    }
    fMRunID.clear();

    fCurrentRunID = -2;
    fMIter        = fMRunID.end();


    if(level > 0) fVRunID.clear();
    if(level > 1) fListChannels.Delete();

}

void HSlowPar::removeRun(Int_t runID) {

    // remove all maps for this runID

    map<Int_t,map<TString,HSlowChan*>* >::iterator iter = fMRunID.find(runID);

    if(iter != fMRunID.end())
    {
	map<TString,HSlowChan*> *m1 = iter->second;

	map<TString,HSlowChan*>::iterator iter2;
	for( iter2 = m1->begin(); iter2 != m1->end(); ++iter2 ) {
	    HSlowChan* c = iter2->second;
	    delete c;
	}
	m1->clear();
	delete m1;

	fMRunID.erase( iter );

        vector<Int_t>::iterator iterv = find(fVRunID.begin(),fVRunID.end(),runID);
        fVRunID.erase( iterv );
    }
}

Bool_t HSlowPar::init(HParIo* inp,Int_t* set) {
    // intitializes the container from an input

    HRun* currentRun = gHades->getRuntimeDb()->getCurrentRun();
    Int_t runID      = currentRun->getRunId();

    if(!findRunID(runID))
    {
        //Info("init()","Run = %i init!",runID);

	HDetParIo* input=inp->getDetParIo("HSpecParIo");

	Bool_t rc = kFALSE;

	TString name = inp->ClassName();
	if(name != "HParOra2Io")
	{   // for ASCII + ROOT IO

            //--------------------------------------
	    // clean up container
	    reset(2);
            //--------------------------------------

	    if (input) rc = input->init(this,set);

	    if(rc)
	    {
		if(name == "HParAsciiFileIo"){
		    sort(fVRunID.begin(),fVRunID.end());
		    setStatic();
		}
	    } else {

                return rc;
	    }

	} else {

           rc = input->init(this,set);
	   sort(fVRunID.begin(),fVRunID.end());
	}

	return rc;
    } else {

        //Info("init()","Run = %i allready inside container!",runID);

    }
    return kTRUE;
}

Int_t HSlowPar::write(HParIo* output) {
    // writes the container to an output
    HDetParIo* out=output->getDetParIo("HSpecParIo");
    if (out) return out->write(this);
    return -1;
}

Bool_t HSlowPar::readline(const Char_t* buf) {
    // decodes one line read from ascii file I/O


    Int_t runID;
    Char_t namec[400];
    HSlowChan sc;

    sscanf(buf,"%i%s%lf%lf%lf%lf",
	   &runID,namec,&sc.mean,&sc.rms,&sc.min,&sc.max);

    TString name = namec;
    return setChannel(runID,name,&sc,kFALSE);
}

void HSlowPar::putAsciiHeader(TString& b) {
    b =
	"#######################################################################\n"
	"# Slow Control parameters for QA\n"
	"# Format:\n"
	"# runID channel mean rms min max\n"
	"#######################################################################\n";
}

Bool_t HSlowPar::write(fstream& out)
{
    // write ascii file

    Bool_t r = kTRUE;

    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	Int_t runId = fVRunID[i];

	map<TString,HSlowChan*> *m = findRunIDMap(runId);

	vector<TString> chan;

	map<TString,HSlowChan*>::iterator iter;
	for( iter = m->begin(); iter != m->end(); ++iter ) {
	    chan.push_back(iter->first);
	}

	sort(chan.begin(),chan.end());

	for(UInt_t j = 0; j < chan.size(); j ++ ){
	    HSlowChan* sc = (*m)[chan[j]];

	    out<<runId<<" "<<sc->GetName()<<" "<<sc->mean<<" "<<sc->rms<<" "<<sc->min<<" "<<sc->max<<endl;

	}
    }
    return r;
}

void HSlowPar::printParam()
{
    // print the content of the container
    // in the order of all channels per runID

    sort(fVRunID.begin(),fVRunID.end());

    Info("printParam()","Container: %s",GetName());

    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	Int_t runId = fVRunID[i];

	map<TString,HSlowChan*> *m = findRunIDMap(runId);

	vector<TString> chan;

	map<TString,HSlowChan*>::iterator iter;
	for( iter = m->begin(); iter != m->end(); ++iter ) {
	    chan.push_back(iter->first);
	}

	sort(chan.begin(),chan.end());

	for(UInt_t j = 0; j < chan.size(); j ++ ){
            HSlowChan* sc = (*m)[chan[j]];
            sc->print(runId);
	}
	cout<<endl;
    }
}

void HSlowPar::printChannel(Int_t runID,TString name)
{
    // print a single channel
    map<TString,HSlowChan*>* m = findRunIDMap (runID);
    if(m) {
	HSlowChan* sc = findChannel(m,name);
	if(sc){
	    sc->print(runID);
	}
    }
}

void HSlowPar::addToChannelList(TString name,Bool_t sort)
{
    // add channel with name name to the list
    // of channels
    if(!fListChannels.FindObject(name)){
	fListChannels.Add(new TObjString(name));
	if(sort) fListChannels.Sort();
    } else {

	Error("addToChannelList()","Channel with name = %s already existing! skipped!",name.Data());
    }
}

void HSlowPar::makeChannelList(Bool_t sort)
{
    // after setting all channels creatye the
    // list of channels

    if( fVRunID.size() > 0 ) {
        fListChannels.Delete();
	HSlowChan c;
        map<TString,HSlowChan*>* m = findRunIDMap(fVRunID[0]);
	if(m){

	    map<TString,HSlowChan*>::iterator iter;
	    for( iter = m->begin(); iter != m->end(); ++iter ) {
		addToChannelList(iter->first,kFALSE);
	    }
	}

        if(sort) fListChannels.Sort();
    }
}

void HSlowPar::printChannelList()
{
    Info("printChannelList()","Container: %s",GetName());
    Int_t ct = 0;
    TIter next(&fListChannels);
    TObject *obj ;
    while ((obj = next())){
       cout<<setw(5)<<ct<<" = "<<((TObjString*)(obj))->GetString().Data()<<endl;
       ct++;
    }
}

void HSlowPar::getRunIDList(TArrayI* ar)
{
    // coppies the list of runIDs into the TArrayI.
    // The array will be rested before.
    sort(fVRunID.begin(),fVRunID.end());
    if(ar && fVRunID.size() > 0){
	ar->Reset();

	ar->Set(fVRunID.size());
	for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	    ar->SetAt(fVRunID[i],i);
	}
    }

}

void HSlowPar::printRunIDList()
{
    Info("printRunIDList()","Container: %s",GetName());
    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	cout<<setw(5)<<i<<" = "<<fVRunID[i]<<endl;
    }
}

Bool_t HSlowPar::setChannel(Int_t runID,TString name, Double_t ar[], Bool_t overwrite)
{
    // set a new channel for runID  with name name
    // to the values provided by the array ar.
    // if overwrite = kTRUE existing channels will
    // be overwritten. if overwrite = kFALSE an attempt
    // to add a channel with existing name will result
    // in an error. To be called from a macro.
    HSlowChan chan;
    chan.set(ar);

    if(!setChannel(runID,name,&chan,overwrite)) return kFALSE;

    return kTRUE;
}

Bool_t HSlowPar::setChannel(Int_t runID,TString name, HSlowChan* chan, Bool_t overwrite)
{
    // set a new channel for runID  with name name
    // to the values provided by the channel chan.
    // if overwrite = kTRUE existing channels will
    // be overwritten. if overwrite = kFALSE an attempt
    // to add a channel with existing name will result
    // in an error. To be called from a macro.

    map<TString,HSlowChan*> *m = findRunIDMap(runID);

    if(m){ // runID already exists

	HSlowChan* vals = findChannel(m,name);
	if(!vals || (vals && overwrite) )
	{
	    if(!vals){ // channel was not existing
		vals = new HSlowChan;
                vals->SetName(name);
		// new channel
		(*m)[name]  = vals;
	    }
            vals->copyValues(chan);

	} else {
	    Error("setChannel()","Channel %s does already exist for runID = %i!",name.Data(),runID );
	    return kFALSE;
	}
    } else { // new RunId

	// new channel map
	map<TString,HSlowChan*> *m1 = new map<TString,HSlowChan*>;

	// new channel
	HSlowChan* vals = new HSlowChan;
        vals->SetName(name);
        vals->copyValues(chan);

	(*m1)[name]   = vals;
 	fMRunID[runID] = m1;

	fVRunID.push_back(runID); // new runID
    }
    return kTRUE;
}


Bool_t HSlowPar::getChannel(Int_t runID,TString name, Double_t values[])
{
    // returns the values of channel name for a given runID
    // to the array values

    HSlowChan* chan = 0;
    getChannel(runID,name,chan);
    if(chan){
	chan->get(values);
      return kTRUE;
    }
    return kFALSE;
}

Bool_t HSlowPar::getChannel(Int_t runID,TString name,HSlowChan* chan)
{
    // returns the channel with name name for
    // a given runID. Donot delete this object!

    map<TString,HSlowChan*> *m = findRunIDMap(runID);

    if(m) {

	HSlowChan* ch = findChannel(m,name);
	if(!ch){
	    Error("getChannel()","Channel %s does not exist for runID = %i!",name.Data(),runID );
	    return kFALSE;
	} else {
            chan->copyValues(ch);
	    return kTRUE;
	}
    } else {
	Error("getChannel()","RunID = %i does not exist!",runID );
        chan->clear();
	return kFALSE;
    }
    return kFALSE;
}

Bool_t HSlowPar::setChannelMap(Int_t runID)
{
    // creates channel map by using channel list for runID
    // if the map exists it will be deleted and created new.

    map<TString,HSlowChan*> *m = findRunIDMap(runID);

    if(m) {
	removeRun(runID);
    }

    TIter next(&fListChannels);
    TObject *obj ;
    HSlowChan chan;
    while ((obj = next())){
	TString name = ((TObjString*)(obj))->GetString();
	setChannel(runID,name,&chan);
    }

    return kTRUE;
}

Bool_t HSlowPar::findRunID(Int_t runId)
{
    // returns kTRUE if the runID exists
    if(find(fVRunID.begin(),fVRunID.end(),runId) != fVRunID.end()) return kTRUE;
    else return kFALSE;
}

HSlowChan* HSlowPar::findChannel(map<TString, HSlowChan* >* m ,TString name)
{
    // returns the channel with name name
    // from the map belonging to a runID
    map<TString, HSlowChan* > ::iterator iter =  m->find(name);
    if(iter != m->end()) return iter->second;
    else return NULL;
}

map<TString,HSlowChan*>* HSlowPar::findRunIDMap (Int_t runId)
{
    // returns the pointer to the map of channels
    // for given runID

    if(fMIter->first != runId || fMIter == fMRunID.end()) {

	fMIter = fMRunID.find(runId);

	if(fMIter != fMRunID.end()) {
	    fCurrentRunID = runId;
	    return fMIter->second;
	} else return NULL;
    } else {
           return fMIter->second;
    }

    return NULL;
}

Bool_t HSlowPar::getRangeRunID(Int_t& first,Int_t& last)
{
    // returns the first and last runID in the container

    sort(fVRunID.begin(),fVRunID.end());

    first = -1;
    last  = -1;

    if(fVRunID.size() > 0) {
	first = fVRunID[0];
	last  = fVRunID[fVRunID.size()-1];
    }

    return kTRUE;
}


ClassImp(HSlowPar) 
