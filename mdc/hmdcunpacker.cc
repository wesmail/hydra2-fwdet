//*****************************************************************************
// File: $RCSfile: $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//
//*****************************************************************************
//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
// HMdcUnpacker
//
// setDoBitFlipCorr(Bool_t do=kTRUE,TString version="auto") enables (default) or
// disables the bit flip corrections. By default the version is recognized by
// the filename (year,dayOfYear) ("auto"). The versions can be set manually
//  version :   "auto"   by filename (works on standard file names like
//               be1222210203001.hld (containing evtbuilder),
//               be12222102030_10.hld (grep files, no evtbuilder but _xx*.hld)
//               and an of the above with .hld.f_ (filtered files))
//              "apr12"  for beam data apr12  (does no analyze filenames)
//*****************************************************************************
#include "hmdcunpacker.h"
#include "hmdcunpackerpar.h"
#include "hmdcdataword.h"
#include "hevent.h"
#include "hldsubevt.h"
#include "heventheader.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hcategory.h"
#include "hmessagemgr.h"
#include "hmdcdetector.h"
#include "hmdcrawstruct.h"
#include "hmdclookupgeom.h"
#include "hmdcraw.h"
#include "hparset.h"
#include "hmdcdef.h"
#include "htrbnetunpacker.h"
#include "hmdcoepaddrcorrpar.h"
#include "hmdcoepstatusdata.h"
#include "hldsource.h"
#include "hspectrometer.h"
#include "htime.h"


#include "TRegexp.h"

ClassImp(HMdcUnpacker)

    HMdcUnpacker::HMdcUnpacker(Int_t id, Bool_t dbg, Bool_t consisCheck)
    : HldUnpack()
{
    // Default constructor.
    // id = sub event id for which the unpacker is called
    // dbg = debug flag
    // consisCheck = enable consistency checking of dataword
    setDefaults();
    debugMode = dbg;
    consistencyCheck = consisCheck;
    subEvtId = id;
}

HMdcUnpacker::HMdcUnpacker(HMdcUnpacker &unp)
: HldUnpack(unp)
{
    // Copy constructor
    debugMode = unp.debugMode;
    continueDecodingAfterInconsistency = unp.continueDecodingAfterInconsistency;
    consistencyCheck = unp.consistencyCheck;
    subEvtId = unp.subEvtId;
    noComment = unp.noComment;
    pUnpackerPar = unp.pUnpackerPar;
    rawc = unp.rawc;
    pRawCat = unp.pRawCat;
    pDetector = unp.pDetector;
    decodeVersion = unp.decodeVersion;
    doAddrCorrection = unp.doAddrCorrection;
    mdcDataWordCatPersistency = unp.mdcDataWordCatPersistency;
    pMdcOepStatusCat = unp.pMdcOepStatusCat;
    fillOepStatus = unp.fillOepStatus;
    mdcOepStatusCatPersistency = unp.mdcOepStatusCatPersistency;;
    doBitFlipCorr = unp.doBitFlipCorr;
    bitFlipVersion = unp.bitFlipVersion;
    trbNetUnpacker  = unp.trbNetUnpacker;
}

HMdcUnpacker::~HMdcUnpacker(void)
{
    // Destructor
    if(dataword)
    {
	delete dataword;
	dataword = NULL;
    }
}

