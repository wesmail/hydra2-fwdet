//

//////////////////////////////////////////////////////
//  HldRemotEvent
//
//  Class for reading events from RPC buf.
//
//////////////////////////////////////////////////////


#ifndef HLDREMOTEVT_H
#define HLDREMOTEVT_H

#include "rpcbuf.h"
#include "hldevt.h"
#include "hldsubevt.h"
#include "hldunpack.h"


class HldRemotEvt: public HldEvt {

private:
const  Char_t* pcNode;            // server node name
void*  rpcHandle;                     // RPC handle for remote access
rpcbuf* rpcBuf;                        // ptr to buffer provided by RPC
Int_t  iEvtNum;                        // last event no. handled

public:
HldRemotEvt(const Char_t* node); // init and open rpc connection
~HldRemotEvt(void){;}

Bool_t  open(const Char_t *pcnode); // open connection to server node
Bool_t  getBuffer(void* handle); // buffer
UInt_t getLen(void);
Bool_t read(void);
Bool_t readSubEvt(size_t i);
Int_t reGetValue(Int_t iNumber);
Bool_t execute(void);
Bool_t swap(void);
public:
//ClassDef(HldRemotEvt, 0)           // HADES LMD Event
};
#endif /* !HLDFILEVT_H */  

