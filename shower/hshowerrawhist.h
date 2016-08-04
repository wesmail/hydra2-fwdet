#ifndef HShowerRawHist_H
#define HShowerRawHist_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"

class HCategory;
class HIterator;
class TH1F;
class TH2F;

class HShowerRawHist : public HReconstructor {
public:
	HShowerRawHist(){fIter = NULL;}

        HShowerRawHist(const Text_t *name,const Text_t *title)
                    : HReconstructor(name, title) {fIter = NULL;} 
	~HShowerRawHist();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t execute(void);

        HCategory* getRawCat(){return m_pRawCat;}
        void setRawCat(HCategory* pRawCat){m_pRawCat = pRawCat;}

        ClassDef(HShowerRawHist,1) //ROOT extension

        Bool_t bookHist();
        Bool_t fillHist(HShowerRaw* pRaw);
        Bool_t fillHist(Int_t nModule, Int_t nRow, Int_t nColumn, Float_t fCharge);
        Bool_t writeHist();
        Bool_t finalizeHist();

private:
        Int_t m_nEvents;
        HCategory *m_pRawCat; //!Pointer to the raw data category

        HIterator *fIter;  //!

        TH1F* m_pChargeHist;  //!
        TH1F* m_pChargeModHist[3];  //!        
        TH2F* m_pChargeFreqHist[3];  //!        
        TH2F* m_pChargeAvgHist[3];  //!        
};

#endif
