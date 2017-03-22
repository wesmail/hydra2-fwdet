
#ifndef HRICH700DATA_H
#define HRICH700DATA_H

#include "TObject.h"
#include <vector>

using namespace std;

#include "TObject.h"

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
	Double_t fX;
	Double_t fY;
	Double_t fZ;
	Int_t fIndX;
	Int_t fIndY;
	Int_t fPmtId;
	Int_t fSector;
        Double_t fTheta;
        Double_t fPhi;
};


#endif 
