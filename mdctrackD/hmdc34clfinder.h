#ifndef HMDC34CLFINDER_H
#define HMDC34CLFINDER_H

#include "TObject.h"
#include "hlocation.h"
#include "hmdclistgroupcells.h"
#include "hmdclistcells.h"
#include "hmdcgeomobj.h"
#include "hparset.h"
#include "hgeomvector.h"
#include "hmdckickplane.h"
#include "TCutG.h"

class HMdcClus;
class HMdcSeg;
class TH2C;
class HMdcGetContainers;
class HSpecGeomPar;
class HMdcSizesCells;
class HMdcGeomPar;
class HCategory;
class HMdcClFnStack;
class HMdcClFnStacksArr;
class HMdcClustersArrs;
class HMdcCluster;
class HMdcDriftTimeParSec;
class HMdcSizesCellsLayer;
class HMdcClusMetaMatch;
class HMdcTrackParam;
class HMdcKickCor;

//-------Project Plot------------------------------
class HMdcProjPlot : public HMdcPlane {
  protected:
  public:
    Double_t  yMin,yMax;     // The sizes of the plot.
    Double_t  xMin,xMax;
    Double_t  xMinD,xMaxD;   // The small side of the MDC's proj.
    Short_t   nBinX;         //  num.bins along axis X (xMin-xMax)
    Double_t  stX;           //  step X = (xMax-xMin)/nBinX
    Short_t   nBinY;         //  num.bins along axis Y
    Double_t  stY;           //  step Y= (yMax-yMin)/nBinY
    Short_t*  xMinL;         // In units of bin X num.
    Short_t*  xMaxL;         //
    Short_t*  yMinL;         // yMaxL[nx]=nBinY-1
    Double_t* xBinsPos;      // Bins positions
    Double_t* yBinsPos;      //
    Float_t   xFirstBin;     // = xBinsPos[0]
    Float_t   yFirstBin;     // = yBinsPos[0]

    Int_t     size;                // The size of the plModF[] and plModS[]
    static UChar_t* weights;       // array of pr.plot bins weights
    static UChar_t* weights3;      // - first half "weights" array
    static UChar_t* weights4;      // - second half "weights" array
    static Int_t    wtArrSize;     // weights array size
    static UChar_t  nBitsLuTb[64];

    TH2C*     rootPlot;

  public:
    HMdcProjPlot(UChar_t mSeg, Int_t inBinX, Int_t inBinY);
    ~HMdcProjPlot();
    void      setEdges(Double_t iyMin, Double_t ixMinD, Double_t ixMaxD,
                      Double_t iyMax, Double_t ixMin,  Double_t ixMax);
    void      print(void);
    Int_t     xbin(Int_t bin) const        {return bin%nBinX;}
    Int_t     ybin(Int_t bin) const        {return bin/nBinX;}
    Double_t  xBnPos(Int_t bin) const      {return xBinsPos[bin%nBinX];}
    Double_t  yBnPos(Int_t bin) const      {return yBinsPos[bin/nBinX];}
    Short_t   binX(Double_t x) const       {return Short_t((x-xMin)/stX);}
    Short_t   binY(Double_t y) const       {return Short_t((y-yMin)/stY);}
    void      calcCrossBin(const HGeomVector &r,const HGeomVector &dir,Short_t& xb,Short_t& yb) const;
    TH2C*     getPlot(Char_t* name, Char_t* title);
    UChar_t   binAmplitude(Int_t nb) const {return nBitsLuTb[weights3[nb]]+nBitsLuTb[weights4[nb]];}
    void      clearBin(Int_t nb)           {weights3[nb] = weights4[nb]=0;}
    inline UChar_t getBinAmplAndClear(Int_t nb);

  protected:
    HMdcProjPlot(void) {}

    ClassDef(HMdcProjPlot,0)
};

