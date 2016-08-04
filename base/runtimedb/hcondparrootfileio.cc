//*-- AUTHOR : Ilse Koenig
//*-- Created : 20/11/2002 by I.Koenig

//_HADES_CLASS_DESCRIPTION 
//////////////////////////////////////////////////////////////////////////////
// HCondParRootFileIo
//
// Interface class to ROOT file for input/output of parameters derived
// from HParCond
//
//////////////////////////////////////////////////////////////////////////////

#include "hcondparrootfileio.h"
#include "hparcond.h"

ClassImp(HCondParRootFileIo)

HCondParRootFileIo::HCondParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor
  // sets the name of the I/O class "HCondParIo"
  // gets the pointer to the ROOT file
  fName="HCondParIo";
}

Bool_t HCondParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // calls HDetParRootFileIo"::read(HParCond*)
  if (!pFile) return kFALSE; 
  if (pPar->InheritsFrom("HParCond"))
    return HDetParRootFileIo::read((HParCond*)pPar);
  Error("HCondParRootFileIo::init(HParSet*,Int_t*)",
        "%s does not inherit from HParCond",pPar->GetName());
  return kFALSE;
}
