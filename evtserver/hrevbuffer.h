//////////////////////////////////////////////////////////////////////
// Copyright:
//   GSI, Gesellschaft fuer Schwerionenforschung mbH
//   Planckstr. 1
//   D-64291 Darmstadt
//   Germany
//////////////////////////////////////////////////////////////////////
// HRevBuffer.h
// description of class HRevBuffer
// client package for multithreaded Hades remote event server
// created 14. 5.1999 by Horst Goeringer
//////////////////////////////////////////////////////////////////////

#ifndef HRevBuffer_H
#define HRevBuffer_H

#include <iostream>
#include "TObject.h"
#include "TSocket.h"

class HRevBuffer : public TObject
{
   private:
      TSocket *pTSocket;     // ptr socket of channel to event server
      Int_t  iSocket;        // socket id of channel to event server
      Int_t  iDebug;         // verbosity level (test)
      Int_t  iBufNo1;        // first buffer no. received (test)
      Int_t  iBufNo2;        // last buffer no. received (test)
      Int_t  iSwap;          // > 0: swap event data

      Int_t  iBufSizeAlloc;  // allocated buffer size
      Int_t  iBufSize;       // size current buffer
      Int_t  iBufNo;         // current buffer no.
      Int_t  iHeadPar;       // no. of (4 byte) parms buffer header
      Int_t  iEvtMax;        // no. of events requested
      Int_t  iEvtNo;         // last event no. handled (absolute)
      Int_t  iEvtRel;        // last event no. handled (in cur. buffer)
      Int_t  iEvtBuf;        // no. of events in current buffer
      Int_t  iEvtPar;        // no. of parameters in event (incl header)
      Int_t  *piBuf;         // ptr event buffer
      Int_t  *piNextEvt;     // ptr first element next event

   public:
      HRevBuffer( Int_t iMode); // constructor
      ~HRevBuffer();            // destructor

      TSocket *RevOpen(const Char_t *pNode, Int_t iPort, Int_t iEvent);
          // input: node name and port number server, req. no. of events
          // returns Socket ptr of server connection

      UInt_t *RevGet( TSocket *pSocket, Int_t iFlush);
          // get next event (pointer) from buffer, input:
          // Socket ptr, 
          // iFlush = 1: skip current buffer (not impl)

      Int_t RevBufsize();
          // get size of current buffer (byte)

      void RevBufWait(Int_t iWait);
          // wait for iWait seconds

      void  RevClose( TSocket *pSocket );           // input Socket ptr 

   ClassDef(HRevBuffer, 0)        // prototype for remote event buffer
};

#endif     // !HRevBuffer_H 
