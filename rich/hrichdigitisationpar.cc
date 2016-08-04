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
//  HRichDigitisationPar
//
//
//////////////////////////////////////////////////////////////////////////////


#include "hparamlist.h"
#include "hrichdigitisationpar.h"

#include <iostream>

using namespace std;

ClassImp(HRichDigitisationPar)


HRichDigitisationPar::HRichDigitisationPar(const Char_t* name, const Char_t* title,
                                           const Char_t* context)
   : HParCond(name, title, context)
{
   clear();
}


HRichDigitisationPar::~HRichDigitisationPar()
{
   clear();
}


void HRichDigitisationPar::clear()
{
   fElectronsNr      = 0.0;
   fSigmaValue       = 0.;
   fIncreaseNoise    = 0.;
   fChargePerChannel = 0.;
   fChargeScaling    = 1.0;
   fQupper           = 0.0;
   fFactor1          = 0.;
   fFactor2          = 0.;
   fFactor1Sig       = 0.;
   fFactor2Sig       = 0.;
   fParameter1       = 0.;
   fParameter2       = 0.;
   fBinsNr           = 0;
   fExpSlope.Reset();
   fPhotonLen.Reset();
   fPhotonEff.Reset();

   for (Int_t i = 0; i < 6; i++) {
      corrSec[i].Reset();
   }
}
/*
HRichDigitisationPar::HRichDigitisationPar(const HRichDigitisationPar& source)
{
   Error("HRichDigitisationPar", "Object can not be initialized with values of another object!");
   Error("HRichDigitisationPar", "Default constructor will be called.");
   HRichDigitisationPar();
}


HRichDigitisationPar& HRichDigitisationPar::operator=(const HRichDigitisationPar& source)
{
   if (this != &source) {
      Error("operator=", "HRichDigitisationPar object can not be assigned!");
      Error("operator=", "Default constructor will be called.");
      HRichDigitisationPar();
   }
   return *this;
}
*/
void HRichDigitisationPar::printParams()
{

   Info("printParams", "RICH DIGITISATION PARAMETERS");

   cout << " Mean number of electrons / keV.............." << fElectronsNr << endl;
   cout << " Charge [e] per one channel.................." << fChargePerChannel << endl;
   cout << " Sigma Value used for Threshold [ADC chan]..." << fSigmaValue << endl;
   cout << " fElectronsNr................................" << fElectronsNr << endl;
   cout << " fSigmaValue................................." << fSigmaValue << endl;
   cout << " fIncreaseNoise.............................." << fIncreaseNoise << endl;
   cout << " fChargePerChannel..........................." << fChargePerChannel << endl;
   cout << " fQupper....................................." << fQupper << endl;
   cout << " fParameter1................................." << fParameter1 << endl;
   cout << " fParameter2................................." << fParameter2 << endl;
   cout << " fFactor1...................................." << fFactor1 << endl;
   cout << " fFactor2...................................." << fFactor2 << endl;
   cout << " fFactor1Sig................................." << fFactor1Sig << endl;
   cout << " fFactor2Sig................................." << fFactor2Sig << endl;
   cout << " fBinsNr....................................." << fBinsNr << endl;
   cout << " fChargeScaling.............................." << fChargeScaling << endl;

   cout << " fPhotonLen ";
   for (Int_t i = 0; i < fPhotonLen.GetSize(); ++i) {
      cout << fPhotonLen[i] << "  ";
   }
   cout << endl;
   cout << " fPhotonEff ";
   for (Int_t i = 0; i < fPhotonEff.GetSize(); ++i) {
      cout << fPhotonEff[i] << "  ";
   }
   cout << endl;
   cout << " fExpSlope ";
   for (Int_t i = 0; i < fExpSlope.GetSize(); ++i) {
      cout << fExpSlope[i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 0:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[0][i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 1:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[1][i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 2:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[2][i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 3:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[3][i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 4:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[4][i] << "  ";
   }
   cout << endl;
   cout << "  Correction  sector 5:  ";
   for (Int_t i = 0; i < 18; i++) {
      cout << corrSec[5][i] << "  ";
   }
   cout << endl;
}
//============================================================================

void HRichDigitisationPar::putParams(HParamList* l)
{
   if (!l) return;
   l->add("fElectronsNr",      fElectronsNr);
   l->add("fSigmaValue",       fSigmaValue);
   l->add("fIncreaseNoise",    fIncreaseNoise);
   l->add("fChargePerChannel", fChargePerChannel);
   l->add("fQupper",           fQupper);
   l->add("fParameter1",       fParameter1);
   l->add("fParameter2",       fParameter2);
   l->add("fFactor1",          fFactor1);
   l->add("fFactor2",          fFactor2);
   l->add("fFactor1Sig",       fFactor1Sig);
   l->add("fFactor2Sig",       fFactor2Sig);
   l->add("fExpSlope",         fExpSlope);
   l->add("fChargeScaling",    fChargeScaling);
   l->add("fBinsNr",           fBinsNr);
   l->add("fPhotonLen",        fPhotonLen);
   l->add("fPhotonEff",        fPhotonEff);
   l->add("corrSec0",          corrSec[0]);
   l->add("corrSec1",          corrSec[1]);
   l->add("corrSec2",          corrSec[2]);
   l->add("corrSec3",          corrSec[3]);
   l->add("corrSec4",          corrSec[4]);
   l->add("corrSec5",          corrSec[5]);

}
//============================================================================

Bool_t HRichDigitisationPar::getParams(HParamList* l)
{
   if (!l) return kFALSE;
   if (!l->fill("fElectronsNr",      &fElectronsNr))      return kFALSE;
   if (!l->fill("fSigmaValue",       &fSigmaValue))       return kFALSE;
   if (!l->fill("fIncreaseNoise",    &fIncreaseNoise))    return kFALSE;
   if (!l->fill("fChargePerChannel", &fChargePerChannel)) return kFALSE;
   if (!l->fill("fQupper",           &fQupper))           return kFALSE;
   if (!l->fill("fParameter1",       &fParameter1))       return kFALSE;
   if (!l->fill("fParameter2",       &fParameter2))       return kFALSE;
   if (!l->fill("fFactor1",          &fFactor1))          return kFALSE;
   if (!l->fill("fFactor2",          &fFactor2))          return kFALSE;
   if (!l->fill("fFactor1Sig",       &fFactor1Sig))       return kFALSE;
   if (!l->fill("fFactor2Sig",       &fFactor2Sig))       return kFALSE;
   if (!l->fill("fExpSlope",         &fExpSlope))         return kFALSE;
   if (!l->fill("fChargeScaling",    &fChargeScaling))    return kFALSE;
   if (!l->fill("fBinsNr",           &fBinsNr))           return kFALSE;
   if (!l->fill("fPhotonLen",        &fPhotonLen))        return kFALSE;
   if (!l->fill("fPhotonEff",        &fPhotonEff))        return kFALSE;
   if (!l->fill("corrSec0",          &corrSec[0]))        return kFALSE;
   if (!l->fill("corrSec1",          &corrSec[1]))        return kFALSE;
   if (!l->fill("corrSec2",          &corrSec[2]))        return kFALSE;
   if (!l->fill("corrSec3",          &corrSec[3]))        return kFALSE;
   if (!l->fill("corrSec4",          &corrSec[4]))        return kFALSE;
   if (!l->fill("corrSec5",          &corrSec[5]))        return kFALSE;

#if DEBUG_LEVEL > 0
   printParams();
#endif

   return kTRUE;
}

void HRichDigitisationPar::Streamer(TBuffer &R__b)
{
   // Stream an object of class HRichDigitisationPar.

   UInt_t R__s, R__c;
   Int_t R__i;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c);
      if (R__v) { }
      HParCond::Streamer(R__b);
      R__b >> fElectronsNr;
      R__b >> fSigmaValue;
      R__b >> fIncreaseNoise;
      R__b >> fChargePerChannel;
      R__b >> fQupper;
      R__b >> fParameter1;
      R__b >> fParameter2;
      R__b >> fFactor1;
      R__b >> fFactor2;
      R__b >> fFactor1Sig;
      R__b >> fFactor2Sig;
      R__b >> fChargeScaling;
      R__b >> fBinsNr;
      fExpSlope.Streamer(R__b);
      fPhotonLen.Streamer(R__b);
      fPhotonEff.Streamer(R__b);
      for (R__i = 0; R__i < 6; R__i++)
         corrSec[R__i].Streamer(R__b);
      R__b.ReadStaticArray((Char_t*)filename);
      R__b.CheckByteCount(R__s, R__c, HRichDigitisationPar::IsA());
   } else {
      R__c = R__b.WriteVersion(HRichDigitisationPar::IsA(), kTRUE);
      HParCond::Streamer(R__b);
      R__b << fElectronsNr;
      R__b << fSigmaValue;
      R__b << fIncreaseNoise;
      R__b << fChargePerChannel;
      R__b << fQupper;
      R__b << fParameter1;
      R__b << fParameter2;
      R__b << fFactor1;
      R__b << fFactor2;
      R__b << fFactor1Sig;
      R__b << fFactor2Sig;
      R__b << fChargeScaling;
      R__b << fBinsNr;
      fExpSlope.Streamer(R__b);
      fPhotonLen.Streamer(R__b);
      fPhotonEff.Streamer(R__b);
      for (R__i = 0; R__i < 6; R__i++)
         corrSec[R__i].Streamer(R__b);
      R__b.WriteArray(filename, 40);
      R__b.SetByteCount(R__c, kTRUE);
   }
}

