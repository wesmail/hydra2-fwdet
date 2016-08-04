//*-- modified : 06/10/2000 by I.Koenig

//_HADES_CLASS_DESCRIPTION
///////////////////////////////////////////////////////////////
// HStartParAsciiFileIo
//
// Class for Start parameter input/output from/into Ascii file
//
///////////////////////////////////////////////////////////////

#include "hades.h"
#include "hdetector.h"
#include "hspectrometer.h"
#include "hstart2calpar.h"
#include "hstart2detector.h"
#include "hstart2trb2lookup.h"
#include "hstart2calrunpar.h"
#include "hstart2geompar.h"
#include "hstart2trb3lookup.h"
#include "hstart2trb3calpar.h"
#include "hstartparasciifileio.h"

ClassImp(HStartParAsciiFileIo)

HStartParAsciiFileIo::HStartParAsciiFileIo(fstream* f) : HDetParAsciiFileIo(f)
{
   // constructor calls the base class constructor
   fName = "HStartParIo";
}

Bool_t HStartParAsciiFileIo::init(HParSet* pPar, Int_t* set)
{
   // calls the appropriate read function for the container
   const  Text_t* name = pPar->GetName();
   if (pFile) {
      if (!strcmp(name, "Start2Trb2Lookup")) return read((HStart2Trb2Lookup*)pPar);
      if (!strcmp(name, "Start2Calpar"))     return read((HStart2Calpar*)pPar, set, kTRUE);
      if (!strcmp(name, "Start2GeomPar"))
            return HDetParAsciiFileIo::read((HDetGeomPar*)pPar,set);
      if (!strcmp(name, "Start2Trb3Lookup")) return read((HStart2Trb3Lookup*)pPar);
      if (!strcmp(name, "Start2Trb3Calpar"))
            return HDetParAsciiFileIo::read((HTrb3Calpar*)pPar);
       Error("init(HParSet*,Int_t*)",
            "initialization of %s not possible from  ASCII file!", name);
      return kFALSE;
   }
   Error("init(HParSet*,Int_t*)", "No input file open");
   return kFALSE;
}

Int_t HStartParAsciiFileIo::write(HParSet* pPar)
{
   // calls the appropriate write function for the container
   if (pFile) {
      const  Text_t* name = pPar->GetName();
      if (!strcmp(name, "Start2Trb2Lookup")) return write((HStart2Trb2Lookup*)pPar);
      if (!strcmp(name, "Start2Calpar"))     return writeFile2((HStart2Calpar*)pPar);
      if (!strcmp(name, "Start2CalRunPar"))  return write((HStart2CalRunPar*)pPar);
      if (!strcmp(name, "Start2GeomPar"))
            return HDetParAsciiFileIo::writeFile((HDetGeomPar*)pPar);
      if (!strcmp(name, "Start2Trb3Lookup")) return write((HStart2Trb3Lookup*)pPar);
      if (!strcmp(name, "Start2Trb3Calpar"))
            return HDetParAsciiFileIo::writeFile((HTrb3Calpar*)pPar);
       Error("write(HParSet*)", "%s could not be written to ASCII file", name);
      return -1;
   }
   Error("write(HParSet*)", "No output file open");
   return -1;
}

template<class T> Bool_t HStartParAsciiFileIo::read(T* pPar)
{
   // template function for parameter containers
   // searches the container in the file, reads the data line by line and
   // called the member function readline(...) of the container class
   // does not check the detector setup
   if (!findContainer(pPar->GetName())) return kFALSE;
   pPar->clear();
   const Int_t maxbuf = 155;
   Text_t buf[maxbuf];
   Bool_t rc = kTRUE;
   while (!pFile->eof() && rc) {
      pFile->getline(buf, maxbuf);
      if (buf[0] == '#') break;
      if (buf[0] != '/' && buf[0] != '\0') rc = pPar->readline(buf);
   }
   if (rc) {
      pPar->setInputVersion(1, inputNumber);
      pPar->setChanged();
      printf("%s initialized from Ascii file\n", pPar->GetName());
   }
   return rc;
}

