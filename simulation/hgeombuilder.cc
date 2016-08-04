//*-- AUTHOR : Ilse Koenig
//*-- Created : 11/11/03 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
///////////////////////////////////////////////////////////////////////////////
// HGeomBuilder
//
// Base class for GEANT and ROOT builder classes to create the geometry
//
///////////////////////////////////////////////////////////////////////////////

#include "hgeombuilder.h"

ClassImp(HGeomBuilder)

HGeomBuilder::HGeomBuilder() {
  // Default constructor
  nRot=0;
  nMed=0;
}

HGeomBuilder::HGeomBuilder(const char* name,const char* title)
            : TNamed(name,title) {
  // Constructor
  nRot=0;
  nMed=0;
}