Bool_t HMdcUnpacker::init(void)
{
    // Initialization routine.
    // Retrieves pointers to the following objects:
    //			- unpacker parameters
    //			- raw structure
    //			- address correction table
    //			- category for raw - level information
    //			- category for debug information (if requested)

    // get Pointer to runtime database
    HRuntimeDb *rtdb = gHades->getRuntimeDb();

    if (kTRUE == debugMode) {
	gHades->getMsg()->info(9,HMessageMgr::DET_MDC,GetName(),"responsible for : 0x%x",subEvtId);
    }
    if((pDetector = gHades->getSetup()->getDetector("Mdc")) == NULL)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not get pointer to Mdc detector!");
	return kFALSE;
    }

    // 1. Parameter container
    pUnpackerPar = (HMdcUnpackerPar *)rtdb->getContainer("MdcUnpackerPar");
    if(!pUnpackerPar)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not get MdcUnpackerPar container!");
	return kFALSE;
    }

    // 2. Raw Struct
    rawc = (HMdcRawStruct *)rtdb->getContainer("MdcRawStruct");
    if(!rawc)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not get MdcRawStruct container!");
	return kFALSE;
    }

    // need implicit initialization if not yet done, i.e. not set to static
    if(!rawc->isStatic())
    {
	((HParSet*)rawc)->init();
    }

    // 3. Lookup table for channel to wire/layer mapping
    lookupGeom = (HMdcLookupGeom *)rtdb->getContainer("MdcLookupGeom");
    if(!lookupGeom)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not get MdcLookupGeom container!");
	return kFALSE;
    }


    // 3. Address correction parameters
    addrCorrPar = (HMdcOepAddrCorrPar*)rtdb->getContainer("MdcOepAddrCorrPar");
    if(!addrCorrPar)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not get MdcOepAddrCorrPar container!");
	return kFALSE;
    }

    // 4. initialize trbNetUnpacker
    if (NULL == trbNetUnpacker) {
	if (gHades->getDataSource()) {
	    HDataSource* source = gHades->getDataSource();
	    if (source->InheritsFrom("HldSource")) {
		trbNetUnpacker = ((HldSource *)gHades->getDataSource())->getTrbNetUnpacker();
	    } else {
		Warning("init", "DataSource not inherited from HldSource! trbNetUnpacker == 0 ");
	    }
	} else {
	    Warning("init", "Could not retrieve DataSource! trbNetUnpacker == 0 ");
	}
    }
    if(!trbNetUnpacker)
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Failed to accquire pointer to HTrbNetUnpacker!");
	return kFALSE;
    }
    /*  if(!trbNetUnpacker->init())
     {
     ERROR_msg(HMessageMgr::DET_MDC, "Failed to initialize HTrbNetUnpacker!");
     return kFALSE;
     }*/
    // initialize categories
    // 1. catMdcRaw
    if(!(pRawCat = initCategory(catMdcRaw, "catMdcRaw")))
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Could not create MdcRaw category");
	return kFALSE;
    }

    // 2. catMdcDataWord
    if(fillDataWord)
    {
	pMdcDataWordCat = initCategory(catMdcDataWord,"catMdcDataWord");
	if(!pMdcDataWordCat)
	{
	    ERROR_msg(HMessageMgr::DET_MDC, "Could not create MdcDataWord category");
	    return kFALSE;
	}
    }
    // 3. catMdcOepStatus
    if(fillOepStatus)
    {
	pMdcOepStatusCat = initCategory(catMdcOepStatus,"catMdcOepStatus");
	if(!pMdcOepStatusCat)
	{
	    ERROR_msg(HMessageMgr::DET_MDC, "Could not create MdcOepStatusData category");
	    return kFALSE;
	}
    }
    // set persistencies
    if(fillDataWord)
    {
	pMdcDataWordCat->setPersistency(mdcDataWordCatPersistency);
    }
    if(fillOepStatus)
    {
	pMdcOepStatusCat->setPersistency(mdcOepStatusCatPersistency);
    }
    return kTRUE;
}

Bool_t HMdcUnpacker::reinit(void)
{
    // reinit function, called for every new file once.
    // Creates dataword depending on data version
    Int_t tmp;

    // Check if decode version changed (from file to file)
    // dataword may change then!
    tmp = pUnpackerPar->getDecodeVersion();
    if(decodeVersion == -1)
    {
	decodeVersion = tmp;
    }
    else
    {
	if(decodeVersion != tmp)
	{
	    delete dataword;
	    dataword = NULL;
	    decodeVersion = tmp;
	}
    }
    if((decodeVersion < 0) || (decodeVersion > MAX_OEP_DECODE_VERSIONS-1))
    {
	ERROR_msg(HMessageMgr::DET_MDC, "Invalid decode version!\nExiting!\n");
	exit(EXIT_FAILURE);
    }
    doAddrCorrection = pUnpackerPar->getDoAddrCorr();
    tdcMode = pUnpackerPar->getTdcMode();
    doMapOutUnusedChannels = pUnpackerPar->getMapUnusedChannels();
    // Initialize dataword only if not yet done
    // Required if Hades eventloop is called more than once.
    if(dataword == NULL)
    {
	switch(decodeVersion)
	{
	case 0:
	case 1:
	    dataword = new HMdcDataword();
	    break;
	default:
	    ERROR_msg(HMessageMgr::DET_MDC, "No valid decoding version defined!\nExiting!!!\n");
	    exit(EXIT_FAILURE);
	}
	dataword->setQuietMode(noComment);
    }

    return kTRUE;
}

