// File: $RCSfile: hmdcslope.h,v $
//
// Version: $Revision: 1.8 $
// Modified by $Author: halo $ on $Date: 2009-07-15 11:42:10 $
////////////////////////////////////////////////////////////

#ifndef HMDSLOPE_H
#define HMDSLOPE_H

#include "hreconstructor.h"
#include "TH1.h"
#include "TH2.h"
#include "TNtuple.h"

typedef Int_t   MyInt;
typedef Float_t MyFloat;

class TFile;
class HCategory;
class HIterator;
class HMdcCalPar;
class HMdcCalParRaw;

typedef MyFloat MyFloatField [6][4][16][96];
typedef MyInt   MyIntField   [6][4][16][96];

typedef MyInt   MboSlopeTrendIntField[16][96][5000]; // slope binning 1e-4
typedef MyInt   MboErrorTrendIntField[16][96][600];  // error binning 5e-7

class HMdcSlope : public HReconstructor {

protected:
    HCategory     *rawCat;     //! pointer to the raw data
    HCategory     *slopeCat;   //! pointer to the calibration's slope
    HIterator     *iter;       //! iterator on raw data
    HMdcCalParRaw *calparraw;  //! calibration parameters on raw level
    
    Char_t *fNameRoot;         //! file name of root output file
    Char_t *fNameNtuple;       //! file name of ntuple output file

    MyFloatField *avgSlope;
    MyFloatField *avgSlopeErr;
    MyIntField   *nEvt;

    //    MboSlopeTrendIntField (*slopeTrend)[6][4];
    MboErrorTrendIntField *errorTrend[6][4];

    Int_t slopeTrend[6][4][16][96][5000];

    Bool_t trendCalculationActive;
    Int_t  calibrationAlgorithm;
    Int_t  nTimeValues;
    Int_t  debugActive;   //! 0x00: off, 0x01: general, 0x02: init, 0x04: finalize, 0x08: execute, 0x10: calc, 0x20: fitHistogramm
    Int_t  fitHistMethod;
 
    Bool_t linRegActive;
    Bool_t histFitActive;
    Bool_t graphFitActive;
    Int_t  binError;

    Bool_t selectorActive;
    Int_t  mbSelector;
    Int_t  tdcSelector;

    Bool_t createNtupleActive;
    TFile *ntupleOutputFile;
    TNtuple *ntuple;

    TFile *file;

    TH1F *hSlope;
    TH1F *hTime;
    
    enum calibrationMethod {HMDC_SLOPE_LIN_REG_6   = 1, 
			    HMDC_SLOPE_HIST_FIT_5  = 2,
			    HMDC_SLOPE_GRAPH_FIT_5 = 3,
			    HMDC_SLOPE_LIN_REG_5   = 4 };

public:
    HMdcSlope();
    HMdcSlope(const Text_t* name,const Text_t* title);
    ~HMdcSlope();

    Bool_t init();
    Bool_t finalize();
    Int_t  execute();
    
    Int_t calcBinNr(Double_t, Double_t, Double_t, Int_t);
    
    void setOutputRoot (Char_t*);
    void setOutputNtuple(Char_t*);

    void selectCalibrationAlgorithm(Int_t);
    void selectHistFitMethod(Int_t);
    void setBinError(Int_t);
    void setCreateNtupleActive(Bool_t);
    void setDebugActive(Int_t);
    void setGraphFitActive(Bool_t);
    void setHistFitActive(Bool_t);
    void setLinRegActive(Bool_t);
    void setMbSelector(Int_t);
    void setnTimeValues(Int_t);
    void setTrendCalculationActive(Bool_t);
    void setSelectorActive(Bool_t);
    void setTdcSelector(Int_t);
    
protected:
    void calc     (Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t, Float_t*, Float_t*);
    void calc5    (Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t, Float_t*, Float_t*);
    void calcCHI  (Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t, Float_t*, Float_t*);
    void calcGraph(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t, Float_t*, Float_t*);
    void calcSlope(Int_t, Int_t, Int_t, Int_t, Int_t, Int_t, Float_t, Float_t*, Float_t*, Float_t*, Int_t, Int_t);
    void fitHistogramm(Float_t*, Float_t*, Int_t, Int_t, Int_t, Int_t, Int_t);
    void fitHistogrammMeanCondition(Float_t*, Float_t*, Int_t, Int_t, Int_t, Int_t);
    void fitHistogrammMaxCondition(Float_t*, Float_t*, Int_t, Int_t, Int_t, Int_t);

public: // This has to be placed at the end (--> root docs)
    ClassDef(HMdcSlope, 0) //! Class calculates slopes and errors for TDC channels from internal calibration.
};

#endif /* !HMDCSLOPE_H */









