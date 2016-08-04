//*-- AUTHOR : C.Muentz

//*-- Modified : 02/01/01 by P.Zumbruch,J.Markert
//*-- Modified : 28/12/00 by P.Zumbruch
//*-- Modified : 19/4/00 by CM, add start detector
//*-- Modified : 11/4/00 by CM
//*-- Modified : 7/4/00 by CM
//*-- Modified : 10/8/00 by CM, get rid of prof histos

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcSelfTracking
//
// Begin_Html
// purpose: analyse CAL1 data of individual chambers, focussing on 0deg-layer's self
//          correlation and fluctuations (as fct of event number).
// input: hades listmode files (*.hld)
// needed: calibration parameters (gain, offset)
// End_Html
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdcselftracking.h"

#include <stdlib.h>
#include <fstream> 
#include <iomanip> 
#include <iostream> 
#include <stdio.h>

#include "TClass.h"
#include "TFile.h"
#include "TH1.h"
#include "TH2.h" 
#include "TH3.h" 
#include "TProfile.h" 
#include "hades.h"
#include "hmdcdetector.h"
#include "hmdccal1.h"
#include "hstart2detector.h"
#include "hstart2cal.h"
#include "hrecevent.h"
#include "htree.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hmatrixcategory.h"
#include "hpartialevent.h"
#include "htaskset.h"
#include "hiterator.h"
#include "hcategory.h"
#include "hmdcdef.h"
#include "hevent.h"

ClassImp(HMdcSelfTracking)

HMdcSelfTracking::HMdcSelfTracking(void)
{
    // Default Constructor

    fout   = NULL;
    calCatMdc = NULL;
    calCatStart = NULL;
    iterMdc   = NULL;
    iterStart   = NULL;

    // some local declarations:
    setDefault();
  
}

HMdcSelfTracking::HMdcSelfTracking(const Text_t* name,const Text_t* title) : HReconstructor(name,title)
{
    //

    fout   = NULL;
    calCatMdc = NULL;
    calCatStart = NULL;
    iterMdc   = NULL;
    iterStart   = NULL;

    // some local declarations:
    setDefault();
}

HMdcSelfTracking::~HMdcSelfTracking(void)
{
    // Destructor.

    if (iterMdc)       delete iterMdc;
    if (iterStart)       delete iterStart;
    if (fNameRoot)  delete fNameRoot;
    //if(fout) delete fout;
    iterMdc = NULL;
    iterStart = NULL;
}

void HMdcSelfTracking::setDefault()
{
    // Sets default values (especially for the cuts) for the parameters

    // *** cuts ***
    clearCutbridges();

    // setCutbridge(TOF_BIT_CUT,kTRUE);
    // setCutbridge(START_CUT],kTRUE);
    // setCutbridge(TIME1_CUT,kTRUE);
    // setCutbridge(TIME2_CUT,kTRUE);
    // setCutbridge(TIME_ABOVE_THRESHOLD_CUT,kTRUE);
    setStartTimeCutWindow(START_STRIP_1,37.2,38.2);
    setStartTimeCutWindow(START_STRIP_2,36.3,37.5);
    setStartTimeCutWindow(START_STRIP_3,37.3,38.5);
    setStartTimeCutWindow(START_STRIP_4,36.5,37.7);
    setTimeCutWindow(0,300);       // cut in time1, time2 (min)
    setTimeAboveThresholdCutWindow(1,10,400); // cut in time above threshold
    setTimeAboveThresholdCutWindow(2,11,400); // cut in time above threshold
    setTimeAboveThresholdCutWindow(3,27,400); // cut in time above threshold
    setTimeAboveThresholdCutWindow(4,10,400); // cut in time above threshold

    setCorrelationFct(1,0,1,0); // default correlation fct parameters
    setCorrelationFct(2,0,1,0); // default correlation fct parameters
    setCorrelationFct(3,0,1,0); // default correlation fct parameters

    // other cuts:
    setTimeDiffL3L4Cut(20); // cut for time1(layer3)-time1(layer4) to project on time-sum
    setAnalysisCellCutWindow(96,116);  // cells to be used for further analysis
    // *** end cuts ***

    setMonitor(3,106); // sets the monitor cell to layer 3 cell number 106
    setTimeDiffMax(10); // used as cut for filling pTimesum_adj_cell4 histogramm

    analysisLayer1=3;
    analysisLayer2=4;

    cntHitslayer1=0;
    cntHitslayer2=0;
    cntHitsWithoutNeighbours=0;
    cntHitsAnalysislayer2=0;
    cntDoubleHitsAnalysislayer2=0;

    eventCounter=0;
    setsector=1; // initial value
    setmodule=1; // initial value
    noStart=kFALSE;
    noCorrSector=kFALSE;
    noCheckCorrSector=kFALSE;
    relevant_data=kFALSE;

    setLeft[0]=0; // define left/right correl.
    setLeft[1]=1;
    setLeft[2]=1;
    setLeft[3]=-1; //-1
    setRight[0]=-1; 
    setRight[1]=0;
    setRight[2]=0;
    setRight[3]=-2;//-2

    return;
}

void HMdcSelfTracking::setOutputRoot(const Char_t *c)
{
  // Sets rootfile output of HMdcSelfTracking where all
  // created histograms were written.
    
    if (fNameRoot) delete fNameRoot;
    fNameRoot = new Char_t[strlen(c)+1];
    strcpy(fNameRoot, c);
}

