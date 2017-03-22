#ifndef HEMCDETECTOR_H
#define HEMCDETECTOR_H

#include "hdetector.h"

class HEmcDetector : public HDetector {

private:
  Int_t numCells;     // number of cells (without spares)

public:
  HEmcDetector(void);
  ~HEmcDetector(void);

  HCategory *buildCategory(Cat_t cat);
  HCategory *buildMatrixCategory(const Text_t *,Float_t );
  HCategory *buildLinearCategory(const Text_t *,Float_t );

  void activateParIo(HParIo* io);
  Bool_t write(HParIo* io);

  Int_t getNumCells()   const {return numCells;}
  Int_t getMaxSecInSetup(void);

  static Int_t getCell(const Char_t row, const Char_t col);
  static void  getRowCol(const Int_t cell, Char_t& row, Char_t& col); 

  ClassDef(HEmcDetector,0) // EMC detector class
};

#endif /* !HEMCDETECTOR_H */
