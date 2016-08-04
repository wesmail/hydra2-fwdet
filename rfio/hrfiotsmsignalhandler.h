//////////////////////////////////////////////////////
//  HRFIOTsmSignalHandler
//
//  Signal handler for RFIO files.
//  Needed to close serversession on taperobot on abnormal program termination.
//
//////////////////////////////////////////////////////
#ifndef HRFIOTSMSIGNALHANDLER_H
#define HRFIOTSMSIGNALHANDLER_H

#include <signal.h>
#include "rawapin.h"
#include "TObject.h"

class HRFIOTsmSignalHandler: public TObject
{
 protected:
		static RFILE *fFile; //! file to be closed on signal
		struct sigaction newAction[10];   //! Array of signal handlers
		static struct sigaction oldAction[10]; //! Array of signal handlers

 public:
		HRFIOTsmSignalHandler(RFILE **file = 0);
		~HRFIOTsmSignalHandler(void);

 protected:
		void registerSignal(void);
		void restoreSignal(void);
		static void handleSignal(Int_t sig);
ClassDef(HRFIOTsmSignalHandler,0)
};

#endif
