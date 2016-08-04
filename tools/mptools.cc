//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////////////////////
//
// Peter's Math Tools
// 
//            Author: Peter W. Zumbruch
//           Contact: P.Zumbruch@gsi.de
//           Created: March 18, 2004
// 
// File: $RCSfile: mptools.cc,v $ 
// Version: $Revision: 1.19 $
// Modified by $Author: halo $ on $Date: 2008-05-09 16:18:32 $  
////////////////////////////////////////////////////////////////////////////////

using namespace std;

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>

#include "TApplication.h"
#include "TCanvas.h"
#include "TError.h"
#include "TFile.h"
#include "TF1.h"
#include "TH1.h"
#include "TH2.h"
#include "TH3.h"
#include "THStack.h"
#include "TLatex.h"
#include "TMath.h"
#include "TObject.h"
#include "TPaveText.h"
#include "TPad.h"
#include "TROOT.h"
#include "TStopwatch.h"
#include "TSystem.h"
#include "TString.h"
#include "TStyle.h"
#include "TTree.h"
#include "TUnixSystem.h"

#include "ptools.h"
#include "mptools.h"

ClassImp(MPTools)

////////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------------

Double_t MPTools::getDistancePointToStraight(HGeomVector &point, HGeomVector &base, HGeomVector &direction)
{
  if (direction.length() > 0)
    {
      direction*=1/direction.length();
    }
  else
    {
      return -1.;
    }

    Double_t val = 
      pow(point(0)-base(0),2)+
      pow(point(1)-base(1),2)+
      pow(point(2)-base(2),2)
      - (
	 pow(
	     (point(0)-base(0))*direction(0)+
	     (point(1)-base(1))*direction(1)+
	     (point(2)-base(2))*direction(2)
	     ,2)
	 );

  if (val<0.)
    {
      return -1.;
    }
  return sqrt(val);

}

// --------------------------------------------------------------------------------

TH1* MPTools::calculateLikeSignCombinatorialBackground(TH1* pp, TH1* mm, TString name)
{
  // calculates binwise the likesign combinatorial background of two given histograms 
  // using the formula
  //
  //              /---------
  // back = 2 *  V pp * mm
  //         
  //
  // and returns the pointer to the background histogramm
  // with the name assigned assigned by name [default: "background"]
  // 
  // if pp or mm are null pointers NULL is returned 
  // if pp and mm are not compatible NULL is returned 
  
  // check for not NULL pointer
  if (!pp || !mm)
    {
	::Error("calculateLikeSignCombinatorialBackground","can't calculate, ++ (%p) or -- (%p) is null pointer, returning NULL",pp, mm);
      return NULL;
    }

  // check for compatibility
  if (! PTools::areHistogramsCompatible(pp,mm))
    {
	::Warning("calculateLikeSignCombinatorialBackground","can't calculate, hists ++ and -- are not compatible, return NULL!");
      return NULL;
    }

  // create hist error structure
  if (! pp->GetSumw2N()) pp->Sumw2();
  if (! mm->GetSumw2N()) mm->Sumw2();

  // Clone 
  TH1 *background = (TH1*) pp->Clone(name.Data());
  background->Reset();

  // 2 * sqrt ( pp * mm );
  background->Multiply(pp,mm);
  PTools::sqrt(background);
  background->Scale(2.);
  
  return background;
} 

// --------------------------------------------------------------------------------
// --------------------------------------------------------------------------------

TH1* MPTools::calculateLikeSignCombinatorialBackground(TH1* pp, TH1* mm, TH1* correction, TString name)
{
  // calculates binwise the likesign combinatorial background of two given histograms 
  // using the formula
  //
  //                          /---------
  // back = 2 * correction * V pp * mm
  //         
  //
  // and returns the pointer to the background histogramm
  // with the name assigned assigned by name [default: "background"]
  // 
  // correction is also a histogram containing bin-wise correction factors (like acceptance correction)
  //
  // if correction, pp or mm are null pointers NULL is returned 
  // if correction, pp and mm are not compatible NULL is returned 
  
  // check for not NULL pointer
  if (!correction)
    {
	::Error("calculateLikeSignCombinatorialBackground","can't calculate, correction (%p) is null pointer, returning NULL",correction);
      return NULL;
    }

  // check for compatibility
  if (! PTools::areHistogramsCompatible(pp,mm))
    {
	::Warning("calculateLikeSignCombinatorialBackground","can't calculate, hists ++ and -- are not compatible, return NULL!");
      return NULL;
    }

  if (! PTools::areHistogramsCompatible(correction,mm))
    {
	::Warning("calculateLikeSignCombinatorialBackground","can't calculate, hists correction and --/++ are not compatible, return NULL!");
      return NULL;
    }
  
  // 2 * sqrt ( pp * mm );
  TH1* background = calculateLikeSignCombinatorialBackground(pp, mm, name);

  // correction 2 * sqrt ( pp * mm );
  background->Multiply(correction);

  return background;
} 

// --------------------------------------------------------------------------------

Double_t MPTools::poissonDistribution(Double_t *x, Double_t *par)
{ 
  // discrete poisson distribution as continuous function
  // poisson is the limiting form of the binomial distribution 
  // for p->0 and N->infinity
  //
  //             r  -mean
  //         mean  e    
  // P(r) = ------------- 
  //             r!
  //
  // translates to 
  //
  //                          r  -mean
  //          amplitude   mean  e     
  // P(x) = 10           ---------------
  //                        Gamma(r+1)
  //
  // if x<0 P(0) is returned
  // 
  // par[0] : amplitude
  // par[1] : mean
  //
  Double_t ampl = par[0];
  Double_t mean = par[1];
  Double_t xx   =   x[0];

  if (xx <= 0.) { xx=0; }

  Double_t denom = TMath::Gamma(xx+1);
  
  if (!TMath::Finite(denom))
    {
	::Warning( "MPTools::binomialDistribution","cannot handle such large numbers Gamma(%f) infinite.. returning -10",xx);
      return -10;
    }

  Double_t res = ((1./denom)  * pow(mean,xx) * exp(-mean)) * pow(10,ampl);

  return res;
}

