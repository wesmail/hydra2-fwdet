#ifndef HShowerCalibrater_H
#define HShowerCalibrater_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"

class HCategory;
class HIterator;

class HShowerCalibrater : public HReconstructor {
public:
        HShowerCalibrater();
        HShowerCalibrater(const Text_t *name,const Text_t *title);
       ~HShowerCalibrater();

        Bool_t init(void);
        Bool_t reinit(void);
        Bool_t finalize(void);
        Int_t execute(void);
        Bool_t calibrate(HShowerRaw *raw);
        HShowerCalibrater &operator=(HShowerCalibrater &c);

        HCategory* getRawCat(){return m_pRawCat;}
        HCategory* getCalCat(){return m_pCalCat;}
        HParSet* getCalPar(){return m_pCalPar;}

        void setRawCat(HCategory* pRawCat){m_pRawCat = pRawCat;}
        void setCalCat(HCategory* pCalCat){m_pCalCat = pCalCat;}
        void setCalPar(HParSet*);

        void initCalPar();

        ClassDef(HShowerCalibrater,0) //ROOT extension

private:
        HLocation m_zeroLoc;
        HLocation m_loc; //Location of object being calibrated
        HCategory *m_pRawCat; //!Pointer to the raw data category
        HCategory *m_pCalCat; //!Pointer to the cal data category

        HParSet *m_pCalPar; //!Pointer to the cal data category

        HIterator *fIter;  //!Iterator for fired pads
};

#endif
