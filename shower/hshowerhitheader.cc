// File: HShowerHitHeader.cc
//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 98/01/20 21:25:08
//

#include "hshowerhitheader.h" 

ClassImp(HShowerHitHeader)


HShowerHitHeader::HShowerHitHeader() {
  setFiredCells(0);
  setLocalMax(0);
  setClusters(0);

  setModule(-1);
  setSector(-1);
}

HShowerHitHeader::~HShowerHitHeader() {
}

Int_t HShowerHitHeader::incFiredCells(Int_t nStep) {
  m_nFiredCells += nStep;
  return m_nFiredCells;
}

Int_t HShowerHitHeader::incLocalMax(Int_t nStep) {
  m_nLocalMax += nStep;
  return m_nLocalMax;
}

Int_t HShowerHitHeader::incClusters(Int_t nStep) {
  m_nClusters += nStep;
  return m_nClusters;
}


