
// from ROOT
#include "TVector3.h"

// from hydra
#include "hades.h"
#include "hmdclayergeompar.h"
#include "hmdcsizescells.h"
#include "hruntimedb.h"

#include "hkaldef.h"
#include "hkaldetcradle.h"
#include "hkalgeomtools.h"
#include "hkalmdcmeaslayer.h"
#include "hmaterials.h"
#include "hkalmixture.h"

ClassImp(HKalDetCradle)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// A detector cradle is a container that stores the detector's measurement layers.
// The constructor HKalDetCradle(Int_t nLayInMdc) will automatically add
// Mdc layers to the cradle.
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalDetCradle::HKalDetCradle() {
    matsMdc      = NULL;
    matAir       = NULL;
    nLayersInMdc = 1;
}

HKalDetCradle::HKalDetCradle(Int_t nLayInMdc, const TObjArray *customMats) :
TObjArray(kNumSecs*kNumMdcs*nLayInMdc) {
    // Default contructor that adds Mdc layers to the cradle.
    // The Mdc layers will be added to the cradle in the following order:
    // 1. Sort by sector number (0..5)
    // 2. Sort by module number (0..3) in a sector
    // 3. Sort by layer number of module in sector
    // nLayInMdc: the number of mdc layers that should be added to each sector in a module.
    // customMats: Materials for MDCs and materials between MDCs. If empty
    //             standard materials will be used

    matsMdc    = new HKalMixture*[kNumMdcs];
    matAir     = NULL;
    nLayersInMdc = nLayInMdc;
    if(customMats) {
	if(customMats->GetEntries() == kNumMdcs+1) {
	    for(Int_t iMod = 0; iMod < kNumMdcs; iMod++) {
		HKalMixture *mat = dynamic_cast<HKalMixture*>(customMats->At(iMod));
		if(mat) {
		    matsMdc[iMod] = mat;
		} else {
		    Error("HKalDetCradle()",
			  Form("No material found for MDC %i", iMod));
		}
	    }
	    HKalMixture *mat = dynamic_cast<HKalMixture*>(customMats->At(kNumMdcs));
	    if(mat) {
		matAir = mat;
	    } else {
		Error("HKalDetCradle()",
		      Form("No object found at index %i", kNumMdcs));
	    }
	} else {
	    Error("HKalDetCradle()",
		  Form("Size of materials array is %i. Expected %i.",
		       customMats->GetEntries(), kNumMdcs));
	}
    } else {
	initMdcMaterials();
    }

    for(Int_t iSec = 0; iSec < kNumSecs; iSec++) {
        for(Int_t iMod = 0; iMod < kNumMdcs; iMod++) {
#if kalDebug > 0
            if(!HMdcSizesCells::getObject()) {
                Warning("HKalDetCradle(Int_t)", "HMdcSizesCells has not been initialized. Measurement layers not added to detector cradle.");
                break;
            }
#endif
            addMdcLayersToCradle(HMdcSizesCells::getObject(), iSec, iMod, nLayersInMdc);
        }
    }
}

