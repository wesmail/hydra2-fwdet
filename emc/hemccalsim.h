#ifndef HEMCCALSIM_H
#define HEMCCALSIM_H

#include "hemccal.h"

class HEmcCalSim : public HEmcCal {
protected:
  Short_t nTracks;        // number of tracks in list
  Short_t totMult;        // total number of tracks hitting this crystal
  Int_t   listTracks[5];  // list of tracknumbers (GEANT)
  Float_t trackEnergy[5]; // energy deposit for each track
  Int_t   timeTrack;     // track number of time1
  Float_t sigmaEnergy;  //
  Float_t sigmaTime;   //

public:
  HEmcCalSim() :
    nTracks(0),
    totMult(0),
    timeTrack(0),
    sigmaEnergy(0),
    sigmaTime(0)
  {
    for(Int_t i=0;i<5;i++) {
      listTracks[i]  = 0;
      trackEnergy[i] = 0.F;
    }
  }
  ~HEmcCalSim() {}
  
  void    setTrack(Int_t trackNumber, Float_t energy);
  void    setTotMult(Int_t n)        {totMult     = n;}
  void    setTimeTrack(Int_t tr)     {timeTrack  = tr;}
  void    setStatus(Int_t f)         {statusTime  = f;}
  void    setSigmaEnergy(Float_t e)  {sigmaEnergy = e;}
  void    setSigmaTime(Float_t t)    {sigmaTime   = t;}

  Short_t getNTracks(void)          const  {return nTracks;}
  Int_t   getTrack(Short_t n=0)     const  {return n>=0&&n<nTracks ? listTracks[n]  : 0;}
  Float_t getTrackEnergy(Short_t n) const  {return n>=0&&n<nTracks ? trackEnergy[n] : 0.F;}
  Short_t getTotMult(void)          const  {return totMult;} 
  Int_t   getTimeTrack(void)        const  {return timeTrack;}
  Short_t getStatus(void)           const  {return statusTime;}
  Float_t getSigmaEnergy(void)      const  {return sigmaEnergy;}
  Float_t getSigmaTime(void)        const  {return sigmaTime;}

  ClassDef(HEmcCalSim,1)
};



#endif  /* HEMCCALSIM_H */
