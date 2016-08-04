using namespace std;
#include <iostream>
#include <iomanip>
#include <math.h>
// Root includes
#include "TObjString.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TList.h"
// Hydra includes
#include "hqahistograms.h"
#include "QAsectioncolors.h"
#include "hqafitfunctions.h"

//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//
//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
//*-- Modified : 20/01/2004 Jacek Otwinowski
//*-- Modified : 23/01/2004 A.Sadovski
//*-- Modified : 04/01/2005 Jacek Otwinowski
//*-- Modified : 08/02/2005 A.Sadovski
//*-- Modified : 18/05/2006 J.Pietraszko
//-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-//

//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
// HQAHistograms
//
// Contains general histograms for use by HQAMaker.
//
// BEGIN_HTML<b><font color="maroon">For more information, see the <a href="http://www-hades.gsi.de/computing/qa/">QA home page</a>.</font></b> END_HTML
//
//////////////////////////////////////////////////////////////////////////////


ClassImp(HQAHistograms)

HQAHistograms::HQAHistograms()
{

   histList = new TList();
   resetPointers();
}

void HQAHistograms::resetPointers()
{
   // --- Zero all histogram pointers

   //---- Start histograms
   stLatchHist = 0;
   stCal_stripMod0 = 0;
   stCal_stripMod1 = 0;
   vtCal_strip = 0;
   stCal_tof = 0;
   vtCal_tof = 0;
   for (Int_t ss = 0; ss < 8; ss++) {
       stCal_tof_strip[ss] =0;
       vtCal_tof_strip[ss] =0;
   }

   stHit_tof_vs_startstripe = 0;
   stHit_vs_stStrip = 0;
   for (Int_t ss = 0; ss < 9; ss++) {
      stHit_vs_stStrip_Trigg[ss] = 0;
   }

   //---- Daq Scalers histograms
   for (Int_t ii = 0; ii < 8; ii++) {
      //histReduce[ii] = 0;
      //histInput[ii] = 0;
      //histStartScaler[ii] = 0;
      //histVetoScaler[ii] = 0;
   }

   histInputScaler    =0;
   histDownScaler     =0;
   histGatedScaler    =0;
   histAcceptScaler   =0;
   histStartScaler    =0;
   histVetoScaler     =0;
   histTofScaler      =0;
   histRpcScaler      =0;

   histAllScalerCh    =0;
   histAllScalerTrend =0;

   stVertexXY = 0;
   stVertexZ = 0;
   

   //---- Rich histograms
   richCal_row = 0;
   richCal_column = 0;
   richCal_nSec = 0;

   richHit_scat = 0;
   richHit_nSec = 0;

   richHit_phi = 0;
   richHit_theta = 0;
   richHit_radius = 0;
   richHit_centroid = 0;
   richHit_chargeAmpl = 0;
   richHit_ringCol = 0;
   richHit_ringRow = 0;
   richHit_ringLocMax4 = 0;

   richHit_houTraVsPatMat = 0;
   richHit_patMatVsTheta = 0;
   richHit_houTraVsTheta = 0;
   richHit_chargeAmplVsTheta = 0;
   richHit_radiusVsTheta = 0;

   for(Int_t i=0;i<6;i++)
   {
       richHit_AverCharge[i] =0;
   }
   for(Int_t i=0;i<6;i++)
   {
       richHit_NumPads[i] =0;
   }
   for(Int_t i=0;i<6;i++)
   {
       richHit_NumRings[i] =0;
   }
   //---- Mdc histograms
   mdcRaw_mboVsSector_m0 = 0;
   mdcRaw_mboVsSector_m1 = 0;
   mdcRaw_mboVsSector_m2 = 0;
   mdcRaw_mboVsSector_m3 = 0;

   mdcCal1_nVsSector_m0 = 0;
   mdcCal1_nVsSector_m1 = 0;
   mdcCal1_nVsSector_m2 = 0;
   mdcCal1_nVsSector_m3 = 0;

   mdcCal1_time1VsSector_m0 = 0;
   mdcCal1_time1VsSector_m1 = 0;
   mdcCal1_time1VsSector_m2 = 0;
   mdcCal1_time1VsSector_m3 = 0;

   mdcCal1_tatVsSector_m0 = 0;
   mdcCal1_tatVsSector_m1 = 0;
   mdcCal1_tatVsSector_m2 = 0;
   mdcCal1_tatVsSector_m3 = 0;

   mdcHit_scat_m0 = 0;
   mdcHit_scat_m1 = 0;
   mdcHit_scat_m2 = 0;
   mdcHit_scat_m3 = 0;


   for(Int_t i=0; i<6; i++)
   {
       for(Int_t j=0; j<2; j++)
       {
           mdcSeg_Chi2[i][j]=0;
       }

       for(Int_t j=0; j<4; j++)
       {
	   for(Int_t k=0; k<6; k++)
	   {
               mdcCal1_t2mt1_V2[i][j][k]=0;
               mdcCal1_t1_V2[i][j][k]=0;
	   }
       }
   }


   //---- Tof histograms
   tofHit_scat = 0;
   tofHit_tof = 0;
   tofHit_nSec = 0;
   tofHit_phi = 0;
   tofHit_theta = 0;

   for (Int_t s = 0; s < 6; s++) {
       tofHit_n[s]=0;
   }
   tofHit_tot = 0;

   //---- Rpc histograms
   //rpcHit_n     = 0;
   rpcHit_nSec  = 0;
   rpcHit_scat  = 0;
   rpcHit_tof   = 0;
   rpcHit_phi   = 0;
   rpcHit_theta = 0;

   for (Int_t s = 0; s < 6; s++) {
       rpcHit_n[s]=0;
   }
   rpcHit_tot = 0;

   //---- Shower histograms
   shoHit_chargeVsSector_m0 = 0;
   shoHit_chargeVsSector_m1 = 0;
   shoHit_chargeVsSector_m2 = 0;

   shoHit_rowVsSector_m0 = 0;
   shoHit_rowVsSector_m1 = 0;
   shoHit_rowVsSector_m2 = 0;

   shoHit_colVsSector_m0 = 0;
   shoHit_colVsSector_m1 = 0;
   shoHit_colVsSector_m2 = 0;

   shoHit_sectorVsModule = 0;
   shoHit_scat = 0;
   shoHit_nSec = 0;
   shoHit_phi = 0;
   shoHit_theta = 0;

   shoHitTof_scat = 0;
   shoHitTof_nSec = 0;
   shoHit_nCol = 0;
   shoHit_nRow = 0;

   for (Int_t s = 0; s < 6; s++)
   {
       shoHit_n[s]= 0;

       for (Int_t m = 0; m < 3; m++)
       {
	   shoHitSums[s][m] = 0;
           shoHit_nm[s][m] = 0;
       }
   }

   //---- ForwardWall histograms
   hWallCellSmall  = 0;
   hWallCellMedium = 0;
   hWallCellLarge  = 0;
   hMultWall       = 0;
   hWallCellTime   = 0;
   hWallCellAdc    = 0;

   hWallHitNumI    = 0;
   hWallHitNumII   = 0;
   hWallHitNumIII  = 0;
   hWallHitNumIV   = 0;
   hWallXY         = 0;


   //---- Tracking histograms
   splineTrack_scat           = 0;
   splineTrack_massCharge     = 0;
   rungeKuttaTrack_scat       = 0;
   rungeKuttaTrack_massCharge = 0;
   for (Int_t s = 0; s < 6; s++) {
      trackingSpline_sys0[ s] = 0;
      trackingRK_sys0[     s] = 0;
      trackingSpline_sys1[ s] = 0;
      trackingRK_sys1[     s] = 0;
   }

   // Rich-MDC histograms

   richmdc_lep = 0;
   for (Int_t s = 0; s < 6; s++) {
      richmdc_dtheta[s] = 0;
      richmdc_dphi[s] = 0;
   }


   // Matching histograms (received on 4-Feb-2005 from P.Tlusty )
   hsecspline           = 0;
   hsecspline0          = 0;
   hsecspline1          = 0;

   for (Int_t s = 0; s < 6; s++) {
       hXdiffTof[s]=0;
       hYdiffTof[s]=0;
   }

   for (Int_t s = 0; s < 6; s++) {
       hXdiffsho[s]=0;
       hYdiffsho[s]=0;
   }

   for (Int_t s = 0; s < 6; s++) {
       hXdiffRpc[s]=0;
       hYdiffRpc[s]=0;
   }


   
   hXdiffvsRpcsector=0;
   hYdiffvsRpcsector=0;
   hXdiffvsRpc=0;
   hYdiffvsRpc=0;
   hRpc_quality=0;



   hXdiffvsshowersector = 0;
   hYdiffvsshowersector = 0;
   hshower_quality      = 0;
   htof_quality         = 0;
   hXdiffvstofstrip     = 0;
   hYdiffvstofstrip     = 0;


   hXdiffvsshowersectorRK = NULL;
   hXdiffvstofstripRK = NULL;
   hYdiffvsshowersectorRK = NULL;
   hYdiffvstofstripRK = NULL;
   hshower_qualityRK = NULL;
   htof_qualityRK = NULL;


   hXdiffvstofstripRK_neg = NULL;
   hYdiffvstofstripRK_neg = NULL;
   hXdiffvsthetaRK_neg = NULL;
   hYdiffvsthetaRK_neg = NULL;

    hXdiffvsshoColRK = NULL;
   hXdiffvsshoRowRK = NULL;
   hYdiffvsshoColRK = NULL;
   hYdiffvsshoRowRK = NULL;

   hXdiffvsshoCol = NULL;
   hXdiffvsshoRow = NULL;
   hYdiffvsshoCol = NULL;
   hYdiffvsshoRow = NULL;

   hparticle_multrk                   = NULL;
   hparticle_lepton_tof_vs_rod_sys0   = NULL;
   hparticle_lepton_tof_vs_rod_sys1   = NULL;
   hparticle_lepton_tof_vs_startstrip_sys0= NULL;
   hparticle_lepton_tof_vs_startstrip_sys1= NULL;
   hparticle_lepton_tof_all_sys0= NULL;                 
   hparticle_lepton_tof_all_sys1= NULL;
   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       hparticle_lepton_tof_vs_start_sys0[i]= NULL;
       hparticle_lepton_tof_vs_start_sys1[i]= NULL;
   }
   
   hparticle_pi_tof_vs_rod_sys0   = NULL;
   hparticle_pi_tof_vs_rod_sys1   = NULL;

   hparticle_pi_metahit_vs_phi_sys1   = NULL;
   hparticle_pi_eloss_vs_rod_sys1   = NULL;
   hparticle_p_eloss_vs_rod_sys1   = NULL;


   hparticle_pi_tof_vs_startstrip_sys0= NULL;
   hparticle_pi_tof_vs_startstrip_sys1= NULL;
   hparticle_pi_tof_all_sys0= NULL;
   hparticle_pi_tof_all_sys1= NULL;
   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       hparticle_pi_tof_vs_start_sys0[i]= NULL;
       hparticle_pi_tof_vs_startMod0_sys0[i]= NULL;
       hparticle_pi_tof_vs_startMod1_sys0[i]= NULL;
       hparticle_pi_tof_vs_start_sys1[i]= NULL;
   }





   hparticle_RK_phi                   = NULL;
   hparticle_RK_neg_phi               = NULL;

   for (Int_t i = 0; i < 6; ++i) {
      hparticle_RK_theta_sec[i]       = NULL;
      hparticle_RK_neg_theta_sec[i]   = NULL;
      hparticle_rk_momdif_sys0_sec[i] = NULL;
      hparticle_rk_momdif_sys1_sec[i] = NULL;

      hparticle_MetaMatchQualTof[i] = NULL;
      hparticle_MetaMatchQualRpc[i] = NULL;
      hparticle_MetaMatchQualShw[i] = NULL;

      for(Int_t j=0; j<2; j++)
      {
	  hparticle_mult[i][j] = 0;
          hparticle_mult_select[i][j] = 0;
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcXdiff_pre[s] = 0;
      shorpcXdiff_post1[s] = 0;
      shorpcXdiff_post2[s] = 0;
      shorpcYdiff_pre[s] = 0;
      shorpcYdiff_post1[s] = 0;
      shorpcYdiff_post2[s] = 0;
      shorpcXvs_pre[s] = 0;
      shorpcXvs_post1[s] = 0;
      shorpcXvs_post2[s] = 0;
      shorpcYvs_pre[s] = 0;
      shorpcYvs_post1[s] = 0;
      shorpcYvs_post2[s] = 0;
   }

   hparticle_mass = 0;

   hparticle_TofdEdx=0;
}

void HQAHistograms::activateHist()
{
   // This function removes histograms from the histList TList which
   // are not listed in QAhists.h.

   TList *activeList = new TList();

   const Char_t *hists[1000] = {
#include "QAhists.h"
   };

   // Convert Char_t to TList of TObjStrings
   for (Int_t i = 0; hists[i] != 0; i++)
      activeList->Add(new TObjString(hists[i]));

   // Iterate over histList, remove entries which aren't in activeList
   TIter next(histList);
   while (TH1 *hist = (TH1*) next()) {
       if (! activeList->Contains(hist->GetName()))
       {
           //cout<<"  -- removing from the list: "<<hist->GetName()<<endl;
  	       histList->Remove(hist);
       }
   }
   activeList->Delete();
   delete activeList;
}

void HQAHistograms::bookHist()
{
   // Book histograms and define axis titles and options for drawing


   bookHistStart();
   bookHistRich();
   bookHistMdc();
   bookHistTof();
   bookHistRpc();
   bookHistShower();
   bookHistWall();
   bookHistMatching();
   bookHistTracking();
   bookHistRichMDC();
   bookHistShowerRpc();
   bookHistPid();
   bookHistDaqScalers();

   activateHist();
}

