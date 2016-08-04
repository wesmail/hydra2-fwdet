//*-- Modified : 23/01/2004 A.Sadovski//*-- Modified : 04/01/2005 Jacek Otwinowski
//*-- Modified : 08/02/2005 A.Sadovski
//*-- Modified : 15/05/2006 J.Pietraszko
//*-- Modified : 21/11/2007 A.Sadovski, M.Golubeva, T.Solovieva
using namespace std;
#include <iostream>
#include <iomanip>
#include <math.h>
//---- Root includes
#include "TH2.h"
#include "TProfile.h"
#include "TROOT.h"
//---- Hydra base includes
#include "hades.h"
#include "htree.h"
#include "hcategory.h"
#include "hdatasource.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h" //asd
#include "hmessagemgr.h"
//---- Data container includes
#include "heventheader.h"
#include "hstart2cal.h"
#include "hstart2hit.h"
#include "htboxchan.h"
#include "hrichcal.h"
#include "hrichhit.h"
#include "hmdcseg.h"
#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdchit.h"
#include "hmdctimecut.h" //asd
#include "hmdccutstat.h" //asd
#include "htofhit.h"
#include "htofcluster.h"
#include "hrpchit.h"
#include "hrpccluster.h"
#include "hshowerhittof.h"
#include "hsplinetrack.h"
#include "hrktrackB.h"
#include "hbasetrack.h"
#include "hmetamatch2.h"
#include "hparticlecand.h"
//---- Category definition includes
#include "hstartdef.h"
#include "richdef.h"
#include "hmdcdef.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "showerdef.h"
#include "walldef.h"
#include "hwallraw.h"
#include "hwallhit.h"
#include "hmdctrackgdef.h"
//---- QA includes
#include "hqaoutputps.h"
#include "hqamaker.h"
#include "hqahistograms.h"
#include "hqavariations.h"
#include "hqascalers.h"

#include "hparticlecand.h"
#include "hrpcdetector.h"

#include "TF1.h"

#include "hqatreemaker.h"

#include "htime.h"

#define DTHETA 5
#define DPHI 5
//#define NSTART_STRIPS 16

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 26/04/2002 Joern Wuestenfeld
//*-- Modified : 24/01/2002
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
// HQAMaker
//
// HQAMaker is a Hydra event-loop task which iterates over events and fills
// histograms and scalers, defined in separate classes.
//
// BEGIN_HTML<b><font color="maroon">For more information, see the <a href="http://www-hades.gsi.de/computing/qa/">QA home page</a>.</font></b> END_HTML
//
//////////////////////////////////////////////////////////////////////////////

Int_t HQAMaker::cutResults[4] = {0, 0, 0, 0};

ClassImp(HQAMaker)

HQAMaker::HQAMaker(void)
{
   // Default constructor
   nEvent = 0;
   nProcessed = 0; //A.Sadovski 21.Oct.2002
   nCountCalEvents = 0;
   hists = 0;
   varHists = 0;
   kFIRST = kTRUE;
   kPDF = kTRUE;
   fUseSlowPar = kTRUE;
   samplingRate = 1;
   intervalSize = 1000;
   nInterval = 0;

   lTrack.set(1, 0);
   lMdc.set(2, 0, 0);

   initIteratorPointers();
   cutStat = 0;        //asd
   setParContainers(); //asd
   betaMin = -1000.;
   nCountCalEvents = 0;
   fbeamtime="none";
}

HQAMaker::HQAMaker(const Text_t *n, const Text_t *t) : HReconstructor(n, t)
{
   // Constructor: Name, title
   nEvent = 0;
   nProcessed = 0; //A.Sadovski 21.Oct.2002
   hists = 0;
   varHists = 0;
   kFIRST = kTRUE;
   kPDF = kTRUE;
   fUseSlowPar = kTRUE;

   samplingRate = 1;
   intervalSize = 1000;
   nInterval = 0;
   isSim = kFALSE;

   lTrack.set(1, 0);
   lMdc.set(2, 0, 0);

   initIteratorPointers();
   cutStat = 0;        //asd
   setParContainers(); //asd
   betaMin = -1000.;
   nCountCalEvents = 0;
   fbeamtime="none";
}

HQAMaker::~HQAMaker(void)
{
   if (cutStat) HMdcCutStat::deleteHMdcCutStat();
}

void HQAMaker::initIteratorPointers()
{
   // Set iterator pointers to NULL
   iterStCal         = NULL;
   iterStHit         = NULL;
   iterRichCal       = NULL;
   iterRichHit       = NULL;
   iterMdcRaw        = NULL;
   iterMdcCal1       = NULL;
   iterMdcHit        = NULL;
   iterMdcSeg        = NULL;
   iterShoHit        = NULL;
   iterTofHit        = NULL;
   iterTofCluster    = NULL;
   iterRpcHit        = NULL;
   iterRpcCluster    = NULL;
   iterFwRaw         = NULL;
   iterFwHit         = NULL;
   iterSplineTrk     = NULL;
   iterRungeKuttaTrk = NULL;
   iterMetaMatch     = NULL;
   iterParticleCand  = NULL;
   iterHTBoxChan     = NULL;

   catSplineTrk      = NULL;
   catRungeKuttaTrk  = NULL;
   catShoHit         = NULL;
   catTfHit          = NULL;
   catTfClust        = NULL;
   fCatRpcHit        = NULL;
   fCatRpcCluster    = NULL;
   fCatParticleCand  = NULL;
}

Bool_t HQAMaker::init(void)
{
   // Create and book QA components, get iterator pointers if the categories
   // exist in current event

   TH1*       hist          = NULL;
   TIterator* hist_iterator = NULL;
   TString    old_directory = gDirectory->GetPath();

   // create the QA histograms in the root directory
   gROOT->cd();

   hists    = new HQAHistograms();
   varHists = new HQAVariations(intervalSize);
   scalers  = new HQAScalers();


   hists->bookHist();
   varHists->bookHist();
   scalers->bookScalers();

   hist_iterator = hists->getHistList()->MakeIterator();
   while ((hist = (TH1*)hist_iterator->Next())) {
      hist->SetDirectory(0);
   }
   delete hist_iterator;

   hist_iterator = varHists->getHistList()->MakeIterator();
   while ((hist = (TH1*)hist_iterator->Next())) {
      hist->SetDirectory(0);
   }
   delete hist_iterator;

   cutStat = HMdcCutStat::getObject();
   if (!cutStat) {
      ERROR_msg(HMessageMgr::DET_QA, "RETRIEVED 0 POINTER FOR HMDCCUTSTAT!");
      exit(1);
   }


   Bool_t FlagIterPointer = getIteratorPointers();

   if(fUseSlowPar){

       SlowPar = (HSlowPar*)gHades->getRuntimeDb()->getContainer("SlowPar");
       if(SlowPar) {

	   TString name;

	   for (Int_t k=0;k<6;k++)
	   {
	       name=Form("HAD:RICH:HV:PS:C%i:vmon",k);
	       SlowPar->addToChannelList(name);
	   }

	   for(Int_t plane=1; plane<5; plane++)
	   {
	       for(Int_t sec=1; sec<7; sec++)
	       {
		   for (Int_t k=0;k<12;k++)
		   {
		       name=Form("HAD:MDCHV:P%i:S%i:VMON_C%i",plane,sec,k);
		       SlowPar->addToChannelList(name);
		   }
	       }
	   }
       }
       else{
	   ERROR_msg(HMessageMgr::DET_QA, "RETRIEVED 0 POINTER FOR HSLOWPAR!");
	   return kFALSE;
       }
   }

   gDirectory->Cd(old_directory.Data());

   return FlagIterPointer;
}

void HQAMaker::Print(const Option_t *)
{
   // Print box showing QA summary

   Int_t nHists = 0, nVarHists = 0;

   if (hists) nHists = hists->getHistList()->GetSize();
   if (varHists) nVarHists = varHists->getHistList()->GetSize();

   SEPERATOR_msg("-", 70);
   INFO_msg(10, HMessageMgr::DET_QA, "HQAMaker: HADES data quality assessment");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%-35s%s", "QA statistics", "Histogram summary");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%-35s%s", "-------------", "-----------------");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%-23s%-5i%9i%s", "Event sampling rate: 1/", samplingRate, nHists, " general hists");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%-15s%-5i%5s%9i%s", "Interval size:", intervalSize, "events  ", nVarHists, " run var hists");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%5i%-41s", intervalSize / samplingRate, "QA'd events/interval");
   gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%s%-8i", "Max # events for var hists (=500*intervalSize): ", 500 * intervalSize);
   INFO_msg(10, HMessageMgr::DET_QA, "*extra events are added to overflow bins*");
   if (kPDF) {
      TString pdfFile = psFile(0, psFile.Last('.')) + ".pdf";
      gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%s%-57s", "PDF file:", (const Char_t*)pdfFile);
      INFO_msg(10, HMessageMgr::DET_QA, "*ps2pdf will be used to generate pdf file*");
      TString txtFile = psFile(0, psFile.Last('.')) + ".txt";
      gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%s%-57s", "scalers:", (const Char_t*)txtFile);
   } else
      gHades->getMsg()->info(10, HMessageMgr::DET_QA, this->GetName(), "%s%-57s", "PS file:", (const Char_t*)psFile);
   SEPERATOR_msg("-", 70);
}

Bool_t HQAMaker::getIteratorPointers()
{
   // Search current event for relevant categories... make an iterator
   // if category exists

   HEvent *event = gHades->getCurrentEvent();
   if (!event) {
      ERROR_msg(HMessageMgr::DET_QA, "QA maker needs a Hydra event structure");
      return kFALSE;
   }

   // Start
   HCategory *catStCal = event->getCategory(catStart2Cal);
   if (catStCal)
      iterStCal = (HIterator *)catStCal->MakeIterator("native");
   HCategory *catStHit = event->getCategory(catStart2Hit);
   if (catStHit)
      iterStHit = (HIterator *)catStHit->MakeIterator("native");

   // Daq Scalers
   HCategory *catDaqScal = event->getCategory(catTBoxChan);
   if (catDaqScal)
      iterHTBoxChan = (HIterator *)catDaqScal->MakeIterator("native");

   // Rich
   HCategory *catRicCal = event->getCategory(catRichCal);
   if (catRicCal)
      iterRichCal = (HIterator *)catRicCal->MakeIterator("native");
   HCategory *catRicHit = event->getCategory(catRichHit);
   if (catRicHit)
      iterRichHit = (HIterator *)catRicHit->MakeIterator("native");

   // Mdc
   HCategory *catMdRaw = event->getCategory(catMdcRaw);
   if (catMdRaw)
      iterMdcRaw = (HIterator *)catMdRaw->MakeIterator("native");
   HCategory *catMdCal1 = event->getCategory(catMdcCal1);
   if (catMdCal1)
      iterMdcCal1 = (HIterator *)catMdCal1->MakeIterator("native");
   if (catMdCal1)(catMdCal1->getClass() != HMdcCal1Sim::Class()) ? isSim = kFALSE : isSim = kTRUE;

   //if(!isSim)timecut  =(HMdcTimeCut*)   gHades->getRuntimeDb()->getContainer("MdcTimeCut");

   HCategory *catMdHit = event->getCategory(catMdcHit);
   if (catMdHit)
      iterMdcHit = (HIterator *)catMdHit->MakeIterator("native");

   HCategory *catMdSeg = event->getCategory(catMdcSeg);
   if (catMdSeg)
      iterMdcSeg = (HIterator *)catMdSeg->MakeIterator();



   // Tof
   catTfHit = event->getCategory(catTofHit);
   if (catTfHit)
      iterTofHit = (HIterator *)catTfHit->MakeIterator("native");

   // Tof-Cluster
   catTfClust = event->getCategory(catTofCluster);
   if (catTfClust)
      iterTofCluster = (HIterator *)catTfClust->MakeIterator("native");

   // RPC
   fCatRpcHit = event->getCategory(catRpcHit);
   if (fCatRpcHit)
      iterRpcHit = (HIterator*)fCatRpcHit->MakeIterator("native");

   fCatRpcCluster = event->getCategory(catRpcCluster);
   if (fCatRpcCluster)
      iterRpcCluster = (HIterator*)fCatRpcCluster->MakeIterator("native");

   // Wall
   HCategory *catFwRaw = event->getCategory(catWallRaw);
   if (catFwRaw)
      iterFwRaw = (HIterator *)catFwRaw->MakeIterator("native");

   HCategory *catFwHit = event->getCategory(catWallHit);
   if (catFwHit)
      iterFwHit = (HIterator *)catFwHit->MakeIterator("native");

   // Shower
   // in case of Simulation MetaMatching is using catShowerHitTofTrack
   // we have to make sure that we use the same cat
   //

   catShoHit = event->getCategory(catShowerHit);
   if (catShoHit) {
       iterShoHit = (HIterator *)catShoHit->MakeIterator("native");
   }
   //if catShowerHitTofTrack exist (sim data) use this cat
   //if not use catShowerHitTof

   // SplineTrack
   catSplineTrk = event->getCategory(catSplineTrack);
   if (catSplineTrk) {
      iterSplineTrk = (HIterator *)catSplineTrk->MakeIterator("native");
   }
   // RungeKuttaTrack
   catRungeKuttaTrk = event->getCategory(catRKTrackB);
   if (catRungeKuttaTrk) {
      iterRungeKuttaTrk = (HIterator *)catRungeKuttaTrk->MakeIterator("native");
   }

   // MetaMatch
   HCategory *catMeta = event->getCategory(catMetaMatch);
   if (catMeta) {
      iterMetaMatch = (HIterator *)catMeta->MakeIterator("native");
   }

   // ParticleCand
   fCatParticleCand = event->getCategory(catParticleCand);
   if (fCatParticleCand) {
      iterParticleCand = (HIterator *)fCatParticleCand->MakeIterator("native");
   }

   // OraRunSum


   return kTRUE;
}

void HQAMaker::setParContainers()   //asd
{
   // creates the parameter containers MdcLookupGeom/MdcLookupRaw/MdcTimeCut if they
   // exist and adds them to the list of parameter containers in the runtime
   // database
   //lookup   =(HMdcLookupGeom*)gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
   //lookupRaw=(HMdcLookupRaw*) gHades->getRuntimeDb()->getContainer("MdcLookupRaw");
}



