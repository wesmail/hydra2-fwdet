#ifndef HMDCTASKSET_H
#define HMDCTASKSET_H

#include "htaskset.h"
#include "hmdcdigitizer.h"

#include <cstdlib>

class HMdcDigitizer;
class HMdcCalibrater1;
class HMdcDeDx2Maker;
class HMdcSetup;
class HMdcCalibrater1Setup;
class HMdcDigitizerSetup;
class HMdcTrackFinderSetup;
class HMdcClusterToHitSetup;
class HMdc12FitSetup;
class HMdcCommonSetup;
class HMdcVertexFind;

class HMdcTaskSet : public HTaskSet {

private:

static Int_t ilevel;            // counter for arguments of analysis levels
static Int_t irealorsimulation; // counter for arguments of real/simulation
static Int_t icalibrater;       // counter for arguments of calibrater mode
static Int_t itimecuts;         // counter for arguments of calibrater time cuts
static Int_t itracker;          // counter for arguments of tracker type
static Int_t imagnet;           // counter for arguments of magnet status
static Int_t itrackermode;      // counter for arguments of tracker mode (DUBNA)

static Int_t analysisLevel;   // 0=raw,1=cal1,2=cal2,3=hit,4=fit
static Int_t cal1TimeCutFlag; // 0=nocuts, 1=cuts
static Int_t cal1ModeFlag;    // 1=nostartandcal,2=startandcal,3=nostartandnocal

static Int_t tracker;         // 1=dubna,2=santiago
static Bool_t isCoilOff;      // kTRUE=magnetoff,kFALSE=magneton
static Int_t typeClustFinder; // 0=combinedchamberclusters,1=singlechamberclusters
static Int_t nLayers[6][4];   // number of layers per module for trackfinder
static Int_t nLevel[4];       // levels for hit finding in seg1 and seg2
static Int_t mdcForSeg1;      // -1=don't fill HMdcSeg for segment 1,
                              //  0 and typeClustFinder=1 and sector has mdc1&2
                              //   =fill HMdcSeg by cluster in mdc1
                              //  1 -/- in mdc2
static Int_t mdcForSeg2;      // -1=don't fill HMdcSeg for segment 1,
                              //  2 and typeClustFinder=1 and sector has
                              //    mdc3&4  and isCoilOff=kTRUE,
                              //   =fill HMdcSeg by cluster in mdc4
                              //  3 -/- in mdc4
static Int_t fitAuthor;       // DUBNA:0=Hedar, 1=Alexandr
static Bool_t fitNTuple;      // DUBNA:kTRUE=fill NTuple,kFALSE=fill nothing
static Bool_t fitPrint;       // DUBNA:kTRUE=print debug,kFALSE=print nothing
static Int_t fitVersion;      // DUBNA:1=old,2=new
static Int_t fitIndep;        // DUBNA: 0=MDC fit, 1=segment fit
static Int_t isSimulation;   // 1=sim.0=real,2=merge

static HMdcDigitizer*         mdcdigi;
static HMdcCalibrater1*       calibrater1;
static HMdcDeDx2Maker*        dedx2maker;  //!
Int_t  versionDeDx;                        //!  0: no dedx, 1=dedx

Bool_t idealTracking;          // kTRUE - ideal tracking
Bool_t fillParallel;           // kTRUE will fill HMdcHitSim,HMdcSegSim and HMdcTrkCand to ideal categories

static HMdcSetup*             mdcsetup;
static HMdcSetup*             mdcsetuplocal;
static HMdcCalibrater1Setup*  calibrater1setlocal;
static HMdcDigitizerSetup*    digisetlocal;
static HMdcTrackFinderSetup*  trackfindersetlocal;
static HMdcClusterToHitSetup* clustertohitsetlocal;
static HMdc12FitSetup*        fittersetlocal;
static HMdcCommonSetup*       commonsetlocal;
static HMdcVertexFind*        vertexfinder;
static Bool_t isCreated;
static Int_t rtdbinput;
  void getSetupContainer();
  void copySetup();
  void getSets();
  void setDefaults();
  void parseArguments(TString);
public:
  HMdcTaskSet(void);
  HMdcTaskSet(const Text_t name[],const Text_t title[]);
  ~HMdcTaskSet(void);
  HTask *make(const Char_t *select="",const Option_t *option="");
  HMdcDigitizer*   getDigitizer();
  HMdcCalibrater1* getCalibrater1();
  HMdcDeDx2Maker*  getDeDx2Maker();
  HMdcSetup*       getMdcSetup();
  HMdcVertexFind*  getMdcVertexFinder();
  void setVersionDeDx(Int_t vers){ if(vers >= 0 && vers < 2){ versionDeDx = vers;} else { Error("setVersionDeDx()","Unknown option!"); exit(1);} }
  void printStatus();
  void printOptions();
  void useIdealTracking(Bool_t fillPar=kFALSE) {idealTracking = kTRUE; fillParallel=fillPar;}

  ClassDef(HMdcTaskSet,1) // Set of tasks
};

#endif /* !HMDCTASKSET_H */
