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
//  HRichCalSim
//
//  cal class for simulated data.
//  contains OLD RICH :
//  fTrackIds[0]+fTrackIds[1] : index1-index2 range in HRichTrack category (indexes of the first and last track numbers)
//  fNofTracks = 2;
//  fEnergy = average energy of the photon hit per track
//
//  RICH700:
//  fTrackIds[10] : Geant track IDS
//  fNofTracks = number of trackIDs
//  fEnergy = 0;
//
//  isNewRich() returns kTRUE when filled by HRich700Digitizer
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichcalsim.h"

#include "TBuffer.h"

ClassImp(HRichCalSim)

    HRichCalSim::HRichCalSim():
    HRichCal(),
    fNofTracks(0),
    fEnergy(0),
    fCt(0)
{
    initTrackIds();
}

HRichCalSim::HRichCalSim(Float_t ch) :
HRichCal(ch),
fNofTracks(0),
fEnergy(0),
fCt(0)
{
    initTrackIds();
}

HRichCalSim::HRichCalSim(Int_t s, Int_t r, Int_t c):
HRichCal(s, c, r),
fNofTracks(0),
fEnergy(0),
fCt(0)
{
    initTrackIds();
}

void HRichCalSim::initTrackIds()
{
    for (Int_t i = 0; i < NMAXTRACKS; i++) {
	fTrackIds[i] = -1;
    }
}

Bool_t HRichCalSim::checkTrackId(Int_t trackId)
{
    // check that trackId is not added
    // kTRUE : track is already in the list of tracks
    // kFALSE : track is not in the list of tracks
    for (Int_t i = 0; i < fNofTracks; i++) {
	if (fTrackIds[i] == trackId) return kTRUE;
    }
    return kFALSE;
}

void HRichCalSim::addTrackId(Int_t trackId)
{
    // store trackId for one richCal
    // use checkTrackId(Int_t trackId) to cekc if the
    // track is in the list of tracks already
    for (Int_t i = 0; i < fNofTracks; i++) {
	if (fTrackIds[i] == trackId) return;
    }
    if (fNofTracks >= NMAXTRACKS) return;
    fTrackIds[fNofTracks] = trackId;
    fNofTracks++;
}


Int_t HRichCalSim::getTrackId(Int_t index)
{
    if (index >= fNofTracks || index < 0 ) return -1;
    return fTrackIds[index];
}


void HRichCalSim::Streamer(TBuffer &R__b)
{
   // Stream an object of class HRichCalSim.

   UInt_t R__s, R__c;
   if (R__b.IsReading()) {
      Version_t R__v = R__b.ReadVersion(&R__s, &R__c); if (R__v) { }
      HRichCal::Streamer(R__b);

      if(R__v > 1){ // RICH700 format
	  R__b.ReadStaticArray((int*)fTrackIds);
          R__b >> fNofTracks;
      } else  {
	  initTrackIds() ;
	  fNofTracks = 0;
      }
      if(R__v == 1){ // RICH format
	  Int_t   fTrack1;
	  Int_t   fTrack2;

          R__b >> fTrack1;
	  R__b >> fTrack2;

          fTrackIds[0] = fTrack1;
          fTrackIds[1] = fTrack2;
	  fNofTracks = 2;
      }
      R__b >> fEnergy;


      R__b.CheckByteCount(R__s, R__c, HRichCalSim::IsA());
   } else {
      R__c = R__b.WriteVersion(HRichCalSim::IsA(), kTRUE);
      HRichCal::Streamer(R__b);
      R__b.WriteArray(fTrackIds, NMAXTRACKS);
      R__b << fNofTracks;
      R__b << fEnergy;
      R__b.SetByteCount(R__c, kTRUE);
   }
}



