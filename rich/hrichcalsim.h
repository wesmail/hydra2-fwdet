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

class HRichCalSim : public HRichCal {

private:

   Int_t   fTrack1;  // index of track nb. for first hit
   Int_t   fTrack2;  // index of track nb. for last hit
   Float_t fEnergy;  // energy of the photon coupling to the pad, in case of direct hits is -1

public:

   HRichCalSim();
   HRichCalSim(Float_t ch);
   HRichCalSim(Int_t s, Int_t r, Int_t c);
   virtual ~HRichCalSim() {}

   Int_t   getNTrack1(void) const;
   Int_t   getNTrack2(void) const;
   Float_t getEnergy() const;

   void setNTrack1(const Int_t n);
   void setNTrack2(const Int_t n);
   void setEnergy(Float_t ene);

   void addEnergy(Float_t ene);
   void addHit();

   ClassDef(HRichCalSim, 1) // RICH simulated cal data
};


// Getters
inline Int_t HRichCalSim::getNTrack1(void) const
{
   return fTrack1;
}
inline Int_t HRichCalSim::getNTrack2(void) const
{
   return fTrack2;
}
inline Float_t HRichCalSim::getEnergy() const
{
   return fEnergy;
}

// Setters
inline void HRichCalSim::setNTrack1(const Int_t n)
{
   fTrack1 = n;
}
inline void HRichCalSim::setNTrack2(const Int_t n)
{
   fTrack2 = n;
}
inline void HRichCalSim::setEnergy(Float_t e)
{
   fEnergy = e;
}

inline void HRichCalSim::addEnergy(Float_t e)
{
   fEnergy += e;
}
inline void HRichCalSim::addHit()
{
   fTrack1++;
}

#endif /* !HRICHCALSIM_H */
