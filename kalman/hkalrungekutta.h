#ifndef HKalRungeKutta_h
#define HKalRungeKutta_h

// from ROOT
#include "TMatrixD.h"
#include "TObjArray.h"
#include "TRotation.h"
class     TVector3;
#include "TVectorD.h"

// from hydra
#include "hmdctrackgfield.h"

#include "hkaldef.h"
class     HKalMdcMeasLayer;
class     HKalPlane;


class HKalRungeKutta : public TObject {

private:

    Bool_t     bConstField;      // Use a constant magnetic field instead of fieldMap.
    Bool_t     bDoDEDX;          // Calculate energy loss.
    Bool_t     bDoMS;            // Calculate coulomb multiple scattering.
    Bool_t     bRotBfield;       //! Do/Don't do coordinate transformation of B-field vectors.
    Bool_t     direction;        // Propagation direction: kIterForward/kIterBackward.

    Double_t   fieldScaleFact;   //! Factor of the magnetic field strength.
    TVector3   magField;         //! Use a constant magnetic field instead of fieldMap.
    TRotation *pRotMat;          //! Rotation matrix for possible coordinate transformations  of B-field vectors.
    Double_t   trackPosAtZ;      //! Store z position of track during Runge-Kutta propagation.

    Double_t   energyLoss;       //! Energy loss during stepping in MeV/particle charge.
    Double_t   trackLength;      //! Track length in mm.
    Double_t   stepLength;       //! Length of last RK step in mm.
    Int_t      jstep;            //! Step number.
    Float_t    initialStepSize;  //! initial RK step size
    Float_t    stepSizeInc;      //! factor to increase stepsize
    Float_t    stepSizeDec;      //! factor to decrease stepsize
    Float_t    maxStepSize;      //! maximum stepsize
    Float_t    minStepSize;      //! minimum stepsize
    Float_t    maxPrecision;     //! maximum required precision for a single step
    Float_t    minPrecision;     //! minimum required precision for a single step
    Float_t    maxDist;          //! maximum distance for straight line approximation
    Int_t      maxNumSteps;      //! maximum number of Runge-Kutta steps
    Int_t      maxNumStepsPCA;   //! maximum number of attempts to find the point of closest approach of the track to a wire
    Double_t   minLengthCalcQ;   //! minimum step length where process noise is still calculated.

    Bool_t     bFillPointArrays; // == kTRUE if point arrays should be filled
    TObjArray  pointsTrack;      // Points along trajectory stored
    TObjArray  fieldTrack;       // Field at Points along trajectory stored

    Bool_t     bElossErr;        // Encountered errors during energy loss calculation. Reset in every propagateToPlane() call.
    Bool_t     bPrintElossErr;   // Avoid repeated prints of the same error message.
    Bool_t     bPrintErr;        // Show error messages.
    Bool_t     bPrintWarn;       // Show warning messages.
    Bool_t     trackOk;          // Encountered errors during track propagation.
    Double_t   maxTan;           // Maximum allowed values for tx and ty.
    Double_t   maxPos;           // Maximum allowed values for x and y coordinates.
    Double_t   maxPropMat;       // Maximmum allowed value for an element of the propagator matrix.
    Double_t   minMom;           // Minimum required value for momentum in MeV/c.

    HMdcTrackGField *fieldMap;   //! Pointer to B-field map.

protected:

    virtual Double_t rkSingleStep(TVectorD &stateVec, TMatrixD &fPropStep, Double_t stepSize, Bool_t bCalcJac=kTRUE);

public:

    HKalRungeKutta();

    virtual ~HKalRungeKutta();

    virtual Double_t calcEnergyLoss        (TMatrixD &fProc, TVectorD &stateVec, Double_t length, Double_t qp,
					    Double_t Z, Double_t ZoverA, Double_t density, Double_t radLength,
					    Double_t exciteEner, Int_t pid);

    virtual Double_t calcDEDXBetheBloch    (Double_t beta, Double_t mass, Double_t ZoverA, Double_t density,
					    Double_t exciteEner, Double_t z=1.);

