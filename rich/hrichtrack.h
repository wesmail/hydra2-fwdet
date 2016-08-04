//////////////////////////////////////////////////////////////////////////////
//
// $Id: $
//
//*-- Author  : Laura Fabbietti <Laura.Fabbietti@ph.tum.de>
//*-- Revised : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//_HADES_CLASS_DESCRIPTION
//////////////////////////////////////////////////////////////////////////////
//
//  HRichTrack
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHTRACK_H
#define HRICHTRACK_H

#include "TObject.h"

class HRichTrack : public TObject {

private:
   Int_t fTrackNr;   // Track number of RICH hit
   Int_t fEventNr;   // Event number taken from CAL<-RAW<-Unpacker
   Int_t fFlag;      // Flag to distinguish photons (0) and direct hits (1)
   Int_t fAddress;   // pad address, cf. HRichCal::calcAddress()

public:
   HRichTrack();
   ~HRichTrack() {}

   ///////////////////////////////////////////////////////////////////
   // GETTERS
   Int_t getTrack(void);
   Int_t getEventNr(void);
   Int_t getFlag(void);
   Int_t getAddress(void);

   ///////////////////////////////////////////////////////////////////
   // SETTERS
   void setTrack(Int_t i);
   void setEventNr(Int_t i);
   void setFlag(Int_t i);
   void setAddress(Int_t i);

   Bool_t IsSortable() const;
   Int_t  Compare(const TObject *obj) const {
      // HRichTrack is sorted by the pad address
      if (fAddress == ((HRichTrack*)obj)->fAddress) return 0;
      else if (fAddress > ((HRichTrack*)obj)->fAddress) return 1;
      else return -1;
   }

   ClassDef(HRichTrack, 1) // RICH HGeant track class
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichTrack::getTrack(void)
{
   return fTrackNr;
}
inline Int_t HRichTrack::getEventNr(void)
{
   return fEventNr;
}
inline Int_t HRichTrack::getFlag(void)
{
   return fFlag;
}
inline Int_t HRichTrack::getAddress(void)
{
   return fAddress;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichTrack::setTrack(Int_t i)
{
   fTrackNr = i;
}
inline void HRichTrack::setEventNr(Int_t i)
{
   fEventNr = i;
}
inline void HRichTrack::setFlag(Int_t i)
{
   fFlag = i;
}
inline void HRichTrack::setAddress(Int_t i)
{
   fAddress = i;
}

inline Bool_t HRichTrack::IsSortable() const
{
   return kTRUE;
}


#endif /* !HRICHTRACK_H */






