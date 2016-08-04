#ifndef HShowerParticleIdentifier_H
#define HShowerParticleIdentifier_H

#include "hreconstructor.h"
#include "hlocation.h"

class HCategory;
class HIterator;
class HShowerHit;
class HShowerPID;
class HShowerCriterium;
class HShowerHitFPar;

class HShowerParticleIdentifier: public HReconstructor {
public:
        HShowerParticleIdentifier ();
        HShowerParticleIdentifier(const Text_t *name,const Text_t *title);
       ~HShowerParticleIdentifier();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t execute(void);
        Bool_t initParameters(void);

        HCategory* getHitCat(){return m_pHitCat;}
        HCategory* getPIDCat(){return m_pPIDCat;}
        HShowerHitFPar* getHitFPar(){return m_pHitFPar;}

        void setHitCat(HCategory* pHitCat){m_pHitCat = pHitCat;}
        void setPIDCat(HCategory* pPIDCat){m_pPIDCat = pPIDCat;}
        void setHitFPar(HShowerHitFPar* pPar){m_pHitFPar= pPar;}
        void setCriterium(HShowerCriterium* pCrit);

        ClassDef(HShowerParticleIdentifier,0) //ROOT extension

private:
        void fillPID(HShowerHit* hit, HShowerPID* pid, Float_t fShower);

        HLocation m_zeroLoc;

        HCategory *m_pHitCat; //!Pointer to the raw data category
        HCategory *m_pPIDCat; //!Pointer to the cal data category
        HShowerHitFPar *m_pHitFPar; //Pointer to the analysis parameters container

        HShowerCriterium* m_pCriterium;  //!definition of shower criterium

        HIterator *fIter;  //!Iterator for hits
};

#endif