inline UChar_t HMdcProjPlot::getBinAmplAndClear(Int_t nb) {
  UChar_t amp  = nBitsLuTb[weights3[nb]];
  weights3[nb] = 0;
  amp         += nBitsLuTb[weights4[nb]];
  weights4[nb] = 0;
  return amp;
}

//-------Layer-----------------------------
class HMdc34ClFinderLayer: public TObject {
  protected:
  public:
    UChar_t  module;             // For diagnostic print is used only
    UChar_t  layer;              // For diagnostic print is used only
    HMdcSizesCellsLayer *pSCLay;

    // Projection of wire (one for all wires in layer):
    HMdcLayListCells* cells;     // list of fired wires
    Int_t    nCells;             // num.of cells in layer
    Short_t  nBinX;              // projection of wire (one for layer)
    Short_t* yBin;               // Y bins for X bins  0 - nBinX
    Int_t    yFirst;             // yFirst=yBin[0] or [nBinx-1]
    Int_t    nYLines;            // num. of lines
    Int_t    wOrType;            // wires orientation type
    Short_t* xBin1;              // size of xBin array = nYLines + 1
    Short_t* xBin2;              // size of xBin array = nYLines + 1
    Double_t tgY;                // (y1-y2)/(x1-x2) one for all cells
    Double_t tgZ;                // (z1-z2)/(x1-x2) one for all cells
    Double_t y0[2];              // If x,y,z - point on the kick plane, then:
    Double_t z0[2];              // y'=tgY*x+nCell*yStep+y0[n] n=0 left rib (nearest to tag)
                                 // z'=tgZ*x+nCell*zStep+z0[n] n=1 right rib
    Double_t yStep;              //
    Double_t zStep;              //
    Int_t    nPSegOnKick[2][2];  // first index [0]-up rib; [1]-down rib;
                                 // second index [0]-left rib; [1]-right rib;
    Double_t maxDrDist;          // max. drift distance
    Double_t y0w;                // As y0[] but for wire
    Double_t z0w;                // As z0[] but for wire
    Double_t xWirDir;            // Wire vector (not nomalized)
    Double_t yWirDir;            //
    Double_t zWirDir;            //
    
    // For MdcKickCor:
    Double_t yCross;             // For MDCIV is filled and used only!
    
    // Parameters of layer second part:
    UChar_t              layerPart;     // layer part numer (0,1,...)
    Int_t                nextPartFCell; // First cell of the layer next part
    HMdc34ClFinderLayer *layerNextPart;

  public:
    HMdc34ClFinderLayer(Int_t sec, Int_t mod, Int_t lay);
    HMdc34ClFinderLayer(HMdc34ClFinderLayer& prevPart);
    ~HMdc34ClFinderLayer();
    Bool_t createArrayBins(Short_t nBins);
    void   setCellsList(HMdcLayListCells& event) {cells = &event;}
    HMdc34ClFinderLayer* nextLayerPart(Int_t nPartFCell);
    HMdc34ClFinderLayer* getLayerPart(Int_t c);
    Bool_t calcWiresProj(HMdcSizesCellsLayer& fSCellsLay,HGeomVector& pKick,
                         HMdcProjPlot* prPlotSeg2,Int_t firstCell);

    ClassDef(HMdc34ClFinderLayer,0)
};

//-------Mod.-----------------------------

class HMdc34ClFinderMod : public TObject {
  protected:
    TObjArray* array;
  public:
    HMdc34ClFinderMod(Int_t sec, Int_t mod);
    ~HMdc34ClFinderMod();
    HMdc34ClFinderLayer& operator[](Int_t i) {
        return *static_cast<HMdc34ClFinderLayer*>((*array)[i]);
    }
    Int_t getNCells(void);
    void  setCellsList(HMdcModListCells& event);
        
    ClassDef(HMdc34ClFinderMod,0)
};

