//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////
// Copyright:
//   GSI, Gesellschaft fuer Schwerionenforschung mbH
//   Planckstr. 1
//   D-64291 Darmstadt
//   Germany
//////////////////////////////////////////////////////////////////////
// HRevBuffer.cc
// ROOT client package for multithreaded Hades remote event server
// created 14. 5.1999 by Horst Goeringer
//////////////////////////////////////////////////////////////////////
//
//  Imported into Hydra as hrevbuffer.cc and hrevbuffer.h by
//  Simon Lang, 08.12.05
//
//////////////////////////////////////////////////////////////////////

#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/select.h>

using namespace std;

#include "hrevbuffer.h"           // class definition

ClassImp(HRevBuffer)

Int_t iTimeOut;        // needed in exitCli to handle CTL C
Int_t imySig = 0;      // needed in exitCli to handle CTL C
Int_t iOutMode = 0;    // needed in exitCli to handle CTL C

/* client requests events from server */
typedef struct
{
   Int_t iSize; /* size of comm. buffer without this element (bytes) */
   Int_t iMode;                  /* must be 1: client requests data  */
   Int_t iIdent;/* =1: NOT IN NET REPR. to identify client endian type */
   Int_t iBufRequ;                    /* number of buffers requested */
} srevComm;

/* server sends info to client */
typedef struct
{
   Int_t iSize;   /* size of info buffer without this element (bytes) */
   Int_t iMode;                  /* must be 1: server sends info */
                               /* else: client tries swap */
   Int_t iHeadPar;               /* no. of header parms in buffer */
   Int_t iTimeOut;               /* wait time client after CTL C */
} srevInfo;

/* server sends dummy buffer to client, if no DAQ events available */
typedef struct
{
   Int_t iSize;                             /* size of data following */
   Int_t iBufNo;                             /* current buffer number */
   Int_t iEvtNo;                     /* no. of events in buffer (= 0) */
} sptrevDummy;


static void exitCli(Int_t signal)                /* handles Ctl C */
{
   Int_t iSleep;                                 /* sleep time here */
   Int_t iSocket;

   iSocket = imySig;
   imySig  = -1;
   iSleep  = iTimeOut;

   printf("\n-I- user specified CTL C: ");
   if (iSocket > 0)
   {
      if (iOutMode)
         printf("close connection (socket %d), hold client for %d s\n",
                 iSocket, iSleep);
      else printf("close connection\n");

      if (iSleep) sleep(iSleep);
   }
   else printf("\n");
}


static Int_t rclose(Int_t *piSocket, Int_t iMode)
{
   Int_t iSocket;
   Int_t iRC;
   Int_t iDebug = 0;
   Int_t iError = 0;
   Int_t iClose = 1;
   Char_t cModule[32] = "rclose";
   Char_t cMsg[128] = "";

   if (iMode < 0)
   {
      iMode = -iMode;
      iClose = 0;                      /* no shutdown, only close */
   }
   if ( (iMode < 0) || (iMode > 3) )
   {
      if (iClose == 0) iMode = -iMode;
      printf("-E- %s: invalid shutdown mode: %d\n", cModule, iMode);
      iError = 2;
   }

   iSocket = *piSocket;
   if (iSocket > 0)
   {
      if (iMode < 3)
      {
         iRC = shutdown(iSocket, iMode);
         if (iRC)
         {
            sprintf(cMsg, "-E- %s: shutdown(%d) rc = %d", 
                    cModule, iMode, iRC);
            perror(cMsg);
            iError = -1;
         }
         else if (iDebug)
            printf("    %s: shutdown(%d) successfull\n",
                   cModule, iMode);    
      }

      if (iClose)
      {
         iRC = close(iSocket);
         if (iRC)   
         {
            sprintf(cMsg, "-E- %s: close rc = %d", cModule, iRC);
            perror(cMsg);
            iError = -2;
         }
         else if (iDebug)
            printf("    %s: connection closed\n", cModule);

      } /* (iClose) */

   } /* (iSocket > 0) */
   else
   {
      printf("-E- %s: invalid socket: %d\n", cModule, iSocket);
      iError = 1;
   }

   *piSocket = 0;
   return(iError);
}