Double_t MPTools::poissonDistribution(Double_t x, Double_t mean, Double_t amplitude)
{ 
  // discrete poisson distribution as continuous function
  // poisson is the limiting form of the binomial distribution 
  // for p->0 and N->infinity
  //
  //             r  -mean
  //         mean  e    
  // P(r) = ------------- 
  //             r!
  //
  // translates to 
  //
  //                          r  -mean
  //          amplitude   mean  e     
  // P(x) = 10           ---------------
  //                        Gamma(r+1)
  //
  // if x<0 P(0) is returned
  
  Double_t xx[1];
  Double_t par[2];
  
  xx[0] = x;
  par[0] = amplitude;
  par[1] = mean;

  return poissonDistribution(xx,par);
}

// --------------------------------------------------------------------------------

Double_t MPTools::poissonDistributionShifted(Double_t *x, Double_t *par)
{ 
  // discrete poisson distribution as continuous function
  // poisson is the limiting form of the binomial distribution 
  // for p->0 and N->infinity
  //
  //             r  -mean
  //         mean  e    
  // P(r) = ------------- 
  //             r!
  //
  // translates to 
  //
  //                          x+shift  -mean
  //          amplitude   mean  e     
  // P(x) = 10           ---------------
  //                        Gamma(x+1+shift)
  //
  // if x<0 -1 is returned
  // 
  // par[0] : amplitude
  // par[1] : mean
  //
  
  Double_t xx[1];
  xx[0]= x[0]+par[2];

  return poissonDistribution(xx,par);
}

Double_t MPTools::poissonDistributionShifted(Double_t x, Double_t mean, Double_t amplitude, Double_t shift)
{ 
  // discrete poisson distribution as continuous function
  // poisson is the limiting form of the binomial distribution 
  // for p->0 and N->infinity
  //
  //             r  -mean
  //         mean  e    
  // P(r) = ------------- 
  //             r!
  //
  // translates to 
  //
  //                          x+shift  -mean
  //          amplitude   mean  e     
  // P(x) = 10           ---------------
  //                        Gamma(x+1+shifta)
  //
  // if x<0 -1 is returned
  
  Double_t xx[1];
  Double_t par[3];
  
  xx[0] = x;
  par[0] = amplitude;
  par[1] = mean;
  par[2] = shift;

  return poissonDistributionShifted(xx,par);
}
// --------------------------------------------------------------------------------

Double_t MPTools::binomialDistribution(Double_t *x, Double_t *par)
{
  // discrete binomial distribution as continuous function
  // 
  //              N!       r       N-r
  // P(r) = ------------- p (1 - p)
  //         r! (N - r) !
  //
  // translates to 
  //
  //          Amplitude           Gamma(N+1)           r       N-r
  // P(x) = 10           ---------------------------- p (1 - p)
  //                     Gamma(r+1) Gamma (N - r + 1)
  //
  // P(x) the probability of r successes in N tries
  // p is the single probability 
  //
  // par[0]: Amplitude, chosen for Normalization purposes 
  // par[1]: number of tries (N)
  // par[2]: single probability (p)
  //
  // if x<0 -1 is returned
  // 
  // if x>N -1 is returned

  Double_t ampl              = par[0];
  Double_t N                 = par[1];
  Double_t singleProbability = par[2];
  Double_t xx                =   x[0];

  if (xx > N) return -1.;

  if ((xx+1) > 150)
    {
      ::Warning( "MPTools::binomialDistribution","cannot handle such large numbers x(%f) > 150).. returning -10",xx);
      return -10;
    }
  if ((N) > 150)
    {
      ::Warning( "MPTools::binomialDistribution","cannot handle such large numbers Number of tries (%f) > 150).. returning -10",N);
      return -10;
    }
  if ((N-xx+1) > 150)
    {
      ::Warning( "MPTools::binomialDistribution","cannot handle such large numbers .. returning -10");
      return -10;
    }

  Double_t denom = TMath::Gamma(xx+1)*TMath::Gamma(N-xx+1);

  Double_t nom = pow(10,ampl) * TMath::Gamma(N+1) * pow(singleProbability,xx) * pow(1-singleProbability,N-xx);

  return nom/denom;
  
}


// --------------------------------------------------------------------------------

Double_t MPTools::binomialDistribution(Double_t x, Double_t singleProbability, Double_t N, Double_t amplitude)
{
  // discrete binomial distribution as continuous function
  // 
  //              N!       r       N-r
  // P(r) = ------------- p (1 - p)
  //         r! (N - r) !
  //
  // translates to 
  //
  //          Amplitude           Gamma(N+1)           r       N-r
  // P(x) = 10           ---------------------------- p (1 - p)
  //                     Gamma(r+1) Gamma (N - r + 1)
  //
  // P(x) the probability of r successes in N tries
  // p is the single probability 
  //
  // par[0]: Amplitude, chosen for Normalization purposes 
  // par[1]: number of tries (N)
  // par[2]: single probability (p)
  // if x<0 -1 is returned
  // 
  // if x>N -1 is returned

  Double_t xx[1];
  Double_t par[3];
  
  xx[0] = x;
  par[0] = amplitude;
  par[1] = N;
  par[2] = singleProbability;
  
  return binomialDistribution(xx, par);
}

// --------------------------------------------------------------------------------