//-------Sec.------------------------------
class HMdc34ClFinderSec : public TObject {
  protected:
    TObjArray*            array;            // array of HMdc34ClFinderMod objects
    Int_t                 sector;
    HMdcClusMetaMatch    *pMetaMatch;       // !=NULL - do meta matching cut
    Int_t                 lMods[4];         // list of mdc's
    UChar_t               mSeg[2];          // mSeg[0,1] -segment 0,1
    HMdcKickPlane        *fkick;
    HGeomVector           target[2];        // Target in sec.coor.sys.
    HMdcProjPlot         *prPlotSeg2;       // pr.plot of MDC3-4
    HGeomVector           seg1;             // [0-3] -region, [4] - hit
    HGeomVector           dirSeg1;          // directions
    HGeomVector           segRegOnKick[4];  // region of HMdcSeg-hit on the kickpl.
    HGeomVector           segOnKick;        // cross point of inner segment with kickplane
    HGeomVector           errSegOnKick;     // error of segOnKick
    Double_t              al[4];
    Double_t              bl[4];
    Double_t              cl[4];
    Int_t                 nbX[4];

    Int_t                 minAmp[4];        // minAmp[2]-mdc3, minAmp[3]-mdc4
    Int_t                 maxAmp[4];        // number of fired layes in MDC's
    Bool_t                notEnoughWrs;     // =kTRUE - not enough fired wires

    Int_t                 nearbyBins[8];    // Lookuptab. for finding of nearby bins
    HMdcClFnStack        *stack;            // stack
    HMdcClFnStacksArr    *stacksArr;        // array of stacks

    Short_t              *xMinClLines;
    Short_t              *xMaxClLines;
    Int_t                 nLMinCl;
    Int_t                 nLMaxCl;
    Bool_t                isClstrInited;    // flag for "clus" object

    Int_t                 indexPar;         // Index parent track
    Int_t                 indexFCh;         // Index first child
    Int_t                 indexLCh;         // Index last child

    Int_t                *xCMin;            // for cleaning proj. plots
    Int_t                *xCMax;            //
    HMdc34ClFinderMod    *cFMod;            // current module
    HMdc34ClFinderLayer  *cFLay;            // current layer
    Int_t                 nBinX;            // X size of plot

    Int_t                 module;           // var. for making layers proj.
    Int_t                 cell;             // var. for making layers proj.
    Int_t                 layInd;           // = (module-2)*6 + layer;
    Double_t              tdcTDist;         //
    Int_t                 nBinYM2;          //
    Int_t                 shUp;             //
    Int_t                 shDown;           //
    Int_t                 nYLinesL;         //
    Short_t              *xBin1L;           //
    Short_t              *xBin2L;           //
    Int_t                 ny1;              //
    Int_t                 ny2;              //
    Short_t               nbL;              //
    Short_t               nbF;              //
    UInt_t                oneArrLay[6][4];
    UInt_t               *oneArr;
    UInt_t                bitsSetLay[6];    // Bits set for each layer (01010101h,02020202h,...)
    UInt_t                bitsSet;          // Bits set for current layer
    Short_t               sUAr[12][420];    // [(mod-2)*6 + layer][210*2]
    Short_t               sDAr[12][420];    //
    Short_t               cNum[12][420];    //
    HMdc34ClFinderLayer  *pLayPar[12][420]; //
    Int_t                 nPRg[12];         // number of filled elements in arrays
    Short_t              *shUpArr;          // = sUAr[layer_index][region]
    Short_t              *shDnArr;          // = sDAr[layer_index][region]
    Short_t              *cellNum;          // = cNum[layer_index][region]
    Int_t                *numPrRegs;        // = &(nPRg[layer_index])
    HMdc34ClFinderLayer **cFLayArr;         // = pLayPar[layer_index]
    Int_t                 regInd;           // current index in arrayss
    Int_t                 shDownMax;        // to prevent to fill one bin twice from one layer
    UChar_t              *weightsArr;       // current array of bin weights
    UChar_t               seg2MinAmpCut;    // current amp.cut
    HMdcSecListCells     *pListCells;       // list of fired wires in current event