static Long_t swaplw(Long_t *pp_source, Long_t l_len, Long_t* pp_dest)
{
   UChar_t *p_source, *p_dest, *p_s, *p_d;
   ULong_t lu_save;

   /* +++ action +++ */
   p_source = (UChar_t *) pp_source;
   p_dest   = (UChar_t *) pp_dest;

   if ( !p_dest)
   {
                                         /* source == destination */
         for (p_d = (UChar_t *) p_source,
              p_s = (UChar_t *) &lu_save;
              p_d < p_source + (l_len * 4);
              )
         {
             lu_save = *( (Long_t *) p_d);
             p_s    += 4;                 /* increment source      */
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
         }
   } else{
                                          /* source != destination */
         for (p_s = (UChar_t *) p_source,
              p_d = (UChar_t *) p_dest;
              p_s < p_source + (l_len * 4);
              p_s += 4)
         {
             p_s     += 4;                /* increment source      */
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
             *(p_d++) = *(--p_s);
         }

   }

   return(1);
}



HRevBuffer::HRevBuffer(Int_t iMode)
{
   iSwap = 0;
   iSocket = 0;
   iEvtPar = 0;
   iBufSize = 0;

   signal(SIGINT, &exitCli);                   // from now catch Ctl C

   iDebug = iMode;
   iOutMode = iMode;
   if (iDebug == 1)
      cout << "-I- client runs in debug mode (1)" << endl;
   else if (iDebug == 2)
      cout << "-I- client shows buffer numbers and select/receive (mode 2)"
           << endl;
   else if (iDebug == 3)
      cout << "-I- client shows buffer numbers (mode 3)" << endl;

   iBufSizeAlloc = 32768;
   piBuf = new Int_t [iBufSizeAlloc/sizeof(int)];        // 32k buffer

   // cout << "    HRevBuffer() executed" << endl;

}  // constructor



HRevBuffer::~HRevBuffer()
{
   // cout << "    ~HRevBuffer() ..." << endl;
   delete [] piBuf;
   piNextEvt = 0;
   // cout << "    ~HRevBuffer() executed" << endl;

} // destructor



TSocket *HRevBuffer::RevOpen (const Char_t *pNode, Int_t iPort, Int_t iEvent)
{
   if (iEvent < 0)
   {
      cout << "-E- number of requested events (" << iEvent
           << ") invalid" << endl;
      return(0);
   }
   iEvtMax = iEvent;
   iEvtNo = 0;   
   iBufNo = 0;   
   iBufNo1 = 0;   
   iBufNo2 = 0;   

   if (!iSocket)   
   {
      iEvtNo = -1;            // initialization (info buffer) required
      if (iPort == 0) iPort = 7031;               // default for Hades 
      cout << "-I- open connection to server " << pNode 
           << ":" << iPort << endl;

      pTSocket = new TSocket(pNode, iPort);
      if ( !pTSocket->IsValid() )
      {
         cout << "-E- open connection to server " << pNode 
              << " failed" << endl;
         return(0);
      }
      cout << "    connection to server " << pNode 
           << ":" << iPort << " okay" << endl;

      iSocket = pTSocket->GetDescriptor();
      imySig = iSocket;
      if (iDebug == 1)
         cout << "    new socket " << iSocket << endl;
   }
   else if (iDebug == 1)
      cout << "-D- still socket " << iSocket << endl;

   return(pTSocket);

} // RevOpen



