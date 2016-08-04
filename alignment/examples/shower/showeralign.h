#ifndef HANALYSISDST_H
#define HANALYSISDST_H
      
#ifdef __CINT__
#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1
#endif

#ifndef __CINT__
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <stdlib.h>

#include <TCanvas.h>
#include <TCutG.h>
#include <TFile.h>
#include <TH1.h>
#include <TH2.h>
#include <TLatex.h>
#include <TMath.h>
#include <TPad.h>
#include <TPaveText.h>
#include <TROOT.h>
//#include <TRFIOFile.h>
#include <TStopwatch.h>
#include <TString.h>
#include <TStyle.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TUnixSystem.h>

#include "hrpccluster.h"

#include "hades.h"
#include "haddef.h"
#include "heventheader.h"
#include "hiterator.h"
#include "hmatrixcategory.h"
#include "hrecevent.h"
#include "hpartialevent.h"
#include "htree.h"

#include "hrichdetector.h"
#include "hmdcdetector.h"
#include "htofdetector.h"
#include "hrpcdetector.h"
#include "hshowerdetector.h"
#include "hstartdetector.h"
#include "hlatchunpacker.h"
#include "htboxdetector.h"
#include "hrpcgeompar.h"
#include "hparora2io.h"

#include "hrichtaskset.h"

#include "hmdctaskset.h"
#include "hmdctrackdset.h"
#include "hshowertaskset.h"
#include "hstarttaskset.h"
#include "htoftaskset.h"

//#include "hqamaker.h"

#include "hmdcunpacker.h"
#include "htofunpacker.h"
#include "hshowerunpacker.h"
#include "hstartunpacker.h"

#include "hspectrometer.h"

#include "showerdef.h"
#include "rpcdef.h"
#include "hstartdef.h"
#include "tofdef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "hmdctrackddef.h"

#include "hshowerhittof.h"
#include "hldfilesource.h"
#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdccal2.h"
#include "hmdccal2sim.h"
#include "hmdcclus.h"
#include "hmdchit.h"
#include "hmdchitsim.h"
#include "hmdcseg.h"
#include "hmdcsegsim.h"
#include "hmdcsetup.h"
#include "hmdcvertexfind.h"
#include "hmdcbitflipcor.h"
#include "hsplinetaskset.h"
#include "hrktrackB.h"
#include "hmdctrackgdef.h"
#include "hparoraio.h"
#include "hparrootfileio.h"
#include "hparasciifileio.h"
#include "hruntimedb.h"
#include "htofraw.h"
#include "htofhit.h"
#include "htofhitsim.h"
#include "hmagnetpar.h"
#include "hmdcwirefit.h"
#include "hmdcclusinf.h"
#include "hmdcclusfit.h"
#include "htool.h"
#include "hrootsource.h"
#include "hrktrackB.h"
#include "hmdcgeompar.h"
#include "htofgeompar.h"
#include "hshowergeometry.h"
#include "hgeomvector.h"
#include "hgeomrotation.h"
#include "hgeomvolume.h"
#include "hdetgeompar.h"
#include "hgeomcompositevolume.h"
#include "hmdcsizescells.h"
#include "TPolyMarker3D.h"
#include "TPolyLine3D.h"
#include "TView.h"
#include "hmdclayergeompar.h"
#include "hmdcgetcontainers.h"

#include "hqamaker.h"
#endif

#endif