template<class T> Bool_t HStartParAsciiFileIo::read(T* pPar, Int_t* set,
                                                    Bool_t needsClear)
{
   // template function for all parameter containers
   // searches the container in the file, reads the data line by line and
   //   called the member function readline(...) of the container class
   const Text_t* name = pPar->GetName();
   HDetector* det = gHades->getSetup()->getDetector("Start");
   Int_t nSize = det->getMaxModules();
   if (!findContainer(name)) return kFALSE;
   if (needsClear) pPar->clear();
   const Int_t maxbuf = 155;
   Text_t buf[maxbuf];
   while (!pFile->eof()) {
      pFile->getline(buf, maxbuf);
      if (buf[0] == '#') break;
      if (buf[0] != '/' && buf[0] != '\0') pPar->readline(buf, set);
   }
   pPar->setInputVersion(1, inputNumber);
   pPar->setChanged();
   Bool_t allFound = kTRUE;
   if (!needsClear) {
      printf("%s: module(s) initialized from Ascii file:\n", name);
      allFound = checkAllFound(set, nSize);
   } else {
      for (Int_t i = 0; i < nSize; i++) {
         if (set[i]) {
            if (set[i] == 999) set[i] = 1;
            else allFound = kFALSE;
         }
      }
      if (allFound) printf("%s initialized from Ascii file\n", name);
   }
   return allFound;
}

template<class T> Bool_t HStartParAsciiFileIo::readStartModule(T* pPar, Int_t* set,
      Bool_t needsClear)
{
   // template function for start module parameter containers
   // searches the container in the file, reads the data line by line and
   //   called the member function readline(...) of the container class
   const  Text_t* name = pPar->GetName();
   if (!findContainer(name)) return kFALSE;
   if (needsClear) pPar->clear();
   const Int_t maxbuf = 155;
   Text_t buf[maxbuf];
   while (!pFile->eof()) {
      pFile->getline(buf, maxbuf);
      if (buf[0] == '#') break;
      if (buf[0] != '/' && buf[0] != '\0') pPar->readline(buf, set);
   }
   pPar->setInputVersion(1, inputNumber);
   pPar->setChanged();
   Bool_t allFound = kTRUE;
   if (!needsClear) {
      printf("%s: module(s) initialized from Ascii file:\n", name);
      allFound = checkAllFound(set, 1);
   } else {
      for (Int_t i = 0; i < 1; i++) {
         if (set[i]) {
            if (set[i] == 999) set[i] = 1;
            else allFound = kFALSE;
         }
      }
      if (allFound) printf("%s initialized from Ascii file\n", name);
   }
   return allFound;
}

template<class T> Int_t HStartParAsciiFileIo::write(T* pPar)
{
   // template function for parameter containers
   // calls the function putAsciiHeader(TString&) of the parameter container,
   // writes the header and calls the function write(fstream&) of the class
   pPar->putAsciiHeader(fHeader);
   writeHeader(pPar->GetName(), pPar->getParamContext());
   pPar->write(*pFile);
   pFile->write(sepLine, strlen(sepLine));
   pPar->setChanged(kFALSE);
   return 1;
}

template<class T> Int_t HStartParAsciiFileIo::writeFile2(T* pPar)
{
   // template function for all parameter containers with 2 levels
   // writes the header, loops over the container and calls its member
   //   function writeline(...)
   pPar->putAsciiHeader(fHeader);
   writeHeader(pPar->GetName());
   Text_t buf[155];
   Int_t n0 = (*pPar).getSize();
   for (Int_t i0 = 0; i0 < n0; i0++) {
      Int_t n1 = (*pPar)[i0].getSize();
      for (Int_t i1 = 0; i1 < n1; i1++) {
         if (pPar->writeline(buf, i0, i1)) pFile->write(buf, strlen(buf));
      }
   }
   pPar->setChanged(kFALSE);
   pFile->write(sepLine, strlen(sepLine));
   return 1;
}
