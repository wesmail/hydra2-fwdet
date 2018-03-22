//*-- AUTHOR   : J. Wuestenfeld


//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
//
// HMdcSlopes
//
// Category to store the results of slopecalibration in the tree.
//
///////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hmdcslopes.h"

#include "TBuffer.h"

HMdcSlopes::HMdcSlopes(void)
{
	// Default constructor
	slope = 0.5;
	error = 0.0;
	slopeMethode = 0;
}

HMdcSlopes::~HMdcSlopes(void)
{
	// Default destructor
}

void HMdcSlopes::Streamer(TBuffer &R__b)
{
   // Stream an object of class HMdcSlopes.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> sector;
      R__b >> module;
      R__b >> mbo;
      R__b >> tdc;
      R__b >> slope;
      R__b >> error;
      R__b >> slopeMethode;
      R__b.CheckByteCount(R__s, R__c, HMdcSlopes::IsA());
   } else {
      R__c = R__b.WriteVersion(HMdcSlopes::IsA(), kTRUE);
      TObject::Streamer(R__b);
      R__b << sector;
      R__b << module;
      R__b << mbo;
      R__b << tdc;
      R__b << slope;
      R__b << error;
      R__b << slopeMethode;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

ClassImp(HMdcSlopes)
