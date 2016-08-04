//*-- Author : J.Wuestenfeld
//*-- Modified : 06/24/2004 by J.Wuestenfeld
//*-- Modified : 04/06/2005 by J.Wuestenfeld

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////
//HRFIOTsmSignalHandler
//
//  Class to close RFIO files on occurence of signal.
//  Needed to terminate serversession on taperobot.
//
/////////////////////////////////////////////////////

#include "hrfiotsmsignalhandler.h"
#include <stdlib.h>
#include <iostream>
#include "TSystem.h"
using namespace std;

ClassImp(HRFIOTsmSignalHandler)

RFILE *HRFIOTsmSignalHandler::fFile;
struct sigaction HRFIOTsmSignalHandler::oldAction[10];

HRFIOTsmSignalHandler::HRFIOTsmSignalHandler(RFILE **file)
{
	// Default constructor. Set new signal handlers.
	//
	// Input parameters:
	//   RFILE** file
	//     Pointer to RFIO file object.
	//
	// Output parameters:
	//   none
	//
	// Return code:
	//   none
	fFile = *file;
	registerSignal();
}

HRFIOTsmSignalHandler::~HRFIOTsmSignalHandler(void)
{
	// Default destructor. Restore original signal handlers.
	//
	// Input parameters:
	//   none
	//
	// Output parameters:
	//   none
	//
	// Return code:
	//   none
	restoreSignal();
}

void HRFIOTsmSignalHandler::handleSignal(Int_t sig)
{
	// Static function, installed as signalhandler.
	//	Handles the following signals:
	// SIGHUP, SIGINT, SIGQUIT, SIGILL, SIGABRT, SIGBUS,
	// SIGSEGV, SIGTERM, SIGSYS
	// Closses the open RFIO file, and calls the previously installed signalhandler.
	//
	// Input parameters:
	//   Int_t sig
	//      signal for which to set new handler.
	//
	// Output parameters:
	//   none
	//
	// Return code:
	//   none
  cout << "Caught signal "<< sig << ", cleaning up!" << endl;
	cout << "Closing open RFIO file!" << endl;
	sigaction(SIGHUP, &oldAction[0], 0);
	sigaction(SIGINT, &oldAction[1], 0);
	sigaction(SIGQUIT, &oldAction[2], 0);
	sigaction(SIGILL, &oldAction[3], 0);
	sigaction(SIGABRT, &oldAction[4], 0);
	sigaction(SIGBUS, &oldAction[5], 0);
	sigaction(SIGKILL, &oldAction[6], 0);
	sigaction(SIGSEGV, &oldAction[7], 0);
	sigaction(SIGTERM, &oldAction[8], 0);
	sigaction(SIGSYS, &oldAction[9], 0);
	switch(sig)
		{
		case SIGHUP:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[0].sa_handler > SIG_IGN)
						oldAction[0].sa_handler(sig);
				}
			break;
		case SIGINT:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[1].sa_handler  > SIG_IGN)
						oldAction[1].sa_handler(sig);
				}
			break;
		case SIGQUIT:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[2].sa_handler  > SIG_IGN)
						oldAction[2].sa_handler(sig);
				}
			break;
		case SIGILL:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[3].sa_handler  > SIG_IGN)
						oldAction[3].sa_handler(sig);
				}
			break;
		case SIGABRT:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[4].sa_handler  > SIG_IGN)
						oldAction[4].sa_handler(sig);
				}
			break;
		case SIGBUS:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[5].sa_handler  > SIG_IGN)
						oldAction[5].sa_handler(sig);
				}
			break;
		case SIGKILL:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[6].sa_handler  > SIG_IGN)
						oldAction[6].sa_handler(sig);
				}
			break;
		case SIGSEGV:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[7].sa_handler  > SIG_IGN)
						oldAction[7].sa_handler(sig);
				}
			break;
		case SIGTERM:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[8].sa_handler  > SIG_IGN)
						oldAction[8].sa_handler(sig);
				}
			break;
		case SIGSYS:
			if(fFile)
				{
					rfio_fclose(fFile);
					fFile = NULL;
					if(oldAction[9].sa_handler  > SIG_IGN)
						oldAction[9].sa_handler(sig);
				}
			break;
		default:
			break;
		}
	exit(1);
}

void HRFIOTsmSignalHandler::registerSignal(void)
{
	// Internal function. Registers the signalHandler function with the apropriate signals.
	//
	// Input Parameters:
	//   none
	//
	// Output parameters:
	//   none
	//
	// Return code:
	//   none
	for(Int_t i=0;i<10;i++)
		{
			newAction[i].sa_handler = handleSignal;
			for(Int_t j=0;j<32;j++)
				newAction[i].sa_mask.__val[j] = 0UL;
			newAction[i].sa_flags =0;
		}
    gSystem->ResetSignal(kSigBus,kTRUE);
    gSystem->ResetSignal(kSigSegmentationViolation,kTRUE);
    gSystem->ResetSignal(kSigSystem,kTRUE);
    gSystem->ResetSignal(kSigIllegalInstruction,kTRUE);
    gSystem->ResetSignal(kSigPipe,kTRUE);
    gSystem->ResetSignal(kSigAlarm,kTRUE);
    gSystem->ResetSignal(kSigUrgent,kTRUE);
    gSystem->ResetSignal(kSigFloatingException,kTRUE);
		gSystem->ResetSignal(kSigInterrupt,kTRUE);
	sigaction(SIGHUP, &newAction[0],&oldAction[0]);
	sigaction(SIGINT, &newAction[1],&oldAction[1]);
	sigaction(SIGQUIT, &newAction[2],&oldAction[2]);
	sigaction(SIGILL, &newAction[3],&oldAction[3]);
	sigaction(SIGABRT, &newAction[4],&oldAction[4]);
	sigaction(SIGBUS, &newAction[5],&oldAction[5]);
	sigaction(SIGKILL, &newAction[6],&oldAction[6]);
	sigaction(SIGSEGV, &newAction[7],&oldAction[7]);
	sigaction(SIGTERM, &newAction[8],&oldAction[8]);
	sigaction(SIGSYS, &newAction[9],&oldAction[9]);
}

void HRFIOTsmSignalHandler::restoreSignal(void)
{
	// Internal function. Restores the old signal handlers.
	//
	// Input Parameters:
	//   none
	//
	// Output parameters:
	//   none
	//
	// Return code:
	//   none
	sigaction(SIGHUP, &oldAction[0], 0);
	sigaction(SIGINT, &oldAction[1], 0);
	sigaction(SIGQUIT, &oldAction[2], 0);
	sigaction(SIGILL, &oldAction[3], 0);
	sigaction(SIGABRT, &oldAction[4], 0);
	sigaction(SIGBUS, &oldAction[5], 0);
	sigaction(SIGKILL, &oldAction[6], 0);
	sigaction(SIGSEGV, &oldAction[7], 0);
	sigaction(SIGTERM, &oldAction[8], 0);
	sigaction(SIGSYS, &oldAction[9], 0);
}