void HQAHistograms::bookHistStart()
{
   Char_t name[256];
   Char_t title[256];

   stLatchHist = new TH1F("stLatchHist", "Trigger Pattern (from 0, norm.)", 32, 0, 32);
   stLatchHist->SetXTitle("Trigger Input");
   histList->Add(stLatchHist, "start--gridx--gridy");


   stCal_stripMod0 = new TH1F("stCal_stripMod0", "StCalMod0: strip pattern", NSTART_STRIPS, 0, NSTART_STRIPS);
   stCal_stripMod0->SetXTitle("strip number");
   histList->Add(stCal_stripMod0, "start--gridx");

   stCal_stripMod1 = new TH1F("stCal_stripMod1", "StCalMod1: strip pattern", NSTART_STRIPS, 0, NSTART_STRIPS);
   stCal_stripMod1->SetXTitle("strip number");
   histList->Add(stCal_stripMod1, "start--gridx");

   vtCal_strip = new TH1F("vtCal_strip", "VtCal: strip pattern", NSTART_STRIPS, 0, NSTART_STRIPS);
   vtCal_strip->SetXTitle("strip number");
   histList->Add(vtCal_strip, "start--gridx");

   stCal_tof = new TH1F("stCal_tof", "startCal: time of flight", 300, -10.0, 10.0);
   stCal_tof->SetXTitle("time of flight");
   histList->Add(stCal_tof, "start--gridx");

   vtCal_tof = new TH1F("vtCal_tof", "vetoCal: time of flight", 300, -10.0, 10.0);
   vtCal_tof->SetXTitle("time of flight");
   vtCal_tof->SetLineColor(2);
   histList->Add(vtCal_tof, "start--gridx--same");

   for (Int_t im = 0; im < NSTART_STRIPS; im++) {
       sprintf(title, "StCal_Time for Strip %i", im);
       sprintf(name, "stCal_tof_strip[%i]", im);
       stCal_tof_strip[im] = new TH1F(name,title,300,-10.0,10.0);
       histList->Add(stCal_tof_strip[im], "start");

       sprintf(title, "VtCal_Time for Strip %i", im);
       sprintf(name, "vtCal_tof_strip[%i]", im);
       vtCal_tof_strip[im] = new TH1F(name,title,300,-10.0,10.0);
       histList->Add(vtCal_tof_strip[im], "start");
   }

   stHit_tof = new TH1F("stHit_tof", "startHit: time of flight", 300, -10.0, 10.0);
   stHit_tof->SetXTitle("time of flight");
   histList->Add(stHit_tof, "start--gridx");

   sprintf(name, "stHit_tof_vs_startstripe");
   sprintf(title, "Start-Tof: stHit_tofTOF_vs_startstripe");
   stHit_tof_vs_startstripe = new TH2F(name, title, NSTART_STRIPS, 0, NSTART_STRIPS, 300, 0, 30);
   stHit_tof_vs_startstripe->SetXTitle("start stripe");
   stHit_tof_vs_startstripe->SetYTitle("tof");
   histList->Add(stHit_tof_vs_startstripe, "start--gridx");

   sprintf(name, "stHit_vs_stStrip");
   sprintf(title, "StartHitTime vs StartHitStrip");
   stHit_vs_stStrip = new TH2F(name, title, NSTART_STRIPS, 0, NSTART_STRIPS, 400, -15, 15);
   stHit_vs_stStrip->SetXTitle("Start Strip");
   stHit_vs_stStrip->SetYTitle("StartHit Time");
   histList->Add(stHit_vs_stStrip, "start--gridx--gridy");

   stVertexXY = new TH2F("stVertexXY", "Vertex: X vs Y", 500, -10, 10, 500, -10, 10);
   stVertexXY->SetXTitle("Vertex X[mm]");
   stVertexXY->SetYTitle("Vertex Y[mm]");
   histList->Add(stVertexXY, "start--gridx--gridy");

   stVertexZ = new TH1F("stVertexZ", "Vertex: Z", 500, -100, 20);
   stVertexZ->SetXTitle("Vertex Z[mm]");
   histList->Add(stVertexZ, "start--gridx--gridy");

   for (Int_t im = 0; im < 9; im++) {
      sprintf(title, "StartTime vs StartStrip for Trigger:%i", im);
      sprintf(name, "stHit_vs_stStrip_Trigg[%i]", im);
      stHit_vs_stStrip_Trigg[im] = new TH2F(name, title, NSTART_STRIPS, 0, NSTART_STRIPS, 157, -5, 5);
      histList->Add(stHit_vs_stStrip_Trigg[im], "start--notshown");
   }

   for (Int_t im = 0; im < NSTART_STRIPS; im++) { // plot StTime - VtoTime
      sprintf(title, "Stm0_T - Vt_%i_T vs StartStrip", im);
      sprintf(name, "Stm0Vtdiff_vs_stStrip[%i]", im);
      Stm0Vtdiff_vs_stStrip[im] = new TH2F(name, title, NSTART_STRIPS, 0, NSTART_STRIPS, 1000, -5, 5);
      histList->Add(Stm0Vtdiff_vs_stStrip[im], "start--gridx--gridy");

      sprintf(title, "Stm1_T - Vt_%i_T vs StartStrip", im);
      sprintf(name, "Stm1Vtdiff_vs_stStrip[%i]", im);
      Stm1Vtdiff_vs_stStrip[im] = new TH2F(name, title, NSTART_STRIPS, 0, NSTART_STRIPS, 1000, -5, 5);
      histList->Add(Stm1Vtdiff_vs_stStrip[im], "start--gridx--gridy");
   }


}

void HQAHistograms::bookHistRich()
{
   Char_t name[256];
   Char_t title[256];

   richCal_row = new TH1F("richCal_row", "richCal: row", 90, 0, 90);
   richCal_row->SetXTitle("Row");
   histList->Add(richCal_row, "rich--");

   richCal_column = new TH1F("richCal_column", "richCal: column", 92, 0, 92);
   richCal_column->SetXTitle("Column");
   histList->Add(richCal_column, "rich--");

   richCal_nSec = new TH1F("richCal_nSec", "richCal: fired pads by sector", 6, -0.5, 5.5);
   histList->Add(richCal_nSec, "rich--");

   richHit_nSec = new TH1F("richHit_nSec", "richHit: hits by sector", 6, -0.5, 5.5);
   histList->Add(richHit_nSec, "rich--");

   richHit_scat = new TH2F("richHit_scat", "richHit: hit distribution in lab frame", 105, -1.05, 1.05, 105, -1.05, 1.05);
   richHit_scat->SetXTitle("Relative X_{lab}");
   richHit_scat->SetXTitle("Relative Y_{lab}");
   histList->Add(richHit_scat, "rich--nobox");

   richHit_phi = new TH1F("richHit_phi", "richHit: #Phi", 120, 0., 360.);
   richHit_phi->SetXTitle("#Phi (deg)");
   histList->Add(richHit_phi, "rich--");

   richHit_theta = new TH1F("richHit_theta", "richHit: #Theta", 80, 10., 90.);
   richHit_theta->SetXTitle("#Theta (deg)");
   histList->Add(richHit_theta, "rich--");

   richHit_radius = new TH1F("richHit_radius", "richHit: Ring radius", 80, 2, 6);
   richHit_radius->SetXTitle("Radius (pad)");
   histList->Add(richHit_radius, "rich--");

   richHit_centroid = new TH1F("richHit_centroid", "richHit: Ring centroid", 100, 0, 4);
   richHit_centroid->SetXTitle("Centroid (a.u.)");
   histList->Add(richHit_centroid, "rich--");

   richHit_chargeAmpl = new TH1F("richHit_chargeAmpl", "richHit: Ring amplitude", 100, 0, 10000);
   richHit_chargeAmpl->SetXTitle("Amplitude (ADC channel)");
   histList->Add(richHit_chargeAmpl, "rich--");

   richHit_ringCol = new TH1F("richHit_ringCol", "richHit: column", 92, 0, 92);
   richHit_ringCol->SetXTitle("column");
   histList->Add(richHit_ringCol, "rich--");

   richHit_ringRow = new TH1F("richHit_ringRow", "richHit: row", 90, 0, 90);
   richHit_ringRow->SetXTitle("row");
   histList->Add(richHit_ringRow, "rich--");

   richHit_ringLocMax4 = new TH1F("richHit_ringLocMax4", "richHit: Ring Loc Max4", 15, 0, 15);
   histList->Add(richHit_ringLocMax4, "rich--");

   richHit_houTraVsPatMat = new TH2F("richHit_houTraVsPatMat", "richHit: ring algorithm comparison", 100, 0, 200, 100, 0, 800);
   richHit_houTraVsPatMat->SetXTitle("Hough Trans.");
   richHit_houTraVsPatMat->SetYTitle("Pattern Matrix");
   histList->Add(richHit_houTraVsPatMat, "rich--");

   richHit_patMatVsTheta = new TH2F("richHit_patMatVsTheta", "richHit: #Theta vs. PM", 100, 200, 800, 90, 0, 90);
   richHit_patMatVsTheta->SetXTitle("Pattern Matrix");
   richHit_patMatVsTheta->SetYTitle("#Theta (deg)");
   histList->Add(richHit_patMatVsTheta, "rich--");

   richHit_houTraVsTheta = new TH2F("richHit_houTraVsTheta", "richHit: #Theta vs. HT", 100, 0, 200, 90, 0, 90);
   richHit_houTraVsTheta->SetXTitle("Hough Trans.");
   richHit_houTraVsTheta->SetYTitle("#Theta (deg)");
   histList->Add(richHit_houTraVsTheta, "rich--");

   richHit_chargeAmplVsTheta = new TH2F("richHit_chargeAmplVsTheta", "richHit: #theta vs. ring amplitude", 100, 0, 10000, 90, 0, 90);
   richHit_chargeAmplVsTheta->SetXTitle("Amplitude (ADC channel)");
   richHit_chargeAmplVsTheta->SetYTitle("#Theta (deg)");
   histList->Add(richHit_chargeAmplVsTheta, "rich--");

   richHit_radiusVsTheta = new TH2F("richHit_radiusVsTheta", "richHit: #Theta vs. radius", 40, 2, 6, 90, 0, 90);
   richHit_radiusVsTheta->SetXTitle("Radius");
   richHit_radiusVsTheta->SetYTitle("#Theta (deg)");
   histList->Add(richHit_radiusVsTheta, "rich--");

   for(Int_t i=0; i<6; i++)
   {
       sprintf(title, "richHit: RingAmpl per Num. of pads in ring, Sec[%i]", i);
       sprintf(name, "richHit_AverCharge[%i]", i);
       richHit_AverCharge[i] = new TH1F(name, title, 250, 0, 500);
       richHit_AverCharge[i]->SetXTitle("Amplitude (ADC channel)/Num of fired pads");
       richHit_AverCharge[i]->SetYTitle("counts");
       richHit_AverCharge[i]->SetLineColor(SectColor[i]);
       if(i==0)
       {
           histList->Add(richHit_AverCharge[i], "rich--6sec");
       }
       else
       {
	   histList->Add(richHit_AverCharge[i], "rich--same--6sec");
       }
   }

   for(Int_t i=0; i<6; i++)
   {
       sprintf(title, "richHit: Nr of pads per ring, Sec[%i]", i);
       sprintf(name, "richHit_NumPads[%i]", i);
       richHit_NumPads[i] = new TH1F(name, title, 50, -0.5, 49.5);
       richHit_NumPads[i]->SetXTitle("Num of pads per Ring");
       richHit_NumPads[i]->SetYTitle("counts");
       richHit_NumPads[i]->SetLineColor(SectColor[i]);
       if(i==0)
       {
           histList->Add(richHit_NumPads[i], "rich--6sec");
       }
       else
       {
	   histList->Add(richHit_NumPads[i], "rich--same--6sec");
       }
   }

   for(Int_t i=0; i<6; i++)
   {
       sprintf(title, "richHit: Nr of rings, Sec[%i]", i);
       sprintf(name, "richHit_NumRings[%i]", i);
       richHit_NumRings[i] = new TH1F(name, title, 20, -0.5, 19.5);
       richHit_NumRings[i]->SetXTitle("Num of Rings per event");
       richHit_NumRings[i]->SetYTitle("counts");
       richHit_NumRings[i]->SetLineColor(SectColor[i]);
       if(i==0)
       {
           histList->Add(richHit_NumRings[i], "rich--6sec");
       }
       else
       {
	   histList->Add(richHit_NumRings[i], "rich--same--6sec");
       }
   }
}

