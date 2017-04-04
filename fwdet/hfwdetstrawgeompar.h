#ifndef HFWDETSTRAWGEOMPAR_H
#define HFWDETSTRAWGEOMPAR_H

#include "fwdetdef.h"
#include "hparcond.h"
#include "hgeomtransform.h"

#include "TObjArray.h"
#include "TArrayF.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TMatrixF.h"

class HParamList;

class HFwDetStrawGeomPar :  public HParCond
{
private:
    Int_t        nModules;       // number of modules (stations)
    struct SingleModule
    {
        Int_t    nLayers;        // number of layers per module
        Int_t    nStraws;        // number of straws in each block of a panel

        TArrayF  fStrawRadius;   // [layer:sublayer]
        TArrayF  fStrawPitch;    // [layer:sublayer]

        TMatrixF fOffsetZ;       // [layer:sublayer] offset of the plane center
                                 // from the layer center in local z-direction
        TMatrixF fOffsetX;       // [layer:sublayer] offset of the plane center
                                 // from the layer center in local x-direction

        TArrayF fLayerRotation;  // rotation of the layer (planes are binded)
    } sm_mods[FWDET_STRAW_MAX_MODULES]; //!

public:
    HFwDetStrawGeomPar(const Char_t* name = "FwDetStrawGeomPar",
                        const Char_t* title = "Geometry parameters of the FwDet Straws",
                     const Char_t* context = "FwDetStrawGeometry");
    virtual ~HFwDetStrawGeomPar();

    void   clear();
    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);

    Int_t getModules() const;
    Int_t getLayers(Int_t m) const;
    Int_t getStraws(Int_t m) const;
    Float_t getStrawRadius(Int_t m, Int_t l) const;
    Float_t getStrawPitch(Int_t m, Int_t l) const;
    Float_t getOffsetZ(Int_t m, Int_t l, Int_t p) const;
    Float_t getOffsetX(Int_t m, Int_t l, Int_t p) const;
    Float_t getLayerRotation(Int_t m, Int_t l) const;

    void setModules(Int_t m);
    void setLayers(Int_t m, Int_t l);
    void setStraws(Int_t m, Int_t s);
    void setStrawRadius(Int_t m, Int_t l, Float_t r);
    void setStrawPitch(Int_t m, Int_t l, Float_t p);
    void setOffsetZ(Int_t m, Int_t l, Int_t p, Float_t z);
    void setOffsetX(Int_t m, Int_t l, Int_t p, Float_t x);
    void setLayerRotation(Int_t m, Int_t l, Float_t r);

    ClassDef(HFwDetStrawGeomPar, 1); // Geometry of a FwDet Straw
}; 

#endif // HFWDETSTRAWGEOMPAR_H
