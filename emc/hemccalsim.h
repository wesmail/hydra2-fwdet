#ifndef HEMCCALSIM_H
#define HEMCCALSIM_H

#include "hemccal.h"

class HEmcCalSim : public HEmcCal {
protected:
  Short_t nTracks;       // number of tracks in list
  Int_t   listTracks[5]; // list of tracknumbers (GEANT) for tracks
  Short_t totMult;       // total number of tracks hitting this crystal

public:
  HEmcCalSim();
  ~HEmcCalSim();
  
  Short_t getNTracks() const {return nTracks;}
  inline Int_t getTrack(Short_t n);
  void setTrack(const Int_t trackNumber);
  Short_t getTotMult() const {return totMult;}
  void setTotMult(const Int_t n) {totMult=n;}   

  ClassDef(HEmcCalSim,1)
};

inline Int_t HEmcCalSim::getTrack(Short_t n) {
  if (n>=0 && n<nTracks) return listTracks[n];
  else return -1;
}

#endif  /* HEMCCALSIM_H */
