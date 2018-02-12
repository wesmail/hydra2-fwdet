//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : S. Lebedev
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRich700RingFinderHough
//
//
//////////////////////////////////////////////////////////////////////////////

#include "hrich700ringfinderhough.h"
#include "hades.h"
#include "hruntimedb.h"
#include "hspectrometer.h"
#include "hcategory.h"
#include "hevent.h"
#include "hlinearcatiter.h"
#include "hmatrixcatiter.h"
#include "hruntimedb.h"
#include "hparset.h"
#include "hrichdetector.h"
#include "hgeantdef.h"
#include "richdef.h"
#include "hrichcal.h"
#include "hrichcalsim.h"
#include "hrichhit.h"
#include "hrichhitsim.h"
#include "hrich700digipar.h"
#include "hrich700ringfinderpar.h"
#include "hrich700ringfittercop.h"


#include "TSystem.h"
#include "TMath.h"

#include <iostream>
#include <cmath>
#include <set>
#include <algorithm>
#include <iostream>

using std::cout;
using std::endl;
using std::vector;
using std::set;
using std::sort;

ClassImp(HRich700RingFinderHough)

    HRich700RingFinderHough::HRich700RingFinderHough(const Text_t* name,const Text_t* title ):
HReconstructor(name,title),
fNofParts(0),

    fMaxDistance(0.f),
    fMinDistance(0.f),
    fMinDistanceSq(0.f),
    fMaxDistanceSq(0.f),

    fMinRadius(0.f),
    fMaxRadius(0.f),

    fDx(0.f),
    fDy(0.f),
    fDr(0.f),
    fNofBinsX(0),
    fNofBinsY(0),
    fNofBinsXY(0),

    fHTCut(0),

    fNofBinsR(0),
    fHTCutR(0),

    fMinNofHitsInArea(0),

    fUsedHitsAllCut(0.0),

    fRmsCoeffCOP(0.f),
    fMaxCutCOP(0.f),

    fCurMinX(0.f),
    fCurMinY(0.f),


    fData(),
    fHist(),
    fHistR(),
    fHitInd()

{
}

HRich700RingFinderHough::~HRich700RingFinderHough()
{

}

Bool_t HRich700RingFinderHough::init()
{

    HRichDetector* pRichDet = static_cast<HRichDetector*>(gHades->getSetup()->getDetector("Rich"));
    if (pRichDet == NULL) {
	Warning("init", "No RICH detector in setup!");
    }

    fIsSimulation = kFALSE;
    if(gHades->getCurrentEvent()->getCategory(catGeantKine)) fIsSimulation = kTRUE;



    fCatRichCal = gHades->getCurrentEvent()->getCategory(catRichCal);
    if (NULL == fCatRichCal) {
	Warning("init", "No category catRichCal found!");
    }

    fCatRichHit = gHades->getCurrentEvent()->getCategory(catRichHit);
    if (NULL == fCatRichHit) {
	if(pRichDet) {

	    if(fIsSimulation) { fCatRichHit = pRichDet->buildLinearCat("HRichHitSim", 10); }
	    else              { fCatRichHit = pRichDet->buildLinearCat("HRichHit"   , 10); }

	    if (NULL == fCatRichHit) {
		Error("init", "Cannot create category catRichHit!");
	    } else {
		gHades->getCurrentEvent()->addCategory(catRichHit, fCatRichHit, "Rich");
	    }
	}
    } else {
	Error("init", "No category catRichHit found");
    }

    fDigiPar = (HRich700DigiPar*) gHades->getRuntimeDb()->getContainer("Rich700DigiPar");
    if(!fDigiPar) {
	Error("init", "Can not retrieve HRich700DigiPar");
        return kFALSE;
    }

    fRingPar = (HRich700RingFinderPar*) gHades->getRuntimeDb()->getContainer("Rich700RingFinderPar");
    if(!fRingPar) {
	Error("init", "Can not retrieve HRich700RingFinderPar");
        return kFALSE;
    }

    return kTRUE;

}


Bool_t HRich700RingFinderHough::reinit()
{
    setParameters();
    fHist  .resize(fNofBinsXY);
    fHistR .resize(fNofBinsR);
    fHitInd.resize(fNofParts);
    return kTRUE;
}


Int_t HRich700RingFinderHough::execute()
{
    fEventNum++;
    processEvent();
    return 0;
}


