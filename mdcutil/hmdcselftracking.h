#ifndef HMDCSELFTRACKING_H
#define HMDCSELFTRACKING_H

#include "hreconstructor.h"

class TDirectory;
class TFile;
class HCategory;
class HIterator;
class HEventHeader;
class TH1F;
class TH1D;
class TH2S;
class TH3F;
class TProfile;

#define HMDCSELFTRACKING_CUTBRIDGE_NUMBER 20
#define HMDCSELFTRACKING_STARTSTRIP_NUMBER 9

class HMdcSelfTracking : public HReconstructor {
public:

protected:
  
  // some local declarations:
  
  // *** cuts ***
  
  
     enum selftrackingCut  {
	                    NO_CUT = 0,
			    TOF_BIT_CUT = 1,
			    START_CUT = 2,
			    TIME1_CUT = 3,
			    TIME2_CUT = 4,
			    TIME_ABOVE_THRESHOLD_CUT = 5,
			    MISC_6  =  6,
			    MISC_7  =  7,
			    MISC_8  =  8,
			    MISC_9  =  9,
			    MISC_10 = 10,
			    MISC_11 = 11,
			    MISC_12 = 12,
			    MISC_13 = 13,
			    MISC_14 = 14,
			    MISC_15 = 15,
			    MISC_16 = 16,
			    MISC_17 = 17,
			    MISC_18 = 18,
			    MISC_19 = 19,
			    MISC_20 = 20};
  
  enum flags  {NEIGHBOURS                 = 0,
               LEFT_CELL_HIT              = 1,
               RIGHT_CELL_HIT             = 2,
               ANALYSISLAYER2_HITS        = 1,
               ANALYSISLAYER2_DOUBLE_HITS = 2,
               RESIDUAL_CUT               = 3 };

  enum selftrackingStartStrip  {START_STRIP_1 = 1,
				START_STRIP_2 = 2,
				START_STRIP_3 = 3,
				START_STRIP_4 = 4,
				START_STRIP_5 = 5,
				START_STRIP_6 = 6,
				START_STRIP_7 = 7,
				START_STRIP_8 = 8 };

  enum rightleft {LEFT  = 1,
                  RIGHT = 2};
  Int_t setLeft[4];  // define conditions for LEFT/RIGHT correlations for different planes (I,II,III,IV)
  Int_t setRight[4]; // same
  
  Float_t A[4],B[4],C[4]; // Correlation fct parameters
  Float_t CorrWindow[4][3]; // Residual cut

  Float_t tdifmin[5]; // lower limit for cut in time-above-threshold (time1-time2)
  Float_t tdifmax[5]; // upper limit for cut in time-above-threshold (time1-time2)
  Float_t tdifcut; // cut for time1(layer3)-time1(layer4)
  Float_t tmin; // lower limit for cut in time
  Float_t tmax; // upper limit for cut in time
  Int_t cellmin; // lower limit for the cells to be used in a further analysis
  Int_t cellmax; // upper limit for the cells to be used in a further analysis
  Float_t startmin[HMDCSELFTRACKING_STARTSTRIP_NUMBER]; // array of lower limits of start time window over all start detector strips
  Float_t startmax[HMDCSELFTRACKING_STARTSTRIP_NUMBER]; // array of upper limits of start time window over all start detector strips
  Bool_t cutbridge[HMDCSELFTRACKING_CUTBRIDGE_NUMBER]; // array of cut bridges to switch of cuts 
  Bool_t cut[20]; // internal
  Int_t monitorCell;  // cell number which is taken into account for further analysis
  Int_t monitorLayer; // cell layer which is taken into account for further analysis
  // *** end cuts ***
  
  Float_t hitTime1[7][200]; // array containing time 1 per layer which fulfill the cut conditions
  Float_t hitTime2[7][200]; // array containing time 2 per layer which fulfill the cut conditions
  Float_t time1;
  Float_t time2;
  Float_t timesum;
  Float_t timediff;
  Float_t timeDiffMax;

  Int_t  setmodule, setsector;   // variables for setActiveModule(sec,mod) start counting from 1
  
  Bool_t noStart;                        // flag if starttime should not be used
  
  Bool_t noCorrSector;   // flag if CorrSector 3dim histogram should not be filled
  Bool_t noCheckCorrSector;   // flag if CorrSector is not checked

  Int_t module, sector, cell, layer;
  Int_t hitCells[7][200];              // array containing the cellnumbers per layer which fulfill the cut conditions
  Int_t nhit[7];                // hits per layer

  Int_t cell0[5][200]; // hits in 0-deg-layers, all chambers of a given sector
  Int_t ncell0[5];
  
