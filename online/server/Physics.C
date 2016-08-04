
#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "hcategorymanager.h"
#include "hparticledef.h"


#include "hparticlecand.h"
#include "hparticleevtinfo.h"
#include "hmdccal1.h"

#include "heventheader.h"

#include "hparticletool.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"
#include "TMath.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > physicsMap;

Bool_t createHistPhysics(TList& histpool){

    mapHolder::setMap(physicsMap); // make physicsMap currentMap

    //####################### USER CODE ##############################################
    // define monitoring hists

    const Char_t* hists[] = {
	    "FORMAT#array TYPE#1F NAME#hPhys_candMult_SUM TITLE#Cand_Mult_SUM ACTIVE#1 RESET#1 REFRESH#1000 BIN#200:0.0:200.0:0:0.0:0.0 SIZE#1:2 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMult_Lep_SUM TITLE#Cand_Mult_Lep_SUM ACTIVE#1 RESET#1 REFRESH#1000 BIN#20:0.0:20.0:0:0.0:0.0 SIZE#1:2 AXIS#multiplicity:counts:no DIR#no OPT#no STATS#0 LOG#0:1:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMult_Sys TITLE#Cand_Mult ACTIVE#1 RESET#1 REFRESH#1000 BIN#6:0.0:6.0:0:0.0:0.0 SIZE#1:4 AXIS#sector:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMult_Used_Sys TITLE#Cand_Mult_Used ACTIVE#1 RESET#1 REFRESH#1000 BIN#6:0.0:6.0:0:0.0:0.0 SIZE#1:4 AXIS#sector:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candTheta_Sys0 TITLE#Cand_Theta_Sys0 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:0.0:90.0:0:0.0:0.0 SIZE#1:6 AXIS#theta:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candTheta_Sys1 TITLE#Cand_Theta_Sys1 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:0.0:90.0:0:0.0:0.0 SIZE#1:6 AXIS#theta:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candTheta_Used_Sys0 TITLE#Cand_Theta_Used_Sys0 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:0.0:90.0:0:0.0:0.0 SIZE#1:6 AXIS#theta:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candTheta_Used_Sys1 TITLE#Cand_Theta_Used_Sys1 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:0.0:90.0:0:0.0:0.0 SIZE#1:6 AXIS#theta:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candPhi_Sys TITLE#Cand_Phi_Sys ACTIVE#1 RESET#1 REFRESH#1000 BIN#90:0.0:360.0:0:0.0:0.0 SIZE#1:2 AXIS#phi:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:Sys%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candPhi_Used_Sys TITLE#Cand_Phi_Used_Sys ACTIVE#1 RESET#1 REFRESH#1000 BIN#90:0.0:360.0:0:0.0:0.0 SIZE#1:2 AXIS#phi:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:Sys%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_Sys0 TITLE#Cand_MatchQA_Sys0 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_Sys1 TITLE#Cand_MatchQA_Sys1 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_SHR TITLE#Cand_MatchQA_SHR ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_Used_Sys0 TITLE#Cand_MatchQA_Used_Sys0 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_Used_Sys1 TITLE#Cand_MatchQA_Used_Sys1 ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
	    ,"FORMAT#array TYPE#1F NAME#hPhys_candMatchQA_Used_SHR TITLE#Cand_MatchQA_Used_SHR ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-1.0:6.0:0:0.0:0.0 SIZE#1:6 AXIS#MetaQA:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#2F NAME#hPhys_candBetaMom_Sys0 TITLE#Cand_BetaMom ACTIVE#1 RESET#1 REFRESH#1000 BIN#100:-2000.0:2000.0:100:0.0:2.0 SIZE#1:6 AXIS#mom*charge[MeV/c]:beta:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#2F NAME#hPhys_candBetaMom_Sys1 TITLE#Cand_BetaMom ACTIVE#1 RESET#1 REFRESH#1000 BIN#100:-2000.0:2000.0:100:0.0:2.0 SIZE#1:6 AXIS#mom*charge[MeV/c]:beta:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#mon TYPE#2F NAME#hPhys_candVertex_XY TITLE#Cand_Vertex_XY ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-15.0:15.0:30:-15.0:15.0 SIZE#0:0 AXIS#X[mm]:Y[mm]:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#mon TYPE#1F NAME#hPhys_candVertex_X TITLE#Cand_Vertex_X ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-15.0:15.0:0:0:0 SIZE#0:0 AXIS#X[mm]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#mon TYPE#1F NAME#hPhys_candVertex_Y TITLE#Cand_Vertex_Y ACTIVE#1 RESET#1 REFRESH#1000 BIN#30:-15.0:15.0:0:0:0 SIZE#0:0 AXIS#Y[mm]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#mon TYPE#1F NAME#hPhys_candVertex_Z TITLE#Cand_Vertex_Z ACTIVE#1 RESET#1 REFRESH#1000 BIN#60:-70.0:10.0:0:0:0 SIZE#0:0 AXIS#Z[mm]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
	    ,"FORMAT#mon TYPE#1F NAME#hPhys_candVertex_Zclust TITLE#Cand_Vertex_Zclust ACTIVE#1 RESET#1 REFRESH#1000 BIN#80:-70.0:10.0:0:0:0 SIZE#0:0 AXIS#Z[mm]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#2:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#-1:-1:-1:-1:no"
            ,"FORMAT#array TYPE#1F NAME#hPhys_candTot_P0 TITLE#Cand_Tot_P0 ACTIVE#1 RESET#1 REFRESH#1000 BIN#70:0.0:200.0:0:0:0 SIZE#1:6 AXIS#time2-time1[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hPhys_candTot_P1 TITLE#Cand_Tot_P1 ACTIVE#1 RESET#1 REFRESH#1000 BIN#70:0.0:200.0:0:0:0 SIZE#1:6 AXIS#time2-time1[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hPhys_candTot_P2 TITLE#Cand_Tot_P2 ACTIVE#1 RESET#1 REFRESH#1000 BIN#80:0.0:250.0:0:0:0 SIZE#1:6 AXIS#time2-time1[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
            ,"FORMAT#array TYPE#1F NAME#hPhys_candTot_P3 TITLE#Cand_Tot_P3 ACTIVE#1 RESET#1 REFRESH#1000 BIN#80:0.0:250.0:0:0:0 SIZE#1:6 AXIS#time2-time1[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99 LEGEND#0.90:0.98:0.99:0.5:S%i"
    };
    //###############################################################################
    Int_t colorsSector[6] = {kRed,kBlue,kMagenta,kGreen+2,kBlack,kOrange-3};
    Int_t colorsSys   [4] = {kRed,kBlue,kMagenta,kGreen+2};


    // create hists and add them to the pool
    mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);


    HOnlineMonHistAddon* addon = 0;

    for( map< TString, HOnlineMonHistAddon*>::iterator iter = physicsMap.begin(); iter != physicsMap.end(); ++iter ) {
	addon           = (*iter).second;
	if(addon){
            addon->setRefreshRate(addon->getRefreshRate()*5) ;
	}
    }



    if( (addon = get("hPhys_candMult_SUM") ))  {
	for(Int_t sys = 0; sys < 2; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
        //LEGEND#0.90:0.98:0.99:0.5:S%i"
	TLegend* leg = new TLegend(0.9,0.98,0.98,0.5,"","brNDC");
	leg->AddEntry(get("hPhys_candMult_SUM")->getP(0,0),"all","lpf");
	leg->AddEntry(get("hPhys_candMult_SUM")->getP(0,1),"used","lpf");
        addon->setLegend(leg);
    }
    if( (addon = get("hPhys_candMult_Lep_SUM") ))  {
	for(Int_t sys = 0; sys < 2; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
        //LEGEND#0.90:0.98:0.99:0.5:S%i"
	TLegend* leg = new TLegend(0.9,0.98,0.98,0.5,"","brNDC");
	leg->AddEntry(get("hPhys_candMult_Lep_SUM")->getP(0,0),"all","lpf");
	leg->AddEntry(get("hPhys_candMult_Lep_SUM")->getP(0,1),"used","lpf");
        addon->setLegend(leg);
    }
    if( (addon = get("hPhys_candMult_Sys") ))  {
	for(Int_t sys = 0; sys < 4; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
        //LEGEND#0.90:0.98:0.99:0.5:S%i"
	TLegend* leg = new TLegend(0.9,0.98,0.98,0.5,"","brNDC");
	leg->AddEntry(get("hPhys_candMult_Sys")->getP(0,0),"-1","lpf");
	leg->AddEntry(get("hPhys_candMult_Sys")->getP(0,1),"0","lpf");
	leg->AddEntry(get("hPhys_candMult_Sys")->getP(0,2),"1","lpf");
	leg->AddEntry(get("hPhys_candMult_Sys")->getP(0,3),"2","lpf");
        addon->setLegend(leg);
    }
    if( (addon = get("hPhys_candMult_Used_Sys") ))  {
	for(Int_t sys = 0; sys < 4; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
        //LEGEND#0.90:0.98:0.99:0.5:S%i"
	TLegend* leg = new TLegend(0.9,0.98,0.98,0.5,"","brNDC");
	leg->AddEntry(get("hPhys_candMult_Used_Sys")->getP(0,0),"-1","lpf");
	leg->AddEntry(get("hPhys_candMult_Used_Sys")->getP(0,1),"0","lpf");
	leg->AddEntry(get("hPhys_candMult_Used_Sys")->getP(0,2),"1","lpf");
	leg->AddEntry(get("hPhys_candMult_Used_Sys")->getP(0,3),"2","lpf");
        addon->setLegend(leg);
    }
    if( (addon = get("hPhys_candTheta_Sys0") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTheta_Sys1") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTheta_Used_Sys0") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTheta_Used_Sys1") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candPhi_Sys") ))  {
	for(Int_t sys = 0; sys < 2; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
    }
    if( (addon = get("hPhys_candPhi_Used_Sys") ))  {
	for(Int_t sys = 0; sys < 2; sys ++){
	    addon->getP(0,sys)->SetLineColor(colorsSys[sys]);
	}
    }

    if( (addon = get("hPhys_candMatchQA_Sys0") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candMatchQA_Sys1") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candMatchQA_SHR") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candMatchQA_Used_Sys0") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candMatchQA_Used_Sys1") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candMatchQA_Used_SHR") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTot_P0") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTot_P1") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTot_P2") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }
    if( (addon = get("hPhys_candTot_P3") ))  {
	for(Int_t s = 0; s < 6; s ++){
	    addon->getP(0,s)->SetLineColor(colorsSector[s]);
	}
    }

    return kTRUE;
}

