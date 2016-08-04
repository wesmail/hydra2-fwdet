#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "showerdef.h"
#include "hshowerraw.h"
#include "hshowercal.h"
#include "hshowerhit.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"
#include "TRegexp.h"

#include <map>
#include <iostream>
using namespace std;

#define NEVT_MEAN_SUM 500 //Number of events used to calculate mean Sum

map < TString , HOnlineMonHistAddon* > showerMap;
Float_t meanSumInEvent[6][3][NEVT_MEAN_SUM];

Bool_t createHistShower(TList& histpool) {

    mapHolder::setMap(showerMap); // make showerMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
        "FORMAT#mon TYPE#2F NAME#hShowerhits_raw TITLE#Shower_Raw ACTIVE#1 RESET#1 REFRESH#500 BIN#12:0.0:6.0:6:0.0:3.0 SIZE#1:2 AXIS#sector:module:counts DIR#no OPT#LEGO2 STATS#0 LOG#0:0:1 GRID#0:0 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#mon TYPE#2F NAME#hShowerhits_cal TITLE#Shower_Cal ACTIVE#1 RESET#1 REFRESH#500 BIN#12:0.0:6.0:6:0.0:3.0 SIZE#1:2 AXIS#sector:module:counts DIR#no OPT#LEGO2 STATS#0 LOG#0:0:1 GRID#0:0 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#mon TYPE#2F NAME#hShowerhits_hit TITLE#Shower_Hit ACTIVE#1 RESET#1 REFRESH#500 BIN#12:0.0:6.0:6:0.0:3.0 SIZE#1:2 AXIS#sector:module:counts DIR#no OPT#LEGO2 STATS#0 LOG#0:0:1 GRID#0:0 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#mon TYPE#1F NAME#hShowerCalCol TITLE#Col ACTIVE#1 RESET#1 REFRESH#5000  BIN#600:0:300:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#1 LOG#0:0:0 GRID#1:1 LINE#4:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#mon TYPE#1F NAME#hShowerCalRow TITLE#Row ACTIVE#1 RESET#1 REFRESH#5000  BIN#600:0:300:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#1 LOG#0:0:0 GRID#1:0 LINE#3:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#trend TYPE#1F NAME#hShowerRawMultTrend TITLE#Pre-Shower_multiplicity_RAW ACTIVE#1 RESET#0 REFRESH#100 BIN#100:0:100:0:0:0 SIZE#0:0 AXIS#trend:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:300"
        ,"FORMAT#mon TYPE#1F NAME#hShowerRawMultTemp TITLE#hRawMultTemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:1000:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#trend TYPE#1F NAME#hShowerCalMultTrend TITLE#Pre-Shower_multiplicity_CAL  ACTIVE#1 RESET#0 REFRESH#100 BIN#100:0:100:0:0:0 SIZE#0:0 AXIS#trend:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:300"
        ,"FORMAT#mon TYPE#1F NAME#hShowerCalMultTemp TITLE#hCalMultTemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:1000:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#trend TYPE#1F NAME#hShowerHitMultTrend TITLE#Pre-Shower_multiplicity_HIT  ACTIVE#1 RESET#0 REFRESH#100 BIN#100:0:100:0:0:0 SIZE#0:0 AXIS#trend:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:300"
        ,"FORMAT#mon TYPE#1F NAME#hShowerHitMultTemp TITLE#hHitMultTemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:1000:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#array TYPE#1F NAME#hShowerSums_0 TITLE#Sums_Sector_0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_1 TITLE#Sums_Sector_1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_2 TITLE#Sums_Sector_2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_3 TITLE#Sums_Sector_3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_4 TITLE#Sums_Sector_4 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_5 TITLE#Sums_Sector_5 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:3 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:MOD%i"

        ,"FORMAT#array TYPE#1F NAME#hShowerSums_Pre TITLE#Sums_Pre ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:6 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:SEC%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_Post1 TITLE#Sums_Post_1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:6 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:SEC%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerSums_Post2 TITLE#Sums_Post_2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#80:0:320:0:0:0 SIZE#1:6 AXIS#Charge:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.7:0.9:0.9:0.45:SEC%i"

        ,"FORMAT#array TYPE#1F NAME#hShowerRowsHit_all TITLE#Rows ACTIVE#1 RESET#1 REFRESH#5000 BIN#192:0:192:0:0:0 SIZE#1:3 AXIS#Row:Counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerColsHit_all TITLE#Columns ACTIVE#1 RESET#1 REFRESH#5000 BIN#192:0:192:0:0:0 SIZE#1:3 AXIS#Col:Counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerRowsCal_all TITLE#Rows ACTIVE#1 RESET#1 REFRESH#5000 BIN#192:0:192:0:0:0 SIZE#1:3 AXIS#Row:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#array TYPE#1F NAME#hShowerColsCal_all TITLE#Columns ACTIVE#1 RESET#1 REFRESH#5000 BIN#192:0:192:0:0:0 SIZE#1:3 AXIS#Col:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.3:0.45:0.7:0.15:MOD%i"

        ,"FORMAT#array TYPE#1F NAME#hShowerCalRows_0 TITLE#ROWS_Module_0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Rows:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowerCalRows_1 TITLE#ROWS_Module_1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Rows:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowerCalRows_2 TITLE#ROWS_Module_2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Rows:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowerCalCols_0 TITLE#COLS_Module_0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Columns:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowerCalCols_1 TITLE#COLS_Module_1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Columns:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowerCalCols_2 TITLE#COLS_Module_2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#100:0:31:0:0:0 SIZE#1:6 AXIS#Columns:Counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#array TYPE#1F NAME#hShowerMultTrend TITLE#Sums ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:256:0:0:0 SIZE#1:3 AXIS#modules[1-24]:ratio:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_0 TITLE#SumTrend_0 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_1 TITLE#SumTrend_1 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_2 TITLE#SumTrend_2 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_3 TITLE#SumTrend_3 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_4 TITLE#SumTrend_4 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowerSumTrend_5 TITLE#SumTrend_5 ACTIVE#1 RESET#0 REFRESH#50 BIN#100:0.0:100.0:0:0.0:0.0 SIZE#1:3 AXIS#Sum:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#0:120 LEGEND#0.3:0.45:0.7:0.15:MOD%i"

        ,"FORMAT#array TYPE#1F NAME#hShowerSumTemp TITLE#hSumTemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#2048:0:2048:0:0:0 SIZE#1:18 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"

        ,"FORMAT#mon TYPE#2F NAME#hShowermult_raw TITLE#Shower_hits_raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#6:0.0:3.0:12:0.:6. SIZE#0:0 AXIS#module:sector:no DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#array TYPE#1F NAME#hShowermult_temp_raw TITLE#Shower_hits_raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#1000:0:1000:0:0:0. SIZE#1:6 AXIS#no:no:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowermult_trend_raw TITLE#Shower_hits_raw ACTIVE#1 RESET#0 REFRESH#100 BIN#50:0.0:50.0:0:0.:0. SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#pl STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#0:0 MARKER#0:1:0.5 RANGE#0:70 LEGEND#0.3:0.42:0.7:0.12:SEC%i"
        ,"FORMAT#array TYPE#1F NAME#hShowermult_temp_cal TITLE#Shower_hits_cal ACTIVE#1 RESET#1 REFRESH#5000 BIN#1000:0:1000:0:0:0. SIZE#1:6 AXIS#no:no:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowermult_trend_cal TITLE#Shower_hits_cal ACTIVE#1 RESET#0 REFRESH#100 BIN#50:0.0:50.0:0:0.:0. SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#pl STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#0:0 MARKER#0:1:0.5 RANGE#0:70 LEGEND#0.3:0.42:0.7:0.12:SEC%i"
        ,"FORMAT#array TYPE#1F NAME#hShowermult_temp_hit TITLE#Shower_hits_hit ACTIVE#1 RESET#1 REFRESH#5000 BIN#1000:0:1000:0:0:0. SIZE#1:6 AXIS#no:no:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
        ,"FORMAT#trendarray TYPE#1F NAME#hShowermult_trend_hit TITLE#Shower_hits_hit ACTIVE#1 RESET#0 REFRESH#100 BIN#50:0.0:50.0:0:0.:0. SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#pl STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#0:0 MARKER#0:1:0.5 RANGE#0:70 LEGEND#0.3:0.9:0.7:0.55:SEC%i"


    };
    //###############################################################################


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsSector[6] = {kRed,kBlue,kMagenta,kGreen+2,kBlack,kOrange-3};
    Int_t colorsModule[3] = {2, 3, 4};

    TRegexp regSum            ("hShowerSums_[012345]");
    TRegexp regSumTrend       ("hShowerSumTrend_[012345]");
    TRegexp regCalRows        ("hShowerCalRows_[012345]");
    TRegexp regCalCols        ("hShowerCalCols_[012]");

    for( map< TString, HOnlineMonHistAddon*>::iterator iter = showerMap.begin(); iter != showerMap.end(); ++iter ) {
        TString name               = (*iter).first;
        HOnlineMonHistAddon* addon = (*iter).second;
        if(addon) {

            if(name(regSum) != "") {
                for(Int_t m=0; m<3; m++) {
                    addon->getP(0,m)->SetLineColor(colorsModule[m]);
                }
            }
            if(name(regSumTrend) != "") {
                for(Int_t m=0; m<3; m++) {
                    addon->getP(0,m)->SetLineColor(colorsModule[m]);
                }
            }
            if(name(regCalRows) != "") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regCalCols) != "") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name == "hShowermult_trend_raw") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
