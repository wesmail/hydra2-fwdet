//*-- AUTHOR : Ilse Koenig
//*-- Modified : 08/12/2000 by I. Koenig
//*-- Modified : 12/09/2001 by D.Zovinec
//*-- Modified : 20/12/2001 by D.Zovinec

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//  HTofParRootFileIo
//
//  Class for TOF parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "htofparrootfileio.h"
#include "hparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "htofdetector.h"
#include "htoflookup.h"
#include "htoftrb2lookup.h"
#include "htofcalpar.h"
#include "htofdigipar.h"
#include "htofgeompar.h"
#include <iostream> 
#include <iomanip>

ClassImp(HTofParRootFileIo)

HTofParRootFileIo::HTofParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HTofParIo"
  fName="HTofParIo";
  HDetector* det=gHades->getSetup()->getDetector("Tof");
  Int_t n=(det->getMaxSectors())*(det->getMaxModules());
  initModules=new TArrayI(n);
}


HTofParRootFileIo::~HTofParRootFileIo() {
  // destructor
  if (modulesFound) {
    delete modulesFound;
    modulesFound=0;
  }
  if (initModules) {
    delete initModules;
    initModules=0;
  }
}

Bool_t HTofParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name, but only the modules
  // defined in the array 'set'
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file
  if (!isActiv) readModules("Tof");
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"TofLookup")==0) return read((HTofLookup*)pPar,set);
    if (strcmp(name,"TofCalPar")==0 || strcmp(name,"TofCalPar_Sim_Embedding")==0 ) return read((HTofCalPar*)pPar,set);
    if (strcmp(name,"TofDigiPar")==0) return read((HTofDigiPar*)pPar,set);
    if (strcmp(name,"TofTrb2Lookup")==0)
        return HDetParRootFileIo::read((HTofTrb2Lookup*)pPar);
    if (strcmp(name,"TofGeomPar")==0)
        return HDetParRootFileIo::read((HTofGeomPar*)pPar,set);
  }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}

Bool_t HTofParRootFileIo::read(HTofLookup* pPar,Int_t* set) {
  // reads and fills the container "TofLookup" needed by the unpacker.
  // Returns kFalse, when not initialized completely from this input
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version<=0) {
    pPar->setInputVersion(-1,inputNumber);
    return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HTofLookup* rTof=(HTofLookup*)findContainer(name,version);
  initModules->Reset();
  pPar->clear();   // no initialization from 2 inputs possible!
  Int_t pos, sector, module, cell;
  Char_t modType, side;
  for(Int_t c=0;c<rTof->getSize();c++) {
    for(Int_t s=0;s<(*rTof)[c].getSize();s++) {
      HTofLookupSlot& slot=(*rTof)[c][s];
      modType=slot.getType();
      for(Int_t i=0;i<slot.getSize();i++) {
        slot[i].getAddress(sector,module,cell,side);
        if (sector>=0) {
          pos=sector*22+module;
          if (set[pos]) {
            pPar->fill(c,s,modType,i,sector,module,cell,side);
            initModules->AddAt(pos+1,pos);
          }
        }
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  printInfo("TofLookup: module(s) initialized from Root file ");
  delete rTof;
  Bool_t allFound=kTRUE;
  for(Int_t i=0;i<initModules->GetSize(); i++) {
    if (set[i] && !initModules->At(i)) allFound=kFALSE;
  }
  return allFound;
}


Bool_t HTofParRootFileIo::read(HTofCalPar* pPar,Int_t* set) {
  // reads and fills the container "TofCalPar" for calibration parameters
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version==-1) {
    pPar->setInputVersion(-1,inputNumber);
    return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HTofCalPar* r=(HTofCalPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HTofCalPar& rTof=*r;
  HTofCalPar& pTof=*pPar;
  for(Int_t s=0;s<pTof.getSize();s++) {
    HTofCalParSec& rSec=rTof[s];
    HTofCalParSec& pSec=pTof[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HTofCalParMod& rMod=rSec[m];
      HTofCalParMod& pMod=pSec[m];
      pos=s*22+m;
      if (set[pos]) {
        Int_t pModSize=pMod.getSize();
        if (pModSize && rMod.getSize()==pModSize) {
          for(Int_t c=0;c<pModSize;c++) pMod[c].fill(rMod[c]);
          set[pos]=0;
          initModules->AddAt(pos+1,pos);
        } else allFound=kFALSE;
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  printInfo("TofCalPar: module(s) initialized from Root file:\n ");
  delete r;
  return allFound;
}

Bool_t HTofParRootFileIo::read(HTofDigiPar* pPar,Int_t* set) {
  // reads and fills the container "TofDigitPar" for digitization parameters
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version==-1) {
    pPar->setInputVersion(-1,inputNumber);
    return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HTofDigiPar* r=(HTofDigiPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  Int_t pos;
  initModules->Reset();
  HTofDigiPar& rTof=*r;
  HTofDigiPar& pTof=*pPar;
  for(Int_t s=0;s<pTof.getSize();s++) {
    HTofDigiParSec& rSec=rTof[s];
    HTofDigiParSec& pSec=pTof[s];
    for(Int_t m=0;m<pSec.getSize();m++) {
      HTofDigiParMod& rMod=rSec[m];
      HTofDigiParMod& pMod=pSec[m];
      pos=s*22+m;
      if (set[pos]) {
        Int_t pModSize=pMod.getSize();
        if (pModSize && rMod.getSize()==pModSize) {
          for(Int_t c=0;c<pModSize;c++) pMod[c].fill(rMod[c]);
          set[pos]=0;
          initModules->AddAt(pos+1,pos);
        } else allFound=kFALSE;
      }
    }
  }
  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  printInfo("TofDigiPar: module(s) initialized from Root file:\n ");
  delete r;
  return allFound;
}
