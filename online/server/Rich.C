#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "richdef.h"
#include "hrichcal.h"
#include "hrichhit.h"
#include "hrichdirclus.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > richMap;



Bool_t createHistRich(TList& histpool){

    mapHolder::setMap(richMap);   // make richMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
	"FORMAT#trendarray TYPE#1F NAME#hRichcalhitstrend TITLE#_hcalhits_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#50:0:50:0:0:0 SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#l STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#mon TYPE#1F NAME#hRichcalhitstrendtemp TITLE#_hcalhits_trendtemp ACTIVE#1 RESET#0 REFRESH#50000 BIN#6:0:6:0:0:0 SIZE#0:0 AXIS#trend:multiplicity:no DIR#no OPT#p STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#trendarray TYPE#1F NAME#hRichHitstrend TITLE#_hHits_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#50:0:50:0:0:0 SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#l STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#mon TYPE#1F NAME#hRichHitstrendtemp TITLE#_hHits_trendtemp ACTIVE#1 RESET#0 REFRESH#50000 BIN#6:0:6:0:0:0 SIZE#0:0 AXIS#trend:multiplicity:no DIR#no OPT#p STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#trendarray TYPE#1F NAME#hRichClustrend TITLE#_hRichClus_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#50:0:50:0:0:0 SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#l STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#mon TYPE#1F NAME#hRichClustrendtemp TITLE#_hRichClus_trendtemp ACTIVE#1 RESET#0 REFRESH#50000 BIN#6:0:6:0:0:0 SIZE#0:0 AXIS#trend:multiplicity:no DIR#no OPT#p STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#array TYPE#1F NAME#hRichcalhitsCharge TITLE#rich_hcalhits_charge ACTIVE#1 RESET#1 REFRESH#5000 BIN#200:0:2000:0:0:0 SIZE#1:6 AXIS#charge_(ADC_ch.):counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#1:0 MARKER#1:20:0.5 RANGE#-99:-99"
	,"FORMAT#array TYPE#2F NAME#hRichcalhitsColRow TITLE#rich_hcalhits_col_row ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:100:100:0:100 SIZE#1:6 AXIS#col:row:no DIR#no OPT#col STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
    };
    //###############################################################################


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsSector[6] = {2,4,6,8,38,46};
    HOnlineMonHistAddon* addon = 0;
    if((addon = get("hRichcalhitstrend"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    if((addon = get("hRichHitstrend"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    if((addon = get("hRichClustrend"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    if((addon = get("hRichcalhitsCharge"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    return kTRUE;
}

Bool_t fillHistRich(Int_t evtCt){


    mapHolder::setMap(richMap);   // make richMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms

    // categorys to loop
    HCategory* richCalCat  = gHades->getCurrentEvent()->getCategory(catRichCal);
    HCategory* richHitCat  = gHades->getCurrentEvent()->getCategory(catRichHit);
    HCategory* richClusCat = gHades->getCurrentEvent()->getCategory(catRichDirClus);


    // RICH cal
    if(richCalCat){
	HRichCal* cal;
        Int_t s,col,row;
	Float_t charge;
	for(Int_t i = 0; i < richCalCat->getEntries(); i ++){
	    cal = (HRichCal*)richCalCat->getObject(i);

 	    if(cal){

	      s   = cal ->getSector();
	      col = cal -> getCol(); 
	      row = cal -> getRow();
	      charge = cal -> getCharge();

               if(get("hRichcalhitstrendtemp")) get("hRichcalhitstrendtemp") ->getP()->Fill(s);
	       if(get("hRichcalhitsCharge")) get("hRichcalhitsCharge") ->getP(0,s)->Fill(charge);
	       if(get("hRichcalhitsColRow")) ((TH2F*)get("hRichcalhitsColRow") ->getP(0,s))->Fill(col,row);

	    }
	}
    }

    if(get("hRichcalhitstrend")&&get("hRichcalhitstrendtemp")){
      if(evtCt%get("hRichcalhitstrend")->getRefreshRate() == 0 && evtCt > 0){
	for(Int_t sec = 0; sec < 6; sec ++){
	  get("hRichcalhitstrend")->fill(0,sec,get("hRichcalhitstrendtemp")->getP()->GetBinContent(sec+1)/get("hRichcalhitstrend")->getRefreshRate());
	}
        get("hRichcalhitstrendtemp")->getP()->Reset();
      }
    }


    //RICH hit
    if(richHitCat){

	HRichHit* hit;
        Int_t s;

	for(Int_t i = 0; i < richHitCat->getEntries(); i ++){
	    hit = (HRichHit*)richHitCat->getObject(i);
            s = hit ->getSector();
	    if(hit){
               if(get("hRichHitstrendtemp")) get("hRichHitstrendtemp") ->getP()->Fill(s);

	    }
	}
    }

    if(get("hRichHitstrend")&&get("hRichHitstrendtemp")){
      if(evtCt%get("hRichHitstrend")->getRefreshRate() == 0 && evtCt > 0){
	for(Int_t sec = 0; sec < 6; sec ++){
	  get("hRichHitstrend")->fill(0,sec,get("hRichHitstrendtemp")->getP()->GetBinContent(sec+1)/get("hRichHitstrend")->getRefreshRate());
	}
        get("hRichHitstrendtemp")->getP()->Reset();
      }
    }
    
    
    //RICH direct cluster
    if(richClusCat){
	HRichDirClus* clus;
        Int_t s;
	for(Int_t i = 0; i < richClusCat->getEntries(); i ++){
	  clus = (HRichDirClus*)richClusCat->getObject(i);
            s = clus->getSector();
	    if(clus){
	      if(get("hRichClustrendtemp")) get("hRichClustrendtemp") ->getP()->Fill(s);

	    }
	}
    }
    
    if(get("hRichClustrend")&&get("hRichClustrendtemp")){
            if(evtCt%get("hRichClustrend")->getRefreshRate() == 0 && evtCt > 0){
      	for(Int_t sec = 0; sec < 6; sec ++){
	  get("hRichClustrend")->fill(0,sec,get("hRichClustrendtemp")->getP()->GetBinContent(sec+1)/get("hRichClustrend")->getRefreshRate());
	}
        get("hRichClustrendtemp")->getP()->Reset();
      }
    }


    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}

