#ifndef HMDCCOSMICSCANDIDATE_H
#define HMDCCOSMICSCANDIDATE_H

#include "hreconstructor.h"
#include "TString.h"
#include "TRandom.h"
#include "hlocation.h"
#include "hgeomvector.h"

using namespace std;
class TH1F;
class TH2F;
class TFile;
class TLine;
class HIterator;
class HCategory;
class HTofHit;
class HTofCluster;
class HMdcSizesCells;
class HMdcSizesCellsLayer;
class HMdcGetContainers;
class HGeomTransform;
class HMdcList12GroupCells;
class HMdcClus;

class HMdcCosmicsCandidate : public HReconstructor {
  protected:
    struct MetaHit {
      Int_t       sec;
      Float_t     tof;
      HGeomVector hit;
      Bool_t      isTOF;    // kTRUE - TOF detector; kFALSE - RPC detector
    };
    MetaHit            metaHits[250];
    Int_t              nMetaHits;
    Int_t              nMetaHitSec[6];
    Int_t              nSectors;
    Float_t            zTofShift;          // Shift z position of TofCluster/Hit on this value
    Float_t            zRpcShift;          // Shift z position of RpcCluster/Hit on this value
    Bool_t             isGeant;
    
    Int_t              nWiresStat[6][4];
    Int_t              numWiresAllMod;
    
    // Track candidate line:
    HGeomVector        hit1;              // Meta hit in sector coor.sys
    HGeomVector        hit2;              // Meta hit in sector coor.sys
    
    struct TrackCand {
      Int_t            metaInd1;          // Track line
      Int_t            metaInd2;
      Char_t           modList[6];
      Int_t            nMods;             // number of modules crossed by track cand.
      Int_t            nSecs;             // number of sectors crossed by track cand.
      // Track quality:
      Int_t            nModules;          // Number of fired modules
      Int_t            nLayers;           // Number of fired layers
      Int_t            nModSec[6];        // Number of fired layers in sector
      Int_t            nLaySec[6];        // Number of fired layers in sector
      Int_t            nWires;            // Number of wires
      Double_t         dCellMean;
      Int_t            nWiresTot;         // Number of fired wires in the crossed modules
      void copy(TrackCand &tc) {
        metaInd1 = tc.metaInd1;
        metaInd2 = tc.metaInd2;
        for(Int_t s=0;s<6;s++) {
          modList[s] = tc.modList[s];
          nModSec[s] = tc.nModSec[s];
          nLaySec[s] = tc.nLaySec[s];
        }
        nMods     = tc.nMods;
        nSecs     = tc.nSecs;
        nModules  = tc.nModules;
        nLayers   = tc.nLayers;
        nWires    = tc.nWires;
        dCellMean = tc.dCellMean;
        nWiresTot = tc.nWiresTot;
      }
      void init(Int_t i1,Int_t i2) {
        metaInd1  = i1;
        metaInd2  = i2;
        nMods     = 0;
        nSecs     = 0;
        nModules  = 0;
        nLayers   = 0;
        nWires    = 0;
        dCellMean = 1.e+30;
        nWiresTot = 0;
        for(Int_t s=0;s<6;s++) modList[s] = nModSec[s] = nLaySec[s] = 0;
      }
      Bool_t trackStatus(void)                 {return nModules>=2 && nLayers>=5 /*|| nWires>=5*/;}
      void   addMod(Int_t s,Int_t m,Int_t nWr) { 
        if(modList[s]==0) nSecs++;
        modList[s] |= 1<<m;
        nMods++;
        nWiresTot += nWr;
      }
      void   unsetMod(Int_t s,Int_t m)         {if(modStatus(s,m)!=0) {
                                                  modList[s] ^= (1<<m);
                                                  if(modList[s] == 0) nSecs--;} }
      Bool_t secStatus(Int_t s) const          {return modList[s] != 0;}
      Char_t segStatus(Int_t s,Int_t sg) const {return (modList[s]>>(sg*2)) & 3;}
      Char_t modStatus(Int_t s,Int_t m) const  {return (modList[s]>>m) & 1;}
      Int_t  numWiresOutTrack(void) const      {return nWiresTot-nWires;}
      Double_t selectQual(void) const          {return Double_t(nWires)/Double_t(nWiresTot);}
      Int_t  numLayersPerMod(void) const       {return nLayers/nModules;}
    };
    TrackCand          bestTrCand;
    TrackCand          currTrCand;

    // Cut parameters"
    Double_t           betaMin;
    Double_t           betaMax;
    Float_t            dCellWind[4];       // [mod] window of cell-cell_calc
    Int_t              nLayInModCut;       // numFiredLayers/numModInTrack must be >= meanNLayCutu
    Int_t              maxNWiresOutTr;     // maximal num.of wires out of track in the crossed modules
    Double_t           selQualCut;         // cut for numWiresInTrack/totalNunWiresInCrossedModules
    