    HCategory            *fClusCat;
    HLocation             locClus;
    Bool_t                isGeant;

    // -- Clusters parameters ---     
    Int_t                 typeClFinder;     // <2 - cl.finding cut for each mdc
                                            // =2  - cl.finding cut for two mdc
    Int_t                 realTypeClFinder; // =2,3 or 0
    
    HMdcClustersArrs     *pClustersArrs;    // clusters arraies
    Int_t                 clusArrSize;      // size of clusArr,clusArrM1,clusArrM2
    HMdcCluster          *clusArr;          // array of clusters parameters
    Int_t                 nClsArr;          // counter of clusters in array pClustersArrs
    HMdcCluster          *cluster;          // current cluster

    HMdcDriftTimeParSec  *pDriftTimeParSec;
    Double_t              dDistCut;         // Cut:  dDist < dDistCut*dDCutCorr[mod][lay]
    Double_t              dDistYCorr;       // Cut correction for y-bins
    Double_t              dDCutCorr[12];    // Correction for layer cut
    Bool_t                useDriftTime;     //
    // Cut for track kick:
    Char_t                useDxDyCut;       //
    TCutG                 cutDxDyArr[36];   // 36 regions of cut
    struct DxDyBinsCut {
      Short_t xBMin;
      Short_t xBMax;
    };
    DxDyBinsCut*          cutXBins[36];     // 36 - number of regions
    Short_t               nYLinesInCut[36];
    Short_t               yLineMin[36];
    Short_t               yLineMax[36];
    Double_t              x0;               // Coor. of cross point of inner segment/cluster
    Double_t              y0;               // with PrPlotSeg2.
    Short_t               xBin0;            // Bin number which cross current
    Short_t               yBin0;            // inner segment/cluster on PrPlotSeg2.
    Int_t                 dXdYCutReg;       // Cut region number (0-8)
    Int_t                 yMinDxDyCut;      // In project plot bins
    Int_t                 yMaxDxDyCut;      // In project plot bins
    Int_t                 yBinToDxDyInd;    // = yBin0+yLineMin[dXdYCutReg]
    
    // Parameters of ghosts removing:
    Char_t                fakeSuppFlag;     // 0-don't do; 1-do; 2-set flag only (don't remove obj.)
    Int_t                 wLev;             // weight of cluster finder level
    Int_t                 wBin;             // weight of bins number in cluster
    Int_t                 wLay;             // weight of cluster amplitude
    Int_t                 dWtCut;           // cut
    
    HMdcKickCor          *pKickCor;

  protected:
    HMdc34ClFinderSec(void) : cFLayArr(NULL) {}
    ~HMdc34ClFinderSec();
    void     makeModS2Plot(Int_t mod);
    void     makeSeg2PlotAmpCut(void);
    Int_t    calcYbin(Int_t upDo,Int_t leRi,Int_t c);
    Int_t    scanPlotSeg2(void);
    Bool_t   calcClusterSeg2FixedLevel(Int_t nBin,UChar_t amp);
    Bool_t   calcClusterSeg2FloatLevel(Int_t nBin,UChar_t amp);
    Bool_t   fillClusterSeg2(void);
    Int_t    findClustersSeg2(void);
    void     initCluster(Int_t nBin,UChar_t amp);
    void     reinitCluster(Int_t nBin,UChar_t amp);
    void     addBinInCluster(Int_t nBin,UChar_t wt);
    Bool_t   increaseClusterNum(void);
    void     mergeClustSeg2(void);
    Int_t    fillClusCat(void);
    Bool_t   calcLayerProjVar(Int_t lay);
    Bool_t   setRegionVar(void);
    void     makeLayProjV1(void);
    void     makeLayProjV2(void);
    void     clearPrSeg2(void);
    Bool_t   testMaxAmp(void);
    void     calcYbinDrTm(Double_t dDCutYCellCorr);
    void     setArrays(Int_t lay);
    void     calcDriftDist(void);
    Double_t calcKickCor(void);
    void     removeGhosts(void);
    void     checkMetaMatch(void);
    void     setFirstLayerPart(Int_t c);
    void     setLayerPart(Int_t c);
    Bool_t   calcXBinsCut(Int_t nReg, TCutG& cutDxDy);
    Int_t    calcCrosses(TCutG &cutDxDy,Double_t yb,DxDyBinsCut& cutB);
    inline void fillBins(Int_t ny);

