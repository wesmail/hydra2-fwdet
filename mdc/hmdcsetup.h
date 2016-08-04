#ifndef HMDCSETUP_H
#define HMDCSETUP_H
#include "TNamed.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "TArrayF.h"
#include "hparcond.h"

class HParamList;
class HMdcDigitizer;
class HMdcCalibrater1;
class HMdcSetup;

class HMdcDigitizerSetup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t tdcModeDigi;        // leading and trailing edge or 2 times leading edge
    Int_t NtupleDigi;         // write NTuple
    Int_t useTofDigi;         // add tof to drift time in cal1
    Int_t useErrorDigi;       // add error to drift time in cal1
    Int_t useWireOffsetDigi;  // add signal time on wire to drift time in cal1
    Int_t useWireStatDigi;    // use wirestat container for dead wires/efficiency
    Int_t embeddingModeDigi;  // keep geant hits or merge realistic
    Int_t useTimeCutsDigi;    // use/d not use time cuts

    Int_t  offsetsOnDigi;     // switch on/off use offset
    Int_t  cellEffOnDigi;     // switch on/off use cellEff
    Int_t  noiseOnDigi;       // switch on/off use noise

    TArrayF offsetsDigi;      // offset in ns to be substracted from (drift time + tof) per plane
    TArrayF cellEffDigi;      // Efficiency level of cell per plane
    Int_t noiseModeDigi;      // switch the different noise modes
    TArrayF noiseLevelDigi;   // noise level per plane
    TArrayI noiseRangeLoDigi; // lower boundery for noise simulation per plane
    TArrayI noiseRangeHiDigi; // upper boundery for noise simulation per plane

