#ifndef HMDCALIGNERD_H
#define HMDCALIGNERD_H

#include "hmdctrackfinder.h"
#include "hmdcstoreevents.h"

class HGeomTransform;
class HGeomVector;
class HMdcEvntListCells;
class HMdcClus;
class HMdcTrackFitInOut;
class HMdcTrackFitter;
class HMdcTrackFitterA;
class HMdcTrackFitterB;
class HMdcSizesCellsMod;
class HMdcAlignerParam;

class HMdcAlignerD : public HMdcTrackFinder {
  protected:
    HMdcEvntListCells * event;
    HMdcStoreEvents storeWires;
    HMdcStoreClusters * storeClusters;
//     Int_t alignSec;
    HMdcTrackFitInOut * fitpar;
    HMdcTrackFitter * fitter;
    HMdcAlignerParam * param;
  public:
//     static HMdcAlignerD * obj;
    HMdcAlignerD(Bool_t isCOff, Int_t typeClFn=0);
    HMdcAlignerD(const Text_t *name,const Text_t *title, Bool_t isCOff, Int_t typeClFn=0);
    HMdcAlignerD(void);
    HMdcAlignerD(const Text_t *name,const Text_t *title);
    ~HMdcAlignerD(void);
    virtual Bool_t init(void);
    virtual Bool_t reinit(void);
    virtual Int_t execute(void);
    virtual Bool_t finalize(void);
    void monitor(HGeomVector * target = 0);
//     void setAlignSec(Int_t s) {alignSec=s;}
//     Int_t getAlignSec() {return alignSec;}
    HMdcStoreEvents * getStoreWires() {return &storeWires;}
    HMdcStoreClusters * getStoreClusters() {return storeClusters;}
    HMdcEvntListCells * getEvent() {return event;}
    HMdcTrackFitter * getFitter() {return fitter;}
    HMdcAlignerParam * getParam() {return param;}

  protected:
    void printCluster(HMdcClus* fClst);

  ClassDef(HMdcAlignerD,0) //Digitizes MDC data.
};

#endif
