#ifndef HMDCLOOKUPTB_H
#define HMDCLOOKUPTB_H

#include "TObject.h"
#include "hparset.h"
#include "hmdcgeomobj.h"
#include "hlocation.h"
#include "hmdcsizescells.h"
#include <stdlib.h>

class TH2C;
class TObjArray;
class HCategory;
class HMdcGeomPar;
class HMdcGetContainers;
class HMdcLayerGeomPar;
class HMdcSizesCells;
class HSpecGeomPar;
class HMdcClus;
class HMdcCluster;
class HMdcClustersArrs;
class HMdcList12GroupCells;
class HMdcClFnStack;
class HMdcClFnStacksArr;
class HMdcSecListCells;
class HMdcDriftTimeParSec;
class HMdcLayListCells;
class HStart2GeomPar;

class HMdcLookUpTbCell: public TObject {
  protected:
    UShort_t  nLines;          // Number of lines (Y-dir.) in cell projections 
    UShort_t  line;            // Counter of lines. After filling should be eq.nLines
    UShort_t  yBinMin;         // First line in the projection
    UShort_t  yBinMax;         // Last line in the projection
    UShort_t *xBinMin;         // First X bin in the projection for each Y line
    UShort_t *xBinMax;         // Last X bin in the projection for each Y line
    Float_t   alphaMean;       // [deg.] Mean value for impact angle
    Float_t   distCutFT;       // [mm] Full target projection. =yPrUncerFTarg*dDistCut
    Float_t   distCut1T;       // [mm] One or piece target pr..=yPrUncer1Targ*dDistCut
    Float_t   distCutVF;       // [mm] For vertex finder.      =yPrUncer1Targ*dDistCutVF
    // For each event:
    Double_t  dDistMinCut;     // = tdcTDist - yProjUncer1T*dDistCut
    Double_t  dDistMaxCut;     // = tdcTDist + yProjUncer1T*dDistCut
    Double_t  dDistMinCut2VF;  // = (tdcTDist - yProjUncer1T*dDistCutVF)^2
    Double_t  dDistMaxCut2VF;  // = (tdcTDist + yProjUncer1T*dDistCutVF)^2
  public:
    HMdcLookUpTbCell(void);
    ~HMdcLookUpTbCell(void);
    void     clear(void) {line=0;}
    void     init(Int_t yBinMinT, Int_t yBinMaxT);
    Bool_t   addLine(UShort_t nc1, UShort_t nc2);
    UShort_t getNLines(void) const             {return line;}
    UShort_t getYBinMin(void) const            {return yBinMin;}
    UShort_t getYBinMax(void) const            {return yBinMax;}
    UShort_t getXBinMin(UInt_t yb) const       {return xBinMin[yb];}
    UShort_t getXBinMax(UInt_t yb) const       {return xBinMax[yb];}
    void     setAlphaMean(Double_t al)         {alphaMean = al;}
    Double_t getAlphaMean(void) const          {return alphaMean;}
    void     setDistCutFT(Double_t ct)         {distCutFT = ct;}
    void     setDistCut1T(Double_t ct)         {distCut1T = ct;}
    void     setDistCut1TVF(Double_t ct)       {distCutVF = ct;}
    void     setTdcTDistAndCuts1T(Double_t d);
    void     setTdcTDistAndCutsFT(Double_t d);
    Double_t getDDistMinCut(void) const        {return dDistMinCut;}
    Double_t getDDistMaxCut(void) const        {return dDistMaxCut;}
    Double_t getDDistMinCut2VF(void) const     {return dDistMinCut2VF;}
    Double_t getDDistMaxCut2VF(void) const     {return dDistMaxCut2VF;}
    
    ClassDef(HMdcLookUpTbCell,0)
};

class HMdcLookUpTbLayer: public TObject {
  protected:
    HMdcSizesCellsLayer* pSCellLay;
    HMdcLookUpTbCell*    pLTCellArr; // max 208
    Int_t    nCells;           // Number of cells in layer
    struct DistCalcLuTb {
      Double_t yt;             // Target position in rot.layer
      Double_t zt;             // coordinate system.
      Double_t C1x;            // LookUpTable for min.dist calc
      Double_t C1y;            //
      Double_t C1;             //
      Double_t C2x;            //
      Double_t C2y;            //
      Double_t C2;             //
      Double_t yVertex;        // One of the point in yTp or yt
      Double_t zVertex;        // One of the point in zTp or zt
      Double_t yTp[250];       // Target position in rot.layer
      Double_t zTp[250];       // coordinate system.
    };
    DistCalcLuTb  layPart1;
    DistCalcLuTb *layPart2;
    