Int_t HMdcUnpacker::execute(void)
{
    // This function is called for each event
    // It does the unpacking of the data from the hld file into the RAW category.
    //
    // return values: always 1 (philosophy: a wrong sub event should not affect the other sub events)

    UInt_t *data;
    UInt_t *end;
    Int_t diff;
    Int_t addr;

    if (pSubEvt)
    {
	/*
	 if(debugMode && !noComment)
	 {
	 gHades->getMsg()->info(1,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - trigger typ 0x%x",
	 gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,
	 gHades->getCurrentEvent()->getHeader()->getId());
	 }
	 */
	data = pSubEvt->getData();
	end  = pSubEvt->getEnd();
	diff = (end - data);

	if((gHades->getCurrentEvent()->getHeader()->getId() == 14) && (!noComment))
	{
	    gHades->getMsg()->info(5,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - trigger typ 0x%x",
				   gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,
				   gHades->getCurrentEvent()->getHeader()->getId());
	}
	if(diff < 0)
	{
	    if(!noComment)
	    {
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - negative eventsize (%i) ... skipping subevent!",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,diff);
		return -1;
	    } else {
		return -1;
	    }
	}
	// data loop
	while(data < end)
	{
	    dataword->clearData();
	    switch(*data & 0xf000)
	    {
		// Data from MDC HUB -> debug information goes via common global unpacker
	    case 0x1000:
		decodeTrbNet(data);
		// move datapointer to end of TRBNet data block
		data+=((*data&0xffff0000)>>16)+1;
		break;
		// Data from Sub Event Builder
	    case 0x5000:
		if((*data & 0xffff) == 0x5555)
		{
		    decodeTrbNet(data,subEvtId);
		    // move datapointer to end of TRBNet data block
		    data+=((*data&0xffff0000)>>16)+1;
		} else {
		    return 1;
		}
		break;
	    case 0x2000:
		if(!dataword->subHeader(data,decodeVersion,consistencyCheck))
		{
		    if(debugMode && !noComment)
		    {
			gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x could not decode subHeader, 0x%x ... skipping rest of subevent",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,*data);
		    }
		    return 1;
		}
		// Check address if listed as wrong assigned.
		if(doAddrCorrection)
		{
		    addr = dataword->getAddress();
		    if(addrCorrPar->checkAddress(&addr))
		    {
			/*							if(debugMode && !noComment)
			 {
			 gHades->getMsg()->info(1,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x corrected address 0x%x -> 0x%x",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,dataword->getAddress(),addr);
			 }*/
			dataword->setAddress(addr);
		    }
		}
		if(data+dataword->getSubEventSize() > end)
		{
		    if(!noComment)
		    {
			gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x could not identify new header!? Wrong number of datawords from 0x%8x",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId, *data);
			gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x .... skipping rest of this subevent!\n",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId);
		    }
		    return 1;
		}
		data++;
		for(Int_t i=0;i<dataword->getSubEventSize();i++)
		{
		    // loop over all datawords from one MBO/OEP
		    if(!dataword->decode(*data,consistencyCheck))
		    {
			if(!noComment && debugMode)
			{
			    dataword->dump();
			}
			if (continueDecodingAfterInconsistency)
			{
			    data++;
			    continue;
			}
			if(!noComment)
			{
			    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"                    ...... skipping unpacking of the rest of this subevent!!!");
			}
			return 1;
		    }
		    if(!checkConsistency(dataword->getSector(), dataword->getModule(), dataword->getMboAddress(), dataword->getTdcNumber(), dataword->getChannel()))
		    {
			if(!noComment)
			{
			    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Inconsistent data detected skipping dataword 0x%x from 0x%x",*data, dataword->getAddress());
			}
			data++;
			continue;
		    }
		    // fill data to HMdcRaw category including all checks
		    if((dataword->getDecodeType() & 0x2) == 0)
		    {
			switch(fillData())
			{
			case 0:
			    break;
			case 1:
			    // can not get slot for HMdcRaw
			    if(!noComment)
			    {
				gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Can not get slot for data!\n");
				gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x .... skipping rest of this subevent!\n",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId, *data);
			    }
			    badEventsCounter++;
			    return 1;
			case 2:
			    // can not get slot for Module, disabled?
			    if(!noComment)
			    {
				gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Module %d disabled in setup!", dataword->getModule());
			    }
			    badEventsCounter++;
			    break;
			case 3:
			    break;
			default:
			    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"unknown return value by fillData() ... exiting!");
			    exit(EXIT_FAILURE);
			}
		    }
		    else
		    {
			if(fillOepStatus)
			{
			    switch(fillStatus())
			    {
			    case 0:
				break;
			    case 1:
				badEventsCounter++;
				if(!noComment && debugMode)
				{
				    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Module %d disabled in setup!", dataword->getModule());
				}
				break;
			    case 2:
				badEventsCounter++;
				if(!noComment && debugMode)
				{
				    gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"Could not create new object for OepStatusData!");
				}
				break;
			    default:
				break;
			    }
			}
		    }
		    data++;
		}
		break;
	    default:
		data++;
		break;
	    }
	}
    }
    else
    {
	if(!noComment & debugMode)
	{
	    gHades->getMsg()->info(5,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - no data set",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId);
	}
    }


    //-----------------------------------------------------------------------
    if(doBitFlipCorr){
	Int_t hour,minute,second,month,day,year;


	if(bitFlipVersion.CompareTo("auto")  == 0 ) {
	    Int_t runid = gHades->getDataSource()->getCurrentRunId();
	    HTime::runIdToBrokenTime(runid,year,month,day,hour,minute,second);
	    year-=2000;
	}
	if(bitFlipVersion.CompareTo("apr12") == 0){
	    // 19.3.2012- 13.5.2012
	    year  = 12;
	    month = 4;
	    day   = 1 ;
	}

	if(pRawCat){
	    HMdcRaw* pRaw=0;
	    for(Int_t i = 0; i < pRawCat->getEntries(); i++){
		pRaw = (HMdcRaw*)pRawCat->getObject(i);
		if(pRaw){
		    correctBitFlip(pRaw,year,month,day);
		}
	    }
	}
    }
    //-----------------------------------------------------------------------



    return 1;
}


