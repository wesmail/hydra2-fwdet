//////////////////////////////////////////////////////////////////////////////
//
// $Id:
//
//*-- Author : Martin Jurkovic <martin.jurkovic@ph.tum.de> 2010
//
//////////////////////////////////////////////////////////////////////////////

#ifndef HSTART2HITFPAR_H_
#define HSTART2HITFPAR_H_

#include "TArrayF.h"

#include "hparcond.h"

class HParamList;

class HStart2HitFPar : public HParCond {

private:

   Int_t   fModules;      // Number of modules
   Int_t   fStrips;       // Number of strips (components)
   TArrayF fMeanTime;     // Mean of the time distribution
   TArrayF fWidth;        // Width of the distribution

private:

   HStart2HitFPar(const HStart2HitFPar& source);
   HStart2HitFPar& operator=(const HStart2HitFPar& source);

public:

   HStart2HitFPar(const Char_t* name    = "Start2HitFPar",
                  const Char_t* title   = "Start2 Hit Finder Parameters",
                  const Char_t* context = "Start2HitFParProduction") :
      HParCond(name, title, context),
      fModules(0), fStrips(0) {}
   ~HStart2HitFPar() {}

   void    clear();
   void    printParams();
   void    putParams(HParamList* l);
   Bool_t  getParams(HParamList* l);

   UInt_t  getModules();
   UInt_t  getStrips();
   Float_t getMeanTime(Int_t m, Int_t s);
   Float_t getWidth(Int_t m, Int_t s);

   ClassDef(HStart2HitFPar, 1) // Container for the START2 hit finder parameters
};

inline UInt_t  HStart2HitFPar::getModules()
{
   return fModules;
}

inline UInt_t  HStart2HitFPar::getStrips()
{
   return fStrips;
}

inline Float_t HStart2HitFPar::getMeanTime(Int_t m, Int_t s)
{
   if (m >= 0 && m < fModules &&
       s >= 0 && s < fStrips  &&
       (fStrips * m + s) < fMeanTime.GetSize()) {
      return fMeanTime[fStrips * m + s];
   }
   return -1e10;
}

inline Float_t HStart2HitFPar::getWidth(Int_t m, Int_t s)
{
   if (m >= 0 && m < fModules &&
       s >= 0 && s < fStrips  &&
       (fStrips * m + s) < fWidth.GetSize()) {
      return fWidth[fStrips * m + s];
   }
   return -1e10;
}

#endif /*HSTART2HITFPAR_H_*/