UInt_t *HRevBuffer::RevGet(TSocket *pSocket, Int_t iFlush)
{
   Int_t iint = sizeof(int);
   Int_t ibyte, ilen, inew = 0;
   Int_t imaxSE = 20;
   Int_t indSize[20];                                 // index imaxSE
   Int_t ioff=0, irem, ind, ii, jj;

   Char_t cMsg[128] = "";
   Char_t *pcBuf;
   Int_t iSize, iRC;

   Int_t iError = 0;
   Int_t iRetry;
   Int_t iRetryMax = 1000;
   Int_t iRetryFirst;
   Int_t iRetryRecv = 0;         // count retries of recv call
   Int_t iRetryRecvLim = 1;      // max. no. of succeeding retries

   Int_t *piComm;
   srevComm sComm;
   Int_t iCommSize = sizeof(sComm);      // size comm. buffer (byte)

   Int_t *piInfo;
   srevInfo sInfo;
   Int_t iInfoSize = sizeof(sInfo);      // size info buffer (byte)
   Int_t iBufNoServ;                     // buffer no., from server

   if (iEvtNo >= iEvtMax) goto gEndGet;

   piComm = &(sComm.iSize);              // communication buffer
   sComm.iSize = htonl(iCommSize-iint);  // size of data following 
   sComm.iMode = htonl(1);               // required: get events
   sComm.iIdent = 1;                     // required: tell endian type
   sComm.iBufRequ = htonl(1);            // send one buffer

   // initialize communication with server
   if (iEvtNo == -1)
   {
      if (iDebug == 1)
         cout << "-D- commbuf (data size " << ntohl(sComm.iSize)
              << " byte): mode(1) " << ntohl(sComm.iMode)
              << ", request " <<  ntohl(sComm.iBufRequ) 
              << " event buffer(s)" << endl;

      // request event buffer from server
      ilen = pSocket->SendRaw(piComm, iCommSize, kDefault);
      if (ilen < 0)
      {
         cout << "-E- sending request for events to server, rc = "
              << ilen << endl;
         iError = 1;
         goto gEndGet;
      }

      if (iDebug == 1)
         cout << "    communication buffer sent (request info buffer) "
              << endl;

      // receive info buffer from server
      piInfo = &(sInfo.iSize);
      ilen = pSocket->RecvRaw(piInfo, iInfoSize, kDefault);
      if (ilen < 0)
      {
         cout << "-E- receiving info buffer from server, rc = "
              << ilen << endl;
         iError = 1;
         goto gEndGet;
      }

      iHeadPar = ntohl(sInfo.iHeadPar);
      iTimeOut = ntohl(sInfo.iTimeOut);
      if (iDebug == 1)
      {
         cout << "-D- info buffer received:" << endl;
         cout << "    size data " << ntohl(sInfo.iSize)
              << ", mode (1) " << ntohl(sInfo.iMode)
              << ", header parms " << iHeadPar
              << ", timeout " << iTimeOut << endl;
      }

      if ( (ntohl(sInfo.iMode) != 1) ||
           ( (int) ntohl(sInfo.iSize) != iInfoSize-iint) )
      {
         cout << "-E- invalid info buffer received: " << endl;
         cout << "    size data ( " << iInfoSize-iint
              << ") "  << ntohl(sInfo.iSize)
              << ", mode (1) " << ntohl(sInfo.iMode)
              << ", header parms " << iHeadPar
              << ", timeout " << iTimeOut << endl;
         iError = 1;
         goto gEndGet;
      }

      iEvtNo = 0;                                // initilization done
      inew = 1;                                  // request new buffer

   } // (iEvtNo == -1)
   else
   {
      if (iFlush)
      {
         inew = 1;                            // request new buffer
         if (iDebug == 1)
            cout << "-D- skip current buffer" << endl;
      }
      else
      {
         if (iEvtNo >= 0)
         {
            // check if new buffer needed
            if (iEvtRel >= iEvtBuf)
            {
               if (iDebug == 1)
                  cout << "    request new buffer" << endl;
               inew = 1;
            }
            else inew = 0;

         } // (iEvtNo > 0)
      } // (!iFlush)
   } // (iEvtNo != -1)

   // request new buffer
   if (inew)
   {
gRetryBuf:
      iEvtRel = 0;
      iRetry = 0;
      iRetryFirst = 1;
      if (imySig == -1)
         sComm.iBufRequ = htonl(0);         // signal finish to server

      // request next buffer or finish
      ilen = pSocket->SendRaw(piComm, iCommSize, kDefault);
      if (ilen < 0)
      {
         cout << "-E- sending request for buffer " << iBufNo+1 
              << " to server, rc = " << ilen << endl;
         iError = 1;
         goto gEndGet;
      }

      if (imySig == -1) goto gEndGet;

      if (iDebug == 1)
         cout << "-D- communication buffer sent (request next buffer) "
              << endl;

gRetryLen:
      // get size of data following
      piBuf[0] = -1;                         // enables receive check
      iSize = iint;
      pcBuf = (Char_t *) piBuf;
      while(iSize > 0)
      {
         if ( (imySig == -1) && (iDebug) )
            cout << "    CTL C detected (before recv len)" << endl;
gNextRecvL:
         iRC = recv(iSocket, pcBuf, iSize, 0);
         if (iRC < 0)
         {
            if (imySig == -1)
            {
               if (iDebug)
               {
                 sprintf(cMsg, "\n-E- receiving data length from server");
                 perror(cMsg);
                 cout << "    CTL C detected (during recv len)" << endl;
               }
               goto gNextRecvL;
            }
            else
            {
               sprintf(cMsg, "\n-E- receiving data length from server");
               perror(cMsg);
               if (iDebug) cout << "    retry" << endl;

               iRetryRecv++;      // count no. of retries to limit them
               if (iRetryRecv > iRetryRecvLim)  // avoid infinite loop 
               {
                  iError = 1;
                  goto gEndGet;
               }
               else goto gNextRecvL;
            }
         }
         if ( iRC == 0 )
         {
            if ( (iDebug == 2) || (iDebug == 3) )
               cout << endl;
            cout << "-E- receiving data length: connection closed by server"
                 << endl;
            iError = 1;
            goto gEndGet;
         }

         iRetryRecv = 0;
         iSize -= iRC;
         pcBuf += iRC;

      } /* while(iSize > 0) */

      if (iDebug == 2)
      {
         printf("Rl:");
         fflush(stdout);
      }

      if ( (imySig == -1) && (iDebug) )
         cout << "    CTL C detected (after recv len)" << endl;

      iBufSize = ntohl(piBuf[0]);
      if (iDebug == 1)
         cout << "    data size received: " << iBufSize << endl;

      if (iBufSize <= 0)
      {
         if (iBufSize == 0)
         {
            if (iDebug)
               cout << endl;
            cout << "-W- server closed connection" << endl;
            cout << "    " << iEvtNo << " of " << iEvtMax
                 << " events received" << endl;
            iError = 1;
            goto gEndGet;
         }

         if (iBufSize == -1)
         {
            if (iRetryFirst)
            {
               cout << endl << "-E- no data length received: ";
               iRetryFirst = 0;
            }
            iRetry++;
            if (iRetry > iRetryMax)
            {
               cout << iRetryMax << "times" << endl;
               iError = 1;
               goto gEndGet;
            }
            goto gRetryLen;
         }
         else
         {
            cout << endl << "-E- invalid data length received: "
                 << iBufSize << endl;
            iError = 1;
         }

         goto gEndGet;
      }
      if (iRetry)
         cout << iRetry << "times" << endl;

      // increase data buffer, if necessary
      if (iBufSize+iint > iBufSizeAlloc)
      {
         delete [] piBuf;
         iBufSizeAlloc = iBufSize+iint;
                     // new total buffer size (including length field)
         piBuf = new Int_t [iBufSizeAlloc/iint];
         piBuf[0] = iBufSize;
                       // keep sent buffer size (without length field)
         if (iDebug == 1)
            cout << "-I- total buffer increased to "
                 << iBufSizeAlloc << " bytes" << endl;
      }

      // get event buffer without length field
      piBuf[1] = -1;                         // enables receive check
      iSize = iBufSize;
      pcBuf = (Char_t *) &(piBuf[1]);
      while(iSize > 0)
      {
         if ( (imySig == -1) && (iDebug) )
            cout << "    CTL C detected (before recv data)" << endl;
gNextRecvD:
         iRC = recv(iSocket, pcBuf, iSize, 0);
         if (iRC < 0)
         {
            if (imySig == -1)
            {
               if (iDebug)
               {
                  sprintf(cMsg, "\n-E- receiving data from server");
                  perror(cMsg);
                  cout << "    CTL C detected (during recv data)" << endl;
               }
               goto gNextRecvD;
            }
            else
            {
               sprintf(cMsg, "\n-E- receiving data from server");
               perror(cMsg);

               iRetryRecv++;      // count no. of retries to limit them
               if (iRetryRecv > iRetryRecvLim)  // avoid infinite loop 
               {
                  iError = 1;
                  goto gEndGet;
               }
               else goto gNextRecvD;
            }
         }
         if ( iRC == 0 )
         {
            if ( (iDebug == 2) || (iDebug == 3) )
               cout << endl;
            cout << "-E- receiving data: connection closed by server"
                 << endl;
            iError = 1;
            goto gEndGet;
         }

         iRetryRecv = 0;
         iSize -= iRC;
         pcBuf += iRC;

      } /* while(iSize > 0) */

      if (iDebug == 2)
      {
         printf("Rd:");
         fflush(stdout);
      }

      if (imySig == -1) 
      {
         if (iDebug)
            cout << "    CTL C detected (after recv data)" << endl;
         goto gEndGet;
      }

      iBufNoServ = ntohl(piBuf[1]);
      iEvtBuf = ntohl(piBuf[2]);
      if (iEvtBuf == 0)
      {
         if (iDebug == 1) 
            printf("    dummy buffer no. %d, %d events\n",
                   iBufNoServ, iEvtBuf);
         printf("*** connection to remote event server okay, but currently no DAQ events (%d)\n",
                iBufNoServ);
         goto gRetryBuf;
      }

      if (!iSwap)
      {
         if ( (piBuf[3] < 1) || (piBuf[3] >= iBufSize) )
            iSwap = 1;              // DDD dirty fix! check piBuf[4] !
      }

      // swap events in buffer (behind buffer header)
      if (iSwap)
      {
	 /*
	  * Hint: There are a lot of basically unnecessary type casts in 
	  * this version of swaplw. One should clean it up.
	  */
	 swaplw( (Long_t*)(&piBuf[1+iHeadPar]), (iBufSize/iint)-iHeadPar, 0);
	 if ( (iBufNo == 0) && (iDebug) )
	    cout << "    Event data swapped" << endl;
      }

      iBufNo++;
      if (iEvtNo == 0)
      {
         iBufNo = 1;                          // restart counting
         iBufNo1 = iBufNoServ;                // keep first buffer no.
      }
      iBufNo2 = iBufNoServ;                   // keep last buffer no.

      if (iDebug >= 2)
      {
         printf("%d:", iBufNoServ);
         fflush(stdout);
      }

      if (iDebug == 1)
      {
         cout << endl << "buffer " << iBufNo
              << " (" << iBufNoServ << "): "
              << " size "
              << iBufSize << " byte" << endl;
      }

      iEvtRel = 1;                      // first event in buffer
      piNextEvt = piBuf + iHeadPar+1;   // ptr first element in buffer

   } // new buffer
   else
   {
      iEvtRel++;
      piNextEvt += iEvtPar;             // skip previous event
      //printf("DDD new event length: %d\n", *piNextEvt);

      // first event after reopen existing connection, no new buffer
      if (iEvtNo == 0)
      {
         iBufNo = 1;                               // restart counting
         iBufNoServ = ntohl(piBuf[1]);
         iBufNo1 = iBufNoServ;

      } // (iEvtNo == 0)
   } // continue with current buffer

   iEvtNo++;                            // total event no.
   ibyte = (piNextEvt[0]/8)*8;
   if ( ibyte < piNextEvt[0] ) ibyte += 8;
   iEvtPar = ibyte/iint;          // no. of parameters with padding

   if (iDebug == 1)
   {
      cout << "    event no. " << iEvtNo << " (" << piNextEvt[3]
           << "): id " << piNextEvt[2] << ", size " << piNextEvt[0] 
           << endl;

      ii = 0;
      indSize[ii] = 0;                           // index event length

      irem = piNextEvt[0] - 32;       // remaining bytes for subevents
      while ( (irem > 0) && (ii < imaxSE) )
      {
         ii++;                                      // count subevents 
         if (ii == 1) ioff = 8;                   // skip event header
         else ioff += ibyte/iint;
         indSize[ii] = ioff; // buffer index of length current subevent

         ibyte = (piNextEvt[ioff]/8)*8;
         if ( ibyte < piNextEvt[ioff] )
            ibyte += 8;                        // padded subevent size
         
         if (irem <= piNextEvt[ioff])                 // last subevent
            irem -= piNextEvt[ioff];
         else irem -= ibyte;

         cout << "      subevent " << ii << ": size " 
              << piNextEvt[ioff];
         if (irem <= 0) cout << endl;
         else cout << ", padded " << ibyte
                   << ", remainder " << irem << endl;

         if (ii == imaxSE-1)
            cout << "-W- only " << ii << " subevents tested" << endl;

      } // while (irem)

      if (iDebug == 2) for (jj=1; jj<=ii; jj++)
      {
         ind = indSize[jj];
         cout << "      subevent " << jj << ": size " << piNextEvt[ind]
              << " (index " << ind << ")" << endl;
      }
   } // (iDebug == 1)

   if (imySig == -1)
   {
      cout << endl << "-D- CTL C specified";
      if (iDebug) cout << " (at end RevGet)" << endl;
      else cout << endl;
      goto gEndGet; 
   }

   if (iEvtNo == iEvtMax)
   {
      cout << endl << "-I- all required events ("
           << iEvtMax << ") received: " << iBufNo << " buffers ("
           << iBufNo1 << " - " << iBufNo2 << ")" << endl;
   }

   return( (UInt_t *) piNextEvt);

gEndGet:
   if ( (iError) || (imySig == -1) )
   {
      if (iDebug)
         cout << "    RevGet: closing connection to server";
      iRC = rclose(&iSocket, 2);
      if ( (iDebug) && (iRC == 0) )
         cout << " - done" << endl;
      imySig = 0;                             // notify CTL C handler
   }
   else if (iDebug)
      cout << "    RevGet: keeping connection to server" << endl;

   return 0 ;

} // RevGet



