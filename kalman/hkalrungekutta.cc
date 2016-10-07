
// from ROOT
#include "TMath.h"
#include "TVector3.h"

// from hydra
#include "hphysicsconstants.h"
#include "hkalgeomtools.h"
#include "hkalmdcmeaslayer.h"
#include "hkalrungekutta.h"
#include "hkaltrackstate.h"

#include <iostream>
#include <fstream>
using namespace std;

ClassImp(HKalRungeKutta)

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////////////////////
//
// This is the track propagator for the Kalman filter. Propagation is done
// with the Runge-Kutta method. Track state updates are done like in CBM, also see:
// S. Gorbunov and I. Kisel,
// An analytic formula for track extrapolation in an inhomogeneous magnetic field.
// CBM-SOFT-note-2005-001, 18 March 2005
// http://www-linux.gsi.de/~ikisel/reco/CBM/DOC-2005-Mar-212-1.pdf
//
// Before doing track propagation, a pointer to the magnetic field map must
// be set using the function
// setFieldMap(HMdcTrackGField *fpField, Double_t fpol).
//
// The main function is propagateToPlane() that will propagate the track
// to a target plane and calculate the track state vector at that plane and
// the propagator matrix. It will return a boolean indicating whether
// problems were encountered during the Runge-Kutta stepping, like for example
// unrealistically hight track state parameters.
//
// All the points and B-field vectors may be stored in two arrays for
// debugging purposes or graphics output if that option has been enabled with
// setFillPointsArrays(kTRUE).
// The functions getPointsTrack() and getFieldTrack() will return these arrays.
// Each array element is a TVector3 object.
//
// The class also offers functions to calculate the material budget in each
// step (calcMaterialProperties()), the effects of multiple scattering
// (calcProcNoise()) and energy loss (calcEnergyLoss()).
//
//-----------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////////


HKalRungeKutta::HKalRungeKutta() : TObject(), magField(0.,0.,0.) {
    // Constructor that sets variables. It is still neccessary to set the field
    // map pointer with
    // setFieldMap(HMdcTrackGField *fpField, Double_t fpol)
    // before Runge-Kutta propation can be used.

    //? Eventually these parameters should be stored in a parameter container!
    initialStepSize = 50.;
    stepSizeDec     = 0.75;
    stepSizeInc     = 1.25;
    maxStepSize     = 200.;
    minStepSize     = 2.;
    minPrecision    = 2.e-04;
    maxPrecision    = 2.e-05;
    maxNumSteps     = 1000;
    maxNumStepsPCA  = 10;
    maxDist         = .1;

    minLengthCalcQ  = 0.01;

    maxTan     = 6.; // tan(6) <=> 80 degrees
    maxPos     = 5.e4;
    maxPropMat = 1.e6;
    minMom     = 20.;

    bFillPointArrays = kFALSE;
    bRotBfield       = kFALSE;
    bPrintErr        = kTRUE;
    bPrintWarn       = kTRUE;
    bElossErr        = kFALSE;
    bPrintElossErr   = kTRUE;
    bConstField      = kFALSE;
    bDoMS            = kTRUE;
    bDoDEDX          = kTRUE;
}

HKalRungeKutta::~HKalRungeKutta() {
}

//  -----------------------------------
//  Implementation of public methods
//  -----------------------------------

Double_t HKalRungeKutta::calcEnergyLoss(TMatrixD &fProc, TVectorD &stateVec, Double_t length, Double_t qp,
                                        Double_t Z, Double_t ZoverA, Double_t density, Double_t radLength,
                                        Double_t exciteEner, Int_t pid) {
    // Calculates energy loss due to bremsstrahlung and ionization.
    // Radiation loss is done for electrons/positrons only.
    // Only yields proper results if particle charge is +/- 1.
    // Return value is energy loss in MeV/particle charge.
    //
    // Input and output:
    // fProc:      Process noise matrix. Contribution of energy loss will be added.
    // stateVec:   track state vector before energy loss. q/p of state will be modified.
    //
    // Input:
    // length:     track length in mm
    // qp:         charge/momentum before energy loss in MeV/c
    // Z:          atomic number.
    // ZoverA:     atomic number / atomic mass of passed material
    // density:    density of passed material in g/mm^3
    // radLength:  radiation length of passed material in mm
    // exciteEner: mean excitation energy of passed material in MeV
    // pid:        Geant particle ID.

#ifdef kalDebug
    Int_t rows = fProc.GetNrows();
    Int_t cols = fProc.GetNcols();
    Int_t sdim = stateVec.GetNrows();
    if(rows < sdim || cols < sdim) {
        if(bPrintErr) {
            Error("calcEnergyLoss()", Form("Process noise matrix is %ix%i, but must be at least %ix%i matrix.", rows, cols, sdim, sdim));
        }
        return 0.;
    }
    rows = stateVec.GetNrows();
    if(rows < sdim) {
        if(bPrintErr) {
            Error("calcEnergyLoss()", Form("State vector has dimension of %i, but must have at least %ix.", cols, sdim));
        }
        return 0.;
    }
#endif

    // Energy loss due to radiation and ionization. Let
    // E' = particle energy including energy loss due to radiation and ionization.
    // E  = particle energy without energy loss.
    // delta(E) = energy loss
    //
    // E' = E - delta(E)
    //    = E * (1 - delta(E)/E)
    // E'/E = 1 - delta(E)/E

    Bool_t bIsLepton = (pid == HPhysicsConstants::pid("e+") || pid == HPhysicsConstants::pid("e-"));

    Double_t mass = HPhysicsConstants::mass(pid);
    Double_t q    = (Double_t)HPhysicsConstants::charge(pid);
    Double_t p    = q / qp;
    Double_t beta = 1. / TMath::Sqrt(1. + mass*mass / (p*p));

    Double_t ElossRad = 0.;
    Double_t ElossIon = 0.;

    // Particle is an electron or positron.
    if(bIsLepton) {
        // Radiation loss for electrons/positrons.
        ElossRad = calcRadLoss(fProc, length, mass, qp, radLength);
        if(direction == kIterBackward) {
            ElossRad *= -1.;
        }

        // Critical energy for gases:
        // E_c = 710 MeV / (Z + 0.92)
        // From "Passage of particles through matter", Particle Data Group, 2009
        Double_t eCritical = 710. / (Z + 0.92);
        if(p < eCritical) {
            ElossIon = length * calcDEDXIonLepton(qp, ZoverA, density, exciteEner, pid);
        }
    } else { // Energy loss for heavy particles.
        // Energy loss due to ionization.
        ElossIon = length * calcDEDXBetheBloch(beta, mass, ZoverA, density, exciteEner, q);
    }
    if(direction == kIterBackward) {
        ElossIon *= -1.;
    }

    Double_t Eloss = ElossRad + ElossIon; // delta(E)

    // Correction of particle energy is:
    // E' = E + delta(E)
    //    = E * (1 + delta(E)/E)
    if(bIsLepton) {
        // For electrons: E ~ p
        // p' = p * (1 + delta(E)/p)
        // Track state parameter change is:
        // q/p' = q/p / (1 + delta(E)/p)
        stateVec(kIdxQP) = qp / (1. +  Eloss / p);
    } else {
        // E' = E + delta(E)
        // E'^2 = E^2 + 2*E*delta(E) + delta(E)^2
        // p'^2 + m^2 = p^2 + m^2 + 2*E*delta(E) + delta(E)^2
        // p'^2 = p^2 + 2*E*delta(E) + delta(E)^2
        Double_t p2 = p * p;
	Double_t E = TMath::Sqrt(p2 + mass*mass);
        Double_t pnew = TMath::Sqrt(p2 + 2.*E*Eloss + Eloss*Eloss);
        if(pnew > 0.) {
            stateVec(kIdxQP) = q / pnew;
        } else {
            if(bPrintErr && bPrintElossErr) { // Print only once.
                Error("calcEnergyLoss()", "Calculated energy loss is higher than particle momentum.");
            }
            Eloss = 0.;
            bElossErr      = kTRUE;
            bPrintElossErr = kFALSE;
        }
    }

    return Eloss;
}


Double_t HKalRungeKutta::calcDEDXBetheBloch(Double_t beta, Double_t mass, Double_t ZoverA,
                                            Double_t density, Double_t exciteEner, Double_t z) {
    // Returns the ionization energy loss for thin materials with Bethe-Bloch formula.
    // - dE/dx = K/A * Z * z^2 / beta^2 * (0.5 * ln(2*me*beta^2*gamma^2*T_max/I^2) - beta^2)
    // T_max = (2*me*beta^2*gamma^2) / (1 + 2*gamma*me/mass + me^2/mass^2)
    //
    // beta:       v/c of particle
    // mass:       mass of the particle in MeV/c^2 traversing through the material
    // ZoverA:     atomic number / atomic mass of passed material
    // density:    density of passed material in g/mm^3
    // exciteEner: mean excitation energy of passed material in MeV
    // z:          atomic number of particle

    Double_t me         = HPhysicsConstants::mass("e-"); // electron mass in MeV/c^2
    Double_t K          = 0.307075 * 100.;               // in MeV*mm^2/g for A = 1 g/mol

    Double_t Krho       = K * density;
    Double_t beta2      = beta*beta;
    Double_t gamma      = 1./TMath::Sqrt(1 - beta2);
    Double_t betagamma  = beta * gamma;
    Double_t betagamma2 = betagamma * betagamma;

    if(betagamma < 0.1 || betagamma > 1000.) {
        if(bPrintErr && bPrintElossErr) { // Print only once.
            Error("calcDXDXBetheBloch()",
                  Form("beta*gamma of particle is %f. Bethe-Bloch formula is only good for values between 0.1 and 1000.", betagamma));
        }
        bElossErr      = kTRUE;
        bPrintElossErr = kFALSE;
        return 0.;
    }
    // Maximum kinetic energy that can be imparted on a free electron in a single collision.
    Double_t tmax = (2. * me * betagamma2) / (1 + 2*gamma*me/mass + (me*me)/(mass*mass));

    return (-((Krho * z*z * ZoverA) / beta2)
            * (0.5 * TMath::Log((2. * me * betagamma2 * tmax)/(exciteEner*exciteEner)) - beta2));
}