  public:
    HMdc34ClFinderMod& operator[](Int_t i) {
        return *static_cast<HMdc34ClFinderMod*>((*array)[i]);
    }
    void     clear(void);
    void     setMinBin(Int_t *mBin);
    Bool_t   notEnoughWires(void) const            {return notEnoughWrs;}
//    Int_t    findClustersSeg2(HMdcSeg* fSeg,HMdcClus* pClus,Int_t *mBin=0);
    Int_t    findClustersSeg2(HMdcClus* pClus,Int_t *mBin=0);
    Int_t    findClustersSeg2(HMdcTrackParam *tSeg1, HMdcClus* pClus,Int_t *mBin=0);
    TH2C*    getPlot(Char_t* name,Char_t* title,Int_t ver=0);
    HMdcProjPlot* getPlotSeg2(void)                {return prPlotSeg2;}

    Int_t    getNBinX(void)                        {return prPlotSeg2->nBinX;}
    Int_t    getNBinY(void)                        {return prPlotSeg2->nBinY;}
    void     setTargetF(const HGeomVector& vec)    {target[0]=vec;}
    void     setTargetL(const HGeomVector& vec)    {target[1]=vec;}
    const    HGeomVector& getTargetF(void)         {return target[0];}
    const    HGeomVector& getTargetL(void)         {return target[1];}
    void     setTypeClFinder(Int_t type)           {typeClFinder=type;}
    Int_t    getTypeClFinder(void)                 {return typeClFinder;} 
    void     setCellsList(HMdcSecListCells& event);
    
    HMdc34ClFinderSec(Int_t sec, Int_t inBinX, Int_t inBinY);
    HGeomVector* getTargetArr(void)                {return target;}
    UChar_t *getMSeg(void)                         {return mSeg;}
    void     setClusCut(HCategory* pClCat)         {fClusCat    = pClCat;}
    void     setKickPlane(HMdcKickPlane* pkick)    {fkick       = pkick;}
    void     setXMinClLines(Short_t* xMin)         {xMinClLines = xMin;}
    void     setXMaxClLines(Short_t* xMax)         {xMaxClLines = xMax;}
    void     setClFnStack(HMdcClFnStack* pst)      {stack       = pst;}
    void     setClFnStArr(HMdcClFnStacksArr* psa)  {stacksArr   = psa;}
    Int_t    mdcFlag(Int_t m)                      {return lMods[m];}
    void     doMetaMatch(HMdcClusMetaMatch* pMM)   {pMetaMatch  = pMM;}
    Bool_t   setDxDyCut(TCutG* cutR);
    void     setFakeSupprFlag(Char_t fl)           {fakeSuppFlag = fl;}
    void     resetCounter(void)                    {} //nClsArr = 0;}

     void    setKickCorr(HMdcKickCor *p)           {pKickCor     = p;}
           
    ClassDef(HMdc34ClFinderSec,0)
};

//----------------------------------------------------------
class HMdc34ClFinder : public HParSet {
  protected:
    static HMdc34ClFinder* fMdc34ClFinder;
    TObjArray*             array;           // array of HMdc34ClFinderSec objects
    HMdcGetContainers*     fGetCont;
    HSpecGeomPar*          fSpecGeomPar;
    HMdcSizesCells*        fSizesCells;
    HMdcKickPlane          kickPlane;
    HMdcGeomPar*           fMdcGeomPar;
    HCategory*             fMdcClusCat;
    Short_t*               xMinClLines;
    Short_t*               xMaxClLines;
    HMdcClFnStack*         stack;           // stack
    HMdcClFnStacksArr*     stacksArr;       // array of stacks
    static Bool_t          quietMode;
    HMdcClusMetaMatch*     pMetaMatch;      // !=NULL - do meta matching cut
    
