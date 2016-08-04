#ifndef HSLOWPAR_H
#define HSLOWPAR_H

#include "hparset.h"
#include <stdio.h>
#include <map>
#include <vector>
#include <iostream>
#include <fstream>


#include "TString.h"
#include "TNamed.h"
#include "TList.h"
#include "TArrayI.h"

using namespace std;

#define NVALS 4

class HSlowChan : public TNamed {
public:
    HSlowChan(){ clear();}
    ~HSlowChan(){;}

    Double_t mean;
    Double_t rms;
    Double_t min;
    Double_t max;

    void clear(){

	mean = -1;
	rms  = -1;
	min  = -1;
	max  = -1;
    }

    void copyValues(HSlowChan* chan){
        // copy values, but not name !
	if(chan){
	    mean = chan->mean;
	    rms  = chan->rms;
	    min  = chan->min;
            max  = chan->max;
	}
    }

    void set(Double_t vals[]){

       mean = vals[0];
       rms  = vals[1];
       min  = vals[2];
       max  = vals[3];
    }
    void get(Double_t vals[]){

       vals[0] = mean;
       vals[1] = rms;
       vals[2] = min;
       vals[3] = max;
    }

    void set(Double_t Mean, Double_t Rms,Double_t Min,Double_t Max){

       mean = Mean;
       rms  = Rms;
       min  = Min;
       max  = Max;
    }

    Int_t getNValues() { return NVALS; }

    void  print(Int_t runID = -1)
    {
	if(runID !=-1) {
	    cout<< "runID "<<runID
		<<" name "<<GetName()
		<<" mean "<<mean
		<<" rms " <<rms
		<<" min " <<min
		<<" max " <<max
		<<endl;
	} else {
	    cout<<" name "<<GetName()
		<<" mean "<<mean
		<<" rms " <<rms
		<<" min " <<min
		<<" max " <<max
		<<endl;
	}
    }

    ClassDef(HSlowChan,1)
};

class HSlowPar : public HParSet {


protected:
    map<Int_t, map<TString, HSlowChan*>* > fMRunID;
    vector<Int_t>                          fVRunID;   // list of runIDs
    TList fListChannels;                              // list of channels
    map<TString, HSlowChan* >* findRunIDMap(Int_t runId);
    HSlowChan*                 findChannel (map<TString, HSlowChan* >* m ,TString name);
    TString fPartition;                                 // Name ORACLE SLOW CONTROL PARTION, to be set by user!!!

    Int_t fCurrentRunID;                                     //! remember the current runID
    map<Int_t,map<TString, HSlowChan* >* >::iterator fMIter; //! remember the current ChannelMap


public:
    HSlowPar(const Char_t* name   ="SlowPar",
	     const Char_t* title  ="Slow Control Summary container",
	     const Char_t* context="SlowParProduction");
    ~HSlowPar(void) {};


    //-----------------------------------------------------------------------
    Bool_t   setChannel    (Int_t runID,TString name, Double_t ar[],Bool_t overwrite=kFALSE);
    Bool_t   getChannel    (Int_t runID,TString name, Double_t ar[]);
    Bool_t   setChannel    (Int_t runID,TString name, HSlowChan* chan,Bool_t overwrite=kFALSE);
    Bool_t   setChannelMap (Int_t runID);
    Bool_t   getChannel    (Int_t runID,TString name, HSlowChan* chan);
    void     setPartition  (TString name) { fPartition = name; }
    TString  getPartition() { return fPartition; }

    Bool_t   findRunID    (Int_t runId);
    void     removeRun    (Int_t runID);
    Bool_t   getRangeRunID(Int_t& first,Int_t& last);
    Int_t    getNRunIDs() { return fVRunID.size();}
    void     clear(void);
    void     reset(Int_t level = 0);
    void     printParam(void);
    void     printChannel(Int_t runID,TString name);
    void         addToChannelList(TString name, Bool_t sort=kFALSE);
    const TList* getChannelList() { return &fListChannels;}
    void         makeChannelList(Bool_t sort=kTRUE);
    void         printChannelList();
    void         getRunIDList(TArrayI* ar=0);
    void         printRunIDList();
    //-----------------------------------------------------------------------


    //-----------------------------------------------------------------------
    Bool_t   init(HParIo* input,Int_t* set);
    Int_t    write(HParIo* output) ;
    Bool_t   readline(const Char_t* buf) ;
    void     putAsciiHeader(TString&);
    Bool_t   write(fstream& out);
    //-----------------------------------------------------------------------


    ClassDef(HSlowPar,1)
};

#endif  /*!HSLOWPAR_H*/
