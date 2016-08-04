#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "hstartdef.h"
#include "hstart2raw.h"
#include "hstart2cal.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > hodoMap;

Bool_t createHistHodo(TList& histpool){

    mapHolder::setMap(hodoMap); // make wallMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
	  "FORMAT#array TYPE#1F NAME#hhodoraw TITLE#Raw_strips ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:0:0.0:0.0 SIZE#1:2 AXIS#CellNr:Nr_of_Hits:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	  ,"FORMAT#mon TYPE#2F NAME#hodorawstriptime TITLE#Time_vs_strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#64:0:64:1000:-2000.0:2000.0 SIZE#0:0 AXIS#CellNr:time_[channel]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	  //  ,"FORMAT#array TYPE#1F NAME#hWallRawMult TITLE#WallRawMult ACTIVE#1 RESET#1 REFRESH#50000 BIN#100:0.5:100.5:0:0:0 SIZE#1:5 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	     
 };
    //###############################################################################


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsSector[6] = {2, 4, 6, 8, 38, 46};
    HOnlineMonHistAddon* addon = 0;
    addon = get("hhodoraw");

    for(Int_t i=0;i<2;i++) {
	addon->getP(0,i)->SetLineColor(colorsSector[i]);
		
    }

    return kTRUE;
}

Bool_t fillHistHodo(Int_t evtCt){

    mapHolder::setMap(hodoMap); // make wallMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms
    // categorys to loop
    HCategory* startRawCat = gHades->getCurrentEvent()->getCategory(catStart2Raw);
    HCategory* startCalCat = gHades->getCurrentEvent()->getCategory(catStart2Cal);


    if(startRawCat){
	HStart2Raw* raw;

	for(Int_t i = 0; i < startRawCat->getEntries(); i ++){
	    raw = (HStart2Raw*)startRawCat->getObject(i);
            if(raw&&raw->getModule()==4)
	    {
		Int_t strip = raw->getStrip();
                Int_t time  = raw->getTime(1);
		if(strip<16) get("hhodoraw")->fill(0,0,strip);
		else         get("hhodoraw")->fill(0,1,strip-16);
		get("hodorawstriptime")->getP()->Fill(strip,time);
		
	    }
	} // end raw loop
    }

    if(startCalCat){
	HStart2Cal* cal;

	for(Int_t i = 0; i < startCalCat->getEntries(); i ++){
	    cal = (HStart2Cal*)startCalCat->getObject(i);
	    if(cal){


	    }
	}
    }

    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}