void HRich700RingFinderHough::processEvent()
{

    if(!fCatRichCal || !fCatRichHit) return; // nothing to do
    fData.clear();                           // clean up

    Int_t nofRichCals = fCatRichCal->getEntries();
    for (Int_t iC = 0; iC < nofRichCals; iC++) {
	HRichCalSim* richCal = static_cast<HRichCalSim*>(fCatRichCal->getObject(iC));
	if (NULL == richCal) continue;

	Int_t loc[3];
	loc[0] = richCal->getSector();
	loc[1] = richCal->getCol();
	loc[2] = richCal->getRow();
	pair<Double_t, Double_t> xy = fDigiPar->getXY(loc);

	HRich700HoughHit houghHit;
	houghHit.fX = xy.first;
	houghHit.fY = xy.second;
	houghHit.fX2plusY2 = houghHit.fX * houghHit.fX + houghHit.fY * houghHit.fY;
	houghHit.fId = iC;
	houghHit.fIsUsed = kFALSE;
	fData.push_back(houghHit);
    }

    if (fData.size() > MAX_NOF_HITS) {
	Warning("processEvent()","Number of hits is more than %i. Skipped!",MAX_NOF_HITS);
    } else {
	std::sort(fData.begin(), fData.end(), HRich700HoughHitCmpUp());
	HoughTransformReconstruction();
    }
}

void HRich700RingFinderHough::setParameters()
{
    fMaxDistance      = fRingPar->getMaxDistance();
    fMinDistance      = fRingPar->getMinDistance();
    fMinDistanceSq    = fMinDistance*fMinDistance;
    fMaxDistanceSq    = fMaxDistance*fMaxDistance;

    fMinRadius        = fRingPar->getMinRadius();
    fMaxRadius        = fRingPar->getMaxRadius();

    fHTCut            = (UShort_t)fRingPar->getHTCut();
    fHTCutR           = (UShort_t)fRingPar->getHTCutR();
    fMinNofHitsInArea = (UShort_t)fRingPar->getMinNofHitsInArea();

    fNofBinsX         = (UShort_t)fRingPar->getNofBinsX();
    fNofBinsY         = (UShort_t)fRingPar->getNofBinsY();
    fNofBinsR         = (UShort_t)fRingPar->getNofBinsR();

    fRmsCoeffCOP      = fRingPar->getRmsCoeffCOP();
    fMaxCutCOP        = fRingPar->getMaxCutCOP();

    fUsedHitsAllCut   = fRingPar->getUsedHitsAllCut();

    fNofParts         = (UShort_t)fRingPar->getNofParts();
    fDx               = 2.f * fMaxDistance / (Float_t)fNofBinsX;
    fDy               = 2.f * fMaxDistance / (Float_t)fNofBinsY;
    fDr               = fMaxRadius / (Float_t)fNofBinsR;
    fNofBinsXY        = fNofBinsX * fNofBinsY;
}

void HRich700RingFinderHough::HoughTransformReconstruction()
{
    fFoundRings.clear();
    Int_t indmin, indmax;
    UInt_t size = fData.size();
    for (UInt_t iHit = 0; iHit < size; iHit++){
	if (fData[iHit].fIsUsed == kTRUE) continue;

	fCurMinX = fData[iHit].fX - fMaxDistance;
	fCurMinY = fData[iHit].fY - fMaxDistance;

	DefineLocalAreaAndHits(fData[iHit].fX, fData[iHit].fY , &indmin, &indmax);
	HoughTransform(indmin, indmax);
	FindPeak(indmin, indmax);
    }

}

void HRich700RingFinderHough::DefineLocalAreaAndHits(Float_t x0,
						     Float_t y0,
						     Int_t *indmin,
						     Int_t *indmax)
{
    // Find hits in a local area.
    // x0 X coordinate of the local area center.
    // y0 Y coordinate of the local area center.
    // indmin Minimum index of the hit in local area.
    // indmax Maximum index of the hit in local area.

    HRich700HoughHit mpnt;
    vector<HRich700HoughHit>::iterator itmin, itmax;

    //find all hits which are in the corridor
    mpnt.fX = x0 - fMaxDistance;
    itmin = std::lower_bound(fData.begin(), fData.end(), mpnt, HRich700HoughHitCmpUp());

    mpnt.fX = x0 + fMaxDistance;
    itmax = std::lower_bound(fData.begin(), fData.end(), mpnt, HRich700HoughHitCmpUp()) - 1;

    *indmin = itmin - fData.begin();
    *indmax = itmax - fData.begin();

    Int_t arSize = *indmax - *indmin + 1;
    if (arSize <= fMinNofHitsInArea) return;

    for (UShort_t i = 0; i < fNofParts; i++){
	fHitInd[i].clear();
	fHitInd[i].reserve( (*indmax-*indmin) / fNofParts);
    }

    UShort_t indmin1 = (UShort_t)(*indmin);
    UShort_t indmax1 = (UShort_t)(*indmax);

    for (UShort_t i = indmin1; i <= indmax1; i++) {
	if (fData[i].fIsUsed == kTRUE) continue;
	Float_t ry = y0 - fData[i].fY;
	if (fabs(ry) > fMaxDistance) continue;
	Float_t rx = x0 - fData[i].fX;
	Float_t d = rx * rx + ry * ry;
	if (d > fMaxDistanceSq) continue;
	fHitInd[i % fNofParts].push_back(i);
    }

    for (UShort_t j = 0; j < fNofBinsXY; j++){
	fHist[j] = 0;
    }

    for (UShort_t k = 0; k < fNofBinsR; k++) {
	fHistR[k] = 0;
    }
}