Bool_t HMdcUnpacker::finalize(void)
{
    // Called at the end of the event loop.
    // Should do nothing!
    if(!noComment && debugMode)
    {
	gHades->getMsg()->info(5,HMessageMgr::DET_MDC,GetName(),"0x%x: number of skipped events: %d\n",subEvtId,badEventsCounter);
    }
    return kTRUE;
}

void HMdcUnpacker::correctBitFlip (HMdcRaw* pRaw,Int_t year,Int_t month,Int_t day)
{

    if((year==12 && month >= 3 && month < 6) ||
       (year==12 && month >  5)              ||    // needs update later
       year > 12                                   // need update later
      )
    {
	// apr12 beamtime and following, as long as
	// there is no other setting
        Int_t sec,mod,mbo,chan;
        pRaw->getAddress(sec,mod,mbo,chan);

        //---------------- CAL DATA --------------------------------------
	if(pRaw->getNHits() == 6) {
	    Int_t time1 = pRaw->getTime(1); // + 2048;
	    Int_t time2 = pRaw->getTime(2); // + 2048;
	    Int_t time3 = pRaw->getTime(3);
	    Int_t time4 = pRaw->getTime(4);
	    Int_t time5 = pRaw->getTime(5);
	    Int_t time6 = pRaw->getTime(6);
	    if(sec == 0 && mod == 2 && mbo == 14) {
		if(time6 < 512)              time6 |= 512;
		if(time5 - time6 <  200)     time5 |= 512;
		if(time4 - time5 <  200)     time4 |= 512;
		Int_t at2 = 2048;
		Int_t at3 = time4-time3<1024 ? 0 : 2048;
		if(time3+at3 - time4     <  200) time3 |= 512;
		if(time2+at2 - time3-at3 <  200) time2 |= 512;
		if(time1     - time2-at2 <  200) time1 |= 512;
	    } else if(sec == 0 && mod == 2 && mbo == 13 && chan >= 80 && chan <= 87) {
		time5 |= 1024;
		time4 |= 1024;
		time3 |= 1024;
	    } else if(sec == 0 && mod == 3 && mbo == 13 && chan >= 32 && chan <= 39) {
		time5 |= 1024;
		time4 |= 1024;
		time3 |= 1024;
	    } else if(sec == 4 && mod == 3 && mbo == 4 && chan >= 88 && chan <= 95) {
		time5 |= 1024;
		time4 |= 1024;
		Int_t at3 = time4-time3<1024 ? 0 : 2048;
		if(time3+at3 < 2000) time3 |= 1024;
	    } else return;
            pRaw->setTimeNew(time1,1);
            pRaw->setTimeNew(time2,2);
            pRaw->setTimeNew(time3,3);
            pRaw->setTimeNew(time4,4);
            pRaw->setTimeNew(time5,5);
            pRaw->setTimeNew(time6,6);
	}
        //----------------------------------------------------------------

        //---------------- REAL DATA -------------------------------------
	if(pRaw->getNHits() == 2) {
	    Int_t time1 = pRaw->getTime(1);
	    Int_t time2 = pRaw->getTime(2);
	    pRaw->getAddress(sec,mod,mbo,chan);
	    if(sec == 0 && mod == 2 && mbo == 14) {
		if(time1 < time2) time1 |= 512;
		else if(time1-time2 > 512) time2 |= 512;
		else {
		    Int_t cut = chan<28||chan>31 ? 2350:2580;
		    if(time1+time2 < cut && time1<1535 && time2<1535) {  //fix3
			time1 += 512;
			time2 += 512;
		    }
		}
	    } else if(sec == 0 && mod == 2 && mbo == 13 && chan >= 80 && chan <= 87) {
		if(time1 < time2) {
		    if(time2 < 1024) time1 |= 1024;
		    else             time2 ^= 1024;
		} else if(time1-time2 > 1024) time2 |= 1024;
		else if(time1+time2 < 2000) {
		    time1 |= 1024;
		    time2 |= 1024;
		}
	    } else if(sec == 0 && mod == 3 && mbo == 13 && chan >= 32 && chan <= 39) {
		if(time1 < 580) {
		    time1 |= 1024;
		    time2 |= 1024;
		} else if(time1 < 1024) {
		    time1 += 512;
		    time2 += 512;
		} else if(time1 > 1700) {
		    time1 ^= 512;
		    time2 ^= 512;
		}
		if(time1 < time2) {
		    if(time1 < 1024) time1 |= 1024;
		    else             time2 ^= 1024;
		}
	    } else if(sec == 4 && mod == 3 && mbo == 4 && chan >= 88 && chan <= 95) {
		if(time1 < time2) {
		    if(time1 < 1024) time1 |= 1024;
		    if(time2 > 1536) time2 ^= 1024;
		} else if(time1 < 768) {
		    time1 |= 1024;
		    time2 |= 1024;
		} else if(time1 < 1024) {
		    time1 += 512;
		    time2 += 512;
		}
	    } else return;
            pRaw->setTimeNew(time1,1);
            pRaw->setTimeNew(time2,2);
	}
        //----------------------------------------------------------------

    }
}
HCategory *HMdcUnpacker::initCategory(Cat_t cat, const char *catname, const char *detector)
{
    // inits the container cat
    // looks first whether it already exists
    // otherwise it is created
    // default for detector is "Mdc"
    // catname is used for Error handling

    HCategory * category = 0;

    category = (HCategory*)(((HEvent*)(gHades->getCurrentEvent()))->getCategory(cat));

    if (!category)
    {
	category=(HCategory*)((HMdcDetector*)(((HSpectrometer*)(gHades->getSetup()))->getDetector(detector))->buildCategory(cat));

	if (!category)
	{
	    return NULL;
	}

	if (!((HEvent*)(gHades->getCurrentEvent()))->addCategory(cat,category,detector))
	{
	    return NULL;
	}
    }
    return category;
}