Int_t HQAMaker::execute()
{
   //Fill histograms for events with a frequency of 1/samplingRate
   if (kFIRST) {
      kFIRST = kFALSE;
      buildPSFilename();
      Print();

      TDirectory* dir = gDirectory;
      //Create outputfile for Tree
      TString histFileTree = psFile(0, psFile.Last('.')) + "_Tree.root";  //is written where dsts are, but with ending "_Tree.root"
      fileTree = new TFile(histFileTree, "RECREATE");

      //dst_filename=
      TString dst_filename = gHades->getDataSource()->getCurrentFileName();
      dst_filename  = dst_filename(dst_filename.Last('/') + 1, dst_filename.Last('.') - dst_filename.Last('/') - 1);

      cout <<"DSTFileName: "<<dst_filename <<endl;


      TFileInfo.fTFileName=dst_filename;
      TFileInfo.fTRunId=gHades->getDataSource()->getCurrentRunId();
      TFileInfo.fTRefId=gHades->getDataSource()->getCurrentRefId();


      HTime::splitFileName(dst_filename,TFileInfo.fTType,TFileInfo.fTYear,TFileInfo.fTDay,TFileInfo.fTHour,TFileInfo.fTMin,TFileInfo.fTSec,TFileInfo.fTEvB);


      //#######################

      //Get High Voltages

      TString name;

      Int_t runID = gHades->getDataSource()->getCurrentRunId();

      if(!isSim && fUseSlowPar)
      {
	  HSlowChan SlowChan;

	  for (Int_t k=0;k<6;k++)
	  {
	      SlowChan.clear();
	      name=Form("HAD:RICH:HV:PS:C%i:vmon",k);
	      SlowPar->getChannel(runID,name,&SlowChan);

	      TRich.fTHVMean[k]=SlowChan.mean;
	      TRich.fTHVSigma[k]=SlowChan.rms;
	      TRich.fTHVMin[k]=SlowChan.min;
	      TRich.fTHVMax[k]=SlowChan.max;
	  }

	  for(Int_t plane=0; plane<4; plane++)
	  {
	      for(Int_t sec=0; sec<6; sec++)
	      {
		  for(Int_t k=0; k<12; k++)
		  {
		      SlowChan.clear();
		      name=Form("HAD:MDCHV:P%i:S%i:VMON_C%i",plane+1,sec+1,k);
		      SlowPar->getChannel(runID,name,&SlowChan);

		      TMdc.fTHVMean[sec][plane][k]=SlowChan.mean;
		      TMdc.fTHVSigma[sec][plane][k]=SlowChan.rms;
		      TMdc.fTHVMin[sec][plane][k]=SlowChan.min;
		      TMdc.fTHVMax[sec][plane][k]=SlowChan.max;
		  }
	      }
	  }

      }
      //End get High Voltages***********************************************************************




      fileTree->cd();
      outputTree = new TTree("T","QA Tree");
      //outputTree->Branch("tdst_filename",&tdst_filename,1000,0);     //100 is buffer size ; 0 is no splitting, so only the String is written
      outputTree->Branch("TFileInfo",&TFileInfo,1000,0);
      outputTree->Branch("TStart",&TStart,1000,0);
      outputTree->Branch("TRich",&TRich,1000,0);
      outputTree->Branch("TMdc",&TMdc,1000,0);
      outputTree->Branch("TTof",&TTof,1000,0);
      outputTree->Branch("TRpc",&TRpc,1000,0);
      outputTree->Branch("TShw",&TShw,1000,0);
      outputTree->Branch("TPhy",&TPhy,1000,0);

      dir->cd();

   }



   //fill daq scaler data. The daq scalers are read out for calibration trigger.
   //Histograms are filled for each calibration event.
  // Int_t eventnb = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
   Int_t eventid = gHades->getCurrentEvent()->getHeader()->getId();

   if (eventid == 14) {  // scaler data are transported in events with id (hex)e
      fillHistDaqScaler();
      nCountCalEvents++;
   }

   if (!(gHades->isCalibration())) {
      if (nEvent % intervalSize == 0) nInterval++;
      if (nEvent % samplingRate == 0) {

         Int_t eventSize = gHades->getCurrentEvent()->getHeader()->getEventSize();
         varHists->evtHeader_eventSize_Var->Fill(nEvent, eventSize);

         fillHistStart();
         fillHistRich();
         fillHistMdc();
         fillHistTof();
         fillHistRpc();
         fillHistShower();
	 fillHistWall();
         fillHistMatching();
         fillHistSpline();
         fillHistRungeKutta();
         fillHistRichMDC();
         fillHistPid();
	 fillHistShowerRpc();
         fillMassSpectrum();
         nProcessed++;
      }
   } // fill detector qa plots only for real events
   nEvent++;
   return 0;
}

void HQAMaker::buildPSFilename()
{
   // Build Postscript file name, either from user-supplied name or from DST filename

   if (fDir.IsNull() && (!psFile.Contains("/"))) {
      WARNING_msg(HMessageMgr::DET_QA, 10, "dir not specified, QA output written to present dir");
   }

   // If psFile is not given, build PS filename from gHades filename
   if (psFile.IsNull()) {
      if (gHades->getOutputFile() != 0) {
         dstFile = gHades->getOutputFile()->GetName();
      } else {
         dstFile = gHades->getDataSource()->getCurrentFileName();
         dstFile.Replace(0, dstFile.Last('/') + 1, "");
      }
//      psFile  = dstFile(dstFile.Last('/') + 1, dstFile.Last('.') - dstFile.Last('/') - 1) + ".ps";
      psFile  = dstFile(dstFile.Last('/') + 1, dstFile.Last('.') - dstFile.Last('/') - 1) + ".pdf";
   }

   // Add "/" at end of directory
   if (!fDir.IsNull()) {
      if (!(fDir[fDir.Length()-1] == '/')) {
         fDir += "/";
      }
   }

   if (!psFile.Contains("/")) {
      psFile = fDir + psFile;
   }
}

Bool_t HQAMaker::finalize(void)
{
   // Finalize histograms and fill scalers

   if (nProcessed == 0) {
      cout << "--------------------------------------------------------- \n";
      cout << "-- WARNING: QA MAKER, Number of processed events is 0. -- \n";
      cout << "------ qa pdf document will not be created -------------- \n";
      cout << "--------------------------------------------------------- \n";
      return 1;
   } else {

      fillScalers();

      finalizeHistStart();
      finalizeHistDaqScaler();
      finalizeHistRich();
      finalizeHistMdc();
      finalizeHistTof();
      finalizeHistRpc();
      finalizeHistShower();
      finalizeHistRichMDC();
      finalizeHistPid();
      finalizeHistShowerRpc();
      finalizeMassSpectrum();

      Bool_t ret = makePS();


      TFileInfo.fTNumEvents=nEvent;

      outputTree->Fill();
      fileTree->cd();
      outputTree->Write();
      fileTree->Save();
      fileTree->Close();

      return ret;
   }
}

Bool_t HQAMaker::makePS()
{
   // After finalizing, generate PS file using HQAOutputPS

   HQAOutputPS *outPS = new HQAOutputPS(psFile);


   outPS->setDSTFileName(dstFile);
   outPS->setStats(nEvent, nProcessed);

   outPS->makeHist(hists->getHistList());
   outPS->writeHist(hists->getHistList(), varHists->getHistList(), psFile);  //asd


   outPS->setNHistPerPage(3);
   outPS->makeHist(varHists->getHistList());


   outPS->makeText(scalers->getScalerList());
   outPS->saveScal(scalers->getScalerList(), psFile); //asd

   //outPS->write(varHists->getHistList());

   delete outPS;

   return kTRUE;
}

void HQAMaker::fillHistStart()
{
   Int_t triggerBit = (gHades->getCurrentEvent()->getHeader()->getTBit());


   HStart2Cal *stCal = 0;
   HStart2Hit *stHit = 0;
   HTofHit *tofHit = 0;
   Float_t  tof_corr = 0;
   Float_t dist = 0;
   Float_t stm0CalTime[NSTART_STRIPS];
   Float_t stm1CalTime[NSTART_STRIPS];
   Float_t vtCalTime[NSTART_STRIPS];
   for (Int_t ii = 0; ii < NSTART_STRIPS ; ii++) {
      stm0CalTime[ii] = -10000.0;
      stm1CalTime[ii] = -10000.0;
      vtCalTime[ii]   = -10000.0;
   }

   //get the TBit (from 1 to 32) from triggerBit (int)
   for (Int_t ii = 0; ii < 32 ; ii++) {
      if ((triggerBit & (1 << ii)) != 0) {
         hists->stLatchHist->Fill(ii);
      }
   }

   fillGlobalVertex(); // fill vertex information

   if (iterStCal) {
      iterStCal->Reset();
      while ((stCal = (HStart2Cal*) iterStCal->Next()) != 0) {
         //if (stCal->getModule() == 0 || stCal->getModule() == 3) {
         if (fabs(stCal->getTime(1)) <30.0) { // select only hits close to the recaction
            if(stCal->getModule() == 0){ 
               if(stCal->getStrip()>NSTART_STRIPS-1) { cout<<"start cal strip to large "<<stCal->getStrip()<<endl; continue;}
               varHists->stCal_meanStrip_Var->Fill(nEvent, stCal->getStrip());
               hists->stCal_tof->Fill(stCal->getTime(1));
               hists->stCal_stripMod0->Fill(stCal->getStrip());
	       stm0CalTime[stCal->getStrip()] = stCal->getTime(1);   //Time of start for 1st hit
               hists->stCal_tof_strip[stCal->getStrip()]->Fill(stCal->getTime(1));
            }
            if(stCal->getModule() == 1){ 
		hists->stCal_stripMod1->Fill(stCal->getStrip());
		stm1CalTime[stCal->getStrip()] = stCal->getTime(1);   //Time of start for 1st hit
            } 
	    if(stCal->getModule() == 3){
               varHists->vtCal_meanStrip_Var->Fill(nEvent, stCal->getStrip());
               hists->vtCal_strip->Fill(stCal->getStrip());
	       vtCalTime[stCal->getStrip()] = stCal->getTime(1);
	       hists->vtCal_tof->Fill(stCal->getTime(1));          //Time of veto for 1st hit
               hists->vtCal_tof_strip[stCal->getStrip()]->Fill(stCal->getTime(1));
            }
         }
      }
   }
   
   // Fill start-veto time res histograms.
   for (Int_t im = 0; im < NSTART_STRIPS; im++) {
      if(vtCalTime[im] > -1000.0){
         for (Int_t jm = 0; jm < NSTART_STRIPS; jm++) {
            if(stm0CalTime[jm] > -10000)hists->Stm0Vtdiff_vs_stStrip[im]->Fill(jm,stm0CalTime[jm] - vtCalTime[im]);
            if(stm1CalTime[jm] > -10000)hists->Stm1Vtdiff_vs_stStrip[im]->Fill(jm,stm1CalTime[jm] - vtCalTime[im]);
         }
      }
   }

   if (iterStHit) {
      iterStHit->Reset();
      while ((stHit = (HStart2Hit*) iterStHit->Next()) != 0) {
         if (stHit->getModule() == 0) {
             if(stHit->getStrip()>NSTART_STRIPS-1) { cout<<"start hit strip to large "<<stHit->getStrip()<<endl; continue;}
            //fill StartTime vs StartStrip 2-dim
	     hists->stHit_vs_stStrip->Fill(stHit->getStrip(), stHit->getTime());
	     hists->stHit_tof->Fill(stHit->getTime());  //Start time


            //get the TBit (from 1 to 8) from triggerBit (int)
            for (Int_t ii = 0; ii < 9 ; ii++) {
               if ((triggerBit & (1 << ii)) != 0) {
                  hists->stHit_vs_stStrip_Trigg[ii+1]->Fill(stHit->getStrip(), stHit->getTime());
               }
            }

            if (iterTofHit) {
               iterTofHit->Reset();
               while ((tofHit = (HTofHit*) iterTofHit->Next()) != 0) {
                  tofHit->getDistance(dist);
                  tof_corr = tofHit->getTof() + (2100. - dist) / 300.;

                  hists->stHit_tof_vs_startstripe->Fill(stHit->getStrip(), tof_corr);
               }
            }
         } //St module 0
      }
   } // iterStHit
}


void HQAMaker::finalizeHistStart()
{
    if (hists->stLatchHist->GetEntries() > 0) {
	if (nProcessed == 0) hists->stLatchHist->Scale(1.);
	else hists->stLatchHist->Scale(1. / nProcessed);
    }
    if(hists->stHit_tof->GetEntries()>0)
    {
	TStart.fTHitTimeMean=hists->stHit_tof->GetMean(1); //Mean Value of StartTime
	TStart.fTHitTimeRms=hists->stHit_tof->GetRMS(1); //Sigma of StartTime
    }
    if(hists->stCal_tof->GetEntries()>0&&hists->vtCal_tof->GetEntries()>0)
    {
	TStart.fTHitToNoveto=hists->stCal_tof->GetEntries()/(hists->vtCal_tof->GetEntries()); // This is the Ratio between hits in start and hits in veto
    }
    if(hists->stVertexXY->GetEntries()>0)
    {
        TStart.fTVertexX=hists->stVertexXY->GetMean(1); //X Value of Vertex
	TStart.fTVertexY=hists->stVertexXY->GetMean(2); //Y Value of Vertex
    }

    if(hists->stVertexZ->GetEntries()>0) {TStart.fTVertexZ=hists->stVertexZ->GetMean(1);}  //Z Value of Vertex

    for(Int_t i=0;i<NSTART_STRIPS;i++)
    {
	if(hists->stCal_tof_strip[i]->GetEntries()>0)
	{
	    TStart.fTStCalTime[i]=hists->stCal_tof_strip[i]->GetMean(1); //Mean Value of Start CalTime for each Strip
	}
	if(hists->vtCal_tof_strip[i]->GetEntries()>0)
	{
	    TStart.fTVtCalTime[i]=hists->vtCal_tof_strip[i]->GetMean(1); //Mean Value of Veto CalTime for each Strip
	}
    }
}



void HQAMaker::fillHistRich()
{

   HRichCal *richCal = 0;
   Int_t nDataObjs;

   Int_t RingCounter[6];
   for(Int_t i=0; i<6; i++)
   {
       RingCounter[i]=0; //Rings in Sector
   }

   if (iterRichCal) {
      iterRichCal->Reset();
      nDataObjs = ((TObjArray*)iterRichCal->GetCollection())->GetEntries();
      varHists->richCal_n_Var->Fill(nEvent, nDataObjs);
      while ((richCal = (HRichCal*) iterRichCal->Next()) != 0) {

         hists->richCal_row->Fill(richCal->getRow());
         hists->richCal_column->Fill(richCal->getCol());
         hists->richCal_nSec->Fill(richCal->getSector());

      }
   }

   HRichHit *richHit = 0;
   Float_t theta, phi, r2d = 57.29578;

   if (iterRichHit) {
      iterRichHit->Reset();
      nDataObjs = ((TObjArray*)iterRichHit->GetCollection())->GetEntries();
      varHists->richHit_n_Var->Fill(nEvent, nDataObjs);
      while ((richHit = (HRichHit*) iterRichHit->Next()) != 0)
      {

	  RingCounter[richHit->getSector()]++;


	  theta = richHit->getTheta();
	  phi = richHit->getPhi();

	  if (theta == 0) {
            continue;
	  }


	  hists->richHit_theta->Fill(theta);
	  hists->richHit_phi->Fill(phi);
	  hists->richHit_nSec->Fill(richHit->getSector());

	  hists->richHit_scat->Fill(sin(theta / r2d)*sin((phi - 90) / r2d), sin(theta / r2d)*cos((phi - 90) / r2d));

	  hists->richHit_radius->Fill(richHit->getRadius());
	  hists->richHit_centroid->Fill(richHit->getCentroid());
	  hists->richHit_chargeAmpl->Fill(richHit->getRingAmplitude());
	  hists->richHit_ringCol->Fill(richHit->getRingCenterX());
	  hists->richHit_ringRow->Fill(richHit->getRingCenterY());
	  hists->richHit_ringLocMax4->Fill(richHit->getRingLocalMax4());
	  hists->richHit_houTraVsPatMat->Fill(richHit->getRingHouTra() , richHit->getRingPatMat());
	  hists->richHit_patMatVsTheta->Fill(richHit->getRingPatMat() , theta);
	  hists->richHit_houTraVsTheta->Fill(richHit->getRingHouTra() , theta);
	  hists->richHit_chargeAmplVsTheta->Fill(richHit->getRingAmplitude() , theta);
	  hists->richHit_radiusVsTheta->Fill(richHit->getRadius() , theta);
	  hists->richHit_AverCharge[richHit->getSector()]->Fill(richHit->getRingAmplitude()/richHit->getRingPadNr());
	  hists->richHit_NumPads[richHit->getSector()]->Fill(richHit->getRingPadNr());
      }

      for(Int_t i=0;i<6;i++)
      {
	  hists->richHit_NumRings[i]->Fill(RingCounter[i]);
      }
   }
}


