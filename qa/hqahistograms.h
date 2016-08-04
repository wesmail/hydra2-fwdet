//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
//*-- Modified : 20/01/2004 Jacek Otwinowski
//*-- Modified : 23/01/2004 A.Sadovski
//*-- Modified : 04/01/2005 Jacek Otwinowski
//*-- Modified : 08/02/2005 A.Sadovski
#ifndef HQAHISTOGRAMS_H
#define HQAHISTOGRAMS_H

#include "TObject.h"
#include "TH1.h"
#include "TH2.h"
#include "TF1.h"
#include "TTree.h"

#define NSTART_STRIPS 64

class HQAHistograms : public TObject {

public:
   HQAHistograms();
   virtual ~HQAHistograms() {
      ;
   }

   virtual void bookHist();
   virtual void activateHist();

   TList *getHistList() {
      return histList;
   }

   
private:

   void resetPointers();

   virtual void bookHistStart();
   virtual void bookHistRich();
   virtual void bookHistMdc();
   virtual void bookHistShower();
   virtual void bookHistTof();
   virtual void bookHistRpc();
   virtual void bookHistWall();

   virtual void bookHistRichMDC();

   virtual void bookHistMatching();
   virtual void bookHistTracking();
   virtual void bookHistPid();
   virtual void bookHistDaqScalers();
   virtual void bookHistShowerRpc();

public:

   TTree *OutputTree;   // A Tree where important numbers from Histograms are stored!

   TList *histList;    //! List of all booked histograms

   // Start histograms

   TH1F *stCal_stripMod0;               //! startCal: strip pattern
   TH1F *stCal_stripMod1;           //! startCal: strip pattern for Mod 1
   TH1F *vtCal_strip;               //! startCal: strip pattern
   TH1F *stLatchHist;               //! startCal: strip pattern
   TH1F *stCal_tof;                 //! startCal: time of flight
   TH1F *vtCal_tof;                 //! vetoCal:  time of flight
   TH1F *stCal_tof_strip[NSTART_STRIPS];       //! startCal: time of flight for each strip
   TH1F *vtCal_tof_strip[NSTART_STRIPS];       //! vetoCal:  time of flight for each strip
   TH1F *stHit_tof;                 //! startHit: time of flight
   TH2F *Stm0Vtdiff_vs_stStrip[NSTART_STRIPS];   //! startCal: St-Vt Time diff for 8 Veto channels  module 0
   TH2F *Stm1Vtdiff_vs_stStrip[NSTART_STRIPS];   //! startCal: St-Vt Time diff for 8 Veto channels  module 1

   TH2F *stHit_tof_vs_startstripe;  //! startHit: tofTOF vs startstripe
   TH2F *stHit_vs_stStrip;          //! startHit: startHitTime vs start stripe
   TH2F *stHit_vs_stStrip_Trigg[NSTART_STRIPS]; //! startHit: startHitTime vs strip for 8 trigger inputs
   //! trigger inputs are counted from 1 (hardware)
   TH2F *stVertexXY;                //! event vertex information, X vs Y
   TH1F *stVertexZ;                 //! event vertex information, Z position
   // Daq Scalers histograms

   TH1F *histInputScalerTrend[8];              //! daqScaler: Trigger Box input - trend
   TH1F *histDownScalerTrend[8];              //! daqScaler: Trigger Box input - trend
   TH1F *histGatedScalerTrend[8];              //! daqScaler: Trigger Box input - trend
   TH1F *histAcceptScalerTrend[8];             //! daqScaler: Trigger Box accepted - trend

   TH1F *histStartScalerTrend[8];        //! daqScaler: Start 8 channels - trend
   TH1F *histVetoScalerTrend[8];         //! daqScaler: Veto 8 channels - trend         

   TH1F *histInputScaler;         //! daqScaler: Trigger Box input      
   TH1F *histDownScaler;         //! daqScaler: Trigger Box downscaled    
   TH1F *histGatedScaler;         //! daqScaler: Trigger Box gated with Start       
   TH1F *histAcceptScaler;         //! daqScaler: Trigger Box accepted by DAQ       

