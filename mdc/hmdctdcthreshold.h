#ifndef HMDCTDCTHRESHOLD_H
#define HMDCTDCTHRESHOLD_H

#include "TObject.h"
#include "TObjArray.h"
#include "TString.h"
#include "hparset.h"

class HMdcRawStruct;

class HMdcTdcThresholdDbo : public TObject {
protected:
    Int_t threshold;   // threshold of the two tdc of one DBO
public:
    HMdcTdcThresholdDbo() : threshold(-1){;}
    ~HMdcTdcThresholdDbo() {;}
    Int_t getThreshold()             {return threshold;}
    void setThreshold(Int_t thresh)  {threshold = thresh;}
    void fill(HMdcTdcThresholdDbo&);
    void clear() {
      threshold = -1;
    }
    ClassDef(HMdcTdcThresholdDbo,1) // Tdc level of the MDC calibration parameters
};


class HMdcTdcThresholdMbo : public TNamed {
protected:
    TObjArray *array; // array of pointers of type HMdcTdcThresholdDbo
public:
    HMdcTdcThresholdMbo(Int_t dbo = 6,const Text_t* name="");
    ~HMdcTdcThresholdMbo();
    HMdcTdcThresholdDbo& operator[](Int_t i) {
      return *static_cast<HMdcTdcThresholdDbo*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcTdcThresholdMbo,1) // Mbo level of the MDC calibration parameters
};

class HMdcTdcThresholdMod : public TObject {
protected:
    TObjArray *array;   // array of pointers of type HMdcTdcThresholdMbo
public:
    HMdcTdcThresholdMod(Int_t mbo = 16);
    ~HMdcTdcThresholdMod();
    HMdcTdcThresholdMbo& operator[](Int_t i) {
      return *static_cast<HMdcTdcThresholdMbo*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    void createMbo(Int_t, Int_t, const Char_t*);
    ClassDef(HMdcTdcThresholdMod,1) // Module level of the MDC calibration parameters
};

class HMdcTdcThresholdSec : public TObject {
protected:
    TObjArray* array;   // array of pointers of type HMdcTdcThresholdMod

public:
    HMdcTdcThresholdSec(Int_t mod = 4);
    ~HMdcTdcThresholdSec();
    HMdcTdcThresholdMod& operator[](Int_t i) {
      return *static_cast<HMdcTdcThresholdMod*>((*array)[i]);
    }
    Int_t getSize() {return array->GetEntries();}
    ClassDef(HMdcTdcThresholdSec,1) // Sector level of the MDC calibration parameters
};

class HMdcTdcThreshold : public HParSet {
protected:
    TObjArray* array;          // array of pointers of type HMdcTdcThresholdSec
    Int_t oraVersion;          // parameter version in Oracle
    HMdcRawStruct* pRawStruct; //! pointer to parameter container MdcRawStruct
    Int_t status;              //! Status of parameters (usable for DAQ?)

public:
    HMdcTdcThreshold(const Char_t* name="MdcTdcThreshold",
                  const Char_t* title="threshold parameters for Mdc",
                  const Char_t* context="MdcTdcThresholdProduction",
                  Int_t n=6);
    ~HMdcTdcThreshold();
    HMdcTdcThresholdSec& operator[](Int_t i) {
        return *static_cast<HMdcTdcThresholdSec*>((*array)[i]);
    }
    void fill(Int_t sector, Int_t module, Int_t mbo, Int_t dbo,Int_t th){
        ((HMdcTdcThresholdSec *)((*array)[0]))[sector][module][mbo][dbo].setThreshold(th);
    }
    Int_t getSize() {return array->GetEntries();}
    Bool_t init(HParIo*, Int_t*);
    Int_t write(HParIo*);
    void readline(const Char_t*, Int_t*);
    void readlinefromfile(const Char_t*);
    void writelinetofile(Char_t* buf, Int_t sec, Int_t mod, Int_t mbo, Int_t dbo);
    void addvalue(Int_t);
    void putAsciiHeader(TString&);
    Bool_t writeline(Char_t*, Int_t, Int_t, Int_t, Int_t);
    void clear();
    void printParam();
    void setOraVersion(Int_t v) { oraVersion=v;}
    void setStatus(Int_t stat){status = stat;};
    Int_t getStatus(void){return status;};
    Int_t getOraVersion() { return oraVersion; }
    ClassDef(HMdcTdcThreshold,4) // Container for the MDC threshold parameters on raw level
};

#endif  /*!HMDCTDCTHRESHOLD_H*/

