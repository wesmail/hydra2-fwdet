//_HADES_CLASS_DESCRIPTION
////////////////////////////////////////////////////////////////////////////////
// Calculate running mean of a distribution of integer values e.g. multiplicities.
// calculate the sigma (RMS) of the distribution.
// A cutoff at a maximum value is defined as mean value plus sigma * a scaling factor.
// A minimum value can be specified as well (default = 1).
// If the mean value gets below 1, the cutoff value is calculated by (minimum value + 1) + scaling factor*(sigma=1)
// instead of mean value + sigma* scaling factor.
// This allows for recovery from large sets with input values of 0 (e.g. sparks).
// It accounts as well for the fact that input values are integer (0 or 1 or 2 ...), thus cutoff values must be > 1.
// The array size used for averaging can be specified in initParam (default = 2000).
// The values of the distribution have to be >= 0
// add an offset if the original distibution extends to negative values
// W. Koenig April 2015
////////////////////////////////////////////////////////////////////////////////

#include "hparticlerunningmean.h"
#include "TMath.h"
#include "TRandom.h"
#include <iostream>
using namespace std;

ClassImp(HParticleRunningMeanI)

Float_t HParticleRunningMeanI::calcMean(Int_t val)
{
  // calculate new mean and new sigma by replacing old  point by actual one.

    if(val < fmin) {
	fvalid = kFALSE;
	return fmean;
    }

    if(val <= fmax || fn < fminEvts) {
	fmax     = Int_t(max(fmean +fscaleFacSigma*fsigma + 0.5F,fmaxMin));
	fvalid   = kTRUE;
	fSum     += val - fnPoints[findex];
 	fSum2    += val*val - fnPoints[findex]*fnPoints[findex];
        fnPoints[findex] = val;
	if(++findex >= fnMax) findex = 0;
	fmean = Float_t(fSum)/Float_t(fn);
	fsigma = sqrt(float(fSum2)/float(fn) - fmean*fmean);
	if(fn < fnMax) ++fn;
    } else {
	fvalid = kFALSE;
    }
    return fmean;
};

Int_t HParticleRunningMeanI::initParam(const Int_t maxEvents, const Int_t minEvents,
				       const Float_t scaleFacSigma, const Float_t initMean, const Float_t initSigma, const Int_t min)
{
    // Reset moving average, clear multiplicity array,
    // Set minimum number of events to generate a valid multiplicity mean value
    // Set default scaling factor (mean + factor * sigma) to maximum multiplicity (in order to reject monsters from calculating mean)


    fnMax = maxEvents; // number of selected events which are used to calculate mean multiplicity
    if(fnPoints != NULL) delete[] fnPoints;
    fnPoints = new Int_t[fnMax];
    reset(kFALSE);
    if(minEvents <= fnMax) fminEvts = minEvents; //minimum events for valid mean value
    else                   fminEvts = fnMax;
    fscaleFacSigma         = scaleFacSigma; // scaling from mean multiplicity to max multiplicity (excluding outliers)
    fmin                   = max(min,0);
    fmaxMin                = (fmin + 1.0F) + fscaleFacSigma + 0.5F;
    fvalid                 = kFALSE;
    fmax                   = fmaxMin;
    initialMean            = initMean;
    initialSigma           = initSigma;
    setInitialMean(initialMean, initialSigma) ;
    return fnMax; // returns array size used for running mean
};

Int_t HParticleRunningMeanI::setInitialMean( const Float_t initMean, const Float_t initSigma)
{
    // create a set of initial mean values filling first 'fminEvts'
    // multiplicity array members. returns number of initialized multiplicity
    // array members (= fminEvts set in 'initParam' before
    if(initMean==-999999 || initSigma==-999999) return 0;
    initialMean  = initMean;
    initialSigma = initSigma;
    Float_t initialWidth = initSigma*sqrt(12.0F); // width of a box-like distribution
    if(initialWidth < 1.0F) initialWidth = 1.0F; // minimum needed to get correct mean for integers
    for (findex = 0; findex < fminEvts; ++findex) {
	fnPoints[findex] = Int_t(initMean + (gRandom->Uniform()-0.5F)*initialWidth + 0.5F);
	fSum += fnPoints[findex];
	fSum2 += fnPoints[findex]*fnPoints[findex]; // quadratic sum needed for sigma distribution
    };
    fn = fminEvts;
    fmean = Float_t(fSum)/Float_t(fn);
    fsigma=  sqrt(Float_t(fSum2)/Float_t(fn) - fmean*fmean);
    fmax = Int_t (max(fmean + fscaleFacSigma * initSigma + 0.5F,fmaxMin));

    return fn;
};

void HParticleRunningMeanI::reset(Bool_t full)
{
    //clear multiplicity sum, multiplicity array, reset findex counter
    for (Int_t i = 0; i < fnMax; ++i) fnPoints[i] = 0;
    fSum    = 0;
    fSum2   = 0;
    fn      = 1; //starts with 1, runs up while multiplicityPoints array is filled and stays constant after fnMax is reached
    findex  = 0; //running findex pointing to the last entry in the list of multiplicity points
    fmean   = 0.0F;
    fsigma  = 0.0F;
    if(full)setInitialMean(initialMean,initialSigma) ;
};

Bool_t HParticleRunningMeanI::isValid(void)
{
    // returns true if event no > fminEvts (reliable mean) and
    // last val < fmax (no outlier)
    return (fn >= fminEvts) && fvalid;
};