HKalDetCradle::~HKalDetCradle() {
    delete [] matsMdc;
    delete matAir;
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

void HKalDetCradle::addMdcLayersToCradle(const HMdcSizesCells *fSizesCells,
					 Int_t sector, Int_t module, Int_t nLayers) {
    // Create a mdc detector with nLayers measurement layers. Each time the constructor
    // is called, a new set if KalMdcMeasLayer objects will be added to the TObjArray.
    // Module and sector identify the drift chamber.
    //
    // Input:
    // fSizesCells: object that can do coordinate transformation layer<->sector
    // sector:      the sector where this detector is located (needed for coordinate transformation)
    // module:      the module where this detector is located (needed for coordinate transformation)
    // nLayers:     number of layers in detector

    HRuntimeDb *rtdb = gHades->getRuntimeDb();

    HMdcLayerGeomPar *layerGeomPar = (HMdcLayerGeomPar*)rtdb->getContainer("MdcLayerGeomPar");

    for (Int_t layer = 0; layer < nLayers; layer++) {
        // A measurement vector is modeled as a plane. A plane is defined by
        // a point on the plane and a normal vector.
        // The Center of layer is the Null vector and the normal vector is (0,0,1)
        // in the layer coordinate system.
        // These vectors need to be transformed into sector coordinates.
#if kalDebug > 0
        if(!secOk(sector)) {
            Error("addMdcLayersToCradle()", Form("Invalid sector number: %i. Layers not added to cradle.", sector));
            break;
        }
        if(!modOk(module)) {
            Error("addMdcLayersToCradle()", Form("Invalid module number: %i. Layers not added to cradle.", module));
            break;
        }
        if(!fSizesCells) {
            Error("addMdcLayersToCradle()", "HMdcSizesCells is not initialized.");
        }
#endif

        Double_t x = 0.;
        Double_t y = 0.;
        Double_t z = 0.;

	if(nLayers == 1) {
	    HKalGeomTools::TransModuleToSector(x, y, z, sector, module);
            //((*fSizesCells)[sector][module]).transFrom(x,y,z); // transform to sector coordinate sys
        } else {
	    HKalGeomTools::TransLayerToSector(x, y, z, sector, module, layer);
            //((*fSizesCells)[sector][module][layer]).transFrom(x,y,z); // transform to sector coordinate sys
        }

	TVector3 vPlaneCenter(x, y, z);

        x = 0.;
        y = 0.;
        z = 1.;
        if(nLayers == 1) {
	    HKalGeomTools::TransModuleToSector(x, y, z, sector, module);
            //((*fSizesCells)[sector][module]).transFrom(x,y,z); // transform to sector coordinate system
        } else {
	    HKalGeomTools::TransLayerToSector(x, y, z, sector, module, layer);
            //((*fSizesCells)[sector][module][layer]).transFrom(x,y,z); // transform to sector coordinate sys
	}

        TVector3 vZ(x, y, z);
	TVector3 vPlaneNormal = vZ - vPlaneCenter;

        vPlaneNormal.Unit();

        // Choose the normal vector, so that the scalar product of the normal vector
        // and the point on the plane is positive.
        if(vPlaneCenter * vPlaneNormal < 0) {
            vPlaneNormal *= -1.;
        }

        HKalMdcMeasLayer *meas;
        HMdcLayerGeomParLay &layerGeomParLay = (HMdcLayerGeomParLay&)(*layerGeomPar)[sector][module][layer];
        Double_t layerThickness = layerGeomParLay.getCatDist();

        if(nLayers == 1) {
            layerThickness *= 8.; // 6 layers + 2 * layer thickness outside
        }
        if(nLayers != 1 && layer == 0) {
            meas = new HKalMdcMeasLayer(vPlaneCenter, vPlaneNormal, matsMdc[module], matAir,
                                        module, sector, layer, nLayersInMdc, layerThickness);
        } else {
            meas = new HKalMdcMeasLayer(vPlaneCenter, vPlaneNormal, matsMdc[module], matAir,
                                        module, sector, layer, nLayersInMdc, layerThickness);
        }
        install(meas);
    }
    SetOwner();
}

const HKalMdcMeasLayer* HKalDetCradle::getMdcLayerAt(Int_t sector, Int_t module, Int_t layer) const {
    // Returns the mdc layer in a sector
    // Input:
    // sector: the sector where this detector is located (needed for coordinate transformation)
    // module: the module where this detector is located (needed for coordinate transformation)
    // layer:  layer number in the sector of the mdc module.

    Int_t layIdx = (sector*kNumMdcs + module)*nLayersInMdc;
    if(layer != 6) { // layer 6 = midplane
        layIdx += layer;
    }
#ifdef kalDebug
    if(layIdx < 0 || layIdx > GetEntries() - 1) {
        Error("getMdcLayerAt()", Form("Mdc layer at sector %i, module %i, layer %i not found in detector cradle. Tried to access element at index %i.", sector, module, layer, layIdx));
        exit(1);
    }
    if(!At(layIdx)->InheritsFrom("HKalMdcMeasLayer")) {
        Error("getMdcLayerAt()", Form("Object at index %i in detector cradle is of class %s. Expected class HKalMdcMeasLayer.", layIdx, At(layIdx)->ClassName()));
        exit(1);
    }
    return (HKalMdcMeasLayer*)At(layIdx);
#else
    return (HKalMdcMeasLayer*)At(layIdx);
#endif
}

HKalMixture* HKalDetCradle::getMdcMaterial(mdcComponent comp, Int_t module) const {
    // Returns the material of an Mdc layer component.
    // Input:
    // comp:   The Mdc component (Cathode/field/sense wires, gas, window, mylar foil)
    // module: The module the Mdc layer is located in.

    if(!modOk(module)) {
        Error("getMdcMaterial()", Form("%i is not a valid module number", module));
        return NULL;
    }

    HKalMixture *mdcMix = NULL;
    Float_t mat[kNumMdcMats];

    switch(comp) {
    // Cathode and field wires consists of the same materials.
    case kCatWi:
    case kFieldWi:
	mdcMix = new HKalMixture("Aluminum", "altu", 1);
	getProperties(mat, "al");
	mdcMix->defineElement(0, mat, 1.F);
	mdcMix->calcMixture();
        break;

    case kSenseWi:
        // Material for sense wires.
	Float_t vol[2], rho[2], w[2];
	rho[0] = HMaterials::getDensity("Copper");
	rho[1] = HMaterials::getDensity("Beryllium");
	// Mdc layers contain a 84:16 volume percent mixture of Argon-Isobutane.
	vol[0] = .98;
	vol[1] = .02;
	// Get mass fractions for gas mixtue.
        mdcMix = new HKalMixture("Copper/Beryllium 98/2", "co/be", 2);
	mdcMix->calcMassFracFromVolFrac(w, 2, rho, vol);
	getProperties(mat, "Copper");
	mdcMix->defineElement(0, mat, w[0]);
	getProperties(mat, "Beryllium");
	mdcMix->defineElement(1, mat, w[1]);
	mdcMix->calcMixture();
        break;

    case kWindow:
        // Material for mylar foil.
        mdcMix = new HKalMixture("Mylar", "mylar", 1);
        getProperties(mat, "mylar");
        mdcMix->defineElement(0, mat, 1.F);
        mdcMix->calcMixture();
        break;

    case kGas:

        // Gas in Mdc 1 is Argon-Isobutane
        if(module == 0) {
            Float_t vol[2], rho[2], w[2];
            rho[0] = HMaterials::getDensity("argon");
            rho[1] = HMaterials::getDensity("CO2");
            // Mdc layers contain a 84:16 volume percent mixture of Argon-Isobutane.
            vol[0] = .70;
            vol[1] = .30;
            // Get mass fractions for gas mixtue.
            mdcMix = new HKalMixture("Argon-Isobutane 70:30", "arco2_70_30", 2);
            mdcMix->calcMassFracFromVolFrac(w, 2, rho, vol);
            getProperties(mat, "argon");
            mdcMix->defineElement(0, mat, w[0]);
            getProperties(mat, "CO2");
            mdcMix->defineElement(1, mat, w[1]);
            mdcMix->calcMixture();
        // Gas in the other Mdcs is Helium-Isobutane.
	} else {

            Float_t vol[2], rho[2], w[2];
            rho[0] = HMaterials::getDensity("helium");
            rho[1] = HMaterials::getDensity("butane");
            // Mdc layers contain a 60:40 volume percent mixture of Helium-Isobutane.
            vol[0] = .84;
            vol[1] = .16;
            // Get mass fractions.
            mdcMix = new HKalMixture("Helium-Isobutane 60:40", "hebutane", 2);
            mdcMix->calcMassFracFromVolFrac(w, 2, rho, vol);
            getProperties(mat, "helium");
            mdcMix->defineElement(0, mat, w[0]);
            getProperties(mat, "butane");
            mdcMix->defineElement(1, mat, w[1]);
	    mdcMix->calcMixture();
        }
        break;

    }
    return mdcMix;
}


void HKalDetCradle::getMdcMaterialVolumeFracs(Float_t volFracs[kNumMdcMats],
					      Int_t sector, Int_t module, Int_t layer) const {
    // Calculate the volume fractions of the different mdc components.
    // volFracs: the volume fractions of the components
    // module:   the module number of the mdc layer
    // sector:   the sector number of the mdc layer
    // layer:    the layer number

    if(!layOk(layer)) {
        Error("getMdcMaterialVolumeFracs()", Form("%i is not a valid layer number", layer));
        return;

    }
    if(!modOk(module)) {
        Error("getMdcMaterialVolumeFracs()", Form("%i is not a valid module number", module));
        return;
    }

    HRuntimeDb *rtdb = gHades->getRuntimeDb();
    HMdcLayerGeomPar    *layerGeomPar    = (HMdcLayerGeomPar*)rtdb->getContainer("MdcLayerGeomPar");
    HMdcLayerGeomParLay &layerGeomParLay = (HMdcLayerGeomParLay&)(*layerGeomPar)[sector][module][layer];

    Float_t nLayers = 6; // number of layers in mdc plane

    // Thickness of cell in mm = distance between cathode layers.
    Float_t xCell   = layerGeomParLay.getCatDist();

    Float_t xMylar  = .012; // thickness of mylar foils in mm
    //Float_t xPlane  = xCell * (nLayers+2) + 2 * xMylar; // thickness of mdc plane in mm

    // Distance (pitch) between two wires in mm.
    Float_t pitchCatWi;
    if(module == 3) {
        pitchCatWi = 4.F;
    } else {
        pitchCatWi = 2.F;
    }
    Float_t pitchFieldWi = xCell;
    Float_t pitchSenseWi = xCell;

    Double_t pi = TMath::Pi();

    // Radii of wires in mm.
    Float_t rCatWi   = layerGeomParLay.getCathodeWireThickness() / 2;
    Float_t rFieldWi;
    if(module == 0 || module == 1) {
        rFieldWi = 0.04;
    } else {
        rFieldWi = 0.05;
    }
    Float_t rSenseWi;
    if(module == 3) {
        rSenseWi = 0.015;
    } else {
        rSenseWi = 0.01;
    }

    Float_t thicknessCell[kNumMdcMats];
    // Spread out wires with circular cross sectional area over the wires' pitch
    // to get a wire with a rectangular cross sectional area of equal proportions.
    // Area of circle: pi * r^2
    // Area of rectangle: pitch * x
    // => Spread out thickness of wire: x = pi * r^2 / pitch
    thicknessCell[kCatWi]   = pi * rCatWi   * rCatWi   / pitchCatWi;
    thicknessCell[kFieldWi] = pi * rFieldWi * rFieldWi / pitchFieldWi;
    thicknessCell[kSenseWi] = pi * rSenseWi * rSenseWi / pitchSenseWi;
    thicknessCell[kWindow]  = xMylar;
    thicknessCell[kGas]     = xCell - (thicknessCell[kCatWi] + thicknessCell[kFieldWi] + thicknessCell[kSenseWi]);

    // Materials' thickness added over the whole Mdc plane.
    Float_t thickness[kNumMdcMats];
    thickness[kCatWi]   = (nLayers+1) * thicknessCell[kCatWi];
    thickness[kFieldWi] = nLayers     * thicknessCell[kFieldWi];
    thickness[kSenseWi] = nLayers     * thicknessCell[kSenseWi];
    thickness[kWindow]  = 2           * thicknessCell[kWindow];
    thickness[kGas]     = nLayers     * thicknessCell[kGas]      + xCell * 2;

    Float_t thicknessTotal = 0.F;
    for(Int_t i = 0; i < kNumMdcMats; i++) {
        thicknessTotal += thickness[i];
    }

    for(Int_t i = 0; i < kNumMdcMats; i++) {
        volFracs[i] = thickness[i] / thicknessTotal;
    }
}

void HKalDetCradle::initMdcMaterials() {

    Float_t mat[5]; // Storage for material properties.

    matAir = new HKalMixture("Air", "air", 1);
    getProperties(mat, "air");
    matAir->defineElement(0, mat, 1.F);
    matAir->calcMixture();

    // Materials of mdc planes.

    // Materials of the different components (gas, wires, window foils) of an mdc plane.
    HKalMixture const *matsMdcComp     [kNumMdcMats];
    // Density of mdc components.
    Float_t      rhoMatsMdcComp  [kNumMdcMats];
    // Fractions on total volume of the different mdc components.
    Float_t      volFracsMdcComp [kNumMdcMats];
    // Mass fractions of the different mdc components.
    Float_t      massFracsMdcComp[kNumMdcMats];

    Int_t sector = 0;
    Int_t layer  = 0;

    for(Int_t iModule = 0; iModule < kNumMdcs; iModule++) {
        // Get the materials of the mdc components.
        for(Int_t iComp = 0; iComp < kNumMdcMats; iComp++) {
            matsMdcComp   [iComp] = getMdcMaterial((mdcComponent)iComp, iModule);
            rhoMatsMdcComp[iComp] = matsMdcComp[iComp]->GetDensity();
        }
        // Calculate volume fractions of mdc components.
        getMdcMaterialVolumeFracs(volFracsMdcComp, sector, iModule, layer);
        // Calculate mass fractions from volume fractions and densities.
        HKalMixture::calcMassFracFromVolFrac(massFracsMdcComp, kNumMdcMats, rhoMatsMdcComp, volFracsMdcComp);

        // Make a mixture of the mdc components' materials.
        matsMdc[iModule] = new HKalMixture(Form("Materials for mdc plane %i.", iModule), "mdc", kNumMdcMats);
        for(Int_t iComp = 0; iComp < kNumMdcMats; iComp++) {
            matsMdc[iModule]->defineElement(iComp, matsMdcComp[iComp]->GetA(), matsMdcComp[iComp]->GetZ(),
                                            matsMdcComp[iComp]->GetDensity(), matsMdcComp[iComp]->GetExciteEner(),
                                            matsMdcComp[iComp]->GetRadLength(), massFracsMdcComp[iComp]);
        }
        matsMdc[iModule]->calcMixture();

        for(Int_t iComp = 0; iComp < kNumMdcMats; iComp++) {
            delete matsMdcComp[iComp];
        }
    }
}

void HKalDetCradle::getProperties(Float_t mat[5], const Char_t *id) {
    // Returns the atomic mass, atomic number, density, excitation energy and
    // radiation length in an array and converts the units from cm to mm and eV to MeV.
    // The array index where a material property is stored is determined by the matIdx
    // enum in hkaldef.h.
    // Output:
    // mat: array with material properties.
    // Input:
    // id:  material name as in HMaterial

    mat[Kalman::kMatIdxA]         = HMaterials::getA(id);
    mat[Kalman::kMatIdxZ]         = HMaterials::getZ(id);
    mat[Kalman::kMatIdxDensity]   = HMaterials::getDensity(id);
    mat[Kalman::kMatIdxExEner]    = HMaterials::getExciteEner(id);
    mat[Kalman::kMatIdxRadLength] = HMaterials::getRadLength(id);

    Float_t corrRho = 1.e-3; // g/cm^3 -> g/mm^3
    Float_t corrI   = 1.e-6; // eV -> MeV
    Float_t corrX0  = 10.F;  // cm -> mm

    mat[Kalman::kMatIdxDensity]   *= corrRho;
    mat[Kalman::kMatIdxExEner]    *= corrI;
    mat[Kalman::kMatIdxRadLength] *= corrX0;
}


void HKalDetCradle::install(HKalMdcMeasLayer *layer) {
    // Adds a new mdc measurement to the array.

    Add(layer);
}

