#ifndef HMDCTRACKFINDER_H
#define HMDCTRACKFINDER_H

#include "hreconstructor.h"

#include "TStopwatch.h"

class HIterator;
class HCategory;
class HMdc34ClFinder;
class HMdcLookUpTb;
class HMdcEvntListCells;
class HMdcGeantEvent;
class HMdcClusMetaMatch;

class HMdcTrackFinder : public HReconstructor {
protected:
  HMdcLookUpTb      *fLookUpTb;         // MDC1&2 clus.finder (+3&4 for type=1)
  HMdc34ClFinder    *p34ClFinder;       // Clusters finder for MDC3&4
  HMdcClusMetaMatch *pClusMetaMatch;
  Int_t              nModules[6];       // [s] - number of MDCs for HMdcLookUpTb
  Int_t              numLayers[6][4];   // [s][m] - number of layers in MDC mod.
  Int_t              typeClFinder;      // =0 - finding in all mdc in sec.
                                        // =1 - finding in each mdc indep.
                                        // =2 - mixed cluster finder
  Bool_t             isClustToHit;      // kTRUE -> Cl.ToHit;  kFALSE->12Fit;
  HCategory         *fClusCat;          // pointer to HMdcClus data category
  HCategory         *fSegCat;           // pointer to Segment data category
  Bool_t             perMdcClus;        // =kTRUE - write HMdcClus to file
  HLocation          locClus;           // loc. for HMdcClus category
  HLocation          locNewCl;          // loc. for HMdcClus category
  HIterator         *iterClus;          // iterator for fClusCat
  HMdcEvntListCells *event;             // list of fired wires in current event
  Bool_t             isMdcLCellsOwn;    // =kTRUE if MdcEvntListCells is own
  HMdcGeantEvent    *geantEvent;        // mdc geant tracks collection
  Bool_t             isGeantEventOwn;   // =kTRUE if HMdcGeantEvent is own
  Int_t              level4[2];         // [seg] - level for 4-layers finding
  Int_t              level5[2];         // [seg] - level for 5-layers finding
  Int_t              clFndrLevel[6][4]; // [s][m] - level for current event
  Bool_t             prntSt;            // kTRUE - status printed already
  Bool_t             tarSFinderOnly;    // kTRUE - do vertex finder only (no clust.finder)
  
public:
  HMdcTrackFinder(Bool_t isCOff, Int_t typeClFn=0);
  HMdcTrackFinder(const Text_t *name, const Text_t *title, 
                  Bool_t isCOff, Int_t typeClFn=0);
  HMdcTrackFinder(void);
  HMdcTrackFinder(const Text_t *name, const Text_t *title);
  ~HMdcTrackFinder(void);
  Bool_t init(void);
  Bool_t reinit(void);
  Int_t  execute(void);
  Bool_t finalize(void) {return kTRUE;}
  void   setPersistency(Bool_t perCl);
  void   setParContainers();
  void   setNLayers(Int_t *lst);
  void   setLevel(Int_t l4s1, Int_t l5s1, Int_t l4s2, Int_t l5s2);
  void   setLevelSeg1(Int_t l4, Int_t l5);
  void   setLevelSeg2(Int_t l4, Int_t l5);
  void   printStatus(void);
  void   calcClFndrLevel(void);
  Int_t  clFndrBeforField(void);
  void   targSegFinderOnly(void)                 {tarSFinderOnly = kTRUE;}
  HMdcClusMetaMatch* getClusMetaMatch(void)      {return pClusMetaMatch;}

protected:
  Int_t  findOuterSegClusters(void);
  void   setMod34Levels(void);
  void   sortTracksInClusters(void);
  Bool_t initMetaMatch(void);
  
  ClassDef(HMdcTrackFinder,0) // Cluster finder manager class.
};

#endif