void HQAMaker::finalizeHistRich()
{
    for(Int_t i=0; i<6; i++)
    {
	if(hists->richHit_AverCharge[i]->GetEntries()>0)
	{
	    TRich.fTAvChargeMax[i]=hists->richHit_AverCharge[i]->GetBinCenter(hists->richHit_AverCharge[i]->GetMaximumBin());
	    TRich.fTAvChargeMean[i]=hists->richHit_AverCharge[i]->GetMean();
	}

	if(hists->richHit_NumPads[i]->GetEntries()>0)
	{
	    TRich.fTNumPadsMax[i]=hists->richHit_NumPads[i]->GetBinCenter(hists->richHit_NumPads[i]->GetMaximumBin());
	    TRich.fTNumPadsMean[i]=hists->richHit_NumPads[i]->GetMean();
	}

        if(hists->richHit_NumRings[i]->GetEntries()>0)
	{
	    TRich.fTNumRingsMax[i]=hists->richHit_NumRings[i]->GetBinCenter(hists->richHit_NumRings[i]->GetMaximumBin());
	    TRich.fTNumRingsMean[i]=hists->richHit_NumRings[i]->GetMean();
	}
    }
}



void HQAMaker::fillHistMdc()
{

    HMdcRaw *mdcRaw = 0;

    if (iterMdcRaw)
    {
	iterMdcRaw->Reset();
	while ((mdcRaw = (HMdcRaw*) iterMdcRaw->Next()) != 0) {

	    Int_t sector = mdcRaw->getSector();
	    Int_t module = mdcRaw->getModule();

	    // mdcRaw: sector vs. mbo mult.
	    Int_t mbo = mdcRaw->getMbo();

	    if (module == 0) hists->mdcRaw_mboVsSector_m0->Fill(mbo, sector);
	    else if (module == 1) hists->mdcRaw_mboVsSector_m1->Fill(mbo, sector);
	    else if (module == 2) hists->mdcRaw_mboVsSector_m2->Fill(mbo, sector);
	    else if (module == 3) hists->mdcRaw_mboVsSector_m3->Fill(mbo, sector);

	    TMdc.fTMboCounts[sector][module][mbo]++;
            TMdc.fTCountsRaw[sector][module]++;
	}
    }

   HMdcCal1 *mdcCal1 = 0;
   HMdcCal1Sim *mdcCal1Sim = 0;

   if (iterMdcCal1) {
      iterMdcCal1->Reset();
      Int_t n[4][6];
      for (Int_t i = 0; i < 4; i++) for (Int_t j = 0; j < 6; j++) n[i][j] = 0;
      while ((mdcCal1 = (HMdcCal1*) iterMdcCal1->Next()) != 0) {

         if (isSim) {
            // in simulation we have to reject drift cells
            // with status < 1 (cutted by efficiency / broken hardware)
            mdcCal1Sim = (HMdcCal1Sim*) mdcCal1;
            if (mdcCal1Sim->getStatus1() < 1) continue;
         }

         Int_t sector = mdcCal1->getSector();
	 Int_t module = mdcCal1->getModule();
         Int_t layer  = mdcCal1->getLayer();

         n[module][sector]++;

         Float_t time1 = mdcCal1->getTime1();
         Float_t time2 = mdcCal1->getTime2();

         // mdcCal1: t2-t1 vs t1
         hists->mdcCal1_t2mt1_vs_t1[sector][module]->Fill(time2 - time1, time1);
         hists->mdcCal1_t2mt1_vs_t1_plane[  module]->Fill(time2 - time1, time1);

         //mdcCal1: t2-t1
	 hists->mdcCal1_t2mt1[sector][module]->Fill(time2 - time1);
         hists->mdcCal1_t2mt1_V2[sector][module][layer]->Fill(time2 - time1);
                
         //mdcCal1: t1
	 hists->mdcCal1_t1[sector][module]->Fill(time1);
         hists->mdcCal1_t1_V2[sector][module][layer]->Fill(time1);

         // mdcCal1: sector vs. t1
         if (module == 0) hists->mdcCal1_time1VsSector_m0->Fill(time1, sector);
         else if (module == 1) hists->mdcCal1_time1VsSector_m1->Fill(time1, sector);
         else if (module == 2) hists->mdcCal1_time1VsSector_m2->Fill(time1, sector);
         else if (module == 3) hists->mdcCal1_time1VsSector_m3->Fill(time1, sector);

         if (module == 0) varHists->mdcCal1_time1_m0_Var->Fill(nEvent, time1);
         else if (module == 1) varHists->mdcCal1_time1_m1_Var->Fill(nEvent, time1);
         else if (module == 2) varHists->mdcCal1_time1_m2_Var->Fill(nEvent, time1);
         else if (module == 3) varHists->mdcCal1_time1_m3_Var->Fill(nEvent, time1);

         // mdcCal1: module vs. t2-t1
         if (module == 0) varHists->mdcCal1_time2m1_m0_Var->Fill(nEvent, time2 - time1);
         else if (module == 1) varHists->mdcCal1_time2m1_m1_Var->Fill(nEvent, time2 - time1);
         else if (module == 2) varHists->mdcCal1_time2m1_m2_Var->Fill(nEvent, time2 - time1);
         else if (module == 3) varHists->mdcCal1_time2m1_m3_Var->Fill(nEvent, time2 - time1);

         // mdcCal1: sector vs. t.a.t.
         Float_t tat = mdcCal1->getTime2() - mdcCal1->getTime1();
         if (module == 0) hists->mdcCal1_tatVsSector_m0->Fill(tat, sector);
         else if (module == 1) hists->mdcCal1_tatVsSector_m1->Fill(tat, sector);
         else if (module == 2) hists->mdcCal1_tatVsSector_m2->Fill(tat, sector);
	 else if (module == 3) hists->mdcCal1_tatVsSector_m3->Fill(tat, sector);

	 TMdc.fTLayerCounts[sector][module][layer]++;
         TMdc.fTCountsCal[sector][module]++;

      }
      for (Int_t sector = 0; sector < 6; sector++) {
         hists->mdcCal1_nVsSector_m0->Fill(n[0][sector], sector);
         hists->mdcCal1_nVsSector_m1->Fill(n[1][sector], sector);
         hists->mdcCal1_nVsSector_m2->Fill(n[2][sector], sector);
         hists->mdcCal1_nVsSector_m3->Fill(n[3][sector], sector);
      }
   }

   HMdcHit *mdcHit = 0;

   if (iterMdcHit) {
      iterMdcHit->Reset();
      while ((mdcHit = (HMdcHit*) iterMdcHit->Next()) != 0) {

         Int_t sector, module;
         Float_t x1, y1, x2, y2, angle;
         mdcHit->getSecMod(sector, module);

         // Scatter plots (made difficult by chamber coordinate system)
         x1 = -mdcHit->getX(); //minus is becouse our coordinate system has Ox looking to the left, when looking from a target to one of MDC planes
         y1 = mdcHit->getY();

         if (module == 0) y1 = (y1 + 650) * .65;
         if (module == 1) y1 = (y1 + 950) * .58;
         if (module == 2) y1 = (y1 + 1800) * .58;
         if (module == 3) y1 = (y1 + 1900) * .68;

         angle = ((float) sector) * 60. / 57.2967;
         x2 =  x1 * cos(angle) + y1 * sin(angle);
         y2 = -x1 * sin(angle) + y1 * cos(angle);

         if (module == 0) hists->mdcHit_scat_m0->Fill(x2, y2);
         if (module == 1) hists->mdcHit_scat_m1->Fill(x2, y2);
         if (module == 2) hists->mdcHit_scat_m2->Fill(x2, y2);
         if (module == 3) hists->mdcHit_scat_m3->Fill(x2, y2);

      }
   }

   HMdcSeg *mdcSeg = 0;

   if (iterMdcSeg)
   {
       iterMdcSeg->Reset();
       while ((mdcSeg = (HMdcSeg*) iterMdcSeg->Next()) != 0)
       {
	   Int_t sector, segment;
	   sector=mdcSeg->getSec();
           segment=mdcSeg->getIOSeg();
           
           hists->mdcSeg_Chi2[sector][segment]->Fill(mdcSeg->getChi2());

	   TMdc.fTSegCounter[sector][segment]++;

	   if(mdcSeg->getChi2()>0)
	   {
	       TMdc.fTSegCounterFit[sector][segment]++;
	   }
       }
   }
}


//***---------------------------------------------------------


void HQAMaker::finalizeHistMdc()
{
   Int_t c[8];
   Char_t buf[500];

   //DEBUG_msg(1,HMessageMgr::DET_QA,"MYOUTPUT----MYOUTPUT----MYOUTPUT----MYOUTPUT----");

   Int_t ib;
   for (Int_t m = 0; m < 4; m++) { //better to have first all sectors of the same module
      for (Int_t s = 0; s < 6; s++) {
         cutStat->getCal1StatCut(s, m, &c[0], &c[1], &c[2], &c[3], &c[4], &c[5], &c[6],&c[7]);
         sprintf(buf, "%i %i %i %i %i %i %i %i %i %i \n",
                 s, m,
                 c[0], c[1], c[2], c[3], c[4], c[5], c[6] ,c[7]
                );
         DEBUG_msg(1, HMessageMgr::DET_QA, buf);

         ib = m * 6 + s + 1; //before I have an error here: ib = m*4+s+1; which caused lost of sector0 Dubna && Rossendorf for nov01-gen3 DST

         // 0   1    2     3     4     5     6       7
         //t1  t2  t1t2  wire  t1+t12  bump  cutall  allhits

         hists->mdcCutCounts[0]->SetBinContent(ib, c[7]); //allhits
         hists->mdcCutCounts[1]->SetBinContent(ib, c[0]); //t1
         hists->mdcCutCounts[2]->SetBinContent(ib, c[1]); //t2
         hists->mdcCutCounts[3]->SetBinContent(ib, c[2]); //t1t2
         hists->mdcCutCounts[4]->SetBinContent(ib, c[3]); //wire
         hists->mdcCutCounts[5]->SetBinContent(ib, c[4]); //t1+t12
         hists->mdcCutCounts[6]->SetBinContent(ib, c[5]); //t1+t12
         hists->mdcCutCounts[7]->SetBinContent(ib, c[6]); //cutall
      }
   }
   DEBUG_msg(1, HMessageMgr::DET_QA, buf);

   if (cutStat) cutStat->printParam();

   cout <<"Events"<<nEvent <<endl;
   for(Int_t i=0; i<6;i++)
   {
       for(Int_t j=0; j<2; j++)
       {
	   TMdc.fTSegCounter[i][j]=TMdc.fTSegCounter[i][j]/nProcessed;
	   TMdc.fTSegCounterFit[i][j]=TMdc.fTSegCounterFit[i][j]/nProcessed;

	   if(hists->mdcSeg_Chi2[i][j]->GetEntries())
	   {
	       TMdc.fTSegChi2Mean[i][j]=hists->mdcSeg_Chi2[i][j]->GetMean();
	       TMdc.fTSegChi2Max[i][j]=hists->mdcSeg_Chi2[i][j]->GetBinCenter(hists->mdcSeg_Chi2[i][j]->GetMaximumBin());
	   }
       }
       for(Int_t j=0; j<4; j++)
       {
	   for(Int_t k=0; k<16; k++)
	   {
	       TMdc.fTMboCounts[i][j][k]=TMdc.fTMboCounts[i][j][k]/nProcessed;
	   }
           for(Int_t k=0; k<6; k++)
	   {
	       TMdc.fTLayerCounts[i][j][k]=TMdc.fTLayerCounts[i][j][k]/nProcessed;

	       if(hists->mdcCal1_t2mt1_V2[i][j][k]->GetEntries())
	       {
		   TMdc.fTToTCalMean[i][j][k]=hists->mdcCal1_t2mt1_V2[i][j][k]->GetMean();
		   TMdc.fTToTCalMax[i][j][k]=hists->mdcCal1_t2mt1_V2[i][j][k]->GetBinCenter(hists->mdcCal1_t2mt1_V2[i][j][k]->GetMaximumBin());
	       }

	       if(hists->mdcCal1_t1_V2[i][j][k]->GetEntries())
	       {
		   TMdc.fTTime1CalMean[i][j][k]=hists->mdcCal1_t1_V2[i][j][k]->GetMean();
		   TMdc.fTTime1CalMax[i][j][k]=hists->mdcCal1_t1_V2[i][j][k]->GetBinCenter(hists->mdcCal1_t1_V2[i][j][k]->GetMaximumBin());
	       }
	   }

	   TMdc.fTCountsRaw[i][j]=TMdc.fTCountsRaw[i][j]/nProcessed;
           TMdc.fTCountsCal[i][j]=TMdc.fTCountsCal[i][j]/nProcessed;
       }
   }
}


void HQAMaker::fillHistTof()
{

   HTofHit *tofHit = 0;
   Float_t x, y, z, theta, phi;
   Int_t nDataObjs;
   Int_t TofCounts[6]={0,0,0,0,0,0};

   if (iterTofHit) {
      iterTofHit->Reset();
      nDataObjs = ((TObjArray*)iterTofHit->GetCollection())->GetEntries();
      //hists->tofHit_n->Fill(nDataObjs);
      varHists->tofHit_n_Var->Fill(nEvent, nDataObjs);

      while ((tofHit = (HTofHit*) iterTofHit->Next()) != 0)
      {

         tofHit->getTheta(theta);
         tofHit->getPhi(phi);

         hists->tofHit_nSec->Fill(tofHit->getSector());
         tofHit->getXYZLab(x, y, z);
         hists->tofHit_scat->Fill(-x, y);
         hists->tofHit_tof->Fill(tofHit->getTof());
         hists->tofHit_phi->Fill(phi);
	 hists->tofHit_theta->Fill(theta);
	 TofCounts[(Int_t)tofHit->getSector()]++;
         TTof.fTHits[(Int_t)tofHit->getSector()]++;

      }
      for(Int_t i=0;i<6;i++)
      {
	  hists->tofHit_n[i]->Fill(TofCounts[i]);
      }

      hists->tofHit_tot->Fill(TofCounts[0]+TofCounts[1]+TofCounts[2]+TofCounts[3]+TofCounts[4]+TofCounts[5]);
   }
}


void HQAMaker::finalizeHistTof()
{
   if (hists->tofHit_nSec->GetEntries() > 0) {
      if (nProcessed == 0)
         hists->tofHit_nSec->Scale(1.);
      else
         hists->tofHit_nSec->Scale(1. / nProcessed);
   }

   //cout <<"nProcessed: "<<nProcessed <<endl;



   for(Int_t i=0; i<6; i++)
   {
       if(hists->tofHit_n[i]->GetEntries()>0)
       {
	   TTof.fTHitMultiMean[i]=hists->tofHit_n[i]->GetMean();
           TTof.fTHitMultiMax[i]=hists->tofHit_n[i]->GetBinCenter(hists->tofHit_n[i]->GetMaximumBin());
       }
       if(nProcessed>0)
       {
	   TTof.fTHits[i]=TTof.fTHits[i]/nProcessed;
       }
   }

   if(hists->tofHit_tot->GetEntries()>0)
   {
       TTof.fTHitTotMultiMean=hists->tofHit_tot->GetMean();
       TTof.fTHitTotMultiMax=hists->tofHit_tot->GetBinCenter(hists->tofHit_tot->GetMaximumBin());
       hists->tofHit_tot->GetXaxis()->SetRangeUser(10,100.5);   //want to find maximum in multiplicity larger than 10
       TTof.fTHitTotMultiMax2=hists->tofHit_tot->GetBinCenter(hists->tofHit_tot->GetMaximumBin());
       hists->tofHit_tot->GetXaxis()->SetRangeUser(-0.5,100.5);
   }

   if(hists->tofHit_tof->GetEntries()>0)
   {
       TTof.fTHitTofMean=hists->tofHit_tof->GetMean();
       TTof.fTHitTofMax=hists->tofHit_tof->GetBinCenter(hists->tofHit_tof->GetMaximumBin());
   }
}