    DistCalcLuTb  *currLayPart;
    
    // For vetex finder mode:
    Int_t    lTargPnt;       // Num_of_elements_in_arrays
    Double_t yWirePos;       // = HMdcSizesCellsCell::getWirePos() for the current cell
    Double_t dDmin;          // = HMdcLookUpTbCell::dDistMinCut for the current cell
    Double_t dDmax;          // = HMdcLookUpTbCell::dDistMaxCut for the current cell
    
  public:
    HMdcLookUpTbLayer(Int_t sec, Int_t mod, Int_t layer);
    ~HMdcLookUpTbLayer(void);
    HMdcLookUpTbCell& operator[](Int_t i) {return pLTCellArr[i];}
    Int_t           getSize(void);
    void            setMinDistLUTb(Int_t lPart,HGeomVector& t,Double_t* carr);
    void            setNTargPnts(Int_t n)           {lTargPnt = n;}
    Double_t*       getYTargetArr(void)             {return layPart1.yTp;}
    Double_t*       getZTargetArr(void)             {return layPart1.zTp;}
    Double_t*       getYTargetArrP2(void)           {return layPart2==NULL ? NULL : layPart2->yTp;}
    Double_t*       getZTargetArrP2(void)           {return layPart2==NULL ? NULL : layPart2->zTp;}
    void            setVertexPoint(Int_t vp);
    void            setCurrentCell(Int_t cell);
    inline Bool_t   drTmTest(Double_t xb,Double_t yb) const;
    inline Bool_t   drTmTest2(Double_t xb,Double_t yb,Double_t corr=0.) const;
    inline void     transToRotLay(Double_t xb,Double_t yb,Double_t &ybl,Double_t &zbl) const;
    DistCalcLuTb   *getLayerPart2Par(void)          {return layPart2;}

    ClassDef(HMdcLookUpTbLayer,0)
};

inline Bool_t HMdcLookUpTbLayer::drTmTest(Double_t xb,Double_t yb) const {
  //  xb - x of bin on the project plot
  //  yb - y of bin
  // current val.:  yVertex=yTp[v]; zVertex=zTp[v],  yOffArr,dDmin2,dDmax2
  Double_t y      = currLayPart->C1x*xb + currLayPart->C1y*yb + currLayPart->C1;
  Double_t z      = currLayPart->C2x*xb + currLayPart->C2y*yb + currLayPart->C2;
  Double_t dyDdz  = (y - currLayPart->yVertex)/(z - currLayPart->zVertex);
  Double_t ct     = 1.+dyDdz*dyDdz;
  Double_t dDist2 = y-dyDdz*z - yWirePos;
  dDist2 *= dDist2;
// printf("New: y=%g,z=%g,dDist=%g,dDmin=%g dDmax=%g",y,z,sqrt(dDist2/ct),sqrt(dDmin2),sqrt(dDmax2));
// printf("  %s\n",dDist2>dDmin2*ct && dDist2<dDmax2*ct ? "in":"out");
  return dDist2>dDmin*dDmin*ct && dDist2<dDmax*dDmax*ct;
}

