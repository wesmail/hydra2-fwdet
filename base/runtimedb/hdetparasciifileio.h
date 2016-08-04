#ifndef HDETPARASCIIFILEIO_H
#define HDETPARASCIIFILEIO_H
using namespace std;

#include <fstream>
#include "hdetpario.h"
#include "TArrayI.h"
#include "hgeomtransform.h"

class HDetGeomPar;
class HGeomVolume;
class HGeomShapes;
class HDetector;
class HTrb3Calpar;

class HDetParAsciiFileIo : public HDetParIo {
protected:
   TString fHeader;  //! header of container output in file
   TString sepLine;  //! comment line
   fstream* pFile;   //! pointer to ascii file
   virtual Bool_t write(HDetector*) {
      return kTRUE;
   }
   Bool_t findContainer(const Text_t* name);
   Bool_t checkAllFound(Int_t*, Int_t);
   void writeHeader(const Text_t*, const Text_t* context = "",
                    const Text_t* author = "", const Text_t* description = "");
   void writeComment(HParSet*);
   void readComment(const Char_t*, HParSet*);
   Bool_t readLabPositions(const Text_t*, HDetGeomPar*, Int_t*, Int_t, Int_t);
   Bool_t readVolumes(const Text_t*, HDetGeomPar*);
   void readTransform(HGeomTransform&);
   Bool_t readVolume(HGeomVolume*, HGeomShapes*, Text_t*);
   void writeTransform(const HGeomTransform&);
   void writeVolume(HGeomVolume*, HGeomShapes*);
public:
   HDetParAsciiFileIo(fstream* f);
   virtual ~HDetParAsciiFileIo() {}
   Bool_t read(HDetGeomPar*, Int_t*);
   Bool_t read(HTrb3Calpar*);
   Int_t writeFile(HDetGeomPar*);

   template<class T> Bool_t readFile(T* pPar);
   template<class T> Int_t writeFile(T* pPar);

   ClassDef(HDetParAsciiFileIo, 0) // Class for detector parameter I/O from ascii file
};

template<class T> Bool_t
HDetParAsciiFileIo::readFile(T* pPar)
{
   // template function for parameter containers
   // searches the container in the file, reads the data line by line and
   // called the member function readline(...) of the container class

   if (kFALSE == findContainer(pPar->GetName())) {
      return kFALSE;
   }

   pPar->clear();
   const Int_t maxbuf = 155;
   Text_t buf[maxbuf];
   Bool_t rc = kTRUE;
   while (!pFile->eof() && rc) {
      pFile->getline(buf, maxbuf);
      if (buf[0] == '#') {
         break;
      }
      if (buf[0] != '/' && buf[0] != '\0') {
         rc = pPar->readline(buf);
      }
   }
   if (kTRUE == rc) {
      pPar->setInputVersion(1, inputNumber);
      pPar->setChanged();
      Info("readFile", "%s initialized from Ascii file", pPar->GetName());
   }
   return rc;
}

template<class T> Int_t
HDetParAsciiFileIo::writeFile(T* pPar)
{
   // template function for parameter containers
   // calls the function putAsciiHeader(TString&) of the parameter container,
   // writes the header and calls the function write(fstream&) of the class

   if (NULL != pFile) {
      pPar->putAsciiHeader(fHeader);
      writeHeader(pPar->GetName(), pPar->getParamContext());
      pPar->write(*pFile);
      pFile->write(sepLine, strlen(sepLine));
      pPar->setChanged(kFALSE);
      return 1;
   }
   Error("writeFile", "Output is not writable");
   return -1;
}

#endif  /* !HDETPARASCIIFILEIO_H */