Bool_t HMdcSelfTracking::setCutbridge(UInt_t index,  Bool_t b)
{
  // Begin_Html Disables (bridges) the following cuts
  // index cut details End_Html
  // 1: TOF_BIT_CUT Tof-Bit set in Event Header? 
  // 2: START_CUT Start multiplicity = 1 and start times of the strips in the given ranges?
  // 3: TIME1_CUT Time1 in the given range?
  // 4: TIME2_CUT Time2 in the given range?
  // 5: TIME_ABOVE_THRESHOLD_CUT Time above threshold in the give range?
  // 6-19: not used

  if(index < HMDCSELFTRACKING_CUTBRIDGE_NUMBER) 
    {
      cutbridge[index]=b;
      return kTRUE;
    }
  return kFALSE;
}

Bool_t HMdcSelfTracking::getCutbridge(UInt_t index)
{
    // Begin_Html Returns the status of the cut bridge index, End_Html
    // description see HMdcSelfTracking::setCutbridge

  if(index < HMDCSELFTRACKING_CUTBRIDGE_NUMBER) 
    {
      return  cutbridge[index];
    }
  return kFALSE;
}

void HMdcSelfTracking::printCutbridge(void)
{
  // Begin_Html Prints out a table of all available cut bridges and their status
  // to the standard output End_Html

  Char_t cutname[HMDCSELFTRACKING_CUTBRIDGE_NUMBER][80]=
  {
    "-", 
      "TOF_BIT_CUT",
      "START_CUT",
      "TIME1_CUT",
      "TIME2_CUT",
      "TIME_ABOVE_THRESHOLD_CUT",
      "-",  "-",  "-",  "-",  "-",
      "-",  "-",  "-",  "-",  "-",  
      "-",  "-",  "-",  "-"};

  cout << endl;
  cout << "Cut bridges :" << endl;
  printf("index  bridged %-40s\n","Discription");
  printf("---------------------------------------------------------\n");
  for (Int_t index = 0 ; index < HMDCSELFTRACKING_CUTBRIDGE_NUMBER; index ++)
    {
      printf(" %2i    %5s    %-40s\n",
	     index, 
	     (getCutbridge(index))?"YES":"no",
	     cutname[index]);
    }
  cout << endl;
  return; 
}

Bool_t HMdcSelfTracking::setStartTimeCutWindow(UInt_t index, Float_t min, Float_t max)
{
  // Begin_Html sets the start time cut window for strip no. index from min to max (in ns)
  // if index is out of bounds kFALSE will be the default value End_Html

  return (setStartTimeCutMin(index,min) && setStartTimeCutMax(index,max));
}

Bool_t HMdcSelfTracking::setStartTimeCutMax(UInt_t index, Float_t max)
{

  // Begin_Html sets the upper limit (in ns) of the start time cut window for strip no. index
  // if index is out of bounds kFALSE will be the default value End_Html

  if(index < HMDCSELFTRACKING_STARTSTRIP_NUMBER)
    {
      startmax[index]=max;
      return kTRUE;
    }
  return kFALSE;
}

Bool_t HMdcSelfTracking::setStartTimeCutMin(UInt_t index, Float_t min)
{
  // Begin_Html sets the lower limit (in ns) of the start time cut window for strip no. index
  // if index is out of bounds kFALSE will be the default value  End_Html

  if(index < HMDCSELFTRACKING_STARTSTRIP_NUMBER)
    {
      startmin[index]=min;
      return kTRUE;
    }
  return kFALSE;
}

Float_t HMdcSelfTracking::getStartTimeCutMax(UInt_t index)
{
  // Begin_Html returns the upper limit (in ns) of the start time cut window of Start Strip no index
  // if index is out of bounds a default value of -999 is returned.  End_Html

  if(index < HMDCSELFTRACKING_STARTSTRIP_NUMBER)
    {
      return startmax[index];
    }
  return -999.;
}

Float_t HMdcSelfTracking::getStartTimeCutMin(UInt_t index)
{
  // Begin_Html returns the lower limit (in ns) of the start time cut window of Start Strip no index
  // if index is out of bounds a default value of -999 is returned. End_Html

  if(index < HMDCSELFTRACKING_STARTSTRIP_NUMBER)
    {
      return startmin[index];
    }
  return -999.;
}

void HMdcSelfTracking::printStartTimeCutWindow(void)
{
  // Begin_Html Prints out a table of all start time cut windows for all start detector strips  
  // to the standard output  End_Html
  
  cout << endl;
  cout << "Start Time Windows :" << endl;
  printf("strip\t\tmin\t\tmax \n");
  printf("-------------------------------------------------------\n");
  for (Int_t index = 1 ; index < HMDCSELFTRACKING_STARTSTRIP_NUMBER; index ++)
    {
      printf(" % 2i\t\t%03.1f \tns\t%03.1f \tns\n",
	     index, 
	     getStartTimeCutMin(index),
	     getStartTimeCutMax(index));
    }
  cout << endl;
  return; 
}
void HMdcSelfTracking::printCorrelationWindow(void)
{
  // Begin_Html Prints out residual cuts  
  // to the standard output  End_Html
  
  cout << endl;
  cout << "Correlation Cuts (Residuals) :" << endl;
  printf("Residual#\tmin\tmax \n");
  printf("-------------------------------------------------------\n");
  for (Int_t index = 1 ; index < 4; index ++)
    {
      printf(" % 2i\t\t%03.1f \t%03.1f \n",
	     index, 
	     getCorrelationWmin(index),
	     getCorrelationWmax(index));
    }
  cout << endl;
  return; 
}

