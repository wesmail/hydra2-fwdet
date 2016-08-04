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
//  HRichCal
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHCAL_H
#define HRICHCAL_H

#include "hlocateddataobject.h"

class HRichCal : public HLocatedDataObject {

private:

   Float_t fCharge;
   Int_t   fSector;
   Int_t   fRow;
   Int_t   fCol;
   Int_t   fEventNr;
   Bool_t  isCleanedSingle;
   Bool_t  isCleanedHigh;
   Bool_t  isCleanedSector;

public:

   HRichCal();
   HRichCal(Float_t q);
   HRichCal(Int_t s, Int_t r, Int_t c);
   virtual ~HRichCal() {}

   ///////////////////////////////////////////////////////////////////
   // GETTERS
   Float_t getCharge(void)          const;
   Int_t   getSector(void)          const;
   Int_t   getRow(void)             const;
   Int_t   getCol(void)             const;
   Int_t   getEventNr(void)         const;
   Int_t   getAddress(void)         const;
   Bool_t  getIsCleanedSingle(void) const;
   Bool_t  getIsCleanedHigh(void)   const;
   Bool_t  getIsCleanedSector(void) const;
   Bool_t  IsSortable(void)         const;

   ///////////////////////////////////////////////////////////////////
   // SETTERS
   void setCharge(Float_t q);
   void setSector(Int_t s);
   void setRow(Int_t r);
   void setCol(Int_t c);
   void setIsCleanedSingle(Bool_t b);
   void setIsCleanedHigh(Bool_t   b);
   void setIsCleanedSector(Bool_t b);
   void setEventNr(Int_t e);

   Float_t addCharge(Float_t q);

   ClassDef(HRichCal, 1) //Rich cal data
};


///////////////////////////////////////////////////////////////////
// GETTERS
inline Float_t HRichCal::getCharge(void) const
{
   return fCharge;
}
inline Int_t HRichCal::getSector(void) const
{
   return fSector;
}
inline Int_t HRichCal::getRow(void) const
{
   return fRow;
}
inline Int_t HRichCal::getCol(void) const
{
   return fCol;
}
inline Int_t HRichCal::getEventNr(void) const
{
   return fEventNr;
}
inline Int_t HRichCal::getAddress(void) const
{
   return 10000 * (fSector ? fSector : 6) + 100 * fRow + fCol;
}
inline Bool_t HRichCal::IsSortable(void) const
{
   return kTRUE;   // the class is sortable by the charge
}
inline Bool_t HRichCal::getIsCleanedSingle(void) const
{
   return isCleanedSingle;
}
inline Bool_t HRichCal::getIsCleanedHigh(void) const
{
   return isCleanedHigh;
}
inline Bool_t HRichCal::getIsCleanedSector(void) const
{
   return isCleanedSector;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichCal::setCharge(Float_t q)
{
   fCharge = q;
}
inline void HRichCal::setSector(Int_t s)
{
   fSector = s;
}
inline void HRichCal::setRow(Int_t r)
{
   fRow = r;
}
inline void HRichCal::setCol(Int_t c)
{
   fCol = c;
}
inline void HRichCal::setIsCleanedSingle(Bool_t b)
{
   isCleanedSingle = b;
}
inline void HRichCal::setIsCleanedHigh(Bool_t b)
{
   isCleanedHigh = b;
}
inline void HRichCal::setIsCleanedSector(Bool_t b)
{
   isCleanedSector = b;
}
inline void HRichCal::setEventNr(Int_t e)
{
   fEventNr = e;
}

inline Float_t HRichCal::addCharge(Float_t q)
{
   return fCharge += q;
}

#endif

