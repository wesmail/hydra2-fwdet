//*-- Author  : Rafal Lalik
//*-- Created : 10.01.2017

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
//  HFwDetRpcGeomPar
//
//  This class contains Forward Rpc detector geometry parameters
//
/////////////////////////////////////////////////////////////

#include "hfwdetdetector.h"
#include "hfwdetrpcgeompar.h"
#include "hparamlist.h"
#include "hades.h"
#include "hspectrometer.h"
#include "fwdetdef.h"

#include <iostream>
#include <vector>

using namespace std;

ClassImp(HFwDetRpcGeomPar);

HFwDetRpcGeomPar::HFwDetRpcGeomPar(const Char_t* name, const Char_t* title,
                                       const Char_t* context) :
                                       HParCond(name, title, context), nModules(0)
{
    clear();
}

HFwDetRpcGeomPar::~HFwDetRpcGeomPar()
{
}

void HFwDetRpcGeomPar::clear()
{
    for (Int_t i = 0; i < FWDET_RPC_MAX_MODULES; ++i)
    {
        // clear all variables
        sm_mods[i].nLayers = 0;
        sm_mods[i].fModulePhi = 0.0;
        sm_mods[i].fModuleZ = 0.0;
        sm_mods[i].fLayerY = 0.0;
        sm_mods[i].nStrips = 0;
        sm_mods[i].fStripLength = 0.0;
        sm_mods[i].fStripWidth = 0.0;
        sm_mods[i].fStripGap = 0.0;
    }
}

Int_t HFwDetRpcGeomPar::getModules() const
{
    // return number of layers in single detector
    return nModules;
}

Int_t HFwDetRpcGeomPar::getLayers(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].nLayers;
    else
        return -1;
}

Float_t HFwDetRpcGeomPar::getModulePhi(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].fModulePhi;
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getModuleZ(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].fModuleZ;
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getLayerY(Int_t m, Int_t l) const
{
    // return number of planes in layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fLayerY[l];
    else
        return -1;
}

Int_t HFwDetRpcGeomPar::getStrips(Int_t m, Int_t l) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].nStrips[l];
    else
        return 0;
}

Float_t HFwDetRpcGeomPar::getStripLength(Int_t m, Int_t l) const
{
    // return strip length
    // m -- module number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fStripLength[l];
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getStripWidth(Int_t m, Int_t l) const
{
    // return strip width
    // m -- module number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fStripWidth[l];
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getStripGap(Int_t m, Int_t l) const
{
    // return strip gap
    // m -- module number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].fStripGap[l];
    else
        return 0.0;
}

void HFwDetRpcGeomPar::setModules(int m)
{
    if (m <= FWDET_RPC_MAX_MODULES)
        nModules = m;
}

void HFwDetRpcGeomPar::setLayers(Int_t m, Int_t l)
{
    // set number of layers, this function automatically
    // resizes all depending arrays
    sm_mods[m].nLayers = l;
    sm_mods[m].fLayerY.Set(l);
    sm_mods[m].nStrips.Set(l);
    sm_mods[m].fStripLength.Set(l);
    sm_mods[m].fStripWidth.Set(l);
    sm_mods[m].fStripGap.Set(l);
}

void HFwDetRpcGeomPar::setModulePhi(Int_t m, Float_t r)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules())
    {
        sm_mods[m].fModulePhi = r;
    }
}

void HFwDetRpcGeomPar::setModuleZ(Int_t m, Float_t z)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules())
    {
        sm_mods[m].fModuleZ = z;
    }
}

void HFwDetRpcGeomPar::setLayerY(Int_t m, Int_t l, Float_t o)
{
    // set number of panels for layer 'l', resizes all depending arrays
    // l -- layer number
    // s -- number of panels
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fLayerY[l] = o;
    }
}

void HFwDetRpcGeomPar::setStrips(Int_t m, Int_t l, Int_t s)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].nStrips[l] = s;
    }
}

void HFwDetRpcGeomPar::setStripLength(Int_t m, Int_t l, Float_t len)
{
    // set strip length
    // m -- module number
    // b -- number of blocks
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fStripLength[l] = len;
    }
}

void HFwDetRpcGeomPar::setStripWidth(Int_t m, Int_t l, Float_t w)
{
    // set strip length
    // m -- module number
    // b -- number of blocks
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fStripWidth[l] = w;
    }
}