public:
    HMdcDigitizerSetup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdcDigitizerSetup();
    void   setTdcModeDigi(Int_t mode)         {tdcModeDigi=mode;}
    void   setNtupleDigi(Bool_t ntuple)       {NtupleDigi=(Int_t)ntuple;}
    void   setUseTofDigi(Bool_t tof)          {useTofDigi=(Int_t)tof;}
    void   setUseErrorDigi(Bool_t err)        {useErrorDigi=(Int_t)err;}
    void   setUseWireOffsetDigi(Bool_t off)   {useWireOffsetDigi=(Int_t)off;}
    void   setUseWireStatDigi(Bool_t stat)    {useWireStatDigi=(Int_t)stat;}
    void   setUseTimeCutsDigi(Bool_t stat)    {useTimeCutsDigi=(Int_t)stat;}
    void   setEmbeddingModeDigi(Int_t mode)   {embeddingModeDigi=mode;}
    void   setNoiseModeDigi(Int_t noise)      {noiseModeDigi=noise;}
    void   setNoiseOffDigi(Int_t noiseoff)    {noiseOnDigi=noiseoff;}
    void   setOffsetsOffDigi(Int_t offsetsoff){offsetsOnDigi=offsetsoff;}
    void   setCellEffOffDigi(Int_t effoff)    {cellEffOnDigi=effoff;}
    void   setNoiseLevelDigi(Float_t l0,Float_t l1,Float_t l2,Float_t l3)
    {
	noiseLevelDigi[0]=l0;
        noiseLevelDigi[1]=l1;
        noiseLevelDigi[2]=l2;
        noiseLevelDigi[3]=l3;
    }
    void   setOffsetsDigi(Float_t o0,Float_t o1,Float_t o2,Float_t o3)
    {
	offsetsDigi[0]=o0;
        offsetsDigi[1]=o1;
        offsetsDigi[2]=o2;
        offsetsDigi[3]=o3;
    }
    void   setCellEffDigi(Float_t e0,Float_t e1,Float_t e2,Float_t e3)
    {
	cellEffDigi[0]=e0;
        cellEffDigi[1]=e1;
        cellEffDigi[2]=e2;
        cellEffDigi[3]=e3;
    }
    void   setNoiseRangeDigi(Int_t rLo0,Int_t rLo1,Int_t rLo2,Int_t rLo3,
			     Int_t rHi0,Int_t rHi1,Int_t rHi2,Int_t rHi3)
    {
	noiseRangeLoDigi[0]=rLo0;
        noiseRangeLoDigi[1]=rLo1;
        noiseRangeLoDigi[2]=rLo2;
        noiseRangeLoDigi[3]=rLo3;
	noiseRangeHiDigi[0]=rHi0;
        noiseRangeHiDigi[1]=rHi1;
        noiseRangeHiDigi[2]=rHi2;
        noiseRangeHiDigi[3]=rHi3;
    }
    Int_t  getTdcModeDigi()      {return tdcModeDigi;}
    Bool_t getNtupleDigi()       {return (Bool_t)NtupleDigi;}
    Bool_t getUseTofDigi()       {return (Bool_t)useTofDigi;}
    Bool_t getUseErrorDigi()     {return (Bool_t)useErrorDigi;}
    Bool_t getUseWireOffsetDigi(){return (Bool_t)useWireOffsetDigi;}
    Bool_t getUseWireStatDigi()  {return (Bool_t)useWireStatDigi;}
    Bool_t getUseTimeCutsDigi()  {return (Bool_t)useTimeCutsDigi;}
    Int_t  getEmbeddingModeDigi(){return embeddingModeDigi;}
    Int_t  getNoiseModeDigi()    {return noiseModeDigi;}
    Int_t  getNoiseOffDigi()     {return noiseOnDigi;}
    Int_t  getOffsetsOffDigi()   {return offsetsOnDigi;}
    Int_t  getCellEffOffDigi()   {return cellEffOnDigi;}

    void   getNoiseLevelDigi(Float_t* l0,Float_t* l1,Float_t* l2,Float_t* l3)
    {
	*l0=noiseLevelDigi[0];
        *l1=noiseLevelDigi[1];
        *l2=noiseLevelDigi[2];
        *l3=noiseLevelDigi[3];
    }
    TArrayF& getNoiseLevelDigi()
    {
	return  noiseLevelDigi;
    }
    void   getOffsetsDigi(Float_t* o0,Float_t* o1,Float_t* o2,Float_t* o3)
    {
       *o0=offsetsDigi[0];
       *o1=offsetsDigi[1];
       *o2=offsetsDigi[2];
       *o3=offsetsDigi[3];
    }
    TArrayF& getOffsetsDigi()
    {
       return offsetsDigi;
    }
    void   getCellEffDigi(Float_t* e0,Float_t* e1,Float_t* e2,Float_t* e3)
    {
       *e0=cellEffDigi[0];
       *e1=cellEffDigi[1];
       *e2=cellEffDigi[2];
       *e3=cellEffDigi[3];
    }
    TArrayF& getCellEffDigi()
    {
       return cellEffDigi;
    }
    void   getNoiseRangeDigi(Int_t* rLo0,Int_t* rLo1,Int_t* rLo2,Int_t* rLo3,
			   Int_t* rHi0,Int_t* rHi1,Int_t* rHi2,Int_t* rHi3)
    {
	*rLo0=noiseRangeLoDigi[0];
	*rLo1=noiseRangeLoDigi[1];
	*rLo2=noiseRangeLoDigi[2];
	*rLo3=noiseRangeLoDigi[3];
	*rHi0=noiseRangeHiDigi[0];
	*rHi1=noiseRangeHiDigi[1];
	*rHi2=noiseRangeHiDigi[2];
	*rHi3=noiseRangeHiDigi[3];
    }
    TArrayI& getNoiseRangeLoDigi()
    {
	return noiseRangeLoDigi;
    }
    TArrayI& getNoiseRangeHiDigi()
    {
	return noiseRangeHiDigi;
    }
    void   clear();
    Bool_t check();
    void   printParam();
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   setupMdcDigitizer(HMdcDigitizer*);
    void   copySet(HMdcDigitizerSetup* par);
    Bool_t isEqual(HMdcDigitizerSetup* par);
    ClassDef(HMdcDigitizerSetup,1) // Container for the MDC Digitizer parameters
};

class HMdcCalibrater1Setup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t ModeFlagCal1;   // switch the different modes of the calibrater1
    Int_t TimeCutFlagCal1;// switch on/off time cuts
public:
    HMdcCalibrater1Setup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdcCalibrater1Setup();
    void   setModeFlagCal1(Int_t flag)  {ModeFlagCal1=flag;}
    void   setTimeCutFlagCal1(Int_t cut){TimeCutFlagCal1=cut;}
    void   setMdcCalibrater1Set(Int_t flag,Int_t cut)
    {
	   setModeFlagCal1(flag);
           setTimeCutFlagCal1(cut);
    }
    Int_t  getModeFlagCal1()            {return ModeFlagCal1;}
    Int_t  getTimeCutFlagCal1()         {return TimeCutFlagCal1;}
    void   getMdcCalibrater1Set(Int_t* mode,Int_t* cut)
    {
	*mode=getModeFlagCal1();
	*cut =getTimeCutFlagCal1();
    }
    void   clear();
    Bool_t check();
    void   printParam(void);
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   setupMdcCalibrater1(HMdcCalibrater1*,Int_t merge=0);
    void   copySet(HMdcCalibrater1Setup* par);
    Bool_t isEqual(HMdcCalibrater1Setup* par);
    ClassDef(HMdcCalibrater1Setup,1) // Container for the MDC Calibrater1 parameters
};

