#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H

#include "hfwdetstrawcal.h"

#include "TMath.h"

class HFwDetStrawCalSim : public HFwDetStrawCal
{
private:
    Int_t   fTrack;         // geant track contributing to the hit
    Float_t fToF;           // ToF to the straw
    Float_t fDriftRadius;   // drifta radius
    Float_t fEloss;         // Eloss in the straw
    Float_t fPx;
    Float_t fPy;
    Float_t fPz;
    Float_t fXhit;
    Float_t fYhit;
    Float_t fZhit;
    Int_t   fHitNumber;

public:
    HFwDetStrawCalSim();
    virtual ~HFwDetStrawCalSim();

    Int_t getTrack() const { return fTrack; }
    Double_t getToF() const { return fToF; }
    Double_t getDriftRadius() const { return fDriftRadius; }
    Double_t getEloss() const { return fEloss; }
    Int_t getHitNumber() const { return fHitNumber; }

    void setTrack(Int_t num) { fTrack = num; }
    void setToF(Double_t tof) { fToF = tof; }
    void setDriftRadius(Double_t drift) { fDriftRadius = drift; }
    void setEloss(Double_t eloss) { fEloss = eloss; }
    void setHitNumber(Int_t num) { fHitNumber = num; }

    inline Float_t getP() const;
    inline Float_t getPx() const { return fPx; }
    inline Float_t getPy() const { return fPy; }
    inline Float_t getPz() const { return fPz; }
    inline void getP(Float_t & x, Float_t & y, Float_t & z) const;
    inline void setP(Float_t x, Float_t y, Float_t z);

    inline void getHitPos(Float_t & x, Float_t & y, Float_t & z) const;
    inline void setHitPos(Float_t x, Float_t y, Float_t z);

    ClassDef(HFwDetStrawCalSim, 1);
};

Float_t HFwDetStrawCalSim::getP() const
{
    return TMath::Sqrt(fPx*fPx + fPy*fPy + fPz*fPz);
}

void HFwDetStrawCalSim::getP(Float_t & x, Float_t & y, Float_t & z) const
{
    x = fPx;
    y = fPy;
    z = fPz;
}

void HFwDetStrawCalSim::setP(Float_t x, Float_t y, Float_t z)
{
    fPx = x;
    fPy = y;
    fPz = z;
}

void HFwDetStrawCalSim::getHitPos(Float_t & x, Float_t & y, Float_t & z) const
{
    x = fXhit;
    y = fYhit;
    z = fZhit;
}

void HFwDetStrawCalSim::setHitPos(Float_t x, Float_t y, Float_t z)
{
    fXhit = x;
    fYhit = y;
    fZhit = z;
}

#endif /* ! HFWDETSTRAWCALSIM_H */
