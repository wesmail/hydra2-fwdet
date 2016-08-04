//*-- AUTHOR : Ilse Koenig
//*-- Modified : 06/12/2009 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//
// HSpecParRootFileIo
//
// Class for parameter input/output from/into ROOT file for
// the spectrometer (not related to a special detector)
//
/////////////////////////////////////////////////////////////

#include "hspecparrootfileio.h"
#include "hgeomvolume.h"
#include "hgeomshapes.h"
#include "hspecgeompar.h"

ClassImp(HSpecParRootFileIo)

HSpecParRootFileIo::HSpecParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor calls the base class constructor
  fName="HSpecParIo";
}


Bool_t HSpecParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function of the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"SpecGeomPar")==0) return read((HSpecGeomPar*)pPar,set);
    if (strcmp(name,"TrbLookup")==0) return HDetParRootFileIo::read(pPar);
    if (strcmp(name,"TrbnetAddressMapping")==0) return HDetParRootFileIo::read(pPar);
    if (strcmp(name,"SlowPar")==0) return HDetParRootFileIo::read(pPar);
  }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}


Bool_t HSpecParRootFileIo::read(HSpecGeomPar* pPar,Int_t* set) {
  // reads and fills the container "SpecGeomPar" which holds the geometry
  // parameters of the cave, the sectors and the target
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version<=0) {
    pPar->setInputVersion(-1,inputNumber);
    return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HSpecGeomPar* r=(HSpecGeomPar*)findContainer(name,version);
  pPar->clear();  
  Bool_t allFound=kTRUE;
  pPar->copyComment(*r);
  if (pPar->isFirstInitialization()) {
    if (set[0]) {
      HGeomVolume* cave=r->getCave();
      if (cave) {
        pPar->addCave(cave);
        set[0]=0;
      }
      else allFound=kFALSE; 
    }
    if (set[1]) {
      Int_t n=r->getNumSectors();
      if (n>0) {
        for(Int_t i=0;i<n;i++) pPar->addSector(r->getSector(i));
        set[1]=0;
      }
      else allFound=kFALSE; 
    }
  } else set[0]=set[1]=0;
  if (set[2]) {
    Int_t n=r->getNumTargets();
    if (n>0) {
      for(Int_t i=0;i<n;i++) pPar->addTarget(r->getTarget(i));
      set[2]=0;
    }
    else allFound=kFALSE; 
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  delete r;
  if (allFound) pPar->setNotFirstInit(); 
  return allFound;
}
  
