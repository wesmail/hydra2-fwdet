#ifndef HMETAMATCHF2_H
#define HMETAMATCHF2_H
#include "hreconstructor.h"
#include "hgeomvector.h"
#include "hlocation.h"
#include "hmdckickplane.h"


class HCategory;
class HMdcSeg;
class HTofCluster;
class HTofHit;
class HRpcCluster;
class HShowerHit;
class HEmcCal;
class HEmcCluster;
class HIterator;
class HMdcTrkCand;
class HMdcTrackGSpline;
class HMetaMatch2;
class HGeomTransform;
class HMdcGetContainers;
class HRichHit;
class HTofGeomPar;
class HRpcGeomPar;
class HEmcGeomPar;
class HModGeomPar;
class HShowerGeometry;
class HMetaMatchPar;
class HMdcSizesCells;

#define RICH_BUF 3
#define MMF_BUF 5

class HMetaMatchF2 : public HReconstructor {
  private:
    HMdcSizesCells    *pSizesCells;
    // Cut parameters:
    HMetaMatchPar     *fMatchPar;
    Float_t            invSigma2TofX[6];        // [6] - for 6 sectors
    Float_t            invSigma2TofY[6];
    Float_t            sTofX[6];
    Float_t            sTofY[6];
    Float_t            quality2TOFCut[6];
    Float_t            sigma2MdcInRpcX[6];
    Float_t            sigma2MdcInRpcY[6];
    Float_t            sigma2MdcInShrX[6];
    Float_t            sigma2MdcInShrY[6];
    Float_t            sigma2MdcInEmcX[6];
    Float_t            sigma2MdcInEmcY[6];
    Float_t            sShowerX[6];
    Float_t            sShowerY[6];
    Float_t            sRpcX[6];
    Float_t            sRpcY[6];
    Float_t            sEmcX[6];
    Float_t            sEmcY[6];
    Float_t            quality2RPCCut[6];
    Float_t            quality2SHOWERCut[6];
    Float_t            quality2EMCCut[6];
    Float_t            dThRich[6];              // matching window for 6 sectors
    Float_t            dPhRich[6];              // ...
    Float_t            dPhRichOff[6];           // ...
    Float_t            qualityRichCut[6];       //
    Float_t            richThetaMinCut[6];
    Float_t            richThetaMaxCut[6];
    Float_t            sigmaEmc;
    
    const HGeomTransform *labTrans[6];
    HMdcGetContainers *fGetCont;
    HMetaMatch2       *meta;
    HCategory         *fCatTrkCand;
    HCategory         *fCatMetaMatch;
    HCategory         *fCatMdcSeg;
    HCategory         *fCatTof;
    HCategory         *fCatTofCluster;
    HCategory         *fCatShower;
    HCategory         *fCatRich;
    HCategory         *fCatRpcCluster;
    HCategory         *fCatEmc;
    HCategory         *fCatEmcCluster;

    HMdcKickPlane      kickplane;
    HMdcTrackGSpline  *pSpline;
    HIterator         *fTrkCandIter;
    HIterator         *iterTof;
    HIterator         *iterTofCluster;
    HIterator         *iterShower;
    HIterator         *iterRich;
    HIterator         *iterRpcCluster;
    HLocation          sectorLoc;
    
    HTofGeomPar       *fTofGeometry;
    HShowerGeometry   *fShrGeometry;
    HRpcGeomPar       *fRpcGeometry;
    HEmcGeomPar       *fEmcGeometry;

    Int_t              nRpcClusters[6];
    HRpcCluster*       fRpcClusters[6][200];
    
    Int_t              nShowerHits[6];
    HShowerHit*        fShowerHits[6][200];
    Short_t            indexShrHit[6][200];

    Int_t              nTofHits[6];
    HGeomVector        tofHits[6][100];
    Char_t             tofModule[6][100];       // Tof module of hit
    Short_t            indexTofHit[6][100];
    Char_t             tofClustSize[6][100];    // 0 - HTofHit obj., >=1 - HTofCluster

