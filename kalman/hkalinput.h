#ifndef HKalInput_h
#define HKalInput_h

// from ROOT
#include "TObjArray.h"
#include "TVector3.h"

// from hydra
class HMdcSizesCells;
class HMdcTrackGFieldPar;
class HMagnetPar;
class HMdcTrkCand;
class HMdcTrkCandIdeal;
class HGeantKine;
class HGeantMdc;
class HMdcGetContainers;
class HMdcSeg;
class HMdcSegIdeal;
class HCategory;
#include "hiterator.h"
// from kalman
#include "hkaldetcradle.h"

class HKalInput : public TObject {

private:
    Bool_t bPrint;                          //! Debug prints.
    HMdcTrackGFieldPar *pField;             //!
    HMdcSizesCells     *fSizesCells;        //!
    HMagnetPar         *pMagnet;            //!
    HCategory          *geantMdcCat;        //!
    HCategory          *kineCat;            //!
    HCategory          *mdcCal1Cat;         //!
    HCategory          *mdcHitCat;          //!
    HCategory          *mdcTrkCandCat;      //!
    HCategory          *mdcSegCat;          //!
    HCategory          *mdcHitIdealCat;     //!
    HCategory          *mdcSegIdealCat;     //!
    HCategory          *mdcTrkCandIdealCat; //!
    HCategory          *metaMatchCat;       //!
    HIterator          *iterKine;           //!
    HIterator          *iterMdcTrkCand;     //!
    HIterator          *iterMdcTrkCandIdeal;//!
    HIterator          *iterMetaMatch;      //!
    HKalDetCradle      *pCradleHades;       //! Pointer to detector cradle.
    HMdcGetContainers  *mdcGetContainers;   //!

public:

    HKalInput();

    HKalInput(HKalDetCradle *detCradle);

    virtual ~HKalInput();

    static  void                calcSegPoints           (const HMdcSeg *seg, Double_t& x1,  Double_t& y1, Double_t& z1, Double_t& x2, Double_t& y2, Double_t& z2);

    virtual void                clearCategories         ();

    virtual void                getCategories           ();

    virtual void                resetIterKine           () { if(iterKine) iterKine->Reset(); }

    virtual void                resetIterMdcTrkCand     () { if(iterMdcTrkCand) iterMdcTrkCand->Reset(); }

    virtual void                resetIterMdcTrkCandIdeal() { if(iterMdcTrkCandIdeal) iterMdcTrkCandIdeal->Reset(); }

    virtual void                resetIterMetaMatch      () { if(iterMetaMatch) iterMetaMatch->Reset(); }

    virtual Bool_t              init                 (Int_t refID);

    virtual Bool_t              initBField           (Int_t refID);

    virtual Bool_t              initSizesCells       (Int_t refID);

    virtual HMdcSizesCells*     getSizesCells        ();

    virtual HMdcTrackGFieldPar* getGFieldPar         () { return pField; }

    virtual HMagnetPar*         getMagnetPar         () { return pMagnet; }

    virtual HMdcTrkCand*        nextMdcTrackCand     (TObjArray &allhits, Int_t measDim=2, Double_t scaleErr=1.);

    virtual HMdcTrkCand*        nextWireTrack        (TObjArray &allhits, Int_t minLayInSeg=5, Int_t minLayOutSeg=5);

    virtual HMdcTrkCand*        nextWireTrackNoComp  (TObjArray &allhits, Int_t minLayInSeg=5, Int_t minLayOutSeg=5);

    virtual HGeantKine*         getGeantKine         (const HMdcTrkCand *cand);

    virtual HGeantKine*         getGeantKine         (const HMdcTrkCandIdeal *cand);

    virtual void                getGeantMdcRawpoints (TObjArray &allhits, HGeantKine *kine, Bool_t midplane);

    virtual void                getPosAndDir         (TVector3 &pos, TVector3 &dir, HGeantMdc *geantMdc, Bool_t sectorCoord);

    virtual void                getMdcSegs           (const HMdcTrkCand *cand, HMdcSeg **segs);

    virtual void                getMdcSegs           (const HMdcTrkCandIdeal *cand, HMdcSegIdeal **segs);

    virtual void                setDetectorCradle    (HKalDetCradle *detCradle) { pCradleHades = detCradle; }
    virtual const HKalDetCradle* getDetectorCradle   () const { return pCradleHades ; }

    virtual void                setPrint             (Bool_t print) { bPrint = print; }

    ClassDef(HKalInput, 1)
};


#endif // HKalInput_h
