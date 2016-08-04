#ifndef HMDCMILLE_H
#define HMDCMILLE_H

#include "hmdc12fit.h"
#include "hgeomtransform.h"
#include <TString.h>

class Mille;
class HMdcSizesCells;
class HMdcSizesCellsMod;
class HMdcSizesCellsLayer;
class HMdcWireData;
class HMdcTrackParam;
class HMdcTrackFitInOut;
class HMdcDetector;
class HMdcGetContainers;

#define ASIZE  40

class HMdcMille : public HMdc12Fit {
  private:
    Mille*               mille;
    Int_t                shiftType;            // =0 shift in sec.sys; 
                                               // =1 shift in mod.sys;
                                               // =2 shift in lab.sys;
    Bool_t               fixFitTOffset;        // For deriv.assume fixed TOffset
    Double_t             parSteps[ASIZE];      // [glob.param.] - step for der
    Double_t             derNorm[ASIZE];       // [glob.par.] = 1./(2.*parSteps)
    UInt_t               nTracks;              // Tracks number sended to Mille
    UInt_t               nWiresTot;            // Wires number sended to Mille
    UInt_t               nLayersTot;           // Layers number sended to Mille
    Bool_t               doCopyGeomFile;
    Bool_t               doCopySumShFile;
    Bool_t               doCopyResFile;
    Bool_t               doCopyLogFile;
    Bool_t               doCopyHisFile;
    Bool_t               useMdcShParOnly;      //
    Int_t                beamTimeId;           // beam time ident. (<=9)
    HMdcGetContainers*   pGetCont;
    HMdcDetector*        pMdcDet;
    TString              milleFileName;        // millepede binary file name
    Int_t                nBinaryFiles;         // number of files
    UInt_t               milleFileSize;        // size of current binary file
    Bool_t               cellThicknFree;       // =kTRUE - cell thickness is free param.
    Bool_t               useSector[6];
    UInt_t               nClustersCut;         // Number of clusters in sec. must be <= nClustersCut
    Int_t                nWiresCut[6];         // Max.number of fired wires in each sector.
    UChar_t              filteringFlag;        // 0 - no filtering, 1 - filtering on, 
                                               // 2 - no track fit, filtering only.

    // Cuts:
    Int_t                nWiresMin;            // min. num. of wires per track
    Int_t                nMods[6];             // num.of mod. in sector
    Int_t                nWiresMinTr;          // Cut for current track
    Double_t             thetaCut;             // Select clus.theta>thetaCut

    // Lookup table:
    HMdcSizesCellsMod*   pSCMod[24];           // [sec*4+mod]
    HMdcSizesCellsLayer* pSCLayer[144];        // [sec*24+mod*6+layer]
    HGeomTransform       posShifts[ASIZE];     // [glob.param.]
    HGeomTransform       negShifts[ASIZE];     // [glob.param.]
    HGeomTransform       posLThSh[6];          // [lay] for cell thickness
    HGeomTransform       negLThSh[6];          // [lay] for cell thickness
    Double_t             derLThNorm[6];        // [lay] for cell thickness
    HGeomTransform       layPosSh[144][ASIZE]; // [layerIndex][param.+20*(LayPart-1)]
    HGeomTransform       layNegSh[144][ASIZE]; // [layerIndex][param.+20*(LayPart-1)]
    TString              shitsInfo[9];         // Descriptions of shifts
    UChar_t              nLayerParts[144];     // number of parts in layer
    Bool_t               isMdcInAlign[6][4];
    
    // Current track data:
    HMdcWireData*        wireData;             // Pointer to current wire date
    Int_t                sector;               // Address of wireData object
    Int_t                module;               // ...
    Int_t                layer;                // ...
    Int_t                iSec;                 //
    Int_t                iMod;                 // = sec*4+mod
    Int_t                iLay;                 // = sec*24+mod*6+layer
    Int_t                layerPart;            // =0 or 1 (first or second part of layer)
    Bool_t               doLayP2Align;         // =kTRUE - determine MdcLayerCorrPar
    HGeomTransform      *layPosShCurr;         // =layPosSh[iLay]
    HGeomTransform      *layNegShCurr;         // =layNegSh[iLay]
    HMdcTrackParam*      finalParam;           // Track parameters
    HGeomVector          p1;                   // Track line in
    HGeomVector          p2;                   // sector coor. system
    
