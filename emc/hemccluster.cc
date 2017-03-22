//*-- Author  : v. Pechenov
//*-- Created : 
//*-- Modified: 

//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////
//
//  HEmcCluster
//
//  Class for the Calibrated EMC data
//
////////////////////////////////////////////////////


#include "hemccluster.h"

ClassImp(HEmcCluster)

void HEmcCluster::setCellList(Int_t ncs,UChar_t* list) {
  cell = list[0];
  ncells = ncs;
  ncs--;
  if(ncs > 0) {
    if(ncs>24) ncs = 24;
    memcpy(cellList, list+1, ncs*sizeof(UChar_t)); //???
  }
}

UChar_t HEmcCluster::getCell(Int_t ind) const {
  if(ind<0 || ind > ncells || ind>24) return -1;
  if(ind == 0) return cell;
  return cellList[ind-1];
}
