// File: hldremotevt.cc
// Authors: H.G & D.B
// Modified by D.B : 02-12-98
// Modified by D.B : 19-03-99
// Modified by D.B : 29-08-99 

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////
//HldRemotEvent
//
//  Class used for reading LMD events from file
//
/////////////////////////////////////////////////////

#include "hldsockremotevt.h"
#include <iomanip> 
#include <iostream> 
#include <unistd.h>
#include <time.h>
#include <cstdlib>

#include "TError.h"


//ClassImp(HldRemotEvt) 

HldSockRemotEvt::HldSockRemotEvt(
const Char_t* node, const Int_t port, const Int_t nevt,Int_t iMode){

 pHdr = (UInt4*) &hdr;
 pData=0;
 lastSubEvtIdx = 0;
 pcNode=node;
 pRevBuf = new HRevBuffer(iMode);
 
 Int_t count =0; 
 while(1){  
 pSocket= pRevBuf->RevOpen(node,port,nevt);
 count++; 
 if( !pSocket) {
         cout <<  "-E- connecting to Remote Event server: " << node << endl;
         cout <<  " trying to connect again...  " << endl;   
         pRevBuf->RevClose(pSocket);
         sleep(5);
	 if (count>10) {
	   cout << " -E- no connection to server on node: " << node << endl; 
           exit(1); 
         }
	 }else{ break;}
 }
}
 

HldSockRemotEvt::~HldSockRemotEvt(){
   if(pSocket) sClose((TSocket*) pSocket);
   if(pRevBuf) delete pRevBuf;   
}



UInt4 *HldSockRemotEvt::getBuffer(TSocket *aSocket, Int_t iFlush){
return pRevBuf->RevGet(aSocket,iFlush);
} //getBuffer

UInt_t  HldSockRemotEvt::getLen(){ //Data Lenght (bytes)
return pRevBuf->RevBufsize();
}


Bool_t HldSockRemotEvt::read() {
pData=0;
// takes the current buffer  
UInt4* pevt=getBuffer(pSocket,0);
if (pevt){
          pHdr = pevt;
          }else{return kFALSE;}
if(pHdr){
     if (isSwapped()) swapHdr();
     if (getSize() > getHdrSize()) {
       pData  = pHdr + getHdrLen();
       if (pData) return kTRUE;
     }
}
return kFALSE;
}

Bool_t HldSockRemotEvt::readSubEvt(size_t i)  {
  UInt4* p;
   if (i)
     p = subEvt[i-1].getPaddedEnd();
   else
     p = pData;
   if (p < getPaddedEnd())
     subEvt[i] = HldSubEvt(p);
   else
     return kFALSE;
   return kTRUE;
}
  


Bool_t HldSockRemotEvt::execute(){
  if(read()){ 
   for (size_t idx = 0; idx < lastSubEvtIdx; idx++)    *subEvtTable[idx].p = 0;
     for (size_t i = 0; i < maxSubEvts && readSubEvt(i); i++){
      for (size_t idx = 0; idx < lastSubEvtIdx; idx++) {
	//	  cout << "\tsubEvt["<<i<<"].getId()= " << subEvt[i].getId() 
	//	 << " table= " << subEvtTable[idx].id << endl;
     
       if (subEvt[i].getId() == subEvtTable[idx].id) {
          subEvt[i].swapData();
          *subEvtTable[idx].p = subEvt + i;
	  //   cout << "Put Subevt into table" << endl;
          }
       }
     }
    return kTRUE;
  } else {
 return kFALSE;
 }
}

void HldSockRemotEvt::sClose( TSocket *pSocket ){
pRevBuf->RevClose(pSocket);  
}


Bool_t HldSockRemotEvt::swap(){
  //Only test the swap byte order in the event  
  if(read()){ 
    return kTRUE;
  } else {
 return kFALSE;
 }
}

