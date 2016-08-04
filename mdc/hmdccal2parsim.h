#ifndef HMDCCAL2PARSIM_H
#define HMDCCAL2PARSIM_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "TRandom.h"
#include "TF1.h"
#include "TString.h"
class HMdcDetector;
class HMdcCal2Par;

class HMdcCal2ParAngleSim : public TObject {
friend class HMdcCal2ParSim;
friend class HMdcCal2ParSecSim;
friend class HMdcCal2ParModSim;
friend class HMdcCal2Par;
friend class HMdcCal2ParSec;
friend class HMdcCal2ParMod;
friend class HMdcCal2ParAngle;

protected:
    Float_t drifttime1[100];
    Float_t drifttime2[100];
    Float_t drifttime1Err[100];
    Float_t drifttime2Err[100];
public:
    HMdcCal2ParAngleSim() { clear(); }
    ~HMdcCal2ParAngleSim() {;}
    void setDriftTime1(Int_t i, Float_t t)     {drifttime1[i]=t;}
    void setDriftTime2(Int_t i, Float_t t)     {drifttime2[i]=t;}
    void setDriftTime1Error(Int_t i, Float_t t){drifttime1Err[i]=t;}
    void setDriftTime2Error(Int_t i, Float_t t){drifttime2Err[i]=t;}
    Float_t getDriftTime1(Int_t i)     {return drifttime1[i];}
    Float_t getDriftTime2(Int_t i)     {return drifttime2[i];}
    Float_t getDriftTime1Error(Int_t i){return drifttime1Err[i];}
    Float_t getDriftTime2Error(Int_t i){return drifttime2Err[i];}

    void setDriftTime1(Int_t line,Int_t i, Float_t t){drifttime1[line*10+i]=t;}
    void setDriftTime2(Int_t line,Int_t i, Float_t t){drifttime2[line*10+i]=t;}
    void setDriftTime1Error(Int_t line,Int_t i, Float_t t){drifttime1Err[line*10+i]=t;}
    void setDriftTime2Error(Int_t line,Int_t i, Float_t t){drifttime2Err[line*10+i]=t;}

    Float_t getDriftTime1(Int_t line,Int_t i){return drifttime1[line*10+i];}
    Float_t getDriftTime2(Int_t line,Int_t i){return drifttime2[line*10+i];}
    Float_t getDriftTime1Error(Int_t line,Int_t i){return drifttime1Err[line*10+i];}
    Float_t getDriftTime2Error(Int_t line,Int_t i){return drifttime2Err[line*10+i];}

    void fillTime1(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDriftTime1(p0,0,p1);
	setDriftTime1(p0,1,p2);
	setDriftTime1(p0,2,p3);
	setDriftTime1(p0,3,p4);
	setDriftTime1(p0,4,p5);
	setDriftTime1(p0,5,p6);
	setDriftTime1(p0,6,p7);
	setDriftTime1(p0,7,p8);
	setDriftTime1(p0,8,p9);
	setDriftTime1(p0,9,p10);

    }
    void fillTime2(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDriftTime2(p0,0,p1);
	setDriftTime2(p0,1,p2);
	setDriftTime2(p0,2,p3);
	setDriftTime2(p0,3,p4);
	setDriftTime2(p0,4,p5);
	setDriftTime2(p0,5,p6);
	setDriftTime2(p0,6,p7);
	setDriftTime2(p0,7,p8);
	setDriftTime2(p0,8,p9);
	setDriftTime2(p0,9,p10);

    }
   void fillTime1Error(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDriftTime1Error(p0,0,p1);
	setDriftTime1Error(p0,1,p2);
	setDriftTime1Error(p0,2,p3);
	setDriftTime1Error(p0,3,p4);
	setDriftTime1Error(p0,4,p5);
	setDriftTime1Error(p0,5,p6);
	setDriftTime1Error(p0,6,p7);
	setDriftTime1Error(p0,7,p8);
	setDriftTime1Error(p0,8,p9);
	setDriftTime1Error(p0,9,p10);

    }
    void fillTime2Error(Int_t p0,
	      Float_t p1,Float_t p2,
	      Float_t p3,Float_t p4,
	      Float_t p5,Float_t p6,
	      Float_t p7,Float_t p8,
	      Float_t p9,Float_t p10)
    {
	setDriftTime2Error(p0,0,p1);
	setDriftTime2Error(p0,1,p2);
	setDriftTime2Error(p0,2,p3);
	setDriftTime2Error(p0,3,p4);
	setDriftTime2Error(p0,4,p5);
	setDriftTime2Error(p0,5,p6);
	setDriftTime2Error(p0,6,p7);
	setDriftTime2Error(p0,7,p8);
	setDriftTime2Error(p0,8,p9);
	setDriftTime2Error(p0,9,p10);

    }
    void fillTime1(HMdcCal2ParAngleSim&);
    void fillTime2(HMdcCal2ParAngleSim&);
    void fillTime1Error(HMdcCal2ParAngleSim&);
    void fillTime2Error(HMdcCal2ParAngleSim&);

    void clear() {
	for(Int_t i=0;i<100;i++)
	{
	    drifttime1[i]=0.;
            drifttime2[i]=0.;
            drifttime1Err[i]=0.;
            drifttime2Err[i]=0.;
	}
    }
    ClassDef(HMdcCal2ParAngleSim,1) // Angle level of the MDC calibration parameters
};

