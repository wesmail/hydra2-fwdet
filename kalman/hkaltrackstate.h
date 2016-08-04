#ifndef HKalTrackState_h
#define HKalTrackState_h

// from ROOT
#include "TMatrixD.h"
class     TVector3;
#include "TVectorD.h"

// from kalman
#include "hkaldef.h"
class     HKalPlane;

using namespace Kalman;


class HKalTrackState : public TObject {

private:

    kalFilterTypes type;    //! State is either for the prediction, filtering, smoothing or inverse filtering step in the Kalman filter.
    TMatrixD fPropagator;   //! Propagator matrix.
    TMatrixD fProjector;    //! Projector matrix.
    TMatrixD fCovariance;   //! Covariance matrix.
    TMatrixD fProcessNoise; //! Process noise matrix.
    TVectorD stateVec;      //! Vector with track state parameters.

public:

    HKalTrackState() {}

    HKalTrackState(Kalman::kalFilterTypes stateType, Int_t measDim, Int_t stateDim);

    HKalTrackState(Kalman::kalFilterTypes stateType, const TVectorD &sv, Int_t measDim);

    virtual ~HKalTrackState();

    virtual void   calcDir                   (TVector3 &dir) const;

    static  void   calcDir                   (TVector3 &dir, const TVectorD &sv);

    virtual void   calcMomVec                (TVector3 &dir) const;

    static  void   calcMomVec                (TVector3 &dir, const TVectorD &sv);

    virtual Bool_t calcPosAtPlane            (TVector3 &pos, const HKalPlane &plane) const;

    static  Bool_t calcPosAtPlane            (TVector3 &pos, const HKalPlane &plane, const TVectorD &sv);

    virtual Bool_t calcPosAndDirAtPlane      (TVector3 &pos, TVector3 &dir, const HKalPlane &plane) const;

    static  void   calcStateVec              (TVectorD &sv, Double_t qp, const TVector3 &pos, const TVector3 &dir);

    virtual void   clear();

    virtual void   print                     (const Option_t *opt="") const;

    virtual void   transform                 (const TRotation &transMat, const HKalPlane &plane);

    static  void   transformFromLayerToSector(TVectorD &svSec, const TVectorD &svLay, const HKalPlane &plane);

    static  void   transformFromSectorToLayer(TVectorD &svLay, const TVectorD &svSec, const HKalPlane &plane);

    virtual TMatrixD const&  getCovMat      () const                 { return fCovariance; }

    virtual TMatrixD const&  getPropMat     () const                 { return fPropagator; }

    virtual TMatrixD const&  getProjMat     () const                 { return fProjector; }

    virtual TMatrixD const&  getProcNoiseMat() const                 { return fProcessNoise; }

    virtual TVectorD const&  getStateVec    () const                 { return stateVec; }

    virtual Int_t            getStateType   () const                 { return type; }

    virtual Double_t         getStateParam  (Kalman::kalStateIdx par) const  { return stateVec(par); }

    virtual Int_t            getMeasDim     () const                 { return fProjector.GetNrows(); }

    virtual Int_t            getStateDim    () const                 { return stateVec.GetNrows(); }

    virtual void setCovMat      (const TMatrixD &fCov);

    virtual void setPropMat     (const TMatrixD &fProp);

    virtual void setProjMat     (const TMatrixD &fProj);

    virtual void setProcNoiseMat(const TMatrixD &fProc);

    virtual void setStateVec    (const TVectorD &sv);

    virtual void setStateVec    (Double_t qp, const TVector3 &pos, const TVector3 &dir) { calcStateVec(stateVec, qp, pos, dir); }

    ClassDef(HKalTrackState,0)
};


#endif // HKalTrackState_h
