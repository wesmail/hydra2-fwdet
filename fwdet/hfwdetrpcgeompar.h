#ifndef HFWDETRPCGEOMPAR_H
#define HFWDETRPCGEOMPAR_H

#include "hparcond.h"
#include "fwdetdef.h"

#include "TObjArray.h"
#include "TArrayF.h"
#include "TArrayD.h"
#include "TArrayI.h"
#include "TMatrixF.h"

class HParamList;

class HFwDetRpcGeomPar :  public HParCond
{
private:
    Int_t        nModules;       // number of modules (stations)
    struct SingleModule
    {
        Int_t    nLayers;        // numer of layers

        Float_t  fModuleZ;       // z coordinate of the module
        Float_t  fModulePhi;     // rotation of the module

        TArrayF  fLayerY;        // y offset of each layer in the module
    } sm_mods[FWDET_RPC_MAX_MODULES]; //!

public:
    HFwDetRpcGeomPar(const Char_t* name = "FwDetStrawGeomPar",
                        const Char_t* title = "Geometry parameters of the FwDet Straws",
                     const Char_t* context = "FwDetStrawGeometry");
    virtual ~HFwDetRpcGeomPar();

    void   printParam() const;
    void   clear();
    void   putParams(HParamList*);
    Bool_t getParams(HParamList*);

    Int_t getModules() const;
    Int_t getLayers(Int_t m) const;
    Float_t getModuleZ(Int_t m) const;
    Float_t getModulePhi(Int_t m) const;
    Float_t getLayerY(Int_t m, Int_t l) const;

    void setModules(Int_t m);
    void setLayers(Int_t m, Int_t l);
    void setModuleZ(Int_t m, Float_t z);
    void setModulePhi(Int_t m, Float_t r);
    void setLayerY(Int_t m, Int_t l, Float_t o);

    ClassDef(HFwDetRpcGeomPar, 1); // Geometry of a FwDet Straw
}; 

#endif // HFWDETRPCGEOMPAR_H