class HMdcCal2ParModSim : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcCal2ParAngleSim
public:
    HMdcCal2ParModSim(Int_t sec = 0, Int_t mod = 0 , Int_t angle = 18);
    ~HMdcCal2ParModSim();
    HMdcCal2ParAngleSim& operator[](Int_t i) {
      return *static_cast<HMdcCal2ParAngleSim*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCal2ParModSim,1) // Module level of the MDC calibration parameters
};


class HMdcCal2ParSecSim : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdcCal2ParModSim
public:
    HMdcCal2ParSecSim(Int_t sec = 0, Int_t mod = 4);
    ~HMdcCal2ParSecSim();
    HMdcCal2ParModSim& operator[](Int_t i) {
      return *static_cast<HMdcCal2ParModSim*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCal2ParSecSim,1) // Sector level of the MDC calibration parameters
};


class HMdcCal2ParSim : public HParSet {
protected:
    TObjArray* array;      // array of pointers of type HMdcCal2ParSecSim
    HMdcDetector *fMdc;
    TString comment;
    Int_t sector;
    Int_t module;
    Float_t angleDeg;
    Float_t time;
    Float_t time2;
    Float_t finaltime;
    Int_t angleStep;
    Float_t distance;
    Int_t linecounter;
    Int_t linecounterwrite;
    Float_t slopeOutside;
    Int_t type;
    Int_t dmin;
    Int_t dmax;
    Float_t scaleError[4]; //! scaler for error of time1 per module type
    Float_t constError[4]; //! const error of time1 per module type
    Bool_t useConstErr;    //! switch for using const errors
    Float_t constVD[4];    //! const Driftvelocity VD[mu/ns] for simple xt-correlation (only in combination with constErr)
    Bool_t useConstVD;     //! switch for using const VD
public:
    HMdcCal2ParSim(const Char_t* name="MdcCal2ParSim",
                   const Char_t* title=
                       "cal2 calibration parameters for Mdc [distance->time]",
                   const Char_t* context="MdcCal2ParSimProduction",
                   Int_t n=6);
    ~HMdcCal2ParSim();
    HMdcCal2ParSecSim& operator[](Int_t i) {
        return *static_cast<HMdcCal2ParSecSim*>((*array)[i]);
    }
    void setContainerComment(TString mycomment){comment=mycomment;}
    TString getContainerComment(){return comment;}
    void printContainerComment(){printf("%s\n",comment.Data());}
    Int_t getSize() {return array->GetEntries();}
    Int_t getNumberOfLines() {return 40;}
    Int_t getNumberOfBins()  {return 100;}
    Bool_t init(HParIo*, Int_t*);
    Int_t write(HParIo*);
    void readline(const Char_t*, Int_t*);
    void putAsciiHeader(TString&);
    Bool_t writeline(Char_t*, Int_t, Int_t, Int_t);
    Double_t calcTime(Int_t ,Int_t ,Double_t,Double_t);
    Double_t calcTimeErr(Int_t ,Int_t ,Double_t,Double_t);
    void calcTime(Int_t ,Int_t ,Double_t,Double_t,Double_t*,Double_t*);
    Float_t calcTime(Int_t ,Int_t ,Float_t,Float_t);
    Float_t calcTimeErr(Int_t ,Int_t ,Float_t,Float_t);
    Float_t calcTime2(Int_t ,Int_t ,Float_t,Float_t);
    Float_t calcTime2Err(Int_t ,Int_t ,Float_t,Float_t);
    Float_t calcDriftVelocity(Int_t,Int_t,Float_t,Float_t);
    Float_t calcResolution(Int_t,Int_t,Float_t,Float_t);
    void calcTimeDigitizer (Int_t ,Int_t ,Float_t,Float_t,Float_t*,Float_t*);
    void calcTime2Digitizer(Int_t ,Int_t ,Float_t,Float_t,Float_t*,Float_t*);
    void transformToDistance(Int_t,Int_t,Int_t,Float_t,HMdcCal2Par*);
    void plot(Int_t,Int_t);
    void plot2D(Int_t,Int_t,Int_t);
    Float_t getSlopeOutside(){return slopeOutside;}
    void setScalerTime1Err(Float_t m0=0,Float_t m1=0,Float_t m2=0,Float_t m3=0)
    {
	scaleError[0]=m0;
	scaleError[1]=m1;
	scaleError[2]=m2;
	scaleError[3]=m3;
    }
    void setConstantTime1Err(Float_t m0=0,Float_t m1=0,Float_t m2=0,Float_t m3=0)
    {
	useConstErr=kTRUE;
	constError[0]=m0;
	constError[1]=m1;
	constError[2]=m2;
	constError[3]=m3;
    }
    void setConstantVD(Float_t m0=40 ,Float_t m1=40 ,Float_t m2=40 ,Float_t m3=40,
                       Float_t m0E=2,Float_t m1E=2,Float_t m2E=2,Float_t m3E=2
		      )
    {
        // constant drift velocity [cm/ns] per module, const drift time error [ns] per module
	useConstVD=kTRUE;
	constVD[0]=m0;
	constVD[1]=m1;
	constVD[2]=m2;
	constVD[3]=m3;

	useConstErr=kTRUE;
	constError[0]=m0E;
	constError[1]=m1E;
	constError[2]=m2E;
	constError[3]=m3E;
    }
    void clear();
    void printParam();
    ClassDef(HMdcCal2ParSim,2) // Container for the MDC calibration parameters
};

#endif  /*!HMDCCAL2PARSIM_H*/