void HMdcSelfTracking::printTimeCutWindow(void)
{
  // Begin_Html Prints out the time cut windows   
  // to the standard output End_Html

  cout << endl;
  cout << "Time Windows :" << endl;
  printf("min\t\tmax \n");
  printf("-------------------------------------------------------\n");
  printf("%#03.1f \tns\t%#03.1f ns\n",getTimeCutMin(),getTimeCutMax());
  cout << endl;
  return; 
}

void HMdcSelfTracking::printTimeAboveThresholdCutWindow(void)
{
  // Begin_Html Prints out the time above threshold cut windows   
  // to the standard output End_Html

  cout << endl;
  cout << "Time Above Threshold Windows :" << endl;
  printf("plane\tmin\t\tmax \n");
  printf("-------------------------------------------------------\n");
  for (Int_t i=1; i<5;i++){
    printf("%i \t%#03.1f \tns\t%#03.1f ns\n",i,getTimeAboveThresholdCutMin(i),getTimeAboveThresholdCutMax(i));
  }
  cout << endl;
  return; 
}

void HMdcSelfTracking::printCorrelationFct(void)
{
  // Begin_Html Prints out the correltation fct parameters (1=plane1 vs. plane2, 2=1 vs. 3, 3=2 vs. 3)   
  // to the standard output End_Html

  cout << endl;
  cout << "Correlation Function parameters:" << endl;
  printf("cor. \tA \tB \tC \n");
  printf("-------------------------------------------------------\n");
  for (Int_t i=1; i<4;i++){
    printf("%i \t%f \t%f \t%f\n",i,getCorrelationFctA(i),getCorrelationFctB(i),getCorrelationFctC(i));
  }
  cout << endl;
  return; 
}

void HMdcSelfTracking::printAnalysisCellCutWindow(void)
{
  // Prints out the range of the cells numbers which will be used in further analysis
  // to the standard output

  cout << endl;
  cout << "Analysis Cell Windows :" << endl;
  printf("min\t\tmax \n");
  printf("-------------------------------------------------------\n");
  printf("%03i\t\t%03i\n",getAnalysisCellCutMin(),getAnalysisCellCutMax());
  cout << endl;
  return; 
}

void HMdcSelfTracking::printTimeDiffL3L4Cut()
{
  // Begin_Html Prints out the value for the difference 
  // between time1 in layer 3 and time1 in layer 4
  // to the standard output  End_Html
  
  cout << endl;
  printf("Time Difference Cut between Layer 3 and 4: %3.1f ns\n",tdifcut);
  cout << endl;
  return;
}

Bool_t HMdcSelfTracking::init(void)
{
//  Initializes HMdcSelfTracking

    // MDC:
    calCatMdc=gHades->getCurrentEvent()->getCategory(catMdcCal1);
    if (!calCatMdc) {
        calCatMdc=gHades->getSetup()->getDetector("Mdc")->buildCategory(catMdcCal1);
        if (!calCatMdc) return kFALSE;
        else gHades->getCurrentEvent()->addCategory(catMdcCal1,calCatMdc,"Mdc");
    }
    // creates an iterator which loops over all fired cells
    iterMdc=(HIterator *)calCatMdc->MakeIterator("native");
    if (fNameRoot && !fout)
        fout = new TFile(fNameRoot,"RECREATE");

    //START: 
    if(!noStart){
	calCatStart=gHades->getCurrentEvent()->getCategory(catStart2Cal);
	if (!calCatStart) {
	    calCatStart=gHades->getSetup()->getDetector("Start")->buildCategory(catStart2Cal);
	    if (!calCatStart) return kFALSE;
	    else gHades->getCurrentEvent()->addCategory(catStart2Cal,calCatStart,"Start");
	}
    }
    //Header:
    evheader = gHades->getCurrentEvent()->getHeader();

    // creates an iterator which loops over all fired cells
    if(!noStart){
	iterStart=(HIterator *)calCatStart->MakeIterator("native");
    }
    if (fNameRoot && !fout)
        fout = new TFile(fNameRoot,"RECREATE");

    createHist();
    resetCounters();
    fActive=kTRUE;

    return kTRUE;
}

