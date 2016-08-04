//*-- AUTHOR :  Pablo Cabanelas
//*-- Created : 17/10/2005
//*-- Modified: 08/09/2006 by D. Gonzalez-Diaz
//*-- Modified: 12/09/2006
//*-- Modified: 26/09/2006
//*-- Modified: 18/08/2007 by P. Cabanelas
                                                                                              
//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////////
// HRpcParAsciiFileIo
//
// Class for RPC parameter input/output from/into Ascii file
//
/////////////////////////////////////////////////////////////
                                                                                              
#include "hrpcparasciifileio.h"
#include "hrpcdetector.h"
#include "hrpcdigipar.h"
#include "hrpcgeomcellpar.h"
#include "hades.h"
#include "hspectrometer.h"
#include "hruntimedb.h"
#include "hrpccalpar.h"
#include "hrpctrb2lookup.h"
                                                                                              
ClassImp(HRpcParAsciiFileIo)
                                                                                              
  HRpcParAsciiFileIo::HRpcParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f) {
  // constructor calls the base class constructor
  fName="HRpcParIo";
  HRpcDetector* det=(HRpcDetector*)gHades->getSetup()->getDetector("Rpc");  
  setSize=det->getMaxSectors(); 
}
                                                                                              
Bool_t HRpcParAsciiFileIo::init(HParSet* pPar,Int_t* set) {
  // calls the appropriate read function for the container
  const  Text_t* name=pPar->GetName();
  if (pFile) {
    pFile->clear();
    pFile->seekg(0,ios::beg);
    if (!strcmp(name,"RpcCalPar")) return read((HRpcCalPar*)pPar,set);
    if (!strcmp(name,"RpcTrb2Lookup")) return read((HRpcTrb2Lookup*)pPar);
    if (!strcmp(name,"RpcGeomPar"))
       return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
    cerr<<"initialization of "<<name<<" not possible from Ascii file!"<<endl;
    return kFALSE;
  }
  cerr<<"no input file open"<<endl;
  return kFALSE;
}
                                                                                              
Int_t HRpcParAsciiFileIo::write(HParSet* pPar) {
  // calls the appropriate write function for the container
  if (pFile) {
    const  Text_t* name=pPar->GetName();
    if (!strcmp(name,"RpcCalPar")) return writeFile3((HRpcCalPar*)pPar);
    if (!strcmp(name,"RpcTrb2Lookup")) return write((HRpcTrb2Lookup*)pPar);
    if (!strcmp(name,"RpcGeomPar"))
        return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
    cerr<<name<<" could not be written to Ascii file"<<endl;
    return -1;
  }
  cerr<<"no output file open"<<endl;
  return -1;
}

template<class T> Int_t HRpcParAsciiFileIo::write(T* pPar) {
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

template<class T> Int_t HRpcParAsciiFileIo::writeFile3(T* pPar) {
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

template<class T> Bool_t HRpcParAsciiFileIo::read(T* pPar) {
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
                                                                                              
template<class T> Bool_t HRpcParAsciiFileIo::read(T* pPar, Int_t* set,
                                                  Bool_t needsClear) {
  // template function for all parameter containers
  // searches the container in the file, reads the data line by line and
  // called the member function readline(...) of the container class
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
                                                                                              


