//*-- AUTHOR Sergey Linev
//*-- Modified : 17/04/2014 by I. Koenig
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HTrb3TdcUnpacker
//
//  This is class to unpack and calibrate data of single FPGA TDC
//  It should be similar to functionality of hadaq::TdcProcessor from 'stream' framework
//
/////////////////////////////////////////////////////////////


#include "htrb3tdcunpacker.h"

#include "TString.h"

#include "htrb3tdciterator.h"
#include "htrb3tdcmessage.h"
#include "htrb3unpacker.h"
#include "htrb3calpar.h"
#include "hades.h"
#include "hevent.h"
#include "heventheader.h"

HTrb3TdcUnpacker::HTrb3TdcUnpacker(UInt_t id) : HTrb3Unpacker(id), 
   calpar(0),nCalSrc(0),fMinAddress(0), fMaxAddress(0)
{
	 fCalpars.clear();
	 fTDCs.clear();
	 fTDCsMap.clear();

}

HTrb3TdcUnpacker::~HTrb3TdcUnpacker()
{
   fCalpars.clear();
   fTDCs.clear();
   fTDCsMap.clear();
}

void HTrb3TdcUnpacker::clearAll()
{
	 if (!isQuietMode())
		 fprintf(stderr, "JAM:HTrb3TdcUnpacker::clearAll is called!\n");

   for (UInt_t ntds=0;ntds<fTDCs.size();ntds++) fTDCs[ntds].clear();
}

Int_t HTrb3TdcUnpacker::getEventSeqNumber()
{
   if (gHades == 0) return 0;
   if (gHades->getCurrentEvent() == 0) return 0;
   if (gHades->getCurrentEvent()->getHeader() == 0) return 0;
   return gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();
}

Int_t HTrb3TdcUnpacker::addTDC(UInt_t trbaddr, size_t numchannels)

{
    Int_t tdcindex=-1;
    TDC tdc(numchannels);
    tdc.pSubEvtId = getSubEvtId();
    tdc.fTdcId = trbaddr;
    tdcindex = fTDCs.size();
    fTDCs.push_back(tdc);
    fTDCsMap[trbaddr] = tdcindex;
    if (!isQuietMode())
	fprintf(stderr, "JAM:HTrb3TdcUnpacker::addTDC finds tdc address 0x%x for tdcindex %d after adding addreess 0x%x\n",
		fTDCs[tdcindex].getTrbAddr(), tdcindex, trbaddr);
    return tdcindex;
}

