#ifndef HMDCCLUSTER_H
#define HMDCCLUSTER_H

#include "TObject.h"
#include "hmdclistgroupcells.h"

class HMdcClus;
class HMdcSecListCells;

class HMdcCluster : public TObject {
  protected:
    HMdcList12GroupCells lCells1;
    HMdcList12GroupCells lCells2;
    Char_t       sector;
    Char_t       segment;     // =0-lCells1 is used; =1-lCells2; -1 - lCells1&2
    Int_t        numCells1;   // number of cells in lCells1
    Int_t        numCells2;   // number of cells in lCells2
    Int_t        nLayers1;    // number of layers in lCells1
    Int_t        nLayers2;    // number of layers in lCells2
    Bool_t       status;      // =kFALSE - removed by merging
    Char_t       flag;        // for combined clusters, >0 - cluster saved
    HMdcCluster* clusMerg;    // clusMerg=0 if status=kTRUE else -merged cluster
    Short_t      nMergedClus; // Number of merged clusters

    Int_t        nBins;       // num. of bins in cluster
    Int_t        sumWt;       // sum weights of bins
    Int_t        meanX;       // var. for calculation of cluster shape
    Int_t        meanY;       // -/-
    Int_t        meanXX;      // -/-
    Int_t        meanYY;      // -/-
    Int_t        meanYX;      // -/-
    Int_t        meanXWt;     // position of cluster (calc. with weights)
    Int_t        meanYWt;     // -/-
    Int_t        meanXXWt;    // X dispersion
    Int_t        meanYYWt;    // Y dispersion
    Int_t        minWt;       // minimal value of Wt
    Int_t        iXFirst;
    Int_t        iYFirst;
    Float_t      x0;          // x of the first bin
    Float_t      y0;          // y of the first bin
    Float_t      xSt;         // bin size
    Float_t      ySt;         // bin size
    UChar_t      fakeFlag;    // flag of fake suppression code
    UChar_t      realFlag;    // flag of fake suppression code
    
    Float_t      x;           // cluster position
    Float_t      y;           //
    Float_t      errX;        // position errors
    Float_t      errY;        //
    Float_t      sigma1;      // cluster shape
    Float_t      sigma2;      //
    Float_t      alpha;       //
    
    // Variables for matching:
    HMdcCluster* clusMod1;    // mod.1 cluster address
    HMdcCluster* clusMod2;    // mod.2 cluster address
  public:
    HMdcCluster(void) {}
    ~HMdcCluster(void) {}
    void init(Char_t sec,Char_t seg,Float_t x,Float_t y,Float_t sx,Float_t sy);
    void clearBinStat(void);
    inline void addBin(Int_t nx,Int_t ny,UChar_t wt);
    void calcXY(void);
    void addClus(HMdcCluster& clst2);
    void sumClus(HMdcCluster& clst1, HMdcCluster& clst2);
    void calcClusParam(void);
    void fillClus(HMdcClus* clus, Int_t nLst, Bool_t fillTrList);
    void print(void);
    void setModCluster(Int_t im, HMdcCluster* cl); 
    Char_t        getSegment(void) const            {return segment;}
    HMdcCluster*  getModCluster(Int_t im)           {return im==0 ? clusMod1 :
                                                                    clusMod2;}
    HMdcCluster  *getMod1Clus(void)                 {return clusMod1;}
    HMdcCluster  *getMod2Clus(void)                 {return clusMod2;}
    Char_t        getIOSeg(void) const              {return segment;}
    Int_t         getNBins(void) const              {return nBins;}
    void          setMod1Clus(HMdcCluster* cl)      {clusMod1=cl;}
    void          setMod2Clus(HMdcCluster* cl)      {clusMod2=cl;}
    HMdcCluster  *getClusMerg(void) const           {return clusMerg;}
    void          setClusMerg(HMdcCluster* cl)      {clusMerg=cl;}
    void          incFlag(void)                     {flag++;}
    HMdcList12GroupCells& getLCells1(void)          {return lCells1;}
    HMdcList12GroupCells& getLCells2(void)          {return lCells2;}
    Bool_t        getStatus(void) const             {return status;}
    void          setStatus(Bool_t st)              {status = st;}
    Char_t        getFlag(void) const               {return flag;}
    void          setFlag(Char_t fl)                {flag=fl;}
    Float_t       getX(void) const                  {return x;}
    Float_t       getY(void) const                  {return y;}
    Int_t         getMinWt(void) const              {return minWt;}
    Int_t         getRealLevel(void) const          {return minWt;}
    Int_t         testForInclude(HMdcCluster& cl2);
    Int_t         testForInc34CFnd(HMdcCluster& cl2);
    Bool_t        testNLayersSeg1(void);
    Bool_t        testNLayersSeg2(void);
    void          setFakeFlag(UChar_t fl)           {fakeFlag = fl;}
    void          setFakeFlagAndStatus(UChar_t fl)  {fakeFlag = fl; status = kFALSE;}
    UChar_t       getFakeFlag(void) const           {return fakeFlag;}
    UChar_t      &fakeFlagS(void)                   {return fakeFlag;}
    UChar_t       getRealFlag(void) const           {return realFlag;}
    UChar_t      &realFlagS(void)                   {return realFlag;}
    void          setMod1ClusSkipMerg(HMdcCluster* clusM1);
    void          setMod2ClusSkipMerg(HMdcCluster* clusM2);
    Bool_t        isModClusNotTheSame(HMdcCluster& cls2);
    Int_t         getNUniqueWiresSeg1(HMdcSecListCells* pListCells);
    Int_t         getNUniqueAndRWiresSeg1(HMdcSecListCells* pListCells,Int_t& nRWires);
    inline Bool_t isClusterAmpEq(Int_t amp) const;
    void          resetFakeFlagSeg1(Bool_t fake,HMdcSecListCells* pListCells);
    void          resetRealFlagSeg1(Bool_t real,HMdcSecListCells* pListCells);
    
