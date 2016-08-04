//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////
//
//  HPionTrackerHitF:
//
//  Hit finder of the PionTracker
//
////////////////////////////////////////////////////////////////

#include "hpiontrackerhitf.h"
#include "hpiontrackerdef.h"
#include "hpiontrackercal.h"
#include "hpiontrackerhit.h"
#include "hpiontrackerdetector.h"
#include "hpiontrackerhitfpar.h"
#include "hpiontrackergeompar.h"
#include "../base/geometry/hgeomtransform.h"
#include "hades.h"
#include "hcategory.h"
#include "hdebug.h"
#include "hevent.h"
#include "hiterator.h"
#include "hruntimedb.h"
#include "hspectrometer.h"

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <string.h>
using namespace std;

#define PR(x) std::cout << "++DEBUG: " << #x << " = |" << x << "| (" << __FILE__ << ", " << __LINE__ << ")\n";
#define PRh(x) std::cout << "++DEBUG: " << #x << " = hex |" << std::hex << x << std::dec << "| (" << __FILE__ << ", " << __LINE__ << ")\n";

typedef std::vector< StripData > strips_vec; // vector for fired strips and also used for cross talk selection
typedef std::vector<HitCanData> hitcan; // vector for hit candidates

struct ClusterCand
{
    strips_vec hits; //neighbours

    //    void print(Int_t m = -1)
    //    {
    //            Int_t min_t, max_t, min_c, max_c;
    //            min_t = max_t = hits[0].first;
    //            min_c = max_c = hits[0].second.second;
    //
    //            printf("Cluster candidate (%d):\n", (int)hits.size());
    //            printf(" -c:\ts: %d,\tt: %.0f,\tc: %.0f\n", hits[0].second.first, hits[0].first, hits[0].second.second);
    //            for (size_t h = 1; h < hits.size(); ++h)
    //            {
    //                    printf("  n:\ts: %d,\tt: %.0f,\tc: %.0f\n", hits[h].second.first, hits[h].first, hits[h].second.second);
    //
    //                    if (hits[h].first < min_t) min_t = hits[h].first;
    //                    if (hits[h].first > max_t) max_t = hits[h].first;
    //
    //                    if (hits[h].second.second < min_c) min_c = hits[h].second.second;
    //                    if (hits[h].second.second > max_c) max_c = hits[h].second.second;
    //            }
    //            if (hits.size() > 1)
    //                    printf("CSTAT:\t%d\t%lu\t%d\t%d\n", m, hits.size(), max_t - min_t, max_c - min_c);
    //    }
};

struct ClusterData
{
    Int_t strip;                  // keeps the info about local maximum strip location

    Float_t pos;                  // weighted position of the cluster
    Float_t time;                 // weighted time of the cluster
    Float_t charge;                       // total charge of teh cluster

    //    void print()
    //    {
    //            printf("Cluster (%d):\n", (int)strip);
    //            printf(" -d:\tp: %.0f,\tt: %.0f,\tc: %.0f\n", pos, time, charge);
    //    }
};


struct hit // vector of hit candidates
{
    hitcan h[2];
};


struct HitsData
{
    Float_t posX;                 // weighted position of the cluster
    Float_t timeX;                        // weighted time of the cluster
    Float_t chargeX;                      // total charge of teh cluster

    Float_t posY;                 // weighted position of the cluster
    Float_t timeY;                        // weighted time of the cluster
    Float_t chargeY;                      // total charge of teh cluster

    Float_t time_avg;                     // weighted time of the cluster
    Float_t charge_avg;                   // total charge of teh cluster

    void calcAvgs()
    {
	Float_t qtot = chargeX + chargeY;
	time_avg = (timeX * chargeX + timeY*chargeY)/qtot;
	charge_avg = (chargeX + chargeY) / 2.0;
    }

