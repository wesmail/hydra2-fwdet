//*-- AUTHOR : Ilse Koenig
//*-- Modified : 17/01/2002 by Ilse Koenig
//*-- Modified : 17/02/2000 by J. Kempter
//*-- Modified : 24/08/99 by R. Holzmann
//*-- Modified : 29/07/99 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcCalibrater1
//
// This transformation class calculates the drift time information in all fired
// cells using parameters from the parameter containers MdcCalParRaw for the
// calibration parameters and from MdcLookupGeom to map the layer number and
// cell number on Cal1 level to the motherboard number and TDC channel number
// on Raw level.
// The drift times are randomized within one bin of TDC resolution.
//
// The constructor gives different options how to fill HMdcCal1 level:
// HMdcCalibrater1(name,title,option input, otption timecut, option embedding )
//    option input   =  1 No Start and Cal (default)
//                      2 Start and Cal
//                      3 No Start and no Cal
//    option timecut =  1 time cuts switched on
//                      0 time cuts switched off
//    option embedding = 0 no embedding
//                     = 1 embedding
// Start means that the startime from the Startdetector will be used to correct.
// Cal means real calibration with parameters from HMdcCalParRaw
// If no Cal (3)is selected no input from HMdcCalParRaw is needed
// and the TDC slope parameter will be always 1.The offset is set to 2048
// which means that the HMdcCal1 level will be a reversed copy of HMdcRaw.
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdccalibrater1.h"
#include "hmdcdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hcategorymanager.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hmdcdetector.h"
#include "hevent.h"
#include "hrecevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdccal1sim.h"
#include "hmdccalparraw.h"
#include "hmdctimecut.h"
#include "hmdccutstat.h"
#include "hmdcwirestat.h"
#include "hmdclookupgeom.h"
#include "hstart2hit.h"

#include <iostream> 
#include <iomanip>
#include <stdlib.h>

Float_t HMdcCalibrater1::globalOffset[4]={0,0,0,0};
Float_t HMdcCalibrater1::globalSecOffset[6][4]={{0,0,0,0},
                                                {0,0,0,0},
                                                {0,0,0,0},
                                                {0,0,0,0},
                                                {0,0,0,0},
                                                {0,0,0,0}};
Float_t HMdcCalibrater1::globalSlope=1;
Int_t HMdcCalibrater1::countNrWiresPerMod[6][4]=
{
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}};
Int_t HMdcCalibrater1::countNrWiresPerModCal[6][4]=
{
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0},
    {0,0,0,0}};

Bool_t HMdcCalibrater1::useWireStat=kFALSE;

ClassImp(HMdcCalibrater1)

HMdcCalibrater1::HMdcCalibrater1(void) {
  // Default constructor .
  initParameters();
}
HMdcCalibrater1::HMdcCalibrater1(const Text_t* name,const Text_t* title,Int_t vers,Int_t cut,Int_t merge)
                 :  HReconstructor(name,title) {
  // Constructor calls the constructor of class HReconstructor with the name
  // and the title as arguments.
  initParameters();
  switchArguments(vers,cut,merge);
}

