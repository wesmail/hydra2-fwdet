#ifndef HMDCTRACKDSET_H
#define HMDCTRACKDSET_H

#include "hreconstructor.h"
#include "TCutG.h"
#include "TString.h"

class HMdcTrackFinder;
class HMdcClusterToHit;
class HMdc12Fit;
class HMdcIdealTracking;

class HMdcTrackDSet {
  protected:
    // HMdcTrackFinder param.:
    static HMdcTrackFinder* pTrackFinder; //
    static Bool_t   trFndrIsAdded;       // kTRUE - HMdcTrackFinder added already
    static Bool_t   isCoiloff;           // kTRUE=magnetoff,kFALSE=magneton
    static Int_t    clFinderType;        // =0 - segment finder, =1 - mdc finder
    static Int_t    level4[2];           // [seg] levels for 4-layers finding
    static Int_t    level5[2];           // [seg] levels for 5-layers finding
    static Int_t    nLayersInMod[6][4];  // num.of layers in modules
    // Vertex finder (HMdcLookUpTb):
    static Bool_t   findVertex;
    static Double_t dDistCutVF;          // Cut:  dDist/yProjUncer < dDistCut
    static Int_t    levelVertF;          // Level is used in HMdcLookUpTbSec::testBinForVertexF(...)
    static Int_t    levelVertPPlot;      // Level of project plot filling
    static Double_t zStartDetertor;      // If <0. - position of the start detector
    // Inner segment (HMdcLookUpTb):
    static Int_t    nBinXSeg1;           // Project plot size;
    static Int_t    nBinYSeg1;
    static Bool_t   useFloatLevSeg1;
    static Bool_t   useAmpCutSeg1;
    static Char_t   fakeSuppFlSeg1;      // -1-mergeClust., 0-no suppr.; 1-doSuppr; 2-set flag only
    static Bool_t   useDrTimeSeg1;
    static Double_t constUncertSeg1;     // Const. part for LookUpTbCell::distCut...
    static Double_t dDistCutSeg1;          
    static Double_t dDistCutYcorSeg1;
    static Double_t dDistCorrLaySeg1[4][6];  // [mod][layer]
    // Outer segment (HMdc34ClFinder):
    static Int_t    nBinXSeg2;           // Project plot size;
    static Int_t    nBinYSeg2;
    static Bool_t   useFloatLevSeg2;
    static Char_t   fakeSuppFlSeg2;      // -1-mergeClust., 0-no suppr.; 1-doSuppr; 2-set flag only
    static Int_t    wLevSeg2;            // weight of cluster finder level
    static Int_t    wBinSeg2;            // weight of bins number in cluster
    static Int_t    wLaySeg2;            // weight of cluster amplitude
    static Int_t    dWtCutSeg2;          // cut
    static Bool_t   useDrTimeSeg2;
    static Double_t dDistCutSeg2;          
    static Double_t dDistCutYcorSeg2;
    static Double_t dDistCorrLaySeg2[2][6]; // [mod-2][layer]
    static Bool_t   useFittedSeg1Par;     // kTRUE - use seg1 fitted track param. for finding seg2
    
    static Bool_t   useDxDyCut;
    static TCutG    cutDxDyArr[36];
    static Bool_t   isDxDyInited;

    static Bool_t   useKickCor;           // kTRUE - use HMdcKickCor for outer segment finder

    // Meta match:
    static Bool_t   doMetaMatch;
    static Bool_t   doMMPlots;
      
    // HMdcClusterToHit param.:
    static HMdcClusterToHit* pClusterToHit; //!
    static Int_t    modForSeg1;          // -1 - don't fill HMdcSeg for segment 1.
                                         //  0 & typeClustFinder=1 & sector_has_mdc1 & 2
                                         //    - fill HMdcSeg by cluster in mdc1
                                         //  1 - -/- in mdc2
    static Int_t    modForSeg2;          // -1 - don't fill HMdcSeg for segment 2
                                         //  2 & typeClustFinder=1 & sector_has_mdc3&4
                                         //    & isCoilOff=kTRUE 
                                         //    - fill HMdcSeg by cluster in mdc3
                                         //  3 - -/- in mdc4
    // HMdc12Fit param.:
    static HMdc12Fit* p12Fit;            //!
    static Bool_t   useAnotherFit;       // another fit MUST be inherited from HMdc12Fit
    static Int_t    fitAuthor;           // 1-Alexander's fit, 2-modified version 1
    static Int_t    distTimeVer;         // 0-calc.time by polin., 1-by J.Market's fun.
    static Int_t    fitType;             // 0-segment fit, 1-mdc fit
    static Bool_t   fntuple;             // kTRUE-create and fill catClusFit&catWireFit
    static Bool_t   fprint;              // kTRUE-print debug
    static Int_t    useWireOffset;       // remove signal time on wire from drift time
    static Int_t    mixFitCut;           // mixed cluster finder cut
    static Int_t    mixClusCut;          // mixed cluster finder cut
    