void HQAHistograms::bookHistMdc()
{

   mdcRaw_mboVsSector_m0 = new TH2F("mdcRaw_mboVsSector_m0", "mdcRaw: Plane I, sector vs. mbo mult.", 14, -0.5, 15.5, 6, -0.5, 5.5);
   mdcRaw_mboVsSector_m0->SetXTitle("mbo #");
   mdcRaw_mboVsSector_m0->SetYTitle("sector");
   histList->Add(mdcRaw_mboVsSector_m0, "mdc--logz");

   mdcRaw_mboVsSector_m1 = new TH2F("mdcRaw_mboVsSector_m1", "mdcRaw: Plane II, sector vs. mbo mult.", 16, -0.5, 15.5, 6, -0.5, 5.5);
   mdcRaw_mboVsSector_m1->SetXTitle("mbo #");
   mdcRaw_mboVsSector_m1->SetYTitle("sector");
   histList->Add(mdcRaw_mboVsSector_m1, "mdc--logz");

   mdcRaw_mboVsSector_m2 = new TH2F("mdcRaw_mboVsSector_m2", "mdcRaw: Plane III, sector vs. mbo mult.", 16, -0.5, 15.5, 6, -0.5, 5.5);
   mdcRaw_mboVsSector_m2->SetXTitle("mbo #");
   mdcRaw_mboVsSector_m2->SetYTitle("sector");
   histList->Add(mdcRaw_mboVsSector_m2, "mdc--logz");

   mdcRaw_mboVsSector_m3 = new TH2F("mdcRaw_mboVsSector_m3", "mdcRaw: Plane IV, sector vs. mbo mult.", 16, -0.5, 15.5, 6, -0.5, 5.5);
   mdcRaw_mboVsSector_m3->SetXTitle("mbo #");
   mdcRaw_mboVsSector_m3->SetYTitle("sector");
   histList->Add(mdcRaw_mboVsSector_m3, "mdc--logz");

   mdcCal1_nVsSector_m0 = new TH2F("mdcCal1_nVsSector_m0", "mdcCal1: Plane I, sector vs. multiplicity", 51, -0.5, 50.5, 6, -0.5, 5.5);
   mdcCal1_nVsSector_m0->SetXTitle("# fired wires");
   histList->Add(mdcCal1_nVsSector_m0, "mdc--logz,sectorhist");

   mdcCal1_nVsSector_m1 = new TH2F("mdcCal1_nVsSector_m1", "mdcCal1: Plane II, sector vs. multiplicity", 51, -0.5, 50.5, 6, -0.5, 5.5);
   mdcCal1_nVsSector_m1->SetXTitle("# fired wires");
   histList->Add(mdcCal1_nVsSector_m1, "mdc--logz,sectorhist");

   mdcCal1_nVsSector_m2 = new TH2F("mdcCal1_nVsSector_m2", "mdcCal1: Plane III, sector vs. multiplicity", 51, -0.5, 50.5, 6, -0.5, 5.5);
   mdcCal1_nVsSector_m2->SetXTitle("# fired wires");
   histList->Add(mdcCal1_nVsSector_m2, "mdc--logz,sectorhist");

   mdcCal1_nVsSector_m3 = new TH2F("mdcCal1_nVsSector_m3", "mdcCal1: Plane IV, sector vs. multiplicity", 51, -0.5, 50.5, 6, -0.5, 5.5);
   mdcCal1_nVsSector_m3->SetXTitle("# fired wires");
   histList->Add(mdcCal1_nVsSector_m3, "mdc--logz,sectorhist");

   mdcCal1_time1VsSector_m0 = new TH2F("mdcCal1_time1VsSector_m0", "mdcCal1: Plane I, sector vs. time1", 200, 0., 1000., 6, -0.5, 5.5);
   mdcCal1_time1VsSector_m0->SetXTitle("time1 (ns)");
   histList->Add(mdcCal1_time1VsSector_m0, "mdc--sectorhist");

   mdcCal1_time1VsSector_m1 = new TH2F("mdcCal1_time1VsSector_m1", "mdcCal1: Plane II, sector vs. time1", 200, 0., 1000., 6, -0.5, 5.5);
   mdcCal1_time1VsSector_m1->SetXTitle("time1 (ns)");
   histList->Add(mdcCal1_time1VsSector_m1, "mdc--sectorhist");

   mdcCal1_time1VsSector_m2 = new TH2F("mdcCal1_time1VsSector_m2", "mdcCal1: Plane III, sector vs. time1", 200, 0., 1000., 6, -0.5, 5.5);
   mdcCal1_time1VsSector_m2->SetXTitle("time1 (ns)");
   histList->Add(mdcCal1_time1VsSector_m2, "mdc--sectorhist");

   mdcCal1_time1VsSector_m3 = new TH2F("mdcCal1_time1VsSector_m3", "mdcCal1: Plane IV, sector vs. time1", 200, 0., 1000., 6, -0.5, 5.5);
   mdcCal1_time1VsSector_m3->SetXTitle("time1 (ns)");
   histList->Add(mdcCal1_time1VsSector_m3, "mdc--sectorhist");

   mdcCal1_tatVsSector_m0 = new TH2F("mdcCal1_tatVsSector_m0", "mdcCal1: Plane I, sector vs. t.a.t.", 150, 0., 300., 6, -0.5, 5.5);
   mdcCal1_tatVsSector_m0->SetXTitle("time above threshold (ns)");
   mdcCal1_tatVsSector_m0->SetYTitle("sector");
   histList->Add(mdcCal1_tatVsSector_m0, "mdc--");

   mdcCal1_tatVsSector_m1 = new TH2F("mdcCal1_tatVsSector_m1", "mdcCal1: Plane II, sector vs. t.a.t.", 150, 0., 300., 6, -0.5, 5.5);
   mdcCal1_tatVsSector_m1->SetXTitle("time above threshold (ns)");
   mdcCal1_tatVsSector_m1->SetYTitle("sector");
   histList->Add(mdcCal1_tatVsSector_m1, "mdc--");

   mdcCal1_tatVsSector_m2 = new TH2F("mdcCal1_tatVsSector_m2", "mdcCal1: Plane III, sector vs. t.a.t.", 150, 0., 300., 6, -0.5, 5.5);
   mdcCal1_tatVsSector_m2->SetXTitle("time above threshold (ns)");
   mdcCal1_tatVsSector_m2->SetYTitle("sector");
   histList->Add(mdcCal1_tatVsSector_m2, "mdc--");

   mdcCal1_tatVsSector_m3 = new TH2F("mdcCal1_tatVsSector_m3", "mdcCal1: Plane IV, sector vs. t.a.t.", 150, 0., 300., 6, -0.5, 5.5);
   mdcCal1_tatVsSector_m3->SetXTitle("time above threshold (ns)");
   mdcCal1_tatVsSector_m3->SetYTitle("sector");
   histList->Add(mdcCal1_tatVsSector_m3, "mdc--");


   //***************************************************
   // time above threshold vs. drift time = t2-t1 vs. t1
   // here: 24 2-dim histograms, one could also think of one histogram per plane (=chamber type) = 4 2-dim histos
   //
   Char_t title[100], name[100];
   Float_t t1tatx[2] = {   0., 500.};
   Float_t t1taty[2] = { -20., 1000.};
   Int_t   t1tatn[2] = { 250 , 370 };
   //
   //Sector-color description
   //                     blue,  red ,black, green,magn,yell
   //Int_t   SectColor[6]={   4 ,  2   ,  1  ,  8   , 6  ,  5 };
#include "QAsectioncolors.h"
   //Char_t *SectDescr[6]={ "s1", "s3" ,"s6" , "s2" ,"s4","s5"};
   //please be careful every time with "same" option for
   //overlayed histograms: by definition D.Magestro made
   //normal 1-dim plot to be blue-colored. So first color
   //of any overlayed histogramm should be blue, otherwise
   //it will be blue anyway whyle printing histograms (finalisation
   //stage), so better if we agree to keep "blue" color to be
   //always first in a color pallete of any overlayed plots
   for (Int_t im = 0; im < 4; im++) {
      sprintf(title, "(t2-t1) vs. t1 for plane%i", im);
      sprintf(name, "mdcCal1_t2mt1_vs_t1_plane[%i]", im);
      mdcCal1_t2mt1_vs_t1_plane[im] = new TH2F(name, title, t1tatn[0], t1tatx[0], t1tatx[1], t1tatn[1], t1taty[0], t1taty[1]);
      mdcCal1_t2mt1_vs_t1_plane[im]->SetXTitle("t2-t1 (ns)");
      mdcCal1_t2mt1_vs_t1_plane[im]->SetYTitle("t1 (ns)");
      histList->Add(mdcCal1_t2mt1_vs_t1_plane[im], "mdc--planehist");
   }

   for (Int_t im = 0; im < 4; im++) {
      for (Int_t is = 0; is < 6; is++) {
         //bi-plots--
         sprintf(title, "(t2-t1) vs. t1 for sector%i module%i", is, im);
         sprintf(name, "mdcCal1_t2mt1_vs_t1[%i][%i]", is, im);
         mdcCal1_t2mt1_vs_t1[is][im] = new TH2F(name, title, t1tatn[0], t1tatx[0], t1tatx[1], t1tatn[1], t1taty[0], t1taty[1]);
         mdcCal1_t2mt1_vs_t1[is][im]->SetXTitle("t2-t1 (ns)");
         mdcCal1_t2mt1_vs_t1[is][im]->SetYTitle("t1 (ns)");
         if (is == 0) {
            histList->Add(mdcCal1_t2mt1_vs_t1[is][im], "mdc--newpage");
         } else {
            histList->Add(mdcCal1_t2mt1_vs_t1[is][im], "mdc--");
         }
      }
   }

   for (Int_t im = 0; im < 4; im++) {
      for (Int_t is = 0; is < 6; is++) {
         //dN/d[t1]
         //sprintf(title,"t1 for sector%i module%i",is,im);
         sprintf(title, "Drift time for mod%i s%i", im, is);
         sprintf(name, "mdcCal1_t1[%i][%i]", is, im);
         mdcCal1_t1[is][im] = new TH1F(name, title, t1tatn[1], t1taty[0], t1taty[1]);
         mdcCal1_t1[is][im]->SetXTitle("t1 (ns)");
         mdcCal1_t1[is][im]->SetYTitle("dN/d[t1]");
         mdcCal1_t1[is][im]->SetLineColor(SectColor[is]);
         mdcCal1_t1[is][im]->SetLineWidth(8);
         //if(is==0 && im==0){histList->Add(mdcCal1_t1[is][im],"mdc--newpage");}
         //else{
         if (is == 0) {
            histList->Add(mdcCal1_t1[is][im], "mdc--6sec");
         } else {
            histList->Add(mdcCal1_t1[is][im], "mdc--same--6sec");
         }
         //}
      }
   }

   for (Int_t im = 0; im < 4; im++) {
      for (Int_t is = 0; is < 6; is++) {
         //single-plots--
         //dN/d[t2-t1]
         //sprintf(title,"(t2-t1) for sector%i module%i",is,im);
         sprintf(title, "Time above threshold for mod%i s%i", im, is);
         sprintf(name, "mdcCal1_t2mt1[%i][%i]", is, im);
         mdcCal1_t2mt1[is][im] = new TH1F(name, title, t1tatn[0], t1tatx[0], t1tatx[1]);
         mdcCal1_t2mt1[is][im]->SetXTitle("t2-t1 (ns)");
         mdcCal1_t2mt1[is][im]->SetYTitle("dN/d[t2-t1] (ns)");
         mdcCal1_t2mt1[is][im]->SetLineColor(SectColor[is]);
         mdcCal1_t2mt1[is][im]->SetLineWidth(8);
         if (is == 0 && im == 0) {
            histList->Add(mdcCal1_t2mt1[is][im], "mdc--newpage");
         } else {
            if (is == 0) {
               histList->Add(mdcCal1_t2mt1[is][im], "mdc--6sec");
            } else {
               histList->Add(mdcCal1_t2mt1[is][im], "mdc--same--6sec");
            }
	 }
      }
   }

   for (Int_t im = 0; im < 4; im++) {
       for (Int_t il = 0; il < 6; il++) {
	   for (Int_t is=0; is < 6; is++)
	   {
	       sprintf(title, "Time above threshold for mod%i s%i lay%i", im, is, il);
	       sprintf(name, "mdcCal1_t2mt1_V2[%i][%i][%i]", is, im,il);
	       mdcCal1_t2mt1_V2[is][im][il]= new TH1F(name, title, t1tatn[0], t1tatx[0], t1tatx[1]);
               mdcCal1_t2mt1_V2[is][im][il]->SetLineColor(SectColor[is]);

	       if(is==0){
		   histList->Add(mdcCal1_t2mt1_V2[is][im][il], "mdc--6sec");
	       } else {
		   histList->Add(mdcCal1_t2mt1_V2[is][im][il], "mdc--same--6sec");
	       }
	   }
       }
   }

   for (Int_t im = 0; im < 4; im++) {
       for (Int_t il = 0; il < 6; il++) {
	   for (Int_t is=0; is < 6; is++)
	   {
	       sprintf(title, "Drift time for mod%i s%i lay%i", im, is, il);
	       sprintf(name, "mdcCal1_t1_V2[%i][%i][%i]", is, im,il);
	       mdcCal1_t1_V2[is][im][il]= new TH1F(name, title, t1tatn[0], t1taty[0], t1taty[1]);
               mdcCal1_t1_V2[is][im][il]->SetLineColor(SectColor[is]);

	       if(is==0){
	       histList->Add(mdcCal1_t1_V2[is][im][il], "mdc--6sec");
	       } else {
		   histList->Add(mdcCal1_t1_V2[is][im][il], "mdc--same--6sec");
	       }
	   }
       }
   }

   for (Int_t ig = 0; ig < 2; ig++) {
       for (Int_t is = 0; is < 6; is++) {

	   sprintf(title, "#chi� seg%i sec%i", ig, is);
	   sprintf(name, "mdcSeg_Chi2[%i][%i]", is, ig);
	   mdcSeg_Chi2[is][ig]= new TH1F(name, title, 220,-10,100);
	   mdcSeg_Chi2[is][ig]->SetLineColor(SectColor[is]);

	   if(is==0){
	       histList->Add(mdcSeg_Chi2[is][ig], "mdc--6sec");
	   } else {
	       histList->Add(mdcSeg_Chi2[is][ig], "mdc--same--6sec");
	   }
       }
   }


   //***************************************************


   mdcHit_scat_m0 = new TH2F("mdcHit_scat_m0", "mdcHit: Plane I, hit distribution", 100, -900, 900, 100, -900, 900);
   mdcHit_scat_m0->SetXTitle("view from target");
   histList->Add(mdcHit_scat_m0, "mdc--newpage");

   mdcHit_scat_m1 = new TH2F("mdcHit_scat_m1", "mdcHit: Plane II, hit distribution", 100, -1000, 1000, 100, -1000, 1000);
   mdcHit_scat_m1->SetXTitle("view from target");
   histList->Add(mdcHit_scat_m1, "mdc--");

   mdcHit_scat_m2 = new TH2F("mdcHit_scat_m2", "mdcHit: Plane III, hit distribution", 100, -2000, 2000, 100, -2000, 2000);
   mdcHit_scat_m2->SetXTitle("view from target");
   histList->Add(mdcHit_scat_m2, "mdc--");

   mdcHit_scat_m3 = new TH2F("mdcHit_scat_m3", "mdcHit: Plane IV, hit distribution", 100, -2500, 2500, 100, -2500, 2500);
   mdcHit_scat_m3->SetXTitle("view from target");
   histList->Add(mdcHit_scat_m3, "mdc--");

   //***************************************************
   //                 blue ,magn,yell, red , gray , green  , lightgray
   Int_t   color[8] = {  4   , 6 ,  5 ,   2  ,  12  ,   8 , kOrange+2   ,  32    };
   //Char_t *descr[7]={"All","t1","t2","t1t2","wire","t1+t12","bump","cutall"};
   for (Int_t i = 0; i < 8; i++) {
      sprintf(title, "Events and cut counts %i", i);
      sprintf(name, "mdcCutCounts[%i]", i);
      mdcCutCounts[i] = new TH1F(name, title, 24, 0., 24.);
      mdcCutCounts[i]->SetXTitle("p1{s1,s2,...,s6}, p2{s1,...,s6},...,p4{s1..}");
      mdcCutCounts[i]->SetYTitle("Counts");
      mdcCutCounts[i]->SetLineColor(color[i]);
      mdcCutCounts[i]->SetLineWidth(8);
      if (i == 0) {
         mdcCutCounts[i]->SetFillColor(color[i]);
      }
      if (i == 0) {
         histList->Add(mdcCutCounts[i], "mdc--");
      } else {
         histList->Add(mdcCutCounts[i], "mdc--same");
      }

   }
   //***************************************************


}