Bool_t HTrb3TdcUnpacker::scanTdcData(UInt_t trbaddr, UInt_t* data, UInt_t datalen)
{
    HTrb3TdcIterator iter;

    iter.assign((UInt_t*) data, datalen, kFALSE);

    HTrb3TdcMessage& msg = iter.msg();

    UInt_t cnt(0);
    //   Bool_t iserr(false);
    //   UInt_t first_epoch(0);
    //   bool isfirstepoch(false);

    while (iter.next()) {

	//      msg.print();

	//if ((cnt==0) && !msg.isHeaderMsg()) iserr = true;

	cnt++;

	/*      if ((cnt==2) && msg.isEpochMsg()) {
	 isfirstepoch = true;
	 first_epoch = msg.getEpochValue();
	 }
	 */


	// get existing TDC or add new one
	// simple caching to avoid repeating the same in each loop
	static Int_t tdc_index = -1;
	static UInt_t last_trbaddr = 0x0;

	if (last_trbaddr != trbaddr)
	    try
	{
	    last_trbaddr = trbaddr;
	    tdc_index = fTDCsMap.at((UInt_t) trbaddr);
	    if (!isQuietMode())
		fprintf(stderr, "HTrb3TdcUnpacker::scanTdcData finds tdcindex %d for trbnetaddress 0x%x .\n",tdc_index, trbaddr);


	}
	catch (const std::exception& e)
	{
	    if((fMinAddress!=0 || fMaxAddress!=0) && (fMinAddress<=trbaddr) && (fMaxAddress>=trbaddr))
	    {
		// here check that we have really an expected tdc and not a hub packet! In this case we add dynamically
		tdc_index =addTDC(trbaddr);
		if (!isQuietMode())
		    fprintf(stderr, "HTrb3TdcUnpacker::scanTdcData  has added new trbnetaddress 0x%x to index %d \n",trbaddr, tdc_index);
	    }
	    else
	    {
		if (!isQuietMode())
		    fprintf(stderr, "HTrb3TdcUnpacker::scanTdcData  unexpected trbnetaddress 0x%x, skip block! \n",trbaddr);
		return kFALSE;
	    }
	}
	TDC& tdc = fTDCs[tdc_index];
	if (!isQuietMode())
	    fprintf(stderr, "HTrb3TdcUnpacker::scanTdcData finds tdc address 0x%x with subevtid 0x%xfor tdcindex %d \n",
		    tdc.getTrbAddr(), tdc.pSubEvtId,  tdc_index);



	// JAM2018: adopted from hldprint code:
	if(msg.isCalibMsg())
	{
	    tdc.fCalibr[0] = msg.getCalibFirst();
	    tdc.fCalibr[1] = msg.getCalibSecond();
	    tdc.fNcalibr = 0;
	    if (getDebugFlag()>0)
		fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x calibr v1 0x%04x v2 0x%04x\n",
			getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr,
			tdc.fCalibr[0] , tdc.fCalibr[1] );
	    continue;
	}
	///////////



	if (msg.isHitMsg() || msg.isHit1Msg() ) {
	    UInt_t chid = msg.getHitChannel();
	    UInt_t fine = msg.getHitTmFine();

	    //UInt_t coarse = msg.getHitTmCoarse();

	    Bool_t isrising = msg.isHitRisingEdge();

	    Double_t localtm = iter.getMsgTimeCoarse();

	    if (!iter.isCurEpoch()) {
		// one expects epoch before each hit message, if not data are corrupted and we can ignore it
		if (!isQuietMode())
		    fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Missing epoch for hit from channel %u\n",
			    getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr, chid);
		// iserr = true;
		continue;
	    }

	    //         if (IsEveryEpoch())
	    //            iter.clearCurEpoch();


	    if (chid >= tdc.numChannels()) {
		if (!isQuietMode())
		    fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x TDC Channel number problem %u\n",
			    getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr, chid);
		//iserr = true;
		continue;
	    }

	    if (fine == 0x3FF) {
		// special case - missing hit, just ignore such value
		if (!isQuietMode())
		    fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Missing hit in channel %u fine counter is %x\n",
			    getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr, chid, fine);
		continue;
	    }

	    ChannelRec& rec = tdc.fCh[chid];

	    try
	    {
		HTrb3CalparTdc * tdcpar = fCalpars.at((UInt_t) trbaddr);
		if (tdcpar && (fine >= (UInt_t)tdcpar->getNBinsPerChannel())) {
		    if (!isQuietMode())
			fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Fine counter %u out of allowed range 0..%u in channel %u\n",
				getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr,
				fine, tdcpar->getNBinsPerChannel(), chid);
		    //iserr = true;
		    continue;
		}


		if (tdcpar) {
		    // JAM2018: if a local calibration parameter is given, ignore all calibration info from hld file
		    // deprecated! we do not use this mode for 2018 data
		    if (isrising) {
			localtm -= tdcpar->getRisingPar(chid, fine) / 1.e12;
		    } else {
			localtm -= tdcpar->getFallingPar(chid, fine) / 1.e12;
		    }
		}
		else
		{
		    //JAM 2018: without hydra calibration parameter, use calibration info from DAQ:
		    // this code was also taken from hldprint and adjusted
		    if (fine<0x3ff) {
			if (msg.isHit1Msg()) {
			    // hit format without original raw data (future)
			    if (isrising) {
				localtm -= fine*5e-12; // calibrated time, 5 ps/bin
			    } else {
				localtm -= (fine & 0x1FF)*10e-12; // for falling edge 10 ps binning is used
				if (fine & 0x200) localtm -= 0x800 * 5.0e-9; // in rare case time correction leads to epoch overflow
			    }
			} else
			    if (tdc.fNcalibr<2) {
				// JAM2018 TODO: encapsulate calibration function into TDC object

				// calibrated time, 5 ns correspond to value 0x3ffe or about 0.30521 ps/bin
				double corr = tdc.fCalibr[tdc.fNcalibr++]*5.0e-9/0x3ffe;
				if (!isrising) corr*=10.; // for falling edge correction 50 ns range is used
				localtm -= corr;
			    } 	else {
				//without calibration parameters use default simple fine time calibration:
				localtm -=iter.getMsgTimeFine();
			    }
		    }
		}

	    }
	    catch (const std::exception& e)
	    {
	    }

	    rec.addHit(isrising, localtm);

	    // if (!isQuietMode())
	    //    printf("TDC Channel %2d time %12.9f\n", chid, localtm);

	    continue;
	}

	// process other messages kinds

	switch (msg.getKind()) {
	case tdckind_Reserved:
	    break;
	case tdckind_Header: {
	    UInt_t errbits = msg.getHeaderErr();
	    if (errbits && !isQuietMode())
		fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x found error bits: 0x%x\n",
			getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr, errbits);
	    break;
	}
	case tdckind_Debug:
	    break;
	case tdckind_Epoch:
	    break;
	default:
	    if (!isQuietMode())
		fprintf(stderr, "Evt:%d Subev:0x%04x TDC:0x%04x Unknown bits 0x%x in header\n",
			getEventSeqNumber(), (UInt_t) getSubEvtId(), (UInt_t) trbaddr, msg.getKind());
	    break;
	}


    } // iter

    return kTRUE;



    //   if (isfirstepoch && !iserr)
    //      iter.setRefEpoch(first_epoch);
}

