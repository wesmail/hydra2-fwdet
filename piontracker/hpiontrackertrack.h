#ifndef HPIONTRACKERTRACK_H
#define HPIONTRACKERTRACK_H

#include "TObject.h"

class HPionTrackerTrack : public TObject
{
private:
	Float_t		fP;
	Float_t		fTheta;
	Float_t		fPhi;
	Float_t		fCosTheta;

	Float_t		fX1;		// position on pion det 1
	Float_t		fY1;

	Float_t		fX2;		// position on pion det 2
	Float_t		fY2;

	Float_t		fXh;		// X position on target
	Float_t		fYh;		// Y position on target

	Float_t		fPx;		// X momentum
	Float_t		fPy;		// Y momentum
	Float_t		fPz;		// Z momentum

	Float_t		fPhi0;		// phi production target	
	Float_t		fTheta0;		// theta production target
	Float_t		fY0;		// y postion at the production target

	Float_t		fMatch;		// quality match
	Float_t         fDist;          // momentum selection based on distance

public:
	HPionTrackerTrack (void) :
		fP(-1000.0), fTheta(-1000.0), fPhi(-1000.0), fCosTheta(-1000.0),
		fX1(-1000.), fY1(-1000.0), fX2(-1000.0), fY2(-1000.0),
		fXh(-1000.0), fYh(-1000.0),
		fPx (-1000000.F), fPy (-1000000.F), fPz (-1000000.F), fPhi0(-1000.0), fTheta0(-1000.0), fY0(-1000.0),
		fMatch(-1000), fDist(-1000.0) 	{}
	~HPionTrackerTrack (void) {}

	inline Float_t	getPx	  (void) const	 { return fPx; }
	inline Float_t	getPy	  (void) const  { return fPy; }
	inline Float_t	getPz	  (void) const  { return fPz; }
	inline Float_t	getMatch   (void) const  { return fMatch; }
	inline Float_t	getP	   (void) const  { return fP; }
	inline Float_t	getTheta   (void) const  { return fTheta; }
	inline Float_t	getPhi	 (void) const  { return fPhi; }
	inline Float_t	getCosTheta(void) const  { return fCosTheta; }
	inline Float_t	getPhi0		(void) const { return fPhi0; }
	inline Float_t	getTheta0	(void) const { return fTheta0; };
	inline Float_t	getY0		(void) const { return fY0; };
	inline Float_t  getDist 	(void) const { return fDist; };

	void setPxyz (Float_t px, Float_t py, Float_t pz, Float_t match);
	void setPThetaPhi (Float_t p, Float_t theta, Float_t phi, Float_t match);
	void setPos1(Float_t, Float_t);
	void setPos2(Float_t, Float_t);
	void setPosH(Float_t, Float_t);
	void setPosAll(Float_t, Float_t, Float_t, Float_t, Float_t, Float_t);
	void setProdAngles(Float_t, Float_t);
	void setProdY(Float_t);
	void setDist(Float_t){};

	void getPos1(Float_t &, Float_t &) const;
	void getPos2(Float_t &, Float_t &) const;
	void getPosH (Float_t &, Float_t &) const;
	void getProdAngles(Float_t &, Float_t &) const;
	void getProdY(Float_t &) const;

	ClassDef (HPionTrackerTrack, 3) // detector hits of PionTracker
};

#endif /* ! HPIONTRACKERTRACK_H */
