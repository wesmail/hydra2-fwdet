//*-- AUTHOR : Ilse Koenig
//*-- Createded : 10/03/2000
//*--Changed: F. Krizek  26/08/2005
//Modified by M.Golubeva 08.10.2005
//Modified by M.Golubeva 01.11.2006
// Adjusted for RICH700 by JAM (j.adamczewski@gsi.de) March 2018

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HRich700GeomPar
//
// Container class for the basic RICH700 geometry parameters
//
//////////////////////////////////////////////////////////////////////////////

using namespace std;
#include "hrich700geompar.h"
#include "hgeomcompositevolume.h"
#include "hpario.h"
#include "hdetpario.h"
#include <iostream> 
#include <iomanip>

ClassImp(HRich700GeomPar)

HRich700GeomPar::HRich700GeomPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
            : HDetGeomPar(name,title,context,"Rich") {
  // constructor calling the base class constructor with the detector name

	maxSectors=-1; // required for ascii input for new rich
	//printf ("HRich700GeomPar CTOR with %d sectors and %d components\n",maxSectors, numComponents);

}

Bool_t HRich700GeomPar::init(HParIo* inp,Int_t* set) {
  // intitializes the container from an input
  HDetParIo* input=inp->getDetParIo("HRichParIo");
  if (input) return (input->init(this,set));
  return kFALSE;
}

Int_t HRich700GeomPar::write(HParIo* output) {
  // writes the container to an output
  HDetParIo* out=output->getDetParIo("HRichParIo");
  if (out) return out->write(this);
  return -1;
}

Int_t HRich700GeomPar::getCompNum(const TString& name) {
	// JAM this is used in hdetparascifileio

	 // we have names of kind RPMT9  --- RPMT568
    TString num=name;
//	Int_t rev=num.Remove(0,4).Atoi();
//	printf ("HRich700GeomPar::getCompNum gets %d from %s \n",rev, name.Data());
//	return rev;
	return (num.Remove(0,4).Atoi());
}




TString HRich700GeomPar::getCellName(const Int_t c) {
// JAM do we need this? WALL uses this in digitizer
	//printf ("HRich700GeomPar::getCellName for id %d\n",c);
	return TString::Format("RPMT%d",c);
}