    // Output arrays for Mille:
    float                locDer[4];            // Local(track) par.derivatives
    float                globDer[ASIZE];       // Global(alignment) par.deriv.
    int                  globLabel[ASIZE];     // Global(alignment) par.labels
    
    // Var. for geometry change.
    TString              pedeResFName;
    Int_t                iteration;            // Iteration number
    TString              stepIter;             // = "Step%i",Iteration
    TString              sumOfShiftsFName;     // File name with shifts sum ???
    TString              geomParFileName;      // File name with geom.parameters
    Bool_t               isGeomFileExist;      //
    Bool_t               isGeomChanged;        // = kFILSE if no param.was chan.
    Int_t                mdcMods[6][4];        // MDC setup
    Int_t                shiftSysMdc[6][4];    // [s][m]
    Int_t                nParMax;               // max. number of parameters (=9)
    Double_t             shiftsMdc[6][4][9];    // [s][m][par]
    Double_t             sigmaMdc[6][4][9];     // [s][m][par]
    Int_t                shiftSysLay[6][4][6]; // [s][m][l]
    Double_t             shiftsLay[6][4][6][9]; // [s][m][l][par]
    Double_t             sigmaLay[6][4][6][9];  // [s][m][l][par]
    
    // Var. for creating of pedeInFileName:
    Bool_t               createPedeInFile;     // kTRUE-create par.file for Pede
    TString              pedeInParFName;       // Name of parameters file
    Bool_t               mShFlag[6][4][9];     // [sec][mod][par]
    Bool_t               lShFlag[6][4][6][9];  // [sec][mod][lay][par]
    Bool_t               mFixFlag[6][4][9];    // [sec][mod][par]
    Bool_t               lFixFlag[6][4][6][9]; // [sec][mod][lay][par]
    Double_t             tofDerCorr[24][9];    // [sec*4+mod][par]
    
    Int_t                nHists;               // number of histograms
    Int_t                histInd[6];           // [sec] index of first sec.hist.
    
    // Pede task file:
    Bool_t               crPedeTaskFile;
    TString              pedeTaskFileName;     // pede task file name
    TString              method;
    Int_t                numOfIter;
    Float_t              accuracy;
    TString              mthDescr;
    Int_t                bandwidth;            // for Cholesky method only
    TString              parConstrainFile;

    Bool_t               printDebugFlag;
    Bool_t               doHists;
  public:
    HMdcMille(const Char_t *milleOutFName,const Char_t* geomPFName=0,
              Int_t bmTimeID=0,const Char_t* sumOfShFName=0);
    HMdcMille();      // Don't use it
    ~HMdcMille(void);
    
    Bool_t   init(void);
    Bool_t   reinit(void);
    Bool_t   finalize(void);
    Int_t    execute(void);
    
    void     setWiresMinNum(Int_t mw)             {nWiresMin = mw;}
    void     doShiftsInSecSys(void)               {shiftType = 0;}
    void     doShiftsInModSys(void)               {shiftType = 1;}
    void     doShiftsInLabSys(void)               {shiftType = 2;}
    void     setFixFitTOffsetFlag(Bool_t fl)      {fixFitTOffset = fl;}
    void     setMille(Mille* m)                   {mille = m;}
    void     setCopyGeomFile(Bool_t fl)           {doCopyGeomFile = fl;}
    void     setCopySumShFile(Bool_t fl)          {doCopySumShFile  = fl;}
    void     setCopyResFile(Bool_t fl)            {doCopyResFile = fl;}
    void     setyCopyLogFile(Bool_t fl)           {doCopyLogFile = fl;}
    void     setCopyHisFile(Bool_t fl)            {doCopyHisFile = fl;}
    void     setMdcShiftParOnly(Bool_t fl=kTRUE);
    void     setThetaCut(Double_t th)             {thetaCut = th;}
    int      packLabel(Int_t s,Int_t m,Int_t l,Int_t parn);
    static Bool_t unpackLabel(Int_t label,Int_t& btId,Int_t& sys,Int_t& sec,
                              Int_t& mod,Int_t& lay,Int_t& parn);
    Int_t    getIterationNumber(void)             {return iteration;}
    TString  getIterName(void)                    {return stepIter;}
    void     setPedeInFileName(const char* fname);
    void     fixFullMod(Int_t s,Int_t m);
    void     fixModPar(Int_t s,Int_t m,Int_t p);
    void     fixFullLay(Int_t s,Int_t m,Int_t l);
    void     fixLayPar(Int_t s,Int_t m,Int_t l,Int_t p);
    void     fixPar(Int_t s,Int_t m,Int_t l,Int_t p);
    void     doLayerPart2Align(Bool_t fl=kTRUE)   {doLayP2Align = fl;}
    void     useCellThicknessParam(void)          {cellThicknFree = kTRUE;}
    
