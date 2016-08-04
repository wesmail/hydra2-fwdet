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
//  HRichCalParCell
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHCALPARCELL_H
#define HRICHCALPARCELL_H

#include "hlocateddataobject.h"

class HRichCalParCell : public HLocatedDataObject {

private:

   Short_t fSector;
   Short_t fRow;
   Short_t fCol;

   Float_t fOffset;
   Float_t fSigma;

public:

   HRichCalParCell();
   HRichCalParCell(Float_t offset, Float_t sigma);
   ~HRichCalParCell() {}

   void reset();


///////////////////////////////////////////////////////////////////
// GETTERS
   Float_t getOffset()  const;
   Float_t getSigma()   const;
   Short_t getSector()  const;
   Short_t getRow()     const;
   Short_t getCol()     const;
   Int_t   getAddress() const;
   Int_t   getNLocationIndex(void);
   Int_t   getLocationIndex(Int_t i);


///////////////////////////////////////////////////////////////////
// SETTERS
   void setOffset(Float_t o);
   void setSigma(Float_t s);
   void setParams(Float_t offset, Float_t sigma);
   void setSector(Short_t s);
   void setRow(Short_t r);
   void setCol(Short_t c);

   ClassDef(HRichCalParCell, 1) // Calibration data of a single cell
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Float_t HRichCalParCell::getOffset() const
{
   return fOffset;
}

inline Float_t HRichCalParCell::getSigma() const
{
   return fSigma;
}

inline Short_t HRichCalParCell::getSector() const
{
   return fSector;
}

inline Short_t HRichCalParCell::getRow() const
{
   return fRow;
}

inline Short_t HRichCalParCell::getCol() const
{
   return fCol;
}

inline Int_t HRichCalParCell::getAddress() const
{
   return 10000 * (fSector ? fSector : 6) + 100 * fRow + fCol;
}

inline Int_t HRichCalParCell::getNLocationIndex()
{
   return 3;
}

inline Int_t HRichCalParCell::getLocationIndex(Int_t i)
{
   switch (i) {
      case 0 :
         return getSector();
         break;
      case 1 :
         return getRow();
         break;
      case 2 :
         return getCol();
         break;
   }
   return -1;
}


///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichCalParCell::setOffset(Float_t o)
{
   fOffset = o;
}

inline void HRichCalParCell::setSigma(Float_t s)
{
   fSigma = s;
}

inline void HRichCalParCell::setParams(Float_t offset, Float_t sigma)
{
   fOffset = offset;
   fSigma  = sigma;
}


inline void HRichCalParCell::setSector(Short_t s)
{
   fSector = s;
}

inline void HRichCalParCell::setRow(Short_t r)
{
   fRow = r;
}

inline void HRichCalParCell::setCol(Short_t c)
{
   fCol = c;
}



inline void HRichCalParCell::reset()
{
   setParams(0.0, 0.0);
}

#endif // HRICHCALPARCELL_H







