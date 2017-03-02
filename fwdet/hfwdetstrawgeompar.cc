#include "hfwdetdetector.h"
#include "hfwdetstrawgeompar.h"
#include "hparamlist.h"
#include "hades.h"
#include "hspectrometer.h"
#include "fwdetdef.h"

#include <iostream>
#include <vector>

using namespace std;

ClassImp(HFwDetStrawGeomPar);

HFwDetStrawGeomPar::HFwDetStrawGeomPar(const Char_t* name, const Char_t* title,
                                       const Char_t* context) :
                                       HParCond(name, title, context), modules(0)
{
    clear();
}

HFwDetStrawGeomPar::~HFwDetStrawGeomPar()
{
}

void HFwDetStrawGeomPar::print() const
{
    cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
    cout << "Number of detectors:    " << getModules() << endl;
    for (Int_t m = 0; m < getModules(); ++m)
    {
        cout << "@@@@@@@@@@@@@@@@@@@@@@@@@@" << endl;
        cout << "Detector number:       " << m << endl;
        // prints the geometry parameters of a single detector
        cout << "Number of layers:      " << sm_mods[m].layers << endl;
        cout << "Number of panels:      ";
        for (Int_t i = 0; i < sm_mods[m].panels.GetSize(); ++i)
        {
            cout << " " << sm_mods[m].panels[i];
        }
        cout << endl;
        cout << "Number of blocks:      " << sm_mods[m].blocks << endl;
        cout << "Number of straws:      " << sm_mods[m].straws << endl;

        cout << "Layer offset Z:        " << endl;
        for (Int_t i = 0; i < sm_mods[m].layers; ++i)
        {
            for (Int_t p = 0; p < FWDET_STRAW_MAX_PLANES; ++p)
                cout << " " << sm_mods[m].offsetZ[i][p];
            cout << "   ";
        }
        cout << endl;

        cout << "Layer offset X:        " << endl;
        for (Int_t i = 0; i < sm_mods[m].layers; ++i)
        {
            for (Int_t p = 0; p < FWDET_STRAW_MAX_PLANES; ++p)
                cout << " " << sm_mods[m].offsetX[i][p];
            cout << "   ";    }
        cout << endl;

        cout << "Layer rotations:       " << endl;
        for (Int_t i = 0; i < sm_mods[m].layerRotation.GetSize(); ++i)
        {
            cout << " " << sm_mods[m].layerRotation[i];
        }
        cout << endl;
    }
}

void HFwDetStrawGeomPar::clear()
{
    for (Int_t i = 0; i < FWDET_STRAW_MAX_MODULES; ++i)
    {
        // clear all variables
        sm_mods[i].layers = 0;
        sm_mods[i].panels = 0;
        sm_mods[i].blocks = 0;
        sm_mods[i].straws = 0;
        sm_mods[i].strawRadius = 0.0;
        sm_mods[i].strawPitch = 0.0;
        sm_mods[i].offsetZ = 0.0;
        sm_mods[i].offsetX = 0.0;
        sm_mods[i].layerRotation = 0;
    }
}

Int_t HFwDetStrawGeomPar::getModules() const
{
    // return number of layers in single detector
    return modules;
}

Int_t HFwDetStrawGeomPar::getLayers(Int_t m) const
{
    // return number of layers in module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].layers;
    else
        return -1;
}

Int_t HFwDetStrawGeomPar::getPanels(Int_t m, Int_t l) const
{
    // return number of planes in layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].panels[l];
    else
        return -1;
}

Int_t HFwDetStrawGeomPar::getBlocks(Int_t m) const
{
    // return number of straws in single layer of module 'm'
    // m -- module number
    if (m < getModules())
        return sm_mods[m].blocks;
    else
        return -1;
}

