using namespace std;
#include <iostream>
#include <iomanip>
#include <math.h>
// Root includes
#include "TObjString.h"
#include "TProfile.h"
#include "TList.h"
// Hydra includes
#include "hqavariations.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
// HQAVariations
//
// Contains TProfile histograms for monitoring the variations of scalar quantities
// over the course of the run, used by HQAMaker.
//
// BEGIN_HTML<b><font color="maroon">For more information, see the <a href="http://www-hades.gsi.de/computing/qa/">QA home page</a>.</font></b> END_HTML
//
//////////////////////////////////////////////////////////////////////////////

ClassImp(HQAVariations)

HQAVariations::HQAVariations(Int_t intervalSize)
{
   // Constructor; must be called with the interval size since this is needed to
   // book the histograms.

   maxEvents = intervalSize * 500;

   histList = new TList();
   resetPointers();
}

void HQAVariations::resetPointers()
{
   // Zero all histogram pointers

   //---- Start histograms
   stCal_meanStrip_Var = 0;
   vtCal_meanStrip_Var = 0;

   //---- Rich histograms
   richCal_n_Var = 0;
   richHit_n_Var = 0;

   //---- Mdc histograms
   mdcCal1_time1_m0_Var = 0;
   mdcCal1_time1_m1_Var = 0;
   mdcCal1_time1_m2_Var = 0;
   mdcCal1_time1_m3_Var = 0;

   //---- Tof histograms
   tofHit_n_Var = 0;

   //---- Tofino histograms

   //---- Shower histograms
   shoHit_n_Var = 0;
   shoHitTof_n_Var = 0;

   //---- Pid histograms
   particleCand_n_Var = 0;
   particleCandLep_n_Var = 0;

}

void HQAVariations::activateHist()
{
   // This function removes histograms from the histList TList which
   // are not listed in QAvarhists.h.

   TList *activeList = new TList();

   const Char_t *hists[1000] = {
#include "QAvarhists.h"
   };

   // Convert Char_t to TList of TObjStrings
   for (Int_t i = 0; hists[i] != 0; i++)
      activeList->Add(new TObjString(hists[i]));

   // Iterate over histList, remove entries which aren't in activeList
   TIter next(histList);
   while (TH1 *hist = (TH1*) next()) {
      hist->SetXTitle("event number (position in DST file)");
      if (! activeList->Contains(hist->GetName()))
         histList->Remove(hist);
   }
   activeList->Delete();
   delete activeList;
}

void HQAVariations::bookHist()
{
   // Book histograms and define axis titles and options for drawing

   bookHistEvent();
   bookHistStart();
   bookHistRich();
   bookHistMdc();
   bookHistTof();
   bookHistRpc();
   bookHistShower();
   bookHistPid();

   activateHist();
}

void HQAVariations::bookHistEvent()
{
 
   evtHeader_eventSize_Var = new TProfile("evtHeader_eventSize_Var", "evtHeader: event size", 500, 0., maxEvents);
   evtHeader_eventSize_Var->SetYTitle("event size (bytes)");
   histList->Add(evtHeader_eventSize_Var, "variations--");
}

void HQAVariations::bookHistStart()
{

   stCal_meanStrip_Var = new TProfile("stCal_meanStrip_Var", "StCal: mean strip number", 500, 0., maxEvents);
   stCal_meanStrip_Var->SetYTitle("St mean strip number");
   histList->Add(stCal_meanStrip_Var, "variations--");

   vtCal_meanStrip_Var = new TProfile("vtCal_meanStrip_Var", "VtCal: mean strip number", 500, 0., maxEvents);
   vtCal_meanStrip_Var->SetYTitle("Vt mean strip number");
   histList->Add(vtCal_meanStrip_Var, "variations--");




}

void HQAVariations::bookHistRich()
{

   richCal_n_Var = new TProfile("richCal_n_Var", "richCal: fired pads/event", 500, 0., maxEvents);
   richCal_n_Var->SetYTitle("fired pads/event");
   histList->Add(richCal_n_Var, "variations--");

   richHit_n_Var = new TProfile("richHit_n_Var", "richHit: hits/event", 500, 0., maxEvents);
   richHit_n_Var->SetYTitle("hits/event");
   histList->Add(richHit_n_Var, "variations--");
}

