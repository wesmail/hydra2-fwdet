#ifndef HPIONTRACKERHITFPAR_H
#define HPIONTRACKERHITFPAR_H

#include "hparcond.h"
#include "TArrayF.h"
#include "TArrayI.h"

class HPionTrackerHitFPar : public HParCond
{
protected:
	Int_t   numModules;				// number of modules
	TArrayF fTimeWindowOffset;		// time window offset
	TArrayF fTimeWindowWidth;		// time window width
	TArrayF fChargeThresh;			// threshold value for single strip charge

	TArrayF fClusterDistX;			// max cluster dist x
	TArrayF fClusterDistT;			// max cluster dist T
	TArrayF fClusterThresh;			// min cluster charge

	TArrayF fHitMatchTimeC;			// center
	TArrayF fHitMatchTimeW;			// width: 3 sigma
	TArrayF fHitMatchChargeC;		// center
	TArrayF fHitMatchChargeW;		// width: 3 sigma

	Int_t   numPlanes;				// number of modules
	TArrayI fPlanePair;

public:
	HPionTrackerHitFPar (const Char_t * name = "PionTrackerHitFPar",
						 const Char_t * title = "HitFinder parameters of the PionTracker",
						 const Char_t * context = "PionTrackerHitFParProduction");
	~HPionTrackerHitFPar() {}

	void    clear();
	void    putParams (HParamList *);
	Bool_t  getParams (HParamList *);

	Float_t getNumModules() const {return numModules;}
	Float_t getTimeWindowOffset (const Int_t) const;
	Float_t getTimeWindowWidth (const Int_t) const;
	Float_t getChargeThresh (const Int_t) const;
	Float_t getClusterDistX (const Int_t) const;
	Float_t getClusterDistT (const Int_t) const;
	Float_t getClusterThresh (const Int_t) const;
	Float_t getHitMatchTimeC (const Int_t) const;
	Float_t getHitMatchTimeW (const Int_t) const;
	Float_t getHitMatchChargeC (const Int_t) const;
	Float_t getHitMatchChargeW (const Int_t) const;
	Int_t	getNumPlanes() const  { return numPlanes;}
	void	getPlanePair (const Int_t, Int_t & x, Int_t & y) const;
	Int_t	getPlaneX (const Int_t) const;
	Int_t	getPlaneY (const Int_t) const;

	
	Bool_t  setNumModules (const Int_t);
	void    setTimeWindowOffset (const Int_t, const Float_t);
	void    setTimeWindowWidth (const Int_t, const Float_t);
	void    setChargeThresh (const Int_t, const Float_t);
	void	setClusterDistX (const Int_t, const Float_t);
	void	setClusterDistT (const Int_t, const Float_t);
	void	setClusterThresh (const Int_t, const Float_t);
	void	setHitMatchTime (const Int_t, const Float_t, const Float_t);
	void	setHitMatchCharge (const Int_t, const Float_t, const Float_t);
	void	setNumPlanes(const Int_t);
	void	setPlanePair (const Int_t, const Int_t, const Int_t);

private:
	Bool_t getParam(HParamList * l, const char * name, TArrayF * arr, Int_t size);
	Bool_t getParam(HParamList * l, const char * name, TArrayI * arr, Int_t size);

	ClassDef (HPionTrackerHitFPar, 1) // Container for the hit finder parameters of the PionTracker
};

#endif  /*!HPIONTRACKERHITFPAR_H*/
