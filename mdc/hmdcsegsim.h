#ifndef HMDCSEGSIM_H
#define HMDCSEGSIM_H

#include "hmdcseg.h"

class HMdcSegSim : public HMdcSeg {
  protected:
    Short_t nTracks;        // number of tracks in the list of tracks
    Int_t   listTracks[5];  // list of tracks
    UChar_t nTimes[5];      // number of times from each track
    
    UChar_t nDigiTimes[5];  // num. of digitized dr.times in segment (or module)
    UChar_t trackStatus[5]; //
    Char_t  nNotFakeTracks; // Number of tracks with non fake contribution
    Char_t  ioSMatchStatus; //  > 0 - num. of not fake tracks with IOseg matching
    
    Int_t   status;         // status of seg. (e.g. 1 if ok)
  public:
    HMdcSegSim(void) {clear();}
    ~HMdcSegSim(void) {}
    void  clear(void);
    void  clearSimInfo(void);
    void  setStatus(Int_t f) { status=f; }
    void  setNTracks(Int_t nTr, const Int_t* listTr, const UChar_t* nTm,const UChar_t* nDi=0);
    void  addTrack(Int_t track, UChar_t nTm,UChar_t nDi=0);
    Int_t calcNTracks(void);
    void  setTrackInf(const Int_t *lst,const Short_t *nTms,const UChar_t *nDTms,const UChar_t *tSt);
    inline void  setNTracks(Int_t nTr,Char_t nNF,Char_t ioS);
    void  setNDigiTimes(Int_t n,UChar_t nt)         {if(indOk(n)) nDigiTimes[n] = nt;}
    void  setTrackStatus(Int_t n,UChar_t status)    {if(indOk(n)) trackStatus[n] = status;}
    
    inline Int_t   getStatus(void) const            {return status;}
    inline Int_t   getNTracks(void) const           {return nTracks;}
    inline Char_t  getNNotFakeTracks(void) const    {return nNotFakeTracks;}
    inline Int_t   getTrack(Int_t n) const;
    inline UChar_t getNTimes(Int_t n) const;
    inline UChar_t getNDigiTimes(Int_t n) const;
    UChar_t        getTrackStatus(Int_t n) const;
    Int_t          getNumNoiseWires(void) const;
    void           sortTrListByContr(void);
    Int_t          getGoodTrack(Int_t i1,HMdcSegSim* outerSeg,Int_t nWiresCut=5) const;
    Int_t          getNextGoodTrack(Int_t& i1,HMdcSegSim* outerSeg,Int_t nWiresCut=5) const;
    
    Bool_t  isGntTrackOk(Int_t i) const             {return  testBit(i,254);}
    Bool_t  isFakeContribution(Int_t i) const       {return !testBit(i,2);}
    Bool_t  isClFnLevelTooHigh(Int_t i) const       {return !testBit(i,4);}
    Bool_t  is1or2HitsNotRec(Int_t i) const         {return !testBit(i,8);}
    Bool_t  isSegNotRec(Int_t i) const              {return !testBit(i,16);}
    Bool_t  isTrackNotRec(Int_t i) const            {return !testBit(i,32);}
    Bool_t  isNoMeta(Int_t i) const                 {return !testBit(i,64);}
    Bool_t  isGEANTBug(Int_t i) const               {return !testBit(i,128);}
    Int_t   findTrack(Int_t geantNum,Int_t nDigTimes=0) const;
    Int_t   getTrackIndex(Int_t tr) const;
    Bool_t  resetFakeContributionFlag(void);
    Bool_t  setFakeContributionFlag(void);

    void print(void);
    
private:
  Bool_t  indOk(Int_t i) const             {return i>=0 && i<nTracks;}
  UChar_t trackBits(Int_t i) const         {return indOk(i) ? trackStatus[i]:0;}
  Bool_t  testBit(Int_t i,UChar_t b) const {return (trackBits(i)&b) == b;}
  void    exchangeTrPos(Int_t t1,Int_t t2);

  ClassDef(HMdcSegSim,1) // simulated seg hit on a MDC 
};

inline Int_t HMdcSegSim::getTrack(Int_t n) const {
  return indOk(n) ? listTracks[n] : -1;
}

inline UChar_t HMdcSegSim::getNTimes(Int_t n) const {
  return indOk(n) ? nTimes[n] : 0;
}
 
inline UChar_t HMdcSegSim::getNDigiTimes(Int_t n) const  {
  return indOk(n) ? nDigiTimes[n] : 0;
}

inline void HMdcSegSim::setNTracks(Int_t nTr,Char_t nNF,Char_t ioS) {
  nTracks        = nTr;
  nNotFakeTracks = nNF; 
  ioSMatchStatus = ioS;
}

#endif  /*HMDCSEGSIM_H */
