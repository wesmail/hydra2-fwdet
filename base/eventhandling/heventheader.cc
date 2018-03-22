#include "heventheader.h"

#include "TBuffer.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 30/03/2000 by R. Holzmann
//*-- Modified : 24/03/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//**************************************************************************
//
// HEventHeader
// 
// This class contains the header information of an event (run number, etc.)
// See data members for more detailed information.
// 
//**************************************************************************

ClassImp(HEventHeader)

void HEventHeader::Streamer(TBuffer &R__b) {
   // Stream an object of class HEventHeader.

   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(); if (R__v) { }
      TObject::Streamer(R__b);
      R__b >> fEventSize;
      R__b >> fEventDecoding;
      R__b >> fId;
      R__b >> fEventSeqNumber;
      R__b >> fDate;
      R__b >> fTime;
      R__b >> fEventRunNumber;
      R__b >> fEventPad;

      if (R__v > 1) {
	R__b >> fTBit;
      } else {
	fTBit = 0;
      }

      if (R__v > 2) {
	fVertex.Streamer(R__b);
      }
      if (R__v > 7) {
	fVertexCluster.Streamer(R__b);
      }
      if (R__v > 8) {
	fVertexReco.Streamer(R__b);
      }

      if (R__v > 3) {
	R__b >> downscaling;
        R__b >> downscalingFlag;
        R__b >> triggerDecision;
      } else {
	downscaling = 0;
        downscalingFlag = 0;
        triggerDecision = 0;
      }

      if (R__v > 4) {
        R__b >> timeInSpill;
      } else {
        timeInSpill = 0;
      }

      if (R__v > 5) {
        R__b >> fVersion;
      } else {
        fVersion = 0;
      }

      if (R__v > 6) {
         R__b >> triggerDecisionEmu;
         R__b >> fErrorBit;
      } else {
         triggerDecisionEmu = 0;
         fErrorBit = 0;
      }

   } else {
      R__b.WriteVersion(HEventHeader::IsA());
      TObject::Streamer(R__b);
      R__b << fEventSize;
      R__b << fEventDecoding;
      R__b << fId;
      R__b << fEventSeqNumber;
      R__b << fDate;
      R__b << fTime;
      R__b << fEventRunNumber;
      R__b << fEventPad;
      R__b << fTBit;
      fVertex.Streamer(R__b);
      fVertexCluster.Streamer(R__b);
      fVertexReco.Streamer(R__b);
      R__b << downscaling;
      R__b << downscalingFlag;
      R__b << triggerDecision;      
      R__b << timeInSpill;
      R__b << fVersion;
      R__b << triggerDecisionEmu;
      R__b << fErrorBit;
   }
}


//**************************************************************************
//
// HVertex
//
// Holds the fitted 3-dim vertex position in the lab system.
//
//**************************************************************************

ClassImp(HVertex)

void HVertex::Streamer(TBuffer &R__b)
{
   // Stream an object of class HVertex.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      TObject::Streamer(R__b);
      pos.Streamer(R__b);
      R__b >> iterations;
      R__b >> chi2;

      if (R__v > 1)
	{
	   R__b >> sumOfWeights;
	}
      else
	 {
	    sumOfWeights = -1.;
	 }
      R__b.CheckByteCount(R__s, R__c, HVertex::IsA());
   } else {
      R__c = R__b.WriteVersion(HVertex::IsA(), kTRUE);
      TObject::Streamer(R__b);
      pos.Streamer(R__b);
      R__b << iterations;
      R__b << chi2;
      R__b << sumOfWeights;
      R__b.SetByteCount(R__c, kTRUE);
   }
}

