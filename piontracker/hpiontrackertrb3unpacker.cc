//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//
//  HPionTrackerTrb3Unpacker
//
//  Class for unpacking TRB3 data and filling
//    the PionTracker Raw category
//
/////////////////////////////////////////////////////////////

#include "hpiontrackertrb3unpacker.h"
#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "heventheader.h"
#include "hldsource.h"
#include "hldsubevt.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hpiontrackerdetector.h"
#include "hpiontrackerraw.h"
#include "hpiontrackertrb3lookup.h"
#include "hpiontrackerdef.h"
#include <iostream>

using namespace std;

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define PRh(x) std::cout << "++DEBUG: " << #x << " = hex |" << std::hex << x << std::dec << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

ClassImp (HPionTrackerTrb3Unpacker)

HPionTrackerTrb3Unpacker::HPionTrackerTrb3Unpacker (UInt_t id)
{
	// constructor
	subEvtId       = id;
	uTrbNetAddress = 0;
	nEvt = 0;

	debugFlag      = 0;
	quietMode      = kFALSE;
	reportCritical = kFALSE;

	pRawCat = NULL;
	pLookup = NULL;
	pTrbBoard = NULL;
}

Bool_t HPionTrackerTrb3Unpacker::init (void)
{
	// creates the raw category and gets the pointer to the TRB3 lookup table

	HPionTrackerDetector * det = (HPionTrackerDetector *) gHades->getSetup()->getDetector ("PionTracker");

	if (!det)
	{
		Error ("init", "No PionTracker found.");
		return kFALSE;
	}

	pRawCat = det->buildCategory (catPionTrackerRaw);
	loc.set (2, 0, 0);
// 	loc.set (1, 0);

	pLookup = (HPionTrackerTrb3Lookup *) (gHades->getRuntimeDb()->getContainer ("PionTrackerTrb3Lookup"));

	if (!pLookup)
	{
		Error ("init", "No Pointer to parameter container PionTrackerTrb3Lookup.");
		return kFALSE;
	}

	return kTRUE;
}

Int_t HPionTrackerTrb3Unpacker::execute (void)
{
	// decodes the subevent of the PionTracker and fills the Raw category
	if (gHades->isCalibration())
	{
		//calibration event
		return 1;
	}

	if (gHades->getCurrentEvent()->getHeader()->getId() == 0xe)
	{
		//scaler event
		return 1;
	}

	nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber(); // Evt Seq. Nb.

	if (NULL == pSubEvt)
	{
#if DEBUG_LEVEL > 0
		Warning ("execute", "EvtId: 0x%x, Unpacker 0x%x ==> no data",
				 gHades->getCurrentEvent()->getHeader()->getId(), subEvtId);
#endif
		return 1;
	}

	// decode subevent data

// pSubEvt - make sure that there is something for decoding
	if (!pSubEvt)
		return 1;

	UInt_t * databeg = pSubEvt->getData();
	UInt_t * dataend  = pSubEvt->getEnd();

	nEvt = gHades->getCurrentEvent()->getHeader()->getEventSeqNumber();

	size_t data_size = dataend - databeg;

	if (data_size == 0)	return 0;

	size_t datanum = 0;
	size_t datacnt = 0;

	ptstate sm = sm_wait_for_board;

	ptheaderset pths;// = { des_none, -1, -1, -1, -1, -1 };
	ptdataset ptds;// = { -1, -1, -1, -1, -1 };

	for (size_t cnt = 0; cnt < data_size; ++cnt)
	{
		UInt_t d = * (databeg + cnt);

		switch (sm)
		{
			case sm_wait_for_board:
				datanum = (d >> 16) & 0xFFFF;   // No. of the data words from one TRB board
				uTrbNetAddress = d & 0xFFFF;     // TRB net Adddress (Old TRB subevent id)
// PRh(uTrbNetAddress);
				if (datanum == 0)
				{
					continue;
				}

				datacnt = 0;

				// which module
				if (uTrbNetAddress == 0x5555)
				{
					cnt += datanum;
					continue;
				}

				pTrbBoard = pLookup->getBoard (uTrbNetAddress);

				if (!pTrbBoard)
				{
					Warning ("HPionTrackerTrb3Unpacker::execute",
							 "Evt Nr : %i  TrbNetAdress: %x (%i) unpacked but TRB Board not in lookup table",
							 nEvt, uTrbNetAddress, uTrbNetAddress);
					// skip proper number of words and continue iwth next board
					cnt += datanum;
					continue;
				}

				// next step is sub-sub-event header
				sm = sm_header;

				break;

			// decode sub-sub-event header
			case sm_header:
				pths = decodeSubevent(d);
				++datacnt;
				sm = sm_data;

				break;

				// decode sub-sub-event data
			case sm_data:
			{
				++datacnt;

				if (d == 0xffffffff)
				{
					if (datacnt != datanum)
					{
						Error ("execute()", "Event %d --> datanum and datacnt differs. datanum=%lu, datacnt=%lu", nEvt, datanum, datacnt);
					}

					sm = sm_wait_for_board;

					break;
				}
				else
				{
					switch (pths.design)
					{
						case des_cosy13:
							ptds = decodeData_Cosy13(d, pths);
							break;
						case des_140115:
							ptds = decodeData_140115(d, pths);
							break;
						default:
							continue;
					}
					fillRaw (ptds);
				}
			}
			break;

			default:
				datacnt = 0;
				sm = sm_wait_for_board;
				break;
		}
	}

	// decode subsubevent data of this TRB
	// .....
	// call function fillRaw to store the data of a channel
	// .....

	return 1;
}


