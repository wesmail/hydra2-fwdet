//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HStart2CalRunPar
//
//   Container to keep mean offsets per strip for the start
//   for a list of runs. Speciality : This container is derived
//   from HParCond, but uses the ASCII IO write() from HStartParAsciiFileIo
//   due to formating problems.
//
//   //##################################################
//
//   HOWTO FILL the container in a macro:
//
//   HStart2CalRunPar p("HStart2CalRunPar_test","HStart2CalRunPar_test");
//
//   HStar2CalRunPars data;
//   data.setVal( 0 , 30.); // first strip offset
//   .... 7 more strips
//   data.print()
//
//   p.addRun(runID,&data,kFALSE); // set channel in macro to write to ROOT/ASCII
//   ..... more runIDs
//   p.finishRunList();  // sort the list of runIDs at the end
//                       // only needed when not reading from
//                       // ASCII, ROOT or ORACLE
//
//   //##################################################
//
//   HOWTO USE the container:
//
//   HStart2CalRunPar* p = (HStart2CalRunPar*)gHades->getRuntimeDb()->getContainer("Start2CalRunPar");
//   HStar2CalRunPars data;
//
//   Bool_t found = p->getRun(runid,&data);   // get params for a given runID
//
//   p->printParams();                        // print container content
//   p->printRun(runID);                      // print a single run
//   Bool_t ok = p->findRunID(runID);         // is this runID inside ?
//   Int_t first,last;
//   p->getRangeRunID(first,last);            // get first and last runID inside the container
//   Int_t n = p->getNRunIDs();               // get the number runIDs inside the container
//
//////////////////////////////////////////////////////////////
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hparamlist.h"
#include "hstart2calrunpar.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hrun.h"
#include "hdetpario.h"
#include "hparasciifileio.h"
#include "hdetparasciifileio.h"
#include "hstartparasciifileio.h"



ClassImp(HStart2CalRunPar)


HStart2CalRunPar::HStart2CalRunPar(const Char_t* name    ,
				   const Char_t* title   ,
				   const Char_t* context
				  ) : HParCond(name, title, context)
{
    fMIter = fMRunID.end();
}

HStart2CalRunPar::~HStart2CalRunPar()
{
    reset(1);
}

void HStart2CalRunPar::clear()
{
    fMIter = fMRunID.end();
}

void HStart2CalRunPar::printParams()
{
    // print the content of the container
    // in the order of all data per runID

    sort(fVRunID.begin(),fVRunID.end());

    Info("printParams()","Container: %s",GetName());

    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	Int_t runId = fVRunID[i];
        printRun(runId);
    }
}

void HStart2CalRunPar::printRun(Int_t runID)
{
    // print a single run
    map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(runID);
    if(iter != fMRunID.end()) {
	HStart2CalRunPars* p = iter->second;
	if(p){
	    p->print(runID);
	}
    } else {
	Warning("printRun()","RunID = %i not found!",runID);
    }
}

void HStart2CalRunPar::putParams(HParamList* l)
{
    if (!l) return;

    if(fVRunID.size() > 0)
    {
	sort(fVRunID.begin(),fVRunID.end());

	//-------------------------------------------------------
        // fill param list as linearized array
        Int_t npars = 8; //default

	if(fVRunID.size() > 0) {
	    map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(fVRunID[0]);
	    if(iter != fMRunID.end()){
                 npars = iter->second->getNVals();
	    }
	}

	TArrayD arFull(fVRunID.size()*(npars+1)+1);
	arFull.SetAt(npars,0);

	for(UInt_t i = 0; i < fVRunID.size(); i ++ )
	{
	    Int_t runId = fVRunID[i];

	    map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(runId);
	    if(iter != fMRunID.end()){
		HStart2CalRunPars* ar = iter->second;
		if(ar){
		    arFull.SetAt(runId, 0 + i*(npars+1) + 1);
		    for(Int_t j=0;j<npars;j++){
			arFull.SetAt(ar->getVal(j),j+1 + i*(npars+1) + 1);
		    }
		}
	    }
	}
	l->add("startOffsets", arFull);

    } else {
	Warning("putParams()","No Runs contained in map!");
    }
}

