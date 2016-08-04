#ifndef HShowerHitTof_h
#define HShowerHitTof_h

#pragma interface

#include "hshowerhit.h"

class HShowerHitTof : public HShowerHit{
protected:
  Float_t m_fTof;       // calibrated tof from Tofino in ns
  Float_t m_fDriftTime; // time of light propagation in scintillator in ns
  Float_t m_fADC;       // energy  
  Int_t m_nTofinoCell;  // cell of Tofino which is used 
  Int_t m_nTofinoMult;  // multiplicity in Tofino cell

public:
  HShowerHitTof(void);
  ~HShowerHitTof(void) {}

  void setTof(Float_t tof) {m_fTof = tof;} //time of flight
  void setDriftTime(Float_t time) {m_fDriftTime = time;} //drift time
  void setADC(Float_t adc) {m_fADC=adc;} //ADC
  void setTofinoCell(Int_t tcell) {m_nTofinoCell=tcell;} //tofino cell
  void setTofinoMult(Int_t mult) {m_nTofinoMult=mult;} //multiplicity

  Float_t getTof() {return m_fTof;} //time of flight
  Float_t getDriftTime() {return m_fDriftTime;} //drift time
  Float_t getADC(void) const {return m_fADC;} //ADC
  Int_t getTofinoCell(void) const {return m_nTofinoCell;} //tofino cell
  Int_t getTofinoMult(void) const {return m_nTofinoMult;} //multiplicity

  HShowerHitTof& operator=(HShowerHitTof& ht);
  HShowerHitTof& operator=(HShowerHit& ht);

  ClassDef(HShowerHitTof,1) // Tofino/Shower calibrated data
};


#endif /* !HShowerHitTof_h */

