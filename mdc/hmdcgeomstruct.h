#ifndef HMDCGEOMSTRUCT_H
#define HMDCGEOMSTRUCT_H

#include "TObject.h"
#include "TObjArray.h"
#include "TArrayI.h"
#include "hparset.h"

class HParIo;

class HMdcGeomModStru: public TObject {
protected:
  Int_t nCells[6];  // number of cells in the layers
public:
  HMdcGeomModStru() {}
  ~HMdcGeomModStru() {}
  Int_t operator [](Int_t i) {return nCells[i];}
  void fill(Int_t i, Int_t n) {if (i<6) nCells[i]=n;}
  void clear() {for(Int_t i=0;i<6;i++) nCells[i] = 0;}
  Int_t getSize() {return 6;}
  ClassDef(HMdcGeomModStru,2) // Layer level of the MdcGeomStruct container
};


class HMdcGeomSecStru: public TObject {
protected:
  TObjArray* array;  // pointer array containing HMdcGeomModStru objects
public:
  HMdcGeomSecStru(Int_t n=4);
  ~HMdcGeomSecStru();
  HMdcGeomModStru& operator[](Int_t i) {
    return *static_cast<HMdcGeomModStru*>((*array)[i]);
  }
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcGeomSecStru,2) // Sector level of the MdcGeomStruct container
};


class HMdcGeomStruct : public HParSet {
protected:
  TObjArray* array;     // pointer array containing HMdcGeomSecStru objects
public:
  HMdcGeomStruct(const Char_t* name="MdcGeomStruct",
                 const Char_t* title="Mdc parameters for geometry structure",
                 const Char_t* context="",
                 Int_t n=6);
  ~HMdcGeomStruct();
  HMdcGeomSecStru& operator[](Int_t i) {
    return *static_cast<HMdcGeomSecStru*>((*array)[i]);
  }
  Bool_t init(HParIo* input,Int_t* set);
  Int_t  write(HParIo* output);
  void putAsciiHeader(TString&);
  void readline(const Char_t*, Int_t*);
  Bool_t writeline(Char_t *, Int_t, Int_t, Int_t);
  void clear();
  void printParam();
  void getMaxIndices(TArrayI*);
  Int_t getSize() {return array->GetEntries();}
  ClassDef(HMdcGeomStruct,2) // MdcGeomStruct container for number of wires per layer
};

#endif /* !HMDCGEOMSTRUCT_H */



