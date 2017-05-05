#ifndef HMETAMATCH2_H
#define HMETAMATCH2_H
#include "hreconstructor.h"
#include "TString.h"
#include "TObject.h"

#define RICH_TAB_SIZE 3
#define META_TAB_SIZE 3

class HMetaMatch2:public TObject {
private:
   Int_t   trkCandInd;                     // index of HTrkCand object
   Int_t   ownIndex;                       // index of "this" object in categoty
   
   Char_t  sector;                         // sector number
   
   UChar_t nRpcClust;                      // Number of matched rpc clusters
   UChar_t nShrHits;                       // Number of matched shower hits
   UChar_t nTofHits;                       // Number of matched tof clusters or hits
   

   Short_t rungeKuttaInd;
   
   Short_t rkIndShower[META_TAB_SIZE];
   Short_t rkIndTofCl[META_TAB_SIZE];
   Short_t rkIndTof1[META_TAB_SIZE];
   Short_t rkIndTof2[META_TAB_SIZE];
   Short_t rkIndRpc[META_TAB_SIZE];

   Short_t kalmanFilterInd;                // index of HKalTrack in catKalTrack

   Short_t kfIndShower[META_TAB_SIZE];
   Short_t kfIndTofCl [META_TAB_SIZE];
   Short_t kfIndTof1  [META_TAB_SIZE];
   Short_t kfIndTof2  [META_TAB_SIZE];
   Short_t kfIndRpc   [META_TAB_SIZE];

   Short_t rpcClstInd[META_TAB_SIZE];      // Index of HRpcCluster object
   Short_t shrHitInd[META_TAB_SIZE];       // Index of HShowerHit object
   Short_t tofClstInd[META_TAB_SIZE];      // Index of HTofCluster object
   Short_t tofHit1Ind[META_TAB_SIZE];      // Index of HTofHit object  ???
   Short_t tofHit2Ind[META_TAB_SIZE];      // Index of HTofHit object  ???
   
   // System 0:
   Float_t rpcQuality[META_TAB_SIZE];      // Matching quality
   Float_t rpcDX[META_TAB_SIZE];           // Deviation in X-coordinate
   Float_t rpcDY[META_TAB_SIZE];           // Deviation in Y-coordinate
   
   Float_t shrQuality[META_TAB_SIZE];      // Matching quality
   Float_t shrDX[META_TAB_SIZE];           // Deviation in X-coordinate
   Float_t shrDY[META_TAB_SIZE];           // Deviation in Y-coordinate
   
   // System 1:
   Float_t tofClstQuality[META_TAB_SIZE];  // Matching quality
   Float_t tofClstDX[META_TAB_SIZE];       // Deviation in X-coordinate
   Float_t tofClstDY[META_TAB_SIZE];       // Deviation in Y-coordinate
   
   Float_t tofHit1Quality[META_TAB_SIZE];  // Matching quality
   Float_t tofHit1DX[META_TAB_SIZE];       // Deviation in X-coordinate
   Float_t tofHit1DY[META_TAB_SIZE];       // Deviation in Y-coordinate
   
   Float_t tofHit2Quality[META_TAB_SIZE];  // Matching quality
   Float_t tofHit2DX[META_TAB_SIZE];       // Deviation in X-coordinate
   Float_t tofHit2DY[META_TAB_SIZE];       // Deviation in Y-coordinate
   
   
   Int_t   splineInd;                      // index of HSplineTrack object
  
   UChar_t nRichId;                        // number of matched rings in richInd[]
   UChar_t nRichIPUId;                     // number of matched rings in richIPUInd[]
   Int_t   richInd[RICH_TAB_SIZE];         // arr.of indexes of HRichHit objects
   Int_t   richIPUInd[RICH_TAB_SIZE];      // arr.of indexes of HRichHitIPU objects
                                           // arrais are sorted by matching quality

   Char_t  flag;               // First bit for shower(=0)/emc(=1) distinguish

   Bool_t isFakeInner;
   Bool_t isFakeOuter;

public:
   HMetaMatch2();
   ~HMetaMatch2(){}
   HMetaMatch2(Short_t sec, Int_t tkInd, Int_t ind);
   
   void setTrkCandInd(Int_t tr)                {trkCandInd = tr;}
   void setOwnIndex(Int_t ind)                 {ownIndex   = ind;}
   
   void setNRpcClust(UChar_t n)                {nRpcClust = n<META_TAB_SIZE ? n:META_TAB_SIZE;}
   void setNShrHits(UChar_t n)                 {nShrHits  = n<META_TAB_SIZE ? n:META_TAB_SIZE;}
   void setNTofHits(UChar_t n)                 {nTofHits  = n<META_TAB_SIZE ? n:META_TAB_SIZE;}
   void setRpcClst(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy);
   void setShrHit(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy);
   void setTofClst(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy);
   void setTofHit1(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy);
   void setTofHit2(UChar_t el,Short_t ind,Float_t ql,Float_t dx,Float_t dy);
   