    TCutG                  cutDxDyArr[36];  // 36 regions of cut
    Bool_t                 useDxDyCut;      //
    
    Bool_t                 useKickCor;      // kTRUE - use correction from HMdcKickCor class
    HMdcKickCor           *pKickCor;
    
  public:
    static HMdc34ClFinder* getExObject(void);
    static HMdc34ClFinder* getObject(void);
    static void            deleteCont(void);
    static void            setQuietMode(Bool_t quiet) {quietMode=quiet;}
    static Bool_t          getQuietMode()             {return quietMode;}
    HMdc34ClFinderSec& operator[](Int_t i) {
      return *static_cast<HMdc34ClFinderSec*>((*array)[i]);
    }
    Bool_t init(HParIo* input,Int_t* set)             {return kTRUE;}
    Bool_t initContainer(HMdcEvntListCells& event);
    void   setCellsList(HMdcEvntListCells& event);
    void   clear(void);
    void   doMetaMatch(HMdcClusMetaMatch* pMM)        {pMetaMatch = pMM;}
    static Int_t calcDxDyCutRegion(const HGeomVector& pnt);
    void   printClFinderParam(void);
    
  protected:
    HMdc34ClFinder(const Char_t* name    = "Mdc34ClFinder",
                   const Char_t* title   = "Cluster finder for outer MDCs",
                   const Char_t* context = "");
    ~HMdc34ClFinder();
    Bool_t   calcTarget(Int_t sec);
    Bool_t   calcProjPlaneSeg2(Int_t sec);
    Bool_t   calcSizePlotSeg2(Int_t sec);
    Bool_t   calcWiresProj(Int_t sec);
    void     calcCrossLines(HGeomVector& p1l1, HGeomVector& p2l1,
                            HGeomVector& p1l2, HGeomVector& p2l2,HGeomVector& cross);
    Double_t xLine(HGeomVector& p1, HGeomVector& p2, Double_t yi);

    ClassDef(HMdc34ClFinder,0)
};

// inline void HMdc34ClFinderSec::fillBins(Int_t ny) {
// //   UChar_t* wt    = weightsArr + nBinX*ny;
// //   UChar_t* wtEnd = wt + nbL;
// //   for(wt+=nbF; wt<=wtEnd; wt++) (*wt)++; // From one layer only once !!!
//   
//   Int_t   nb    = nbL-nbF+1;
//   Int_t   nbR   = nb&3; // = nb%4;
//   UInt_t* wt4    = (UInt_t*)(weightsArr+nbF + ny*nBinX); //wt;
//   UInt_t* wt4Max = wt4 + (nb>>2); //((nbL-nbF+1)>>2);
//   for(; wt4<wt4Max; wt4++) (*wt4) += 16843009; // += 01010101h
// /*  if(nbR>0)*/ (*wt4) += oneArr[nbR];
// }

inline void HMdc34ClFinderSec::fillBins(Int_t ny) {
//   UChar_t* wt    = weightsArr + nBinX*ny;
//   UChar_t* wtEnd = wt + nbL;
//   for(wt+=nbF; wt<=wtEnd; wt++) (*wt)++; // From one layer only once !!!
  
  Int_t   nb    = nbL-nbF+1;
  Int_t   nbR   = nb&3; // = nb%4;
  UInt_t* wt4    = (UInt_t*)(weightsArr+nbF + ny*nBinX); //wt;
  UInt_t* wt4Max = wt4 + (nb>>2); //((nbL-nbF+1)>>2);
  for(; wt4<wt4Max; wt4++) (*wt4) |= bitsSet;
/*  if(nbR>0)*/ (*wt4) |= oneArr[nbR];
}

#endif  /*!HMDC34CLFINDER_H*/
