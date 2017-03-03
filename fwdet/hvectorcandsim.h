/**  HVectorCandSim.h
 *@author A.Zinchenko <alexander.zinchenko@jinr.ru>
 *@since 2016
 **
 ** Segments / tracks in the Forward Straw tracker.
 **/

#ifndef HVECTORCANDSIM_H
#define HVECTORCANDSIM_H

#include "Rtypes.h"

#include "hvectorcand.h"
#include "haddef.h"
#include "fwdetdef.h"

#include "TBits.h"
#include "TLorentzVector.h"
#include "TMatrixDSym.h"
#include "TObject.h"

class HVectorCandSim: public HVectorCand
{
public:
    HVectorCandSim();
    virtual ~HVectorCandSim();

    inline Int_t getTrack() const { return fTrack; }
    inline Int_t getRpcTrack() const { return fRpcTrack; }

    inline Double_t getPx1() const { return fPx1; }
    inline Double_t getPy1() const { return fPy1; }
    inline Double_t getPz1() const { return fPz1; }
    inline Double_t getPx2() const { return fPx2; }
    inline Double_t getPy2() const { return fPy2; }
    inline Double_t getPz2() const { return fPz2; }
    inline Double_t getX1() const { return fX1; }
    inline Double_t getY1() const { return fY1; }
    inline Double_t getZ1() const { return fZ1; }
    inline Double_t getX2() const { return fX2; }
    inline Double_t getY2() const { return fY2; }
    inline Double_t getZ2() const { return fZ2; }

    inline void setTrack(Int_t track) { fTrack = track; }
    inline void setRpcTrack(Int_t track) { fRpcTrack = track; }

    inline void setPx1(Double_t px) { fPx1 = px; }
    inline void setPy1(Double_t py) { fPy1 = py; }
    inline void setPz1(Double_t pz) { fPz1 = pz; }
    inline void setPx2(Double_t px) { fPx2 = px; }
    inline void setPy2(Double_t py) { fPy2 = py; }
    inline void setPz2(Double_t pz) { fPz2 = pz; }
    inline void setX1(Double_t x) { fX1 = x; }
    inline void setY1(Double_t y) { fY1 = y; }
    inline void setZ1(Double_t z) { fZ1 = z; }
    inline void setX2(Double_t x) { fX2 = x; }
    inline void setY2(Double_t y) { fY2 = y; }
    inline void setZ2(Double_t z) { fZ2 = z; }

    inline Float_t getSimTx() const;
    inline Float_t getSimTy() const;

    void print() const;

private:
    Int_t fTrack;       // track number
    Int_t fRpcTrack;    // the same but for RPC

    Double_t fPx1;      // momentum of the hit in the first straw
    Double_t fPy1;
    Double_t fPz1;

    Double_t fPx2;      // momentum of the hit in the last straw
    Double_t fPy2;
    Double_t fPz2;

    Double_t fX1;       // position of the hit in the first straw
    Double_t fY1;
    Double_t fZ1;

    Double_t fX2;       // position of the hit in the last straw
    Double_t fY2;
    Double_t fZ2;

    ClassDef(HVectorCandSim, 1);
};

inline Float_t HVectorCandSim::getSimTx() const
{
    Float_t dz = fZ2 - fZ1;
    Float_t dx = fX2 - fX1;
    return dx/dz;
}

inline Float_t HVectorCandSim::getSimTy() const
{
    Float_t dz = fZ2 - fZ1;
    Float_t dy = fY2 - fY1;
    return dy/dz;
}

#endif
