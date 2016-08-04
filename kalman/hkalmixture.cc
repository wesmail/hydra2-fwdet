
// from ROOT
#include "TMath.h"
#include "TString.h"

// from hydra
#include "hkalmixture.h"
#include "hkaldef.h"
#include "htool.h"

#include <iostream>
#include <iomanip>
using namespace std;

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This class manages the following properties for a mixture of materials:
// mass number A, atomic number Z, density, radiation length and excitation
// energy.
// First, the various components of the mixture must be added using
// defineElements().
// After that, calling calcMixture() will calculate the properties of the
// mixture.
//
///////////////////////////////////////////////////////////////////////////////


//  -----------------------------------
//  Ctors and Dtor
//  -----------------------------------

HKalMixture::HKalMixture() : TMixture() {
    fDensityMixt    = NULL;
    fExciteEnerMixt = NULL;
    fRadLengthMixt  = NULL;
}

HKalMixture::HKalMixture(const char *name, const char *title, Int_t nmixt)
: TMixture(name, title, nmixt) {
    // Make a mixture that consists of nmixt different materials.
    // To add materials use the defineElement() method. After all materials
    // have been added, the atomic and nuclear properties of the mixture have
    // to be calculated with the calcMean() function.
    //
    // Input:
    // name:   material name.
    // title:  title.
    // nmixt:  number of materials the mixture contains.

    if (nmixt == 0) {
        fDensityMixt    = NULL;
        fExciteEnerMixt = NULL;
        fRadLengthMixt  = NULL;
        return;
    }

    Int_t nm = TMath::Abs(nmixt);
    fDensityMixt    = new Float_t[nm];
    fExciteEnerMixt = new Float_t[nm];
    fRadLengthMixt  = new Float_t[nm];

    // Make sure weights are initialized to 0 in case less than GetNmixt()
    // components are added.
    for(Int_t i = 0; i < GetNmixt(); i++) {
	fWmixt[i] = 0.;
    }
}