void HQAHistograms::bookHistTof()
{
   Char_t name[256];
   Char_t title[256];

   tofHit_scat = new TH2F("tofHit_scat", "tofHit: hit distribution in lab frame", 100, -2750, 2750, 100, -2750, 2750);
   tofHit_scat->SetXTitle("X_{lab} (mm)");
   tofHit_scat->SetYTitle("Y_{lab} (mm)");
   histList->Add(tofHit_scat, "tof--nobox");

   tofHit_tof = new TH1F("tofHit_tof", "tofHit: time of flight", 200, 0., 50.);
   tofHit_tof->SetXTitle("time of flight (ns)");
   histList->Add(tofHit_tof, "tof--");

   tofHit_theta = new TH1F("tofHit_theta", "tofHit: theta angle", 50, 40., 90.);
   tofHit_theta->SetXTitle("#theta (degrees)");
   histList->Add(tofHit_theta, "tof--");

   tofHit_phi = new TH1F("tofHit_phi", "tofHit: phi angle", 120, 0., 360.);
   tofHit_phi->SetXTitle("#phi (degrees)");
   histList->Add(tofHit_phi, "tof--");

   /*??tofHit_n = new TH1F("tofHit_n", "tofHit: hit multiplicity", 51, -0.5, 50.5);
   tofHit_n->SetXTitle("num hits");
   histList->Add(tofHit_n, "tof--");
   */
   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "tofHit: hit multiplicity");
       sprintf(name, "tofHit_n[%i]", is);
       tofHit_n[is] = new TH1F(name, title, 51, -0.5, 50.5);
       tofHit_n[is]->SetXTitle("num hits");
       tofHit_n[is]->SetYTitle("counts");
       tofHit_n[is]->SetLineColor(SectColor[is]);
       tofHit_n[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(tofHit_n[is], "tof--6sec");
       } else {
	   histList->Add(tofHit_n[is], "tof--same--6sec");
       }
   }

   tofHit_tot = new TH1F("tofHit_tot","tofHit: total hit multiplicity",101, -0.5, 100.5);
   tofHit_tot ->SetXTitle("num hits");
   tofHit_tot ->SetYTitle("counts");
   histList->Add(tofHit_tot, "tof--");

   tofHit_nSec = new TH1F("tofHit_nSec", "tofHit: num hits/event/sector", 6, -0.5, 5.5);
   tofHit_nSec->SetXTitle("sector");
   tofHit_nSec->SetYTitle("counts");
   histList->Add(tofHit_nSec, "tof--");

}


void HQAHistograms::bookHistRpc()
{
   Char_t name[256];
   Char_t title[256];

   rpcHit_scat = new TH2F("rpcHit_scat", "rpcHit: hit distribution in lab frame", 100, -2750, 2750, 100, -2750, 2750);
   rpcHit_scat->SetXTitle("X_{lab} (mm)");
   rpcHit_scat->SetYTitle("Y_{lab} (mm)");
   histList->Add(rpcHit_scat, "rpc--nobox");

   rpcHit_tof = new TH1F("rpcHit_tof", "rpcHit: time of flight", 200, -20., 60.);
   rpcHit_tof->SetXTitle("time of flight (ns)");
   histList->Add(rpcHit_tof, "rpc--");

   rpcHit_theta = new TH1F("rpcHit_theta", "rpcHit: theta angle", 40, 10., 50.);
   rpcHit_theta->SetXTitle("#theta (degrees)");
   histList->Add(rpcHit_theta, "rpc--");

   rpcHit_phi = new TH1F("rpcHit_phi", "rpcHit: phi angle", 120, 0., 360.);
   rpcHit_phi->SetXTitle("#phi (degrees)");
   histList->Add(rpcHit_phi, "rpc--");

  /*?? rpcHit_n = new TH1F("rpcHit_n", "rpcHit: hit multiplicity", 71, -0.5, 70.5);
   rpcHit_n->SetXTitle("num hits");
   histList->Add(rpcHit_n, "rpc--");*/

   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "rpcHit: hit multiplicity");
       sprintf(name, "rpcHit_n[%i]", is);
       rpcHit_n[is] = new TH1F(name, title, 71, -0.5, 70.5);
       rpcHit_n[is]->SetXTitle("num hits");
       rpcHit_n[is]->SetYTitle("counts");
       rpcHit_n[is]->SetLineColor(SectColor[is]);
       rpcHit_n[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(rpcHit_n[is], "rpc--6sec");
       } else {
	   histList->Add(rpcHit_n[is], "rpc--same--6sec");
       }
   }

   rpcHit_tot = new TH1F("rpcHit_tot","rpcHit: total hit multiplicity",101, -1, 201);
   rpcHit_tot ->SetXTitle("num hits");
   rpcHit_tot ->SetYTitle("counts");
   histList->Add(rpcHit_tot, "rpc--");

   rpcHit_nSec = new TH1F("rpcHit_nSec", "rpcHit: num hits/event/sector", 6, -0.5, 5.5);
   rpcHit_nSec->SetXTitle("sector");
   rpcHit_nSec->SetYTitle("counts");
   histList->Add(rpcHit_nSec, "rpc--");

}


void HQAHistograms::bookHistShower()
{
   Char_t name[256];
   Char_t title[256];

   shoHit_scat = new TH2F("shoHit_scat", "showerHit (LAB)", 100, -2100, 2100, 100, -2100, 2100);
   shoHit_scat->SetXTitle("X_{lab} (mm)");
   shoHit_scat->SetYTitle("Y_{lab} (mm)");
   histList->Add(shoHit_scat, "shower--nobox");

   shoHit_sectorVsModule = new TH2F("shoHit_sectorVsModule" , "showerHit: Sectors vs. Modules", 11, -0.25, 5.25, 5, -0.25, 2.25);
   shoHit_sectorVsModule->SetXTitle("sector");
   shoHit_sectorVsModule->SetYTitle("module");
   shoHit_sectorVsModule->GetXaxis()->SetTitleOffset(4);
   shoHit_sectorVsModule->GetYaxis()->SetTitleOffset(4);
   histList->Add(shoHit_sectorVsModule, "shower--lego2");

   shoHit_theta = new TH1F("shoHit_theta", "showerHit: theta angle", 50, 5., 55.);
   shoHit_theta->SetXTitle("#theta (degrees)");
   histList->Add(shoHit_theta, "shower--");

   shoHit_phi = new TH1F("shoHit_phi", "showerHit: phi angle", 120, 0., 360.);
   shoHit_phi->SetXTitle("#phi (degrees)");
   histList->Add(shoHit_phi, "shower--");

   //?? where are they
   shoHit_chargeVsSector_m0 = new TH2F("shoHit_chargeVsSector_m0" , "showerHit: Mod 0, charge on loc. max.", 100, 0., 100., 6, -0.5, 5.5);
   shoHit_chargeVsSector_m0->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_chargeVsSector_m0, "shower--logz,sectorhist");

   shoHit_chargeVsSector_m1 = new TH2F("shoHit_chargeVsSector_m1" , "showerHit: Mod 1, charge on loc. max.", 100, 0., 100., 6, -0.5, 5.5);
   shoHit_chargeVsSector_m1->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_chargeVsSector_m1, "shower--logz,sectorhist");

   shoHit_chargeVsSector_m2 = new TH2F("shoHit_chargeVsSector_m2" , "showerHit: Mod 2, charge on loc. max.", 100, 0., 100., 6, -0.5, 5.5);
   shoHit_chargeVsSector_m2->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_chargeVsSector_m2, "shower--logz,sectorhist");

   shoHit_rowVsSector_m0 = new TH2F("shoHit_rowVsSector_m0" , "showerHit: Mod 0, sector vs. row", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_rowVsSector_m0->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_rowVsSector_m0, "shower--logz,sectorhist");

   shoHit_rowVsSector_m1 = new TH2F("shoHit_rowVsSector_m1" , "showerHit: Mod 1, sector vs. row", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_rowVsSector_m1->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_rowVsSector_m1, "shower--logz,sectorhist");

   shoHit_rowVsSector_m2 = new TH2F("shoHit_rowVsSector_m2" , "showerHit: Mod 2, sector vs. row", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_rowVsSector_m2->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_rowVsSector_m2, "shower--logz,sectorhist");

   shoHit_colVsSector_m0 = new TH2F("shoHit_colVsSector_m0" , "showerHit: Mod 0, sector vs. col", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_colVsSector_m0->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_colVsSector_m0, "shower--logz,sectorhist");

   shoHit_colVsSector_m1 = new TH2F("shoHit_colVsSector_m1" , "showerHit: Mod 1, sector vs. col", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_colVsSector_m1->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_colVsSector_m1, "shower--logz,sectorhist");

   shoHit_colVsSector_m2 = new TH2F("shoHit_colVsSector_m2" , "showerHit: Mod 2, sector vs. col", 32, 0., 32., 6, -0.5, 5.5);
   shoHit_colVsSector_m2->SetXTitle("charge on loc. max.");
   histList->Add(shoHit_colVsSector_m2, "shower--logz,sectorhist");

   shoHit_nRow = new TH1F("shoHit_nRow", "ShowerHit: Rows (All sectors)", 32, 0, 31);
   shoHit_nRow->SetXTitle("Row");
   histList->Add(shoHit_nRow, "shower--");

   shoHit_nCol = new TH1F("shoHit_nCol", "ShowerHit: Cols (All sectors)", 32, 0, 31);
   shoHit_nCol->SetXTitle("Col");
   histList->Add(shoHit_nCol, "shower--");

   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "ShowerHit: hit multiplicity");
       sprintf(name, "ShowerHit_n[%i]", is);
       shoHit_n[is] = new TH1F(name, title, 51, -0.5, 50.5);
       shoHit_n[is]->SetXTitle("num hits");
       shoHit_n[is]->SetYTitle("counts");
       shoHit_n[is]->SetLineColor(SectColor[is]);
       shoHit_n[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(shoHit_n[is], "shower--6sec");
       } else {
	   histList->Add(shoHit_n[is], "shower--same--6sec");
       }
   }

   Int_t   color[3] = {  4   , 30 ,  6 };
   Char_t buf[80];
   Char_t buf1[80];
   for (Int_t s = 0; s < 6; s++) {
      for (Int_t m = 0; m < 3; m++) {
         sprintf(buf, "showersums_%i_%i", s, m);
         sprintf(buf1, "Shower Sums (Normalized): Sector %i", s);
         shoHitSums[s][m] = new TH1F(buf, buf1, 200, 0, 400);
         shoHitSums[s][m]->SetXTitle("Charge");
         shoHitSums[s][m]->SetLineColor(color[m]);
         shoHitSums[s][m]->SetLineWidth(8);
         if (s == 0 && m == 0) {
            histList->Add(shoHitSums[s][m], "shower--newpage--3mod");
         } else {
            if (m == 0) histList->Add(shoHitSums[s][m], "shower--3mod");
            else histList->Add(shoHitSums[s][m], "shower--same--3mod");
         }
      }
   }

   for (Int_t s = 0; s < 6; s++) {
      for (Int_t m = 0; m < 3; m++) {
         sprintf(buf, "shoHit_nm_%i_%i", s, m);
         sprintf(buf1, "Shower Multiplicity: Sector %i", s);
         shoHit_nm[s][m] = new TH1F(buf, buf1, 51, -0.5, 50.5);
         shoHit_nm[s][m]->SetXTitle("num hits");
         shoHit_nm[s][m]->SetLineColor(color[m]);
         shoHit_nm[s][m]->SetLineWidth(8);
         if (s == 0 && m == 0) {
            histList->Add(shoHit_nm[s][m], "shower--newpage--3mod");
         } else {
            if (m == 0) histList->Add(shoHit_nm[s][m], "shower--3mod");
            else histList->Add(shoHit_nm[s][m], "shower--same--3mod");
         }
      }
   }




   shoHit_nSec = new TH1F("shoHit_nSec", "showerHit: num hits/event/sector", 6, -0.5, 5.5);
   shoHit_nSec->SetXTitle("sector");
   histList->Add(shoHit_nSec, "shower--logy");

   shoHitTof_scat = new TH2F("shoHitTof_scat", "showerHitTof (LAB) ", 100, -2100, 2100, 100, -2100, 2100);
   shoHitTof_scat->SetXTitle("X_{lab} (mm)");
   shoHitTof_scat->SetYTitle("Y_{lab} (mm)");
   histList->Add(shoHitTof_scat, "shower--nobox");

   shoHitTof_nSec = new TH1F("shoHitTof_nSec", "showerHitTof: num hits/event/sector", 6, -0.5, 5.5);
   shoHitTof_nSec->SetXTitle("sector");
   histList->Add(shoHitTof_nSec, "shower--logy");
}

void HQAHistograms::bookHistWall()
{

   hWallCellSmall  = new TH1F("hWallCellSmall" , "Wall cell Small"     , 144,    0. , 144.);
   hWallCellSmall->SetXTitle("cell Numb");
   histList->Add(hWallCellSmall, "wall--");

   hWallCellMedium = new TH1F("hWallCellMedium", "Wall cell Medium"    , 64,   144., 208.);
   hWallCellMedium->SetXTitle("cell Numb");
   histList->Add(hWallCellMedium, "wall--");

   hWallCellLarge  = new TH1F("hWallCellLarge" , "Wall cell Large"     , 92,   210., 302.);
   hWallCellLarge->SetXTitle("cell Numb");
   histList->Add(hWallCellLarge, "wall--");

   hMultWall       = new TH1F("hMultWall"      , "Wall mult"           , 120,    0. ,  120.);
   hMultWall->SetXTitle("multiplicity");
   histList->Add(hMultWall, "wall--");

   //hWallCellTime   = new TH2F("hWallCellTime"  , "Wall Time vs Cell"   ,302,    0. , 302,    30,   0., 60.);
   hWallCellTime   = new TH2F("hWallCellTime"  , "Wall Time vs Cell"   , 151,    0. , 302,   120,   0., 60.);
   hWallCellTime->SetXTitle("cell");
   hWallCellTime->SetYTitle("time [ns]");
   histList->Add(hWallCellTime, "wall--");

   //hWallCellAdc    = new TH2F("hWallCellAdc"   , "Wall Charge vs Cell" ,302,    0. , 302,   120,   0., 60.);
   hWallCellAdc    = new TH2F("hWallCellAdc"   , "Wall Charge vs Cell" , 151,    0. , 302,    30,   0., 150.);
   hWallCellAdc->SetXTitle("cell");
   hWallCellAdc->SetYTitle("ADC");
   histList->Add(hWallCellAdc, "wall--");

   hWallHitNumI    = new TH1F("hWallHitNumI"   , "Number of hits I   quarter", 40, 0, 40); //  1-6 ; 61-66
   hWallHitNumI->SetXTitle("cell");
   histList->Add(hWallHitNumI, "wall--");

   hWallHitNumII   = new TH1F("hWallHitNumII"  , "Number of hits II  quarter", 40, 0, 40); //  7-12; 67-72
   hWallHitNumII->SetXTitle("cell");
   histList->Add(hWallHitNumII, "wall--");

   hWallHitNumIII  = new TH1F("hWallHitNumIII" , "Number of hits III quarter", 40, 0, 40); // 73-78;133-138
   hWallHitNumIII->SetXTitle("cell");
   histList->Add(hWallHitNumIII, "wall--");

   hWallHitNumIV   = new TH1F("hWallHitNumIV"  , "Number of hits IV  quarter", 40, 0, 40); // 79-84;139-144
   hWallHitNumIV->SetXTitle("cell");
   histList->Add(hWallHitNumIV, "wall--");

   hWallXY         = new TH2F("hWallXY"        , "Wall X vs Wall Y"    , 120, -120., 120., 120, -120., 120.);
   hWallXY->SetXTitle("x");
   hWallXY->SetYTitle("y");
   histList->Add(hWallXY, "wall--");

   ////histProfTofino->TH1::SetStats(kFALSE);

}

