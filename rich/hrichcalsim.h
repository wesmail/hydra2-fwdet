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
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHCALSIM_H
#define HRICHCALSIM_H

#include "hrichcal.h"


#define NMAXTRACKS 10

class HRichCalSim : public HRichCal {

private:
   Int_t   fTrackIds[NMAXTRACKS]; // array trackids
   Int_t   fNofTracks;            // number  stored track ids
   Float_t fEnergy;               // energy of the photon coupling to the pad, in case of direct hits is -1
   Int_t   fCt;                   //! tmp counter for HRichDigitizer
   void    initTrackIds();

public:

   HRichCalSim();
   HRichCalSim(Float_t ch);
   HRichCalSim(Int_t s, Int_t r, Int_t c);
   virtual ~HRichCalSim() {}

   void    setEnergy(Float_t ene)  { fEnergy = ene; }
   void    addEnergy(Float_t e)    {  fEnergy += e; }
   void    addHit()                { fCt++; }
   Bool_t  checkTrackId(Int_t trackId);
   void    addTrackId  (Int_t trackId);

   Int_t   getNHits()               { return fCt; }
   Int_t   getNofTrackIds()         { return fNofTracks;}
   Int_t   getTrackId(Int_t index);
   Float_t getEnergy(void)  const   { return  fEnergy; }

   Bool_t  isNewRich() {return fEnergy == 0 ? kTRUE : kFALSE;}
   
   ClassDef(HRichCalSim, 2) // RICH simulated cal data
};



#endif /* !HRICHCALSIM_H */
