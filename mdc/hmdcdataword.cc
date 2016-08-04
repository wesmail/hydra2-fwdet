//*****************************************************************************
// File: $RCSfile: $
//
// Author: Joern Wuestenfeld
//
// Version: $Revision $
// Modified by $Author $
//
//*****************************************************************************
#include "hmdcdataword.h"
#include "hmessagemgr.h"
#include "hevent.h"
#include "heventheader.h"

ClassImp(HMdcDataword)

    HMdcDataword::HMdcDataword(Bool_t comment)
{
    // Default constructor.
    // Coment switches off possible debug printouts.
    noComment = comment;
    clearAll();
}

HMdcDataword::HMdcDataword(void)
{
    // Constructor.
    // Debugoutput is always switched on. Use setQuiteMode() to disable it.
    clearAll();
}

Bool_t HMdcDataword::checkConsistency(UInt_t version, UInt_t data)
{
		// checks for the different decoding version whether the decoded value are consistent.
		// I.e. that
		//           tdcNb in [ 0,   11]
		//        channels in [ 0,    7]
		//           time1 in [ 0,0x7ff] (only here hex value!)
		//            time in [ 0,0x7ff] (only here hex value!)
		//
		// return values:
		//  kTRUE all checks passed
		//  kFALSE else
		Int_t tdcNbMin = 0;
		Int_t tdcNbMax = 11;
		Int_t chanMin  = 0;
		Int_t chanMax  = 7;
		Int_t hitMin   = 0;
		Int_t hitMax   = 1;
		Int_t timeMin  = 0;
		Int_t timeMax  = 0x7ff;
		
		switch(version)
		{
				case 0:
						//           tdcNb in [ 0,   11]
						//        channels in [ 0,    7]
						//            time in [ 0,0x7ff]
						if (tdcNumber < tdcNbMin || tdcNumber > tdcNbMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%i] while decoding: 0x%08x", "TDC Number", tdcNumber, tdcNbMin, tdcNbMax, data  );
								return kFALSE;
						}
						if (channel < chanMin || channel > chanMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%i] while decoding: 0x%08x", "Channel Number", channel, chanMin, chanMax, data  );
								return kFALSE;
						}
						if (hit < hitMin || hit > hitMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%i] while decoding: 0x%08x", "Hit Number (modulo 2)", hit, hitMin, hitMax, data  );
								return kFALSE;
						}
						if (time < timeMin || time > timeMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%x] while decoding: 0x%08x", "Time", time, timeMin, timeMax, data  );
								return kFALSE;
						}
						break;
				case 2:
						break;
				case 4:
						//           tdcNb in [ 0,   11]
						//        channels in [ 0,    7]
						//           time1 in [ 0,0x7ff]
						//            time in [ 0,0x7ff]
						
						if (tdcNumber < tdcNbMin || tdcNumber > tdcNbMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%i] while decoding: 0x%08x", "TDC Number", tdcNumber, tdcNbMin, tdcNbMax, data  );
								return kFALSE;
						}
						if (channel < chanMin || channel > chanMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%i] while decoding: 0x%08x", "Channel Number", channel, chanMin, chanMax, data  );
								return kFALSE;
						}
						if (time < timeMin || time > timeMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%x] while decoding: 0x%08x", "Time", time, timeMin, timeMax, data  );
								return kFALSE;
						}
						if (time1 < timeMin || time1 > timeMax )
						{
								if(!noComment)Error("checkConsistency()","%s = %i out of range [%i,%x] while decoding: 0x%08x", "Time 1", time1, timeMin, timeMax, data  );
								return kFALSE;
						}
						break;
				default:
						if(!noComment)Error("checkConsistency()","decoding version %i not valid",version);
						return kFALSE;
		}
		return kTRUE;
}

void HMdcDataword::clearData(void)
{
    mboAddress=tdcNumber=channel=hit=time=-1;
    time1 = -1;
		statusCode = -1;
		statusData = 1;
}

