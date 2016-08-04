
#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "haddef.h"
#include "htrbnetdebuginfo.h"

#include "hcategory.h"
#include "hstartdef.h"
#include "hstart2raw.h"
#include "hstart2cal.h"

#include "htboxchan.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > startMap;



#define START_MODULES 5
#define START_STRIPS 16


Bool_t createHistStart(TList& histpool){
  
  mapHolder::setMap(startMap); // make startMap currentMap
  
  //####################### USER CODE ##############################################
  // define monitoring hists

  // IF NUMBER OF MODULES OR STRIPS CHANGE , SIZES HAVE TO BE ADAPTED!

  const Char_t* hists[] = {
     "FORMAT#trendarray TYPE#1F NAME#hLatch_Trend TITLE#_hLatch_TBit ACTIVE#1 RESET#0 REFRESH#1000 BIN#50:0:50:0:0:0 SIZE#1:32 AXIS#time:counts:no DIR#no OPT#l STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
     ,"FORMAT#mon TYPE#1F NAME#hLatch_TrendTemp TITLE#_hLatch_TBit ACTIVE#1 RESET#0 REFRESH#500000 BIN#32:0:32:0:0:0 SIZE#0:0 AXIS#TBit:multiplicity:no DIR#no OPT#0 STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
     ,"FORMAT#mon TYPE#1F NAME#hLatch_Norm TITLE#_hLatch_Norm ACTIVE#1 RESET#1 REFRESH#5000 BIN#32:0:32:0:0:0 SIZE#0:0 AXIS#TBit:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#1:0 MARKER#1:20:0.5 RANGE#-99:-99"
     ,"FORMAT#mon TYPE#1F NAME#hLatch_Mult TITLE#_hLatch_Mult ACTIVE#1 RESET#0 REFRESH#5000 BIN#10:0:10:0:0:0 SIZE#0:0 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#1:0 MARKER#1:20:0.5 RANGE#-99:-99"
     ,"FORMAT#trendarray TYPE#1F NAME#hScalers_Trend TITLE#_hScalers_Trend ACTIVE#1 RESET#0 REFRESH#2000 BIN#50:0:50:0:0:0 SIZE#1:135 AXIS#trend:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#mon TYPE#1F NAME#hScalers_TrendTemp TITLE#_hScalers_Int ACTIVE#1 RESET#0 REFRESH#50000 BIN#135:0:135:0:0:0 SIZE#0:0 AXIS#scaler:multiplicity:no DIR#no OPT#0 STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hTrbNetDebug TITLE#hTrbNetDebug ACTIVE#1 RESET#1 REFRESH#50000 BIN#32:0:32 SIZE#1:27 AXIS#Bit:counts:counts DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#1:0 MARKER#1:20:0.5 RANGE#-99:-99"
     // depends on mod+strip
     ,"FORMAT#mon TYPE#2F NAME#hStart_Raw_Mod_Strip TITLE#hStart_Raw_Mod_Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#5:0:4:16:0:16 SIZE#0:0 AXIS#module:strip:counts DIR#no OPT#LEGO2 STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#mon TYPE#2F NAME#hStart_Cal_Mod_Strip TITLE#hStart_Cal_Mod_Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#5:0:4:16:0:16 SIZE#0:0 AXIS#module:strip:counts DIR#no OPT#LEGO2 STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Raw_Mult TITLE#hStart_Raw_Mult ACTIVE#1 RESET#1 REFRESH#5000 BIN#10:0:10:0:0:0 SIZE#5:16 AXIS#nr_of_times:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Raw_Mult_All TITLE#hStart_Raw_Mult_All ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:0:0:0 SIZE#1:5 AXIS#nr_of_strips:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Cal_Mult_All TITLE#hStart_Cal_Mult_All ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:0:0:0 SIZE#1:5 AXIS#nr_of_strips:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Raw_Width TITLE#hStart_Raw_Width ACTIVE#1 RESET#1 REFRESH#5000 BIN#501:-10:5000:0:0:0 SIZE#5:16 AXIS#width:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Raw_Times TITLE#hStart_Raw_Times ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:-40000:20000:0:0:0 SIZE#5:16 AXIS#width:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     ,"FORMAT#array TYPE#1F NAME#hStart_Cal_TimeDiff TITLE#_hStart_Cal_TimeDiff ACTIVE#1 RESET#1 REFRESH#5000 BIN#500:-5000:5000:0:0:0 SIZE#80:80 AXIS#timedifference:counts:no DIR#no OPT#0 STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
     // depends on trip
     ,"FORMAT#mon TYPE#2F NAME#hStart_Cal_XY TITLE#Start_Cal_XY ACTIVE#1 RESET#1 REFRESH#1000 BIN#16:0:16:16:0:16 SIZE#0:0 AXIS#X:Y:counts DIR#no OPT#COLZ STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
     //
     ,"FORMAT#mon TYPE#1F NAME#hStart_EventID TITLE#Event_ID ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:0:0:0 SIZE#0:0 AXIS#no:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:0 LINE#0:0 FILL#2:0 MARKER#0:0:0 RANGE#-99:-99"
  };
    //###############################################################################



    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsLatch[6] = {2,4,6,8};
    Int_t colorsScal[80] = {2,4,6,8,20,24,38,46,    //8
			    2,4,6,8,20,24,38,46,    //16
			    2,4,6,8,38,46,          //22
			    2,4,6,8,38,46,          //28
			    2,4,6,8,                //32
			    2,4,                    //34
			    2,4,6,8,                //38
			    2,4,6,8,20,24,38,46,50, //56
			    1,1,1,1,1,1,1,1,        //64
                            1,1,1,1,1,1,1,1,        //72
                            1,1,1,1,1,1,1,1         //80
    };

    TString titleDebug[27] = {"UNKNOWN", "START", "RICH_1", "RICH_2", "RICH_3",
			      "inner_MDC_1", "inner_MDC_2", "inner_MDC_3",
 			      "inner_MDC_4", "inner_MDC_5", "inner_MDC_6",
			      "outer_MDC_1", "outer_MDC_2", "outer_MDC_3",
 			      "outer_MDC_4", "outer_MDC_5", "outer_MDC_6",
			      "Shower_1","Shower_2","Shower_3","Shower_4","Shower_5","Shower_6",
			      "RPC_1","RPC_2","TOF","WALL"};

    TString titleScaler[70] = {"Start_1","Start_2","Start_3", "Start_4","Start_5","Start_6","Start_7","Start_8",
			       "Veto_1","Veto_2","Veto_3", "Veto_4","Veto_5","Veto_6","Veto_7","Veto_8",
			       "TOF_1","TOF_2","TOF_3", "TOF_4","TOF_5","TOF_6",
			       "RPC_1","RPC_2","RPC_3", "RPC_4","RPC_5","RPC_6",
			       "PT1","PT2","PT3","PT4",
			       "DS_Start_OR","DS_Veto_OR",
			       "DS_Mult_TOFRPC_1","DS_Mult_TOFRPC_2","DS_Mult_TOFRPC_3","DS_Mult_TOFRPC_4","DS_Mult_TOFRPC_5",
			       "DS_Mult_TOFRPC_6","DS_Mult_TOFRPC_7","DS_Mult_TOFRPC_8","DS_Mult_TOFRPC_9",
			       "DS_PT1","DS_PT2","DS_PT3","DS_PT4",
			       "DS_Start_OR","DS_Veto_OR",
			       "ACC_Mult_TOFRPC_1","ACC_Mult_TOFRPC_2","ACC_Mult_TOFRPC_3","ACC_Mult_TOFRPC_4","ACC_Mult_TOFRPC_5",
			       "ACC_Mult_TOFRPC_6","ACC_Mult_TOFRPC_7","ACC_Mult_TOFRPC_8","ACC_Mult_TOFRPC_9",
			       "ACC_PT1","ACC_PT2","ACC_PT3","ACC_PT4","","","","","","","",""};


    TString evtids[]={
        "sim",       //0
	"real",      //1
	"start",     //2
	"stop",      //3
	"",          //4
	"",          //5
	"",          //6
	"",          //7
	"",          //8
	"Mcal" ,    //9
	"SHcal",    //10
	"SHped",    //11
	"RIped",   //12
        "",          //13
	"CTS",   //14
	""           //15
    };

    HOnlineMonHistAddon* addon = 0;

    if((addon = get("hStart_EventID"))) {
	addon->getP()->SetFillColor(colorsLatch[0]);
	for (Int_t i=0;i<addon->getP()->GetNbinsX();i++){
	    addon->getP()->GetXaxis()->SetBinLabel(i+1,evtids[i].Data());
	}
	addon->getP()->LabelsOption("v");
	addon->getP()->GetXaxis()->SetLabelSize(0.05);
    }

    if((addon = get("hLatch_Trend"))) {
	for(Int_t s = 0; s < 4; s ++){
	    addon->getP(0,s)->SetLineColor(colorsLatch[s]);
	    addon->getP(0,s)->SetFillColor(colorsLatch[s]);
	    addon->getP(0,s+11)->SetLineColor(colorsLatch[s]);
	    addon->getP(0,s+11)->SetFillColor(colorsLatch[s]);
	}
    }
    if((addon = get("hLatch_Mult"))) {
	addon->getP()->SetFillColor(colorsLatch[0]);
    }
    if((addon = get("hLatch_Norm"))) {
	addon->getP()->SetFillColor(colorsLatch[1]);
    }
    if((addon = get("hScalers_Trend"))) {
	for(Int_t s = 0; s < 70; s ++){
	    addon->getP(0,s)->SetLineColor(2);
	    addon->getP(0,s)->SetFillColor(4);
	    //addon->getP(0,s)->SetLineColor(colorsScal[s]);
	    //addon->getP(0,s)->SetFillColor(colorsScal[s]);
	    addon->getP(0,s)->SetTitle(titleScaler[s]);
	}
    }
    if(NULL != (addon = get("hStart_Raw_Mult_All")) ){
	for(Int_t m = 0; m < START_MODULES; m ++){
	    addon->getP(0,m)->SetLineWidth(2);
	    addon->getP(0,m)->SetLineColor(colorsScal[m]);
	}
    }
    if(NULL != (addon = get("hStart_Cal_Mult_All")) ){
	for(Int_t m = 0; m < START_MODULES; m ++){
	    addon->getP(0,m)->SetLineWidth(2);
	    addon->getP(0,m)->SetLineColor(colorsScal[m]);
	}
    }

    if((addon = get("hStart_Raw_Mult"))) {
	for(Int_t m = 0; m < START_MODULES; m ++){
	    for(Int_t s = 0; s < START_STRIPS; s ++){
		addon->getP(m,s)->SetLineColor(colorsScal[s+16]);
		addon->getP(m,s)->SetFillColor(colorsScal[s+16]);
	    }
	}
    }

    if((addon = get("hStart_Raw_Width"))) {
	for(Int_t m = 0; m < START_MODULES; m ++){
	    for(Int_t s = 0; s < START_STRIPS; s ++){
		addon->getP(m,s)->SetLineColor(colorsScal[s+16]);
		addon->getP(m,s)->SetFillColor(colorsScal[s+16]);
	    }
	}
    }

    if((addon = get("hStart_Raw_Times"))) {
	for(Int_t m = 0; m < START_MODULES; m ++){
	    for(Int_t s = 0; s < START_STRIPS; s ++){
		addon->getP(m,s)->SetLineColor(colorsScal[s+16]);
		addon->getP(m,s)->SetFillColor(colorsScal[s+16]);
	    }
	}
    }

    if((addon = get("hTrbNetDebug"))) {
	for(Int_t s = 0; s < 27; s ++){
	    addon->getP(0,s)->SetLineColor(1);
	    addon->getP(0,s)->SetFillColor(2);
	    addon->getP(0,s)->SetTitle(titleDebug[s]);
	}
    }


    return kTRUE;
}