Int_t HMdcUnpacker::fillData(void)
{
    // Converts the content of the already decoded dataword via the lookup table HMdcEvReadout
    // to sector/module/mbo/tdc coordinates.
    // Then, if available the information is stored in a new or added to an existing
    // data slot of HMdcRaw
    // If activated, the raw data is written by fillMdcDataWord()

    // check if detected chamber is enabled in the setup
    if(!pDetector->getModule(dataword->getSector(), dataword->getModule()))
    {
	return 2;
    }

    // first check if wire is connected / exists
    // for real data and calibration data the check
    // is made in a different way
    if(doMapOutUnusedChannels)
    {
	Bool_t isCalibration =  gHades->getCurrentEvent()->getHeader()->getId() == 9 ? kTRUE : kFALSE;
	Int_t nMBo           = (*rawc)[dataword->getSector()][dataword->getModule()].getSize();

	if(dataword->getMboAddress() < 0 || dataword->getMboAddress() > nMBo - 1 )
	{
	    return 3;
	}

	Int_t maxTdc = (*rawc)[dataword->getSector()][dataword->getModule()][dataword->getMboAddress()].getNTdcs();
	Int_t tdcCh  = dataword->getTdcNumber()*8 + dataword->getChannel();

	if(!isCalibration){
	    if(
	       ((*lookupGeom)[dataword->getSector()][dataword->getModule()][dataword->getMboAddress()][tdcCh].getNLayer() < 0) &&
	       ((*lookupGeom)[dataword->getSector()][dataword->getModule()][dataword->getMboAddress()][tdcCh].getNCell()  < 0) )
	    {
		return 3;
	    }
	} else {
	    // for calibration check only if tdc channel
	    // corresponds to mbo max tdc

	    if(tdcCh < 0 || tdcCh > maxTdc - 1) {
		return 3;
	    }
	}
    }

    // second get a free or existing slot

    static HMdcRaw *pMdcRaw;
    pMdcRaw = NULL;
    pMdcRaw = getFreeOrExistingSlot(dataword->getSector(), dataword->getModule(), dataword->getMboAddress(),
				    dataword->getTdcNumber()*8 + dataword->getChannel());
    if (!pMdcRaw) return 1; // cannot get slot

    // fill the time to this slot

    if (!pMdcRaw->setTime(dataword->getTime(),tdcMode, noComment))
    {
	if(!noComment)
	{
	    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - error filling slot for dataword 0x%08x   at sector %d, module %d-->",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,dataword->getCodedDataword(decodeVersion),dataword->getSector(),dataword->getModule());
	    gHades->getMsg()->info(1,HMessageMgr::DET_MDC,GetName(),"time1 %d, time2 %d -> time %d",pMdcRaw->getTime(1),pMdcRaw->getTime(2),dataword->getTime());
	}
    }
    // fill dataWord to category HMdcDataWord
    if(fillDataWord)
    {
	fillMdcDataWord(dataword->getSector(), dataword->getModule(), dataword->getMboAddress(), dataword->getTdcNumber()*8 + dataword->getChannel());
    }
    if(dataword->getDecodeType()==4)
    {
	pMdcRaw = NULL;
	pMdcRaw = getFreeOrExistingSlot(dataword->getSector(), dataword->getModule(), dataword->getMboAddress(),
					dataword->getTdcNumber()*8 + dataword->getChannel());
	if (!pMdcRaw) return 1; // cannot get slot

	// fill the time to this slot

	if (!pMdcRaw->setTime(dataword->getTime1(),tdcMode, noComment))
	{
	    if(!noComment)
	    {
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - error filling slot for dataword 0x%08x at sector %d, module %d -->",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,dataword->getCodedDataword(decodeVersion),dataword->getSector(),dataword->getModule());
		gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"time1 %d, time2 %d -> time %d",pMdcRaw->getTime(1),pMdcRaw->getTime(2),dataword->getTime());
	    }
	}
    }
    return 0;
}


