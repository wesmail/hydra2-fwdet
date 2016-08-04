#ifndef HShowerCopy_H
#define HShowerCopy_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hcategory.h"
#include "hshowerraw.h"

class HCategory;
class HIterator;

class HShowerCopy : public HReconstructor {
public:
        HShowerCopy();
        HShowerCopy(const Text_t *name,const Text_t *title,Float_t masterOffset=1.0);
       ~HShowerCopy();

        Int_t execute(void);
        virtual Bool_t init(void);
        Bool_t finalize(void) {return kTRUE;}

        void setInCat(HCategory* pInCat){m_pInCat = pInCat;}
        void setOutCat(HCategory* pOutCat){m_pOutCat = pOutCat;}

        Bool_t copy(HShowerRawMatr *pMatr);

        Int_t m_nEvents;

private:
        HLocation m_zeroLoc;
        HCategory *m_pInCat; //!Pointer to the rawMatr data category
        HCategory *m_pOutCat; //!Pointer to the raw  data category

        HIterator* fIter;     //!
        HIterator* fCalIter;  //!

	Float_t mOffset;

        ClassDef(HShowerCopy,0) //ROOT extension
};

#endif
