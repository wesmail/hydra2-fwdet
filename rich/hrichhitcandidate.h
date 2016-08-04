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
//  HRichHitCandidate
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHHITCANDIDATE_H
#define HRICHHITCANDIDATE_H

#include "TArrayI.h"
#include "TObject.h"

class HRichHitCandidate: public TObject {

private:

   Int_t fX;
   Int_t fY;
   Int_t fA;
   Int_t fPadLabel;
   Int_t fMaxLabel;
   Float_t xMean;
   Float_t yMean;
   Int_t nEqualNeighbors;

public:

   HRichHitCandidate();
   HRichHitCandidate(const Int_t x, const Int_t y, const Int_t a, const Int_t p, const Int_t l);
   virtual ~HRichHitCandidate();

   void Reset();

   Int_t getX() {
      return fX;
   }
   Int_t getY() {
      return fY;
   }
   Int_t getA() {
      return fA;
   }
   Float_t getXMean() {
      return xMean;
   }
   Float_t getYMean() {
      return yMean;
   }
   Int_t getPadLabel() {
      return fPadLabel;
   }
   Int_t getMaxLabel() {
      return fMaxLabel;
   }
   Int_t getNoEqualNeighbors() {
      return nEqualNeighbors;
   }

   void setX(const Int_t x) {
      fX = x;
   }
   void setY(const Int_t y) {
      fY = y;
   }
   void setA(const Int_t a) {
      fA = a;
   }
   void setXMean(const Float_t x) {
      xMean = x;
   }
   void setYMean(const Float_t y) {
      yMean = y;
   }
   void setPadLabel(const Int_t p) {
      fPadLabel = p;
   }
   void setMaxLabel(const Int_t l) {
      fMaxLabel = l;
   }
   void setNoEqualNeighbors(const Int_t n) {
      nEqualNeighbors = n;
   }

   Bool_t  IsSortable() const {
      return kTRUE;
   }
   Int_t Compare(const TObject *obj) const {
      if (fA == ((HRichHitCandidate*)obj)->getA()) return 0;
      return (fA > ((HRichHitCandidate*)obj)->getA()) ? 1 : -1;
   }

   ClassDef(HRichHitCandidate, 1)
};

#endif // HRICHHITCANDIDATE_H