   TH1F *histStartScaler;         //! daqScaler: Start Patttern     
   TH1F *histVetoScaler;         //! daqScaler: Veto Pattern 
   TH1F *histTofScaler;         //! daqScaler: Tof Pattern 
   TH1F *histRpcScaler;         //! daqScaler: Rpc Pattern       

   TH1F *histAllScalerCh;         //! daqScaler: counts for all 146 Scaler channels
   TH2F *histAllScalerTrend;     //! daqScaler: shows trend for all 146 Scaler channels


   // Rich histograms
   TH1F *richCal_row;               //! richCal: row
   TH1F *richCal_column;            //! richCal: column
   TH1F *richCal_nSec;              //! richCal: fired pads by sector

   TH2F *richHit_scat;              //! richHit: hit distribution in lab frame
   TH1F *richHit_nSec;              //! richHit: hits by sector

   TH1F *richHit_phi;               //! richHit: phi angle
   TH1F *richHit_theta;             //! richHit: theta angle
   TH1F *richHit_radius;            //! richHit: Ring radius
   TH1F *richHit_centroid;          //! richHit: Ring centroid
   TH1F *richHit_chargeAmpl;        //! richHit: Ring charge amplitude
   TH1F *richHit_ringCol;           //! richHit: Ring column
   TH1F *richHit_ringRow;           //! richHit: Ring row
   TH1F *richHit_ringLocMax4;       //! richHit: Ring Loc Max4

   TH2F *richHit_houTraVsPatMat;    //! richHit: ring algorithm comparison
   TH2F *richHit_patMatVsTheta;     //! richHit: #theta vs. PM
   TH2F *richHit_houTraVsTheta;     //! richHit: #theta vs. HT
   TH2F *richHit_chargeAmplVsTheta; //! richHit: #theta vs. ring amplitude
   TH2F *richHit_radiusVsTheta;     //! richHit: #theta vs. radius

   TH1F *richHit_AverCharge[6];     //! richHit: RingAmplitude per Number of pads in Ring for ach Sector
   TH1F *richHit_NumPads[6];        //! richHit: Number of pads per Ring for each Sector
   TH1F *richHit_NumRings[6];       //! richHit: Number of Rings per Event for each Sector


   // Mdc histograms
   TH2F *mdcRaw_mboVsSector_m0; //! mdcRaw: Plane I, sector vs. mbo mult.
   TH2F *mdcRaw_mboVsSector_m1; //! mdcRaw: Plane II, sector vs. mbo mult.
   TH2F *mdcRaw_mboVsSector_m2; //! mdcRaw: Plane III, sector vs. mbo mult.
   TH2F *mdcRaw_mboVsSector_m3; //! mdcRaw: Plane IV, sector vs. mbo mult.

   TH2F *mdcCal1_nVsSector_m0;  //! mdcCal1: Plane I, sector vs. multiplicity
   TH2F *mdcCal1_nVsSector_m1;  //! mdcCal1: Plane II, sector vs. multiplicity
   TH2F *mdcCal1_nVsSector_m2;  //! mdcCal1: Plane III, sector vs. multiplicity
   TH2F *mdcCal1_nVsSector_m3;  //! mdcCal1: Plane IV, sector vs. multiplicity

   TH2F *mdcCal1_time1VsSector_m0;  //! mdcCal1: Plane I, sector vs. time1
   TH2F *mdcCal1_time1VsSector_m1;  //! mdcCal1: Plane II, sector vs. time1
   TH2F *mdcCal1_time1VsSector_m2;  //! mdcCal1: Plane III, sector vs. time1
   TH2F *mdcCal1_time1VsSector_m3;  //! mdcCal1: Plane IV, sector vs. time1

   TH2F *mdcCal1_tatVsSector_m0;    //! mdcCal1: Plane I, sector vs. t.a.t.
   TH2F *mdcCal1_tatVsSector_m1;    //! mdcCal1: Plane II, sector vs. t.a.t.
   TH2F *mdcCal1_tatVsSector_m2;    //! mdcCal1: Plane III, sector vs. t.a.t.
   TH2F *mdcCal1_tatVsSector_m3;    //! mdcCal1: Plane IV, sector vs. t.a.t.