void HRich700RingFinderHough::HoughTransform(UShort_t indmin,UShort_t indmax)
{
   // Run HoughTransformGroup for each group of hits.
   // indmin Minimum index of the hit in local area.
   // indmax Maximum index of the hit in local area.

    for (Int_t iPart = 0; iPart < fNofParts; iPart++){
	HoughTransformGroup(indmin, indmax, iPart);
    }//iPart
}

void HRich700RingFinderHough::HoughTransformGroup(UShort_t indmin,
						  UShort_t indmax,
						  Int_t iPart)
{
    //  Main procedure for Hough Transform.
    // indmin Minimum index of the hit in local area.
    // indmax Maximum index of the hit in local area.
    // iPart Index of the hit group.


    UShort_t nofHits = fHitInd[iPart].size();
    Float_t xcs, ycs; // xcs = xc - fCurMinX
    Float_t dx = 1.0f/fDx, dy = 1.0f/fDy, dr = 1.0f/fDr;

    vector<HRich700HoughHit> data;
    data.resize(nofHits);
    for (Int_t i = 0; i < nofHits; i++){
	data[i] = fData[ fHitInd[iPart][i] ];
    }

    typedef vector<HRich700HoughHit>::iterator iH;

    for (iH iHit1 = data.begin(); iHit1 != data.end(); iHit1++) {
	Float_t iH1X = iHit1->fX;
	Float_t iH1Y = iHit1->fY;

	for (iH iHit2 = iHit1 + 1; iHit2 != data.end(); iHit2++) {
	    Float_t iH2X = iHit2->fX;
	    Float_t iH2Y = iHit2->fY;

	    Float_t rx0 = iH1X - iH2X;//rx12
	    Float_t ry0 = iH1Y- iH2Y;//ry12
	    Float_t r12 = rx0 * rx0 + ry0 * ry0;
	    if (r12 < fMinDistanceSq || r12 > fMaxDistanceSq)	continue;

	    Float_t t10 = iHit1->fX2plusY2 - iHit2->fX2plusY2;
	    for (iH iHit3 = iHit2 + 1; iHit3 != data.end(); iHit3++) {
		Float_t iH3X = iHit3->fX;
		Float_t iH3Y = iHit3->fY;

		Float_t rx1 = iH1X - iH3X;//rx13
		Float_t ry1 = iH1Y - iH3Y;//ry13
		Float_t r13 = rx1 * rx1 + ry1 * ry1;
		if (r13 < fMinDistanceSq || r13 > fMaxDistanceSq)continue;

		Float_t rx2 = iH2X - iH3X;//rx23
		Float_t ry2 = iH2Y - iH3Y;//ry23
		Float_t r23 = rx2 * rx2 + ry2 * ry2;
		if (r23	< fMinDistanceSq || r23 > fMaxDistanceSq)continue;

		Float_t det = rx2*ry0 - rx0*ry2;
		if (det == 0.0f) continue;
		Float_t t19 = 0.5f / det;
		Float_t t5 = iHit2->fX2plusY2 - iHit3->fX2plusY2;

		Float_t xc = (t5 * ry0 - t10 * ry2) * t19;
		xcs = xc - fCurMinX;
		Int_t intX = Int_t( xcs *dx);
		if (intX < 0 || intX >= fNofBinsX ) continue;

		Float_t yc = (t10 * rx2 - t5 * rx0) * t19;
		ycs = yc - fCurMinY;
		Int_t intY = Int_t( ycs *dy);
		if (intY < 0 || intY >= fNofBinsY ) continue;

		//radius calculation
		Float_t t6 = iH1X - xc;
		Float_t t7 = iH1Y - yc;
		//if (t6 > fMaxRadius || t7 > fMaxRadius) continue;
		Float_t r = sqrt(t6 * t6 + t7 * t7);
		//if (r < fMinRadius) continue;
		Int_t intR = Int_t(r *dr);
		if (intR < 0 || intR >= fNofBinsR) continue;

		fHist[intX * fNofBinsX + intY]++;
		fHistR[intR]++;
	    }// iHit1
	}// iHit2
    }// iHit3
    //hitIndPart.clear();
}

