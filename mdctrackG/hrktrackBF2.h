#ifndef HRKTRACKBF2_H
#define HRKTRACKBF2_H

#include "hreconstructor.h"
#include "hgeomtransform.h"
#include "hmdctrkcand.h"
#include "hmdcgetcontainers.h"
#include "htofclustersim.h"

class HCategory;
class HIterator;
class HMetaMatch2;
class HMetaMatchPar;
class HMdcSeg;
class HMdcHit;
class HMdcTrkCand;
class HBaseTrack;
class HSplineTrack;
class HShowerHit;
class HTofHit;
class HTofCluster;
class HRungeKutta;
class HRKTrackB;
class HMdcTrackGFieldPar;
class HMagnetPar;
class HSpecGeomPar;
class HMdcGeomPar;
class HTofGeomPar;
class HRpcGeomPar;
class HShowerGeometry;
class HEmcGeomPar;
class HMdcGetContainers;
class HMdcSizesCells;
class HRpcCluster;
class HEmcCluster;

class HRKTrackBF2 : public HReconstructor 
{
 private:
   Float_t fieldFactor; // field scaling factor
   Short_t mode;        // mode = {0 || 2} for initial momentum as {selfGuess, SplineTrack-Guess}
   HGeomTransform      secTrans[6];     // sector transformation, used to transform META point to MdcSegment-ccord-system
   HGeomTransform      showerSM[6];     //trans shower, modtosec
   HGeomTransform      emcSM[6];        //trans emc, modtosec
   HGeomTransform      tofSM[6][8];     //trans tof,  modtosec
   HGeomTransform      rpcSM[6];        //trans rpc,  modtosec
   HGeomTransform      transMetaSM;     //trans meta, modtosec
   
   
    Float_t            sigma2TofX[6];       
    Float_t            sigma2TofY[6];
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
    Float_t            sEmcX[6];
    Float_t            sEmcY[6];
    Float_t            sRpcX[6];
    Float_t            sRpcY[6];
    Float_t            quality2RPCCut[6];
    Float_t            quality2SHOWERCut[6];
    Float_t            quality2EMCCut[6];
   
   
   
   HMdcTrackGFieldPar* field;           // field map
   HMagnetPar*         pMagnet;         // parameter container for magnet settings
   HSpecGeomPar*       fSpecGeomPar;    // sector and target geometry
   HMdcGetContainers*  fGetCont;        // pointer to HMdcGetContainers object
   HTofGeomPar*        fTofGeometry;    // TOF geometry
   HRpcGeomPar*        fRpcGeometry;    // Rpc geometry
   HShowerGeometry*    fShowerGeometry; // Shower geometry
   HEmcGeomPar*        fEmcGeometry;    // Emc geometry
   HMdcSizesCells*     pMSizesCells;    // pointer to HMdcSizesCells objects
   
   HCategory*          fCatMetaMatch;   // pointer to MetaMatch category
   HIterator*          fMetaMatchIter;  // iterator on the MetaMatch category
   HMetaMatch2*        pMetaMatch;      // pointer to the MetaMatch object
   HMetaMatchPar*      fMatchPar;
   HCategory*          fCatMdcTrkCand;  // pointer to MDC track candidate category
   HMdcTrkCand*        pMdcTrkCand;     // pointer to the MDC track candidate
   HCategory*          fCatMdcSeg;      // pointer to the MDC segments category
   HCategory*          fCatMdcHit;      // pointer to the MDC hit
   HCategory*          fSplineTrack;    // pointer to the Spline track category (for initial momentum guess)
   HSplineTrack*       pSplineTrack;    // Spline track 
   HCategory*          fCatKine;        // pointer to the Kine category
   HCategory*          fCatShower;      // pointer to the Shower category
   HCategory*          fCatEmc;         // pointer to the Emc category
   HCategory*          fCatTof;         // pointer to the Tof hit category
   HTofHit  *          pTofHit[3];
   HCategory*          fCatTofCluster;  // pointer to the Tof cluster category
   HCategory*          fCatRpcCluster;
   HTofCluster*        pTofCluster;     // Tof cluster
   HCategory*          fCatRKTrack;     // pointer to the Runge Kutta track category 
   HRungeKutta*        pRungeKutta;     // Runge Kutta track

   HLocation   sectorloc;        // sector location
   Int_t  sector;                // sector number (0..5)
   Short_t  system;              // meta detector (0==Tofino, 1==Tof)
   HMdcSeg* pMdcSeg1;            // pointer to inner segment
   HMdcSeg* pMdcSeg2;            // pointer to outer segment
   Bool_t mdcInstalled[4][6];    //! remembers which MDCs have known geometry
   Float_t multSig[8];           // multiplicators for resolution
   Float_t vertex[3];            // vertex
   HGeomVector normVecRpc[6];    // normal vector on the Tofino module in the sector coordinate system
   HGeomVector centerRpc[6];     // physical center of Tofino module in the sector coordinate system
   HGeomVector normVecShower[6]; // normal vector on the Shower module in the sector coordinate system
   HGeomVector normVecEmc[6];    // normal vector on the Emc module in the sector coordinate system
   HGeomVector normVecTof[6][8]; // normal vector on each Tof module in the sector coordinate system
   HGeomVector metaNormVec;      // normal vector on the meta module in the sector coordinate system
   HGeomVector pointMeta;        // META hit

   Bool_t  success;              // flag indicating severe problems on input of RK
   Short_t qRK;                  // polarity
   Float_t pRK;                  // momentum
   Float_t momentumGuess;        // initial value for momentum if supplied
   Float_t chiqRK;               // chi2 of Runge Kutta
   Float_t trackLength;          // full track length (Target--MDC1--MDC4--META)
   Float_t tof;                  // time-of-flight
   Float_t metaeloss;            // energy loss from meta detector
   Float_t beta;                 // beta value
   Float_t mass2;                // mass**2
   Float_t RKxyzMETA[3];         // META point after RK fitting

   Float_t xTof,yTof,zTof;
   Float_t zMod;

   Float_t dXrms2;
   Float_t dYrms2;
   Float_t dX, dY;
   Float_t qualityRpc;
   Float_t qualityShower;
   Float_t qualityEmc;
   Float_t qualityTof;
   
   HShowerHit  *pShowerHit;
   HEmcCluster *pEmcCluster;
   HRpcCluster *pRpc;
   Short_t indRpc;
   Short_t indTof[3];
   Short_t indShower;
   Short_t indEmc;

   Bool_t doMassStuff();
   void matchWithRpc();
   void matchWithShower();
   void matchWithEmc();
   void matchWithTof();
   void calcBeta(Float_t, Int_t , Bool_t option = kTRUE);
   Int_t indexRK;
   

   HRKTrackB* fillData(HMdcSeg*,HMdcSeg*,HSplineTrack*, Int_t &);
   Int_t calcPosDirFromSegment(HMdcSeg* pSeg,Int_t ioseg,Double_t*,Double_t*,Bool_t flag);
   void setMatchingParams(Int_t );
   Float_t getQuality(Float_t, Float_t, Float_t, Float_t);
   
 public:
   HRKTrackBF2();
   HRKTrackBF2(const Text_t name[], Short_t m = 2);
   ~HRKTrackBF2(); 
 
   void clear();
   Bool_t init();
   Bool_t reinit();
   Int_t  execute();
   Bool_t finalize() { return kTRUE; }

   
   ClassDef(HRKTrackBF2,0) // Runge-Kutta reconstructor
};

#endif
