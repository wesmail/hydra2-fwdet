#include "honlinemonhist.h"
#include "honlinehistarray.h"
#include "honlinetrendhist.h"

#include "hcategory.h"
#include "hpiontrackerdef.h"
#include "hpiontrackerraw.h"
#include "hpiontrackercal.h"
#include "hpiontrackerhit.h"

#include "hstart2raw.h"

#include "helpers.h"

#include "TList.h"
#include "TString.h"

#include <map>
#include <iostream>
using namespace std;

map < TString , HOnlineMonHistAddon* > piontrackerMap;

typedef struct
{
	Int_t strip;
	Int_t time;
	Int_t charge;
} PtHitData;

Bool_t createHistPionTracker(TList& histpool){

	mapHolder::setMap(piontrackerMap); // make wallMap currentMap

	//####################### USER CODE ##############################################
	// define monitoring hists
	const Char_t* hists[] = {
		"FORMAT#array TYPE#1F NAME#hPTRawMult TITLE#Multiplicity ACTIVE#1 RESET#1 REFRESH#5000 BIN#10:0:10:0:0.0:0.0 SIZE#1:4 AXIS#mult:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTRawTimeStamp TITLE#TimeStamp ACTIVE#1 RESET#1 REFRESH#5000 BIN#250:0:1000:0:0.0:0.0 SIZE#1:4 AXIS#TimeStamp_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTRawStrip TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTRawPileup TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:pileups:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTRawOverflow TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:overflows:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTRawStripADC TITLE#StripADC ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:1024:0.0:4096.0 SIZE#1:4 AXIS#strip:ADC:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTRawStripTimeStamp TITLE#StripTimeStamp ACTIVE#1 RESET#1 REFRESH#5000 BIN#250:0:1000:128:0.0:128.0 SIZE#1:4 AXIS#TimeStamp_[ns]:strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTRawXY TITLE#X_Y_correlation ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0.:128.:128:0.:128.0 SIZE#1:2 AXIS#X_strip:Y_strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTRawHodoDt TITLE#PT_HODO_DT ACTIVE#1 RESET#1 REFRESH#5000 BIN#1000:-1000.:1000.:128:0.:128.0 SIZE#1:4 AXIS#dT_Pt-Hodo:PT_strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99",

		"FORMAT#array TYPE#1F NAME#hPTCalMult TITLE#Multiplicity ACTIVE#1 RESET#1 REFRESH#5000 BIN#10:0:10:0:0.0:0.0 SIZE#1:4 AXIS#mult:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTCalTimeStamp TITLE#TimeStamp ACTIVE#1 RESET#1 REFRESH#5000 BIN#200:-200:200:0:0.0:0.0 SIZE#1:4 AXIS#TimeStamp_[ns]:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTCalStrip TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:counts:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTCalPileup TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:pileups:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#1F NAME#hPTCalOverflow TITLE#Strip ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:0:0.0:0.0 SIZE#1:4 AXIS#strip:overflows:no DIR#no OPT#no STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTCalStripADC TITLE#StripADC ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0:128:1024:0.0:4096.0 SIZE#1:4 AXIS#strip:ADC:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTCalStripTimeStamp TITLE#StripTimeStamp ACTIVE#1 RESET#1 REFRESH#5000 BIN#200:-200:200:128:0.0:128.0 SIZE#1:4 AXIS#TimeStamp_[ns]:strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#46:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTCalXY TITLE#X_Y_correlation ACTIVE#1 RESET#1 REFRESH#5000 BIN#128:0.:128.:128:0.:128.0 SIZE#1:2 AXIS#X_strip:Y_strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99",
		"FORMAT#array TYPE#2F NAME#hPTCalHodoDt TITLE#PT_HODO_DT ACTIVE#1 RESET#1 REFRESH#5000 BIN#200:400.:800.:128:0.:128.0 SIZE#1:4 AXIS#dT_Pt-Hodo:PT_strip:no DIR#no OPT#colz STATS#0 LOG#0:0:0 GRID#1:1 LINE#1:0 FILL#0:0 MARKER#0:0:0 RANGE#-99:-99",

	};
	//###############################################################################


	// create hists and add them to the pool
	mapHolder::createHists(sizeof(hists)/sizeof(Char_t*),hists,histpool);

	Int_t colorsSector[6] = {2, 4, 6, 8, 38, 46};


	return kTRUE;
}

