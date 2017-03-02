#ifndef HFWDETSTRAWCAL_H
#define HFWDETSTRAWCAL_H

#include "TObject.h"
#include <map>

class HFwDetStrawCal : public TObject
{
private:
    Char_t  fModule;        // module number (0 and 1 for Straw modules)
    Char_t  fLayer;         // layer number (0 - 3)
    Char_t  fPlane;         // plane number inside layer (0 or 1)
    Int_t   fCell;          // cell number in fLayer (straw tube number)
    Float_t fTime;          // straw time
    Float_t fEloss;         // straw energy loss
    Float_t fDriftRad;      // drifta radius
    Float_t fX;             // X-coordinate in the lab system
    Float_t fZ;             // Z-coordinate in the lab system
    Int_t   fStraw;         // straw number in the plane
    std::multimap<Double32_t,Int_t> fDriftId; // - drift radius - track ID map

public:
    HFwDetStrawCal();
    virtual ~HFwDetStrawCal();

    void  getAddress(Char_t& m, Char_t& l, Char_t& p, Int_t& c);
    void  getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& s) const;
    Double_t getDrift() const { return fDriftRad; }
    Int_t getStation() const { return fModule; }
    Int_t getLayer() const { return fLayer; }
    Int_t getPlane() const { return fPlane; }
    Int_t getTube() const { return fCell; }
    Int_t getVPlane() const { return fModule * 8 + fLayer * 2 + fPlane; }
    Double_t getX() const { return fX; }

    void  setAddress(Char_t m, Char_t l, Char_t p, Int_t c);
    void  setDrift(Double_t drift) { fDriftRad = drift; }

    void  setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t s);

    ClassDef(HFwDetStrawCal, 1);
};

inline void HFwDetStrawCal::getAddress(Char_t &m, Char_t &l, Char_t &p, Int_t &c)
{
    m   = fModule;
    l   = fLayer;
    p   = fPlane;
    c   = fCell;
}

inline void HFwDetStrawCal::setAddress(Char_t m, Char_t l, Char_t p, Int_t c)
{
    fModule     = m;
    fLayer      = l;
    fPlane      = p;
    fCell       = c;
}

inline void HFwDetStrawCal::getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& s) const
{
    time = fTime;
    elos = fEloss;
    radi = fDriftRad;
    X = fX;
    Z = fZ;
    s = fStraw; //fStraw;
}

inline void HFwDetStrawCal::setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t s)
{
    fTime = time;
    fEloss = elos;
    fDriftRad = radi;
    fX = X;
    fZ = Z;
    fStraw = s; //StrawN;
}

#endif /* ! HFWDETSTRAWCAL_H */