void HMdcUnpacker::setDefaults(void)
{
    // Define defaults for all internal variables.

    continueDecodingAfterInconsistency = kFALSE;
    consistencyCheck 			= kFALSE;
    dataword				= NULL;
    decodeVersion			= -1;
    doAddrCorrection 			= kFALSE;
    fillDataWord			= kFALSE;
    module				= 0;
    noComment				= kFALSE;
    pUnpackerPar			= NULL;
    rawc 				= NULL;
    sector				= 0;
    tdcMode				= 0;
    badEventsCounter 			= 0;
    pDetector				= NULL;
    pMdcOepStatusCat                    = NULL;
    fillOepStatus                       = kFALSE;
    mdcOepStatusCatPersistency          = kFALSE;
    doMapOutUnusedChannels              = kTRUE;
    doBitFlipCorr                       = kTRUE;
    bitFlipVersion                      = "auto";
    trbNetUnpacker                      = NULL;
}

void HMdcUnpacker::setQuietMode(Bool_t quiet, Bool_t warn)
{
    // switch to disable all error messages and warnings from hmdcunpacker AND hmdcdataword
    // quiet = kTRUE : messaging switch of
    //       = kFALSE: messages are displayed

    noComment = quiet;
    if (kTRUE == quiet && kTRUE == debugMode)
    {
	if(warn)gHades->getMsg()->info(10,HMessageMgr::DET_MDC,GetName(),"SubEvtId 0x%x - no further messages printed!",subEvtId);
    }
}

