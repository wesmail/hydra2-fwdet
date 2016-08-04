#ifndef HMDCCAL2PAR_H
#define HMDCCAL2PAR_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "TRandom.h"
#include "TString.h"
class HMdcDetector;
class HMdcCal2ParSim;

class HMdcCal2ParAngle : public TObject {
friend class HMdcCal2ParSim;
friend class HMdcCal2ParSecSim;
friend class HMdcCal2ParModSim;
friend class HMdcCal2Par;
friend class HMdcCal2ParSec;
friend class HMdcCal2ParMod;

protected:
    Float_t distance[100];
    Float_t distanceErr[100];
public:
    HMdcCal2ParAngle() { clear(); }
    ~HMdcCal2ParAngle() {;}
    void setDistance(Int_t line,Int_t i, Float_t t){distance[line*10+i]=t;}
    void setDistanceError(Int_t line,Int_t i, Float_t t){distanceErr[line*10+i]=t;}

    Float_t getDistance(Int_t line,Int_t i){return distance[line*10+i];}
    Float_t getDistanceError(Int_t line,Int_t i){return distanceErr[line*10+i];}

    void fillDistance(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDistance(p0,0,p1);
	setDistance(p0,1,p2);
	setDistance(p0,2,p3);
	setDistance(p0,3,p4);
	setDistance(p0,4,p5);
	setDistance(p0,5,p6);
	setDistance(p0,6,p7);
	setDistance(p0,7,p8);
	setDistance(p0,8,p9);
	setDistance(p0,9,p10);

    }
   void fillDistanceError(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDistanceError(p0,0,p1);
	setDistanceError(p0,1,p2);
	setDistanceError(p0,2,p3);
	setDistanceError(p0,3,p4);
	setDistanceError(p0,4,p5);
	setDistanceError(p0,5,p6);
	setDistanceError(p0,6,p7);
	setDistanceError(p0,7,p8);
	setDistanceError(p0,8,p9);
	setDistanceError(p0,9,p10);

    }
    void fillDistance(HMdcCal2ParAngle&);
    void fillDistanceError(HMdcCal2ParAngle&);

    void clear() {
	for(Int_t i=0;i<100;i++)
	{
	    distance[i]=0.;
            distanceErr[i]=0.;
	}
    }
    ClassDef(HMdcCal2ParAngle,1) // Angle level of the MDC calibration parameters
};

class HMdcCal2ParMod : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcCal2ParAngle
public:
    HMdcCal2ParMod(Int_t sec = 0, Int_t mod = 0 , Int_t angle = 18);
    ~HMdcCal2ParMod();
    HMdcCal2ParAngle& operator[](Int_t i) {
      return *static_cast<HMdcCal2ParAngle*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCal2ParMod,1) // Module level of the MDC calibration parameters
};


class HMdcCal2ParSec : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdcCal2ParMod
public:
    HMdcCal2ParSec(Int_t sec = 0, Int_t mod = 4);
    ~HMdcCal2ParSec();
    HMdcCal2ParMod& operator[](Int_t i) {
      return *static_cast<HMdcCal2ParMod*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCal2ParSec,1) // Sector level of the MDC calibration parameters
};


class HMdcCal2Par : public HParSet {
friend class HMdcCal2ParSim;
protected:
    TObjArray* array;      // array of pointers of type HMdcCal2ParSec
    HMdcDetector *fMdc;
    TString comment;
    Int_t sector;
    Int_t module;
    Int_t linecounter;
    Int_t linecounterwrite;
    Int_t type;
    HMdcCal2ParSim* cal2parsim;
    Float_t myslopeOutside;
public:
    HMdcCal2Par(const Char_t* name="MdcCal2Par",
                   const Char_t* title=
                       "cal2 calibration parameters for Mdc [time->distance]",
                   const Char_t* context="MdcCal2ParSimProduction",
                   Int_t n=6);
    ~HMdcCal2Par();
    HMdcCal2ParSec& operator[](Int_t i) {
        return *static_cast<HMdcCal2ParSec*>((*array)[i]);
    }
    void setContainerComment(TString mycomment){comment=mycomment;}
    TString getContainerComment(){return comment;}
    void printContainerComment(){printf("%s\n",comment.Data());}
    Int_t getSize() {return array->GetEntries();}
    Int_t getNumberOfLines() {return 20;}
    Bool_t init(HParIo*);
    Int_t write(HParIo*);
    void putAsciiHeader(TString&);
    Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
    Double_t calcDistance(Int_t ,Int_t ,Double_t,Double_t);
    Double_t calcDistanceErr(Int_t ,Int_t ,Double_t,Double_t);
    void calcDistance(Int_t ,Int_t ,Double_t,Double_t,Double_t*,Double_t*);
    void plot(Int_t,Int_t);
    void plot2D(Int_t,Int_t,Int_t);
    void clear();
    void printParam();
    ClassDef(HMdcCal2Par,1) // Container for the MDC calibration parameters
};

#endif  /*!HMDCCAL2PAR_H*/

