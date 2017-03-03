#ifndef HFWCOMPGEOMPAR_H
#define HFWCOMPGEOMPAR_H

#include "hdetgeompar.h"
#include "TObjArray.h"

class HGeomShapes;

class HFwDetGeomPar : public HDetGeomPar
{
public:
    HFwDetGeomPar(const Char_t* name = "FwDetGeomPar",
                 const Char_t* title = "FwDet geometry parameters",
                 const Char_t* context = "GeomProduction");
    virtual ~HFwDetGeomPar();

    Bool_t init(HParIo*, Int_t*);
    Int_t write(HParIo*);
    inline Int_t getModNumInMod(const TString&);
    inline Int_t getModNumInComp(const TString&);
    inline Int_t getCompNum(const TString&);
    ClassDef(HFwDetGeomPar, 1); // Geometry parameter container for the Forward Detector
};

Int_t HFwDetGeomPar::getModNumInMod(const TString& name)
{
    // returns the module index retrieved from the module name ADxM
    return (Int_t)(name[2]-'0') - 1;
}

Int_t HFwDetGeomPar::getModNumInComp(const TString& name)
{
    // returns the module index retrieved from the element name Ax
    return (Int_t)(name[1]-'0') - 1;
}

Int_t HFwDetGeomPar::getCompNum(const TString& name)
{
    // returns the cell index retrieved from the component name AxSVy
    return (Int_t)(name[4]-'0') - 1;
}

#endif /* !HFWCOMPGEOMPAR_H */