void HMdcSelfTracking::createHist()
{
    // allocate histograms

    Char_t title[20], name[20];

    sprintf(title, "%s%i","Time_cut",0);
    sprintf(name, "%s%i%s","pTime_cut[",0,"]");
    pTime_cut[0] = new TH1D(name,title,350,-100,600);
    for (Int_t i=1;i<7;i++){
	sprintf(title, "%s%i","Time_cut",i);
	sprintf(name, "%s%i%s","pTime_cut[",i,"]");
	pTime_cut[i] = new TH1D(name,title,350,-100,600);
	sprintf(title, "%s%i","Time0_cell",i);
	sprintf(name, "%s%i%s","pTime0_cell[",i,"]");
	pTime0_cell[i] = new TH2S(name,title,350,-100,600,200,0,200);
	sprintf(title, "%s%i","Time_cell",i);
	sprintf(name, "%s%i%s","pTime_cell[",i,"]");
	pTime_cell[i] = new TH2S(name,title,300,0,600,200,0,200);
	sprintf(title, "%s%i","Layer",i);
	sprintf(name, "%s%i%s","pLayer[",i,"]");
	player[i] = new TH1D(name,title,200,0,200);
	sprintf(title, "%s%i","NhitLayer",i);
	sprintf(name, "%s%i%s","pNhitLayer[",i,"]");
	pNhitlayer[i] = new TH1D(name,title,50,0,50);
	if (i>0 && i<3) {
	    sprintf(title, "%s%i","Timesum_cell",i);
	    sprintf(name, "%s%i%s","pTimesum_cell[",i,"]");
	    pTimesum_cell[i] = new TH2S(name,title,200,0,200,75,0,300);
	    sprintf(title, "%s%i","Time0diff_cell",i);
	    sprintf(name, "%s%i%s","pTime0diff_cell[",i,"]");
	    pTime0diff_cell[i] = new TH2S(name,title,250,-50,200,100,0,200);
	    for (Int_t j=cellmin; j<=cellmax; j++){
		sprintf(title, "%s%i%s%i","Fish",i,"_",j);
		sprintf(name, "%s%i%s%i%s","pFish[",i,"][",j,"]");
		pFish[i][j] = new TH2S(name,title,150,0,300,125,-250,250);
// 		sprintf(title, "%s%i%s%i","Tsum_ev_prof",i,"_",j);
// 		sprintf(name, "%s%i%s%i%s","pTsum_ev_prof[",i,"][",j,"]");
// 		pTsum_ev_prof[i][j] = new TProfile(name,title,150,0,3000000,20,250);
		sprintf(title, "%s%i%s%i","Tsum_ev",i,"_",j);
		sprintf(name, "%s%i%s%i%s","pTsum_ev[",i,"][",j,"]");
		pTsum_ev[i][j] = new TH2S(name,title,150,0,3000000,60,0,240);
	    }
	}
    }
    sprintf(title, "%s","Time0_diff");
    sprintf(name, "%s","pTime0_diff");
    pTime0_diff = new TH2S(name,title,250,-50,450,250,-50,450);
    pTime_diff[0] = new TH1D("pTime_diff[0]","tdif0",650,-50,600); // t1-t2 before cuts
    pTime_diff[1] = new TH1D("pTime_diff[1]","tdif1",650,-50,600); // t1-t2 after cuts
    player0[1] = new TH1D("player01","layer01",200,0,200);
    player0[2] = new TH1D("player02","layer02",200,0,200);
    pinfo = new TH1F("pinfo","info",60,-0.25,29.25);
    peff = new TH1F("peff","eff",160,0,160);
    pTimesum_adj_cell3 = new TH2S("pTimesum_adj_cell3","pTimesum_adj_cell3",250,0,250,100,0,200);
    pTimesum_adj_cell4 = new TH2S("pTimesum_adj_cell4","pTimesum_adj_cell4",250,0,250,100,0,200);
    if(!noStart){
	pStart_time_strip = new TH2S("pStart_time_strip","pStart_time_strip",300,0,150,100,0.5,8.5);
	for (Int_t i=1;i<HMDCSELFTRACKING_STARTSTRIP_NUMBER;i++){
	    sprintf(title, "%s%i","Start_time",i);
	    sprintf(name, "%s%i%s","pStart_time[",i,"]");
	    pStart_time[i] = new TH1F(name,title,1200,0,150);
	}
	pStart_mult = new TH1F("pStart_mult","pStart_mult",5,-0.5,4.5);
    }
    if(!noCorrSector) {
       pCorrSector = new TH3F("pCorrSector","CorrSector",180,0,180,180,0,180,180,0,180);
       for (Int_t i=1;i<4;i++){
	    sprintf(title, "%s%i","CorrDiff0",i);
	    sprintf(name, "%s%i%s","CorrDiff0[",i,"]");
	    pCorrDiff[0][i] = new TH1F(name,title,100,-20,20);
	}
    }

    if(!noCheckCorrSector) {
       for (Int_t i=1;i<4;i++){
	    sprintf(title, "%s%i","CorrDiff1",i);
	    sprintf(name, "%s%i%s","CorrDiff1[",i,"]");
	    pCorrDiff[1][i] = new TH1F(name,title,100,-20,20);
	}
    }
}

void HMdcSelfTracking::checkAdjacentCellAnalysisLayer1(Int_t ihit, Int_t ii)
{
    // Begin_Html Checks whether cell ihit and cell ii are adjacent
    // cells in analysis layer 1 ( default= layer 3)
    // and if this cell combination has not been used before End_Html
    // fills the sum of their times to pTimesum_adj_cell3

    if (abs(hitCells[analysisLayer1][ihit]-hitCells[analysisLayer1][ii])==1)
    {
	flag[NEIGHBOURS] = kTRUE;
	timesum  = hitTime1[analysisLayer1][ihit] + hitTime1[analysisLayer1][ii];
	timediff = hitTime1[analysisLayer1][ihit] - hitTime1[analysisLayer1][ii];

	if (alreadyUsedCellFlag_AnalysisLayer1[hitCells[analysisLayer1][ihit]] == 0 &&
	    alreadyUsedCellFlag_AnalysisLayer1[hitCells[analysisLayer1][ii]]   == 0)
	{
	    if (fabs(timediff)<timeDiffMax) pTimesum_adj_cell3 -> Fill(timesum,hitCells[analysisLayer1][ihit],1.);
	    alreadyUsedCellFlag_AnalysisLayer1[hitCells[analysisLayer1][ihit]] = 1;
	    alreadyUsedCellFlag_AnalysisLayer1[hitCells[analysisLayer1][ii]]   = 1;
	}
    }
    return;
}