void HQAVariations::bookHistMdc()
{

   mdcCal1_time1_m0_Var = new TProfile("mdcCal1_time1_m0_Var", "mdcCal1: Plane I, average time1", 500, 0., maxEvents);
   mdcCal1_time1_m0_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time1_m0_Var, "variations--");

   mdcCal1_time1_m1_Var = new TProfile("mdcCal1_time1_m1_Var", "mdcCal1: Plane II, average time1", 500, 0., maxEvents);
   mdcCal1_time1_m1_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time1_m1_Var, "variations--");

   mdcCal1_time1_m2_Var = new TProfile("mdcCal1_time1_m2_Var", "mdcCal1: Plane III, average time1", 500, 0., maxEvents);
   mdcCal1_time1_m2_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time1_m2_Var, "variations--");

   mdcCal1_time1_m3_Var = new TProfile("mdcCal1_time1_m3_Var", "mdcCal1: Plane IV, average time1", 500, 0., maxEvents);
   mdcCal1_time1_m3_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time1_m3_Var, "variations--");

   mdcCal1_time2m1_m0_Var = new TProfile("mdcCal1_time2m1_m0_Var", "mdcCal1: Plane I, average time above threshold", 500, 0., maxEvents);
   mdcCal1_time2m1_m0_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time2m1_m0_Var, "variations--");

   mdcCal1_time2m1_m1_Var = new TProfile("mdcCal1_time2m1_m1_Var", "mdcCal1: Plane II, average time above threshold", 500, 0., maxEvents);
   mdcCal1_time2m1_m1_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time2m1_m1_Var, "variations--");

   mdcCal1_time2m1_m2_Var = new TProfile("mdcCal1_time2m1_m2_Var", "mdcCal1: Plane III, average time above threshold", 500, 0., maxEvents);
   mdcCal1_time2m1_m2_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time2m1_m2_Var, "variations--");

   mdcCal1_time2m1_m3_Var = new TProfile("mdcCal1_time2m1_m3_Var", "mdcCal1: Plane IV, average time above threshold", 500, 0., maxEvents);
   mdcCal1_time2m1_m3_Var->SetYTitle("time (ns)");
   histList->Add(mdcCal1_time2m1_m3_Var, "variations--");

}

void HQAVariations::bookHistTof()
{

   tofHit_n_Var = new TProfile("tofHit_n_Var", "tofHit: num hits/event", 500, 0., maxEvents);
   tofHit_n_Var->SetYTitle("hits/event");
   histList->Add(tofHit_n_Var, "variations--");
}

void HQAVariations::bookHistRpc()
{

   rpcHit_n_Var = new TProfile("rpcHit_n_Var", "rpcHit: num hits/event", 500, 0., maxEvents);
   rpcHit_n_Var->SetYTitle("hits/event");
   histList->Add(rpcHit_n_Var, "variations--");

}

void HQAVariations::bookHistShower()
{

   shoHit_n_Var = new TProfile("shoHit_n_Var", "showerHit: num hits/event", 500, 0., maxEvents);
   shoHit_n_Var->SetYTitle("hits/event");
   histList->Add(shoHit_n_Var, "variations--");

   shoHitTof_n_Var = new TProfile("shoHitTof_n_Var", "showerHitTof: num hits/event", 500, 0., maxEvents);
   shoHitTof_n_Var->SetYTitle("hits/event");
   histList->Add(shoHitTof_n_Var, "variations--");
}

void HQAVariations::bookHistPid()
{
   particleCand_n_Var = new TProfile("particleCand_n_Var", "particleCand: objects/event", 500, 0., maxEvents);
   particleCand_n_Var->SetYTitle("particleCands/event");
   histList->Add(particleCand_n_Var, "variations--");

   particleCandLep_n_Var = new TProfile("particleCandLep_n_Var", "particleCand: objects with ring/event", 500, 0., maxEvents);
   particleCandLep_n_Var->SetYTitle("particleCand with ring/event");
   histList->Add(particleCandLep_n_Var, "variations--");
}
