void HQAMaker::fillHistRpc()
{

   HRpcHit *rpcHit = 0;
   Float_t x, y, z;
   Int_t nDataObjs;
   Int_t RPCCounts[6]={0,0,0,0,0,0};

   if (iterRpcHit)
   {
      iterRpcHit->Reset();
      nDataObjs = ((TObjArray*)iterRpcHit->GetCollection())->GetEntries();
      //hists->rpcHit_n->Fill(nDataObjs);
      varHists->rpcHit_n_Var->Fill(nEvent, nDataObjs);

      while ((rpcHit = (HRpcHit*) iterRpcHit->Next()) != 0)
      {

	  rpcHit->getXYZLab(x, y, z);

	  hists->rpcHit_nSec ->Fill(rpcHit->getSector());
	  hists->rpcHit_scat ->Fill(-x, y);
	  hists->rpcHit_tof  ->Fill(rpcHit->getTof());
	  hists->rpcHit_phi  ->Fill(rpcHit->getPhi());
	  hists->rpcHit_theta->Fill(rpcHit->getTheta());
	  RPCCounts[rpcHit->getSector()]++;
          TRpc.fTHits[(Int_t)rpcHit->getSector()]++;
      }
      for(Int_t i=0;i<6;i++)
      {
	  //cout<<"Hits in RPC Sector :"<<i<<", "<<RPCCounts[i]<<endl;
	  hists->rpcHit_n[i]->Fill(RPCCounts[i]);
      }

      hists->rpcHit_tot->Fill(RPCCounts[0]+RPCCounts[1]+RPCCounts[2]+RPCCounts[3]+RPCCounts[4]+RPCCounts[5]);
   }

}



void HQAMaker::finalizeHistRpc()
{
   if (hists->rpcHit_nSec->GetEntries() > 0) {
      if (nProcessed == 0)
         hists->rpcHit_nSec->Scale(1.);
      else
         hists->rpcHit_nSec->Scale(1. / nProcessed);
   }

   /*
   HRpcHit *rpcHit = 0;
   Float_t x, y, z;
   Int_t nDataObjs;

   if (iterRpcHit) {
      iterRpcHit->Reset();
      nDataObjs = ((TObjArray*)iterRpcHit->GetCollection())->GetEntries();
      //??hists->rpcHit_n->Fill(nDataObjs);
      varHists->rpcHit_n_Var->Fill(nEvent, nDataObjs);

      while ((rpcHit = (HRpcHit*) iterRpcHit->Next()) != 0) {

         rpcHit->getXYZLab(x, y, z);

         hists->rpcHit_nSec ->Fill(rpcHit->getSector());
         hists->rpcHit_scat ->Fill(-x, y);
         hists->rpcHit_tof  ->Fill(rpcHit->getTof());
         //hists->rpcHit_phi  ->Fill((rpcHit->getPhi()-60.)+(rpcHit->getSector()*60.));
         hists->rpcHit_phi  ->Fill(rpcHit->getPhi());
         hists->rpcHit_theta->Fill(rpcHit->getTheta());

      }
   }
   */

   for(Int_t i=0; i<6; i++)
   {
       if(hists->rpcHit_n[i]->GetEntries()>0)
       {
	   TRpc.fTHitMultiMean[i]=hists->rpcHit_n[i]->GetMean();
           TRpc.fTHitMultiMax[i]=hists->rpcHit_n[i]->GetBinCenter(hists->rpcHit_n[i]->GetMaximumBin());
       }
       if(nProcessed>0)
       {
	   TRpc.fTHits[i]=TRpc.fTHits[i]/nProcessed;
       }
   }

   if(hists->rpcHit_tot->GetEntries()>0)
   {
       TRpc.fTHitTotMultiMean=hists->rpcHit_tot->GetMean();
       TRpc.fTHitTotMultiMax=hists->rpcHit_tot->GetBinCenter(hists->rpcHit_tot->GetMaximumBin());
       hists->rpcHit_tot->GetXaxis()->SetRangeUser(10,200.5);   //want to find maximum in multiplicity larger than 10
       TRpc.fTHitTotMultiMax2=hists->rpcHit_tot->GetBinCenter(hists->rpcHit_tot->GetMaximumBin());
       hists->rpcHit_tot->GetXaxis()->SetRangeUser(-0.5,200.5);
   }

   if(hists->rpcHit_tof->GetEntries()>0)
   {
       TRpc.fTHitTofMean=hists->rpcHit_tof->GetMean();
       TRpc.fTHitTofMax=hists->rpcHit_tof->GetBinCenter(hists->rpcHit_tof->GetMaximumBin());
   }
}



void HQAMaker::fillHistShower()
{

   Float_t sum, dummy1, phi, theta, charge, x, y, z;
   Int_t row, col, mod, sec;
   Int_t nDataObjs;
   Int_t ShowerCounts[6]={0,0,0,0,0,0};

   Int_t ShowerCounts2[6][3];
   for(Int_t i=0; i<6; i++)
   {
       for(Int_t j=0; j<3; j++)
       {
           ShowerCounts2[i][j]=0;
       }
   }

   HShowerHit *shoHit = 0;

   if (iterShoHit) {
      iterShoHit->Reset();
      nDataObjs = ((TObjArray*)iterShoHit->GetCollection())->GetEntries();
      varHists->shoHit_n_Var->Fill(nEvent, nDataObjs);

      while ((shoHit = (HShowerHit*) iterShoHit->Next()) != 0) {

         row = shoHit->getRow();
         col = shoHit->getCol();
         mod = shoHit->getModule();
         sec = shoHit->getSector();
         charge = shoHit->getCharge();
         hists->shoHit_nSec->Fill(sec);
         hists->shoHit_sectorVsModule->Fill(sec, mod);
         hists->shoHit_nRow->Fill(row);
         hists->shoHit_nCol->Fill(col);
         if ((sum = shoHit->getSum(mod)) > 0.0) {
            hists->shoHitSums[sec][mod]->Fill(sum);
         }
         if (mod == 0) hists->shoHit_chargeVsSector_m0->Fill(charge, sec);
         else if (mod == 1) hists->shoHit_chargeVsSector_m1->Fill(charge, sec);
         else if (mod == 2) hists->shoHit_chargeVsSector_m2->Fill(charge, sec);

         if (mod == 0) hists->shoHit_rowVsSector_m0->Fill(row, sec);
         else if (mod == 1) hists->shoHit_rowVsSector_m1->Fill(row, sec);
         else if (mod == 2) hists->shoHit_rowVsSector_m2->Fill(row, sec);

         if (mod == 0) hists->shoHit_colVsSector_m0->Fill(col, sec);
         else if (mod == 1) hists->shoHit_colVsSector_m1->Fill(col, sec);
         else if (mod == 2) hists->shoHit_colVsSector_m2->Fill(col, sec);

         shoHit->getSphereCoord(&dummy1, &phi, &theta);
         hists->shoHit_phi->Fill(phi);
         hists->shoHit_theta->Fill(theta);

         shoHit->getLabXYZ(&x, &y, &z);
         hists->shoHit_scat->Fill(-x, y);

	 ShowerCounts[sec]++;
         ShowerCounts2[sec][mod]++;
      }
      for(Int_t i=0;i<6;i++)
      {
	  hists->shoHit_n[i]->Fill(ShowerCounts[i]);

	  for(Int_t j=0;j<3;j++)
	  {
              hists->shoHit_nm[i][j]->Fill(ShowerCounts2[i][j]);
	  }
      }
   }

}


void HQAMaker::finalizeHistShower()
{
    if (hists->shoHit_nSec->GetEntries() > 0)
    {
	if (nProcessed == 0)
	    hists->shoHit_nSec->Scale(1.);
	else {
	    hists->shoHit_nSec->Scale(1. / nProcessed);
	    hists->shoHit_nCol->Scale(1. / nProcessed);
	    hists->shoHit_nRow->Scale(1. / nProcessed);
	    hists->shoHit_sectorVsModule->Scale(1. / nProcessed);
	    hists->shoHit_scat->Scale(1. / nProcessed);
	    hists->shoHit_theta->Scale(1. / nProcessed);
	    hists->shoHit_phi->Scale(1. / nProcessed);
	    for (Int_t s = 0; s < 6; s++) {
		for (Int_t m = 0; m < 3; m++) {
		    hists->shoHitSums[s][m]->Scale(1. / nProcessed);
		}
	    }
	}
    }

    for(Int_t i=0; i<6; i++)
    {
	for(Int_t j=0; j<3; j++)
	{
	    if(hists->shoHit_nm[i][j]->GetEntries() > 0)
	    {
                TShw.fTHitMultiMean[i][j]=hists->shoHit_nm[i][j]->GetMean();
                TShw.fTHitMultiMax[i][j]=hists->shoHit_nm[i][j]->GetBinCenter(hists->shoHit_nm[i][j]->GetMaximumBin());
	    }

	    if(hists->shoHitSums[i][j]!=0)
	    {
		TShw.fTHitChargeMean[i][j]=hists->shoHitSums[i][j]->GetMean();
                TShw.fTHitChargeMax[i][j]=hists->shoHitSums[i][j]->GetBinCenter(hists->shoHitSums[i][j]->GetMaximumBin());
	    }
	}
    }
    // hists->shoHit_avgCharge_m0->Divide(hists->shoHit_rowVsCol_m0);
}



void HQAMaker::fillHistWall()
{

   //introduced for ForwardWall

   HWallRaw *fwRaw = 0;
   HWallHit *fwHit = 0;

   Int_t multWall = 0;
   Int_t wallCell = 0;
   Int_t multCell = 0;
   Int_t wallCellArr[4];
   for (Int_t i = 0; i < 4; i++) {
      wallCellArr[i] = 0;
   }

   // Iteration over HWallRaw

   if (iterFwRaw) {
      iterFwRaw->Reset();

      while ((fwRaw = (HWallRaw*) iterFwRaw->Next()) != 0) {
         multCell = fwRaw->getNHits();
         if (multCell <= 0) continue;
         if (multCell > fwRaw->getMaxMult()) {
            multCell = fwRaw->getMaxMult();
         }
         wallCell = fwRaw->getCell();

         //I quarter -> 1-6;61-66
         if (((wallCell >=   0) && (wallCell <=   5)) ||
             ((wallCell >=  12) && (wallCell <=  17)) ||
             ((wallCell >=  24) && (wallCell <=  29)) ||
             ((wallCell >=  36) && (wallCell <=  41)) ||
             ((wallCell >=  48) && (wallCell <=  53)) ||
             ((wallCell >=  60) && (wallCell <=  64)))  wallCellArr[0]++;

         //II quarter -> 7-12;67-72
         if (((wallCell >=   6) && (wallCell <=  11)) ||
             ((wallCell >=  18) && (wallCell <=  23)) ||
             ((wallCell >=  30) && (wallCell <=  35)) ||
             ((wallCell >=  42) && (wallCell <=  47)) ||
             ((wallCell >=  54) && (wallCell <=  59)) ||
             ((wallCell >=  67) && (wallCell <=  71)))  wallCellArr[1]++;

         //III quarter -> 73-78;133-138
         if (((wallCell >=  72) && (wallCell <=  76)) ||
             ((wallCell >=  84) && (wallCell <=  89)) ||
             ((wallCell >=  96) && (wallCell <= 101)) ||
             ((wallCell >= 108) && (wallCell <= 113)) ||
             ((wallCell >= 120) && (wallCell <= 125)) ||
             ((wallCell >= 132) && (wallCell <= 137)))  wallCellArr[2]++;

         //IV quarter -> 79-84;139-144
         if (((wallCell >=  79) && (wallCell <=  83)) ||
             ((wallCell >=  90) && (wallCell <=  95)) ||
             ((wallCell >= 102) && (wallCell <= 107)) ||
             ((wallCell >= 114) && (wallCell <= 119)) ||
             ((wallCell >= 126) && (wallCell <= 131)) ||
             ((wallCell >= 138) && (wallCell <= 143)))  wallCellArr[3]++;

         if (wallCell < 144) hists->hWallCellSmall ->Fill(wallCell);               //  0-143
         if (wallCell >= 144 && wallCell < 208) hists->hWallCellMedium->Fill(wallCell); //144-207
         if (wallCell >= 208) hists->hWallCellLarge ->Fill(wallCell);              //210-301

         multWall++;

      }//-end-while-(HWallRaw)--

      hists->hMultWall     ->Fill((Float_t)multWall);
      hists->hWallHitNumI  ->Fill((Float_t)wallCellArr[0]);
      hists->hWallHitNumII ->Fill((Float_t)wallCellArr[1]);
      hists->hWallHitNumIII->Fill((Float_t)wallCellArr[2]);
      hists->hWallHitNumIV ->Fill((Float_t)wallCellArr[3]);

   }

   // Iteration over HWallHit

   if (iterFwHit) {
      iterFwHit->Reset();

      Int_t wallCell;
      Float_t wallX, wallY, wallZ, wallTime, wallCharge;

      while ((fwHit = (HWallHit*)iterFwHit->Next())) {

         wallCell   = fwHit->getCell();
         wallTime   = fwHit->getTime();
         wallCharge = fwHit->getCharge();

         fwHit->getXYZLab(wallX, wallY, wallZ);
         wallX = wallX / 10;
         wallY = wallY / 10;

         hists->hWallXY->Fill(wallX, wallY);

         hists->hWallCellTime->Fill((Float_t)wallCell, wallTime);
         hists->hWallCellAdc ->Fill((Float_t)wallCell, wallCharge);

      }//-end-while-(HWallHit)--
   }

   // Marking up non existing FW cells

   //Double_t minSmall=1,minMedium=1,minLarge=1;
   //Stat_t fprofSmall[146],fprofLarge[94];
   //minSmall=hists->hWallCellSmall->GetMinimum(minSmall);
   //minLarge=hists->hWallCellLarge->GetMinimum(minLarge);
   //
   //for(Int_t ic=0;ic<146;ic++) fprofSmall[ic] = 0;
   //for(Int_t ic=0;ic< 94;ic++) fprofLarge[ic] = 0;
   //
   //fprofSmall[66]=fprofSmall[67]=fprofSmall[78]=fprofSmall[79]=minSmall*2;
   ////hists->hWallCellSmallHole->SetContent(fprofSmall);
   //hists->hWallCellSmall->SetContent(fprofSmall);
   //
   //fprofLarge[209-208]=fprofLarge[210-208]=fprofLarge[218-208]=fprofLarge[219-208]=fprofLarge[220-208]=fprofLarge[230-208]=fprofLarge[283-208]=fprofLarge[293-208]=fprofLarge[294-208]=fprofLarge[295-208]=minLarge*2;
   ////hists->hWallCellLargeHole->SetContent(fprofLarge);
   //hists->hWallCellLarge->SetContent(fprofLarge);


}






void HQAMaker::fillHistSpline()
{
}


