#ifndef HFWDETSCINCAL_H
#define HFWDETSCINCAL_H

#include "TObject.h"

class HFwDetScinCal : public TObject
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
    HFwDetScinCal();
    virtual ~HFwDetScinCal();

    Int_t getTrack(void) const { return fTrack; }
    inline void  getAddress(Char_t& m, Char_t& c, Char_t& s) const;
    inline void  getHit(Float_t& time, Float_t& elos) const;

    void  setTrack(Int_t num) { fTrack = num; }
    inline void  setAddress(const Char_t m, const Char_t c, const Char_t s);
    inline void  setHit(const Float_t time, const Float_t elos);

    ClassDef(HFwDetScinCal, 1);
};

void HFwDetScinCal::getAddress(Char_t& m, Char_t& c, Char_t& s) const
{
    m = fModule;
    c = fGeantCell;
    s = fScin;
}

void HFwDetScinCal::setAddress(const Char_t m, const Char_t c, const Char_t s)
{
    fModule    = m;
    fGeantCell = c;
    fScin      = s;
}

void HFwDetScinCal::getHit(Float_t& time, Float_t& elos) const
{
    time = fTime;
    elos = fElos;
}

void HFwDetScinCal::setHit(const Float_t time, const Float_t elos)
{
    fTime = time;
    fElos = elos;
}

#endif /* ! HFWDETSCINCAL_H */
