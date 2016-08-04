//*-- AUTHOR : Ilse Koenig
//*-- Created : 24/11/2014 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomMdcWire
//
// Class for a MDC wire in Geant and ROOT
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeommdcwire.h"

#include <iostream>
#include <iomanip>

using namespace std;

void HGeomMdcWire::print() {
  cout<<"  "<<wireNumber<<"  "<<wireName.Data()<<"  "<<copyNumber<<"  "<<wireType
      <<"  "<<gparams[0] <<"  "<<gparams[1]<<"  "<<gparams[2]
      <<"  "<<gpos[0]<<"  "<<gpos[1]<<"  "<<gpos[2]<<endl;
}
