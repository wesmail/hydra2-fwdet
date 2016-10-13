#ifndef HFWDETRPCCAL_H
#define HFWDETRPCCAL_H

#include "TObject.h"

class HFwDetRpcCal : public TObject
{
private:
    Int_t  fTrack;     // geant track contributing to the hit
    Char_t fModule;    // module number (0..1 for Rpc modules)
    Char_t fGeantCell; // geant cell number
    Char_t fRpc;       // rpc cell number
    Float_t fTime;     //
    Float_t fElos;     //

public:
    HFwDetRpcCal();
    ~HFwDetRpcCal();

    Int_t getTrack(void) const { return fTrack; }
    void  getAddress(Char_t& m, Char_t& c, Char_t& s) const;
    void  getHit(Float_t& time, Float_t& elos) const;

    void  setTrack(Int_t num) { fTrack = num; }
    void  setAddress(const Char_t m, const Char_t c, const Char_t s);
    void  setHit(const Float_t time, const Float_t elos);

    ClassDef(HFwDetRpcCal, 1)
};

#endif /* !HFWDETRPCCAL_H */