Bool_t HStart2CalRunPar::getParams(HParamList* l)
{
   if (!l) return kFALSE;

   TArrayD arFull;
   if (!l->fill("startOffsets",  &arFull))  return kFALSE;

   Int_t npars = (Int_t) arFull.At(0);
   Int_t nRuns = (arFull.GetSize()-1)/(npars+1);

   for(Int_t i = 0; i < nRuns; i ++ )
   {
       Int_t runId = (Int_t) arFull.At(i*(npars+1) + 1);

       HStart2CalRunPars p(npars);

       for(Int_t j = 0; j < npars; j ++){
	   p.setVal(j,arFull.At(j+1 + i*(npars+1) + 1));
       }
       addRun(runId,&p);
   }

   sort(fVRunID.begin(),fVRunID.end());

   return kTRUE;
}

Int_t HStart2CalRunPar::write(HParIo* output) {
    // writes the parameter container to the output
    

    TString name = output->ClassName();
    
    if(name == "HParAsciiFileIo"){

	HDetParIo* out = output->getDetParIo("HStartParIo");
	if(out){
	    return out->write(this);
	} else {
            Error("write()","HStartParIo not found, but needed for ASCII io write. You have to add the HStartDetector!");
	    return -1;
	}
    } else {
	HDetParIo* out = output->getDetParIo("HCondParIo");
	if(out){
	    return out->write(this);
	} else return -1;
    }
    return -1;
}

void HStart2CalRunPar::putAsciiHeader(TString& b)
{
    // header information needed by HStartParAsciiFileIo::write
    b = "# Start2 mean offsets per strip per run\n";
}

Int_t HStart2CalRunPar::write(fstream& pFile)
{
    // needed by HStartParAsciiFileIo::write
    sort(fVRunID.begin(),fVRunID.end());

    //-----------------------------------------------------
    pFile<<"startOffsets: Double_t \\"<<endl;
    Int_t npars = 8;
    if(fVRunID.size()>0){
	map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(fVRunID[0]);
	HStart2CalRunPars* p = iter->second;
        npars = p->getNVals();
    }
    pFile<<" "<<npars;
    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	Int_t runID = fVRunID[i];
	pFile<<" "<<runID;
	map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(runID);
	if(iter != fMRunID.end()) {
	    HStart2CalRunPars* p = iter->second;
	    if(p){
		for(Int_t j = 0; j < p->getNVals(); j ++){
                    pFile<<" "<<p->getVal(j);
		}
                if(i<fVRunID.size()-1) pFile<<" \\"<<endl;
                else                   pFile<<endl;
	    }
	}
    }
    //-----------------------------------------------------
    return 0;
}
Bool_t HStart2CalRunPar::init(HParIo* inp,Int_t* set) {
    // intitializes the container from an input

    HRun* currentRun = gHades->getRuntimeDb()->getCurrentRun();
    Int_t runID      = currentRun->getRunId();

    Bool_t rc = kTRUE;

    if(!findRunID(runID))
    {
	HDetParIo* input = inp->getDetParIo("HCondParIo");

	rc = kFALSE;

	TString name = inp->ClassName();


	//--------------------------------------
	// clean up container : map + runid list
	reset(1);
	//--------------------------------------

	if (input) rc = input->init(this,set);

	if(rc && name == "HParAsciiFileIo"){
	    setStatic();
	}
    }

    return rc;
}



void HStart2CalRunPar::getRunIDList(TArrayI* ar)
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

void HStart2CalRunPar::printRunIDList()
{
    Info("printRunIDList()","Container: %s",GetName());
    for(UInt_t i = 0; i < fVRunID.size(); i ++ ){
	cout<<setw(5)<<i<<" = "<<fVRunID[i]<<endl;
    }
}
Bool_t HStart2CalRunPar::getRangeRunID(Int_t& first,Int_t& last)
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