Double_t MPTools::chiSquareDistribution(Double_t *x, Double_t *par)
{
  // chi square distriubtion
  //
  //                       (0.5 * NDF) -1     (- 0.5 * chi2)
  //           (0.5 * chi2)               * e
  // P(chi2) = ----------------------------------------------
  //                         2 * Gamma (0.5 * NDF)
  // 
  // adding a normalization amplitude 
  // 
  //                                    (0.5 * NDF) -1     (- 0.5 * x[0])
  //             amplitude   (0.5 * x[0])               * e
  // P(x[0]) = 10         *  ------------------------------------------------
  //                                       2 * Gamma (0.5 * NDF)
  //
  // 
  // par[1]: NDF are number of degrees of freedom
  // par[0]: amplitude
  //
  // if ndf <=0 -1 is returned

  Double_t amplitude = par[0];
  Double_t ndf = par[1];
  Double_t xx = x[0];


  if (ndf<=0) return -1;
  
  Double_t denom = 2 * TMath::Gamma( 0.5 * ndf);
  
  Double_t nom = pow (10, amplitude) * pow(0.5 * xx, 0.5*ndf -1) * exp(-0.5*xx);

  return nom/denom;
  
}

// --------------------------------------------------------------------------------

Double_t MPTools::chiSquareDistribution(Double_t x, Double_t ndf, Double_t amplitude)
{
  // chi square distriubtion
  //
  //                       (0.5 * NDF) -1     (- 0.5 * chi2)
  //           (0.5 * chi2)               * e
  // P(chi2) = ----------------------------------------------
  //                         2 * Gamma (0.5 * NDF)
  // 
  // adding a normalization amplitude 
  // 
  //                                 (0.5 * NDF) -1     (- 0.5 * x)
  //          amplitude   (0.5 * x)               * e
  // P(x) = 10         *  ------------------------------------------------
  //                                    2 * Gamma (0.5 * NDF)
  //
  // 
  // NDF are number of degrees of freedom
  // amplitude
  //
  // if ndf <=0 -1 is returned

  Double_t xx[1];
  Double_t par[2];
  
  xx[0] = x;
  par[0] = amplitude;
  par[1] = ndf;
  
  return chiSquareDistribution(xx, par);
}

// --------------------------------------------------------------------------------

Double_t MPTools::calcCMMomentumOfPairDecay(Double_t minv, Double_t m1, Double_t m2, 
					    Double_t minvErr, Double_t m1Err, Double_t m2Err, 
					    Double_t &err)
{
  // calculates in a 2-body decay the center of momentum momentum of the decay particles
  // where 
  //    minv is the invariant mass of the parent
  //    m1 is the mass of the particle 1
  //    m2 is the mass of the particle 2
  //    minvErr is the absolute error parents invariant mass
  //    m1Err is absolute error of the mass of the particle 1
  //    m2Err is absolute error of the mass of the particle 2
  // 
  //    the error is returned via err
  //    in case of errors -1. is returned
  //
  // !!!! in case of m1!=m2 I am not quite sure if the result is ok. !!!


  if (m1Err < 0. || m2Err < 0 || minvErr < 0)
    {
      ::Error("calcCMMomentumOfPairDecay",
	      "one or more input error(s) are negativ: minvErr: %f , m1Err: %f, m2Err: %f .. return: -1",
	      minvErr, m1Err, m2Err);
      return -1.;
    }
  
  Double_t p = calcCMMomentumOfPairDecay(minv, m1, m2);
  
  if (p<=0) {return -1;}
  
  Double_t A=0;

  A=(m1*m1 - m2*m2)/(minv*minv);

  if (m1 != m2)
    {
      A=(m1*m1 - m2*m2)/(minv*minv);
    }
  
  err = 
    pow((minv * minvErr)/2 * (1- pow( A ,2)) ,2) +
    pow(m1Err * m1 *( A-1) ,2) +
    pow(m2Err * m2 *(-A-1) ,2);

  if (err<0)
    {
      ::Warning("calcCMMomentumOfPairDecay",
	       "rounding problems? while calculating error: negative square .. return err: -1");
      err = -1.;
    }
  else
    {
      err = 1/(2*p) * sqrt (err);
    } 
  
  return p;

}

// --------------------------------------------------------------------------------

Double_t MPTools::calcCMMomentumOfPairDecay(Double_t minv, Double_t m1, Double_t m2)
{
  // calculates in a 2-body decay the center of momentum momentum of the decay particles
  // where 
  //    minv is the invariant mass of the parent
  //    m1 is the mass of the particle 1
  //    m2 is the mass of the particle 2
  // 
  //    the error is returned via err
  //    in case of errors -1. is returned

  if ((m1+m2)>minv)
    {
      ::Error("calcCMMomentumOfPairDecay",
	      "minv is smaller than m1 + m2 : minv: %f , m1: %f, m2: %f .. return: -1",minv, m1, m2);
      return -1.;
    }

  if (minv == 0)
    {
      ::Error("calcCMMomentumOfPairDecay",
	      "minv is zero: minv: %f .. return: -1",minv);
      return -1.;
    }
  if (m1 < 0. || m2 < 0 || minv < 0)
    {
      ::Error("calcCMMomentumOfPairDecay",
	      "one or more input(s) are negativ: minv: %f , m1: %f, m2: %f .. return: -1",minv, m1, m2);
      return -1.;
    }

  Double_t p2;
  if (m1 != m2)
    {
      p2 = pow(minv/2,2) - 0.5 * ( m1*m1 + m2*m2 ) + pow( (m1*m1 - m2*m2)/(2*minv) , 2);
    }
  else
    {
      p2 = pow(minv/2,2) - 0.5 * ( m1*m1 + m2*m2 );
    } 

  if (p2 < 0)
    {
      ::Error("calcCMMomentumOfPairDecay",
	      "rounding problems?: negative square .. return: -1");
      return -1.;
    }
  else
    {
      return sqrt(p2);
    }
}

// --------------------------------------------------------------------------------