void HQAMaker::fillHistRungeKutta()
{

   Float_t mass, theta, phi, r2d = 57.29578;
   Int_t system, charge, sec;

   HRKTrackB *rungeKuttaTrk = 0;

   if (iterRungeKuttaTrk) {
      iterRungeKuttaTrk->Reset();

      while ((rungeKuttaTrk = (HRKTrackB*) iterRungeKuttaTrk->Next()) != 0) {

         if (rungeKuttaTrk->getBeta() > betaMin) {
            if (rungeKuttaTrk->getChiq() > 100000.) continue;

            system =      rungeKuttaTrk->getSystem();
            charge =      rungeKuttaTrk->getPolarity();
            mass   =      rungeKuttaTrk->getMass2();
            sec    =      rungeKuttaTrk->getSector();

            if (mass >= 0) {
               mass   = sqrt(mass);
               hists->rungeKuttaTrack_massCharge->Fill(mass * charge);
            }

            theta = rungeKuttaTrk->getTheta() * r2d;
            phi   = rungeKuttaTrk->getPhi() * r2d + 60.*sec;
            if (phi > 360.) phi -= 360.;
            hists->rungeKuttaTrack_scat->Fill(sin(theta / r2d)*sin((phi - 90) / r2d), sin(theta / r2d)*cos((phi - 90) / r2d));

            if (system == 0) {
               hists->trackingRK_sys0[sec]->Fill(mass * charge);
	    }

            if (system == 1) {
               hists->trackingRK_sys1[sec]->Fill(mass * charge);
            }
         }
      }
   }
}







void HQAMaker::finalizeHistDaqScaler()
{
   // show only not empty bins:
   //  ( (TAxis *)h->GetXaxis()) ->SetRange(0,nCountCalEvents)
   //---- set Xaxis range for  Daq Scalers


   for (Int_t ii = 0; ii < 8; ii++) {
/*
      ((TAxis *)(hists->histReduce[ii]) ->GetXaxis()) ->SetRange(0, nCountCalEvents);
      ((TAxis *)(hists->histInput[ii]) ->GetXaxis()) ->SetRange(0, nCountCalEvents);
      ((TAxis *)(hists->histStartScaler[ii]) ->GetXaxis()) ->SetRange(0, nCountCalEvents);
      ((TAxis *)(hists->histVetoScaler[ii]) ->GetXaxis()) ->SetRange(0, nCountCalEvents);
      // and request that Y axis should start from 0
      hists->histReduce[ii] -> SetMinimum(0.0);
      hists->histInput[ii] -> SetMinimum(0.0);
      hists->histStartScaler[ii] -> SetMinimum(0.0);
      hists->histVetoScaler[ii] -> SetMinimum(0.0);
*/
 
     ((TAxis *)(hists->histAllScalerTrend) ->GetXaxis()) ->SetRange(0, nCountCalEvents);

   }
}





void HQAMaker::fillHistRichMDC()
{
   HMdcSeg *mdcSeg = 0;
   HRichHit *richHit = 0;

   Int_t   richSec;
   Float_t mdcPhi, mdcTheta;
   Float_t richPhi, richTheta;

   Int_t iSec;
   Float_t rad2deg = 180.0 / TMath::Pi();

   for (iSec = 0; iSec < 6; iSec++) {
      lMdc[0] = iSec;
      lMdc[1] = 0;

      if (iterMdcSeg) {
         iterMdcSeg->Reset();
         iterMdcSeg->gotoLocation(lMdc);

         while ((mdcSeg = (HMdcSeg*) iterMdcSeg->Next()) != 0) {
            mdcPhi = mdcSeg->getPhi() * rad2deg ;
            if (iSec != 5) mdcPhi = mdcPhi + iSec * 60;
            else mdcPhi = mdcPhi - 60.;

            mdcTheta = mdcSeg->getTheta() * rad2deg;

            if (iterRichHit) {
               iterRichHit->Reset();
               while ((richHit = (HRichHit*) iterRichHit->Next()) != 0) {
                  richTheta = richHit->getTheta();
                  richPhi = richHit->getPhi();
                  richSec = richHit->getSector();

                  if (richSec != iSec) continue;

                  hists->richmdc_dtheta[richSec]->Fill(richTheta - mdcTheta);
                  hists->richmdc_dphi[richSec]->Fill((richPhi - mdcPhi)*sin(mdcTheta / rad2deg));

                  if (fabs((richPhi - mdcPhi)*sin(mdcTheta / rad2deg)) > DPHI) continue;
                  if (fabs((richTheta - mdcTheta)) > DTHETA) continue;

                  hists->richmdc_lep->Fill(iSec);
               }
            }
         }
      }
   }
}


void HQAMaker::finalizeHistRichMDC()
{
   Float_t scaleF;

   if (nProcessed == 0) scaleF = 1.0;
   else scaleF = 1. / nProcessed;

   for (Int_t s = 0; s < 6; s++) {
      hists->richmdc_dtheta[s]->Scale(scaleF);
      hists->richmdc_dphi[s]->Scale(scaleF);
   }
   hists->richmdc_lep->Scale(scaleF);
}

void HQAMaker::fillScalers()
{
   // Fill scalar quantities with operations on QA histograms


   TH1D *proj = 0;

   // Sectorwise scalers
   for (Int_t sec = 0; sec < 6; sec++) {

      //---- Rich scalers
      if (hists->richCal_nSec->GetEntries() > 0)
         (*scalers->richCal_n).fData[sec] = (float) hists->richCal_nSec->GetBinContent(sec + 1) / nProcessed;
      if (hists->richHit_nSec->GetEntries() > 0)
         (*scalers->richHit_n)[sec] = 1000. * (float) hists->richHit_nSec->GetBinContent(sec + 1) / nProcessed;

      //---- Mdc scalers
      if (hists->mdcCal1_nVsSector_m0->GetEntries() > 0) {
         proj = hists->mdcCal1_nVsSector_m0->ProjectionX("proj", sec + 1, sec + 1);
         (*scalers->mdcCal1_n_m0)[sec] = proj->GetMean();
         delete proj;
      }
      if (hists->mdcCal1_nVsSector_m1->GetEntries() > 0) {
         proj = hists->mdcCal1_nVsSector_m1->ProjectionX("proj", sec + 1, sec + 1);
         (*scalers->mdcCal1_n_m1)[sec] = proj->GetMean();
         delete proj;
      }
      if (hists->mdcCal1_nVsSector_m2->GetEntries() > 0) {
         proj = hists->mdcCal1_nVsSector_m2->ProjectionX("proj", sec + 1, sec + 1);
         (*scalers->mdcCal1_n_m2)[sec] = proj->GetMean();
         delete proj;
      }
      if (hists->mdcCal1_nVsSector_m3->GetEntries() > 0) {
         proj = hists->mdcCal1_nVsSector_m3->ProjectionX("proj", sec + 1, sec + 1);
         (*scalers->mdcCal1_n_m3)[sec] = proj->GetMean();
         delete proj;
      }

      //---- Tof scalers
      if (hists->tofHit_nSec->GetEntries() > 0)
         (*scalers->tofHit_n)[sec] = (float) hists->tofHit_nSec->GetBinContent(sec + 1) / nProcessed;

      // Shower scalers
      if (hists->shoHit_nSec->GetEntries() > 0)
         (*scalers->shoHit_n)[sec] = (float) hists->shoHit_nSec->GetBinContent(sec + 1) / nProcessed;
      if (hists->shoHitTof_nSec->GetEntries() > 0)
         (*scalers->shoHitTof_n)[sec] = (float) hists->shoHitTof_nSec->GetBinContent(sec + 1) / nProcessed;

   }

   // Detector-wise scalers

}



void HQAMaker::fillHistMatching()
{

   HSplineTrack  *track  = 0;
   HRKTrackB *trackRK    = NULL;
   HTofCluster   *Cluster = 0;
   HShowerHit    *Shower = 0;
   HMetaMatch2   *meta   = 0;
   HRpcHit       *Rpc    = 0;

   Float_t rad2deg = 180.0 / TMath::Pi();

   //for low multiplicity
   /*
   //Int_t nDataObjs_tof=1000,nDataObjs_rpc=1000;//unused
   if (iterTofHit) {
      iterTofHit->Reset();
      nDataObjs_tof = ((TObjArray*)iterTofHit->GetCollection())->GetEntries(); //unused
   }
   if (iterRpcHit)
   {
      iterRpcHit->Reset();
      nDataObjs_rpc = ((TObjArray*)iterRpcHit->GetCollection())->GetEntries(); //unused
   }
   //end
   */

   if (iterMetaMatch) {
       iterMetaMatch->Reset();
       while ((meta = (HMetaMatch2 *)iterMetaMatch->Next()) != 0) {

//low multiplicity cut added
	 //if((nDataObjs_tof+nDataObjs_rpc)>100) continue;  //40% of M20 events less central
	 //   if(nDataObjs_tof>23) continue;  //20% of M20 events less central, TOF only



	   if (meta->getSystem() < 0) continue; // no metamatch                            //is not always below 0
	   if ((meta->getSplineInd() != -1)) {  // inner+outer seg + spline                // index to get the appropriate spline object
	       track = (HSplineTrack *)catSplineTrk->getObject(meta->getSplineInd());
	       //cout <<"AAAAAAAAAAAAAAAAAAAAAAAAAAAAA"<<endl;
	       //--------------------------------------------------------------------
	       // was rk succesful ?  (rk indedx has to taken from another place ....)
	       // also quality parameters ...
	       // caution : meta->getRungeKuttaInd();
	       //          is filled in case a.) no METAHIT b.) rk failed
	       //  a. is skipped , we have to check if rk failed
	       Bool_t  rkSuccess    = kTRUE;
	       Int_t   rkReplaceInd = meta->getRungeKuttaInd();                            // index to get the appropriate RungeKutta object
	       Float_t rkchi2       = -1;
	       if (rkReplaceInd >= 0) trackRK = (HRKTrackB*) catRungeKuttaTrk->getObject(rkReplaceInd);
	       else                  trackRK = 0;

	       if (trackRK != 0)          {
		   rkchi2 = trackRK->getChiq();
	       }
	       if (trackRK && rkchi2 < 0)  {
		   rkSuccess = kFALSE   ;
	       }

	       //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	       //. . . . .Loop over the 3 best Meta Matches for the RK Track . . . . .
	       //. . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . . .
	//Dont loop over META HITS JUST TAKE THE BEST       for (Int_t i = 0; i < META_TAB_SIZE; i++) {
		   //if (track->getSystem() > -1){hists->hsecspline ->Fill(track->getSector());}        // System is always 0 ???
		   //if (track->getSystem() == 1){hists->hsecspline1->Fill(track->getSector());}        //changed from track - to - meta!!!!!!!!
		   //if (track->getSystem() == 0){hists->hsecspline0->Fill(track->getSector());}        //changed from track - to - meta!!!!!!!!

		   if (meta->getSystem() > -1){hists->hsecspline ->Fill(track->getSector());}        // System is always 0 ???
		   if (meta->getSystem() == 1){hists->hsecspline1->Fill(track->getSector());}        //changed from track - to - meta!!!!!!!!
		   if (meta->getSystem() == 0){hists->hsecspline0->Fill(track->getSector());}        //changed from track - to - meta!!!!!!!!

                   Int_t MetaSector = meta->getSector();
		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   //° ° °Fill Histograms for MetaMatch in TOF ° °
		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   //if (track->getSystem() == 1) {
		   if (meta->getSystem()==1) {                                                   //TESTTESTTEST
		       Int_t indtof = meta->getTofClstInd(0);                                       // index for the appropriate Tof Cluster hit (larger than 0 if there was a hit)

		       if (indtof >= 0) Cluster = (HTofCluster *) catTfClust->getObject(indtof);
		       else            Cluster = NULL;

		       if (Cluster) { //-system==1 (TOF case)

			   if (!rkSuccess) trackRK = NULL;
			   else { // get RK from meta hits
			       Int_t indrk = meta->getRungeKuttaIndTofClst(0);                             // index for the appropriate RungeKutta object with Tof hit (>-1)
			       if (indrk >= 0) trackRK = (HRKTrackB*)catRungeKuttaTrk -> getObject(indrk);
			   }

			   //calculation of tofXSeg, tofYSeg - positions of TOF hit in module coordinates
                           Int_t TOFsector;
 			   //gibts nichTOFSigmaX = Cluster->getXRMS();
			   //gibt nichTOFSigmaY = Cluster->getYRMS();
			   TOFsector = Cluster->getSector();

                           if(MetaSector!=TOFsector)cout<<"Something strange with META TOF"<<endl;

			   hists->hXdiffvstofstrip->Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), meta->getTofClstDX(0));
			   hists->hYdiffvstofstrip->Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), meta->getTofClstDY(0));
			   hists->htof_quality->Fill(meta->getSector(), meta->getTofClstQuality(0));


			   hists->hXdiffTof[TOFsector] ->Fill(meta->getTofClstDX(0));
			   hists->hYdiffTof[TOFsector] ->Fill(meta->getTofClstDY(0));

			   if (trackRK) {
			       hists->hXdiffvstofstripRK -> Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), (trackRK -> getMETAdx()));
			       hists->hYdiffvstofstripRK -> Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), (trackRK -> getMETAdy()));
                               hists->htof_qualityRK     -> Fill(Cluster->getSector(),trackRK ->getQualityTof());
			   }
			   //neg 300<mom<1000  6.5<tof<9.5
			   if (trackRK&&trackRK->getPolarity()<0&&trackRK->getP()>300&&Cluster->getTof()>6.5&&Cluster->getTof()<9.5) {
			       hists->hXdiffvstofstripRK_neg -> Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), (trackRK -> getMETAdx()));
			       hists->hYdiffvstofstripRK_neg -> Fill((Cluster->getSector() * 64 + (Cluster->getModule() * 8) + Cluster->getCell()), (trackRK -> getMETAdy()));
			       hists->hXdiffvsthetaRK_neg -> Fill((Cluster->getSector() * 100 + trackRK->getTheta()*rad2deg), (trackRK -> getMETAdx()));
			       hists->hYdiffvsthetaRK_neg -> Fill((Cluster->getSector() * 100 + trackRK->getTheta()*rad2deg), (trackRK -> getMETAdy()));
			   }

		       }
		   }//-system==1, TOF


		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   //° ° °Fill Histograms for MetaMatch in Shower° °
		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   //if (track->getSystem() == 0) { //-system==0 (SHOWER | RPC case)
		   if (meta->getSystem()==0) {

		       Int_t indshower = meta->getShowerHitInd(0);

		       if (indshower >= 0) Shower = (HShowerHit *) catShoHit->getObject(indshower);
		       else                Shower = NULL;

		       if (Shower) {
			   Float_t ShwX, ShwY, ShwSigmaX, ShwSigmaY;
                           Int_t ShwSector;
			   Char_t ShwRow, ShwCol;
			   Shower->getXY(&ShwX, &ShwY);
			   ShwRow = Shower->getRow();
			   ShwCol = Shower->getCol();
			   ShwSigmaX = Shower->getSigmaX();
			   ShwSigmaY = Shower->getSigmaY();
                           ShwSector = Shower->getSector();
			   if (ShwSigmaX == 0 || ShwSigmaY == 0.0) cout << "--Error: ShowerSigma could not be 0! " << endl;
                           if(MetaSector!=ShwSector)cout<<"Something strange with META Shower"<<endl;

			   hists->hXdiffvsshowersector->Fill(meta->getSector(), meta->getShowerHitDX(0));
			   hists->hYdiffvsshowersector->Fill(meta->getSector(), meta->getShowerHitDY(0));

			   hists->hXdiffvsshw->Fill(meta->getSector(), (meta->getShowerHitDX(0)) / ShwSigmaX);
			   hists->hYdiffvsshw->Fill(meta->getSector(), (meta->getShowerHitDY(0)) / ShwSigmaY);

			   hists->hshower_quality     ->Fill(meta->getSector(),  meta->getShowerHitQuality(0));

			   //hists->hXdiffvsshoCol      ->Fill(ShwCol + (meta->getSector() * 33), meta->getShowerHitDX(i));        //Changed by Johannes Siebenson
			   hists->hXdiffvsshoCol      ->Fill(ShwCol + (meta->getSector() * 33), meta->getShowerHitDX(0));
			   hists->hXdiffvsshoRow      ->Fill(ShwRow + (meta->getSector() * 33), meta->getShowerHitDX(0));
			   hists->hYdiffvsshoCol      ->Fill(ShwCol + (meta->getSector() * 33), meta->getShowerHitDY(0));
			   hists->hYdiffvsshoRow      ->Fill(ShwRow + (meta->getSector() * 33), meta->getShowerHitDY(0));


			   hists->hXdiffsho[ShwSector] ->Fill(meta->getShowerHitDX(0)/ShwSigmaX);
			   hists->hYdiffsho[ShwSector] ->Fill(meta->getShowerHitDY(0)/ShwSigmaY);

			   if (!rkSuccess) trackRK = NULL;
			   else { // get RK from meta hits
			       Int_t indrk = meta->getRungeKuttaIndShowerHit(0);
			       if (indrk >= 0) trackRK = (HRKTrackB*)catRungeKuttaTrk -> getObject(indrk);
			   }
			   if (trackRK) {
	        	       hists -> hXdiffvsshowersectorRK -> Fill(meta->getSector(), trackRK -> getMETAdx());
			       hists -> hYdiffvsshowersectorRK -> Fill(meta->getSector(), trackRK -> getMETAdy());
                               hists -> hshower_qualityRK      -> Fill(meta->getSector(), trackRK -> getQualityShower());
			       //    hists -> hXdiffvsshwRK->Fill(meta->getSector(),(ShwX-trackRK -> getMETAdx()) / ShwSigmaX );
			       //    hists -> hYdiffvsshwRK->Fill(meta->getSector(),(ShwY-trackRK -> getMETAdy()) / ShwSigmaY);
			       hists -> hXdiffvsshoColRK      ->Fill(ShwCol + (meta->getSector() * 33), trackRK -> getMETAdx());
			       hists -> hXdiffvsshoRowRK      ->Fill(ShwRow + (meta->getSector() * 33), trackRK -> getMETAdx());
			       hists -> hYdiffvsshoColRK      ->Fill(ShwCol + (meta->getSector() * 33), trackRK -> getMETAdy());
			       hists -> hYdiffvsshoRowRK      ->Fill(ShwRow + (meta->getSector() * 33), trackRK -> getMETAdy());
			   }
		       }

		   }//-system==0, Shower

		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   //° ° °Fill Histograms for MetaMatch in RPC ° °
		   //° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° ° °
		   if (meta->getSystem()==0) {

		       Int_t indRpc = meta->getRpcClstInd(0);

		       if (indRpc >= 0) Rpc = (HRpcHit *) fCatRpcHit->getObject(indRpc);
		       else             Rpc = NULL;

		       if (Rpc) {
			   //Float_t RpcX, RpcY;
			   Float_t RpcSigmaX, RpcSigmaY;
			   RpcSigmaX = Rpc->getXRMS();
			   RpcSigmaY = Rpc->getYRMS();
			   if (RpcSigmaX == 0 || RpcSigmaY == 0.0) cout << "--Error: RpcSigma could not be 0! " << endl;

			   hists->hXdiffvsRpcsector->Fill(meta->getSector(), meta->getRpcClstDX(0));
			   hists->hYdiffvsRpcsector->Fill(meta->getSector(), meta->getRpcClstDY(0));

                           hists->hXdiffvsRpc->Fill(meta->getSector(), (meta->getRpcClstDX(0)) / RpcSigmaX);
			   hists->hYdiffvsRpc->Fill(meta->getSector(), (meta->getRpcClstDY(0)) / RpcSigmaY);

			   hists->hRpc_quality     ->Fill(meta->getSector(),  meta->getRpcClstQuality(0));

			   hists->hXdiffRpc[(Int_t)meta->getSector()] ->Fill(meta->getRpcClstDX(0)/RpcSigmaX);
			   hists->hYdiffRpc[(Int_t)meta->getSector()] ->Fill(meta->getRpcClstDY(0)/RpcSigmaY);
		       }
		   }//-system==0, RPC

	       //  } // loop meta candidates

	   }//-end-of-loop-over-spline--

       }//-end-while--
   }
}

