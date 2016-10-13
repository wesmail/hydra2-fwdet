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
//  HRichWireSignal
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHWIRESIGNAL_H
#define HRICHWIRESIGNAL_H

#include "TObject.h"

#include <iomanip>
#include <iostream>


// ****************************************************************************

class HRichWireSignal : public TObject {

// ****************************************************************************

public:

   HRichWireSignal() {
      clear();
   }
   HRichWireSignal(const Float_t charge) {
      clear();
      fCharge = charge;
   }
   HRichWireSignal(const Int_t sector, const Int_t wirenr,
                   const Float_t charge, const Float_t xpos,
                   const Float_t ypos, const Float_t ene) {
      fSector = sector;
      fWireNr = wirenr;
      fCharge = charge;
      fXpos = xpos;
      fYpos = ypos;
      fEnergy = ene;
   }
   virtual ~HRichWireSignal() {}

   void clear();

   Float_t getCharge() {
      return fCharge;
   }
   Float_t getX() {
      return fXpos;
   }
   Float_t getY() {
      return fYpos;
   }
   Float_t getEnergy() {
      return fEnergy;
   }
   void getXY(Float_t *pX, Float_t *pY) {
      *pX = fXpos;
      *pY = fYpos;
   }


   void setCharge(Float_t ampl) {
      fCharge = ampl;
   }
   void setX(Float_t xpos) {
      fXpos = xpos;
   }
   void setY(Float_t ypos) {
      fYpos = ypos;
   }
   void setXY(Float_t xpos, Float_t ypos) {
      fXpos = xpos;
      fYpos = ypos;
   }
   void setEne(Float_t ene) {
      fEnergy = ene;
   }
   friend std::ostream& operator<< (std::ostream& output, HRichWireSignal& element);

   Int_t getSector() {
      return fSector;
   }
   Int_t getWireNr() {
      return fWireNr;
   }

   void setSector(Int_t sec) {
      fSector = sec;
   }
   void setWireNr(Int_t nr) {
      fWireNr = nr;
   }

   // void setEventNr(Int_t lNr) { fEventNr = lNr; }
   // Int_t getEventNr() { return fEventNr; }

private:

   Int_t fSector;
   Int_t fWireNr;

   Float_t fCharge;
   Float_t fXpos;
   Float_t fYpos;
   Float_t fEnergy; //photon energy, necessary for the OEM simulations

   ClassDef(HRichWireSignal, 1) // Rich auxiliary class
};

//============================================================================


#endif // HRICHWIRESIGNAL_H

