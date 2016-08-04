using namespace std;
#include "hlocation.h"
#include <iostream> 
#include <iomanip>

//*-- Author : Manuel Sanchez 
//*-- Modified : 20/08/99 by R. Holzmann
//*-- Modified : 27/05/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////
//HLocation
//
//  ATENCION! Se ha eliminado el index check en operator[]
// 
//  A location is a set of indexes defining the location of a particular 
//data object within the HEvent structure; so within a loop is recommended to 
//directly use one HLocation instead of a set of indexes (i,j,k,l...) (and
//better an HIterator instead of HLocation).
//
//  An HEvent holds the data corresponding to an event, those data are arranged
// in categories (mdc raw data, mdc cal data, rich rings,...) which are objects
// instantianting one HCategory, to access one of these categories the user
// can call HEvent::getCategory(). Within the categories, each data object has
// a location (given by an object instantiating HLocation) which identifies 
// that object in the category (in some sense, the location is a generalized
// index).
//
////////////////////////////////

ClassImp(HLocation) 

Int_t HLocation::getLinearIndex(Int_t *sizes,Int_t n) {
  Int_t r=1,i;
  r=fIndexes.fArray[0];
  for (i=1;i<fIndexes.fN;i++) 
    r=r*(sizes[i])+fIndexes.fArray[i];
  for(;i<n;i++) {
    r*=sizes[i];
  }
  return (r);
}





Int_t HLocation::getLinearIndex(TArrayI *sizes) {
  Int_t r=1,i;
  r=fIndexes.fArray[0];
  for (i=1;i<fIndexes.fN;i++) 
    r=r*(sizes->fArray[i])+fIndexes.fArray[i];
  for(;i<sizes->fN;i++) {
    r*=sizes->fArray[i];
  }
  return (r);
}

void  HLocation::set(Int_t nIndex, Int_t i1, Int_t i2, Int_t i3, Int_t i4, Int_t i5) {  // this handles up to 5 indexes, but can easily be extended
   fIndexes.Set(nIndex);
   switch(nIndex) {
   case 5:
     fIndexes[4] = i5;
   case 4:
     fIndexes[3] = i4;
   case 3:
     fIndexes[2] = i3;
   case 2:
     fIndexes[1] = i2;
   case 1:
     fIndexes[0] = i1;
   default: ;
   }
}


void HLocation::incIndex(Int_t nIndex) {
  //Increments the index nIndex and sets all the following indexes to 0
  Int_t i;
  fIndexes[nIndex]++;
  for (i=nIndex+1;i<fIndexes.fN;i++) fIndexes[i]=0;
}


void HLocation::readIndexes(HLocation &loc) {
  //Sets the loc.getNIndex() first indexes to the value given in loc
  //and the others are set to 0
  Int_t n=0,i=0;
  Int_t nInd=loc.getNIndex();
  n=((fIndexes.fN>nInd)? nInd : fIndexes.fN);
  for (i=0;i<n;i++) {
    fIndexes.fArray[i]=loc.fIndexes.fArray[i];
  }
  for (;i<fIndexes.fN;i++) fIndexes[i]=0;
}

void HLocation::Dump(void) {
  //Dumps the object to cout in the format:
  //
  //index1:index2:....:offset
  Int_t i;
  
  cout << "fIndexes.fN|";
  for (i=0;i<fIndexes.fN;i++) {
    if (i==fIndexes.fN-1) cout << fIndexes[i] << endl;
    else cout << fIndexes[i] << ":" ;
  }
} 



