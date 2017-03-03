#ifndef HFWDETSTRAWCAL_H
#define HFWDETSTRAWCAL_H

#include "TObject.h"

class HFwDetStrawCal : public TObject
{
private:
    Char_t  fModule;    // module number (0 and 1 for Straw modules)
    Char_t  fLayer;     // layer number (0 - 3)
    Char_t  fPlane;     // plane number inside layer (0 or 1)
    Int_t   fCell;      // cell number in fLayer (straw tube number)
    Float_t fTime;      // hit detection time (tof + drift_time - start_offset)
    Float_t fADC;       // straw energy loss
    Float_t fX;         // X-coordinate in the lab system
    Float_t fZ;         // Z-coordinate in the lab system
    Int_t   fStraw;     // straw number in the plane

public:
    HFwDetStrawCal();
    virtual ~HFwDetStrawCal();

    void  getAddress(Char_t& m, Char_t& l, Char_t& p, Int_t& c) const;
    void  getHit(Float_t& time, Float_t& adc, Float_t& x, Float_t& z, Int_t& s) const;
    Int_t getStation() const { return fModule; }
    Int_t getLayer() const { return fLayer; }
    Int_t getPlane() const { return fPlane; }
    Int_t getTube() const { return fCell; }
    Double_t getX() const { return fX; }

    void  setAddress(Char_t m, Char_t l, Char_t p, Int_t c);
    void  setHit(Float_t time, Float_t adc, Float_t x, Float_t z, Int_t s);

    Int_t getVPlane() const { return getVPlane(fModule, fLayer, fPlane); }
    static Int_t getVPlane(Int_t m, Int_t l, Int_t p);

    ClassDef(HFwDetStrawCal, 1);
};

inline void HFwDetStrawCal::getAddress(Char_t &m, Char_t &l, Char_t &p, Int_t &c) const
{
    m = fModule;
    l = fLayer;
    p = fPlane;
    c = fCell;
}

inline void HFwDetStrawCal::setAddress(Char_t m, Char_t l, Char_t p, Int_t c)
{
    fModule = m;
    fLayer  = l;
    fPlane  = p;
    fCell   = c;
}

inline void HFwDetStrawCal::getHit(Float_t& time, Float_t& adc, Float_t& x, Float_t& z, Int_t& s) const
{
    time = fTime;
    adc  = fADC;
    x    = fX;
    z    = fZ;
    s    = fStraw;
}

inline void HFwDetStrawCal::setHit(Float_t time, Float_t adc, Float_t x, Float_t z, Int_t s)
{
    fTime  = time;
    fADC   = adc;
    fX     = x;
    fZ     = z;
    fStraw = s;
}

#endif /* ! HFWDETSTRAWCAL_H */