void HRich700RingFinderHough::FindPeak(Int_t indmin, Int_t indmax)
{
    // Find peak in the HT histograms.
    // indmin Minimum index of the hit in local area.
    // indmax Maximum index of the hit in local area.

    // Find MAX bin R and compare it with CUT
    Int_t maxBinR = -1, maxR = -1;
    UInt_t size = fHistR.size();
    for (UInt_t k = 0; k < size; k++){
	if (fHistR[k] > maxBinR){
	    maxBinR = fHistR[k];
	    maxR = k;
	}
    }
    if (maxBinR < fHTCutR) return;

    // Find MAX bin XY and compare it with CUT
    Int_t maxBinXY = -1, maxXY = -1;
    size = fHist.size();
    for (UInt_t k = 0; k < size; k++){
	if (fHist[k] > maxBinXY){
	    maxBinXY = fHist[k];
	    maxXY = k;
	}
    }
    if (maxBinXY < fHTCut) return;

    HRich700Ring ring1;

    // Find Preliminary Xc, Yc, R
    Float_t xc, yc, r;
    Float_t rx, ry, dr;
    xc = (maxXY / fNofBinsX + 0.5f)* fDx + fCurMinX;
    yc = (maxXY % fNofBinsX + 0.5f)* fDy + fCurMinY;
    r = (maxR + 0.5f) * fDr;
    for (Int_t j = indmin; j < indmax + 1; j++) {
	rx = fData[j].fX - xc;
	ry = fData[j].fY - yc;

	dr = fabs(sqrt(rx * rx + ry * ry) - r);
	if (dr > 10.) continue;
	HRich700Hit hit1;
	hit1.fX = fData[j].fX;
	hit1.fY = fData[j].fY;
	ring1.fHits.push_back(hit1);
    }

    if (ring1.fHits.size() < 5) {
	return;
    }

    HRich700RingFitterCOP::FitRing(&ring1);

    Float_t drCOPCut = fRmsCoeffCOP * sqrt(ring1.fCircleChi2 / ring1.fHits.size());
    if (drCOPCut > fMaxCutCOP)	drCOPCut = fMaxCutCOP;

    xc = ring1.fCircleXCenter;
    yc = ring1.fCircleYCenter;
    r = ring1.fCircleRadius;

    HRich700Ring ring2;
    for (Int_t j = indmin; j < indmax + 1; j++) {
	rx = fData[j].fX - xc;
	ry = fData[j].fY - yc;

	dr = fabs(sqrt(rx * rx + ry * ry) - r);
	if (dr > drCOPCut) continue;
	HRich700Hit hit1;
	hit1.fX = fData[j].fX;
	hit1.fY = fData[j].fY;
	hit1.fId = fData[j].fId;
	ring2.fHits.push_back(hit1);
    }

    if (ring2.fHits.size() < 5) {
	return;
    }

    HRich700RingFitterCOP::FitRing(&ring2);

    RemoveHitsOfFoundRing(indmin, indmax, &ring2);

    AddRing(&ring2);
}

void HRich700RingFinderHough::RemoveHitsOfFoundRing(Int_t indmin, Int_t indmax, HRich700Ring* ring)
{
    // Set fIsUsed flag to true for hits attached to the ring.
    // indmin Minimum index of the hit in local area.
    // indmax Maximum index of the hit in local area.
    // ring Found ring.

    for (UInt_t iH = 0; iH < ring->fHits.size(); iH++) {
	for (Int_t j = indmin; j < indmax + 1; j++) {
	    if (ring->fHits[iH].fX == fData[j].fX && ring->fHits[iH].fY == fData[j].fY){
		fData[j].fIsUsed = kTRUE;
	    }
	}
    }
}

