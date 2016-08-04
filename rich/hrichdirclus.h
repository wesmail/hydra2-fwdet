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
//  HRichDirClus
//
// This class stores information about the clusters in the RICH
// detector like cluster size, total charge accumulated in the
// cluster and number of fired pads in the cluster.
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHDIRCLUS_H
#define HRICHDIRCLUS_H

#include "TObject.h"

class HRichDirClus: public TObject {

private:
   Int_t   fSector;
   Int_t   fNrPads;
   Float_t fDimX;
   Float_t fDimY;
   Float_t fTotalCharge;
   Float_t fPhiDiff;
   Float_t fTheta;
   Float_t fMeanX;
   Float_t fMeanY;

public:
   HRichDirClus();
   virtual ~HRichDirClus() {}

   ///////////////////////////////////////////////////////////////////
   // GETTERS
   Int_t   getSector();
   Int_t   getNrPad();
   Float_t getXDim();
   Float_t getYDim();
   Float_t getTotalCharge();
   Float_t getPhiDiff();
   Float_t getTheta();
   Float_t getX();
   Float_t getY();

   ///////////////////////////////////////////////////////////////////
   // SETTERS
   void setXYDim(Float_t x, Float_t y);
   void setSector(Int_t s);
   void setTotalCharge(Float_t ch);
   void setNrPad(Int_t padnr);
   void setPhiDiff(Float_t p);
   void setTheta(Float_t t);
   void setX(Float_t x);
   void setY(Float_t y);

   ClassDef(HRichDirClus, 1) // RICH direct clusters
};

///////////////////////////////////////////////////////////////////
// GETTERS
inline Int_t HRichDirClus::getSector()
{
   return fSector;
}
inline Int_t HRichDirClus::getNrPad()
{
   return fNrPads;
}
inline Float_t HRichDirClus::getXDim()
{
   return fDimX;
}
inline Float_t HRichDirClus::getYDim()
{
   return fDimY;
}
inline Float_t HRichDirClus::getTotalCharge()
{
   return fTotalCharge;
}
inline Float_t HRichDirClus::getPhiDiff()
{
   return fPhiDiff;
}
inline Float_t HRichDirClus::getTheta()
{
   return fTheta;
}
inline Float_t HRichDirClus::getX()
{
   return fMeanX;
}
inline Float_t HRichDirClus::getY()
{
   return fMeanY;
}

///////////////////////////////////////////////////////////////////
// SETTERS
inline void HRichDirClus::setXYDim(Float_t x, Float_t y)
{
   fDimX = x;
   fDimY = y;
}
inline void HRichDirClus::setSector(Int_t s)
{
   fSector = s;
}
inline void HRichDirClus::setTotalCharge(Float_t ch)
{
   fTotalCharge = ch;
}
inline void HRichDirClus::setNrPad(Int_t padnr)
{
   fNrPads = padnr;
}
inline void HRichDirClus::setPhiDiff(Float_t p)
{
   fPhiDiff = p;
}
inline void HRichDirClus::setTheta(Float_t t)
{
   fTheta = t;
}
inline void HRichDirClus::setX(Float_t x)
{
   fMeanX = x;
}
inline void HRichDirClus::setY(Float_t y)
{
   fMeanY = y;
}

#endif // HRICHDIRCLUS_H
