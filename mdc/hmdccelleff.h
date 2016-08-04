#ifndef HMDCCELLEFF_H
#define HMDCCELLEFF_H

#include "TObject.h"
#include "TObjArray.h"
#include "hparset.h"
#include "TString.h"

class HMdcDetector;

class HMdcCellEffAngle : public TObject {
protected:
    Float_t maxR;  // maximum distance from sense wire
    Float_t slope; // loss of charge per percent of required maximum signal
public:

    HMdcCellEffAngle() { clear(); }
    ~HMdcCellEffAngle() {;}
    void setFunctionValues(Float_t r, Float_t s){maxR=r;slope=s;}
    Float_t  getMaxR(void) {return maxR;}
    Float_t  getSlope(void){return slope;}
    void fill(HMdcCellEffAngle &r)
    {
	maxR=r.maxR;
	slope=r.slope;
    }
    void clear() {
	maxR=0;
        slope=0;
    }
    void print(void) {
	printf("maxR: %7.5f, slope: %7.5f\n",
	   maxR,slope);
    }
    ClassDef(HMdcCellEffAngle,1) // Angle level of the MDC cell efficiency parameters
};

class HMdcCellEffMod : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcCellEffAngle
public:
    HMdcCellEffMod(Int_t sec = 0, Int_t mod = 0 , Int_t angle = 18);
    ~HMdcCellEffMod();
    HMdcCellEffAngle& operator[](Int_t i) {
      return *static_cast<HMdcCellEffAngle*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcCellEffMod,1) // Module level of the MDC calibration parameters
};

class HMdcCellEff : public HParSet {
protected:
    TObjArray* array;      // array of pointers of type HMdcCellEffSec
    HMdcDetector *fMdc;
    TString comment;
public:
    HMdcCellEff(const Char_t* name="MdcCellEff",
                const Char_t* title="cell efficiency parameters for MdcDigitizer",
                const Char_t* context="MdcCellEffProduction",
                Int_t n=4);
    ~HMdcCellEff();
    HMdcCellEffMod& operator[](Int_t i) {
        return *static_cast<HMdcCellEffMod*>((*array)[i]);
    }
    void setContainerComment(TString mycomment){comment=mycomment;}
    TString getContainerComment(){return comment;}
    void printContainerComment(){printf("%s\n",comment.Data());}
    Int_t getSize() {return array->GetEntries();}
    Bool_t init(HParIo*, Int_t*);
    Int_t write(HParIo*);
    void readline(const Char_t*, Int_t*);
    void putAsciiHeader(TString&);
    Bool_t writeline(Char_t*,Int_t, Int_t);
    Int_t calcEfficiency(Int_t m, Float_t r, Float_t a, Float_t l);
    Float_t calcEffval(Int_t m, Float_t r, Float_t a, Float_t l);

    void clear();

    void printParam(void) {
	printf ("HMdcCellEff:\n");
	for (Int_t i=0;i<getSize();i++) {
	    HMdcCellEffMod &mod = (*this)[i];
	    for (Int_t a=0; a<mod.getSize(); a++) {
		HMdcCellEffAngle &angle = mod[a];
		printf("%i %2i  ",i,a);
		angle.print();
	    }
	}
    }

    ClassDef(HMdcCellEff,1) // Container for the MDC cell efficiency parameters
};

#endif  /*!HMDCCELLEFF_H*/