void HFwDetRpcGeomPar::setStripGap(Int_t m, Int_t l, Float_t g)
{
    // set strip length
    // m -- module number
    // b -- number of blocks
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].fStripGap[l] = g;
    }
}

void HFwDetRpcGeomPar::putParams(HParamList* l)
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
    TArrayF par_modulePhi(nModules);
    TArrayF par_moduleZ(nModules);
    TArrayF par_layerY(total_layers);
    TArrayI par_strips(total_layers);
    TArrayF par_stripLength(total_layers);
    TArrayF par_stripWidth(total_layers);
    TArrayF par_stripGap(total_layers);

    Int_t cnt_layers = 0;

    for (Int_t i = 0; i < nModules; ++i)
    {
        // get number of layers
        Int_t layers = getLayers(i);
        Float_t rot = getModulePhi(i);
        Float_t offZ = getModuleZ(i);

        // set number of layers
        par_layers.SetAt(layers, i);
        par_modulePhi.SetAt(rot, i);
        par_moduleZ.SetAt(offZ, i);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            Int_t offset = getLayerY(i, l);
            Int_t sn = getStrips(i, l);
            Float_t sl = getStripLength(i, l);
            Float_t sw = getStripWidth(i, l);
            Float_t sg = getStripGap(i, l);

            par_layerY.SetAt(offset, cnt_layers + l);
            par_strips.SetAt(sn, cnt_layers + l);
            par_stripLength.SetAt(sl, cnt_layers + l);
            par_stripWidth.SetAt(sw, cnt_layers + l);
            par_stripGap.SetAt(sg, cnt_layers + l);
        }

        cnt_layers += layers;
    }

    l->add("fModules",     nModules);
    l->add("fLayers",      par_layers);
    l->add("fModulePhi",   par_modulePhi);
    l->add("fModuleZ",     par_moduleZ);
    l->add("fLayerY",      par_layerY);
    l->add("nStrips",      par_strips);
    l->add("fStripLength", par_stripLength);
    l->add("fStripWidth",  par_stripWidth);
    l->add("fStripGap",    par_stripGap);
}

Bool_t HFwDetRpcGeomPar::getParams(HParamList* l)
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
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of layers does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF par_modulePhi(par_modules);
    if (!l->fill("fModulePhi", &par_modulePhi))
        return kFALSE;

    if (par_modulePhi.GetSize() != par_modules)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of modulePhi does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF par_moduleZ(par_modules);
    if (!l->fill("fModuleZ", &par_moduleZ))
        return kFALSE;

    if (par_moduleZ.GetSize() != par_modules)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of moduleZ does not fit to number of detectors");
        return kFALSE;
    }

    Int_t total_layers = 0;
    for (Int_t d = 0; d < par_modules; ++d)
    {
        total_layers += par_layers[d];
    }

    TArrayF par_layerY(total_layers);
    if (!l->fill("fLayerY", &par_layerY))
        return kFALSE;

    if (par_layerY.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of panels=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayI par_strips(total_layers);
    if (!l->fill("nStrips", &par_strips))
        return kFALSE;

    if (par_strips.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of strips=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayF par_stripLength(total_layers);
    if (!l->fill("fStripLength", &par_stripLength))
        return kFALSE;

    if (par_stripLength.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of stripLength=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayF par_stripWidth(total_layers);
    if (!l->fill("fStripWidth", &par_stripWidth))
        return kFALSE;

    if (par_stripWidth.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of stripWidth=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayF par_stripGap(total_layers);
    if (!l->fill("fStripGap", &par_stripGap))
        return kFALSE;

    if (par_stripGap.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of stripGap=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
        return kFALSE;
    }

    setModules(par_modules);

    Int_t cnt_layers = 0;
    for (Int_t i = 0; i < par_modules; ++i)
    {
        // get number of layers
        Int_t layers = par_layers[i];

        // set number of layers
        setLayers(i, layers);

        setModulePhi(i, par_modulePhi[i]);
        setModuleZ(i, par_moduleZ[i]);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            setLayerY(i, l, par_layerY[cnt_layers + l]);
            setStrips(i, l, par_strips[cnt_layers + l]);
            setStripLength(i, l, par_stripLength[cnt_layers + l]);
            setStripWidth(i, l, par_stripWidth[cnt_layers + l]);
            setStripGap(i, l, par_stripGap[cnt_layers + l]);
        }
        cnt_layers += layers;
    }

    return kTRUE;
}