Double_t MPTools::calcCMEnergyOfPairDecay(Double_t minv, Double_t m1, Double_t m2)
{
  // calculates in a 2-body decay the center of momentum energy of the decay particles
  // of particle with mass m1
  // where 
  //    minv is the invariant mass of the parent
  //    m1 is the mass of the particle 1
  //    m2 is the mass of the particle 2
  // 
  //    in case of errors -1. is returned

  if ((m1+m2)>minv)
    {
      ::Error("calcCMEnergyOfPairDecay",
	      "minv is smaller than m1 + m2 : minv: %f , m1: %f, m2: %f .. return: -1",minv, m1, m2);
      return -1.;
    }

  if (minv == 0)
    {
      ::Error("calcCMEnergyOfPairDecay",
	      "minv is zero: minv: %f .. return: -1",minv);
      return -1.;
    }
  if (m1 < 0. || m2 < 0 || minv < 0)
    {
      ::Error("calcCMEnergyOfPairDecay",
	      "one or more input(s) are negativ: minv: %f , m1: %f, m2: %f .. return: -1",minv, m1, m2);
      return -1.;
    }

  Double_t p = calcCMMomentumOfPairDecay(minv, m1, m2);
  if (p<=0) {return -1;}

  return sqrt(m1*m1 + p*p);
}

// --------------------------------------------------------------------------------

Double_t MPTools::integralGauss(Double_t *x, Double_t *par)
{
  // Parametric 1-dimensional function with 3 parameters
  // par[0] = Integral of Gaus-Funktion in range +- infinity
  // par[1] = mean of gauss
  // par[2] = sigma
  //
  // returns 0 if par[2] = 0
  //
  // else returns
  //
  // par[0]/(sqrt(TMath::Pi()*2)*par[2])*TMath::Gaus(x[0],par[1],par[2]) 


  // integral of gauss +/- infinity = sqrt(2Pi)*sigma*A
  // A = Integral/(sqrt(2Pi)*sigma)
  // par[2]<=>Integral
  // returns 0, if par[2] == 0

  if (par[2]==0) return 0.;

  return par[0]/(sqrt(TMath::Pi()*2)*par[2])*TMath::Gaus(x[0],par[1],par[2]);
}

// --------------------------------------------------------------------------------

// Double_t MPTools::calcArmenterosPt(Double_t alpha, Double_t minv, Double_t m1, Double_t m2)
// {
//   // calculates in the armenteros p_t for a given armenteros alpha
//   // where 
//   //    minv is the invariant mass of the parent
//   //    m1 is the mass of the positive particle 1 
//   //    m2 is the mass of the negative particle 2
//   // 
//   //    in case of errors -1. is returned

//   Double_t Ep =   calcCMEnergyOfPairDecay(minv, m1, m2);
//   Double_t Em =   calcCMEnergyOfPairDecay(minv, m2, m1);
//   Double_t p  = calcCMMomentumOfPairDecay(minv, m1, m2);
  
//   if (Ep < 0. || Em < 0 || p < 0)
//     {
//       return -1.;
//     }

//   Double_t alphaE = (Ep-Em)/minv;
//   Double_t beta = p/minv;

//   Double_t pt2 = p*p - pow( alpha - alphaE ,2)/pow( 2 * beta ,2);

//   return p - pow( alpha - alphaE ,2)/pow( 2 * beta ,2);

//   if (pt2<0)
//     {
//       ::Error("calcArmenterosPt",
// 	      "rounding problems?: negative square .. return: -1");
//       return -1.;
//     }
//   else
//     {
//       return sqrt(pt2);
//     }
// }

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToPt(TH2* hist, Int_t ymin, Int_t ymax)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the projection of hist in a bin range ymin to ymax on the x-Axis
  if (!hist)
    {
      ::Error("projectPtYDownToPt"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPt"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": "+hist->GetYaxis()->GetTitle();
  ytitle = TString("#frac{d^{2}N}{dp_{T}dy}_{") + yRangeTitle + "} [1/";
  ytitle+=Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  ytitle+=" MeV/c]";
  
  TString name;
  name = "projPt_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

  TH1D* proj = PTools::getTH1D(name,title,
			       nbinsy,
			       hist->GetYaxis()->GetXmin(),
			       hist->GetYaxis()->GetXmax(),
			       xtitle,
			       ytitle);

  TH1D* projErr2 = PTools::getTH1D(name+"_Err2",title,
 				   nbinsy,
				   hist->GetYaxis()->GetXmin(),
				   hist->GetYaxis()->GetXmax(),
				   xtitle,
				   ytitle);
  proj->Sumw2();

  Double_t weight=1.;
  Double_t pt=-1.;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (binx >= ymin && binx <= ymax)
		{
		  weight = 1.;

		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);

		  proj->Fill(pt,value*weight);
		  projErr2->Fill(pt,error*error*weight*weight);
		}
	    }
	}
    }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToPtInInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the invariant cross section weighted (1/pt) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToPtInInvariantRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPtInInvariantRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": "+hist->GetYaxis()->GetTitle();
  ytitle = TString("#frac{1}{2#pi p_{T}}#frac{d^{2}N}{dp_{T}dy}_{") + yRangeTitle + "} [1/";
  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  ytitle+="]";
  
  TString name;
  name = "projPtInvariant_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "#frac{1}{2#pi p_{T}} weighted projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

  TH1D* proj = PTools::getTH1D(name,title,
			       nbinsy,
			       hist->GetYaxis()->GetXmin(),
			       hist->GetYaxis()->GetXmax(),
			       xtitle,
			       ytitle);

  TH1D* projErr2 = PTools::getTH1D(name+"_Err2",title,
 				   nbinsy,
				   hist->GetYaxis()->GetXmin(),
				   hist->GetYaxis()->GetXmax(),
				   xtitle,
				   ytitle);
  proj->Sumw2();

  Double_t weight=1.;
  Double_t pt=-1.;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (binx >= ymin && binx <= ymax)
		{
		  if (pt) {weight = 1./(pt);}
		  else {weight = 0;}

		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);

		  proj->Fill(pt,value*weight);
		  projErr2->Fill(pt,error*error*weight*weight);
		}
	    }
	}
    }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  return proj;
}
 
