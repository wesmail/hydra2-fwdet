#ifndef COMPUNPACKER_H
#define COMPUNPACKER_H

#ifndef __CINT__

#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>

#include "TCanvas.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TMath.h"
#include "TPaveText.h"
#include "TROOT.h"
#include "TObject.h"
#include "TError.h"

//#include "TRFIOFile.h"

#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TStyle.h"
#include "TTree.h"
#include "TUnixSystem.h"

#include "hades.h"
#include "hspectrometer.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hldfilesource.h"
#include "hldgrepfilesource.h"
#include "hmatrixcategory.h"
#include "hparasciifileio.h"
#include "hrecevent.h"
#include "hruntimedb.h"
#include "hldsockremotesource.h"
#include "htree.h"

#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdccal2.h"
#include "hmdccal2sim.h"
#include "hmdccalibrater1.h"
#include "hmdcclus.h"
#include "hmdcdetector.h"
#include "hmdchit.h"
#include "hmdchitsim.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdctaskset.h"
#include "hmdcunpacker.h"
#include "horainfo.h"
#include "hparoraio.h"
#include "hparrootfileio.h"
#include "hsudummyrec.h"
#include "htask.h"
#include "htaskset.h"
#include "htofraw.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "htofinocal.h"
#include "htofinocalsim.h"
#endif

enum modes {OFF=0,ORAFILE=1,ASCIIFILE=2,ROOTFILE=3,RAWLEVEL=1,CALLEVEL=2};

void unpacker();
void unpacker(Char_t *);
void unpacker(Char_t *, Char_t *, Int_t);
void unpacker(Char_t *, Char_t *, Char_t *, Int_t nev=0, UInt_t sev=0, Bool_t RemoteSource=kFALSE);

Char_t *myexec(Char_t * in);

void evalStopwatch(TStopwatch *, Int_t);
HParOraIo* setParameterSource(HRuntimeDb* , Int_t , Int_t );
void cutEnd(Char_t *, Char_t *);
Int_t getExistingRefRunId(TString inputfile, TString beamtime, Int_t primaryFile, Int_t secondaryFile, Int_t refRunId, HParOraIo *oracle);
#endif
