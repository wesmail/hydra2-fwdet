#include "hfwdetdetector.h"
#include "hfwdetstrawgeompar.h"
#include "hparamlist.h"
#include "hades.h"
#include "hspectrometer.h"

#include <iostream>
#include <vector>

using namespace std;

ClassImp(HFwDetStrawGeomPar);

HFwDetStrawGeomPar::HFwDetStrawGeomPar(const Char_t* name, const Char_t* title,
                                       const Char_t* context) :
                                       HParCond(name, title, context), nModules(0)
{
    clear();
}

HFwDetStrawGeomPar::~HFwDetStrawGeomPar()
{
}

void HFwDetStrawGeomPar::clear()
{
    for (Int_t i = 0; i < FWDET_STRAW_MAX_MODULES; ++i)
    {
        // clear all variables
        sm_mods[i].nLayers = 0;
        sm_mods[i].nStraws = 0;
        sm_mods[i].fStrawRadius = 0.0;
        sm_mods[i].fStrawPitch = 0.0;
        sm_mods[i].fOffsetZ = 0.0;
        sm_mods[i].fOffsetX = 0.0;
        sm_mods[i].fLayerRotation = 0;
    }
}

Int_t HFwDetStrawGeomPar::getModules() const
{
    // return number of layers in single detector
    return nModules;
}

Int_t HFwDetStrawGeomPar::getLayers(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].nLayers;
    else
        return -1;
}

