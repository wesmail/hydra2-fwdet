// File: HShowerHitHeader.h
//
// Author: Walter Karig <W.Karig@gsi.de>
// Last update: 98/01/20 21:25:08
//

#ifndef __HShowerHitHeader_H
#define __HShowerHitHeader_H

#include "TObject.h"

class HShowerHitHeader: public TObject
{
public:
  HShowerHitHeader();
  virtual ~HShowerHitHeader();

  void setFiredCells(Int_t nFiredCells){m_nFiredCells = nFiredCells;}
  Int_t getFiredCells(){return m_nFiredCells;}
  Int_t incFiredCells(Int_t nStep = 1);

  void setLocalMax(Int_t nLocalMax){m_nLocalMax= nLocalMax;}
  Int_t getLocalMax(){return m_nLocalMax;}
  Int_t incLocalMax(Int_t nStep = 1);
 
  void setClusters(Int_t nClusters){m_nClusters = nClusters;}
  Int_t getClusters(){return m_nClusters;}
  Int_t incClusters(Int_t nStep = 1);

  Char_t getSector(void) {return m_nSector;}
  Char_t getModule(void) {return m_nModule;}
  void setSector(Char_t s) {m_nSector = s;}
  void setModule(Char_t m) {m_nModule = m;}
 
private:
  Int_t m_nFiredCells;
  Int_t m_nLocalMax;
  Int_t m_nClusters;

  Char_t m_nSector;
  Char_t m_nModule;

  ClassDef(HShowerHitHeader, 1)
};

#endif /* !__HShowerHitHeader_H */