    Int_t              nEmcHits[6];
    HEmcCal*           fEmcHits[6][160];
    Short_t            indexEmcHit[6][160];
    Int_t              nEmcClusters[6];
    HEmcCluster*       fEmcCluster[6][160];
    Short_t            indexEmcCluster[6][160];
    
    HMdcSeg           *segments[2];
    HGeomVector        mdcTrackPar[4];
    UChar_t            nTofMatched;             // Number of matched tof hits/clusters
    Float_t            qual2TofBestAr[MMF_BUF]; // Array of best quality of cluster,hit1 and hit2
    Float_t            qual2TofAr[MMF_BUF][9];  // Matchung quality^2,dx,dx of clust.,hit1 and hit2
    Short_t            tofInd[MMF_BUF][3];      // [][0]-HTofCluster index, [][1,2]-HTofHit index
    UChar_t            nShrMatched;             // Number of matched shower hits
    Float_t            qual2ShrAr[MMF_BUF][3];  // [][0]-quality^2,[][1]-dx,[][2]-dy
    Short_t            shrInd[MMF_BUF];         // HShowerHit indexis
    UChar_t            nRpcMatched;             // Number of matched rpc clusters
    Float_t            qual2RpcAr[MMF_BUF][3];  // [?][0]-quality^2,[?][1]-dx,[?][2]-dy
    Short_t            rpcInd[MMF_BUF];         // HRpcCluster indexis
    
    UChar_t            nEmcMatched;             // Number of matched shower hits
    Float_t            qual2EmcAr[MMF_BUF][3];  // [][0]-quality^2,[][1]-dx,[][2]-dy
    Short_t            emcInd[MMF_BUF];         // HEmcCal indexis
    UChar_t            nEmcClusMatched;         // Number of matched shower hits
    Float_t            qual2EmcClusAr[MMF_BUF][3];  // [][0]-quality^2,[][1]-dx,[][2]-dy
    Short_t            emcClusInd[MMF_BUF];     // HEmcCal indexis

    UChar_t            nRichId;                 // Number of matched HRichHit
    Short_t            richInd[RICH_BUF];       // indexis
    Float_t            qualRich[RICH_BUF];      // Matching quality
    
    Int_t              sector;                  // current sector                           
    Int_t              nShowerHitsSec;          // == nShowerHits[sec] for current sector
    HShowerHit       **fShowerHitsSec;          // ...
    Short_t           *indexShrHitSec;          // ...
    
    Int_t              nRpcClustersSec;         // == nRpcClusters[sec] for current sector
    HRpcCluster      **fRpcClustersSec;         // ...
    Short_t           *indexRpcClustersSec;     // ...
    HGeomVector        p1SegInMod;              // Segment point 1 in rpc or shower module coor.sys.
    HGeomVector        p2SegInMod;              // Segment point 2 in rpc or shower module coor.sys.
    Double_t           invDZ;                   // = 1./(p1SegInMod.Z()-p2SegInMod.Z())
    
    Int_t              nTofHitsSec;             // == nTofHits[sec] for current sector    
    HGeomVector       *tofHitsSec;              // ...
    Short_t           *indexTofHitSec;          // ...
    Char_t            *tofClustSizeSec;         // ...
    Char_t            *tofModuleSec;            // ...
    const HGeomTransform* secLabTrans;
    
    Int_t              nEmcHitsSec;             // == nEmcHits[sec] for current sector
    HEmcCal          **fEmcHitsSec;             // ...
    Short_t           *indexEmcHitSec;          // ...
    Int_t              nEmcClusSec;             // == nEmcHits[sec] for current sector
    HEmcCluster      **fEmcClusSec;             // ...
    Short_t           *indexEmcClusSec;         // ...
      
