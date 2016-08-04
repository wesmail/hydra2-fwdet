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


#include "hparamlist.h"
#include "hrichanalysispar.h"

#include <iomanip>
#include <iostream>
#include <string.h>

using namespace std;

ClassImp(HRichAnalysisPar)


//----------------------------------------------------------------------------
HRichAnalysisPar::HRichAnalysisPar(const Char_t* name, const Char_t* title,
                                   const Char_t* context)
   : HParCond(name, title, context)
{
   clear();
}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysisPar::~HRichAnalysisPar()
{
   clear();
}
//============================================================================

//----------------------------------------------------------------------------
void HRichAnalysisPar::clear()
{

   iCutOffThresheold = 0;
   iRingMatrixSize = 0;
   iRingMaskSize = 0;
   iRingRadius = 0;
   iRingRadiusError = 0;

   isActiveCleanAlonePad = 0;
   iCleanAlonePadBorder = 0;
   iCleanAlonePadLowerThreshold = 0;

   isActiveCleanHighPulse = 0;
   iCleanHighPulseBorder = 0;
   iCleanHighPulseUpperThreshold = 0;

   maxFiredSectorPads = 0;
   isActiveBorderAmplitReduction = 0;

   isActiveLabelPads = 0;
   iLabeledPadsDistance = 0;
   isActiveTestAsymmetry = 0;

   isActiveTestDensity = 0;
   fThresholdDensity = 0.;
   fSurfaceArea = 0.;

   isActiveDynamicThrAmplitude = 0;
   fLowerAmplFactor = 0.;
   fFormulaParam1 = 0.;
   fFormulaParam2 = 0.;
   fFormulaParam3 = 0.;

   isActiveFiredRingPadsRatio = 0;
   fFiredRingPadsRatio = 0.;

   isActiveTestCharge = 0;
   fRingMinCharge = 0.;
   fRingMaxCharge = 0.;

   isActiveFakesRejection = 0;
   fFakeQualityRatio = 0.;
   fFakeCentroidCut = 0.;

   iSuperiorAlgorithmID = 0;

   isActiveRingFindFitMatrix = 0;
   iMinimalFitMatrixRingQuality = 0;
   iMinimalFitMatrixRingDistance = 0;
   iHowManyFitMatrixRings = 0;

   isActiveRingHoughTransf = 0;
   iMinimalHoughTransfRingQuality = 0;
   iMinimalHoughTransfRingDistance = 0;
   iHowManyHoughTransfRings = 0;

   fAmpCorrFac.Set(6);
   fAmpCorrFac.Reset(1.F);

}
//============================================================================

