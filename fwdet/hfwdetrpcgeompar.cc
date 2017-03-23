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

void HFwDetRpcGeomPar::printParam() const
{
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "Number of detectors:   " << getModules() << endl;
    for (Int_t m = 0; m < getModules(); ++m)
    {
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        cout << "Detector number:       " << m << endl;
        // prints the geometry parameters of a single detector
        cout << "Number of layers:      " << getLayers(m) << endl;
        cout << "Module rotations:      " << getModulePhi(m) << endl;
        cout << "Module Z-offset:       " << getModuleZ(m) << endl;

        cout << "Layer Y-offsets:      ";
        for (Int_t i = 0; i < getLayers(m); ++i)
        {
            cout << " " << getLayerY(m, i);
        }
        cout << endl;
    }
}

void HFwDetRpcGeomPar::clear()
{
    for (Int_t i = 0; i < FWDET_RPC_MAX_MODULES; ++i)
    {
        // clear all variables
        sm_mods[i].nLayers = 0;
        sm_mods[i].fModulePhi = 0;
        sm_mods[i].fModuleZ = 0;
        sm_mods[i].fLayerY = 0;
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

            // set number of panels
            par_layerY.SetAt(offset, cnt_layers + l);
        }

        cnt_layers += layers;
    }


    l->add("fModules",   nModules);
    l->add("fLayers",    par_layers);
    l->add("fModulePhi", par_modulePhi);
    l->add("fModuleZ",   par_moduleZ);
    l->add("fLayerY",    par_layerY);
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

    TArrayF par_layerY;
    if (!l->fill("fLayerY", &par_layerY))
        return kFALSE;

    if (par_layerY.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of panels=%d does not fit to number of layers=%d", par_layers.GetSize(), total_layers);
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
            Float_t offset = par_layerY[cnt_layers + l];

            // set number of planes
            setLayerY(i, l, offset);
        }
        cnt_layers += layers;
    }

    return kTRUE;
}