inline Bool_t HMdcLookUpTbLayer::drTmTest2(Double_t xb,Double_t yb,Double_t corr) const {
  // It calculate the same as 
  // HMdcSizesCellsLayer::getDist(target[0],target[1],target[2],
  //                              xb,yb,prPlane.getZOnPlane(xb,yb), cell);
  // xb - x of bin on the project plot
  // yb - y of bin
  // corr - additional cut correction for bin
  
  //// current val.:  yVertex=yTp[v]; zVertex=zTp[v],  yOffArr,dDmin2,dDmax2
  Double_t y      = currLayPart->C1x*xb + currLayPart->C1y*yb + currLayPart->C1;
  Double_t z      = currLayPart->C2x*xb + currLayPart->C2y*yb + currLayPart->C2;
  Double_t dyDdz  = (y - currLayPart->yVertex)/(z - currLayPart->zVertex);
  Double_t ct     = 1.+dyDdz*dyDdz;
  Double_t dDist2 = y-dyDdz*z - yWirePos;
  dDist2 *= dDist2;
  if(corr<=0.) return dDist2>dDmin*dDmin*ct && dDist2<dDmax*dDmax*ct;
  Double_t dDminCorr = dDmin>corr ? dDmin-corr : 0.;
  Double_t dDmaxCorr = dDmax+corr;
  // Cut: Abs(dDist) < (dDist1T + constUncert)*dDistCut*dDCutCorr[mod][lay]+yDDistCorr[y]
  return dDist2>dDminCorr*dDminCorr*ct && dDist2<dDmaxCorr*dDmaxCorr*ct;
}

inline void HMdcLookUpTbLayer::transToRotLay(Double_t xb,Double_t yb,Double_t &ybl,Double_t &zbl) const {
  // xb and yb must be on the project plane and in sec.coor.sys.
  // This function is used in vertex finder only and valid for inner MDC planes!
  ybl = layPart1.C1x*xb + layPart1.C1y*yb + layPart1.C1;
  zbl = layPart1.C2x*xb + layPart1.C2y*yb + layPart1.C2;
}

//-------Mod.-----------------------------

class HMdcLookUpTbMod : public TObject {
  protected:
    TObjArray* array;      // Array of HMdcLookUpTbLayer objects
    Int_t      nLayers;    // Number of working layers
  public:
    HMdcLookUpTbMod(Int_t sec, Int_t mod);
    ~HMdcLookUpTbMod(void);
    HMdcLookUpTbLayer& operator[](Int_t i) {return *static_cast<HMdcLookUpTbLayer*>((*array)[i]);}
    Int_t getSize(void);
    void  setNLayers(Int_t nl) {nLayers=nl;}
    Int_t getNLayers(void)     {return nLayers;}

  ClassDef(HMdcLookUpTbMod,0)
};

//-------Sec.-----------------------------
class HMdcLookUpTbSec : public TObject {
  protected:
    Int_t               sector;          // sector number
    Int_t               segment;         // =0 -inner segment, -1 inner&outer
    Int_t               nSegments;       // num. of segments for clus. finder
    Int_t               nModules;        // num. of modules for cluster finder.
    Int_t               maxNModules;     // num. of modules (eq.2 or 4)
    Bool_t              isCoilOff;
                                         // Size of plot:
    Int_t               nBinX;           // num.bins along axis X,
    Double_t            xLow;            // low edge of X
    Double_t            xUp;             // upper edge of X
    Double_t            xStep;           // step X = (xUp-xLow)/nBinX
    Int_t               nBinY;           // num.bins along axis Y
    Double_t            yLow;            // low edge of Y
    Double_t            yUp;             // upper edge of Y
    Double_t            yStep;           // step Y= (yUp-yLow)/nBinY
    Float_t             xFirstBin;       // x of first bin
    Float_t             yFirstBin;       // y of first bin
    Double_t           *xBinsPos;        // Bins positions
    Double_t           *yBinsPos;        //

    Int_t               size;            // project plot size
    static UChar_t     *hPlMod[4];       // hPlMod[0] - mod.1, ...
    static Int_t        hPlModsSize;     // real length of hPlMod[mod] (can be >size)

    Int_t               sizeBAr;         // Size of the bit array plotBAr (in bytes)
    static Int_t        sizeBArSt;       // sizeBArSt>=sizeBAr;
    static UChar_t     *plotBArSc;       // Bit array (1 bit - 1 bin) (for scaning).
    UInt_t              maxBinBAr4Sc;    // regiong of bins for scaning
    UInt_t              minBinBAr4Sc;    //

    static UChar_t     *plotBArM[4];     // Bit arraies for MDC's.
    UInt_t              maxBinBAr4M[4];  //
    UInt_t              minBinBAr4M[4];  //

    UInt_t*             xMin[4];         // for cleaning proj. plots
    UInt_t*             xMax[4];         //

    HMdcClFnStack      *stack;           // stack
    HMdcClFnStacksArr  *stacksArr;       // array of stacks