Double_t HKalRungeKutta::calcDEDXIonLepton(Double_t qp, Double_t ZoverA, Double_t density,
                                           Double_t exciteEner, Int_t pid) const {
    // Calculates energy loss dE/dx in MeV/mm due to ionization for relativistic electrons/positrons.
    //
    // For formula used, see:
    // Track fitting with energy loss
    // Stampfer, Regler and Fruehwirth
    // Computer Physics Communication 79 (1994), 157-164
    //
    // qp:         particle charge divided by momentum
    // ZoverA:     atomic number / atomic mass of passed material
    // density:    density of material in g/mm^3
    // exciteEner: mean excitation energy in MeV
    // pid:        Geant particle id (2 = positron, 3 = electron)

    if(!(pid == HPhysicsConstants::pid("e+") || pid == HPhysicsConstants::pid("e-"))) {
        Warning("calcDEDXIonLepton()", Form("Can only use this function for electrons or positrons. Pid is %i.", pid));
        return 0.;
    }

    Double_t de       = 5.0989 * 1.e-23;                 // 4*pi*re*me*c^2 in MeV * mm^2 with re = classical electron radius
    Double_t avogadro = TMath::Na();                     // Avogadro constant in 1/mol
    Double_t ne       = ZoverA * avogadro * density;     // electron density
    Double_t me       = HPhysicsConstants::mass("e-");   // electron mass in MeV/c^2
    // gamma = E/m
    Double_t E        = 1. / TMath::Abs(qp);             // Energy for relativistic electrons.
    Double_t gamma    = E / me;                          // Relativistic gamma-factor.

    // Formula is slightly different for electrons and positrons.
    // Factors for positrons:
    Double_t gammaFac = 4.;
    Double_t corr     = 2.;
    if(pid == HPhysicsConstants::pid("e-")) { // particle is electron
        gammaFac = 3.;
        corr     = 1.95;
    }

    Double_t dedx = 0.5 * de * ne * (2 * TMath::Log(2*me/exciteEner) + gammaFac * TMath::Log(gamma) - corr);
    return (- dedx);
}

Double_t HKalRungeKutta::calcRadLoss(TMatrixD &fProc, Double_t length, Double_t mass, Double_t qp,
                                     Double_t radLength) const {
    // Calculates energy loss due to bremsstrahlung for electrons with Bethe-Heitler formula.
    // Sign will be negative if propagating in forward direction, i.e. particle looses energy.
    //
    // Input and Output:
    // fProc:     process noise matrix (will be modified)
    //
    // Input:
    // length:    track length in mm
    // mass:      particle mass in MeV/c^2
    // qp:        charge/momentum before energy loss in MeV/c
    // radLength: radiation length of material in mm.

    if(mass != HPhysicsConstants::mass("e-")) {
        if(bPrintWarn) {
            Warning("calcRadLoss()", Form("Particle with mass is %f MeV not an electron/positron.", mass));
        }
        return 0.;
    }

    Double_t t = length / radLength; // t := l/xr. Track length in units of radiation length.

    // Bethe and Heitler formula:
    // - dE/dx = E / xr
    // Integrating yields:
    // E' = E * exp(-t)
    // E'/E = exp(-t)
    // with t := l/xr
    // l = track length
    // xr = radiation length

    // delta(E) = E' - E
    //          = E * (E'/E - 1)
    //          = (E'/E - 1) / qp since E ~ p for electrons
    Double_t ElossRad = (TMath::Exp(-t) - 1.) / TMath::Abs(qp);

    // The variance of E'/E as done in:
    // D. Stampfer, M. Regler and R. Fruehwirth,
    // Track fitting with energy loss.
    // Comp. Phys. Commun. 79 (1994) 157-164
    // http://www-linux.gsi.de/~ikisel/reco/Methods/Fruehwirth-FittingWithEnergyLoss-CPC79-1994.pdf
    Double_t varElossRadFac = TMath::Exp(- t * TMath::Log(3.) / TMath::Log(2.)) - TMath::Exp(-2. * t);

    // Update process noise.
    fProc(kIdxQP, kIdxQP) += qp * qp * varElossRadFac;

    return ElossRad;
}


TVector3 HKalRungeKutta::calcFieldIntegral() const {
    // Calculates the field integral (Tm) by numerical integration
    // along the stepping points :  | sum over i  x(i+1)-x(i) x B(i) |
    // where x and B are position and field vectors.
    // setFillPointArrays(kTRUE) has to set before call propagate
    // to fill the point and field arrays which are needed for
    // the calculation.

    TVector3 bdl;
    if(!bFillPointArrays) {
        Warning("calcFieldIntegral()", "Stepping point arrays not filled. Cannot calculate field integral.");
        return bdl;
    }
    Int_t n = pointsTrack.GetEntries() - 1;
    for(Int_t i = 0; i < n ; i++){
        TVector3& x1 = (*(TVector3*)pointsTrack.At(i    ));
        TVector3& x2 = (*(TVector3*)pointsTrack.At(i + 1));
        TVector3& B  = (*(TVector3*)fieldTrack .At(i    ));
        TVector3 l   = (x2 - x1);
        bdl += l.Cross(B);
    }
    return bdl * 0.001; // mm -> m
}

void HKalRungeKutta::calcField_mm(const TVector3& xv, TVector3& btos, Double_t fpol) const {
    // Calculate B-field (T) in x,y,z (mm).
    //
    // Output:
    // btos: field vector at position xv in Tesla.
    //
    // Input:
    // xv:   return field at this position vector. Coordinates must be in mm.
    // fpol: field scaling factor * polarity.

    if(bConstField) {
        btos.SetXYZ(magField.X(), magField.Y(), magField.Z());

    } else {
        static Double_t abtos[3];
        static Double_t axv  [3];
        axv[0] = xv.X() * 0.1;  // convert to cm
        axv[1] = xv.Y() * 0.1;
        axv[2] = xv.Z() * 0.1;

#ifdef kalDebug
        if(!fieldMap) {
            Error("calcField_mm()", "Pointer to field map not set.");
            exit(1);
        }
#endif

        fieldMap->calcField(axv,abtos,fpol);

        btos.SetX(abtos[0]);
        btos.SetY(abtos[1]);
	btos.SetZ(abtos[2]);
    }
}

void HKalRungeKutta::calcField_mm(Double_t* xv, Double_t* btos, Double_t fpol) const {
    // Calculate B-field (T) in x,y,z (mm).
    //
    // Output:
    // btos: field vector at position xv in Tesla.
    //
    // Input:
    // xv:   return field at this position vector. Coordinates must be in mm.
    // fpol: field scaling factor * polarity.

    if(bConstField) {
        btos[0] = magField.X();
        btos[1] = magField.Y();
        btos[2] = magField.Z();
    } else {
        static Double_t axv  [3];
        axv[0] = xv[0] * 0.1;  // convert to cm
        axv[1] = xv[1] * 0.1;
        axv[2] = xv[2] * 0.1;

#ifdef kalDebug
        if(!fieldMap) {
            Error("calcField_mm()", "Pointer to field map not set.");
            exit(1);
        }
#endif

        fieldMap->calcField(axv,btos,fpol);
    }
}

