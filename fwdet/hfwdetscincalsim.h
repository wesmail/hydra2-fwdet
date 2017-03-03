#ifndef HFWDETSCINCALSIM_H
#define HFWDETSCINCALSIM_H

#include "hfwdetscincal.h"

#include "TObject.h"

class HFwDetScinCalSim : public HFwDetScinCal
{
private:
    Int_t  fTrack;     // geant track contributing to the hit
    Char_t fModule;    // module number (0..1 for Scin modules)
    Char_t fGeantCell; // geant cell number
    Char_t fScin;      // scintillator cell number
    Float_t fTime;     //
    Float_t fElos;     //

public:
    HFwDetScinCalSim();
    virtual ~HFwDetScinCalSim();

    Int_t getTrack(void) const { return fTrack; }
    inline void  getAddress(Char_t& m, Char_t& c, Char_t& s) const;
    inline void  getHit(Float_t& time, Float_t& elos) const;

    void  setTrack(Int_t num) { fTrack = num; }
    inline void  setAddress(const Char_t m, const Char_t c, const Char_t s);
    inline void  setHit(const Float_t time, const Float_t elos);

    ClassDef(HFwDetScinCalSim, 1);
};

void HFwDetScinCalSim::getAddress(Char_t& m, Char_t& c, Char_t& s) const
{
    m = fModule;
    c = fGeantCell;
    s = fScin;
}

void HFwDetScinCalSim::setAddress(const Char_t m, const Char_t c, const Char_t s)
{
    fModule    = m;
    fGeantCell = c;
    fScin      = s;
}

void HFwDetScinCalSim::getHit(Float_t& time, Float_t& elos) const
{
    time = fTime;
    elos = fElos;
}

void HFwDetScinCalSim::setHit(const Float_t time, const Float_t elos)
{
    fTime = time;
    fElos = elos;
}

#endif /* ! HFWDETSCINCALSIM_H */