//----------------------------------------------------------------------------
void HRichAnalysisPar::defaultInit()
{

// so far dummy
   clear();
}
//============================================================================
/*
//----------------------------------------------------------------------------
HRichAnalysisPar::HRichAnalysisPar(const HRichAnalysisPar& source)
{
   cerr << "HRichAnalysisPar object can not be initialized with values of another object!\n";
   cerr << "Default constructor will be called.\n";
   HRichAnalysisPar();
}
//============================================================================

//----------------------------------------------------------------------------
HRichAnalysisPar& HRichAnalysisPar::operator=(const HRichAnalysisPar& source)
{
   if (this != &source) {
      cerr << "HRichAnalysisPar object can not be assigned!\n";
      cerr << "Default constructor will be called.\n";
      HRichAnalysisPar();
   }
   return *this;
}
//============================================================================
*/
//----------------------------------------------------------------------------
void HRichAnalysisPar::printParams()
{

   cout << endl << endl;
   cout << "$RING PARAMETERS$" << endl;
   cout << "\tRing radius       = " << iRingRadius << endl;
   cout << "\tRing radius error = " << iRingRadiusError << endl << endl;

   cout << "$PAD CLEANING$" << endl;
   cout << "\tClean Alone Pad lower threshold  = " << iCleanAlonePadLowerThreshold << endl;
   cout << "\tClean High Pulse upper threshold = " << iCleanHighPulseUpperThreshold << endl << endl;

   cout << "$PATTERN MATRIX$" << endl;
   if (isActiveRingFindFitMatrix) {
      cout << "\tMinimal Ring Quality in Pattern Matrix         = " << iMinimalFitMatrixRingQuality << endl;
      cout << "\tMinimal Rings Distances in Pattern Matrix      = " << iMinimalFitMatrixRingDistance << endl;
      cout << "\tNumber of Rings to be sought by Pattern Matrix = " << iHowManyFitMatrixRings << endl << endl;
   } else {
      cout << "\t(not active)" << endl << endl;
   }

   cout << "$HOUGH TRANSFORM$" << endl;
   if (isActiveRingHoughTransf) {
      cout << "\tMinimal Ring Quality in Hough Transform         = " << iMinimalHoughTransfRingQuality << endl;
      cout << "\tMinimal Rings Distances in Hough Transform      = " << iMinimalHoughTransfRingDistance << endl;
      cout << "\tNumber of Rings to be sought by Hough Transform = " << iHowManyHoughTransfRings << endl << endl;
   } else {
      cout << "\t(not active)" << endl << endl;
   }

   cout << "$RING TESTS$" << endl;
   if (isActiveTestDensity) cout << "\tRing Test Density                 = yes" << endl;
   else cout << "\tRing Test Density                 = no" << endl;
   if (isActiveBorderAmplitReduction) cout << "\tAmplitude reduction at the border = yes" << endl;
   else cout << "\tAmplitude reduction at the border = no" << endl;
   if (isActiveDynamicThrAmplitude) cout << "\tDynamic Amplitude Threshold       = yes" << endl;
   else cout << "\tDynamic Amplitude Threshold       = no" << endl;
   if (isActiveFiredRingPadsRatio) cout << "\tFired Pads Ratio in ring          = yes" << endl;
   else cout << "\tFired Pads Ratio in ring          = no" << endl;
   if (isActiveTestAsymmetry) cout << "\tTest Ring Asymmetry               = yes" << endl;
   else cout << "\tTest Ring Asymmetry               = no" << endl;
   if (isActiveTestCharge) cout << "\tTest Ring Charge                  = yes" << endl;
   else cout << "\tTest Ring Charge                  = no" << endl;
   if (isActiveFakesRejection) cout << "\tFakes Rejection                   = yes" << endl;
   else cout << "\tFakes Rejection                   = no" << endl << endl;

   Info("printParams", "RICH ANALYSIS PARAMETERS\n");
   cout << " iCutOffThresheold.................." << iCutOffThresheold << endl;
   cout << " iRingRadius........................" << iRingRadius << endl;
   cout << " iRingMatrixSize...................." << iRingMatrixSize << endl;
   cout << " iRingMaskSize......................" << iRingMaskSize << endl;
   cout << " isActiveCleanAlonePad.............." << isActiveCleanAlonePad << endl;
   cout << " iCleanAlonePadBorder..............." << iCleanAlonePadBorder << endl;
   cout << " iCleanAlonePadLowerThreshold......." << iCleanAlonePadLowerThreshold << endl;
   cout << " isActiveCleanHighPulse............." << isActiveCleanHighPulse << endl;
   cout << " iCleanHighPulseBorder.............." << iCleanHighPulseBorder << endl;
   cout << " iCleanHighPulseUpperThreshold......" << iCleanHighPulseUpperThreshold << endl;
   cout << " isActiveLabelPads.................." << isActiveLabelPads << endl;
   cout << " iLabeledPadsDistance..............." << iLabeledPadsDistance << endl;
   cout << " maxFiredSectorPads................." << maxFiredSectorPads << endl;
   cout << " isActiveBorderAmplitReduction......" << isActiveBorderAmplitReduction << endl;
   cout << " isActiveTestDensity................" << isActiveTestDensity << endl;
   cout << " fThresholdDensity.................." << fThresholdDensity << endl;
   cout << " fSurfaceArea......................." << fSurfaceArea << endl;
   cout << " isActiveDynamicThrAmplitude........" << isActiveDynamicThrAmplitude << endl;
   cout << " fLowerAmplFactor..................." << fLowerAmplFactor << endl;
   cout << " fFormulaParam1....................." << fFormulaParam1 << endl;
   cout << " fFormulaParam2....................." << fFormulaParam2 << endl;
   cout << " fFormulaParam3....................." << fFormulaParam3 << endl;
   cout << " isActiveFiredRingPadsRatio........." << isActiveFiredRingPadsRatio << endl;
   cout << " fFiredRingPadsRatio................" << fFiredRingPadsRatio << endl;
   cout << " isActiveTestAsymmetry.............." << isActiveTestAsymmetry << endl;
   cout << " isActiveTestCharge................." << isActiveTestCharge << endl;
   cout << " fRingMinCharge....................." << fRingMinCharge << endl;
   cout << " fRingMaxCharge....................." << fRingMaxCharge << endl;
   cout << " isActiveFakesRejection............." << isActiveFakesRejection << endl;
   cout << " fFakeQualityRatio.................." << fFakeQualityRatio << endl;
   cout << " fFakeCentroidCut..................." << fFakeCentroidCut << endl;
   cout << " iSuperiorAlgorithmID..............." << iSuperiorAlgorithmID << endl;
   cout << " isActiveRingFindFitMatrix.........." << isActiveRingFindFitMatrix << endl;
   cout << " iMinimalFitMatrixRingQuality......." << iMinimalFitMatrixRingQuality << endl;
   cout << " iMinimalFitMatrixRingDistance......" << iMinimalFitMatrixRingDistance << endl;
   cout << " iHowManyFitMatrixRings............." << iHowManyFitMatrixRings << endl;
   cout << " isActiveRingHoughTransf............" << isActiveRingHoughTransf << endl;
   cout << " iMinimalHoughTransfRingQuality....." << iMinimalHoughTransfRingQuality << endl;
   cout << " iMinimalHoughTransfRingDistance...." << iMinimalHoughTransfRingDistance << endl;
   cout << " iHowManyHoughTransfRings..........." << iHowManyHoughTransfRings << endl << endl;
   cout << " fAmpCorrFac........................";
   for (Int_t i = 0; i < 6; ++i) {
     cout << " " << fAmpCorrFac.At(i);
     cout << endl << endl;
   }
}
//============================================================================

