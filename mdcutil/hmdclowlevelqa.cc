// : Author  A.Sadovski,K.Kanaki

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcLowLevelQA
//
///////////////////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdclowlevelqa.h"
#include "hmdcdef.h"
#include "hdebug.h"
#include "hades.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hevent.h"
#include "hcategory.h"
#include "hlocation.h"
#include "hmdccal1.h"
#include "hmdcraw.h"
#include "hmdclookupgeom.h"
#include "hmdclookupraw.h"
#include "hmdctimecut.h"
#include "hmdccalparraw.h"
#include "hmessagemgr.h"
#include "htool.h"
#include <iostream> 
#include <iomanip>
#include <stdlib.h>
#include "TH2.h"
#include "TH1.h"
#include "TDirectory.h"
#include "TStopwatch.h"
#include "TMath.h"

Int_t HMdcLowLevelQA::colorsSector[6]={2,4,6,8,38,46};
Int_t HMdcLowLevelQA::colorsModule[4]={2,4,6,8};
Int_t HMdcLowLevelQA::colorsCuts[3]={3,5,4};
Int_t HMdcLowLevelQA::cutResults[4]={0,0,0,0};

ClassImp(HMdcLowLevelQA)

HMdcLowLevelQA::HMdcLowLevelQA(void) {
  // Default constructor calls the function setParContainers().
  initVariables();
}

HMdcLowLevelQA::HMdcLowLevelQA(const Text_t* name,const Text_t* title)
                 :  HReconstructor(name,title) {
  // Constructor calls the constructor of class HReconstructor with the name
  // and the title as arguments. It also calls the function setParContainers().
  initVariables();
}

HMdcLowLevelQA::~HMdcLowLevelQA(void) {
  // destructor deletes the iterator
  if (itercal1) delete itercal1;
  if (iterraw)  delete iterraw;
  itercal1=0;
  iterraw=0;
}

void HMdcLowLevelQA::initVariables()
{
    fNameRootHists=0;
    calCat=0;
    rawCat=0;
    itercal1=0;
    iterraw=0;
    lookup=0;
    lookupRaw=0;
    Eventcounter=0;
    isInitialized = kFALSE;
    for(Int_t i=0; i<6; i++){ for(Int_t j=0; j<4; j++){ cnt123[i][j]=0; }}
    for(Int_t s=0; s<6; s++){ for(Int_t m=0; m<4; m++){ for(Int_t mb=0; mb<16; mb++){ NhitsPERmbo[s][m][mb]=0;} }}
    sm_old[0]=-1, sm_old[1]=-1;
    step_trend_meantime1_counter=1;
    stepsize_trend_meantime1=5000;
    resetCounters();
}

void HMdcLowLevelQA::printStatus(){
  SEPERATOR_msg("*",30);
  INFO_msg(10,HMessageMgr::DET_MDC,"HMdcLowLevelQA");
  //cout << "************* HMdcLowLevelQA *************" << endl;
    //cout << "* current file:" << hldFileCurrent.getCurrentFileName() << endl;
    if(fNameRootHists){
      gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"FileName : %s",fNameRootHists);
      //cout << "* File name:" << fNameRootHists << endl;
    }else{
      INFO_msg(10,HMessageMgr::DET_MDC,"File name:    IS NOT SET!!!");
      //cout << "* File name:    IS NOT SET!!!" << endl;
    }
    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Step size for trend Histo.= %d events",stepsize_trend_meantime1);
    //cout << "* Step size for trend Histo.= " << stepsize_trend_meantime1 << " events" << endl;
    //cout << "******************************************" << endl;
    SEPERATOR_msg("*",30);
}

void HMdcLowLevelQA::setParContainers() {
    // creates the parameter containers MdcLookupGeom/MdcLookupRaw/MdcTimeCut if they do not
    // exist and adds them to the list of parameter containers in the runtime
    // database
    lookup   =(HMdcLookupGeom*)gHades->getRuntimeDb()->getContainer("MdcLookupGeom");
    lookupRaw=(HMdcLookupRaw*) gHades->getRuntimeDb()->getContainer("MdcLookupRaw");
    timecut  =(HMdcTimeCut*)   gHades->getRuntimeDb()->getContainer("MdcTimeCut");

    //calparraw=(HMdcCalParRaw*) gHades->getRuntimeDb()->getContainer("MdcCalParRaw");
}

void HMdcLowLevelQA::setOutputRoot(const Char_t *c)
{
    // Sets rootfile output of HMdcFillTimeHists where all created histograms were written.
    //
    if (fNameRootHists) delete fNameRootHists;
    fNameRootHists = new Char_t[strlen(c)+1];
    strcpy(fNameRootHists, c);
}

Bool_t HMdcLowLevelQA::init(void) {
  // creates the MdcRaw  and MdcCal1 categories and adds them to the current
  // event
  // creates an iterator which loops over all fired cells
  // calls the function setParContainers()
  Bool_t test=kFALSE;

  setParContainers();
  test=getCategories();
  getSetup();
  printStatus();
  fActive=kTRUE;
  return test;
}

Bool_t HMdcLowLevelQA::reinit(void)
{
  if(!isInitialized)
    {
      nReqEvents = gHades->getNumberOfRequestedEvents();
      isInitialized = kTRUE;
      createHistograms();
    }
  return kTRUE;
}

Bool_t HMdcLowLevelQA::getCategories()
{
    calCat=gHades->getCurrentEvent()->getCategory(catMdcCal1);
    if (!calCat) {
	Error("HMdcLowLevelQA:init()","Category HMdcCal1 is not filled! Sorry Guy, will quit.");
	exit(1);
	calCat=gHades->getSetup()->getDetector("Mdc")->buildCategory(catMdcCal1);
	if (!calCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catMdcCal1,calCat,"Mdc");
    }
    rawCat=gHades->getCurrentEvent()->getCategory(catMdcRaw);
    if (!rawCat) {
	Error("HMdcLowLevelQA:init()","Category HMdcRaw is not filled! Sorry Guy, will quit.");
	exit(1);
	rawCat=gHades->getSetup()->getDetector("Mdc")->buildCategory(catMdcRaw);
	if (!rawCat) return kFALSE;
	else gHades->getCurrentEvent()->addCategory(catMdcRaw,rawCat,"Mdc");
    }

    itercal1=(HIterator *)calCat->MakeIterator("native");
    iterraw =(HIterator *)rawCat->MakeIterator("native");

    loccal1.set(4,0,0,0,0);
    locraw.set(4,0,0,0,0);
    return kTRUE;
}