void HMdcUnpacker::setContinueDecodingAfterInconsistency(Bool_t cont)
{
    // this function is only for debugging use!
    // the decoding of the data words continue even if there is
    // unconsistency in the datawords
    // cont = kTRUE : continue
    // cont = kFALSE: skip event

    continueDecodingAfterInconsistency=cont;
}

HMdcRaw* HMdcUnpacker::getFreeOrExistingSlot(Int_t sector, Int_t module, Int_t mbo, Int_t tdc)
{
    // get a free or existing slot from the HMdcRaw category
    // at the location of HLocation loc
    // returns the pointer to the (new) HMdcRaw object
    // NULL if it couldn't be done

    HLocation loc;
    loc.set(4, 0, 0, 0, 0);

    // filling the data to the location
    // set location indexes

    loc[0] = sector;
    loc[1] = module;
    loc[2] = mbo;
    loc[3] = tdc;

    HMdcRaw *pMdcRaw = (HMdcRaw*) pRawCat->getObject(loc);
    if (!pMdcRaw)
    {
	pMdcRaw = (HMdcRaw*) pRawCat->getSlot(loc);
	if (pMdcRaw)
	{
	    pMdcRaw = new (pMdcRaw) HMdcRaw();
	    //setAddress to sector,module,mbo and tdc
	    pMdcRaw->setAddress(sector,module,mbo,tdc);
	}
	else
	{
	    if(!noComment)
	    {
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x - can't get slot for s: %i m: %i mb: %i tdc: %i",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(), subEvtId, sector,module,mbo,tdc);
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"                           ...... skipping unpacking of the rest of the subevent!!!");
	    }
	    if (debugMode)
	    {
		pSubEvt->dumpIt();
	    }
	}
    }
    return pMdcRaw;
}