Bool_t fillHistPhysics(Int_t evtCt){

    mapHolder::setMap(physicsMap); // make tofMap currentMap



    //####################### USER CODE ##############################################
    // Fill Histograms

    // categorys to loop
    HCategory* candCat    = gHades->getCurrentEvent()->getCategory(catParticleCand);
    //HCategory* evtInfoCat = gHades->getCurrentEvent()->getCategory(catParticleEvtInfo);

    HVertex vertex      = gHades->getCurrentEvent()->getHeader()->getVertex();
    HVertex vertexClust = gHades->getCurrentEvent()->getHeader()->getVertexCluster();


    if(candCat){
	HParticleCand*    cand;
        //HParticleEvtInfo* evtInfo;


	Int_t ctAll=0;
        Int_t ctAllUsed=0;
        Int_t ctLep=0;
        Int_t ctLepUsed=0;


	for(Int_t i = 0; i < candCat->getEntries(); i ++){
	    cand = HCategoryManager::getObject(cand,candCat,i);
	    if(cand){

		//Short_t index  = cand->getIndex();
		//Int_t   pid    = cand->getPID();
		Short_t sec    = cand->getSector();
		Short_t system = cand->getSystem();
		//UShort_t = cand->getRingCorr();
		//Bool_t   = cand->isRichMatch(eMatching match);
		//Short_t  = cand->getTofRec();
		//Float_t mdcdedx = cand->getMdcdEdx();
		//Float_t tofdedx = cand->getTofdEdx();
		Short_t charge  = cand->getCharge();
		Float_t beta    = cand->getBeta();
		Float_t mom     = cand->getMomentum();
		//Float_t mass2   = cand->getMass2();
		Float_t phi     = cand->getPhi();
		Float_t theta   = cand->getTheta();
		//Float_t = cand->getR();
		//Float_t = cand->getZ();
		//Float_t chi2     = cand->getChi2();
		//Float_t distMeta = cand->getDistanceToMetaHit();
		//Float_t ich2     = cand->getInnerSegmentChi2();
		//Float_t ochi2    = cand->getOuterSegmentChi2();
		//Float_t oAfit    = cand->getAngleToNearbyFittedInner();
		//Float_t oAnfit   = cand->getAngleToNearbyUnfittedInner();
		//Int_t  rnpads    = cand->getRingNumPads() ;
		//Int_t  ramp      = cand->getRingAmplitude();
		//Int_t   = cand->getRingHouTra();
		//Int_t  rpattern   = cand->getRingPatternMatrix();
		//Float_t rcentroid = cand->getRingCentroid();
		//Float_t rphi      = cand->getRichPhi();
		//Float_t rtheta    = cand->getRichTheta();
		Float_t metaQA    = cand->getMetaMatchQuality();
		Float_t metaQAShr = cand->getMetaMatchQualityShower();
		//Float_t shsum0    = cand->getShowerSum0();
		//Float_t shsum1    = cand->getShowerSum1();
		//Float_t shsum2    = cand->getShowerSum2();
                /*
		Int_t   cand->getSelectedMeta();
		Bool_t  cand->isTofHitUsed();
		Bool_t  cand->isTofClstUsed();
		Bool_t  cand->isRpcClstUsed();
		Bool_t  cand->isShowerUsed();
		Bool_t  cand->isMetaUsed();

		Int_t   cand->getMetaInd();
		Int_t   cand->getRichInd();
		Int_t   cand->getInnerSegInd();
		Int_t   cand->getOuterSegInd();
		Int_t   cand->getRpcInd() ;
		Int_t   cand->getShowerInd();
		Int_t   cand->getTofHitInd();
		Int_t   cand->getTofClstInd();


		void    cand->calc4vectorProperties(Double_t mass = 0.51099892);
		Float_t cand->getDeltaTheta();
		Float_t cand->getDeltaPhi();
		Float_t cand->getRichMatchingQuality();
		Float_t cand->getZprime(Float_t xBeam,Float_t yBeam,Float_t& rPrime);
		Float_t cand->getZprime(Float_t xBeam,Float_t yBeam);
		Float_t cand->getRprime(Float_t xBeam,Float_t yBeam);
                */


                if(cand->getOuterSegInd() < 0) continue;    // only candidates with outer segment

                Bool_t used = cand->isFlagBit(Particle::kIsUsed);
                Bool_t lep  = cand->isFlagBit(Particle::kIsLepton);
		ctAll++;
		if(used)       ctAllUsed++;
                if(lep)        ctLep++;
                if(used&&lep)  ctLepUsed++;



		if(get("hPhys_candMult_Sys"))              get("hPhys_candMult_Sys")     ->getP(0,system+1)->Fill(sec);
                if(used && get("hPhys_candMult_Used_Sys")) get("hPhys_candMult_Used_Sys")->getP(0,system+1)->Fill(sec);

		if(system == 0 || system == 1){
		    if(get("hPhys_candPhi_Sys"))              get("hPhys_candPhi_Sys")     ->getP(0,system)->Fill(phi);
		    if(used && get("hPhys_candPhi_Used_Sys")) get("hPhys_candPhi_Used_Sys")->getP(0,system)->Fill(phi);
		}

		if(system == 0){
		    if(get("hPhys_candTheta_Sys0"))      get("hPhys_candTheta_Sys0")     ->getP(0,sec)->Fill(theta);
                    if(beta !=-1 && get("hPhys_candBetaMom_Sys0")) get("hPhys_candBetaMom_Sys0")->getP(0,sec)->Fill(mom*charge,beta);

                    if(cand->isRpcClstUsed() && get("hPhys_candMatchQA_Sys0")) get("hPhys_candMatchQA_Sys0") ->getP(0,sec)->Fill(metaQA);
		    if(metaQAShr > 0 && get("hPhys_candMatchQA_SHR")) get("hPhys_candMatchQA_SHR") ->getP(0,sec)->Fill(metaQAShr);

		    if(used){
			if(get("hPhys_candTheta_Used_Sys0")) get("hPhys_candTheta_Used_Sys0")->getP(0,sec)->Fill(theta);
			if(cand->isRpcClstUsed() && get("hPhys_candMatchQA_Used_Sys0")) get("hPhys_candMatchQA_Used_Sys0") ->getP(0,sec)->Fill(metaQA);
                        if(metaQAShr > 0 && get("hPhys_candMatchQA_Used_SHR")) get("hPhys_candMatchQA_Used_SHR") ->getP(0,sec)->Fill(metaQAShr);
		    }
		}

		if(system == 1) {
		    if(get("hPhys_candTheta_Sys1")) get("hPhys_candTheta_Sys1")->getP(0,sec)->Fill(theta);
                    if(beta !=-1 && get("hPhys_candBetaMom_Sys1")) get("hPhys_candBetaMom_Sys1")->getP(0,sec)->Fill(mom*charge,beta);

		    if( (cand->isTofHitUsed() || cand->isTofClstUsed())  && get("hPhys_candMatchQA_Sys1")) get("hPhys_candMatchQA_Sys1") ->getP(0,sec)->Fill(metaQA);
		    if(used){
			if(get("hPhys_candTheta_Used_Sys1")) get("hPhys_candTheta_Used_Sys1")->getP(0,sec)->Fill(theta);
			if((cand->isTofHitUsed() || cand->isTofClstUsed())  && get("hPhys_candMatchQA_Used_Sys1")) get("hPhys_candMatchQA_Used_Sys1") ->getP(0,sec)->Fill(metaQA);
		    }
		}

		// vertex histograms
		if(get("hPhys_candVertex_XY")) get("hPhys_candVertex_XY")->getP()->Fill(vertex.getX(),vertex.getY());
		if(get("hPhys_candVertex_X")) get("hPhys_candVertex_X")->getP()->Fill(vertex.getX());
		if(get("hPhys_candVertex_Y")) get("hPhys_candVertex_Y")->getP()->Fill(vertex.getY());
		if(get("hPhys_candVertex_Z")) get("hPhys_candVertex_Z")->getP()->Fill(vertex.getZ());
		if(get("hPhys_candVertex_Zclust")) get("hPhys_candVertex_Zclust")->getP()->Fill(vertexClust.getZ());



		if(get("hPhys_candMult_SUM")) {
		    get("hPhys_candMult_SUM") -> getP(0,0)->Fill(ctAll);
		    get("hPhys_candMult_SUM") -> getP(0,1)->Fill(ctAllUsed);
		}
		if(get("hPhys_candMult_Lep_SUM")) {
		    get("hPhys_candMult_Lep_SUM") -> getP(0,0)->Fill(ctLep);
		    get("hPhys_candMult_Lep_SUM") -> getP(0,1)->Fill(ctLepUsed);
		}




		//---------------------------------------------
		// t2-t1 from segment

		// define MIPS
		Bool_t isMIP = kFALSE;
		if(-1==charge && mom<800 && mom>300.0) isMIP=kTRUE;

		if(isMIP) {

		    TObjArray* ar = HParticleTool::getMdcCal1Seg(cand->getInnerSegInd());

		    for(Int_t j=0;j<ar->GetEntries();j++)
		    {
			HMdcCal1* cal1 = (HMdcCal1*) ar->At(j);

			Float_t t1  = cal1->getTime1();
			Float_t t2  = cal1->getTime2();
			Int_t mdcsec= cal1->getSector();
			Int_t mdcmod= cal1->getModule();

			if(t2 > -998         &&  t1 > -998 &&
			   TMath::Finite(t1) &&  TMath::Finite(t2)
			  )
			{   // some times t1 or t2 can be missing (-999,-998)

			    if(mdcmod==0 && get("hPhys_candTot_P0")) get("hPhys_candTot_P0")->getP(0,mdcsec)->Fill(t2-t1);
			    if(mdcmod==1 && get("hPhys_candTot_P1")) get("hPhys_candTot_P1")->getP(0,mdcsec)->Fill(t2-t1);
			}
		    } // for cal1
		    delete ar;

		    ar = HParticleTool::getMdcCal1Seg(cand->getOuterSegInd());

		    for(Int_t j=0;j<ar->GetEntries();j++)
		    {
			HMdcCal1* cal1 = (HMdcCal1*) ar->At(j);

			Float_t t1  = cal1->getTime1();
			Float_t t2  = cal1->getTime2();
			Int_t mdcsec= cal1->getSector();
			Int_t mdcmod= cal1->getModule();

			if(t2 > -998         &&  t1 > -998 &&
			   TMath::Finite(t1) &&  TMath::Finite(t2)
			  )
			{   // some times t1 or t2 can be missing (-999,-998)
			    if(mdcmod==2 && get("hPhys_candTot_P2")) get("hPhys_candTot_P2")->getP(0,mdcsec)->Fill(t2-t1);
			    if(mdcmod==3 && get("hPhys_candTot_P3")) get("hPhys_candTot_P3")->getP(0,mdcsec)->Fill(t2-t1);

			}
		    } // for cal1
		    delete ar;
		    //---------------------------------------------
		}
	    }
	} // end loop
    }
    
    /*
    //---------------Trend hists raw ----------------------------------
    if(get("hTofmult_temp_raw") && get("hTofmult_trend_raw") &&
       evtCt%get("hTofmult_trend_raw")->getRefreshRate() == 0 && evtCt > 0){
	for(Int_t sec = 0; sec < 6; sec ++){
	    get("hTofmult_trend_raw")->fill(0,sec,((TH2*)(get("hTofmult_temp_raw")->getP()))->Integral(1,64,sec+1,sec+1));
	}
	get("hTofmult_temp_raw")->getP()->Reset();
    }
    //-----------------------------------------------------------------
    */

    //###############################################################################

    // do reset if needed
    mapHolder::resetHists(evtCt);

    return kTRUE;
}