  Bool_t relevant_data; //Boolean flag to indicate that data arrays had been filled, triggers correlation analysis
  Bool_t flag[4];  // Boolean array to trigger on flags (NEIGHBOURS = 0, LEFT_CELL_HIT = 1, RIGHT_CELL_HIT = 2, RESIDUAL_CUT = 3)
  Int_t alreadyUsedCellFlag_AnalysisLayer1[200];
  Int_t alreadyUsedCellFlag_AnalysisLayer2[200];
 // Int_t ihit;
  Int_t lhit[3];
  
  Int_t analysisLayer1;
  Int_t analysisLayer2;
  Int_t cntHitslayer1;
  Int_t cntHitslayer2;
  Int_t cntHitsWithoutNeighbours;
  Int_t cntHitsAnalysislayer2;
  Int_t cntDoubleHitsAnalysislayer2;
  Int_t eventCounter;
  
  Float_t counter[3][160];
  Double_t ctrl[11];
  
  Int_t start_strip, nstartstrip;
  Float_t start_time;
  
  HCategory     *calCatMdc;           // pointer to the MDC-CAL1 data
  HCategory     *calCatStart;         // pointer to the START-CAL1 data
  HIterator     *iterMdc;             // iterator on MDC raw data
  HIterator     *iterStart;           // iterator on START raw data
  
  HEventHeader  *evheader;            // Hades event header
  
  Char_t *fNameRoot;                  // file name of root output file
  
  // pointer to histos:
  
  TH1F *pinfo; // scaler info
  TH1D *player[7]; // hit pattern layer 1-6
  TH1D *pNhitlayer[7]; // hits per layer 1-6
  TH1D *player0[3]; // selftracking: hit pattern +/-0deg layers
  TH1F *peff; // efficiency layer 4 relative to layer 3
  
  TH1D *pTime_cut[7]; // drift time spectra for different cut levels
  TH2S *pTime0_cell[7]; // cell# vs. drifttime per layer without cuts
  TH2S *pTime0_diff; // time above threshold vs. drifttime
  TH2S *pTime_cell[7];  // cell# vs. drifttime after cuts
  TH1D *pTime_diff[2]; // time above threshold, w/[0]. w/o[1] threshold
  TH2S *pTime0diff_cell[3]; // cell# vs. time above threshold (0deg layers)
  TH2S *pTimesum_adj_cell3; // cell# vs. time sum of adjacent cells, layer 3
  TH2S *pTimesum_adj_cell4; // cell# vs. time sum of adj. cells, layer 4, cut on time diff
  TH2S *pTimesum_cell[3]; // selftracking: cell# vs. time sum of correl. cells
  TH2S *pFish[3][120];    // selftracking: time diff vs. sum of correl. cells
  TH2S *pTsum_ev[3][120]; //  selftracking: time sum vs. event#
  TProfile *pTsum_ev_prof[3][120]; //  selftracking: time sum vs. event#, profile
  
  TH2S *pStart_time_strip; // start detector: strip# vs. time
  TH1F *pStart_time[9]; // start detector: time, incl. cut1 and strip-mult=1
  TH1F *pStart_mult; // start detector:strip multiplicity
  
  TH3F *pCorrSector; // Correlations in a given sector
  TH1F *pCorrDiff[2][4]; // residuals from correlation in one sector
  
  TFile *fout; // output file pointer
  
 public:
  HMdcSelfTracking();
  HMdcSelfTracking(const Text_t* name,const Text_t* title);
  ~HMdcSelfTracking();
  Bool_t init(void);
  Bool_t finalize(void);
  Int_t execute();
  
  void setOutputRoot (const Char_t*);
  void setActiveModule (Int_t sec,Int_t mod){setsector=sec;setmodule=mod;} // only this module will be analyzed
  void setNoStart() {noStart=kTRUE;}
  void setNoCorrSector() {noCorrSector=kTRUE;}
  void setNoCheckCorrSector() {noCheckCorrSector=kTRUE;}
  
  Bool_t setCutbridge(UInt_t, Bool_t);
  Bool_t getCutbridge(UInt_t index);
  void clearCutbridges(void){for (Int_t i=0; i < HMDCSELFTRACKING_CUTBRIDGE_NUMBER; i++) setCutbridge(i,kFALSE);}// set all cutbridges to 0
  void printCutbridge(void);
  
  Bool_t setStartTimeCutWindow(UInt_t, Float_t, Float_t);
  Bool_t setStartTimeCutMax(UInt_t, Float_t);
  Bool_t setStartTimeCutMin(UInt_t, Float_t);
  Float_t getStartTimeCutMax(UInt_t);
  Float_t getStartTimeCutMin(UInt_t);
  void printStartTimeCutWindow(void);
  