void HQAMaker::fillHistPid()
{
    HParticleCand    *pCand       = NULL;
    HStart2Cal       *stCal       = NULL;
    HStart2Hit       *pStartHit   = NULL;
    //HTofHit          *pTof        = NULL;
    //HRpcHit          *pRpc        = NULL;
    HRpcCluster      *pRpcClu     = NULL;
    HTofCluster      *pTofClu     = NULL;

    Int_t             multRK      = 0; // Mult. counters
    Int_t             multLepCand = 0;
    Bool_t            isGoodRich  = kFALSE; 


    Int_t MetaCounts[6][2];
    Int_t MetaCountsSelect[6][2];

    for(Int_t i=0; i<6; i++)
    {
	for(Int_t j=0; j<2; j++)
	{
	    MetaCounts[i][j]=0;
            MetaCountsSelect[i][j]=0;
	}
    }

   //-----------------------StartDet part-----------------------------
   // first step is to get global timing information
   // there is only one "time 0" and is located in
   // Start2Hit category
   Float_t startTime  = 0.0;   // put here 0.0 as default, means no start HIT 
   Int_t   startStrip = -1;

   if (iterStHit) {
      iterStHit->Reset();
      while (NULL != (pStartHit = static_cast<HStart2Hit*>(iterStHit->Next()))) {
         if (pStartHit->getModule() == 0) {
            startStrip = pStartHit->getStrip();
            startTime  = pStartHit->getTime();
         }
      }  //end of start iteration
   } // if iterStart

  // We need also StartCal information ------------------------------
  // this is needed to prepare calibration check for Start Module 1
   Float_t startCalTimeMod0  = 0.0;   // put here 0.0 as default, means no start HIT 
   Int_t   startCalStripMod0 = -1;
   Float_t startCalTimeMod1  = 0.0;   // put here 0.0 as default, means no start HIT 
   Int_t   startCalStripMod1 = -1;


   //-------------------End of Start part-----------------------------


   if (NULL == iterParticleCand) {
      return ;  // do nothing if ParticleCandidate does not exist
   }

   iterParticleCand->Reset();
   Int_t nDataObjs = static_cast<TObjArray*>(const_cast<TCollection*>(iterParticleCand->GetCollection()))->GetEntries();
   varHists->particleCand_n_Var->Fill(nEvent, nDataObjs);

   //----------------- loop over particleCand -----------------------
   while (NULL != (pCand = static_cast<HParticleCand*>(iterParticleCand->Next()))) { //ParticleCandIter
      // strong RICH cut
      // getRingNumPads()  and average ampl ( getRingAmplitude() / getRingNumPads()
      // very strong condition, will select mainly doubly hits of conversion pairs.
      isGoodRich  = kFALSE;
      if( ( pCand->getRingAmplitude() ) / (pCand->getRingNumPads() )  > 80 ) {
         isGoodRich = kTRUE;
      }

      if (pCand->getSystem() > -1 ) {
         multRK++;
         if (pCand->isRichMatch(Particle::kIsRICHRK)  && isGoodRich ) {
            ++multLepCand;
         }
      } else {
         continue;
      }

      //------------------- RK in Pid ----------------------------------
      if (pCand->getChi2() > 0 && pCand->getChi2() < 1000)
      {

	  //times for leptons
	  Float_t fPidTimeCorr = 0.0;
	  if (pCand->getBeta() != 0.) {
	      fPidTimeCorr = 7. / pCand->getBeta();
	  }
	  if (pCand->isRichMatch(Particle::kIsRICHRK)  && isGoodRich  )
	  {
	      if (pCand->getSystem() == 0 && pCand->getRpcInd() > -1 && fCatRpcCluster )
	      {
		  if (NULL != (pRpcClu = static_cast<HRpcCluster*>(fCatRpcCluster->getObject(pCand->getRpcInd()))) && (pRpcClu->getClusterType() == 1 ) ) {
		      hists->hparticle_lepton_tof_vs_rod_sys0->Fill(pCand->getSector() * 192 + pRpcClu->getColumn1() * 32 + pRpcClu->getCell1(), fPidTimeCorr);
		  }
	      }

	      // switch to TofCluster category
	      if (pCand->getSystem() == 1 && pCand->getTofClstInd() > -1 && catTfClust)
	      {
		  if (NULL != (pTofClu = static_cast<HTofCluster*>(catTfClust->getObject(pCand->getTofClstInd())))) {
		      hists->hparticle_lepton_tof_vs_rod_sys1->Fill(pCand->getSector() * 64 + (pTofClu->getModule() * 8 + pTofClu->getCell()), fPidTimeCorr);
		  }
	      }

	      if (pCand->getSystem() == 0)
	      {
		  hists->hparticle_lepton_tof_vs_startstrip_sys0->Fill(startStrip, fPidTimeCorr);
		  hists->hparticle_lepton_tof_all_sys0->Fill(fPidTimeCorr);
		  if(startStrip>-1)
		  {
		      hists->hparticle_lepton_tof_vs_start_sys0[startStrip]->Fill(fPidTimeCorr);
		  }
	      }

	      if (pCand->getSystem() == 1)
	      {
		  hists->hparticle_lepton_tof_vs_startstrip_sys1->Fill(startStrip, fPidTimeCorr);
		  hists->hparticle_lepton_tof_all_sys1->Fill(fPidTimeCorr);
                  if(startStrip>-1)
		  {
		      hists->hparticle_lepton_tof_vs_start_sys1[startStrip]->Fill(fPidTimeCorr);
		  }
	      }
	  }

	  //For Pions
	  //theta and phi
	  if (pCand->getSystem() > -1)
	  {
	      hists->hparticle_RK_theta_sec[pCand->getSector()]->Fill(pCand->getTheta());
	      hists->hparticle_RK_phi->Fill(pCand->getPhi());
	      if (pCand->getCharge() == -1 && pCand->getMomentum() > 300)
	      {
		  hists->hparticle_RK_neg_theta_sec[pCand->getSector()]->Fill(pCand->getTheta());
		  hists->hparticle_RK_neg_phi->Fill(pCand->getPhi());

		  //TOF and RPC time calibration QA plots
		  Float_t beta_c = (pCand->getMomentum())/TMath::Sqrt((pCand->getMomentum())*(pCand->getMomentum())+139*139.);

		  Float_t tof_c;
		  Float_t time_diff;
		  if (pCand->getSystem() ==0 && pCand->getRpcInd() > -1 && fCatRpcCluster)
		  {
		      if (NULL != (pRpcClu = static_cast<HRpcCluster*>(fCatRpcCluster->getObject(pCand->getRpcInd()))) &&
			  (pRpcClu->getClusterType() == 1) && pCand->getCharge() < 0. && pCand->getMomentum() > 200.0  )
		      {
			  tof_c  = ( pCand->getBeta()*( pRpcClu->getTof()))/beta_c;
			  time_diff =  (pRpcClu->getTof()) - tof_c;
			  hists->hparticle_pi_tof_vs_rod_sys0->Fill(pCand->getSector() * 192 + pRpcClu->getColumn1() * 32 + pRpcClu->getCell1(), time_diff);
			  hists->hparticle_pi_tof_vs_startstrip_sys0->Fill(startStrip, time_diff);
			  hists->hparticle_pi_tof_all_sys0->Fill(time_diff);
			  if(startStrip>-1)  // we have hit in Start det Mod0
			  {
			      hists->hparticle_pi_tof_vs_start_sys0[startStrip]->Fill(time_diff);
                         // this part is prepared for Start detector calibration
                         // 1. subtract StartHit time
                         // 2. Correct tof using Mod0 data if exists

                  //Search all hits in Mod0 within +-20ns and for each of them plot neg pions
                  // needed for Start det calibration: 
                  if (iterStCal) {
                      iterStCal->Reset();
                      while (( stCal = (HStart2Cal*) iterStCal->Next()) != 0 ) {
                          if (stCal->getModule() == 0) {
                              //+-20 ns cut
                              for(int t=1; t<5 ; t++){
                                  if( fabs( stCal->getTime(t)) < 20.0 ){
                                      startCalTimeMod0  = stCal->getTime(t);
                                      startCalStripMod0 = stCal->getStrip();
			                              hists->hparticle_pi_tof_vs_startMod0_sys0[startCalStripMod0]
                                                            ->Fill(time_diff + startTime - startCalTimeMod0 ); 
                                  }
                              } 
                          } //stCal->getModule() == 0  condition
                          // Below Similar procedure for Start Module 1  


                          if (stCal->getModule() == 1) {
                              //+-20 ns cut
                              for(int t=1; t<5 ; t++){
                                  if( fabs( stCal->getTime(t)) < 20.0 ){
                                      startCalTimeMod1  = stCal->getTime(t);
                                      startCalStripMod1 = stCal->getStrip();
			                              hists->hparticle_pi_tof_vs_startMod1_sys0[startCalStripMod1]
                                                            ->Fill(time_diff + startTime - startCalTimeMod1 ); 
                                  }
                              } 
                          } //stCal->getModule() == 0  condition


                      } //end of while
                  }//end of if(iterStCal)
                  
			  }


		      }
		  }
		  if (pCand->getSystem() == 1 && pCand->getTofClstInd() > -1 && catTfClust)
		  {
		      if (NULL != (pTofClu = static_cast<HTofCluster*>(catTfClust->getObject(pCand->getTofClstInd()))) &&
			  pCand->getCharge() < 0. && pCand->getMomentum() > 300.0)
		      {
			  tof_c  = ( pCand->getBeta()*( pTofClu->getTof()))/beta_c;
			  time_diff =  (pTofClu->getTof()) - tof_c;
			  hists->hparticle_pi_tof_vs_rod_sys1->Fill(pCand->getSector() * 64 + (pTofClu->getModule() * 8 + pTofClu->getCell()), time_diff);
			  hists->hparticle_pi_eloss_vs_rod_sys1->Fill(pCand->getSector() * 64 + (pTofClu->getModule() * 8 + pTofClu->getCell()),pTofClu->getEdep() );
			  hists->hparticle_pi_tof_vs_startstrip_sys1->Fill(startStrip, time_diff);
			  hists->hparticle_pi_tof_all_sys1->Fill(time_diff);
			  hists->hparticle_pi_metahit_vs_phi_sys1->Fill(pCand->getPhi(),(pTofClu->getModule() * 8 + pTofClu->getCell()));


                          if(startStrip>-1)
			  {
			      hists->hparticle_pi_tof_vs_start_sys1[startStrip]->Fill(time_diff);
			  }
		      }
		  }
	      }
	  }

	  //For slow protons
	  //eloss
	  if (pCand->getSystem() == 1 && catTfClust)
	  {
	    if (NULL != (pTofClu = static_cast<HTofCluster*>(catTfClust->getObject(pCand->getTofClstInd()))) && pCand->getCharge() == 1 && pCand->getMomentum() > 300 && pCand->getMomentum() < 350 &&pCand->getBeta()>0.25&&pCand->getBeta()<0.4 )
	      {
		hists->hparticle_p_eloss_vs_rod_sys1->Fill(pCand->getSector() * 64 + (pTofClu->getModule() * 8 + pTofClu->getCell()),pTofClu->getEdep() );
	      }
	  }
	  

	  //proton momentum calculated from velocity vs reconstructed momentum
	  if (pCand->getCharge() > 0 && pCand->getBeta() < 1) {
	      if (pCand->getSystem() == 0) hists->hparticle_rk_momdif_sys0_sec[pCand->getSector()]
		  ->Fill(pCand->getMomentum(), pCand->getMomentum() - (938.*pCand->getBeta() * 1. / sqrt(1. - pCand->getBeta()*pCand->getBeta())));
	      if (pCand->getSystem() == 1) hists->hparticle_rk_momdif_sys1_sec[pCand->getSector()]
		  ->Fill(pCand->getMomentum(), pCand->getMomentum() - (938.*pCand->getBeta() * 1. / sqrt(1. - pCand->getBeta()*pCand->getBeta())));
	  }


	  //MetaMatching qualities for Tof, RPC and Shower
	  if(pCand->isTofClstUsed()||pCand->isTofClstUsed())
	  {
	      hists->hparticle_MetaMatchQualTof[pCand->getSector()]->Fill(pCand->getMetaMatchQuality());
	  }

	  if(pCand->isRpcClstUsed())
	  {
	      hists->hparticle_MetaMatchQualRpc[pCand->getSector()]->Fill(pCand->getMetaMatchQuality());
	  }

	  if(pCand->getShowerInd()>-1)
	  {
	      hists->hparticle_MetaMatchQualShw[pCand->getSector()]->Fill(pCand->getMetaMatchQualityShower());
	  }

	  //Multiplicity for Tof, RPC and Shower with and witout partile seclection
	  if(pCand->getSystem()>-1&&pCand->getSystem()<2)
	  {
	      MetaCounts[pCand->getSector()][pCand->getSystem()]++;
	      if(pCand->isFlagBit(Particle::kIsUsed))
	      {
		  MetaCountsSelect[pCand->getSector()][pCand->getSystem()]++;
	      }
	  }

	  if(pCand->getSystem()==1)
	  {
              hists->hparticle_TofdEdx->Fill(pCand->getTofdEdx());
	  }

      }
      //------------End of Runge-Kutta in Pid ----------------------------
   } //PidTrackCandIter
   //----------------- End of loop over particleCand -----------------------
   hists->hparticle_multrk->Fill(multRK);
   varHists->particleCandLep_n_Var->Fill(nEvent, multLepCand);

   for(Int_t i=0; i<6; i++)
   {
       for(Int_t j=0;j<2;j++)
       {
	   hists->hparticle_mult[i][j]->Fill(MetaCounts[i][j]);
	   hists->hparticle_mult_select[i][j]->Fill(MetaCountsSelect[i][j]);
       }
   }
}