   // Functions for HMetaMatchF2:
   void setRpcClstMMF(UChar_t nln,Short_t ind[],Float_t ql2[][3]);
   void setShrHitMMF( UChar_t nln,Short_t ind[],Float_t ql2[][3]);
   void setTofClstMMF(UChar_t nln,Short_t ind[][3],Float_t ql2[][9]);
   void setEmcClstMMF(UChar_t nln,Short_t ind[],Float_t ql2[][3]);


   void setSplineInd(Int_t _splineInd) { splineInd =  _splineInd; }

   void setSector(Char_t sec)                  {sector     = sec;}
   void setNCandForRich(UChar_t nrich)         {nRichId    = nrich;}
   void setNCandForIPU(UChar_t nrich)          {nRichIPUId = nrich;}
   void setRichInd(UChar_t id, Short_t ind)    {if(id<RICH_TAB_SIZE) richInd[id] = ind;}
   void setRichIPUInd(UChar_t id, Short_t ind) {if(id<RICH_TAB_SIZE) richIPUInd[id] = ind;}


   void setRungeKuttaIndShowerHit(UChar_t n, Short_t ind)  { rkIndShower[n] = ind; }
   void setRungeKuttaIndEmcCluster(UChar_t n, Short_t ind) { rkIndShower[n] = ind; }
   void setRungeKuttaIndTofClst(UChar_t n, Short_t ind)    { rkIndTofCl[n]  = ind; }
   void setRungeKuttaIndTofHit1(UChar_t n,  Short_t ind)   { rkIndTof1[n]   = ind; }
   void setRungeKuttaIndTofHit2(UChar_t n,  Short_t ind)   { rkIndTof2[n]   = ind; }
   void setRungeKuttaIndRpcClst(UChar_t n,   Short_t ind)  { rkIndRpc[n]    = ind; }
   
   void setRungeKuttaInd(Short_t _rungeKuttaInd)    { rungeKuttaInd = _rungeKuttaInd; }
   
   void setKalmanFilterInd(Int_t _kalmanFilterInd) {kalmanFilterInd = _kalmanFilterInd; }

   void setKalmanFilterIndShowerHit(UChar_t n, Short_t ind)  { kfIndShower[n] = ind; }
   void setKalmanFilterIndEmcCluster(UChar_t n, Short_t ind) { kfIndShower[n] = ind; }
   void setKalmanFilterIndTofClst  (UChar_t n, Short_t ind)  { kfIndTofCl[n]  = ind; }
   void setKalmanFilterIndTofHit1  (UChar_t n, Short_t ind)  { kfIndTof1[n]   = ind; }
   void setKalmanFilterIndTofHit2  (UChar_t n, Short_t ind)  { kfIndTof2[n]   = ind; }
   void setKalmanFilterIndRpcClst  (UChar_t n, Short_t ind)  { kfIndRpc[n]    = ind; }

   void setInnerFake(Bool_t fake) { isFakeInner=fake;}
   void setOuterFake(Bool_t fake) { isFakeOuter=fake;}
   void setEmcClusterFlag(void)   { flag |= 1;}
   void setShowerHitFlag(void)    { flag &= ~1;}

   Char_t  getSector(void) const                {return sector;}
   Int_t   getTrkCandInd(void) const            {return trkCandInd;}
   Int_t   getOwnIndex(void) const              {return ownIndex;}

   Int_t   getMetaArrSize(void) const           {return META_TAB_SIZE;}
   
   Bool_t  isEmcCluster(void) const             {return (flag&1)==1;}
   Bool_t  isShowerHit(void) const              {return (flag&1)==0;}
   
   Bool_t  isEmcClustValid(UChar_t n) const     {return isEmcCluster() && n<nShrHits;}
   Bool_t  isShowerHitValid(UChar_t n) const    {return isShowerHit() && n<nShrHits;}
   
   Int_t   getSystem(void) const;
   UChar_t getNRpcClusters(void) const          {return nRpcClust;}
   UChar_t getNShrHits(void) const              {return isShowerHit()  ? nShrHits : 0;}
   UChar_t getNEmcClusters(void) const          {return isEmcCluster() ? nShrHits : 0;}
   UChar_t getNTofHits(void) const              {return nTofHits;}
   
   Short_t getRpcClstInd(UChar_t n) const       {return n<nRpcClust ? rpcClstInd[n] : -1;}
   Short_t getShowerHitInd(UChar_t n) const     {return isShowerHitValid(n) ? shrHitInd[n] : -1;}
   Short_t getEmcClusterInd(UChar_t n) const    {return isEmcClustValid(n)  ? shrHitInd[n] : -1;}
   Short_t getTofClstInd(UChar_t n) const       {return n<nTofHits  ? tofClstInd[n] : -1;}
   Short_t getTofHit1Ind(UChar_t n) const       {return n<nTofHits  ? tofHit1Ind[n] : -1;}
   Short_t getTofHit2Ind(UChar_t n) const       {return n<nTofHits  ? tofHit2Ind[n] : -1;}
   
