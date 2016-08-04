#ifndef HPARTICLERUNNINGMEAN_H
#define HPARTICLERUNNINGMEAN_H

#include "TObject.h"
#include <cmath>
class HParticleRunningMeanI : public TObject {
protected:
    Int_t                fnMax; // (fixed)    maximum no of events used to calculate mean multiplicity
    Int_t             fminEvts; // (fixed)    minimum events for valid mean value
    Float_t     fscaleFacSigma; // (fixed)    scaling factor for cutoff of val distribution in units of the distribution width sigma
    Float_t            fmaxMin; // (fixed)    minimum value allowed for fmax
    Int_t                 fmin; // (fixed)    minimum allowed value
    Float_t        initialMean; // (fixed)    init values for first fminEvts events
    Float_t       initialSigma; // (fixed)    init values for first fminEvts events


    Float_t              fmean; // (floating) mean value
    Float_t             fsigma; // (floating) mean value
    Int_t                 fmax; // (floating) maximum allowed value
    Bool_t              fvalid; // value < max or fn (event counter) < fminEvts

    //event-wise determination of mean multiplicity of particleCandidates is calculated by averaging over nMultMax events
    Int_t            *fnPoints; // used to calculate a moving average
    Int_t               findex; // current index in mean array

    Int_t                 fSum; // (floating) sum of all acumulated means
    Int_t                fSum2; // (floating) sum of all acumulated means squared
    Int_t                   fn; // actual counter of entries until fnMax

    Int_t   setInitialMean(const Float_t initialMean, const Float_t initialSigma);
    Bool_t  isValid       (void);

public:
    HParticleRunningMeanI (void) { fnPoints = NULL;}
    ~HParticleRunningMeanI(void) { if(fnPoints != NULL) delete[] fnPoints;}
    Int_t   initParam     (const Int_t Max=2000, const Int_t minEvents=100, const Float_t scaleFacSigma=sqrt(12.0F),
			   const Float_t initMean=-999999, const Float_t initSigma=-999999, const Int_t min=1);
    void    reset         (Bool_t full=kTRUE);
    Float_t calcMean      (Int_t val);

    Float_t getMean          (void) { return fmean; }
    Float_t getSigma         (void) { return fsigma; }
    Bool_t  getValid         (void) { return fvalid;}
    Int_t   getMax           (void) { return fmax  ;}
    Int_t   getMin           (void) { return fmin  ;}
    Int_t   getMaxMin        (void) { return fmaxMin;}
    Int_t   getNMax          (void) { return fnMax;}
    Int_t   getNMin          (void) { return fminEvts;}
    Float_t getMeanScaleToMax(void) { return fscaleFacSigma;}

    ClassDef(HParticleRunningMeanI,0) //calculate moving average
};

#endif //HPARTICLERUNNINGMEAN_H