   TH2F *mdcCal1_t2mt1_vs_t1[6][4]; //! mdcCal1: plane, sector t2-t1 vs. t1
   TH2F *mdcCal1_t2mt1_vs_t1_plane[4]; //! mdcCal1: plane(all sectors) t2-t1 vs. t1
   TH1F *mdcCal1_t1[6][4];          //! mdcCal1: plane, sector t1 distribution
   TH1F *mdcCal1_t1_V2[6][4][6];    //! mdcCal1: plane, sector, layer t1 distribution
   TH1F *mdcCal1_t2mt1[6][4];       //! mdcCal1: plane, sector t2-t1 distribution
   TH1F *mdcCal1_t2mt1_V2[6][4][6]; //! mdcCal1: plane, sector, layer  t2-t1 distribution
   TH1F *mdcCutCounts[8];           //! mdc event counts total and vs. cuts applied

   TH2F *mdcHit_scat_m0;    //! mdcHit: Plane I, hit distribution
   TH2F *mdcHit_scat_m1;    //! mdcHit: Plane II, hit distribution
   TH2F *mdcHit_scat_m2;    //! mdcHit: Plane III, hit distribution
   TH2F *mdcHit_scat_m3;    //! mdcHit: Plane IV, hit distribution

   TH1F *mdcSeg_Chi2[6][2]; //! mdcSeg: sector, I/O Segmment chi2 distribution

   // Tof histograms
   TH1F *tofHit_n[6];       //! tofHit: hit multiplicity for each sector
   TH1F *tofHit_tot;        //! tofHit: total hit multiplicity
   TH1F *tofHit_nSec;       //! tofHit: hit multiplicity/sector
   TH2F *tofHit_scat;       //! tofHit: hit distribution in lab frame
   TH1F *tofHit_tof;        //! tofHit: time of flight
   TH1F *tofHit_phi;        //! tofHit: phi angle
   TH1F *tofHit_theta;      //! tofHit: theta angle


   // Rpc histograms
   TH1F *rpcHit_n[6];       //! rpcHit: hit multiplicity
   TH1F *rpcHit_tot;        //! rpcHit: total hit multiplicity
   TH1F *rpcHit_nSec;       //! rpcHit: hit multiplicity/sector
   TH2F *rpcHit_scat;       //! rpcHit: hit distribution in lab frame
   TH1F *rpcHit_tof;        //! rpcHit: time of flight
   TH1F *rpcHit_phi;        //! rpcHit: phi angle
   TH1F *rpcHit_theta;      //! rpcHit: theta angle


   // Shower histograms
   TH2F *shoHit_scat;           //! showerHit: hit distribution in lab frame
   TH1F *shoHit_theta;          //! showerHit: theta angle
   TH1F *shoHit_phi;            //! showerHit: phi angle

   TH2F *shoHit_chargeVsSector_m0;   //! showerHit: Mod 0, charge on loc. max.
   TH2F *shoHit_chargeVsSector_m1;   //! showerHit: Mod 1, charge on loc. max.
   TH2F *shoHit_chargeVsSector_m2;   //! showerHit: Mod 2, charge on loc. max.

   TH2F *shoHit_rowVsSector_m0;   //! showerHit: Mod 0, sector vs. row
   TH2F *shoHit_rowVsSector_m1;   //! showerHit: Mod 1, sector vs. row
   TH2F *shoHit_rowVsSector_m2;   //! showerHit: Mod 2, sector vs. row

   TH2F *shoHit_colVsSector_m0;   //! showerHit: Mod 0, sector vs. col
   TH2F *shoHit_colVsSector_m1;   //! showerHit: Mod 1, sector vs. col
   TH2F *shoHit_colVsSector_m2;   //! showerHit: Mod 2, sector vs. col

   TH2F *shoHit_sectorVsModule; //! showerHit: num hits by module
   TH1F *shoHit_nSec;           //! showerHit: num hits/event/sector

   TH2F *shoHitTof_scat;        //! showerHitTof: hit distribution in lab frame
   TH1F *shoHitTof_nSec;        //! showerHitTof: num hits/event/sector
   TH1F *shoHit_nRow;           //! Shower Rows (All sectors)
   TH1F *shoHit_nCol;           //! Shower Cols (All sectors)
   TH1F *shoHitSums[6][3];      //! ShowerSums
   TH1F *shoHit_n[6];           //! Shower hit multiplicities per Sector
   TH1F *shoHit_nm[6][3];       //! Shower hit multiplicities per Sector and module



