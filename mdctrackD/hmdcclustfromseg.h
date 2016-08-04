#ifndef HMDCCLUSTFROMSEG_H
#define HMDCCLUSTFROMSEG_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hmdcgeomobj.h"

class HIterator;
class HCategory;
class HMdcSizesCells;
class HMdcClus;
class HMdcSeg;
class HParamList;

class HMdcClustFromSeg : public HReconstructor {
  protected:
    HCategory* pCatMetaMatch;
    HIterator* iterMetaMatch;
    HCategory* pCatSplineTrack;
    HCategory* pCatMdcTrkCand;
    HCategory* pCatMdcSeg;
    HCategory* pCatMdcHit;
    HCategory* pCatMdcClusInf;
    HMdcSizesCells* pSizesCells;
    HLocation locClus;
    HCategory* pCatMdcClus;
    HMdcLineParam trackLine;
  public:
    HMdcClustFromSeg();
    HMdcClustFromSeg(const Text_t *name,const Text_t *title);
    HMdcClustFromSeg(const Text_t *name,const Text_t *title, HParamList *cut);
    ~HMdcClustFromSeg(void);
    virtual Int_t execute(void);
    virtual Bool_t init(void);
    virtual Bool_t reinit(void);
    virtual Bool_t finalize(void);
  private:
    Double_t fMinMomentum;
    Double_t fMinBeta;
    Double_t fMaxBeta;
    Int_t    fPolarity;
    Double_t  fInnerChi2;
    Double_t  fOuterChi2;

    void clear(void);
    void initCut(HParamList* cut);
    void fillCluster(Short_t segInd);
    HMdcClus* getClusterSlot(HMdcSeg* pSeg);

  ClassDef(HMdcClustFromSeg,0)
};

#endif