Int_t HFwDetStrawGeomPar::getStraws(Int_t m) const
{
    // return number of straws in single layer of module 'm'
    // l -- layer number
    if (m < getModules())
        return sm_mods[m].nStraws;//[_off_straws[l][d][b]];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getStrawRadius(Int_t m, Int_t l) const
{
    // return straw radius in single sublayer of layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fStrawRadius[l];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getStrawPitch(Int_t m, Int_t l) const
{
    // return straw pitch in single sublayer of layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fStrawPitch[l];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getOffsetZ(Int_t m, Int_t l, Int_t p) const
{
    // return Z-coordinate of the beginning of the sublayer 's' in layer 'l'
    // m --module number
    // l -- layer number
    // s -- sublayer number
    if (m < getModules() && l < getLayers(m) && p < FWDET_STRAW_MAX_PLANES)
        return sm_mods[m].fOffsetZ[l][p];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getOffsetX(Int_t m, Int_t l, Int_t p) const
{
    // return X-coordinate of the beginning of the sublayer 's' in layer 'l'
    // m -- module number
    // l -- layer number
    // s -- sublayer number
    if (m < getModules() && l < getLayers(m) && p < FWDET_STRAW_MAX_PLANES)
        return sm_mods[m].fOffsetX[l][p];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getLayerRotation(Int_t m, Int_t l) const
{
    // return transformation matrix for a layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fLayerRotation[l];
    else
        return -1;
}

void HFwDetStrawGeomPar::setModules(int m)
{
    if (m <= FWDET_STRAW_MAX_MODULES)
        nModules = m;
}

void HFwDetStrawGeomPar::setLayers(Int_t m, Int_t l)
{
    // set number of layers, this function automatically
    // resizes all depending arrays
    sm_mods[m].nLayers = l;
    sm_mods[m].fStrawRadius.Set(l);
    sm_mods[m].fStrawPitch.Set(l);
    sm_mods[m].fLayerRotation.Set(l);
    sm_mods[m].fOffsetZ.ResizeTo(l, FWDET_STRAW_MAX_PLANES);
    sm_mods[m].fOffsetX.ResizeTo(l, FWDET_STRAW_MAX_PLANES);
}

void HFwDetStrawGeomPar::setStraws(Int_t m, Int_t s)
{
    // set number of straws for moduke 'm'
    // m -- module number
    // s -- number of straws
    if (m < getModules())
    {
        sm_mods[m].nStraws = s;
    }
}

void HFwDetStrawGeomPar::setStrawRadius(Int_t m, Int_t l, Float_t r)
{
    // set straw radius in each sublayer of layer 'l'
    // l -- layer number
    // r -- straw radius
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fStrawRadius[l] = r;
    }
}

void HFwDetStrawGeomPar::setStrawPitch(Int_t m, Int_t l, Float_t p)
{
    // set straws pitch in each sublayer of layer 'l'
    // l -- layer number
    // p -- straws pitch
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fStrawPitch[l] = p;
    }
}

void HFwDetStrawGeomPar::setOffsetZ(Int_t m, Int_t l, Int_t p, Float_t z)
{
    // set Z-coordinate of the begining of 's' sublayer in layer 'l'
    // l -- layer number
    // s -- sublater number
    // z -- z-coordinate
    if (m < getModules() && l < getLayers(m) && p < FWDET_STRAW_MAX_PLANES)
    {
        sm_mods[m].fOffsetZ[l][p] = z;
    }
}

void HFwDetStrawGeomPar::setOffsetX(Int_t m, Int_t l, Int_t p, Float_t x)
{
    // set X-coordinate of the begining of 's' sublayer in layer 'l'
    // l -- layer number
    // s -- sublater number
    // x -- z-coordinate
    if (m < getModules() && l < getLayers(m) && p < FWDET_STRAW_MAX_PLANES)
    {
        sm_mods[m].fOffsetX[l][p] = x;
    }
}

void HFwDetStrawGeomPar::setLayerRotation(Int_t m, Int_t l, Float_t r)
{
    // set lab transform for later 'l'
    // l -- layer number
    // lt -- lab transformation matrix [3x3 rot., 3-x transl.]
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fLayerRotation[l] = r;
    }
}

void HFwDetStrawGeomPar::putParams(HParamList* l)
{
    // add the parameters to the list for writing
    if (!l) return;

    Int_t total_layers = 0;

    // first find total number of layers
    for (Int_t i = 0; i < nModules; ++i)
    {
        total_layers += getLayers(i);
    }

    TArrayI par_layers(nModules);
    TArrayI par_straws(nModules);

    TArrayF par_offsetX(total_layers * FWDET_STRAW_MAX_PLANES);
    TArrayF par_offsetZ(total_layers * FWDET_STRAW_MAX_PLANES);

    TArrayF par_strawR(total_layers);
    TArrayF par_strawP(total_layers);

    TArrayF par_layerRotation(total_layers);

    Int_t cnt_layers = 0;

    for (Int_t i = 0; i < nModules; ++i)
    {
        // get number of layers
        Int_t layers = getLayers(i);

        // set number of layers
        par_layers.SetAt(layers, i);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            par_strawR.SetAt(getStrawRadius(i, l), cnt_layers + l);
            par_strawP.SetAt(getStrawPitch(i,l), cnt_layers + l);

            for (Int_t s = 0; s < FWDET_STRAW_MAX_PLANES; ++s)
            {
                par_offsetZ.SetAt(getOffsetZ(i, l, s), 2*(cnt_layers+l) + s);
                par_offsetX.SetAt(getOffsetX(i, l, s), 2*(cnt_layers+l) + s);
            }

            par_layerRotation.SetAt(getLayerRotation(i, l), cnt_layers + l);
        }

        // set number of straws in each block
        par_straws.SetAt(getStraws(i), i);

        cnt_layers += layers;
    }

    l->add("nModules",       nModules);
    l->add("nLayers",        par_layers);
    l->add("nStraws",        par_straws);
    l->add("fOffsetZ",       par_offsetZ);
    l->add("fOffsetX",       par_offsetX);
    l->add("fStrawRadius",   par_strawR);
    l->add("fStrawPitch",    par_strawP);
    l->add("fLayerRotation", par_layerRotation);
}

Bool_t HFwDetStrawGeomPar::getParams(HParamList* l)
{
    // gets the parameters from the list (read from input)
    if (!l) return kFALSE;

    Int_t par_modules;
    if (!l->fill("nModules", &par_modules))
        return kFALSE;

    TArrayI par_layers(par_modules);
    if (!l->fill("nLayers", &par_layers))
        return kFALSE;

    if (par_layers.GetSize() != par_modules)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of layers does not fit to number of detectors");
        return kFALSE;
    }

    Int_t total_layers = 0;
    for (Int_t d = 0; d < par_modules; ++d)
    {
        total_layers += par_layers[d];
    }

    TArrayI par_straws;
    if (!l->fill("nStraws", &par_straws))
        return kFALSE;

    if (par_straws.GetSize() != par_modules)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of straws does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF par_strawRadius(total_layers);
    if (!l->fill("fStrawRadius", &par_strawRadius))
        return kFALSE;

    if (par_strawRadius.GetSize() != total_layers)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of strawRadius=%d does not fit to number of layers=%d", par_strawRadius.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayF par_strawPitch(total_layers);
    if (!l->fill("fStrawPitch", &par_strawPitch))
        return kFALSE;

    if (par_strawPitch.GetSize() != total_layers)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of strawPitch=%d does not fit to number of layers=%d", par_strawPitch.GetSize(), total_layers);
        return kFALSE;
    }

    Int_t cnt_layers = 0;
    for (Int_t d = 0; d < par_modules; ++d)
    {
        cnt_layers += par_layers[d];
    }
    const Int_t cnt_planes = cnt_layers * FWDET_STRAW_MAX_PLANES;

    TArrayF par_offsetZ(cnt_planes);
    if (!l->fill("fOffsetZ", &par_offsetZ))
        return kFALSE;

    if (par_offsetZ.GetSize() != cnt_planes)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of planeZ=%d does not fit to number of planes=%d", par_offsetZ.GetSize(), cnt_planes);
        return kFALSE;
    }

    TArrayF par_offsetX(cnt_planes);
    if (!l->fill("fOffsetX", &par_offsetX))
        return kFALSE;

    if (par_offsetX.GetSize() != cnt_planes)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of planeX=%d does not fit to number of planes=%d", par_offsetX.GetSize(), cnt_planes);
        return kFALSE;
    }

    TArrayF par_layerRotation(total_layers);
    if (!l->fill("fLayerRotation", &par_layerRotation))
        return kFALSE;

    if (par_layerRotation.GetSize() != (Int_t)(total_layers))
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of layerRotation=%d does not fit to number of layers=%d", par_layerRotation.GetSize(), total_layers);
        return kFALSE;
    }

    cnt_layers = 0;

    setModules(par_modules);

    for (Int_t i = 0; i < par_modules; ++i)
    {
        // get number of layers
        Int_t layers = par_layers[i];

        // set number of layers
        setLayers(i, layers);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            setStrawRadius(i, l, par_strawRadius[cnt_layers + l]);
            setStrawPitch(i, l, par_strawPitch[cnt_layers + l]);

            for (Int_t s = 0; s < FWDET_STRAW_MAX_PLANES; ++s)
            {
                setOffsetZ(i, l, s, par_offsetZ[2*(cnt_layers + l) + s]);
                setOffsetX(i, l, s, par_offsetX[2*(cnt_layers + l) + s]);
            }
            setLayerRotation(i, l, par_layerRotation[cnt_layers + l]);
        }

        // set number of straws in each block
        setStraws(i, par_straws[i]);

        cnt_layers += layers;
    }

    return kTRUE;
}