class HMdcTrackFinderSetup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t isCoilOff;       // magnet on/off
    Int_t typeClustFinder; // segment or mdc
    TArrayI nLayers;       // number of layers per module [6][4]
    TArrayI nLevel;        // level of hit finding
public:
    HMdcTrackFinderSetup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdcTrackFinderSetup();
    void   setIsCoilOff(Bool_t field)    {isCoilOff=(Int_t)field;}
    void   setTypeClustFinder(Int_t type){typeClustFinder=type;}
    Bool_t getIsCoilOff()                {return (Bool_t)isCoilOff;}
    void   setNLayers(Int_t* myLayers)
    {
	for(Int_t s=0;s<6;s++){
	    for(Int_t m=0;m<4;m++){
               nLayers[s*4+m]=myLayers[s*4+m];
	    }
	}
    }
    void   setNLevel(Int_t* L)
    {
	for(Int_t i=0;i<4;i++)
	{
           nLevel[i]=L[i];
	}
    }
    void   setMdcTrackFinderSet(Bool_t field,Int_t type,Int_t* myLayers,Int_t* L)
    {
	setIsCoilOff(field);
	setTypeClustFinder(type);
	setNLayers(myLayers);
	setNLevel(L);
    }
    Int_t  getTypeClustFinder()          {return typeClustFinder;}
    void   getNLayers(Int_t* myLayers)
    {
	for(Int_t s=0;s<6;s++){
	    for(Int_t m=0;m<4;m++){
               myLayers[s*4+m]=nLayers[s*4+m];
	    }
	}
    }
    void   getNLevel(Int_t* L)
    {
	for(Int_t i=0;i<4;i++)
	{
           L[i]=nLevel[i];
	}
    }
    void   getMdcTrackfinderSet(Bool_t* field,Int_t* type,Int_t* nlay)
    {
	*field=getIsCoilOff();
	*type =getTypeClustFinder();
               getNLayers(nlay);
    }
    void   clear();
    Bool_t check();
    void   printParam(void);
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   setupMdcTrackFinder();
    void   copySet(HMdcTrackFinderSetup* par);
    Bool_t isEqual(HMdcTrackFinderSetup* par);
    ClassDef(HMdcTrackFinderSetup,1) // Container for the MDC trackfinder parameters
};

class HMdcClusterToHitSetup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t mdcForSeg1;  // what to fill in segment1
    Int_t mdcForSeg2;  // what to fill in segment2
public:
    HMdcClusterToHitSetup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdcClusterToHitSetup();
    void   setMdcForSeg1(Int_t mdcseg1){mdcForSeg1=mdcseg1;}
    void   setMdcForSeg2(Int_t mdcseg2){mdcForSeg2=mdcseg2;}
    void   setMdcClusterToHitSet(Int_t mdcseg1,Int_t mdcseg2)
    {
	setMdcForSeg1(mdcseg1);
        setMdcForSeg2(mdcseg2);
    }
    Int_t  getMdcForSeg1()             {return mdcForSeg1;}
    Int_t  getMdcForSeg2()             {return mdcForSeg2;}
    void   getMdcClusterToHitSet(Int_t* mdcseg1,Int_t*mdcseg2)
    {
	*mdcseg1=getMdcForSeg1();
	*mdcseg2=getMdcForSeg2();
    }
    void   clear();
    Bool_t check();
    void   printParam(void);
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   setupMdcClusterToHit();
    void   copySet(HMdcClusterToHitSetup* par);
    Bool_t isEqual(HMdcClusterToHitSetup* par);
    ClassDef(HMdcClusterToHitSetup,1) // Container for the MDC clustertoseg parameters
};

class HMdc12FitSetup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t fitauthor;   // different fit algorhythms
    Int_t fitVersion;  // use polynom,cal2parsim for distance/time
    Int_t fitIndep;    // segment,mdc
    Int_t fitNTuple;   // NTuple
    Int_t fitPrint;    // print