HKalMixture::~HKalMixture() {
    delete [] fDensityMixt;
    delete [] fExciteEnerMixt;
    delete [] fRadLengthMixt;
    fDensityMixt    = NULL;
    fExciteEnerMixt = NULL;
    fRadLengthMixt  = NULL;
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

void HKalMixture::calcMassFracFromVolFrac(Float_t w[], Int_t n, const Float_t rho[], const Float_t vol[]) {
    // This function will calculate the mass fractions of a mixture given the components' densities and
    // volume fractions. Only works for ideal mixtures.
    //
    // Output:
    // w: the mass weights.
    //
    // Input:
    // n: the number of components in the mixture. All arrays must have at least n elements.
    // rho: the densities of the components
    // vol: the volume fractions

    Float_t rhoMixt = 0;
    // Density of mixture.
    for(Int_t i = 0; i < n; i++) {
        rhoMixt += rho[i] * vol[i];
    }
    // Calculate mass fraction in mixture from volume fractions.
    // Formula from Geant manual
    for(Int_t i = 0; i < n; i++) {
        w[i] = vol[i] * rho[i] / rhoMixt;
    }
}


void HKalMixture::calcMixture() {
    // Calculates properties of the mixture:
    // atomic mass A, atomic number Z, density, radiation length and mean excitation energy.
    // Call this after adding the mixture's components with the defineElements() function.

#if kalDebug > 0
    Double_t w = 0.;
    for(Int_t i = 0; i < GetNmixt(); i++) {
	w += fWmixt[i];
    }
    if(w > (1. + 1.e-3)) {
	Warning("calcMixture()", Form("Weights of all components do not sum up to 1. Sum of weights = %f.", w));
    }
#endif

    fA          = 0;
    fExciteEner = 0.;
    fDensity    = 0;
    fRadLength  = 0;
    fZ          = 0;
    Float_t ZA  = 0.;
    Float_t lnExEnerWeight = 0.;

    // Calculating density of a compound:
    // Effective A, Z and density are done as in Geant 3.16, chapter CONS110.
    // The mean excitation energy is calculated using Bragg's rule:
    // S.M. Seltzer and M.J. Berger, Int. J. of Applied Rad. 33, 1189 (1982).
    //
    // rho = sum(m_i)   / V                   with V = sum(V_i) = sum(m_i/rho_i)
    //     = sum(m_i)   / sum(m_i/rho_i)      m_i = p_i * M
    //     = sum(p_i*M) / sum(p_i*M/rho_i)
    //     = sum(p_i)*M / (sum(p_i/rho_i)*M)
    //     = sum(p_i)   / sum(p_i/rho_i)      sum(pi) = 1
    //     = 1 / (sum(p_i/rho_i)
    // p_i = m_i/M  mass fraction of component i

    for(Int_t i = 0; i < GetNmixt(); i++) {
        if(fWmixt[i] != 0.) {
            fA             += fWmixt[i] * fAmixt[i];
            fZ             += fWmixt[i] * fZmixt[i];
            // Add weighted radiation lengths of components in 1/(g/mm^2)
            fRadLength     += fWmixt[i] / (fDensityMixt[i] * fRadLengthMixt[i]);
            fDensity       += fWmixt[i] / fDensityMixt[i];
            ZA             += fWmixt[i] * fZmixt[i] / fAmixt[i];
            lnExEnerWeight += TMath::Log(fExciteEnerMixt[i]) * fWmixt[i] * fZmixt[i] / fAmixt[i];
        }
    }
    if(fDensity != 0.) {
        fDensity = 1. / fDensity;
    }
    if(fRadLength != 0.) {
        //fRadLength *= fDensity; // total radiation length in mm
        fRadLength = 1. / (fDensity * fRadLength);
    }
    fExciteEner = TMath::Exp(lnExEnerWeight/ZA);
}

void HKalMixture::defineElement(Int_t n, const Float_t mat[5], Float_t w) {
    // Add a component to the mixture.
    //
    // Input:
    // n:   index of the component.
    // mat: array with material properties. Index of a material propertie is defined in
    //      the enum matIdx in hkaldef.h.
    // w:   mass fraction of this component in the mixture.

    defineElement(n, mat[Kalman::kMatIdxA], mat[Kalman::kMatIdxZ], mat[Kalman::kMatIdxDensity],
		  mat[Kalman::kMatIdxExEner], mat[Kalman::kMatIdxRadLength], w);
}

void HKalMixture::defineElement(Int_t n, Float_t a, Float_t z, Float_t rho, Float_t i,
				Float_t x0, Float_t w) {
    // Add a component to the mixture.
    //
    // Input:
    // n:   index of the component.
    // a:   atomic mass.
    // z:   atomic number.
    // rho: density.
    // I:   excitation energy.
    // x0:  radiation length.
    // w:   mass fraction of this component in the mixture.

    if (n < 0) {
        Error("defineElement()", Form("Invalid element index %i.", n));
        return;
    }

    // Need to resize arrays with material properties.
    if((n+1) > GetNmixt()) {
        Int_t fNmixtNew = n + 1;
        Float_t *zmixture   = new Float_t[fNmixtNew];
        Float_t *amixture   = new Float_t[fNmixtNew];
        Float_t *rhomixture = new Float_t[fNmixtNew];
        Float_t *imixture   = new Float_t[fNmixtNew];
        Float_t *x0mixture  = new Float_t[fNmixtNew];
        Float_t *weights    = new Float_t[fNmixtNew];

        // Copy old values to arrays of correct size.
        for (Int_t j = 0; j < GetNmixt(); j++) {
            zmixture  [j] = fZmixt[j];
            amixture  [j] = fAmixt[j];
            rhomixture[j] = fDensityMixt[j];
            imixture  [j] = fExciteEnerMixt[j];
            x0mixture [j] = fRadLengthMixt[j];
            weights   [j] = fWmixt[j];
        }

        for(Int_t j = GetNmixt()+1; j < fNmixtNew; j++) {
            weights[j] = 0.;
        }

        // Replace the old arrays with new ones of correct size.
        delete [] fZmixt;
        delete [] fAmixt;
        delete [] fDensityMixt;
        delete [] fExciteEnerMixt;
        delete [] fRadLengthMixt;
        delete [] fWmixt;

        fZmixt          = zmixture;
        fAmixt          = amixture;
        fDensityMixt    = rhomixture;
        fExciteEnerMixt = imixture;
        fRadLengthMixt  = x0mixture;
        fWmixt          = weights;

        fNmixt = fNmixtNew;
    }

    DefineElement(n, a, z, w); // parent's method
    fDensityMixt   [n] = rho;
    fExciteEnerMixt[n] = i;
    fRadLengthMixt [n] = x0;
}

void HKalMixture::print(const char *opt) const {
    // opt = "mix":      Print properties the mixture.
    // opt = "elements": Print properties of all the mixture's components.

    cout<<"**** Print content of material "<<GetName()<<" ****"<<endl;
    TString stropt(opt);
    if(stropt.Contains("mix", TString::kIgnoreCase) || stropt.IsNull()) {
        cout<<"Mixture properties:"<<endl;
        cout<<"A                      = "<<GetA()<<endl;
        cout<<"Z                      = "<<GetZ()<<endl;
        cout<<"density                = "<<GetDensity()<<endl;
        cout<<"mean excitation energy = "<<GetExciteEner()<<endl;
        cout<<"radiation length       = "<<GetRadLength()<<endl;
    }
    if(stropt.Contains("elements", TString::kIgnoreCase)) {
        for(Int_t i = 0; i < GetNmixt(); i++) {
            cout<<"Properties of material "<<i<<" in mixture:"<<endl;
            cout<<"A                      = "<<setprecision(3)<<fAmixt[i]<<endl;
            cout<<"Z                      = "<<setprecision(3)<<fZmixt[i]<<endl;
            cout<<"density                = "<<setprecision(8)<<fDensityMixt[i]<<endl;
            cout<<"radiation length       = "<<setprecision(8)<<fRadLengthMixt[i]<<endl;
            cout<<"mean excitation energy = "<<setprecision(8)<<fExciteEnerMixt[i]<<endl;
            cout<<"material weight        = "<<setprecision(3)<<fWmixt[i]<<endl;
        }
    }
    cout << resetiosflags(ios::showpoint) << endl;

    cout<<"**** End print of mixture ****"<<endl;
}
