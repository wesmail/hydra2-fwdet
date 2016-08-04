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


#include "TMath.h"

#include "hades.h"
#include "hparamlist.h"
#include "hrichdetector.h"
#include "hrichgeometrypar.h"
#include "hspectrometer.h"
#include "richdef.h"

#include <cmath>
#include <iostream>

using namespace std;

ClassImp(HRichGeometryPar)

HRichGeometryPar::HRichGeometryPar(const Char_t* name,
                                   const Char_t* title,
                                   const Char_t* context)
   : HParCond(name, title, context)
{
   clear();
}

HRichGeometryPar::~HRichGeometryPar()
{
   clear();
}

void
HRichGeometryPar::clear()
{
   fSectorsNr         = 0;
   fColumns           = 0;
   fRows              = 0;
   fDistanceWiresPads = 0.;
   fSectorShift       = 0.;

   fSectorPhi.Set(6);
   fSectorActive.Set(6);
   fSectorPhi.Reset(0.);
   fSectorActive.Reset(0);
}

void
HRichGeometryPar::initParameters()
{
// Here we calculate x, y, z in LAB and phi and store it for each pad.
// The calculated phi value is compared to the value stored in oracle

   Float_t x;
   Float_t y;
   Float_t z;
   Float_t xl;
   Float_t yl;
   Float_t zl;
   Float_t len;
   Float_t phi;
   Float_t sphi;

   for (Int_t i = 0; i < 6; i++) {
      if (getSectorActive(i) > 0) {
         for (Int_t j = 0; j < getPadsNr(); j++) {
            x = fPads.getPad(j)->getY(); // local y is X in lab before rotation and shift
            y = fPads.getPad(j)->getX(); // local x is Y in lab before rotation and shift
            z = 0.;

            xl = cos(20. * TMath::DegToRad()) * x;
            yl = y;
            zl = sin(20. * TMath::DegToRad()) * x;

            //   xl += fSectorShift;

            x = cos(fSectorPhi[i] * TMath::DegToRad()) * xl -
                sin(fSectorPhi[i] * TMath::DegToRad()) * yl;
            y = sin(fSectorPhi[i] * TMath::DegToRad()) * xl +
                cos(fSectorPhi[i] * TMath::DegToRad()) * yl;
            z = zl;

            fPads.getPad(j)->setXYZlab(i, x, y, z);

            len = sqrt(xl * xl + yl * yl);
            phi = 0.;
            if (len > 0)
               phi = TMath::RadToDeg() * acos(xl / len);
            if (yl < 0)
               phi *= -1.;

            phi += fSectorPhi[i];

            sphi = fPads.getPad(j)->getPhi(i);
            if (sphi > 0. && sphi - phi > 1.e-2) {
               Error("initParameters", "Inconsistency between phi from param source and calculated phi");
               Error("initParameters", "(from pad plane coord in mm)");
               Error("initParameters", "ora phi: % f,  calc phi: % f", sphi, phi);
            }

            fPads.getPad(j)->setPhi(i, phi);

         }
      }
   }
}

Bool_t
HRichGeometryPar::getParams(HParamList* l)
{

   HRichDetector *pRichDet = static_cast<HRichDetector*>(gHades->getSetup()->getDetector("Rich"));
   if (NULL == pRichDet) {
      Error("getParams", "Pointer to RICH Detector not initialized...");
      return kFALSE;
   }

   for (Int_t i = 0; i < 6; ++i) {
      fSectorActive[i] = pRichDet->getModule(i, 0);
      if (fSectorActive[i] > 0) {
         fSectorsNr++;
      }
   }
   if (fSectorsNr > 0) {
      fColumns = RICH_MAX_COLS;
      fRows    = RICH_MAX_ROWS;
   }

   // sector parameters
   for (Int_t i = 0; i < 6; ++i) {
      if (fSectorActive[i] > 0) {
         switch (i) {
            case 0:
               fSectorPhi[i] = 90.;
               break;
            case 1:
               fSectorPhi[i] = 150.;
               break;
            case 2:
               fSectorPhi[i] = 210.;
               break;
            case 3:
               fSectorPhi[i] = 270.;
               break;
            case 4:
               fSectorPhi[i] = 330.;
               break;
            case 5:
               fSectorPhi[i] = 30.;
               break;
         }
      }
   }

   if (NULL == l) return kFALSE;
   if (kFALSE == l->fill("fDistanceWiresPads", &fDistanceWiresPads)) return kFALSE;
   if (kFALSE == l->fill("fSectorShift", &fSectorShift)) return kFALSE;

   if (fSectorsNr > 0) {
      if (kFALSE == fFrame.getParams(l)) return kFALSE;
      if (kFALSE == fWires.getParams(l)) return kFALSE;
      if (kFALSE == fPads.getParams(l))  return kFALSE;
   }

   initParameters();

#if DEBUG_LEVEL > 1
   printParams();
#endif
   return kTRUE;
}

void
HRichGeometryPar::printParams()
{
   cout << "HRichGeometryParameters" << endl;
   cout << "==========================================" << endl;
   cout << "fDistanceWiresPads     " << fDistanceWiresPads << endl;
   cout << "fSectorShift           " << fSectorShift << endl;
   cout << endl;

   fFrame.printParams();
   fWires.printParams();

#if DEBUG_LEVEL > 2
   fPads.printParams();
#endif

}

void
HRichGeometryPar::putParams(HParamList* l)
{
   if (NULL == l) return;
   l->add("fDistanceWiresPads", fDistanceWiresPads);
   l->add("fSectorShift", fSectorShift);

   if (fSectorsNr > 0) {
      fFrame.putParams(l);
      fWires.putParams(l);
      fPads.putParams(l);
   }
}
