//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Witold Przygoda (przygoda@psja1.if.uj.edu.pl)
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichHit
//
//  Hit candidate container
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichhit.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichHit)


Int_t HRichHit::iRingImageSize = 0;
HRichHit::HRichHit()
{
   iRingX = 0;
   iRingY = 0;
   iRingQuality = 0;
   iRingPatMat = 0;
   iRingHouTra = 0;
   iRingPadNr = 0;
   iRingAmplitude = 0;
   iRingLocalMax4 = 0;
   iRingLocalMax8 = 0;
   iRingClusterNr = 0;
   iRingImageSize = 0;
   iRingAlgorithmIndex = 0;
   fRingClusterSize = 0.;
   fRingClusterLMax4 = 0.;
   fRingClusterLMax8 = 0.;
   for (Int_t i = 0; i < 256; i++) iRingImage[i] = 0;
   iRingFreeParam = 0;
   iRingMaxLabel = 0;
   fLabX = 0.;
   fLabY = 0.;
   fLabZ = 0.;
   fX = 0.;
   fY = 0.;
   fPadX = 0.;
   fPadY = 0.;
   fMeanTheta = 0.;
   fMeanPhi = 0.;
   fTheta = 0.;
   fPhi = 0.;

   fMaxClusterSize = 0;
   fMaxClusterSum = 0;
   fMaxThrClusterSize = 0;

   fTestDens = kFALSE;
   fTestBord = kFALSE;
   fTestDyna = kFALSE;
   fTestRati = kFALSE;
   fTestAsym = kFALSE;
   fRejFake = kTRUE;
   fTests = 0;
   fBorderFactor = 0.;
   fRingCentroid = 0.;
   fRingRadius = 0.;

   fAddress = 0;
   lEventNr = 0;
   nSector = 0;
}

HRichHit::HRichHit(Int_t x, Int_t y, Int_t q, Int_t fp, Int_t lab)
{

   iRingPatMat = 0;
   iRingHouTra = 0;
   iRingPadNr = 0;
   iRingAmplitude = 0;
   iRingLocalMax4 = 0;
   iRingLocalMax8 = 0;
   iRingClusterNr = 0;
   iRingImageSize = 0;
   iRingAlgorithmIndex = 0;
   fRingClusterSize = 0.;
   fRingClusterLMax4 = 0.;
   fRingClusterLMax8 = 0.;
   for (Int_t i = 0; i < 256; i++) iRingImage[i] = 0;
   fLabX = 0.;
   fLabY = 0.;
   fLabZ = 0.;
   fX = 0.;
   fY = 0.;
   fPadX = 0.;
   fPadY = 0.;
   fMeanTheta = 0.;
   fMeanPhi = 0.;
   fTheta = 0.;
   fPhi = 0.;

   fMaxClusterSize = 0;
   fMaxClusterSum = 0;
   fMaxThrClusterSize = 0;

   fTestDens = kFALSE;
   fTestBord = kFALSE;
   fTestDyna = kFALSE;
   fTestRati = kFALSE;
   fTestAsym = kFALSE;
   fRejFake = kTRUE;
   fTests = 0;
   fBorderFactor = 0.;
   fRingCentroid = 0.;
   fRingRadius = 0.;

   fAddress = 0;
   lEventNr = 0;
   nSector = 0;

   iRingX = x;
   iRingY = y;
   iRingQuality = q;
   iRingFreeParam = fp;
   iRingMaxLabel = lab;
}

HRichHit::~HRichHit() {}

HRichHit::HRichHit(const HRichHit& source) :TObject(source)
{
   iRingX = source.iRingX;
   iRingY = source.iRingY;
   iRingQuality = source.iRingQuality;
   iRingPatMat = source.iRingPatMat;
   iRingHouTra = source.iRingHouTra;
   iRingPadNr = source.iRingPadNr;
   iRingAmplitude = source.iRingAmplitude;
   iRingLocalMax4 = source.iRingLocalMax4;
   iRingLocalMax8 = source.iRingLocalMax8;
   iRingClusterNr = source.iRingClusterNr;
   iRingImageSize = source.iRingImageSize;

   fRingClusterSize = source.fRingClusterSize;
   fRingClusterLMax4 = source.fRingClusterLMax4;
   fRingClusterLMax8 = source.fRingClusterLMax8;

   iRingAlgorithmIndex = source.iRingAlgorithmIndex;
   for (Int_t i = 0; i < 256; i++) iRingImage[i] = source.iRingImage[i];

   iRingFreeParam = source.iRingFreeParam;
   iRingMaxLabel = source.iRingMaxLabel;
   fLabX = source.fLabX;
   fLabY = source.fLabY;
   fLabZ = source.fLabZ;
   fX = source.fX;
   fY = source.fY;
   fPadX = source.fPadX;
   fPadY = source.fPadY;
   fMeanTheta = source.fMeanTheta;
   fMeanPhi = source.fMeanPhi;
   fTheta = source.fTheta;
   fPhi = source.fPhi;

   fMaxClusterSize = source.fMaxClusterSize;
   fMaxClusterSum = source.fMaxClusterSum;
   fMaxThrClusterSize = source.fMaxThrClusterSize;

   fTestDens = source.fTestDens;
   fTestBord = source.fTestBord;
   fTestDyna = source.fTestDyna;
   fTestRati = source.fTestRati;
   fTestAsym = source.fTestAsym;
   fTestCharge = source.fTestCharge;
   fRejFake = source.fRejFake;
   fTests = source.fTests;
   fBorderFactor = source.fBorderFactor;
   fRingCentroid = source.fRingCentroid;
   fRingRadius = source.fRingRadius;


   fAddress = source.fAddress;
   lEventNr = source.lEventNr;
   nSector = source.nSector;
}