void HQAHistograms::bookHistRichMDC()
{
   Char_t name[256], title[256];
   Int_t   SectColor[6] = {   4 ,  2   ,  1  ,  8   , 6  ,  5 };

   for (Int_t s = 0; s < 6; s++) {
      sprintf(name, "richmdc_dtheta_sec%d", s);
      sprintf(title, "Rich-Mdc: #Delta #Theta sec%d", s);
      richmdc_dtheta[s] = new TH1F(name, title, 80, -40, 40);

      richmdc_dtheta[s]->SetXTitle("#Delta #Theta [deg]");
      richmdc_dtheta[s]->SetYTitle("counts/event");
      richmdc_dtheta[s]->SetLineColor(SectColor[s]);
      richmdc_dtheta[s]->SetLineWidth(4);

      if (s == 0) histList->Add(richmdc_dtheta[s], "richmdc--6sec");
      else histList->Add(richmdc_dtheta[s], "richmdc--same--6sec");

   }

   for (Int_t s = 0; s < 6; s++) {
      sprintf(name, "richmdc_dphi_sec%d", s);
      sprintf(title, "Ric-Mdc: #Delta #Phi sec%d", s);
      richmdc_dphi[s] = new TH1F(name, title, 80, -40, 40);
      richmdc_dphi[s]->SetXTitle("#Delta #Phi*sin#Theta [deg]");
      richmdc_dphi[s]->SetYTitle("counts/event");
      richmdc_dphi[s]->SetLineColor(SectColor[s]);
      richmdc_dphi[s]->SetLineWidth(4);

      if (s == 0) histList->Add(richmdc_dphi[s], "richmdc--6sec");
      else  histList->Add(richmdc_dphi[s], "richmdc--same--6sec");
   }

   //richmdc_lep = new TH1F("richmdc_lep", "#splitline{Rich-Mdc: leptons}{|#Delta #Theta|<5 deg, |#Delta #Phi|<5 deg}", 6, 0, 6);
   richmdc_lep = new TH1F("richmdc_lep", "Rich-Mdc: leptons |#Delta #Theta|<5 deg, |#Delta #Phi|<5 deg", 6, 0, 6);
   richmdc_lep->SetXTitle("sector");
   richmdc_lep->SetYTitle("leptons/event");
   richmdc_lep->SetLineWidth(4);
   histList->Add(richmdc_lep, "richmdc--");
}



void HQAHistograms::bookHistMatching()
{

#include "QAsectioncolors.h"
   Char_t title[100], name[100];

   //같같같같같같같같같같같같같
   //같컎pline-sector-hists같같
   //같같같같같같같같같같같같같

   hsecspline  = new TH1F("hsecspline"          , "spline sec"               , 6, 0, 6);
   hsecspline0 = new TH1F("hsecspline0"         , "spline sec, sys 0"        , 6, 0, 6);
   hsecspline1 = new TH1F("hsecspline1"         , "spline sec, sys 1"        , 6, 0, 6);

   hsecspline ->SetXTitle("sector");
   hsecspline ->SetYTitle("N");
   histList->Add(hsecspline , "metamatching--gridx");
   hsecspline0->SetXTitle("sector");
   hsecspline0->SetYTitle("N");
   histList->Add(hsecspline0, "metamatching--gridx");
   hsecspline1->SetXTitle("sector");
   hsecspline1->SetYTitle("N");
   histList->Add(hsecspline1, "metamatching--gridx");


   //같같같같같같같같같같같같같
   //같캴of-Meta-hists같같같같�
   //같같같같같같같같같같같같같

   hXdiffvstofstrip     = new TH2F("hXdiffvstofstrip"    , "Xdiff vs tofstrip"        , 384, 0, 384, 200, -200, 200);
   hYdiffvstofstrip     = new TH2F("hYdiffvstofstrip"    , "Ydiff vs tofstrip"        , 384, 0, 384, 200, -200, 200);
   htof_quality         = new TH2F("htof_quality"        , "quality vs sector, TOF" , 6, 0, 6, 110, -1, 10);


   hXdiffvstofstrip    ->SetXTitle("tofstrip");
   hXdiffvstofstrip    ->SetYTitle("Xdiff");
   histList->Add(hXdiffvstofstrip    , "metamatching--gridx--gridy--newpage");
   hYdiffvstofstrip    ->SetXTitle("tofstrip");
   hYdiffvstofstrip    ->SetYTitle("Ydiff");
   histList->Add(hYdiffvstofstrip    , "metamatching--gridx--gridy");
   htof_quality        ->SetXTitle("sector");
   htof_quality        ->SetYTitle("Quality");
   histList->Add(htof_quality        , "metamatching--gridx--gridy");

   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta X MetaMatch, Tof");
       sprintf(name, "hXdiffTof[%i]", is);
       hXdiffTof[is] = new TH1F(name, title, 200, -150, 150);
       hXdiffTof[is]->SetXTitle("#Delta X (TOFhit - MetaMatch)");
       hXdiffTof[is]->SetYTitle("counts");
       hXdiffTof[is]->SetLineColor(SectColor[is]);
       hXdiffTof[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hXdiffTof[is], "metamatching--6sec");
       } else {
	   histList->Add(hXdiffTof[is], "metamatching--same--6sec");
       }
   }
   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta Y MetaMatch, Tof");
       sprintf(name, "hYdiffTof[%i]", is);
       hYdiffTof[is] = new TH1F(name, title, 200, -100, 100);
       hYdiffTof[is]->SetXTitle("#Delta Y (TOFhit - MetaMatch)");
       hYdiffTof[is]->SetYTitle("counts");
       hYdiffTof[is]->SetLineColor(SectColor[is]);
       hYdiffTof[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hYdiffTof[is], "metamatching--6sec");
       } else {
	   histList->Add(hYdiffTof[is], "metamatching--same--6sec");
       }
   }

   //####### RK Tof ###########

   hXdiffvstofstripRK     = new TH2F("hXdiffvstofstripRK"    , "Xdiff vs tofstrip, RK"        , 384, 0, 384, 200, -200, 200);
   hYdiffvstofstripRK     = new TH2F("hYdiffvstofstripRK"    , "Ydiff vs tofstrip,RK"        , 384, 0, 384, 200, -200, 200);
   htof_qualityRK         = new TH2F("htof_qualityRK"        , "quality vs sector, TOF, RK" , 6, 0, 6, 110, -1, 10);

   hXdiffvstofstripRK     -> SetXTitle("tofstrip");
   hXdiffvstofstripRK     -> SetYTitle("Xdiff");
   histList               -> Add(hXdiffvstofstripRK, "metamatching--gridx--gridy");
   hYdiffvstofstripRK     -> SetXTitle("tofstrip");
   hYdiffvstofstripRK     -> SetYTitle("Ydiff");
   histList               -> Add(hYdiffvstofstripRK, "metamatching--gridx--gridy");
   htof_qualityRK         -> SetXTitle("sector");
   htof_qualityRK         -> SetYTitle("Quality");
   histList               -> Add(htof_qualityRK, "metamatching--gridx--gridy");

   hXdiffvstofstripRK_neg     = new TH2F("hXdiffvstofstripRK_neg"    , "Xdiff vs tofstrip, RK neg"        , 384, 0, 384, 200, -200, 200);
   hYdiffvstofstripRK_neg     = new TH2F("hYdiffvstofstripRK_neg"    , "Ydiff vs tofstrip,RK neg"        , 384, 0, 384, 200, -200, 200);
   hXdiffvsthetaRK_neg     = new TH2F("hXdiffvsthetaRK_neg"    , "Xdiff vs theta, RK neg"        , 600, 0, 600, 200, -200, 200);
   hYdiffvsthetaRK_neg     = new TH2F("hYdiffvsthetaRK_neg"    , "Ydiff vs theta,RK neg"        , 600, 0, 600, 200, -200, 200);

   hXdiffvstofstripRK_neg     -> SetXTitle("tofstrip");
   hXdiffvstofstripRK_neg     -> SetYTitle("Xdiff");
   histList               -> Add(hXdiffvstofstripRK_neg, "metamatching--gridx--gridy");
   hYdiffvstofstripRK_neg     -> SetXTitle("tofstrip");
   hYdiffvstofstripRK_neg     -> SetYTitle("Ydiff");
   histList               -> Add(hYdiffvstofstripRK_neg, "metamatching--gridx--gridy");
   hXdiffvsthetaRK_neg     -> SetXTitle("theta");
   hXdiffvsthetaRK_neg     -> SetYTitle("Xdiff");
   histList               -> Add(hXdiffvsthetaRK_neg, "metamatching--gridx--gridy");
   hYdiffvsthetaRK_neg     -> SetXTitle("theta");
   hYdiffvsthetaRK_neg     -> SetYTitle("Ydiff");
   histList               -> Add(hYdiffvsthetaRK_neg, "metamatching--gridx--gridy");


   //같같같같같같같같같같같같같
   //같캳hower-Meta-hists같같같
   //같같같같같같같같같같같같같

   hXdiffvsshowersector = new TH2F("hXdiffvsshowersector", "Xdiff vs sector, shower"   , 6, 0, 6, 200, -200, 200);
   hYdiffvsshowersector = new TH2F("hYdiffvsshowersector", "Ydiff vs sector, shower"   , 6, 0, 6, 200, -200, 200);
   hshower_quality      = new TH2F("hshower_quality"     , "quality vs sector, shower" , 6, 0, 6, 100, 0, 10);

   hXdiffvsshowersector->SetXTitle("sector");
   hXdiffvsshowersector->SetYTitle("Xdiff");
   histList->Add(hXdiffvsshowersector, "metamatching--gridx--gridy--newpage");
   hYdiffvsshowersector->SetXTitle("sector");
   hYdiffvsshowersector->SetYTitle("Ydiff");
   histList->Add(hYdiffvsshowersector, "metamatching--gridx--gridy");
   hshower_quality     ->SetXTitle("sector");
   hshower_quality     ->SetYTitle("Quality");
   histList->Add(hshower_quality     , "metamatching--gridx--gridy");
   
   hXdiffvsshw = new TH2F("hXdiffvsshw", "Xdiff/ShwSigma_vs_Sect, shower", 6, 0, 6, 200, -6, 6);
   hYdiffvsshw = new TH2F("hYdiffvsshw", "Ydiff/ShwSigma_vs_Sect, shower", 6, 0, 6, 200, -6, 6);

   hXdiffvsshw->SetXTitle("sector");
   hXdiffvsshw->SetYTitle("Xdiff/ShwSigmaX");
   histList->Add(hXdiffvsshw, "metamatching--gridx--gridy");

   hYdiffvsshw->SetXTitle("sector");
   hYdiffvsshw->SetYTitle("Ydiff/ShwSigmaY");
   histList->Add(hYdiffvsshw, "metamatching--gridx--gridy");

   hXdiffvsshoCol = new TH2F("hXdiffvsshoCol", "Xdiff vs Shw Col"   , 198, 0, 198, 100, -100, 100);
   hXdiffvsshoRow = new TH2F("hXdiffvsshoRow", "Xdiff vs Shw Row"   , 198, 0, 198, 100, -100, 100);
   hYdiffvsshoCol = new TH2F("hYdiffvsshoCol", "Ydiff vs Shw Col"   , 198, 0, 198, 100, -100, 100);
   hYdiffvsshoRow = new TH2F("hYdiffvsshoRow", "Ydiff vs Shw Row"   , 198, 0, 198, 100, -100, 100);
   

   hXdiffvsshoCol ->SetXTitle("COL + 33 * SEC");
   hXdiffvsshoCol ->SetYTitle("Xdiff");
   histList->Add(hXdiffvsshoCol, "metamatching--gridx--gridy");
   hXdiffvsshoRow ->SetXTitle("ROW + 33 * SEC");
   hXdiffvsshoRow ->SetYTitle("Xdiff");
   histList->Add(hXdiffvsshoRow, "metamatching--gridx--gridy");
   hYdiffvsshoCol ->SetXTitle("COL + 33 * SEC");
   hYdiffvsshoCol ->SetYTitle("Ydiff");
   histList->Add(hYdiffvsshoCol, "metamatching--gridx--gridy");
   hYdiffvsshoRow ->SetXTitle("ROW + 33 * SEC");
   hYdiffvsshoRow ->SetYTitle("Ydiff");
   histList->Add(hYdiffvsshoRow, "metamatching--gridx--gridy");

   

   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta X MetaMatch, shower");
       sprintf(name, "hXdiffsho[%i]", is);
       hXdiffsho[is] = new TH1F(name, title, 200, -25, 25);
       hXdiffsho[is]->SetXTitle("#Delta X (Shohit - MetaMatch)");
       hXdiffsho[is]->SetYTitle("counts");
       hXdiffsho[is]->SetLineColor(SectColor[is]);
       hXdiffsho[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hXdiffsho[is], "metamatching--6sec");
       } else {
	   histList->Add(hXdiffsho[is], "metamatching--same--6sec");
       }
   }
   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta Y MetaMatch, shower");
       sprintf(name, "hYdiffsho[%i]", is);
       hYdiffsho[is] = new TH1F(name, title, 200, -25, 25);
       hYdiffsho[is]->SetXTitle("#Delta Y (Shohit - MetaMatch)");
       hYdiffsho[is]->SetYTitle("counts");
       hYdiffsho[is]->SetLineColor(SectColor[is]);
       hYdiffsho[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hYdiffsho[is], "metamatching--6sec");
       } else {
	   histList->Add(hYdiffsho[is], "metamatching--same--6sec");
       }
   }


   //####### RK Shower ###########

   hXdiffvsshowersectorRK = new TH2F("hXdiffvsshowersectorRK", "Xdiff vs sector, shower, RK"   , 6, 0, 6, 200, -200, 200);
   hYdiffvsshowersectorRK = new TH2F("hYdiffvsshowersectorRK", "Ydiff vs sector, shower, RK"   , 6, 0, 6, 200, -200, 200);
   hshower_qualityRK      = new TH2F("hshower_qualityRK"     , "quality vs sector, shower, RK" , 6, 0, 6, 100, 0, 10);
   hXdiffvsshowersectorRK -> SetXTitle("sector");
   hXdiffvsshowersectorRK -> SetYTitle("Xdiff");
   histList               -> Add(hXdiffvsshowersectorRK, "metamatching--gridx--gridy");
   hYdiffvsshowersectorRK -> SetXTitle("sector");
   hYdiffvsshowersectorRK -> SetYTitle("Ydiff");
   histList               -> Add(hYdiffvsshowersectorRK, "metamatching--gridx--gridy");
   hshower_qualityRK      -> SetXTitle("sector");
   hshower_qualityRK      -> SetYTitle("Quality");
   histList               -> Add(hshower_qualityRK, "metamatching--gridx--gridy");

   hXdiffvsshoColRK = new TH2F("hXdiffvsshoColRK", "Xdiff vs Shw Col, RK"   , 198, 0, 198, 100, -100, 100);
   hXdiffvsshoRowRK = new TH2F("hXdiffvsshoRowRK", "Xdiff vs Shw Row, RK"   , 198, 0, 198, 100, -100, 100);
   hYdiffvsshoColRK = new TH2F("hYdiffvsshoColRK", "Ydiff vs Shw Col, RK"   , 198, 0, 198, 100, -100, 100);
   hYdiffvsshoRowRK = new TH2F("hYdiffvsshoRowRK", "Ydiff vs Shw Row, RK"   , 198, 0, 198, 100, -100, 100);

   hXdiffvsshoColRK -> SetXTitle("COL + 33 * SEC");
   hXdiffvsshoColRK -> SetYTitle("Xdiff");
   histList       -> Add(hXdiffvsshoColRK, "metamatching--gridx--gridy");
   hXdiffvsshoRowRK -> SetXTitle("ROW + 33 * SEC");
   hXdiffvsshoRowRK -> SetYTitle("Xdiff");
   histList       -> Add(hXdiffvsshoRowRK, "metamatching--gridx--gridy");
   hYdiffvsshoColRK -> SetXTitle("COL + 33 * SEC");
   hYdiffvsshoColRK -> SetYTitle("Ydiff");
   histList       -> Add(hYdiffvsshoColRK, "metamatching--gridx--gridy");
   hYdiffvsshoRowRK -> SetXTitle("ROW + 33 * SEC");
   hYdiffvsshoRowRK -> SetYTitle("Ydiff");
   histList         -> Add(hYdiffvsshoRowRK, "metamatching--gridx--gridy");
   


   



   //같같같같같같같같같같같같같
   //같캲pc-Meta-hists같같같같�
   //같같같같같같같같같같같같같
   hXdiffvsRpcsector = new TH2F("hXdiffvsRpcsector", "Xdiff vs sector, Rpc"   , 6, 0, 6, 200, -100, 100);
   hYdiffvsRpcsector = new TH2F("hYdiffvsRpcsector", "Ydiff vs sector, Rpc"   , 6, 0, 6, 200, -100, 100);
   hXdiffvsRpc        = new TH2F("hXdiffvsRpc", "Xdiff/RpcSigma vs sector, Rpc"   , 6, 0, 6, 200, -10, 10);
   hYdiffvsRpc        = new TH2F("hYdiffvsRpc", "Ydiff/RpcSigma vs sector, Rpc"   , 6, 0, 6, 200, -10, 10);
   hRpc_quality       = new TH2F("hRpc_quality"     , "quality vs sector, Rpc"    , 6, 0, 6, 100, -10, 10);

   hXdiffvsRpcsector -> SetXTitle("sector");
   hXdiffvsRpcsector -> SetYTitle("XDiff");
   histList -> Add(hXdiffvsRpcsector, "metamatching--gridx--gridy--newpage");
   hYdiffvsRpcsector -> SetXTitle("sector");
   hYdiffvsRpcsector -> SetYTitle("YDiff");
   histList -> Add(hYdiffvsRpcsector, "metamatching--gridx--gridy");
   hXdiffvsRpc -> SetXTitle("sector");
   hXdiffvsRpc -> SetYTitle("XDiff/SigmaRpc");
   histList         -> Add(hXdiffvsRpc, "metamatching--gridx--gridy");
   hYdiffvsRpc -> SetXTitle("sector");
   hYdiffvsRpc -> SetYTitle("YDiff/SigmaRpc");
   histList         -> Add(hYdiffvsRpc, "metamatching--gridx--gridy");
   hRpc_quality        ->SetXTitle("sector");
   hRpc_quality        ->SetYTitle("Quality");
   histList->Add(hRpc_quality        , "metamatching--gridx--gridy");


   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta X MetaMatch, Rpc");
       sprintf(name, "hXdiffRpc[%i]", is);
       hXdiffRpc[is] = new TH1F(name, title, 200, -25, 25);
       hXdiffRpc[is]->SetXTitle("#Delta X (Rpchit - MetaMatch)");
       hXdiffRpc[is]->SetYTitle("counts");
       hXdiffRpc[is]->SetLineColor(SectColor[is]);
       hXdiffRpc[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hXdiffRpc[is], "metamatching--6sec");
       } else {
	   histList->Add(hXdiffRpc[is], "metamatching--same--6sec");
       }
   }
   for (Int_t is = 0; is < 6; is++)
   {
       sprintf(title, "#Delta Y MetaMatch, Rpc");
       sprintf(name, "hYdiffRpc[%i]", is);
       hYdiffRpc[is] = new TH1F(name, title, 200, -100, 100);
       hYdiffRpc[is]->SetXTitle("#Delta Y (Rpchit - MetaMatch)");
       hYdiffRpc[is]->SetYTitle("counts");
       hYdiffRpc[is]->SetLineColor(SectColor[is]);
       hYdiffRpc[is]->SetLineWidth(8);
       if (is == 0) {
	   histList->Add(hYdiffRpc[is], "metamatching--6sec");
       } else {
	   histList->Add(hYdiffRpc[is], "metamatching--same--6sec");
       }
   }


   
}