    //    void print()
    //    {
    //            printf("Matched hit:\tx\ty\n");
    //            printf(" position:\t%.1f\t%.1f\n time\t%.0f\t%.0f\t%.0f\n charge:\t%.0f\t%.0f\t%.0f\n",
    //                       posX, posY, timeX, timeY, time_avg, chargeX, chargeY, charge_avg);
    //    }
};

// Sort hits by charge
Bool_t hit_sort_c(StripData a, StripData b)
{
    return a.second.second < b.second.second;
}

typedef std::vector< ClusterCand > clus_cand_vec;
typedef std::vector< ClusterData > clusters_vec;
typedef std::vector< HitsData > hits_vec;


ClassImp (HPionTrackerHitF)
    HPionTrackerHitF::HPionTrackerHitF (void)
{
    // default constructor
    pCalCat      = NULL;
    pHitCat      = NULL;
    iter         = NULL;
    pHitfpar     = NULL;
    pGeompar     = NULL;
}

HPionTrackerHitF::HPionTrackerHitF (const Text_t * name, const Text_t * title, Bool_t skip)
: HReconstructor (name, title)
{
    // constructor
    // the parameter "skip" (default; kFALSE) defines, if the event is skipted in case no hit can be reconstructed
    pCalCat      = NULL;
    pHitCat      = NULL;
    iter         = NULL;
    pHitfpar     = NULL;
    pGeompar     = NULL;
}

HPionTrackerHitF::~HPionTrackerHitF (void)
{
    //destructor deletes the iterator on the raw category
    if (NULL != iter)
    {
	delete iter;
	iter = NULL;
    }
}