void HMdcLowLevelQA::writeTimeCuts(){
  //for(Int_t s=0; s<6; s++){ //time cut is the same for the same plane (different sectors)
  Int_t s=0;
  for(Int_t p=0; p<4; p++){
    //cout << "timecut->getCutT1L=="<< timecut->getCutT1L( s,p) << endl;
    hmdc_summ_tCuts_info[p]->SetBinContent(1, (Double_t)timecut->getCutT1L(      s,p) );
    hmdc_summ_tCuts_info[p]->SetBinContent(2, (Double_t)timecut->getCutT1R(      s,p) );
    hmdc_summ_tCuts_info[p]->SetBinContent(3, (Double_t)timecut->getCutT2L(      s,p) );
    hmdc_summ_tCuts_info[p]->SetBinContent(4, (Double_t)timecut->getCutT2R(      s,p) );
    hmdc_summ_tCuts_info[p]->SetBinContent(5, (Double_t)timecut->getCutDTime21L( s,p) );
    hmdc_summ_tCuts_info[p]->SetBinContent(6, (Double_t)timecut->getCutDTime21R( s,p) );
    hmdc_summ_tCuts_info[p]->Write();
  }
  //}
}

void HMdcLowLevelQA::lookupSlopes(Int_t s, Int_t m){
     //cout << "s="<<s<<" m="<<m<<" Slope="<<calparraw->getSlope(s,m) << endl;

     
}



void HMdcLowLevelQA::testTimeCuts(Int_t s,Int_t m,Float_t t1,Float_t t2)
{

//    cout << "timecut->getCutTime1Left()=" << timecut->getCutTime1Left() << endl;

    hmdc_summ_t2mt1_vs_t1[s][m]->Fill(t1, t2-t1);
    if(timecut->cutTime1(s,m,t1))
       {
	   cutResults[0]=1;
       }
    else
    {
        cutResults[0]=0;
    }
    if(timecut->cutTime2(s,m,t2))
    {
	cutResults[1]=1;
    }
    else
    {
         cutResults[1]=0;
    }
    if(timecut->cutTimesDif(s,m,t1,t2))
    {
	cutResults[2]=1;
    }
    else
    {
        cutResults[2]=0;
    }
    if(cutResults[0]==1&&cutResults[1]==1&&cutResults[2]==1)
    {
	cutResults[3]=1;
    }
    else
    {
        cutResults[3]=0;
    }
}