//============================================================================

void HRichAnalysisPar::putParams(HParamList* l)
{
   if (!l) return;
   l->add("iCutOffThresheold", iCutOffThresheold);
   l->add("iRingRadius", iRingRadius);
   l->add("iRingRadiusError", iRingRadiusError);
   l->add("iRingMatrixSize", iRingMatrixSize);
   l->add("iRingMatrix", iRingMatrix);
   l->add("iRingMaskSize", iRingMaskSize);
   l->add("iRingMask", iRingMask);
   l->add("isActiveCleanAlonePad", isActiveCleanAlonePad);
   l->add("iCleanAlonePadBorder", iCleanAlonePadBorder);
   l->add("iCleanAlonePadLowerThreshold", iCleanAlonePadLowerThreshold);
   l->add("isActiveCleanHighPulse", isActiveCleanHighPulse);
   l->add("iCleanHighPulseBorder", iCleanHighPulseBorder);
   l->add("iCleanHighPulseUpperThreshold", iCleanHighPulseUpperThreshold);
   l->add("isActiveLabelPads", isActiveLabelPads);
   l->add("iLabeledPadsDistance", iLabeledPadsDistance);
   l->add("maxFiredSectorPads", maxFiredSectorPads);
   l->add("isActiveBorderAmplitReduction", isActiveBorderAmplitReduction);
   l->add("isActiveTestDensity", isActiveTestDensity);
   l->add("fThresholdDensity", fThresholdDensity);
   l->add("fSurfaceArea", fSurfaceArea);
   l->add("isActiveDynamicThrAmplitude", isActiveDynamicThrAmplitude);
   l->add("fLowerAmplFactor", fLowerAmplFactor);
   l->add("fFormulaParam1", fFormulaParam1);
   l->add("fFormulaParam2", fFormulaParam2);
   l->add("fFormulaParam3", fFormulaParam3);
   l->add("isActiveFiredRingPadsRatio", isActiveFiredRingPadsRatio);
   l->add("fFiredRingPadsRatio", fFiredRingPadsRatio);
   l->add("isActiveTestAsymmetry", isActiveTestAsymmetry);
   l->add("isActiveTestCharge", isActiveTestCharge);
   l->add("fRingMinCharge", fRingMinCharge);
   l->add("fRingMaxCharge", fRingMaxCharge);
   l->add("isActiveFakesRejection", isActiveFakesRejection);
   l->add("fFakeQualityRatio", fFakeQualityRatio);
   l->add("fFakeCentroidCut", fFakeCentroidCut);
   l->add("iSuperiorAlgorithmID", iSuperiorAlgorithmID);
   l->add("isActiveRingFindFitMatrix", isActiveRingFindFitMatrix);
   l->add("iMinimalFitMatrixRingQuality", iMinimalFitMatrixRingQuality);
   l->add("iMinimalFitMatrixRingDistance", iMinimalFitMatrixRingDistance);
   l->add("iHowManyFitMatrixRings", iHowManyFitMatrixRings);
   l->add("isActiveRingHoughTransf", isActiveRingHoughTransf);
   l->add("iMinimalHoughTransfRingQuality", iMinimalHoughTransfRingQuality);
   l->add("iMinimalHoughTransfRingDistance", iMinimalHoughTransfRingDistance);
   l->add("iHowManyHoughTransfRings", iHowManyHoughTransfRings);
   l->add("fAmpCorrFac", fAmpCorrFac);
}
//============================================================================