////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToPtInBoltzmannRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToPtInBolzmannRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPtInBolzmannRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": "+hist->GetYaxis()->GetTitle();
  ytitle = TString("#frac{1}{p_{T} E }#frac{d^{2}N}{dp_{T}dy}_{") + yRangeTitle + "} [1/";
  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  ytitle+=" MeV/c]";
  
  TString name;
  name = "projPtBoltzmann_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "#frac{1}{p_{T} E} weighted projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

  TH1D* proj = PTools::getTH1D(name,title,
			       nbinsy,
			       hist->GetYaxis()->GetXmin(),
			       hist->GetYaxis()->GetXmax(),
			       xtitle,
			       ytitle);

  TH1D* projErr2 = PTools::getTH1D(name+"_Err2",title,
 				   nbinsy,
				   hist->GetYaxis()->GetXmin(),
				   hist->GetYaxis()->GetXmax(),
				   xtitle,
				   ytitle);
  proj->Sumw2();

  Double_t weight=1.;
  Double_t y=-1.;
  Double_t pt=-1.;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      y = hist->GetXaxis()->GetBinCenter(binx);
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (binx >= ymin && binx <= ymax)
		{
		  if (pt) {weight = 1./(pt*sqrt(pt*pt+mass*mass)*cosh(y));} // calculate weight
		  else {weight = 0;}

		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);

		  proj->Fill(pt,value*weight);
		  projErr2->Fill(pt,error*error*weight*weight);
		}
	    }
	}
    }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToMtInBoltzmannRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToPtInBolzmannRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPtInBolzmannRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": m_{T}";
  ytitle = TString("#frac{1}{m_{T} E }#frac{d^{2}N}{dm_{T}dy}_{") + yRangeTitle + "} ";//[1/";
