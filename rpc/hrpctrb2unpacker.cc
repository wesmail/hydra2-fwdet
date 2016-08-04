//*-- AUTHOR  : P.Cabanelas 
//*-- Created : 16/06/2010

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
//  HRpcTrb2Unpacker
//
//  Class for unpacking TRB data and filling 
//	the Rpc Raw category
//  Base subevent decoding is performed by decode function
//  see: htrb2unpacker.h (/base/datasource/) 
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hrpctrb2unpacker.h"
#include "rpcdef.h"
#include "hrpctrb2lookup.h"
#include "htrbnetaddressmapping.h"
#include "htrb2correction.h"
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
#include "hdatasource.h"
#include <iostream>
#include <iomanip>
#include <stdlib.h>

#include "TCanvas.h"
#include "TStyle.h"


ClassImp(HRpcTrb2Unpacker) 

HRpcTrb2Unpacker::HRpcTrb2Unpacker (UInt_t id):HTrb2Unpacker(id) {
    // constructor
    pRawCat = NULL;
    bNoTimeRefCorr=false;
    bStoreSpareChanData=false;

}

Bool_t HRpcTrb2Unpacker::init (void) {

    // creates the raw category and gets the pointer to the lookup table
    pRawCat = gHades->getCurrentEvent()->getCategory(catRpcRaw);
    if (!pRawCat) {
	pRawCat = gHades->getSetup()->getDetector("Rpc")->buildCategory(catRpcRaw);
	if (!pRawCat)
	    return kFALSE;
	gHades->getCurrentEvent()->addCategory(catRpcRaw,pRawCat,"Rpc");
    }
    loc.set(3,0,0,0);

    lookup = (HRpcTrb2Lookup *)(gHades->getRuntimeDb()->getContainer("RpcTrb2Lookup"));
    if (!lookup) {cerr << "init(): could not get container RpcTrb2Lookup" << endl;  return kFALSE; }

    if (correctINL) {
	trbaddressmap=(HTrbnetAddressMapping*)(gHades->getRuntimeDb()->getContainer("TrbnetAddressMapping"));
	if (!trbaddressmap) {
	    Error("init","No Pointer to parameter container TrbnetAddressMapping.");
	    return kFALSE;
	}
    }

    return kTRUE;
}

Bool_t HRpcTrb2Unpacker::finalize(void)
{
    return kTRUE;
}


Int_t HRpcTrb2Unpacker::execute(void) {

    HRpcRaw *pRaw = 0;	   // pointer to Raw category
    Int_t nEvt = 0;	       // Evt Seq. Nb.
    Char_t side = 'U';     // side declaration
    Float_t ns2bin = 10.;  // Connversion from ns to bin [100 ps TDC bin]. Used only by TotLast.
    Int_t FeAddress;

    // decode subevent data
    if (pSubEvt) {		// pSubEvt - make sure that there is something for decoding

	nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

	// decodes the subevent and fill arrays, see: htrb2unpacker.h
	// decode() return values:
	//    100  - end of subevent, stop decoding by purpose
	//      0  - error, decoding failed
	//      1  - data  decode
	//     10  - IF CTS -> skip
	// decode all subsubevents until subevent is finished - even if there is an error
	Int_t DecodeOutput = 1;

	while (1) {

	    DecodeOutput = decode();

	    if(DecodeOutput==kDsSkip){
                Error("decode","subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x .... skip full Event.",nEvt,subEvtId);
                return kDsSkip;
	    }
	    if (DecodeOutput==0) {
		Error("decode","subsubevent decoding failed!!! Evt Nr : %i SubEvtId: %x .... skip Event.",nEvt,subEvtId);
		break;
	    }
	    if (DecodeOutput==100 || DecodeOutput==10) {
		uStartPosition=0;
		if ( debugFlag > 0 ) Info("decode","DecodeOutput : %i",DecodeOutput);
	       break;
	    }

	    // Correct times by Reference Time: still working???
	    if(!bNoTimeRefCorr){
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

	    HRpcTrb2LookupBoard *board = lookup->getBoard(uTrbNetAdress);
	    if (!board) {
		Warning ("execute", " Evt Nr : %i  TrbNetAdress: %x (%i) unpacked but TRB Board not in lookup table",nEvt,uTrbNetAdress,uTrbNetAdress);
		return 1;
	    }

	    // go to next TRB
	    uStartPosition = uStartPosition + uSubBlockSize + 1; //start TRB decoding at the correct startposition

	    //fill RAW category for the Rpc detector
	    for (Int_t i = 0; i < board->getSize(); i++) {
		if (trbLeadingMult[i]<1 && trbTrailingTotalMult[i]<1) continue;
		//Leading Time data and Trailing for this channel has to exist
		// fill rpc raw category
		HRpcTrb2LookupChan *chan = board->getChannel(i);

		if(chan->getSector()<0) {
		    Warning("execute", "No location for channel %i in board %x (%i)!\n",i,uTrbNetAdress,uTrbNetAdress);
		    continue;
		}

		side      = chan->getSide();
		loc[0]    = chan->getSector();
		loc[1]    = chan->getColumn();
		loc[2]    = chan->getCell();
		FeAddress = chan->getFeAddress();

		if(!bStoreSpareChanData && loc[2]>31) continue;

		pRaw = (HRpcRaw *)pRawCat->getObject(loc);
		if (!pRaw) {
		    pRaw = (HRpcRaw *)pRawCat->getSlot(loc);
		    pRaw = new (pRaw) HRpcRaw;
		}
		if (pRaw) {

		    if((pRaw->getRightTot()>0)&&(pRaw->getLeftTot()>0)) {
			if(debugFlag>1) Warning("execute", "Slot already exists!\n"); //I.e. the time right and left was filled already!
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
			    if(debugFlag>1) Warning("execute","No valid trailings: TrbNetAdress %i, Evt %i, Chan %i",uTrbNetAdress,nEvt,i);
			    totLast=-100000;
			}

			if (lastTrailingIndex==-1) lastTrailingIndex = k; //All the trbTrailings are valid.

			totLast = trbTrailingTime[i][lastTrailingIndex] - trbLeadingTime[i][0];

			Int_t ind=0;
			while(totLast>(max_time*ns2bin)) {    // totLast should be typically smaller than 500ns;
			    // if it is bigger, then we have two different events in the TRB window
			    lastTotFlag=1;
			    ind++;
			    //if(ind>lastTrailingIndex) cerr<<"algorithm for last Tot does not work!"<<endl;
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

		} else {
		    Error ("execute()", "Can't get slot");
		    cerr << "execute(): Can't get slot Column: "<<loc[1]<< " Cell: "
			<<loc[2]<< " Sec: " <<loc[0]<< " SubEvent: "
			<<subEvtId << " EvtNb: " <<nEvt << endl;
		    return -1;
		}

	    }  // for(Int_t i=0; i<128; i++)

	} //while loop

    }	//if(pSubEvt)

    return 1;

}
