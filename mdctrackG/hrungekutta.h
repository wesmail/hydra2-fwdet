//*-- Authors: A.Ivashkin && A.Sadovsky (04.11.2004)
#ifndef HRUNGEKUTTA_H
#define HRUNGEKUTTA_H

#include "TString.h"
#include "TObject.h"
#include "TNamed.h"
#include "TMatrixD.h"
#include "hgeomvector.h"
#include "hgeomrotation.h"
#include "hgeomtransform.h"
#include "hmdctrackgfield.h"

class HMdcSizesCells;

class HRungeKutta {
 private:
   HMdcTrackGField* fieldMap;              // pointer to field map
   Double_t fieldScalFact;                 // factor of the magnetic field strength
   Bool_t mdcInstalled[4][6];              // remembers which MDCs have known geometry
   Double_t mdcPos[4][6][3];               // real position of mdc's in sector coordinate system
   Double_t normVecMdc[4][6][3];           // normal vector on each Mdc module in sector coordinate system
   Double_t pointForVertexRec[6][3];       // point on virtual plane in front of MDC1 used for vertex reconstruction
   Double_t dzfacMdc[2][6];                // factor to calculate dz from dy in the first two MDC planes
   Double_t p0Guess;                       // initial momentum

   //--------------------------------- RK-tracking ----------------------------
   Float_t  initialStepSize;               // initial RK step size
   Float_t  stepSizeInc;                   // factor to increase stepsize
   Float_t  stepSizeDec;                   // factor to decrease stepsize
   Float_t  maxStepSize;                   // maximum stepsize
   Float_t  minStepSize;                   // minimum stepsize
   Float_t  minPrecision;                  // minimum required precision for a single step
   Float_t  maxPrecision;                  // maximum required precision for a single step
   Int_t    maxNumSteps;                   // maximum number of steps
   Double_t maxDist;                       // maximum distance for straight line approximation
   Float_t  pfit;                          // momentum after RK-fit
   Float_t  chi2;                          // normalized chi^2 after RK-fit
   Float_t  trackLength;                   // track length
   Float_t  dpar[5];                       // variations (momentum, xdir, ydir, xpos, ypos)
   Float_t  resolution[8];                 // resolution
   Float_t  sig[8];                        // sigmas for hit points
   Double_t hit[4][3],fit[4][3];           // hits and fitted points at mdc's
   Double_t fitdp[4][3],fitdh[4][3],fitdv[4][3],fit1h[4][3],fit1v[4][3]; // fitted positions for derivatives
   Int_t    mdcModules[4];                 // module number in hit/fit
   Int_t    mdcLookup[4];                  // index in mdcModules for all 4 modules
   Double_t dydpar[5][8];                  // derivatives
   Double_t ydata[8];                      // difference between fitted and measured hit positions
   Double_t fitpar[5];                     // fit parameters
   Int_t    ndf;                           // number of degrees of freedom
   Int_t    nMaxMod;                       // maximum hit module number (3 or 4)
   Int_t    sector;                        // sector index
   Double_t dirAtFirstMDC[3];              // fitted direction at first MDC
   Double_t posNearLastMDC[3];             // fitted direction at last MDC
   Double_t dirAtLastMDC[3];               // fitted direction at last MDC
   Float_t  stepSizeAtLastMDC;             // RK step size at last MDC
   Float_t  trackLengthAtLastMDC;          // track length from target to last MDC
   Double_t trackPosOnMETA[3];             // fitted trajectory point on META
   Int_t    jstep;                         // step number
   Double_t zSeg1,rSeg1,thetaSeg1,phiSeg1; // fitted data for inner segment
   Double_t zSeg2,rSeg2,thetaSeg2,phiSeg2; // fitted data for outer segment
   Float_t  polbending;                    // polarity of bending (field scaling factor * pfit)
   Double_t mTol;                          //! tolerance for invertion of matrix (default : DBL_EPSILON)
public:
   HRungeKutta();
   virtual ~HRungeKutta() {}
   void clear();
   void setField(HMdcTrackGField*,Float_t);
   void setFieldFactor(Float_t fpol) {fieldScalFact = fpol;}

   void setMdcPosition(Int_t,Int_t,HGeomTransform&);
   Bool_t fit4Hits(Double_t*,Double_t*,Double_t*,Double_t*,Float_t*,Int_t,Double_t pGuess=999999999.);
   Bool_t fit3Hits(Double_t*,Double_t*,Double_t*,Double_t*,Float_t*,Int_t,Double_t pGuess=999999999.);
   void traceToVertex(HMdcSizesCells*);
   void traceToMETA(HGeomVector&,HGeomVector&,HGeomVector* point = NULL,HGeomVector* norm = NULL);

   Float_t getPfit() {return pfit;}
   Float_t getChi2() {return chi2/Float_t(ndf);}
   Int_t getNMaxMod() {return nMaxMod;}

