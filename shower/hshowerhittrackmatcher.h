#ifndef HShowerHitTrackMatcher_H
#define HShowerHitTrackMatcher_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"

class HCategory;
class HIterator;
class HShowerHit;
class HShowerTrack;

class HShowerHitTrackMatcher : public HReconstructor {
public:
        HShowerHitTrackMatcher();
        HShowerHitTrackMatcher(const Text_t *name,const Text_t *title);
        ~HShowerHitTrackMatcher();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t  execute(void);

        HCategory* getTrackCat()   { return m_pTrackCat;}
        HCategory* getHitCat()     { return m_pHitCat;}

        void setTrackCat(HCategory* pTrackCat)   {m_pTrackCat    = pTrackCat;}
        void setHitCat  (HCategory* pHitCat)     {m_pHitCat      = pHitCat;}

        ClassDef(HShowerHitTrackMatcher,0) //ROOT extension

private:
        HLocation m_zeroLoc;

        HCategory *m_pTrackCat; //! Pointer to the track data category
        HCategory *m_pHitCat;   //! Pointer to the hit data category

        HIterator *fHitIter;    //! Iterator for local maxima
        HIterator *fTrackIter;  //! Iterator for tracks
};

#endif
