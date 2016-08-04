#ifndef HPIONTRACKERTRACKF_H
#define HPIONTRACKERTRACKF_H

#include "hreconstructor.h"
#include "hpiontrackermompar.h"
#include "hpiontrackermomrunpar.h"

class HCategory;
class HIterator;
class HPionTrackerTrackFPar;
class HPionTrackerBeamPar;
class HPionTrackerMomPar;
class HPionTrackerMomRunPar;


#include <fstream>
// TODO: FLAG GOOD MOMENTUM
struct track
{
	track() : fP(0), fTheta(0), fPhi(0), fMatch(0),
    fX1(0), fY1(0), fX2(0), fY2(0), fXh(0), fYh(0), fPhi0(0), fTheta0(0), fY0(0), fDist(0)  {}

	Float_t fP;
	Float_t fTheta;
	Float_t fPhi;
	Float_t fMatch;
	Float_t fX1;
	Float_t fY1;
	Float_t fX2;
	Float_t fY2;
	Float_t fXh;
	Float_t fYh;
	Float_t fPhi0;
	Float_t fTheta0;
	Float_t fY0;
        Float_t fDist;
};


class HPionTrackerTrackF : public HReconstructor
{

protected:
	HCategory * pHitCat;			// pointer to the cal data
	HCategory * pTrackCat;			// pointer to the hit data
	HIterator * iter;			// iterator on cal data.
	HLocation lochit;			// location of the cal object
	HLocation loc;				// location for new hit object
	HPionTrackerTrackFPar * pTrackfpar;	// pointer to track finder parameters
        HPionTrackerBeamPar   * pTrackBeampar;	// pointer to beam parameters
	HPionTrackerMomPar * pTrackerMompar;	// pointer to momentum selection & correction parameters
        HPionTrackerMomRunPar   * pTrackerMomRunpar;	// pointer to momentum selection & correction  parameters per run
	HPionTrackerMomRunPars correction; // momentum selection & corrections per run
	Double_t corrs[2]; // momentum correction: max transmission & mean offset

	// momentum selection: sort function for minimal distance
	static bool sortfunction(const track tr1, const track tr2)
	{
	  return (tr1.fDist < tr2.fDist);
	}
	
        Int_t idet1 ;
	Int_t idet2 ;

	Int_t id_det1;  // beam element
        Int_t id_det2;  // beam element
        Int_t id_outQ9; // beam element
        Int_t id_targ;  // beam element
        //------------------------------------------------------
        // local vars
	Double_t T12[2];    //!
	Double_t T14[2];    //!
	Double_t T16[2];    //!
	Double_t T126[2];   //!
	Double_t T146[2];   //!
	Double_t T166[2];   //!
	Double_t T32[2];    //!
	Double_t T33[2];    //!
	Double_t T34[2];    //!
	Double_t T36[2];    //!
	Double_t T336[2];   //!
	Double_t T346[2];   //!
	Double_t T366[2];   //!

	Double_t T12_t;     //!
	Double_t T14_t;     //!
	Double_t T16_t;     //!
	Double_t T126_t;    //!
	Double_t T146_t;    //!
	Double_t T166_t;    //!
	Double_t T32_t;     //!
	Double_t T33_t;     //!
	Double_t T34_t;     //!
	Double_t T36_t;     //!
	Double_t T326_t;    //!
	Double_t T336_t;    //!
	Double_t T346_t;    //!
	Double_t T366_t;    //!

	Double_t T21_t;     //!
	Double_t T22_t;     //!
	Double_t T23_t;     //!
	Double_t T24_t;     //!
	Double_t T26_t;     //!
	Double_t T226_t;    //!
	Double_t T246_t;    //!
	Double_t T266_t;    //!
	Double_t T41_t;     //!
	Double_t T42_t;     //!
	Double_t T43_t;     //!
	Double_t T44_t;     //!
	Double_t T46_t;     //!
	Double_t T426_t;    //!
	Double_t T436_t;    //!
	Double_t T446_t;    //!
	Double_t T466_t;    //!
	Int_t nev ;         //! EVENT COUNTER FOR FILE
	std::ifstream inputpos;  //!
	std::ifstream scanpos;  //!
//------------------------------------------------------

        void   initVars();
	void   insertTrack(const track & tr);
        Bool_t momrec (Float_t x1, Float_t y1, Float_t x2, Float_t y2, Float_t momref, track & tr);
public:
	HPionTrackerTrackF (void);
	HPionTrackerTrackF (const Text_t * name, const Text_t * title, Bool_t skip = kFALSE);
	~HPionTrackerTrackF (void);
	void setBeamElements(Int_t id1,Int_t id2,Int_t outQ9,Int_t idtarg) { id_det1=id1; id_det2=id2; id_outQ9=outQ9; id_targ=idtarg;  }

	Bool_t init (void);
	Bool_t reinit (void);
	Int_t  execute (void);
	Bool_t finalize (void) {return kTRUE;}


	ClassDef (HPionTrackerTrackF, 0) // Hit finder cal->hit for PionTracker
};

#endif /* !HPIONTRACKERTRACKF_H */
