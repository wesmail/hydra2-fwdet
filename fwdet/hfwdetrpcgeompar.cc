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
                                       HParCond(name, title, context), modules(0)
{
    clear();
}

HFwDetRpcGeomPar::~HFwDetRpcGeomPar()
{
}

void HFwDetRpcGeomPar::print() const
{
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "Number of detectors:   " << getModules() << endl;
    for (Int_t m = 0; m < getModules(); ++m)
    {
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        cout << "Detector number:       " << m << endl;
        // prints the geometry parameters of a single detector
        cout << "Number of layers:      " << sm_mods[m].layers << endl;
        cout << "Module rotations:      " << sm_mods[m].modulePhi<< endl;
        cout << "Module Z-offset:       " << sm_mods[m].moduleZ << endl;

        cout << "Layer Y-offsets:      ";
        for (Int_t i = 0; i < sm_mods[m].layers; ++i)
        {
            cout << " " << sm_mods[m].layerY[i];
        }
        cout << endl;
    }
}

void HFwDetRpcGeomPar::clear()
{
    for (Int_t i = 0; i < FWDET_RPC_MAX_MODULES; ++i)
    {
        // clear all variables
        sm_mods[i].layers = 0;
        sm_mods[i].modulePhi = 0;
        sm_mods[i].moduleZ = 0;
        sm_mods[i].layerY = 0;
    }
}

Int_t HFwDetRpcGeomPar::getModules() const
{
    // return number of layers in single detector
    return modules;
}

Int_t HFwDetRpcGeomPar::getLayers(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].layers;
    else
        return -1;
}

Float_t HFwDetRpcGeomPar::getModulePhi(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].modulePhi;
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getModuleZ(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].moduleZ;
    else
        return 0.0;
}

Float_t HFwDetRpcGeomPar::getLayerY(Int_t m, Int_t l) const
{
    // return number of planes in layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].layerY[l];
    else
        return -1;
}

void HFwDetRpcGeomPar::setModules(int m)
{
    if (m <= FWDET_RPC_MAX_MODULES)
        modules = m;
}

void HFwDetRpcGeomPar::setLayers(Int_t m, Int_t l)
{
    // set number of layers, this function automatically
    // resizes all depending arrays
    sm_mods[m].layers = l;
    sm_mods[m].layerY.Set(l);
}

void HFwDetRpcGeomPar::setModulePhi(Int_t m, Float_t r)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules())
    {
        sm_mods[m].modulePhi = r;
    }
}

void HFwDetRpcGeomPar::setModuleZ(Int_t m, Float_t z)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules())
    {
        sm_mods[m].moduleZ = z;
    }
}

void HFwDetRpcGeomPar::setLayerY(Int_t m, Int_t l, Float_t o)
{
    // set number of panels for layer 'l', resizes all depending arrays
    // l -- layer number
    // s -- number of panels
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].layerY[l] = o;
    }
}

void HFwDetRpcGeomPar::putParams(HParamList* l)
{
    // add the parameters to the list for writing
    if (!l) return;

    Int_t total_layers = 0;

    // first find total number of layers
    for (Int_t i = 0; i < modules; ++i)
    {
        total_layers += getLayers(i);
    }

    TArrayI nLayers(modules);
    TArrayF fModulePhi(modules);
    TArrayF fModuleZ(modules);
    TArrayF fLayerY(total_layers);

    Int_t cnt_layers = 0;

    for (Int_t i = 0; i < modules; ++i)
    {
        // get number of layers
        Int_t layers = getLayers(i);
        Float_t rot = getModulePhi(i);
        Float_t offZ = getModuleZ(i);

        // set number of layers
        nLayers.SetAt(layers, i);
        fModulePhi.SetAt(rot, i);
        fModuleZ.SetAt(offZ, i);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            Int_t offset = getLayerY(i, l);

            // set number of panels
            fLayerY.SetAt(offset, cnt_layers + l);
        }

        cnt_layers += layers;
    }


    l->add("modules",   modules);
    l->add("layers",    nLayers);
    l->add("modulePhi", fModulePhi);
    l->add("moduleZ",   fModuleZ);
    l->add("layerY",    fLayerY);
}

Bool_t HFwDetRpcGeomPar::getParams(HParamList* l)
{
    // gets the parameters from the list (read from input)
    if (!l) return kFALSE;

    Int_t nModules;
    if (!l->fill("modules", &nModules))
        return kFALSE;

    TArrayI nLayers(nModules);
    if (!l->fill("layers", &nLayers))
        return kFALSE;

    if (nLayers.GetSize() != nModules)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of layers does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF fModulePhi(nModules);
    if (!l->fill("modulePhi", &fModulePhi))
        return kFALSE;

    if (fModulePhi.GetSize() != nModules)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of modulePhi does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF fModuleZ(nModules);
    if (!l->fill("moduleZ", &fModuleZ))
        return kFALSE;

    if (fModuleZ.GetSize() != nModules)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of moduleZ does not fit to number of detectors");
        return kFALSE;
    }

    Int_t total_layers = 0;
    for (Int_t d = 0; d < nModules; ++d)
    {
        total_layers += nLayers[d];
    }

    TArrayF fLayerY;
    if (!l->fill("layerY", &fLayerY))
        return kFALSE;

    if (fLayerY.GetSize() != total_layers)
    {
        Error("HFwDetRpcGeomPar::getParams(HParamList* l)",
              "Array size of panels=%d does not fit to number of layers=%d", nLayers.GetSize(), total_layers);
        return kFALSE;
    }

    setModules(nModules);

    Int_t cnt_layers = 0;
    for (Int_t i = 0; i < modules; ++i)
    {
        // get number of layers
        Int_t layers = nLayers[i];

        // set number of layers
        setLayers(i, layers);

        setModulePhi(i, fModulePhi[i]);
        setModuleZ(i, fModuleZ[i]);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            Float_t offset = fLayerY[cnt_layers + l];

            // set number of planes
            setLayerY(i, l, offset);
        }
        cnt_layers += layers;
    }

    return kTRUE;
}