   // fitted points on MDC in Segment Coord System, Mdc={1,2,3,4}
   Float_t getXfit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1) ? fit[(mdcLookup[iMdc-1])][0] : -999.;
   }
   Float_t getYfit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1) ? fit[(mdcLookup[iMdc-1])][1] : -999.;
   }
   Float_t getZfit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1) ? fit[(mdcLookup[iMdc-1])][2] : -999.;
   }

   // hit points on MDC (from segments) in Segment Coord System, Mdc={1,2,3,4}
   Float_t getXhit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1) ? hit[(mdcLookup[iMdc-1])][0] : -999.;
   }
   Float_t getYhit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1) ? hit[(mdcLookup[iMdc-1])][1] : -999.;
   }

   // difference between of fitted points and hit points
   Float_t getXfithit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1)
       ? (fit[(mdcLookup[iMdc-1])][0] - hit[(mdcLookup[iMdc-1])][0]) : -999.;
   }
   Float_t getYfithit(Int_t iMdc) {
     return (mdcLookup[iMdc-1]!=-1)
       ? (fit[(mdcLookup[iMdc-1])][1] - hit[(mdcLookup[iMdc-1])][1]) : -999.;
   }

   // position and direction on first MDC after RungeKutta fit
   Float_t getXtrackFirstMDCFitPos()  { return fit[0][0]; }        // x
   Float_t getYtrackFirstMDCFitPos()  { return fit[0][1]; }        // y
   Float_t getZtrackFirstMDCFitPos()  { return fit[0][2]; }        // z
   Float_t getDXtrackFirstMDCFitPos() { return dirAtFirstMDC[0]; } // dx
   Float_t getDYtrackFirstMDCFitPos() { return dirAtFirstMDC[1]; } // dy
   Float_t getDZtrackFirstMDCFitPos() { return dirAtFirstMDC[2]; } // dz

   // position and direction on last MDC after RungeKutta fit
   Float_t getXtrackLastMDCFitPos()  { return fit[nMaxMod-1][0]; } // x
   Float_t getYtrackLastMDCFitPos()  { return fit[nMaxMod-1][1]; } // y
   Float_t getZtrackLastMDCFitPos()  { return fit[nMaxMod-1][2]; } // z
   Float_t getDXtrackLastMDCFitPos() { return dirAtLastMDC[0]; }   // dx
   Float_t getDYtrackLastMDCFitPos() { return dirAtLastMDC[1]; }   // dy
   Float_t getDZtrackLastMDCFitPos() { return dirAtLastMDC[2]; }   // dz

   // position and direction on META after RungeKutta fit
   Float_t getXtrackOnMETA()  { return trackPosOnMETA[0];} // x
   Float_t getYtrackOnMETA()  { return trackPosOnMETA[1];} // y
   Float_t getZtrackOnMETA()  { return trackPosOnMETA[2];} // z

   Float_t getTrackLength() { return trackLength; } // RK-track length

   Float_t getZSeg1() { return zSeg1; }         // z of inner segment after RK-fit
   Float_t getRSeg1() { return rSeg1; }         // r of inner segment after RK-fit
   Float_t getThetaSeg1() { return thetaSeg1; } // theta of inner segment after RK-fit
   Float_t getPhiSeg1() { return phiSeg1; }     // phi of inner segment after RK-fit

   Float_t getZSeg2() { return zSeg2; }         // z of outer segment after RK-fit
   Float_t getRSeg2() { return rSeg2; }         // r of outer segment after RK-fit
   Float_t getThetaSeg2() { return thetaSeg2; } // theta of outer segment after RK-fit
   Float_t getPhiSeg2() { return phiSeg2; }     // phi of outer segment after RK-fit

   void     setMTol(Double_t tol) { mTol=tol; }
   Double_t getMTol()             { return mTol; }

 private:
   Bool_t   fitMdc();
   void     findMdcIntersectionPoint(Double_t*,Double_t*,Int_t,Double_t*);
   Bool_t   findIntersectionPoint(Double_t*,Double_t*,Double_t*,Double_t*,Double_t*);
   void     initMom();
   Float_t  distance(Double_t*,Double_t*);
   void     parSetNew0(Float_t,Float_t,Int_t);
   void     parSetNewVar(Float_t,Float_t);
   void     cosines(Float_t,Float_t,Double_t*);
   void     gentrkNew(Float_t,Double_t*,Double_t*,Double_t*,Int_t);
   Float_t  rkgtrk(Float_t,Float_t,Double_t*,Double_t*,Int_t kind=0);
   void     rkeqfw(Double_t*,Float_t,Double_t*,Float_t*);
   void     derive(Double_t*,Int_t);
   Bool_t   linSys();
   Bool_t   decompose(TMatrixD &lu,Double_t &sign,Double_t tol,Int_t &nrZeros);
   ClassDef(HRungeKutta,0)
};


#endif
