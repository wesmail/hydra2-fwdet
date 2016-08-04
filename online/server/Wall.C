#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "walldef.h"
#include "hwallraw.h"
#include "hwallhit.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > wallMap;

Bool_t createHistWall(TList& histpool){

    mapHolder::setMap(wallMap); // make wallMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
	     "FORMAT#array TYPE#1F NAME#hWallProf1 TITLE#Raw_SmallCellHits ACTIVE#1 RESET#1 REFRESH#5000 BIN#144:0:144:0:0.0:0.0 SIZE#1:2 AXIS#CellNr:Nr_of_Hits:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#array TYPE#1F NAME#hWallProf2 TITLE#Raw_MiddleCellHits ACTIVE#1 RESET#1 REFRESH#5000 BIN#64:144:208:0:0.0:0.0 SIZE#1:2 AXIS#CellNr:Nr_of_Hits:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#array TYPE#1F NAME#hWallProf3 TITLE#Raw_LargeCellHits ACTIVE#1 RESET#1 REFRESH#5000 BIN#92:210:302:0:0.0:0.0 SIZE#1:2 AXIS#CellNr:Nr_of_Hits:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hWallRawTime1 TITLE#Raw_Time ACTIVE#1 RESET#1 REFRESH#5000 BIN#302:0.5:302.5:256:-1500.0:5000.0 SIZE#0:0 AXIS#CellNr:time_[channel]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hWallRawWidth1 TITLE#Raw_Width ACTIVE#1 RESET#1 REFRESH#5000 BIN#302:0.5:302.5:256:0.0:5000.0 SIZE#0:0 AXIS#Fiber:width_[channel]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hWallHitTime TITLE#Hit_Time ACTIVE#1 RESET#1 REFRESH#5000 BIN#305:0:305:250:0:50 SIZE#0:0 AXIS#Cell:Time:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hWallHitCharge TITLE#Hit_Charge ACTIVE#1 RESET#1 REFRESH#5000 BIN#305:0:305:250:0:500 SIZE#0:0 AXIS#Cell:Charge:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#2F NAME#hWallHitXY TITLE#Wall_XY ACTIVE#1 RESET#1 REFRESH#5000 BIN#120:-120:120:120:-120:120 SIZE#0:0 AXIS#X_cm:Y_cm:no DIR#no OPT#box,colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#array TYPE#1F NAME#hWallRawMult TITLE#WallRawMult ACTIVE#1 RESET#1 REFRESH#50000 BIN#100:0.5:100.5:0:0:0 SIZE#1:5 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallTimeSmall TITLE#hWallTimeSmall ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:-500:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallTimeMedium TITLE#hWallTimeMedium ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:-500:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallTimeLarge TITLE#hWallTimeLarge ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:-500:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallADCSmall TITLE#hWallADCSmall ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallADCMedium TITLE#hWallADCMedium ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallADCLarge TITLE#hWallADCLarge ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchTimeSmall TITLE#hWallLatchTimeSmall ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:0:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchTimeMedium TITLE#hWallLatchTimeMedium ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:0:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchTimeLarge TITLE#hWallLatchTimeLarge ACTIVE#1 RESET#1 REFRESH#5000 BIN#1200:0:1200:0:0:0 SIZE#0:0 AXIS#Time:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchADCSmall TITLE#hWallLatchADCSmall ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchADCMedium TITLE#hWallLatchADCMedium ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     ,"FORMAT#mon TYPE#1F NAME#hWallLatchADCLarge TITLE#hWallLatchADCLarge ACTIVE#1 RESET#1 REFRESH#5000 BIN#600:0:600:0:0:0 SIZE#0:0 AXIS#ADC:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	     
 };
    //###############################################################################


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsSector[6] = {2, 4, 6, 8, 38, 46};

    if(get("hWallProf1")) {
	    get("hWallProf1")->getP(0,0)->SetFillColor(46);
	    get("hWallProf1")->getP(0,1)->SetFillColor(38);
    }
    if(get("hWallProf2")) {
	    get("hWallProf2")->getP(0,0)->SetFillColor(46);
	    get("hWallProf2")->getP(0,1)->SetFillColor(38);
    }
    if(get("hWallProf3")) {
	    get("hWallProf3")->getP(0,0)->SetFillColor(46);
	    get("hWallProf3")->getP(0,1)->SetFillColor(38);
    }
    if(get("hWallTimeSmall")) {
	    get("hWallTimeSmall")->getP()->SetFillColor(2);
    }
    if(get("hWallTimeMedium")) {
	    get("hWallTimeMedium")->getP()->SetFillColor(4);
    }
    if(get("hWallTimeLarge")) {
	    get("hWallTimeLarge")->getP()->SetFillColor(6);
    }
    if(get("hWallADCSmall")) {
	    get("hWallADCSmall")->getP()->SetFillColor(2);
    }
    if(get("hWallADCMedium")) {
	    get("hWallADCMedium")->getP()->SetFillColor(4);
    }
    if(get("hWallADCLarge")) {
	    get("hWallADCLarge")->getP()->SetFillColor(6);
    }
    if(get("hWallRawMult") ){
	for(Int_t sec = 0; sec < 5; sec ++){
	    get("hWallRawMult")->getP(0,sec)->SetLineColor(colorsSector[sec]);
	}
    }



    return kTRUE;
}