Int_t HFwDetStrawGeomPar::getStraws(Int_t m) const
{
    // return number of straws in single layer of module 'm'
    // l -- layer number
    if (m < getModules())
        return sm_mods[m].straws;//[_off_straws[l][d][b]];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getStrawRadius(Int_t m, Int_t l) const
{
    // return straw radius in single sublayer of layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].strawRadius[l];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getStrawPitch(Int_t m, Int_t l) const
{
    // return straw pitch in single sublayer of layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].strawPitch[l];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getOffsetZ(Int_t m, Int_t l, Int_t p) const
{
    // return Z-coordinate of the beginning of the sublayer 's' in layer 'l'
    // m --module number
    // l -- layer number
    // s -- sublayer number
    if (m < getModules() && l < getLayers(m) && p < getPanels(m, l))
        return sm_mods[m].offsetZ[l][p];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getOffsetX(Int_t m, Int_t l, Int_t p) const
{
    // return X-coordinate of the beginning of the sublayer 's' in layer 'l'
    // m -- module number
    // l -- layer number
    // s -- sublayer number
    if (m < getModules() && l < getLayers(m) && p < getPanels(m, l))
        return sm_mods[m].offsetX[l][p];
    else
        return -1;
}

Float_t HFwDetStrawGeomPar::getLayerRotation(Int_t m, Int_t l) const
{
    // return transformation matrix for a layer 'l'
    // l -- layer number
    if (m < getModules() && l < getLayers(m))
        return sm_mods[m].layerRotation[l];
    else
        return -1;
}

void HFwDetStrawGeomPar::setModules(int m)
{
    if (m <= FWDET_STRAW_MAX_MODULES)
        modules = m;
}

void HFwDetStrawGeomPar::setLayers(Int_t m, Int_t l)
{
    // set number of layers, this function automatically
    // resizes all depending arrays
    sm_mods[m].layers = l;
    sm_mods[m].panels.Set(l);
    sm_mods[m].strawRadius.Set(l * FWDET_STRAW_MAX_PLANES);
    sm_mods[m].strawPitch.Set(l * FWDET_STRAW_MAX_PLANES);
    sm_mods[m].layerRotation.Set(l);
}

void HFwDetStrawGeomPar::setPanels(Int_t m, Int_t l, Int_t d)
{
    // set number of panels for layer 'l', resizes all depending arrays
    // l -- layer number
    // s -- number of panels
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].panels[l] = d;

        Int_t max_panels = 0;
        for (Int_t i = 0; i < getLayers(m); ++i)
        {
            sm_mods[m].panels[i] > max_panels ? max_panels = sm_mods[m].panels[l] : max_panels;
        }

        sm_mods[m].offsetZ.ResizeTo(getLayers(m), FWDET_STRAW_MAX_PLANES);
        sm_mods[m].offsetX.ResizeTo(getLayers(m), FWDET_STRAW_MAX_PLANES);
    }
}

void HFwDetStrawGeomPar::setBlocks(Int_t m, Int_t b)
{
    // set number of blocks for module 'm'
    // m -- module number
    // b -- number of blocks
    if (m < getModules())
    {
        sm_mods[m].blocks = b;
    }
}

void HFwDetStrawGeomPar::setStraws(Int_t m, Int_t s)
{
    // set number of straws for moduke 'm'
    // m -- module number
    // s -- number of straws
    if (m < getModules())
    {
        sm_mods[m].straws = s;
    }
}

void HFwDetStrawGeomPar::setStrawRadius(Int_t m, Int_t l, Float_t r)
{
    // set straw radius in each sublayer of layer 'l'
    // l -- layer number
    // r -- straw radius
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].strawRadius[l] = r;
    }
}

void HFwDetStrawGeomPar::setStrawPitch(Int_t m, Int_t l, Float_t p)
{
    // set straws pitch in each sublayer of layer 'l'
    // l -- layer number
    // p -- straws pitch
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].strawPitch[l] = p;
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
        sm_mods[m].offsetZ[l][p] = z;
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
        sm_mods[m].offsetX[l][p] = x;
    }
}

void HFwDetStrawGeomPar::setLayerRotation(Int_t m, Int_t l, Float_t r)
{
    // set lab transform for later 'l'
    // l -- layer number
    // lt -- lab transformation matrix [3x3 rot., 3-x transl.]
    if (m < getModules() && l < getLayers(m))
    {
        sm_mods[m].layerRotation[l] = r;
    }
}

void HFwDetStrawGeomPar::putParams(HParamList* l)
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
    TArrayI nPanels(total_layers);
    TArrayI nBlocks(modules);
    TArrayI nStraws(modules);

    TArrayF fOffsetX(total_layers * FWDET_STRAW_MAX_PLANES);
    TArrayF fOffsetZ(total_layers * FWDET_STRAW_MAX_PLANES);

    TArrayF fStrawR(total_layers);
    TArrayF fStrawP(total_layers);

    TArrayF fLayerRotation(total_layers);

    Int_t cnt_layers = 0;

    for (Int_t i = 0; i < modules; ++i)
    {
        // get number of layers
        Int_t layers = getLayers(i);

        // set number of layers
        nLayers.SetAt(layers, i);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            Int_t panels = getPanels(i, l);

            // set number of panels
            nPanels.SetAt(panels, cnt_layers + l);

            fStrawR.SetAt(getStrawRadius(i, l), cnt_layers + l);
            fStrawP.SetAt(getStrawPitch(i,l), cnt_layers + l);

            for (Int_t s = 0; s < FWDET_STRAW_MAX_PLANES; ++s)
            {
                fOffsetZ.SetAt(getOffsetZ(i, l, s), 2*cnt_layers + s);
                fOffsetX.SetAt(getOffsetX(i, l, s), 2*cnt_layers + s);
            }

            fLayerRotation.SetAt(getLayerRotation(i, l), cnt_layers + l);
        }

        // set number of blocks in each panel
        nBlocks.SetAt(getBlocks(i), i);

        // set number of straws in each block
        nStraws.SetAt(getStraws(i), i);

        cnt_layers += layers;
    }


    l->add("modules",       modules);
    l->add("layers",        nLayers);
    l->add("panels",        nPanels);
    l->add("straws",        nStraws);
    l->add("offsetZ",       fOffsetZ);
    l->add("offsetX",       fOffsetX);
    l->add("strawRadius",   fStrawR);
    l->add("strawPitch",    fStrawP);
    l->add("layerRotation", fLayerRotation);
}