void HQAMaker::finalizeHistPid()
{
    Text_t buffer[80];

    for(Int_t i=0; i<6; i++)
    {
	if(hists->hparticle_MetaMatchQualTof[i]->GetEntries()>0)
	{
	    TPhy.fTMetaMatchTofMean[i]=hists->hparticle_MetaMatchQualTof[i]->GetMean();
	    TPhy.fTMetaMatchTofMax[i]=hists->hparticle_MetaMatchQualTof[i]->GetBinCenter(hists->hparticle_MetaMatchQualTof[i]->GetMaximumBin());
	    hists->hparticle_MetaMatchQualTof[i]->GetXaxis()->SetRangeUser(0,10);
	    TPhy.fTMetaMatchTofMax2[i]=hists->hparticle_MetaMatchQualTof[i]->GetBinCenter(hists->hparticle_MetaMatchQualTof[i]->GetMaximumBin());
            hists->hparticle_MetaMatchQualTof[i]->GetXaxis()->SetRangeUser(-10,10);
	}

        if(hists->hparticle_MetaMatchQualRpc[i]->GetEntries()>0)
	{
	    TPhy.fTMetaMatchRpcMean[i]=hists->hparticle_MetaMatchQualRpc[i]->GetMean();
	    TPhy.fTMetaMatchRpcMax[i]=hists->hparticle_MetaMatchQualRpc[i]->GetBinCenter(hists->hparticle_MetaMatchQualRpc[i]->GetMaximumBin());
	    hists->hparticle_MetaMatchQualRpc[i]->GetXaxis()->SetRangeUser(0,10);
	    TPhy.fTMetaMatchRpcMax2[i]=hists->hparticle_MetaMatchQualRpc[i]->GetBinCenter(hists->hparticle_MetaMatchQualRpc[i]->GetMaximumBin());
            hists->hparticle_MetaMatchQualRpc[i]->GetXaxis()->SetRangeUser(-10,10);
	}

        if(hists->hparticle_MetaMatchQualShw[i]->GetEntries()>0)
	{
	    TPhy.fTMetaMatchShwMean[i]=hists->hparticle_MetaMatchQualShw[i]->GetMean();
	    TPhy.fTMetaMatchShwMax[i]=hists->hparticle_MetaMatchQualShw[i]->GetBinCenter(hists->hparticle_MetaMatchQualShw[i]->GetMaximumBin());
	    hists->hparticle_MetaMatchQualShw[i]->GetXaxis()->SetRangeUser(0,10);
	    TPhy.fTMetaMatchShwMax2[i]=hists->hparticle_MetaMatchQualShw[i]->GetBinCenter(hists->hparticle_MetaMatchQualShw[i]->GetMaximumBin());
            hists->hparticle_MetaMatchQualShw[i]->GetXaxis()->SetRangeUser(-10,10);
	}

	for(Int_t j=0; j<2; j++)
	{
	    if(hists->hparticle_mult[i][j]->GetEntries()>0)
	    {
		TPhy.fTMultiMean[i][j]=hists->hparticle_mult[i][j]->GetMean();
                TPhy.fTMultiMax[i][j]=hists->hparticle_mult[i][j]->GetBinCenter(hists->hparticle_mult[i][j]->GetMaximumBin());
	    }
            if(hists->hparticle_mult_select[i][j]->GetEntries()>0)
	    {
		TPhy.fTMultiMeanSelect[i][j]=hists->hparticle_mult_select[i][j]->GetMean();
                TPhy.fTMultiMaxSelect[i][j]=hists->hparticle_mult_select[i][j]->GetBinCenter(hists->hparticle_mult_select[i][j]->GetMaximumBin());
	    }
	}
    }

    if(hists->hparticle_lepton_tof_all_sys0->GetEntries()>0)
    {
	TPhy.fTTimeLepSumSys0Mean=hists->hparticle_lepton_tof_all_sys0->GetMean();
	TPhy.fTTimeLepSumSys0Max=hists->hparticle_lepton_tof_all_sys0->GetBinCenter(hists->hparticle_lepton_tof_all_sys0->GetMaximumBin());
        TPhy.fTTimeLepSumSys0Sig=hists->hparticle_lepton_tof_all_sys0->GetRMS();
    }

    if(hists->hparticle_lepton_tof_all_sys1->GetEntries()>0)
    {
	TPhy.fTTimeLepSumSys1Mean=hists->hparticle_lepton_tof_all_sys1->GetMean();
	TPhy.fTTimeLepSumSys1Max=hists->hparticle_lepton_tof_all_sys1->GetBinCenter(hists->hparticle_lepton_tof_all_sys1->GetMaximumBin());
        TPhy.fTTimeLepSumSys1Sig=hists->hparticle_lepton_tof_all_sys1->GetRMS();
    }

    if(hists->hparticle_pi_tof_all_sys0->GetEntries()>0)
    {
	TPhy.fTTimePiSumSys0Mean=hists->hparticle_pi_tof_all_sys0->GetMean();
	TPhy.fTTimePiSumSys0Max=hists->hparticle_pi_tof_all_sys0->GetBinCenter(hists->hparticle_pi_tof_all_sys0->GetMaximumBin());
	TPhy.fTTimePiSumSys0Sig=hists->hparticle_pi_tof_all_sys0->GetRMS();
	//Fitting
	for(Int_t n=0; n<3; n++)
	{
	    hists->hparticle_pi_tof_GaussFit_all_sys0->ReleaseParameter(n);
	    hists->hparticle_pi_tof_GaussFit_all_sys0->SetParameter(n,0.0);
	}
	if(hists->hparticle_pi_tof_all_sys0->GetEntries()>300)
	{
	    hists->hparticle_pi_tof_GaussFit_all_sys0->SetParameter(0,hists->hparticle_pi_tof_all_sys0->GetMaximum());
	    hists->hparticle_pi_tof_GaussFit_all_sys0->SetParameter(1,TPhy.fTTimePiSumSys0Max);
	    hists->hparticle_pi_tof_GaussFit_all_sys0->SetParameter(2,1.0);
	    hists->hparticle_pi_tof_GaussFit_all_sys0->SetRange(TPhy.fTTimePiSumSys0Max-1,TPhy.fTTimePiSumSys0Max+1);
	    hists->hparticle_pi_tof_all_sys0->Fit("hparticle_pi_tof_GaussFit_all_sys0","+Q","",TPhy.fTTimePiSumSys0Max-0.7,TPhy.fTTimePiSumSys0Max+0.5);
            TPhy.fTTimePiSumSys0FitMean=hists->hparticle_pi_tof_GaussFit_all_sys0->GetParameter(1);
	}
    }

    if(hists->hparticle_pi_tof_all_sys1->GetEntries()>0)
    {
	TPhy.fTTimePiSumSys1Mean=hists->hparticle_pi_tof_all_sys1->GetMean();
	TPhy.fTTimePiSumSys1Max=hists->hparticle_pi_tof_all_sys1->GetBinCenter(hists->hparticle_pi_tof_all_sys1->GetMaximumBin());
	TPhy.fTTimePiSumSys1Sig=hists->hparticle_pi_tof_all_sys1->GetRMS();
        //Fitting
	for(Int_t n=0; n<3; n++)
	{
	    hists->hparticle_pi_tof_GaussFit_all_sys1->ReleaseParameter(n);
	    hists->hparticle_pi_tof_GaussFit_all_sys1->SetParameter(n,0.0);
	}
	if(hists->hparticle_pi_tof_all_sys1->GetEntries()>300)
	{
	    hists->hparticle_pi_tof_GaussFit_all_sys1->SetParameter(0,hists->hparticle_pi_tof_all_sys1->GetMaximum());
	    hists->hparticle_pi_tof_GaussFit_all_sys1->SetParameter(1,TPhy.fTTimePiSumSys1Max);
	    hists->hparticle_pi_tof_GaussFit_all_sys1->SetParameter(2,1.0);
	    hists->hparticle_pi_tof_GaussFit_all_sys1->SetRange(TPhy.fTTimePiSumSys1Max-1,TPhy.fTTimePiSumSys1Max+1);
	    hists->hparticle_pi_tof_all_sys1->Fit("hparticle_pi_tof_GaussFit_all_sys1","+Q","",TPhy.fTTimePiSumSys1Max-0.7,TPhy.fTTimePiSumSys1Max+0.5);
            TPhy.fTTimePiSumSys1FitMean=hists->hparticle_pi_tof_GaussFit_all_sys1->GetParameter(1);
	}
    }

    for(Int_t i=0; i<NSTART_STRIPS; i++)
    {
	if(hists->hparticle_lepton_tof_vs_start_sys0[i]->GetEntries()>0)
	{
	    TPhy.fTTimeLepStaSys0Mean[i]=hists->hparticle_lepton_tof_vs_start_sys0[i]->GetMean();
	    TPhy.fTTimeLepStaSys0Max[i]=hists->hparticle_lepton_tof_vs_start_sys0[i]->GetBinCenter(hists->hparticle_lepton_tof_vs_start_sys0[i]->GetMaximumBin());
            TPhy.fTTimeLepStaSys0Sig[i]=hists->hparticle_lepton_tof_vs_start_sys0[i]->GetRMS();
	}

        if(hists->hparticle_lepton_tof_vs_start_sys1[i]->GetEntries()>0)
	{
	    TPhy.fTTimeLepStaSys1Mean[i]=hists->hparticle_lepton_tof_vs_start_sys1[i]->GetMean();
	    TPhy.fTTimeLepStaSys1Max[i]=hists->hparticle_lepton_tof_vs_start_sys1[i]->GetBinCenter(hists->hparticle_lepton_tof_vs_start_sys1[i]->GetMaximumBin());
            TPhy.fTTimeLepStaSys1Sig[i]=hists->hparticle_lepton_tof_vs_start_sys1[i]->GetRMS();
	}

        if(hists->hparticle_pi_tof_vs_start_sys0[i]->GetEntries()>0)
	{
	    TPhy.fTTimePiStaSys0Mean[i]=hists->hparticle_pi_tof_vs_start_sys0[i]->GetMean();
	    TPhy.fTTimePiStaSys0Max[i]=hists->hparticle_pi_tof_vs_start_sys0[i]->GetBinCenter(hists->hparticle_pi_tof_vs_start_sys0[i]->GetMaximumBin());
	    TPhy.fTTimePiStaSys0Sig[i]=hists->hparticle_pi_tof_vs_start_sys0[i]->GetRMS();
            //Fitting
	    for(Int_t n=0; n<3; n++)
	    {
		hists->hparticle_pi_tof_GaussFit_sys0[i]->ReleaseParameter(n);
		hists->hparticle_pi_tof_GaussFit_sys0[i]->SetParameter(n,0.0);
	    }
	    if(hists->hparticle_pi_tof_vs_start_sys0[i]->GetEntries()>300)
	    {
                sprintf(buffer,"hparticle_pi_tof_GaussFit_sys0[%i]",i);
		hists->hparticle_pi_tof_GaussFit_sys0[i]->SetParameter(0,hists->hparticle_pi_tof_vs_start_sys0[i]->GetMaximum());
		hists->hparticle_pi_tof_GaussFit_sys0[i]->SetParameter(1,TPhy.fTTimePiStaSys0Max[i]);
		hists->hparticle_pi_tof_GaussFit_sys0[i]->SetParameter(2,1.0);
		hists->hparticle_pi_tof_GaussFit_sys0[i]->SetRange(TPhy.fTTimePiStaSys0Max[i]-1,TPhy.fTTimePiStaSys0Max[i]+1);
		hists->hparticle_pi_tof_vs_start_sys0[i]->Fit(buffer,"+Q","",TPhy.fTTimePiStaSys0Max[i]-0.7,TPhy.fTTimePiStaSys0Max[i]+0.5);
		TPhy.fTTimePiStaSys0FitMean[i]=hists->hparticle_pi_tof_GaussFit_sys0[i]->GetParameter(1);
	    }
	}

        if(hists->hparticle_pi_tof_vs_start_sys1[i]->GetEntries()>0)
	{
	    TPhy.fTTimePiStaSys1Mean[i]=hists->hparticle_pi_tof_vs_start_sys1[i]->GetMean();
	    TPhy.fTTimePiStaSys1Max[i]=hists->hparticle_pi_tof_vs_start_sys1[i]->GetBinCenter(hists->hparticle_pi_tof_vs_start_sys1[i]->GetMaximumBin());
	    TPhy.fTTimePiStaSys1Sig[i]=hists->hparticle_pi_tof_vs_start_sys1[i]->GetRMS();
            //Fitting
	    for(Int_t n=0; n<3; n++)
	    {
		hists->hparticle_pi_tof_GaussFit_sys1[i]->ReleaseParameter(n);
		hists->hparticle_pi_tof_GaussFit_sys1[i]->SetParameter(n,0.0);
	    }
	    if(hists->hparticle_pi_tof_vs_start_sys1[i]->GetEntries()>300)
	    {
                sprintf(buffer,"hparticle_pi_tof_GaussFit_sys1[%i]",i);
		hists->hparticle_pi_tof_GaussFit_sys1[i]->SetParameter(0,hists->hparticle_pi_tof_vs_start_sys1[i]->GetMaximum());
		hists->hparticle_pi_tof_GaussFit_sys1[i]->SetParameter(1,TPhy.fTTimePiStaSys1Max[i]);
		hists->hparticle_pi_tof_GaussFit_sys1[i]->SetParameter(2,1.0);
		hists->hparticle_pi_tof_GaussFit_sys1[i]->SetRange(TPhy.fTTimePiStaSys1Max[i]-1,TPhy.fTTimePiStaSys1Max[i]+1);
		hists->hparticle_pi_tof_vs_start_sys1[i]->Fit(buffer,"+Q","",TPhy.fTTimePiStaSys1Max[i]-0.7,TPhy.fTTimePiStaSys1Max[i]+0.5);
		TPhy.fTTimePiStaSys1FitMean[i]=hists->hparticle_pi_tof_GaussFit_sys1[i]->GetParameter(1);
	    }
	}
    }

    if(hists->hparticle_TofdEdx->GetEntries()>0)
    {
	TPhy.fTTofdEdxMean=hists->hparticle_TofdEdx->GetMean();
        TPhy.fTTofdEdxMax=hists->hparticle_TofdEdx->GetBinCenter(hists->hparticle_TofdEdx->GetMaximumBin());
    }
}