    virtual Double_t calcDEDXIonLepton     (Double_t qp, Double_t ZoverA, Double_t density, Double_t exciteEner, Int_t pid) const;

    virtual Double_t calcRadLoss           (TMatrixD &fProc, Double_t length, Double_t mass, Double_t qp, Double_t radLength) const;

    virtual TVector3 calcFieldIntegral     () const;

    virtual void     calcField_mm          (const TVector3& xv, TVector3& btos, Double_t fpol) const;

    virtual void     calcField_mm          (Double_t* xv, Double_t* btos,Double_t fpol) const;

    virtual Bool_t   calcMaterialProperties(Double_t &A, Double_t &Z, Double_t &density, Double_t &radLength, Double_t &exciteEner,
                                            const TVector3 &posPreStep, const TVector3 &posPostStep,
                                            const HKalMdcMeasLayer &layerFrom, const HKalMdcMeasLayer &layerTo) const;

    virtual void     calcMultScat          (TMatrixD &fProc, const TVectorD &stateVec, Double_t length, Double_t radLength, Double_t beta, Int_t pid) const;

    virtual void     clear                 ();

    virtual Bool_t   checkPropMat          (TMatrixD &fProp) const;

    virtual Bool_t   checkTrack            (TVectorD &stateVec) const;

    virtual Bool_t   propagateToPlane      (TMatrixD &fProp, TMatrixD &fProc,
                                            TVectorD &stateVecTo, const TVectorD &stateVecFrom,
                                            const HKalPlane &planeFrom, const HKalPlane &planeTo,
                                            const HKalMdcMeasLayer &measLayFrom, const HKalMdcMeasLayer &measLayTo,
                                            Int_t pid, Bool_t propDir, Bool_t bCalcJac=kTRUE);

    virtual void     propagateStraightLine (TVectorD &stateVec, TMatrixD &DF, Double_t &zPos, Double_t dz);

    virtual Bool_t   propagateStraightLine (TVectorD &stateVec, TMatrixD &DF, Double_t &zPos, const HKalPlane &planeTo, Bool_t propDir);

    virtual Double_t         getEnergyLoss () const                  { return energyLoss; }

    virtual Int_t            getNrStep     () const                  { return jstep; }

    virtual Double_t         getTrackLength() const                  { return trackLength; }

    virtual Double_t         getStepLength () const                  { return stepLength; }

    virtual TObjArray const& getPointsTrack() const                  { return pointsTrack; }

    virtual TObjArray const& getFieldTrack () const                  { return fieldTrack; }

    virtual void     setDirection          (Bool_t dir)              { direction = dir; }

    virtual void     setDoEnerLoss         (Bool_t dedx)             { bDoDEDX = dedx; }

    virtual void     setDoMultScat         (Bool_t ms)               { bDoMS = ms; }

    virtual void     setFieldFact          (Double_t fpol)           { fieldScaleFact = fpol; }

    virtual void     setFieldMap           (HMdcTrackGField *fpField, Double_t fpol) { fieldMap = fpField; fieldScaleFact = fpol; }

    virtual void     setFieldVector        (const TVector3 &B)       { magField = B; }

    virtual void     setFillPointsArrays   (Bool_t fill)             { bFillPointArrays = fill; }

    virtual void     setPrintErrors        (Bool_t print)            { bPrintErr = print; }

    virtual void     setPrintWarnings      (Bool_t print)            { bPrintWarn = print; }

    virtual void     setRotationMatrix     (TRotation *pRM)          { pRotMat = pRM; }

    virtual void     setRotateBfieldVecs   (Bool_t rotB)             { bRotBfield = rotB; }

    virtual void     setRungeKuttaParams   (Float_t initialStpSize, Float_t stpSizeDec, Float_t stpSizeInc, Float_t maxStpSize, Float_t minStpSize, Float_t minPrec, Float_t maxPrec, Int_t maxNumStps, Int_t maxNumStpsPCA, Float_t maxDst, Double_t minLngthCalcQ);

    virtual void     setUseConstField      (Bool_t constField)       { bConstField = constField; }

    ClassDef(HKalRungeKutta,0)
};

#endif // HKalRungeKutta_h
