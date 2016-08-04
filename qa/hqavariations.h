//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
#ifndef HQAVARIATIONS_H
#define HQAVARIATIONS_H

#include "TObject.h"

class TProfile;

class HQAVariations : public TObject {

public:
   HQAVariations(Int_t intervalSize = 5000);
   virtual ~HQAVariations() {}

   virtual void bookHist();
   virtual void activateHist();

   TList *getHistList() {
      return histList;
   }

private:
   virtual void bookHistEvent();
   virtual void bookHistStart();
   virtual void bookHistRich();
   virtual void bookHistMdc();
   virtual void bookHistShower();
   virtual void bookHistTof();
   virtual void bookHistRpc();
   virtual void bookHistPid();

   void resetPointers();

public:
   TList *histList;    //! List of booked histograms

   Int_t maxEvents;    //! Max. #events to be QA'd (= 200*IntervalSize)

   // Event histograms
   TProfile *evtHeader_eventSize_Var;  //! evtHeader: event size

   // Start-Veto histograms
   TProfile *stCal_meanStrip_Var;  //! startCal: mean strip number
   TProfile *vtCal_meanStrip_Var;  //! vetoCal: mean strip number


   // Rich histograms
   TProfile *richCal_n_Var;     //! richCal: fired pads/event
   TProfile *richHit_n_Var;     //! richHit: hits/event

   // Mdc histograms
   TProfile *mdcCal1_time1_m0_Var;  //! mdcCal1: Plane I, average time1
   TProfile *mdcCal1_time1_m1_Var;  //! mdcCal1: Plane II, average time1
   TProfile *mdcCal1_time1_m2_Var;  //! mdcCal1: Plane III, average time1
   TProfile *mdcCal1_time1_m3_Var;  //! mdcCal1: Plane IV, average time1

   TProfile *mdcCal1_time2m1_m0_Var;  //! mdcCal1: Plane I, average time above threshold
   TProfile *mdcCal1_time2m1_m1_Var;  //! mdcCal1: Plane II, average time above threshold
   TProfile *mdcCal1_time2m1_m2_Var;  //! mdcCal1: Plane III, average time above threshold
   TProfile *mdcCal1_time2m1_m3_Var;  //! mdcCal1: Plane IV, average time above threshold

   // Tof histograms
   TProfile *tofHit_n_Var;       //! tofHit: num hits/event

   // Rpc histograms
   TProfile *rpcHit_n_Var;       //! rpcHit: num hits/event

   // Shower histograms
   TProfile *shoHit_n_Var;          //! showerHit: num hits/event
   TProfile *shoHitTof_n_Var;       //! showerHitTof: num hits/event

   // Pid histograms
   TProfile *particleCand_n_Var; //! particleCand objects/event
   TProfile *particleCandLep_n_Var; //! pidTrackCand objects with ring/event

   ClassDef(HQAVariations, 1) // QA variation histograms
};

#endif