ptheaderset HPionTrackerTrb3Unpacker::decodeSubevent(UInt_t h)
{
	ptheaderset pths;

	bool olddesign = ((h >> 30) & 0x03) == 0x00;
	if (olddesign)
	{
		pths.design = des_cosy13;

		pths.tsref = h & 0xfff;
		pths.evcnt = (h >> 12) & 0x3ff;
		pths.romode = (h >> 22) & 0x3;
		pths.version = 0;
		pths.boardid = (h >> 24) & 0xff;
	}
	else
	{
		pths.design = des_140115;

		pths.tsref = h & 0xfff;
		pths.evcnt = (h >> 12) & 0x3ff;
		pths.romode = (h >> 22) & 0x3;
		pths.version = (h >> 26) & 0xf;
		pths.boardid = (h >> 30) & 0x3;
	}

	return pths;
}

ptdataset HPionTrackerTrb3Unpacker::decodeData_Cosy13(UInt_t d, const ptheaderset & pths)
{
	ptdataset ptds;
	ptds.ts = (d >> 0) & 0xfff;
	ptds.adc = (d >> 12) & 0xfff;
	ptds.strip = (d >> 24) & 0x7f;
	ptds.pileup = (d >> 31) & 0x01;
	ptds.overflow = 0;

	return ptds;
}

ptdataset HPionTrackerTrb3Unpacker::decodeData_140115(UInt_t d, const ptheaderset& hs)
{
	ptdataset ptds;//PR(hs.romode);
	switch (hs.romode)
	{
		case 0x00:
			ptds.ts = d & 0x7ff;
			ptds.adc = (d >> 11) & 0xfff;
			ptds.overflow = (d >> 23) & 0x01;
			ptds.pileup = (d >> 24) & 0x01;
			ptds.strip = (d >> 25) & 0x7f;
			break;
		case 0x01:
			ptds.ts = d & 0xfff;
			ptds.adc = (d >> 11) & 0xffe; // adc[11:1]
			ptds.overflow = (d >> 23) & 0x01;
			ptds.pileup = (d >> 24) & 0x01;
			ptds.strip = (d >> 25) & 0x7f;
			break;
		case 0x02:
			ptds.ts = d & 0x3fff;
			ptds.adc = (d >> 11) & 0xff8; // adc[11:1]
			ptds.overflow = (d >> 23) & 0x01;
			ptds.pileup = (d >> 24) & 0x01;
			ptds.strip = (d >> 25) & 0x7f;
			break;
		case 0x03:
			ptds.ts = d & 0x3fff;
			ptds.adc = (d >> 13) & 0xffe; // adc[11:1]
			ptds.overflow = -1;
			ptds.pileup = -1;
			ptds.strip = (d >> 25) & 0x7f;
			break;
		default:
			ptds.ts = -1;
			ptds.adc = -1;
			ptds.overflow = -1;
			ptds.pileup = -1;
			ptds.strip = 0;
			break;
	}

	return ptds;
}

Bool_t HPionTrackerTrb3Unpacker::fillRaw(const ptdataset& ptds)
{
	return fillRaw(ptds.strip, ptds.ts, ptds.adc, ptds.pileup, ptds.overflow);
}


Bool_t HPionTrackerTrb3Unpacker::fillRaw (Int_t channel, Float_t time, Float_t charge, Char_t pileup, Char_t overflow)
{
	// fill the raw category of the PionTracker for the channel "

	if (pTrbBoard)
	{
		HPionTrackerTrb3LookupChan * ch = pTrbBoard->getChannel (channel);
		ch->getAddress (loc[0], loc[1]);

		if (loc[0] >= 0)
		{
			HPionTrackerRaw * pRaw = (HPionTrackerRaw *) pRawCat->getObject (loc);

			if (!pRaw)
			{
				pRaw = (HPionTrackerRaw *) pRawCat->getSlot (loc);

// 							PRh(pRaw);
// 			if (pRaw) delete pRaw;
// PR(pRaw);
				if (pRaw)
				{
// 					printf("=== new pRaw\n");
					pRaw = new (pRaw) HPionTrackerRaw;
					pRaw->setAddress (loc[0], loc[1]);
				}
				else
				{
					Warning ("HPionTrackerTrb3Unpacker::execute()",
							 "Can't get slot mod=%i, chan=%i", loc[0], loc[1]);
					return kFALSE;
				}
				
			}
// 			printf("+ pRaw: %x at %x\n", pRaw, &pRaw);

			pRaw->setData (time, charge, pileup, overflow);
		}
		else
		{
			Warning ("HPionTrackerTrb3Unpacker::execute()",
					 "Not found in lookup table: TrbNetAddress=%i, channel=%i", uTrbNetAddress, channel);
		}
	}

	return kTRUE;
}

