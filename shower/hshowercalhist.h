#ifndef HShowerCalHist_H
#define HShowerCalHist_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"
#include "hshowercal.h"

class HCategory;
class HIterator;
class TH1F;
class TH2F;

class HShowerCalHist : public HReconstructor {
public:
        HShowerCalHist(){fIter = NULL;}

        HShowerCalHist(const Text_t *name,const Text_t *title)
                    : HReconstructor(name, title) {fIter = NULL;}
       ~HShowerCalHist();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t execute(void);

        HCategory* getCalCat(){return m_pCalCat;}
        void setCalCat(HCategory* pCalCat){m_pCalCat = pCalCat;}

        Bool_t bookHist();
        Bool_t fillHist(HShowerCal* pCal);
        Bool_t fillHist(Int_t nModule, Int_t nRow, Int_t nColumn, Float_t fCharge);
        Bool_t writeHist();
        Bool_t finalizeHist();

private:
        Int_t m_nEvents;
        HCategory *m_pCalCat; //!Pointer to the calib data category

        HIterator *fIter;  //!

        TH1F* m_pChargeCalHist;  //!
        TH1F* m_pChargeCalModHist[3];  //!
        TH2F* m_pChargeCalFreqHist[3];  //!
        TH2F* m_pChargeCalAvgHist[3];  //!

        ClassDef(HShowerCalHist,0) //ROOT extension
};

#endif
