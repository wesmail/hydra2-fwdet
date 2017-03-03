#ifndef HFWDETRPCHIT_H
#define HFWDETRPCHIT_H

#include "hades.h"
#include "fwdetdef.h"

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

    inline Float_t getX() const { return fX; }
    inline Float_t getY() const { return fY; }
    inline Float_t getZ() const { return fZ; }
    inline Float_t getTof() const { return fTof; }
    inline Float_t getP() const { return fP; }
    inline Int_t getTrack() const { return fTrack; }

    inline void setHit(Float_t x, Float_t y, Float_t z, Float_t tof);
    inline void getHit(Float_t & x, Float_t & y, Float_t & z, Float_t & tof) const;
    inline void setP(Float_t p) { fP = p; }
    inline void setTrack(Float_t t) { fTrack = t; }

    Float_t calcMomentum(Float_t mass);
    static Float_t calcMomentum(Float_t l, Float_t t, Float_t mass);

    void print() const;

    ClassDef(HFwDetRpcHit, 1)
};

inline void HFwDetRpcHit::setHit(Float_t x, Float_t y, Float_t z, Float_t tof)
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