Bool_t HKalRungeKutta::calcMaterialProperties(Double_t &A, Double_t &Z, Double_t &density, Double_t &radLength, Double_t &exciteEner,
                                              const TVector3 &posFrom, const TVector3 &posTo,
                                              const HKalMdcMeasLayer &layerFrom, const HKalMdcMeasLayer &layerTo) const {
    // Calculates material budget for a Runge-Kutta step between two points.
    //
    // Output:
    // A:          mass number of mixture of passed material.
    // Z:          atomic number of mixture of passed material.
    // density:    density of mixture of passed material in g/mm^3.
    // radLength:  radiation length of mixture of passed material in mm.
    // exciteEner: mean excitation energy of mixture of passed material in MeV.
    //
    // Input:
    // posFrom:    position of track before the Runge-Kutta step.
    // posTo:      position of track after the Runge-Kutta step.
    // layerFrom:  measurement layer at posFrom.
    // layerTo:    measurement layer at posTo.

#if rkDebug > 3
    cout<<"Calculating material properties."<<endl;
    cout<<"Track is between mdc sec "<<layerFrom.getSector()<<" / mod "<<layerFrom.getModule()<<" / lay "<<layerFrom.getLayer()
        <<" and mdc sec "<<layerTo.getSector()  <<" / "<<layerTo.getModule()  <<" / "<<layerTo.getLayer()  <<endl;
#endif

    Int_t dirsign = 1.;
    if(direction == kIterBackward) {
        dirsign = -1.;
    }

    // Track direction.
    TVector3 dir(dirsign * (posTo - posFrom).Unit());

    // Track length.
    Double_t length = (posTo - posFrom).Mag();

    // Cosines of intersection angles of direction with the planes.
    Double_t angleFromStart = TMath::Cos(dir.Angle(layerFrom.getNormal()));
    Double_t angleToEnd     = TMath::Cos(dir.Angle(layerTo.getNormal()));

    // Distance of the start point of the RK step to its closest mdc layer.
    Double_t distFromStart = dirsign * layerFrom.signedDistanceToPlane(posFrom) / angleFromStart;
    // Distance of the end point of the RK step to its closest mdc layer.
    Double_t distToEnd     = dirsign * layerTo  .signedDistanceToPlane(posTo)   / angleToEnd;

    // Distance from the cathode layer (located in the midle of the layer
    // to the boundary of this mdc module.
    Double_t distFromModBound = layerFrom.getDistToModBound( direction) / angleFromStart;
    Double_t distToModBound   = layerTo  .getDistToModBound(!direction) / angleToEnd;

    // Calculate fractions of the track where the particle has been inside
    // and outside the layers.
    Double_t fracInsideFrom = 0.;
    Double_t fracInsideTo   = 0.;

    // Track stayed in the same module.
    if(layerFrom.getModule() == layerTo.getModule()) {
        fracInsideFrom = 1.;
        fracInsideTo   = 0.;
#if rkDebug > 3
        cout<<"Track in the same module."<<endl;
#endif
    } else {
        if(layerFrom.isInsideMod(posFrom)) {
            // Track stayed inside an mdc.
            if(layerFrom.isInsideMod(posTo)) {
                fracInsideFrom = 1.;
#if rkDebug > 3
                cout<<"Start and end points are inside the source layer."<<endl;
#endif
            } else {
                // Track started inside an mdc and ended inside the next mdc.
                if(layerTo.isInsideMod(posTo)) {
                    fracInsideFrom = (distFromModBound - distFromStart) / length;
                    fracInsideTo   = (distToModBound   + distToEnd)  / length;
#if rkDebug > 3
                    cout<<"Start point inside source layer, end point inside target layer."<<endl;
#endif
                } else {
                    // Track started inside an mdc and ended outside of this mdc.
                    fracInsideFrom = (distFromModBound - distFromStart) / length;
#if rkDebug > 3
                    cout<<"Start point inside source layer, end point outside layer."<<endl;
                    cout<<"Distance to module boundary: "<<distFromModBound<<", distance track start point to middle of source layer: "<<distFromStart<<", track length: "<<length<<endl;
                    cout<<"sec/mod/lay layer from: "<<layerFrom.getSector()<<" / "<<layerFrom.getModule()<<" / "<<layerFrom.getLayer()<<endl;
                    cout<<"sec/mod/lay layer to  : "<<layerTo.getSector()  <<" / "<<layerTo.getModule()  <<" / "<<layerTo.getLayer()  <<endl;
#endif
                }
            }
        } else {
            if(layerTo.isInsideMod(posTo)) {
                // Track stayed in an mdc.
                if(layerTo.isInsideMod(posFrom)) {
                    fracInsideTo = 1.;
#if rkDebug > 3
                    cout<<"Start and end points lie inside target layer."<<endl;
#endif
                } else {
                    // Track started outside of an mdc and ended inside of an mdc.
                    fracInsideTo = (distToModBound + distToEnd) / length;
#if rkDebug > 3
                    cout<<"Start point outside and end point is inside target layer."<<fracInsideTo<<endl;
                    cout<<"Distance to module boundary: "<<distToModBound<<", distance track end point to middle of target layer: "<<distToEnd<<", track length: "<<length<<endl;
                    cout<<"sec/mod/lay layer from: "<<layerFrom.getSector()<<" / "<<layerFrom.getModule()<<" / "<<layerFrom.getLayer()<<endl;
                    cout<<"sec/mod/lay layer to  : "<<layerTo.getSector()  <<" / "<<layerTo.getModule()  <<" / "<<layerTo.getLayer()  <<endl;
#endif
                }
            }
        }
    }

    Double_t fracOutside = 1. - fracInsideTo - fracInsideFrom;

#if rkDebug > 3
    cout<<"Length fraction inside start layer "<<fracInsideFrom<<", inside end layer "<<fracInsideTo
        <<", outside both layers "<<fracOutside<<endl;
#endif

    Double_t prec = 1.e-5;
    if(TMath::Abs(fracInsideFrom + fracInsideTo + fracOutside ) > 1.F + prec
       || (fracInsideFrom + fracInsideTo + fracOutside < -prec)
       || fracInsideFrom > 1.F + prec || fracInsideTo > 1.F + prec || fracOutside > 1.F + prec
       || fracInsideFrom < -prec || fracInsideTo < -prec || fracOutside < -prec) {
        if(bPrintWarn) {
            Warning("calcMaterialProperties()", "Track length fractions do not sum up to 1.");
            Warning("", Form("Fraction inside source layer: %f, inside target layer: %f, outside layers: %f"
                             , fracInsideFrom, fracInsideTo, fracOutside));
        }
        // Attempt to approximate fractions in order to continue with valid numbers.
        // Round calculated fractions to nearest integers.
        if(TMath::Nint(fracInsideFrom) == 1) {
            fracOutside    = 0.;
            fracInsideFrom = 1.;
            fracInsideTo   = 0.;
        } else {
            if(TMath::Nint(fracInsideTo) == 1) {
                fracOutside    = 0.;
                fracInsideFrom = 0.;
                fracInsideTo   = 1.;
            } else {
                fracOutside    = 1.;
                fracInsideFrom = 0.;
                fracInsideTo   = 0.;
            }
        }
    }

    // Make a mixture of passed material.
    HKalMixture mat("Passed material for an RK step.", "rkstep", 3);
    Float_t w[3];    // mass fractions to be calculated
    Float_t vol[3];  // volume fractions
    vol[0] = fracInsideFrom;
    vol[1] = fracInsideTo;
    vol[2] = fracOutside;
    Float_t rho[3]; // densities of passed materials
    rho[0] = layerFrom.getDensity(1);
    rho[1] = layerTo.getDensity(1);
    rho[2] = layerTo.getDensity(0);
    mat.calcMassFracFromVolFrac(w, 3, rho, vol);

    // Calculate properties of material mixture.
    mat.defineElement(0, layerFrom.getA(1), layerFrom.getZ(1), layerFrom.getDensity(1), layerFrom.getExcitationEnergy(1), layerFrom.getRadLength(1), w[0]);
    mat.defineElement(1, layerTo  .getA(1), layerTo  .getZ(1), layerTo  .getDensity(1), layerTo  .getExcitationEnergy(1), layerTo  .getRadLength(1), w[1]);
    mat.defineElement(2, layerTo  .getA(0), layerTo  .getZ(0), layerTo  .getDensity(0), layerTo  .getExcitationEnergy(0), layerTo  .getRadLength(0), w[2]);
    mat.calcMixture();

    A          = mat.GetA();
    Z          = mat.GetZ();
    density    = mat.GetDensity();
    exciteEner = mat.GetExciteEner();
    radLength  = mat.GetRadLength();

#if rkDebug > 3
    cout<<"Properties of passed material: "<<endl;
    mat.print();
#endif

    return kTRUE;
}

void HKalRungeKutta::calcMultScat(TMatrixD &fProc, const TVectorD &stateVec,
                                  Double_t length, Double_t radLength, Double_t beta, Int_t pid) const {
    // Add multiple scattering to the process noise covariance.
    //
    // Input and output:
    // fProc:     Process noise covariance. Contribution of multiple scattering is added to this matrix.
    //
    // Input:
    // stateVec:  Track state vector at start of an RK step.
    // length:    Track length in mm.
    // radLength: Radiation length of passed material in mm.
    // beta:      v/c of particle.
    // pid:       Geant particle ID.

    Double_t tx = stateVec(kIdxTanPhi);
    Double_t ty = stateVec(kIdxTanTheta);
    Double_t t  = 1. + tx*tx + ty*ty;

    // 1/beta^2
    Double_t beta2Inv = 1. / (beta*beta);

    // 1/momentum^2
    Double_t mom2Inv  = TMath::Power(stateVec(kIdxQP) / (Double_t)HPhysicsConstants::charge(pid), 2);

    // Squared scatter angle cms2.
    // cms = 13.6 MeV / (beta * c * p) * sqrt(l/X0) * (1 + 0.038 * ln(l/X0))
    // with l/X0 = length of particle track in units of radiation length.
    Double_t lx0 = length / radLength;
    Double_t cms2 = (13.6 * 13.6 * beta2Inv * mom2Inv * lx0 * TMath::Power((1 + .038 * TMath::Log(lx0)),2));

    // Update process noise.
    fProc(kIdxTanPhi  , kIdxTanPhi)   += (1 + tx*tx) * t * cms2;
    fProc(kIdxTanTheta, kIdxTanTheta) += (1 + ty*ty) * t * cms2;
    fProc(kIdxTanPhi  , kIdxTanTheta) += tx * ty     * t * cms2;
    fProc(kIdxTanTheta, kIdxTanPhi)    = fProc(kIdxTanPhi, kIdxTanTheta); // matrix is symmetric
}

void HKalRungeKutta::clear() {
    // Resets error flags and clears arrays that store points/field along the trajectory.
    // Call this every time when fitting a new track.

    pointsTrack.Clear();
    fieldTrack.Clear();

    trackOk        = kTRUE;
    bElossErr      = kFALSE;
    bPrintElossErr = kTRUE;
}

