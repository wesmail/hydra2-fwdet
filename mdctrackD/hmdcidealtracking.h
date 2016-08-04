#ifndef HMDCIDEALTRACKING_H
#define HMDCIDEALTRACKING_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hmdclistgroupcells.h"

class HIterator;
class HMdcHitSim;
class HMdcSegSim;
class HCategory;
class HMdcTrkCand;
class HMdcSizesCells;
class HGeantKine;
class HGeantMdc;

class HMdcIdealTracking : public HReconstructor {
  private:
    // Input categories:
    HCategory* pGeantKineCat;      // pointer to the category catGeantKine
    HCategory* pGeantMdcCat;       // pointer to the category catMdcGeantRaw
    HCategory* pMdcCal1Cat;        // pointer to the category catMdcCal1
    HIterator* iterGeantKine;      // iterator for catGeantKine
    HLocation  locCal1;            // location of HMdcCal1Sim object
    
    // Output categories:
    HCategory* pMdcSegCat;         // pointer to the category catMdcSeg
    HCategory* pMdcHitCat;         // pointer to the category catMdcHit
    HCategory* pMdcTrkCandCat;     // pointer to the category catMdcTrkCand    
    HLocation  locSeg;             // location for new "segment" object
    HLocation  locHit;             // location for new "hit" object
    HLocation  locTrkCand;         // location for new HMdcTrkCand object

    HMdcSizesCells* pMSizesCells;
    Bool_t isMdcActive[6][4];     // [sec][mod] =kTRUE if MDC is active
    
    // Cuts:
    Int_t nFiredLayersSeg1cut;    // minimal number of fired layers in inner segment
    Int_t nFiredLayersSeg2cut;    // minimal number of fired layers in outer segment
    
    // Current track:
    Int_t      trackNumber;       // track number
    Int_t      trackSector;       // sector number of track
    HGeantMdc* geantHitMod[4];    // geant hits in 4 modules of sector
    HGeantMdc* geantHitLay[4][6]; // geant hits in 24 layers of sector
    UChar_t    nGeantMdcLay[4];   // num. of HGeantMdc or HMdcCal1 hits in each module
    Double_t   x1[4],y1[4],z1[4]; // x1,y1,z1 - x2,y2,z2 track piece param.
    Double_t   x2[4],y2[4],z2[4]; // in sector coor.sys. for each module
    HMdcList24GroupCells* lCells; // list of fired cells from this track
    Bool_t     fillParallel;      // switch : kTRUE will fill HMdcHitSim,HMdcSegSim and HMdcTrkCand to ideal categories

    Double_t   sigX[4];           // sigma for gaussian resolution for MDC in x [mm]
    Double_t   sigY[4];           // sigma for gaussian resolution for MDC in y [mm]

  public:
    HMdcIdealTracking(void);
    HMdcIdealTracking(const Text_t *name,const Text_t *title);
    ~HMdcIdealTracking(void);
    void   fillParallelCategories(){fillParallel=kTRUE;}
    void   setResolutionX(Double_t x0,Double_t x1,Double_t x2,Double_t x3) {sigX[0] = x0; sigX[1] = x1; sigX[2] = x2; sigX[3] = x3;}
    void   setResolutionY(Double_t y0,Double_t y1,Double_t y2,Double_t y3) {sigY[0] = y0; sigY[1] = y1; sigY[2] = y2; sigY[3] = y3;}
    Bool_t init(void);
    Bool_t reinit(void);
    Bool_t finalize(void);
    Int_t  execute(void);
    void   setNFiredLayersCuts(Int_t c1,Int_t c2) {nFiredLayersSeg1cut=c1; nFiredLayersSeg2cut=c2;}
    void printStatus(void);

  private:
    void         clear(void);
    Bool_t       testTrack(HGeantKine* pGeantKine);
    Int_t        fillHitsSeg(Int_t segment);
    HMdcSegSim*  getSegSlot(Int_t segment, Int_t& index);
    Int_t        fillHit(Int_t module);
    HMdcHitSim*  getHitSlot(Int_t module, Int_t& index);
    HMdcTrkCand* fillTrkCandISeg(Int_t segIndex);
    HMdcTrkCand* fillTrkCandOSeg(HMdcTrkCand* fTCand,Int_t segIndex);
    void         collectWires(Int_t s,Int_t m, Int_t l, HGeantMdc* pGeantMdc);
    void         setWires(HMdcSegSim* pMdcSeg, Int_t seg);
    void         setWires(HMdcHitSim* pMdcHit, Int_t mod);

  ClassDef(HMdcIdealTracking,0) // Filling HMdcSeg, HMdcHit and HMdcTrkCand by GEANT hits
};

#endif
