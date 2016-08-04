#ifndef HShowerHitFinder_H
#define HShowerHitFinder_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hratree.h"
#include "hspecgeompar.h"
#include "hgeomvector.h"

class HCategory;
class HIterator;
class HShowerCal;
class HShowerHit;
class HShowerPID;
class HShowerHitHeader;
class HShowerCriterium;
class HShowerHitFPar;
class HShowerGeometry;
class HSpecGeomPar;

class HShowerHitFinder : public HReconstructor {
private:
    HLocation          m_Loc;            //!
    HRaTree           *m_pCellArr;       //! random access table for searching local maxima

    HCategory         *m_pCalCat;        //! Pointer to the cal data category
    HCategory         *m_pHitCat;        //! Pointer to the hit data category
    HCategory         *m_pPIDCat;        //! Pointer to the hit data category
    HCategory         *m_pHitHdrCat;     //! Pointer to the hit header data category

    HShowerHitFPar    *m_pHitFPar;       //! Pointer to the hit finder parameters container
    HShowerGeometry   *m_pGeometry;      //! Pointer to geometry parameters container
    HSpecGeomPar      *m_pHSpecGeomPar;

    HIterator         *fIter;            //! Iterator for calibrated fired pads
    Bool_t            m_bIsFillPID;      //! flag for filling PID level - default TRUE
    Bool_t            m_bIsSort;         //! flag for sorting data by m_nAddress - default FALSE

    HShowerCriterium *m_pCriterium;      //! definition of shower criterium

    Bool_t isSim ;                       //! is simulation ?

public:
    HShowerHitFinder();
    HShowerHitFinder(const Text_t *name,const Text_t *title);
    ~HShowerHitFinder();

    Int_t  execute       (void);
    Bool_t init          (void);
    Bool_t finalize      (void);
    Bool_t initParameters(void);

    Bool_t  lookForHit(HShowerCal* cal, HLocation &fLoc);
    Float_t calculateSum(HLocation &fLoc, Int_t nRange, Int_t* pncs = NULL);
    Float_t calculateVar(HLocation &fLoc, Int_t nRange, Float_t avg);
    Bool_t  isLocalMax  (HLocation &fLoc);
    HShowerHitFinder &operator=(HShowerHitFinder &c);

    HCategory*       getCalCat      () { return m_pCalCat;}
    HCategory*       getHitCat      () { return m_pHitCat;}
    HCategory*       getPIDCat      () { return m_pPIDCat;}
    HShowerHitFPar*  getHitFPar     () { return m_pHitFPar;}
    HShowerGeometry* getGeometry    () { return m_pGeometry;}
    HSpecGeomPar*    getHSpecGeomPar() { return m_pHSpecGeomPar;}


    void setCalCat(HCategory* pCalCat)               { m_pCalCat = pCalCat;}
    void setHitCat(HCategory* pHitCat)               { m_pHitCat = pHitCat;}
    void setPIDCat(HCategory* pPIDCat)               { m_pPIDCat = pPIDCat;}
    void setHitFPar(HShowerHitFPar* pPar)            { m_pHitFPar= pPar;}
    void setGeometry(HShowerGeometry* pGeometry)     { m_pGeometry= pGeometry;}
    void setCriterium(HShowerCriterium* pCrit);
    void setHSpecGeomPar(HSpecGeomPar *pSpecGeometry){ m_pHSpecGeomPar = pSpecGeometry;}

    Bool_t IsSortFlagSet()                           { return m_bIsSort;}
    Bool_t IsFillPID()                               { return m_bIsFillPID;}
    void setSortFlag(Bool_t bSort = kTRUE)           { m_bIsSort = bSort;}
    void setFillPID(Bool_t bIsFillPID = kTRUE)       { m_bIsFillPID = bIsFillPID;}

private:
    void fillSums(HShowerHit* hit, HLocation &fLoc);
    virtual void calcCoord(HShowerHit* hit, HLocation &fLoc);
    virtual void calcCoordWithSigma(HShowerHit* hit, HLocation &fLoc, Int_t nRange);


    void fillPID(HShowerHit* hit, HShowerPID* pid);

    HShowerHitHeader* getHitHeader(HLocation &fLoc);
    void updateClusters(HLocation &fLoc);
    void updateLocalMax(HLocation &fLoc);
    void updateFiredCells(HLocation &fLoc);

    ClassDef(HShowerHitFinder,0) //ROOT extension
};

#endif
