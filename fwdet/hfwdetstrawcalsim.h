#ifndef HFWDETSTRAWCALSIM_H
#define HFWDETSTRAWCALSIM_H

#include "TObject.h"
#include <map>

class HFwDetStrawCalSim : public TObject
{
private:
    Int_t   fTrack;        // geant track contributing to the hit
    Char_t  fModule;       // module number (0 and 1 for Straw modules)
    Char_t  fDoubleLayer;  // double layer number (0 - 4 )
    Char_t  fLayer;        // layer number inside  double layer (0 or 1)
    Int_t   fCell;         // cell number in fLayer (straw tube number)
//     Char_t  fGeantCell; // geant cell number
//     Int_t   fStraw;     // straw number
    Float_t fTime;      // straw time
    Float_t fEloss;     // straw energy loss
    Float_t fDriftRad;  // drifta radius
    Float_t fX;         // X-coordinate in respect to straw-0 !  X position of wire in the coor.sys.of doubleLayer
    Float_t fZ;         // Z-coordinate of fired straw        !  Z position of wire in the coor.sys.of doubleLayer
    Int_t   fNStraw;    // TODO what is this?
    std::multimap<Double32_t,Int_t> fDriftId; //AZ - drift radius - track ID map

public:
    HFwDetStrawCalSim();
    virtual ~HFwDetStrawCalSim();

    Int_t getTrack() const { return fTrack; }
    void  getAddress(Char_t& m, Char_t& dl, Char_t& l, Int_t& c);
    void  getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& StrawN) const;
    Double_t getDrift() const { return fDriftRad / 10; } //AZ - in cm
    Int_t getStation() const { return fModule; } //AZ
    Int_t getDoublet() const { return fDoubleLayer; } //AZ
    Int_t getLayer() const { return (fZ < 0) ? 0 : 1; } //AZ
    Int_t getTube() const { return fCell; } //AZ
    Int_t getPlane() const { return fModule * 8 + fDoubleLayer * 2 + fLayer; } //AZ
    Double_t getU() const { return fX / 10; } //AZ - in cm
    std::multimap<Double_t,Int_t>& getDriftId() { return fDriftId; } //AZ

    void  setTrack(Int_t num) { fTrack = num; }
    void  setAddress(Char_t m, Char_t dl, Char_t l, Int_t c);
    void  setDrift(Double_t drift) { fDriftRad = drift; } //AZ
    void  addTrack(Double_t drift, Int_t trId) { fDriftId.insert(std::pair<Double_t,Int_t>(drift, trId)); } //AZ

    void  setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t StrawN);

    ClassDef(HFwDetStrawCalSim, 1);
};

inline void HFwDetStrawCalSim::getAddress(Char_t &m, Char_t &dl, Char_t &l, Int_t &c)
{
        m  = fModule;
    dl = fDoubleLayer;
    l  = fLayer;
        c  = fCell;
}

inline void HFwDetStrawCalSim::setAddress(Char_t m, Char_t dl, Char_t l, Int_t c)
{
        fModule      = m;
    fDoubleLayer = dl;
    fLayer       = l;
        fCell        = c;
}

inline void HFwDetStrawCalSim::getHit(Float_t& time, Float_t& elos, Float_t& radi, Float_t& X, Float_t& Z, Int_t& StrawN) const
{
    time = fTime;
    elos = fEloss;
    radi = fDriftRad;
    X = fX;
    Z = fZ;
    StrawN = fCell; //fStraw;
}

inline void HFwDetStrawCalSim::setHit(Float_t time, Float_t elos, Float_t radi, Float_t X, Float_t Z, Int_t StrawN)
{
    fTime = time;
    fEloss = elos;
    fDriftRad = radi;
    fX = X;
    fZ = Z;
    fNStraw = fCell; //StrawN;
}

#endif /* ! HFWDETSTRAWCALSIM_H */