HMdcCalibrater1::~HMdcCalibrater1(void) {
  // destructor deletes the iterator
  if (iter) delete iter;
  iter=0;
  if (iterstart) delete iterstart;
  iterstart=0;
  if(cutStat)HMdcCutStat::deleteHMdcCutStat();
}
void HMdcCalibrater1::switchArguments(Int_t vers,Int_t cut,Int_t merge)
{
    if(cut==0 || cut==1)
    {
	if(cut==0) setTimeCut=kFALSE;
	if(cut==1) setTimeCut=kTRUE;
    }
    else
    {
	Error("HMdcCalibrater1:switchArguments(Int_t vers,Int_t cut,Int_t merge):"
	      ,"\n unknown argument cut: use \n 0 for no time cuts \n 1 for time cuts");
	exit(1);
    }
    if(vers==1 || vers==2 || vers==3)
    {
	if(vers==1)
	{
	    NoStartandCal=kTRUE;
	    StartandCal=kFALSE;
	    NoStartandNoCal=kFALSE;
	}
	if(vers==2)
	{
	    StartandCal=kTRUE;
	    NoStartandCal=kFALSE;
	    NoStartandNoCal=kFALSE;
	}
	if(vers==3)
	{
	    NoStartandNoCal=kTRUE;
	    StartandCal=kFALSE;
	    NoStartandCal=kFALSE;
	}
    }
    else
    {
	Error("HMdcCalibrater1:switchArguments(Int_t vers,Int_t cut,Int_t merge):"
	      ,"\n unknown argument vers: use \n 1 for nostartandcal,\n 2 for startandcal,\n 3 for nostartandnocal,\n ");
	exit(1);
    }
    if(merge==0 || merge==1)
    {
         embedding=merge;
    }
    else
    {
	Error("HMdcCalibrater1:switchArguments(Int_t vers,Int_t cut,Int_t merge):"
	      ,"\n unknown argument merge: use \n 0 for normal mode,\n 1 for embedding!\n ");
	exit(1);
    }
}
void HMdcCalibrater1::initParameters()
{
    rawCat=0;
    calCat=0;
    raw=0;
    cal=0;
    startHitCat=0;
    iter=0;
    iterstart=0;
    calparraw=0;
    wireStat=0;
    timecut=0;
    cutStat=0;
    lookup=0;
    NoStartandNoCal=kFALSE;
    StartandCal=kFALSE;
    NoStartandCal=kTRUE;
    setTimeCut=kFALSE;
    hasPrinted=kFALSE;
    embedding=0;
    cutthreshold=100;
    useMultCut=kFALSE;
    doprint=kFALSE;
    skipCal=kTRUE;
}
void HMdcCalibrater1::setParContainers() {
  // creates the parameter containers MdcCalParRaw and MdcLookupGeom if they do not
  // exist and adds them to the list of parameter containers in the rumtime
  // database
    lookup=(HMdcLookupGeom*)(((HRuntimeDb*)(gHades->getRuntimeDb()))->getContainer("MdcLookupGeom"));
    if(setTimeCut) {
	timecut=(HMdcTimeCut*)(((HRuntimeDb*)(gHades->getRuntimeDb()))->getContainer("MdcTimeCut"));
	timecut->setIsUsed(kTRUE);
    }
    if(setTimeCut==kTRUE)
    {
	cutStat=(HMdcCutStat*)HMdcCutStat::getObject();
	if(!cutStat)
	{
	    Error("HMdcCalibrater1:setParContainers()","RETRIEVED 0 POINTER FOR HMDCCUTSTAT!");
	    exit(1);
	}
    }
    if(StartandCal||NoStartandCal){
	calparraw=(HMdcCalParRaw*)(((HRuntimeDb*)(gHades->getRuntimeDb()))->getContainer("MdcCalParRaw"));
    }
    if(useWireStat){
       wireStat = (HMdcWireStat*)(((HRuntimeDb*)(gHades->getRuntimeDb()))->getContainer("MdcWireStat"));

    } else {
       wireStat = 0;
    }

}
Bool_t HMdcCalibrater1::init(void) {
  // creates the MdcRaw  and MdcCal1 categories and adds them to the current
  // event
  // creates an iterator which loops over all fired cells
  // calls the function setParContainers()
  if(gHades->getEmbeddingMode()>0)
  {
      if(embedding==0)
      {
	  Warning("HMdcCalibrater1::init()","global embedding mode >0 detected, but local running embedding==0, switch to common mode!");
	  embedding=1;
      }
  }
  setParContainers();
  rawCat=(HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcRaw));
  if (!rawCat) {
      Error("HMdcCalibrater1:init()","HMdcRaw Category not available!");
      exit(1);
  }
  calCat=(HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catMdcCal1));
  if (!calCat) {
      if(embedding==0)calCat=(HCategory*)((HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"))->buildCategory(catMdcCal1));
      if(embedding==1)
      {
          HMdcDetector* mymdc=(HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector("Mdc"));
	  calCat=(HCategory*)(((HMdcDetector*)mymdc)->buildMatrixCategory("HMdcCal1Sim",0.5F));
      }
      if (!calCat) return kFALSE;
      else ((HEvent*)(gHades->getCurrentEvent()))->addCategory(catMdcCal1,calCat,"Mdc");
  }
  if(StartandCal){
      startHitCat=(HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(catStart2Hit));
      if (!startHitCat) {
	  Error("HMdcCalibrater1:init()","Start2Hit Category not available,\n but Option StartAndCal used!");
          exit(1);
      }
  }

  iter=(HIterator *)((HCategory*)rawCat)->MakeIterator("native");
  loc.set(4,0,0,0,0);

  if(StartandCal){
      iterstart=(HIterator *)((HCategory*)startHitCat)->MakeIterator("native");
  }
  if(!hasPrinted)printStatus();
  fActive=kTRUE;
  return kTRUE;
}
void HMdcCalibrater1::printStatus()
{
    Int_t ModeFlagCal1,TimeCutFlagCal1;
    ModeFlagCal1=TimeCutFlagCal1=-99;
    if(NoStartandCal)  ModeFlagCal1   =1;
    if(StartandCal)    ModeFlagCal1   =2;
    if(NoStartandNoCal)ModeFlagCal1   =3;
    if(!setTimeCut)    TimeCutFlagCal1=0;
    if(setTimeCut)     TimeCutFlagCal1=1;

    // prints the parameters to the screen
    SEPERATOR_msg("-",60);
    INFO_msg(10,HMessageMgr::DET_MDC,"HMdcCalibrater1Setup:");
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"ModeFlagCal1    =  %i :  1 = NoStartandCal, 2 = StartandCal, 3 = NoStartandNoCal",ModeFlagCal1);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"TimeCutFlagCal1 =  %i :  0 = noTimeCut    , 1 = TimeCut",TimeCutFlagCal1);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"Embedding       =  %i :  0 = noEmbedding  , 1 = Embedding",embedding);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"MultCut         =  %i :  0 = noCut  , 1 = Cut",(Int_t)useMultCut);
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName()
			   ,"Cut Threshold   =  %i ",cutthreshold);
    SEPERATOR_msg("-",60);
    hasPrinted=kTRUE;
}