    TObjArray          *array;           // array of pointers of type HMdcLookUpTbMod

    HCategory          *fClusCat;        // category of clusters
    HLocation           locClus;         // location of cluster category
    Bool_t              isGeant;         // =kTRUE - geant data
    Bool_t              trackListFlag;   // =kTRUE - filling tracks list in ClusSim
    Bool_t              noFiredCells;    // =kTRUE if no fired cells
    Int_t               maxAmp[4];       // number of fired layes in MDC
    
    HMdcSecListCells*   pListCells;      // list of fired wires in current event
    
    Int_t               minAmp[4];       // max.clus. for mdc1,2,3,4
    Int_t               nMods;           // num. of modules in sector for cl.finding
    Int_t               typeClFinder;    // =0 - finding in all mdc in sec.
                                         // =1 - finding in each mdc indep.
                                         // =2 - combinedchamber+chamber clusters
    Int_t               neighbBins[8];
    HMdcPlane           prPlane;         // Projections plane
    Float_t             target[3];       // target
    Float_t             eTarg[3];        // errors of target position
    HGeomVector         targVc[3];       // [0] First target point in sec.coor.sys.
                                         // [1] Last target point in sec.coor.sys.
                                         // [2] Middle point in sec.coor.sys.
    Bool_t              doVertexFn;      //
    Double_t            dDistCutVF;      // As dDistCut but fo vertex finder. <0. no vertex finder.
    Int_t               levelVertF;      // Level for vertex finder.
    Int_t               levelVertFPP;    // Level for vertex finder project plot.
    Int_t               lTargPnt;        // Num. of target points for vertex finder.
    Int_t               indFirstTPnt;
    Int_t               indLastTPnt;
    HGeomVector         targetPnts[250];
    Double_t            vertZErr;        //
    Int_t              *vertexStat;      // It is HMdcLookUpTb::vertexStat
    Int_t               vertexPoint;     // Result of vertex finder
    

    Int_t               nClusters;       // counter of clusters

    TH2C*               hist;            //
    Int_t               plBining;

    // -- Clusters parameters ---
    HMdcClustersArrs*   pClustersArrs;   // clusters arraies
    Int_t               nModSeg[2];
    Int_t               clusArrSize;     // size of clusArr,clusArrM1,clusArrM2
    HMdcCluster*        clusArr;         // array of clusters parameters
    Int_t               nClsArr;         // counter of clusters in array

    static Short_t*     clusIndM1;       // array of MDC1 clusters indexis
    static Int_t        clIndArrSzM1;    // clusIndM1 array size
    HMdcCluster*        clusArrM1;       // array of clusters parameters in MDC1
    Int_t               nClsArrM1;       // counter of clusters in MDC1
    static Short_t*     clusIndM2;       // array of MDC2 clusters indexis
    static Int_t        clIndArrSzM2;    // clusIndM2 array size
    HMdcCluster*        clusArrM2;       // array of clusters parameters in MDC2
    Int_t               nClsArrM2;       // counter of clusters in MDC2
    
    Int_t               clusArrInd;      // current clus.array index:
                                         // 0-clusArr, 1-clusArrM1, 2-clusArrM2
    HMdcCluster*        cClusArr;        // current clusters array
    Int_t*              cNClusArr;       // current clusters array counter
    Int_t               cSeg;
    Int_t               cMod1;
    Int_t               cMod2;
    UChar_t*            cPlModF;
    UChar_t*            cPlModS;
    Int_t               cMod;
    UChar_t*            cPlMod;
    Short_t*            clusInd;
        
    HMdcLookUpTbMod*    cFMod;           // current module
    UChar_t*            cHPlModM;        // current mod.pr.plot
    HMdcCluster*        clus;            // current cluster
    Bool_t              isClstrInited;   // flag for "clus" object
    UInt_t*             cXMinM;          // current xMin[mod]
    UInt_t*             cXMaxM;          // current xMax[mod]
    UInt_t*             pXMinM;          // xMin[mod] for previous mdc
    UInt_t*             pXMaxM;          // xMax[mod] for previous mdc
    UChar_t*            cPlotBAr;        // current bit array

