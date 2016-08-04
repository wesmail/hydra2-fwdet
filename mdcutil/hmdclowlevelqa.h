#ifndef HMDCLOWLEVELQA_H
#define HMDCLOWLEVELQA_H

//#include "hldfilesource.h" //to get file name
#include "hreconstructor.h"
#include "hlocation.h"
#include "TRandom.h"

class HCategory;
class HIterator;
class HMdcLookupGeom;
class HMdcLookupRaw;
class HMdcTimeCut;
class HMdcCalParRaw;
class TH2F;
class TH1F;
class TDirectory;


class HMdcLowLevelQA : public HReconstructor {
protected:
  //HldFileSource hldFileCurrent;

  //########################### ANALYSIS CAL1 #############################################

  //Summary samples of mean time1
  TH1F *hmdc_time1_everithing_summ[6][4];  //Regardless of any time cuts summary for the file
  TH1F *hmdc_time1_good_t1_summ[6][4];     //All cuts are OK, summary for the file
  TH1F *hmdc_time1_not_t1_summ[6][4];      //Time1 cut is not done, summary for the file

  // Trend mean of time1
  TH1F *hmdc_trend_meantime1[6][4];        // PASS        //trend of mean of time1 per sector and module
  TH1F *hmdc_time1[6][4];                  // all t-cuts  //time1 per sector and module

  TH1F *hmdc_trend_meantime1not_t1[6][4];  // DOES NOT    //trend of mean of time1 per sector and module
  TH1F *hmdc_time1not_t1[6][4];            // PASS t1-cut //time1 per sector and module
  TH1F *hmdc_trend_meantime1not_t2[6][4];  // DOES NOT    //trend of mean of time1 per sector and module
  TH1F *hmdc_time1not_t2[6][4];            // PASS t2-cut //time1 per sector and module
  TH1F *hmdc_trend_meantime1not_t12[6][4]; // DOES NOT    //trend of mean of time1 per sector and module
  TH1F *hmdc_time1not_t12[6][4];           // PASS t12cut //time1 per sector and module

  TH1F *hmdc_trend_meantime1_plane[4];  // trend of mean of time1 per plane
  TH1F *hmdc_time1_plane[4];            // time1 per plane
  TH1F *hmdc_trend_meanSlope[6][4];     //slope of CAL1 as a function of file
  TH1F *hmdc_Slope[6][4];               //technical histogramm for calculating slopes mean value for trend histogramm
  
  // Neighboring cells
  TH1F *hmdc_trend_neighbors[6][4];// trend of ratio single to neighboring cells per sector and module
  Int_t previousCellStatus;  // buffer for cell number of previous hit
  Int_t double_all[6][4][2]; // keeps conter for cells with neighbors and all cells

  // Ratio of (events with 1,2,3 fired wires)/(total events number) per module for all planes
  TH1F *hmdc_trend_123ratio[6][4];      //for 6 modules in 4 planes
  Int_t cnt123[6][4];                   //number of "noise" wires (we assume this to be a noise if only 1 or 2 or 3 wires have fired in a module)
  Int_t sect_module_NumberOfHits[6][4]; //number of hits per module=(sector,plane)
  Int_t sm_old[2];                      //sector => sm_old[0], module =>  sm_old[1]

  // Hits Multiplicity per module (average above N-events)
  TH1F *hmdc_multiplicity[6][4];       //hits multiplicity event by event storage
  TH1F *hmdc_trend_multiplicity[6][4]; //mean value over N-step events of multiplicity

  // Summary: MBO vs. TDCchannel (once per file, but for each module)
  TH1F *a_thisFileName;
  TH2F *hmdc_summ_MBOvsTDC[   6][4];
  TH1F *hmdc_summ_dNdNhits[   6][4];
  TH2F *hmdc_summ_t2mt1_vs_t1[6][4];
  TH1F *hmdc_summ_tCuts_info[    4]; //histogram contains 6 bins for each time cut
  TH1F *hmdc_summ_dNdWire[    6][4][6][3]; //sector//plane//layer//histogramm_kind_(four different histograms: for different time cut conditions)
  
  
  TH1F *hmdc_summ_trend_meantime1_plane;
  TH1F *hmdc_summ_trend_meantime1;
  TH1F *hmdc_summ_trend_meantime1not_t1;
  TH1F *hmdc_summ_trend_meantime1not_t2;
  TH1F *hmdc_summ_trend_meantime1not_t12;
  TH1F *hmdc_summ_trend_neighbors;
  TH1F *hmdc_summ_trend_123ratio;
  TH1F *hmdc_summ_trend_multiplicity;



