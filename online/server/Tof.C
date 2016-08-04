
#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "rpcdef.h"
#include "hrpcraw.h"

#include "hcategory.h"
#include "tofdef.h"
#include "htofraw.h"
#include "htofhit.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > tofMap;

Bool_t createHistTof(TList& histpool){

    mapHolder::setMap(tofMap); // make tofMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
	     "FORMAT#array TYPE#1F NAME#hTofhits_raw TITLE#Tof_Raw_Hits ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:6 AXIS#cells:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofmulttof_raw TITLE#Tof_multiplicity ACTIVE#1 RESET#1 REFRESH#5000 BIN#65:-0.5:64.5:0:0.0:0.0 SIZE#0:0 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofpatternL_raw TITLE#Tof_PatternL ACTIVE#1 RESET#1 REFRESH#5000 BIN#385:0.5:385.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#38:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofpatternR_raw TITLE#Tof_PatternR ACTIVE#1 RESET#1 REFRESH#5000 BIN#385:0.5:385.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hToftdcL_raw TITLE#Tof_TdcL ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:256:0:4096 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hToftdcR_raw TITLE#Tof_TdcR ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:256:0:4096 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofadcL_raw TITLE#Tof_AdcL ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:256:0:4096 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofadcR_raw TITLE#Tof_AdcR ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:256:0:4096 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofmult_raw TITLE#Tof_hits_raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#64:0.0:64.0:6:0.:6. SIZE#0:0 AXIS#module*8+cell:sector:no DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofmult_temp_raw TITLE#Tof_hits_raw ACTIVE#1 RESET#0 REFRESH#5000 BIN#64:0.0:64.0:6:0.:6. SIZE#0:0 AXIS#module*8+cell:sector:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#trendarray TYPE#1F NAME#hTofmult_trend_raw TITLE#Tof_hits_raw ACTIVE#1 RESET#0 REFRESH#5000 BIN#50:0.0:50.0:0:0.:0. SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#l STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hToftdcSum_hit TITLE#Tof_TdcSum ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:250:-50:100 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hToftdcDiff_hit TITLE#Tof_TdcDiff ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:250:-5000:5000 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofadcSum_hit TITLE#Tof_AdcSum ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:250:0:15 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofadcPos_hit TITLE#Tof_AdcPos ACTIVE#1 RESET#1 REFRESH#5000 BIN#384:0.5:384.5:250:-5000:5000 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofsum_hit TITLE#Tof_Sum ACTIVE#1 RESET#1 REFRESH#5000 BIN#1500:0.0:150.0:0:0.0:0.0 SIZE#0:0 AXIS#time[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hToftheta_hit TITLE#Tof_theta ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#0:0 AXIS#theta:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#38:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofphi_hit TITLE#Tof_phi ACTIVE#1 RESET#1 REFRESH#5000 BIN#360:0.0:360.0:0:0.0:0.0 SIZE#0:0 AXIS#phi:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hTofyvsx_hit TITLE#Tof_y_vs_x ACTIVE#1 RESET#1 REFRESH#5000 BIN#200:-3000.0:3000.0:200:-3000.0:3000.0 SIZE#0:0 AXIS#y_[mm]:x_[mm]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofmulttofrpc_raw TITLE#Tof_Rpc_multiplicity ACTIVE#1 RESET#1 REFRESH#5000 BIN#89:-0.5:88.5:0:0.0:0.0 SIZE#0:0 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#array TYPE#1F NAME#hRpcTMultiplicity TITLE#hRpcMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#1:4 AXIS#Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#array TYPE#1F NAME#hTofMultiplicity TITLE#hTofMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#1:4 AXIS#Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
             ,"FORMAT#array TYPE#1F NAME#hRpcTofMultiplicity TITLE#hRpcTofMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#1:4 AXIS#Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hTofpattern_hit TITLE#Tof_PatternHit ACTIVE#1 RESET#1 REFRESH#5000 BIN#385:0.5:385.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*64+mod*8+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
    };
    //###############################################################################
    Int_t colorsSector[6] = {2, 4, 6, 8, 38, 46};


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    HOnlineMonHistAddon* addon = 0;
    if( (addon = get("hTofhits_raw") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hTofmult_trend_raw") )) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	    addon->getP(0,s)->SetMarkerColor(colorsSector[s]);
	}
    }
    if((addon = get("hRpcTMultiplicity"))) for(Int_t i = 0; i < 4; i++){
      addon->getP(0,i)->SetLineColor(colorsSector[i]);
      addon->getP(0,i)->SetLineWidth(2);
    }
    if((addon = get("hTofMultiplicity"))) for(Int_t i = 0; i < 4; i++){
      addon->getP(0,i)->SetLineColor(colorsSector[i]);
      addon->getP(0,i)->SetLineWidth(2);
    }
    if((addon = get("hRpcTofMultiplicity"))) for(Int_t i = 0; i < 4; i++){
      addon->getP(0,i)->SetLineColor(colorsSector[i]);
      addon->getP(0,i)->SetLineWidth(2);
    }

    return kTRUE;
}