Float_t HMdcCalibrater1::getstarttime(){
  // Need some work for multiple hists in start detector
  // Better select multiplicity 1 in start.
  HStart2Hit* starthit=0;
  Float_t starttime=0;

  starthit=HCategoryManager::getObject(starthit,startHitCat,0,kTRUE); // silent
  if(!starthit) {
      return starttime;
  }
  starttime = -starthit->getTime();

  return starttime;
}
Bool_t HMdcCalibrater1::translateAddress(Int_t* mbo,Int_t* ch)
{
    raw->getAddress(loc[0],loc[1],*mbo,*ch);
    if (loc[0]<0 || loc[1]<0) return kFALSE;
    HMdcLookupMoth& lookGMoth = (*lookup)[loc[0]][loc[1]][*mbo];
    if(&lookGMoth == NULL || lookGMoth.getSize() == 0) return kFALSE;
    HMdcLookupChan& chan=(*lookup)[loc[0]][loc[1]][*mbo][*ch];
    loc[2]=chan.getNLayer();
    loc[3]=chan.getNCell();
    return kTRUE;
}
Bool_t HMdcCalibrater1::testTimeCuts(Float_t testTime1,Float_t testTime2){
  // tests if the measured time will pass the time cuts
  //
    if(useWireStat&&wireStat){
	if(wireStat->getStatus(loc[0],loc[1],loc[2],loc[3]) < 0) return kFALSE;
    }

    if(!timecut) return kTRUE;

    if(setTimeCut==kTRUE)
    {
	if(timecut->cutTime1(loc[0],loc[1],testTime1))
	{cuts[0]=0;}else{cuts[0]=1;}

	if(timecut->cutTime2(loc[0],loc[1],testTime2))
	{cuts[1]=0;}else{cuts[1]=1;}

	if(timecut->cutTimesDif(loc[0],loc[1],testTime1,testTime2))
	{cuts[2]=0;}else{cuts[2]=1;};

        if(countNrWiresPerMod[(Int_t)loc[0]][(Int_t)loc[1]]>3)
	{cuts[3]=0;}else{cuts[3]=1;};

	if(timecut->cutBump    (loc[0],loc[1],testTime1,testTime2-testTime1))
	{cuts[4]=0;}else{cuts[4]=1;};
                                                            //t1    t2      tot     bump    wire
        cutStat->setCal1StatCut((Int_t)loc[0],(Int_t)loc[1],cuts[0],cuts[1],cuts[2],cuts[4],cuts[3]);
    }

    if(timecut->cutTime1   (loc[0],loc[1],testTime1)&&
       timecut->cutTime2   (loc[0],loc[1],testTime2)&&
       timecut->cutBump    (loc[0],loc[1],testTime1,testTime2-testTime1)&&
       timecut->cutTimesDif(loc[0],loc[1],testTime1,testTime2))
    {
	return kTRUE;
    }
    else
    {
	return kFALSE;
    }
}
void HMdcCalibrater1::countWiresPerMod()
{
  // counts the number of fired wires per mod
  //
    raw=0;
    iter->Reset();
    while ((raw=(HMdcRaw *)iter->Next())!=0)
    {
	raw->getAddress(loc[0],loc[1],loc[2],loc[3]);
        countNrWiresPerMod[(Int_t)loc[0]][(Int_t)loc[1]]++;
    }
}
void HMdcCalibrater1::fillCal1(Float_t testTime1,Float_t testTime2,Int_t nHits)
{
  // fill time1,time2 and nHits to Cal1
  //
    if(embedding==0)
    {
	cal=(HMdcCal1 *)calCat->getSlot(loc);
	cal=new(cal) HMdcCal1;
    }
    else
    {
      	cal=(HMdcCal1Sim *)calCat->getSlot(loc);
	cal=new(cal) HMdcCal1Sim;
    }
    if(cal)
    {
	cal->setNHits(nHits);
	cal->setAddress(loc[0],loc[1],loc[2],loc[3]);

	countNrWiresPerModCal[loc[0]][loc[1]]++; // counting per module

	if(StartandCal||NoStartandCal)
	{
	    cal->setTime1(testTime1);
	    cal->setTime2(testTime2);
	}
	if(NoStartandNoCal)
	{
	    cal->setTime1(testTime1);

	    if (abs(nHits)>=2)
	    {
		cal->setTime2(testTime2);
	    }
	}
    }
    else
    {
	Warning("HMdcCalibrater1::fillCal1()","NO SLOT AVAILABLE IN catMdcCal1 FOR SECTOR %i . \n SIZE OF CATEGORY IS %i .",loc[0],calCat->getEntries());
    }
}
void HMdcCalibrater1::calcTimes(Float_t* testTime1,Float_t* testTime2,Float_t starttime,Int_t nHits,HMdcCalParTdc* tdc)
{
  // calc testtime1,testtime2
  //
    Float_t offset; 
    Float_t slope;

    if(tdc){
	offset = tdc->getOffset();
	slope  = tdc->getSlope();
    } else {
	offset = 0;
        slope  = 1;
    }

    Int_t t1       = raw->getTime(1);
    Int_t t2       = raw->getTime(2);
    Float_t rnd1   = gRandom->Rndm() - 0.5F;
    Float_t rnd2   = gRandom->Rndm() - 0.5F;


    if(NoStartandCal)
    {
	*testTime1 = (offset - ((t1 + rnd1) * slope)) - globalSecOffset[loc[0]][loc[1]];
	if (abs(nHits) >= 2) {
	    *testTime2 = (offset - ((t2 + rnd2) * slope)) - globalSecOffset[loc[0]][loc[1]];
	}
    }
    if(StartandCal)
    {
	*testTime1 = (offset - ((t1 + rnd1) * slope) + starttime) - globalSecOffset[loc[0]][loc[1]];
	if (abs(nHits) >= 2) {
	    *testTime2 = (offset - ((t2 + rnd2) * slope) + starttime) - globalSecOffset[loc[0]][loc[1]];
	}
    }
    if(NoStartandNoCal)
    {
	switch (t1){
	case 0:    *testTime1 =- 998.;
	break;
	case -999: *testTime1 =- 999.;
	break;
	default :
	    *testTime1 = (2048 - (t1 + rnd1)) * globalSlope - globalSecOffset[loc[0]][loc[1]];
	    break;
	}
	if (abs(nHits) >= 2){
	    switch(t2){
	    case 0:    *testTime2 =- 998.;
	    break;
	    case -999: *testTime2 =- 999.;
	    break;
	    default:   *testTime2 = (2048 - (t2 + rnd2)) * globalSlope - globalSecOffset[loc[0]][loc[1]] ;
	    break;
	    }
	}
    }
}
Int_t HMdcCalibrater1::execute(void) {

    // calibrates all fired cells
    // For time1 or time2 equal -999 in raw the times in cal1 are set to -999.
    // For time1 or time2 equal 0 in raw the times in cal1 are set to -998.
    // This differnt value is used to distinguish between not filled times and times equal 0.
    // The calibration parameters are taken from HMdcCalParRaw container.The
    // Calibration is only performed if Offset and Slope are not 0.
    // Only time1 and time2 are calibrated.
    Bool_t debug = kFALSE;

    if(skipCal&&gHades->getCurrentEvent()->getHeader()->getId()==9) {
	return 0;
    }

    //######################### count wires per mod #########################
    
    initCounters();    // set conters to 0
    initCountersCal(); // set conters to 0
    countWiresPerMod();// check for less than 4 wires per module
    //#######################################################################


    raw=0;
    cal=0;
    Float_t testTime1=0;
    Float_t testTime2=0;
    Float_t starttime=0;
    Int_t mbo, ch;
    iter->Reset();
    if(StartandCal)starttime=getstarttime();
    while ((raw=(HMdcRaw *)iter->Next())!=0)
    {
	if(!translateAddress(&mbo,&ch))continue;
	if (loc[3]>=0)
	{
	    const Int_t nHits=raw->getNHits();
	    HMdcCalParTdc* tdc=0;
	    if(calparraw)
	    {
		tdc=&(*calparraw)[loc[0]][loc[1]][mbo][ch];
	    }
	    calcTimes(&testTime1,&testTime2,starttime,nHits,tdc);

	    // if time cuts are used
	    if(testTimeCuts(testTime1,testTime2)) // test timecuts + wirestat (if enabled)
	    {
		fillCal1(testTime1,testTime2,nHits);
	    }
	}
	else if (debug)
	{
	    gHades->getMsg()->debug(10,HMessageMgr::DET_MDC,GetName()
				    ,"lookup s=%i m=%i mbo=%2i chan=%2i not connected\n", loc[0],loc[1],mbo,ch);
	}
    }
    if(useMultCut)
    {
	if(doMultCut())
	{
	    if(doprint)
	    {
		printWires();
	    }
            return kSkipEvent;
	}
    }
    return 0;
}

void HMdcCalibrater1::printWires()
{
    cout<<"HMdcCalibrater1-----------------------------"<<endl;
    cout<<"MultiplicityCut-----------------------------"<<endl;
    cout<<"EVT="<<gHades->getEventCounter()<<endl;
    cout<<"Number of wires in Cal1="<<calCat->getEntries()<<endl;
    for(Int_t i=0;i<6;i++){
	printf("s %i : %i %i %i %i\n"
               ,i
	       ,countNrWiresPerModCal[i][0]
	       ,countNrWiresPerModCal[i][1]
	       ,countNrWiresPerModCal[i][2]
	       ,countNrWiresPerModCal[i][3]);
    }
    cout<<"-------------------------------------------"<<endl;
}

