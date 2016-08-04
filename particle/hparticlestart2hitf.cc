//*-- Created : 03/08/2012 by S.Spataro
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HParticleStart2HitF
//
//  This class finds the correct start hit
//  Th correctiom method is store in HStart2Hit::gerCorrFlag() :
//      -1 : no corr, 0 : mod0, 1 : mod1. 2: both mods
//
///////////////////////////////////////////////////////////////////////////////

#include "hades.h"
#include "hcategory.h"
#include "hevent.h"
#include "hruntimedb.h"
#include "hspectrometer.h"

#include "hstart2cal.h"
#include "hstart2hit.h"
#include "htofhit.h"
#include "htofcluster.h"
#include "hrpchit.h"
#include "hrpccluster.h"
#include "htofhitsim.h"
#include "htofclustersim.h"
#include "hrpchitsim.h"
#include "hrpcclustersim.h"
#include "hparticlestart2hitf.h"
#include "tofdef.h"
#include "rpcdef.h"
#include "hstartdef.h"

#include "TMath.h"

#include <iomanip>
#include <iostream>

ClassImp(HParticleStart2HitF)

    HParticleStart2HitF::HParticleStart2HitF(void)
    : HReconstructor()
{
    // default constructor

    fCatTof      = NULL;
    fCatTofClu   = NULL;
    fCatRpc      = NULL;
    fCatRpcClu   = NULL;
    fCatStartCal = NULL;
    fCatStartHit = NULL;
    bDebug            = kFALSE;
    startTimeOriginal = 0.;
    startFlag         =-1;
}

HParticleStart2HitF::HParticleStart2HitF(const Text_t *name, const Text_t *title, Bool_t skip)
: HReconstructor(name, title)
{
    // constructor

    fCatTof      = NULL;
    fCatTofClu   = NULL;
    fCatRpc      = NULL;
    fCatRpcClu   = NULL;
    fCatStartCal = NULL;
    fCatStartHit = NULL;
    bDebug            = kFALSE;
    startTimeOriginal = 0.;
    startFlag         =-1;
}

Bool_t HParticleStart2HitF::init(void)
{
    // init function

    fCatTof = gHades->getCurrentEvent()->getCategory(catTofHit);
    if (NULL == fCatTof) {
	Warning("init", "HTofHit category not available!");
    }

    fCatTofClu = gHades->getCurrentEvent()->getCategory(catTofCluster);
    if (NULL == fCatTofClu) {
	Warning("init", "HTofCluster category not available!");
    }

    fCatRpc = gHades->getCurrentEvent()->getCategory(catRpcHit);
    if (NULL == fCatRpc) {
	Warning("init", "HRpcHit category not available!");
    }

    fCatRpcClu = gHades->getCurrentEvent()->getCategory(catRpcCluster);
    if (NULL == fCatRpcClu) {
	Warning("init", "HRpcCluster category not available!");
    }

    fCatStartCal = gHades->getCurrentEvent()->getCategory(catStart2Cal);
    if (NULL == fCatStartCal) {
	Warning("init", "HStart2Cal category not available!");
    }

    fCatStartHit = gHades->getCurrentEvent()->getCategory(catStart2Hit);
    if (NULL == fCatStartHit) {
	Warning("init", "HStart2Hit category not available!");
    }

    if (bDebug)
    {
	out = new TFile("./start_output.root","RECREATE");
	nt1 = new TNtuple("nt1","nt1","evt:sys:tof:dist:tofn");
	nt2 = new TNtuple("nt2","nt2","evt:size0:size1:tof1:tof0:tof:n1:n0:n");
	nt3 = new TNtuple("nt3","nt3","evt:tof:start0:start1:dist0:dist1:startold:startnew:flag");
    }

    return kTRUE;
}