void HQAMaker::fillHistDaqScaler()
{
   HTBoxChan *pTBoxChannel;
   Int_t chan   = 0;
   UInt_t scaler = 0;

   if (!iterHTBoxChan) {
      return;
   }

   //Int_t eventnb = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
   //cout<<"----event seq nb: "<<eventnb<<endl;
   iterHTBoxChan->Reset();

   while ((pTBoxChannel = (HTBoxChan *)iterHTBoxChan->Next()) != 0) {

      pTBoxChannel->getScalerData(chan, scaler);   // read scaler data 
   
      //cout<<"---scaler data, chan: "<<chan<<"   data: "<<scaler<<endl;
      hists-> histAllScalerCh -> Fill(chan+1,scaler);
      hists-> histAllScalerTrend -> Fill(nCountCalEvents, chan+1,scaler);

      if(chan>=74&&chan<82)  hists->  histInputScaler->Fill(chan+1-74,scaler);
      if(chan>=93&&chan<101) hists->  histDownScaler->Fill(chan+1-93,scaler);
      if(chan>=129&&chan<137)hists->  histGatedScaler->Fill(chan+1-129,scaler);
      if(chan>=112&&chan<120)hists->  histAcceptScaler->Fill(chan+1-112,scaler);
                                    
      if(chan>=46&&chan<54)  hists->  histStartScaler->Fill(chan+1-46,scaler);
      if(chan==82)           hists->  histStartScaler->Fill(9,scaler);
      if(chan>=54&&chan<62)  hists->  histVetoScaler->Fill(chan+1-54,scaler);
      if(chan>=62&&chan<68)  hists->  histTofScaler->Fill(chan+1-62,scaler);
      if(chan>=68&&chan<74)  hists->  histRpcScaler->Fill(chan+1-68,scaler);

//      hists->histInput[chan]->Fill(nCountCalEvents, scaler);
//      hists->histReduce[chan]->Fill(nCountCalEvents, scaler);
//      hists->histStartScaler[chan]->Fill(nCountCalEvents, scaler);
//      hists->histVetoScaler[chan]->Fill(nCountCalEvents, scaler);
   }
}

void HQAMaker::fillHistShowerRpc()
{
   HShowerHit *shoHit = 0;
   HRpcHit *rpcHit = 0;
   Int_t shoSec, shoMod, rpcSec;
   Float_t shoX, shoY, rpcX, rpcY;

   if (iterShoHit && iterRpcHit) {
      iterShoHit->Reset();
//        nDataObjs = ((TObjArray*)iterShoHit->GetCollection())->GetEntries();
//        varHists->shoHit_n_Var->Fill(nEvent, nDataObjs);

      while ((shoHit = (HShowerHit*) iterShoHit->Next()) != 0) {
         shoSec = shoHit->getSector();
         shoMod = shoHit->getModule();
         shoHit->getXY(&shoX, &shoY);
         iterRpcHit->Reset();
         while ((rpcHit = (HRpcHit*) iterRpcHit->Next()) != 0) {
            rpcSec = rpcHit->getSector();
            rpcX   = rpcHit->getXMod();
            rpcY   = rpcHit->getYMod();
            if (shoSec == rpcSec) {
               switch (shoMod) {
                  case 0:
		     hists->shorpcXdiff_pre[shoSec]->Fill(shoX - rpcX);       // ## schimmeck -- changed back to -
                     hists->shorpcYdiff_pre[shoSec]->Fill(shoY - rpcY);       
                     hists->shorpcXvs_pre[shoSec]->Fill(shoX, rpcX);         // ## schimmeck -- changed back to -
                     hists->shorpcYvs_pre[shoSec]->Fill(shoY, rpcY);          
                     break;
                  case 1:
                     hists->shorpcXdiff_post1[shoSec]->Fill(shoX - rpcX);  // ## schimmeck -- changed back to -
                     hists->shorpcYdiff_post1[shoSec]->Fill(shoY - rpcY);
                     hists->shorpcXvs_post1[shoSec]->Fill(shoX, rpcX);      // ## schimmeck -- changed back to -
                     hists->shorpcYvs_post1[shoSec]->Fill(shoY, rpcY);
                     break;
                  case 2:
                     hists->shorpcXdiff_post2[shoSec]->Fill(shoX - rpcX);  // ## schimmeck -- changed back to -
                     hists->shorpcYdiff_post2[shoSec]->Fill(shoY - rpcY);
                     hists->shorpcXvs_post2[shoSec]->Fill(shoX, rpcX);
                     hists->shorpcYvs_post2[shoSec]->Fill(shoY, rpcY);
                     break;
               }
            }
         }
      }
   }
}

void HQAMaker::finalizeHistShowerRpc()
{
}

void HQAMaker::fillGlobalVertex()
{
   HVertex pGlobVertex = gHades->getCurrentEvent()->getHeader()->getVertex();
   Float_t fVertX = pGlobVertex.getX();
   Float_t fVertY = pGlobVertex.getY();
   Float_t fVertZ = pGlobVertex.getZ();
   Float_t fVertChi2 = pGlobVertex.getChi2();

   if (fVertChi2 > -1.0) {
      hists->stVertexXY->Fill(fVertX, fVertY);
      hists->stVertexZ->Fill(fVertZ);
   }
}

//Johannes Test
void HQAMaker::fillMassSpectrum()
{
    HParticleCand   *pCand   = 0;

    if (NULL == iterParticleCand) {
	return ;  // do nothing if ParticleCandidate does not exist
    }

    iterParticleCand->Reset();

    while (NULL != (pCand = static_cast<HParticleCand*>(iterParticleCand->Next())))
    {
	hists->hparticle_mass ->Fill(sqrt(pCand->getMass2())*pCand->getCharge());

	if(pCand->getSystem()==0)
	{
            hists->hparticle_mass_RPC ->Fill(sqrt(pCand->getMass2())*pCand->getCharge());
	}
        if(pCand->getSystem()==1)
	{
            hists->hparticle_mass_TOF ->Fill(sqrt(pCand->getMass2())*pCand->getCharge());
	}
    }
}

void HQAMaker::finalizeMassSpectrum()
{
    //************************************************************
    for(Int_t n = 0; n < 3; n++)
    {
        hists->hparticle_mass_GaussFit->ReleaseParameter(n);
	hists->hparticle_mass_GaussFit->SetParameter(n,0.0);
	hists->hparticle_mass_GaussFit_1->ReleaseParameter(n);
	hists->hparticle_mass_GaussFit_1->SetParameter(n,0.0);
        hists->hparticle_mass_GaussFit_2->ReleaseParameter(n);
	hists->hparticle_mass_GaussFit_2->SetParameter(n,0.0);

        hists->hparticle_mass_RPC_GaussFit->ReleaseParameter(n);
	hists->hparticle_mass_RPC_GaussFit->SetParameter(n,0.0);
        hists->hparticle_mass_RPC_GaussFit_1->ReleaseParameter(n);
	hists->hparticle_mass_RPC_GaussFit_1->SetParameter(n,0.0);
        hists->hparticle_mass_RPC_GaussFit_2->ReleaseParameter(n);
	hists->hparticle_mass_RPC_GaussFit_2->SetParameter(n,0.0);

        hists->hparticle_mass_TOF_GaussFit->ReleaseParameter(n);
	hists->hparticle_mass_TOF_GaussFit->SetParameter(n,0.0);
        hists->hparticle_mass_TOF_GaussFit_1->ReleaseParameter(n);
	hists->hparticle_mass_TOF_GaussFit_1->SetParameter(n,0.0);
        hists->hparticle_mass_TOF_GaussFit_2->ReleaseParameter(n);
	hists->hparticle_mass_TOF_GaussFit_2->SetParameter(n,0.0);
    }
    //*************************************************************

    //**********************************************************************************************
    hists->hparticle_mass_GaussFit->SetParameter(0,hists->hparticle_mass->GetMaximum());  //Protons
    hists->hparticle_mass_GaussFit->SetParameter(1,900.0);
    hists->hparticle_mass_GaussFit->SetParameter(2,30.0);

    hists->hparticle_mass_GaussFit_1->SetParameter(0,hists->hparticle_mass->GetMaximum()/3.0); //Pi+
    hists->hparticle_mass_GaussFit_1->SetParameter(1,140.0);
    hists->hparticle_mass_GaussFit_1->SetParameter(2,30.0);

    hists->hparticle_mass_GaussFit_2->SetParameter(0,hists->hparticle_mass->GetMaximum()); //Pi-
    hists->hparticle_mass_GaussFit_2->SetParameter(1,-140.0);
    hists->hparticle_mass_GaussFit_2->SetParameter(2,30.0);

    hists->hparticle_mass_RPC_GaussFit->SetParameter(0,hists->hparticle_mass_RPC->GetMaximum());  //Protons
    hists->hparticle_mass_RPC_GaussFit->SetParameter(1,900.0);
    hists->hparticle_mass_RPC_GaussFit->SetParameter(2,30.0);

    hists->hparticle_mass_RPC_GaussFit_1->SetParameter(0,hists->hparticle_mass_RPC->GetMaximum()/3.0); //Pi+
    hists->hparticle_mass_RPC_GaussFit_1->SetParameter(1,140.0);
    hists->hparticle_mass_RPC_GaussFit_1->SetParameter(2,30.0);

    hists->hparticle_mass_RPC_GaussFit_2->SetParameter(0,hists->hparticle_mass_RPC->GetMaximum()); //Pi-
    hists->hparticle_mass_RPC_GaussFit_2->SetParameter(1,-140.0);
    hists->hparticle_mass_RPC_GaussFit_2->SetParameter(2,30.0);

    hists->hparticle_mass_TOF_GaussFit->SetParameter(0,hists->hparticle_mass_TOF->GetMaximum());  //Protons
    hists->hparticle_mass_TOF_GaussFit->SetParameter(1,900.0);
    hists->hparticle_mass_TOF_GaussFit->SetParameter(2,30.0);

    hists->hparticle_mass_TOF_GaussFit_1->SetParameter(0,hists->hparticle_mass_TOF->GetMaximum()/3.0); //Pi+
    hists->hparticle_mass_TOF_GaussFit_1->SetParameter(1,140.0);
    hists->hparticle_mass_TOF_GaussFit_1->SetParameter(2,30.0);

    hists->hparticle_mass_TOF_GaussFit_2->SetParameter(0,hists->hparticle_mass_TOF->GetMaximum()); //Pi-
    hists->hparticle_mass_TOF_GaussFit_2->SetParameter(1,-140.0);
    hists->hparticle_mass_TOF_GaussFit_2->SetParameter(2,30.0);
    //*********************************************************************************************

    //*********************************************************************************************
    hists->hparticle_mass_GaussFit->SetRange(810,1030);
    hists->hparticle_mass_GaussFit_1->SetRange(90,190);
    hists->hparticle_mass_GaussFit_2->SetRange(-190,-90);

    hists->hparticle_mass_RPC_GaussFit->SetRange(810,1030);
    hists->hparticle_mass_RPC_GaussFit_1->SetRange(90,190);
    hists->hparticle_mass_RPC_GaussFit_2->SetRange(-190,-90);

    hists->hparticle_mass_TOF_GaussFit->SetRange(810,1030);
    hists->hparticle_mass_TOF_GaussFit_1->SetRange(90,190);
    hists->hparticle_mass_TOF_GaussFit_2->SetRange(-190,-90);
    //*********************************************************************************************

    //*********************************************************************************************
    hists->hparticle_mass->Fit("hparticle_mass_GaussFit","+Q","",810,1030);
    hists->hparticle_mass->Fit("hparticle_mass_GaussFit_1","+Q","",90,190);
    hists->hparticle_mass->Fit("hparticle_mass_GaussFit_2","+Q","",-190,-90);

    if(hists->hparticle_mass->GetEntries()>0)
    {
	TPhy.fTMass_proton = hists->hparticle_mass_GaussFit->GetParameter(1);
	TPhy.fTMass_pip    = hists->hparticle_mass_GaussFit_1->GetParameter(1);
	TPhy.fTMass_pim    = hists->hparticle_mass_GaussFit_2->GetParameter(1);
	TPhy.fTChi2_proton = hists->hparticle_mass_GaussFit->GetChisquare();
	TPhy.fTChi2_pip    = hists->hparticle_mass_GaussFit_1->GetChisquare();
	TPhy.fTChi2_pim    = hists->hparticle_mass_GaussFit_2->GetChisquare();
    }

    hists->hparticle_mass_RPC->Fit("hparticle_mass_RPC_GaussFit","+Q","",810,1030);
    hists->hparticle_mass_RPC->Fit("hparticle_mass_RPC_GaussFit_1","+Q","",90,190);
    hists->hparticle_mass_RPC->Fit("hparticle_mass_RPC_GaussFit_2","+Q","",-190,90);

    if(hists->hparticle_mass_RPC->GetEntries()>0)
    {
	TPhy.fTMass_proton_RPC = hists->hparticle_mass_RPC_GaussFit->GetParameter(1);
	TPhy.fTMass_pip_RPC    = hists->hparticle_mass_RPC_GaussFit_1->GetParameter(1);
	TPhy.fTMass_pim_RPC    = hists->hparticle_mass_RPC_GaussFit_2->GetParameter(1);
	TPhy.fTChi2_proton_RPC = hists->hparticle_mass_RPC_GaussFit->GetChisquare();
	TPhy.fTChi2_pip_RPC    = hists->hparticle_mass_RPC_GaussFit_1->GetChisquare();
	TPhy.fTChi2_pim_RPC    = hists->hparticle_mass_RPC_GaussFit_2->GetChisquare();
    }

    hists->hparticle_mass_TOF->Fit("hparticle_mass_TOF_GaussFit","+Q","",810,1030);
    hists->hparticle_mass_TOF->Fit("hparticle_mass_TOF_GaussFit_1","+Q","",90,190);
    hists->hparticle_mass_TOF->Fit("hparticle_mass_TOF_GaussFit_2","+Q","",-190,90);

    if(hists->hparticle_mass_TOF->GetEntries()>0)
    {
	TPhy.fTMass_proton_TOF = hists->hparticle_mass_TOF_GaussFit->GetParameter(1);
	TPhy.fTMass_pip_TOF    = hists->hparticle_mass_TOF_GaussFit_1->GetParameter(1);
	TPhy.fTMass_pim_TOF    = hists->hparticle_mass_TOF_GaussFit_2->GetParameter(1);
	TPhy.fTChi2_proton_TOF = hists->hparticle_mass_TOF_GaussFit->GetChisquare();
	TPhy.fTChi2_pip_TOF    = hists->hparticle_mass_TOF_GaussFit_1->GetChisquare();
	TPhy.fTChi2_pim_TOF    = hists->hparticle_mass_TOF_GaussFit_2->GetChisquare();
    }


}