    Int_t              nEventsTot;
    Int_t              nEventsSel;
    Int_t              nSelTr[9];          // index is number of MDC modules in track
                                           // nSelTr[index] - number of tracks with "index" number of MDCs
    Double_t           trackScaling[9];    // index is number of MDC modules in track
                                           // trackScaling[index] - take this fraction 
                                           // (=1.-all, =0.5-half, ...) of tracks with "index" number of MDCs
    Int_t              nSelTrSc[9];
    Int_t              nEventsSelSc;
    
    TRandom            rndm;
    
    HCategory         *pCatTof; 
    UChar_t            useTofCat;
    HCategory         *pCatRpc;
    UChar_t            useRpcCat;
    HCategory         *pCatMdcCal1;
    HLocation          locCal1;
    HCategory         *pCatMdcClus;
    HLocation          locClus;
    HMdcSizesCells*    sizes;              //! comment to fill in.
    HMdcGetContainers* pGetCont;
    Int_t              index;
    Int_t              returnFlag;
    Int_t              mdcsetup[6][4];
    Bool_t             setParContStat;
    
    // Layer size in layer coor.system. Due to symmmetry 2 points enough:
    struct LayerSize {
      Double_t             y1;             // minimal value
      Double_t             y2;             // maximal value
      Double_t             x2;             // correspond y2
      Double_t             inc;            // (x1-x2)/(y1-y2);
      HMdcSizesCellsLayer *pScLay;
    };
    LayerSize          laySize[4];         // [MDCplane][0-layer1; 1-layer6]
    
    TH1F              *hsBeta;
    TH2F              *plStat;
    TH2F              *plQualVsWrLo;
    TH1F              *hsDCell[6][4];
    TH1F              *hsDCellL[6][4][6];

    Bool_t             makeHists;
    TFile             *histFile;
    TString            histFileName;
    TString            histFileOption;

  private:
    void               printParam(void);
    Bool_t             createHists(void);
    void               initVariables(void);
    void               collectTofHits(void);
    void               collectRpcHits(void);
    Bool_t             testBeta(const MetaHit &h1,const MetaHit &h2,Bool_t mHists=kFALSE);
    HMdcClus*          fillCluster(Int_t sec,Int_t seg,HMdcList12GroupCells &listCells,HMdcClus* prevClus);
    Bool_t             fillTrack(TrackCand &bestCand);
    Bool_t             testTrack(void);
    Bool_t             isCrossMod(Int_t sec,Int_t mod);
    void               testSectors(TrackCand &trackCand,Int_t sec,Int_t part=0);
    Bool_t             calcSectors(TrackCand &trackCand);
    
  public:
    HMdcCosmicsCandidate(const Text_t *name, const Text_t *title);
    HMdcCosmicsCandidate(void);
    ~HMdcCosmicsCandidate(void);

    virtual Bool_t     init(void);
    virtual Bool_t     reinit(void);
    virtual Bool_t     finalize(void);
    virtual Int_t      execute(void);

    void               setBetaCut(Double_t min,Double_t max);
    void               setDCellWindow(Float_t wM1,Float_t wM2,Float_t wM3,Float_t wM4);
    void               setNLayPerModCut(Int_t cut)      {nLayInModCut = cut;};
    void               setSelectionCut(Int_t maxLW,Double_t qu) {maxNWiresOutTr = maxLW;
                                                                 selQualCut     = qu;}

    void               makeHistograms(Bool_t fl=kTRUE)  {makeHists = fl;}
    void               setHistFile(const Char_t* dir="./",const Char_t* fileNm="cosmCandHists",
                                   const Char_t* opt="NEW");
    void               setSkipEvent(Bool_t fl=kTRUE)    {returnFlag = fl ? kSkipEvent : 0;}
    void               scaleDownTrack(UInt_t nMdcTr,Double_t sc);
    void               scaleDownTrack(Double_t *sca);
    void               setParContainersStatic(void)     {setParContStat = kTRUE;}
    void               setZShiftForTof(Float_t sh)      {zTofShift = sh;}
    void               setZShiftForRpc(Float_t sh)      {zRpcShift = sh;}
    void               useTofHits(void)                 {useTofCat = 1;}
    void               useRpcHits(void)                 {useRpcCat = 1;}
    void               useTofClusters(void)             {useTofCat = 2;}
    void               useRpcClusters(void)             {useRpcCat = 2;}
 
  ClassDef(HMdcCosmicsCandidate,0)
};

class HCosmicCalibEvSkip : public HReconstructor {
  public:
    HCosmicCalibEvSkip(void);
    HCosmicCalibEvSkip(const Text_t *name, const Text_t *title);
    ~HCosmicCalibEvSkip(void) {}
    virtual Bool_t     init(void)     {return kTRUE;}
    virtual Bool_t     reinit(void)   {return kTRUE;}
    virtual Bool_t     finalize(void) {return kTRUE;}
    virtual Int_t      execute(void);
  
   ClassDef(HCosmicCalibEvSkip,0)
};
  
#endif