Int_t HParticleStart2HitF::execute(void)
{
    // makes the hits and fills the HStartHit category


    //--------------------------------------------------------------------
    // find the minimum tof from tof+rpc (already corrected for start hit)
    Float_t minTime = findMinimumTime();
    if (minTime < 0) return 0; // if no good minimum tof value then skip the rest
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // find the start time from HStart2Hit
    startTimeOriginal = findOriginalStartTime();
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // find new start time, "minTime + startTimeOriginal" is the time
    // before the start correction from start hit
    firstCluster.clear();
    secondCluster.clear();
    fSecondTime = -10000;
    Float_t startTime = findStartTime(minTime + startTimeOriginal);
    //--------------------------------------------------------------------

    //if (TMath::Abs(startTime - startTimeOriginal) < 0.5) return kSkipEvent; // to comment out!!

    //--------------------------------------------------------------------
    correctTof(startTime - startTimeOriginal);
    correctRpc(startTime - startTimeOriginal);
    correctStart(startTime - startTimeOriginal);
    //--------------------------------------------------------------------

    return 0;
}

Float_t HParticleStart2HitF::findMinimumTime(void)
{
    // Function which loops inside tof and rpc hits, find the 3 hits
    // with minimum time-of-flight, and returns the average value -> fastes signal

    HTofHit* pTofHit = NULL;
    HRpcHit* pRpcHit = NULL;

    vector<Float_t> tofList, rpcList, allList; // lists of time-of-flight

    //--------------------------------------------------------------------
    // loop in HTofHit category
    if(fCatTof) {
	for (Int_t tofEntry = 0; tofEntry < fCatTof->getEntries(); ++tofEntry)
	{
	    if (NULL == (pTofHit = static_cast<HTofHit*>(fCatTof->getObject(tofEntry)))) {
		Error("findMinimumTime", "Pointer to HTofHit object == NULL, returning");
		return -1;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HTofHitSim* pTofHitSim = 0;
		pTofHitSim = dynamic_cast<HTofHitSim*>(pTofHit);
		if(pTofHitSim){
                    if(pTofHitSim->getNTrack1() > 0 || pTofHitSim->getNTrack2() > 0) continue;
		}
	    }
	    //--------------------------------------------------------------------


	    Float_t tofTof = pTofHit->getTof();
	    if ( (tofTof < 0) || (tofTof > 300) ) continue; // cuts too low or high tof values

	    Float_t distTof = 0;
	    pTofHit->getDistance(distTof);
	    if (distTof > 3000) continue; // cuts not feasible distances

	    tofList.push_back(2100. * tofTof / distTof);  // fill with normalized tof to 2100 mm
	    allList.push_back(2100. * tofTof / distTof);  // fill with normalized tof to 2100 mm
	    if (bDebug) nt1->Fill(gHades->getEventCounter(), 1, tofTof, distTof, 2100. * tofTof / distTof);
	} // end of HTofHit loop
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // loop in RpcHit category
    Int_t tracksL[10];
    Int_t tracksR[10];

    if(fCatRpc){
	for (Int_t rpcEntry = 0; rpcEntry < fCatRpc->getEntries(); ++rpcEntry)
	{
	    if (NULL == (pRpcHit = static_cast<HRpcHit*>(fCatRpc->getObject(rpcEntry)))) {
		Error("execute", "Pointer to HRpcHit object == NULL, returning");
		return -1;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HRpcHitSim* pRpcHitSim = 0;
		pRpcHitSim = dynamic_cast<HRpcHitSim*>(pRpcHit);
		if(pRpcHitSim){
		    pRpcHitSim->getTrackLArray(tracksL);
                    pRpcHitSim->getTrackRArray(tracksR);
                    Bool_t simTrack=kFALSE;
		    for(Int_t i = 0; i < 10; i++){
			if(tracksL[i] > 0 || tracksR[i] > 0) { simTrack = kTRUE; break;}
		    }
                    if(simTrack) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofRpc = pRpcHit->getTof();
	    if ( (tofRpc < 0) || (tofRpc > 300) ) continue; // cuts too low or high tof values

	    Float_t xRpc = 0., yRpc = 0., zRpc = 0.;
	    pRpcHit->getXYZLab(xRpc, yRpc, zRpc);
	    Float_t distRpc = TMath::Sqrt(xRpc * xRpc + yRpc * yRpc + zRpc * zRpc);
	    if (distRpc > 3000) continue; // cuts not feasible distances

	    rpcList.push_back(2100. * tofRpc / distRpc);  // fill with normalized tof to 2100 mm;
	    allList.push_back(2100. * tofRpc / distRpc);  // fill with normalized tof to 2100 mm;
	    if (bDebug) nt1->Fill(gHades->getEventCounter(), 0, tofRpc, distRpc, 2100. * tofRpc / distRpc);
	} // end of HRpcHit loop
    }
    //--------------------------------------------------------------------

    if (allList.size() == 0) return -1; // if no good time, returns a negative value

    //--------------------------------------------------------------------
    // sort tof lists from minimum to maximum value
    std::sort(tofList.begin(), tofList.end());
    std::sort(rpcList.begin(), rpcList.end());
    std::sort(allList.begin(), allList.end());
    //--------------------------------------------------------------------


    //--------------------------------------------------------------------
    // select the three tof values with minimum time,
    // and calculate the average, for tof rpc and both
    Float_t tofSum   = 0, rpcSum   = 0, allSum   = 0;
    Float_t tofMean  = 0, rpcMean  = 0, allMean  = 0;
    Int_t   tofCount = 0, rpcCount = 0, allCount = 0;
    for (vector<Float_t>::iterator it = tofList.begin(); it != tofList.end(); ++it) {
	if (tofCount < 3) {
	    tofSum += *it;
	    tofCount++;
	}
    }
    if (tofCount > 0) tofMean = tofSum / tofCount;

    for (vector<Float_t>::iterator it = rpcList.begin(); it != rpcList.end(); ++it) {
	if (rpcCount < 3) {
	    rpcSum += *it;
	    rpcCount++;
	}
    }
    if (rpcCount > 0) rpcMean = rpcSum/rpcCount;

    for (vector<Float_t>::iterator it = allList.begin(); it != allList.end(); ++it) {
	if (allCount < 3) {
	    allSum += *it;
	    allCount++;
	}
    }
    if (allCount > 0) allMean = allSum / allCount;
    //--------------------------------------------------------------------

    if (bDebug) nt2->Fill(gHades->getEventCounter(), rpcList.size(), tofList.size(), tofMean, rpcMean, allMean, tofCount, rpcCount, allCount);

    return allMean; // returns the minimum time from tof+rpc
}

Float_t HParticleStart2HitF::findOriginalStartTime(void)
{
    // function to retrieve the start time from HStart2Hit category
    
    Float_t start_time = 0;
    //--------------------------------------------------------------------
    if(fCatStartHit)
    {
	HStart2Hit* pStartHit = NULL;
	for (Int_t startEntry = 0; startEntry < fCatStartHit->getEntries(); ++startEntry)
	{
	    // loop on HStartHit
	    if (NULL == (pStartHit = static_cast<HStart2Hit*>(fCatStartHit->getObject(startEntry)))) {
		Error("findOriginalStartTime", "Pointer to HStartHit object == NULL, returning");
		return -1;
	    }
	    start_time = pStartHit->getTime();
	} // end of start hit loop
    }
    //--------------------------------------------------------------------
    return start_time;
}

Float_t HParticleStart2HitF::findStartTime(Float_t minTof)
{
    // Find the correct start time, where time-start closer to 7ns

    
    Float_t startTimeDiff[2] = { 300., 300.};
    Float_t startTime    [2] = { 300., 300. };

    Int_t strip[2]={-1,-1};
    //--------------------------------------------------------------------
    if(fCatStartCal)
    {
	HStart2Cal* pStartCal = NULL;
	for (Int_t startEntry = 0; startEntry < fCatStartCal->getEntries(); ++startEntry)
	{
	    // loop on HStartCal
	    if (NULL == (pStartCal = static_cast<HStart2Cal*>(fCatStartCal->getObject(startEntry)))) {
		Error("findStartTime", "Pointer to HStartCal object == NULL, returning");
		return -1;
	    }

	    Int_t mod = pStartCal->getModule();
	    if (mod > 1) continue; // skip VETO

	    for (Int_t nStart = 0; nStart < 4; nStart++) // loop on the 4 start time
	    {
		Float_t time = pStartCal->getTime(nStart + 1);
		//if ((time<-10) || (time>230) ) continue;  // use only [-10,230] ns as confidential range
		if ( ((minTof-time) < 5) || ((minTof-time) > 15) ) continue; // consider start hits where tof is [5,15] ns

		// select start time (modulewise) where tof-start is closer to 7 ns.
		if (TMath::Abs(minTof - time - 7.) < TMath::Abs(startTimeDiff[mod]))
		{
		    startTimeDiff[mod] = minTof - time - 7.;
		    startTime    [mod] = time;
                    strip        [mod] = startEntry*1000+nStart+1;
		}
	    } // end of loop inside 4 start time

	} // end of start hit loop
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    Float_t newStart = 0;
    startFlag = -1;
    if ( (startTime[0] != 300) && (startTime[1] != 300) ) // if there is good start time for both modules
    {
	// if they are within 0.5ns then calculates mean value
	if (TMath::Abs(startTime[0] - startTime[1]) < 0.5)
	{
	    newStart = 0.5 * (startTime[0] + startTime[1]);
	    startFlag = 2;
	} else {
	    // if the 2 start times are not correlated, take the closer to 7 ns
	    if (TMath::Abs(startTimeDiff[0]) < TMath::Abs(startTimeDiff[1]))
	    {
		newStart = startTime[0];
		startFlag = 0;
	    }
	    else
	    {
		newStart = startTime[1];
		startFlag = 1;
	    }
	}
    } // end of both modules
    else
    {
	// if only one start time is present, select such module for time
	if (startTime[0] != 300)
	{
	    newStart = startTime[0];
	    startFlag = 0;
	}
	if (startTime[1] != 300)
	{
	    newStart = startTime[1];
	    startFlag = 1;
	}
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    // collect cluster infos
    if(startFlag>-1){ // any good found

        Float_t minDiffSecond = 0.5;

        // put the closest strip allways first in vector
        if(startFlag == 0 || startFlag == 2) firstCluster.push_back(strip[0]);
        if(startFlag == 1 || startFlag == 2) firstCluster.push_back(strip[1]);

        vector<Int_t> vtemp;   // temp vector for strips will be refilled later
	Int_t    nextHit =-1;
        Float_t timediff = 100000;   // local var
        Float_t newStart2= -100000;  // clostest scond time
	HStart2Cal* pStartCal = NULL;
	for (Int_t startEntry = 0; startEntry < fCatStartCal->getEntries(); ++startEntry)
	{
	    // loop on HStartCal
	    if (NULL == (pStartCal = static_cast<HStart2Cal*>(fCatStartCal->getObject(startEntry)))) {
		Error("findStartTime", "Pointer to HStartCal object == NULL, returning");
		return -1;
	    }

	    Int_t mod = pStartCal->getModule();
	    if (mod > 1) continue; // skip VETO

	    for (Int_t nStart = 0; nStart < 4; nStart++) // loop on the 4 start time
	    {
		Float_t time = pStartCal->getTime(nStart + 1);
		//if ((time<-10) || (time>230) ) continue;  // use only [-10,230] ns as confidential range
		if ( ((minTof-time) < 5) || ((minTof-time) > 15) ) continue; // consider start hits where tof is [5,15] ns
                Int_t newstrip = startEntry*1000+nStart+1 ;
		// fill all strips of first cluster

                Float_t diff =  TMath::Abs( newStart - time );
		if (diff < 0.5)
		{
		    if(find(firstCluster.begin(),firstCluster.end(),newstrip)==firstCluster.end()){
			firstCluster.push_back(newstrip);
		    }

		} else {

		    if (diff > minDiffSecond)
		    {
			if(diff<timediff) {
			    timediff  = diff;
			    newStart2 = time;
			    nextHit   = newstrip;
			}
			vtemp.push_back(newstrip);
		    }
		}
	    } // end of loop inside 4 start time

	} // end of start hit loop

	if(vtemp.size()>0){  // there was something else
	    fSecondTime = newStart2;
	    secondCluster.push_back(nextHit);
	    for(UInt_t i=0;i<vtemp.size();i++){
		if(find(secondCluster.begin(),secondCluster.end(),vtemp[i])==secondCluster.end()){
		    secondCluster.push_back(vtemp[i]);
		}
	    }
	}

    }

    //--------------------------------------------------------------------





    // if no good time is present in no start modules, the start time remains "0"

    if (bDebug) nt3->Fill(gHades->getEventCounter(), minTof, startTime[0], startTime[1], startTimeDiff[0], startTimeDiff[1], startTimeOriginal, newStart, startFlag);

    return newStart;
}

void HParticleStart2HitF::correctTof(Float_t corrTime)
{
    // function which loops into HTofHit/HTofCluster categories and correct the tof
    // newtof = oldtof - corrTime

    //--------------------------------------------------------------------
    if(fCatTof){
	HTofHit* pTofHit = NULL;
	for (Int_t tofEntry = 0; tofEntry < fCatTof->getEntries(); ++tofEntry)
	{
	    // loop in HTofHit category
	    if (NULL == (pTofHit = static_cast<HTofHit*>(fCatTof->getObject(tofEntry)))) {
		Error("correctTof", "Pointer to HTofHit object == NULL, returning");
		return;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HTofHitSim* pTofHitSim = 0;
		pTofHitSim = dynamic_cast<HTofHitSim*>(pTofHit);
		if(pTofHitSim){
                    if(pTofHitSim->getNTrack1() > 0 || pTofHitSim->getNTrack2() > 0) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofTof = pTofHit->getTof();
	    pTofHit->setTof(tofTof - corrTime);
	} // end of HTofHit loop
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    if(fCatTofClu){
	HTofCluster* pTofCluster = NULL;
	for (Int_t tofEntry = 0; tofEntry < fCatTofClu->getEntries(); ++tofEntry)
	{
	    // loop in HTofCluster category
	    if (NULL == (pTofCluster = static_cast<HTofCluster*>(fCatTofClu->getObject(tofEntry)))) {
		Error("findMinimumTime", "Pointer to HTofCluster object == NULL, returning");
		return;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HTofClusterSim* pTofClusterSim = 0;
		pTofClusterSim = dynamic_cast<HTofClusterSim*>(pTofCluster);
		if(pTofClusterSim){
                    if(pTofClusterSim->getNTrack1() > 0 || pTofClusterSim->getNTrack2() > 0) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofTof = pTofCluster->getTof();
	    pTofCluster->setTof(tofTof - corrTime);
	} // end of HTofCluster loop
    }
    //--------------------------------------------------------------------

    return;
}

void HParticleStart2HitF::correctRpc(Float_t corrTime)
{
    // function which loops into HRpcHit/HRpcCluster categories and correct the tof
    // newtof = oldtof - corrTime

    //--------------------------------------------------------------------

    if(fCatRpc){
        Int_t tracksL[10];
        Int_t tracksR[10];

	HRpcHit* pRpcHit = NULL;
	for (Int_t tofEntry = 0; tofEntry < fCatRpc->getEntries(); ++tofEntry)
	{
	    // loop in HRpcHit category
	    if (NULL == (pRpcHit = static_cast<HRpcHit*>(fCatRpc->getObject(tofEntry)))) {
		Error("correctRpc", "Pointer to HRpcHit object == NULL, returning");
		return;
	    }
	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HRpcHitSim* pRpcHitSim = 0;
		pRpcHitSim = dynamic_cast<HRpcHitSim*>(pRpcHit);
		if(pRpcHitSim){
		    pRpcHitSim->getTrackLArray(tracksL);
                    pRpcHitSim->getTrackRArray(tracksR);
                    Bool_t simTrack=kFALSE;
		    for(Int_t i = 0; i < 10; i++){
			if(tracksL[i] > 0 || tracksR[i] > 0) { simTrack = kTRUE; break;}
		    }
                    if(simTrack) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofRpc  = pRpcHit->getTof();
	    Float_t newTof  = tofRpc - corrTime;
	    Float_t acharge = pRpcHit->getCharge();
	    Float_t axmod   = pRpcHit->getXMod();
	    Float_t aymod   = pRpcHit->getYMod();
	    Float_t azmod   = pRpcHit->getZMod();

	    pRpcHit->setHit(newTof, acharge, axmod, aymod, azmod);
	} // end of HRpcHit loop
    }
    //--------------------------------------------------------------------

    //--------------------------------------------------------------------
    if(fCatRpcClu){
        Int_t tracks[4];
	HRpcCluster* pRpcCluster = NULL;
	for (Int_t tofEntry = 0; tofEntry < fCatRpcClu->getEntries(); ++tofEntry)
	{
	    // loop in HRpcCluster category
	    if (NULL == (pRpcCluster = static_cast<HRpcCluster*>(fCatRpcClu->getObject(tofEntry)))) {
		Error("findMinimumTime", "Pointer to HRpcCluster object == NULL, returning");
		return;
	    }

	    //--------------------------------------------------------------------
            // skipp the embedded sim tracks
	    if(gHades->getEmbeddingMode()>0){
		HRpcClusterSim* pRpcClusterSim = 0;
		pRpcClusterSim = dynamic_cast<HRpcClusterSim*>(pRpcCluster);
		if(pRpcClusterSim){
		    pRpcClusterSim->getTrackList(tracks);
                    Bool_t simTrack=kFALSE;
		    for(Int_t i = 0; i < 4; i++){
			if(tracks[i] > 0) { simTrack = kTRUE; break;}
		    }
                    if(simTrack) continue;
		}
	    }
	    //--------------------------------------------------------------------

	    Float_t tofRpc  = pRpcCluster->getTof();
	    Float_t newTof  = tofRpc - corrTime;
	    Float_t acharge = pRpcCluster->getCharge();
	    Float_t axmod   = pRpcCluster->getXMod();
	    Float_t aymod   = pRpcCluster->getYMod();
	    Float_t azmod   = pRpcCluster->getZMod();

	    pRpcCluster->setCluster(newTof, acharge, axmod, aymod, azmod);
	} // end of HRpcCluster loop
    }
    //--------------------------------------------------------------------

    return;
} // end of correctRpc function

void HParticleStart2HitF::correctStart(Float_t corrTime)
{
    // function which loops into HStart2Hit category and correct the tof
    // newtof = oldtof - corrTime

    //--------------------------------------------------------------------
    if(fCatStartHit){
	HStart2Hit* pStartHit = NULL;
	for (Int_t startEntry = 0; startEntry < fCatStartHit->getEntries(); ++startEntry)
	{
	    // loop in HStart2Hit category
	    if (NULL == (pStartHit = static_cast<HStart2Hit*>(fCatStartHit->getObject(startEntry)))) {
		Error("correctStart", "Pointer to HStart2Hit object == NULL, returning");
		return;
	    }

	    Float_t timeStart = 0, widthStart = 0;
	    pStartHit->getTimeAndWidth(timeStart, widthStart);
	    pStartHit->setTimeAndWidth(timeStart + corrTime, widthStart);
	    pStartHit->setCorrFlag(startFlag);
            pStartHit->setIteration(2);


	    //--------------------------------------------------------------------
	    // fill cluster infos
	    if(startFlag!=-1){
		pStartHit->resetClusterStrip(2);
		for(UInt_t i=0;i<firstCluster.size();i++){
		    pStartHit->setClusterStrip (0, firstCluster[i]);
		}
		for(UInt_t i=0;i<secondCluster.size();i++){
		    pStartHit->setClusterStrip (1, secondCluster[i]);
		}
		pStartHit->setSecondTime(fSecondTime);

		if(secondCluster.size()>0) pStartHit->setMultiplicity(2);
                else                       pStartHit->setMultiplicity(1);
	    }
	    //--------------------------------------------------------------------









	} // end of HStart2Hit loop
    }
    //--------------------------------------------------------------------

    return;
}

Bool_t HParticleStart2HitF::finalize(void)
{
    if (bDebug){
	out->cd();
	nt1->Write();
	nt2->Write();
	nt3->Write();
	out->Save();

	nt1->Delete();
	nt2->Delete();
	nt3->Delete();
	nt1 = 0;
	nt2 = 0;
	nt3 = 0;

	out->Close();
    }

    return kTRUE;
}
