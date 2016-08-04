#ifndef HShowerPadDigitizer_H
#define HShowerPadDigitizer_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hiterator.h"
#include "hshowerpad.h"
#include "hshowerraw.h"
#include "hshowerdigitizer.h"
#include "hshowergeantwire.h"

#include <map>
#include <vector>

class HCategory;
class HLinearCategory;
class HShowerRaw;

#define MAX_PADS_DIST 3

class HShowerPadDigitizer : public HShowerDigitizer {
public:
   HShowerPadDigitizer();
   HShowerPadDigitizer(const Text_t *name,const Text_t *title);
   ~HShowerPadDigitizer();

   virtual Int_t execute(void);
   Bool_t digitize(TObject *pHit);
   HShowerPadDigitizer &operator=(HShowerPadDigitizer &c);

   HCategory* getTrackCat(){return m_pTrackCat;}
   void setTrackCat(HCategory* pTrackCat){m_pTrackCat = pTrackCat;}

   Bool_t init(void);
   Bool_t finalize(void) {return kTRUE;}

   Bool_t checkEfficiency(HShowerRawMatr *pRaw);
   Double_t gainCharge(HShowerRawMatr *pRaw);
   static void setModeTrack(Int_t mode) { modeTrack = mode;}
   ClassDef(HShowerPadDigitizer,0) // Shower pad digitizer

private:
   Int_t sort(void);
   Float_t calcCharge(Float_t charge, Float_t dist,
                      Float_t Xd, Float_t Yd, Float_t Xu, Float_t Yu);
   void digiPads(HShowerGeantWire* pWireHit);
   Int_t calcLimit(Float_t fCor, Int_t nMatrixRange, Float_t fBoxSize);
   void moveCoord(HShowerPad *pPad, Float_t distWire, Float_t fDx, Float_t fDy,
           Float_t *corXld, Float_t *corYld, Float_t *corXlu, Float_t *corYlu,
           Float_t *corXrd, Float_t *corYrd, Float_t *corXru, Float_t *corYru);
   void analyticCalc(HShowerPad *pPad, HShowerGeantWire* pWireHit);

   Int_t numericalCalc(HShowerPad *pPad, HShowerGeantWire* pWireHit,
                Float_t pfValues[MAX_PADS_DIST]);

   void updatePad(HShowerPad *pPad, Float_t fIndQ, Int_t nSect,
                  Int_t nMod, Int_t nTrack);
   Int_t findFirstHitInShower(Int_t trackID);
   Int_t findFirstHitInRpc(Int_t trackID);
   Int_t findFirstHit(Int_t trackID);
   Float_t fChannelCoeff;

   HCategory *m_pTrackCat;    //!Pointer to the raw data category
   HIterator* fTrackIter;     //!
   HIterator* fShowerCalIter; //!
   HIterator* fShowerRawMatrIter; //!


   HLinearCategory* fGeantKineCat; //!
   HLinearCategory* fGeantShowerCat; //!
   HLinearCategory* fGeantTofCat;    //!
   HLinearCategory* fGeantRpcCat;    //!
   Bool_t isRpc;                     //! is rpc used ?

   static Int_t modeTrack;    // mode select for storing the track numbers :
                              // 0 = registrered tracks,
                              // 1 = primaries of registered tracks,
                              // 2 = tracks first time entrering the SHOWER
                              // 3 = tracks first time entrering the TOF (default)

   std::map<Int_t , std::vector<Int_t> > trackMap;    //!


};

#endif
