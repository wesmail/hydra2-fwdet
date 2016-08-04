#ifndef HPIONTRACKERHIT_H
#define HPIONTRACKERHIT_H

#include "TObject.h"

class HPionTrackerHit : public TObject
{
private:
	Int_t		fPlane;		// plane number (0,1)
	Int_t		fHitCnt;	// hit counter

	Float_t		fTime;
	Float_t		fTimeX;		// time of hit in cluster X
	Float_t		fTimeY;		// time of hit in cluster Y

	Float_t		fCharge;
	Float_t		fChargeX;	// charge of hit in cluster X
	Float_t		fChargeY;	// charge of hit in cluster Y

	Float_t		fLocalX;	// position of cluster in detector coordinate system [mm]
	Float_t		fLocalY;	// position of cluster in detector coordinate system [mm]

	Float_t		fLabX;		// X position of cluster in lab coordinate system [mm]
	Float_t		fLabY;		// Y position of cluster in lab coordinate system [mm]
	Float_t		fLabZ;		// Z position of cluster in lab coordinate system [mm]

public:
	HPionTrackerHit (void) :
		fPlane(-1), fHitCnt(-1),
		fTime (-1000000.F), fTimeX (-1000000.F), fTimeY (-1000000.F),
		fCharge (-1000000.F), fChargeX (-1000000.F), fChargeY (-1000000.F),
		fLocalX (-1000000.F), fLocalY (-1000000.F),
		fLabX (-1000000.F), fLabY (-1000000.F), fLabZ (-1000000.F) {}
	~HPionTrackerHit (void) {}

	inline Int_t	getPlane (void) const			{ return fPlane; }
	inline Int_t	getHitCnt (void) const			{ return fHitCnt; }
	inline Float_t	getTime (void) const 			{ return fTime; }
	inline Float_t	getCharge (void) const 			{ return fCharge; }
	void	getTimeAndCharge (Float_t &, Float_t &);
	void	getTimeAndChargeX (Float_t &, Float_t &);
	void	getTimeAndChargeY (Float_t &, Float_t &);
	void	getLocalPos (Float_t &, Float_t &) const;
	void	getLabPos (Float_t &, Float_t &, Float_t &) const;

	inline void		setPlaneHit (const Int_t m, const Int_t c)		{ fPlane = m; fHitCnt = c; }
	inline void		setPlane (const Int_t m)		{ fPlane = m; }
	inline void		setHitCnt (const Int_t c)		{ fHitCnt = c; }
	void	setTimeAndCharge (const Float_t, const Float_t, const Float_t, const Float_t);
	void	setLocalPos (const Float_t x, const Float_t y);
	void	setLabPos (const Float_t x, const Float_t y, const Float_t z);

private:
	void calcAvgs();

	ClassDef (HPionTrackerHit, 1) // detector hits of PionTracker
};

#endif /* ! HPIONTRACKERHIT_H */