void HRich700RingFinderHough::AddRing(HRich700Ring* ring)
{
    HRich700RingFitterCOP::FitRing(ring);

    HLocation loc;
    loc.set(1, 1);
    HRichHitSim* hitS=0;
    HRichHit* hit = (HRichHit*)fCatRichHit->getNewSlot(loc);
    //          new (hit) HRichHit
    if(fIsSimulation) {
    	hit = new (hit) HRichHitSim();
    	hitS = (HRichHitSim*) hit;
    } else {
	       hit = new (hit) HRichHit();
    }
    if (NULL != hit) {

	Float_t x = ring->fCircleXCenter;
	Float_t y = ring->fCircleYCenter;
        Float_t theta,phi;
	//----------------------------------------------------------------------------------
        hit->nSector               = fDigiPar->getInterpolatedSectorThetaPhi(x,y,theta,phi);
        hit->fTheta                = theta;
        hit->fPhi                  = phi;
	hit->fRich700NofRichCals   = ring->fHits.size();
	hit->fRich700CircleCenterX = ring->fCircleXCenter;
	hit->fRich700CircleCenterY = ring->fCircleYCenter;
	hit->fRich700CircleRadius  = ring->fCircleRadius;
	hit->fRich700CircleChi2    = ring->fCircleChi2;

	hit->setXY(x,y);
        Int_t pmtID = fDigiPar->getPMTId(x,y);
	if(pmtID!=-1){

	    HRich700PmtData* data = fDigiPar->getPMTData(pmtID);
	    if(data){
		hit->setRingCenterX(data->fIndX);
		hit->setRingCenterY(data->fIndY);
		hit->setLabXYZ     (x,y,data->fZ);
	    }
	} else {
	    hit->setLabXYZ(x,y,-1000);
	}


	if(hitS){
           // assign MC information
	    Int_t trackIds[3];
	    Int_t weights[3];
	    RecoToMc(ring, trackIds, weights);

	    hitS->track1 = trackIds[0];
	    hitS->track2 = trackIds[1];
	    hitS->track3 = trackIds[2];

	    hitS->weigTrack1 = weights[0];
	    hitS->weigTrack2 = weights[1];
	    hitS->weigTrack3 = weights[2];
	}
    }
}

void HRich700RingFinderHough::RingSelection()
{
    Int_t nofRings = fFoundRings.size();
    sort(fFoundRings.begin(), fFoundRings.end(), HRich700RingComparatorMore());
    set<UShort_t> usedHitsAll;
    vector<UShort_t> goodRingIndex;
    goodRingIndex.reserve(nofRings);

    for (Int_t iRing = 0; iRing < nofRings; iRing++){
	fFoundRings[iRing].fIsGood = kFALSE;
	Int_t nofHits = fFoundRings[iRing].fHits.size();
	Bool_t isGoodRingAll = kTRUE;
	Int_t nofUsedHitsAll = 0;
	for(Int_t iHit = 0; iHit < nofHits; iHit++){
	    set<UShort_t>::iterator it = usedHitsAll.find(fFoundRings[iRing].fHits[iHit].fId);
	    if(it != usedHitsAll.end()){
		nofUsedHitsAll++;
	    }
	}
	if ((Float_t)nofUsedHitsAll/(Float_t)nofHits > fUsedHitsAllCut){
	    isGoodRingAll = kFALSE;
	}

	if (isGoodRingAll){
	    fFoundRings[iRing].fIsGood = kTRUE;
	    goodRingIndex.push_back(iRing);
	    for(Int_t iHit = 0; iHit < nofHits; iHit++){
		usedHitsAll.insert(fFoundRings[iRing].fHits[iHit].fId);
	    }
	}// isGoodRing
    }// iRing

    usedHitsAll.clear();
    goodRingIndex.clear();
}

void HRich700RingFinderHough::RecoToMc(const HRich700Ring* ring,
				       Int_t* trackIds,
				       Int_t* weights)
{
    map<Int_t, Int_t> trackIdMap;
    for (UInt_t iH = 0; iH < ring->fHits.size(); iH++) {
	Int_t calId = ring->fHits[iH].fId;
	HRichCalSim* richCal = static_cast<HRichCalSim*>(fCatRichCal->getObject(calId));
	if (NULL == richCal) continue;
	Int_t nofTrackIds = richCal->getNofTrackIds();

	for (Int_t iT = 0; iT < nofTrackIds; iT++) {
	    Int_t trackId = richCal->getTrackId(iT);
	    trackIdMap[trackId]++;
	}
    }

    Int_t maxTrackId = 0;
    Int_t maxNofHits = 0;
    for(map<Int_t, Int_t>::iterator it = trackIdMap.begin(); it != trackIdMap.end(); it++) {
	Int_t trackId = it->first;
	Int_t nofHits = it->second;
	if (nofHits > maxNofHits) {
	    maxTrackId = trackId;
	    maxNofHits = nofHits;
	}
    }

    trackIds[0] = maxTrackId;
    trackIds[1] = -1;
    trackIds[2] = -1;

    weights[0] = maxNofHits ;
    weights[1] = 0;
    weights[2] = 0;
}


Bool_t HRich700RingFinderHough::finalize()
{
    return kTRUE;
}
