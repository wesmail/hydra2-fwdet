#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H


#include "TObject.h"

class HFwDetStrawCalSim : public TObject {

private:
    Int_t  fTrack;     // geant track contributing to the hit
    Char_t fModule;    // module number (0  and 1 for Straw modules)
    Char_t fGeantCell; // geant cell number
    Char_t fStraw;     // straw number
    Float_t fTime;     // 
    Float_t fElos;     //
    //    Float_t fTime2;
    //    Float_t fElos2;

public:
    HFwDetStrawCalSim(void);
    ~HFwDetStrawCalSim(void) {}

    Int_t getTrack(void) {return fTrack;}
    void  getAddress(Char_t& m, Char_t& c, Char_t& s);
    void  getHit(Float_t& time, Float_t& elos);

    void  setTrack(Int_t num) {fTrack = num;}
    void  setAddress(const Char_t m, const Char_t c, const Char_t s);
    void  setHit(const Float_t time, const Float_t elos);

    ClassDef(HFwDetStrawCalSim,1)

};

inline void HFwDetStrawCalSim::getAddress(Char_t& m, Char_t& c, Char_t& s) {
    m = fModule;
    c = fGeantCell;
    s = fStraw;
}

inline void HFwDetStrawCalSim::setAddress(const Char_t m, const Char_t c, const Char_t s) {
    fModule    = m;
    fGeantCell = c;
    fStraw     = s;
}

inline void HFwDetStrawCalSim::getHit(Float_t& time, Float_t& elos) {
    time = fTime;
    elos = fElos;
}

inline void HFwDetStrawCalSim::setHit(const Float_t time, const Float_t elos) {
    fTime = time;
    fElos = elos;
}

#endif /* ! HFWDETSTRAWCALSIM_H */