void HMdcSelfTracking::checkAdjacentCellAnalysisLayer2(Int_t ihit, Int_t ii)
{
    // Begin_Html Checks whether cell ihit and cell ii are adjacent cells in analysis layer 2 ( default= layer 4)
    // and if this cell combination has not been used before End_Html
    // fills the sum of their times  to pTimesum_adj_cell4
    // but only if the time difference is smaller than a given value (default: 10 ns)

    if (abs(hitCells[analysisLayer2][ihit] - hitCells[analysisLayer2][ii]) == 1)
    {
	timesum  = hitTime1[analysisLayer2][ihit] + hitTime1[analysisLayer2][ii];
	timediff = hitTime1[analysisLayer2][ihit] - hitTime1[analysisLayer2][ii];

	if (alreadyUsedCellFlag_AnalysisLayer2[hitCells[analysisLayer2][ihit]] == 0 &&
	    alreadyUsedCellFlag_AnalysisLayer2[hitCells[analysisLayer2][ii]]   == 0)
	{
	    if (fabs(timediff)<timeDiffMax) pTimesum_adj_cell4 -> Fill(timesum,hitCells[analysisLayer2][ihit],1.);
	    alreadyUsedCellFlag_AnalysisLayer2[hitCells[analysisLayer2][ihit]] = 1;
	    alreadyUsedCellFlag_AnalysisLayer2[hitCells[analysisLayer2][ii]]   = 1;
	}
    }
    return;
}

void HMdcSelfTracking::findCorrelatedHits(Int_t ihit)
{
    // Begin_Html Compares all hit cells found in analysis layer 2 with cell ihit
    // and looks if there are correlated cells in analysis layer 1 and 2
    // Then it determines whether the hit in analysis layer 2 was in the cell left or right
    // beneath cell ihit in analysis layer 1 End_Html

    flag[LEFT_CELL_HIT]  = kFALSE;
    flag[RIGHT_CELL_HIT] = kFALSE;

    for (Int_t ii=1; ii < nhit[analysisLayer2]+1; ii++)
    { // loop over hits in layer4
	if ((hitCells[analysisLayer1][ihit] - hitCells[analysisLayer2][ii]) == setLeft[setmodule-1])
	{ // left
	    flag[LEFT_CELL_HIT]  = kTRUE;
	    lhit[LEFT] = ii;
	}
	if ((hitCells[analysisLayer1][ihit] - hitCells[analysisLayer2][ii]) == setRight[setmodule-1])
	{ //right
	    flag[RIGHT_CELL_HIT] = kTRUE;
	    lhit[RIGHT] = ii;
	}
    } // layer4 hit loop

    return;
}

void HMdcSelfTracking::fillHist()
{

// analyze layer 3: +0deg, layer 4: -0deg, fill histos

    if (nhit[analysisLayer2] > 0)
    { // analysis of adjacent cells in layer 4
        cntHitslayer2=cntHitslayer2+nhit[analysisLayer2];
	for (Int_t ihit = 1; ihit < nhit[analysisLayer2] + 1; ihit++)
	{ // loop over hits in layer4
	    for (Int_t ii = 1; ii < nhit[analysisLayer2] + 1; ii++)
	    {
		checkAdjacentCellAnalysisLayer2(ihit, ii);
	    }
	}
    }

    if (nhit[analysisLayer1] > 0)
    { // adjacent cell analysis and selfcorrelation
        cntHitslayer1=cntHitslayer1+nhit[analysisLayer1];
	for (Int_t ihit = 1; ihit < nhit[analysisLayer1] + 1; ihit++)
	{ // loop over hits in layer3
	    flag[NEIGHBOURS] = kFALSE;
	    for (Int_t ii = 1; ii < nhit[analysisLayer1] + 1; ii++)
	    {
		checkAdjacentCellAnalysisLayer1(ihit, ii);
	    }

            flag[RESIDUAL_CUT]=kTRUE;
            if (!noCheckCorrSector) checkCorrSector(hitCells[analysisLayer1][ihit],setmodule);

	    if (!flag[NEIGHBOURS] && flag[RESIDUAL_CUT])
	    { // hit cells  w/o neighbours & residual cut
		cntHitsWithoutNeighbours++;
		counter[NEIGHBOURS][ hitCells[analysisLayer1][ihit] ]++;

                findCorrelatedHits(ihit);
 
		if ( flag[LEFT_CELL_HIT]  || flag[RIGHT_CELL_HIT])
		{ // hit(s) in layer4, incl. Double_t hits
		    cntHitsAnalysislayer2++;
		    counter[ANALYSISLAYER2_HITS][hitCells[analysisLayer1][ihit]]++;
		}

		if ( flag[LEFT_CELL_HIT]  && flag[RIGHT_CELL_HIT])
		{ // Double_t hit in layer4
		    cntDoubleHitsAnalysislayer2++;
		    counter[ANALYSISLAYER2_DOUBLE_HITS][hitCells[analysisLayer1][ihit]]++;
		}

		if ((flag[LEFT_CELL_HIT]) ^ (flag[RIGHT_CELL_HIT]))
		{ // single hit in layer4
		    fillAnalysisHists(ihit);
		} // end case single hit in layer4

	    } // end case layer3 sinlge hit cell
	} // end layer3 hit loop
    } // end case layer3
    // end event-wise analysis
}