Bool_t fillHistStart(Int_t evtCt){

    mapHolder::setMap(startMap); // make wallMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms
    // categorys to loop
    HCategory* startRawCat = gHades->getCurrentEvent()->getCategory(catStart2Raw);
    HCategory* startCalCat = gHades->getCurrentEvent()->getCategory(catStart2Cal);
    HCategory* tboxChanCat = gHades->getCurrentEvent()->getCategory(catTBoxChan);
    HCategory* trbNetCat   = gHades->getCurrentEvent()->getCategory(catTrbNet);
    HEventHeader* header   = gHades->getCurrentEvent()->getHeader();

    if(get("hStart_EventID")) get("hStart_EventID")->getP()->Fill(header->getId()&0xFF);

    Int_t nLatchWord = 0;
    Int_t bScal = 0;
    Int_t ch;
    UInt_t cnt;
    Int_t latchCnt;
    Int_t strip, mod, mult, width,time;
    Int_t strip1, mod1, strip2, mod2 ;
    Float_t width1 =-1;
    Float_t width2 =-1;
    Float_t time1_0, time2_0 ;

    mod=-1;
    // TRB Net Debug Info
    if(trbNetCat){

	HTrbNetDebugInfo* debug;
	Int_t add;
	Int_t hadd;
	Int_t nDebugWord;

	for(Int_t i = 0; i < trbNetCat->getEntries(); i ++){

	    debug      = (HTrbNetDebugInfo*)trbNetCat->getObject(i);
	    add        = debug->getAddress();
	    nDebugWord = debug->getStatusWord();

	    if     (add==0x8800)	hadd = 1;
	    else if(add==0x8300)	hadd = 2;
	    else if(add==0x8310)	hadd = 3;
	    else if(add==0x8320)	hadd = 4;
	    else if(add==0x1000)	hadd = 5;
	    else if(add==0x1010)	hadd = 6;
	    else if(add==0x1020)	hadd = 7;
	    else if(add==0x1030)	hadd = 8;
	    else if(add==0x1040)	hadd = 9;
	    else if(add==0x1050)	hadd = 10;
	    else if(add==0x1100)	hadd = 11;
	    else if(add==0x1110)	hadd = 12;
	    else if(add==0x1120)	hadd = 13;
	    else if(add==0x1130)	hadd = 14;
	    else if(add==0x1140)	hadd = 15;
	    else if(add==0x1150)	hadd = 16;
	    else if(add==0x3200)        hadd = 17;
	    else if(add==0x3210)        hadd = 18;
	    else if(add==0x3220)        hadd = 19;
	    else if(add==0x3230)        hadd = 20;
	    else if(add==0x3240)        hadd = 21;
	    else if(add==0x3250)        hadd = 22;
	    else if(add==0x8400)	hadd = 23;
	    else if(add==0x8410)	hadd = 24;
	    else if(add==0x8600)	hadd = 25;
	    else if(add==0x8700)	hadd = 26;
	    else hadd = 0;

	    if(get("hTrbNetDebug")){
		for(Int_t b = 0; b < 32; b++){
		    if(( nDebugWord & (1<<b))!=0 ){
			if(b!=0){
			    get("hTrbNetDebug")->fill(0,hadd,b);
			}
		    }
		}
	    }
	}
    }

    // LATCH data
    latchCnt = 0;
    nLatchWord = gHades->getCurrentEvent()->getHeader()->getTBit();
    for(Int_t i=0;i<32;i++){
	if(( nLatchWord & (1<<i))!=0 ){
	    if(get("hLatch_TrendTemp"))  get("hLatch_TrendTemp")->getP()->Fill(i);
	    if(get("hLatch_Norm"))  get("hLatch_Norm")->getP()->Fill(i);
	    latchCnt ++ ;
	}
    }
    if(get("hLatch_Mult"))  get("hLatch_Mult")->getP()->Fill(latchCnt);


    if(get("hLatch_TrendTemp")&&get("hLatch_Trend")){
	if(evtCt%get("hLatch_Trend")->getRefreshRate() == 0 && evtCt > 0){
	    for(Int_t bit = 0; bit < 32; bit ++){
		get("hLatch_Trend")->fill(0,bit,get("hLatch_TrendTemp")->getP()->GetBinContent(bit+1)/get("hLatch_Trend")->getRefreshRate());
	    }
	    get("hLatch_TrendTemp")->getP()->Reset();
	}
    }

    //scalers data
    if(tboxChanCat){
	HTBoxChan* scaler;
	for(Int_t i = 0; i < tboxChanCat->getEntries(); i ++){

	    bScal ++;
	    scaler = (HTBoxChan*)tboxChanCat->getObject(i);

	    if(scaler){
		scaler -> getScalerData(ch,cnt);
		if(get("hScalers_TrendTemp")) get("hScalers_TrendTemp")->getP()->Fill(ch,cnt);

	    }
	}

    }

    if(get("hScalers_TrendTemp")&&get("hScalers_Trend")){
	if(bScal > 0 && evtCt > 0){
	    for(Int_t bit = 0; bit < 135; bit ++){
		get("hScalers_Trend")->fill(0,bit,get("hScalers_TrendTemp")->getP()->GetBinContent(bit+1));
	    }
	    get("hScalers_TrendTemp")->getP()->Reset();
	}
    }


    // Start data
    // RAW
    Int_t nrStrips[START_MODULES];
    for(Int_t i=0; i<START_MODULES;i++){ nrStrips[i]=0; }

    if(startRawCat){
	HStart2Raw* raw;
	for(Int_t i = 0; i < startRawCat->getEntries(); i ++){
	    raw = (HStart2Raw*)startRawCat->getObject(i);
	    if(raw){

		mod   = raw->getModule();
		strip = raw->getStrip();
		mult  = raw->getMultiplicity();
		width =(Int_t) raw->getWidth(1);
		time  =(Int_t) raw->getTime(1);

		// avoid reference time
		if(raw->getTime(1) != 0 ) nrStrips[mod]++;

		if(mod < 4 && get("hStart_Raw_Mod_Strip")&&raw->getTime(1) != 0)  ((TH2F*)get("hStart_Raw_Mod_Strip")->getP())->Fill(mod,strip,1);
		if(get("hStart_Raw_Mult"))   get("hStart_Raw_Mult")->getP(mod,strip)->Fill(mult);
		if(get("hStart_Raw_Width"))  get("hStart_Raw_Width")->getP(mod,strip)->Fill(width);
		if(get("hStart_Raw_Times"))  get("hStart_Raw_Times")->getP(mod,strip)->Fill(time);


	    }
	}
    }
    for(Int_t m = 0; m < START_MODULES; m++){
	if(get("hStart_Raw_Mult_All")) get("hStart_Raw_Mult_All")->fill(0,m,nrStrips[m]);
    }

    for(Int_t i=0; i<START_MODULES;i++){ nrStrips[i]=0; }

    // CAL
    if(startCalCat){
	HStart2Cal* cal1;
	HStart2Cal* cal2;
	for(Int_t i = 0; i < startCalCat->getEntries(); i ++){

	    cal1 = (HStart2Cal*)startCalCat->getObject(i);
	    if(cal1){

		mod1      = cal1->getModule();
		strip1    = cal1->getStrip();
		time1_0   = cal1->getTime(1);
		width1    = cal1->getWidth(1);

		// avoid reference time
		if(width1 < 95 ) nrStrips[mod1]++;

		if(mod1 < 4 && width1 < 95 && cal1->getTime(1) != 0 && get("hStart_Cal_Mod_Strip")) {

		    ((TH2F*)get("hStart_Cal_Mod_Strip")->getP())->Fill(mod1,strip1,1);

		}

		for(Int_t j = i + 1; j < startCalCat->getEntries(); j ++){

		    cal2 = (HStart2Cal*)startCalCat->getObject(j);

		    if(cal2){

			mod2      = cal2->getModule();
			strip2    = cal2->getStrip();
			time2_0   = cal2->getTime(1);
			width2    = cal2->getWidth(1);

			if(mod1 < 2 && mod2 < 2
                           && mod1 != mod2
			   && width1 < 95
			   && width2 < 95
                           && fabs(time2_0 - time1_0) < 0.5
			   && get("hStart_Cal_XY")
			  ){

			   if(mod1==0) get("hStart_Cal_XY")->getP()->Fill(strip1,strip2);
		           if(mod1==1) get("hStart_Cal_XY")->getP()->Fill(strip2,strip1);
		        }

			if(get("hStart_Cal_TimeDiff")) get("hStart_Cal_TimeDiff")->getP(mod1*START_STRIPS+strip1,mod2*START_STRIPS+strip2)->Fill(time1_0-time2_0);

		    }
		}
	    }
	}

	for(Int_t m = 0; m < START_MODULES; m++){
	    if(get("hStart_Cal_Mult_All")) get("hStart_Cal_Mult_All")->fill(0,m,nrStrips[m]);
	}
    }


    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}