    Int_t               nLMaxCl;         // cluster region on proj.plane (bins)
    Int_t               nLMinCl;         // nLMinCl-nLMaxCl region of lines
    UShort_t*           xMaxCl;          // regions of x bins for each line
    UShort_t*           xMinCl;          // in nLMinCl-nLMaxCl region

    HMdcClus*           fClus;           // pointer to cluster;
    Int_t               nFirstClust;     // needed for clusters merger
    Bool_t              isSlotAv[2];     // =kFALSE if cluster slot not available

    Int_t               layerOrder[6];   // 
    Int_t               module;          // current MDC module
    Int_t               layer;           // layer
    Int_t               cell;            // cell
    Float_t             tdcTime;         // and time
    UChar_t             add;             //
    HMdcLayListCells*   pLayLCells;
    HMdcLookUpTbLayer*  pLUTLayer;
    HMdcLookUpTbCell*   pLUTCell;
    HMdcSizesCellsSec*  pSCellSec;
    HMdcDriftTimeParSec* pDriftTimeParSec;
    Double_t            constUncert;     // Constant part in LookUpTbCell::yProjUncer
    Double_t            dDCutCorr[4][6]; // Correction for layer cut 
    Double_t            dDistCut;        // Cut:  dDist/yProjUncer < dDistCut*dDCutCorr[mod][layer]
    Double_t           *yDDistCorr;      // Cut correction for y-bins
    Bool_t              useDriftTime;    //
    Char_t              fakeSuppFlag;    // 0-don't do; 1-do; 2-set flag only (don't remove obj.)
    
  public:
    HMdcLookUpTbMod& operator[](Int_t i) {return *static_cast<HMdcLookUpTbMod*>((*array)[i]);}
    Int_t        getSize(void);
    Int_t        getNClusters(void) const            {return nClusters;}
    Int_t        getMaxClus(Int_t m=-1) const;
    Int_t        getNBinX(void) const                {return nBinX;}
    Double_t     getXlow(void) const                 {return xLow;}
    Double_t     getXup(void) const                  {return xUp;}
    Int_t        getNBinY(void) const                {return nBinY;}
    Double_t     getYlow(void) const                 {return yLow;}
    Double_t     getYup(void) const                  {return yUp;}
    void         clearwk(void);
    Int_t        findClusters(Int_t* imax);
    void         setParPlane(const HMdcPlane &plane) {prPlane.setPlanePar(plane);}
    void         setTargetF(const HGeomVector& vec)  {targVc[0]=vec;}
    void         setTargetL(const HGeomVector& vec)  {targVc[1]=vec;}
    HMdcPlane&   getPrPlane(void)                    {return prPlane;}
    const        HGeomVector& getTargetF(void)       {return targVc[0];}
    const        HGeomVector& getTargetL(void)       {return targVc[1];}
    TH2C*        fillTH2C(const Char_t* name,const Char_t* title,Int_t type=0,Int_t bining=2);
    void         setTypeClFinder(Int_t type)         {typeClFinder = type;}
    Int_t        getTypeClFinder(void)               {return typeClFinder;}
    Int_t        xBinNum(Double_t x)                 {return Int_t((x-xLow)/xStep);}
    Int_t        yBinNum(Double_t y)                 {return Int_t((y-yLow)/yStep);}
    Int_t        xBinNumInBounds(Double_t x);
    Int_t        yBinNumInBounds(Double_t y);
    Bool_t       calcXYBounds(Double_t& xL,Double_t& xU,Double_t& yL,Double_t& yU);
    void         setPrPlotSize(Double_t xL,Double_t xU,Double_t yL,Double_t yU);
    
    HMdcLookUpTbSec(Int_t sec, Int_t nSegs, Int_t inBinX, Int_t inBinY);
    void         setClusCat(HCategory* cat)          {fClusCat  = cat;}
    void         setStack(HMdcClFnStack* st)         {stack     = st;}
    void         setStacksArr(HMdcClFnStacksArr* sA) {stacksArr = sA;}
    void         setCoilFlag(Bool_t flg)             {isCoilOff = flg;}
    void         calcTarget(Double_t* targLenInc);
    Bool_t       calcLookUpTb(Bool_t quiet);
    void         fillTrackList(Bool_t fl)            {trackListFlag = fl;}
    void         calcTdcDrDist(void);
    void         findVertex(void);
    void         findSecVertex(void);
    void         setVertexStat(Int_t *vs)            {vertexStat = vs;}
    void         calcVertexFnTarg(Int_t nTrPnts,HGeomVector* trPnts);
    void         setVertexPoint(Int_t vp);
    void         setVertexZErr(Double_t vze)         {vertZErr = vze;}
    