  //########################### ANALYSIS RAW ##############################################
  TH1F *hmdc_trend_NhitsPERmbo[6][4][16]; //for 6 modules in 4 planes
  Int_t NhitsPERmbo[6][4][16];            //for 6 modules in 4 planes
  Int_t smmb_old[3];                      //[sector,module,mbo] combination
  


  //########################### COMMON VARIABLES ##########################################

  Char_t *fNameRootHists;  // file name of root output file
  // color codes
  static Int_t colorsSector[6]; // colors for hists to be compared in one plane
  static Int_t colorsModule[4]; // colors for hists to be compared in one sector
  static Int_t colorsCuts[3];   // colors for hists to be compared for cuts

  // Define counters
  Int_t Eventcounter; // counts the number of events
  Int_t step_trend_meantime1_counter; // counts the number of bin to be filled in trend hists
  Int_t stepsize_trend_meantime1;     // defines the number of events to be filled for one data point
  Int_t numberOfSectorsPerModule[4];  // stores number of chambers per plane
  Int_t mySetup[6][4];                // actual setup of the detector
  static Int_t cutResults[4];         // contains 0/1 for cuts in t1,t2 and t2-1 after testTimeCuts()
  Bool_t isInitialized;               // flag for initialization of histograms
  Int_t nReqEvents;                   // nuber of Events from Hades::EventLoop
  Int_t nBinsTrendX;                  // max number of bins for trend histograms
  // pointer to categories and iterators
  HCategory* calCat;        //! pointer to the cal data
  HCategory* rawCat;        //! pointer to the raw data

  HLocation loccal1;        //! location for new object.
  HLocation locraw;         //! location for new object.

  HIterator* itercal1;      //! iterator on cal1 data.
  HIterator* iterraw;       //! iterator on raw data.

  // pointer to used parameter containers
  HMdcLookupGeom* lookup;   //! lookup table for mapping
  HMdcLookupRaw*  lookupRaw;//! lookup table for mapping
  HMdcTimeCut*    timecut;  //! container for time cuts
  
  //HMdcCalParRaw* calparraw; //! container for CalParRaw if it exists..?
  
public:
  HMdcLowLevelQA(void);
  HMdcLowLevelQA(const Text_t* name,const Text_t* title);
  ~HMdcLowLevelQA(void);
  void setOutputRoot (const Char_t*);
  void initVariables();
  void printStatus();
  void resetCounters()
  {
      previousCellStatus=0;
      for(Int_t s=0;s<6;s++)
	{
	    for(Int_t m=0;m<4;m++)
	    {
              double_all[s][m][0]=0;
              double_all[s][m][1]=0;
	    }
	}
  }
  void checkNeighboringCells(Int_t s,Int_t m,Int_t c)
  {
      if(c==previousCellStatus+1)
      {   // if neighbor
	  double_all[s][m][0]++;
	  double_all[s][m][1]++;
      }
      else
      {   // if no neighbor
	  double_all[s][m][1]++;
      }
      previousCellStatus=c;
  }
  void setStepSize(Int_t size){stepsize_trend_meantime1=size;}
  void createHistograms();
  Bool_t getCategories();
  void writeHistograms(Int_t,Int_t,Int_t,Int_t,Int_t,Int_t);
  void fillHistogramsCal(Int_t,Int_t,Int_t,Int_t,Float_t,Float_t);
  void fillHistogramsRaw(Int_t,Int_t,Int_t,Int_t,Float_t,Float_t);
  void lookupSlopes(Int_t s,Int_t m);
  void testTimeCuts(Int_t s,Int_t m,Float_t t1,Float_t t2);
  void writeTimeCuts();
  void getSetup();
  Bool_t init(void);
  Bool_t reinit(void);
  TDirectory *Mkdir(TDirectory *,const Char_t *, Int_t, Int_t p=1);
  void setParContainers(void);
  Bool_t finalize();
  Int_t execute();
  ClassDef(HMdcLowLevelQA,0) // fills different hists for cal1 data
};

#endif /* !HMDCLOWLEVELQA_H */