Bool_t HKalRungeKutta::checkPropMat(TMatrixD &fProp) const {
    // Check propagator matrix for INFs/NaNs or too large entries.

    Bool_t matOk = kTRUE;
    TMatrixD fPropIn = fProp;

    for(Int_t iRow = 0; iRow < fProp.GetNrows(); iRow++) {
        for(Int_t iCol = 0; iCol < fProp.GetNcols(); iCol++) {
            Double_t elem = fProp(iRow, iCol);
            if(TMath::Abs(elem) > maxPropMat || elem == numeric_limits<Double_t>::infinity() || TMath::IsNaN(elem)) {
                if(elem >= 0.) {
                    fProp(iRow, iCol) = maxPropMat;
                } else {
                    fProp(iRow, iCol) = - maxPropMat;
                }
                matOk = kFALSE;
            }
        }
    }

    if(!matOk && bPrintWarn) {
        Warning("checkPropMat()", "Some elements in propagator matrix are too large or invalid.");
        fPropIn.Print();
    }

    return matOk;
}

Bool_t HKalRungeKutta::checkTrack(TVectorD &stateVec) const {
    // Reduces values in the state vector that have become too large to avoid
    // overflows or underflows and print a warning.
    // Returns whether track parameters have been reduced or not as a boolean.
    //
    // sv: 5-dimensional track state vector. May be modified in case of errors.

    Bool_t noerr = kTRUE;
    TVectorD stateVecIn = stateVec;

    if(TMath::IsNaN(stateVec(kIdxX0))) {
        if(bPrintErr) {
            Error("checkTrack()", "X-position in state vector is a NaN.");
        }
        stateVec(kIdxX0) = maxPos;
        noerr = kFALSE;
    }
    if(TMath::IsNaN(stateVec(kIdxY0))) {
        if(bPrintErr) {
            Error("checkTrack()", "Y-position in state vector is a NaN.");
        }
        stateVec(kIdxY0) = maxPos;
        noerr = kFALSE;
    }
    if(TMath::IsNaN(stateVec(kIdxTanPhi))) {
        if(bPrintErr) {
            Error("checkTrack()", "State parameter tx is a NaN.");
        }
        stateVec(kIdxTanPhi) = maxTan;
        noerr = kFALSE;
    }
    if(TMath::IsNaN(stateVec(kIdxTanTheta))) {
        if(bPrintErr) {
            Error("checkTrack()", "State parameter ty is a NaN.");
        }
        stateVec(kIdxTanTheta) = maxTan;
        noerr = kFALSE;
    }
    if(TMath::IsNaN(stateVec(kIdxQP))) {
        if(bPrintErr) {
            Error("checkTrack()", "State parameter q/p is a NaN.");
        }
        stateVec(kIdxQP) = minMom;
        noerr = kFALSE;
    }

    if(stateVec(kIdxTanPhi) > maxTan) {
        stateVec(kIdxTanPhi) = maxTan;
        noerr = kFALSE;
    }
    if(stateVec(kIdxTanTheta) > maxTan) {
        stateVec(kIdxTanTheta) = maxTan;
        noerr = kFALSE;
    }
    if(stateVec(kIdxTanPhi) < -maxTan) {
        stateVec(kIdxTanPhi) = -maxTan;
        noerr = kFALSE;
    }
    if(stateVec(kIdxTanTheta) < -maxTan) {
        stateVec(kIdxTanTheta) = -maxTan;
        noerr = kFALSE;
    }
    if(stateVec(kIdxX0) > maxPos) {
        stateVec(kIdxX0) = maxPos;
        noerr = kFALSE;
    }
    if(stateVec(kIdxX0) < -maxPos) {
        stateVec(kIdxX0) = -maxPos;
        noerr = kFALSE;
    }
    if(stateVec(kIdxY0) > maxPos) {
        stateVec(kIdxY0) = maxPos;
        noerr = kFALSE;
    }
    if(stateVec(kIdxY0) < -maxPos) {
        stateVec(kIdxY0) = -maxPos;
        noerr = kFALSE;
    }

    if(!noerr) {
        if(bPrintWarn) {
            Warning("checkTrack()", "Track parameters are too large/small or invalid.");
        }
    }

    if(TMath::Abs(1/stateVec(kIdxQP)) < minMom) {
        if(stateVec(kIdxQP) > 0.) {
            stateVec(kIdxQP) =   1. / minMom;
        } else {
            stateVec(kIdxQP) = - 1. / minMom;
        }
        noerr = kFALSE;
        if(bPrintWarn) {
            Warning("checkTrack()", Form("Track momentum smaller than %f MeV", minMom));
        }
    }

    if(!noerr && bPrintWarn) {
        stateVecIn.Print();
    }

    return noerr;
}

