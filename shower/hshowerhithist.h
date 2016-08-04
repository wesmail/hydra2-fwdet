#ifndef HShowerHitHist_H
#define HShowerHitHist_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"
#include "hshowercal.h"
#include "hshowerhit.h"

class HCategory;
class HIterator;
class TH1F;
class TH2F;

class HShowerHitHist : public HReconstructor {
public:
	HShowerHitHist(){fIter = NULL;}

        HShowerHitHist(const Text_t *name,const Text_t *title)
                    : HReconstructor(name, title) {fIter = NULL;} 
	~HShowerHitHist();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t execute(void);

        HCategory* getHitCat(){return m_pHitCat;}
        void setHitCat(HCategory* pHitCat){m_pHitCat = pHitCat;}

        ClassDef(HShowerHitHist,1) //ROOT extension

        Bool_t bookHist();
        Bool_t fillHist(HShowerHit* pHit);
        Bool_t fillHist(Int_t nModule, Int_t nRow, Int_t nColumn, Float_t fCharge);
        Bool_t writeHist();
        Bool_t finalizeHist();

private:
        Int_t m_nEvents;
        HCategory *m_pHitCat; //!Pointer to the calib data category

        HIterator *fIter;  //!

        TH1F* m_pChargeHitHist;  //!
        TH1F* m_pChargeHitModHist[3];  //!        
        TH2F* m_pChargeHitFreqHist[3];  //!        
        TH2F* m_pChargeHitAvgHist[3];  //!        
};

#endif
