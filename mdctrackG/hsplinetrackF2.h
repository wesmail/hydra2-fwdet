#ifndef HSPLINETRACKF2_H
#define HSPLINETRACKF2_H

#include "hreconstructor.h"
#include "hgeomvector.h"

class HCategory;
class HMdcSeg;
class HMdcGeomPar;
class HTofGeomPar;
class HTofinoGeomPar;
class HSpecGeomPar;
class HIterator;
class HMdcTrackGFieldPar;
class HMdcTrackGCorrPar;
class HMdcTrackGSpline;
class HSplineTrack;
class HSplinePar;
class HGeomTransform;
class HTofHit;
class HMetaMatch2;
class HShowerHit;
class HEmcCluster;
class HMagnetPar;
class HMdcSizesCells;
class HEventHeader;
class HMdcGetContainers;
class HMdcTrkCand;
class HRpcCluster;

class HSplineTrackF2 : public HReconstructor {
private:
   HGeomVector pointMeta;
   HGeomTransform *tRans[6];
  // HGeomTransform secTrans[6];
   HMdcSizesCells *pSizesCells;
   Float_t qIOMatching;
   HEventHeader *evHeader;
   Float_t tarDist;   
   Double_t target;
   Double_t distanceTof;
   Float_t xTof,yTof,zTof;
   Double_t C;
   Int_t system;
   Short_t outerHitInd;
   Short_t indTrkCand;
   Short_t index1;
   Short_t index2;
   Float_t tof;
   Float_t mass2;
   Float_t beta;
   Double_t TOFdistance;
   Char_t sector;
   HMdcGetContainers *fGetCont;
   HMdcSeg *segments[2];
   HRpcCluster *pRpc;
   Int_t polarity;
   Double_t dist;
   Short_t IndTrkCand;  
   Int_t tofClSize;
   Short_t metaInd;
   HSplineTrack *sp;
   HCategory *fCatMdcTrkCand;
   HCategory *fCatSplineTrack;
   HCategory *fCatSplinePar;
   HCategory *fCatMdcSegSim;
   HCategory *fCatTof;
   HCategory *fCatTofCluster;
   HMetaMatch2 *pMetaMatch;
   HMdcTrkCand *pMdcTrkCand;      
   HCategory *fCatRpcCluster;     
   HCategory *fCatShowerHit;    
   HCategory *fCatEmcCluster;
   HCategory *fCatMetaMatch;  
   HMdcGeomPar *fMdcGeometry;
   HSpecGeomPar *fSpecGeomPar;
   HTofHit *pTofHit[3];
   HShowerHit  *pShowerHit; 
   HEmcCluster *pEmcCluster;
   HMdcTrackGSpline *Spline;
   HIterator *fMetaMatchIter;
   HIterator *IterTof;
   HIterator *IterShower;
   HMdcTrackGFieldPar *field;
   HMdcTrackGCorrPar *corr;
   HMagnetPar *pMagnet;
   Float_t Momentum;
   Float_t errP,errMass2;
   HLocation sectorloc;
   void calcMomentum(HMdcSeg **,HMetaMatch2 *);
   void calcMomentum(HMdcSeg *segments[]);
   Bool_t doMassStuff(HMetaMatch2 *);
   void doMassStuff2(TString , HMetaMatch2 *); 
   void calcBeta(Float_t , Float_t , Float_t , Float_t );    
   Bool_t doMomentum(HMetaMatch2 *);
   Int_t firstCandIndex;
   Float_t fScal;
   Float_t qSpline;
   Int_t numChambers;
   
  
  
   Bool_t isSplinePar;
   Float_t xPoints[52];
   Float_t yPoints[52];
   Float_t zPoints[52];


public:
   HSplineTrack  *fillData(HMdcSeg *,Bool_t cond = kTRUE);
   
   HSplinePar  *fillParData();
   HSplineTrackF2(void);
   HSplineTrackF2(const Text_t name[],const Text_t title[]);
   ~HSplineTrackF2();
   Int_t  execute();
   Bool_t init();
   Bool_t reinit();
   Bool_t finalize();
   void   makeSplinePar();
   Short_t indRpc;
   Short_t indTof[3];
   Short_t indShower;
   Short_t indEmc;
  
   
private:   
   void setDef(void);
public:
   ClassDef(HSplineTrackF2,0)
      };
#endif