Bool_t HFwDetStrawGeomPar::getParams(HParamList* l)
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
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of layers does not fit to number of detectors");
        return kFALSE;
    }

    Int_t total_layers = 0;
    for (Int_t d = 0; d < nModules; ++d)
    {
        total_layers += nLayers[d];
    }

    TArrayI nPanels(total_layers);
    if (!l->fill("panels", &nPanels))
        return kFALSE;

    if (nPanels.GetSize() != total_layers)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of panels=%d does not fit to number of layers=%d", nLayers.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayI nBlocks;
    if (!l->fill("blocks", &nBlocks))
        return kFALSE;

    if (nBlocks.GetSize() != nModules)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of blocks does not fit to number of detectors");
        return kFALSE;
    }

    TArrayI nStraws;
    if (!l->fill("straws", &nStraws))
        return kFALSE;

    if (nStraws.GetSize() != nModules)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of straws does not fit to number of detectors");
        return kFALSE;
    }

    TArrayF fStrawRadius(total_layers);
    if (!l->fill("strawRadius", &fStrawRadius))
        return kFALSE;

    if (fStrawRadius.GetSize() != total_layers)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of strawRadius=%d does not fit to number of layers=%d", fStrawRadius.GetSize(), total_layers);
        return kFALSE;
    }

    TArrayF fStrawPitch(total_layers);
    if (!l->fill("strawPitch", &fStrawPitch))
        return kFALSE;

    if (fStrawPitch.GetSize() != total_layers)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of strawPitch=%d does not fit to number of layers=%d", fStrawPitch.GetSize(), total_layers);
        return kFALSE;
    }

    Int_t cnt_layers = 0;
    for (Int_t d = 0; d < nModules; ++d)
    {
        cnt_layers += nLayers[d];
    }
    const Int_t cnt_planes = cnt_layers * FWDET_STRAW_MAX_PLANES;

    TArrayF fOffsetZ(cnt_planes);
    if (!l->fill("offsetZ", &fOffsetZ))
        return kFALSE;

    if (fOffsetZ.GetSize() != cnt_planes)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of planeZ=%d does not fit to number of planes=%d", fOffsetZ.GetSize(), cnt_planes);
        return kFALSE;
    }

    TArrayF fOffsetX(cnt_planes);
    if (!l->fill("offsetX", &fOffsetX))
        return kFALSE;

    if (fOffsetX.GetSize() != cnt_planes)
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of planeX=%d does not fit to number of planes=%d", fOffsetX.GetSize(), cnt_planes);
        return kFALSE;
    }

    TArrayF fLayerRotation(total_layers);
    if (!l->fill("layerRotation", &fLayerRotation))
        return kFALSE;

    if (fLayerRotation.GetSize() != (int)(total_layers))
    {
        Error("HFwDetStrawGeomPar::getParams(HParamList* l)",
              "Array size of layerRotation=%d does not fit to number of layers=%d", fLayerRotation.GetSize(), total_layers);
        return kFALSE;
    }

    cnt_layers = 0;

    setModules(nModules);

    for (Int_t i = 0; i < modules; ++i)
    {
        // get number of layers
        Int_t layers = nLayers[i];

        // set number of layers
        setLayers(i, layers);

        // iterate over layers
        for (Int_t l = 0; l < layers; ++l)
        {
            Int_t panels = nPanels[l];

            // set number of planes
            setPanels(i, l, panels);

            setStrawRadius(i, l, fStrawRadius[cnt_layers + l]);
            setStrawPitch(i, l, fStrawPitch[cnt_layers + l]);

            for (Int_t s = 0; s < FWDET_STRAW_MAX_PLANES; ++s)
            {
                setOffsetZ(i, l, s, fOffsetZ[2*(cnt_layers + l) + s]);
                setOffsetX(i, l, s, fOffsetX[2*(cnt_layers + l) + s]);
            }

            setLayerRotation(i, l, fLayerRotation[cnt_layers + l]);
        }

        // set number of blocks in each double layer
        setBlocks(i, nBlocks[i]);

        // set number of straws in each block
        setStraws(i, nStraws[i]);

        cnt_layers += layers;
    }

    return kTRUE;
}

// void HFwDetStrawGeomPar::printParams() const
// {
//     // prints the parameters (overloads HParCond::printParams())
//     cout << "----- Geometry of FwDetStraw detectors ------------------" << endl;
//     for (Int_t i = 0; i < modules; ++i)
//     {
//         HFwDetStrawGeomPar* p = (HFwDetStrawGeomPar*)pDetectors->At(i);  
//         cout << "Detector number:       " << i << endl;
//         print();
//         cout << "-----------------------" << endl;
//     }
// }