//   ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
//   ytitle+=" MeV/c]";
  
  TString name;
  name = "projMtBoltzmann_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "#frac{1}{m_{T} E} weighted m_{T} projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

    Double_t *arrayMt = new Double_t[nbinsy+1];
    //    Double_t yaverage = 0.5*(hist->GetXaxis()->GetBinUpEdge(ymax) - hist->GetXaxis()->GetBinLowEdge(ymin));
    for (Int_t binIter=0; binIter<=nbinsy; binIter++)
      {
	Double_t pt=hist->GetYaxis()->GetBinLowEdge(binIter+1);
	arrayMt[binIter]= sqrt(pt*pt+mass*mass);
      }
    
    TH1D* proj = new TH1D(name,title, nbinsy, arrayMt);
    PTools::setTitleArts(proj,xtitle, ytitle);
    
    TH1D* projErr2 = new TH1D(name+"_Err2",title, nbinsy, arrayMt);
    PTools::setTitleArts(projErr2,xtitle, ytitle);
    
    proj->Sumw2();
    
    Double_t weight=1.;
    Double_t y=-1.;
    Double_t pt=-1.;
    Double_t mt=-1.;
    
    for (binz=0;binz<=nbinsz+1;binz++)
      {
	for (biny=0;biny<=nbinsy+1;biny++)
	  {
	    pt = hist->GetYaxis()->GetBinCenter(biny);
	    for (binx=0;binx<=nbinsx+1;binx++)
	      {
		y = hist->GetXaxis()->GetBinCenter(binx);
		bin = hist->GetBin(binx,biny,binz);
		
		if (binx >= ymin && binx <= ymax)
		  {
		    mt=sqrt(pt*pt+mass*mass);
		    
		    weight = 1./(mt*mt*cosh(y)); // calculate weight
		
		    value = hist->GetBinContent(bin);
		    error = hist->GetBinError(bin);
		    
		    proj->Fill(mt,value*weight);
		    projErr2->Fill(mt,error*error*weight*weight);
		  }
	      }
	  }
      }
    
    PTools::sqrt(projErr2);
    PTools::setHistErrors(proj,projErr2);
    
    delete projErr2;
    delete arrayMt;
    return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToMtInInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": m_{T}";
  ytitle = TString("#frac{1}{m_{T}}#frac{d^{2}N}{dm_{T}dy}_{") + yRangeTitle + "}";// [1/";
  //  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  //  ytitle+=" MeV/c]";
  
  TString name;
  name = "projMtInvariant_";
  name= name+ hist->GetName() + "_" + yRangeName;
  TString title; 
  title = "#frac{1}{m_{T}} weighted m_{T} projection of: ";
  title = title + hist->GetTitle() + " in range: " + yRangeTitle;
  
  Double_t *arrayMt = new Double_t[nbinsy+1];
  //  Double_t yaverage = 0.5*(hist->GetXaxis()->GetBinUpEdge(ymax) - hist->GetXaxis()->GetBinLowEdge(ymin));
  for (Int_t binIter=0; binIter<=nbinsy; binIter++)
    {
      Double_t pt=hist->GetYaxis()->GetBinLowEdge(binIter+1);
      arrayMt[binIter]= sqrt(pt*pt+mass*mass);
    }
  
  TH1D* proj = new TH1D(name,title, nbinsy, arrayMt);
  PTools::setTitleArts(proj,xtitle, ytitle);
  
  TH1D* projErr2 = new TH1D(name+"_Err2",title, nbinsy, arrayMt);
  PTools::setTitleArts(projErr2,xtitle, ytitle);
  
  proj->Sumw2();
  
  Double_t weight=1.;
  Double_t pt=-1.;
  Double_t mt=-1.;
  
  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (binx >= ymin && binx <= ymax)
		{
		  mt=sqrt(pt*pt+mass*mass);
		  
		  weight = 1./(mt); // calculate weight
		  
		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);
		  
		  proj->Fill(mt,value*weight);
		  projErr2->Fill(mt,error*error*weight*weight);
		}
	    }
	}
      }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  delete arrayMt;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToMtM0InInvariantRepresentation(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": m_{T}-m_{0}";
  ytitle = TString("#frac{1}{m_{T}}#frac{d^{2}N}{dm_{T}dy}_{") + yRangeTitle + "}";// [1/";
  //  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  //  ytitle+=" MeV/c]";
  
  TString name;
  name = "projMtM0Invariant_";
  name= name+ hist->GetName() + "_" + yRangeName;
  TString title; 
  title = "#frac{1}{m_{T}} weighted m_{T} projection of: ";
  title = title + hist->GetTitle() + " in range: " + yRangeTitle;
  
  Double_t *arrayMt = new Double_t[nbinsy+1];
  for (Int_t binIter=0; binIter<=nbinsy; binIter++)
    {
      Double_t pt=hist->GetYaxis()->GetBinLowEdge(binIter+1);
      arrayMt[binIter]= sqrt(pt*pt+mass*mass)-mass;
    }
  
  TH1D* proj = new TH1D(name,title, nbinsy, arrayMt);
  PTools::setTitleArts(proj,xtitle, ytitle);
  
  TH1D* projErr2 = new TH1D(name+"_Err2",title, nbinsy, arrayMt);
  PTools::setTitleArts(projErr2,xtitle, ytitle);
  
  proj->Sumw2();
  
  Double_t weight=1.;
  Double_t pt=-1.;
  Double_t mt=-1.;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  pt = hist->GetYaxis()->GetBinCenter(biny);
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (binx >= ymin && binx <= ymax)
		{
		  mt=sqrt(pt*pt+mass*mass);
		  
		  weight = 1./(mt-mass); // calculate weight
		  
		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);
		  
		  proj->Fill(mt-mass,value*weight);
		  projErr2->Fill(mt-mass,error*error*weight*weight);
		}
	    }
	}
      }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  delete arrayMt;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToMtM0(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToMtM0"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToMtM0"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": m_{T}-m_{0}";
  ytitle = TString("#frac{1}{m_{T}}#frac{d^{2}N}{dm_{T}dy}_{") + yRangeTitle + "}";// [1/";
  //  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  //  ytitle+=" MeV/c]";
  
  TString name;
  name = "projMtM0Invariant_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "#frac{1}{m_{T}} weighted m_{T}-m_{0} projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

    Double_t *arrayMt = new Double_t[nbinsy+1];
    for (Int_t binIter=0; binIter<=nbinsy; binIter++)
      {
	Double_t pt=hist->GetYaxis()->GetBinLowEdge(binIter+1);
	arrayMt[binIter]= sqrt(pt*pt+mass*mass)-mass;
      }

    TH1D* proj = new TH1D(name,title, nbinsy, arrayMt);
    PTools::setTitleArts(proj,xtitle, ytitle);

    TH1D* projErr2 = new TH1D(name+"_Err2",title, nbinsy, arrayMt);
    PTools::setTitleArts(projErr2,xtitle, ytitle);

    proj->Sumw2();
    
    Double_t weight=1.;
    Double_t pt=-1.;
    Double_t mt=-1.;
    
    for (binz=0;binz<=nbinsz+1;binz++)
      {
	for (biny=0;biny<=nbinsy+1;biny++)
	  {
	    pt = hist->GetYaxis()->GetBinCenter(biny);
	    for (binx=0;binx<=nbinsx+1;binx++)
	      {
		bin = hist->GetBin(binx,biny,binz);
		
		if (binx >= ymin && binx <= ymax)
		  {
		    mt=sqrt(pt*pt+mass*mass);

		    weight = 1; // calculate weight
		    
		    value = hist->GetBinContent(bin);
		    error = hist->GetBinError(bin);
		    
		    proj->Fill(mt-mass,value*weight);
		    projErr2->Fill(mt-mass,error*error*weight*weight);
		  }
	      }
	  }
      }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  delete arrayMt;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToMt(TH2* hist, Int_t ymin, Int_t ymax, Double_t mass)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the boltzmann weighted (1/ptE = 1/(pt*sqrt(pt^2+mass^2)) projection of hist in a bin range ymin to ymax on the x-Axis
  // if pt = 0 the weight is also set to 0
  if (!hist)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToMtInInvariantRepresentation"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = hist->GetNbinsZ();
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString yRangeTitle = "";
  TString yRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  yRangeTitle = "y: ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeTitle+=" - ";
  yRangeTitle+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  yRangeName = "y:";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinLowEdge(ymin));
  yRangeName+="-";
  yRangeName+=Form("%1.1f",hist->GetXaxis()->GetBinUpEdge(ymax));

  xtitle = yRangeTitle+": m_{T}";
  ytitle = TString("#frac{1}{m_{T}}#frac{d^{2}N}{dm_{T}dy}_{") + yRangeTitle + "}";// [1/";
  //  ytitle+= Form("%2.1f",(hist->GetYaxis()->GetXmax()-hist->GetYaxis()->GetXmin())/nbinsy);
  //  ytitle+=" MeV/c]";
  
  TString name;
  name = "projMt_";
    name= name+ hist->GetName() + "_" + yRangeName;
    TString title; 
    title = "#frac{1}{m_{T}} weighted m_{T} projection of: ";
    title = title + hist->GetTitle() + " in range: " + yRangeTitle;

    Double_t *arrayMt = new Double_t[nbinsy+1];
    //    Double_t yaverage = 0.5*(hist->GetXaxis()->GetBinUpEdge(ymax) - hist->GetXaxis()->GetBinLowEdge(ymin));
    for (Int_t binIter=0; binIter<=nbinsy; binIter++)
      {
	Double_t pt=hist->GetYaxis()->GetBinLowEdge(binIter+1);
	arrayMt[binIter]= sqrt(pt*pt+mass*mass);
      }

    TH1D* proj = new TH1D(name,title, nbinsy, arrayMt);
    PTools::setTitleArts(proj,xtitle, ytitle);

    TH1D* projErr2 = new TH1D(name+"_Err2",title, nbinsy, arrayMt);
    PTools::setTitleArts(projErr2,xtitle, ytitle);

    proj->Sumw2();
    
    Double_t weight=1.;
    Double_t pt=-1.;
    Double_t mt=-1.;
    
    for (binz=0;binz<=nbinsz+1;binz++)
      {
	for (biny=0;biny<=nbinsy+1;biny++)
	  {
	    pt = hist->GetYaxis()->GetBinCenter(biny);
	    for (binx=0;binx<=nbinsx+1;binx++)
	      {
		bin = hist->GetBin(binx,biny,binz);
		
		if (binx >= ymin && binx <= ymax)
		  {
		    mt=sqrt(pt*pt+mass*mass);

		    weight = 1.; // calculate weight
		    
		    value = hist->GetBinContent(bin);
		    error = hist->GetBinError(bin);
		    
		    proj->Fill(mt,value*weight);
		    projErr2->Fill(mt,error*error*weight*weight);
		  }
	      }
	  }
      }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  delete arrayMt;
  return proj;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

TH1D* MPTools::projectPtYDownToY(TH2* hist, Int_t ptmin, Int_t ptmax)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a histogram is returned
  // that is the projection of hist in a bin range ptmin to ptmax on the x-Axis
  if (!hist)
    {
      ::Error("projectPtYDownToY"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToY"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbinsx = hist->GetNbinsX();
  Int_t nbinsy = hist->GetNbinsY();
  Int_t nbinsz = 0;
  
  nbinsz = -1;

  Int_t binx,biny,binz,bin;

  Double_t value, error;

  TString ptRangeTitle = "";
  TString ptRangeName = "";
  TString xtitle = "";
  TString ytitle = "";

  ptRangeTitle = "p_{T}: ";
  ptRangeTitle+=Form("%1.1f",hist->GetYaxis()->GetBinLowEdge(ptmin));
  ptRangeTitle+=" - ";
  ptRangeTitle+=Form("%1.1f",hist->GetYaxis()->GetBinUpEdge(ptmax));

  ptRangeName = "p_{T}:";
  ptRangeName+=Form("%1.1f",hist->GetYaxis()->GetBinLowEdge(ptmin));
  ptRangeName+="-";
  ptRangeName+=Form("%1.1f",hist->GetYaxis()->GetBinUpEdge(ptmax));

  xtitle = ptRangeTitle+": "+hist->GetXaxis()->GetTitle();

  ytitle = TString("#frac{d^{2}N}{dp_{T}dy}_{") + ptRangeTitle + "} [1/";
  ytitle+=Form("%2.1f",(hist->GetXaxis()->GetXmax()-hist->GetXaxis()->GetXmin())/nbinsx);
  ytitle+=" ]";
  
  TString name;
  name = "projY_";
  name= name+ hist->GetName() + "_" + ptRangeName;
  TString title; 
  title = "projection of: ";
  title = title + hist->GetTitle() + " in range: " + ptRangeTitle;

  TH1D* proj = PTools::getTH1D(name,title,
			       nbinsx,
			       hist->GetXaxis()->GetXmin(),
			       hist->GetXaxis()->GetXmax(),
			       xtitle,
			       ytitle);

  TH1D* projErr2 = PTools::getTH1D(name+"_Err2",title,
 				   nbinsx,
				   hist->GetXaxis()->GetXmin(),
				   hist->GetXaxis()->GetXmax(),
				   xtitle,
				   ytitle);
  proj->Sumw2();

  Double_t weight=1.;
  Double_t y=-1.;

  for (binz=0;binz<=nbinsz+1;binz++)
    {
      for (biny=0;biny<=nbinsy+1;biny++)
	{
	  for (binx=0;binx<=nbinsx+1;binx++)
	    {
	      y = hist->GetXaxis()->GetBinCenter(binx);
	      bin = hist->GetBin(binx,biny,binz);
	      
	      if (biny >= ptmin && biny <= ptmax)
		{
		  weight = 1.;

		  value = hist->GetBinContent(bin);
		  error = hist->GetBinError(bin);

		  proj->Fill(y,value*weight);
		  projErr2->Fill(y,error*error*weight*weight);
		}
	    }
	}
    }
  
  PTools::sqrt(projErr2);
  PTools::setHistErrors(proj,projErr2);

  delete projErr2;
  return proj;
}

////////////////////////////////////////////////////////////////////////////////

TObjArray* MPTools::projectPtYDownToPtScaled(TH2* hist, Double_t scaleFactor, Int_t stepSize)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a TObjArray of histograms is returned
  // where each histogram is the projection of hist from first bin for "stepSize" bins together,
  // default: stepsize = 1, i.e for each bin allone
  // beginning from the second histogram, all histograms are scaled by scaleFactor with respect to the previous histogram
  //   i.e. the (n+1th) histogramm is scaled by scaleFactor to the nth power
  //   if scale factor is negativ than the scaling order is reversed, i.e the first histogramm is scaled most
 
 if (!hist)
    {
      ::Error("projectPtYDownToPtScaled"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPtScaled"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbins = hist->GetNbinsX();
  Int_t steps = (Int_t) (nbins/stepSize);

  if ((steps*stepSize) < nbins) 
    {
	::Info("projectPtYDownToPt",
	   "generating %i histograms, but last histogram, covers only %i bins instead of %i "
	   ,steps,nbins-steps*stepSize, stepSize);
      steps++;
    }
  else
    {
	::Info("projectPtYDownToPt","generating %i histograms",steps);
    }
  if (scaleFactor !=1)
    {
	::Info ("projectPtYDownToPt","hists are scaled by (%.0f^n) %s",fabs(scaleFactor),scaleFactor>0?"":"in reversed order");
    }

  TObjArray* hists = new TObjArray(steps);
  TH1D* temp;
  TString name;
  TString title;
  TString scale;
  for (Int_t index = 0; index < steps; index+=stepSize)
    {
      temp = projectPtYDownToPt(hist, index+1, index+stepSize);
      // scaling
      if (scaleFactor != 1.)
	{
	  name = temp->GetName();
	  title = temp->GetTitle();
	  scale = "_scaled_by_";
	  scale += Form("%.0f",fabs(scaleFactor));
	  scale += "_^";
	  if (scaleFactor > 0)
	    {
	      temp->Scale(pow(fabs(scaleFactor),index));
	      scale += Form("%i",index); 
	    }
	  else
	    {
	      temp->Scale(pow(fabs(scaleFactor),steps-index));
	      scale += Form("%i",steps-index); 
	    }
	  name+=scale;
	  title+=scale;
	  temp->SetNameTitle(name.Data(),title.Data());
	}
      temp->SetLineColor(PTools::getColorJM(index+1));
      temp->SetMarkerColor(PTools::getColorJM(index+1));
      temp->SetMarkerStyle(PTools::getMarkerJM(index));
      hists->AddAt(temp,index);
    }
  
  return hists;
  
}

////////////////////////////////////////////////////////////////////////////////

TObjArray* MPTools::projectPtYDownToPt(TH2* hist, Int_t stepSize)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a TObjArray of histograms is returned
  // where each histogram is the projection of hist from first bin for "stepSize" bins together,
  // default: stepSize = 1, i.e for each bin allone
  // beginning from the second histogram, all histograms are scaled by scaleFactor with respect to the previous histogram
  //   i.e. the (n+1th) histogramm is scaled by scaleFactor to the nth power
  //   if scale factor is negativ than the scaling order is reversed, i.e the first histogramm is scaled most
 
 if (!hist)
    {
      ::Error("projectPtYDownToPt"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToPt"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  return projectPtYDownToPtScaled(hist, 1., stepSize);
}

////////////////////////////////////////////////////////////////////////////////

TObjArray* MPTools::projectPtYDownToYScaled(TH2* hist, Double_t scaleFactor, Int_t stepSize)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a TObjArray of histograms is returned
  // where each histogram is the projection of hist from first bin for "stepSize" bins together,
  // default: stepsize = 1, i.e for each bin allone
  // beginning from the second histogram, all histograms are scaled by scaleFactor with respect to the previous histogram
  //   i.e. the (n+1th) histogramm is scaled by scaleFactor to the nth power
  //   if scale factor is negativ than the scaling order is reversed, i.e the first histogramm is scaled most
 
 if (!hist)
    {
      ::Error("projectPtYDownToYScaled"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToYScaled"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  Int_t nbins = hist->GetNbinsY();
  Int_t steps = (Int_t) (nbins/stepSize);

  if ((steps*stepSize) < nbins) 
    {
	::Info("projectPtYDownToY",
	   "generating %i histograms, but last histogram, covers only %i bins instead of %i "
	   ,steps,nbins-steps*stepSize, stepSize);
      steps++;
    }
  else
    {
	::Info("projectPtYDownToY","generating %i histograms",steps);
    }
  if (scaleFactor !=1)
    {
	::Info("projectPtYDownToY","hists are scaled by (%.0f^n) %s",fabs(scaleFactor),scaleFactor>0?"":"in reversed order");
    }

  TObjArray* hists = new TObjArray(steps);
  TH1D* temp;
  TString name;
  TString title;
  TString scale;
  for (Int_t index = 0; index < steps; index+=stepSize)
    {
      temp = projectPtYDownToY(hist, index+1, index+stepSize);
      // scaling
      if (scaleFactor != 1.)
	{
	  name = temp->GetName();
	  title = temp->GetTitle();
	  scale = "_scaled_by_";
	  scale += Form("%.0f",fabs(scaleFactor));
	  scale += "_^";
	  if (scaleFactor > 0)
	    {
	      temp->Scale(pow(fabs(scaleFactor),index));
	      scale += Form("%i",index); 
	    }
	  else
	    {
	      temp->Scale(pow(fabs(scaleFactor),steps-index));
	      scale += Form("%i",steps-index); 
	    }
	  name+=scale;
	  title+=scale;
	  temp->SetNameTitle(name.Data(),title.Data());
	}
      temp->SetLineColor(PTools::getColorJM(index+1));
      temp->SetMarkerColor(PTools::getColorJM(index+1));
      temp->SetMarkerStyle(PTools::getMarkerJM(index));
      hists->AddAt(temp,index);
    }
  
  return hists;
  
}

////////////////////////////////////////////////////////////////////////////////

TObjArray* MPTools::projectPtYDownToY(TH2* hist, Int_t stepSize)
{
  // if hist is NULL ... NULL is returned
  // otherwise
  // a TObjArray of histograms is returned
  // where each histogram is the projection of hist from first bin for "stepSize" bins together,
  // default: stepSize = 1, i.e for each bin allone
  // beginning from the second histogram, all histograms are scaled by scaleFactor with respect to the previous histogram
  //   i.e. the (n+1th) histogramm is scaled by scaleFactor to the nth power
  //   if scale factor is negativ than the scaling order is reversed, i.e the first histogramm is scaled most
 
 if (!hist)
    {
      ::Error("projectPtYDownToY"," hist is NULL pointer ... cannot act on a NULL pointer.");
      return NULL;
    }

  if (hist->GetDimension() != 2)
    {
      ::Error("projectPtYDownToY"," can only project 2-dimensional histograms ... returning NULL pointer.");
      return NULL;
    }

  return projectPtYDownToYScaled(hist, 1., stepSize);
}

////////////////////////////////////////////////////////////////////////////////