Bool_t fillHistPionTracker(Int_t evtCt){

	mapHolder::setMap(piontrackerMap); // make wallMap currentMap



	//####################### USER CODE ##############################################
	// Fill Histograms
	// categorys to loop
	HCategory* piontrackerRawCat = gHades->getCurrentEvent()->getCategory(catPionTrackerRaw);
	HCategory* piontrackerCalCat = gHades->getCurrentEvent()->getCategory(catPionTrackerCal);

	HCategory* startRawCat = gHades->getCurrentEvent()->getCategory(catStart2Raw);
	HCategory* startCalCat = gHades->getCurrentEvent()->getCategory(catStart2Cal);

	vector<PtHitData> vraw[4], vcal[4];
	Int_t nxpair[4] = { 1, 0, 3, 2 };

	Char_t pileup, overflow;

	if(piontrackerRawCat)
	{
		HPionTrackerRaw* raw;
		Int_t module,nStrip;

		size_t dsize[4] = { 0, 0, 0, 0 };

		for(Int_t i = 0; i < piontrackerRawCat->getEntries(); i ++)
		{
			raw = (HPionTrackerRaw*)piontrackerRawCat->getObject(i);
			if(raw){
				raw->getAddress(module,nStrip);
				//   det1         det2
				// x  0            2
				// y  1            3
				size_t s = raw->getMultiplicity();

				dsize[module] += s;

				for (size_t i = 0; i < s; ++i)
				{

					// most shorter notation
					PtHitData  hit;
					hit.strip =(Int_t)nStrip;
					hit.time  =(Int_t)raw->getTime(i);
					hit.charge=(Int_t)raw->getCharge(i);

					raw->getErrors(i, pileup, overflow);
					if (pileup) get("hPTRawPileup")->fill(0,i,hit.strip);
					if (overflow) get("hPTRawOverflow")->fill(0,i,hit.strip);


					vraw[module].push_back(hit);


					for(Int_t i = 0; i < startRawCat->getEntries(); i ++)
					{
						HStart2Raw * raw = (HStart2Raw*)startRawCat->getObject(i);


						if(raw&&raw->getModule()==4)
						{
							Int_t time  = raw->getTime(1);
							Int_t strip  = raw->getStrip();
							//printf("t = %d, s = %d, dt = %f\n", time, strip, hit.time-time);

							if(strip<16) get("hPTRawHodoDt")->fill(0,module,hit.time-time,hit.strip);
						}
					}
				}
			}
		} // end raw loop


		for (UInt_t i = 0; i < 4; ++i)
		{
			get("hPTRawMult")->fill(0,i,dsize[i]);

			for (UInt_t j = 0; j < vraw[i].size(); ++j)  //hits
			{

				get("hPTRawStrip")->fill(0,i,vraw[i][j].strip);
				get("hPTRawTimeStamp")->fill(0,i,vraw[i][j].time);
				get("hPTRawStripTimeStamp")->fill(0,i,vraw[i][j].time, vraw[i][j].strip);
				get("hPTRawStripADC")->fill(0,i,vraw[i][j].strip, vraw[i][j].charge);

				if (i % 2 != 1) {
					for (UInt_t k = 0; k < vraw[nxpair[i]].size(); ++k) // hits
					{
						get("hPTRawXY")->fill(0,i/2,vraw[i][j].strip,vraw[nxpair[i]][k].strip);
					}
				}
			}
		}


	}

	if(piontrackerCalCat)
	{
		HPionTrackerCal* cal;
		Int_t module,nStrip;

		size_t dsize[4] = { 0, 0, 0, 0 };

		for(Int_t i = 0; i < piontrackerCalCat->getEntries(); i ++)
		{
			cal = (HPionTrackerCal*)piontrackerCalCat->getObject(i);
			if(cal){
				cal->getAddress(module,nStrip);
				//   det1         det2
				// x  0            2
				// y  1            3
				size_t s = cal->getMultiplicity();

				dsize[module] += s;

				for (size_t i = 0; i < s; ++i)
				{

					// most shorter notation
					PtHitData  hit;
					hit.strip =(Int_t)nStrip;
					hit.time  =(Int_t)cal->getTime(i);
					hit.charge=(Int_t)cal->getCharge(i);

					cal->getErrors(i, pileup, overflow);
//					if (pileup) get("hPTCalPileup")->fill(0,i,hit.strip);
//					if (overflow) get("hPTCalOverflow")->fill(0,i,hit.strip);

					vcal[module].push_back(hit);
					//cal->getErrors(i, pileup, overflow);


					for(Int_t i = 0; i < startCalCat->getEntries(); i ++)
					{
						HStart2Raw * cal = (HStart2Raw*)startCalCat->getObject(i);


						if(cal&&cal->getModule()==4)
						{
							Int_t time  = cal->getTime(1);
							Int_t strip  = cal->getStrip();

							if(strip<16) get("hPTCalHodoDt")->fill(0,module,hit.time-time,hit.strip);
						}
					}
				}
			}
		} // end cal loop


		for (UInt_t i = 0; i < 4; ++i)
		{
			get("hPTCalMult")->fill(0,i,dsize[i]);

			for (UInt_t j = 0; j < vcal[i].size(); ++j)  //hits
			{

//				get("hPTCalStrip")->fill(0,i,vcal[i][j].strip);
				get("hPTCalTimeStamp")->fill(0,i,vcal[i][j].time);
				get("hPTCalStripTimeStamp")->fill(0,i,vcal[i][j].time, vcal[i][j].strip);
				get("hPTCalStripADC")->fill(0,i,vcal[i][j].strip, vcal[i][j].charge);

				if (i % 2 != 1) {
					for (UInt_t k = 0; k < vcal[nxpair[i]].size(); ++k) // hits
					{
//						get("hPTCalXY")->fill(0,i/2,vcal[i][j].strip,vcal[nxpair[i]][k].strip);
					}
				}
			}
		}


	}

	/*
	   if(piontrackerHitCat){
	   HPionTrackerHit* hit;

	   for(Int_t i = 0; i < piontrackerHitCat->getEntries(); i ++){
	   hit = (HPionTrackerHit*)piontrackerHitCat->getObject(i);
	   if(hit){


	   }
	   }
	   }

	 */
	//###############################################################################

	// do reset if needed
	mapHolder::resetHists(evtCt);

	return kTRUE;
}



