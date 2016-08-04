//*-- AUTHOR : Jerzy Pietraszko, Ilse Koenig 
//*-- Created : 03/02/2006
//*-- Modified: 14/09/2006 by P.Cabanelas
//*-- Modified: 27/08/2007 by P.Cabanelas

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HRpcTrbUnpacker
//
//  Class for unpacking TRB data and filling 
//	the Rpc Raw category
//  Base subevent decoding is performed by decode function
//  see: htrbbaseunpacker.h (/base/datasource/) 
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hrpctrbunpacker.h"
#include "rpcdef.h"
#include "htrblookup.h"
#include "hrpcraw.h"
#include "hdebug.h"
#include "hades.h"
#include "hevent.h"
#include "hspectrometer.h"
#include "hdetector.h"
#include "hruntimedb.h"
#include "hcategory.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "TCanvas.h"
#include "TStyle.h"


ClassImp(HRpcTrbUnpacker) 

  HRpcTrbUnpacker::HRpcTrbUnpacker (UInt_t id):HTrbBaseUnpacker(id) {
  // constructor
  pRawCat = NULL;
  fNoTimeRefCorr=false;
  fControlHistograms=false;
  histdbos = NULL;
  histdbombo = NULL;
  histmbotrb = NULL;
  histtrbchan = NULL;
  debugRpcFlag=0;
  printf("\n\nDetector Id: %d\n\n",id);
}

Bool_t HRpcTrbUnpacker::init (void) {
  // creates the raw category and gets the pointer to the lookup table
  pRawCat = gHades->getCurrentEvent()->getCategory(catRpcRaw);
  if (!pRawCat) {
    pRawCat = gHades->getSetup()->getDetector("Rpc")->buildCategory(catRpcRaw);
    if (!pRawCat)
      return kFALSE;
    gHades->getCurrentEvent()->addCategory(catRpcRaw,pRawCat,"Rpc");
  }
  loc.set(3,0,0,0);

  lookup = (HTrbLookup *)(gHades->getRuntimeDb()->getContainer("TrbLookup"));
  if (!lookup) {cerr << "init(): could not get container TrbLookup" << endl;  return kFALSE; }


  if(fControlHistograms){
    TString name="MBO*5+DBOinput_DBOnum_";
    name+=getSubEvtId();
    histdbos = new TH2F(name,name,9,-1,8,21,-1,20);
    name="MBOnum_DBOnum_";
    name+=getSubEvtId();
    histdbombo = new TH2F(name,name,9,-1,8,5,-1,4);
    name="TRBnum_MBOnum_";
    name+=getSubEvtId();
    histmbotrb = new TH2F(name,name,5,-1,4,5,-1,4);
    name="TRBnum_CHANnum_";
    name+=getSubEvtId();
    histtrbchan = new TH2F(name,name,129,-1,128,5,-1,4);
  }

  return kTRUE;
}

Bool_t HRpcTrbUnpacker::finalize(void)
{
  if(fControlHistograms){
    if(histdbos){
      histdbos->Write();
      gStyle->SetPalette(1);
      TCanvas c1("c1","c1");
      histdbos->Draw("colz");
      histdbos->SetStats(kFALSE);
      c1.Print(histdbos->GetName()+TString(".ps"));
      delete histdbos;
      histdbos=NULL;
    }
    if(histdbombo){
      histdbombo->Write();
      gStyle->SetPalette(1);
      TCanvas c1("c1","c1");
      histdbombo->Draw("colz");
      histdbombo->SetStats(kFALSE);
      //c1.Pad()->SetLogz();
      c1.Print(histdbombo->GetName()+TString(".ps"));
      delete histdbombo;
      histdbombo=NULL;
    }
    if(histmbotrb){
      histmbotrb->Write();
      gStyle->SetPalette(1);
      TCanvas c1("c1","c1");
      histmbotrb->Draw("colz");
      histmbotrb->SetStats(kFALSE);
      c1.Print(histmbotrb->GetName()+TString(".ps"));
      delete histmbotrb;
      histmbotrb=NULL;
    }
    if(histtrbchan){
      histtrbchan->Write();
      gStyle->SetPalette(1);
      TCanvas c1("c1","c1");
      histtrbchan->Draw("colz");
      histtrbchan->SetStats(kFALSE);
      c1.Print(histtrbchan->GetName()+TString(".ps"));
      delete histtrbchan;
      histtrbchan=NULL;
    }

  }
  return kTRUE;
}