void HQAHistograms::bookHistTracking()
{

   Char_t name[256];
   Char_t title[256];

   splineTrack_scat = new TH2F("splineTrack_scat", "splineTrack (LAB) ", 105, -1.05, 1.05, 105, -1.05, 1.05);
   splineTrack_scat->SetXTitle("sin(theta)*sin(phi-90)");
   splineTrack_scat->SetYTitle("sin(theta)*cos(phi-90)");
   histList->Add(splineTrack_scat, "tracking--nobox");

   splineTrack_massCharge = new TH1F("splineTrack_massCharge", "splineTrack: mass * charge", 125, -500., 2000.);
   splineTrack_massCharge->SetXTitle("mass*charge");
   histList->Add(splineTrack_massCharge, "tracking--");

   rungeKuttaTrack_scat = new TH2F("rungeKuttaTrack_scat", "Runge Kutta (LAB)", 105, -1.05, 1.05, 105, -1.05, 1.05);
   rungeKuttaTrack_scat->SetXTitle("sin(theta)*sin(phi-90)");
   rungeKuttaTrack_scat->SetYTitle("sin(theta)*cos(phi-90)");
   histList->Add(rungeKuttaTrack_scat, "tracking--nobox");

   rungeKuttaTrack_massCharge = new TH1F("rungeKuttaTrack_massCharge", "Runge Kutta: mass * charge", 125, -500., 2000.);
   rungeKuttaTrack_massCharge->SetXTitle("mass*charge");
   histList->Add(rungeKuttaTrack_massCharge, "tracking--");

   for (Int_t s = 0; s < 6; s++) { //for RPC/SHOWER
      sprintf(name , "trackingSpline_sys0[%d]", s);
      sprintf(title, "Mass SplineTrack123(4) sec%d (mass for TOFINO)", s);
      trackingSpline_sys0[s] = new TH1F(name, title, 125, -500, 2000);

      trackingSpline_sys0[s]->SetXTitle("M [MeV]");
      trackingSpline_sys0[s]->SetYTitle("counts/event");
      trackingSpline_sys0[s]->SetLineColor(SectColor[s]);
      trackingSpline_sys0[s]->SetLineWidth(4);

      if (s == 0) {
         histList->Add(trackingSpline_sys0[s], "tracking--6sec");
      } else      {
         histList->Add(trackingSpline_sys0[s], "tracking--same--6sec");
      }
   }

   for (Int_t s = 0; s < 6; s++) { //for TOF
      sprintf(name , "trackingSpline_sys1[%d]", s);
      sprintf(title, "Mass SplineTrack123(4) sec%d (mass for TOF)", s);
      trackingSpline_sys1[s] = new TH1F(name, title, 125, -500, 2000);

      trackingSpline_sys1[s]->SetXTitle("M [MeV]");
      trackingSpline_sys1[s]->SetYTitle("counts/event");
      trackingSpline_sys1[s]->SetLineColor(SectColor[s]);
      trackingSpline_sys1[s]->SetLineWidth(4);

      if (s == 0) {
         histList->Add(trackingSpline_sys1[s], "tracking--6sec");
      } else      {
         histList->Add(trackingSpline_sys1[s], "tracking--same--6sec");
      }
   }

   for (Int_t s = 0; s < 6; s++) { //for RPC/SHOWER
      sprintf(name , "trackingRK_sys0[%d]", s);
      sprintf(title, "Mass Runge.K.Track123(4) sec%d (mass for TOFINO)", s);
      trackingRK_sys0[s] = new TH1F(name, title, 125, -500, 2000);

      trackingRK_sys0[s]->SetXTitle("M [MeV]");
      trackingRK_sys0[s]->SetYTitle("counts/event");
      trackingRK_sys0[s]->SetLineColor(SectColor[s]);
      trackingRK_sys0[s]->SetLineWidth(4);

      if (s == 0) {
         histList->Add(trackingRK_sys0[s], "tracking--6sec");
      } else      {
         histList->Add(trackingRK_sys0[s], "tracking--same--6sec");
      }
   }

   for (Int_t s = 0; s < 6; s++) { //for TOF
      sprintf(name , "trackingRK_sys1[%d]", s);
      sprintf(title, "Mass Runge.K.Track123(4) sec%d (mass for TOF)", s);
      trackingRK_sys1[s] = new TH1F(name, title, 125, -500, 2000);

      trackingRK_sys1[s]->SetXTitle("M [MeV]");
      trackingRK_sys1[s]->SetYTitle("counts/event");
      trackingRK_sys1[s]->SetLineColor(SectColor[s]);
      trackingRK_sys1[s]->SetLineWidth(4);

      if (s == 0) {
         histList->Add(trackingRK_sys1[s], "tracking--6sec");
      } else      {
         histList->Add(trackingRK_sys1[s], "tracking--same--6sec");
      }
   }

}