void HMdcSelfTracking::fillAnalysisHists(Int_t ihit)
{
    // Fills histogramms after all neccessary conditions are checked.

    Int_t leftOrRight = (flag[RIGHT_CELL_HIT])? RIGHT:LEFT; // fill right or left

    Float_t time1AnalysisLayer1 = hitTime1[analysisLayer1][ihit];
    Float_t time1AnalysisLayer2 = hitTime1[analysisLayer2][lhit[leftOrRight]];

    if (hitCells[analysisLayer1][ihit] >= cellmin &&
	hitCells[analysisLayer1][ihit] <= cellmax)
    {
	pFish[leftOrRight][hitCells[analysisLayer1][ihit]]
	    -> Fill(time1AnalysisLayer1 + time1AnalysisLayer2,
		    time1AnalysisLayer1 - time1AnalysisLayer2,
		    1.);
	if ( fabs(time1AnalysisLayer1 - time1AnalysisLayer2) < tdifcut)
	  { 
//             pTsum_ev_prof[leftOrRight][hitCells[analysisLayer1][ihit]]
// 	    -> Fill(eventCounter,time1AnalysisLayer1 + time1AnalysisLayer2);
            pTsum_ev[leftOrRight][hitCells[analysisLayer1][ihit]]
	    -> Fill(eventCounter,time1AnalysisLayer1 + time1AnalysisLayer2);
          }
    }
    if ( fabs(time1AnalysisLayer1 - time1AnalysisLayer2) < tdifcut)
    {
	pTimesum_cell[leftOrRight]
	    -> Fill(hitCells[analysisLayer1][ihit],
		    time1AnalysisLayer1 + time1AnalysisLayer2,
		    1.);
	player0[leftOrRight]
	    -> Fill( (Float_t) hitCells[analysisLayer1][ihit],
		     1.);
    }
    return;
}

void HMdcSelfTracking::writeHist()
{
    // Write histogramms to a rootfile.

    fout->cd();

    pinfo -> Write();
    pTime_diff[0] -> Write(); pTime_diff[1] -> Write();
    player0[1] -> Write(); player0[2] -> Write();
    peff -> Write();
    pTimesum_adj_cell3 -> Write(); pTimesum_adj_cell4 -> Write();
    pTime_cut[0] -> Write();
    pTime0_diff -> Write();
    for (Int_t i=1; i<7; i++){
        pTime_cut[i] -> Write();
        player[i] -> Write();
        pNhitlayer[i] -> Write();
        pTime_cell[i] -> Write();
        pTime0_cell[i] -> Write();
        if (i>0 && i<3){
            pTimesum_cell[i] -> Write();
            pTime0diff_cell[i] -> Write();
            for (Int_t j=cellmin; j<=cellmax; j++){
                pFish[i][j] -> Write();
//                 pTsum_ev_prof[i][j] -> Write();
                pTsum_ev[i][j] -> Write();
            }
        }
    }
    if(!noStart){
	pStart_time_strip -> Write();
	for (Int_t i=1; i<HMDCSELFTRACKING_STARTSTRIP_NUMBER; i++) {pStart_time[i] -> Write();}
	pStart_mult -> Write();
    }
    if(!noCorrSector) {
       pCorrSector -> Write();
       for (Int_t i=1; i<4; i++){pCorrDiff[0][i] -> Write();}
    }

    if(!noCheckCorrSector) {
      for (Int_t i=1; i<4; i++){pCorrDiff[1][i] -> Write();}
    }
}


Bool_t HMdcSelfTracking::finalize()
{
    // Fills control histogramms and saves all histogramms to file fout

    fillControlHists();
    if (fout)  writeHist ();

    fout->Save();
    fout->Close();
    delete fout;

    return kTRUE;
}

void HMdcSelfTracking::resetCounters()
{
    // reset counters

    for (Int_t ii=0; ii<3 ; ii++)
    {
	for (Int_t jj=0;jj<160;jj++) counter[ii][jj]=0;
    }
    for (Int_t ii=0; ii<11; ii++) ctrl[ii]=0;
    for (Int_t ii=0; ii<7 ; ii++) nhit[ii]=0;
}

void HMdcSelfTracking::fillControlHists()
{
    // fill some info histos

    pinfo->Fill(10.0, (Float_t) cntHitslayer1);
    pinfo->Fill(11.0, (Float_t) cntHitslayer2);
    pinfo->Fill(12.0, (Float_t) cntHitsWithoutNeighbours);
    pinfo->Fill(13.0, (Float_t) cntHitsAnalysislayer2);
    pinfo->Fill(14.0, (Float_t) cntDoubleHitsAnalysislayer2);
    pinfo->Fill(20.0, (Float_t) (ctrl[NO_CUT]));
    pinfo->Fill(21.0, (Float_t) (ctrl[TOF_BIT_CUT]));
    pinfo->Fill(22.0, (Float_t) (ctrl[START_CUT]));
    pinfo->Fill(23.0, (Float_t) (ctrl[TIME1_CUT]));
    pinfo->Fill(24.0, (Float_t) (ctrl[TIME2_CUT]));
    pinfo->Fill(25.0, (Float_t) (ctrl[TIME_ABOVE_THRESHOLD_CUT]));

    printf(" layer3 hits: %i, corresponding layer4 hits: %i (Double_t hits: %i)\n",
	   cntHitsWithoutNeighbours,
	   cntHitsAnalysislayer2,
	   cntDoubleHitsAnalysislayer2);

    for (Int_t i=1; i<160; i++){
	if (counter[NEIGHBOURS][i]>0 && counter[ANALYSISLAYER2_HITS][i]>0)
	    peff->Fill(i, (counter[ANALYSISLAYER2_HITS][i]/counter[NEIGHBOURS][i])*100 );
    }

}

