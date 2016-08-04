#ifndef HPIONTRACKERMOMRUNPAR_H_
#define HPIONTRACKERMOMRUNPAR_H_

#include "TArrayD.h"
#include "TArrayF.h"
#include "TArrayI.h"

#include "hparcond.h"
#include "hpario.h"
#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>

using namespace std;

class HParamList;

class HDetParAsciiFileIo;

class HPionTrackerMomRunPars : public TObject {

protected:
    Int_t   npars; // ???????
    TArrayD offsets; // ???

public:

    HPionTrackerMomRunPars(Int_t size=2) { offsets.Set(size); clear();} // size 2: max trans. value + offset
    ~HPionTrackerMomRunPars(){;}
    inline Int_t getNVals() { return offsets.GetSize(); }

    void setSize(Int_t size) { offsets.Set(size); clear(); }
    void setVal(Int_t i,Float_t val) {
	if( i >= 0 && i < getNVals()){
	    offsets[i] = val;
	} else { Error("setVal()","Index i=%i out of bounds",i);}
    }
    Float_t getVal(Int_t i) {
	if( i >= 0 && i < getNVals()){
	    return offsets[i];
	} else {
	    Error("setVal()","Index i=%i out of bounds",i);
            return 0;
	}
    }

    void copyVals(HPionTrackerMomRunPars* data){ for(Int_t i = 0; i < getNVals(); i ++) setVal(i,data->getVal(i)); }
    void getVals(HPionTrackerMomRunPars* data) { for(Int_t i = 0; i < getNVals(); i ++) data->setVal(i,offsets[i]);   }
    void print(Int_t runID = -1){

	if(runID != -1) cout<<"RunID "<<setw(10)<<runID <<" = "<<flush;
        else            cout<<" = "<<flush;

	for(Int_t i = 0; i < getNVals() ; i ++){
	    if(i == 0) cout<<setw(8)<<offsets[i]<<flush;
	    else       cout<<", "<<setw(8)<<offsets[i]<<flush;
	}
        cout<<endl;
    }
    Double_t* data()     { return &offsets[0]; }
    void      clear()    { for(Int_t i=0;i < getNVals();i++) offsets[i] = 0; }

    ClassDef(HPionTrackerMomRunPars,1)
};




class HPionTrackerMomRunPar : public HParCond {

protected:

    map<Int_t, HPionTrackerMomRunPars* > fMRunID;
    vector<Int_t>                        fVRunID;           // list of runIDs
    map<Int_t, HPionTrackerMomRunPars* >::iterator  fMIter; //! remember the current runMap

public:

    HPionTrackerMomRunPar(const Char_t* name    = "PionTrackerMomRunPar",
			  const Char_t* title   = "PionTracker Mom Parameters per run",
			  const Char_t* context = "PionTrackerMomRunParProduction");
    ~HPionTrackerMomRunPar();

    Bool_t        findRunID(Int_t runID);
    Bool_t        removeRun(Int_t runID,Int_t level=1);
    Bool_t        addRun   (Int_t runID,HPionTrackerMomRunPars* data,Bool_t overwrite=kFALSE);
    Bool_t        getRun   (Int_t runID,HPionTrackerMomRunPars* data);
    void          finishRunList();
    Bool_t        getRangeRunID(Int_t& first,Int_t& last);
    Int_t         getNRunIDs() { return fVRunID.size();}
    void          getRunIDList(TArrayI* ar=0);
    void          printRunIDList();
    void          printRun(Int_t runID);

    void          reset(Int_t level = 0);

    Bool_t        init(HParIo*, Int_t*);
    void          clear();
    void          printParams();
    void          putParams(HParamList* l);
    Bool_t        getParams(HParamList* l);
    virtual Int_t write(HParIo*);
    virtual Int_t write(fstream& pFile);
    virtual void  putAsciiHeader(TString& b);

    ClassDef(HPionTrackerMomRunPar, 1) // Container for the PionTracker momentum parameters per run
};


#endif /*HPIONTRACKERMOMRUNPAR_H_*/