Bool_t fillHistWall(Int_t evtCt){

    mapHolder::setMap(wallMap); // make wallMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms
    // categorys to loop
    HCategory* wallRawCat = gHades->getCurrentEvent()->getCategory(catWallRaw);
    HCategory* wallHitCat = gHades->getCurrentEvent()->getCategory(catWallHit);

    Int_t nLatchWord = 0;
    Bool_t latchRpc =kFALSE;
    Bool_t latchTof =kFALSE;
    Bool_t latchWall=kFALSE;
    nLatchWord = gHades->getCurrentEvent()->getHeader()->getTBit();
    for(Int_t i=0;i<16;i++){
	if(( nLatchWord & (1<<i))!=0 ){
            if(i==11) latchTof =kTRUE;
            if(i==13) latchRpc=kTRUE;
	    if(i==12 && !latchTof && !latchRpc) latchWall=kTRUE;
	}
    }

    if(wallRawCat){
	HWallRaw* raw;
	Int_t cell,nr;
	Float_t time, width;

	Int_t wallCellArr[5]={0,0,0,0,0};

	for(Int_t i = 0; i < wallRawCat->getEntries(); i ++){
	    raw = (HWallRaw*)wallRawCat->getObject(i);
	    if(raw){
		if(raw->getNHits()<=0) continue;// will never happen

		wallCellArr[4]++;// all
		cell = raw->getCell();
                nr   = raw->getNHits();

		//I quarter -> 1-6;61-66
		if (((cell >= 0  ) && (cell <= 5 )) ||
		    ((cell >= 12 ) && (cell <= 17)) ||
		    ((cell >= 24 ) && (cell <= 29)) ||
		    ((cell >= 36 ) && (cell <= 41)) ||
		    ((cell >= 48 ) && (cell <= 53)) ||
		    ((cell >= 60 ) && (cell <= 64))) wallCellArr[0]++;

		//II quarter -> 7-12;67-72
		if (((cell >= 6  ) && (cell <= 11)) ||
		    ((cell >= 18 ) && (cell <= 23)) ||
		    ((cell >= 30 ) && (cell <= 35)) ||
		    ((cell >= 42 ) && (cell <= 47)) ||
		    ((cell >= 54 ) && (cell <= 59)) ||
		    ((cell >= 67 ) && (cell <= 71))) wallCellArr[1]++;

		//III quarter -> 73-78;133-138
		if (((cell >= 72 )  && (cell <= 76))  ||
		    ((cell >= 84 )  && (cell <= 89))  ||
		    ((cell >= 96 )  && (cell <= 101)) ||
		    ((cell >= 108 ) && (cell <= 113)) ||
		    ((cell >= 120 ) && (cell <= 125)) ||
		    ((cell >= 132 ) && (cell <= 137))) wallCellArr[2]++;

		//IV quarter -> 79-84;139-144
		if (((cell >= 79  ) && (cell <= 83))  ||
		    ((cell >= 90  ) && (cell <= 95))  ||
		    ((cell >= 102 ) && (cell <= 107)) ||
		    ((cell >= 114 ) && (cell <= 119)) ||
		    ((cell >= 126 ) && (cell <= 131)) ||
		    ((cell >= 138 ) && (cell <= 143))) wallCellArr[3]++;




		if( get("hWallProf1") ){
		    get("hWallProf1")->fill(0,0,cell);
		    get("hWallProf1")->getP(0,1)->Fill(65,0.0025);
		    get("hWallProf1")->getP(0,1)->Fill(66,0.0025);
		    get("hWallProf1")->getP(0,1)->Fill(77,0.0025);
		    get("hWallProf1")->getP(0,1)->Fill(78,0.0025);
		}
		if( get("hWallProf2")){
		    get("hWallProf2")->fill(0,0,cell);
		    //hProf2->fill(0,1,144);
		}
		if( get("hWallProf3")){
		    get("hWallProf3")->fill(0,0,cell);
		    get("hWallProf3")->getP(0,1)->Fill(217,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(218,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(219,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(229,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(282,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(292,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(293,0.01);
		    get("hWallProf3")->getP(0,1)->Fill(294,0.01);
		}
		time  = raw->getTime(1);
		width = raw->getWidth(1);
		if(get("hWallRawTime1")) get("hWallRawTime1")->getP()->Fill(cell,time);
		if(get("hWallRawWidth1")) get("hWallRawWidth1")->getP()->Fill(cell,width);
		
	    }
	} // end raw loop

	if(get("hWallRawMult")){
	  for(Int_t j = 0; j < 5; j ++) {
	    get("hWallRawMult")->fill(0,j,wallCellArr[j]);
	  }
	}

    }

    if(wallHitCat){
	HWallHit* hit;
	Int_t cell;
	Float_t x,y,z,time,charge;


	for(Int_t i = 0; i < wallHitCat->getEntries(); i ++){
	    hit = (HWallHit*)wallHitCat->getObject(i);
	    if(hit){

		cell   = hit->getCell();
		time   = hit->getTime();
		charge = hit->getCharge();
		hit->getXYZLab(x,y,z);

		x = x/10;
		y = y/10;
		if(get("hWallHitXY") )    get("hWallHitXY")    ->getP()->Fill(x,y);
		if(get("hWallHitTime") )  get("hWallHitTime")  ->getP()->Fill((Float_t)cell,time);
		if(get("hWallHitCharge")) get("hWallHitCharge")->getP()->Fill((Float_t)cell,charge);

		if(get("hWallADCSmall")&&get("hWallTimeSmall")){
		  if(cell<144){ 
		    get("hWallADCSmall") ->getP()->Fill(charge);
		    get("hWallTimeSmall") ->getP()->Fill(time);
		  }
		}
		if(get("hWallADCMedium")&&get("hWallTimeMedium")){
		  if(cell>143&&cell<210){ 
		    get("hWallADCMedium") ->getP()->Fill(charge);
		    get("hWallTimeMedium") ->getP()->Fill(time);
		  }
		}
		if(get("hWallADCLarge")&&get("hWallTimeLarge")){
		  if(cell>209){ 
		    get("hWallADCLarge") ->getP()->Fill(charge);
		      get("hWallTimeLarge") ->getP()->Fill(time);
		  }
		}
		
		
		if(latchWall){
		  if(get("hWallLatchADCSmall")&&get("hWallLatchTimeSmall")){
		    if(cell<144){ 
		      get("hWallLatchADCSmall") ->getP()->Fill(charge);
		      get("hWallLatchTimeSmall") ->getP()->Fill(time);
		    }
		  }
		  if(get("hWallLatchADCMedium")&&get("hWallLatchTimeMedium")){
		    if(cell>143&&cell<210){ 
		      get("hWallLatchADCMedium") ->getP()->Fill(charge);
		      get("hWallLatchTimeMedium") ->getP()->Fill(time);
		    }
		  }
		  if(get("hWallLatchADCLarge")&&get("hWallLatchTimeLarge")){
		    if(cell>209){ 
		      get("hWallLatchADCLarge") ->getP()->Fill(charge);
		      get("hWallLatchTimeLarge") ->getP()->Fill(time);
		    }
		  }
		}

	    }
	}
    }

    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}