void HMdcSelfTracking::executeEventHeaderCheck()
{
    // Checks Event Header for TofBit

    pinfo->Fill(NO_CUT,1.);

//  if (evheader->IsTofBit() || cutbridge[TOF_BIT_CUT]) {
    if (cutbridge[TOF_BIT_CUT]) {
	cut[TOF_BIT_CUT]=kTRUE; // cut on trigger condition
	pinfo->Fill(TOF_BIT_CUT,1.);
    }
    return;
}

void HMdcSelfTracking::executeStart()
{
    // First checks start detector for single hits
    // and secondly filters for certain time windows on the start detector times
    // defined per default in HMdcSelfTracking:setDefaults() or
    // changed by HMdcSelfTracking:setStartTimeCutWindow(strip,min,max)

    HStart2Cal *objStartC;
    while ( (objStartC=(HStart2Cal *)iterStart->Next())!=0 ) {
	start_strip = objStartC->getStrip() + 1;
//	start_time = objStartC->getTime();
	if (start_strip<5) nstartstrip++; //check start, NOT veto counter
    }
    pStart_mult->Fill(nstartstrip,1.);
//     if (nstartstrip==1 || cutbridge[START_CUT]){ // cut on start strip multiplicity=1 & start time
// 	pStart_time_strip -> Fill(start_time, start_strip);
// 	if (cut[TOF_BIT_CUT]) pStart_time[start_strip] -> Fill(start_time,1.);
// 	if ((start_time>startmin[start_strip] && start_time<startmax[start_strip]) || cutbridge[START_CUT]){
// 	    cut[START_CUT]=kTRUE;
// 	    pinfo->Fill(START_CUT,1.);
// 	}
//     }
    return;
}

void HMdcSelfTracking::executeMdc()
{

    relevant_data=kFALSE;

    HMdcCal1 *objMdcC;
    while ( (objMdcC=(HMdcCal1 *)iterMdc->Next())!=0 ) {
	sector = objMdcC->getSector() + 1;
	module = objMdcC->getModule() + 1;
	layer = objMdcC->getLayer() + 1;
	cell  = objMdcC->getCell() + 1;
	time1   = objMdcC->getTime1();
	time2   = objMdcC->getTime2();

        if(sector==4&&layer==3&& // Sector correlation analysis w/ time cuts, sector 4 only!
          (time1>tmin && time1<tmax)&&(time2>tmin && time2<tmax)&&
          ((time2-time1)>tdifmin[module] && (time2-time1)<tdifmax[module])) {
             ncell0[module]++;
	     cell0[module][ncell0[module]]=cell;
        }
  
	if(sector==setsector&&module==setmodule){

	    pTime_diff[0] -> Fill(time2-time1,1.);
	    pTime0_cell[layer] -> Fill(time1,(Float_t) cell,1.);
	    if (time1>25 && time1<45) {
		if (layer == analysisLayer1) pTime0diff_cell[1] -> Fill((time2-time1),(Float_t) cell,1.);
		if (layer == analysisLayer2) pTime0diff_cell[2] -> Fill((time2-time1),(Float_t) cell,1.);
	    }

	    if (layer == monitorLayer && cell == monitorCell) pTime_cut[NO_CUT] -> Fill(time1,1.);
 
            ctrl[NO_CUT]++;
	    if (cut[TOF_BIT_CUT] || cutbridge[TOF_BIT_CUT]) {
		if (layer == monitorLayer && cell == monitorCell) pTime_cut[TOF_BIT_CUT] -> Fill(time1,1.);

                ctrl[TOF_BIT_CUT]++;
		if (cut[START_CUT] || cutbridge[START_CUT]) {
		    if (layer == monitorLayer && cell == monitorCell){
			pTime_cut[START_CUT] -> Fill(time1,1.);
			pTime0_diff -> Fill(time1,(time2-time1),1.);

		    }

		    // MDC-CUTS:
		    ctrl[START_CUT]++;
		    if ((time1>tmin && time1<tmax) || cutbridge[TIME1_CUT])
		    {   // cut time1 (time window due to "-99")
			if (layer == monitorLayer && cell == monitorCell) pTime_cut[TIME1_CUT] -> Fill(time1,1.);
			ctrl[TIME1_CUT]++;
			if ((time2>tmin && time2<tmax) || cutbridge[TIME2_CUT])
			{  // cut time2
			    if (layer == monitorLayer && cell == monitorCell) pTime_cut[TIME2_CUT] -> Fill(time1,1.);
			    ctrl[TIME2_CUT]++;
			    if (((time2-time1)>tdifmin[setmodule] && (time2-time1)<tdifmax[setmodule]) ||
				cutbridge[TIME_ABOVE_THRESHOLD_CUT])
			        {  // cut time-above-thr.
				relevant_data=kTRUE;
			     	if (layer == monitorLayer && cell == monitorCell) pTime_cut[TIME_ABOVE_THRESHOLD_CUT] -> Fill(time1,1.);
				nhit[layer]++;
				ctrl[TIME_ABOVE_THRESHOLD_CUT]++;
				hitCells[layer][nhit[layer]] = cell;
				hitTime1[layer][nhit[layer]] = time1;
				hitTime2[layer][nhit[layer]] = time2;
				pTime_cell[layer] -> Fill(time1,(Float_t) cell,1.);
				pTime_diff[1] -> Fill(time2-time1,1.);
				player[layer] -> Fill((Float_t) cell, 1.);
			    } // cut5: time-dif
			} // cut4: time2
		    } // cut3: time1
                    pNhitlayer[layer] -> Fill((Float_t) nhit[layer],1.);
		} // cut2: start
	    } // cut1: trigger
	} // set sector/module
    } // end of while MDC Iterator

    return;
}