Bool_t HKalRungeKutta::propagateToPlane(TMatrixD &fProp, TMatrixD &fProc, TVectorD &stateVecTo,
                                        const TVectorD &stateVecFrom,
                                        const HKalPlane &planeFrom, const HKalPlane &planeTo,
                                        const HKalMdcMeasLayer &measLayFrom, const HKalMdcMeasLayer &measLayTo,
                                        Int_t pid, Bool_t propDir, Bool_t bCalcJac) {
    // Propagate a particle trajectory to a plane using a 4th order Runge-Kutta.
    // Returns true if no errors were encountered during track propagation.
    //
    // Output:
    // fProp:         Propagator matrix.
    // fProc:         Process noise matrix with contributions of multiple scattering and energy loss.
    // stateVecTo:    Track state vector (x,y,tx,ty,q/p) at target plane.
    //
    // Input:
    // stateVecFrom:  Track state vector at (x,y,tx,ty,q/p) at source plane.
    // planeFrom:     Plane to propagate from.
    // planeTo:       Plane to propagate to.
    // measLayerFrom: Detector element in which the track is currently in. Used to calculate passed materials.
    // measLayerTo:   Detector element the track will move to. Used to calculate passed materials.
    // pid:           Geant particle ID. Needed for process noise calculations.
    // propDir:       propagation direction kIterForward or kIterBackward.
    // bCalcJac:      Calculate the propagator matrix.

    bElossErr      = kFALSE;
    trackOk        = kTRUE;
    direction      = propDir; // forward/backward
    trackLength    = 0.;
    stepLength     = 0.;
    energyLoss     = 0.;
    jstep          = 0;

    Double_t mass  = HPhysicsConstants::mass(pid);

    stateVecTo     = stateVecFrom;

    fProp.UnitMatrix();
    fProc.Zero();

    TVectorD stateVecPreStep(stateVecTo.GetNrows());
    TVector3 posPreStep;
    TVector3 posAt;
    if(!HKalTrackState::calcPosAtPlane(posAt, planeFrom, stateVecFrom)) {
        // Should not happen unless plane is parallel to xy-plane.
        if(bPrintErr) {
            Error("propagateToPlane()", "Could not extract a position vector from track state.");
        }
        return kFALSE;
    }
    trackPosAtZ = posAt.z();

    Double_t sd = planeTo.signedDistanceToPlane(posAt);
    if((sd > 0. && propDir == kIterForward) || (sd < 0. && propDir == kIterBackward)) {
        if(TMath::Abs(sd) > 1.e-3) {
	    if(bPrintErr) {
		Error("propagateToPlane()", Form("Track was already past the target plane before propagation by %f.", sd));
	    }
            return kFALSE;
        } else {
	    if(bPrintWarn) {
		Warning("propagateToPlane()", Form("Track was already past the target plane before propagation by %f.", sd));
	    }
            return kTRUE;
        }
    }

    Double_t step     = initialStepSize;
    Double_t nextStep = step;
    Double_t stepFac  = 1.;
    Bool_t doNextStep = kTRUE;

    // Reduce step size if too close to the target plane.
    TVector3 dirAt;
    HKalTrackState::calcDir(dirAt, stateVecFrom);
    TVector3 pointIntersect;
    if(!planeTo.findIntersection(pointIntersect, posAt, dirAt)) {
        if(bPrintErr) {
            Error("propagateToPlane()", "Track is parallel to target plane.");
        }
        return kFALSE;
    }

    Double_t d = HKalGeomTools::distance2Points(posAt, pointIntersect);
    if(d < step) {
        step     = d;
        nextStep = step;
    }

    // Steps are taken in z-direction. Compensate for track inclination.
    Double_t stepz = step * dirAt.Z();

    TMatrixD DFstep(fProp.GetNrows(), fProp.GetNcols());

#if rkDebug > 1
    cout<<"***************************"<<endl;
    cout<<"**** START RUNGE-KUTTA ****"<<endl;
    cout<<"***************************"<<endl;
#endif

#if rkDebug > 2
    cout<<"Propagate from plane in sector "<<measLayFrom.getSector()<<", module "<<measLayFrom.getModule()<<", layer "<<measLayFrom.getLayer()
        <<" to plane in sector "<<measLayTo.getSector()<<", module "<<measLayTo.getModule()<<", layer "<<measLayTo.getLayer()<<endl;

    cout<<"RK input sv: "<<endl;
    stateVecFrom.Print();

    cout<<"distance to source meas layer "<<measLayFrom.signedDistanceToPlane(posAt)<<
        ", distance to target meas layer "<<measLayTo.signedDistanceToPlane(posAt)<<endl;
    cout<<"distance to source plane "     <<planeFrom.signedDistanceToPlane(posAt)<<
        ", distance to target plane "     <<planeTo.signedDistanceToPlane(posAt)<<endl;
#endif

    // --------------------------
    // Begin Runge-Kutta stepping.

    while (d >= maxDist && doNextStep && jstep < maxNumSteps && step) {

        // Store some properties before stepping.
        posPreStep.SetXYZ(stateVecTo(kIdxX0), stateVecTo(kIdxY0), trackPosAtZ);
        stateVecPreStep = stateVecTo;

        // Calculate step size in z-direction.
        TVector3 dirAt;
        HKalTrackState::calcDir(dirAt, stateVecTo);
        stepz = step * dirAt.z();

        stepFac = rkSingleStep(stateVecTo, DFstep, stepz, bCalcJac); // do one step

        posAt.SetXYZ(stateVecTo(kIdxX0), stateVecTo(kIdxY0), trackPosAtZ);
        Double_t sd = planeTo.signedDistanceToPlane(posAt);

#if rkDebug > 1
        cout<<"**** Do single step "<<jstep<<" ****"<<endl;
#endif
#if rkDebug > 2
        cout<<"Step size = "<<stepz<<endl;
        cout<<"Remaining distance to target plane: "<<sd<<endl;
        cout<<"State vector after step "<<jstep<<":"<<endl;
        stateVecTo.Print();
#endif

        Double_t prec = 1.e-6;
        if(((sd > 0. + prec) && propDir == kIterForward) || ((sd < 0. - prec) && propDir == kIterBackward)) {
            // Track went past target plane during propagtion. Repeating last step with reduced step size.
            step       *= stepSizeDec;
            nextStep   *= stepSizeDec;
            stateVecTo  = stateVecPreStep;
            trackPosAtZ = posPreStep.Z();
            posAt.SetXYZ(posPreStep.X(), posPreStep.Y(), posPreStep.Z());
            trackLength -= stepLength;
            continue;
        }

        // After each Runge-Kutta step i the covariance matrix C_i would have to be
        // transported with the propagator matrix F_i:
        // C_1 = F_1 * C_0   * F_1^T
        // C_2 = F_2 * C_1   * F_2^T
        // ...
        // C_n = F_n * C_n-1 * F_n^T
        // This can be transformed to:
        // C_n = F_n * ... * F_2 * F_1 * C_0 * F_1^T * F_2^T * ... * F_n^T
        //     =             F         * C_0 *         F^T

        fProp = DFstep * fProp; // update propagator

        // -----------------------------
        //  Process Noise and Energy Loss

        if((bDoMS || bDoDEDX) && stepLength > minLengthCalcQ) {
            // Track inclination = sqrt(1 + tx^2 + ty^2).
            Double_t trackIncl = TMath::Sqrt(1. + stateVecPreStep(kIdxTanPhi)*stateVecPreStep(kIdxTanPhi) + stateVecPreStep(kIdxTanTheta)*stateVecPreStep(kIdxTanTheta));

            // Total step length = step length in z direction * track inclination.
            stepLength = stepz * trackIncl;

            Double_t beta = 1. / TMath::Sqrt(1. + mass*mass * stateVecPreStep(kIdxQP)*stateVecPreStep(kIdxQP));

            // Properties of passed material.
            Double_t A, Z, density, radLength, exciteEner;
            if(calcMaterialProperties(A, Z, density, radLength, exciteEner, posPreStep, posAt, measLayFrom, measLayTo)) {
                if(bDoMS) {
                    calcMultScat(fProc, stateVecPreStep, stepLength, radLength, beta, pid);
                }

                if(bDoDEDX) {
                    energyLoss += calcEnergyLoss(fProc, stateVecTo, stepLength, stateVecPreStep(kIdxQP), Z, Z/A, density, radLength, exciteEner, pid);
                }
            } else {
                if(bPrintErr) {
                    Error("propagateToPlane()", "Could not calculate process noise.");
                }
            }
        }

        //  End Process Noise and Energy Loss
        // ----------------------------------

        // Decide if more steps must be done and calculate new step size.
        // -----------
        HKalTrackState::calcDir(dirAt, stateVecTo);
        if(!planeTo.findIntersection(pointIntersect, posAt, dirAt)) {
            trackOk = kFALSE;
            if(bPrintErr) {
                Error("propagateToPlane()", "No intersection with target plane found.");
            }
        }
        d = HKalGeomTools::distance2Points(posAt, pointIntersect);

        if (step < nextStep) {  // last step was rest step to chamber
            if (stepFac < 1.) nextStep = step * stepFac;
        } else {
            nextStep *= stepFac;
        }

        if (d > nextStep || d < maxDist) step = nextStep;
        else step = d;

        if(direction == kIterForward) {
            if(posAt.z() < pointIntersect.z()) { doNextStep = kTRUE; }
            else { doNextStep = kFALSE; }
        } else {
            if(posAt.z() > pointIntersect.z()) { doNextStep = kTRUE; }
            else { doNextStep = kFALSE; }
        }
        // -----------

#if rkDebug > 1
        cout<<"**** End single step. step: "<<jstep<<", z: "<<trackPosAtZ<<", size of next step: "<<nextStep<<" ****"<<endl;
#endif
    }

    // Done with Runge-Kutta stepping.
    // ------------------------------

    // To make sure the track position is on the target layer propagate to the target plane
    // using a straight line.
    stateVecPreStep = stateVecTo;
    posPreStep.SetXYZ(stateVecPreStep(kIdxX0), stateVecPreStep(kIdxY0), trackPosAtZ);

    if(!propagateStraightLine(stateVecTo, DFstep, trackPosAtZ, planeTo, propDir)) {
        trackOk = kFALSE;
        if(bPrintErr) {
            Error("propagateToPlane()", "Failed final propagation to target plane.");
        }
    }

    fProp = DFstep * fProp;

    // avoid too large/too small values
    if(!checkTrack(stateVecTo)) {
        trackOk = kFALSE;
        if(bPrintErr) {
            Error("propagateToPlane()", "Encountered nonsensical track parameters.");
        }
    }
    if(!checkPropMat(fProp)) {
        trackOk = kFALSE;
        if(bPrintErr) {
            Error("propagateToPlane()", "Encountered erroneous elements in covariance matrix.");
        }
    }

    // Energy loss and multiple scattering for straight line propagating.
    if((bDoDEDX || bDoMS) && stepLength > minLengthCalcQ) {
        Double_t A, Z, density, radLength, exciteEner;
        TVector3 posAt;
        HKalTrackState::calcPosAtPlane(posAt, planeTo, stateVecTo);
        Double_t beta = 1. / TMath::Sqrt(1. + mass*mass * stateVecPreStep(kIdxQP)*stateVecPreStep(kIdxQP));

        if(calcMaterialProperties(A, Z, density, radLength, exciteEner, posPreStep, posAt, measLayFrom, measLayTo)) {
            if(bDoMS) {
                calcMultScat(fProc, stateVecPreStep, stepLength, radLength, beta, pid);
            }

            if(bDoDEDX) {
                energyLoss += calcEnergyLoss(fProc, stateVecTo, stepLength, stateVecTo(kIdxQP), Z, Z/A, density, radLength, exciteEner, pid);
            }
        } else {
            if(bPrintErr) {
                Error("propagateToPlane()", "Could not calculate process noise.");
            }
        }
    }

    if(stateVecTo.GetNrows() == 6) {
        stateVecTo(kIdxZ0) = trackPosAtZ;
    }

    if(bPrintErr && bElossErr) {
        Error("propagateToPlane()", "Energy loss calculation failed.");
    }

#if rkDebug > 1
    cout<<"*************************"<<endl;
    cout<<"**** END RUNGE-KUTTA ****"<<endl;
    cout<<"*************************"<<endl;
#endif

    return (trackOk & !bElossErr);
}

void HKalRungeKutta::propagateStraightLine(TVectorD &stateVec, TMatrixD &fPropChange, Double_t &zPos, Double_t dz) {
    // Propagate the track state along a straight line in its current direction.
    // (x',y',z') = (x,y,z) + dz * (tx,ty,1)
    //
    // Output:
    // fPropChange: Change in propagator matrix.
    //
    // Input & output:
    // stateVec:    Current track state vector (x,y,tx,ty,qp).
    //
    // Input:
    // zPos:        Current z position of track.
    // dz:          Step length in z coordinate.

    Double_t tx = stateVec(kIdxTanPhi);
    Double_t ty = stateVec(kIdxTanTheta);

    // Update state vector.
    stateVec(kIdxX0) = stateVec(kIdxX0) + dz * tx;
    stateVec(kIdxY0) = stateVec(kIdxY0) + dz * ty;
    zPos             = zPos             + dz;
    trackPosAtZ      = zPos;
    if(stateVec.GetNrows() == 6) {
        stateVec(kIdxZ0) = trackPosAtZ;
    }

    // Update propagator matrix.
    fPropChange.UnitMatrix();

    fPropChange(kIdxX0, kIdxTanPhi)   = dz;
    fPropChange(kIdxY0, kIdxTanTheta) = dz;

    stepLength   = TMath::Abs(dz) * TMath::Sqrt(1. + tx*tx + ty*ty);
    trackLength += stepLength;

    //------------------------------------------------------------------------
    // For debugging + graphics purpose.
    if(bFillPointArrays) {
        TVector3 posAt  (stateVec(kIdxX0), stateVec(kIdxY0), trackPosAtZ);

	// only for the first time add
        if(pointsTrack.GetEntries() == 0) {
            // start point
            TVector3 posFrom(stateVec(kIdxX0) - dz * tx, stateVec(kIdxY0) - dz * ty, trackPosAtZ - dz);
            pointsTrack.Add(new TVector3(posFrom));
            fieldTrack.Add(new TVector3(0.,0.,0.));
        }

        pointsTrack.Add(new TVector3(posAt));
        fieldTrack.Add(new TVector3(0.,0.,0.));
    }
    //------------------------------------------------------------------------
}

