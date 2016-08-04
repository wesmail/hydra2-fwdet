//*-- Author : Dan Magestro (magestro@gsi.de)
//*-- Modified : 24/01/2002
#ifndef HQASCALERS_H
#define HQASCALERS_H
#include "TNamed.h"

class HQAScaler;
class HQAScaler6;

class HQAScalers : public TObject {

public:
   HQAScalers();
   virtual ~HQAScalers() {}

   virtual void bookScalers();
   virtual void activateScalers();

   TList *getScalerList() {
      return scalerList;
   }

private:
   void resetPointers();

public:
   TList *scalerList;  //! List of scalers

   // Start scalers

   // Rich scalers
   HQAScaler6 *richCal_n;     //! richCal: pads/event
   HQAScaler6 *richHit_n;     //! richHit: hits/event (*1000)

   // Mdc scalers
   HQAScaler6 *mdcCal1_n_m0;  //! mdcCal1: Plane I, avg. mult.
   HQAScaler6 *mdcCal1_n_m1;  //! mdcCal1: Plane II, avg. mult.
   HQAScaler6 *mdcCal1_n_m2;  //! mdcCal1: Plane III, avg. mult.
   HQAScaler6 *mdcCal1_n_m3;  //! mdcCal1: Plane IV, avg. mult.

   // Tof scalers
   HQAScaler6 *tofHit_n;      //! tofHit: multiplicity/event

   // Tofino scalers

   // Shower scalers
   HQAScaler6 *shoHit_n;    //! showerHit: multiplicity/event
   HQAScaler6 *shoHitTof_n; //! showerHit: multiplicity/event

   // Kick scalers
   HQAScaler6 *kickTrack_n;  //! kickTrack: tracks/event

   // Trigger scalers
   HQAScaler6 *trigRich_n;      //! trigRich    : RichIPU   hits/event
   HQAScaler6 *trigTof_n;       //! trigTof     : TofIPU    hits/event
   HQAScaler6 *trigShower_n;    //! trigShower  : ShowerIPU hits/event
   HQAScaler6 *trigTlepton_n;   //! trigTlepton : MU Leptons from Tof hits    / event
   HQAScaler6 *trigSlepton_n;   //! trigSlepton : MU Leptons from Shower hits / event

   ClassDef(HQAScalers, 1) // QA scalers
};


//----------------------------------------------------------------------//

// HQAScaler class - wraps Double_t to take advantage of TNamed/TObject
// functionality (such as including in TCollections)

class HQAScaler : public TNamed {

public:
   HQAScaler() {
      ;
   }
   inline HQAScaler(const Char_t *name, const Char_t *title);
   virtual ~HQAScaler() {
      ;
   }

   inline HQAScaler operator=(Double_t d);

   Double_t fData;

   ClassDef(HQAScaler, 1) // TObject wrapper DOUBLE
};

inline HQAScaler::HQAScaler(const Char_t *name, const Char_t *title)
   : TNamed(name, title)
{

   fData = 0;
}

inline HQAScaler HQAScaler::operator=(Double_t d)
{

   fData = d;
   return *this;
}

//----------------------------------------------------------------------//

// HQAScaler class - wraps Double_t to take advantage of TNamed/TObject
// functionality (such as including in TCollections)

class HQAScaler6 : public TNamed {

public:
   HQAScaler6() {
      ;
   }
   inline HQAScaler6(const Char_t *name, const Char_t *title);
   virtual ~HQAScaler6() {
      ;
   }

   inline HQAScaler6& operator=(Double_t d);
   inline Double_t& operator[](Int_t sec);

   Double_t fData[6];

   ClassDef(HQAScaler6, 1) // TObject wrapper for double[6]
};

inline HQAScaler6::HQAScaler6(const Char_t *name, const Char_t *title)
   : TNamed(name, title)
{

   for (Int_t i = 0; i < 6; i++) fData[i] = 0;
}

inline HQAScaler6& HQAScaler6::operator=(Double_t d)
{

   for (Int_t i = 0; i < 6; i++) fData[i] = d; // set all scalers to value d
   return *this;
}

inline Double_t& HQAScaler6::operator[](Int_t sec)
{

   return fData[sec];
}


#endif
