
#ifndef HRICH700DATA_H
#define HRICH700DATA_H

#include "TObject.h"
#include <vector>

using namespace std;

#include "TObject.h"
#include "hrich700pmttypeenum.h"

class HRich700Hit {
public:
	Float_t fX;
	Float_t fY;
	Int_t fId; // index of the richCal object
};


class HRich700Ring {
public:
	// circle parameters
	Double_t fCircleXCenter;
	Double_t fCircleYCenter;
	Double_t fCircleRadius;
	Double_t fCircleChi2;

	Bool_t fIsGood;

	// array of RICH hits assigned to the ring
	vector<HRich700Hit> fHits;
};

class HRich700PmtData {
public:
	Double_t fX; // PMT center X position in [mm]
	Double_t fY; // PMT center Y position in [mm]
	Double_t fZ; // PMT center Z position in [mm]
	Int_t fIndX; // index X of PMT
	Int_t fIndY; // index Y of PMT
	Int_t fPmtId; // PMT ID
    HRich700PmtTypeEnum fPmtType; // type of PMT (QE curve)
	Int_t fSector; // Sector number
    Double_t fTheta; // theta for track matching
    Double_t fPhi; // phi for track matching
};


#endif
