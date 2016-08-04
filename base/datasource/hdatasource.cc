#include "hdatasource.h"
#include "hevent.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 9/10/1998 by Manuel Sanchez
//*-- Modified : 27/05/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////
//HDataSource
// 
//  This is an abstract base class. Its derived classes provide the data needed 
// for the reconstruction, reading them from different sources.
//
//  The main method of this class is getNextEvent() which is responsible of 
// reading data from the source (file...) and put them into the event pointed 
// by fEventAddr
//
//  The return value is:
//      kDsOk --------> no error.
//      kDsEndFile ---> file's end.
//      kDsEndData ---> data's end.
//      kDsError -----> error.
//
//  Another important method is init(). In this method the data source must
// setup all what it needs to run (get pointers to the target categories, and 
// if the returned pointer is NULL, create those categories ...).
//
//
//  Within the whole framework, the data sources (classes inherited from
// HDataSource) are the responsible for reading the event's data before its
// proccessing.
////////////////////////////////

ClassImp(HDataSource)

void HDataSource::setEventAddress(HEvent **ev) {
  // Used to give the data source the address of the event it must fill in
  // Input:
  //  ev --> Address of a pointer to the event that will be filled in by the 
  //         data source
   fEventAddr=ev;
}

EDsState HDataSource::skipEvents(Int_t n) {
  enum EDsState state = kDsOk;
  for (Int_t i=0; i<n; i++) {
    (*fEventAddr)->Clear();
    state = getNextEvent(kFALSE);
    if (state == kDsEndData || state == kDsError) break;
  }
  return state;
}