   // mom histograms
   TH1F *splineTrack_massCharge;     //! spline track: mass * charge
   TH2F *splineTrack_scat;           //! splineTrack: track distribution in lab frame
   TH1F *rungeKuttaTrack_massCharge; //! spline track: mass * charge
   TH2F *rungeKuttaTrack_scat;       //! splineTrack: track distribution in lab frame

   // ForwardWall histograms
   TH1F *hWallCellSmall;   //!Wall cell Small
   TH1F *hWallCellMedium;  //!Wall cell Medium
   TH1F *hWallCellLarge;   //!Wall cell Large
   TH1F *hMultWall;        //!Wall mult
   TH2F *hWallCellTime;    //!Wall Time vs Cell
   TH2F *hWallCellAdc;     //!Wall Charge vs Cell
   TH2F *hWallXY;          //!Wall X vs Wall Y
   TH1F *hWallHitNumI;     //!Number of hits I   quarter
   TH1F *hWallHitNumII;    //!Number of hits II  quarter
   TH1F *hWallHitNumIII;   //!Number of hits III quarter
   TH1F *hWallHitNumIV;    //!Number of hits IV  quarter


   //Tracking  sector-vise TOF/SHOWER-TOFINO
   TH1F *trackingSpline_sys0[6];      //! Spline     tracking MDC12-3(4)
   TH1F *trackingRK_sys0[6];          //! RungeKutta tracking MDC12-3(4)

   TH1F *trackingSpline_sys1[6];      //! Spline     tracking MDC12-3(4)
   TH1F *trackingRK_sys1[6];          //! RungeKutta tracking MDC12-3(4)

   // Rich-Mdc histograms
   TH1F *richmdc_dtheta[6];   //! RichHit-MdcInerSeg: delta theta (sectorwise)
   TH1F *richmdc_dphi[6];     //! RichHit-MdcInerSeg: delta phi (sectorwise)
   TH1F *richmdc_lep;         //! RichHit-MdcInerSeg: leptons per event (sectorwise)

   TH1F *hsecspline;           //! Number of spline objects matched to meta per sector
   TH1F *hsecspline0;          //! the same for system 0
   TH1F *hsecspline1;          //! the same for system 1

   // MetaMatch histograms
   TH2F *hXdiffvsshowersector; //! Difference between X SHOWER hit position and crossing of track through SHOWER, vs sector
   TH2F *hYdiffvsshowersector; //! Difference between Y SHOWER hit position and crossing of track through SHOWER, vs sector
   TH2F *hXdiffvsshw;          //! Diff.  ShwX and MdcX vs sector / SigmaShw
   TH2F *hYdiffvsshw;          //! Diff.  ShwY and MdcY vs sector / SigmaShw
   TH2F *hshower_quality;      //! SHOWER quality vs sector
   TH2F *htof_quality;         //! TOF    quality vs sector
   TH2F *hXdiffvstofstrip;     //! Difference between X TOF hit position and crossing of track through TOF, vs TOF strip
   TH2F *hYdiffvstofstrip;     //! Difference between Y TOF hit position and crossing of track through TOF, vs TOF strip
   TH2F *hXdiffvsshoCol;       //! Difference between X SHOWER hit position and crossing of track through SHOWER, vs Column
   TH2F *hXdiffvsshoRow;       //! Difference between X SHOWER hit position and crossing of track through SHOWER, vs Row
   TH2F *hYdiffvsshoCol;       //! Difference between Y SHOWER hit position and crossing of track through SHOWER, vs Column
   TH2F *hYdiffvsshoRow;       //! Difference between Y SHOWER hit position and crossing of track through SHOWER, vs Row


