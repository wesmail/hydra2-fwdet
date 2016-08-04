#ifndef HWALLEVENTPLANEF_H
#define HWALLEVENTPLANEF_H

#include "hreconstructor.h"
#include <iostream> 
#include <iomanip>

#include "TVector2.h"
#include "TFile.h"
#include "TH1F.h"
#include "TRandom.h"

using namespace std;

class HCategory;
class HWallEventPlanePar;


class HWallFiredCellsVA {

protected:
    Int_t    nCells;
    TVector2 vCells[304];

    //-Fired Cells Vector Array---------------------------------------------------------------------//
    //----------------------------------------------------------------------------------------------//
    //- this class was introduced to avoid two loops over events and hence two identical cuts      -//
    //- in each of sample (while in reality it triggers small mistakes, such that cut in one sample-//
    //- is updated, but in the second loop the same update of the cut could be forgotten)          -//
    //- so it is better to enable array where identical selection from the first loop is stored    -//
    //----------------------------------------------------------------------------------------------//
public:

    HWallFiredCellsVA(){
	reset();
    }

    void reset(){
	nCells = 0;
    }

    void setCellVect(TVector2 vc){
	if(nCells < 304){
	    vCells[nCells] = vc;
	    nCells++;
	} else {
	    printf("Error from class HWallFiredCellsVA: attempt to ADD to many cells, can't be so many!!\n");
	}
    }

    TVector2 getCellVector(Int_t n){
	if(n >= 0 && n < nCells){
	    return vCells[n];
	} else {
	    TVector2 emptyVect;
	    emptyVect.Set(0.0,0.0);
	    printf("Error from class HWallFiredCellsVA: attempt to GET cell vector which was not defined, n=%i > nCells=%i!!\n", n, nCells);
	    return emptyVect;
	}
    }

    void print(){
	printf("=== HWallFiredCellsVA::print(//begin//) ===\n");
	for(Int_t i = 0; i < nCells; i ++){
	    printf("==> [%i] == ",i); vCells[i].Print();
	}
	printf("=== HWallFiredCellsVA::print(// end //) ===\n");
    }

    Int_t getNumbOfCells(){ return nCells; }

};

class HWallEventPlaneF : public HReconstructor {
protected:
    HCategory*             fWallHitCat;        //!
    HCategory*             fWallEventPlaneCat; //!
    HWallEventPlanePar*    fWallEventPlanePar; //!
    static Bool_t          fUseCorrection;     //!
    HWallFiredCellsVA      fcellsVect;         //!

public:
    HWallEventPlaneF(const Text_t* name="WallEventPlaneF",const Text_t* title="WallEventPlaneF");
    ~HWallEventPlaneF(void);
    Bool_t init(void);
    Bool_t finalize(void) { return kTRUE; }
    Int_t  execute(void);
    static void setUseCorrection(Bool_t use) { fUseCorrection = use; }
    ClassDef(HWallEventPlaneF,0) // Reconstructor for eventplane
};

#endif
