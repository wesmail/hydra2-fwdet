#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hmdccutstat.h"

#include "hcategory.h"
#include "hmdcdef.h"
#include "hmdcraw.h"
#include "hmdccal1.h"
#include "hmdcoepstatusdata.h"
#include "hmdclookupgeom.h"
#include "hmdclookupraw.h"

#include "helpers.h"


#include "TList.h"
#include "TString.h"
#include "TRegexp.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > mdcMap;
map<Int_t,TString> oepWords;
map<Int_t,Int_t>   oepWordsToLinIndex;

Bool_t createHistMdc(TList& histpool){

    mapHolder::setMap(mdcMap);     // make mdcMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists

    const Char_t* hists[] = {
        "FORMAT#array TYPE#1F NAME#hMdctime1Cal1 TITLE#Mdc_timeCal1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#800:-100:700:0:0:0 SIZE#1:2 AXIS#time_[channel]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#trend TYPE#1F NAME#hMdctime1Cal1MeanTrend TITLE#Mdc_time1Cal1MeanTrend ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#0:0 AXIS#trend:mean_time1[channel]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#mon TYPE#1F NAME#hMdctime1Cal1MeanTrendtemp TITLE#time1Cal1MeanTrendtemp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdccutstat TITLE#Mdc_hcutstat ACTIVE#1 RESET#0 REFRESH#500 BIN#24:0:24:0:0:0 SIZE#1:4 AXIS#no:ratio:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#mon TYPE#2F NAME#hMdccal1hits TITLE#Mdc_hcal1hits ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:4:12:0:6 SIZE#0:0 AXIS#module:sector:no DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#mon TYPE#2F NAME#hMdccal1hitstemp TITLE#Mdc_hcal1hits ACTIVE#1 RESET#1 REFRESH#5000000 BIN#4:0:4:6:0:6 SIZE#0:0 AXIS#module:sector:counts DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdccal1hitstrend TITLE#Mdc_hcal1hits_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#50:0:50:0:0:0 SIZE#6:4 AXIS#trend:multiplicity:no DIR#no OPT#p STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#2F NAME#hMdcrawError_Stat TITLE#Mdc_Raw_Error_Stat ACTIVE#1 RESET#1 REFRESH#5000 BIN#96:0:96:10:0:10 SIZE#1:4 AXIS#:no:error_bit:no DIR#no OPT#COLZ STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS0M0 TITLE#Mdc_layerS0M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS1M0 TITLE#Mdc_layerS1M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS2M0 TITLE#Mdc_layerS2M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS3M0 TITLE#Mdc_layerS3M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS4M0 TITLE#Mdc_layerS4M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS5M0 TITLE#Mdc_layerS5M0 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS0M1 TITLE#Mdc_layerS0M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS1M1 TITLE#Mdc_layerS1M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS2M1 TITLE#Mdc_layerS2M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS3M1 TITLE#Mdc_layerS3M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS4M1 TITLE#Mdc_layerS4M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS5M1 TITLE#Mdc_layerS5M1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS0M2 TITLE#Mdc_layerS0M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS1M2 TITLE#Mdc_layerS1M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS2M2 TITLE#Mdc_layerS2M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS3M2 TITLE#Mdc_layerS3M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS4M2 TITLE#Mdc_layerS4M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS5M2 TITLE#Mdc_layerS5M2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS0M3 TITLE#Mdc_layerS0M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS1M3 TITLE#Mdc_layerS1M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS2M3 TITLE#Mdc_layerS2M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS3M3 TITLE#Mdc_layerS3M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS4M3 TITLE#Mdc_layerS4M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#1F NAME#hMdclayerS5M3 TITLE#Mdc_layerS5M3 ACTIVE#1 RESET#1 REFRESH#5000 BIN#210:0:210:0:0:0 SIZE#1:6 AXIS#wire:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:L%i"
            ,"FORMAT#array TYPE#2F NAME#hMdcmbotdc TITLE#Mdc_mbo_tdc ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:12:0:12 SIZE#6:4 AXIS#mbo:tdc:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#2F NAME#hMdcmbotdcMissing TITLE#Mdc_mbo_tdc ACTIVE#1 RESET#0 REFRESH#5000000 BIN#16:0:16:12:0:12 SIZE#6:4 AXIS#mbo:tdc:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Mod0 TITLE#MdcI_time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Mod1 TITLE#MdcII_time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Mod2 TITLE#MdcIII_time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Mod3 TITLE#MdcIV_time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2Mod0 TITLE#MdcI_time2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time2_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2Mod1 TITLE#MdcII_time2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time2_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2Mod2 TITLE#MdcIII_time2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time2_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2Mod3 TITLE#MdcIV_time2 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-300:1000:0:0:0 SIZE#1:6 AXIS#time2_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12Mod0 TITLE#MdcI_time2-time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-50:400:0:0:0 SIZE#1:6 AXIS#time2-time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12Mod1 TITLE#MdcII_time2-time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-50:400:0:0:0 SIZE#1:6 AXIS#time2-time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12Mod2 TITLE#MdcIII_time2-time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-50:400:0:0:0 SIZE#1:6 AXIS#time2-time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12Mod3 TITLE#MdcIV_time2-time1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-50:400:0:0:0 SIZE#1:6 AXIS#time2-time1_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.98:0.5:S%i"
            ,"FORMAT#array TYPE#2F NAME#hMdctime1_12 TITLE#t2-t1:t1 ACTIVE#1 RESET#1 REFRESH#5000 BIN#225:-50:400:200:0:200 SIZE#6:4 AXIS#time1_[ns]:time2-time1_[ns]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1rawMod0 TITLE#MdcI_time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1rawMod1 TITLE#MdcII_time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1rawMod2 TITLE#MdcIII_time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1rawMod3 TITLE#MdcIV_time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2rawMod0 TITLE#MdcI_time2raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2rawMod1 TITLE#MdcII_time2raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2rawMod2 TITLE#MdcIII_time2raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime2rawMod3 TITLE#MdcIV_time2raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12rawMod0 TITLE#MdcI_time2-time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2-time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12rawMod1 TITLE#MdcII_time2-time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2-time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12rawMod2 TITLE#MdcIII_time2-time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2-time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime12rawMod3 TITLE#MdcIV_time2-time1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:0:0:0 SIZE#1:6 AXIS#time2-time1_[ch]:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#2F NAME#hMdctime1_12raw TITLE#t2-t1:t1raw ACTIVE#1 RESET#1 REFRESH#5000 BIN#256:0:2048:200:0:400 SIZE#6:4 AXIS#time1_[ns]:time2-time1_[ns]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#2F NAME#hMdcMbo TITLE#Mbo_per_plane ACTIVE#1 RESET#1 REFRESH#5000 BIN#6:0:6:16:0:16 SIZE#1:4 AXIS#sector:mbo:no DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1RawMeanTrendMod0 TITLE#Mdc_time1RawMeanTrendMod0 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:2000 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1RawMeanTrendMod1 TITLE#Mdc_time1RawMeanTrendMod1 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:2000 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1RawMeanTrendMod2 TITLE#Mdc_time1RawMeanTrendMod2 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:2000 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1RawMeanTrendMod3 TITLE#Mdc_time1RawMeanTrendMod3 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#0:2000 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1RawMeanTrendMod0temp TITLE#time1RawMeanTrendMod0temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:2000:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1RawMeanTrendMod1temp TITLE#time1RawMeanTrendMod1temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:2000:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1RawMeanTrendMod2temp TITLE#time1RawMeanTrendMod2temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:2000:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1RawMeanTrendMod3temp TITLE#time1RawMeanTrendMod3temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1000:0:2000:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcMboTrendMod0 TITLE#Mbo_counts_per_plane ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcMboTrendMod1 TITLE#Mbo_counts_per_plane ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcMboTrendMod2 TITLE#Mbo_counts_per_plane ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcMboTrendMod3 TITLE#Mbo_counts_per_plane ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#mon TYPE#1F NAME#hMdcMboTrendCount TITLE#Mbo_counts_per_plane ACTIVE#1 RESET#0 REFRESH#50000 BIN#24:0:24:0:0:0 SIZE#0:0 AXIS#trend:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#2F NAME#hMdcmbotdcCalib TITLE#Mdc_mbo_tdc_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#16:0:16:12:0:12 SIZE#6:4 AXIS#mbo:tdc:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcmodtimestatCalibMod0 TITLE#Mdc_Mod_timestat_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:8:0:0:0 SIZE#1:6 AXIS#timeN:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcmodtimestatCalibMod1 TITLE#Mdc_Mod_timestat_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:8:0:0:0 SIZE#1:6 AXIS#timeN:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcmodtimestatCalibMod2 TITLE#Mdc_Mod_timestat_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:8:0:0:0 SIZE#1:6 AXIS#timeN:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcmodtimestatCalibMod3 TITLE#Mdc_Mod_timestat_calib ACTIVE#1 RESET#1 REFRESH#5000 BIN#8:0:8:0:0:0 SIZE#1:6 AXIS#timeN:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1Cal1MeanTrendMod0 TITLE#Mdc_time1Cal1MeanTrendMod0 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1Cal1MeanTrendMod1 TITLE#Mdc_time1Cal1MeanTrendMod1 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1Cal1MeanTrendMod2 TITLE#Mdc_time1Cal1MeanTrendMod2 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#trendarray TYPE#1F NAME#hMdctime1Cal1MeanTrendMod3 TITLE#Mdc_time1Cal1MeanTrendMod3 ACTIVE#1 RESET#0 REFRESH#500 BIN#100:0:100:0:0:0 SIZE#1:6 AXIS#trend:mean_time1[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Cal1MeanTrendMod0temp TITLE#time1Cal1MeanTrendMod0temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Cal1MeanTrendMod1temp TITLE#time1Cal1MeanTrendMod1temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Cal1MeanTrendMod2temp TITLE#time1Cal1MeanTrendMod2temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdctime1Cal1MeanTrendMod3temp TITLE#time1Cal1MeanTrendMod3temp ACTIVE#1 RESET#1 REFRESH#5000  BIN#1200:0:1200:0:0:0 SIZE#1:6 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // mean ToT trend all chambers
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcTotMeanTrend TITLE#ToT_mean_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#100:-50000:0:0:0:0 SIZE#1:24 AXIS#previous_events:mean_time2-time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99  LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcTotMeanTrendtemp TITLE#ToT_trendtemp ACTIVE#1 RESET#1 REFRESH#50000 BIN#1000:0:1000:0:0:0 SIZE#1:24 AXIS#trend:mean_time2-time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // mean ToT NOISE trend all chambers
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcTotMeanNoiseTrend TITLE#ToT_Noise_mean_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#100:-50000:0:0:0:0 SIZE#1:24 AXIS#previous_events:mean_time2-time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99  LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcTotMeanNoiseTrendtemp TITLE#ToT_Noise_trendtemp ACTIVE#1 RESET#1 REFRESH#50000 BIN#1000:0:1000:0:0:0 SIZE#1:24 AXIS#trend:mean_time2-time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // mean time1 trend all chambers
            ,"FORMAT#trendarray TYPE#1F NAME#hMdcTime1Trend TITLE#time1_mean_trend ACTIVE#1 RESET#0 REFRESH#500 BIN#100:-50000:0:0:0:0 SIZE#1:24 AXIS#previous_events:mean_time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99  LEGEND#-1:-1:-1:-1:no "
            ,"FORMAT#array TYPE#1F NAME#hMdcTime1Trendtemp TITLE#Mdc_time1_trendtemp ACTIVE#1 RESET#1 REFRESH#50000 BIN#3000:0:3000:0:0:0 SIZE#1:24 AXIS#trend:mean_time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            //  mean ToT all chambers   and all layers
            ,"FORMAT#array TYPE#1F NAME#hMdcTotMean TITLE#ToT_mean ACTIVE#1 RESET#0 REFRESH#500 BIN#144:0:144:0:0:0 SIZE#1:3 AXIS#no:mean_time2-time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcTotMeantemp TITLE#Mdc_ToT_All ACTIVE#1 RESET#1 REFRESH#5000 BIN#1000:0:1000:0:0:0 SIZE#3:144 AXIS#time2-time1_[ns]:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            //  mean time1 all chambers and all layers
            ,"FORMAT#array TYPE#1F NAME#hMdcTime1Mean TITLE#Time1_mean ACTIVE#1 RESET#0 REFRESH#500 BIN#144:0:144:0:0:0 SIZE#1:3 AXIS#no:mean_time1_[ns]:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcTime1Meantemp TITLE#Time1_mean ACTIVE#1 RESET#1 REFRESH#5000 BIN#5000:0:5000:0:0:0 SIZE#3:144 AXIS#time1_[ns]:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // fired wires/event all chambers   and all layers
            ,"FORMAT#array TYPE#1F NAME#hMdcWiresEventMean TITLE#fired_wires_per_event ACTIVE#1 RESET#0 REFRESH#500 BIN#144:0:144:0:0:0 SIZE#1:3 AXIS#no:fired_wires/event:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hMdcWiresEventMeantemp TITLE#fired_wires_per_event ACTIVE#1 RESET#1 REFRESH#5000 BIN#144:0:144:0:0:0 SIZE#2:3 AXIS#no:fired_wires/event:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // counts in MBO for all chambers
            ,"FORMAT#array TYPE#2F NAME#hMdcMboVsChamber TITLE#Mbo_counts ACTIVE#1 RESET#1 REFRESH#5000 BIN#24:0:24:16:0:16 SIZE#1:3 AXIS#no:mbo:counts DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#mon TYPE#2F NAME#hMdcMboVsChamberCalibration TITLE#Mbo_Calibration_counts ACTIVE#1 RESET#1 REFRESH#5000 BIN#24:0:24:16:0:16 SIZE#0:0 AXIS#no:mbo:counts DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#mon TYPE#2F NAME#hMdcMboVsChamberMissing TITLE#Mbo_per_plane_sector ACTIVE#1 RESET#0 REFRESH#5000000 BIN#24:0:24:16:0:16 SIZE#0:0 AXIS#no:mbo:no DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // counts in DBO for all chambers
            ,"FORMAT#array TYPE#2F NAME#hMdcDboVsChamber TITLE#DBO_counts ACTIVE#1 RESET#1 REFRESH#5000 BIN#24:0:24:96:0:96 SIZE#1:3 AXIS#no:no:counts DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // 2D ToT all chambers
            ,"FORMAT#mon TYPE#2F NAME#hMdcTotVsChamber TITLE#ToT_per_chamber_Time1Cut ACTIVE#1 RESET#1 REFRESH#5000 BIN#24:0:24:300:0:300 SIZE#0:0 AXIS#no:time2-time1_[ns]:no DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            // 2D Time1 all chambers
            ,"FORMAT#mon TYPE#2F NAME#hMdcTime1VsChamber TITLE#Time1_per_chamber_TotCut ACTIVE#1 RESET#1 REFRESH#5000 BIN#24:0:24:1000:0:1000 SIZE#0:0 AXIS#no:time1_[ns]:no DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
    };

    Int_t colorsSector[6] = {kRed,kBlue,kMagenta,kGreen+2,kBlack,kOrange-3};
    Int_t colorsPlane[4] = {kOrange,kBlue,kMagenta,kGreen};
    Int_t styleModule [4] = {20,21,22,23};
    Int_t colorsCuts  [4] = {3,5,4,2};

    const Char_t *mdclabel[24] = {
        "I1","I2","I3","I4","I5","I6",
        "II1","II2","II3","II4","II5","II6",
        "III1","III2","III3","III4","III5","III6",
        "IV1","IV2","IV3","IV4","IV5","IV6"
    };

    /*

    status 03.09.2010
    Name               Code  Content                                                                                        Note
    Basic Information  0x00  "Bit 15 - 0: Internal trigger number, Bit 16: Short MBO, Bit 17: Long MBO, Bit 18: CMS active"
    Token Missing      0x01  "Bit 23 - 0: Number of missing token"       1
    Phys. Triggers     0x02  "Bit 23 - 0: Number of received triggers"   1
    Calib. Triggers    0x03  "Bit 23 - 0: Number of received calibration triggers" 1
    Discarded Hit 1    0x04  "Bit 23 - 0: Number of discarded hit 1 words from TDC due to threshold setting"  1
    Discarded Hit 0    0x05  "Bit 23 - 0: Number of discarded hit 0 words from TDC due to threshold setting"  1
    Discarded Words    0x06  "Bit 23 - 0: Number of discarded words due to limit of words per event"  1
    Truncated Events   0x07  "Bit 23 - 0: Number of truncated events due to limit of words per event" 1
    Single Hit 1       0x08  "Bit 23 - 0: Number of single, double or triple hit 1" 1
    Single Hit 0       0x09  "Bit 23 - 0: Number of single, double or triple hit 0" 1
    Retransmit Req.    0x0A  "Bit 11 - 0: Number of retransmit requests sent, Bit 23 - 12: Number of retransmit requests received" 1
    Dummy Word         0x1E  "Dummy data word. Sent in every event when selected by CCR2 Bit 22 (see table 39). Bit 23 - 16: Lower 8 bit of trigger number. Bit 11 - 0: Word counter"
    Debug Word         0x1F  "Debug word. Sent in every event when selected by CCR2 Bit 30 (see table 39). Bit 15 - 0: Trigger number"

    MDC status words. Note 1: is reset when CCR0 Bit 5 is set. In the 0xE-event, the
    relative counter value since the last 0xE-event is shown. The status words 0x00 to
    0x1D are also available in the address range 0x9100 to 0x911D. Here, the absolute
    counter value is read
    */


    //oepWords[0x00]  = "Basic";
    oepWords[0x01]  = "miss. Tok";
    oepWords[0x02]  = ""; //"phys. Trig";
    oepWords[0x03]  = "cal. Trig";
    oepWords[0x04]  = "disc. Hit1";
    oepWords[0x05]  = "disc. Hit0";
    oepWords[0x06]  = "disc. word";
    oepWords[0x07]  = "trunc. Ev";
    oepWords[0x08]  = "single Hit1";
    oepWords[0x09]  = "single Hit0";
    //oepWords[0x0A]  = "trans Req";
    //oepWords[0x01E] = "dummy word";
    //oepWords[0x01F] = "debug word";

    //oepWordsToLinIndex[0x00]  = 0;
    oepWordsToLinIndex[0x01]  = 0;
    //oepWordsToLinIndex[0x02]  = 1;
    oepWordsToLinIndex[0x03]  = 1;
    oepWordsToLinIndex[0x04]  = 2;
    oepWordsToLinIndex[0x05]  = 3;
    oepWordsToLinIndex[0x06]  = 4;
    oepWordsToLinIndex[0x07]  = 5;
    oepWordsToLinIndex[0x08]  = 6;
    oepWordsToLinIndex[0x09]  = 7;
    //oepWordsToLinIndex[0x0A]  = 9;
    //oepWordsToLinIndex[0x01E] = 10;
    //oepWordsToLinIndex[0x01F] = 11;


    //###############################################################################



    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);



    //----------------------- CUSTOMIZING HISTS--------------------------------
    TRegexp regLayer            ("hMdclayerS[012345]M[0123]");
    TRegexp regTimeStatCalib    ("hMdcmodtimestatCalibMod[0123]");
    TRegexp regTimeMod          ("hMdctime[12]+Mod[0123]");
    TRegexp regTimeRawMod       ("hMdctime[12]+rawMod[0123]");
    TRegexp regMboTrendMod      ("hMdcMboTrendMod[0123]");
    TRegexp regTime1MeanTrendMod("hMdctime1Cal1MeanTrendMod[0123]");
    TRegexp regTime1RawMeanTrendMod("hMdctime1RawMeanTrendMod[0123]");


    HOnlineMonHistAddon* addon = get("hMdcrawError_Stat");
    if(addon){
        for(Int_t m = 0; m < 4; m ++){
            for(Int_t s = 0; s < 6; s ++){
                addon->getP(0,m)->GetXaxis()->SetBinLabel(s*16+1,Form("s%i",s));
                //for(Int_t mb = 0; mb < 16; mb ++){
                //    addon->getP(0,m)->GetXaxis()->SetBinLabel(s*16+mb+1,Form("s%mb%i",s,mb));
                //}
            }
            for(map< Int_t, TString>::iterator it = oepWords.begin(); it != oepWords.end(); ++it){
                TString tmp = (*it).second;
                addon->getP(0,m)->GetYaxis()->SetBinLabel(oepWordsToLinIndex[(*it).first] + 1,Form("%s",tmp.Data()));
            }
            addon->getP(0,m)->LabelsOption("v");
        }
    }


    for( map< TString, HOnlineMonHistAddon*>::iterator iter = mdcMap.begin(); iter != mdcMap.end(); ++iter ) {
        TString name    = (*iter).first;
        addon           = (*iter).second;
        if(addon){
            if( name == "hMdctime1Cal1"){
                addon->getP(0,0)->SetLineColor(1);
                addon->getP(0,1)->SetLineColor(2);
            }
            if( name == "hMdcTotMean" ) {
                TLegend* leg = new TLegend(0.9,0.78,0.98,0.7,"","brNDC");
                leg->AddEntry(get("hMdcTotMean")->getP(0,0),"uncut","lpf");
                leg->AddEntry(get("hMdcTotMean")->getP(0,1),"indiv. Time Cuts","lpf");
                leg->AddEntry(get("hMdcTotMean")->getP(0,2),"noise","lpf");
                get("hMdcTotMean")->setLegend(leg);
                if(get("hMdcWiresEventMean")) get("hMdcWiresEventMean")->setLegend(leg);
                if(get("hMdcTime1Mean")) get("hMdcTime1Mean")->setLegend(leg);
            }
            if( name == "hMdcTotMeanTrend" || name=="hMdcTime1Trend" || name == "hMdcTotMeanNoiseTrend"){
                TLegend* leg = new TLegend(0.9,0.98,0.98,0.1,"","brNDC");
                for(Int_t m = 0; m < 4; m ++){
                    for(Int_t s = 0; s < 6; s ++){
                        addon->getP(0,s+m*6)->SetLineColor(colorsPlane[m]-s);
                        leg->AddEntry(get("hMdcTotMeanTrend")->getP(0,s+m*6),Form("P%i S%i",m,s),"lpf");
                    }
                }
                get("hMdcTotMeanTrend")->setLegend(leg);
                get("hMdcTime1Trend")->setLegend(leg);
                get("hMdcTotMeanNoiseTrend")->setLegend(leg);
            }

            if(name == "hMdcTotMean" || name == "hMdcWiresEventMean"  || name == "hMdcTime1Mean") {
                addon->getP(0,0)->SetLineColor(kBlack);
                addon->getP(0,1)->SetLineColor(kGreen+2);
                addon->getP(0,2)->SetLineColor(kOrange);

                for(Int_t i=0;i<24;i++){
                    addon->getP(0,3)->GetXaxis()->SetBinLabel(6*i+1,mdclabel[i]);
                }
                addon->getP(0,3)->GetXaxis()->SetLabelSize(0.05);
                addon->getP(0,3)->LabelsOption("v");
                addon->getP(0,3)->SetTitleOffset(1.15,"X");
            }
            if( name == "hMdcMboVsChamberCalibration" || name == "hMdcTotVsChamber" || name == "hMdcTime1VsChamber" ) {
                for(Int_t i=0;i<24;i++){
                    addon->getP()->GetXaxis()->SetBinLabel(i+1,mdclabel[i]);
                }
                addon->getP()->GetXaxis()->SetLabelSize(0.05);
                addon->getP()->LabelsOption("v");
                addon->getP()->SetTitleOffset(1.15,"X");
            }
            if(name == "hMdcMboVsChamber" || name == "hMdcDboVsChamber" ) {
                for(Int_t j=0;j<3;j++){
                    for(Int_t i=0;i<24;i++){
                        addon->getP(0,j)->GetXaxis()->SetBinLabel(i+1,mdclabel[i]);
                    }
                    for(Int_t i=0;i<16;i++){
                        if(name == "hMdcDboVsChamber" ) addon->getP(0,j)->GetYaxis()->SetBinLabel(6*i+1,Form("MB %i",i));
                    }
                    addon->getP(0,j)->GetXaxis()->SetLabelSize(0.05);
                    addon->getP(0,j)->LabelsOption("v");
                    addon->getP(0,j)->SetTitleOffset(1.15,"X");
                    TString newTitle = addon->getP(0,j)->GetTitle();
                    if(j==0) newTitle += "_all_Hits";
                    if(j==1) newTitle += "_real_Hits";
                    if(j==2) newTitle += "_Noise";
                    addon->getP(0,j)->SetTitle(newTitle);
                }
            }

            if(name == "hMdccutstat") {
                addon->getP(0,0)->SetFillColor(colorsCuts[0]);
                addon->getP(0,1)->SetFillColor(colorsCuts[1]);
                addon->getP(0,2)->SetFillColor(colorsCuts[2]);
                addon->getP(0,3)->SetFillColor(colorsCuts[3]);

                for(Int_t i=0;i<24;i++){
                    addon->getP(0,4)->GetXaxis()->SetBinLabel(i+1,mdclabel[i]);
                }
                addon->getP(0,4)->GetXaxis()->SetLabelSize(0.05);
                addon->getP(0,4)->LabelsOption("v");
                addon->getP(0,4)->SetTitleOffset(1.15,"X");
            }
            if(name == "hMdccal1hitstrend") {
                for(Int_t s = 0; s < 6; s ++){
                    for(Int_t m = 0; m < 4; m ++){
                        addon->getP(s,m)->SetMarkerColor(colorsSector[s]);
                        addon->getP(s,m)->SetMarkerStyle(styleModule[m]);
                    }
                }
            }
            if(name(regLayer) != "") {
                for(Int_t s = 0; s < 6; s ++){
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regTimeStatCalib) != "") {
                const Char_t *timelabel[8] = {"1","2","3","4","5","6","0","<0"};
                for(Int_t s = 0; s < 6; s ++){
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                    for(Int_t l = 0; l < 8; l ++){
                        addon->getP(0,s)->GetXaxis()->SetBinLabel(l+1,timelabel[l]);
                        addon->getP(0,s)->GetXaxis()->SetLabelSize(0.055);
                    }
                }
            }
            if(name(regTimeMod) != "") {
                for(Int_t s = 0; s < 6; s ++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regTimeRawMod) != "") {
                for(Int_t s = 0; s < 6; s ++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regMboTrendMod) != "") {
                for(Int_t s = 0; s < 6; s ++){
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regTime1MeanTrendMod) != "") {
                for(Int_t s = 0; s < 6; s ++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
            if(name(regTime1RawMeanTrendMod) != "") {
                for(Int_t s = 0; s < 6; s ++) {
                    addon->getP(0,s)->SetLineColor(colorsSector[s]);
                }
            }
        }

    } // end loop map
    //-------------------------------------------------------------------------

    if(get("hMdcmbotdcMissing") && get("hMdcmbotdc")  && get("hMdcMboVsChamberMissing"))
    {

        HMdcLookupGeom* lookupgeom = (HMdcLookupGeom*)gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
        for(Int_t s = 0; s < 6; s ++){
            for(Int_t m = 0; m < 4; m ++){
                if(gHades->getSetup()->getDetector("Mdc")->getModule(s,m) == 0) continue;
                Int_t nMb = (*lookupgeom)[s][m].getSize();
                Int_t ctMb = 0;
                for(Int_t mb = 0; mb < nMb; mb ++){
                    Int_t nTdc = (*lookupgeom)[s][m][mb].getSize();
                    if(nTdc < 0 ) continue;
                    ctMb = mb;
                    for(Int_t t = 0; t < nTdc/8; t ++){
                        for(Int_t ti = 0; ti < 8; ti ++){

                            Int_t c = (*lookupgeom)[s][m][mb][t*8+ti].getNCell();
                            Int_t l = (*lookupgeom)[s][m][mb][t*8+ti].getNLayer();
                            if(c < 0 || l < 0) {
                                if(get("hMdcmbotdcMissing")) get("hMdcmbotdcMissing")->fill(s,m,mb,t);
                            }
                        }
                    }
                    // Fill missing Tdcs
                    Int_t NmissTDC(0);
                    for(Int_t t = nTdc; t < 96; t ++){
                        get("hMdcmbotdcMissing")->fill(s,m,mb,t/8);
                        NmissTDC++;
                    }
                    get("hMdcMboVsChamberMissing")->getP()->SetBinContent(1+m*6+s,1+mb,NmissTDC/96);
                }
                // fill missing Mbos
                for(Int_t mb = nMb; mb < 16; mb ++){
                    /// no missing MBOs in DB all chambers have nMB = 16 !
                }
            }

        }


        HOnlineMonStack* stack;

        for(Int_t s = 0; s < 6; s ++){
            for(Int_t m = 0; m < 4; m ++){
                stack = new HOnlineMonStack(Form("hMdcmbotdcStack[%i][%i]",s,m));
                stack->addToStack(get("hMdcmbotdc")->getP(s,m),"OPT#colz");
                stack->addToStack(get("hMdcmbotdcMissing")->getP(s,m),"MARKER#-1:-1:3.0 OPT#text");
                stack->setLogZ(kTRUE);
                stack->setGridX(kTRUE);
                stack->setGridY(kTRUE);
                mapHolder::addStack(stack,histpool);
            }
        }
        HOnlineMonStack* stackMboReal;
        stackMboReal = new HOnlineMonStack("hMdcMboVsChamberStack");
        stackMboReal->addToStack(get("hMdcMboVsChamber")->getP(0,0),"OPT#colz");
        stackMboReal->addToStack(get("hMdcMboVsChamberMissing")->getP(),"MARKER#-1:-1:3.0 OPT#text");
        stackMboReal->setLogZ(kTRUE);
        stackMboReal->setGridX(kTRUE);
        stackMboReal->setGridY(kTRUE);
        mapHolder::addStack(stackMboReal,histpool);

        HOnlineMonStack* stackMboCal;
        stackMboCal = new HOnlineMonStack("hMdcMboVsChamberCalibrationStack");
        stackMboCal->addToStack(get("hMdcMboVsChamberCalibration")->getP(),"OPT#colz");
        stackMboCal->addToStack(get("hMdcMboVsChamberMissing")->getP(),"MARKER#-1:-1:3.0 OPT#text");
        stackMboCal->setLogZ(kTRUE);
        stackMboCal->setGridX(kTRUE);
        stackMboCal->setGridY(kTRUE);
        mapHolder::addStack(stackMboCal,histpool);
    }

    return kTRUE;
}

Bool_t fillHistMdc(Int_t evtCt){



    mapHolder::setMap(mdcMap);     // make mdcMap currentMap

    //####################### USER CODE ##############################################
    // Fill Histograms

    // categorys to loop
    HCategory* mdcCal1Cat    = gHades->getCurrentEvent()->getCategory(catMdcCal1);
    HCategory* mdcRawCat     = gHades->getCurrentEvent()->getCategory(catMdcRaw);
    HCategory* mdcOepStatCat = gHades->getCurrentEvent()->getCategory(catMdcOepStatus);
    // lookup Table:
    HMdcLookupRaw* flookupRaw=(HMdcLookupRaw*)gHades -> getRuntimeDb()->getContainer("MdcLookupRaw");

    const Float_t TotMinCut[4][6]  = {
        {40, 40, 40, 40, 40, 40},
        {40, 40, 40, 40, 40, 40},
        {30, 30, 30, 30, 30, 30},
        {30, 30, 30, 30, 30, 30},
    };
    ///drift time cuts:
    //     const Float_t Cal1Time1Max[4][6] = {   // active
    //  { 50,  50,  50,  50,  50,  50},
    //  {100, 100, 100, 100, 100, 100},
    //  {200, 200, 200, 200, 200, 200},
    //  {200, 200, 200, 200, 200, 200}
    //     };
    const Float_t Cal1Time1Max[4][6] = {   // inactive
        { 50000,  50000,  50000,  50000,  50000,  50000},
        { 50000,  50000,  50000,  50000,  50000,  50000},
        { 50000,  50000,  50000,  50000,  50000,  50000},
        { 50000,  50000,  50000,  50000,  50000,  50000}
    };

    const Int_t TDCtoDBOmapping[12] = {0,1,0,1,2,3,2,3,4,5,4,5};

    if(mdcOepStatCat)
    {
        Int_t word,s,m,mb;
        HMdcOepStatusData *oepStat;

        for(Int_t i = 0; i < mdcOepStatCat->getEntries(); i ++)
        {
            oepStat = (HMdcOepStatusData*)mdcOepStatCat->getObject(i);
            if(oepStat)
            {
                for(Int_t w = 0; w < 32; w++){

                    map<Int_t,TString>::iterator iter = oepWords.find(w);
                    if( iter == oepWords.end() ) continue;  // code not in the list of filled codes

                    word = oepStat->getStatusWord(w);
                    if(word > 0){
                        s  = oepStat->getSector();
                        m  = oepStat->getModule();
                        mb = oepStat->getMbo();
                        if(get("hMdcrawError_Stat")) ((TH2*)get("hMdcrawError_Stat")->getP(0,m))->Fill(s*16+mb,oepWordsToLinIndex[w],word);
                    }
                }
            }
        }


    }


    if(mdcRawCat)
    {

        HOnlineMonHistAddon*  mhMdctime1rawMod[4] = {
            get("hMdctime1rawMod0"),
            get("hMdctime1rawMod1"),
            get("hMdctime1rawMod2"),
            get("hMdctime1rawMod3")
        };
        HOnlineMonHistAddon*  mhMdctime2rawMod[4] = {
            get("hMdctime2rawMod0"),
            get("hMdctime2rawMod1"),
            get("hMdctime2rawMod2"),
            get("hMdctime2rawMod3")
        };
        HOnlineMonHistAddon*  mhMdctime12rawMod[4] = {
            get("hMdctime12rawMod0"),
            get("hMdctime12rawMod1"),
            get("hMdctime12rawMod2"),
            get("hMdctime12rawMod3")
        };


        HOnlineMonHistAddon* mhMdcmodtimestatCalib[4] = {
            get("hMdcmodtimestatCalibMod0"),
            get("hMdcmodtimestatCalibMod1"),
            get("hMdcmodtimestatCalibMod2"),
            get("hMdcmodtimestatCalibMod3")
        };

        HOnlineMonHistAddon* mhMdctime1RawMeanTrendMod[4] = {
            get("hMdctime1RawMeanTrendMod0"),
            get("hMdctime1RawMeanTrendMod1"),
            get("hMdctime1RawMeanTrendMod2"),
            get("hMdctime1RawMeanTrendMod3")
        };
        HOnlineMonHistAddon* mhMdctime1RawMeanTrendModtemp[4] = {
            get("hMdctime1RawMeanTrendMod0temp"),
            get("hMdctime1RawMeanTrendMod1temp"),
            get("hMdctime1RawMeanTrendMod2temp"),
            get("hMdctime1RawMeanTrendMod3temp")
        };

        Int_t s,m,mb,ch;
        HMdcRaw* raw;
        for(Int_t i = 0; i < mdcRawCat->getEntries(); i ++)
        {
            raw = (HMdcRaw*)mdcRawCat->getObject(i);
            if(raw)
            {
                raw->getAddress(s,m,mb,ch);
                Float_t time1=raw->getTime(1);
                Float_t time2=raw->getTime(2);

                if(gHades->isReal()){
                    if(get("hMdcmbotdc"))        get("hMdcmbotdc")        ->getP(s,m)->Fill((Axis_t)mb,(Axis_t)((Int_t)(ch/8)));
                    if(get("hMdcMboTrendCount")) get("hMdcMboTrendCount") ->getP()->Fill(s*4+m);
                    if(mhMdctime1RawMeanTrendModtemp[m]) mhMdctime1RawMeanTrendModtemp[m]->getP(0,s)->Fill(time1);

                    if(mhMdctime1rawMod [m]) mhMdctime1rawMod [m] ->getP(0,s)->Fill(time1);
                    if(mhMdctime2rawMod [m]) mhMdctime2rawMod [m] ->getP(0,s)->Fill(time2);
                    if(mhMdctime12rawMod[m]) mhMdctime12rawMod[m] ->getP(0,s)->Fill(time1-time2);

                    if(get("hMdctime1_12raw")) get("hMdctime1_12raw")->fill(s,m,time1,time1-time2);
                    if(get("hMdcMbo"))         get("hMdcMbo")        ->fill(0,m,s,mb);

                }

                //--------------------------- calibration --------------------------
                if(gHades->isCalibration()){

                    if(get("hMdcmbotdcCalib")) get("hMdcmbotdcCalib")->getP(s,m)->Fill((Axis_t)mb,(Axis_t)((Int_t)(ch/8)));
                    if(get("hMdcMboVsChamberCalibration"))  get("hMdcMboVsChamberCalibration")->getP() ->Fill(s+6*m,mb);
                    if(mhMdcmodtimestatCalib[m])
                    {
                        for(Int_t i = 0; i < 6; i++)
                        {
                            if      (raw->getTime(i+1) >  0){ mhMdcmodtimestatCalib[m]->getP(0,s)->Fill(i);}
                            else if (raw->getTime(i+1) == 0){ mhMdcmodtimestatCalib[m]->getP(0,s)->Fill(6);}
                            else                            { mhMdcmodtimestatCalib[m]->getP(0,s)->Fill(7);}
                        }
                    }
                }
                //------------------------------------------------------------------
            }
        } // end raw loop

        //---------------Trend hists raw----------------------------------


        HOnlineMonHistAddon*  mhMdcMboTrendMod[4] = {
            get("hMdcMboTrendMod0"),
            get("hMdcMboTrendMod1"),
            get("hMdcMboTrendMod2"),
            get("hMdcMboTrendMod3")
        };

        if(mhMdcMboTrendMod[0] && mhMdcMboTrendMod[1] && mhMdcMboTrendMod[2] && mhMdcMboTrendMod[3] &&
           get("hMdcMboTrendCount") ){
            for(Int_t sec = 0; sec < 6; sec ++){
                for(Int_t mod = 0; mod < 4; mod ++){
                    if(evtCt%mhMdcMboTrendMod[mod]->getRefreshRate() == 0 && evtCt > 0){
                        mhMdcMboTrendMod[mod]->fill(0,sec,get("hMdcMboTrendCount")->getP()->GetBinContent(sec*4+mod+1));
                    }
                    get("hMdcMboTrendCount")->getP()->SetBinContent(sec*4+mod+1,0);
                }
            }
        }

        for(Int_t mod = 0; mod < 4; mod ++){
            if( !mhMdctime1RawMeanTrendMod[mod] || !mhMdctime1RawMeanTrendModtemp[mod]) continue;
            if(evtCt%mhMdctime1RawMeanTrendMod[mod]->getRefreshRate() != 0 && evtCt > 0) continue;

            for(Int_t sec = 0; sec < 6; sec ++){
                mhMdctime1RawMeanTrendMod[mod]->fill(0,sec,mhMdctime1RawMeanTrendModtemp[mod]->getP(0,sec)->GetMean());
                mhMdctime1RawMeanTrendModtemp[mod]->getP(0,sec)->Reset();
            }

        }



        //-------------------------------------------------------------




    }





    if(mdcCal1Cat){
        HMdcCal1* cal1;
        Int_t s,m,l,c;
        s=m=l=c=0;
        Float_t time1,time2;
        //------------------------ group hists for easy filling -----------------
        HOnlineMonHistAddon* mhMdctime1Cal1MeanTrendMod[4] = {
            get("hMdctime1Cal1MeanTrendMod0"),
            get("hMdctime1Cal1MeanTrendMod1"),
            get("hMdctime1Cal1MeanTrendMod2"),
            get("hMdctime1Cal1MeanTrendMod3")
        };
        HOnlineMonHistAddon* mhMdctime1Cal1MeanTrendModtemp[4] = {
            get("hMdctime1Cal1MeanTrendMod0temp"),
            get("hMdctime1Cal1MeanTrendMod1temp"),
            get("hMdctime1Cal1MeanTrendMod2temp"),
            get("hMdctime1Cal1MeanTrendMod3temp")
        };

        HOnlineMonHistAddon* mhMdctime1Mod[4] = {
            get("hMdctime1Mod0"),
            get("hMdctime1Mod1"),
            get("hMdctime1Mod2"),
            get("hMdctime1Mod3")
        };
        HOnlineMonHistAddon* mhMdctime2Mod[4] = {
            get("hMdctime2Mod0"),
            get("hMdctime2Mod1"),
            get("hMdctime2Mod2"),
            get("hMdctime2Mod3")
        };
        HOnlineMonHistAddon* mhMdctime12Mod[4] = {
            get("hMdctime12Mod0"),
            get("hMdctime12Mod1"),
            get("hMdctime12Mod2"),
            get("hMdctime12Mod3")
        };

        HOnlineMonHistAddon* mhMdclayer[6][4] = {
            {get("hMdclayerS0M0"),get("hMdclayerS0M1"),get("hMdclayerS0M2"),get("hMdclayerS0M3") },
            {get("hMdclayerS1M0"),get("hMdclayerS1M1"),get("hMdclayerS1M2"),get("hMdclayerS1M3") },
            {get("hMdclayerS2M0"),get("hMdclayerS2M1"),get("hMdclayerS2M2"),get("hMdclayerS2M3") },
            {get("hMdclayerS3M0"),get("hMdclayerS3M1"),get("hMdclayerS3M2"),get("hMdclayerS3M3") },
            {get("hMdclayerS4M0"),get("hMdclayerS4M1"),get("hMdclayerS4M2"),get("hMdclayerS4M3") },
            {get("hMdclayerS5M0"),get("hMdclayerS5M1"),get("hMdclayerS5M2"),get("hMdclayerS5M3") }
        };
        //-----------------------------------------------------------------------


        for(Int_t i = 0; i < mdcCal1Cat->getEntries(); i ++){
            cal1 = (HMdcCal1*)mdcCal1Cat->getObject(i);
            if(cal1 && gHades->isReal()){
                cal1->getAddress(s,m,l,c);
                time1 = cal1->getTime1();
                time2 = cal1->getTime2();

                // lookup mbo and tdc
                HMdcLookupRMod  * flookupRMod = (flookupRaw) ? &(*flookupRaw)[s][m] :0;
                HMdcLookupLayer* fLay=&(*flookupRMod)[l];
                HMdcLookupCell* fcell  = &(*fLay)[c];
                Int_t mb = fcell->getNMoth();
                Int_t ch  = fcell->getNChan();
                if(get("hMdcMboVsChamber"))     {
                    get("hMdcMboVsChamber")   ->getP(0,0)   ->Fill(s+6*m,mb);
                    if( (time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]  ) get("hMdcMboVsChamber")   ->getP(0,1)   ->Fill(s+6*m,mb);
                    else get("hMdcMboVsChamber")   ->getP(0,2)   ->Fill(s+6*m,mb);
                }
                if(get("hMdcDboVsChamber"))     {
                    Int_t db = TDCtoDBOmapping[ch/8];
                    get("hMdcDboVsChamber")   ->getP(0,0)   ->Fill(s+6*m,mb*6+db);
                    if( (time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]  ) get("hMdcDboVsChamber")   ->getP(0,1)   ->Fill(s+6*m,mb*6+db);
                    else get("hMdcDboVsChamber")   ->getP(0,2)   ->Fill(s+6*m,mb*6+db);
                }



                get("hMdccal1hits")              ->getP()->Fill(m,s);
                get("hMdctime1Cal1MeanTrendtemp")->getP()->Fill(time1);
                if(mhMdctime1Cal1MeanTrendModtemp[m]) mhMdctime1Cal1MeanTrendModtemp[m]->getP(0,s)->Fill(time1);

                if(get("hMdctime1Cal1")) {
                    get("hMdctime1Cal1")->getP(0,0)->Fill(time1);
                    get("hMdctime1Cal1")->getP(0,1)->Fill(time2);
                }

                if(get("hMdccal1hitstemp")) get("hMdccal1hitstemp")->getP()->Fill(m,s);
                if(mhMdclayer[s][m])  mhMdclayer[s][m]  ->getP(0,l)->Fill(c);

                if(mhMdctime1Mod[m])  mhMdctime1Mod[m]  ->getP(0,s)->Fill(time1);
                if(mhMdctime2Mod[m])  mhMdctime2Mod[m]  ->getP(0,s)->Fill(time2);
                if(mhMdctime12Mod[m]) mhMdctime12Mod[m] ->getP(0,s)->Fill(time2-time1);

                if(get("hMdctime1_12")) get("hMdctime1_12")->fill(s,m,time1,time2-time1);


                if(get("hMdcTotMeanTrendtemp")) {
                    if((time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]  ) get("hMdcTotMeanTrendtemp")->getP(0,s+m*6)->Fill(time2-time1);
                }
                if(get("hMdcTotMeanNoiseTrendtemp")) {
                    if((time2-time1) < TotMinCut[m][s] ) get("hMdcTotMeanNoiseTrendtemp")->getP(0,s+m*6)->Fill(time2-time1);
                }
                if(get("hMdcTime1Trendtemp")) {
                    if((time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s] )     get("hMdcTime1Trendtemp")->getP(0,s+m*6)->Fill(time1);
                }
                if(get("hMdcTotMeantemp")) {
                    get("hMdcTotMeantemp")->getP(0,l+s*6+m*36)->Fill(time2-time1);
                    if((time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]  ) get("hMdcTotMeantemp")->getP(1,l+s*6+m*36)->Fill(time2-time1);
                    if((time2-time1) < TotMinCut[m][s]) get("hMdcTotMeantemp")->getP(2,l+s*6+m*36)->Fill(time2-time1);
                }
                if(get("hMdcTime1Meantemp")) {
                    get("hMdcTime1Meantemp")->getP(0,l+s*6+m*36)->Fill(time1);
                    if((time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]) get("hMdcTime1Meantemp")->getP(1,l+s*6+m*36)->Fill(time1);
                    if((time2-time1) < TotMinCut[m][s] ) get("hMdcTime1Meantemp")->getP(2,l+s*6+m*36)->Fill(time1);
                }
                if(get("hMdcWiresEventMeantemp")) {
                    get("hMdcWiresEventMeantemp")->getP(0,0)->Fill(l+s*6+m*36);
                    if((time2-time1) > TotMinCut[m][s]
                       && time1 < Cal1Time1Max[m][s]  ) get("hMdcWiresEventMeantemp")->getP(0,1)->Fill(l+s*6+m*36);
                    else get("hMdcWiresEventMeantemp")->getP(0,2)->Fill(l+s*6+m*36);
                }
                if(get("hMdcTotVsChamber")) {
                    if ( time1 < Cal1Time1Max[m][s] ) get("hMdcTotVsChamber")->getP()->Fill(s+m*6,time2-time1);
                }
                if(get("hMdcTime1VsChamber")) {
                    if ( (time2-time1) > TotMinCut[m][s] ) get("hMdcTime1VsChamber")->getP()->Fill(s+m*6,time1);
                }
            }
        }
        // Eventcounter for plotting fired wires per event
        if(get("hMdcWiresEventMeantemp")) { get("hMdcWiresEventMeantemp")->getP(1,0)->Fill(1);  }

        //---------------Trend hists cal1----------------------------------
        if(get("hMdctime1Cal1MeanTrend") && get("hMdctime1Cal1MeanTrendtemp") &&
           evtCt%get("hMdctime1Cal1MeanTrend")->getRefreshRate() == 0 && evtCt > 0){
            get("hMdctime1Cal1MeanTrend")->fill(get("hMdctime1Cal1MeanTrendtemp")->getP()->GetMean());
            get("hMdctime1Cal1MeanTrendtemp")->getP()->Reset();
        }


        if(get("hMdccal1hitstemp") && get("hMdccal1hitstrend") &&
           evtCt%get("hMdccal1hitstrend")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    get("hMdccal1hitstrend")->fill(sec,mod,get("hMdccal1hitstemp")->getP()->GetBinContent(mod+1,sec+1));
                    get("hMdccal1hitstemp")->getP()->SetBinContent(mod+1,sec+1,0);
                }
            }
        }

        if(get("hMdcTotMeanTrendtemp") && get("hMdcTotMeanTrend") &&
           evtCt%get("hMdcTotMeanTrend")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    get("hMdcTotMeanTrend")->fill(0,sec+6*mod,(get("hMdcTotMeanTrendtemp")->getP(0,sec+6*mod)->GetMean()) );
                    get("hMdcTotMeanTrendtemp")->getP(0,sec+6*mod)->Reset();
                }
            }
        }
        if(get("hMdcTotMeanNoiseTrendtemp") && get("hMdcTotMeanNoiseTrend") &&
           evtCt%get("hMdcTotMeanNoiseTrend")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    get("hMdcTotMeanNoiseTrend")->fill(0,sec+6*mod,(get("hMdcTotMeanNoiseTrendtemp")->getP(0,sec+6*mod)->GetMean()) );
                    get("hMdcTotMeanNoiseTrendtemp")->getP(0,sec+6*mod)->Reset();
                }
            }
        }
        if(get("hMdcTime1Trendtemp") && get("hMdcTime1Trend") &&
           evtCt%get("hMdcTime1Trend")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    get("hMdcTime1Trend")->fill(0,sec+6*mod,(get("hMdcTime1Trendtemp")->getP(0,sec+6*mod)->GetMean()) );
                    get("hMdcTime1Trendtemp")->getP(0,sec+6*mod)->Reset();
                }
            }
        }
        if(get("hMdcTotMeantemp") && get("hMdcTotMean") &&
           evtCt%get("hMdcTotMean")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    for(Int_t lay = 0; lay < 6; lay ++){
                        for(Int_t icut = 0; icut < 3; icut ++){
                            get("hMdcTotMean")->getP(0,icut)->SetBinContent(1+lay+6*sec+36*mod,(get("hMdcTotMeantemp")->getP(icut,lay+6*sec+36*mod)->GetMean()) );
                            get("hMdcTotMeantemp")->getP(icut,lay+6*sec+36*mod)->Reset();
                        }
                    }
                }
            }
        }
        if(get("hMdcTime1Meantemp") && get("hMdcTime1Mean") &&
           evtCt%get("hMdcTime1Mean")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t mod = 0; mod < 4; mod ++){
                for(Int_t sec = 0; sec < 6; sec ++){
                    for(Int_t lay = 0; lay < 6; lay ++){
                        for(Int_t icut = 0; icut < 3; icut ++){
                            get("hMdcTime1Mean")->getP(0,icut)->SetBinContent(1+lay+6*sec+36*mod,(get("hMdcTime1Meantemp")->getP(icut,lay+6*sec+36*mod)->GetMean()) );
                            get("hMdcTime1Meantemp")->getP(icut,lay+6*sec+36*mod)->Reset();
                        }
                    }
                }
            }
        }
        if(get("hMdcWiresEventMeantemp") && get("hMdcWiresEventMean") &&
           evtCt%get("hMdcWiresEventMean")->getRefreshRate() == 0 && evtCt > 0){
            for(Int_t icut = 0; icut < 3; icut ++){
                get("hMdcWiresEventMean")->getP(0,icut)->Reset();
                get("hMdcWiresEventMean")->getP(0,icut)->Add(get("hMdcWiresEventMeantemp")->getP(0,icut), 1.0/(get("hMdcWiresEventMeantemp")->getP(1,0)->GetEntries())) ;
                get("hMdcWiresEventMeantemp")->getP(0,icut)->Reset();
            }
            get("hMdcWiresEventMeantemp")->getP(1,0)->Reset();
        }
        for(Int_t mod = 0; mod < 4; mod ++){
            if( !mhMdctime1Cal1MeanTrendMod[mod] || !mhMdctime1Cal1MeanTrendModtemp[mod]) continue;
            if(evtCt%mhMdctime1Cal1MeanTrendMod[mod]->getRefreshRate() != 0 && evtCt > 0) continue;

            for(Int_t sec = 0; sec < 6; sec ++){
                mhMdctime1Cal1MeanTrendMod[mod]->fill(0,sec,mhMdctime1Cal1MeanTrendModtemp[mod]->getP(0,sec)->GetMean());
                mhMdctime1Cal1MeanTrendModtemp[mod]->getP(0,sec)->Reset();
            }

        }
        //-------------------------------------------------------------


        //---------------  cut stat -----------------------------------
        HMdcCutStat* cutstat = HMdcCutStat::getExObject();
        if(cutstat && get("hMdccutstat"))
        {
            if(evtCt%get("hMdccutstat")->getRefreshRate() == 0 && evtCt > 0){
                Int_t t1,t2,t1t2,wire,t1_t1t2,cutall,allhits;
                for(Int_t mod = 0; mod < 4; mod ++) {
                    for(Int_t sec = 0; sec < 6; sec ++) {
                        cutstat->getCal1StatCut(sec,mod,&t1,&t2,&t1t2,&wire,&t1_t1t2,&cutall,&allhits);
                        if(allhits!=0){
                            get("hMdccutstat")->getP(0,1)->SetBinContent(mod*6+sec+1,(Float_t)(allhits-t1)     /allhits); //3
                            get("hMdccutstat")->getP(0,2)->SetBinContent(mod*6+sec+1,(Float_t)(allhits-t1t2)   /allhits); //5
                            get("hMdccutstat")->getP(0,3)->SetBinContent(mod*6+sec+1,(Float_t)(allhits-cutall) /allhits); //4
                            get("hMdccutstat")->getP(0,0)->SetBinContent(mod*6+sec+1,(Float_t)allhits/allhits); //2
                        }
                    }
                }

                if(get("hMdccutstat")->getResetable() == 1) cutstat->clear();
            }
        }
        //-------------------------------------------------------------

    }
    //###############################################################################



    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}
