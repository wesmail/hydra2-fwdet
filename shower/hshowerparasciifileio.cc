//*-- AUTHOR : Denis Bertini
//*-- Last modified : 02/07/2002 by Ilse Koenig

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
//HShowerParAsciiFileIo
//
//  Class for Mdc parameter input/output from/into Asci file
//
/////////////////////////////////////////////////////////////
using namespace std;
#include "hshowerparasciifileio.h"
#include "hshowercalpar.h"
#include "hshowerhist.h"
#include "hparset.h"
#include <iostream> 
#include <iomanip>

ClassImp(HShowerParAsciiFileIo)

HShowerParAsciiFileIo::HShowerParAsciiFileIo(fstream* f) 
                            : HDetParAsciiFileIo(f) {
  fName="HShowerParIo";
}


Bool_t HShowerParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (!strcmp(name,"ShowerGeomPar"))
        return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
    if (!strncmp(name, "ShowerCalPar", strlen("ShowerCalPar")))
        return readFile<HShowerCalPar>((HShowerCalPar*)pPar);
    if (strcmp(name,"ShowerHist")==0)
        {
            ((HShowerHist*)pPar)->setStatic();
            return ((HShowerHist*)pPar)->defaultInit();
        }
    Error("init(HParSet*,Int_t*)",
          "initialization of %s not possible from ASCII file!",name);
    return kFALSE;
  }
  Error("init(HParSet*,Int_t*)","no input file open");
  return kFALSE;
}


Int_t HShowerParAsciiFileIo::write(HParSet* pPar) {
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    if (!strcmp(name,"ShowerGeomPar"))
        return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
    if (!strncmp(name, "ShowerCalPar", strlen("ShowerCalPar")))
        return writeFile<HShowerCalPar>((HShowerCalPar*)pPar);
    Error("write(HParSet*)",
          "No ASCII write interfact for %s",name);
    return -1;
  }
  Error("write(HParSet*)","no output file open");
  return -1;
}


