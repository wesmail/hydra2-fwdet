#ifndef HPIONTRACKERMOMPAR_H
#define HPIONTRACKERMOMPAR_H

#include "hparcond.h"
#include "TArrayF.h"
#include "TArrayI.h"

class HPionTrackerMomPar : public HParCond
{
protected:
	Int_t fYYWeight;           // weighting factor of the yy correlation
	Float_t fYYSlope;	   // slope of the yy correlation
	Float_t fYYOffset;	   // offset of the yy correlation
	Int_t fMomWeight;          // weighting factor of the yy correlation
	Float_t fElossCorr;	   // energy loss correction of the pion momentum



public:
	HPionTrackerMomPar(const Char_t * name = "PionTrackerMomPar",
			      const Char_t * title = "Momentum correction parameters of the PionTracker",
			      const Char_t * context = "PionTrackerMomParProduction");
	virtual ~HPionTrackerMomPar() {}

	void    clear();
	void    putParams (HParamList *);
	Bool_t  getParams (HParamList *);

	Int_t	getYYWeight() const { return fYYWeight; }
	Float_t	getYYSlope() const { return fYYSlope; }
	Float_t	getYYOffset()  const { return fYYOffset; }
	Int_t	getMomWeight()  const { return fMomWeight; }
	Float_t	getElossCorr()  const { return fElossCorr; }


	void    setYYWeight(const Int_t bt) { fYYWeight = bt; }
	void    setYYSlope(const Float_t rm)     { fYYSlope = rm; }
	void    setYYOffset(const Float_t rx)      { fYYOffset = rx; }
	void    setMomWeight(const Float_t ry)      { fMomWeight = ry; }
	void    setElossCorr(const Float_t rx)      { fElossCorr = rx; }


	ClassDef (HPionTrackerMomPar, 1) // Container for the momentum selection and correction parameters of the PionTracker
};

#endif  /*!HPIONTRACKERMOMPAR_H*/
