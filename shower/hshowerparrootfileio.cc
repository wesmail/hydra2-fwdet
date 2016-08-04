//*-- AUTHOR : Ilse Koenig
//*-- Modified : 02/11/98
//*-- Modified : 10/08/2001 by J. Otwinowski 
//*-- Modified : 30/08/2001 by T. Wojcik
//*-- Modified : 28/02/2002 by J. Otwinowski 

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//HShowerParRootFileIo
//
//  Class for Shower parameter input/output from/into ROOT file
//
//  It is derived from the base class HDetParFileIo and
//  inherits from it basic functions e.g. write(...)
//
/////////////////////////////////////////////////////////////

using namespace std;
#include "hshowerparrootfileio.h"
#include "hshowergeompar.h"
#include "hshowerhist.h"

#include <iostream> 
#include <iomanip>

ClassImp(HShowerParRootFileIo)

HShowerParRootFileIo::HShowerParRootFileIo(HParRootFile* f) : HDetParRootFileIo(f) {
  // constructor sets the name of the detector I/O "HShowerParIo"
  fName="HShowerParIo";
  initModules=new TArrayI(18);
}


HShowerParRootFileIo::~HShowerParRootFileIo() {
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


Bool_t HShowerParRootFileIo::init(HParSet* pPar,Int_t* set) {
  // initializes a container called by name, but only the modules
  // defined in the array 'set'
  // calls the special read function for this container
  // If it is called the first time it reads the setup found in the file

  if (!isActiv) readModules("Shower");
  const Text_t* name = ((TNamed*)pPar)->GetName();
  if (strcmp(name,"ShowerHist")==0) {
    ((HShowerHist*)pPar)->defaultInit(); //nothing to read
    return kTRUE;  
  }
  if (pFile) {
    if (strcmp(name,"ShowerGeomPar")==0)     return read((HShowerGeomPar*)pPar,set);
    if (strcmp(name,"ShowerCalPar")==0)      return read(pPar);
  }
  Error("init(HParSet*,Int_t*)","initialization of %s not possible from ROOT file!",name);
  return kFALSE;
}