Bool_t HTrb3TdcUnpacker::correctRefTimeCh(UInt_t refch)
{
   for (UInt_t i = 0; i < fTDCs.size(); ++i)
   {
      Bool_t rc = fTDCs[i].correctRefTimeCh(refch);
      if (!rc) return kFALSE;
   }
   return kTRUE;
}

void HTrb3TdcUnpacker::addCalpar(UInt_t id, HTrb3CalparTdc* tdcpar)
{
   fCalpars.insert(std::pair<UInt_t, HTrb3CalparTdc*>(id,  tdcpar));
}

Bool_t HTrb3TdcUnpacker::reinit(void) {
   // Called in the beginning of each run, used here to initialize TDC unpacker
   // if TDCs processors are not yet created, use parameter to instantiate them
   Bool_t rc = kTRUE;
   if ((calpar!=0) && calpar->hasChanged()) {
      std::map<UInt_t, HTrb3CalparTdc*>::iterator it;
      for (it = fCalpars.begin(); it != fCalpars.end(); ++it)
         delete it->second;
      fCalpars.clear();

      if (nCalSrc == 1)
          return kTRUE;

      for(Int_t n=0;n<calpar->getSize();n++) {
         HTrb3CalparTdc* tdcpar = (*calpar)[n];
         if (tdcpar!=0) {
	    Int_t subevtid=tdcpar->getSubEvtId();
	    if (subevtid==(Int_t)subEvtId) {
	       if (tdcpar->getNChannels()>0 && tdcpar->getNBinsPerChannel()>0) {
	          addCalpar(calpar->getArrayOffset() + n, tdcpar);
                  Info("reinit","Instantiate TDC 0x%04x from HTrb3Calpar", calpar->getArrayOffset() + n);
               } else {
                  Error("reinit","No calibration found for TDC 0x%04x", calpar->getArrayOffset() + n);
                  continue;
               }
            }
         }
      }
   }
   return rc;
}

Bool_t HTrb3TdcUnpacker::decodeData(UInt_t trbaddr, UInt_t n, UInt_t * data)
{
    return scanTdcData(trbaddr, data, n);
}
