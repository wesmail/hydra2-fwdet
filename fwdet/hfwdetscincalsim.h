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
    //    Float_t fTime2;
    //    Float_t fElos2;

public:
    HFwDetScinCalSim();
    virtual ~HFwDetScinCalSim();

    Int_t getTrack(void) const { return fTrack; }
    void  getAddress(Char_t& m, Char_t& c, Char_t& s) const;
    void  getHit(Float_t& time, Float_t& elos) const;

    void  setTrack(Int_t num) { fTrack = num; }
    void  setAddress(const Char_t m, const Char_t c, const Char_t s);
    void  setHit(const Float_t time, const Float_t elos);

    ClassDef(HFwDetScinCalSim, 1);
};

inline void HFwDetScinCalSim::getAddress(Char_t& m, Char_t& c, Char_t& s) const
{
    m = fModule;
    c = fGeantCell;
    s = fScin;
}

inline void HFwDetScinCalSim::setAddress(const Char_t m, const Char_t c, const Char_t s)
{
    fModule    = m;
    fGeantCell = c;
    fScin      = s;
}

inline void HFwDetScinCalSim::getHit(Float_t& time, Float_t& elos) const
{
    time = fTime;
    elos = fElos;
}

inline void HFwDetScinCalSim::setHit(const Float_t time, const Float_t elos)
{
    fTime = time;
    fElos = elos;
}

#endif /* ! HFWDETSCINCALSIM_H */
