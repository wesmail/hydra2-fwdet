#ifndef HShowerTofinoCorrelator_h
#define HShowerTofinoCorrelator_h

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hparcond.h"

class HCategory;
class HIterator;
class HShowerHit;
class HShowerHitTof;

class HShowerTofinoCorrelator : public HReconstructor {
public:
    HShowerTofinoCorrelator();
    HShowerTofinoCorrelator(const Text_t *name,const Text_t *title);
    ~HShowerTofinoCorrelator();

    Bool_t init(void);
    Bool_t finalize(void);
    Int_t  execute(void);


    void   setSimulationFlag(Bool_t bSim = kFALSE) {m_bIsSimulation = bSim;}
    Bool_t isSimulation     (void)                 {return m_bIsSimulation;}

    void   setLowShowerEfficiencyFlag(Bool_t bLowEfficiency = kFALSE) {m_bLowShowerEfficiency = bLowEfficiency;}
    Bool_t isLowShowerEfficiency     (void)                           {return m_bLowShowerEfficiency;}

    ClassDef(HShowerTofinoCorrelator,2) // Produce Tofino/Shower HIT data

private:
    HLocation m_zeroLoc;

    HCategory *m_pTofinoCat;   //!Pointer to the Tofino CAL data category
    HCategory *m_pHitCat;      //!Pointer to the Shower HIT data category
    HCategory *m_pHitTofCat;   //!Pointer to the Tofino HIT data category

    HIterator *fHitIter;       //!Iterator for Shower HIT
    HIterator *fTofinoIter;    //!Iterator for Tofino CAL
    HIterator *fHitTofinoIter; //!Iterator for combined data

    HParSet* m_pTofinoCalPar;   //!Tofino's calibration parameters
    HParSet* m_pTofinoDigitPar; //!Tofino's digitisation parameters
    HParSet* m_pTofShowerMap;   //!Shower-Tofino Map

    Bool_t m_bIsSimulation;        // simulation flag
    Bool_t m_bLowShowerEfficiency; // low shower efficiency

    HShowerHitTof* addHitTof(HShowerHit* pHit);

    HCategory* getTofinoCat() {return m_pTofinoCat;}
    HCategory* getHitCat   () {return m_pHitCat;}
    HCategory* getHitTofCat() {return m_pHitTofCat;}

    void   setTofinoCat(HCategory* pTofinoCat) {m_pTofinoCat = pTofinoCat;}
    void   setHitCat   (HCategory* pHitCat)    {m_pHitCat = pHitCat;}
    void   setHitTofCat(HCategory* pHitTofCat) {m_pHitTofCat = pHitTofCat;}

    Float_t calcDriftTime (Int_t mode,Int_t nSector,Int_t nTofCell,Float_t nRow);
    Int_t   getADCPedestal(Int_t mode,HLocation& loc1);
    void    getElossParams(Int_t mode,HLocation& loc1,Float_t* pEloss);
    Float_t calcDistance  (Int_t mode,Int_t nSector,Int_t nTofCell,Float_t nRow);

};

#endif