   TH2F *hXdiffvsRpcsector;    //! Difference between X Rpc hit position and crossing of track through Rpc, vs Sector
   TH2F *hYdiffvsRpcsector;    //! Difference between Y Rpc hit position and crossing of track through Rpc, vs Sector
   TH2F *hXdiffvsRpc;          //! Diff.  RpcX and MetaX vs sector / SigmaRpc
   TH2F *hYdiffvsRpc;          //! Diff.  RpcY and MetaY vs sector / SigmaRpc
   TH2F *hRpc_quality;         //! SHOWER Rpc quality vs sector



   TH2F *hXdiffvsshowersectorRK;
   TH2F *hXdiffvstofstripRK;
   TH2F *hYdiffvsshowersectorRK;
   TH2F *hYdiffvstofstripRK;
   TH2F *hshower_qualityRK;
   TH2F *htof_qualityRK;

   TH2F *hXdiffvstofstripRK_neg;
   TH2F *hYdiffvstofstripRK_neg;
   TH2F *hXdiffvsthetaRK_neg;
   TH2F *hYdiffvsthetaRK_neg;

   TH2F *hXdiffvsshoColRK;
   TH2F *hXdiffvsshoRowRK;
   TH2F *hYdiffvsshoColRK;
   TH2F *hYdiffvsshoRowRK;

   TH1F *hXdiffTof[6];      //Delta X for TOF-MDC in MetahitClass
   TH1F *hYdiffTof[6];      //Delta Y for TOF-MDC in MetahitClass

   TH1F *hXdiffsho[6];
   TH1F *hYdiffsho[6];

   TH1F *hXdiffRpc[6];
   TH1F *hYdiffRpc[6];

   // Runge-Kutta
   TH1F *hparticle_multrk;                              //! multiplicity of runge-kutta tracks
   TH2F *hparticle_lepton_tof_vs_rod_sys0;              //! lepton time vs rod for sys0
   TH2F *hparticle_lepton_tof_vs_rod_sys1;              //! lepton time vs rod for sys1
   TH2F *hparticle_lepton_tof_vs_startstrip_sys0;       //! lepton time in system0 vs strip for start
   TH2F *hparticle_lepton_tof_vs_startstrip_sys1;       //! lepton time in system1 vs strip for start
   TH1F *hparticle_lepton_tof_all_sys0;                 //! lepton time in system 0 for all start strips
   TH1F *hparticle_lepton_tof_all_sys1;                 //! lepton time in system 1 for all start strips
   TH1F *hparticle_lepton_tof_vs_start_sys0[NSTART_STRIPS];    //! lepton time for in system0 each start strip
   TH1F *hparticle_lepton_tof_vs_start_sys1[NSTART_STRIPS];    //! lepton time for in system1 each start strip for start
   TH2F *hparticle_pi_tof_vs_rod_sys0;                  //! pion  time vs rod sys0
   TH2F *hparticle_pi_tof_vs_rod_sys1;                  //! pion time vs rod sys1
   TH2F *hparticle_pi_eloss_vs_rod_sys1;                  //! pion eloss vs rod sys1
   TH2F *hparticle_p_eloss_vs_rod_sys1;                  //! slow proton eloss vs rod sys1
   TH2F *hparticle_pi_metahit_vs_phi_sys1;                  //! pion rod vs track phi - sys1
   TH2F *hparticle_pi_tof_vs_startstrip_sys0;           //! pi timediff in system0 vs strip for start
   TH2F *hparticle_pi_tof_vs_startstrip_sys1;           //! pi timediff in system1 vs strip for start
   TH1F *hparticle_pi_tof_all_sys0;                     //! pi timediff in system 0 for all start strips
   TH1F *hparticle_pi_tof_all_sys1;                     //! pi timediff in system 1 for all start strips
   TH1F *hparticle_pi_tof_vs_start_sys0[NSTART_STRIPS];        //! pi timediff for in system0 each start strip
   TH1F *hparticle_pi_tof_vs_startMod0_sys0[NSTART_STRIPS];        //! pi timediff for in system0 each start strip Mod0
   TH1F *hparticle_pi_tof_vs_startMod1_sys0[NSTART_STRIPS];        //! pi timediff for in system0 each start strip Mod1
   TH1F *hparticle_pi_tof_vs_start_sys1[NSTART_STRIPS];        //! pi timediff for in system1 each start strip for start
   TF1 *hparticle_pi_tof_GaussFit_all_sys0;        // Gauss fit for Pion Tof
   TF1 *hparticle_pi_tof_GaussFit_all_sys1;        // Gauss fit for Pion Tof
   TF1 *hparticle_pi_tof_GaussFit_sys0[NSTART_STRIPS];         // Gauss fit for Pion Tof
   TF1 *hparticle_pi_tof_GaussFit_sys1[NSTART_STRIPS];         // Gauss fit for Pion Tof


