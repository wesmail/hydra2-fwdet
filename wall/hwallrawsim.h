#ifndef HWALLRAWSIM_H
#define HWALLRAWSIM_H

#include "hwallraw.h"

class HWallRawSim : public HWallRaw {
protected:
  Int_t nTrack1;   // number of Track for first hit in cell
  Int_t nTrack2;   // number of Track for second hit in cell
  //Int_t nTrack3;   // number of Track for third hit in cell
  //Int_t nTrack4;   // number of Track for fourth hit in cell

  Short_t  nHit; //number of hit //FK//

public:
  HWallRawSim(void) : nTrack1(-1), nTrack2(-1) {}
  //HWallRawSim(void) : nTrack1(-1), nTrack2(-1), nTrack3(-1), nTrack4(-1) {}
  ~HWallRawSim(void) {}
  void clear(void);
  inline void setNTrack1(const Int_t n) {nTrack1=n;}
  inline void setNTrack2(const Int_t n) {nTrack2=n;}
  //inline void setNTrack3(const Int_t n) {nTrack3=n;}
  //inline void setNTrack4(const Int_t n) {nTrack4=n;}
  void setNTrack(Int_t );
  inline Int_t getNTrack1(void) const {return nTrack1;}
  inline Int_t getNTrack2(void) const {return nTrack2;}
  //inline Int_t getNTrack3(void) const {return nTrack3;}
  //inline Int_t getNTrack4(void) const {return nTrack4;}

  //FK//
  inline Short_t getNHit(void);
  inline void setNHit(Short_t anHit);
  void incNHit(void) {nHit++;} 
  //FK//

  ClassDef(HWallRawSim,1) // simulated raw data level of Wall
};

//FK//

inline Short_t HWallRawSim::getNHit(void) {
  return  nHit;
}

//FK//
inline void HWallRawSim::setNHit(Short_t anHit){
   nHit=anHit;
}



#endif  /* HWALLRAWSIM_H */

