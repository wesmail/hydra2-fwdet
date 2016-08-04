//*-- AUTHOR :   Pablo Cabanelas 
//*-- Created :  17/10/2005

/////////////////////////////////////////////////////////////
//  HRpcParRootFileIo
//
//  Class for RPC parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParRootFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hrpcparrootfileio.h"
#include "hparrootfileio.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hrpcdetector.h"
#include "hrpccalpar.h"
#include "hrpcgeompar.h"
#include "hrpctrb2lookup.h"
#include <iostream> 
#include <iomanip>

ClassImp(HRpcParRootFileIo)

HRpcParRootFileIo::HRpcParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HRpcParIo"
  fName="HRpcParIo";
  HDetector* det=gHades->getSetup()->getDetector("Rpc");
  Int_t n = det->getMaxSectors();
  initModules=new TArrayI(n);
}

HRpcParRootFileIo::~HRpcParRootFileIo(void) {
  // destructor
  if (initModules) {
    delete initModules;
    initModules=0;
  }
}

Bool_t HRpcParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name, but only the modules
  // defined in the array 'set'
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file
  if (!isActiv) readModules("Rpc");
  const Text_t* name=pPar->GetName();
  if (pFile) {
    if (strcmp(name,"RpcCalPar")==0)   return read((HRpcCalPar*)pPar,set);
    if (strcmp(name,"RpcGeomPar")==0)
        return HDetParRootFileIo::read((HRpcGeomPar*)pPar,set);
    if (strcmp(name,"RpcTrb2Lookup")==0)
        return HDetParRootFileIo::read((HRpcTrb2Lookup*)pPar);
  }
  cerr<<"initialization of "<<name<<" not possible from ROOT file!"<<endl;
  return kFALSE;
}

Bool_t HRpcParRootFileIo::read(HRpcCalPar* pPar,Int_t* set) {
  // reads and fills the container "RpcCalPar" for calibration parameters
  Text_t* name=(Char_t*)pPar->GetName();
  Int_t version=findInputVersion(name);

  if (version<=0) {
    pPar->setInputVersion(-1,inputNumber);
    return kFALSE;    // not in ROOT file
  }
  if (pPar->getInputVersion(inputNumber)==version
      && pPar->getInputVersion(inputNumber)!=-1) return kTRUE;
  // needs reinitialization
  HRpcCalPar* r=(HRpcCalPar*)findContainer(name,version);
  Bool_t allFound=kTRUE;

  initModules->Reset();
  HRpcCalPar& rRpc=*r;
  HRpcCalPar& pRpc=*pPar;
  for(Int_t sect=0;sect<pRpc.getSize();sect++) {
    HRpcCalParSec& rSec=rRpc[sect];
    HRpcCalParSec& pSec=pRpc[sect];
    if (set[sect]) {//Note: In the RPC, sector and module is the same.
      for(Int_t col=0;col<pSec.getSize();col++) {
	HRpcCalParCol& rCol=rSec[col];
	HRpcCalParCol& pCol=pSec[col];
	  
	if (pCol.getSize() && rCol.getSize()==pCol.getSize()) {
	  for(Int_t cell=0;cell<pCol.getSize();cell++) pCol[cell].fill(rCol[cell]);
	}
	else allFound=kFALSE;
      }
      set[sect]=0;
      initModules->AddAt(sect+1,sect);
    }
  }

  pPar->setInputVersion(version,inputNumber);
  pPar->setChanged();
  printInfo("RpcCalPar: module(s) initialized from Root file:\n ");
  delete r;
  return allFound;
}
