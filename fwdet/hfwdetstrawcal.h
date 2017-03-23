#ifndef HFWDETSTRAWCAL_H
#define HFWDETSTRAWCAL_H

#include "TObject.h"

class HFwDetStrawCal : public TObject
{
private:
    Char_t  fModule;    // module number (0 and 1 for Straw modules)
    Char_t  fLayer;     // layer number (0 - 3)
    Char_t  fPlane;     // plane number inside layer (0 or 1)
    Int_t   fStraw;     // straw number in the plane
    Char_t  fUpDown;    // up-down straw
    Float_t fTime;      // hit detection time (tof + drift_time - start_offset)
    Float_t fADC;       // straw energy loss
    Float_t fU;         // U-coordinate in the lab system
    Float_t fZ;         // Z-coordinate in the lab system

public:
    HFwDetStrawCal();
    virtual ~HFwDetStrawCal();

    inline void  getAddress(Char_t& m, Char_t& l, Char_t& p, Int_t& s, Char_t& ud) const;
    inline void  getHit(Float_t& time, Float_t& adc, Float_t& x, Float_t& z) const;
    Int_t getStation() const { return fModule; }
    Int_t getLayer() const { return fLayer; }
    Int_t getPlane() const { return fPlane; }
    Int_t getStraw() const { return fStraw; }
    Float_t getTime() const { return fTime; }
    Float_t getU() const { return fU; }
    Int_t getUDconf() const { return fUpDown; }

    inline void  setAddress(Char_t m, Char_t l, Char_t p, Int_t s, Char_t ud);
    inline void  setHit(Float_t time, Float_t adc, Float_t x, Float_t z);

    Int_t getVPlane() const { return getVPlane(fModule, fLayer, fPlane); }
    static Int_t getVPlane(Int_t m, Int_t l, Int_t p);

    ClassDef(HFwDetStrawCal, 1);
};

void HFwDetStrawCal::getAddress(Char_t &m, Char_t &l, Char_t &p, Int_t &s, Char_t& ud) const
{
    m  = fModule;
    l  = fLayer;
    p  = fPlane;
    s  = fStraw;
    ud = fUpDown;
}

void HFwDetStrawCal::setAddress(Char_t m, Char_t l, Char_t p, Int_t s, Char_t ud)
{
    fModule = m;
    fLayer  = l;
    fPlane  = p;
    fStraw  = s;
    fUpDown = ud;
}

void HFwDetStrawCal::getHit(Float_t& time, Float_t& adc, Float_t& u, Float_t& z) const
{
    time = fTime;
    adc  = fADC;
    u    = fU;
    z    = fZ;
}

void HFwDetStrawCal::setHit(Float_t time, Float_t adc, Float_t u, Float_t z)
{
    fTime  = time;
    fADC   = adc;
    fU     = u;
    fZ     = z;
}

#endif /* ! HFWDETSTRAWCAL_H */