  protected:
    HMdcLookUpTbSec(void) : vertexStat(NULL) {}
    ~HMdcLookUpTbSec(void);
    Bool_t       fillLookUpTb(Int_t m, Int_t l,HMdcTrapPlane& cellPr,
                                               HMdcLookUpTbCell& fCell);
    void         clearPrArrs(void);
    void         clearPrMod(Int_t mod);
    void         clearPrMod(void);
    void         clearPrModInSec(void);
    void         setDrTimeCutYCorr(Double_t corr);
    void         fillClusCat(Int_t mod, Int_t segp, Int_t tpClFndr);
    void         findClusInSeg(Int_t seg);
    void         findClusInSec(void);
    void         findClusInMod(Int_t mod);
    Int_t        getClusterSlot(Int_t seg, HMdcList12GroupCells& list);
    void         fillModWiresList(Int_t mod, HMdcList12GroupCells& list);
    Bool_t       fillModCluster(Int_t mod);
    Bool_t       fillSegCluster(void);
    Bool_t       fillSecCluster(void);
    void         makeModPlot(Int_t mod);
    void         makeSPlot(void);
    void         makeS1PlotAmpCut(void);
    void         makeLayProjV0(void);
    void         makeLayProjV1(void);
    void         makeLayProjV1b(void);
    void         makeLayProjV2(void);
    void         findClusInSeg1(void);
    void         mergeClusInMod(Int_t mod);
    void         mergeClusMod1to2(void);
    void         mergeClusInSeg(void);
    void         mergeClusInSec(void);
    void         testClusMod12toSeg(void);
    void         scanPlotInMod(Int_t mod);
    Bool_t       calcMixedClusterFixedLevel(Int_t nBin);
    Bool_t       calcMixedClusterFloatLevel(Int_t nBin);
    void         scanPlotInSeg1(Int_t seg, UChar_t* plotBAr);
    Bool_t       calcClusterInSecFixedLevel(Int_t nBin);
    Bool_t       calcClusterInSecFloatLevel(Int_t nBin);
    Bool_t       calcClusterInSegFixedLevel(Int_t nBin);
    Bool_t       calcClusterInSegFloatLevel(Int_t nBin);
    Bool_t       calcClusterInSeg1FixedLevel(Int_t nBin);
    Bool_t       calcClusterInSeg1FloatLevel(Int_t nBin);
    void         testSeg1ModClMatching(void);
    Bool_t       calcClusterInModFixedLevel(Int_t nBin);
    Bool_t       calcClusterInModFloatLevel(Int_t nBin);
    void         calcClParam(void);

    void         initCluster(Int_t nBin);
    void         reinitCluster(Int_t nBin);
    void         initCluster(Int_t nBin,UChar_t amp);
    void         reinitCluster(Int_t nBin,UChar_t amp);
    void         addBinInCluster(Int_t nBin,UChar_t wt);
    void         initClusterT2(Int_t nBin,UChar_t amp);
    void         reinitClusterT2(Int_t nBin,UChar_t amp);
    void         addBinInClusterT2(Int_t nBin,UChar_t wt);
    Bool_t       increaseClusterNum(void);
    Bool_t       setLayerVar(void);
    Bool_t       setNextCell(void);
    void         removeGhosts(void);
    void         testBinForVertexF(Int_t bx, Int_t by);
    void         testBinForVertex(Int_t bx, Int_t by);
    void         addToClusCounter(Int_t clusAmp);
    void         markFakesNBins(Int_t clusAmp,Int_t arrSize,Int_t *nUnWiresCut);
    void         markFakes(Int_t clusAmp,Int_t arrSize,Int_t *nRlWiresCut);
    void         markReals(Int_t clusAmp,Int_t arrSize,const Int_t *nUnWiresCut);
    
    inline void  setCurrentArraySec(void);
    inline void  setCurrentArrayMod1(void);
    inline void  setCurrentArrayMod2(void);