Bool_t HKalRungeKutta::propagateStraightLine(TVectorD &stateVec, TMatrixD &fPropChange, Double_t &zPos, const HKalPlane &planeTo, Bool_t propDir) {
    // From the position and direction stored in the track state vector, propagate the track
    // to a target plane using a straight line. The track state and reference layer are updated
    // and the propagator matrix is calculated. The function returns the length of the straight line.
    // The class variable trackPosAtZ must contain the current z-value of the track.
    //
    // Output:
    // fPropChange: Change in propagator matrix
    //
    // Input and output:
    // stateVec:    Current Track state vector.
    // zPos:        Current z-position of track.
    //
    // Input:
    // planeTo:     Plane to propagate to.
    // propDir:     Propagation direction.

    stepLength = 0.;
    TVector3 pos(stateVec(kIdxX0), stateVec(kIdxY0), zPos);
    TVector3 dir;
    HKalTrackState::calcDir(dir, stateVec);
    TVector3 pointIntersect;
    planeTo.findIntersection(pointIntersect, pos, dir);
    Double_t dz = (pointIntersect.Z() - pos.Z());

#if rkDebug > 1
    cout<<"Final step using straight line. Signed distance to target layer = "<<planeTo.signedDistanceToPlane(pos)<<endl;
#endif

    if((dz > 0. && propDir == kIterForward) || (dz < 0. && propDir == kIterBackward)) {
        propagateStraightLine(stateVec, fPropChange, zPos, dz);
        stepLength = (pos - pointIntersect).Mag();
    } else {
        fPropChange.UnitMatrix();
        if(TMath::Abs(dz) > 0.001) {
            if(bPrintWarn) {
                Warning("propagateStraightLine()", Form("Track already past target plane by dz = %f.", TMath::Abs(dz)));
            }
            return kFALSE;
        }
    }
    return kTRUE;
}


