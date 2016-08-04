#ifndef HSPLINETOFCLF2_H
#define HSPLINETOFCLF2_H
#include <iostream>
#include "hreconstructor.h"
class HCategory;
class HIterator;
class HRuntimeDb;
class HMetaMatch2;

class HSplineTofClF2 : public HReconstructor {
private:
   Int_t ind1,ind2,ind3,ind4;
   Int_t mode;
   Int_t step,step1; 
   HMetaMatch2 *pMetaMatch1, *pMetaMatch2;
   HIterator *iterMetaMatch1, *iterMetaMatch2;
   HCategory *fCatMetaMatch;
   HCategory *fCatTrkCand;
   Bool_t cond(Int_t , Int_t , Int_t , Int_t );
   Bool_t checkForSeg(HMetaMatch2 *, HMetaMatch2*);
   Bool_t condMeta(HMetaMatch2*, HMetaMatch2*);
   void checkCluster(HMetaMatch2 *pMetaMatch);
   Bool_t checkNextTofHits(HMetaMatch2 *);
   Bool_t checkCandForSameMeta(HMetaMatch2 *);
   void checkShowerOverlap();
public:
   HSplineTofClF2(void);
   HSplineTofClF2(const Text_t name[],const Text_t title[],Int_t mode=0);
   ~HSplineTofClF2();
   Int_t execute();
   Bool_t init();
   Bool_t reinit();
   Bool_t finalize();
   Int_t unsetTofHitsFromCluster();
   
   
   ClassDef(HSplineTofClF2,0)
      };
#endif

