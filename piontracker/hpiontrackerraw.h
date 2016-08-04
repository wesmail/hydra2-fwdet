#ifndef HPIONTRACKERRAW_H
#define HPIONTRACKERRAW_H

#include "TObject.h"

class HPionTrackerRaw : public TObject
{
protected:
	Int_t   fMultiplicity;    // number of hits per strip per event
	Int_t   fModule;          // module number
	Int_t	fStrip;

	static const Int_t fMaxMultiplicity = 1;

	Float_t fTime[fMaxMultiplicity];
	Float_t fCharge[fMaxMultiplicity];
	Char_t fPileUp[fMaxMultiplicity];
	Char_t fOverflow[fMaxMultiplicity];

public:
	HPionTrackerRaw ();
	~HPionTrackerRaw ();

	inline Int_t	getMultiplicity ()	const { return fMultiplicity; }
	inline Int_t	getModule ()		const { return fModule; }
	inline Int_t	getStrip ()			const { return fStrip; };
	Float_t	getTime (const Int_t n)		const;
	Float_t	getCharge (const Int_t n)	const;
	Char_t	getPileup (const Int_t n)	const;
	Char_t	getOverflow (const Int_t n)	const;
	void	getTimeAndCharge (const Int_t n, Float_t & time, Float_t & charge);	// hit: time, charge
	void	getErrors (const Int_t n, Char_t & pileup, Char_t & overflow);	// hit: pileup, overflow

	inline void	getAddress (Int_t & m, Int_t & s)	{ m = fModule; s = fStrip; }
	inline void	setModule (const Int_t m)			{ fModule = m; }
	inline void	setStrip (const Int_t s)			{ fStrip = s; }
	inline void	setAddress (const Int_t m, const Int_t s)	{ fModule = m; fStrip = s; }
	Bool_t	setData (const Float_t time, const Float_t charge, const Char_t pileup = 0, const Char_t overflow = 0);

	ClassDef (HPionTrackerRaw, 1) // raw data of PionTracker
};

#endif /* ! HPIONTRACKERRAW_H */
