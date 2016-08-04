//*-- AUTHOR : Ilse Koenig
//*-- created : 10/03/2000 by Ilse Koenig
//*-- modified: 12/09/2001 by D.Zovinec

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////////
// HTofParAsciiFileIo
//
// Class for Tof parameter input/output from/into Ascii file
//
/////////////////////////////////////////////////////////////

#include "htofparasciifileio.h"
#include "htofdetector.h"
#include "htoflookup.h"
#include "htoftrb2lookup.h"
#include "htofcalpar.h"
#include "htofdigipar.h"
#include "htofgeompar.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"

ClassImp(HTofParAsciiFileIo)

HTofParAsciiFileIo::HTofParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="HTofParIo";
  HDetector* det=gHades->getSetup()->getDetector("Tof");
  setSize=(det->getMaxSectors())*(det->getMaxModules());
}

Bool_t HTofParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function for the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    pFile->clear();
    pFile->seekg(0,ios::beg);
    if (!strcmp(name,"TofLookup"))     return read((HTofLookup*)pPar,set,kTRUE);
    if (!strcmp(name,"TofCalPar"))     return read((HTofCalPar*)pPar,set);
    if (!strcmp(name,"TofDigiPar"))    return read((HTofDigiPar*)pPar,set);
    if (!strcmp(name,"TofTrb2Lookup")) return read((HTofTrb2Lookup*)pPar);
    if (!strcmp(name,"TofGeomPar"))
        return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
    Error("init(HParSet*,Int_t*)",
          "initialization of %s not possible from ASCII file",name);
    return kFALSE;
  }
  Error("init(HParSet*,Int_t*)","No input file open");
  return kFALSE;
}

Int_t HTofParAsciiFileIo::write(HParSet* pPar) {
  // calls the appropriate write function for the container
  if (pFile) {
    const  Text_t* name=pPar->GetName();
    if (!strcmp(name,"TofLookup"))     return writeFile3((HTofLookup*)pPar);
    if (!strcmp(name,"TofCalPar"))     return writeFile3((HTofCalPar*)pPar);
    if (!strcmp(name,"TofDigiPar"))    return writeFile3((HTofDigiPar*)pPar);
    if (!strcmp(name,"TofTrb2Lookup")) return write((HTofTrb2Lookup*)pPar);
    if (!strcmp(name,"TofGeomPar"))
        return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
    Error("write(HParSet*)","%s could not be written to ASCII file",name);
    return -1;
  }
  Error("write(HParSet*)","No output file open");
  return -1;
}

template<class T> Bool_t HTofParAsciiFileIo::read(T* pPar) {
  // template function for parameter containers
  // searches the container in the file, reads the data line by line and
  // called the member function readline(...) of the container class
  // does not check the detector setup
  if (!findContainer(pPar->GetName())) return kFALSE;
  pPar->clear();
  const Int_t maxbuf=155;
  Text_t buf[maxbuf];
  Bool_t rc=kTRUE;
  while (!pFile->eof()&&rc) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0') rc=pPar->readline(buf);
  }
  if (rc) {
    pPar->setInputVersion(1,inputNumber);
    pPar->setChanged();
    printf("%s initialized from Ascii file\n",pPar->GetName());
  }
  return rc;
}

template<class T> Bool_t HTofParAsciiFileIo::read(T* pPar, Int_t* set,
                                                  Bool_t needsClear) {
  // template function for all parameter containers
  // searches the container in the file, reads the data line by line and
  //   called the member function readline(...) of the container class
  const  Text_t* name=pPar->GetName();
  if (!findContainer(name)) return kFALSE;
  if (needsClear) pPar->clear();
  const Int_t maxbuf=155;
  Text_t buf[maxbuf];
  while (!pFile->eof()) {
    pFile->getline(buf, maxbuf);
    if (buf[0]=='#') break;
    if (buf[0]!='/' && buf[0]!='\0') pPar->readline(buf, set);
  }
  pPar->setInputVersion(1,inputNumber);
  pPar->setChanged();
  Bool_t allFound=kTRUE;
  if (!needsClear) {
    printf("%s: module(s) initialized from Ascii file:\n",name);
    allFound=checkAllFound(set,setSize);
  } else {
    for(Int_t i=0; i<setSize; i++) {
      if (set[i]) {
        if (set[i]==999) set[i]=1;
        else allFound=kFALSE;
      }
    }
  }
  if (allFound) printf("%s initialized from Ascii file\n",name);
  return allFound;
}

template<class T> Int_t HTofParAsciiFileIo::write(T* pPar) {
  // template function for parameter containers
  // calls the function putAsciiHeader(TString&) of the parameter container,
  // writes the header and calls the function write(fstream&) of the class
  pPar->putAsciiHeader(fHeader);
  writeHeader(pPar->GetName(),pPar->getParamContext());
  pPar->write(*pFile);
  pFile->write(sepLine,strlen(sepLine));
  pPar->setChanged(kFALSE);
  return 1;
}

template<class T> Int_t HTofParAsciiFileIo::writeFile3(T* pPar) {
  // template function for all parameter containers with 3 levels
  // writes the header, loops over the container and calls its member
  //   function writeline(...)
  pPar->putAsciiHeader(fHeader);
  writeHeader(pPar->GetName());
  Text_t buf[155];
  Int_t n0 = (*pPar).getSize();
  for(Int_t i0=0; i0<n0; i0++) {
    Int_t n1 = (*pPar)[i0].getSize();
    for(Int_t i1=0; i1<n1; i1++) {
      Int_t n2 = (*pPar)[i0][i1].getSize();
      for(Int_t i2=0; i2<n2; i2++) {
        if (pPar->writeline(buf, i0, i1, i2))
          pFile->write(buf,strlen(buf));
      }
    }
  }
  pPar->setChanged(kFALSE);
  pFile->write(sepLine,strlen(sepLine));
  return 1;
}
