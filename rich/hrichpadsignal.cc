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
//  HRichPadSignal
//
//  Pad parameters used in HRichAnalysis
//
//////////////////////////////////////////////////////////////////////////////


#include "hrichpadsignal.h"

#include <iomanip>
#include <iostream>

using namespace std;

ClassImp(HRichPadSignal)

//----------------------------------------------------------------------------
ostream& operator<< (ostream& output, HRichPadSignal& raw)
{
   output << "pad amplitude, label, lock: "
          << raw.fAmplitude << ", " << raw.fLabel << ", " << raw.fLock << endl;
   return output;
}
//============================================================================