  protected:
    void         correctContent(Int_t iXf,Int_t iYf);
  ClassDef(HMdcCluster,0)
};

inline void HMdcCluster::addBin(Int_t nx,Int_t ny,UChar_t wt) {
  if(nBins == 0) {
    iXFirst = nx;
    iYFirst = ny;
    minWt   = wt;
  } else {
    nx       -= iXFirst;
    meanX    += nx;
    meanXWt  += nx*wt;
    meanXX   += nx*nx;
    meanXXWt += nx*nx*wt;
    ny       -= iYFirst;
    meanY    += ny;
    meanYWt  += ny*wt;
    meanYY   += ny*ny;
    meanYYWt += ny*ny*wt;
    meanYX   += ny*nx;    // For cluster shape calc. only
    if(wt<minWt) minWt = wt;
  }
  sumWt += wt;
  nBins++;
}

inline Bool_t HMdcCluster::isClusterAmpEq(Int_t amp) const {
  // For ghosts removing in inner segment
  return minWt==amp && (fakeFlag==0 || fakeFlag > 9);
  //                    fakeFlag>0 && < 10 suppreset in testForInclude
}

//==================================================================
class HMdcClustersArrs : public TObject {
  protected:
    static HMdcClustersArrs* pMdcClustersArrs;
    HMdcCluster* clusArr[3];
    Int_t        arrSize[3];
    
    //
    Float_t      dXWind1;      // window for clusters comparison in seg.1
    Float_t      dYWind1;      // window for clusters comparison in seg.1
    Float_t      dXWind2;      // window for clusters comparison in seg.1\2
    Float_t      dYWind2;      // window for clusters comparison in seg.2
    
  public:
    static HMdcClustersArrs* getExObject(void)   {return pMdcClustersArrs;}
    static HMdcClustersArrs* getObject(void);
    static void deleteCont(void);
    HMdcClustersArrs(void);
    ~HMdcClustersArrs(void);
    Int_t        createAllArrays(Int_t size=500);
    HMdcCluster* createArray1(Int_t size=500)    {return createArray(0,size);}
    HMdcCluster* createArray2(Int_t size=500)    {return createArray(1,size);}
    HMdcCluster* createArray3(Int_t size=500)    {return createArray(2,size);}
    HMdcCluster* getArray1(void)                 {return clusArr[0];}
    HMdcCluster* getArray2(void)                 {return clusArr[1];}
    HMdcCluster* getArray3(void)                 {return clusArr[2];}
    Int_t        getArray1Size(void)             {return arrSize[0];}
    Int_t        getArray2Size(void)             {return arrSize[1];}
    Int_t        getArray3Size(void)             {return arrSize[2];}
    HMdcCluster* getArr1Cluster(Int_t ind)       {return getCluster(0,ind);}
    HMdcCluster* getArr2Cluster(Int_t ind)       {return getCluster(1,ind);}
    HMdcCluster* getArr3Cluster(Int_t ind)       {return getCluster(2,ind);}
    void         setDXDYWindow(Float_t dx,
                               Float_t dy)       {dXWind1 = dx; dYWind1 = dy;}
    void         setM34DXDYWindow(Float_t dx,
                                  Float_t dy)    {dXWind2 = dx; dYWind2 = dy;}
    void         testCluster(Int_t indArr,Int_t indClus);
    Bool_t       testMdc34Cluster(Int_t indArr,Int_t indClus,Int_t firstClst=0);
  protected:
    HMdcCluster* createArray(Int_t ind,Int_t size);
    HMdcCluster* getCluster(Int_t indarr,Int_t  indClus);
  ClassDef(HMdcClustersArrs,0)
};

#endif  /*!HMDCCLUSTER_H*/
