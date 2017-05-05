#ifndef HMDCCLUSMETAMATCH_H
#define HMDCCLUSMETAMATCH_H
#include "TObject.h"
#include "hgeomtransform.h"
#include "hmdcgeomobj.h"

class HCategory;
class HTofHit;
class HIterator;
class HTofGeomPar;
class HRpcGeomPar;
class HShowerGeometry;
class HEmcGeomPar;
class HMetaMatchPar;
class TH2F;

class HMdcClusMetaMatch : public TObject {
  private:
    HTofGeomPar     *pTofGeometry;
    HCategory       *pCatTof;
    HIterator       *iterTof;
    HCategory       *pCatTofCluster;
    HIterator       *iterTofCluster;
    
    HRpcGeomPar     *pRpcGeometry;
    HCategory       *pCatRpc;
    HIterator       *iterRpc;
    
    HShowerGeometry *pShrGeometry;
    HCategory       *pCatShower;
    HIterator       *iterShower;
  
    HEmcGeomPar     *fEmcGeometry;    // Emc geometry
    HCategory       *fCatEmc;         // pointer to the Emc category

    // Matching cut parameters (for 6 sectors):
    HMetaMatchPar   *pMatchPar;
    Float_t          qualityMulp2;        // Incr.qual.cut by sqrt(qualityMulp2)
    Float_t          sigma2TofXi[6];      // =1./sigmaTofX^2
    Float_t          sigma2TofYi[6];
    Float_t          offsetTofX[6];
    Float_t          offsetTofY[6];
    Float_t          quality2TofCut[6];
    
    Float_t          sigma2RpcX[6];
    Float_t          sigma2RpcY[6];
    Float_t          offsetRpcX[6];
    Float_t          offsetRpcY[6];
    Float_t          quality2RpcCut[6];
    
    Float_t          sigma2ShrX[6];
    Float_t          sigma2ShrY[6];
    Float_t          offsetShrX[6];
    Float_t          offsetShrY[6];
    Float_t          quality2ShrCut[6];
    
    Float_t          sigma2EmcX[6];
    Float_t          sigma2EmcY[6];
    Float_t          offsetEmcX[6];
    Float_t          offsetEmcY[6];
    Float_t          quality2EmcCut[6];
    
    Double_t         rpcSecModTrans[6][12];      // One module is used only
    Double_t         shrSecModTrans[6][12];      // One module is used only
    Double_t         tofSecModTrans[6][8][12];
    Double_t         tofLabModTrans[6][8][12];
    
    // One event collection of meta hits:
    struct TofHit {
      Float_t x;
      Float_t y;
      UChar_t mod;
    };
    struct RpcHit {
      Float_t x;
      Float_t y;
      Float_t z;
      Float_t xSigma2i;
      Float_t ySigma2i;
    };
    struct ShowerHit {
      Float_t x;
      Float_t y;
      Float_t z;
      Float_t xSigma2i;
      Float_t ySigma2i;
    };

    UInt_t           nRpcHits[6];    
    RpcHit           rpcHitArr[6][200];
    UInt_t           nShowerHits[6];     // It is used for EMC also 
    ShowerHit        shrHitArr[6][200];  // It is used for EMC also 
    UInt_t           nTofHits[6];
    TofHit           tofHitArr[6][100];
    
    Bool_t           fillPlots;
    TH2F*            rpcPlots[6];
    TH2F*            shrPlots[6];
    TH2F*            tofPlots[6];
    
  public:
    HMdcClusMetaMatch(void);
    ~HMdcClusMetaMatch();
    Bool_t init(void);
    Bool_t reinit(void);
    void   setFillPlotsFlag(void)          {fillPlots = kTRUE;}
    void   collectMetaHits(void);
    Bool_t hasClusMathToMeta(Int_t sec,const HGeomVector& targ,
                             Double_t xcl,Double_t ycl,Double_t zcl);
    TH2F*  getRpcPlot(Int_t s)             {return rpcPlots[s];}
    TH2F*  getShrPlot(Int_t s)             {return shrPlots[s];}
    TH2F*  getTofPlot(Int_t s)             {return tofPlots[s];}
    void   deletePlots(void);
    void   savePlots(void);
    void   setQualMultp(Float_t mp)         {qualityMulp2 = mp*mp;}
    
  private:
    void   setInitParam(void);
    void   collectRpcClusters(void);
    void   collectShowerHits(void);
    void   collectEmcClusters(void);
    void   collectTofHits(void);
    void   addTofHit(HTofHit* pTofHit);
    Bool_t testAndFill(Int_t sec,const HGeomVector& targ,
                       Double_t xcl,Double_t ycl,Double_t zcl);
      
  ClassDef(HMdcClusMetaMatch,0)
};

#endif