Int_t HRevBuffer::RevBufsize()
{
   return iBufSize;
} // RevBufsize



void HRevBuffer::RevBufWait( Int_t iWait )
{
   if (iWait > 0) sleep(iWait);
} // RevBufWait



void HRevBuffer::RevClose( TSocket *pSocket )
{
   Int_t iRC;
   Int_t *piComm;
   srevComm sComm;
   Int_t iCommSize = sizeof(sComm);       // size comm. buffer (byte)

   if (imySig < 0) return;        // CTL Y: connection closed elsewhere
   if (iSocket == 0) return;

   // tell server that no more events needed
   piComm = &(sComm.iSize);               // communication buffer
   sComm.iSize = htonl(iCommSize-sizeof(int));// size of data following
   sComm.iMode = htonl(1);                // required
   sComm.iIdent = 1;                      // required: tell endian type
   sComm.iBufRequ = htonl(0);             // no more event buffers

   if (iDebug == 1)
      cout << "-D- send communication buffer (close request, size data "
           << ntohl(sComm.iSize) << " byte): "
           << ntohl(sComm.iMode) << ", "
           << ntohl(sComm.iBufRequ) << endl;

   iRC = pSocket->SendRaw(piComm, iCommSize, kDefault);
   if (iRC < 0)
      cout << "-E- sending close request to server, rc = "
           << iRC << endl;
   else if (iDebug == 1)
      cout << "    close request sent" << endl;

   if (iDebug)
      cout << "    RevClose: closing connection to server";
   iRC = rclose(&iSocket, 2);
   if ( (iDebug) && (iRC == 0) )
      cout << " - done" << endl;

   imySig = 0;                               // notify CTL C handler
   cout << "-I- connection to server closed" << endl;

} // RevClose
