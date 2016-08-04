#ifndef HPIONTRACKERHITF_H
#define HPIONTRACKERHITF_H

#include "hreconstructor.h"

class HCategory;
class HIterator;
class HPionTrackerHitFPar;
class HPionTrackerGeomPar;

// { t, { s, a } }
typedef std::pair< Float_t, std::pair<Int_t, Float_t> > StripData;

struct HitCanData
{
	Float_t radtimecharge;
	Float_t posX;
	Float_t timeX;
	Float_t chargeX;
	Float_t eventnrX;
	Float_t posY;
	Float_t timeY;
	Float_t chargeY;
	Float_t eventnrY;
	Float_t timeavg;
	Float_t chargeavg;
};

typedef std::vector<HitCanData> hitcan;

class HPionTrackerHitF : public HReconstructor
{
protected:
	HCategory * pCalCat;			// pointer to the cal data
	HCategory * pHitCat;			// pointer to the hit data
	HIterator * iter;				// iterator on cal data.
	HLocation loccal;				// location of the cal object
	HLocation loc;					// location for new hit object
	HPionTrackerHitFPar * pHitfpar;	// hit finder parameters
	HPionTrackerGeomPar * pGeompar;	// geometry parameters

	static bool sortfunction(const HitCanData can1, const HitCanData can2)
	{
	return (can1.radtimecharge < can2.radtimecharge);
	}

	/*enum HitFinderAlgorithm {HFA_ADC_TS, HFA_TS_ONLY};
	  HitFinderAlgorithm hfa;*/
public:
	HPionTrackerHitF (void);
	HPionTrackerHitF (const Text_t * name, const Text_t * title, Bool_t skip = kFALSE);
	~HPionTrackerHitF (void);
	Bool_t init (void);
	Int_t  execute (void);
	Bool_t finalize (void) {return kTRUE;}

	ClassDef (HPionTrackerHitF, 0) // Hit finder cal->hit for PionTracker
};

#endif /* !HPIONTRACKERHITF_H */