    Bool_t             isPrint;
    
    Float_t            dX;                   // Matching current deviation
    Float_t            dY;                   // Current va
    Float_t            qual2TofC[9];
    Short_t            indTofC[3];

    static Float_t scaleRpcRMS   [2];
    static Float_t scaleShowerRMS[2];
    static Float_t scaleTofRMS   [2];
    static Float_t scaleEmcRMS   [2];

    static Float_t scaleRpcCut   ;
    static Float_t scaleShowerCut;
    static Float_t scaleTofCut   ;
    static Float_t scaleEmcCut   ;


    Bool_t             stNotMatTracks;      // kTRUE - store not matched tracks in cat.
  public:
    HMetaMatchF2(void);
    HMetaMatchF2(const Text_t *name,const Text_t *title);
    ~HMetaMatchF2();
    Int_t        execute();
    Bool_t       init();
    Bool_t       reinit();
    Bool_t       finalize();
    void         storeNotMatchedTracks(Bool_t fl=kTRUE) {stNotMatTracks = fl;}
    static void  setScaleRMS(Float_t tofrmsx,Float_t tofrmsy,Float_t rpcrmsx,Float_t rpcrmsy,Float_t shrrmsx,Float_t shrrmsy) {
	scaleRpcRMS   [0] = rpcrmsx;
	scaleRpcRMS   [1] = rpcrmsy;
	scaleShowerRMS[0] = shrrmsx;
	scaleShowerRMS[1] = shrrmsy;
	scaleTofRMS   [0] = tofrmsx;
	scaleTofRMS   [1] = tofrmsy;
	scaleEmcRMS   [0] = shrrmsx;  // for EMS parameter shrrmsx is used
	scaleEmcRMS   [1] = shrrmsy;  // for EMS parameter shrrmsy is used
    }
    static void  setScaleCut(Float_t tofcut,Float_t rpccut,Float_t shrcut) {
	scaleRpcCut    = rpccut;
	scaleTofCut    = tofcut;
	scaleShowerCut = shrcut;
	scaleEmcCut    = shrcut;    // for EMS parameter shrcut is used
    }
  private:
    void         setInitParam(void);
    void         makeRichMatching(void);
    void         collectTofHits(void);
    void         collectRpcClusters(void);
    void         collectShowerHits(void);
    void         collectEmcHits(void);
    void         collectEmcClusters(void);
    void         makeOuterSegMatch(HMdcTrkCand* pTrkCand);
    Bool_t       getMetaMatchSlot(Int_t trkCandIndex);
    Bool_t       quality2TofClustS2(Int_t& hit);
    Bool_t       quality2TofClustS1(Int_t hit);
    Bool_t       quality2TofHit(Int_t hit);
    Float_t      quality2Shower(HShowerHit* pShrHit);
    Float_t      quality2Rpc(HRpcCluster* pRpcCl);
    Float_t      quality2Emc(HEmcCal* pEmcHit,HModGeomPar *pmodgeom,Double_t xSegCr,Double_t ySegCr);
    Float_t      quality2EmcClus(HEmcCluster* pEmcClus,Double_t xSegCr,Double_t ySegCr);
    void         setCurrentSector(Int_t sec);
    void         addTofCluster(HTofCluster* pTofCluster);
    void         addTofHit(HTofHit* pTofHit,Int_t clSize=0);
    void         addTof(HTofHit* pTofHit,Int_t index, Int_t clSize);
    void         addRing(Float_t quality, Short_t ind, Short_t* indTable,UChar_t& nRich);
    void         setMatchingParam(void);
    void         insertQual(Float_t qual,Short_t ind,UChar_t& nEl,Float_t qualArr[][3],Short_t indArr[]);
    void         insertQualTof(Float_t qual);
    void         transMdcToMeta(const HGeomTransform& modSys);

  public:
    ClassDef(HMetaMatchF2,0)
};

#endif
