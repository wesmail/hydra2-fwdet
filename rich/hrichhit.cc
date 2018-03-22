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

#include "TBuffer.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichHit)


Int_t HRichHit::iRingImageSize = 0;
HRichHit::HRichHit()
{
   fRich700CircleCenterX = -10000;
   fRich700CircleCenterY = -10000;
   fRich700CircleRadius  = -10000;
   fRich700CircleChi2    = -10000;
   fRich700NofRichCals   = 0;

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

   fRich700CircleCenterX = -10000;
   fRich700CircleCenterY = -10000;
   fRich700CircleRadius  = -10000;
   fRich700CircleChi2    = -10000;
   fRich700NofRichCals   = 0;

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
   fRich700CircleCenterX = source.fRich700CircleCenterX;
   fRich700CircleCenterY = source.fRich700CircleCenterY;
   fRich700CircleRadius  = source.fRich700CircleRadius;
   fRich700CircleChi2    = source.fRich700CircleChi2;
   fRich700NofRichCals   = source.fRich700NofRichCals;

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
      fRich700CircleCenterX = source.fRich700CircleCenterX;
      fRich700CircleCenterY = source.fRich700CircleCenterY;
      fRich700CircleRadius  = source.fRich700CircleRadius;
      fRich700CircleChi2    = source.fRich700CircleChi2;
      fRich700NofRichCals   = source.fRich700NofRichCals;
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
   fRich700CircleCenterX = -10000;
   fRich700CircleCenterY = -10000;
   fRich700CircleRadius  = -10000;
   fRich700CircleChi2    = -10000;
   fRich700NofRichCals   = 0;

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

void HRichHit::Streamer(TBuffer &R__b)
{
   // Stream an object of class HRichHit.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      if(R__v>1){
	  R__b >> fRich700CircleCenterX;
	  R__b >> fRich700CircleCenterY;
	  R__b >> fRich700CircleRadius;
	  R__b >> fRich700CircleChi2;
	  R__b >> fRich700NofRichCals;
      } else {
	  fRich700CircleCenterX = -10000;
	  fRich700CircleCenterY = -10000;
	  fRich700CircleRadius  = -10000;
	  fRich700CircleChi2    = -10000;
	  fRich700NofRichCals   = 0;
      }
      R__b >> iRingX;
      R__b >> iRingY;
      R__b >> iRingPatMat;
      R__b >> iRingHouTra;
      R__b >> iRingPadNr;
      R__b >> iRingAmplitude;
      R__b >> iRingLocalMax4;
      R__b >> iRingLocalMax8;
      R__b >> iRingClusterNr;
      R__b >> fRingClusterSize;
      R__b >> fRingClusterLMax4;
      R__b >> fRingClusterLMax8;
      R__b >> iRingAlgorithmIndex;
      R__b >> fMaxClusterSize;
      R__b >> fMaxClusterSum;
      R__b >> fMaxThrClusterSize;
      R__b >> fLabX;
      R__b >> fLabY;
      R__b >> fLabZ;
      R__b >> fX;
      R__b >> fY;
      R__b >> fPadX;
      R__b >> fPadY;
      R__b >> fMeanTheta;
      R__b >> fMeanPhi;
      R__b >> fTheta;
      R__b >> fPhi;
      R__b >> nSector;
      R__b >> lEventNr;
      R__b >> fAddress;
      R__b >> fTestDens;
      R__b >> fTestBord;
      R__b >> fTestDyna;
      R__b >> fTestRati;
      R__b >> fTestAsym;
      R__b >> fTestCharge;
      R__b >> fRejFake;
      R__b >> fTests;
      R__b >> fRingCentroid;
      R__b >> fRingRadius;
      R__b >> fBorderFactor;
      R__b.CheckByteCount(R__s, R__c, HRichHit::IsA());
   } else {
      R__c = R__b.WriteVersion(HRichHit::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << fRich700CircleCenterX;
      R__b << fRich700CircleCenterY;
      R__b << fRich700CircleRadius;
      R__b << fRich700CircleChi2;
      R__b << fRich700NofRichCals;
      R__b << iRingX;
      R__b << iRingY;
      R__b << iRingPatMat;
      R__b << iRingHouTra;
      R__b << iRingPadNr;
      R__b << iRingAmplitude;
      R__b << iRingLocalMax4;
      R__b << iRingLocalMax8;
      R__b << iRingClusterNr;
      R__b << fRingClusterSize;
      R__b << fRingClusterLMax4;
      R__b << fRingClusterLMax8;
      R__b << iRingAlgorithmIndex;
      R__b << fMaxClusterSize;
      R__b << fMaxClusterSum;
      R__b << fMaxThrClusterSize;
      R__b << fLabX;
      R__b << fLabY;
      R__b << fLabZ;
      R__b << fX;
      R__b << fY;
      R__b << fPadX;
      R__b << fPadY;
      R__b << fMeanTheta;
      R__b << fMeanPhi;
      R__b << fTheta;
      R__b << fPhi;
      R__b << nSector;
      R__b << lEventNr;
      R__b << fAddress;
      R__b << fTestDens;
      R__b << fTestBord;
      R__b << fTestDyna;
      R__b << fTestRati;
      R__b << fTestAsym;
      R__b << fTestCharge;
      R__b << fRejFake;
      R__b << fTests;
      R__b << fRingCentroid;
      R__b << fRingRadius;
      R__b << fBorderFactor;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

