//_HADES_CLASS_DESCRIPTION
/////////////////////////////////////////////////////////
//
// HShowerHitFPar 
// It contains parameters for hit finding process
//
/////////////////////////////////////////////////////////

#include "hshowerhitfpar.h"
#include "hparamlist.h"

ClassImp(HShowerHitFPar)

HShowerHitFPar::HShowerHitFPar(const Char_t* name,const Char_t* title,
                               const Char_t* context)
               : HParCond(name,title,context) {

  nSectors = 6;
  nModules = 3;
  defaultInit();
}

void HShowerHitFPar::defaultInit() {
  nThreshold = 0;
  pColBorder.Set(nSectors * nModules * 2);
  pRowBorder.Set(nSectors * nModules * 2);
  Int_t nLower = 0;
  Int_t nUpper = 31;
  for(Int_t i = 0; i < nSectors; i++) {
    for(Int_t j = 0; j < nModules; j++) {
       setColBord(i, j, nLower, nUpper);
       setRowBord(i, j, nLower, nUpper);
    }
  }
  fGainPost1 = 1.9;
  fThresholdPost1 = 0.0;
  fGainPost2 = 1.9;
  fThresholdPost2 = 0.0;
}

void HShowerHitFPar::clear() {
  nThreshold=0;
  fGainPost1=0.F;
  fThresholdPost1=0.F;
  fGainPost2=0.F;
  fThresholdPost2=0.F;
  pRowBorder.Reset();
  pColBorder.Reset();
}

void HShowerHitFPar::putParams(HParamList* l) {
  if (!l) return;
  l->add("pRowBorder",     pRowBorder);
  l->add("pColBorder",     pColBorder);
  l->add("nThreshold",     nThreshold);
  l->add("fGainPost1",     fGainPost1);
  l->add("fThresholdPost1",fThresholdPost1);
  l->add("fGainPost2",     fGainPost2);
  l->add("fThresholdPost2",fThresholdPost2);
}

Bool_t HShowerHitFPar::getParams(HParamList* l) {
  if (!l) return kFALSE;
  if (!l->fill("pRowBorder",     &pRowBorder))      return kFALSE;
  if (!l->fill("pColBorder",     &pColBorder))      return kFALSE;
  if (!l->fill("nThreshold",     &nThreshold))      return kFALSE;
  if (!l->fill("fGainPost1",     &fGainPost1))      return kFALSE;
  if (!l->fill("fThresholdPost1",&fThresholdPost1)) return kFALSE;
  if (!l->fill("fGainPost2",     &fGainPost2))      return kFALSE;
  if (!l->fill("fThresholdPost2",&fThresholdPost2)) return kFALSE;
#if DEBUG_LEVEL > 0
  printParams();
#endif
  return kTRUE;
}

Int_t HShowerHitFPar::getIndex(Int_t nSect, Int_t nMod) {
  if ((nSect<0) || (nSect>=nSectors) || (nMod<0) || (nMod>=nModules)) return -1;
  return (nSect * nMod) * 2;
}

Int_t HShowerHitFPar::setColBord(Int_t nSect, Int_t nMod, 
                                 Int_t nLowerCol, Int_t nUpperCol) {
  Int_t nIdx = getIndex(nSect, nMod);
  if (nIdx>=0) {
    pColBorder[nIdx] = nLowerCol;
    pColBorder[nIdx + 1] =  nUpperCol;
    return 1;
  } else return 0;
}

Int_t HShowerHitFPar::setRowBord(Int_t nSect, Int_t nMod,
                                 Int_t nLowerRow, Int_t nUpperRow) {
  Int_t nIdx = getIndex(nSect, nMod);
  if (nIdx>=0) {
    pRowBorder[nIdx] = nLowerRow;
    pRowBorder[nIdx + 1] = nUpperRow;
    return 1;
  } else return 0;
}

Int_t HShowerHitFPar::getColBord(Int_t nSect, Int_t nMod, 
                                 Int_t* nLowerCol, Int_t* nUpperCol) {
  Int_t nIdx = getIndex(nSect, nMod);
  if (nIdx>=0) {
    *nLowerCol = pColBorder[nIdx];
    *nUpperCol = pColBorder[nIdx + 1];
    return 1;
  } else return 0;
}

Int_t HShowerHitFPar::getRowBord(Int_t nSect, Int_t nMod,
                             Int_t* nLowerRow, Int_t* nUpperRow) {
  Int_t nIdx = getIndex(nSect, nMod);
  if (nIdx>=0) {
    *nLowerRow = pRowBorder[nIdx];
    *nUpperRow = pRowBorder[nIdx + 1];
    return 1;
  } else return 0;
}
