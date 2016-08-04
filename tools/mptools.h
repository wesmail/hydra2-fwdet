#ifndef MPTOOLS__H
#define MPTOOLS__H

////////////////////////////////////////////////////////////////////////////////
//
// MPTools
// Maths:Peter's Tools
// 
//            Author: Peter W.Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: Mar 18, 2004
// Last modification: May 24, 2004
// 
// File: $RCSfile: mptools.h,v $ 
// Version: $Revision: 1.16 $
// Modified by $Author: halo $ on $Date: 2008-05-09 16:18:32 $  
////////////////////////////////////////////////////////////////////////////////

#include "hgeomvector.h"
#include "TObject.h"

class MPTools : public TObject
{
private:

 public:
  static Double_t        getDistancePointToStraight(HGeomVector &point, HGeomVector &base, HGeomVector &direction);
  static TH1*            calculateLikeSignCombinatorialBackground(TH1* pp, TH1* mm, TString name="background");
  static TH1*            calculateLikeSignCombinatorialBackground(TH1* pp, TH1* mm, TH1* correction, TString name="background");
  static Double_t        poissonDistribution(Double_t *x, Double_t *par);
  static Double_t        poissonDistribution(Double_t x, Double_t mean, Double_t logAmplitude=0.);
  static Double_t        poissonDistributionShifted(Double_t *x, Double_t *par);
  static Double_t        poissonDistributionShifted(Double_t x, Double_t mean, Double_t logAmplitude=0., Double_t shift=0);
  static Double_t        binomialDistribution(Double_t *x, Double_t *par);
  static Double_t        binomialDistribution(Double_t x, Double_t singleProbability, Double_t N, Double_t logAmplitude=0.);
  static Double_t        chiSquareDistribution(Double_t *x, Double_t *par);
  static Double_t        chiSquareDistribution(Double_t x, Double_t ndf, Double_t logAmplitude=0. );
  static Double_t        integralGauss(Double_t *x, Double_t *par);
  static Double_t        calcCMMomentumOfPairDecay(Double_t minv, Double_t m1, Double_t m2, 
						   Double_t minvErr, Double_t m1Err, Double_t m2Err, 
						   Double_t &err);
  static Double_t        calcCMMomentumOfPairDecay(Double_t minv, Double_t m1, Double_t m2);
  static Double_t        calcCMEnergyOfPairDecay(Double_t minv, Double_t m1, Double_t m2);
  //  static Double_t        calcArmenterosPt(Double_t alpha, Double_t minv, Double_t m1, Double_t m2);
  static TH1D*           projectPtYDownToPt(TH2* hist, Int_t  ymin, Int_t  ymax);
  static TH1D*           projectPtYDownToY (TH2* hist, Int_t ptmin, Int_t ptmax);
  static TH1D*           projectPtYDownToPtInInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax);
  static TH1D*           projectPtYDownToPtInBoltzmannRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TH1D*           projectPtYDownToMtInInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TH1D*           projectPtYDownToMtInBoltzmannRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TH1D*           projectPtYDownToMtM0InInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TH1D*           projectPtYDownToMtM0(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TH1D*           projectPtYDownToMt(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass);
  static TObjArray*      projectPtYDownToPtScaled(TH2* hist, Double_t scaleFactor=1., Int_t stepSize=1);
  static TObjArray*      projectPtYDownToPt(TH2* hist, Int_t stepSize=1);
  static TObjArray*      projectPtYDownToYScaled(TH2* hist, Double_t scaleFactor=1., Int_t stepSize=1);
  static TObjArray*      projectPtYDownToY(TH2* hist, Int_t stepSize=1);
  ClassDef(MPTools,0) // Peter's Math Tools
};
#endif