HRichHit& HRichHit::operator=(const HRichHit& source)
{

   if (this != &source) {
      iRingX = source.iRingX;
      iRingY = source.iRingY;
      iRingQuality = source.iRingQuality;
      iRingPatMat = source.iRingPatMat;
      iRingHouTra = source.iRingHouTra;
      iRingPadNr = source.iRingPadNr;
      iRingAmplitude = source.iRingAmplitude;
      iRingLocalMax4 = source.iRingLocalMax4;
      iRingLocalMax8 = source.iRingLocalMax8;
      iRingClusterNr = source.iRingClusterNr;
      iRingImageSize = source.iRingImageSize;

      fRingClusterSize = source.fRingClusterSize;
      fRingClusterLMax4 = source.fRingClusterLMax4;
      fRingClusterLMax8 = source.fRingClusterLMax8;

      iRingAlgorithmIndex = source.iRingAlgorithmIndex;
      for (Int_t i = 0; i < 256; i++) iRingImage[i] = source.iRingImage[i];

      iRingFreeParam = source.iRingFreeParam;
      iRingMaxLabel = source.iRingMaxLabel;
      fLabX = source.fLabX;
      fLabY = source.fLabY;
      fLabZ = source.fLabZ;
      fX = source.fX;
      fY = source.fY;
      fPadX = source.fPadX;
      fPadY = source.fPadY;
      fMeanTheta = source.fMeanTheta;
      fMeanPhi = source.fMeanPhi;
      fTheta = source.fTheta;
      fPhi = source.fPhi;

      fMaxClusterSize = source.fMaxClusterSize;
      fMaxClusterSum = source.fMaxClusterSum;
      fMaxThrClusterSize = source.fMaxThrClusterSize;

      fTestDens = source.fTestDens;
      fTestBord = source.fTestBord;
      fTestDyna = source.fTestDyna;
      fTestRati = source.fTestRati;
      fTestAsym = source.fTestAsym;
      fTestCharge = source.fTestCharge;
      fRejFake = source.fRejFake;
      fTests = source.fTests;
      fBorderFactor = source.fBorderFactor;
      fRingCentroid = source.fRingCentroid;
      fRingRadius = source.fRingRadius;


      fAddress = source.fAddress;
      lEventNr = source.lEventNr;
      nSector = source.nSector;
   }

   return *this;
}


void HRichHit::Reset()
{
   iRingX = 0;
   iRingY = 0;
   iRingQuality = 0;
   iRingPatMat = 0;
   iRingHouTra = 0;
   iRingPadNr = 0;
   iRingAmplitude = 0;
   iRingLocalMax4 = 0;
   iRingLocalMax8 = 0;
   fRingClusterSize = 0.;
   fRingClusterLMax4 = 0.;
   fRingClusterLMax8 = 0.;
   iRingClusterNr = 0;
   iRingAlgorithmIndex = 0;
   for (Int_t i = 0; i < 256; i++) iRingImage[i] = 0;
   iRingFreeParam = 0;
   iRingMaxLabel = 0;
   fLabX = 0.;
   fLabY = 0.;
   fLabZ = 0.;
   fX = 0.;
   fY = 0.;
   fPadX = 0.;
   fPadY = 0.;
   fMeanTheta = 0.;
   fMeanPhi = 0.;
   fTheta = 0.;
   fPhi = 0.;
   fMaxClusterSize = 0;
   fMaxClusterSum = 0;
   fMaxThrClusterSize = 0;
   fTestDens = kFALSE;
   fTestBord = kFALSE;
   fTestDyna = kFALSE;
   fTestRati = kFALSE;
   fTestAsym = kFALSE;
   fRejFake = kTRUE;
   fTests = 0;
   fBorderFactor = 0.;
   fRingCentroid = 0.;
   fRingRadius = 0.;
   lEventNr = 0;
   nSector = 0;
   fAddress = 0;
}

Int_t HRichHit::calcAddress()
{
   Char_t s = (nSector) ? nSector : 6;
   fAddress = 10000 * s;
   fAddress += 100 * iRingY;
   fAddress += iRingX;

   return fAddress;
}

void HRichHit::dumpToStdout()
{
   cout << "***************************************************" << endl;
   cout << "*** RICH Hit Properties (Ring) *** Sec:" << getSector() << endl;
   cout << "Ring X (azim)   : " << getRingCenterX()
        << " / Ring Y (polar)  : " << getRingCenterY() << endl;
   cout << "Ring PHI (azim)   : " << getPhi()
        << " / Ring THETA (polar)  : " << getTheta() << endl;
   cout << "Ring HT         : " << getRingHouTra()
        << " / Ring PM         :" << getRingPatMat() << endl;
   cout << "Ring Ampl :" << getRingAmplitude()
        << " / Ring PadNr  :" << getRingPadNr()
        << " / Ring LocMax4  :" << getRingLocalMax4() << endl;
   cout << "***************************************************" << endl;

}

