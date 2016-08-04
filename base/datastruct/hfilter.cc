#include "hfilter.h"

//*-- Author : Manuel Sanchez
//*-- Modified : 24/03/98 by Manuel Sanchez
//*-- Copyright : GENP (Univ. Santiago de Compostela)

//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////
//HFilter
// 
//  ABC defining the basic interface of filters.
//  
//  A filter is a objects which is able to take one YObject and tell
//if it pass or not that particular filter.
//
//  The most important method of this class is 
//HFilter::check(TObject *dataObject), if it returns kTRUE "dataObject"
//passes the filter and if the function returns kFALSE, then "dataObject"
//doesn't pass the filter.
////////////////////////////////

ClassImp(HFilter)

HFilter::HFilter(void) {
}

HFilter::~HFilter(void) {
}