Bool_t fillHistTof(Int_t evtCt){

    mapHolder::setMap(tofMap); // make tofMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms

    // categorys to loop
    HCategory* tofRawCat = gHades->getCurrentEvent()->getCategory(catTofRaw);
    HCategory* tofHitCat = gHades->getCurrentEvent()->getCategory(catTofHit);
    HCategory* rpcRawCat = gHades->getCurrentEvent()->getCategory(catRpcRaw);

    Int_t nrRpc      = 0;
    Int_t nLatchWord = 0;

    Bool_t latchRpcTof1 = kFALSE;
    Bool_t latchRpcTof2 = kFALSE;
    Bool_t latchRpcTof3 = kFALSE;
    Bool_t latchWall    = kFALSE;

    // Latch Info
    nLatchWord = gHades->getCurrentEvent()->getHeader()->getTBit();
    for(Int_t i=0;i<16;i++){
	if(( nLatchWord & (1<<i))!=0 ){
            if(i==11) latchRpcTof1 =kTRUE;
            if(i==12) latchRpcTof2 =kTRUE;
            if(i==13) latchRpcTof3 =kTRUE;
            if(i==14) latchWall    =kTRUE;
	}
    }

   // RPC data
    if(rpcRawCat){
      HRpcRaw* raw;
	  Float_t lTime = -1.;
      Float_t rTime = -1.;

      for(Int_t i = 0; i < rpcRawCat->getEntries(); i ++){
		raw = (HRpcRaw*)rpcRawCat->getObject(i);
		if(raw){
			lTime = raw->getLeftTime();
	    	rTime = raw->getRightTime();
	  		if(raw->getCell()<32 && rTime > 0 && rTime < 39900)  nrRpc ++;
 			if(raw->getCell()<32 && lTime > 0 && lTime < 39900)  nrRpc ++;
		}
      }
      if(get("hRpcTMultiplicity"))                   get("hRpcTMultiplicity")    ->fill(0,0,(Float_t)nrRpc/2.);
      if(get("hRpcTMultiplicity")&&latchRpcTof1)     get("hRpcTMultiplicity")    ->fill(0,1,(Float_t)nrRpc/2.);
      if(get("hRpcTMultiplicity")&&latchRpcTof2)     get("hRpcTMultiplicity")    ->fill(0,2,(Float_t)nrRpc/2.);
      if(get("hRpcTMultiplicity")&&latchRpcTof3)     get("hRpcTMultiplicity")    ->fill(0,3,(Float_t)nrRpc/2.);
    }
    
    if(tofRawCat){
	HTofRaw* raw;

	Float_t rtime ,ltime ,rcharge ,lcharge ;
        Int_t  s,m,c;
        Int_t rawMult = 0;
	for(Int_t i = 0; i < tofRawCat->getEntries(); i ++){
	    raw = (HTofRaw*)tofRawCat->getObject(i);
	    if(raw){
		rtime   = raw->getRightTime();
		ltime   = raw->getLeftTime();
		rcharge = raw->getRightCharge();
		lcharge = raw->getLeftCharge();
		s       = raw->getSector();
		m       = raw->getModule();
		c       = raw->getCell();

		Int_t itof2= (s*64+m*8+c+1);

		if(get("hTofhits_raw"))     get("hTofhits_raw")    ->fill(0,s,m*8+c);
		if(get("hTofpatternL_raw")&&ltime>0) get("hTofpatternL_raw")->getP()->Fill(itof2);
		if(get("hTofpatternR_raw")&&rtime>0) get("hTofpatternR_raw")->getP()->Fill(itof2);
		if(get("hToftdcL_raw"))     get("hToftdcL_raw")    ->getP()->Fill(itof2,ltime);
		if(get("hToftdcR_raw"))     get("hToftdcR_raw")    ->getP()->Fill(itof2,rtime);
		if(get("hTofadcL_raw"))     get("hTofadcL_raw")    ->getP()->Fill(itof2,lcharge);
		if(get("hTofadcR_raw"))     get("hTofadcR_raw")    ->getP()->Fill(itof2,rcharge);

		if(rtime>10)
		{
		    if(get("hTofmult_raw"))      get("hTofmult_raw")      ->getP()->Fill(m*8+c,s);
		    if(get("hTofmult_temp_raw")) get("hTofmult_temp_raw") ->getP()->Fill(m*8+c,s);
                    rawMult++;
		}
		if(ltime>10)
		{
		    if(get("hTofmult_raw"))      get("hTofmult_raw")      ->getP()->Fill(m*8+c,s);
		    if(get("hTofmult_temp_raw")) get("hTofmult_temp_raw") ->getP()->Fill(m*8+c,s);
                    rawMult++;
		}

	    }

	} // end raw loop

	if(get("hTofMultiplicity"))                   get("hTofMultiplicity")    ->fill(0,0,(Float_t)rawMult/2.);
	if(get("hTofMultiplicity")&&latchRpcTof1)     get("hTofMultiplicity")    ->fill(0,1,(Float_t)rawMult/2.);
	if(get("hTofMultiplicity")&&latchRpcTof2)     get("hTofMultiplicity")    ->fill(0,2,(Float_t)rawMult/2.);
	if(get("hTofMultiplicity")&&latchRpcTof3)     get("hTofMultiplicity")    ->fill(0,3,(Float_t)rawMult/2.);
	
	if(get("hRpcTofMultiplicity"))                   get("hRpcTofMultiplicity")    ->fill(0,0,(Float_t)rawMult/2.+(Float_t)nrRpc/2.);
	if(get("hRpcTofMultiplicity")&&latchRpcTof1)     get("hRpcTofMultiplicity")    ->fill(0,1,(Float_t)rawMult/2.+(Float_t)nrRpc/2.);
	if(get("hRpcTofMultiplicity")&&latchRpcTof2)     get("hRpcTofMultiplicity")    ->fill(0,2,(Float_t)rawMult/2.+(Float_t)nrRpc/2.);
	if(get("hRpcTofMultiplicity")&&latchRpcTof3)     get("hRpcTofMultiplicity")    ->fill(0,3,(Float_t)rawMult/2.+(Float_t)nrRpc/2.);
	
        if(get("hTofmulttof_raw")) get("hTofmulttof_raw")->getP()->Fill(rawMult/2);
	
	//---------------Trend hists raw ----------------------------------
	if(get("hTofmult_temp_raw") && get("hTofmult_trend_raw") &&
	   evtCt%get("hTofmult_trend_raw")->getRefreshRate() == 0 && evtCt > 0){
	    for(Int_t sec = 0; sec < 6; sec ++){
		get("hTofmult_trend_raw")->fill(0,sec,((TH2*)(get("hTofmult_temp_raw")->getP()))->Integral(1,64,sec+1,sec+1));
	    }
	    get("hTofmult_temp_raw")->getP()->Reset();
	}
 	//-----------------------------------------------------------------

    }

    if(tofHitCat){
	HTofHit* hit;

	for(Int_t i = 0; i < tofHitCat->getEntries(); i ++){
	    hit = (HTofHit*)tofHitCat->getObject(i);
	    if(hit){
		Int_t s          = hit->getSector();
		Int_t m          = hit->getModule();
		Int_t c          = hit->getCell();
		Float_t xpos     = hit->getXpos();
		Float_t tof      = hit->getTof();
		Float_t Edep     = hit->getEdep();
		Float_t xposAdc  = hit->getXposAdc();
		Int_t   Adcflag  = hit->getAdcFlag();

		Float_t dist  = 0;
		Float_t theta = 0;
		Float_t phi   = 0;
		Float_t lab_x = 0;
		Float_t lab_y = 0;
		Float_t lab_z = 0;

		hit->getDistance(dist);
		hit->getTheta(theta);
		hit->getPhi(phi);
		hit->getXYZLab(lab_x,lab_y,lab_z);
		Int_t itof2= (s*64+m*8+c+1);

		Float_t corrT = ((dist-2100)/300);
		Float_t time  =  (tof - corrT);

		if(get("hToftdcSum_hit"))  get("hToftdcSum_hit") ->getP()->Fill(itof2,time);
                if(get("hToftdcDiff_hit")) {
                    get("hToftdcDiff_hit")->getP()->Fill(itof2,xpos);
                    if(get("hTofpattern_hit")) get("hTofpattern_hit")->getP()->Fill(itof2);
                }
		if(Adcflag == 2){
		    if(get("hTofadcSum_hit")) get("hTofadcSum_hit")->getP()->Fill(itof2,Edep);
		    if(get("hTofadcPos_hit")) get("hTofadcPos_hit")->getP()->Fill(itof2,xposAdc);
		}
		if(get("hTofsum_hit"))   get("hTofsum_hit")  ->getP()->Fill(time);
		if(get("hToftheta_hit")) get("hToftheta_hit")->getP()->Fill(theta);
		if(get("hTofphi_hit"))   get("hTofphi_hit")  ->getP()->Fill(phi);
		if(get("hTofyvsx_hit"))  get("hTofyvsx_hit") ->getP()->Fill(lab_x,lab_y);

	    }
	}
    }


    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}