Bool_t HPionTrackerHitF::init (void)
{
    // gets the parameter containers
    // gets the PionTrackerCal category and creates the PionTrackerHit category
    // creates an iterator which loops over all fired strips in PionTrackerCal

    HPionTrackerDetector * det = (HPionTrackerDetector *) gHades->getSetup()->getDetector ("PionTracker");

    if (!det)
    {
	Error ("init", "No PionTracker found.");
	return kFALSE;
    }

    pHitfpar = (HPionTrackerHitFPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerHitFPar");

    if (!pHitfpar) return kFALSE;

    pGeompar = (HPionTrackerGeomPar *) gHades->getRuntimeDb()->getContainer ("PionTrackerGeomPar");

    if (!pGeompar) return kFALSE;

    pCalCat = gHades->getCurrentEvent()->getCategory (catPionTrackerCal);

    if (!pCalCat)
    {
	Error ("init()", "HPionTrackerCal category not available!");
	return kFALSE;
    }

    iter = (HIterator *) pCalCat->MakeIterator();
    loccal.set (2, 0, 0);

    pHitCat = det->buildCategory (catPionTrackerHit);
    if (!pHitCat) return kFALSE;

    loc.set(2, 0, 0);

    fActive = kTRUE;

    return kTRUE;
}

Int_t HPionTrackerHitF::execute (void)
{
    // make hits
    HPionTrackerCal * pCal = NULL;
    HPionTrackerHit * pHit = NULL;

    const Int_t modules = pHitfpar->getNumModules();

    strips_vec fired_strips[modules];
    strips_vec can_strips[modules];
    //clus_cand_vec clus_cand[modules];
    //clusters_vec clusters[modules];

    for (Int_t entry = 0; entry < pCalCat->getEntries(); ++entry)
    {
	if (NULL == (pCal = static_cast<HPionTrackerCal *> (pCalCat->getObject (entry))))
	{
	    Error ("execute", "Pointer to HPionTrackerCal object == NULL, returning");
	    return -1;
	}

	Int_t module = pCal->getModule();
	Int_t strip = pCal->getStrip();

	size_t mult = pCal->getMultiplicity();

	Float_t win_l = pHitfpar->getTimeWindowOffset(module);
	Float_t win_u = pHitfpar->getTimeWindowWidth(module) + win_l;
	//

	// put all fired strips info into array
	for (size_t i = 0; i < mult; ++i)
	{
	    Float_t hit_t;
	    Float_t hit_c;
	    // get time and charge
	    pCal->getTimeAndCharge(i, hit_t, hit_c);

	    // check if time is inside the selection window
	    if ( (hit_t < win_l) or (hit_t > win_u))  // ----------------------------or (hit_c < q_thresh) )
		continue;

	    //---------------------------------------------------------------
	    // fix warnings gcc < 4.7
	    std::pair<Int_t, Float_t> pinner(strip, hit_c);
	    StripData stripdat(hit_t,pinner);
	    fired_strips[module].push_back(stripdat);
	    //fired_strips[module].push_back( { hit_t, { strip, hit_c } } );  //  for C++11 gcc > 4.7
	    //---------------------------------------------------------------
	}
    }

    //-----------------------------------------------------HIT SELECTION ONLY BY TIME +Charge  (Joana & Alessandro) ------------------------------------------------------------------------

    hit hits;

    hits.h[0].clear();
    hits.h[1].clear();


    // FIRST : Loop over strips to check for crosstalk. If 2 or 3 near strips (of the same module) are fired than merge together as only 1 hit

    for (Int_t m1 = 0; m1 < modules; ++m1)
    {
	for (UInt_t h1 = 0; h1 < fired_strips[m1].size(); ++h1)
	{
	    Float_t stripdiff1=fired_strips[m1][h1].second.first-fired_strips[m1][h1+1].second.first;
	    Float_t stripdiff2=fired_strips[m1][h1].second.first-fired_strips[m1][h1+2].second.first;
	    if(abs(stripdiff1)==1)
	    {
		if(abs(stripdiff2)==2)
		{
		    Int_t av_strip=fired_strips[m1][h1+1].second.first;
		    Float_t av_time=(fired_strips[m1][h1].first+fired_strips[m1][h1+1].first+fired_strips[m1][h1+2].first)/3.;
		    Float_t av_charge=(fired_strips[m1][h1].second.second+fired_strips[m1][h1+1].second.second+fired_strips[m1][h1+2].second.second)/3.;
		    std::pair<Int_t, Float_t> stripper(av_strip, av_charge);
		    StripData stripcandat(av_time,stripper);
		    can_strips[m1].push_back(stripcandat);
		    h1++;

		}
		else
		{
		    Int_t av_strip=fired_strips[m1][h1].second.first;
		    Float_t av_time=(fired_strips[m1][h1].first+fired_strips[m1][h1+1].first)/2.;
		    Float_t av_charge=(fired_strips[m1][h1].second.second+fired_strips[m1][h1+1].second.second)/2.;
		    std::pair<Int_t, Float_t> stripper(av_strip, av_charge);
		    StripData stripcandat(av_time,stripper);
		    can_strips[m1].push_back(stripcandat);

		}
		h1++;

	    }
	    else
	    {
		std::pair<Int_t, Float_t> stripper(fired_strips[m1][h1].second.first,fired_strips[m1][h1].second.second);
		StripData stripcandat(fired_strips[m1][h1].first,stripper);
		can_strips[m1].push_back(stripcandat);

	    }
	}
    }


    // SECOND: Loop over all hit candidates and application of the -30:30 time difference between front&back sides of each detector + charge difference cut

    for (Int_t m1 = 0; m1 < modules; ++m1)
    {
	Float_t clus_time_diff = pHitfpar->getClusterDistT(m1);
	Float_t charge_diff = pHitfpar->getChargeThresh(m1);

	for (Int_t m2 = m1+1; m2< modules; ++m2)
	{
	    for (UInt_t h1 = 0; h1 < can_strips[m1].size(); ++h1)
	    {
		for (UInt_t h2 = 0; h2 < can_strips[m2].size(); ++h2)
		{
		    Float_t difft = can_strips[m1][h1].first-can_strips[m2][h2].first;

		    if (difft > (-1. * clus_time_diff) and difft < clus_time_diff) //time difference cut
		    {

			Float_t diffc = can_strips[m1][h1].second.second-can_strips[m2][h2].second.second;
			Float_t radius = sqrt(diffc*diffc + difft * difft);

			if (m1 == 0 and m2 == 1) // first detector
			{
			    Float_t chargethr_u =  charge_diff;
			    Float_t chargethr_l =  -1.0 * charge_diff;

			    if (can_strips[m1][h1].second.second != HPionTrackerCal::InvalidAdc() and can_strips[m2][h2].second.second != HPionTrackerCal::InvalidAdc() ) // check for valid ADC data
			    {
				if( diffc >= chargethr_l and diffc <= chargethr_u) //charge difference cut
				{
				    Float_t avg_t = (can_strips[m1][h1].first+can_strips[m2][h2].first)/2.;
				    Float_t avg_c = (can_strips[m1][h1].second.second+can_strips[m2][h2].second.second)/2.;

				    HitCanData can;
				    can.radtimecharge = radius;
				    can.posX = can_strips[m1][h1].second.first;
				    can.timeX = can_strips[m1][h1].first;
				    can.chargeX = can_strips[m1][h1].second.second;
				    can.eventnrX = h1;
				    can.posY = can_strips[m2][h2].second.first;
				    can.timeY = can_strips[m2][h2].first;
				    can.chargeY = can_strips[m2][h2].second.second;
				    can.eventnrY = h2;
				    can.timeavg = avg_t;
				    can.chargeavg = avg_c;
				    hits.h[0].push_back(can);
				}
			    }

			    else
			    {	Float_t avg_t = (can_strips[m1][h1].first+can_strips[m2][h2].first)/2.;
				Float_t avg_c = (can_strips[m1][h1].second.second+can_strips[m2][h2].second.second)/2.;

			    HitCanData can;
			    can.radtimecharge = radius;
			    can.posX = can_strips[m1][h1].second.first;
			    can.timeX = can_strips[m1][h1].first;
			    can.chargeX = can_strips[m1][h1].second.second;
			    can.eventnrX = h1;
			    can.posY = can_strips[m2][h2].second.first;
			    can.timeY = can_strips[m2][h2].first;
			    can.chargeY = can_strips[m2][h2].second.second;
			    can.eventnrY = h2;
			    can.timeavg = avg_t;
			    can.chargeavg = avg_c;
			    hits.h[0].push_back(can);

			    }
			}

			if (m1 == 2 and m2 == 3) //second detector
			{
			    Float_t chargethr_u =  charge_diff;
			    Float_t chargethr_l =  -1.0 * charge_diff;

			    if (can_strips[m1][h1].second.second != HPionTrackerCal::InvalidAdc() and can_strips[m2][h2].second.second != HPionTrackerCal::InvalidAdc()) // check for valid ADC data
			    {

				if( diffc >= chargethr_l and diffc <= chargethr_u) //charge difference cut
				{
				    Float_t avg_t = (can_strips[m1][h1].first+can_strips[m2][h2].first)/2.;
				    Float_t avg_c = (can_strips[m1][h1].second.second+can_strips[m2][h2].second.second)/2.;

				    HitCanData can;
				    can.radtimecharge = radius;
				    can.posX = can_strips[m1][h1].second.first;
				    can.timeX = can_strips[m1][h1].first;
				    can.chargeX = can_strips[m1][h1].second.second;
				    can.eventnrX = h1;
				    can.posY = can_strips[m2][h2].second.first;
				    can.timeY = can_strips[m2][h2].first;
				    can.chargeY = can_strips[m2][h2].second.second;
				    can.eventnrY = h2;
				    can.timeavg = avg_t;
				    can.chargeavg = avg_c;
				    hits.h[1].push_back(can);
				}
			    }

			    else
			    {
				Float_t avg_t = (can_strips[m1][h1].first+can_strips[m2][h2].first)/2.;
				Float_t avg_c = (can_strips[m1][h1].second.second+can_strips[m2][h2].second.second)/2.;

				HitCanData can;
				can.radtimecharge = radius;
				can.posX = can_strips[m1][h1].second.first;
				can.timeX = can_strips[m1][h1].first;
				can.chargeX = can_strips[m1][h1].second.second;
				can.eventnrX = h1;
				can.posY = can_strips[m2][h2].second.first;
				can.timeY = can_strips[m2][h2].first;
				can.chargeY = can_strips[m2][h2].second.second;
				can.eventnrY = h2;
				can.timeavg = avg_t;
				can.chargeavg = avg_c;
				hits.h[1].push_back(can);
			    }
			}
		    }
		}
	    }
	}
    }

    //Sorting minimal radius(minimal time difference & minimal charge difference) (Preparing for clustering)
    std::sort(hits.h[1].begin(), hits.h[1].end(), HPionTrackerHitF::sortfunction);
    std::sort(hits.h[0].begin(), hits.h[0].end(), HPionTrackerHitF::sortfunction);

    //FORTH: Loop over particles: CLUSTERING (if ADC valid)
    for (Int_t p = 0; p < 2; ++p)
    {
	for (UInt_t h1 = 0; h1 < hits.h[p].size(); ++h1)
	{
	    if (hits.h[p][h1].chargeX == HPionTrackerCal::InvalidAdc())
		continue;

	    int eventnrX_ref = hits.h[p][h1].eventnrX;
	    int eventnrY_ref = hits.h[p][h1].eventnrY;

	    for (UInt_t h2 = h1+1; h2 < hits.h[p].size(); ++h2)
	    {
		int eventnrX = hits.h[p][h2].eventnrX;
		int eventnrY = hits.h[p][h2].eventnrY;

		if (eventnrX_ref == eventnrX or eventnrY_ref == eventnrY)
		{
		    hits.h[p].erase(hits.h[p].begin()+h2);
		    --h2;
		}
	    }
	}
    }


    //--------------------------------------------------END-------------------------------------------------------------------------------------

    // make hit matching for each plane
    for (Int_t p = 0; p < pHitfpar->getNumPlanes(); ++p)
    {
	Int_t planeX, planeY;
	pHitfpar->getPlanePair(p, planeX,  planeY); // (0 1), (2 3)

	HPionTrackerDetGeomPar * dgeo = pGeompar->getDetector(p); // detector geometry
	HGeomTransform gt = dgeo->getLabTransform();           // transformation info

	Int_t plane_hit_cnt = 0;
	loc[0] = p;

	for (size_t i = 0; i < hits.h[p].size(); ++i)
	{
	    loc[1] = plane_hit_cnt++; //first save value to loc[1], then increase hit counter by one

	    pHit = (HPionTrackerHit*)pHitCat->getObject(loc);
	    if (!pHit)
	    {
		pHit = (HPionTrackerHit *)pHitCat->getSlot(loc);
		if (pHit)
		{
		    pHit = new(pHit) HPionTrackerHit;
		    pHit->setPlaneHit(p, loc[1]); // save plane & hit number info to the object

		    pHit->setTimeAndCharge(hits.h[p][i].timeX, hits.h[p][i].chargeX, hits.h[p][i].timeY, hits.h[p][i].chargeY);
		    pHit->setLocalPos(hits.h[p][i].posX, hits.h[p][i].posY);

		    // hit postion in the local frame
		    HGeomVector v1(
				   dgeo->getStripPos(hits.h[p][i].posX),
				   dgeo->getStripPos(hits.h[p][i].posY),
				   0);

		    // hit position in the lab frame
		    HGeomVector v2 = gt.getTransVector() + gt.getRotMatrix() * v1;

		    // set lab values to the object
		    pHit->setLabPos(v2.getX(), v2.getY(), v2.getZ());
		}
		else
		{
		    Error("execute()", "Can't get slot plane=%i, cnt=%i for hit %lu",
			  loc[0], loc[1], i+1);
		    return 0;
		}
	    }
	    else
	    {
		Error("execute()", "Slot already exists for plane=%i, cnt=%i", loc[0], loc[1]);
		return -1;
	    }
	}
    }
    return 0;
}
