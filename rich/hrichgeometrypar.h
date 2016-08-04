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
//  HRichGeometryPar
//
//
//////////////////////////////////////////////////////////////////////////////


#ifndef HRICHGEOMETRYPAR_H
#define HRICHGEOMETRYPAR_H

#include "TArrayF.h"
#include "TArrayI.h"

#include "hparcond.h"
#include "hrichframe.h"
#include "hrichpad.h"
#include "hrichpadtab.h"
#include "hrichwiretab.h"

class HParamList;

class HRichGeometryPar : public HParCond {

private:

   Int_t   fSectorsNr;
   TArrayI fSectorActive;
   Int_t   fColumns;
   Int_t   fRows;
   Float_t fDistanceWiresPads;   // Distance between the wire and the padplane
   TArrayF fSectorPhi;
   Float_t fSectorShift; // this number corresponds to the shift in Y (lab coordinates)
   // of the Rich volume in HGeant. The factor fSectorShift/cos(20)
   // has to be added to the y coordinate in the detector ( pad plane)
   // reference system in order to get the correct pad number
   // and the correct theta. This correction takes place in the
   // hrichdigitizer class.

   HRichFrame     fFrame;
   HRichPadTab    fPads;
   HRichWireTab   fWires;

public:

   HRichGeometryPar(const Char_t* name    = "RichGeometryParameters",
                    const Char_t* title   = "Rich Geometry Parameters",
                    const Char_t* context = "RichGeometryParProduction");
   virtual ~HRichGeometryPar();

   Int_t incrementSectorNr();
   Int_t calcCol(Int_t padnr);
   Int_t calcRow(Int_t padnr);

   void   clear();
   void   initParameters();
   void   printParams();
   void   putParams(HParamList* l);
   Bool_t getParams(HParamList* l);


// Getters
   Int_t   getSectorsNr();
   Int_t   getColumns();
   Int_t   getRows();
   Int_t   getPadsNr();
   Int_t   getSectorActive(Int_t s);
   Int_t   getPadStatus(Int_t padnr);
   Float_t getSectorShift();
   Float_t getDistanceWiresPads();
   Float_t getSectorPhi(Int_t s);
   HRichFrame*   getFramePar();
   HRichWireTab* getWiresPar();
   HRichPadTab*  getPadsPar();


// Setters
   void setSectorActive(Int_t s, Int_t i);
   void setColumns(Int_t c);
   void setRows(Int_t r);
   void setSectorPhi(Int_t s, Float_t phi);
   void setSectorShift(Float_t shift);
   void setDistanceWiresPads(Float_t dis);

   ClassDef(HRichGeometryPar, 1) //Geometry parameters

};

/////////////////////////////////////////////

inline Int_t HRichGeometryPar::incrementSectorNr() {
      return ++fSectorsNr;
   }
inline Int_t HRichGeometryPar::calcCol(Int_t padnr) {
      return (padnr % fColumns);
   }
inline Int_t HRichGeometryPar::calcRow(Int_t padnr) {
      return (padnr / fColumns);
   }

// Getters
inline Int_t HRichGeometryPar::getSectorsNr() {
      return fSectorsNr;
   }
inline Int_t HRichGeometryPar::getColumns() {
      return fColumns;             // pads in X
   }
inline Int_t HRichGeometryPar::getRows() {
      return fRows;                // pads in Y
   }
inline Int_t HRichGeometryPar::getPadsNr() {
      return (fColumns * fRows);
   }
inline Int_t HRichGeometryPar::getSectorActive(Int_t s) {
      return (s >= 0 && s < 6) ? fSectorActive[s] : -1;
   }
inline Int_t HRichGeometryPar::getPadStatus(Int_t padnr) {
      return fPads.getPad(padnr)->getPadActive();
   }
inline Float_t HRichGeometryPar::getSectorShift() {
      return fSectorShift;
   }
inline Float_t HRichGeometryPar::getDistanceWiresPads() {
      return fDistanceWiresPads;
   }
inline Float_t HRichGeometryPar::getSectorPhi(Int_t s) {
      return (s >= 0 && s < 6) ? fSectorPhi[s] : -1;
   }
inline HRichFrame* HRichGeometryPar::getFramePar() {
      return &fFrame;
   }
inline HRichWireTab* HRichGeometryPar::getWiresPar() {
      return &fWires;
   }
inline HRichPadTab* HRichGeometryPar::getPadsPar()  {
      return &fPads;
   }


// Setters
inline void HRichGeometryPar::setSectorActive(Int_t s, Int_t i) {
      fSectorActive[s] = i;
   }
inline void HRichGeometryPar::setColumns(Int_t c) {
      fColumns = c;
   }
inline void HRichGeometryPar::setRows(Int_t r) {
      fRows = r;
   }
inline void HRichGeometryPar::setSectorPhi(Int_t s, Float_t phi) {
      fSectorPhi[s] = phi;
   }
inline void HRichGeometryPar::setSectorShift(Float_t shift) {
      fSectorShift = shift;
   }
inline void HRichGeometryPar::setDistanceWiresPads(Float_t dis) {
      fDistanceWiresPads = dis;
   }

#endif // HRICHGEOMETRYPAR_H
