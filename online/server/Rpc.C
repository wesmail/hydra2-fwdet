#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "rpcdef.h"
#include "hrpcraw.h"
#include "hrpchit.h"
#include "hshowerraw.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"
#include "TLegend.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > rpcMap;

Bool_t createHistRpc(TList& histpool){

    mapHolder::setMap(rpcMap); // make rpcMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists
    const Char_t* hists[] = {
	     "FORMAT#trendarray TYPE#1F NAME#hRpcrawhitstrend TITLE#Rpc_AvgRawMult_trend ACTIVE#1 RESET#0 REFRESH#100 BIN#50:0:50:0:0:0 SIZE#1:6 AXIS#trend:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcrawhitstrendtemp TITLE#Rpc_rawhits_trendtemp ACTIVE#1 RESET#0 REFRESH#20000 BIN#6:0:6:0:0:0 SIZE#0:0 AXIS#trend:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#0:0 MARKER#1:20:0.5 RANGE#-99:-99"
	    ,"FORMAT#array TYPE#1F NAME#hRpcrawsecmult TITLE#Rpc_rawsec_mult ACTIVE#1 RESET#1 REFRESH#20000 BIN#20:0:20:0:0:0 SIZE#1:6 AXIS#Sector_Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcrawsecmultTemp TITLE#Rpc_rawsec_mult_Temp ACTIVE#1 RESET#0 REFRESH#20000 BIN#6:0:6:0:0:0 SIZE#0:0 AXIS#sector:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcColCellmult TITLE#Rpc_ColCell_mult ACTIVE#1 RESET#1 REFRESH#20000 BIN#6:0:6:32:0:32 SIZE#1:6 AXIS#column:cell:counts DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s0 TITLE#Rpc_ColCell_mult_2ends_s0 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s1 TITLE#Rpc_ColCell_mult_2ends_s1 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s2 TITLE#Rpc_ColCell_mult_2ends_s2 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s3 TITLE#Rpc_ColCell_mult_2ends_s3 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s4 TITLE#Rpc_ColCell_mult_2ends_s4 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcColCellmult2s5 TITLE#Rpc_ColCell_mult_2ends_s5 ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:0:0:0 SIZE#1:2 AXIS#cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcColCellCharge TITLE#Rpc_ColCell_Charge ACTIVE#1 RESET#1 REFRESH#20000 BIN#192:0:192:60:-580:580 SIZE#1:6 AXIS#BothEnds_rawCharge_Diff:no:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#mon TYPE#1F NAME#hRpcMultiplicity TITLE#RpcMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#0:0 AXIS#Full_RPC_Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#1:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#mon TYPE#1F NAME#hRpcMultiplicityTemp TITLE#RpcMultiplicityTemp ACTIVE#1 RESET#0 REFRESH#10000 BIN#1:0:1:0:0:0 SIZE#0:0 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcTrbMbomult TITLE#Rpc_TrbMbo_mult ACTIVE#1 RESET#1 REFRESH#20000 BIN#4:0:4:4:0:4 SIZE#1:6 AXIS#TRB_Number:MBO_Number:counts DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcMboDbomult TITLE#Rpc_MboDbo_mult ACTIVE#1 RESET#1 REFRESH#20000 BIN#16:0:16:8:0:8 SIZE#1:6 AXIS#MBO_Number:DBO_Number:counts DIR#no OPT#lego2 STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcTrbChanmult TITLE#Rpc_TrbChan_mult ACTIVE#1 RESET#1 REFRESH#20000 BIN#128:0:128:4:0:4 SIZE#1:6 AXIS#TRB_Channel:TRB_Number:counts DIR#no OPT#colz STATS#0 LOG#0:0:1 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#array TYPE#2F NAME#hRpcXYHit TITLE#Rpc_XY_Hit ACTIVE#1 RESET#1 REFRESH#20000 BIN#190:-850:850:35:350:1750 SIZE#1:6 AXIS#X_[mm]:Y_[mm]:counts DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#2F NAME#hRpcShowerCorr TITLE#Rpc_Shower_Correlation ACTIVE#1 RESET#1 REFRESH#20000 BIN#32:0:32:32:0:32 SIZE#1:6 AXIS#Shower_Row:Rpc_Cell:counts DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#mon TYPE#1F NAME#hRpcLatch TITLE#LatchMutiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#16:0:16:0:0:0 SIZE#0:0 AXIS#TBit:multiplicity:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:1 LINE#0:0 FILL#1:0 MARKER#1:20:0.5 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcTofPTMultiplicity TITLE#RpcTofPTMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#1:3 AXIS#RPC_+_Tof_Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcPTMultiplicity TITLE#RpcPhysicTriggerMultiplicity ACTIVE#1 RESET#1 REFRESH#10000 BIN#50:0:50:0:0:0 SIZE#1:3 AXIS#PhysTrigger_RPC_Multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcTofMultiplicityTemp TITLE#RpcTofMultiplicityTemp ACTIVE#1 RESET#0 REFRESH#10000 BIN#1:0:1:0:0:0 SIZE#1:3 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#array TYPE#1F NAME#hRpcPTMultiplicityTemp TITLE#RpcPTMultiplicityTemp ACTIVE#1 RESET#0 REFRESH#10000 BIN#1:0:1:0:0:0 SIZE#1:3 AXIS#no:no:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#0:0 LINE#0:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99"
            ,"FORMAT#mon TYPE#1F NAME#hRpcTof_hit TITLE#Rpc_Tof ACTIVE#1 RESET#1 REFRESH#10000 BIN#1500:0:150:0:0:0 SIZE#0:0 AXIS#time[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcPhi_hit TITLE#Rpc_Phi ACTIVE#1 RESET#1 REFRESH#10000 BIN#360:0.0:360.0:0:0.0:0.0 SIZE#0:0 AXIS#phi:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcPatternL_raw TITLE#Rpc_PatternL ACTIVE#1 RESET#1 REFRESH#10000 BIN#1153:0:1153.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*192+col*32+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#38:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcPatternR_raw TITLE#Rpc_PatternR ACTIVE#1 RESET#1 REFRESH#10000 BIN#1153:0.5:1153.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*192+col*32+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99"
	    ,"FORMAT#mon TYPE#1F NAME#hRpcPattern_raw TITLE#Rpc_Pattern ACTIVE#1 RESET#1 REFRESH#10000 BIN#1153:0.5:1153.5:0:0.0:0.0 SIZE#0:0 AXIS#sec*192+col*32+cell:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#30:0 MARKER#0:0:0 RANGE#-99:-99"
    };
    //###############################################################################


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

    Int_t colorsSector[6] = {2,4,6,8,38,46};
    HOnlineMonHistAddon* addon = 0;
    if((addon = get("hRpcrawhitstrend"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    if((addon = get("hRpcrawsecmult"))) {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetFillColor(colorsSector[s]);
	}
    }
    if((addon = get("hRpcMultiplicity"))) addon->getP()->SetFillColor(5);
    if((addon = get("hRpcLatch"))) addon->getP()->SetFillColor(4);
    if((addon = get("hRpcColCellmult2s0"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcColCellmult2s1"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcColCellmult2s2"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcColCellmult2s3"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcColCellmult2s4"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcColCellmult2s5"))) for(Int_t side = 0; side < 2; side++) addon->getP(0,side)->SetLineColor(side+2);
    if((addon = get("hRpcPTMultiplicity"))) for(Int_t pt = 0; pt < 3; pt++) addon->getP(0,pt)->SetLineColor(pt+2);
    if((addon = get("hRpcTofPTMultiplicity"))) for(Int_t pt = 0; pt < 3; pt++) addon->getP(0,pt)->SetLineColor(pt+2);
    if((addon = get("hRpcPTMultiplicity"))) for(Int_t pt = 0; pt < 3; pt++) addon->getP(0,pt)->SetLineWidth(2);
    if((addon = get("hRpcTofPTMultiplicity"))) for(Int_t pt = 0; pt < 3; pt++) addon->getP(0,pt)->SetLineWidth(2);

    if(get("hRpcPTMultiplicity")){
        TLegend* leg = new TLegend(0.85,0.75,0.99,0.9,"","brNDC");
        for(Int_t pt = 0; pt < 3; pt++) leg->AddEntry(get("hRpcPTMultiplicity")->getP(0,pt),Form("PTMult %i",pt+1),"lpf");
        get("hRpcPTMultiplicity")->setLegend(leg);
    }
    return kTRUE;
}

Bool_t fillHistRpc(Int_t evtCt){


    mapHolder::setMap(rpcMap); // make rpcMap currentMap




    //####################### USER CODE ##############################################
    // Fill Histograms

    // categorys to loop
    HCategory* rpcRawCat    = gHades->getCurrentEvent()->getCategory(catRpcRaw);
    HCategory* rpcHitCat    = gHades->getCurrentEvent()->getCategory(catRpcHit);
    HCategory* showerRawCat = gHades->getCurrentEvent()->getCategory(catShowerRaw);
    HCategory* tofRawCat    = gHades->getCurrentEvent()->getCategory(catTofRaw);


    // LATCH data
    Int_t nLatchWord = 0;
    Bool_t latchRpcTof  = kFALSE;
    Bool_t latchRpcTof1 = kFALSE;
    Bool_t latchRpcTof2 = kFALSE;
    Bool_t latchRpcTof3 = kFALSE;
    Bool_t latchWall    = kFALSE;
    nLatchWord = gHades->getCurrentEvent()->getHeader()->getTBit();
    for(Int_t i=0;i<16;i++){
	if(( nLatchWord & (1<<i))!=0 ){
	    if(get("hRpcLatch"))  get("hRpcLatch")->getP()->Fill(i);
            if(i==11) latchRpcTof1 =kTRUE;
            if(i==12) latchRpcTof2 =kTRUE;
            if(i==13) latchRpcTof3 =kTRUE;
            if(i==14) latchWall    =kTRUE;
	}
    }
    if(!latchWall) latchRpcTof=kTRUE;

	// TOF data
	if(tofRawCat){
		HTofRaw* raw;
		Float_t rtime ,ltime;
		for(Int_t i = 0; i < tofRawCat->getEntries(); i ++){
			raw = (HTofRaw*)tofRawCat->getObject(i);
			if(raw){
				rtime   = raw->getRightTime();
				ltime   = raw->getLeftTime();
				if(rtime>0 && ltime>0) {
					if(get("hRpcTofMultiplicityTemp")&&latchRpcTof1) get("hRpcTofMultiplicityTemp")->getP(0,0)->Fill(0.5);
					if(get("hRpcTofMultiplicityTemp")&&latchRpcTof2) get("hRpcTofMultiplicityTemp")->getP(0,1)->Fill(0.5);
					if(get("hRpcTofMultiplicityTemp")&&latchRpcTof3) get("hRpcTofMultiplicityTemp")->getP(0,2)->Fill(0.5);
				}
			}
		} // end raw loop
	}

    // RPC data
    if(rpcRawCat){
	HRpcRaw* raw;
	Int_t sec,col,cell;
	Float_t tl,tr;
	Float_t ql,qr;
	Int_t trbl,trbr;
	Int_t mbol,mbor;
	Int_t dbol,dbor;
        Int_t chanl,chanr;
	for(Int_t i = 0; i < rpcRawCat->getEntries(); i ++){
	    raw = (HRpcRaw*)rpcRawCat->getObject(i);
	    if(raw){
		sec  = raw->getSector();
		col  = raw->getColumn();
		cell = raw->getCell();
		tl   = raw->getLeftTime();
		tr   = raw->getRightTime();
		ql   = raw->getLeftTot();
		qr   = raw->getRightTot();
		trbl = raw->getLeftTRBNum();
		trbr = raw->getRightTRBNum();
		mbol = raw->getLeftMBONum();
		mbor = raw->getRightMBONum();
		dbol = raw->getLeftDBONum();
		dbor = raw->getRightDBONum();
		chanl= raw->getLeftTrbChannel();
                chanr= raw->getRightTrbChannel();

		if(get("hRpcTrbChanmult")) {
		    if(tl>0) get("hRpcTrbChanmult")->fill(0,sec,chanl,trbl);
		    if(tr>0) get("hRpcTrbChanmult")->fill(0,sec,chanr,trbr);
		}
		if(cell<32) {
		    //if(get("hRpcMultiplicityTemp")&&tl>0&&tr>0) get("hRpcMultiplicityTemp")->getP()->Fill(0.5);
			//if(get("hRpcLatchMultiplicityTemp")&&tl>0&&tr>0&&latchRpc) get("hRpcLatchMultiplicityTemp")->getP()->Fill(0.5);
			if(tl>0 && tr>0 && tl<36000 && tr<36000) {
				if(get("hRpcMultiplicityTemp")) get("hRpcMultiplicityTemp")->getP()->Fill(0.5);
				if(get("hRpcTofMultiplicityTemp")&&latchRpcTof1) get("hRpcTofMultiplicityTemp")->getP(0,0)->Fill(0.5);
				if(get("hRpcTofMultiplicityTemp")&&latchRpcTof2) get("hRpcTofMultiplicityTemp")->getP(0,1)->Fill(0.5);
				if(get("hRpcTofMultiplicityTemp")&&latchRpcTof3) get("hRpcTofMultiplicityTemp")->getP(0,2)->Fill(0.5);
				if(get("hRpcPTMultiplicityTemp")&&latchRpcTof1) get("hRpcPTMultiplicityTemp")->getP(0,0)->Fill(0.5);
				if(get("hRpcPTMultiplicityTemp")&&latchRpcTof2) get("hRpcPTMultiplicityTemp")->getP(0,1)->Fill(0.5);
				if(get("hRpcPTMultiplicityTemp")&&latchRpcTof3) get("hRpcPTMultiplicityTemp")->getP(0,2)->Fill(0.5);
				if(get("hRpcrawhitstrendtemp")) get("hRpcrawhitstrendtemp") ->getP()->Fill(sec);
                                if(get("hRpcColCellmult")&&tl>0&&tr>0) get("hRpcColCellmult") ->fill(0,sec,col,cell);
			}
			if(get("hRpcTrbMbomult")) {
			if(tl>0) get("hRpcTrbMbomult")->fill(0,sec,trbl,mbol);
			if(tr>0) get("hRpcTrbMbomult")->fill(0,sec,trbr,mbor);
		    }
		    if(get("hRpcMboDbomult")) {
			if(tl>0) get("hRpcMboDbomult")->fill(0,sec,mbol+(trbl*4),dbol);
			if(tr>0) get("hRpcMboDbomult")->fill(0,sec,mbor+(trbr*4),dbor);
		    }
		    if(get("hRpcColCellmult2s0")&&tl>0&&tl<36000&&sec==0) get("hRpcColCellmult2s0")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s0")&&tr>0&&tr<36000&&sec==0) get("hRpcColCellmult2s0")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s1")&&tl>0&&tl<36000&&sec==1) get("hRpcColCellmult2s1")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s1")&&tr>0&&tr<36000&&sec==1) get("hRpcColCellmult2s1")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s2")&&tl>0&&tl<36000&&sec==2) get("hRpcColCellmult2s2")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s2")&&tr>0&&tr<36000&&sec==2) get("hRpcColCellmult2s2")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s3")&&tl>0&&tl<36000&&sec==3) get("hRpcColCellmult2s3")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s3")&&tr>0&&tr<36000&&sec==3) get("hRpcColCellmult2s3")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s4")&&tl>0&&tl<36000&&sec==4) get("hRpcColCellmult2s4")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s4")&&tr>0&&tr<36000&&sec==4) get("hRpcColCellmult2s4")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s5")&&tl>0&&tl<36000&&sec==5) get("hRpcColCellmult2s5")->getP(0,0)->Fill(cell+col*32);
		    if(get("hRpcColCellmult2s5")&&tr>0&&tr<36000&&sec==5) get("hRpcColCellmult2s5")->getP(0,1)->Fill(cell+col*32);
		    if(get("hRpcrawsecmultTemp")&&tl>0&&tr>0&&tl<36000&&tr<36000) get("hRpcrawsecmultTemp") ->getP()->Fill(sec);
		    if(get("hRpcColCellCharge")) {
			if(ql>0&&qr>0)  { get("hRpcColCellCharge")->fill(0,sec,cell+col*32,ql-qr);
			} else if(ql>0) { get("hRpcColCellCharge")->fill(0,sec,cell+col*32,-500);
			} else if(qr>0) { get("hRpcColCellCharge")->fill(0,sec,cell+col*32,500);
			}
                    }
                    Int_t addr = sec * 192 + col * 32 + cell;
                    if(get("hRpcPatternL_raw") && tl > 0) get("hRpcPatternL_raw")->getP()->Fill((Float_t)addr);
                    if(get("hRpcPatternR_raw") && tr > 0) get("hRpcPatternR_raw")->getP()->Fill((Float_t)addr);
                    if(get("hRpcPattern_raw") && tl > 0 && tr > 0 ) get("hRpcPattern_raw")->getP()->Fill((Float_t)addr);
		}

	    }
	}
    }

    if(get("hRpcrawhitstrend")&&get("hRpcrawhitstrendtemp")&&
       evtCt%get("hRpcrawhitstrend")->getRefreshRate() == 0 && evtCt > 0){
	for(Int_t sec = 0; sec < 6; sec ++){
	    get("hRpcrawhitstrend")->fill(0,sec,(get("hRpcrawhitstrendtemp")->getP()->GetBinContent(sec+1))/100);
	}
	get("hRpcrawhitstrendtemp")->getP()->Reset();
    }

    if(get("hRpcMultiplicity")&&get("hRpcMultiplicityTemp")) {
	get("hRpcMultiplicity")->getP()->Fill(get("hRpcMultiplicityTemp")->getP()->GetBinContent(1));
        get("hRpcMultiplicityTemp")->getP()->Reset();
    }

    if(get("hRpcTofPTMultiplicity")&&get("hRpcTofMultiplicityTemp")) {
	get("hRpcTofPTMultiplicity")->getP(0,0)->Fill(get("hRpcTofMultiplicityTemp")->getP(0,0)->GetBinContent(1));
	get("hRpcTofMultiplicityTemp")->getP(0,0)->Reset();
    }

    if(get("hRpcTofPTMultiplicity")&&get("hRpcTofMultiplicityTemp")->getP(0,1)->GetBinContent(1)>0) {
	get("hRpcTofPTMultiplicity")->getP(0,1)->Fill(get("hRpcTofMultiplicityTemp")->getP(0,1)->GetBinContent(1));
	get("hRpcTofMultiplicityTemp")->getP(0,1)->Reset();
    }

    if(get("hRpcTofPTMultiplicity")&&get("hRpcTofMultiplicityTemp")->getP(0,2)->GetBinContent(1)>0) {
	get("hRpcTofPTMultiplicity")->getP(0,2)->Fill(get("hRpcTofMultiplicityTemp")->getP(0,2)->GetBinContent(1));
	get("hRpcTofMultiplicityTemp")->getP(0,2)->Reset();
    }

    if(get("hRpcPTMultiplicity")&&get("hRpcPTMultiplicityTemp")->getP(0,0)->GetBinContent(1)>0) {
	get("hRpcPTMultiplicity")->getP(0,0)->Fill(get("hRpcPTMultiplicityTemp")->getP(0,0)->GetBinContent(1));
	get("hRpcPTMultiplicityTemp")->getP(0,0)->Reset();
    }

    if(get("hRpcPTMultiplicity")&&get("hRpcPTMultiplicityTemp")->getP(0,1)->GetBinContent(1)>0) {
	get("hRpcPTMultiplicity")->getP(0,1)->Fill(get("hRpcPTMultiplicityTemp")->getP(0,1)->GetBinContent(1));
	get("hRpcPTMultiplicityTemp")->getP(0,1)->Reset();
    }

    if(get("hRpcPTMultiplicity")&&get("hRpcPTMultiplicityTemp")->getP(0,2)->GetBinContent(1)>0) {
	get("hRpcPTMultiplicity")->getP(0,2)->Fill(get("hRpcPTMultiplicityTemp")->getP(0,2)->GetBinContent(1));
	get("hRpcPTMultiplicityTemp")->getP(0,2)->Reset();
    }

    if(get("hRpcrawsecmult")&&get("hRpcrawsecmultTemp")){
	for(Int_t s = 0; s < 6; s ++){
	    get("hRpcrawsecmult")->getP(0,s)->Fill(get("hRpcrawsecmultTemp")->getP()->GetBinContent(s+1));
	}
	get("hRpcrawsecmultTemp")->getP()->Reset();
    }

    if(rpcHitCat){
	HRpcHit* hit;
	Int_t sec,col,cell;
        Float_t X,Y;
        Float_t phi;
	Float_t charge, tof;
	Int_t secS;
        Int_t rowS;
	for(Int_t i = 0; i < rpcHitCat->getEntries(); i ++){
	    hit = (HRpcHit*)rpcHitCat->getObject(i);
	    if(hit){
		sec    = hit->getSector();
		col    = hit->getColumn();
		cell   = hit->getCell();
		X      = hit->getXSec();
                Y      = hit->getYSec();
                phi    = hit->getPhi();
                charge = hit->getCharge();
                tof    = hit->getTof();

		if(charge>12) {
                    if(get("hRpcXYHit")) get("hRpcXYHit")->fill(0,sec,X,Y);
                    if(get("hRpcTof_hit")) get("hRpcTof_hit")->getP()->Fill(tof);
                    if(get("hRpcPhi_hit")) get("hRpcPhi_hit")->getP()->Fill(phi);

		    if(showerRawCat&&latchRpcTof) {
			HShowerRaw* rawS;
			for(Int_t j = 0; j < showerRawCat->getEntries(); j ++){
			    rawS = (HShowerRaw*)showerRawCat->getObject(j);
			    if(rawS) {

				secS = rawS->getSector();
				rowS = rawS->getRow();

				if(sec==secS)  if(get("hRpcShowerCorr")) get("hRpcShowerCorr")->fill(0,sec,rowS,cell);

			    }

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

