#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H

#include "TObject.h"

class HFwDetStrawCalSim : public TObject
{
private:
    Int_t   fTrack;     // geant track contributing to the hit
    Char_t  fModule;    // module number (0 and 2 for Straw modules)
    Char_t  fGeantCell; // geant cell number
    Int_t   fStraw;     // straw number
    Float_t fTime;      // straw time
    Float_t fEloss;     // straw energy loss
    Float_t fDriftRad;  // drifta radius
    Float_t fX;         // X-coordinate in respect to straw-0
    Float_t fZ;         // Z-coordinate of fired straw
    Int_t   fNStraw;    // TODO what is this?

public:
    HFwDetStrawCalSim();
    virtual ~HFwDetStrawCalSim();

    Int_t getTrack() const { return fTrack; }
    void  getAddress(Char_t& m, Char_t& c, Int_t& s);
    void  getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& StrawN) const;

    void  setTrack(Int_t num) { fTrack = num; }
    void  setAddress(Char_t m, Char_t c, Int_t s);

    void  setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t StrawN);

    ClassDef(HFwDetStrawCalSim, 1);
};

inline void HFwDetStrawCalSim::getAddress(Char_t& m, Char_t& c, Int_t& s)
{
    m = fModule;
    c = fGeantCell;
    s = fStraw;
}

inline void HFwDetStrawCalSim::setAddress(Char_t m, Char_t c, Int_t s)
{
    fModule    = m;
    fGeantCell = c;
    fStraw     = s;
}

inline void HFwDetStrawCalSim::getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& StrawN) const
{
    time = fTime;
    elos = fEloss;
    radi = fDriftRad;
    X = fX;
    Z = fZ;
    StrawN = fStraw;
}

inline void HFwDetStrawCalSim::setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t StrawN)
{
    fTime = time;
    fEloss = elos;
    fDriftRad = radi;
    fX = X;
    fZ = Z;
    fNStraw = StrawN;
}

#endif /* ! HFWDETSTRAWCALSIM_H */
