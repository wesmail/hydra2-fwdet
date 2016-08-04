//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : RICH team member
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichAnalysisPar
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHANALYSISPAR_H
#define HRICHANALYSISPAR_H

#include "TArrayI.h"
#include "TArrayF.h"

#include "hparcond.h"

class HParamList;

class HRichAnalysisPar: public HParCond {

private:
   HRichAnalysisPar(const HRichAnalysisPar& source);
   HRichAnalysisPar& operator=(const HRichAnalysisPar& source);

public:

   Int_t iCutOffThresheold;

   Int_t iRingRadius;
   Int_t iRingRadiusError;

   Int_t iRingMatrixSize;
   TArrayI iRingMatrix;
   Int_t iRingMaskSize;
   TArrayI iRingMask;

// ---- parameters for pad cleaning ----

   Int_t isActiveCleanAlonePad;
   Int_t iCleanAlonePadBorder;
   Int_t iCleanAlonePadLowerThreshold;

   Int_t isActiveCleanHighPulse;
   Int_t iCleanHighPulseBorder;
   Int_t iCleanHighPulseUpperThreshold;

// ---- parameters for pad labeling ----

   Int_t isActiveLabelPads;
   Int_t iLabeledPadsDistance;

// ---- parameters for ring finding ----

   Int_t maxFiredSectorPads;

   Int_t isActiveBorderAmplitReduction;

   Int_t isActiveTestDensity;
   Float_t fThresholdDensity;
   Float_t fSurfaceArea;

   Int_t isActiveDynamicThrAmplitude;
   Float_t fLowerAmplFactor;
   Float_t fFormulaParam1;
   Float_t fFormulaParam2;
   Float_t fFormulaParam3;

   Int_t isActiveFiredRingPadsRatio;
   Float_t fFiredRingPadsRatio;

   Int_t isActiveTestAsymmetry;

   Int_t isActiveTestCharge;
   Float_t fRingMinCharge;
   Float_t fRingMaxCharge;

   Int_t isActiveFakesRejection;
   Float_t fFakeQualityRatio;//minimal quality for a fake ring found in the nearhodd of a bright rone.
   Float_t fFakeCentroidCut;//minimal shift of the centroid of the fake ring respect to the centroid of the bright one.

   Int_t iSuperiorAlgorithmID;

   Int_t isActiveRingFindFitMatrix;
   Int_t iMinimalFitMatrixRingQuality;
   Int_t iMinimalFitMatrixRingDistance;
   Int_t iHowManyFitMatrixRings;

   Int_t isActiveRingHoughTransf;
   Int_t iMinimalHoughTransfRingQuality;
   Int_t iMinimalHoughTransfRingDistance;
   Int_t iHowManyHoughTransfRings;

   TArrayF fAmpCorrFac;  // amplitude correction factor for each sector

private:

   Char_t filename[40];

public:

   HRichAnalysisPar(const Char_t* name = "RichAnalysisParameters",
                    const Char_t* title = "Rich Analysis Parameters",
                    const Char_t* context = "RichAnaNormalBias");
   virtual ~HRichAnalysisPar();


   void clear();
   void defaultInit();
   void printParams();
   void putParams(HParamList* l);
   Bool_t getParams(HParamList* l);
   void setFileName(const Char_t* fn) {
      strncpy(filename, fn, 40);
   }

   ClassDef(HRichAnalysisPar, 2)
};


#endif // HRICHANALYSISPAR_H


