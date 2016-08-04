#ifndef HMDCCLUSTERTOHIT_H
#define HMDCCLUSTERTOHIT_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hmdchitsegfiller.h"

class HCategory;
class HIterator;
class HMdcDetector;
class HMdcGetContainers;
class HMdcClus;
class HGeomVector;
class HMdcSeg;
class HMdcHit;
class HMdcTrkCand;
class HMdcSizesCells;
class HMdcClusInf;

class HMdcClusterToHit : public HReconstructor {
  private:
    HLocation          locHit;          // location for new "hit" object
    HLocation          locSeg;          // location for new "segment" object
    HLocation          loc;             // 
    HLocation          locTrkCand;      // location for new HMdcTrkCand object
    HCategory         *fHitCat;         // pointer to HMdcHit data category
    HCategory         *fSegCat;         // pointer to HMdcSeg data category
    HCategory         *fClusInfCat;     // pointer to HMdcSeg data category
    HCategory         *fTrkCandCat;     // pointer to HMdcTrkCand data category
    HIterator         *iter;
    HCategory         *fClustCat;       // pointer to Cluster data category
    HMdcDetector      *fMdcDet;
    HMdcGetContainers *fGetCont;
    HMdcSizesCells    *fSizesCells;
    HMdcClus          *fClst;
    Int_t              clusIndex;       // index of fClst in catMdcClus
    HMdcHit           *fHit;
    HMdcSeg           *fSeg;
    HMdcTrkCand       *fTrkCand;
    HMdcHitSegFiller   hitSegFiller;
    Bool_t             isGeant;
    Bool_t             slotNotAv;
    Int_t              tFillSeg[2];     // [0]-segment 1, [1]-segment 2
                                        // =-1 - don't fill HMdcSeg
                                        // =0,1 (2,3) - at the typeClustFinder=1
                                        // (see hmdctrackfinder.cc) HMdcSeg will filled
                                        // by hits from MDC1,MDC2 (MDC3,MDC4)
                                        // If in segment used only one MDC and tFillSeg=0,1
                                        // HMdcSeg will filled by Hit in this MDC.
    Int_t              nMdcs;           // Numb. mdc's in cluster
    Bool_t             prntSt;          // kTRUE - status printed already
    
  private:
    void         fillSeg(void);
    Bool_t       fillHit(Short_t mod);
    void         setParContainers(void);
    HMdcClusInf* fillClusInf(Int_t iMod);
    void         fillContainers(void);                        
    void         fillTrkCandISeg(Short_t sec,Int_t indexSeg); 
    void         fillTrkCandOSeg(Short_t sec,Int_t indexSeg); 
    void         fillTrkCandOSeg(Int_t indexSeg);             
  public:
    HMdcClusterToHit();
    HMdcClusterToHit(const Text_t *name,const Text_t *title);
    HMdcClusterToHit(Int_t fSeg1, Int_t fSeg2);
    HMdcClusterToHit(const Text_t *name,const Text_t *title,
                     Int_t fSeg1,Int_t fSeg2);
    ~HMdcClusterToHit(void);
    Bool_t       init(void);
    Bool_t       reinit(void);
    Bool_t       finalize(void) {return kTRUE;}
    Int_t        execute(void);
    void         printStatus(void);

  ClassDef(HMdcClusterToHit,0)
};

#endif
