//*-- Author : Vladimir Pechenov 24.11.2011

#ifndef __HALIGNMENTMETA_H__
#define __HALIGNMENTMETA_H__

#include "TObject.h"
#include "TMath.h"
#include "hgeomtransform.h"

class TFile;
class TNtuple;

class HAlignmentMeta:public TObject {
  
  private:
    struct TrackMdcMeta {
      HGeomVector mdcPnt1Sec;
      HGeomVector mdcPnt2Sec;
      Double_t    xMeta;        // In the coor.sys. of meta-module
      Double_t    yMeta;        // In the coor.sys. of meta-module
      Double_t    zMeta;        // In the coor.sys. of meta-module
      Double_t    xMinDistInit;
      Double_t    yMinDistInit;
      Double_t    xMinDist;
      Double_t    yMinDist;
      Double_t    minDist2;         // chi2 or minDist^2
      Double_t    sigmaX;
      Double_t    sigmaY;
      Double_t    sigmaZ;
      Double_t    wt;
      Int_t       startTrInd;
      Short_t     metaMod;
      Short_t     binScWt;
      Short_t     column;           // RPC,Shower-column, TOF- =0
      Short_t     cell;             // RPC,TOF - cell, Shower - row
      Short_t     cellInd;          // For RPC = column*32+cell; for TOF = module*8+cell
      Bool_t      useIt;
      Bool_t      oneLay(TrackMdcMeta &t) const {return TMath::Abs(t.zMeta-zMeta) < 2.; }
      Double_t    dXNorm(void) const            {return xMinDist/sigmaX;}
      Double_t    dYNorm(void) const            {return yMinDist/sigmaY;}
    };
    TrackMdcMeta    tracks[1000000];

    Int_t           metaDetector;    // 0-TOF, 1-Shower, 2-RPC
    Double_t        yMinMetaLocal;
    Double_t        yMaxMetaLocal;
    Int_t           alignSec;
    Int_t           nTracks;
    Double_t        trackSelecCutX;
    Double_t        trackSelecCutY;
    Bool_t          filterFlag;

    HGeomTransform  transMetaModLabOld[8];  // 8 - for tof
    HGeomTransform  transMetaModLabNew[8];  // 8 - for tof
    HGeomTransform  transMetaModSecOld[8];  // 8 - for tof
    HGeomTransform  transMetaModSecNew[8];  // 8 - for tof
    Int_t           nMetaModules;
    Bool_t          fitTofModYPos;
    Double_t        tofModYSh[8];
    Int_t           cellStat[192];  // 192 = 32*6
    Double_t        cellXCorr[192];
    Int_t           nCells;
    Int_t           nCellsTot;
    Double_t        xShitfRpc;
    Bool_t          calcCellXOffset;

    TNtuple        *nt;
    Float_t         sec;
    Float_t         x1, y1, z1, x2, y2, z2;
    Float_t         metaModule;
    Float_t         metaColumn,metaCell;
    Float_t         xMetaLocal,yMetaLocal,zMetaLocal;
    Float_t         xRMS,yRMS,zRMS;

    Double_t        meanX;
    Double_t        sigmX;
    Double_t        meanY;
    Double_t        sigmY;
    Double_t        meanZ;
    Double_t        sigmZ;
    Bool_t          isFirstSIter;

    void            fillArray(void);
    Bool_t          selectTracksIter(Double_t nSigmasCut);
    void            selectTracks(Double_t nSigmasCut);
    void            calcMinDist(void);
    void            calcMinDist(Double_t *par);
    void            calcMinDist(HGeomTransform& trans);
    void            setWeights(void);
    void            setNtuple(TNtuple *nt);
    void            calcXOffset(Double_t nSigmasCut);
    Bool_t          calcXOffset(Double_t nSigmasCut,Short_t cellInd);

  public:
    HAlignmentMeta();
    virtual ~HAlignmentMeta();

    void            doFiltering(Bool_t st)              {filterFlag = st;}
    void            alignMeta(Int_t sec,TNtuple *nt);

    void            checkAlignment(void);                                    

    Int_t           getAlignSec()                       {return alignSec;} 
    void            setCuts(Double_t cutX,Double_t cutY);
    Double_t        getMinFunction(Double_t *par);
    static void     fcnMeta(Int_t &npar, Double_t *gin, Double_t &fn, Double_t *par, Int_t iflag);
    HGeomTransform* getArrTansOld(void)                 {return transMetaModLabOld;}
    HGeomTransform* getArrTansNew(void)                 {return transMetaModLabNew;}
    void            setNMetaModules(Int_t nm)           {nMetaModules = nm;}
    void            setRpcDetector(Double_t cutX=2.4,Double_t cutY=2.2);
    void            setShowerDetector(Double_t cutX=2.0,Double_t cutY=2.0);
    void            setTofDetector(Double_t cutX=3.6,Double_t cutY=2.6);
    void            fitTofModYPositions(Bool_t fl)      {fitTofModYPos = fl;}
    Double_t        getRpcXCorr(Int_t co,Int_t ce)      {return cellXCorr[co*nCells+ce];}
    Double_t        getTofXCorr(Int_t mo,Int_t ce)      {return cellXCorr[mo*nCells+ce];}
    void            calcRpcCellXOffset(void)            {calcCellXOffset = kTRUE;}

     ClassDef(HAlignmentMeta,0)
    };   
#endif  /* !__HALIGNMENTMETA_H__ */
    