    // OffVertex tracking param.:
    static Bool_t   findOffVertTrk;      // kTRUE - offVertex tracking ON
    static Int_t    nLayersCutOVT;       // Minimal number of fired layers in MDCI+II
    static Int_t    nWiresCutOVT;        // Mum.wires cut: nWrMdcI*nWrMdcII < nWiresCutOVT^2
    static Int_t    nCellsCutOVT;        // MDCII window (+/-,in cells) for combining wires with MDCI
    
    // HMdcTrackFitter & HMdcWireArr param.:
    static UChar_t  calcInitValue;       // =0 don't calculate initial track parameters.
                                         // =1 calculate initial track param. for all segments
                                         // =2 calc.init.param and refit if fit had problem
    static Double_t cInitValCutSeg1;     // Cut for the HMdcWireArr::calcInitialValue
    static Double_t cInitValCutSeg2;     // Cut for the HMdcWireArr::calcInitialValue
    
    // HMdcIdealTracking:
    static HMdcIdealTracking* pIdealTracking; //!
    
    static Bool_t   isSetCall;

  public:
    static HReconstructor* hMdcTrackFinder(const Char_t* name, const Char_t* title);
    static void useThisTrackFinder(HMdcTrackFinder* trFndr);
    static HReconstructor* hMdcClusterToHit(const Char_t* name, const Char_t* title);
    static HReconstructor* hMdcClusterToHit(const Char_t* name, const Char_t* title,
                                            Int_t mSeg1, Int_t mSeg2);
    static HReconstructor* hMdc12Fit(const Char_t* name, const Char_t* title);
    static HReconstructor* hMdc12Fit(const Char_t* name, const Char_t* title, Int_t author,
                                     Int_t version, Int_t indep, Bool_t nTuple, Bool_t prnt);
    static void     setAnotherFit(HMdc12Fit* fit);

    static Bool_t   isCoilOff(void)                          {return isCoiloff;}
    static Int_t    getClFinderType(void)                    {return clFinderType;}
    static void     getTrFnLevels(Int_t& l4s1,Int_t& l5s1,Int_t& l4s2,Int_t& l5s2);
    static Int_t    getNLayersInMod(Int_t s,Int_t m)         {return nLayersInMod[s][m];}
    static Bool_t   isModActive(UInt_t sec,UInt_t mod);
    static Bool_t   isSegActive(UInt_t sec,UInt_t seg);
    static Bool_t   isSecActive(UInt_t sec);
    static Int_t    getModForSeg1(void)                      {return modForSeg1;}
    static Int_t    getModForSeg2(void)                      {return modForSeg2;}
    static Int_t    getModForSeg(Int_t sg)                   {return sg==0 ? modForSeg1:modForSeg2;}
    
