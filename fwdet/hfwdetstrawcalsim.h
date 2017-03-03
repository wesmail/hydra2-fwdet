#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H

#include "hfwdetstrawcal.h"

class HFwDetStrawCalSim : public HFwDetStrawCal
{
private:
    Int_t   fTrack;         // geant track contributing to the hit
    Float_t fToF;           // ToF to the straw
    Float_t fDriftRad;      // drifta radius
    Float_t fEloss;         // Eloss in the straw
    Float_t px;
    Float_t py;
    Float_t pz;

public:
    HFwDetStrawCalSim();
    virtual ~HFwDetStrawCalSim();

    inline Int_t getTrack() const { return fTrack; }
    Double_t getToF() const { return fToF; }
    Double_t getDrift() const { return fDriftRad; }
    Double_t getEloss() const { return fEloss; }

    inline void  setTrack(Int_t num) { fTrack = num; }
    void  setToF(Double_t tof) { fToF = tof; }
    void  setDrift(Double_t drift) { fDriftRad = drift; }
    void  setEloss(Double_t eloss) { fEloss = eloss; }

    Float_t getP();
    void getP(Float_t & x, Float_t & y, Float_t & z) const;
    void setP(Float_t x, Float_t y, Float_t z);

    ClassDef(HFwDetStrawCalSim, 1);
};

#endif /* ! HFWDETSTRAWCALSIM_H */
