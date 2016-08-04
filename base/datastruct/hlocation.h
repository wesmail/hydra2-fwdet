#ifndef HLOCATION_H
#define HLOCATION_H

#include "TObject.h"
#include "TArrayI.h"
#include "haddef.h"

class HLocation : public TObject {
protected:
  TArrayI fIndexes; //High indexes indicating the location
  Int_t fOffset;    //Less significant index of the location
public:
  HLocation(void) {fOffset=0;}
  HLocation(HLocation &aLoc) : TObject(aLoc) {  fIndexes.Copy(aLoc.fIndexes); }
  ~HLocation(void) {   fIndexes.Reset(); }
  void set(Int_t nInd, Int_t i1=0, Int_t i2=0, Int_t i3=0, Int_t i4=0, Int_t i5=0);
  void setNIndex(Int_t nIdx) {   fIndexes.Set(nIdx); }
  void setOffset(Int_t aOfs) {  fIndexes[fIndexes.fN-1]=aOfs; }
  Int_t getIndex(Int_t aIdx) {  return(fIndexes[aIdx]); }
  Int_t getUncheckedIndex(Int_t aIdx) {return fIndexes.fArray[aIdx];}
  void setIndex(Int_t aIdx,Int_t aValue) {  fIndexes[aIdx]=aValue; }
  Int_t getLinearIndex(TArrayI *sizes);
  Int_t getLinearIndex(Int_t *sizes,Int_t n);
  Int_t getNIndex(void) { return fIndexes.fN; }
  TArrayI* getIndexes(void) { return &fIndexes;}
  Int_t getOffset(void) {  return fIndexes[fIndexes.fN-1]; }
  void incIndex(Int_t nIndex);
  void operator+=(Int_t shift) {  fIndexes[fIndexes.fN-1]+=shift; } 
  Int_t operator++(void) {   return fIndexes[fIndexes.fN-1]++; }
  void operator-=(Int_t shift) {  fIndexes[fIndexes.fN-1]-=shift; } 
  void operator--(void) {   fIndexes[fIndexes.fN-1]--; }
  Int_t &operator[](Int_t aIdx) {return fIndexes.fArray[aIdx];}
  HLocation &operator=(HLocation &loc) {  loc.fIndexes.Copy(fIndexes); return loc; }
  void readIndexes(HLocation &loc);
  void Dump(void);
  ClassDef(HLocation,1) //Indicator for a place in the event
};

#endif /* !HLOCATION_H */