Bool_t HStart2CalRunPar::findRunID(Int_t runId)
{
    // returns kTRUE if the runID exists
    if(find(fVRunID.begin(),fVRunID.end(),runId) != fVRunID.end()) return kTRUE;
    else return kFALSE;
}

void HStart2CalRunPar::reset(Int_t level) {

    // empty all maps, vectors
    // of runIds and data
    // level  : 0 reset map
    //          1 reset map + runID list

    map<Int_t,HStart2CalRunPars*>::iterator iter;
    for( iter = fMRunID.begin(); iter != fMRunID.end(); ++iter ) {
	HStart2CalRunPars* ar = iter->second;
	delete ar;
    }
    fMRunID.clear();

    fMIter = fMRunID.end();

    if(level > 0) fVRunID.clear();

}

Bool_t HStart2CalRunPar::removeRun(Int_t runID, Int_t level) {

    // remove all maps for this runID
    // level = 0 : remove Data for runID, but keep runID in list of runs
    //       = 1 (default): remove Data for runID, and runID in list of runs

    map<Int_t,HStart2CalRunPars*>::iterator iter = fMRunID.find(runID);

    if(iter != fMRunID.end())
    {
	HStart2CalRunPars* ar = iter->second;
        delete ar;

	fMRunID.erase( iter );

	if(level > 0){
	    vector<Int_t>::iterator iterv = find(fVRunID.begin(),fVRunID.end(),runID);
	    fVRunID.erase( iterv );
	}

	fMIter = fMRunID.end();  // iter could be invalid
    } else {
	Error("removeRun()","RunID = %i not found!",runID);
	return kFALSE;
    }

    return kTRUE;
}

Bool_t HStart2CalRunPar::addRun(Int_t runID,HStart2CalRunPars* data, Bool_t overwrite) {

    // return data for this run to data
    // if overwrite = kTRUE, an existing data will be overwritten (default=kFALSE)

    if(!data){
	Error("addRun()","Recieved NULL pointer for RunID = %i!",runID);
        return kFALSE;
    }

    fMIter = fMRunID.find(runID);

    if(fMIter == fMRunID.end() || (overwrite && fMIter != fMRunID.end() ) )
    {
	if(fMIter != fMRunID.end()) removeRun(runID,0); // keep runID
	else                        fVRunID.push_back(runID);

	HStart2CalRunPars* ar = new HStart2CalRunPars(data->getNVals());
        ar->copyVals(data);
	fMRunID[runID] = ar;

    } else {
        Error("addRun()","RunID = %i already exists !",runID);
        return kFALSE;
    }
    return kTRUE;
}

Bool_t HStart2CalRunPar::getRun(Int_t runID,HStart2CalRunPars* data) {

    // return data for this run to data
    // data in data will be overwritten!
    // if the size does not fit data will
    // be adapted

    if(!data) Error("addRun()","Recieved NULL pointer for RunID = %i!",runID);


    if(fMIter == fMRunID.end() || fMIter->first != runID) fMIter = fMRunID.find(runID);

    if(fMIter != fMRunID.end() )
    {
	HStart2CalRunPars* ar = fMIter->second;

	if(data->getNVals() < ar->getNVals()) {
	    Warning("getData()","Size of HStart2CalRunPars : in container %i vals, your input object %i vals ! ... Resize output object!",ar->getNVals(),data->getNVals());
	    data->setSize(ar->getNVals());
	}
	ar->getVals(data);

    } else {
        Error("getData()","RunID = %i not found !",runID);
	return kFALSE;
    }
    return kTRUE;
}

void HStart2CalRunPar::finishRunList()
{
    // to be called after last addRun() call
    // needed to sort runIDs in the case
    // the container is filled by hand.
    // For standard IO from ASCII,ROOT or ORACLE
    // input not needed!
    sort(fVRunID.begin(),fVRunID.end());
}