    static Int_t    getFitAuthor(void)                       {return fitAuthor;}
    static Int_t    getFitVersion(void)                      {return distTimeVer;}
    static Int_t    getFitType(void)                         {return fitType;}
    static Int_t    getUseWireOffset(void)                   {return useWireOffset;}
    static Int_t    getMixFitCut(void)                       {return mixFitCut;}
    static Int_t    getMixClusCut(void)                      {return mixClusCut;}
    static Int_t    getLevel4(Int_t iseg)                    {return level4[iseg];}
    static Int_t    getLevel5(Int_t iseg)                    {return level5[iseg];}
    static Char_t   getGhostRemovingParamSeg2(Int_t &wlev,Int_t &wbin,Int_t &wlay,Int_t &dWtCut);
    static Char_t   getGhostRemovingFlagSeg1(void)           {return fakeSuppFlSeg1;}
    static Char_t   getGhostRemovingFlagSeg2(void)           {return fakeSuppFlSeg2;}
    static void     getProjectPlotSizeSeg1(Int_t &xb,Int_t &yb);
    static void     getProjectPlotSizeSeg2(Int_t &xb,Int_t &yb);
    static Bool_t   useFloatLevelSeg1(void)                  {return useFloatLevSeg1;}
    static Bool_t   useFloatLevelSeg2(void)                  {return useFloatLevSeg2;}
    static Bool_t   useSegAmpCutSeg1(void)                   {return useAmpCutSeg1;}
    static Bool_t   useDriftTimeSeg1(void)                   {return useDrTimeSeg1;}
    static Bool_t   useDriftTimeSeg2(void)                   {return useDrTimeSeg2;}
    static Bool_t   getVertexFinderPar(Double_t &d,Int_t &l,Int_t &lp);
    static Bool_t   findClustVertex(void)                    {return findVertex;}
    static Double_t getZStartDetector(void)                  {return zStartDetertor;}
    static void     getDrTimeProjParSeg1(Double_t &uns,Double_t &cut,Double_t &ycor,Double_t *lcor);
    static void     getDrTimeProjParSeg2(Double_t &cut,Double_t &ycor,Double_t *lcor);
    static Bool_t   doMetaMatching(Bool_t &mp)               {mp = doMMPlots; return doMetaMatch;}
    static Bool_t   getDxDyKickCut(TCutG* cutR);
    static UChar_t  getCalcInitValueFlag(void)               {return calcInitValue;}
    static Double_t getCalcInitValueCut(Int_t seg)           {return seg==0 ? cInitValCutSeg1:cInitValCutSeg2;}
    static Bool_t   useFittedSeg1ParFlag(void)               {return useFittedSeg1Par;}
    static Bool_t   getUseKickCorFlag(void)                  {return useKickCor;}
    static Bool_t   getFindOffVertTrkFlag(void)              {return findOffVertTrk;}
    static Int_t    getNLayersCutOVT(void)                   {return nLayersCutOVT;}
    static Int_t    getNWiresCutOVT(void)                    {return nWiresCutOVT;}
    static Int_t    getNCellsCutOVT(void)                    {return nCellsCutOVT;}

    static HMdcTrackFinder* getMdcTrackFinder(void)          {return pTrackFinder;}
    static void     clearPTrackFinder(void)                  {pTrackFinder = NULL;}
    static HMdcClusterToHit* getMdcClusterToHit(void)        {return pClusterToHit;}
    static void     clearPClusterToHit(void)                 {pClusterToHit = NULL;}
    static HMdc12Fit* getMdc12Fit(void)                      {return p12Fit;}
    static void     clearP12Fit(void)                        {p12Fit = NULL;}
    static Bool_t   fNTuple(void)                            {return fntuple;}
    static Bool_t   fPrint(void)                             {return fprint;}
    
            
    static void setCoilStatus(Bool_t stat)                 {isCoiloff = stat;}
    static void setClFinderType(Int_t type);
    static void setTrFnNLayers(Int_t s, Int_t m,Int_t nl);
    static void setTrackFinderLevel(Int_t l4s1, Int_t l5s1, Int_t l4s2, Int_t l5s2);
    static void setTrFnSegLevel(Int_t seg, Int_t l4, Int_t l5);
    static void setRemoveGhostsSeg1(Char_t fl=1)           {fakeSuppFlSeg1 = fl;}
    static void writeFakesToTreeSeg1(void)                 {fakeSuppFlSeg1 = 2;}
    static void setRemoveGhostsSeg2(Char_t fl=1)           {fakeSuppFlSeg2 = fl;}
    static void writeFakesToTreeSeg2(void)                 {fakeSuppFlSeg2 = 2;}
    static void setGhostRemovingParamSeg2(Int_t wlev,Int_t wbin,Int_t wlay,Int_t dWtCut);
    static void setProjectPlotSizeSeg1(Int_t xb,Int_t yb);
    static void setProjectPlotSizeSeg2(Int_t xb,Int_t yb);
    static void setFloatLevelSeg1(Bool_t fl=kTRUE)         {useFloatLevSeg1 = fl;}
    static void setFloatLevelSeg2(Bool_t fl=kTRUE)         {useFloatLevSeg2 = fl;}
    static void setSegAmpCutSeg1(Bool_t fl=kTRUE)          {useAmpCutSeg1   = fl;}
    static void setMergeClusSeg1(void)                     {fakeSuppFlSeg1  = -1;}
    static void setMergeClusSeg2(void)                     {fakeSuppFlSeg2  = -1;}
    static void setUseDriftTimeSeg1(Bool_t fl=kTRUE)       {useDrTimeSeg1   = fl;}
    static void setUseDriftTimeSeg2(Bool_t fl=kTRUE)       {useDrTimeSeg2   = fl;}
    static void setVertexFinderFlag(Bool_t fl=kTRUE)       {findVertex      = fl;}
    static void setVertexFinderPar(Double_t dc, Int_t lv, Int_t lp);
    static void setZStarDetector(Double_t zs)              {zStartDetertor  = zs;}
    static void setMetaMatchFlag(Bool_t mf,Bool_t mp)      {doMetaMatch     = mf;
                                                            doMMPlots       = mp;}
    static void setDxDyKickCut(TCutG* cutR);
    