public:
    HMdc12FitSetup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdc12FitSetup();
    void   setFitAuthor(Int_t author){fitauthor=author;}
    void   setFitVersion(Int_t vers) {fitVersion=vers;}
    void   setFitIndep(Int_t indep)  {fitIndep=indep;}
    void   setFitNTuple(Bool_t tuple){fitNTuple=(Int_t)tuple;}
    void   setFitPrint(Bool_t print) {fitPrint=(Int_t)print;}

    void   setMdc12FitSet(Int_t author,Int_t vers,Int_t indep,Bool_t tuple,Bool_t print)
    {
           setFitAuthor (author);
           setFitVersion(vers);
           setFitIndep  (indep);
           setFitNTuple (tuple);
           setFitPrint  (print);
    }

    Int_t  getFitAuthor()            {return fitauthor;}
    Int_t  getFitVersion()           {return fitVersion;}
    Int_t  getFitIndep()             {return fitIndep;}
    Bool_t getFitNTuple()            {return (Bool_t)fitNTuple;}
    Bool_t getFitPrint()             {return (Bool_t)fitPrint;}
    void   getMdc12FitSet(Int_t* author,Int_t* vers,Int_t* indep,Bool_t* tuple,Bool_t* print)
    {
	*author=getFitAuthor();
	*vers  =getFitVersion();
	*indep =getFitIndep();
	*tuple =getFitNTuple();
	*print =getFitPrint();
    }
    void   clear();
    Bool_t check();
    void   printParam(void);
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   setupMdc12Fit();
    void   copySet(HMdc12FitSetup* par);
    Bool_t isEqual(HMdc12FitSetup* par);
    ClassDef(HMdc12FitSetup,1) // Container for the MDC fitter parameters
};
class HMdcCommonSetup : public TNamed {
protected:
    HMdcSetup* setup;
    Int_t isSimulation;   // sim,real data ,merge
    Int_t analysisLevel;  // raw,cal1,cal2,hit,fit
    Int_t tracker;        // dubna, santiago
public:
    HMdcCommonSetup(const Char_t* name="",const Char_t* title="",HMdcSetup* set=0);
    ~HMdcCommonSetup();
    void   setIsSimulation(Int_t sim)   {isSimulation=sim;}
    void   setAnalysisLevel(Int_t level){analysisLevel=level;}
    void   setTracker(Int_t track)      {tracker=track;}
    void   setMdcCommonSet(Int_t sim,Int_t level,Int_t track)
    {
           setIsSimulation(sim);
           setAnalysisLevel(level);
           setTracker(track);
    }
    Int_t  getIsSimulation()            {return isSimulation;}
    Int_t  getAnalysisLevel()           {return analysisLevel;}
    Int_t  getTracker()                 {return tracker;}
    void   getMdcCommonSet(Int_t* sim,Int_t* level,Int_t* track)
    {
	*sim  =getIsSimulation();
	*level=getAnalysisLevel();
        *track=getTracker();
    }
    void   clear();
    Bool_t check();
    void   printParam(void);
    Bool_t getParams(HParamList*);
    void   putParams(HParamList*);
    void   copySet(HMdcCommonSetup* par);
    Bool_t isEqual(HMdcCommonSetup* par);
    ClassDef(HMdcCommonSetup,1) // Container for the MDC common parameters
};

class HMdcSetup : public HParCond {
protected:
    HMdcDigitizerSetup*    digiset;
    HMdcCalibrater1Setup*  cal1set;
    HMdcTrackFinderSetup*  trackfinderset;
    HMdcClusterToHitSetup* clustertohitset;
    HMdc12FitSetup*        fitterset;
    HMdcCommonSetup*       commonset;

public:
    HMdcSetup(const Char_t* name   ="MdcSetup",
	      const Char_t* title  ="parameters for MdcTaskSet",
	      const Char_t* context="MdcSetupProduction");
    ~HMdcSetup();

    HMdcDigitizerSetup*    getMdcDigitizerSet()   {return digiset;}
    HMdcCalibrater1Setup*  getMdcCalibrater1Set() {return cal1set;}
    HMdcTrackFinderSetup*  getMdcTrackFinderSet() {return trackfinderset;}
    HMdcClusterToHitSetup* getMdcClusterToHitSet(){return clustertohitset;}
    HMdc12FitSetup*        getMdc12FitSet()       {return fitterset;}
    HMdcCommonSetup*       getMdcCommonSet()      {return commonset;}
    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);
    void   clear();
    Bool_t check();
    void   printParam(void);
    void   copySet(HMdcSetup* par);
    Bool_t isEqual(HMdcSetup* par);
    ClassDef(HMdcSetup,1) // Container for the MDC taskset parameters
};

#endif  /*!HMDCSETUP_H*/