   Float_t getRpcClstQuality(UChar_t n) const   {return n<nRpcClust ? rpcQuality[n] : -1.f;}
   Float_t getShowerHitQuality(UChar_t n) const  {return isShowerHitValid(n) ? shrQuality[n] : -1.f;}
   Float_t getEmcClusterQuality(UChar_t n) const {return isEmcClustValid(n)  ? shrQuality[n] : -1.f;}
   Float_t getTofClstQuality(UChar_t n) const   {return n<nTofHits  ? tofClstQuality[n] : -1.f;}
   Float_t getTofHit1Quality(UChar_t n) const   {return n<nTofHits  ? tofHit1Quality[n] : -1.f;}
   Float_t getTofHit2Quality(UChar_t n) const   {return n<nTofHits  ? tofHit2Quality[n] : -1.f;}
   
   Float_t getRpcClstDX(UChar_t n) const        {return n<nRpcClust ? rpcDX[n] : -1000.f;}
   Float_t getShowerHitDX(UChar_t n) const      {return isShowerHitValid(n) ? shrDX[n] : -1000.f;}
   Float_t getEmcClusterDX(UChar_t n) const     {return isEmcClustValid(n)  ? shrDX[n] : -1000.f;}
   Float_t getTofClstDX(UChar_t n) const        {return n<nTofHits  ? tofClstDX[n] : -1000.f;}
   Float_t getTofHit1DX(UChar_t n) const        {return n<nTofHits  ? tofHit1DX[n] : -1000.f;}
   Float_t getTofHit2DX(UChar_t n) const        {return n<nTofHits  ? tofHit2DX[n] : -1000.f;}
   
   Float_t getRpcClstDY(UChar_t n) const        {return n<nRpcClust ? rpcDY[n] : -1000.f;}
   Float_t getShowerHitDY(UChar_t n) const      {return isShowerHitValid(n) ? shrDY[n] : -1000.f;}
   Float_t getEmcClusterDY(UChar_t n) const     {return isEmcClustValid(n)  ? shrDY[n] : -1000.f;}
   Float_t getTofClstDY(UChar_t n) const        {return n<nTofHits  ? tofClstDY[n] : -1000.f;}
   Float_t getTofHit1DY(UChar_t n) const        {return n<nTofHits  ? tofHit1DY[n] : -1000.f;}
   Float_t getTofHit2DY(UChar_t n) const        {return n<nTofHits  ? tofHit2DY[n] : -1000.f;}


   Int_t   getSplineInd(void) const             {return splineInd;}
   
   
   Short_t getRungeKuttaIndShowerHit(UChar_t n)  const { return isShowerHitValid(n) ? rkIndShower[n] : -1; }
   Short_t getRungeKuttaIndEmcCluster(UChar_t n) const { return isEmcClustValid(n)  ? rkIndShower[n] : -1; }
   Short_t getRungeKuttaIndTofClst(UChar_t n)    const { return n < nTofHits  ?  rkIndTofCl[n] : -1; }
   Short_t getRungeKuttaIndTofHit1(UChar_t n)    const { return n < nTofHits  ?   rkIndTof1[n] : -1; }
   Short_t getRungeKuttaIndTofHit2(UChar_t n)    const { return n < nTofHits  ?   rkIndTof2[n] : -1; }
   Short_t getRungeKuttaIndRpcClst(UChar_t n)    const { return n < nRpcClust ?    rkIndRpc[n] : -1; }
   
   Short_t getRungeKuttaInd() const        { return rungeKuttaInd; }
   
   
   
   Short_t getKalmanFilterInd(void) const       {return kalmanFilterInd;}

   Short_t getKalmanFilterIndShowerHit(UChar_t n)  const { return isShowerHitValid(n) ? kfIndShower[n] : -1; }
   Short_t getKalmanFilterIndEmcCluster(UChar_t n) const { return isEmcClustValid(n)  ? kfIndShower[n] : -1; }
   Short_t getKalmanFilterIndTofClst  (UChar_t n) const { return n < nTofHits  ?  kfIndTofCl[n] : -1; }
   Short_t getKalmanFilterIndTofHit1  (UChar_t n) const { return n < nTofHits  ?   kfIndTof1[n] : -1; }
   Short_t getKalmanFilterIndTofHit2  (UChar_t n) const { return n < nTofHits  ?   kfIndTof2[n] : -1; }
   Short_t getKalmanFilterIndRpcClst  (UChar_t n) const { return n < nRpcClust ?    kfIndRpc[n] : -1; }
   
   UChar_t getNCandForRich(void) const          {return nRichId;}
   UChar_t getNCandForIPU(void) const           {return nRichIPUId;}
   Short_t getARichInd(UChar_t id) const        {return id<RICH_TAB_SIZE ? richInd[id]    : -1;}
   Short_t getARichIPUInd(UChar_t id) const     {return id<RICH_TAB_SIZE ? richIPUInd[id] : -1;}

   Bool_t isFake()      { return isFakeInner|isFakeOuter;}
   Bool_t isInnerFake() { return isFakeInner;}
   Bool_t isOuterFake() { return isFakeOuter;}

   void    print(void);
   
private:   
   void    setDefForRest(void);

   ClassDef(HMetaMatch2,2)
};

#endif