void HMdcSelfTracking::executeCorrSector()
{
  // combinatorics:
  for (Int_t i3=1; i3<(ncell0[3]+1); i3++){
    for (Int_t i2=1; i2<(ncell0[2]+1); i2++){
      for (Int_t i1=1; i1<(ncell0[1]+1); i1++){

	 pCorrSector -> Fill(cell0[1][i1],cell0[2][i2],cell0[3][i3]); // fill 3D

    	 // fill residuals to determine cut later:
   	 Float_t x0=cell0[1][i1]; 
	 Float_t y0=cell0[2][i2];
	 Float_t z0=cell0[3][i3];

         Float_t x1=A[1]+B[1]*y0+C[1]*y0*y0;
         Float_t x2=A[2]+B[2]*z0+C[2]*y0*y0;
         Float_t y1=A[3]+B[3]*z0+C[3]*z0*z0;

         Float_t difx1=x0-x1;
         Float_t difx2=x0-x2;
         Float_t dify1=y0-y1;

         pCorrDiff[0][1]->Fill(difx1,1.);
         pCorrDiff[0][2]->Fill(difx2,1.);
         pCorrDiff[0][3]->Fill(dify1,1.);

      }   
    }
  }
   return;
}

void HMdcSelfTracking::checkCorrSector(Int_t cell, Int_t module)
{

  Int_t m1, m2;
  Float_t x[4] = {0,0,0,0};

  Float_t dif[4] = {1000, 1000, 1000, 1000}; // default residuals
  Bool_t check[4] = {kFALSE, kFALSE, kFALSE};

  check[module] = kTRUE;

  m1=2; m2=3;

  switch(module){ // set loop & residual variables
    case 1:
      m1=2; m2=3;
    break;  
    case 2:
      m1=1; m2=3;
    break;  
    case 3:
      m1=1; m2=2;
    break;   
    default:  
      printf("!!! checkCorrSector: not yet initialized !!!");
    break;
  }

  flag[RESIDUAL_CUT]=kFALSE;

  for (Int_t i1=1; i1<(ncell0[m1]+1); i1++){
     for (Int_t i2=1; i2<(ncell0[m2]+1); i2++){
       if(module==1){x[1]=cell; x[2]=cell0[m1][i1]; x[3]=cell0[m2][i2];}
       if(module==2){x[1]=cell0[m1][i1]; x[2]=cell; x[3]=cell0[m2][i2];}
       if(module==3){x[1]=cell0[m1][i1]; x[2]=cell0[m2][i2]; x[3]=cell;}
       dif[1]=x[1]-(A[1]+B[1]*x[2]+C[1]*x[2]*x[2]);
       dif[2]=x[1]-(A[2]+B[2]*x[3]+C[2]*x[3]*x[3]);
       dif[3]=x[2]-(A[3]+B[3]*x[3]+C[3]*x[3]*x[3]);
//        pCorrDiff[0][1]->Fill(dif[1],1.);
//        pCorrDiff[0][2]->Fill(dif[2],1.);
//        pCorrDiff[0][3]->Fill(dif[3],1.);
       if(((dif[1]>CorrWindow[1][1] && dif[1]<CorrWindow[1][2])|!check[1]) &&
          ((dif[2]>CorrWindow[2][1] && dif[2]<CorrWindow[2][2])|!check[2]) &&
          ((dif[3]>CorrWindow[3][1] && dif[3]<CorrWindow[3][2])|!check[3])) flag[RESIDUAL_CUT]=kTRUE;
     }
  }

  if (flag[RESIDUAL_CUT]) {
    for(Int_t i=1;i<4;i++){pCorrDiff[1][i]->Fill(dif[i],1.);}
  }  

return;
}


void HMdcSelfTracking::executeReset()
{
    // reset iterators
    iterMdc->Reset();
    if(!noStart) iterStart->Reset();

    // reset counters, flags
    for (Int_t ii=0; ii<5; ii++){ ncell0[ii] = 0;}
    for (Int_t ii=0; ii<7; ii++){ nhit[ii] = 0;}
    for (Int_t ii=0; ii<20; ii++){ cut[ii] = 0;}
    for (Int_t ii=0; ii<200; ii++)
    {
	alreadyUsedCellFlag_AnalysisLayer1[ii] = 0;
	alreadyUsedCellFlag_AnalysisLayer2[ii] = 0;
        for (Int_t i=0; i<5;i++){ cell0[i][ii] = 0;}
    }
    nstartstrip=0;

    return;
}

Int_t HMdcSelfTracking::execute()
{
    // loop inside event and fill pre-defined structures, apply cuts

    executeReset();

    // hit loop, fill local variables:
    eventCounter++;
    if ( !(eventCounter % 2000) ) printf(" ... event %i\n",eventCounter);

    // Event Header:
    executeEventHeaderCheck();

    // Start:
    if(!noStart) executeStart();

    // Mdc:
    executeMdc();

    // Correlations in Sector:
    if(!noCorrSector) executeCorrSector();

    // Correlation Analysis:
    if (relevant_data) fillHist();

    return 0; // end event
}