Int_t HMdcUnpacker::fillMdcDataWord(Int_t sector,Int_t module,Int_t mbo,Int_t tdc)
{
    // function for filling the category HMdcDataword
    // Return 1 on success, -2 on failure.

    Int_t entry=0;
    UChar_t maxentry;

    maxentry=8;

    HLocation dataLoc;
    dataLoc.set(5,0,0,0,0,0);
    //set location indexes

    dataLoc[0] = sector;
    dataLoc[1] = module;
    dataLoc[2] = mbo;
    dataLoc[3] = tdc;
    dataLoc[4] = entry;

    if(!pDetector->getModule(sector,module))
    {
	return 1;
    }
    HMdcDataword *pMdcDataWord=NULL;

    for (entry = 0; entry < maxentry; entry++)
    {
	pMdcDataWord = NULL;

	dataLoc[4] = entry;
	pMdcDataWord = (HMdcDataword*) pMdcDataWordCat->getObject(dataLoc);
	if (!pMdcDataWord)
	{
	    break;
	}
    }

    if (pMdcDataWord)
    {
	if (entry==maxentry-1)
	{
	    if(!noComment)
	    {
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x too many entries to the same location (entries >%i)", gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,maxentry);
	    }
	}
	else
	{
	    if(!noComment)
	    {
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x can't get slot for MdcDataword",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId);
		gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"          ... but continue unpacking for the rest of this subevent");
	    }
	}
	return -2;
    }
    else
    {
	pMdcDataWord = (HMdcDataword*) pMdcDataWordCat->getSlot(dataLoc);
	if (pMdcDataWord)
	{
	    pMdcDataWord = new (((HMdcDataword*)pMdcDataWord)) HMdcDataword(*((HMdcDataword*)dataword));
	    ((HMdcDataword*)pMdcDataWord)->setAddress(sector,module,mbo,tdc,entry);
	}
    }
    if(!pMdcDataWord)
    {
	if(!noComment)
	{
	    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Event: %i - SubEvtId 0x%x can't get slot for MdcDataword at %d %d %d %d %d",gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(),subEvtId,sector,module,mbo,tdc,entry);
	    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"          ... but continue unpacking for the rest of this subevent");
	}
	return -2;
    }
    return 1;
}


Int_t HMdcUnpacker::fillStatus(void)
{
    // function for filling the category HMdcDataword
    // Return 1 on success, -2 on failure.
    HLocation dataLoc;
    Int_t addr=0;
    dataLoc.set(3,0,0,0);
    //set location indexes

    dataLoc[0] = dataword->getSector();
    dataLoc[1] = dataword->getModule();
    dataLoc[2] = dataword->getMboAddress();

    addr = (dataword->getModule() << 8) + (dataword->getSector() << 4) + dataword->getMboAddress() ;
    if(!pDetector->getModule(dataword->getSector(),dataword->getModule()))
    {
	return 1;
    }
    HMdcOepStatusData *pMdcOepStatusData = NULL;
    pMdcOepStatusData = (HMdcOepStatusData*) pMdcOepStatusCat->getObject(dataLoc);
    if (!pMdcOepStatusData)
    {
	pMdcOepStatusData = (HMdcOepStatusData*) pMdcOepStatusCat->getSlot(dataLoc);
	if(pMdcOepStatusData)
	{
	    pMdcOepStatusData = new (pMdcOepStatusData)HMdcOepStatusData();
	}
	else
	{
	    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"%d %d %d",dataLoc[0],dataLoc[1],dataLoc[2]);
	    return 2;
	}
    }
    if(!pMdcOepStatusData->setStatusWord(dataword->getStatusCode(),addr,dataword->getStatusData()))
    {
	if(debugMode && !noComment)
	{
	    gHades->getMsg()->error(10,HMessageMgr::DET_MDC,GetName(),"Invalid status word %d at index &d decoded!",dataword->getStatusData(),dataword->getStatusCode());
	}
    }
    return 0;
}

Bool_t HMdcUnpacker::checkConsistency(Int_t sector, Int_t module, Int_t mbo, Int_t tdc, Int_t channel)
{
    HMdcRawMothStru t = (*rawc)[sector][module][mbo];
    if((t.getNTdcs()) < tdc*8+channel)
	return kFALSE;
    return kTRUE;
}