Bool_t HMdcDataword::decode(const UInt_t data, Bool_t consistencyCheck)
{
		// Decoding of the data word. version sets the
		// different decode version. A consistency check of the data
		// word is performed optionally (consistencyCheck=kTRUE).
		// Bit 29 to 31 define decoding type:
		// (data>>29)&0x7 -> type: 
		//          0 = debug(uncomressed) dataformat
		//          2 = status datawords
		//          4 = compressed data format (t1 and t2 in one word )
		
		if(data != 0)
		{
				decodeType = (data>>29)&m3;
				switch(decodeType)
				{
						case 0:
								//*****************************************************
								// Debug dataformat
								// decoding of 32 bit data word from right->left
								//
								//  bits        sz  mask           comments
								// ----------------------------------------------------
								//  0 - 10      11  0x7ff          time value (hit 0)
								//  11           1  0x1            hit
								//  12 - 14      3  0x7            Channel# of TDC chip 0..7
								//  15 - 18      4  0xf            Tdc chip# 1..12
								//*****************************************************
								tdcNumber		= ((data & m32) >> 25 & m4)-1;
								channel			=  (data & m32) >> 22 & m3;
								hit					=  (data & m32) >> 21 & m1;
								time				=  (data & m32) & m11;
								break;
						case 2:
								//*****************************************************
								// Status word 
								// decoding of 32 bit data word from right->left
								//
								//  bits        sz  mask           comments
								// ----------------------------------------------------
								//  0 - 23       24 0xFFFFFF       data
								//  24 - 28      5  0x1F           code
								//*****************************************************
								statusCode = ((data & m32) >> 24 & m5);
								statusData = (data & m24);
								break;
						case 4:
								//*****************************************************
								// compressed data format
								// decoding of 32 bit data word from right->left
								//
								//  bits        sz  mask           comments
								// ----------------------------------------------------
								//  0 - 10      11  0x7ff          time value (hit 0)
								// 11 - 21      11  0x7ff          time value (hit 1- if normal mode, 0x0 otherwise)
								// 22 - 24       3  0x7            Channel of TDC Chip#0..7
								// 25 - 28       4  0xf            TDC Chip#1..12
								//(29 - 31)      3  0x7            data type, decoded in unpacker already)
								//******************************************************
								time1				=   (data & m32) & m11;
								time				=  ((data & m32) >> 11) & m11;
								channel			=  ((data & m32) >> 22) & m3;
								tdcNumber		= (((data & m32) >> 25) & m4)-1;
								break;
						default:
								if(!noComment) Error("decode","decoding version %i not a valid number",decodeType);
								return kFALSE;
				}
				if (consistencyCheck) return checkConsistency(decodeType, data);
				return kTRUE;
		}
		return kFALSE;
}

void HMdcDataword::dump(void)
{
		// dumps the event sequence number to the screen.
		
		Info(" dump()","----------------------------------------");
		if (gHades) Info(" dump()","--- EvtSeqNr: %i\n",
				((HEventHeader*)((HEvent*)(gHades->getCurrentEvent())->getHeader()))->getEventSeqNumber());
		print(decodeType);
		Info(" dump()","----------------------------------------");
}


UInt_t HMdcDataword::getCodedDataword(UInt_t version)
{
    // Recode the 32 bit data word. version sets the
    // different decode version.

    UInt_t code = 0;

    switch(version)
    {
    case 0:
	code |= (tdcNumber+1) << 25;
	code |= channel << 22;
	code |= hit << 21;
	code |= time;
	break;
    case 1:
	code |= time & m11;
	code |= (time1 & m11) << 11;
	code |= (channel & m3) << 21;
	code |= ((tdcNumber+1) & m4) << 24;
	code |= (decodeType &m3) << 28;
	break;
    default:
	if(!noComment) Error("recode","version %i not a valid number",version);
	return kFALSE;
    }
    return code;
}

Bool_t HMdcDataword::subHeader(const UInt_t *data, UInt_t version, Bool_t consistencyCheck)
{
    // decodes the subheader information of the mdc data word
    // version sets the different decode version. A consistency check of the data
    // word is performed optionally (consistencyCheck=kTRUE).

    switch(version)
    {
    case 0:
    case 1:
	if((*data & m16) > 0x1fff )
	{
	    module	= ((*data &0x37f) & 0xf00) >> 8;
	    sector	= ((*data &0x37f) & 0xf0) >> 4;
	    mboAddress = ((*data &0x37f) &0xf);
	    subEventSize = (*data & 0xffff0000) >> 16;
	    mbo = mboAddress;
	    return kTRUE;
	}
	break;
    default:
	return kFALSE;
	break;
    }
    return kFALSE;
}

void HMdcDataword::print(UInt_t version)
{
    //Print out the contents of the dataword.
    switch(version)
    {
    case 0:
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),"%s: %2i | %s: %2i | %s: %2i | %s: %2i | %s: %4i ","mboNb",mboAddress, "tdcNb",tdcNumber, "chan", channel,"hit", hit,"time", time);
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),">>> RAW-Address:");
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),"%s: %2i | %s: %2i | %s: %2i | %s: %2i","sector" ,sector, "module" ,module, "mbo"    ,mbo, "tdc"    ,tdc);
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName()," | %s: %2i | %s: %2i", "channel ",channel, "hit ", hit);
	break;
    case 1:
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),"%s: %2i | %s: %2i | %s: %2i | %s: %2i | %s: %4i | %s: %4i   ","mboNb",mboAddress, "tdcNb",tdcNumber, "chan", channel,"hit", hit,"time", time,"time 1",time1);
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),">>> RAW-Address:");
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName(),"%s: %2i | %s: %2i | %s: %2i | %s: %2i","sector" ,sector, "module" ,module, "mbo"    ,mbo, "tdc"    ,tdc);
	gHades->getMsg()->info(10,HMessageMgr::DET_MDC,ClassName()," | %s: %2i | %s: %2i", "channel ",channel, "hit ", hit);
	break;
    default:
	Error("print()","decoding version %i not valid",version);
    }
    gHades->getMsg()->messageSeperator("-",60);
}


Int_t HMdcDataword::getAddress(void)
{
    return (0x2000 + (module << 8) + (sector << 4) + mbo);
}

void HMdcDataword::setAddress(Int_t addr)
{
    module = (addr >> 8) & 0xf;
    sector = (addr >> 4) & 0xf;
    mboAddress = addr    & 0xf;
    mbo = mboAddress;
}
