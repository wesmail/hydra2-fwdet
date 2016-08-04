#pragma implementation
//*-- Author : Manuel Sanchez Garcia
//*-- Modified : 27/05/98
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
/////////////////////////////////////////////////////////
//HIndexTable
//
//  This class handles an index table with an entry per 
//  possible HLocation. In such a way that, given a HLocation,it returns
//  an index.
//
//  You can also iterate on the index table (on the locations) using the
// functions gotoBegin(),gotoLocation() and next().
//
//  You can also access the indexes as if they were lineally distributed,
//
//////////////////////////////////////////////////////////
using namespace std;
#include "hindextable.h"
#include "hlocation.h"
#include "hdebug.h"
#include "Rtypes.h"
#include <iostream> 
#include <iomanip>

ClassImp(HIndexTable)

HIndexTable::HIndexTable(void) {
  //Default constructor
}

HIndexTable::~HIndexTable(void) {
  //Destructor
}

void HIndexTable::setDimensions(Int_t nDim,Int_t *sizes) {
  //Sets the number of dimensions of the table and its corresponding
  //sizes.
  //
  //Input:
  // nDim ---> number of indexes to define an entry
  // sizes --> max values of those indexes.
#if DEBUG_LEVEL>2
  gDebuger->enterFunc("HIndexTable::setDimensions");
  gDebuger->message("nDim= %i",nDim);
#endif
  Int_t prod=1,i=0;
  fSizes.Set(nDim,sizes);
  for (i=0;i<nDim;i++) { prod*=sizes[i];}
#if DEBUG_LEVEL>2
  gDebuger->message("Setting index array");
#endif
  fCompactTable.setCapacity(prod);
  fIndexArray.Set(prod);
  for (Int_t i=0;i<prod;i++) fIndexArray.fArray[i]=-1;
#if DEBUG_LEVEL > 2
  gDebuger->leaveFunc("HIndexTable::setDimensions");
#endif
}


void HIndexTable::Clear(Option_t *) {
  //Resets all the indexes in the table.
  //Int_t i;
 // for (i=0;i<fIndexArray.fN;i++) fIndexArray.fArray[i]=-1;
  register Int_t i=0;
  for (i=0;i<fCompactTable.getN();i++) {
    fIndexArray.fArray[fCompactTable.getIndex1(i)]=-1;
  }
  fCompactTable.clear();
}


Bool_t HIndexTable::checkLocation(HLocation &aLoc) {
  //Checks if there is an entry in the index table for this location
  Int_t i;
  for (i=0;i<fSizes.fN;i++) {
    if (aLoc[i]>=fSizes.fArray[i]) return kFALSE;
  }
  return kTRUE;
}

Int_t HIndexTable::gotoLocation(HLocation &aLoc) {
  //This function along with HIndexTable::next allows iteration on the index
  //table through all the indexes corresponding to the location aLoc
  //
  //gotoLocation positions a cursor at the index given by aLoc. It should be called
  //before any call to HIndexTable::next()
  //
  //Return value:
  //  The number of positions corresponding to the given location.
  Int_t n=1,i=0;
  if (aLoc.getNIndex()==fSizes.fN) 
    n=1;
  else {
    n=1;
    for(i=aLoc.getNIndex();i<fSizes.fN;i++) 
      n*=fSizes[i];
  }
  fCurrentPos=aLoc.getLinearIndex(&fSizes);
  return n;
}

Int_t HIndexTable::gotoBegin(void) {
  //This function positions the cursor for iterating on the HIndexTable at its
  // very beggining.
  fCurrentPos=0;
  return fIndexArray.fN;
}

Int_t HIndexTable::next(void) {
  //This function along with HIndexTable::gotoLocation allows iteration on the index
  //table.
  //
  //next advances to the next location by incrementing a cursor. It returns
  //the index associated to the current location (the location before advancing)
  Int_t idx;
  idx=fIndexArray[fCurrentPos];
  if (fCurrentPos<fIndexArray.fN-1) fCurrentPos++;
  return idx;
}

void HIndexTable::Streamer(TBuffer &R__b)
{
   // Stream an object of class HIndexTable.
   if (R__b.IsReading()) {
     Int_t prod=1;
     Version_t R__v = R__b.ReadVersion(); if (R__v) { }
     TObject::Streamer(R__b);
     fSizes.Streamer(R__b);
     if (R__v==1) {
       fIndexArray.Streamer(R__b);
       for (Int_t i=0;i<fSizes.fN;i++) { prod*=fSizes.fArray[i];}
       fCompactTable.setCapacity(prod);
       fCompactTable.clear();
       for (Int_t i=0;i<fIndexArray.fN;i++) {
	 if (fIndexArray.fArray[i]!=-1) {
	   fCompactTable.add(i,fIndexArray.fArray[i]);
	 }
       }
     } else if (R__v==2) {
       for (Int_t i=0;i<fSizes.fN;i++) { prod*=fSizes.fArray[i];}
       fIndexArray.Set(prod);
       for (Int_t i=0;i<prod;i++) fIndexArray.fArray[i]=-1;
       fCompactTable.Streamer(R__b);
       
       for (Int_t i=0;i<fCompactTable.getN();i++) {
	 fIndexArray.fArray[fCompactTable.getIndex1(i)]=
	   (fCompactTable.getIndex2(i) == kMaxUInt)?-1:fCompactTable.getIndex2(i);
       }
     } else {
       Error("Streamer","Object version not known");
     }
   } else {
      R__b.WriteVersion(HIndexTable::IsA());
      TObject::Streamer(R__b);
      fSizes.Streamer(R__b);
      fCompactTable.Streamer(R__b);
   }
}

/********************** HPairListI ********************************/
ClassImp(HPairListI)

void HPairListI::setCapacity(Int_t n) {
  if (fCapacity!=n) {
    fCapacity=n;
    delete[] fArray[0];
    delete[] fArray[1];
    if (fCapacity>0) {
      fArray[0]=new UInt_t[n];
      fArray[1]=new UInt_t[n];
    }
  }
  clear();
}

void HPairListI::Streamer(TBuffer & b) {

  if (b.IsReading()) {
    Int_t cap;
    b >> cap;
    setCapacity(cap);
    b >> fN;
    if (fN>0) {
      b.ReadFastArray(fArray[0],fN);
      b.ReadFastArray(fArray[1],fN);
    }
  } else {
    b << fCapacity;
    b << fN;
    if (fN>0) {
      b.WriteFastArray(fArray[0],fN);
      b.WriteFastArray(fArray[1],fN);
    }
  }
}

void HPairListI::remove(Int_t idx) {
  //warning "HPairListI::remove not yet implemented"
}