   TH1F *hparticle_RK_theta_sec[6];            //! theta distr. for all tracks RK
   TH1F *hparticle_RK_neg_theta_sec[6];        //! theta distr. for neg. tracks and mom > 300 RK
   TH1F *hparticle_RK_phi;                     //! phi distr. for all tracks RK
   TH1F *hparticle_RK_neg_phi;                 //! phi distr. for neg. tracks and mom > 300 RK
   TH2F *hparticle_rk_momdif_sys0_sec[6];      //! runge-kutta, sya0
   TH2F *hparticle_rk_momdif_sys1_sec[6];      //! runge-kutta, sys1

   TH1F *shorpcXdiff_pre[6];   //! Difference between X SHOWER Pre hit position and RPC hit position
   TH1F *shorpcXdiff_post1[6];   //! Difference between X SHOWER Post I hit position and RPC hit position
   TH1F *shorpcXdiff_post2[6];   //! Difference between X SHOWER Post II hit position and RPC hit position
   TH1F *shorpcYdiff_pre[6];     //! Difference between Y SHOWER Pre hit position and RPC hit position
   TH1F *shorpcYdiff_post1[6];   //! Difference between Y SHOWER Post I hit position and RPC hit position
   TH1F *shorpcYdiff_post2[6];   //! Difference between Y SHOWER Post II hit position and RPC hit position
   TH2F *shorpcXvs_pre[6];       //! X SHOWER Pre hit position vs RPC hit position
   TH2F *shorpcXvs_post1[6];     //! X SHOWER Post I hit position vs RPC hit position
   TH2F *shorpcXvs_post2[6];     //! X SHOWER Post II hit position vs RPC hit position
   TH2F *shorpcYvs_pre[6];       //! Y SHOWER Pre hit position vs RPC hit position
   TH2F *shorpcYvs_post1[6];     //! Y SHOWER Post I hit position vs RPC hit position
   TH2F *shorpcYvs_post2[6];     //! Y SHOWER Post II hit position vs RPC hit position

   TH1F *hparticle_MetaMatchQualTof[6]; //! HParticleCand: MetaMatchQuality in Tof
   TH1F *hparticle_MetaMatchQualRpc[6]; //! HParticleCand: MetaMatchQuality in Rpc
   TH1F *hparticle_MetaMatchQualShw[6]; //! HParticleCand: MetaMatchQuality in Shower

   TH1F *hparticle_mult[6][2];          //! HParticleCand: Multiplicity for each Sector(6) and System==0 or System==1 (2)
   TH1F *hparticle_mult_select[6][2];   //! HParticleCand: Multiplicity for each Sector(6) and System==0 or System==1 (2) for accepted particles
   
   TH1F *hparticle_mass;
   TF1 *hparticle_mass_GaussFit;  //fit protons
   TF1 *hparticle_mass_GaussFit_1; //fit pi+
   TF1 *hparticle_mass_GaussFit_2; //fit pi-
   
   TH1F *hparticle_mass_RPC;
   TF1 *hparticle_mass_RPC_GaussFit;  //fit protons
   TF1 *hparticle_mass_RPC_GaussFit_1; //fit pi+
   TF1 *hparticle_mass_RPC_GaussFit_2; //fit pi-

   TH1F *hparticle_mass_TOF;
   TF1 *hparticle_mass_TOF_GaussFit;  //fit protons
   TF1 *hparticle_mass_TOF_GaussFit_1; //fit pi+
   TF1 *hparticle_mass_TOF_GaussFit_2; //fit pi-

   TH1F* hparticle_TofdEdx;     //dEdx of Tof detector

   //#############################################################

   ClassDef(HQAHistograms, 1) // QA general histograms

};


#endif
