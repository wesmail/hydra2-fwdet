#pragma interface
#ifndef HINDEXTABLE_H
#define HINDEXTABLE_H

#include "TObject.h"
#include "TArrayI.h"
#include "hlocation.h"
#include "TBuffer.h"

class HPairListI : public TObject {
 private:
  Int_t fCapacity;
  Int_t fN;
  UInt_t *fArray[2];
 public:
  HPairListI(void) {fCapacity=0; fN=0; fArray[0]=fArray[1]=0;}
  ~HPairListI(void) { if (fCapacity>0) {delete[] fArray[0]; delete[] fArray[1];} }
  void setCapacity(Int_t n);
  void add(UInt_t i1,UInt_t i2) {
  #if DEBUG_LEVEL==1
      if (fN<fCapacity) {
	  fArray[0][fN]=i1; fArray[1][fN]=i2;
	  fN++;
      }
  #else
      fArray[0][fN]=i1; fArray[1][fN]=i2;
      fN++;
  #endif
  }
  void set(Int_t index, UInt_t i1,UInt_t i2) {
    fArray[0][index]=i1; fArray[1][index]=i2;
  }
  void remove(Int_t idx);
  Int_t getN(void) {return fN;}
  UInt_t &getIndex1(Int_t idx) {return fArray[0][idx]; }
  UInt_t &getIndex2(Int_t idx) {return fArray[1][idx]; }
  void clear(Option_t *opt="") {fN=0;} // The default value of a pair is not
                                       // granted to be 0
  ClassDef(HPairListI,2)
};

class HIndexTable : public TObject {
protected:
  TArrayI fSizes; //Sizes of the dimensions of the index matrix
  HPairListI fCompactTable; //Table of linear addres -- index pairs
  TArrayI fIndexArray; //!Index matrix
  Int_t fCurrentPos; //!Current position in the table for iterations.
public:
  HIndexTable(void);
  ~HIndexTable(void);
  void setDimensions(Int_t nDim,Int_t *sizes);
  Int_t getIndex(HLocation &aLoc) { return (fIndexArray[aLoc.getLinearIndex(&fSizes)]); }
  void setIndex(HLocation &aLoc,Int_t idx) { 
    Int_t la=aLoc.getLinearIndex(&fSizes);
    fCompactTable.add(la,idx);
    fIndexArray[la]=idx; 
  }
  TArrayI *getDimensions(void) {return &fSizes;}
  inline Int_t getIndex(UInt_t linAddr) {
    return ((Int_t)linAddr >= fIndexArray.fN)?-1:fIndexArray.fArray[linAddr];
  }
  void setIndex(Int_t linAddr,Int_t idx) { 
    fIndexArray[linAddr]=idx; 
    fCompactTable.add(linAddr,idx); 
  }
  Int_t getEntries(void) { return fIndexArray.fN; }
  Bool_t checkLocation(HLocation &aLoc);
  Int_t gotoLocation(HLocation &aLoc);
  Int_t gotoBegin(void);
  Int_t next(void);
  void Clear(Option_t *opt="");
  HPairListI* getCompactTable(void) {return &fCompactTable;}
  ClassDef(HIndexTable,2) //Utility class handling an index table.
};

#endif /* !HINDEXTABLE_H */

