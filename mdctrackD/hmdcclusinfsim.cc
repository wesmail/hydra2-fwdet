//*-- AUTHOR : Vladimir Pechenov
//*-- Modified : 06/04/2003 by V.Pechenov


//_HADES_CLASS_DESCRIPTION 
////////////////////////////////////////////////////////////////
// HMdcClusInfSim
//
// Data object keep information about sim. tracks in chamber cluster.
//
////////////////////////////////////////////////////////////////
using namespace std;
#include "hmdcclusinfsim.h"
#include <iostream> 
#include <iomanip>

ClassImp(HMdcClusInfSim)

void HMdcClusInfSim::setTracksList(Int_t nTr, const Int_t* list, 
    const Short_t* nTm) {
  if(nTr<=0) {
    nTracks=0;
    return;
  }
  nTracks=(nTr<=5) ? nTr:5;
  for(Int_t i=0;i<nTracks;i++) {
    listTr[i]=list[i];
    nTimes[i]=nTm[i];
  }
}

void HMdcClusInfSim::print(void) const {
  HMdcClusInf::print();
  printf("   %i tracks in chamber cluster:",nTracks);
  for(Int_t n=0; n<nTracks; n++) printf(" %i(%i)",listTr[n],nTimes[n]);
  printf("\n");
}
