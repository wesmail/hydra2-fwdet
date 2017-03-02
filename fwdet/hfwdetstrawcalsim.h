#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H

#include "hfwdetstrawcal.h"

#include "TObject.h"

#include <map>

class HFwDetStrawCalSim : public HFwDetStrawCal
{
private:
    Int_t   fTrack;         // geant track contributing to the hit

public:
    HFwDetStrawCalSim();
    virtual ~HFwDetStrawCalSim();

    inline Int_t getTrack() const { return fTrack; }
    inline void  setTrack(Int_t num) { fTrack = num; }

    ClassDef(HFwDetStrawCalSim, 1);
};

#endif /* ! HFWDETSTRAWCALSIM_H */