Double_t HKalRungeKutta::rkSingleStep(TVectorD &stateVec, TMatrixD &fPropStep, Double_t stepSize, Bool_t bCalcJac) {
    // One step of track tracing from track state.
    //
    // Input and output:
    // stateVec:  Starting track parameters.
    // fPropStep: Change in propagator matrix for this step.
    //
    // Input:
    // totStep:   Step size.
    // bCalcJac:  Update the Jacobian (propagator matrix).


    const Int_t numPars = 4; // x, y, tx, ty

    // Convert the unit of Lorentz force from Newton to MeV/(c*s).
    // Lorentz force:
    // F = dp/dt = q * v x B
    // q in C, v in m/s, B in T
    // Unit conversion:
    // 1 N = 1 J/m
    //     = 10^6 * e / c  MeV/(c*s)
    //     = 10^9 * e / c  MeV/(c*s)  if c is in mm/s
    //
    // => F = dp/dt = kappa * q * v x B
    // q in e, v in mm/s, B in T
    const Double_t kappa = TMath::C() / 1.e9; // in MeV/(c * T * mm)

    //  Changes of state vector a by stepping in z direction:
    //  da/dz, a = (x, y, tx, ty, qp), z = stepping dir
    //  x'  = tx
    //  y'  = ty
    // tx'  = k * (tx * ty   * B[0] - (1 + tx2) * B[1] + ty * B[2])
    // ty'  = k * ((1 + ty2) * B[0] - txty      * B[1] - tx * B[2])
    //  k   = c * (q/p) * sqrt(1 + tx2 + ty2)

    // f    = f(z, a(z))
    // k1   = f(zn        , an)
    // k2   = f(zn + 1/2*h, an + 1/2*k1)
    // k3   = f(zn + 1/2*h, an + 1/2*k2)
    // k4   = f(zn + h    , an + k3)
    // fn+1 = fn + 1/6*k1 + 1/3*k2 + 1/3*k3 + 1/6*k4 + O(h^5)

    // Constants for RK stepping.
    static Double_t a[numPars] = { 0.0    , 0.5    , 0.5    , 1.0     };
    static Double_t b[numPars] = { 0.0    , 0.5    , 0.5    , 1.0     };
    static Double_t c[numPars] = { 1.0/6.0, 1.0/3.0, 1.0/3.0, 1.0/6.0 };

    // for rk stepping
    //Int_t step4;
    const Int_t rksteps = 4; // The 4 points used in Runge-Kutta stepping: start, 2 mid points and end
    const Int_t rkStart = 0;
    const Int_t rkMid1  = 1;
    const Int_t rkMid2  = 2;
    const Int_t rkEnd   = 3;

    // Change of track parameters in each step.
    // First index: step point (0 = start; 1,2 = mid, 3 = end), second index: state parameter.
    Double_t k[rksteps][numPars]; 

    // for propagator matrix
    Double_t F_tx[numPars], F_ty[numPars], F_tx_tx[numPars], F_ty_tx[numPars], F_tx_ty[numPars], F_ty_ty[numPars];
    Double_t F2_tx[numPars], F2_ty[numPars]; // @2tx/@z^2 and @2ty/@z^2

    //----------------------------------------------------------------
    Double_t est     = 0.; // error estimation
    Double_t stepFac = 1.;

    TVector3 B;                  // B-field
    Double_t h = stepSize;       // step size
    if(direction == kIterBackward) {
        h *= -1;                 // stepping in negative z-direction
    }
    Double_t half    = h * 0.5;  // half step interval for fourth order RK

    Double_t qp_in   = stateVec(kIdxQP);
    TVector3 posFrom = TVector3(stateVec(kIdxX0), stateVec(kIdxY0), trackPosAtZ);
    Double_t z_in    = posFrom.z();
    trackPosAtZ      = z_in;
    TVector3 posAt   = posFrom;

    Double_t hC      = h * kappa;

    // Input track state vector and state vector during stepping.
    Double_t sv_in[numPars], sv_step[numPars];
    sv_in[kIdxX0]       = stateVec(kIdxX0);
    sv_in[kIdxY0]       = stateVec(kIdxY0);
    sv_in[kIdxTanPhi]   = stateVec(kIdxTanPhi);
    sv_in[kIdxTanTheta] = stateVec(kIdxTanTheta);

    fPropStep.UnitMatrix();

    //------------------------------------------------------------------------
    //   Runge-Kutta step
    //
    Int_t istep;
    Int_t ipar;

    do {
        half  = h * 0.5;
        for (istep = 0; istep < rksteps; ++istep) {  // k1,k2,k3,k4  (k1=start, k2,k3 = half, k4=end of interval)
            for(ipar = 0; ipar < numPars; ++ipar) {  // 4 track parameters
                if(istep == 0) {
                    sv_step[ipar] = sv_in[ipar];     // in first step copy input track parameters (x,y,tx,ty)
                } else {
                    sv_step[ipar] = sv_in[ipar] + b[istep] * k[istep-1][ipar];     // do step
                }
            }
            trackPosAtZ = z_in + a[istep] * h; // move z along with track
            posAt.SetXYZ(sv_step[kIdxX0], sv_step[kIdxY0], trackPosAtZ ); // update z value for current position

            // rotate B-field vector
            if(bRotBfield) {
                TVector3 posAtSector = posAt;
                posAtSector.Transform(pRotMat->Inverse());
                calcField_mm(posAtSector, B, fieldScaleFact);
                B.Transform(*pRotMat);
            } else {
                calcField_mm(posAt, B, fieldScaleFact);
            }

            // Check if position or direction angles are too large.
            Bool_t svOk = kTRUE;
            if(sv_step[kIdxTanPhi] > maxTan) {
                sv_step[kIdxTanPhi] = maxTan;
                svOk = kFALSE;
            }
            if(sv_step[kIdxTanTheta] > maxTan) {
                sv_step[kIdxTanTheta] = maxTan;
                svOk = kFALSE;
            }
            if(sv_step[kIdxTanPhi] < -maxTan) {
                sv_step[kIdxTanPhi] = -maxTan;
                svOk = kFALSE;
            }
            if(sv_step[kIdxTanTheta] < -maxTan) {
                sv_step[kIdxTanTheta] = -maxTan;
                svOk = kFALSE;
            }
            if(sv_step[kIdxX0] > maxPos) {
                sv_step[kIdxX0] = maxPos;
                svOk = kFALSE;
            }
            if(sv_step[kIdxX0] < -maxPos) {
                sv_step[kIdxX0] = -maxPos;
                svOk = kFALSE;
            }
            if(sv_step[kIdxY0] > maxPos) {
                sv_step[kIdxY0] = maxPos;
                svOk = kFALSE;
            }
            if(sv_step[kIdxY0] < -maxPos) {
                sv_step[kIdxY0] = -maxPos;
                svOk = kFALSE;
            }
            if(!svOk) {
                if(bPrintWarn && trackOk) {
                    Warning("rkSingleStep()", "Bad elements in state vector.");
                }
                trackOk = kFALSE;
            }

            Double_t tx        = sv_step[kIdxTanPhi];
            Double_t ty        = sv_step[kIdxTanTheta];
            Double_t tx2       = tx * tx;
            Double_t ty2       = ty * ty;
            Double_t txty      = tx * ty;
            Double_t tx2ty21   = 1.0 + tx2 + ty2;

            Double_t I_tx2ty21 = 1.0 / tx2ty21 * qp_in;
            Double_t tx2ty2    = sqrt(tx2ty21);
                     tx2ty2   *= hC;
            Double_t tx2ty2qp  = tx2ty2 * qp_in;

            // for state propagation
            F_tx[istep] = ( txty        *B.x() - ( 1.0 + tx2 )*B.y() + ty*B.z()) * tx2ty2;    // h * @tx/@z / (qp) = h * tx' / (qp)
            F_ty[istep] = (( 1.0 + ty2 )*B.x() - txty         *B.y() - tx*B.z()) * tx2ty2;    // h * @ty/@z / (qp) = h * ty' / (qp)

            //cout<<"\n values for state propagation: "<<endl;
            //cout<<"F_tx[istep]    = (   txty        *B.x()    -    ( 1.0 + tx2 )*B.y()    +    ty*B.z()   ) *    tx2ty2"<<endl;
            //cout<< F_tx[istep]<<" = ( "<<txty       *B.x()<<" - "<<( 1.0 + tx2 )*B.y()<<" + "<<ty*B.z()<<") * "<<tx2ty2 <<endl;
            //cout<<"F_ty[istep]    = (    ( 1.0 + ty2 )*B.x()    -    txty         *B.y()    -    tx*B.z()   ) *    tx2ty2"<<endl;
            //cout<< F_ty[istep]<<" = ( "<<( 1.0 + ty2 )*B.x()<<" - "<<txty         *B.y()<<" - "<<tx*B.z()<<") * "<<tx2ty2 <<endl;

            //------------------------------------------------------------------------
            // for transport matrix
            F_tx_tx[istep] = F_tx[istep]*tx*I_tx2ty21 + ( ty*B.x()-2.0*tx*B.y() ) * tx2ty2qp; // h * @tx'/@tx
            F_tx_ty[istep] = F_tx[istep]*ty*I_tx2ty21 + ( tx*B.x()+B.z()        ) * tx2ty2qp; // h * @tx'/@ty
            F_ty_tx[istep] = F_ty[istep]*tx*I_tx2ty21 + (-ty*B.y()-B.z()        ) * tx2ty2qp; // h * @ty'/@tx
            F_ty_ty[istep] = F_ty[istep]*ty*I_tx2ty21 + ( 2.0*ty*B.x()-tx*B.y() ) * tx2ty2qp; // h * @ty'/@ty

            // Change of track parameters in each step.
            k[istep][kIdxX0]       = tx * h;              // dx
            k[istep][kIdxY0]       = ty * h;              // dy
            k[istep][kIdxTanPhi]   = F_tx[istep] * qp_in; // dtx
            k[istep][kIdxTanTheta] = F_ty[istep] * qp_in; // dty

            // h * @tx'/@z = h * (@tx/@dz)/@dz
            F2_tx[istep]   = qp_in *
                (tx*k[istep][kIdxTanPhi]/TMath::Sqrt(tx2ty21)*(txty*B.X() - (1. + tx2)*B.Y() + ty*B.Z())
                  + TMath::Sqrt(tx2ty21) *
                 (k[istep][kIdxTanPhi]*ty*B.X() + tx*k[istep][kIdxTanTheta]*B.X()
                  - 2.*tx*k[istep][kIdxTanPhi]*B.Y() + k[istep][kIdxTanTheta]*B.Z())
                );

            // h * @ty'/@z = h * (@ty/@dz)/@dz
            F2_ty[istep]   = qp_in *
                (ty*k[istep][kIdxTanTheta]/TMath::Sqrt(tx2ty21)*(( 1.0 + ty2 )*B.X() - txty*B.Y() - tx*B.Z())
                  + TMath::Sqrt(tx2ty21) *
                 (2.*ty*k[istep][kIdxTanTheta]*B.X() - k[istep][kIdxTanPhi]*ty*B.Y() - tx*k[istep][kIdxTanTheta]*B.Y()
                 - k[istep][kIdxTanPhi]*B.Z()));

        }  // end of Runge-Kutta steps
        //------------------------------------------------------------------------

        //------------------------------------------------------------------------
        // error estimation ala Geant
        est = 0.;

        est += fabs(k[rkStart][kIdxX0]       + k[rkEnd][kIdxX0]       - k[rkMid1][kIdxX0]       - k[rkMid2][kIdxX0])       * half;
        est += fabs(k[rkStart][kIdxY0]       + k[rkEnd][kIdxY0]       - k[rkMid1][kIdxY0]       - k[rkMid2][kIdxY0])       * half;
        est += fabs(k[rkStart][kIdxTanPhi]   + k[rkEnd][kIdxTanPhi]   - k[rkMid1][kIdxTanPhi]   - k[rkMid2][kIdxTanPhi])   * half;
        est += fabs(k[rkStart][kIdxTanTheta] + k[rkEnd][kIdxTanTheta] - k[rkMid1][kIdxTanTheta] - k[rkMid2][kIdxTanTheta]) * half;
        //------------------------------------------------------------------------

        if (est < minPrecision || h <= minStepSize || stepFac <= minStepSize) {
            // we found a step size with good precision
            jstep ++;
            break;
        } else {
            // precision not good enough. make smaller step
            stepFac *= stepSizeDec;
            h       *= stepSizeDec;
            hC       = h * kappa;

#if rkDebug > 2
            cout<<"Precision not good enough. Reducing step size to "<<h<<endl;
#endif
        }

    } while (jstep < maxNumSteps);

    if (est < maxPrecision && h < maxStepSize) {
        stepFac *= stepSizeInc;
    }

    //------------------------------------------------------------------------
    // set output track parameters
    for(ipar = 0; ipar < numPars; ++ ipar) {
        // yn+1        = yn         + 1/6*k1                    + 1/3*k2                  + 1/3*k3                  + 1/6*k4
        stateVec(ipar) = sv_in[ipar]+c[rkStart]*k[rkStart][ipar]+c[rkMid1]*k[rkMid1][ipar]+c[rkMid2]*k[rkMid2][ipar]+c[rkEnd]*k[rkEnd][ipar];
    }


    //------------------------------------------------------------------------
    // for debugging + graphics purpose
    if(bFillPointArrays) {
        TVector3 posSec;

        // only for the first time
        if(pointsTrack.GetEntries() == 0) {
            // start point
            calcField_mm(posFrom, B, fieldScaleFact);

            posSec = posFrom;
            if(bRotBfield) {
                posSec.Transform(pRotMat->Inverse());
            }
            pointsTrack.Add(new TVector3(posFrom));
            fieldTrack .Add(new TVector3(B));
        }

        posSec = posAt;
        if(bRotBfield) {
            posSec.Transform(pRotMat->Inverse());
        }
        calcField_mm(posAt, B, fieldScaleFact);

        pointsTrack.Add(new TVector3(posAt));
        fieldTrack .Add(new TVector3(B));
    }
    //------------------------------------------------------------------------

    stepLength = fabs(HKalGeomTools::distance2Points(posFrom, posAt));
    trackLength += stepLength;  // calculate track length

    if (est < maxPrecision && h < maxStepSize) {
        stepFac *= stepSizeInc;
    }

    if(!bCalcJac) {
        return stepFac;
    }

    //------------------------------------------------------------------------
    //
    //     Derivatives

    //     x  y tx ty qp
    //  x  0  5 10 15 20    J[ 0 ...] = da/dx       = 0,   1 = 1
    //  y  1  6 11 16 21    J[ 5 ...] = da/dy       = 0,   6 = 1
    // tx  2  7 12 17 22    J[10 ...] = da/dtx            12 = 1, 14 = 0
    // ty  3  8 13 18 23    J[15 ...] = da/dty            18 = 1, 19 = 0
    // qp  4  9 14 19 24    J[20 ...] = da/dqp            24 = 1
    //


    //------------------------------------------------------------------------
    //
    //     Derivatives    dx/dqp
    //

    // Update of Jacobian matrix for each step:
    // F_i = @a(zi)/@a(z0) =  @f(a,zi)/@a(zi) * (I + F_i-1 * (zi - z0)/dz) * dz
    //
    // z0      : starting z position
    // zi      : z position after step i = (z0, z0 + 0.5*dz, z0 + 0.5*dz, z0 + dz)
    // dz      : step size
    // (zi - z0) / dz = (0, 0.5, 0.5, 1)
    //
    // F_i = @a(zi)/@a(z0) : Jacobian matrix of RK step i
    // f(a,zi) = @a(zi)/@zi: derivation of track state vector a at step i by z
    // E                   : unit matrix
    // a(zi)               : track state vector at step i
    // a(z0)               : initial track state vector

    // x0: contribution of the Unit matrix to Jacobian
    // x:  contribution of Jacobian from previous step
    Double_t x0[numPars], x[numPars];

    // Elements of the Jacobian matrix F_i of one step i for the derivation of one specific track parameter.
    Double_t k1[rksteps][numPars];

    x0[kIdxX0] = 0.0; x0[kIdxY0] = 0.0; x0[kIdxTanPhi] = 0.0; x0[kIdxTanTheta] = 0.0;

    //   Runge-Kutta step for derivatives dx/dqp

    for(istep = 0; istep < rksteps; ++ istep) {
        for(ipar = 0; ipar < numPars; ++ ipar) {
            if(istep == 0) {
                x[ipar] = x0[ipar];
            } else {
                x[ipar] = x0[ipar] + b[istep] * k1[istep-1][ipar];
            }
        }
        //     F_i(x, qp)       = @x'/@tx * F_(i-1)(tx,qp) * dz
        //                      = F_(i-1)(tx,qp) * dz
        k1[istep][kIdxX0]       = x[kIdxTanPhi]   * h;
        //     F_i(y, qp)       = @y'/@ty * F_(i-1)(ty,qp) * dz
        //                      = F_(i-1)(ty,qp) * dz
        k1[istep][kIdxY0]       = x[kIdxTanTheta] * h;
        //     F_i(tx, qp)      = @tx'/@qp    + @tx'/@tx * F_(i-1)(tx,qp) * dz + @tx'/@ty * F_(i-1)(ty,qp) * dz
        k1[istep][kIdxTanPhi]   = F_tx[istep] + F_tx_tx[istep] * x[kIdxTanPhi] + F_tx_ty[istep] * x[kIdxTanTheta];
        //     F_i(ty, qp)      = @ty'/@qp    + @ty'/@tx * F_(i-1)(tx,qp) * dz + @ty'/@ty * F_(i-1)(ty,qp) * dz
        k1[istep][kIdxTanTheta] = F_ty[istep] + F_ty_tx[istep] * x[kIdxTanPhi] + F_ty_ty[istep] * x[kIdxTanTheta];
    }  // end of Runge-Kutta steps for derivatives dx/dqp


    for (ipar = 0; ipar < numPars; ++ipar ) {
        fPropStep(ipar, kIdxQP) = x0[ipar] + c[rkStart]*k1[rkStart][ipar] + c[rkMid1]*k1[rkMid1][ipar]
                                           + c[rkMid2] *k1[rkMid2][ipar]  + c[rkEnd] *k1[rkEnd][ipar];
    }
    fPropStep(kIdxQP,kIdxQP) = 1.;
    //------------------------------------------------------------------------



    //------------------------------------------------------------------------
    //     Derivatives    dx/tx
    //

    x0[kIdxX0] = 0.0; x0[kIdxY0] = 0.0; x0[kIdxTanPhi] = 1.0; x0[kIdxTanTheta] = 0.0;

    //
    //   Runge-Kutta step for derivatives dx/dtx
    //

    for (istep = 0; istep < 4; ++ istep) {
        for(ipar = 0; ipar < numPars; ++ipar) {
            if(istep == 0) {
                x[ipar] = x0[ipar];
            } else if ( ipar != kIdxTanPhi ){
                x[ipar] = x0[ipar] + b[istep] * k1[istep-1][ipar];
            }
        }

        k1[istep][kIdxX0]       = x[kIdxTanPhi]   * h;
        k1[istep][kIdxY0]       = x[kIdxTanTheta] * h;
        //k1[istep][kIdxTanPhi] = F_tx_tx[istep] * x[kIdxTanPhi] + F_tx_ty[istep] * x[kIdxTanTheta]; // not needed
        k1[istep][kIdxTanTheta] = F_ty_tx[istep] * x[kIdxTanPhi] + F_ty_ty[istep] * x[kIdxTanTheta];
    }  // end of Runge-Kutta steps for derivatives dx/dtx

    for(ipar = 0; ipar < numPars; ++ipar ) {
        if(ipar != kIdxTanPhi) {
            fPropStep(ipar, kIdxTanPhi) = x0[ipar] + c[rkStart]*k1[rkStart][ipar] + c[rkMid1]*k1[rkMid1][ipar]
                                                   + c[rkMid2] *k1[rkMid2][ipar]  + c[rkEnd] *k1[rkEnd][ipar];
        }
    }
    //      end of derivatives dx/dtx
    fPropStep(kIdxTanPhi, kIdxTanTheta) = 1.;
    fPropStep(kIdxQP,     kIdxTanPhi)   = 0.;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    //     Derivatives    dx/ty
    //

    x0[kIdxX0] = 0.0; x0[kIdxY0] = 0.0; x0[kIdxTanPhi] = 0.0; x0[kIdxTanTheta] = 1.0;

    //
    //   Runge-Kutta step for derivatives dx/dty
    //

    for (istep = 0; istep < 4; ++ istep) {
        for(ipar = 0; ipar < numPars; ++ipar) {
            if(istep == 0) {
                x[ipar] = x0[ipar];           // ty fixed
            } else if(ipar != kIdxTanTheta) {
                x[ipar] = x0[ipar] + b[istep] * k1[istep-1][ipar];
                //x[ipar] = x0[ipar] + b[istep] * k1[istep*4-4+ipar];
            }
        }

        k1[istep][kIdxX0]         = x[kIdxTanPhi]   * h;
        k1[istep][kIdxY0]         = x[kIdxTanTheta] * h;
        k1[istep][kIdxTanPhi]     = F_tx_tx[istep] * x[kIdxTanPhi] + F_tx_ty[istep] * x[kIdxTanTheta];
        //k1[istep][kIdxTanTheta] = F_ty_tx[istep] * x[kIdxTanPhi] + F_ty_ty[istep] * x[kIdxTanTheta]; // not needed
    }  // end of Runge-Kutta steps for derivatives dx/dty

    for(ipar = 0; ipar < 3; ++ipar ) {
        fPropStep(ipar, kIdxTanTheta) = x0[ipar] + c[rkStart]*k1[rkStart][ipar] + c[rkMid1]*k1[rkMid1][ipar]
                                                 + c[rkMid2] *k1[rkMid2][ipar]  + c[rkEnd] *k1[rkEnd][ipar];
    }
    //      end of derivatives dx/dty
    fPropStep(kIdxTanTheta,kIdxTanTheta) = 1.;
    fPropStep(kIdxQP      ,kIdxTanTheta) = 0.;
    //------------------------------------------------------------------------

    //------------------------------------------------------------------------
    //
    //    derivatives dx/dx and dx/dy

    for(ipar = 0; ipar < numPars + 1; ipar++) {
        fPropStep(ipar, kIdxX0) = 0.;
        fPropStep(ipar, kIdxY0) = 0.;
    }
    fPropStep(kIdxX0, kIdxX0) = 1.;
    fPropStep(kIdxY0, kIdxY0) = 1.;


    // Propagator has entry for z-coordinate as well.

    if(fPropStep.GetNrows() == 6) {
        Double_t x0z[numPars+1], xz[numPars+1];

        Double_t k1z[rksteps][numPars+1];

        Int_t idxZ0 = 4;
        x0z[kIdxX0] = 0.; x0z[kIdxY0] = 0.; x0z[kIdxTanPhi] = 0.; x0z[kIdxTanTheta] = 0.; x0z[idxZ0] = 1.;

        for (istep = 0; istep < 4; ++ istep) {
            for(ipar = 0; ipar < numPars+1; ++ipar) {
                if(istep == 0) {
                    xz[ipar] = x0z[ipar];
                } else
                    xz[ipar] = x0z[ipar] + b[istep] * k1z[istep-1][ipar];
            }

            // @x'/@z = @tx/@z
            k1z[istep][kIdxX0]       = xz[kIdxTanPhi]   * h + F_tx[istep] * qp_in * xz[idxZ0];
            k1z[istep][kIdxY0]       = xz[kIdxTanTheta] * h + F_ty[istep] * qp_in * xz[idxZ0];
            k1z[istep][kIdxTanPhi]   = F_tx_ty[istep] * xz[kIdxTanPhi] + F_tx_ty[istep] * xz[kIdxTanTheta]
                + F2_tx[istep] * xz[idxZ0];
            k1z[istep][kIdxTanTheta] = F_ty_tx[istep] * xz[kIdxTanPhi] + F_ty_ty[istep] * xz[kIdxTanTheta]
                + F2_ty[istep] * xz[idxZ0];
            k1z[istep][idxZ0]        = 0.;
        }
        for (ipar = 0; ipar < numPars; ++ipar ) {
            fPropStep(ipar, kIdxZ0) = x0z[ipar] + c[rkStart]*k1z[rkStart][ipar] + c[rkMid1]*k1z[rkMid1][ipar]
                                                + c[rkMid2] *k1z[rkMid2][ipar]  + c[rkEnd] *k1z[rkEnd][ipar];
        }

        fPropStep(kIdxZ0, kIdxZ0) = 1.;
    }

    return stepFac;

}

void HKalRungeKutta::setRungeKuttaParams(Float_t initialStpSize,
					 Float_t stpSizeDec, Float_t stpSizeInc,
					 Float_t maxStpSize, Float_t minStpSize,
					 Float_t minPrec, Float_t maxPrec,
					 Int_t maxNumStps, Int_t maxNumStpsPCA,
					 Float_t maxDst, Double_t minLngthCalcQ) {

    initialStepSize = initialStpSize;
    stepSizeDec     = stpSizeDec;
    stepSizeInc     = stpSizeInc;
    maxStepSize     = maxStpSize;
    minStepSize     = minStpSize;
    minPrecision    = minPrec;
    maxPrecision    = maxPrec;
    maxNumSteps     = maxNumStps;
    maxNumStepsPCA  = maxNumStpsPCA;
    maxDist         = maxDst;
    minLengthCalcQ  = minLngthCalcQ;
}

