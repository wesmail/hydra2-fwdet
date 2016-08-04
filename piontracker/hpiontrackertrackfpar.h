#ifndef HPIONTRACKERTRACKFPAR_H
#define HPIONTRACKERTRACKFPAR_H

#include "hparcond.h"
#include "TArrayF.h"
#include "TArrayI.h"

class HPionTrackerTrackFPar : public HParCond
{
protected:
	Int_t	fTrackingFlag;	   // tracking flag for momentum reconstruction (0: no tracking) 
	Float_t	fRefMom;	   // central momentum
	Float_t fRefX1;		   // reference position X1 at central momentum
	Float_t fRefY1;		   // reference position Y1 at central momentum
	Float_t fRefX2;		   // reference position X2 at central momentum
	Float_t fRefY2;		   // reference position Y2 at central momentum

public:
	HPionTrackerTrackFPar(const Char_t * name = "PionTrackerTrackFPar",
			      const Char_t * title = "HitFinder parameters of the PionTracker",
			      const Char_t * context = "PionTrackerTrackFParProduction");
	virtual ~HPionTrackerTrackFPar() {}

	void    clear();
	void    putParams (HParamList *);
	Bool_t  getParams (HParamList *);

	Int_t	getTrackingFlag() const { return fTrackingFlag; }
	Float_t	getRefMom() const { return fRefMom; }
	Float_t	getRefX1()  const { return fRefX1; }
	Float_t	getRefY1()  const { return fRefY1; }
	Float_t	getRefX2()  const { return fRefX2; }
	Float_t	getRefY2()  const { return fRefY2; }

	void    setTrackingFlag(const Int_t bt) { fTrackingFlag = bt; }
	void    setRefMom(const Float_t rm)     { fRefMom = rm; }
	void    setRefX1(const Float_t rx)      { fRefX1 = rx; }
	void    setRefY1(const Float_t ry)      { fRefY1 = ry; }
	void    setRefX2(const Float_t rx)      { fRefX2 = rx; }
	void    setRefY2(const Float_t ry)      { fRefY2 = ry; }

	ClassDef (HPionTrackerTrackFPar, 1) // Container for the hit finder parameters of the PionTracker
};

#endif  /*!HPIONTRACKERTRACKFPAR_H*/