//		    addon->getP(0,s)->SetMarkerColor(colorsSector[s]);
                }

            }
            if(name == "hShowermult_trend_cal") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
//                  addon->getP(0,s)->SetMarkerColor(colorsSector[s]);
                }

            }
            if(name == "hShowermult_trend_hit") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
//		    addon->getP(0,s)->SetMarkerColor(colorsSector[s]);
                }

            }
            if(name == "hShowerSums_Pre" || name == "hShowerSums_Post1" || name == "hShowerSums_Post2") {
                for(Int_t s=0; s<6; s++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name == "hShowerRowsHit_all" || name == "hShowerColsHit_all" || name == "hShowerRowsCal_all" || name == "hShowerColsCal_all") {
                for(Int_t m=0; m<3; m++) {
                    addon->getP(0,m)->SetLineColor(colorsModule[m]);
                }
            }


        } //addon
    }  // end loop map


    return kTRUE;
}

Bool_t fillHistShower(Int_t evtCt) {

    mapHolder::setMap(showerMap); // make showerMap currentMap

    //####################### USER CODE ##############################################
    // Fill Histograms


    if(!gHades->isReal()) return kTRUE;

    // categorys to loop
    HCategory* showerCalCat = gHades->getCurrentEvent()->getCategory(catShowerCal);
    HCategory* showerRawCat = gHades->getCurrentEvent()->getCategory(catShowerRaw);
    HCategory* showerHitCat = gHades->getCurrentEvent()->getCategory(catShowerHit);


    if(showerRawCat) {
        Int_t mult_raw[] = { 0, 0, 0, 0, 0, 0 };
        HShowerRaw* raw;
        for(Int_t i = 0; i < showerRawCat->getEntries(); i ++) {
            raw = (HShowerRaw*) showerRawCat->getObject(i);
            if(raw) {
                Int_t loc[4] = {raw->getSector(),raw->getModule(),raw->getRow(),raw->getCol()};

                if(get("hShowerhits_raw") )      get("hShowerhits_raw")     ->getP()->Fill(loc[0],loc[1]);
//                if(get("hShowerRawMultTemp") )  get("hShowerRawMultTemp") ->getP()->Fill(showerRawCat->getEntries());

                if(get("hShowermult_raw") )      get("hShowermult_raw")     ->getP()->Fill(loc[1],loc[0]);
//		if(get("hShowermult_temp_raw") ) get("hShowermult_temp_raw")->getP()->Fill(loc[1],loc[0]);

                mult_raw[loc[0]]++;
            }
        }

        if(get("hShowerRawMultTemp") )  get("hShowerRawMultTemp") ->getP()->Fill(mult_raw[0]+mult_raw[1]+mult_raw[2]+mult_raw[3]+mult_raw[4]+mult_raw[5]);
//        if(get("hShowerRawMultTemp") )  get("hShowerRawMultTemp") ->getP()->Fill(showerRawCat->getEntries());
        if(get("hShowermult_temp_raw")) {
            for (Int_t s = 0; s < 6; ++s) {
                get("hShowermult_temp_raw")->fill(0,s,mult_raw[s]);
            }
        }

        //--------------------RAW TREND----------------------------
        if(get("hShowerRawMultTrend") && get("hShowerRawMultTemp") &&
                evtCt%get("hShowerRawMultTrend")->getRefreshRate() == 0 && evtCt > 0) {
            get("hShowerRawMultTrend")->fill( get("hShowerRawMultTemp")->getP()->GetMean(), get("hShowerRawMultTemp")->getP()->GetRMS());
            get("hShowerRawMultTemp")->getP()->Reset();
        }

        if(get("hShowermult_temp_raw") && get("hShowermult_trend_raw") &&
                evtCt%get("hShowermult_trend_raw")->getRefreshRate() == 0 && evtCt > 0) {
            for(Int_t s=0; s<6; s++) {
                get("hShowermult_trend_raw")->fill(0,s,((TH2*)(get("hShowermult_temp_raw")->getP(0,s)))->GetMean(),((TH2*)(get("hShowermult_temp_raw")->getP(0,s)))->GetRMS());
                get("hShowermult_temp_raw")->getP(0,s)->Reset();
//		get("hShowermult_trend_raw")->fill(0,s,((TH2*)(get("hShowermult_temp_raw")->getP()))->Integral(1,3,s+1,s+1));
            }
//            get("hShowermult_temp_raw")->getP(0,s)->Reset();
        }


    }

    if(showerCalCat) {
        Int_t mult_cal[] = { 0, 0, 0, 0, 0, 0 };

        HOnlineMonHistAddon* mhShowerCalCols[3]= {get("hShowerCalCols_0"),get("hShowerCalCols_1"),get("hShowerCalCols_2")};
        HOnlineMonHistAddon* mhShowerCalRows[3]= {get("hShowerCalRows_0"),get("hShowerCalRows_1"),get("hShowerCalRows_2")};

        HShowerCal* cal;
        for(Int_t i = 0; i < showerCalCat->getEntries(); i ++) {
            cal = (HShowerCal*) showerCalCat->getObject(i);
            if(cal) {
                Int_t loc[4] = {cal->getSector(),cal->getModule(),cal->getRow(),cal->getCol()};

                if(mhShowerCalCols[loc[1]]) mhShowerCalCols[loc[1]]->getP(0,loc[0])->Fill(loc[3]);
                if(mhShowerCalRows[loc[1]]) mhShowerCalRows[loc[1]]->getP(0,loc[0])->Fill(loc[2]);
                if(get("hShowerhits_cal") )   get("hShowerhits_cal")   ->getP()->Fill(loc[0],loc[1]);
                if(get("hShowerCalCol") )     get("hShowerCalCol")     ->getP()->Fill(loc[2]+(loc[0]*32)+50);
                if(get("hShowerCalRow") )     get("hShowerCalRow")     ->getP()->Fill(loc[3]+(loc[0]*32)+50);
//                if(get("hShowerCalMultTemp")) get("hShowerCalMultTemp")->getP()->Fill( showerCalCat->getEntries());
                if(get("hShowerRowsCal_all")) get("hShowerRowsCal_all")->fill(0,loc[1],32*loc[0]+loc[2]);
                if(get("hShowerColsCal_all")) get("hShowerColsCal_all")->fill(0,loc[1],32*loc[0]+loc[3]);

                mult_cal[loc[0]]++;
            }
        }

        if(get("hShowermult_temp_cal")) {
            for (Int_t s = 0; s < 6; ++s) {
                get("hShowermult_temp_cal")->fill(0,s,mult_cal[s]);
            }
        }


        if(get("hShowerCalMultTemp")) get("hShowerCalMultTemp")->getP()->Fill( showerCalCat->getEntries());

        //--------------------- CAL TREND--------------------------------

        if(get("hShowerCalMultTrend") && get("hShowerCalMultTemp") &&
                evtCt%get("hShowerCalMultTrend")->getRefreshRate() == 0 && evtCt > 0) {
            get("hShowerCalMultTrend")->fill(get("hShowerCalMultTemp")->getP()->GetMean(), get("hShowerCalMultTemp")->getP()->GetRMS());
            get("hShowerCalMultTemp")->getP()->Reset();
        }

        if(get("hShowermult_temp_cal") && get("hShowermult_trend_cal") &&
                evtCt%get("hShowermult_trend_cal")->getRefreshRate() == 0 && evtCt > 0) {
            for(Int_t s=0; s<6; s++) {
                get("hShowermult_trend_cal")->fill(0,s,((TH2*)(get("hShowermult_temp_cal")->getP(0,s)))->GetMean(),((TH2*)(get("hShowermult_temp_cal")->getP(0,s)))->GetRMS());
                get("hShowermult_temp_cal")->getP(0,s)->Reset();
//              get("hShowermult_trend_cal")->fill(0,s,((TH2*)(get("hShowermult_temp_cal")->getP()))->Integral(1,3,s+1,s+1));
            }
//            get("hShowermult_temp_cal")->getP(0,s)->Reset();
        }
        
    }


    if(showerHitCat) {
        Int_t mult_hit[] = { 0, 0, 0, 0, 0, 0 };
        HShowerHit* hit;

        HOnlineMonHistAddon* mhShowerSums[6] = {
            get("hShowerSums_0"),get("hShowerSums_1"),get("hShowerSums_2"),
            get("hShowerSums_3"),get("hShowerSums_4"),get("hShowerSums_5")
        };

        HOnlineMonHistAddon* mhShowerSumsModule[3] = {
            get("hShowerSums_Pre"),get("hShowerSums_Post1"),get("hShowerSums_Post2")
        };

        Float_t meanSum[6][3] = {{0.,0.,0.},{0.,0.,0.},{0.,0.,0.},{0.,0.,0.},{0.,0.,0.},{0.,0.,0.}};
        Int_t counter[6][3] = {{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0},{0,0,0}};
//        cout << "Mean Sum: " << meanSum[3][1] << "Counter: " << counter[3][1] << endl;

        for(Int_t i = 0; i < showerHitCat->getEntries(); i ++) {
            hit = (HShowerHit*) showerHitCat->getObject(i);
            if(hit) {
                Float_t sums[3] = {hit->getSum(0)  ,hit->getSum(1)  ,hit->getSum(2)};
                Int_t   loc [4] = {hit->getSector(),hit->getModule(),hit->getRow(),hit->getCol()};
                if(get("hShowerSumTemp")) {
                    for(Int_t m = 0 ; m< 3; m ++) {
                        if(sums[m]) {
                            get("hShowerSumTemp")->getP(0,loc[0]*3+m)->Fill(sums[m]);
                            meanSum[loc[0]][m] += sums[m];
                            counter[loc[0]][m]++;
                        }
                    }
                }
                if(mhShowerSums[loc[0]] && sums[loc[1]]) mhShowerSums[loc[0]]->fill(0,loc[1],sums[loc[1]]);
                if(mhShowerSumsModule[loc[1]] && sums[loc[1]]) mhShowerSumsModule[loc[1]]->fill(0,loc[0],sums[loc[1]]);
                if(get("hShowerhits_hit"))    get("hShowerhits_hit")   ->getP()->Fill(loc[0],loc[1]);
//                if(get("hShowerHitMultTemp")) get("hShowerHitMultTemp")->getP()->Fill(showerHitCat->getEntries());
                if(get("hShowerRowsHit_all")) get("hShowerRowsHit_all")->fill(0,loc[1],32*loc[0]+loc[2]);
                if(get("hShowerColsHit_all")) get("hShowerColsHit_all")->fill(0,loc[1],32*loc[0]+loc[3]);

                mult_hit[loc[0]]++;
            }
        }

        for (Int_t s = 0; s < 6; ++s) {
            for (Int_t m = 0; m < 3; ++m) {
                if (counter[s][m] > 0) {
//                    if (s == 3 && m == 1) cout << "Mean Sum: " << meanSum[s][m] << "Counter: " << counter[s][m] << endl;
                    meanSum[s][m] /= counter[s][m];
//                    if (s == 3 && m == 1) cout << "Mean Sum: " << meanSum[s][m] << endl;
                }
                Int_t evtCtForMeanSum = evtCt % NEVT_MEAN_SUM;
                meanSumInEvent[s][m][evtCtForMeanSum] = meanSum[s][m];

            }
        }

        if(get("hShowermult_temp_hit")) {
            for (Int_t s = 0; s < 6; ++s) {
                get("hShowermult_temp_hit")->fill(0,s,mult_hit[s]);
            }
        }


        if(get("hShowerHitMultTemp")) get("hShowerHitMultTemp")->getP()->Fill(showerHitCat->getEntries());

        //---------------------HIT TREND----------------------------

        if(get("hShowerHitMultTrend") && get("hShowerHitMultTemp") &&
                evtCt%get("hShowerHitMultTrend")->getRefreshRate() == 0 && evtCt > 0) {

            get("hShowerHitMultTrend")->fill(get("hShowerHitMultTemp")->getP()->GetMean(), get("hShowerHitMultTemp")->getP()->GetRMS());
            get("hShowerHitMultTemp")->getP()->Reset();
        }

        if(get("hShowerSumTemp") ) {

            HOnlineMonHistAddon* mhShowerSumTrend[6] = {
                get("hShowerSumTrend_0"),get("hShowerSumTrend_1"), get("hShowerSumTrend_2"),
                get("hShowerSumTrend_3"),get("hShowerSumTrend_4"), get("hShowerSumTrend_5")
            };


            for(Int_t s=0; s<6; s++) {
                for(Int_t m=0; m<3; m++) {
                    if(mhShowerSumTrend[s] &&
                            evtCt%mhShowerSumTrend[s]->getRefreshRate() == 0 && evtCt > 0)
                    {
                        //mhShowerSumTrend[s]->fill(0,m,get("hShowerSumTemp")->getP(0,m+s*3)->GetMean());
                        Float_t mean = 0.0;
                        for (Int_t ev = 0; ev < NEVT_MEAN_SUM; ++ev) {
                            mean += meanSumInEvent[s][m][ev];
                        }
                        mean /= NEVT_MEAN_SUM;
                        mhShowerSumTrend[s]->fill(0,m,mean);

                    }
                }
            }
        }
        
        if(get("hShowermult_temp_hit") && get("hShowermult_trend_hit") &&
                evtCt%get("hShowermult_trend_hit")->getRefreshRate() == 0 && evtCt > 0) {
            for(Int_t s=0; s<6; s++) {
                get("hShowermult_trend_hit")->fill(0,s,((TH2*)(get("hShowermult_temp_hit")->getP(0,s)))->GetMean(),((TH2*)(get("hShowermult_temp_hit")->getP(0,s)))->GetRMS());
                get("hShowermult_temp_hit")->getP(0,s)->Reset();
//              get("hShowermult_trend_hit")->fill(0,s,((TH2*)(get("hShowermult_temp_hit")->getP()))->Integral(1,3,s+1,s+1));
            }
//            get("hShowermult_temp_hit")->getP(0,s)->Reset();
        }


    }

    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}

