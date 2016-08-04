// modified 22-05-00 by D.B
// modified  3-12-1998 D.B
// modified  23/3/99 by D.B
// modified  22/11/99 by D.B <d.bertini@gsi.de>
//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//HLdSockremoteSource
//
//  This is a particular kind of "data source"  (see HDataSource) which takes 
// the event's data from  directly from a Data event builder.
//
//  The data are accessed via a server using Root's TSockets  
// and need to be unpacked.  For This user must specify the 
// node of a dedicated remote event 
// SERVER as argument in the constructor of this class. 
// Unpacking/swapping data is then similar 
// to other data source.(see HldSource).
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "hldsockremotesource.h"
#include "hldsockremotevt.h"
#include "hrecevent.h"
#include "hruntimedb.h"
// #include "heventfile.h"
#include "hldevt.h"
#include "hldsubevt.h"
#include "hldunpack.h"  
#include "hades.h"
#include <iostream> 

ClassImp(HldSockRemoteSource)

HldSockRemoteSource::HldSockRemoteSource(void): HldSource() {
	refId=-1;
}

HldSockRemoteSource::HldSockRemoteSource(const Text_t *nodeName,Int_t evtMax): HldSource(){
   fReadEvent=new HldSockRemotEvt(nodeName,0,evtMax,0);
   currNodeName=nodeName;
   iter=0;
   iter=fUnpackerList->MakeIterator();
  isDumped=kFALSE;   
}



HldSockRemoteSource::~HldSockRemoteSource(void) {
  //Destructor for a remote data source
   if (fUnpackerList) fUnpackerList->Delete();
  fUnpackerList=0;
  if (fReadEvent) delete fReadEvent;
  fReadEvent=0;
  if(iter) delete iter;
}


Bool_t HldSockRemoteSource::init(void) {
  //Calls the init() function for each unpacker.
 if (!(*fEventAddr)) {
   (*fEventAddr) = new HRecEvent;
 }
 if(isDumped) return kTRUE;
 iter=fUnpackerList->MakeIterator();
 if (initUnpacker()==kFALSE) {
      Error("init()","error in initialisation of  unpackers \n");
    return kFALSE;
 }
 HldUnpack* unpacker;
 while ( (unpacker=(HldUnpack *)iter->Next())!=NULL) {
       fReadEvent->appendSubEvtIdx(*unpacker);      
 }  
 return kTRUE;  
}


EDsState HldSockRemoteSource::getNextEvent(Bool_t doUnpack) {
 // test Server IO
if(isDumped) return dumpEvt();

 iter->Reset();
 HldUnpack *unpacker;
 if (!fReadEvent) return kDsError;
  if (fReadEvent) fReadEvent->execute();
  decodeHeader((*fEventAddr)->getHeader());
  while ( (unpacker=(HldUnpack *)iter->Next())!=NULL) {
    if (!unpacker->execute()) return kDsError;
  }
return kDsOk;
}

Int_t HldSockRemoteSource::getCurrentRunId(void) {
  ((HldSockRemotEvt*)fReadEvent)->read();
  return fReadEvent->getRunNr();
}
