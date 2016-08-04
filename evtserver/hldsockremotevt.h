//////////////////////////////////////////////////////
//  HldRemotEvent
//
//  Class for reading events from RPC buf.
//
//////////////////////////////////////////////////////


#ifndef HLDSOCKREMOTEVT_H
#define HLDSOCKREMOTEVT_H

#include "TSocket.h"
#include "hldevt.h"
#include "hldsubevt.h"
#include "hldunpack.h"
#include "hrevbuffer.h"

class HldSockRemotEvt: public HldEvt {

private:
TString  pcNode;           // remote event server node name
TSocket *pSocket;          //socket ptr to server connection
HRevBuffer *pRevBuf;       // ptr to an Remote event buffer class

public:
 // init and open rpc connection
HldSockRemotEvt(const Char_t* node, const Int_t port,
            const Int_t nevt,Int_t iMode=0); 
~HldSockRemotEvt(void); 

void   sClose(TSocket* pSocket); // close connections
 
UInt4 *getBuffer(TSocket *pSocket, Int_t iFlush=0); // buffer
UInt_t getLen(void);
Bool_t read(void);
Bool_t readSubEvt(size_t i);
Bool_t execute(void);
Bool_t swap(void);

public:
//ClassDef(HldSockRemotEvt, 0)           // HADES LMD Event
};
#endif /* !HLDFILEVT_H */  