    void     creatPedeTaskFile(const char* fileName="pedeTask.txt");
    void     useInversionMethod(      Int_t nit=3,Float_t acc=0.001);
    void     useBandCholeskyMethod(   Int_t nit=8,Float_t acc=0.01, Int_t bwidth=6);
    void     useCholeskyMethod(       Int_t nit=5,Float_t acc=0.001,Int_t bwidth=6);
    void     useSparseGMRESMethod(    Int_t nit=5,Float_t acc=0.01);
    void     useFullGMRESMethod(      Int_t nit=5,Float_t acc=0.01);
    void     useDiagonalizationMethod(Int_t nit=5,Float_t acc=0.01);
    void     setParConstrainFile(const char* file);
    void     setNoHistsFlag(void)                 {doHists = kFALSE;}
    void     setClusterMultCut(UInt_t cut)        {nClustersCut = cut;}
    void     noFiltering(void)                    {filteringFlag = 0;}
    void     doFiltering(void)                    {filteringFlag = 1;}
    void     doFilteringOnly(void)                {filteringFlag = 2;}
    void     setMaxNumWiresCut(Int_t* wc);
    void     setMaxNumWiresCut(Int_t cs1,Int_t cs2,Int_t cs3,Int_t cs4,Int_t cs5,Int_t cs6);

  private:
    void     setDef(void);
    void     setIMod(Int_t s,Int_t m)             {iMod = s*4 + m; iSec = s;}
    void     setILay(Int_t l)                     {iLay = iMod*6 + l;}
    void     setIModILay(Int_t s,Int_t m,Int_t l) {setIMod(s,m); setILay(l);}
    void     setIModILay(void);
    void     setShiftTransformation(void);
    void     setTrShiftInModSys(void);
    void     calcShiftInModSys(const HGeomTransform* mdcSys, const HGeomTransform* laySysMod,
                               const HGeomTransform& shift, HGeomTransform& laySh);
    void     setTrShiftInSecSys(void);
    void     calcShiftInSecSys(const HGeomTransform* laySec, const HGeomTransform& shift,
                               HGeomTransform& laySh);
    void     setTrShiftInLabSys(void);
    void     calcShiftInLabSys(const HGeomTransform* secSys, const HGeomTransform* laySysMod,
                               const HGeomTransform& shift, HGeomTransform& laySh);
    void     fitAlgorithmForMille(void);
    void     fitAlgForMilleCosmic(void);
    void     sendToMille(void);
    Int_t    getGlobalDer(void);
    Int_t    getGlobalDerWTof(void);
    Double_t calcGlobDer(Int_t parNum);
    Double_t calcDriftTime(const HGeomTransform& laySys,Int_t& distSign);
    Double_t calcDerTofCorr(Int_t p,Double_t* dDrTm=0);
    void     creatPedeInParamFile(void);
    void     creatSumOfShiftsFile(void);
    void     readPedeResFile(const char* fileName);
    Bool_t   loadGeometryPar(void);
    Bool_t   makeShifts(void);
    Bool_t   writeParAsciiFile(void);
    void     printErrorAndExit(const char* func,const char* form,const char* str=NULL);
    void     readSumOfShiftsFile(void);
    Bool_t   addShifts(TString& buffer);
    void     createHists(Int_t sec);
    void     openNewBinaryFile(void);
    void     writePedeTaskFile(void);
    void     copyFile(const char* op,TString& file,const char* ext=0);
    Double_t calcDDriftTime(Int_t p);
    Bool_t   eventFilter(void);
  
  ClassDef(HMdcMille,0) // Interface class for Millipede alignment.
};

#endif