    static void setUseKickCorFlag(Bool_t fl=kTRUE)         {useKickCor      = fl;}
    
    static void setDrTimeProjParamSeg1(Double_t cUns, Double_t cut);
    static void setDrTimeCutYCorrSeg1(Double_t corr)       {dDistCutYcorSeg1 = corr;}
    static void setDrTimeCutLCorrSeg1(Int_t m,Double_t* corr);  // corr array for 6 layers
    
    static void setDrTimeProjParamSeg2(Double_t cut)       {dDistCutSeg2     = cut;}
    static void setDrTimeCutYCorrSeg2(Double_t corr)       {dDistCutYcorSeg2 = corr;}
    static void setDrTimeCutLCorrSeg2(Int_t m,Double_t* corr);  // corr array for 6 layers
    
    static void setFitAuthor(Int_t author);
    static void setFitVersion(Int_t ver);
    static void setFitType(Int_t type);
    static void setFitOutput(Bool_t nTuple, Bool_t prnt);
    static void setMixFitCut(Int_t cut);
    static void setMixClusCut(Int_t cut);
    static void setMixCuts(Int_t cutFit,Int_t cutClus);
    
    static void setTrackFinder(HMdcTrackFinder* tr);
    static void setClusterToHit(HMdcClusterToHit* cl);
    static void set12Fit(HMdc12Fit* fit);
    static void setDxDyCut(Bool_t fl=kTRUE)                {useDxDyCut       = fl;}
    static void setCalcInitialValue(UChar_t fl=1)          {calcInitValue    = fl;}
    static void setCalcInitialValueSeg1(Double_t c)        {cInitValCutSeg1  = c;}
    static void setCalcInitialValueSeg2(Double_t c)        {cInitValCutSeg2  = c;}
    static void setUseFittedSeg1ParFlag(Bool_t fl=kTRUE)   {useFittedSeg1Par = fl;}
    static void setFindOffVertTrkFlag(Bool_t fl=kTRUE)     {findOffVertTrk   = fl;}
    static void setNLayersCutOVT(Int_t c)                  {nLayersCutOVT    = c;}
    static void setNWiresCutOVT(Int_t c)                   {nWiresCutOVT     = c;}
    static void setNCellsCutOVT(Int_t c)                   {nCellsCutOVT     = c;}
    
    static void setTrackParam(TString beamtime);

    static void printStatus(void);
    static void printMdc34ClFinderPar(void);
    
    // Functions for HMdcTrackFinderSetup only (see mdc/hmdcsetup.cc)
    static void setMdcTrackFinder(Bool_t coilSt,Int_t type);
    static void setTrFnLevel(Int_t l4s1, Int_t l5s1, Int_t l4s2, Int_t l5s2);
    static void setTrFnNLayers(const Int_t *lst);
    static void setMdcClusterToHit(Int_t mSeg1,Int_t mSeg2);
    static void setMdc12Fit(Int_t author, Int_t version, Int_t indep, Bool_t nTuple, Bool_t prnt);
    static void setUseWireOffset(Bool_t off);
    
    // IdealTracking
    static HReconstructor* hMdcIdealTracking(const Char_t* name,const Char_t* title,Bool_t fillParallel=kFALSE);
    static HMdcIdealTracking* getMdcIdealTracking(void)    {return pIdealTracking;}
    
  protected:
    static void exitSetup(const Char_t* funNm,Int_t npar,const Char_t* use,Int_t inst);
    static void exitSetup(const Char_t* funNm,const Char_t* className);
    
    static void setDefParam(void);      // default parameters
    static void setDefParAu15Au(void);
    static void setTrackParamAug11(void);
    static void setTrackParamApr12(void);
    static void setTrackParamJul14(void);
    
    static void initDxDyCutAug11(void);
    static void initDxDyCutApr12(void);
  private:
    HMdcTrackDSet(void) {}
    virtual ~HMdcTrackDSet(void) {}
    
  ClassDef(HMdcTrackDSet,1) // Set of tasks
};

#endif /* !HMDCTRACKDSET_H */
