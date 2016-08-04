#ifndef HShowerPIDTrackMatcher_H
#define HShowerPIDTrackMatcher_H

#include "hreconstructor.h"
#include "hlocation.h"
#include "hparset.h"
#include "hshowerraw.h"

class HCategory;
class HIterator;
class HShowerPID;
class HShowerTrack;

class HShowerPIDTrackMatcher : public HReconstructor {
public:
        HShowerPIDTrackMatcher();
        HShowerPIDTrackMatcher(const Text_t *name,const Text_t *title);
       ~HShowerPIDTrackMatcher();

        Bool_t init(void);
        Bool_t finalize(void);
        Int_t execute(void);

        void addPIDTrack(HShowerPID* pPID, HShowerTrack* pTrack);

        HCategory* getTrackCat(){return m_pTrackCat;}
        HCategory* getPIDCat(){return m_pPIDCat;}
        HCategory* getPIDTrack(){return m_pPIDTrackCat;}

        void setTrackCat(HCategory* pTrackCat){m_pTrackCat = pTrackCat;}
        void setPIDCat(HCategory* pPIDCat){m_pPIDCat = pPIDCat;}
        void setPIDTrack(HCategory* pPIDTrackCat){m_pPIDTrackCat=pPIDTrackCat;}

        ClassDef(HShowerPIDTrackMatcher,0) //ROOT extension

private:
        HLocation m_zeroLoc;

        HCategory *m_pTrackCat; //!Pointer to the track data category
        HCategory *m_pPIDCat; //!Pointer to the PID data category
        HCategory *m_pPIDTrackCat; //!Pointer to the PIDtrack data category

        HIterator *fPIDIter;    //!Iterator for local maxima in PID
        HIterator *fTrackIter;  //!Iterator for tracks
};

#endif