void HQAHistograms::bookHistPid()
{

   Int_t   SelectColor[16] = { 4, 1, 2, 3, 6, 7, 14, 17, 11, 12, 13, 15, 16, 18, 22, 33 };

   Text_t buffer[80], buffer2[80];

   //multiplicities
   hparticle_multrk = new TH1F("hparticle_multrk", "rk mult", 65, -0.5, 64.5);
   hparticle_multrk->SetXTitle("Runge-Kutta Mult.");
   histList->Add(hparticle_multrk, "particlecand--");


   //lepton times
   hparticle_lepton_tof_vs_rod_sys0 = new TH2F("hparticle_lepton_tof_vs_rod_sys0", "Pid tof for leptons RK - sys0", 1152, 0, 1152, 200, -30., 20.);
   hparticle_lepton_tof_vs_rod_sys0->SetXTitle("RPC Cell");
   hparticle_lepton_tof_vs_rod_sys0->SetYTitle("TOF for electrons");
   histList->Add(hparticle_lepton_tof_vs_rod_sys0, "particlecand--");

   hparticle_lepton_tof_vs_rod_sys1 = new TH2F("hparticle_lepton_tof_vs_rod_sys1", "Pid tof for leptons RK - sys1", 384, 0, 384, 200, 0., 20.);
   hparticle_lepton_tof_vs_rod_sys1->SetXTitle("Tof rod");
   hparticle_lepton_tof_vs_rod_sys1->SetYTitle("TOF for electrons");
   histList->Add(hparticle_lepton_tof_vs_rod_sys1, "particlecand--");

   hparticle_lepton_tof_vs_startstrip_sys0 = new TH2F("hparticle_lepton_tof_vs_startstrip_sys0", "Pid tof for leptons RK vs start strip sys0", NSTART_STRIPS, 0, NSTART_STRIPS, 200, 0., 20.);
   hparticle_lepton_tof_vs_startstrip_sys0->SetXTitle("Start Strip");
   hparticle_lepton_tof_vs_startstrip_sys0->SetYTitle("TOF for electrons");
   histList->Add(hparticle_lepton_tof_vs_startstrip_sys0, "particlecand--");

   hparticle_lepton_tof_vs_startstrip_sys1= new TH2F("hparticle_lepton_tof_vs_startstrip_sys1", "Pid tof for leptons RK vs start strip sys1", NSTART_STRIPS, 0, NSTART_STRIPS, 200, 0., 20.);
   hparticle_lepton_tof_vs_startstrip_sys1->SetXTitle("Start Strip");
   hparticle_lepton_tof_vs_startstrip_sys1->SetYTitle("TOF for electrons");
   histList->Add(hparticle_lepton_tof_vs_startstrip_sys1, "particlecand--");


   hparticle_lepton_tof_all_sys0 = new TH1F("hparticle_lepton_tof_all_sys0", "Pid tof for leptons RK sys0 sum", 200, 0., 20.);
   hparticle_lepton_tof_all_sys0 ->SetXTitle("normalized tof");
   hparticle_lepton_tof_all_sys0 ->SetYTitle("counts");
   histList->Add(hparticle_lepton_tof_all_sys0, "particlecand--");

   hparticle_lepton_tof_all_sys1 = new TH1F("hparticle_lepton_tof_all_sys1", "Pid tof for leptons RK sys1 sum ", 200, 0., 20.);
   hparticle_lepton_tof_all_sys1 ->SetXTitle("normalized tof");
   hparticle_lepton_tof_all_sys1 ->SetYTitle("counts");
   histList->Add(hparticle_lepton_tof_all_sys1, "particlecand--");

   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_lepton_tof_vs_start_sys0[%i]",i);
       hparticle_lepton_tof_vs_start_sys0[i] = new TH1F(buffer,"Pid tof for leptons RK sys0 per start strip",100,0.,20.);
       hparticle_lepton_tof_vs_start_sys0[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_lepton_tof_vs_start_sys0[i] ->SetXTitle("normalized tof");
       hparticle_lepton_tof_vs_start_sys0[i] ->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_lepton_tof_vs_start_sys0[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_lepton_tof_vs_start_sys0[i],"particlecand--same");
       }
   }

   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_lepton_tof_vs_start_sys1[%i]",i);
       hparticle_lepton_tof_vs_start_sys1[i] = new TH1F(buffer,"Pid tof for leptons RK sys1 per start strip",100,0.,20.);
       hparticle_lepton_tof_vs_start_sys1[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_lepton_tof_vs_start_sys1[i] ->SetXTitle("normalized tof");
       hparticle_lepton_tof_vs_start_sys1[i] ->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_lepton_tof_vs_start_sys1[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_lepton_tof_vs_start_sys1[i],"particlecand--same");
       }
   }


   //pion times
   hparticle_pi_tof_vs_rod_sys0 = new TH2F("hparticle_pi_tof_vs_rod_sys0", "Pid tof for pions RK - sys0", 1152, 0, 1152, 200, -10., 10.);
   hparticle_pi_tof_vs_rod_sys0->SetXTitle("RPC Cell");
   hparticle_pi_tof_vs_rod_sys0->SetYTitle("TOF for pions");
   histList->Add(hparticle_pi_tof_vs_rod_sys0, "particlecand--");

   hparticle_pi_tof_vs_rod_sys1 = new TH2F("hparticle_pi_tof_vs_rod_sys1", "Pid tof for pions RK - sys1", 384, 0, 384, 200, -10., 10.);
   hparticle_pi_tof_vs_rod_sys1->SetXTitle("Tof rod");
   hparticle_pi_tof_vs_rod_sys1->SetYTitle("TOF for pions");
   histList->Add(hparticle_pi_tof_vs_rod_sys1, "particlecand--");

   hparticle_pi_eloss_vs_rod_sys1 = new TH2F("hparticle_pi_eloss_vs_rod_sys1", "Pid eloss for pions RK - sys1", 384, 0, 384, 100, 0., 10.);
   hparticle_pi_eloss_vs_rod_sys1->SetXTitle("Tof rod");
   hparticle_pi_eloss_vs_rod_sys1->SetYTitle("Eloss for pions");
   histList->Add(hparticle_pi_eloss_vs_rod_sys1, "particlecand--");

   hparticle_p_eloss_vs_rod_sys1 = new TH2F("hparticle_p_eloss_vs_rod_sys1", "Pid eloss forprotons RK - sys1", 384, 0, 384, 100, 0., 10.);
   hparticle_p_eloss_vs_rod_sys1->SetXTitle("Tof rod");
   hparticle_p_eloss_vs_rod_sys1->SetYTitle("Eloss for protons");
   histList->Add(hparticle_p_eloss_vs_rod_sys1, "particlecand--");

   hparticle_pi_metahit_vs_phi_sys1 = new TH2F("hparticle_pi_metahit_vs_phi_sys1", "Pid pions RK metahit vs phi - sys1", 360, 0, 360, 64, 0., 64.);
   hparticle_pi_metahit_vs_phi_sys1->SetXTitle("track phi");
   hparticle_pi_metahit_vs_phi_sys1->SetYTitle("TOF rod");
   histList->Add(hparticle_pi_metahit_vs_phi_sys1, "particlecand--");

   hparticle_pi_tof_vs_startstrip_sys0 = new TH2F("hparticle_pi_tof_vs_startstrip_sys0", "Pid tof for pion RK vs start strip sys0", NSTART_STRIPS, 0, NSTART_STRIPS, 200, -10., 10.);
   hparticle_pi_tof_vs_startstrip_sys0->SetXTitle("Start Strip");
   hparticle_pi_tof_vs_startstrip_sys0->SetYTitle("TOF for pions");
   histList->Add(hparticle_pi_tof_vs_startstrip_sys0, "particlecand--");

   hparticle_pi_tof_vs_startstrip_sys1= new TH2F("hparticle_pi_tof_vs_startstrip_sys1", "Pid tof for pion RK vs start strip sys1", NSTART_STRIPS, 0, NSTART_STRIPS, 200, -10., 10.);
   hparticle_pi_tof_vs_startstrip_sys1->SetXTitle("Start Strip");
   hparticle_pi_tof_vs_startstrip_sys1->SetYTitle("TOF for pions");
   histList->Add(hparticle_pi_tof_vs_startstrip_sys1, "particlecand--");


   hparticle_pi_tof_GaussFit_all_sys0 = new TF1("hparticle_pi_tof_GaussFit_all_sys0",GaussFitFunc,-20,20,3);
   hparticle_pi_tof_GaussFit_all_sys0 -> SetLineColor(2);

   hparticle_pi_tof_all_sys0 = new TH1F("hparticle_pi_tof_all_sys0", "Pid tof for pion RK sys0 sum", 800, -10., 10.);
   hparticle_pi_tof_all_sys0 ->SetXTitle("TOF for pions");
   hparticle_pi_tof_all_sys0 ->SetYTitle("counts");
   histList->Add(hparticle_pi_tof_all_sys0, "particlecand--");

   hparticle_pi_tof_GaussFit_all_sys1 = new TF1("hparticle_pi_tof_GaussFit_all_sys1",GaussFitFunc,-20,20,3);
   hparticle_pi_tof_GaussFit_all_sys1 -> SetLineColor(2);

   hparticle_pi_tof_all_sys1 = new TH1F("hparticle_pi_tof_all_sys1", "Pid tof for pion RK sys1 sum ", 800, -10., 10.);
   hparticle_pi_tof_all_sys1 ->SetXTitle("TOF for pions");
   hparticle_pi_tof_all_sys1 ->SetYTitle("counts");
   histList->Add(hparticle_pi_tof_all_sys1, "particlecand--");

   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_pi_tof_GaussFit_sys0[%i]",i);
       hparticle_pi_tof_GaussFit_sys0[i] = new TF1(buffer,GaussFitFunc,-20,20,3);
       hparticle_pi_tof_GaussFit_sys0[i] -> SetLineColor(SelectColor[i%16]);

       sprintf(buffer,"hparticle_pi_tof_vs_start_sys0[%i]",i);
       hparticle_pi_tof_vs_start_sys0[i] = new TH1F(buffer,"Pid tof for pion RK sys0 per start strip",400,-10.,10.);
       hparticle_pi_tof_vs_start_sys0[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_pi_tof_vs_start_sys0[i] ->SetXTitle("TOF for pions");
       hparticle_pi_tof_vs_start_sys0[i] ->SetYTitle("counts");


       if(i==0)
       {
	       histList->Add(hparticle_pi_tof_vs_start_sys0[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_pi_tof_vs_start_sys0[i],"particlecand--same");
       }
   }


   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_pi_tof_vs_startMod0_sys0[%i]",i);
       hparticle_pi_tof_vs_startMod0_sys0[i] = new TH1F(buffer,"Pid tof for pion RK sys0 per startMod0 strip",400,-10.,10.);
       hparticle_pi_tof_vs_startMod0_sys0[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_pi_tof_vs_startMod0_sys0[i] ->SetXTitle("TOF for pions");
       hparticle_pi_tof_vs_startMod0_sys0[i] ->SetYTitle("counts");

       if(i==0)
       {
           histList->Add(hparticle_pi_tof_vs_startMod0_sys0[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_pi_tof_vs_startMod0_sys0[i],"particlecand--same");
       }

   }


   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_pi_tof_vs_startMod1_sys0[%i]",i);
       hparticle_pi_tof_vs_startMod1_sys0[i] = new TH1F(buffer,"Pid tof for pion RK sys0 per startMod1 strip",400,-10.,10.);
       hparticle_pi_tof_vs_startMod1_sys0[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_pi_tof_vs_startMod1_sys0[i] ->SetXTitle("TOF for pions");
       hparticle_pi_tof_vs_startMod1_sys0[i] ->SetYTitle("counts");


       if(i==0)
       {
           histList->Add(hparticle_pi_tof_vs_startMod1_sys0[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_pi_tof_vs_startMod1_sys0[i],"particlecand--same");
       }
   }


   for(Int_t i=0; i<NSTART_STRIPS; i++)
   {
       sprintf(buffer,"hparticle_pi_tof_GaussFit_sys1[%i]",i);
       hparticle_pi_tof_GaussFit_sys1[i] = new TF1(buffer,GaussFitFunc,-20,20,3);
       hparticle_pi_tof_GaussFit_sys1[i] -> SetLineColor(SelectColor[i%16]);

       sprintf(buffer,"hparticle_pi_tof_vs_start_sys1[%i]",i);
       hparticle_pi_tof_vs_start_sys1[i] = new TH1F(buffer,"Pid tof for pion RK sys1 per start strip",400,-10.,10.);
       hparticle_pi_tof_vs_start_sys1[i] ->SetLineColor(SelectColor[i%16]);
       hparticle_pi_tof_vs_start_sys1[i] ->SetXTitle("TOF for pions");
       hparticle_pi_tof_vs_start_sys1[i] ->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_pi_tof_vs_start_sys1[i],"particlecand--");
       }
       else
       {
           histList->Add(hparticle_pi_tof_vs_start_sys1[i],"particlecand--same");
       }
   }
   

   


   //theta and phi

   //RK histos
   for (Int_t i = 0; i < 6; i++) {
      sprintf(buffer, "hparticle_RK_theta_sec%i", i);
      hparticle_RK_theta_sec[i] = new TH1F(buffer, buffer, 50, 0, 100);
      hparticle_RK_theta_sec[i]->SetLineColor(SectColor[i]);
      hparticle_RK_theta_sec[i]->SetXTitle("#Theta [degrees]");
      if (i == 0) histList->Add(hparticle_RK_theta_sec[i], "particlecand--6sec");
      else     histList->Add(hparticle_RK_theta_sec[i], "particlecand--same--6sec");
   }
   hparticle_RK_phi = new TH1F("hparticle_RK_phi", "hparticle_RK_phi", 120, 0, 360);
   hparticle_RK_phi->SetXTitle("#phi (degrees)");
   histList->Add(hparticle_RK_phi, "particlecand--");

   //RK neg, mom>300
   for (Int_t i = 0; i < 6; i++) {
      sprintf(buffer, "hparticle_RK_neg_theta_sec%i", i);
      sprintf(buffer2, "hparticle_RK_theta_sec%i, neg, mom>300", i);
      hparticle_RK_neg_theta_sec[i] = new TH1F(buffer, buffer2, 50, 0, 100);
      hparticle_RK_neg_theta_sec[i]->SetLineColor(SectColor[i]);
      hparticle_RK_neg_theta_sec[i]->SetXTitle("#Theta [degrees]");
      if (i == 0) histList->Add(hparticle_RK_neg_theta_sec[i], "particlecand--6sec");
      else     histList->Add(hparticle_RK_neg_theta_sec[i], "particlecand--same--6sec");

   }
   hparticle_RK_neg_phi = new TH1F("hparticle_RK_neg_phi", "hparticle_RK_phi, neg, mom>300", 120, 0, 360);
   hparticle_RK_neg_phi->SetXTitle("#phi (degrees)");
   histList->Add(hparticle_RK_neg_phi, "particlecand--");

   //proton momentum calculated from velocity vs reconstructed momentum
   for (Int_t i = 0; i < 6; i++) {
      sprintf(buffer, "hparticle_rk_proton_momdif_sys0_sec%i", i);
      hparticle_rk_momdif_sys0_sec[i] = new TH2F(buffer, buffer, 100, 0, 1000, 50, -200, 200);
      histList->Add(hparticle_rk_momdif_sys0_sec[i], "particlecand--");

      sprintf(buffer, "hparticle_rk_proton_momdif_sys1_sec%i", i);
      hparticle_rk_momdif_sys1_sec[i] = new TH2F(buffer, buffer, 100, 0, 1000, 50, -200, 200);
      histList->Add(hparticle_rk_momdif_sys1_sec[i], "particlecand--");
   }

   //MetaMatching for Tof, Rpc and Shower
   for (Int_t i = 0; i < 6; i++)
   {
       sprintf(buffer, "hparticle_MetaMatchQualTof[%i]", i);
       sprintf(buffer2, "ParticleCand MetaMatchTof, sec%i", i);
       hparticle_MetaMatchQualTof[i] = new TH1F(buffer, buffer2, 60, -10, 10);
       hparticle_MetaMatchQualTof[i]->SetXTitle("MetaMatchQuality");
       hparticle_MetaMatchQualTof[i]->SetLineColor(SectColor[i]);
       hparticle_MetaMatchQualTof[i]->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_MetaMatchQualTof[i], "particlecand--6sec");
       }
       else
       {
	   histList->Add(hparticle_MetaMatchQualTof[i], "particlecand--6sec--same");
       }
   }

   for (Int_t i = 0; i < 6; i++)
   {
       sprintf(buffer, "hparticle_MetaMatchQualRpc[%i]", i);
       sprintf(buffer2, "ParticleCand MetaMatchRpc, sec%i", i);
       hparticle_MetaMatchQualRpc[i] = new TH1F(buffer, buffer2, 60, -10, 10);
       hparticle_MetaMatchQualRpc[i]->SetXTitle("MetaMatchQuality");
       hparticle_MetaMatchQualRpc[i]->SetLineColor(SectColor[i]);
       hparticle_MetaMatchQualRpc[i]->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_MetaMatchQualRpc[i], "particlecand--6sec");
       }
       else
       {
	   histList->Add(hparticle_MetaMatchQualRpc[i], "particlecand--6sec--same");
       }
   }

   for (Int_t i = 0; i < 6; i++)
   {
       sprintf(buffer, "hparticle_MetaMatchQualShw[%i]", i);
       sprintf(buffer2, "ParticleCand MetaMatchShw, sec%i", i);
       hparticle_MetaMatchQualShw[i] = new TH1F(buffer, buffer2, 60, -10, 10);
       hparticle_MetaMatchQualShw[i]->SetXTitle("MetaMatchQuality");
       hparticle_MetaMatchQualShw[i]->SetLineColor(SectColor[i]);
       hparticle_MetaMatchQualShw[i]->SetYTitle("counts");
       if(i==0)
       {
	   histList->Add(hparticle_MetaMatchQualShw[i], "particlecand--6sec");
       }
       else
       {
	   histList->Add(hparticle_MetaMatchQualShw[i], "particlecand--6sec--same");
       }
   }

   for(Int_t s=0; s<2; s++)
   {
       for (Int_t i = 0; i < 6; i++)
       {
	   sprintf(buffer, "hparticle_mult[%i][%i]", i, s);
	   sprintf(buffer2, "Multiplicity, sec%i, syst%i", i, s);
	   hparticle_mult[i][s] = new TH1F(buffer, buffer2, 55, -4.5, 50.5);
	   hparticle_mult[i][s] ->SetXTitle("num hits");
	   hparticle_mult[i][s] ->SetLineColor(SectColor[i]);
	   hparticle_mult[i][s] ->SetYTitle("counts");
	   if(i==0)
	   {
	       histList->Add(hparticle_mult[i][s], "particlecand--6sec");
	   }
	   else
	   {
	       histList->Add(hparticle_mult[i][s], "particlecand--6sec--same");
	   }
       }
   }

   for(Int_t s=0; s<2; s++)
   {
       for (Int_t i = 0; i < 6; i++)
       {
	   sprintf(buffer, "hparticle_mult_select[%i][%i]", i, s);
	   sprintf(buffer2, "Multiplicity of accepted tracks, sec%i, syst%i", i, s);
	   hparticle_mult_select[i][s] = new TH1F(buffer, buffer2, 55, -4.5, 50.5);
	   hparticle_mult_select[i][s] ->SetXTitle("num hits");
	   hparticle_mult_select[i][s] ->SetLineColor(SectColor[i]);
	   hparticle_mult_select[i][s] ->SetYTitle("counts");
	   if(i==0)
	   {
	       histList->Add(hparticle_mult_select[i][s], "particlecand--6sec");
	   }
	   else
	   {
	       histList->Add(hparticle_mult_select[i][s], "particlecand--6sec--same");
	   }
       }
   }



   //Particle Mass Spectrum ##########################################################################
   hparticle_mass_GaussFit = new TF1("hparticle_mass_GaussFit",GaussFitFunc,-3000,6000,3);
   hparticle_mass_GaussFit -> SetLineColor(2);
   hparticle_mass_GaussFit_1 = new TF1("hparticle_mass_GaussFit_1",GaussFitFunc,-3000,6000,3);
   hparticle_mass_GaussFit_1 -> SetLineColor(2);
   hparticle_mass_GaussFit_2 = new TF1("hparticle_mass_GaussFit_2",GaussFitFunc,-3000,6000,3);
   hparticle_mass_GaussFit_2 -> SetLineColor(2);

   hparticle_mass = new TH1F("hparticle_mass","hparticle_mass",1000,-3000,3000);
   hparticle_mass->SetXTitle("mass/polarity [MeV/c�]");
   hparticle_mass->SetYTitle("counts");
   hparticle_mass->SetLineColor(1);
   histList->Add(hparticle_mass, "particlecand--logy");

   hparticle_mass_RPC_GaussFit = new TF1("hparticle_mass_RPC_GaussFit",GaussFitFunc,-3000,6000,3);
   hparticle_mass_RPC_GaussFit -> SetLineColor(2);
   hparticle_mass_RPC_GaussFit_1 = new TF1("hparticle_mass_RPC_GaussFit_1",GaussFitFunc,-3000,6000,3);
   hparticle_mass_RPC_GaussFit_1 -> SetLineColor(2);
   hparticle_mass_RPC_GaussFit_2 = new TF1("hparticle_mass_RPC_GaussFit_2",GaussFitFunc,-3000,6000,3);
   hparticle_mass_RPC_GaussFit_2 -> SetLineColor(2);

   hparticle_mass_RPC = new TH1F("hparticle_mass_RPC","hparticle_mass_RPC",1000,-3000,3000);
   histList->Add(hparticle_mass_RPC, "particlecand--same");

   hparticle_mass_TOF_GaussFit = new TF1("hparticle_mass_TOF_GaussFit",GaussFitFunc,-3000,6000,3);
   hparticle_mass_TOF_GaussFit -> SetLineColor(2);
   hparticle_mass_TOF_GaussFit_1 = new TF1("hparticle_mass_TOF_GaussFit_1",GaussFitFunc,-3000,6000,3);
   hparticle_mass_TOF_GaussFit_1 -> SetLineColor(2);
   hparticle_mass_TOF_GaussFit_2 = new TF1("hparticle_mass_TOF_GaussFit_2",GaussFitFunc,-3000,6000,3);
   hparticle_mass_TOF_GaussFit_2 -> SetLineColor(2);

   hparticle_mass_TOF = new TH1F("hparticle_mass_TOF","hparticle_mass_TOF",1000,-3000,3000);
   hparticle_mass_TOF -> SetLineColor(6);
   histList->Add(hparticle_mass_TOF, "particlecand--same");
   //################################################################################################


   hparticle_TofdEdx = new TH1F("hparticle_TofdEdx","dEdx in Tof",200,-10,30);
   hparticle_TofdEdx->SetXTitle("Tof dEdx");
   hparticle_TofdEdx->SetYTitle("counts");
   histList->Add(hparticle_TofdEdx, "particlecand");
}



