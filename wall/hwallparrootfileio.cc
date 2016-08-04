//*-- AUTHOR : Ilse Koenig
//*-- Created : 17/01/2005
// Modified by M.Golubeva 01.11.2006

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////////////
//
//  HWallParRootFileIo
//
//  Class for Forward Wall parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////////////

using namespace std;
#include "hwallparrootfileio.h"
#include "hparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hwalldetector.h"
#include "hwalllookup.h"
#include "hwalltrb2lookup.h"
#include "hwallcalpar.h"
#include "hwallgeompar.h"
#include "hruntimedb.h"
#include <iostream> 
#include <iomanip>

ClassImp(HWallParRootFileIo)

HWallParRootFileIo::HWallParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HWallParIo"
  fName="HWallParIo";
  initModules=new TArrayI(1);
}

Bool_t HWallParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file
  if (!isActiv) readModules("Wall");
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"WallLookup")==0) return read((HWallLookup*)pPar,set);
    if (strcmp(name,"WallCalPar")==0) return read((HWallCalPar*)pPar,set);
    if (strcmp(name,"WallTrb2Lookup")==0)
        return HDetParRootFileIo::read((HWallTrb2Lookup*)pPar);
    if (strcmp(name,"WallGeomPar")==0)
        return HDetParRootFileIo::read((HWallGeomPar*)pPar,set);
  }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}

Bool_t HWallParRootFileIo::read(HWallLookup* pPar,Int_t* set) {
  // reads and fills the container "WallLookup" needed by the unpacker.
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
  HWallLookup* rWall=(HWallLookup*)findContainer(name,version);
  pPar->clear();
  Int_t cell;
  Char_t modType; // type of digital converter (A=ADC, T=TDC, U=undefined)
  Bool_t allFound=kFALSE;
  for(Int_t c=0;c<rWall->getSize();c++) {
    for(Int_t s=0;s<(*rWall)[c].getSize();s++) {
      HWallLookupSlot& slot=(*rWall)[c][s];
      modType=slot.getType();
      for(Int_t i=0;i<slot.getSize();i++) {
        cell=slot[i].getCell();
        if (cell>=0) {
          pPar->fill(c,s,modType,i,cell);
          allFound=kTRUE;
        }
      }
    }
  }
  if (allFound) {
    pPar->setInputVersion(version,inputNumber);
    pPar->setChanged();
    cout<<"WallLookup: module(s) initialized from ROOT file\n";
  }
  return allFound;
}
Bool_t HWallParRootFileIo::read(HWallCalPar* pPar,Int_t* set) {
  // reads and fills the container "WallCalPar" for calibration parameters
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);
  if (version==-1) {
   pPar->setInputVersion(-1,inputNumber);
   return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
        && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  pPar->clear();
  HWallCalPar* r=(HWallCalPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;
  HWallCalPar& rWall=*r;
  HWallCalPar& pWall=*pPar;
  Int_t pWallSize=pWall.getSize();
  if (pWallSize && (rWall.getSize()==pWallSize)) {
    for(Int_t c=0;c<pWallSize;c++) pWall[c].fill(rWall[c]);
  } else allFound=kFALSE;
  if (allFound) {
    pPar->setInputVersion(version,inputNumber);
    pPar->setChanged();
    cout<<"WallCalPar initialized from ROOT file\n";
  } else pPar->setInputVersion(-1,inputNumber);
  delete r;
  return allFound;
}