  void setTimeCutWindow(Float_t min, Float_t max){setTimeCutMin(min);setTimeCutMax(max);return;} // values in ns
  void setTimeCutMax(Float_t f){tmax = f; return;} // values in ns
  void setTimeCutMin(Float_t f){tmin = f; return;} // values in ns
  Float_t getTimeCutMax(){return tmax;} // values in ns
  Float_t getTimeCutMin(){return tmin;} // values in ns
  void printTimeCutWindow(void);
  
/*   void setTimeAboveThresholdCutWindow(Float_t a, Float_t b){setTimeAboveThresholdCutMin(a);setTimeAboveThresholdCutMax(b);return;} // values in ns */
/*   void setTimeAboveThresholdCutMax(Float_t f){tdifmax = f; return;} // values in ns */
/*   void setTimeAboveThresholdCutMin(Float_t f){tdifmin = f; return;} // values in ns */
/*   Float_t getTimeAboveThresholdCutMax(){return tdifmax;} // values in ns */
/*   Float_t getTimeAboveThresholdCutMin(){return tdifmin;} // values in ns */
/*   void printTimeAboveThresholdCutWindow(void); */

  void setTimeAboveThresholdCutWindow(Int_t i, Float_t a, Float_t b){setTimeAboveThresholdCutMin(i,a);setTimeAboveThresholdCutMax(i,b);return;} // values in ns
  void setTimeAboveThresholdCutMax(Int_t i, Float_t f){tdifmax[i] = f; return;} // values in ns
  void setTimeAboveThresholdCutMin(Int_t i, Float_t f){tdifmin[i] = f; return;} // values in ns
  Float_t getTimeAboveThresholdCutMax(Int_t i){return tdifmax[i];} // values in ns
  Float_t getTimeAboveThresholdCutMin(Int_t i){return tdifmin[i];} // values in ns
  void printTimeAboveThresholdCutWindow(void);

  void setCorrelationFct(Int_t i, Float_t a, Float_t b, Float_t c){setCorrelationFctA(i,a);setCorrelationFctB(i,b);setCorrelationFctC(i,c);return;}
  void setCorrelationFctA(Int_t i, Float_t f){A[i] = f; return;} 
  void setCorrelationFctB(Int_t i, Float_t f){B[i] = f; return;} 
  void setCorrelationFctC(Int_t i, Float_t f){C[i] = f; return;} 
  Float_t getCorrelationFctA(Int_t i){return A[i];} 
  Float_t getCorrelationFctB(Int_t i){return B[i];} 
  Float_t getCorrelationFctC(Int_t i){return C[i];} 
  void printCorrelationFct(void);

  void setCorrelationWindow(Int_t i, Float_t a, Float_t b){setCorrelationWmin(i,a);setCorrelationWmax(i,b);return;}
  void setCorrelationWmin(Int_t i, Float_t f){CorrWindow[i][1] = f; return;} 
  void setCorrelationWmax(Int_t i, Float_t f){CorrWindow[i][2] = f; return;} 
  Float_t getCorrelationWmin(Int_t i){return CorrWindow[i][1];} 
  Float_t getCorrelationWmax(Int_t i){return CorrWindow[i][2];} 
  void printCorrelationWindow(void);
 
  void setAnalysisCellCutWindow(UInt_t a, UInt_t b){setAnalysisCellCutMin(a);setAnalysisCellCutMax(b);return;}
  void setAnalysisCellCutMax(UInt_t f){cellmax = f; return;}
  void setAnalysisCellCutMin(UInt_t f){cellmin = f; return;}
  UInt_t getAnalysisCellCutMax(){return cellmax;}
  UInt_t getAnalysisCellCutMin(){return cellmin;}
  void printAnalysisCellCutWindow(void);
  
  void setTimeDiffL3L4Cut(Float_t f){tdifcut = f; return;} // values in ns
  Float_t getTimeDiffL3L4Cut(){return tdifcut;} // values in ns
  void printTimeDiffL3L4Cut();

  void  setMonitor(UInt_t layer, UInt_t cellNumber){setMonitorCell(cellNumber);setMonitorLayer(layer);return;}
  void  setMonitorCell(UInt_t cell){monitorCell=(Int_t)cell;return;}
  Int_t getMonitorCell(){return monitorCell;}
  void  setMonitorLayer(UInt_t layer){monitorLayer=(Int_t)layer;return;}
  Int_t getMonitorLayer(){return monitorLayer;}

  void setTimeDiffMax(Float_t time){timeDiffMax = time;return;}
  Float_t getTimeDiffMax(){return timeDiffMax;}

protected:
  void createHist();
  void resetCounters();
  void fillControlHists();
  void fillHist  ();
  void fillAnalysisHists(Int_t);
  void checkAdjacentCellAnalysisLayer1(Int_t, Int_t);
  void checkAdjacentCellAnalysisLayer2(Int_t, Int_t);
  void findCorrelatedHits(Int_t);
  void checkCorrSector(Int_t, Int_t);
  void writeHist ();
  void setDefault ();
  void executeStart();
  void executeMdc();
  void executeCorrSector();
  void executeEventHeaderCheck();
  void executeReset();
  
 public: // This has to be placed at the end (--> root dox)
  ClassDef(HMdcSelfTracking, 0)  // class for the MDC 0deg-layer's self tracking
    };
    
#endif