void HQAHistograms::bookHistDaqScalers()
{

   Int_t nHistRange = 10000;

   histInputScaler  = new TH1F("histInputScaler","Triggerbox In",8,0.5,8.5);
   histDownScaler   = new TH1F("histDownScaler","Triggerbox Down",8,0.5,8.5);
   histGatedScaler  = new TH1F("histGatedScaler","Gated",8,0.5,8.5);
   histAcceptScaler = new TH1F("histAcceptScaler","Accepted",8,0.5,8.5);

   histStartScaler  = new TH1F("histStartScaler","PatternStart",9,0.5,9.5);
   histVetoScaler   = new TH1F("histVetoScaler","PatternVeto",8,0.5,8.5);
   histTofScaler    = new TH1F("histTofScaler","PatternTof",6,0.5,6.5);
   histRpcScaler    = new TH1F("histRpcScaler","PatternRpc",6,0.5,6.5);

   histAllScalerCh  = new TH1F("histAllScalerCh","All scaler channels",150,0.5,150.5);
   histAllScalerCh -> SetXTitle("Scaler Channel");
   histAllScalerCh -> SetYTitle("Counts");
   histAllScalerTrend = new TH2F("histAllScalerTrend","histAllScalerTrend",nHistRange,0,nHistRange,150,0,150);
   histAllScalerTrend  -> SetXTitle("CalEvent Nb.");
   histAllScalerTrend  -> SetYTitle("Scaler Channel");

//      histAcceptScalerTrend[chan]-> SetXTitle("CalEvent Nb.");
//      histAcceptScalerTrend[chan]-> SetYTitle("Counts");


   //add them to the list:
   histList->Add(histInputScaler, "daqscl--gridx--gridy");
   histList->Add(histDownScaler, "daqscl--gridx--gridy");
   histList->Add(histGatedScaler, "daqscl--gridx--gridy");
   histList->Add(histAcceptScaler, "daqscl--gridx--gridy");
   histList->Add(histStartScaler, "daqscl--gridx--gridy");
   histList->Add(histVetoScaler, "daqscl--gridx--gridy");
   histList->Add(histTofScaler, "daqscl--gridx--gridy");
   histList->Add(histRpcScaler, "daqscl--gridx--gridy");

   histList->Add(histAllScalerCh, "daqscl--gridx--gridy");
   histList->Add(histAllScalerTrend, "daqscl--gridx--gridy");

/*
   for (chan = 0; chan < 8; chan++) {
      sprintf(title, "histInputScalerTrend_ch%i", chan);
      sprintf(name, "histInputScalerTrend[%i]", chan);
      histInputScalerTrend[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histInputScalerTrend[chan]->SetXTitle("CalEvent Nb.");
      histInputScalerTrend[chan]->SetYTitle("Counts");
      histList->Add(histInputScalerTrend[chan], "daqscl--gridx--gridy");

      sprintf(title, "histDownScalerTrend_ch%i", chan);
      sprintf(name, "histDownScalerTrend[%i]", chan);
      histDownScalerTrend[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histDownScalerTrend[chan]-> SetXTitle("CalEvent Nb.");
      histDownScalerTrend[chan]-> SetYTitle("Counts");
      histList->Add(histDownScalerTrend[chan], "daqscl--gridx--gridy");

      sprintf(title, "histGatedScalerTrend_ch%i", chan);
      sprintf(name, "histGatedScalerTrend[%i]", chan);
      histGatedScalerTrend[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histGatedScalerTrend[chan]-> SetXTitle("CalEvent Nb.");
      histGatedScalerTrend[chan]-> SetYTitle("Counts");
      histList->Add(histGatedScalerTrend[chan], "daqscl--gridx--gridy");

      sprintf(title, "histAcceptScalerTrend_ch%i", chan);
      sprintf(name, "histAcceptScalerTrend[%i]", chan);
      histAcceptScalerTrend[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histAcceptScalerTrend[chan]-> SetXTitle("CalEvent Nb.");
      histAcceptScalerTrend[chan]-> SetYTitle("Counts");
      histList->Add(histAcceptScalerTrend[chan], "daqscl--gridx--gridy");


      sprintf(title, "Scaler_Start_CH_%i", chan);
      sprintf(name, "histStartScaler[%i]", chan);
      histStartScaler[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histStartScaler[chan]-> SetXTitle("CalEvent Nb.");
      histStartScaler[chan]-> SetYTitle("Counts");
      histList->Add(histStartScaler[chan], "daqscl--gridx--gridy-notshown");

      sprintf(title, "Scaler_Veto_CH_%i", chan);
      sprintf(name, "histVetoScaler[%i]", chan);
      histVetoScaler[chan] = new TH1F(name, title, nHistRange, 0, nHistRange);
      histVetoScaler[chan]-> SetXTitle("CalEvent Nb.");
      histVetoScaler[chan]-> SetYTitle("Counts");
      histList->Add(histVetoScaler[chan], "daqscl--gridx--gridy--notshown");

   }
*/



} // end of bookHistDaqScalers

void HQAHistograms::bookHistShowerRpc()
{

   for (Int_t s = 0; s < 6; ++s) {
      shorpcXdiff_pre[s] = new TH1F(Form("shorpcXdiff_pre_s%d", s), "Shower Pre and RPC X hit correlation", 100, -250, 250);
      shorpcXdiff_pre[s]->SetXTitle("X difference (mm)");
      shorpcXdiff_pre[s]->SetYTitle("yield");
      shorpcXdiff_pre[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcXdiff_pre[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcXdiff_pre[s], "showerrpc--same--6sec");
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcYdiff_pre[s] = new TH1F(Form("shorpcYdiff_pre_s%d", s), "Shower Pre and RPC Y hit correlation", 100, -250, 250);
      shorpcYdiff_pre[s]->SetXTitle("Y difference (mm)");
      shorpcYdiff_pre[s]->SetYTitle("yield");
      shorpcYdiff_pre[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcYdiff_pre[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcYdiff_pre[s], "showerrpc--same--6sec");
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcXdiff_post1[s] = new TH1F(Form("shorpcXdiff_post1_s%d", s), "Shower Post I and RPC X hit correlation", 100, -250, 250);
      shorpcXdiff_post1[s]->SetXTitle("X difference (mm)");
      shorpcXdiff_post1[s]->SetYTitle("yield");
      shorpcXdiff_post1[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcXdiff_post1[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcXdiff_post1[s], "showerrpc--same--6sec");
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcYdiff_post1[s] = new TH1F(Form("shorpcYdiff_post1_s%d", s), "Shower Post I and RPC Y hit correlation", 100, -250, 250);
      shorpcYdiff_post1[s]->SetXTitle("Y difference (mm)");
      shorpcYdiff_post1[s]->SetYTitle("yield");
      shorpcYdiff_post1[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcYdiff_post1[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcYdiff_post1[s], "showerrpc--same--6sec");
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcXdiff_post2[s] = new TH1F(Form("shorpcXdiff_post2_s%d", s), "Shower Post II and RPC X hit correlation", 100, -250, 250);
      shorpcXdiff_post2[s]->SetXTitle("X difference (mm)");
      shorpcXdiff_post2[s]->SetYTitle("yield");
      shorpcXdiff_post2[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcXdiff_post2[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcXdiff_post2[s], "showerrpc--same--6sec");
      }
   }
   for (Int_t s = 0; s < 6; ++s) {
      shorpcYdiff_post2[s] = new TH1F(Form("shorpcYdiff_post2_s%d", s), "Shower Post II and RPC Y hit correlation", 100, -250, 250);
      shorpcYdiff_post2[s]->SetXTitle("Y difference (mm)");
      shorpcYdiff_post2[s]->SetYTitle("yield");
      shorpcYdiff_post2[s]->SetLineColor(SectColor[s]);
      if (s == 0) {
         histList->Add(shorpcYdiff_post2[s], "showerrpc--6sec");
      } else {
         histList->Add(shorpcYdiff_post2[s], "showerrpc--same--6sec");
      }
   }

   for (Int_t s = 0; s < 6; ++s) {
      shorpcXvs_pre[s] = new TH2F(Form("shorpcXvs_pre_s%d", s),
                                  Form("Shower Pre vs RPC X hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcXvs_pre[s]->SetXTitle("X in Pre (mm)");
      shorpcXvs_pre[s]->SetYTitle("X in RPC (mm)");
      histList->Add(shorpcXvs_pre[s], "showerrpc--");

      shorpcXvs_post1[s] = new TH2F(Form("shorpcXvs_post1_s%d", s),
                                    Form("Shower Post I vs RPC X hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcXvs_post1[s]->SetXTitle("X in Post I (mm)");
      shorpcXvs_post1[s]->SetYTitle("X in RPC (mm)");
      histList->Add(shorpcXvs_post1[s], "showerrpc--");

      shorpcXvs_post2[s] = new TH2F(Form("shorpcXvs_post2_s%d", s),
                                    Form("Shower Post II vs RPC X hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcXvs_post2[s]->SetXTitle("X in Post II (mm)");
      shorpcXvs_post2[s]->SetYTitle("X in RPC (mm)");
      histList->Add(shorpcXvs_post2[s], "showerrpc--");

      shorpcYvs_pre[s] = new TH2F(Form("shorpcYvs_pre_s%d", s),
                                  Form("Shower Pre vs RPC Y hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcYvs_pre[s]->SetXTitle("Y in Pre (mm)");
      shorpcYvs_pre[s]->SetYTitle("Y in RPC (mm)");
      histList->Add(shorpcYvs_pre[s], "showerrpc--");

      shorpcYvs_post1[s] = new TH2F(Form("shorpcYvs_post1_s%d", s),
                                    Form("Shower Post I vs RPC Y hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcYvs_post1[s]->SetXTitle("Y in Post I (mm)");
      shorpcYvs_post1[s]->SetYTitle("Y in RPC (mm)");
      histList->Add(shorpcYvs_post1[s], "showerrpc--");

      shorpcYvs_post2[s] = new TH2F(Form("shorpcYvs_post2_s%d", s),
                                    Form("Shower Post II vs RPC Y hit position sector %d", s), 32, -800, 800, 32, -800, 800);
      shorpcYvs_post2[s]->SetXTitle("Y in Post II (mm)");
      shorpcYvs_post2[s]->SetYTitle("Y in RPC (mm)");
      histList->Add(shorpcYvs_post2[s], "showerrpc--");
   }

}




