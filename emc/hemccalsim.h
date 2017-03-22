#ifndef HEMCCALSIM_H
#define HEMCCALSIM_H

#include "hemccal.h"

class HEmcCalSim : public HEmcCal {
protected:
  Short_t nTracks;        // number of tracks in list
  Short_t totMult;        // total number of tracks hitting this crystal
  Int_t   listTracks[5];  // list of tracknumbers (GEANT)
  Float_t trackEnergy[5]; // energy deposit for each track
  Int_t   time1track;     // track number of time1
  Int_t   time2track;     // track number of time2

public:
  HEmcCalSim() :
    nTracks(0),
    totMult(0),
    time1track(0),
    time2track(0)
  {
    for(Int_t i=0;i<5;i++) {
      listTracks[i]  = 0;
      trackEnergy[i] = 0.F;
    }
  }
  ~HEmcCalSim() {}
  
  void    setTrack(Int_t trackNumber, Float_t energy);
  void    setTotMult(Int_t n)        {totMult     = n;}
  void    setTime1Track(Int_t tr)    {time1track  = tr;}
  void    setTime2Track(Int_t tr)    {time2track  = tr;}
   
  Short_t getNTracks(void)          const  {return nTracks;}
  Int_t   getTrack(Short_t n=0)     const  {return n>=0&&n<nTracks ? listTracks[n]  : 0;}
  Float_t getTrackEnergy(Short_t n) const  {return n>=0&&n<nTracks ? trackEnergy[n] : 0.F;}
  Short_t getTotMult(void)          const  {return totMult;} 
  Int_t   getTime1Track(void)       const  {return time1track;}
  Int_t   getTime2Track(void)       const  {return time2track;}

  ClassDef(HEmcCalSim,1)
};



#endif  /* HEMCCALSIM_H */