  ClassDef(HMdcLookUpTbSec,0)
};

inline void HMdcLookUpTbSec::setCurrentArraySec(void) {
  cClusArr   = clusArr;
  cNClusArr  = &nClsArr;
  clusArrInd = 0;
}

inline void HMdcLookUpTbSec::setCurrentArrayMod1(void) {
  cClusArr   = clusArrM1;
  cNClusArr  = &nClsArrM1;
  clusArrInd = 1;
}

inline void HMdcLookUpTbSec::setCurrentArrayMod2(void) {
  cClusArr   = clusArrM2;
  cNClusArr  = &nClsArrM2;
  clusArrInd = 2;
}

class HMdcLookUpTb : public HParSet {
  protected:
    static HMdcLookUpTb* fMdcLookUpTb;
    TObjArray*           array;           // array of HMdcLookUpTbSec objects
    Bool_t               isCoilOff;
    Int_t                typeClFinder;    // =0 - finding in all mdc in sec.
                                          // =1 - finding in each mdc indep.
                                          // =2 - combinedchamber+chamber clusters
    HMdcGetContainers*   fGetCont;
    HMdcSizesCells*      fSizesCells;
    HMdcGeomPar*         fMdcGeomPar;
    HSpecGeomPar*        fSpecGeomPar;
    HMdcLayerGeomPar*    fLayerGeomPar;
    HStart2GeomPar*      fStartGeomPar;
    HCategory*           fMdcClusCat;     // category of clusters
    Double_t             targLenInc[2];
    Bool_t               quietMode;
    HMdcClFnStack*       stack;           // stack
    HMdcClFnStacksArr*   stacksArr;       // array of stacks
    
    Bool_t               useDriftTime;    //
    Double_t             constUncert;     // Const. part for LookUpTbCell::distCut...
    Double_t             dDCutCorr[4][6]; // Correction for layer cut 
    Double_t             dDistCut;        // Cut:  dDist/yProjUncer < dDistCut*dDCutCorr[mod][lay]
    Double_t             dDistYCorr;      // Cut correction vs Ybin
 
    Int_t                numVFPoins;      // Num. of points for the vertex finder
    Bool_t               use3PointMax;    //
    HGeomVector          targetPnts[250];
    Double_t             vertZErr;        //
    Int_t                vertexStat[250]; // ???
    Int_t                numStartDPoints; // Num. of points of start detector for the vertex finder
    
  public:
    static HMdcLookUpTb* getExObject(void)          {return fMdcLookUpTb;}
    static HMdcLookUpTb* getObject(void);
    static void          deleteCont(void);
    HMdcLookUpTbSec& operator[](Int_t i) {return *static_cast<HMdcLookUpTbSec*>((*array)[i]);}
    Int_t                getSize(void);
    Bool_t               init(HParIo* input,Int_t* set)           {return kTRUE;}
    virtual Bool_t       initContainer(void);
    void                 clear(void);
    void                 setTargLenInc(Double_t lf,Double_t rt);
    void                 setTypeClFinder(Int_t type)              {typeClFinder = type;}
    void                 setIsCoilOffFlag(Bool_t fl)              {isCoilOff = fl;}
    void                 setQuietMode(Bool_t md=kTRUE)            {quietMode = md;}
    void                 fillTrackList(Bool_t fl);
    void                 calcTdcDrDist(void);
    Int_t                findVertex(void);
    Int_t                getNVertexPnts(void) const               {return numVFPoins;}
    Int_t*               getVertexStat(void)                      {return vertexStat;}
    HGeomVector*         getVertexPnts(void)                      {return targetPnts;}

  protected:
    HMdcLookUpTb(const Char_t* name="MdcLookUpTb",
                 const Char_t* title="Cluster finder for MDC plane I&II",
                 const Char_t* context="");
    ~HMdcLookUpTb(void);
    virtual Bool_t       calcPrPlane(Int_t sec);
    virtual Bool_t       calcPlotSize(Int_t sec);
    Bool_t               calcTarget(Int_t sec);
    Bool_t               calcVertexFnTarg(void);

  ClassDef(HMdcLookUpTb,0)
};

#endif  /*!HMDCLOOKUPTB_H*/
