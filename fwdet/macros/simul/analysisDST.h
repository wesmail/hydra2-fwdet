#ifndef HANALYSISDST_H
#define HANALYSISDST_H
      
#ifdef __CINT__
#define EXIT_SUCCESS  0
#define EXIT_FAILURE  1
#endif

#ifndef __CINT__
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
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
#include <TStopwatch.h>
#include <TString.h>
#include <TStyle.h>
#include <TNtuple.h>
#include <TTree.h>
#include <TUnixSystem.h>

#include "hades.h"
#include "haddef.h"
#include "hrootsource.h"
#include "hevent.h"
#include "hcategory.h"
#include "htree.h"
#include "htask.h"
#include "htaskset.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#ifdef ORACLE_SUPPORT
#include "hparora2io.h"
#endif
#include "hparrootfileio.h"
#include "hparasciifileio.h"

#include "fwdetdef.h"
#include "hfwdetdetector.h"
#include "hfwdettaskset.h"

#endif

#endif
