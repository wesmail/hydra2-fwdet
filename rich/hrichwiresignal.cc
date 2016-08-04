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
//  Wire signal (charge calculated by Pola function).
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichwiresignal.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichWireSignal)

//----------------------------------------------------------------------------
ostream& operator<< (ostream& output, HRichWireSignal& raw)
{
   output << "sector, wire number, charge, xpos, ypos: "
          << raw.fSector << ", " << raw.fWireNr << ", "
          << raw.fCharge << ", " << raw.fXpos << ", " << raw.fYpos << endl;
   return output;
}
//============================================================================

//----------------------------------------------------------------------------
void HRichWireSignal::clear()
{
   fSector = 0;
   fWireNr = 0;
   fCharge = 0.;
   fXpos = 0.;
   fYpos = 0.;
}
//============================================================================