Int_t HRpcTrbUnpacker::execute(void) {

  HRpcRaw *pRaw = 0;	  // pointer to Raw category 
  Int_t nEvt = 0;	  // Evt Seq. Nb. 
  Char_t side = 'U';      // side declaration
  Float_t ns2bin = 10.;   // Connversion from ns to bin [100 ps TDC bin]
  Int_t FeAddress;

  nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

  // decode subevent data
  if (pSubEvt) {		// pSubEvt - make sure that there is something for decoding 
    decode();			// decode the subevent and fill arrays, see: htrbbaseunpacker.cc 

    // get the part of the lookup table which belongs to the subevent ID
    // in order to check which TRBchannel belongs to the RpcDetector

    /////////////////////////////////////////////////////////////////////
    //
    //  In Hydra Allowed SubEvents Ids for TRB starts from 800...
    //  Check RPC ID in data  
    //  
    /////////////////////////////////////////////////////////////////////

    // Correct times by Reference Time
    if(!fNoTimeRefCorr){
      switch(trbDataVer){
      case 0:   // Old data structure
      case 2:   // RefTime in the last channel of each TDC (RPC style) 
	correctRefTimeCh31();
	break;
      case 3:   // FIXME: Obsolote. Include the case with average of the 4 reference times.
	correctRefTimeCh31();
	//correctRefTimeCh127();
	break;
      default:  // Do nothing
	break;
      }
    }


    HTrbLookupBoard *board = lookup->getBoard(subEvtId);
    if (!board) {  
      // Problem: unpacker for specific SubId was called but for this SubId 
      // no channel in lookup table exist. 
      // 	
      Warning ("execute", "SubEvent: %i unpacked but channel in Lookup table does not exist", subEvtId);
    }

    //fill RAW category for the Rpc detector 
    for (Int_t i = 0; i < 128; i++) {
      if (trbLeadingMult[i]<1 && trbTrailingTotalMult[i]<1) continue;		
      //Leading Time data and Trailing for this channel has to exist 
      // fill rpc raw category
      HTrbLookupChan *chan = board->getChannel(i);
      if (chan && 'R' == chan->getDetector()) {	//Channel belongs to Rpc detector

	side      = chan->getSide();
	loc[0]    = chan->getSector();
	loc[1]    = chan->getCell()/100;
	loc[2]    = chan->getCell() - 100*loc[1];
	FeAddress = chan->getFeAddress();

	pRaw = (HRpcRaw *)pRawCat->getObject(loc);
	if (!pRaw) {
	  pRaw = (HRpcRaw *)pRawCat->getSlot(loc);
	  pRaw = new (pRaw) HRpcRaw;
	}
	if (pRaw) {

	  if((pRaw->getRightTot()>=0)&&(pRaw->getLeftTot()>=0)) {
#if DEBUG_LEVEL>4
	    Warning("execute", "Slot already exists!\n"); //I.e. the time right and left was filled already!
#endif
	    continue;
	  }

	  pRaw->setAddress(loc[0],loc[1],loc[2]);

	  if(side=='r') {
	    pRaw->setRightTime(trbLeadingTime[i][0]);
	    pRaw->setRightTot(trbADC[i][0]);
	    pRaw->setRightTime2(trbLeadingTime[i][1]);
	    pRaw->setRightTot2(trbADC[i][1]);
	  } else {
	    pRaw->setLeftTime(trbLeadingTime[i][0]);
	    pRaw->setLeftTot(trbADC[i][0]);
	    pRaw->setLeftTime2(trbLeadingTime[i][1]);
	    pRaw->setLeftTot2(trbADC[i][1]);
	  }

	  // Algorithm for measurements of ToT of streamers with first leading and last trailing
	  Float_t totLast=trbADC[i][0];     // default: totLast = 1st ADC
	  Int_t lastTotFlag=0;              // flag for data info 
	  Float_t max_time=500.;            // [ns] Maximum time for considering that the hit 
	  Int_t  k_trail_fail =1;
	  Int_t lastTrailingIndex=-1;
	  Int_t k;

	  // corresponds to the 'next event'

	  if(trbLeadingMult[i]>1) {         // we have more than one leading (and therefore trailing)

	    for(k=0;k<trbLeadingMult[i];k++) {       
	      if(trbTrailingTime[i][k]<0) {
		k_trail_fail++;
		if (k>0) lastTrailingIndex = k-1;
	      }
	    }
	    if (k_trail_fail==k) {
	      //Warning("execute","No valid trailings: SubEvent %i, Evt %i, Chan %i", subEvtId,nEvt,i); 
	      totLast=-100000;
	    } 

	    if (lastTrailingIndex==-1) lastTrailingIndex = k; //All the trbTrailings are valid.

	    totLast = trbTrailingTime[i][lastTrailingIndex] - trbLeadingTime[i][0];

	    Int_t ind=0;
	    while(totLast>(max_time*ns2bin)) {    // totLast should be typically smaller than 500ns; 
	      // if it is bigger, then we have two different events in the TRB window 
	      lastTotFlag=1;
	      ind++;
	      if(ind>lastTrailingIndex) cerr<<"algorithm for last Tot does not work!"<<endl;
	      totLast = trbTrailingTime[i][lastTrailingIndex-ind] - trbLeadingTime[i][0]; 
	    }
	  }

	  if(side=='r') {    // setting totLast to the HRpcRaw category
	    pRaw->setRightTotLast(totLast);
	  } else {
	    pRaw->setLeftTotLast(totLast);
	  }

	  Int_t trbDataType=0,extra=0;


	  if(side=='r') {
	    pRaw->setRightLogBit(trbLeadingMult[i],trbTrailingTotalMult[i],lastTotFlag,
				 (Int_t)trbDataPairFlag,trbDataVer,trbDataType,extra); 
	  } else {
	    pRaw->setLeftLogBit(trbLeadingMult[i],trbTrailingTotalMult[i],lastTotFlag,
				(Int_t)trbDataPairFlag,trbDataVer,trbDataType,extra); 
	  }

	  // Setting FEE address
	  Int_t dboInput,dboNum,mboNum,trbNum;
                            
	  trbNum    = FeAddress/1000000;
	  mboNum    = (FeAddress-trbNum*1000000)/10000;
	  dboNum    = (FeAddress-trbNum*1000000-mboNum*10000)/100;
	  dboInput  = FeAddress-trbNum*1000000-mboNum*10000-dboNum*100;
	  
	  if (side=='r') pRaw->setRightFeeAddress(subEvtId,i,dboInput,dboNum,mboNum,trbNum);
	  if (side=='l') pRaw->setLeftFeeAddress(subEvtId,i,dboInput,dboNum,mboNum,trbNum);
	
	  // Filling FEE Control Histograms
	  if(fControlHistograms && histdbos && histdbombo && histmbotrb && histtrbchan) {
	    histdbos->Fill(dboNum,mboNum*5+dboInput);
	    histdbombo->Fill(dboNum,mboNum);
	    histmbotrb->Fill(mboNum,trbNum);
	    histtrbchan->Fill(i,trbNum);
	  }
	  
	} else {
	  Error ("execute()", "Can't get slot");
	  cerr << "execute(): Can't get slot Column: "<<loc[1]<< " Cell: " 
	       <<loc[2]<< " Sec: " <<loc[0]<< " SubEvent: " 
	       <<subEvtId << " EvtNb: " <<nEvt << endl;
	  return -1;
	}
      }  // channel belongs to RPC 
    }  // for(Int_t i=0; i<128; i++)  
  }	//if(pSubEvt)

  return 1;

}
