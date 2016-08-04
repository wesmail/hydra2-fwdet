#ifndef HMDCCLUSSIM_H
#define HMDCCLUSSIM_H

#include "hmdcclus.h"

class HMdcClusSim : public HMdcClus {
protected:
  Int_t   nTracks;         // num. of GEANT tracks in cluster
  Int_t   listTr[5];       // list of tracks
  Short_t nTimes[5];       // num. of hits in cluster from track
  UChar_t nLayers[5][2];   // list of layers with hits from track (bits array)
  UChar_t nDigiTimes[5];   // num. of digitized dr.times in segment (or module)
  UChar_t trackStatus[5];  //
  Float_t xGeant[5];       // GEANT hit position on the proj.plane
  Float_t yGeant[5];       // GEANT hit position on the proj.plane
  Char_t  nNotFakeTracks;  // Number of tracks with non fake contribution
  Char_t  ioSMatchStatus;  //  > 0 - num. of not fake tracks with IOseg matching
                           //  = 0 - no matching
                           //  =-1 - old files, cham.clust. or no outer seg.
  
  Int_t   nTracksM[2];     //! [mod] num. of GEANT tracks in cluster
  Int_t   listTrM[2][5];   //! list of tracks
  Short_t nTimesM[2][5];   //! num. of hits in cluster from track
public:
  HMdcClusSim(void) {setDef();}
  HMdcClusSim(HMdcList12GroupCells& lcells)  : HMdcClus(lcells) {setDef();}
  ~HMdcClusSim() {}
  
  void    setDef(void);
  void    setNTracks(Int_t nTrs)                  {nTracks = nTrs; cleanRest();}
  void    cleanRest(void);
  void    clear(void)                             {setDef(); HMdcClus::clear();}
  Int_t   addTrack(Int_t tr,Short_t nTm,UChar_t lM1,UChar_t lM2);
  Bool_t  resetTrack(Int_t ind,Int_t tr,Short_t nTm,UChar_t lM1,UChar_t lM2);
  void    setNTracksM(Int_t modi,Int_t nTrs);
  void    setTrackM(Int_t modi,Int_t ind,Int_t tr,Short_t nTm);
  void    setNDigiTimes(Int_t ind,Int_t nt);
  void    setTrackStatus(Int_t ind,UChar_t status);
  void    setXYGeant(Int_t ind,Float_t xg,Float_t yg);
  void    calcTrList(void);
  void    calcTrListMod(HMdcList12GroupCells& list,Int_t m);
  void    sortTrListByContr(void);

  Bool_t  isGeant(void) const                     {return kTRUE;}
  
  const Int_t*   getTrackList(void) const   {return listTr;}
  const Short_t* getNumWires(void) const    {return nTimes;}
  const UChar_t* getNDigiTimes(void) const  {return nDigiTimes;}
  const UChar_t* getTrackStatus(void) const {return trackStatus;}
  
  
  
  
  Int_t   getArrsSize(void) const                 {return 5;}
  Int_t   getNTracks(void) const                  {return nTracks;}
  Char_t  getNNotFakeTracks(void) const           {return nNotFakeTracks;}
  Int_t   getNMatchedTracks(void) const;
  Int_t   getTrack(Int_t indx) const;
  Short_t getNTimesInTrack(Int_t indx) const;
  Int_t   getNLayersInTrack(Int_t indx,Int_t m=-1) const;
  UChar_t getLayListForMod(Int_t indx,Int_t mi) const;
  UChar_t getNLayOrientation(Int_t indx,Int_t mi=-1) const;
  Bool_t  is40degCross(Int_t indx,Int_t mi=-1) const;
  UChar_t getNDigiTimesInTrack(Int_t indx) const  {return indOk(indx) ? nDigiTimes[indx]:0;}
  
  Int_t   getNTracksMod(Int_t m) const            {return nTracksM[m&1];}
  Int_t   getTrackMod(Int_t m,Int_t indx) const;
  Short_t getNTimesInMod(Int_t m,Int_t indx) const;
  Int_t   getTrackIndex(Int_t tr) const;
  Int_t   getNoiseIndex(void) const;
  const Int_t*   getListTrM(Int_t m) const        {return listTrM[m&1];}
  const Short_t* getNTimesM(Int_t m) const        {return nTimesM[m&1];}
  UChar_t getTrackStatus(Int_t ind) const;
  Bool_t  getXYGeant(Int_t i,Float_t& xg,
                             Float_t& yg) const;
  Float_t getXGeant(Int_t i) const                {return indOk(i) ? xGeant[i]:-10000.;}
  Float_t getYGeant(Int_t i) const                {return indOk(i) ? yGeant[i]:-10000.;}
  Float_t dX(Int_t i) const                       {return indOk(i) ? xGeant[i]-x:-10000.;}
  Float_t dY(Int_t i) const                       {return indOk(i) ? yGeant[i]-y:-10000.;}
  
  Bool_t  isGntTrackOk(Int_t i) const             {return  testBit(i,255);}
  Bool_t  isIOSegMatch(Int_t i) const             {return  testBit(i,1);}
  Bool_t  noIOSegMatch(Int_t i) const             {return !testBit(i,1);}
  Bool_t  isFakeContribution(Int_t i) const       {return !testBit(i,2);}
  Bool_t  isClFnLevelTooHigh(Int_t i) const       {return !testBit(i,4);}
  Bool_t  is1or2HitsNotRec(Int_t i) const         {return !testBit(i,8);}
  Bool_t  isSegNotRec(Int_t i) const              {return !testBit(i,16);}
  Bool_t  isTrackNotRec(Int_t i) const            {return !testBit(i,32);}
  Bool_t  isNoMeta(Int_t i) const                 {return !testBit(i,64);}
  Bool_t  isGEANTBug(Int_t i) const               {return !testBit(i,128);}
  Char_t  getIOSegMatchingStatus(void) const      {return ioSMatchStatus;}
  
  void    printCont(Bool_t fl=kTRUE) const;
  
private:
  Bool_t  indOk(Int_t i) const             {return i>=0 && i<nTracks;}
  UChar_t trackBits(Int_t i) const         {return indOk(i) ? trackStatus[i]:0;}
  Bool_t  testBit(Int_t i,UChar_t b) const {return (trackBits(i)&b) == b;}
  void    exchangeTrPos(Int_t t1,Int_t t2);
  
  ClassDef(HMdcClusSim,1)
};

#endif