void HMdcLowLevelQA::createHistograms()
{
  Char_t name [100];
  Char_t axistitle[200];

  sprintf(name,"%s","a_thisFileName");
  a_thisFileName=new TH1F(name,name,1,0,1);
  a_thisFileName->SetXTitle(fNameRootHists);

	// Calculate number of steps for trend histograms, based on number of requested events
  nBinsTrendX = (Int_t)TMath::Floor(nReqEvents / stepsize_trend_meantime1);

  for(Int_t s=0;s<6;s++)
    {
      //###################### CREATE PER SECTOR ####################################



      //#############################################################################
      for(Int_t m=0;m<4;m++)
	{
	  //###################### CREATE PER MODULE ####################################

	  //------------------- hmdc_trend_meanSlope ------------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_trend_meanSlope[",s,"][",m,"]");
	  hmdc_trend_meanSlope[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_meanSlope[s][m]=new TH1F(name,name,500,-100,900);
	  hmdc_trend_meanSlope[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_meanSlope[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_meanSlope[s][m]->SetMarkerStyle(29);
	  hmdc_trend_meanSlope[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," counts]");
	  hmdc_trend_meanSlope[s][m]->SetXTitle(axistitle);
	  hmdc_trend_meanSlope[s][m]->SetYTitle("Mean Slope [1/ns]");
	  hmdc_trend_meanSlope[s][m]->SetOption("PL");

	  //------------------- hmdc_trend_meantime1 ------------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_trend_meantime1[",s,"][",m,"]");
	  hmdc_trend_meantime1[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_meantime1[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_meantime1[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_meantime1[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_meantime1[s][m]->SetMarkerStyle(29);
	  hmdc_trend_meantime1[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_meantime1[s][m]->SetXTitle(axistitle);
	  hmdc_trend_meantime1[s][m]->SetYTitle("Mean time1 [ns]");
	  hmdc_trend_meantime1[s][m]->SetOption("PL");

	  sprintf(name,"%s%i%s%i%s","hmdc_trend_meantime1not_t1[",s,"][",m,"]");
	  hmdc_trend_meantime1not_t1[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_meantime1not_t1[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_meantime1not_t1[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t1[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t1[s][m]->SetMarkerStyle(29);
	  hmdc_trend_meantime1not_t1[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_meantime1not_t1[s][m]->SetXTitle(axistitle);
	  hmdc_trend_meantime1not_t1[s][m]->SetYTitle("Mean time1 [ns]");
	  hmdc_trend_meantime1not_t1[s][m]->SetOption("PL");

	  sprintf(name,"%s%i%s%i%s","hmdc_trend_meantime1not_t2[",s,"][",m,"]");
	  hmdc_trend_meantime1not_t2[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_meantime1not_t2[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_meantime1not_t2[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t2[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t2[s][m]->SetMarkerStyle(29);
	  hmdc_trend_meantime1not_t2[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_meantime1not_t2[s][m]->SetXTitle(axistitle);
	  hmdc_trend_meantime1not_t2[s][m]->SetYTitle("Mean time1 [ns]");
	  hmdc_trend_meantime1not_t2[s][m]->SetOption("PL");

	  sprintf(name,"%s%i%s%i%s","hmdc_trend_meantime1not_t12[",s,"][",m,"]");
	  hmdc_trend_meantime1not_t12[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_meantime1not_t12[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_meantime1not_t12[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t12[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_meantime1not_t12[s][m]->SetMarkerStyle(29);
	  hmdc_trend_meantime1not_t12[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_meantime1not_t12[s][m]->SetXTitle(axistitle);
	  hmdc_trend_meantime1not_t12[s][m]->SetYTitle("Mean time1 [ns]");
	  hmdc_trend_meantime1not_t12[s][m]->SetOption("PL");


	  sprintf(name,"%s%i%s%i%s","hmdc_Slope[",s,"][",m,"]");
	  hmdc_Slope[s][m]=new TH1F(name,name,500,-100,900);

	  sprintf(name,"%s%i%s%i%s","hmdc_time1[",s,"][",m,"]");
	  hmdc_time1[s][m]=new TH1F(name,name,500,-100,900);

	  sprintf(name,"%s%i%s%i%s","hmdc_time1_everithing_summ[",s,"][",m,"]");
	  hmdc_time1_everithing_summ[s][m]=new TH1F(name,name,500,-100,900);

	  sprintf(name,"%s%i%s%i%s","hmdc_time1_good_t1_summ[",s,"][",m,"]");
	  hmdc_time1_good_t1_summ[s][m]=new TH1F(name,name,500,-100,900);

	  sprintf(name,"%s%i%s%i%s","hmdc_time1_not_t1_summ[",s,"][",m,"]");
	  hmdc_time1_not_t1_summ[s][m]=new TH1F(name,name,500,-100,900);

	  sprintf(name,"%s%i%s%i%s","hmdc_time1not_t1[",s,"][",m,"]");
	  hmdc_time1not_t1[s][m]=new TH1F(name,name,500,-100,900);
	  sprintf(name,"%s%i%s%i%s","hmdc_time1not_t2[",s,"][",m,"]");
	  hmdc_time1not_t2[s][m]=new TH1F(name,name,500,-100,900);
	  sprintf(name,"%s%i%s%i%s","hmdc_time1not_t12[",s,"][",m,"]");
	  hmdc_time1not_t12[s][m]=new TH1F(name,name,500,-100,900);

	  //----------------------------------------------------------------------------
	  //------------------- hmdc_trend_neighbors -----------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_trend_neighbors[",s,"][",m,"]");
	  hmdc_trend_neighbors[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_neighbors[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_neighbors[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_neighbors[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_neighbors[s][m]->SetMarkerStyle(29);
	  hmdc_trend_neighbors[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_neighbors[s][m]->SetXTitle(axistitle);
	  hmdc_trend_neighbors[s][m]->SetYTitle("Ratio Double/All cells");
	  hmdc_trend_neighbors[s][m]->SetOption("PL");
	  //----------------------------------------------------------------------------

	  //----------------------- hmdc_trend_123ratio --------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_trend_123ratio[",s,"][",m,"]");
	  hmdc_trend_123ratio[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_123ratio[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_123ratio[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_123ratio[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_123ratio[s][m]->SetMarkerStyle(29);
	  hmdc_trend_123ratio[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_123ratio[s][m]->SetXTitle(axistitle);
	  //hmdc_trend_123ratio[s][m]->SetYTitle("Ratio Events_with(123 f. wires)/Events_all");
	  hmdc_trend_123ratio[s][m]->SetYTitle("Ratio Events_with(123 f. wires)/StepSize");
	  hmdc_trend_123ratio[s][m]->SetOption("PL");
	  //----------------------------------------------------------------------------

	  //---------------------- hmdc_trend_NhitsPERmbo ------------------------------
	  for(Int_t mb=0; mb<16; mb++)
	    {
	      Char_t mbo_name[300];
	      sprintf(mbo_name,"%s%i%s%i%s%i%s","hmdc_trend_NhitsPERmbo[",s,"][",m,"][",mb,"]");
	      hmdc_trend_NhitsPERmbo[s][m][mb]=new TH1F(mbo_name,mbo_name,nBinsTrendX,0,nBinsTrendX);
	      //  	       hmdc_trend_NhitsPERmbo[s][m][mb]=new TH1F(mbo_name,mbo_name,200,0,200);
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetLineColor(colorsSector[s]);
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetMarkerColor(colorsSector[s]);
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetMarkerStyle(29);
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetMarkerSize(.8);
	      sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetXTitle(axistitle);
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetYTitle("hits/mbo");
	      hmdc_trend_NhitsPERmbo[s][m][mb]->SetOption("PL");
            }
	  //----------------------------------------------------------------------------

	  //----------------------- hmdc_trend_multiplicity ----------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_trend_multiplicity[",s,"][",m,"]");
	  hmdc_trend_multiplicity[s][m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
	  // 	    hmdc_trend_multiplicity[s][m]=new TH1F(name,name,200,0,200);
	  hmdc_trend_multiplicity[s][m]->SetLineColor(colorsSector[s]);
	  hmdc_trend_multiplicity[s][m]->SetMarkerColor(colorsSector[s]);
	  hmdc_trend_multiplicity[s][m]->SetMarkerStyle(29);
	  hmdc_trend_multiplicity[s][m]->SetMarkerSize(.8);
	  sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
	  hmdc_trend_multiplicity[s][m]->SetXTitle(axistitle);
	  hmdc_trend_multiplicity[s][m]->SetYTitle("Hits Multiplicity Mean Value");
	  hmdc_trend_multiplicity[s][m]->SetOption("PL");

	  // hmdc_multiplicity
	  sprintf(name,"%s%i%s%i%s","hmdc_multiplicity[",s,"][",m,"]");
	  hmdc_multiplicity[s][m]=new TH1F(name,name,500,-0,500);

	  //----------------------------------------------------------------------------

	  //---------------------------- Summary plots ---------------------------------
	  //-------- hmdc_summ_MBOvsTDC ------------------------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_summ_MBOvsTDC[",s,"][",m,"]");
	  hmdc_summ_MBOvsTDC[s][m]=new TH2F(name,name,96,0.5,96.5, 16,0.5,16.5);
	  hmdc_summ_MBOvsTDC[s][m]->SetXTitle("TDC channel number");
	  hmdc_summ_MBOvsTDC[s][m]->SetYTitle("MBO number");
	  hmdc_summ_MBOvsTDC[s][m]->SetOption("COLZ");
	  //----------------------------------------------------------------------------

	  //-------- hmdc_summ_dNdNhits ------------------------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_summ_dNdNhits[",s,"][",m,"]");
	  hmdc_summ_dNdNhits[s][m]=new TH1F(name,name,50,0.,50. );
	  hmdc_summ_dNdNhits[s][m]->SetXTitle("N of hits");
	  hmdc_summ_dNdNhits[s][m]->SetYTitle("dN/dNhits");
	  hmdc_summ_dNdNhits[s][m]->SetOption("");
	  //----------------------------------------------------------------------------

	  //-------- hmdc_summ_t2mt1_vs_t1 ---------------------------------------------
	  sprintf(name,"%s%i%s%i%s","hmdc_summ_t2mt1_vs_t1[",s,"][",m,"]");
	  hmdc_summ_t2mt1_vs_t1[s][m]=new TH2F(name,name, 250,0.,500., 250, 0., 500.);
	  hmdc_summ_t2mt1_vs_t1[s][m]->SetXTitle("t1 [ns]");
	  hmdc_summ_t2mt1_vs_t1[s][m]->SetYTitle("t2-t1 [ns]");
	  hmdc_summ_t2mt1_vs_t1[s][m]->SetOption("COLZ");
	  //----------------------------------------------------------------------------

	  //-------- hmdc_summ_dNdWire -------------------------------------------------
	  Int_t colorsSector[4]={4,2,8,6}; //r,b,m,g//
	  for(Int_t cut=0; cut<3; cut++){ //for each kind of cut used.
	    for(Int_t l=0; l<6; l++){ //for each layer
	      sprintf(name,"%s%i%s%i%s%i%s%i%s","hmdc_summ_dNdWire[",s,"][",m,"][",l,"][",cut,"]");
	      hmdc_summ_dNdWire[s][m][l][cut]=new TH1F(name,name,191,-0.5,190.5 );
	      hmdc_summ_dNdWire[s][m][l][cut]->SetXTitle("N of Wire");
	      hmdc_summ_dNdWire[s][m][l][cut]->SetYTitle("dN/dWire");
	      hmdc_summ_dNdWire[s][m][l][cut]->SetOption("");
	      hmdc_summ_dNdWire[s][m][l][cut]->SetLineColor(colorsSector[cut]);
	      hmdc_summ_dNdWire[s][m][l][cut]->SetFillColor(colorsSector[cut]);
	    }
	  }
	  //----------------------------------------------------------------------------



	  //----------------------------------------------------------------------------

	  //#############################################################################
     
	}
    }
  //-------- other summary plots [4]x[6]=24 channels----------------------------//

  //time Cuts storage for this file---------------------------------------------
  for(Int_t p=0; p<4; p++){
    sprintf(name,"%s%i","hmdc_summ_tCuts_info",p);
    hmdc_summ_tCuts_info[p] = new TH1F(name,name,6,0.,6.);
    hmdc_summ_tCuts_info[p]->SetXTitle("t1CutL,t1CutR,t2CutL,t2CutR,t2-t1CutL,t2-t1CutR");
  }
  //----------------------------------------------------------------------------


  sprintf(name,"%s","hmdc_summ_trend_meantime1_plane");
  hmdc_summ_trend_meantime1_plane=new TH1F(name,name,4,0,4);
  hmdc_summ_trend_meantime1_plane->SetMarkerStyle(29);
  hmdc_summ_trend_meantime1_plane->SetMarkerSize(.8);
  hmdc_summ_trend_meantime1_plane->SetXTitle("Plane Number");
  hmdc_summ_trend_meantime1_plane->SetYTitle("Mean Value");
  hmdc_summ_trend_meantime1_plane->SetOption("PL");


  sprintf(name,"%s","hmdc_summ_trend_meantime1");
  hmdc_summ_trend_meantime1=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_meantime1->SetMarkerStyle(29);
  hmdc_summ_trend_meantime1->SetMarkerSize(.8);
  hmdc_summ_trend_meantime1->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ ms }}");
  hmdc_summ_trend_meantime1->SetYTitle("Mean Value");
  hmdc_summ_trend_meantime1->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_meantime1not_t1");
  hmdc_summ_trend_meantime1not_t1=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_meantime1not_t1->SetMarkerStyle(29);
  hmdc_summ_trend_meantime1not_t1->SetMarkerSize(.8);
  hmdc_summ_trend_meantime1not_t1->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_meantime1not_t1->SetYTitle("Mean Value");
  hmdc_summ_trend_meantime1not_t1->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_meantime1not_t2");
  hmdc_summ_trend_meantime1not_t2=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_meantime1not_t2->SetMarkerStyle(29);
  hmdc_summ_trend_meantime1not_t2->SetMarkerSize(.8);
  hmdc_summ_trend_meantime1not_t2->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_meantime1not_t2->SetYTitle("Mean Value");
  hmdc_summ_trend_meantime1not_t2->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_meantime1not_t12");
  hmdc_summ_trend_meantime1not_t12=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_meantime1not_t12->SetMarkerStyle(29);
  hmdc_summ_trend_meantime1not_t12->SetMarkerSize(.8);
  hmdc_summ_trend_meantime1not_t12->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_meantime1not_t12->SetYTitle("Mean Value");
  hmdc_summ_trend_meantime1not_t12->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_neighbors");
  hmdc_summ_trend_neighbors=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_neighbors->SetMarkerStyle(29);
  hmdc_summ_trend_neighbors->SetMarkerSize(.8);
  hmdc_summ_trend_neighbors->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_neighbors->SetYTitle("Mean Value");
  hmdc_summ_trend_neighbors->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_123ratio");
  hmdc_summ_trend_123ratio=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_123ratio->SetMarkerStyle(29);
  hmdc_summ_trend_123ratio->SetMarkerSize(.8);
  hmdc_summ_trend_123ratio->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_123ratio->SetYTitle("Mean Value");
  hmdc_summ_trend_123ratio->SetOption("PL");

  sprintf(name,"%s","hmdc_summ_trend_multiplicity");
  hmdc_summ_trend_multiplicity=new TH1F(name,name,24,0,24);
  hmdc_summ_trend_multiplicity->SetMarkerStyle(29);
  hmdc_summ_trend_multiplicity->SetMarkerSize(.8);
  hmdc_summ_trend_multiplicity->SetXTitle("for(m=0;m<4;m++){for(s=0;s<6;s++){ mc }}");
  hmdc_summ_trend_multiplicity->SetYTitle("Mean Value");
  hmdc_summ_trend_multiplicity->SetOption("PL");
  //----------------------------------------------------------------------------//


  //###################### CREATE PER PLANE ####################################
  for(Int_t m=0;m<4;m++)
    {
      //------------------- hmdc_trend_meantime1_plane -----------------------------
      sprintf(name,"%s%i%s","hmdc_trend_meantime1_plane[",m,"]");
      hmdc_trend_meantime1_plane[m]=new TH1F(name,name,nBinsTrendX,0,nBinsTrendX);
      // 	hmdc_trend_meantime1_plane[m]=new TH1F(name,name,200,0,200);
      hmdc_trend_meantime1_plane[m]->SetLineColor(colorsModule[m]);
      hmdc_trend_meantime1_plane[m]->SetMarkerColor(colorsModule[m]);
      hmdc_trend_meantime1_plane[m]->SetMarkerStyle(29);
      hmdc_trend_meantime1_plane[m]->SetMarkerSize(.8);
      sprintf(axistitle,"%s%i%s","step in file [",stepsize_trend_meantime1," events]");
      hmdc_trend_meantime1_plane[m]->SetXTitle(axistitle);
      hmdc_trend_meantime1_plane[m]->SetYTitle("Mean time1 [ns]");
      hmdc_trend_meantime1_plane[m]->SetOption("P");
      sprintf(name,"%s%i%s","hmdc_time1_plane[",m,"]");
      hmdc_time1_plane[m]=new TH1F(name,name,500,-100,900);
      //----------------------------------------------------------------------------


    }
  //############################################################################

}
void HMdcLowLevelQA::writeHistograms(Int_t s,Int_t mo,Int_t l,Int_t c,Int_t level,Int_t main)
{
    // Writes hists to file dependend on level (0=raw,1=cal), main (1=summary,2=plane),
    // sector,module,layer/mb,cell/channel.-99 means that this ind3ex is not used.
  TH1 *tmp;
  TString name;
  Int_t bins;
  
	// calculate number of bins needed for trend histograms based on number of analyzed events
  bins = (Int_t)TMath::Floor(Eventcounter / stepsize_trend_meantime1);

    if(level==0)
    {//########################### WRITE RAW ######################################
      	if(main==1)
	{// write in directory summary
		a_thisFileName->Write();

                TString uFile(fNameRootHists);
                //cout <<"uFile "<<uFile << endl;
                //psFile  = dstFile( dstFile.Last('/')+1,dstFile.Last('.')-dstFile.Last('/')-1 )+".ps";
                //TString origFile = uFile( uFile.Last('/')+1,uFile.Last('_')-uFile.Last('/')-1 )+".hld";
                //TString origFile = uFile(0,uFile.Last('_'))+".hld";
                //cout <<"origFile "<<origFile << endl;
                //Char_t* ChOrigFile = (Char_t*)origFile.Data();
                //cout <<"ChOrigFile "<<ChOrigFile << endl;
                //TNamed *s1=new TNamed("thisFileName",ChOrigFile);
		TNamed *s1=new TNamed("thisFileName",uFile.Data());
                s1->Write();
                

		for(Int_t is=0; is<6 ; is++)
		{
		    for(Int_t im=0; im<4; im++)
		    {
			hmdc_summ_MBOvsTDC[   is][im]->Write();
                        hmdc_summ_dNdNhits[   is][im]->Write();
                        //hmdc_summ_t2mt1_vs_t1[is][im]->Write();
		    }
		}
	        //renormalise before
	        for(Int_t b=1; b<= 4; b++)
	        { // planes
		  hmdc_summ_trend_meantime1_plane->SetBinContent(b, hmdc_summ_trend_meantime1_plane->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
	        }
	        for(Int_t b=1; b<=24; b++)
	        { // sector, module
		  hmdc_summ_trend_meantime1->SetBinContent(b, hmdc_summ_trend_meantime1->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		  hmdc_summ_trend_meantime1not_t1->SetBinContent(b,hmdc_summ_trend_meantime1not_t1->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		  hmdc_summ_trend_meantime1not_t2->SetBinContent(b,hmdc_summ_trend_meantime1not_t2->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		  hmdc_summ_trend_meantime1not_t12->SetBinContent(b,hmdc_summ_trend_meantime1not_t12->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		  hmdc_summ_trend_neighbors->SetBinContent(b,hmdc_summ_trend_neighbors->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		  hmdc_summ_trend_123ratio->SetBinContent(b,hmdc_summ_trend_123ratio->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
	  	hmdc_summ_trend_multiplicity->SetBinContent(b,hmdc_summ_trend_multiplicity->GetBinContent(b)/Double_t(step_trend_meantime1_counter) );
		}
		// Rescale trend histograms to real number of 
		// Events = TMAth::Floor(Eventcounter / stepsize_trend_meantime1).
		tmp = hmdc_summ_trend_meantime1_plane;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_meantime1_plane = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_meantime1;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_meantime1 = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_meantime1not_t1;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_meantime1not_t1 = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_meantime1not_t2;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_meantime1not_t2 = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_meantime1not_t12;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_meantime1not_t12 = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_neighbors;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_neighbors = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_123ratio;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_123ratio = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
		tmp = hmdc_summ_trend_multiplicity;
		name = (Char_t *)tmp->GetName();
		tmp->SetName("tmp");
		hmdc_summ_trend_multiplicity = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
		delete tmp;
        	hmdc_summ_trend_meantime1_plane->Write();
		hmdc_summ_trend_meantime1->Write();
		hmdc_summ_trend_meantime1not_t1->Write();
		hmdc_summ_trend_meantime1not_t2->Write();
		hmdc_summ_trend_meantime1not_t12->Write();
		hmdc_summ_trend_neighbors->Write();
		hmdc_summ_trend_123ratio->Write();
		hmdc_summ_trend_multiplicity->Write();

	}
        else if(main==2)
	{// write in directory plane


	}
	else if(s!=-99&&mo==-99&&l==-99&&c==-99&&main==-99)
	{// write per sector

	}
        else if(s!=-99&&mo!=-99&&l==-99&&c==-99&&main==-99)
	{// write per module

	}
        else if(s!=-99&&mo!=-99&&l!=-99&&c==-99&&main==-99)
	{
	  // Rescale trend histograms to real number of Events = TMAth::Floor(Eventcounter / stepsize_trend_meantime1).
	  
	  tmp = hmdc_trend_NhitsPERmbo[s][mo][l];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_NhitsPERmbo[s][mo][l] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  
	  // write per mb

	    hmdc_trend_NhitsPERmbo[s][mo][l]->Write();

	}
        else if(s!=-99&&mo!=-99&&l!=-99&&c!=-99&&main==-99)
	{// write per channel


	}

    }
    else  //########################### WRITE CAL ######################################
    {// write Cal
	if(main==1)
	{// write in main directory summary

	for(Int_t is=0; is<6 ; is++)
	{
            for(Int_t im=0; im<4; im++)
	    {
                hmdc_summ_t2mt1_vs_t1[is][im]->Write();
                for(Int_t il=0; il<6; il++)
                {
                    for(Int_t cut=0; cut<3; cut++)
                    {
                        hmdc_summ_dNdWire[is][im][il][cut]->Write();
                    }
                }
	    }
	}


	}
        else  if(main==2)
	{// write in main directory plane
	    hmdc_trend_meantime1_plane[mo]->Write();

	}

	else if(s!=-99&&mo==-99&&l==-99&&c==-99&&main==-99)
	{// write per sector

	}
        else if(s!=-99&&mo!=-99&&l==-99&&c==-99&&main==-99)
	{
	  // Rescale trend histograms to real number of Events = TMAth::Floor(Eventcounter / stepsize_trend_meantime1).
	  
	  tmp = hmdc_trend_meantime1[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_meantime1[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_meantime1not_t1[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_meantime1not_t1[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_meantime1not_t2[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_meantime1not_t2[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_meantime1not_t12[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_meantime1not_t12[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_neighbors[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_neighbors[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_123ratio[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_123ratio[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_multiplicity[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_multiplicity[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  tmp = hmdc_trend_meanSlope[s][mo];
	  name = (Char_t *)tmp->GetName();
	  tmp->SetName("tmp");
	  hmdc_trend_meanSlope[s][mo] = (TH1F *)HTool::copyHistRange(tmp,name,0,-99,0,bins);
	  delete tmp;
	  // write per module
	    hmdc_trend_meantime1[       s][mo]->Write();
	    hmdc_trend_meantime1not_t1[ s][mo]->Write();
	    hmdc_trend_meantime1not_t2[ s][mo]->Write();
	    hmdc_trend_meantime1not_t12[s][mo]->Write();
	    hmdc_trend_neighbors[       s][mo]->Write();
	    hmdc_trend_123ratio[        s][mo]->Write();
            hmdc_trend_multiplicity[    s][mo]->Write();
            hmdc_trend_meanSlope[       s][mo]->Write();
            hmdc_Slope[                 s][mo]->Write();

            //Not a trend one but summary for the file to see if everithing is OK
            hmdc_time1_everithing_summ[ s][mo]->Write();
            hmdc_time1_good_t1_summ[    s][mo]->Write();
            hmdc_time1_not_t1_summ[     s][mo]->Write();
	}
        else if(s!=-99&&mo!=-99&&l!=-99&&c==-99&&main==-99)
	{// write per layer

	}
        else if(s!=-99&&mo!=-99&&l!=-99&&c!=-99&&main==-99)
	{// write per cell


	}

    }

}
void HMdcLowLevelQA::fillHistogramsCal(Int_t s,Int_t m,Int_t l,Int_t c,Float_t time1,Float_t time2)
{

    hmdc_time1_everithing_summ[s][m]->Fill(time1);  // fill working hist per sector and module regardless on time cuts

    if(cutResults[3]==1)
    { // all time cuts passed
	hmdc_time1[s][m]   ->Fill(time1);  // fill working hist per sector and module
	hmdc_time1_plane[m]->Fill(time1);  // fill working hist per plane
        hmdc_time1_good_t1_summ[s][m]->Fill(time1); // fill working hist per sector and module only if all cuts are OK for the file at once (summary)
    }
    else if(cutResults[0]==0)
    { // cut on time1 not passed
	hmdc_time1not_t1[s][m]->Fill(time1);       // fill working hist per sector and module
        hmdc_time1_not_t1_summ[s][m]->Fill(time1); // fill working hist per sector and module only if time1 cuts failed for the file at once (summary)
    }
    else if(cutResults[1]==0)
    { // cut on time2 not passed
	hmdc_time1not_t2[s][m]->Fill(time1);  // fill working hist per sector and module
    }
    else if(cutResults[2]==0)
    { // cut on time2-time1 not passed
	hmdc_time1not_t12[s][m]->Fill(time1);  // fill working hist per sector and module
    }
}
void HMdcLowLevelQA::fillHistogramsRaw(Int_t s,Int_t m,Int_t mb,Int_t ch,Float_t time1,Float_t time2)
{
    // hmdc_summ_MBOvsTDC (independent from any cuts)
    hmdc_summ_MBOvsTDC[s][m]->Fill(ch+1, mb+1);
}

TDirectory *HMdcLowLevelQA::Mkdir(TDirectory *dirOld,const Char_t *c, Int_t i, Int_t p)    //! Makes new Dir, changes to Dir, returns pointer to new Dir
{
    // Function to create subdirectories
    static Char_t sDir[255];// = new Char_t[strlen(c)+3];
    static Char_t sFmt[10];
    if(i!=-99)
    {
	sprintf(sFmt, "%%s %%0%1ii", p);
	sprintf(sDir, sFmt, c, i);
    }
    else
    {
	sprintf(sDir,"%s", c);
    }
    TDirectory *dirNew = dirOld->mkdir(sDir);
    dirOld->cd(sDir);
    //  free (sDir);
    return dirNew;
}

void HMdcLowLevelQA::getSetup()
{
    HDetector *mdcDet = gHades->getSetup()->getDetector("Mdc");

    for(Int_t m=0; m<4; m++)
    {  //loop over modules
	numberOfSectorsPerModule[m]=0;
    }

    for(Int_t s=0; s<6; s++)
    {  //loop over sectors
	for(Int_t m=0; m<4; m++)
	{  //loop over modules
	    if (!mdcDet->getModule(s, m))
	    {
		mySetup[s][m]=0;
	    }
	    else
	    {
		mySetup[s][m]=1;
                numberOfSectorsPerModule[m]++;
	    }
	}
    }
}
Bool_t HMdcLowLevelQA::finalize(void)
{
    TFile *file = NULL;
    if (fNameRootHists)
    {
	file = new TFile(fNameRootHists,"RECREATE");
	file->cd();
	HDetector *mdcDet = gHades->getSetup()->getDetector("Mdc");

	if (!mdcDet){
	  ERROR_msg(HMessageMgr::DET_MDC,"HMdcOffsetCheck::finalize: Mdc-Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing.");
	}
	/*cout << "Error in HMdcOffsetCheck::finalize: Mdc-Detector setup (gHades->getSetup()->getDetector(\"Mdc\")) missing." << endl;*/
	else
	    for(Int_t s=0; s<6; s++)
	    {  //loop over sectors
	      //cout<<"Sector "<<s<<endl;
		TDirectory *dirSec=NULL;
		if (file) dirSec=Mkdir(file, "sector", s);

		//################### WRITE PER SECTOR ##############################
		writeHistograms(s,-99,-99,-99,1,-99);// write hists of cal1


		//###################################################################
		for(Int_t mo=0; mo<4; mo++)
		{  //loop over modules
		  //cout<<"Module "<<mo<<endl;
		    if (!mdcDet->getModule(s, mo)) continue;
		    TDirectory *dirMod=NULL;
		    if (dirSec) dirMod=Mkdir(dirSec, "module", mo);

		    //################### WRITE PER MODULE ##############################
                    writeHistograms(s,mo,-99,-99,1,-99);// write hists of cal1
                         //################### WRITE PER MBO  ################################
                         for(Int_t mb=0; mb<16; mb++){
                         writeHistograms(s,mo,mb,-99,0,-99);// write hists of raw
                         }
		    //###################################################################
		   
			 //cout<<" "<<endl;
		    if (dirMod) dirMod->TDirectory::Cd("..");
		}
		//cout<<" "<<endl;
		if (dirSec) dirSec->TDirectory::Cd("..");
	    }
	//cout << endl;



	TDirectory *dirPlane=NULL;
        if (file) dirPlane=Mkdir(file, "Plane", -99);
	//################### WRITE PER PLANE ##############################
	for(Int_t m=0; m<4; m++)
	{  //loop over planes
	    writeHistograms(-99,m,-99,-99,1,2);
	}
	//##################################################################
        if (dirPlane) dirPlane->TDirectory::Cd("..");

	TDirectory *dirSummary=NULL;
        if (file) dirSummary=Mkdir(file, "Summary", -99);
	//################### WRITE SUMMARY ################################
	writeHistograms(-99,-99,-99,-99,0,1); //RAW
	writeHistograms(-99,-99,-99,-99,1,1); //CAL1
        writeTimeCuts();

	//##################################################################
	if (dirSummary) dirSummary->TDirectory::Cd("..");


	file->Save();
	file->Close();
    }
    else
    {
	Error("HMdcLowLevelQA:finalize()","NO OUTPUT DEFINED!");
    }
    return kTRUE;

}

Int_t HMdcLowLevelQA::execute()
{
    //cout << "----   ---- event ---- ----" << endl;
    Eventcounter++;
    //########################## FILL TREND HISTS ################################
    if(Eventcounter%stepsize_trend_meantime1==0)
    {
	if(step_trend_meantime1_counter<=200)
	{
	    for(Int_t i=0;i<6;i++)     // loop over sector
	    {
		for(Int_t j=0;j<4;j++) // loop over module
		{
		    if(hmdc_time1[i][j]->Integral()>0)
		    {

                        hmdc_trend_meanSlope[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1[i][j]->GetMean());

			hmdc_trend_meantime1[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1[i][j]->GetMean());
			hmdc_trend_meantime1[i][j]->SetBinError(  step_trend_meantime1_counter,stepsize_trend_meantime1/hmdc_time1[i][j]->Integral());
                        hmdc_summ_trend_meantime1->Fill(Double_t(j*6+i+0.5),Double_t(hmdc_time1[i][j]->GetMean()) );
		    }
		    hmdc_time1[i][j]->Reset();

		    if(hmdc_time1not_t1[i][j]->Integral()>0)
		    {
			hmdc_trend_meantime1not_t1[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1not_t1[i][j]->GetMean());
			hmdc_trend_meantime1not_t1[i][j]->SetBinError(  step_trend_meantime1_counter,stepsize_trend_meantime1/hmdc_time1not_t1[i][j]->Integral());
                        hmdc_summ_trend_meantime1not_t1->Fill(Double_t(j*6+i+0.5),Double_t(hmdc_time1not_t1[i][j]->GetMean()) );
		    }
		    hmdc_time1not_t1[i][j]->Reset();

		    if(hmdc_time1not_t2[i][j]->Integral()>0)
		    {
			hmdc_trend_meantime1not_t2[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1not_t2[i][j]->GetMean());
			hmdc_trend_meantime1not_t2[i][j]->SetBinError(  step_trend_meantime1_counter,stepsize_trend_meantime1/hmdc_time1not_t2[i][j]->Integral());
                        hmdc_summ_trend_meantime1not_t2->Fill(Double_t(j*6+i+0.5),Double_t(hmdc_time1not_t2[i][j]->GetMean()) );
		    }
                    hmdc_time1not_t2[i][j]->Reset();

		    if(hmdc_time1not_t12[i][j]->Integral()>0)
		    {
			hmdc_trend_meantime1not_t12[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1not_t12[i][j]->GetMean());
			hmdc_trend_meantime1not_t12[i][j]->SetBinError(  step_trend_meantime1_counter,stepsize_trend_meantime1/hmdc_time1not_t12[i][j]->Integral());
                        hmdc_summ_trend_meantime1not_t12->Fill(Double_t(j*6+i+0.5),Double_t(hmdc_time1not_t12[i][j]->GetMean()) );
		    }
		    hmdc_time1not_t12[i][j]->Reset();

		    if(double_all[i][j][1]!=0)
		    {
			hmdc_trend_neighbors[i][j]->SetBinContent(step_trend_meantime1_counter,(Float_t)double_all[i][j][0]/(Float_t)double_all[i][j][1]);
                        hmdc_summ_trend_neighbors->Fill( Double_t(j*6+i+0.5) , Double_t(double_all[i][j][0])/Double_t(double_all[i][j][1]) );
		    }
		    else
		    {
			hmdc_trend_neighbors[i][j]->SetBinContent(step_trend_meantime1_counter,0);
		    }

		    if(stepsize_trend_meantime1>0)
		    {
			hmdc_trend_123ratio[i][j]->SetBinContent(step_trend_meantime1_counter,(Float_t(cnt123[i][j])/Float_t(stepsize_trend_meantime1)) );
                        hmdc_summ_trend_123ratio->Fill(Double_t(j*6+i+0.5),(Double_t(cnt123[i][j])/Double_t(stepsize_trend_meantime1))  );
		    }
		    //################### reset "step" variables #######################
		    cnt123[i][j]=0;

		    //################### filling the trend histogram for s,m,mb counters ###############
		    for(Int_t mb=0; mb<16; mb++){
			if(stepsize_trend_meantime1>0)
			{
			    hmdc_trend_NhitsPERmbo[i][j][mb]->SetBinContent(step_trend_meantime1_counter,Float_t(NhitsPERmbo[i][j][mb])/Float_t(stepsize_trend_meantime1));
			}
			NhitsPERmbo[i][j][mb]=0; //resetting for the next iteration step
		    }

		    if(hmdc_multiplicity[i][j]->Integral()>0)
		    {
			hmdc_trend_multiplicity[i][j]->SetBinContent(step_trend_meantime1_counter,hmdc_multiplicity[i][j]->GetMean());
			hmdc_trend_multiplicity[i][j]->SetBinError(  step_trend_meantime1_counter,hmdc_multiplicity[i][j]->GetRMS()/hmdc_multiplicity[i][j]->Integral());
                        hmdc_summ_trend_multiplicity->Fill(Double_t(j*6+i+0.5),Double_t(hmdc_multiplicity[i][j]->GetMean()) );
		    }
		    hmdc_multiplicity[i][j]->Reset();
		}
	    }
	    for(Int_t j=0;j<4;j++)// loop over planes
	    {
		if(hmdc_time1_plane[j]->Integral()>0)
		{
		    hmdc_trend_meantime1_plane[j]->SetBinContent(step_trend_meantime1_counter,hmdc_time1_plane[j]->GetMean());
		    hmdc_trend_meantime1_plane[j]->SetBinError(step_trend_meantime1_counter,(stepsize_trend_meantime1*numberOfSectorsPerModule[j])/hmdc_time1_plane[j]->Integral());
                    hmdc_summ_trend_meantime1_plane->Fill(Double_t(j), Double_t(hmdc_time1_plane[j]->GetMean()));
		}
		hmdc_time1_plane[j]->Reset();
	    }

	    step_trend_meantime1_counter++;  // count up the bin to be filled
	    resetCounters();
	}
	else
	{
	    Error("HMdcLOwLevelQA:execute()","Maximum number of steps in trend histograms exceeded!");
	}
    }
    //############################################################################


    HMdcCal1* cal=0;
    HMdcRaw* raw=0;

    Int_t mb, ch;
    Int_t l, c;
    Int_t s,m;
    Float_t time1=-99;
    Float_t time2=-99;

    itercal1->Reset();
    iterraw->Reset();

    //########################## Fill RAW HISTS ##################################
    while ((raw=(HMdcRaw *)iterraw->Next())!=0) {
	raw->getAddress(s,m,mb,ch);
        //cout<<">>s,m,mb,ch =="<<s<<" "<<m<<" "<<mb<<" "<<ch<< endl;

	time1=raw->getTime(1);
        time2=raw->getTime(2);

	time1=-99;
        time2=-99;

	//############# s,m,mb combination counter ##############################
	{
	    if(s==smmb_old[0] && m==smmb_old[1] && mb==smmb_old[2]){
	    }else{
		//new chamber is read from "cal->getAddress(s,m,l,c);"
                smmb_old[0]=s, smmb_old[1]=m, smmb_old[2]=mb;  //store the old step values
	    }
            NhitsPERmbo[s][m][mb]++;
	}
        //#######################################################################

	fillHistogramsRaw(s, m, mb, ch, time1, time2);

    }
    //############################################################################

    //######################### FIll CAL1 HISTS ##################################
    while ((cal=(HMdcCal1 *)itercal1->Next())!=0) {
	cal->getAddress(s,m,l,c);
	//cout<<">>s,m,l,c =="<<s<<" "<<m<<" "<<l<<" "<<c<< endl;


	//############# 123 cells counter #######################################
	{
	    if(s==sm_old[0] && m==sm_old[1])
	    {
	    }
	    else
	    {
		//new chamber started to be red from "cal->getAddress(s,m,l,c);"
		if(sect_module_NumberOfHits[sm_old[0]][sm_old[1]]>0 &&
		   sect_module_NumberOfHits[sm_old[0]][sm_old[1]]<4    )
		{
                 cnt123[sm_old[0]][sm_old[1]]++;
		}
                hmdc_multiplicity[ s][m]->Fill(sect_module_NumberOfHits[s][m]);
                hmdc_summ_dNdNhits[s][m]->Fill(sect_module_NumberOfHits[s][m]); //the same as "hmdc_multiplicity" but will be saved and has less number of bins
		sect_module_NumberOfHits[s][m]=0;
                sm_old[0]=s, sm_old[1]=m;  //store the old step values
	    }
            sect_module_NumberOfHits[s][m]++;
	}
        //#######################################################################

	HMdcLookupCell& cell=(*lookupRaw)[s][m][l][c];
	mb=cell.getNMoth();
	ch=cell.getNChan();
	time1=cal->getTime1();
        time2=cal->getTime2();
        
        testTimeCuts(s,m,time1,time2);
        lookupSlopes(s,m);
	//############# FILL NEIGHBORS/ALL #######################################
        if(cutResults[3]==1)checkNeighboringCells(s,m,c);// if time cuts are passed
	//########################################################################
        fillHistogramsCal(s,m,l,c,time1,time2);

        hmdc_summ_dNdWire[s][m][l][0]->Fill(Double_t(c-0.5),1.);
        if(cutResults[0]==1){
           hmdc_summ_dNdWire[s][m][l][1]->Fill(Double_t(c-0.5),1.);
        }
        if(cutResults[3]==1){
           hmdc_summ_dNdWire[s][m][l][2]->Fill(Double_t(c-0.5),1.);
        }
        

	time1=-99;
        time2=-99;

    }
    //############################################################################



    return 0;
}

