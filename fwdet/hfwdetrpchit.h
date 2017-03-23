#ifndef HFWDETRPCHIT_H
#define HFWDETRPCHIT_H

#include "TMath.h"
#include "TObject.h"

class HFwDetRpcHit : public TObject
{
private:
    Float_t fX;     // pos coordinates
    Float_t fY;
    Float_t fZ;
    Float_t fTof;   // time of flight
    Float_t fP;
    Int_t fTrack;

public:
    HFwDetRpcHit();
    ~HFwDetRpcHit();

    Float_t getX() const { return fX; }
    Float_t getY() const { return fY; }
    Float_t getZ() const { return fZ; }
    Float_t getTof() const { return fTof; }
    Float_t getP() const { return fP; }
    Int_t getTrack() const { return fTrack; }
    Float_t getDistance() const { return TMath::Sqrt(fX*fX + fY*fY + fZ*fZ); }

    inline void setHit(Float_t x, Float_t y, Float_t z, Float_t tof);
    inline void getHit(Float_t & x, Float_t & y, Float_t & z, Float_t & tof) const;
    void setP(Float_t p) { fP = p; }
    void setTrack(Float_t t) { fTrack = t; }

    Float_t calcMomentum(Float_t mass);
    static Float_t calcMomentum(Float_t l, Float_t t, Float_t mass);

    void print() const;

    ClassDef(HFwDetRpcHit, 1)
};

void HFwDetRpcHit::setHit(Float_t x, Float_t y, Float_t z, Float_t tof)
{
    fX = x;
    fY = y;
    fZ = z;
    fTof = tof;
}

void HFwDetRpcHit::getHit(Float_t& x, Float_t& y, Float_t& z, Float_t& tof) const
{
    x = fX;
    y = fY;
    z = fZ;
    tof = fTof;
}

#endif /* !HFWDETRPCHIT_H */