Bool_t HRichAnalysisPar::getParams(HParamList* l)
{
   if (!l) return kFALSE;
   if (!l->fill("iCutOffThresheold", &iCutOffThresheold)) return kFALSE;
   if (!l->fill("iRingRadius", &iRingRadius)) return kFALSE;
   if (!l->fill("iRingRadiusError", &iRingRadiusError)) return kFALSE;
   if (!l->fill("iRingMatrixSize", &iRingMatrixSize)) return kFALSE;
   if (!l->fill("iRingMatrix", &iRingMatrix)) return kFALSE;
   if (!l->fill("iRingMaskSize", &iRingMaskSize)) return kFALSE;
   if (!l->fill("iRingMask", &iRingMask)) return kFALSE;
   if (!l->fill("isActiveCleanAlonePad", &isActiveCleanAlonePad)) return kFALSE;
   if (!l->fill("iCleanAlonePadBorder", &iCleanAlonePadBorder)) return kFALSE;
   if (!l->fill("iCleanAlonePadLowerThreshold", &iCleanAlonePadLowerThreshold)) return kFALSE;
   if (!l->fill("isActiveCleanHighPulse", &isActiveCleanHighPulse)) return kFALSE;
   if (!l->fill("iCleanHighPulseBorder", &iCleanHighPulseBorder)) return kFALSE;
   if (!l->fill("iCleanHighPulseUpperThreshold", &iCleanHighPulseUpperThreshold)) return kFALSE;
   if (!l->fill("isActiveLabelPads", &isActiveLabelPads)) return kFALSE;
   if (!l->fill("iLabeledPadsDistance", &iLabeledPadsDistance)) return kFALSE;
   if (!l->fill("maxFiredSectorPads", &maxFiredSectorPads)) return kFALSE;
   if (!l->fill("isActiveBorderAmplitReduction", &isActiveBorderAmplitReduction)) return kFALSE;
   if (!l->fill("isActiveTestDensity", &isActiveTestDensity)) return kFALSE;
   if (!l->fill("fThresholdDensity", &fThresholdDensity)) return kFALSE;
   if (!l->fill("fSurfaceArea", &fSurfaceArea)) return kFALSE;
   if (!l->fill("isActiveDynamicThrAmplitude", &isActiveDynamicThrAmplitude)) return kFALSE;
   if (!l->fill("fLowerAmplFactor", &fLowerAmplFactor)) return kFALSE;
   if (!l->fill("fFormulaParam1", &fFormulaParam1)) return kFALSE;
   if (!l->fill("fFormulaParam2", &fFormulaParam2)) return kFALSE;
   if (!l->fill("fFormulaParam3", &fFormulaParam3)) return kFALSE;
   if (!l->fill("isActiveFiredRingPadsRatio", &isActiveFiredRingPadsRatio)) return kFALSE;
   if (!l->fill("fFiredRingPadsRatio", &fFiredRingPadsRatio)) return kFALSE;
   if (!l->fill("isActiveTestAsymmetry", &isActiveTestAsymmetry)) return kFALSE;
   if (!l->fill("isActiveTestCharge", &isActiveTestCharge)) return kFALSE;
   if (!l->fill("fRingMinCharge", &fRingMinCharge)) return kFALSE;
   if (!l->fill("fRingMaxCharge", &fRingMaxCharge)) return kFALSE;
   if (!l->fill("isActiveFakesRejection", &isActiveFakesRejection)) return kFALSE;
   if (!l->fill("fFakeQualityRatio", &fFakeQualityRatio)) return kFALSE;
   if (!l->fill("fFakeCentroidCut", &fFakeCentroidCut)) return kFALSE;
   if (!l->fill("iSuperiorAlgorithmID", &iSuperiorAlgorithmID)) return kFALSE;
   if (!l->fill("isActiveRingFindFitMatrix", &isActiveRingFindFitMatrix)) return kFALSE;
   if (!l->fill("iMinimalFitMatrixRingQuality", &iMinimalFitMatrixRingQuality)) return kFALSE;
   if (!l->fill("iMinimalFitMatrixRingDistance", &iMinimalFitMatrixRingDistance)) return kFALSE;
   if (!l->fill("iHowManyFitMatrixRings", &iHowManyFitMatrixRings)) return kFALSE;
   if (!l->fill("isActiveRingHoughTransf", &isActiveRingHoughTransf)) return kFALSE;
   if (!l->fill("iMinimalHoughTransfRingQuality", &iMinimalHoughTransfRingQuality)) return kFALSE;
   if (!l->fill("iMinimalHoughTransfRingDistance", &iMinimalHoughTransfRingDistance)) return kFALSE;
   if (!l->fill("iHowManyHoughTransfRings", &iHowManyHoughTransfRings)) return kFALSE;
   if (!l->fill("fAmpCorrFac", &fAmpCorrFac)) return kFALSE;
   return kTRUE;
}

void HRichAnalysisPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HRichAnalysisPar.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HParCond::Streamer(R__b);
      R__b >> iCutOffThresheold;
      R__b >> iRingRadius;
      R__b >> iRingRadiusError;
      R__b >> iRingMatrixSize;
      iRingMatrix.Streamer(R__b);
      R__b >> iRingMaskSize;
      iRingMask.Streamer(R__b);
      R__b >> isActiveCleanAlonePad;
      R__b >> iCleanAlonePadBorder;
      R__b >> iCleanAlonePadLowerThreshold;
      R__b >> isActiveCleanHighPulse;
      R__b >> iCleanHighPulseBorder;
      R__b >> iCleanHighPulseUpperThreshold;
      R__b >> isActiveLabelPads;
      R__b >> iLabeledPadsDistance;
      R__b >> maxFiredSectorPads;
      R__b >> isActiveBorderAmplitReduction;
      R__b >> isActiveTestDensity;
      R__b >> fThresholdDensity;
      R__b >> fSurfaceArea;
      R__b >> isActiveDynamicThrAmplitude;
      R__b >> fLowerAmplFactor;
      R__b >> fFormulaParam1;
      R__b >> fFormulaParam2;
      R__b >> fFormulaParam3;
      R__b >> isActiveFiredRingPadsRatio;
      R__b >> fFiredRingPadsRatio;
      R__b >> isActiveTestAsymmetry;
      R__b >> isActiveTestCharge;
      R__b >> fRingMinCharge;
      R__b >> fRingMaxCharge;
      R__b >> isActiveFakesRejection;
      R__b >> fFakeQualityRatio;
      R__b >> fFakeCentroidCut;
      R__b >> iSuperiorAlgorithmID;
      R__b >> isActiveRingFindFitMatrix;
      R__b >> iMinimalFitMatrixRingQuality;
      R__b >> iMinimalFitMatrixRingDistance;
      R__b >> iHowManyFitMatrixRings;
      R__b >> isActiveRingHoughTransf;
      R__b >> iMinimalHoughTransfRingQuality;
      R__b >> iMinimalHoughTransfRingDistance;
      R__b >> iHowManyHoughTransfRings;
      if (R__v > 1) fAmpCorrFac.Streamer(R__b);
      else          fAmpCorrFac.Reset(1.F);
      R__b.ReadStaticArray((char*)filename);
      R__b.CheckByteCount(R__s, R__c, HRichAnalysisPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HRichAnalysisPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      R__b << iCutOffThresheold;
      R__b << iRingRadius;
      R__b << iRingRadiusError;
      R__b << iRingMatrixSize;
      iRingMatrix.Streamer(R__b);
      R__b << iRingMaskSize;
      iRingMask.Streamer(R__b);
      R__b << isActiveCleanAlonePad;
      R__b << iCleanAlonePadBorder;
      R__b << iCleanAlonePadLowerThreshold;
      R__b << isActiveCleanHighPulse;
      R__b << iCleanHighPulseBorder;
      R__b << iCleanHighPulseUpperThreshold;
      R__b << isActiveLabelPads;
      R__b << iLabeledPadsDistance;
      R__b << maxFiredSectorPads;
      R__b << isActiveBorderAmplitReduction;
      R__b << isActiveTestDensity;
      R__b << fThresholdDensity;
      R__b << fSurfaceArea;
      R__b << isActiveDynamicThrAmplitude;
      R__b << fLowerAmplFactor;
      R__b << fFormulaParam1;
      R__b << fFormulaParam2;
      R__b << fFormulaParam3;
      R__b << isActiveFiredRingPadsRatio;
      R__b << fFiredRingPadsRatio;
      R__b << isActiveTestAsymmetry;
      R__b << isActiveTestCharge;
      R__b << fRingMinCharge;
      R__b << fRingMaxCharge;
      R__b << isActiveFakesRejection;
      R__b << fFakeQualityRatio;
      R__b << fFakeCentroidCut;
      R__b << iSuperiorAlgorithmID;
      R__b << isActiveRingFindFitMatrix;
      R__b << iMinimalFitMatrixRingQuality;
      R__b << iMinimalFitMatrixRingDistance;
      R__b << iHowManyFitMatrixRings;
      R__b << isActiveRingHoughTransf;
      R__b << iMinimalHoughTransfRingQuality;
      R__b << iMinimalHoughTransfRingDistance;
      R__b << iHowManyHoughTransfRings;
      fAmpCorrFac.Streamer(R__b);
      R__b.WriteArray(filename, 40);
      R__b.SetByteCount(R__c, kTRUE);
   }
}
